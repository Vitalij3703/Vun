cmake_minimum_required(VERSION 4.0)
project(vun)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Ofast -s -ffunction-sections -fdata-sections -march=native -flto -DNDEBUG -Wl,--as-needed -Wl,--gc-sections")
file(GLOB SOURCES "src/*.c" "src/*.cpp")
add_executable(vun ${SOURCES})
target_include_directories(vun PRIVATE src)

