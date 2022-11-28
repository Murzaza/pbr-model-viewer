# Adapted from https://github.com/Polytonic/Glitter/blob/master/CMakeLists.txt
cmake_minimum_required(VERSION 3.6)
project(ogl-scene-viewer)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TEST OFF)
add_subdirectory(vendor/glfw)

option(TINYGLTF_BUILD_LOADER_EXAMPLE OFF)
option(TINYGLTF_BUILD_GL_EXAMPLES OFF)
option(TINYGLTF_BUILD_VALIDATOR_EXAMPLE OFF)
option(TINYGLTF_BUILD_BUILDER_EXAMPLE OFF)
option(TINYGLTF_HEADER_ONLY ON)
option(TINYGLTF_INSTALL OFF)
add_subdirectory(vendor/tinygltf)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpendantic -std=c++17")
  if(NOT WIN32)
    set(GLAD_LIBRARIES dl)
  endif()
endif()

include_directories(inc/
                    vendor/tinygltf/
                    vendor/glad/include/
                    vendor/glm/
                    vendor/glfw/include/)

file(GLOB VENDORS_SOURCES vendor/glad/src/glad.c)
file(GLOB PROJECT_HEADERS inc/*.hpp)
file(GLOB PROJECT_SOURCES src/*.cpp)
file(GLOB PROJECT_TEXTURES textures/*.jpg
                           textures/*.png)
file(GLOB PROJECT_SHADERS shaders/*.comp
                          shaders/*.vert
                          shaders/*.frag
                          shaders/*.geom)
file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                          .gitattributes
                          .gitignore
                          .gitmodules)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Shaders" FILES ${PROJECT_SHADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Vendors" FILES ${VENDORS_SOURCES})
source_group("Textures" FILES ${PROJECT_TEXTURES})

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
                               ${VENDORS_SOURCES})
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} ${GLAD_LIBRARIES})
set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>
  DEPENDS ${PROJECT_SHADERS})
