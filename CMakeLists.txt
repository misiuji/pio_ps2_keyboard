cmake_minimum_required(VERSION 3.16)
include(pico-sdk/pico_sdk_init.cmake)

project(pico)

pico_sdk_init()

add_executable(main
    main.c
)

pico_generate_pio_header(main ${CMAKE_CURRENT_LIST_DIR}/keyboard.pio)

pico_enable_stdio_usb(main 0)
pico_enable_stdio_uart(main 1)


target_link_libraries(main PRIVATE pico_stdlib hardware_irq hardware_pio)
pico_add_extra_outputs(main)


#example_auto_set_url(main)
