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
    ${CMAKE_CURRENT_SOURCE_DIR}/estimation/madgwick.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sensors/IMU.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sensors/IMU_processing.c
    ${CMAKE_CURRENT_SOURCE_DIR}/RC/SBUS.c
    ${CMAKE_CURRENT_SOURCE_DIR}/actuators/PWM.c
    ${CMAKE_CURRENT_SOURCE_DIR}/actuators/mixer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/actuators/dshotx4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stability/PID.c
    ${CMAKE_CURRENT_SOURCE_DIR}/config/flash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/core/init.c
)

set(INCLUDES
    ${CMAKE_SOURCE_DIR}/include
)

add_library(picoflight_lib STATIC ${SOURCES})
target_include_directories(picoflight_lib PUBLIC ${INCLUDES})
target_link_libraries(picoflight_lib PUBLIC ${LINKED_LIBRARIES})
target_compile_options(picoflight_lib PUBLIC
    -Wall -Wextra
)

add_executable(picoflight 
    ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)
target_include_directories(picoflight PRIVATE ${INCLUDES})
target_link_libraries(picoflight PRIVATE ${LINKED_LIBRARIES})
target_compile_options(picoflight PRIVATE
    -Wall -Wextra
)

# Assemble PIO program.
set(PIO_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/pio/dshotx4.pio
)
pico_generate_pio_header(picoflight_lib ${PIO_SOURCES})
pico_generate_pio_header(picoflight ${PIO_SOURCES})
