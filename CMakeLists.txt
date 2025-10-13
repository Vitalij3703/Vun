cmake_minimum_required(VERSION 4.0)
project(Vun)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-O3 -march=native -flto -DNDEBUG -fno-exceptions")
add_executable(Vun src/Vun.cpp)
target_include_directories(Vun PRIVATE src)

