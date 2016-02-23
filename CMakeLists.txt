cmake_minimum_required(VERSION 3.3)
project(Tracer)



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp -g")

add_definitions(-D_USE_MATH_DEFINES -DGLEW_STATIC)
include_directories(${CMAKE_SOURCE_DIR} $ENV{FREEGLUT}/include)

set(SOURCE_FILES Main.cpp Scene.cpp Tracer.cpp glew.c Material.h Hit.h Filter.h Filter.cpp PostProcess.cpp PostProcess.h Material.cpp BVH.cpp BVH.h EnvMap.h HDRloader/hdrloader.cpp HDRloader/hdrloader.h Texture.cpp Texture.h Object.cpp)

add_executable(Tracer ${SOURCE_FILES})

target_link_libraries(Tracer ${OPENGL_LIBRARIES} opengl32.lib $ENV{FREEGLUT}/lib/freeglut.lib)