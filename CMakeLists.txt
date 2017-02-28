cmake_minimum_required(VERSION 3.0)
project(voxel-game)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(lib/glfw)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

include_directories(lib/glfw/include/
                    lib/stb/
                    lib/glm/
                    lib/AntTweakBar/include/
                    lib/glew-2.0.0/include/)

link_directories(lib/AntTweakBar/lib/
				 lib/glew-2.0.0/lib/)

file(GLOB PROJECT_SOURCES src/*.cpp
                          src/*.c)
file(GLOB PROJECT_HEADERS src/*.h
                          src/*.hpp)
file(GLOB PROJECT_RESOURCES resources/*.* resources/shaders/*.*)

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES}
                               ${PROJECT_HEADERS}
                               ${PROJECT_RESOURCES})

target_link_libraries(${PROJECT_NAME} glfw GLEW GL AntTweakBar)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
