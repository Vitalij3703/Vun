cmake_minimum_required(VERSION 4.0)
project(vun)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Ofast -s -ffunction-sections -fdata-sections -march=native -flto -DNDEBUG -Wl,--as-needed -Wl,--gc-sections")
add_executable(vun src/Vun.cpp)
target_include_directories(Vun PRIVATE src)

