cmake_minimum_required(VERSION 3.12)

set(PICO_BOARD pico2)

include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(MPU6050_demo C CXX ASM)

set(SOURCES
    src/main.c
    src/IMU_processing.h
    src/IMU_processing.c
    src/sensors/MPU6050_register_map.h
    src/sensors/IMU.h
    src/sensors/IMU.c
)

pico_sdk_init()

if (TARGET tinyusb_device)
    add_executable(MPU6050_demo ${SOURCES})

    # pull in common dependencies
    target_link_libraries(MPU6050_demo
        pico_stdlib
        hardware_i2c
    )

    # enable usb output, disable uart output
    pico_enable_stdio_usb(MPU6050_demo 1)
    pico_enable_stdio_uart(MPU6050_demo 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(MPU6050_demo)

elseif(PICO_ON_DEVICE)
    message("Skipping hello_usb because TinyUSB submodule is not initialized in the SDK")
endif()
