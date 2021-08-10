cmake_minimum_required(VERSION 3.20)
project(json_comparison)

set(CMAKE_CXX_STANDARD 11)

add_executable(json_comparison main.cpp picojson.h json.h)
