/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef AHT30_DEMO_H
#define AHT30_DEMO_H

#include "platform_autoconf.h"

/*
 * UART pins for AHT30 UART module and GPIO pin for the trigger button.
 *
 * AHT30 UART module wiring:
 *   Module TX  <-> AHT30_UART_RX pin on board
 *   Module RX  <-> AHT30_UART_TX pin on board
 *   Module VCC <-> 5V
 *   Module GND <-> GND
 *
 * Button wiring: connect one end to BUTTON_PIN, the other to GND.
 * The pin uses an internal/external pull-up; pressing pulls it low.
 */

#if defined(CONFIG_AMEBASMART)

#define AHT30_UART_TX       _PA_9
#define AHT30_UART_RX       _PA_10
#define AHT30_BUTTON_PIN    _PB_5

#elif defined(CONFIG_AMEBALITE)

#define AHT30_UART_TX       _PA_28
#define AHT30_UART_RX       _PA_29
#define AHT30_BUTTON_PIN    _PA_30

#elif defined(CONFIG_AMEBADPLUS)

#define AHT30_UART_TX       _PA_28
#define AHT30_UART_RX       _PA_29
#define AHT30_BUTTON_PIN    _PA_30

#elif defined(CONFIG_AMEBAD)

#define AHT30_UART_TX       _PA_26
#define AHT30_UART_RX       _PA_25
#define AHT30_BUTTON_PIN    _PA_13

#elif defined(CONFIG_AMEBAGREEN2)

#define AHT30_UART_TX       _PA_26
#define AHT30_UART_RX       _PA_25
#define AHT30_BUTTON_PIN    _PA_13

#elif defined(CONFIG_RTL8720F)

#define AHT30_UART_TX       _PA_21
#define AHT30_UART_RX       _PA_22
#define AHT30_BUTTON_PIN    _PA_24

#endif

extern int aht30_demo(void);

#endif /* AHT30_DEMO_H */
