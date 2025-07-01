#include "hud.h"

#include <SDL_render.h>

void hud_render(SDL_Renderer *renderer, Player *player) {
    const int bar_width = 200;
    const int bar_height = 20;
    const int bar_x = 70;
    const int bar_y = 5;

    SDL_Rect bg = {bar_x, bar_y, bar_width, bar_height};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &bg);

    SDL_Rect fg = {bar_x, bar_y, bar_width * player->health / 100, bar_height};
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    SDL_RenderFillRect(renderer, &fg);

    SDL_Rect bullet_rect;
    bullet_rect.y = bar_y;
    bullet_rect.w = 10;
    bullet_rect.h = 20;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < player->ammo; i++) {
        bullet_rect.x = 400 + i * 12;
        SDL_RenderFillRect(renderer, &bullet_rect);
    }
}
