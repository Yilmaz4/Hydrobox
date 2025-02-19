cmake_minimum_required(VERSION 3.20)

project(Hydrobox VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory("lib/boxer")
add_subdirectory("lib/sfml")

include_directories(SYSTEM "lib/glad/include")
include_directories(SYSTEM "lib/sfml/include")

find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp lib/glad/src/glad.c)

target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics OpenGL::GL Boxer)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DPLATFORM_WINDOWS)
elseif(APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DPLATFORM_MAC)
elseif(UNIX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DPLATFORM_LINUX)
endif()