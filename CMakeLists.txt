cmake_minimum_required(VERSION 3.0)
project(voxel-game)

add_subdirectory(lib/sfml)
add_subdirectory(lib/crucible)

link_directories(lib/luajit/lib)

include_directories(lib/crucible/lib/glfw/include/
                    lib/crucible/lib/stb/
                    lib/crucible/lib/imgui/
					lib/crucible/lib/glad/include/
                    lib/crucible/src/
                    lib/sfml/include/
                    lib/luajit/include/
                    )

file(GLOB PROJECT_SOURCES src/*.cpp
                          src/*.c
                          )
file(GLOB PROJECT_HEADERS src/*.h
                          src/*.hpp
                          )

add_definitions(-DSFML_STATIC)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})

if(WIN32)
    target_link_libraries(${PROJECT_NAME} crucible sfml-network sfml-graphics sfml-window sfml-system luajit lua51)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} crucible sfml-network sfml-graphics sfml-window sfml-system luajit-5.1)
endif(WIN32)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++14")
endif()
