cmake_minimum_required(VERSION 3.13)

# Use command-line variable instead
if(NOT DEFINED PICO_SDK_PATH)
    message(FATAL_ERROR "Please set PICO_SDK_PATH to the Pico SDK location")
endif()

include(D:/Projects/pico-sdk/external/pico_sdk_import.cmake)

project(mouse_firmware C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(mouse_firmware 
    main.cpp
    usb_descriptors.c
)

# Add this so TinyUSB finds your tusb_config.h
target_include_directories(mouse_firmware PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(mouse_firmware 
    pico_stdlib
    tinyusb_device
    tinyusb_board
    pico_stdio_usb
)

pico_enable_stdio_usb(mouse_firmware 1)
pico_enable_stdio_uart(mouse_firmware 0)

pico_add_extra_outputs(mouse_firmware )