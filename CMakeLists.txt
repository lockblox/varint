cmake_minimum_required(VERSION 3.1)

project(varint VERSION 1.0 LANGUAGES CXX)
include(CTest)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(libvarint INTERFACE)
target_include_directories(libvarint INTERFACE 
                           $<BUILD_INTERFACE:${varint_SOURCE_DIR}>
                           $<INSTALL_INTERFACE:include>)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${varint_BINARY_DIR}/varintConfigVersion.cmake"
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion)

install(TARGETS libvarint
        EXPORT varintTargets
        INCLUDES DESTINATION include COMPONENT Development)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${varint_SOURCE_DIR}/cmake/varintConfig.cmake"
    "${varint_BINARY_DIR}/varintConfig.cmake"
    INSTALL_DESTINATION share/cmake/varint
)

install(EXPORT varintTargets DESTINATION share/cmake/varint)
install(FILES "${varint_BINARY_DIR}/varintConfigVersion.cmake"
              "${varint_BINARY_DIR}/varintConfig.cmake"
        DESTINATION share/cmake/varint)
install(DIRECTORY ${varint_SOURCE_DIR}/varint/ 
        DESTINATION include/varint
        FILES_MATCHING PATTERN "*.h")
        
if(BUILD_TESTING)
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
endif()