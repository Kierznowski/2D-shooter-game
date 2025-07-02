#include "player.h"
#include "tilemap.h"

#include <math.h>

#define PLAYER_ROTATION_SPEED 4.0f // rads per sec
#define PLAYER_SPEED 300.0f // pixels per sec
#define PLAYER_WIDTH 30
#define PLAYER_HEIGHT 30

bool host = false;

static SDL_Texture *player_texture = NULL;
static SDL_Texture *opponent_texture = NULL;

void player_init(Player *player, int x, int y) {
    player->x = x;
    player->y = y;
    player->angle = 0.0f;
    player->ammo = 25;
    player->health = 100;
}

void player_set_textures(SDL_Renderer *renderer) {
    SDL_Surface *surface = SDL_LoadBMP("assets/textures/wall.bmp");
    if (!surface) {
        SDL_Log("BMP loading failed: %s", SDL_GetError());
        exit(1);
    }

    player_texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!player_texture) {
        SDL_Log("Player texture creation failed: %s", SDL_GetError());
        exit(1);
    }
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

void player_render(SDL_Renderer *renderer, Player *player, float camera_x, float camera_y, bool opponent) {
    float screen_x = player->x - camera_x;
    float screen_y = player->y - camera_y;

    SDL_Rect dst = {
        (int)(screen_x - PLAYER_WIDTH / 2),
        (int)(screen_y - PLAYER_HEIGHT / 2),
        PLAYER_WIDTH,
        PLAYER_HEIGHT
    };

    SDL_Point center = {PLAYER_WIDTH / 2, PLAYER_HEIGHT / 2};

    if (opponent) {
        SDL_SetTextureColorMod(player_texture, 0, 128, 128);
    } else {
        SDL_SetTextureColorMod(player_texture, 128, 64, 0);
    }


    SDL_RenderCopyEx(renderer, player_texture, NULL, &dst, player->angle*(180 / M_PI), &center, SDL_FLIP_NONE);
}

void player_destroy_texture() {
    SDL_DestroyTexture(player_texture);
}
