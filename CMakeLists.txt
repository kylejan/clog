cmake_minimum_required(VERSION 2.8.12)
project(aslog)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

file(GLOB_RECURSE headers ${PROJECT_SOURCE_DIR}/include/*.h ${PROJECT_SOURCE_DIR}/include/*.cc)
file(GLOB sources ${PROJECT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${headers} ${sources})

include_directories(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/include)
