cmake_minimum_required(VERSION 3.31)
project(2D_shooter_game C)
set(CMAKE_C_STANDARD 11)

# Find SDL2 package installed on the system
find_package(SDL2 REQUIRED)

# Include SDL2 headers
include_directories(${SDL_INCLUDE_DIRS})

add_executable(2D_shooter_game main.c
        player.c
        player.h
        bullet.c
        bullet.h)

# Link SDL2 + math libraries
target_link_libraries(2D_shooter_game ${SDL2_LIBRARIES} m)
