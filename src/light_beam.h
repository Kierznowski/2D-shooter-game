#ifndef LIGHT_BEAM_H
#define LIGHT_BEAM_H

#include <SDL2/SDL.h>

void light_beam_init(SDL_Renderer* renderer, int width, int height);
void light_beam_render(SDL_Renderer *renderer, float player_angle);
void light_beam_destroy();

#endif //LIGHT_BEAM_H
