#ifndef MAP_H
#define MAP_H

#include <SDL2/SDL.h>

#define MAX_WALLS 64

typedef struct {
    SDL_Rect rect;
} Wall;

extern Wall walls[MAX_WALLS];
extern int walls_count;

void map_init();
void map_render(SDL_Renderer *renderer);

#endif //MAP_H
