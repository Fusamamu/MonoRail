cmake_minimum_required(VERSION 3.16)
project(HelpPrincess)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2   REQUIRED COMPONENTS SDL2)
find_package(GLEW   REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm    REQUIRED)
find_package(ZLIB   REQUIRED)

find_library(COCOA_FRAMEWORK Cocoa)
find_library(IOKIT_FRAMEWORK IOKit)
find_library(COREVIDEO_FRAMEWORK CoreVideo)

add_executable(${PROJECT_NAME} 
    ./src/main.cpp
    ./src/Engine.cpp
    ./src/Shader.cpp
    ./src/MeshRenderer.cpp
        src/Cube.h)

target_precompile_headers(${PROJECT_NAME} PRIVATE ./src/PCH.h)

target_link_libraries(${PROJECT_NAME} PUBLIC ZLIB::ZLIB)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2)
target_link_libraries(${PROJECT_NAME} PUBLIC GLEW::glew)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)
#target_link_libraries(${PROJECT_NAME} PUBLIC ${ASSIMP_PATH})
target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENGL_gl_LIBRARY})
target_link_libraries(${PROJECT_NAME} PUBLIC ${COCOA_FRAMEWORK})
target_link_libraries(${PROJECT_NAME} PUBLIC ${IOKIT_FRAMEWORK})
target_link_libraries(${PROJECT_NAME} PUBLIC ${COREVIDEO_FRAMEWORK})
