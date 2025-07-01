#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>

typedef struct {
    float x, y; // position
    float angle; // facing direction in rads
    int health;
    int ammo;
} Player;

void player_init(Player *player, int x, int y);
void player_update(Player *player, const Uint8 *key_state, float deltaTime);
void player_render(SDL_Renderer *renderer, Player *player, float camera_x, float camera_y);

#endif //PLAYER_H
