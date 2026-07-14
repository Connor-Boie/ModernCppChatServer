cmake_minimum_required(VERSION 3.16)

project(ModernCppChatServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)
find_package(Python3 COMPONENTS Interpreter REQUIRED)

add_executable(ModernCppChatServer
    src/main.cpp
    src/Server.cpp
    src/Socket.cpp
)

target_include_directories(
    ModernCppChatServer
    PRIVATE
    include
)

target_compile_options(
    ModernCppChatServer
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_link_libraries(
    ModernCppChatServer
    PRIVATE
    Threads::Threads
)

enable_testing()

add_test(
    NAME ChatServerIntegration
    COMMAND
        ${Python3_EXECUTABLE}
        ${CMAKE_SOURCE_DIR}/tests/integration_test.py
        $<TARGET_FILE:ModernCppChatServer>
)

set_tests_properties(
    ChatServerIntegration
    PROPERTIES
    TIMEOUT 20
)