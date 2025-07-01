#ifndef TILEMAP_H
#define TILEMAP_H

#include <stdbool.h>
#include <SDL2/SDL.h>

#define TILE_SIZE 32

void tilemap_load(SDL_Renderer *renderer);
void tilemap_render(SDL_Renderer *renderer, float camera_x, float camera_y);
int tilemap_get_tile(int world_x, int world_y);
bool tilemap_is_colliding(int world_x, int world_y);
void tilemap_unload();

#endif //TILEMAP_H
