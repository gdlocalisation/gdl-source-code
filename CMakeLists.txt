cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
project(GDL VERSION 1.1.0)

file (GLOB SOURCES
    src/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/libraries"
    "${CMAKE_SOURCE_DIR}/libraries/gd.h/include"
    "${CMAKE_SOURCE_DIR}/libraries/gd.h"
)

add_subdirectory("libraries/cocos-headers")
add_subdirectory("libraries/minhook")

target_link_libraries(${PROJECT_NAME} minhook cocos2d)

# target_link_options(${PROJECT_NAME} PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/FORCE:MULTIPLE>)