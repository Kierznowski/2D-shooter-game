#ifndef BULLET_H
#define BULLET_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#define MAX_BULLETS 20

typedef struct {
    float x, y;
    float vx, vy; // velocity
    bool active;
} Bullet;

void bullet_fire(Bullet bullets[], float x, float y, float angle);
void bullet_update_all(Bullet bullets[], const Uint8 *key_state, float delta_time, float x, float y, float angle, int *player_ammo);
void bullet_render_all(SDL_Renderer *renderer, Bullet bullets[], float camera_x, float camera_y);

#endif //BULLET_H
