cmake_minimum_required(VERSION 2.8)
project(AntTweakBar)

include_directories(include)

file(GLOB_RECURSE SOURCES src/TwColors.cpp
    src/TwFonts.cpp
    src/TwOpenGL.cpp
    src/TwOpenGLCore.cpp
    src/TwBar.cpp
    src/TwMgr.cpp
    src/TwPrecomp.cpp
    src/LoadOGL.cpp
    src/LoadOGLCore.cpp
    src/TwEventGLFW.c
    src/TwEventGLUT.c
    src/TwEventSDL.c
    src/TwEventSDL12.c
    src/TwEventSDL13.c
    src/TwEventSFML.cpp
    src/TwEventX11.c)

ADD_DEFINITIONS(-D_UNIX)
	 
add_library(AntTweakBar ${SOURCES})
target_link_libraries(AntTweakBar GL)
