set(TEST_NAME dshot_rc_test)

add_executable(${TEST_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/dshot_rc_test.c
)

target_link_libraries(${TEST_NAME} PRIVATE picoflight_lib)

pico_enable_stdio_usb(${TEST_NAME} 1)
pico_enable_stdio_uart(${TEST_NAME} 0)

pico_add_uf2_output(${TEST_NAME})
