enable_testing()
find_package(GTest MODULE REQUIRED)
add_executable(varint_test varint_test.cpp main.cpp)
target_link_libraries(varint_test PRIVATE libvarint GTest::GTest GTest::Main)
set_target_properties(varint_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                      ${CMAKE_CURRENT_BINARY_DIR})
add_test(NAME varint_test
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/varint_test)