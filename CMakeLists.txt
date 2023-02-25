cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(minimatcher VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_compile_options(-O3)
add_executable(minimatcher src/main.cpp)
add_executable(testclient test/udp_client.cpp)
add_library(asio INTERFACE)

# target_compile_definitions(asio PUBLIC ASIO_STANDALONE)
# target_compile_options(asio INTERFACE ASIO_STANDALONE)

target_include_directories(asio INTERFACE include/asio)
target_link_libraries(asio INTERFACE pthread)
target_link_libraries(minimatcher PRIVATE asio ${Boost_LIBRARIES})
target_link_libraries(testclient PRIVATE asio)
