cmake_minimum_required(VERSION 3.16)

project(dim0126-ia-jogos-atv1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall ")

add_subdirectory(lib/raylib)


add_library(ia-jogos INTERFACE)

target_include_directories(ia-jogos INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(dim0126-ia-jogos-atv1
  src/main.cpp
  src/Player.cpp
  src/Enemy.cpp
  src/Bullet.cpp
  src/Game.cpp
  src/World.cpp
)

target_link_libraries(dim0126-ia-jogos-atv1
  PRIVATE 
    ia-jogos
    raylib
)