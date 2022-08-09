cmake_minimum_required(VERSION 3.10)
project(test_json)

set(CMAKE_CXX_FLAGS "-std=c++17 -g")
set(CMAKE_VERBOSE_MAKEFILE ON)


include_directories("./include")
set(SOURCE_FILE "./src/main.cpp" "./src/object.cpp" "./src/parse.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_FILE})

