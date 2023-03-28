cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

# set the project name and version
project(minimatcher VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# find_package(Boost REQUIRED)
# include_directories(${Boost_INCLUDE_DIR})
find_package(Catch2 3 REQUIRED)
add_executable(tests test/test.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

# These tests need their own main
# add_executable(custom-main-tests test.cpp test-main.cpp)
# target_link_libraries(custom-main-tests PRIVATE Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests(tests)

add_compile_options(-O3)
add_executable(minimatcher src/main.cpp)
add_executable(testclient test/udp_client.cpp)
add_library(asio INTERFACE)

# target_compile_definitions(asio PUBLIC ASIO_STANDALONE)
# target_compile_options(asio INTERFACE ASIO_STANDALONE)

target_include_directories(asio INTERFACE include/asio)
target_link_libraries(asio INTERFACE pthread)
# target_link_libraries(minimatcher PRIVATE asio ${Boost_LIBRARIES})
target_link_libraries(minimatcher PRIVATE asio)
target_link_libraries(testclient PRIVATE asio)
