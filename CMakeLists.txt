cmake_minimum_required(VERSION 3.0)
project(voxel-game)

option(CRUCIBLE_BUILD_EXAMPLES OFF)
add_subdirectory(lib/crucible)

add_subdirectory(lib/luajit)

include_directories(lib/crucible/include/
                    lib/luajit/src/
                    ${CMAKE_BINARY_DIR}/lib/luajit
                    )

file(GLOB PROJECT_SOURCES src/*.cpp
                          src/*.c
                          )
file(GLOB PROJECT_HEADERS src/*.h
                          src/*.hpp
                          )

add_definitions(-DSFML_STATIC -DGLFW_STATIC)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})

if(WIN32)
    target_link_libraries(${PROJECT_NAME} crucible liblua)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} crucible liblua)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++14")
endif()
