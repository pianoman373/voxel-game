cmake_minimum_required(VERSION 3.0)
project(voxel-game)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(lib/glfw)

include_directories(lib/glew)
link_libraries(${GLEW_LIBRARIES})
add_library (glew lib/glew/src/glew.c)

include_directories(lib/glfw/include/
                    lib/stb/
                    lib/glew/include
                    lib/imgui/
                    lib/glm/glm/
                    )

file(GLOB PROJECT_SOURCES src/*.cpp
                          src/*.c
                          lib/imgui/*.cpp
                          lib/imgui/*.c
                          )
file(GLOB PROJECT_HEADERS src/*.h
                          src/*.hpp
                          )

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES}
                               ${PROJECT_HEADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(WIN32)
	target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} glew opengl32)
elseif(UNIX AND NOT APPLE)
	target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} glew GL)
endif(WIN32)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                               COMMAND ${CMAKE_COMMAND} -E copy_directory
                               ${CMAKE_SOURCE_DIR}/resources/
                               $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/)
