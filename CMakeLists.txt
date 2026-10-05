cmake_minimum_required(VERSION 3.12)

set(PICO_BOARD pico2)

include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(picoflight C CXX ASM)

set(LINKED_LIBRARIES
    # pico-sdk libraries
    pico_stdlib
    hardware_i2c
    hardware_gpio
    hardware_pwm
    hardware_flash
    hardware_pio
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/estimation/madgwick.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sensors/IMU.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sensors/IMU_processing.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RC/SBUS.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/actuators/PWM.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/actuators/mixer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/actuators/dshot.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stability/PID.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/config/flash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/init.c
)

set(INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

pico_sdk_init()

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDES})

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
