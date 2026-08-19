# Please refer to path/to/your/ameba_sdk/cmake/CMakeLists-template.cmake
set(private_sources)
ameba_list_append(private_sources app_main.c veml6040_demo.c)
ameba_add_internal_library(app_example
    p_SOURCES
	    ${private_sources}
)
