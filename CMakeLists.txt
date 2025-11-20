cmake_minimum_required(VERSION 3.16)
project(HelpPrincess)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG   "-DMU_BUILD_DEBUG"  )
set(CMAKE_CXX_FLAGS_RELEASE "-DMU_BUILD_RELEASE")

# --- APP BUNDLE SETUP (added) ---
#set(MACOSX_BUNDLE_ICON_FILE icon.icns)
#set_source_files_properties(${CMAKE_SOURCE_DIR}/icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
#set(APP_ICON ${CMAKE_SOURCE_DIR}/icon.icns)
# --------------------------------

set(IMGUI_INCLUDE  ./vendors/imgui)
set(IMGUI_BACKENDS ./vendors/imgui/backends)

file(GLOB IMGUI_SRCS ${IMGUI_INCLUDE}/*.cpp)
file(GLOB IMGUIZMOS_SRCS ./vendors/ImGuizmo/*.cpp)

add_subdirectory(./vendors/entt)
get_target_property(_aliased EnTT::EnTT ALIASED_TARGET)
message(STATUS "EnTT target found as: ${_aliased}")

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(./vendors/json)

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

message(STATUS "CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")

find_library(COCOA_FRAMEWORK Cocoa)
find_library(IOKIT_FRAMEWORK IOKit)
find_library(COREVIDEO_FRAMEWORK CoreVideo)

file(GLOB         SRCS            ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp             )
file(GLOB_RECURSE CORE_SRCS       ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/*.cpp        )
file(GLOB_RECURSE RENDERER_SRCS   ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/*.cpp    )
file(GLOB_RECURSE UI_SRCS         ${CMAKE_CURRENT_SOURCE_DIR}/src/UI/*.cpp          )
file(GLOB_RECURSE ASSET_SRCS      ${CMAKE_CURRENT_SOURCE_DIR}/src/Asset/*.cpp       )
file(GLOB_RECURSE NAVIGATION_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/Navigation/*.cpp  )
file(GLOB_RECURSE TILE_SRCS       ${CMAKE_CURRENT_SOURCE_DIR}/src/TileGrid/*.cpp    )
file(GLOB_RECURSE ANIMATION_SRCS  ${CMAKE_CURRENT_SOURCE_DIR}/src/Animation/*.cpp   )
file(GLOB_RECURSE ENTITY_SYS_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/EntitySystem/*.cpp)
file(GLOB_RECURSE MATH_SRCS       ${CMAKE_CURRENT_SOURCE_DIR}/src/Math/*.cpp        )
file(GLOB_RECURSE SCENE_SRCS      ${CMAKE_CURRENT_SOURCE_DIR}/src/Scene/*.cpp       )
file(GLOB_RECURSE COMPONENTS_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/Components/*.cpp   )
file(GLOB_RECURSE PROCEDURAL_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/Procedural/*.cpp  )

add_executable(${PROJECT_NAME}
        ${SRCS}
        ${CORE_SRCS}
        ${RENDERER_SRCS}
        ${UI_SRCS}
        ${ASSET_SRCS}
        ${NAVIGATION_SRCS}
        ${TILE_SRCS}
        ${IMGUI_SRCS}
        ${ANIMATION_SRCS}
        ${ENTITY_SYS_SRCS}
        ${MATH_SRCS}
        ${SCENE_SRCS}
        ${COMPONENTS_SRCS}
        ${PROCEDURAL_SRCS}
        ${IMGUI_BACKENDS}/imgui_impl_sdl2.cpp
        ${IMGUI_BACKENDS}/imgui_impl_opengl3.cpp)

target_precompile_headers(${PROJECT_NAME} PRIVATE ./src/PCH.h)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/vendors/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/vendors/imgui/backends
        ${CMAKE_CURRENT_SOURCE_DIR}/vendors/stb
        ${CMAKE_CURRENT_SOURCE_DIR}/vendors/assimp/include
        ${CMAKE_CURRENT_SOURCE_DIR}/vendors/csv)

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
target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res
        ${CMAKE_CURRENT_BINARY_DIR}/res
        COMMENT "Copying game assets..."
)

# --- COPY RESOURCES INTO THE APP BUNDLE ---
#add_custom_command(
#        TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E make_directory
#        "$<TARGET_BUNDLE_CONTENT_DIR:${PROJECT_NAME}>/Resources/res"
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        ${CMAKE_SOURCE_DIR}/res
#        "$<TARGET_BUNDLE_CONTENT_DIR:${PROJECT_NAME}>/Resources/res"
#        COMMENT "Copying game assets into .app bundle..."
#)

