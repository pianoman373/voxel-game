cmake_minimum_required(VERSION 3.0)
project(voxel-game)

add_subdirectory(lib/luajit)

add_subdirectory(lib/enet)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(lib/glfw)

include_directories(lib/crucible/include/
                    lib/luajit/src/
                    ${CMAKE_BINARY_DIR}/lib/luajit
                    lib/enet/include/

                    lib/crucible/include/
                    lib/GLFW/include/
                    lib/stb/
                    lib/imgui/
                    lib/freetype/include/
                    lib/glad/include/

                    src/
                    )

file(GLOB PROJECT_SOURCES src/*.cpp
                          src/*.c

                          src/*/*.cpp
                          src/*/*.c

                          src/*/*/*.cpp
                          src/*/*/*.c

                          lib/imgui/imgui.cpp
                          lib/imgui/imgui_demo.cpp
                          lib/imgui/imgui_draw.cpp
                          lib/imgui/imgui_widgets.cpp
                          lib/imgui/examples/imgui_impl_glfw.cpp
                          lib/imgui/examples/imgui_impl_opengl3.cpp
                          lib/glad/glad.c
                          )
file(GLOB PROJECT_HEADERS src/*.h
                          src/*.hpp

                          src/*/*.h
                          src/*/*.hpp
                          
                          src/*/*/*.h
						  src/*/*/*.hpp
                          )

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\"
                -DIMGUI_IMPL_OPENGL_LOADER_GLAD)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})

if(WIN32)
    target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} opengl32 liblua enet ws2_32 winmm)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} GL liblua enet)
endif(WIN32)

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


source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Headers" FILES ${PROJECT_HEADERS})
