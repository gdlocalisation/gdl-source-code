cmake_minimum_required(VERSION 3.0.0)

project(gdl)

file(GLOB_RECURSE GDL_SOURCES src/*.[hc]pp)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(minhook)
add_subdirectory(json)
add_subdirectory(cocos-headers)
add_subdirectory(utfcpp)

include_directories(src gd.h gd.h/include)

add_library(gdl SHARED ${GDL_SOURCES})

target_link_libraries(gdl PRIVATE cocos2d nlohmann_json::nlohmann_json minhook utf8cpp)