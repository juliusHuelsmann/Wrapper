cmake_minimum_required(VERSION 3.5)
project(wrapper LANGUAGES C CXX VERSION 0.0.0.1)

file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_library(wrapper ${SRC})

target_include_directories(wrapper PUBLIC include PRIVATE src/include)

add_library(Wrapper::Wrapper ALIAS wrapper)
