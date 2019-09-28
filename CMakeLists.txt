cmake_minimum_required(VERSION 3.5)
project(wrapper LANGUAGES C CXX VERSION 0.0.0.1)

file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_library(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME} 
  PUBLIC ${PROJECT_SOURCE_DIR}/include 
  PRIVATE ${PROJECT_SOURCE_DIR}/src/include)

target_link_libraries(${PROJECT_NAME} PUBLIC RCV::Lib)
add_library(Wrapper::Wrapper ALIAS ${PROJECT_NAME})
