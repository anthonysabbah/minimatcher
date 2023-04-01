cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

# set the project name and version
project(minimatcher VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*)
file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS test/*.test.cpp)

add_compile_options(-O3)

# add_library(SRC_LIB STATIC ${SOURCE_FILES})
# target_include_directories(SRC_LIB PRIVATE src)
add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE include/asio)


add_executable(testclient test/udp_client.cpp)
add_executable(tests ${TEST_FILES})
add_library(asio INTERFACE)
add_library(catch INTERFACE)
add_library(SRC_LIBS INTERFACE)

# target_compile_definitions(asio PUBLIC ASIO_STANDALONE)
# target_compile_options(asio INTERFACE ASIO_STANDALONE)

target_include_directories(asio INTERFACE include/asio)
target_link_libraries(asio INTERFACE pthread)
target_include_directories(catch INTERFACE include/catch)
target_include_directories(SRC_LIBS INTERFACE src)
# target_link_libraries(${PROJECT_NAME} PRIVATE asio ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE asio)
target_link_libraries(testclient asio)
target_link_libraries(tests PRIVATE catch SRC_LIBS)
