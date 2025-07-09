#ifndef HUD_H
#define HUD_H

#include "player.h"

void hud_init(SDL_Renderer *renderer);
void hud_destroy();
void hud_set_blood_screen(bool blood);
void hud_render(SDL_Renderer* renderer, Player *player);

#endif //HUD_H
