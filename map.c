#include "map.h"

Wall walls[MAX_WALLS];
int wall_count = 0;

void map_init() {
    wall_count = 0;

    walls[wall_count++].rect = (SDL_Rect) {200, 150, 100, 100};
    walls[wall_count++].rect = (SDL_Rect) {400, 300, 50, 200};
    walls[wall_count++].rect = (SDL_Rect) {600, 100, 150, 50};
}

void map_render(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    for (int i = 0; i < wall_count; i++) {
        SDL_RenderFillRect(renderer, &walls[i].rect);
    }
}