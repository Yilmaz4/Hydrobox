cmake_minimum_required(VERSION 3.21)

project(Hydrobox VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(SFML_BUILD_AUDIO FALSE)
set(SFML_BUILD_NETWORK FALSE)
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/lib/imgui)
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_SFML_IMGUI_DEMO ON)
set(B_PRODUCTION_MODE ON)

add_subdirectory("lib/boxer")
add_subdirectory("lib/sfml")
add_subdirectory("lib/imgui-sfml")
add_subdirectory("lib/embed")

include_directories(SYSTEM "lib/glad/include")
include_directories(SYSTEM "lib/sfml/include")
include_directories(SYSTEM "src")

find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp lib/glad/src/glad.c)

b_embed(${PROJECT_NAME} assets/consola.ttf)

target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics OpenGL::GL Boxer ImGui-SFML::ImGui-SFML)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DPLATFORM_WINDOWS)
elseif(APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DPLATFORM_MAC)
elseif(UNIX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DPLATFORM_LINUX)
endif()