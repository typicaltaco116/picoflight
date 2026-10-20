set(TEST_NAME dshot_bare_minimum)

add_executable(${TEST_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/dshot_bare_minimum.c
)
target_link_libraries(${TEST_NAME} PRIVATE picoflight_lib)
set_target_properties(${TEST_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)
pico_enable_stdio_usb(${TEST_NAME} 1)
pico_enable_stdio_uart(${TEST_NAME} 0)
pico_add_extra_outputs(${TEST_NAME})
set_target_properties(${TEST_NAME} PROPERTIES 
    ADDITIONAL_CLEAN_FILES "$<TARGET_FILE:${TEST_NAME}>.map"
)
