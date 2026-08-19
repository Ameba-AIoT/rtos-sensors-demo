/*
 *  AHT30 Temperature & Humidity Sensor Demo via UART
 *
 *  Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 *
 *  Description:
 *    Demonstrates reading the AHT30 UART module (TX/RX/GND/5V interface).
 *    The module exposes an ASCII string interface at 9600 8N1.
 *    On power-up the module defaults to manual-read mode.
 *    Press the button (falling edge) to trigger a single measurement and
 *    print the result via the serial log.
 *
 *  Hardware connections:
 *    Module TX  <-> AHT30_UART_RX pin (see aht30_demo.h)
 *    Module RX  <-> AHT30_UART_TX pin (see aht30_demo.h)
 *    Module VCC <-> 3.3V or 5V
 *    Module GND <-> GND
 *    Button     <-> AHT30_BUTTON_PIN (other end to GND; uses pull-up)
 *
 *  AHT30 UART module protocol (ASCII, 9600 8N1):
 *    Command  : "Read\r\n"
 *    Response : "R:HHH.HRH TTT.TC\r\n"
 *    Example  : "R:070.0RH 032.4C\r\n"  -> 70.0 %RH, 32.4 C
 */

#include "PinNames.h"
#include "basic_types.h"
#include "diag.h"
#include "os_wrapper.h"
#include "serial_api.h"
#include "gpio_irq_api.h"
#include "aht30_demo.h"

#define APP_VERSION         "1.0.2"

#define AHT30_UART_BAUD     9600
#define AHT30_UART_IDX      1   /* UART1: PA_28(TX)/PA_29(RX) on AmebaLite */

#define AHT30_RESP_MAXLEN   64

static const char AHT30_CMD_READ[] = "Read\r\n";

static const char *const TAG = "AHT30";

static serial_t     uart_obj;
static gpio_irq_t   button_irq;
static volatile int g_button_pressed = 0;

/* ------------------------------------------------------------------ */
/* Receive chars until '\n' or timeout. Returns length, -1 on timeout */
/* ------------------------------------------------------------------ */
static int uart_readline(char *buf, int maxlen, uint32_t byte_timeout_ms)
{
    int n = 0;
    uint32_t timeout;

    while (n < maxlen - 1)
    {
        timeout = byte_timeout_ms;
        while (!serial_readable(&uart_obj))
        {
            rtos_time_delay_ms(1);
            if (--timeout == 0)
            {
                RTK_LOGI(TAG, "RX timeout at byte %d\r\n", n);
                buf[n] = '\0';
                return -1;
            }
        }
        buf[n] = (char)serial_getc(&uart_obj);
        if (buf[n] == '\n')
        {
            n++;
            break;
        }
        n++;
    }
    buf[n] = '\0';
    return n;
}

/* ------------------------------------------------------------------ */
/* Send "Read\r\n", parse "R:HHH.HRH TTT.TC\r\n"                     */
/* Returns 0 on success, -1 on error.                                 */
/* ------------------------------------------------------------------ */
static int aht30_read(float *temperature, float *humidity)
{
    char  resp[AHT30_RESP_MAXLEN];
    char *rh_sep;
    char *t_start;
    uint8_t i, k;
    int  neg;

    serial_clear(&uart_obj);

    /* Flush the RX buffer manually to ensure no garbage is left */
    while (serial_readable(&uart_obj))
    {
        serial_getc(&uart_obj);
    }

    for (i = 0; AHT30_CMD_READ[i]; i++)
    {
        serial_putc(&uart_obj, AHT30_CMD_READ[i]);
    }

    if (uart_readline(resp, sizeof(resp), 2000) < 0)
    {
        return -1;
    }

    /* Validate prefix */
    if (resp[0] != 'R' || resp[1] != ':')
    {
        RTK_LOGI(TAG, "Unexpected prefix: 0x%02x 0x%02x\r\n", resp[0], resp[1]);
        return -1;
    }

    /* Find "RH " separator starting after "R:" */
    rh_sep = NULL;
    for (k = 2; k + 2 < AHT30_RESP_MAXLEN && resp[k]; k++)
    {
        if (resp[k] == 'R' && resp[k + 1] == 'H' && resp[k + 2] == ' ')
        {
            rh_sep = &resp[k];
            break;
        }
    }
    if (!rh_sep)
    {
        RTK_LOGI(TAG, "No 'RH ' found in response\r\n");
        return -1;
    }

    /* Humidity: 5 chars before "RH " -> "HHH.H" */
    {
        char *h = rh_sep - 5;
        *humidity = (float)((h[0] - '0') * 100 + (h[1] - '0') * 10 + (h[2] - '0'))
                  + (float)(h[4] - '0') / 10.0f;  /* h[3] == '.' */
    }

    /* Temperature: after "RH ", possibly negative */
    t_start = rh_sep + 3;
    neg = 0;
    if (*t_start == '-')
    {
        neg = 1;
        t_start++;
        if (*t_start == ' ')
        {
            t_start++;  /* some modules pad: "- XX.X" */
        }
    }
    /* "TTT.T" or "TT.T" depending on magnitude; find 'C' as end marker */
    {
        char *c_pos = NULL;
        for (k = 0; k < 8 && t_start[k]; k++)
        {
            if (t_start[k] == 'C')
            {
                c_pos = &t_start[k];
                break;
            }
        }
        if (!c_pos || c_pos < t_start + 3)
        {
            RTK_LOGI(TAG, "No 'C' marker found\r\n");
            return -1;
        }
        /* c_pos - t_start == 5 for "TTT.T", 4 for "TT.T" */
        int int_len = (int)(c_pos - t_start) - 2; /* subtract ".X" */
        int t_int = 0;
        for (k = 0; k < (uint8_t)int_len; k++)
        {
            t_int = t_int * 10 + (t_start[k] - '0');
        }
        int t_frac = t_start[int_len + 1] - '0';  /* skip '.' */
        *temperature = (float)t_int + (float)t_frac / 10.0f;
        if (neg)
        {
            *temperature = -*temperature;
        }
    }

    return 0;
}

/* ------------------------------------------------------------------ */
/* Print helper                                                        */
/* ------------------------------------------------------------------ */
static void print_result(float temperature, float humidity)
{
    /* Convert to integerx10 to avoid float truncation errors (e.g. 55.3 -> 55.2) */
    int t_sign = (temperature < 0.0f) ? 1 : 0;
    int t_x10  = (int)(temperature * 10.0f + (t_sign ? -0.5f : 0.5f));
    if (t_x10 < 0)
    {
        t_x10 = -t_x10;
    }
    int h_x10  = (int)(humidity * 10.0f + 0.5f);

    RTK_LOGI(TAG, "Temperature: %s%d.%d C   Humidity: %d.%d %%RH\r\n",
             t_sign ? "-" : "", t_x10 / 10, t_x10 % 10,
             h_x10 / 10, h_x10 % 10);
}

/* ------------------------------------------------------------------ */
/* Button ISR - only sets a flag                                       */
/* ------------------------------------------------------------------ */
static void button_irq_handler(uint32_t id, uint32_t event)
{
    (void)id;
    (void)event;
    g_button_pressed = 1;
}

/* ------------------------------------------------------------------ */
/* Main demo task                                                      */
/* ------------------------------------------------------------------ */
static void aht30_demo_task(void *param)
{
    float temperature, humidity;

    (void)param;

    RTK_LOGI(TAG, "=== AHT30 Demo v%s ===\r\n", APP_VERSION);

    /* ----- UART init ----- */
    uart_obj.uart_idx = AHT30_UART_IDX;
    serial_init(&uart_obj, AHT30_UART_TX, AHT30_UART_RX);
    serial_baud(&uart_obj, AHT30_UART_BAUD);
    serial_format(&uart_obj, 8, ParityNone, 1);

    RTK_LOGI(TAG, "AHT30 UART module ready (%d baud, ASCII protocol).\r\n", AHT30_UART_BAUD);

    /* ----- Button: falling-edge interrupt ----- */
    gpio_irq_init(&button_irq, AHT30_BUTTON_PIN, button_irq_handler, 0);
    gpio_irq_pull_ctrl(&button_irq, PullUp);
    gpio_irq_set(&button_irq, IRQ_FALL, 1);
    gpio_irq_enable(&button_irq);

    RTK_LOGI(TAG, "Press the button to read temperature and humidity.\r\n");

    /* Give the sensor some time to stabilize on power up */
    rtos_time_delay_ms(1500);

    RTK_LOGI(TAG, "Auto read at startup...\r\n");
    if (aht30_read(&temperature, &humidity) == 0)
    {
        print_result(temperature, humidity);
    }

    /* ----- Main loop ----- */
    while (1)
    {
        if (g_button_pressed)
        {
            g_button_pressed = 0;
            rtos_time_delay_ms(20); /* simple debounce */

            RTK_LOGI(TAG, "Button pressed - reading AHT30...\r\n");

            if (aht30_read(&temperature, &humidity) == 0)
            {
                print_result(temperature, humidity);
            }
        }
        rtos_time_delay_ms(50);
    }
}

/* ------------------------------------------------------------------ */
/* Entry point                                                         */
/* ------------------------------------------------------------------ */
int aht30_demo(void)
{
    if (rtos_task_create(NULL, "AHT30_DEMO", (rtos_task_t)aht30_demo_task,
                         NULL, 2048, 1) != RTK_SUCCESS)
    {
        RTK_LOGI(TAG, "Cannot create AHT30_DEMO task\r\n");
    }

    return 0;
}
