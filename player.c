#include "player.h"
#include <math.h>

#define PLAYER_ROTATION_SPEED 3.0f // rads per sec
#define PLAYER_SPEED 200.0f // pixels per sec

void player_init(Player *player, int x, int y) {
    player->x = x;
    player->y = y;
    player->angle = 0.0f;
}

void player_update(Player *player, const Uint8 *key_state, float delta_time) {
    if (key_state[SDL_SCANCODE_A]) {
        player->angle -= PLAYER_ROTATION_SPEED * delta_time;
    }
    if (key_state[SDL_SCANCODE_D]) {
        player->angle += PLAYER_ROTATION_SPEED * delta_time;
    }

    // Forward/Backward
    float dx = cosf(player->angle);
    float dy = sinf(player->angle);

    if (key_state[SDL_SCANCODE_W]) {
        player->x += dx * PLAYER_SPEED * delta_time;
        player->y += dy * PLAYER_SPEED * delta_time;
    }

    if (key_state[SDL_SCANCODE_S]) {
        player->x -= dx * PLAYER_SPEED * delta_time;
        player->y -= dy * PLAYER_SPEED * delta_time;
    }
}

void player_render(SDL_Renderer *renderer, Player *player) {
    // player body
    const int w = 40;
    const int h = 40;

    // player center
    float x_cen = w / 2.0f;
    float y_cen = h / 2.0f;

    // Player corners position before rotation
    SDL_FPoint rect[4] = {
        {-x_cen, -y_cen},   // upper-left
        {x_cen, -y_cen},    // upper-right
        {x_cen, y_cen},     // bottom-right
        {-x_cen, y_cen},    // bottom-left
    };

    // Player corners after rotation
    SDL_FPoint points[4];
    // Rotate player
    for (int i = 0; i < 4; i++) {
        points[i].x = rect[i].x * cosf(player->angle) - rect[i].y * sinf(player->angle) + player->x;
        points[i].y = rect[i].x * sinf(player->angle) + rect[i].y * cosf(player->angle) + player->y;
    }

    SDL_Rect player_body = {(int)(player->x - x_cen), (int)(player->y - x_cen), w, h};
    SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);
    SDL_RenderFillRect(renderer, &player_body);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer,
        (int)player->x, (int)player->y,
        (int)(player->x + cosf(player->angle) * x_cen),
        (int)(player->y + sinf(player->angle) * x_cen));
}
