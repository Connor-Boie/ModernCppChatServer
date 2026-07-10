cmake_minimum_required(VERSION 3.16)

project(ModernCppChatServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ModernCppChatServer
    src/main.cpp
    src/Socket.cpp
)

target_include_directories(ModernCppChatServer PRIVATE include)