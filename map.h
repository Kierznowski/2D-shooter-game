#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <SDL2/SDL.h>

#define MAX_WALLS 64

typedef struct {
    SDL_Rect rect;
} Wall;

extern Wall walls[MAX_WALLS];
extern int walls_count;

void map_init();
void map_render(SDL_Renderer *renderer);

bool map_check_player_collision(float next_x, float next_y, int player_w, int player_h);
bool map_check_bullet_collision(float next_x, float next_y, int bullet_w, int bullet_h);

#endif //MAP_H
