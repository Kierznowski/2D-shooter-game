#include "map.h"

Wall walls[MAX_WALLS];
int wall_count = 0;

void map_init() {
    wall_count = 0;

    walls[wall_count++].rect = (SDL_Rect) {200, 150, 100, 100};
    walls[wall_count++].rect = (SDL_Rect) {500, 300, 50, 200};
    walls[wall_count++].rect = (SDL_Rect) {600, 100, 150, 50};
}

void map_render(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    for (int i = 0; i < wall_count; i++) {
        SDL_RenderFillRect(renderer, &walls[i].rect);
    }
}

bool map_check_player_collision(float next_x, float next_y, int player_w, int player_h) {
    SDL_Rect next_pos = {
        (int)(next_x - player_w / 2),
        (int)(next_y - player_h / 2),
        player_w,
        player_h
    };

    for (int i = 0; i < wall_count; i++) {
        if (SDL_HasIntersection(&next_pos, &walls[i].rect)) {
            return true;
        }
    }
    return false;
}

bool map_check_bullet_collision(float next_x, float next_y, int bullet_w, int bullet_h) {
    SDL_Rect bullet_pos = {
        (int) (next_x - bullet_w / 2),
        (int) (next_y - bullet_h / 2),
        bullet_w,
        bullet_h
    };
    for (int i = 0; i < wall_count; i++) {
        if (SDL_HasIntersection(&bullet_pos, &walls[i].rect)) {
            return true;
        }
    }
    return false;
}