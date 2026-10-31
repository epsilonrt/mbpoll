# Test targets for mbpoll
# Unit tests for serial module functions

# Enable testing with CTest
enable_testing()

# Serial module tests
add_executable(test_serial test_serial.c ../src/serial.c)
target_include_directories(test_serial PRIVATE ../src)
add_test(NAME test_serial COMMAND test_serial)

# Utils module tests
add_executable(test_utils test_utils.c ../src/utils.c)
target_include_directories(test_utils PRIVATE ../src)
target_compile_definitions(test_utils PRIVATE SKIP_FAILURE_EXIT)
add_test(NAME test_utils COMMAND test_utils)

# Override the shared libmodbus connect function to inspect the configured
# timeout without opening a network connection or requiring a Modbus device.
if(UNIX)
  add_executable(test_connect_timeout test_connect_timeout.c
    ../src/custom-rts.c ../src/serial.c ../src/chipio.c ../src/utils.c)
  target_include_directories(test_connect_timeout PRIVATE
    ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}
    ${LIBMODBUS_INCLUDE_DIRS})
  target_compile_definitions(test_connect_timeout PRIVATE SKIP_FAILURE_EXIT)
  target_link_directories(test_connect_timeout PRIVATE ${LIBMODBUS_LIBRARY_DIRS})
  target_link_libraries(test_connect_timeout PRIVATE ${LIBMODBUS_LIBRARIES} m)
  add_test(NAME test_connect_timeout_default
    COMMAND test_connect_timeout 1 0 -m tcp -1 127.0.0.1)
  add_test(NAME test_connect_timeout_fractional
    COMMAND test_connect_timeout 0 750000 -m tcp -o 0.75 -1 127.0.0.1)
  add_test(NAME test_connect_timeout_slow
    COMMAND test_connect_timeout 10 0 -m tcp -o 10 -1 127.0.0.1)
endif()
