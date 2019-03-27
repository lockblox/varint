cmake_minimum_required(VERSION 3.1)

project(varint LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(libvarint INTERFACE)
target_include_directories(libvarint INTERFACE ${varint_SOURCE_DIR})

enable_testing()

find_package(GTest MODULE REQUIRED)
add_executable(varint_test 
               test/varint_test.cpp 
               test/uleb128_test.cpp 
               test/main.cpp
               varint/codecs/uleb128.h
               varint/codecs/uleb128.cpp
               varint/varint.cpp
               varint/varint.h)
target_link_libraries(varint_test PRIVATE libvarint GTest::GTest GTest::Main)
add_test(NAME GTest COMMAND varint_test)

if (MSVC) 
    target_compile_options(varint_test PRIVATE /W4 /WX /MP)
else() 
    target_compile_options(varint_test PRIVATE -Wall -Wextra -pedantic -Werror)
endif()