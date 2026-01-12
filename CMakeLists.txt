cmake_minimum_required(VERSION 3.10)
project(fix_dropcopy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(fix_dropcopy
    src/main.cpp
    src/application.cpp
    src/socket.cpp
    src/config_parser.cpp
    src/token_handler.cpp
    src/fix_message.cpp
    src/fix_session.cpp
)

target_include_directories(fix_dropcopy PRIVATE include)
