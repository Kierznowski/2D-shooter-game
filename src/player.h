#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "bullet.h"

typedef struct {
    int x, y; // position
    float angle; // facing direction in rads
    int health;
    int ammo;
} Player;

void player_set_textures(SDL_Renderer *renderer, bool opponent);
void player_init(Player *player, int x, int y);
void player_update(Player *player, const Uint8 *key_state, float deltaTime, Bullet *bullets);
void player_check_collision_with_bullets(Player *player, Bullet *bullets);
void player_render(SDL_Renderer *renderer, Player *player, float camera_x, float camera_y, bool opponent);
void player_destroy_texture();

#endif //PLAYER_H
