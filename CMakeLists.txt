cmake_minimum_required(VERSION 3.0)
project(voxel-game)

option(CRUCIBLE_BUILD_EXAMPLES OFF)
add_subdirectory(lib/crucible)

add_subdirectory(lib/luajit)

add_subdirectory(lib/enet)

include_directories(lib/crucible/include/
                    lib/luajit/src/
                    ${CMAKE_BINARY_DIR}/lib/luajit
                    lib/crucible/lib/GLFW/include/
                    lib/crucible/lib/bullet/src/
                    lib/crucible/lib/assimp/include/
                    lib/crucible/lib/stb/
                    lib/crucible/lib/imgui/
                    lib/enet/include/
                    )

file(GLOB PROJECT_SOURCES src/*.cpp
                          src/*.c
                          )
file(GLOB PROJECT_HEADERS src/*.h
                          src/*.hpp
                          )

add_definitions(-DSFML_STATIC -DGLFW_STATIC)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})

target_link_libraries(${PROJECT_NAME} crucible liblua enet)

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    if(WIN32)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -Wall -O3 -Wextra -Wpedantic -std=c++14 -static-libgcc -static-libstdc++")
    elseif(UNIX AND NOT APPLE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -Wextra -Wpedantic -std=c++14")
    endif()

endif()
