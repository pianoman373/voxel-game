# Voxel Game

[![GitHub issues](https://img.shields.io/github/issues/pianoman373/voxel-game.svg)](https://github.com/pianoman373/voxel-game/issues)
[![GitHub (pre-)release](https://img.shields.io/github/release/pianoman373/voxel-game/all.svg)](https://github.com/pianoman373/voxel-game/releases)
[![Github All Releases](https://img.shields.io/github/downloads/pianoman373/voxel-game/total.svg)](https://github.com/pianoman373/voxel-game/releases)

A yet unnamed project which aims to be a moddable voxel game written in modern C++11 inspired by Minecraft and Minetest.

# Building

This project uses [cmake](http://www.cmake.org/download/) as it's build system. Setting up a project for your particular IDE via cmake is pretty simple
and instructions can be found on the cmake website.

# Running

Voxel-Game requires several files that are available from the root folder of the project and the resources folder. If you attempt to run
the program after a build the program will crash because it can't find the resources folder. To fix this set your working directory to the
project root folder for debugging/running.