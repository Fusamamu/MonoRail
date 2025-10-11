cmake_minimum_required(VERSION 3.16)
project(HelpPrincess)

set(CMAKE_CXX_STANDARD 20)

set(IMGUI_INCLUDE  ./vendors/imgui)
set(IMGUI_BACKENDS ./vendors/imgui/backends)

file(GLOB IMGUI_SRCS ${IMGUI_INCLUDE}/*.cpp)
file(GLOB IMGUIZMOS_SRCS ./vendors/ImGuizmo/*.cpp)

add_subdirectory(./vendors/entt)
get_target_property(_aliased EnTT::EnTT ALIASED_TARGET)
message(STATUS "EnTT target found as: ${_aliased}")

find_package(SDL2   REQUIRED COMPONENTS SDL2)
find_package(GLEW   REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm    REQUIRED)
find_package(ZLIB   REQUIRED)

find_library(ASSIMP_PATH assimp PATHS ./vendors/assimp/lib)
if(ASSIMP_PATH)
    message(STATUS      "Found Assimp library: ${ASSIMP_PATH}")
else()
    message(FATAL_ERROR "Assimp library not found!")
endif()

find_library(COCOA_FRAMEWORK Cocoa)
find_library(IOKIT_FRAMEWORK IOKit)
find_library(COREVIDEO_FRAMEWORK CoreVideo)

add_executable(${PROJECT_NAME}
        ${IMGUI_SRCS}
        ${IMGUI_BACKENDS}/imgui_impl_sdl2.cpp
        ${IMGUI_BACKENDS}/imgui_impl_opengl3.cpp
        ./src/main.cpp
        ./src/Engine.cpp
        ./src/Shader.cpp
        ./src/MeshRenderer.cpp
        ./src/ResourceManager.cpp
        ./src/InputSystem.cpp
        ./src/Grid.cpp
        src/Scene.cpp
        src/Scene.h
        src/SceneManager.cpp
        src/SceneManager.h
        src/Ray.cpp
        src/Ray.h
        src/PerlinNoise.cpp
        src/PerlinNoise.h
        src/FrameBuffer.cpp
        src/FrameBuffer.h
        src/GizmosRenderer.cpp
        src/GizmosRenderer.h
        src/RenderPipeline.cpp
        src/RenderPipeline.h
        src/ApplicationConfig.h
        src/SkeletonMeshRenderer.cpp
        src/SkeletonMeshRenderer.h
        src/Animation.cpp
        src/Animation.h)

target_include_directories(${PROJECT_NAME} PUBLIC
        ./vendors/assimp/include)

target_precompile_headers(${PROJECT_NAME} PRIVATE ./src/PCH.h)

target_include_directories(${PROJECT_NAME} PUBLIC ./vendors/imgui)
target_include_directories(${PROJECT_NAME} PUBLIC ./vendors/imgui/backends)

target_link_libraries(${PROJECT_NAME} PUBLIC ZLIB::ZLIB)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2)
target_link_libraries(${PROJECT_NAME} PUBLIC GLEW::glew)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)
target_link_libraries(${PROJECT_NAME} PUBLIC ${ASSIMP_PATH})
target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENGL_gl_LIBRARY})
target_link_libraries(${PROJECT_NAME} PUBLIC ${COCOA_FRAMEWORK})
target_link_libraries(${PROJECT_NAME} PUBLIC ${IOKIT_FRAMEWORK})
target_link_libraries(${PROJECT_NAME} PUBLIC ${COREVIDEO_FRAMEWORK})

target_link_libraries(${PROJECT_NAME} PUBLIC EnTT)
