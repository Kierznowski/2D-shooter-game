#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
    int x, y; // position
    float angle; // facing direction in rads
    int health;
    int ammo;
} Player;

void player_set_textures(SDL_Renderer *renderer);
void player_init(Player *player, int x, int y);
void player_update(Player *player, const Uint8 *key_state, float deltaTime);
void player_render(SDL_Renderer *renderer, Player *player, float camera_x, float camera_y, bool opponent);
void player_destroy_texture();

#endif //PLAYER_H
