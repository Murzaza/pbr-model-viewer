cmake_minimum_required(VERSION 3.20)
project(ogl-pbr-viewer)

## Find deps
find_package(glfw3 REQUIRED)
find_package(glad REQUIRED)
find_package(glm REQUIRED)
find_package(Freetype REQUIRED)
find_package(imgui REQUIRED)

# stb does not have a CMake build, but is header-only
find_path(STB_INCLUDE_DIRS, "stb.h")
# same with tinygltf
find_path(TINYGLTF_INCLUDE_DIRS, "tiny_gltf.h")

file(GLOB HEADERS inc/*.hpp)
file(GLOB SOURCES src/*.cpp)
file(GLOB SHADERS shaders/*.comp
                  shaders/*.vert
                  shaders/*.frag
                  shaders/*.geom)

source_group("Headers", FILES ${HEADERS})
source_group("Sources", FILES ${SOURCES})

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")

# Create EXE
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${SHADERS})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${STB_INCLUDE_DIRS}
        ${TINYGLTF_INCLUDE_DIRS}
        "${CMAKE_CURRENT_LIST_DIR}/inc")

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        Freetype::Freetype
        glfw
        glad::glad
        glm::glm
        imgui::imgui
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>
    DEPENDS ${SHADERS})
