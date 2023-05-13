cmake_minimum_required(VERSION 3.0.0)
project(socket-test)

add_executable(socket-test main.cpp)
target_link_libraries(socket-test PRIVATE pthread)
