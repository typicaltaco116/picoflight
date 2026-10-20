set(TEST_NAME madgwick_test)

add_executable(${TEST_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/madgwick_test.c
)

target_link_libraries(${TEST_NAME} PRIVATE picoflight_lib)
set_target_properties(${TEST_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    ADDITIONAL_CLEAN_FILES "$<TARGET_FILE:${TEST_NAME}>.map"
)

pico_enable_stdio_usb(${TEST_NAME} 1)
pico_enable_stdio_uart(${TEST_NAME} 0)

pico_add_uf2_output(${TEST_NAME})
