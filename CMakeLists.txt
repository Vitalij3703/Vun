cmake_minimum_required(VERSION 4.0)
project(Vun)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Os -s -ffunction-sections -fdata-sections -march=native -flto -DNDEBUG")
add_executable(Vun src/Vun.cpp)
target_include_directories(Vun PRIVATE src)

