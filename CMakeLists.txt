cmake_minimum_required(VERSION 3.1)

project(varint LANGUAGES CXX)

if(POLICY CMP0074)
    # 3.12+ compatibility
    cmake_policy(SET CMP0074 OLD)
endif()


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(libvarint
        varint/codecs/uleb128.h
        varint/codecs/uleb128.cpp
        varint/varint.cpp
        varint/varint.h)
target_include_directories(libvarint PUBLIC ${varint_SOURCE_DIR} ${MICROSOFT.GSL_ROOT}/include)
set_target_properties(libvarint PROPERTIES OUTPUT_NAME varint)
if (MSVC)
    # warning level 4 and all warnings as errors
    target_compile_options(libvarint PRIVATE /W4 /WX)
else()
    # lots of warnings and all warnings as errors
    target_compile_options(libvarint PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
enable_testing()
add_subdirectory(test)