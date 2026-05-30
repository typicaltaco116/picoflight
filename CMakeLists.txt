cmake_minimum_required(VERSION 3.12)

set(PICO_BOARD pico2)

include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(picoflight C CXX ASM)

add_subdirectory(submodules/pico-pio-loader)

set(LINKED_LIBRARIES
    # pico-sdk libraries
    pico_stdlib
    hardware_i2c
    hardware_gpio
    hardware_pio
    hardware_pwm
    hardware_flash

    # Additional libraries
    pico_pio_loader
)

set(SOURCES
    src/main.c

    src/orientation/types.h

    src/estimation/madgwick.h
    src/estimation/madgwick.c

    src/sensors/MPU6050_register_map.h
    src/sensors/IMU.h
    src/sensors/IMU.c
    src/sensors/IMU_processing.h
    src/sensors/IMU_processing.c

    src/RC/RC.h
    src/RC/SBUS.h
    src/RC/SBUS.c

    src/actuators/PWM.h
    src/actuators/PWM.c
    src/actuators/mixer.h
    src/actuators/mixer.c
    src/actuators/outputs.h
    src/actuators/dshot.h
    src/actuators/dshot.c

    src/stability/PID.h
    src/stability/PID.c

    src/config/flash.h
    src/config/flash.c
    src/config/IMU.h
    src/config/pin_mapping.h

    src/core/init.h
    src/core/init.c
)

pico_sdk_init()

if (TARGET tinyusb_device)
    add_executable(${PROJECT_NAME} ${SOURCES})

    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    # pull in common dependencies
    target_link_libraries(${PROJECT_NAME} ${LINKED_LIBRARIES})

    target_compile_options(${PROJECT_NAME}
        PRIVATE -Wall -Wextra
    )

    pico_set_linker_script(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/picoflight.ld)

    pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/PIO/dshot.pio)

    # enable usb output, disable uart output
    pico_enable_stdio_usb(${PROJECT_NAME} 1)
    pico_enable_stdio_uart(${PROJECT_NAME} 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(${PROJECT_NAME})

elseif(PICO_ON_DEVICE)
    message("Skipping hello_usb because TinyUSB submodule is not initialized in the SDK")
endif()
