#include "player.h"
#include "tilemap.h"

#include <math.h>

#define PLAYER_ROTATION_SPEED 4.0f // rads per sec
#define PLAYER_SPEED 300.0f // pixels per sec
#define PLAYER_WIDTH 30
#define PLAYER_HEIGHT 30

void player_init(Player *player, int x, int y) {
    player->x = x;
    player->y = y;
    player->angle = 0.0f;
    player->ammo = 25;
    player->health = 100;
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
    float next_x = player->x;
    float next_y = player->y;

    if (key_state[SDL_SCANCODE_W]) {
        next_x += dx * PLAYER_SPEED * delta_time;
        next_y += dy * PLAYER_SPEED * delta_time;
    }

    if (key_state[SDL_SCANCODE_S]) {
        next_x -= dx * PLAYER_SPEED * delta_time;
        next_y -= dy * PLAYER_SPEED * delta_time;
    }

    int player_half_width = PLAYER_WIDTH / 2;
    int player_half_height = PLAYER_HEIGHT / 2;

    if (tilemap_is_colliding(next_x + player_half_width, player->y + player_half_height) ||
        tilemap_is_colliding(next_x + player_half_width, player->y - player_half_height) ||
        tilemap_is_colliding(next_x - player_half_width, player->y + player_half_height) ||
        tilemap_is_colliding(next_x - player_half_width, player->y - player_half_height)) {
        next_x = player->x;
    }

    if (tilemap_is_colliding(player->x + player_half_width, next_y + player_half_height) ||
        tilemap_is_colliding(player->x + player_half_height, next_y - player_half_height) ||
        tilemap_is_colliding(player->x - player_half_width, next_y + player_half_height) ||
        tilemap_is_colliding(player->x - player_half_width, next_y - player_half_height)) {
        next_y = player->y;
    }

    player->x = next_x;
    player->y = next_y;
}

void player_render(SDL_Renderer *renderer, Player *player, float camera_x, float camera_y) {
    float screen_x = player->x - camera_x;
    float screen_y = player->y - camera_y;

    // player body
    const int w = PLAYER_WIDTH;
    const int h = PLAYER_HEIGHT;

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

    SDL_Rect player_body = {
        (int)(screen_x - x_cen),
        (int)(screen_y - y_cen),
        w, h
    };
    SDL_SetRenderDrawColor(renderer, 0, 128, 128, 255);
    SDL_RenderFillRect(renderer, &player_body);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer,
        (int)screen_x, (int)screen_y,
        (int)(screen_x + cosf(player->angle) * x_cen),
        (int)(screen_y + sinf(player->angle) * x_cen));
}
