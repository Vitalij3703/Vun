cmake_minimum_required(4.0)
project(Vun)
set(CMAKE_CXX_STANDARD 23)

add_executable(Vun src/Vun.cpp src/debug.cpp)
target_include_directories(Vun PRIVATE src/include)
