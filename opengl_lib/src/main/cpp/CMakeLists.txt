cmake_minimum_required(VERSION 3.22.1)

project("gllib")

include_directories(
        util
        renderers
        glm
)

find_library(
        log-lib
        log)

find_library(
        android-lib
        android)

file(GLOB src_files
        ${CMAKE_SOURCE_DIR}/*.cpp
        ${CMAKE_SOURCE_DIR}/util/*.cpp
        ${CMAKE_SOURCE_DIR}/renderers/*.cpp)

add_library(${CMAKE_PROJECT_NAME} SHARED
        ${src_files})

target_link_libraries(${CMAKE_PROJECT_NAME}
        ${log-lib}
        ${android-lib}
        GLESv3
        EGL)