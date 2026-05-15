cmake_minimum_required(VERSION 3.12)

set(PICO_BOARD pico2)

include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(picoflight C CXX ASM)

set(SOURCES
    src/main.c
    src/IMU_processing.h
    src/IMU_processing.c
    src/orientation_types.h
    src/sensor_fusion.h
    src/sensor_fusion.c

    src/sensors/MPU6050_register_map.h
    src/sensors/IMU.h
    src/sensors/IMU.c

    src/RC/RC.h
    src/RC/SBUS.h
    src/RC/SBUS.c
)

pico_sdk_init()

if (TARGET tinyusb_device)
    add_executable(${PROJECT_NAME} ${SOURCES})

    # pull in common dependencies
    target_link_libraries(${PROJECT_NAME}
        pico_stdlib
        hardware_i2c
    )

    target_compile_options(${PROJECT_NAME}
        PRIVATE -Wall -Wextra
    )

    # enable usb output, disable uart output
    pico_enable_stdio_usb(${PROJECT_NAME} 1)
    pico_enable_stdio_uart(${PROJECT_NAME} 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(${PROJECT_NAME})

elseif(PICO_ON_DEVICE)
    message("Skipping hello_usb because TinyUSB submodule is not initialized in the SDK")
endif()
