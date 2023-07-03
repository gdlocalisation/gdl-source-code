cmake_minimum_required(VERSION 3.0.0)
project(GDL VERSION 1.1.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file (GLOB SOURCES
    src/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "GDLocalisation")

target_include_directories(${PROJECT_NAME} PRIVATE
    libs
    libs/gd.h/include
    libs/gd.h
)

add_subdirectory(libs/cocos-headers)
add_subdirectory(libs/minhook)
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
add_subdirectory(libs/fmt)

target_link_libraries(${PROJECT_NAME} minhook cocos2d fmt)