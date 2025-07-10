#include "player.h"
#include "tilemap.h"

#include <math.h>
#include <SDL2/SDL_image.h>

#define PLAYER_ROTATION_SPEED 4.0f // rads per sec
#define PLAYER_SPEED 300.0f // pixels per sec
#define PLAYER_WIDTH 30
#define PLAYER_HEIGHT 30

bool host = false;
bool *collision_mask;

static SDL_Texture *player_texture = NULL;
static SDL_Texture *opponent_texture = NULL;

bool player_collision_with_tilemap(Player *player);

void player_init(Player *player, int x, int y) {
    player->x = x;
    player->y = y;
    player->angle = 0.0f;
    player->ammo = 100;
    player->health = 100;
}

void player_set_textures(SDL_Renderer *renderer, bool opponent) {
    SDL_Surface *surface;
    if (opponent) {
        surface = IMG_Load("assets/sprites/ufoBlack.png");
    } else {
        surface = IMG_Load("assets/sprites/ufoBlue.png");
    }
    if (!surface) {
        SDL_Log("Player sprite loading failed: %s", SDL_GetError());
        exit(1);
    }

    SDL_Surface *formatted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface);
    if (!formatted) {
        SDL_Log("Unable to convert surface format: %s", SDL_GetError());
        exit(1);
    }

    int width = formatted->w;
    int height = formatted->h;

    SDL_LockSurface(formatted);

    collision_mask = malloc(width * height);
    if (!collision_mask) {
        SDL_Log("Unable to allocate collision mask: %s", SDL_GetError());
        SDL_UnlockSurface(formatted);
        SDL_FreeSurface(formatted);
        exit(1);
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int pixel_index = y * width + x;
            Uint8 *pixel_ptr = (Uint8 *)formatted->pixels + pixel_index * 4;
            Uint8 alpha = pixel_ptr[3];
            collision_mask[pixel_index] = (alpha > 32);
        }
    }

    SDL_UnlockSurface(formatted);

    if (opponent) {
        opponent_texture = SDL_CreateTextureFromSurface(renderer, formatted);
        SDL_SetTextureBlendMode(opponent_texture, SDL_BLENDMODE_BLEND);
    } else {
        player_texture = SDL_CreateTextureFromSurface(renderer, formatted);
        SDL_SetTextureBlendMode(player_texture, SDL_BLENDMODE_BLEND);
    }
    SDL_FreeSurface(formatted);
}

void player_update(Player *player, const Uint8 *key_state, float delta_time, Bullet *bullets) {
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
        SDL_RenderCopyEx(renderer, opponent_texture, NULL, &dst, player->angle*(180 / M_PI), &center, SDL_FLIP_NONE);
    } else {
        SDL_RenderCopyEx(renderer, player_texture, NULL, &dst, player->angle*(180 / M_PI), &center, SDL_FLIP_NONE);
    }
}

bool player_check_collision_with_bullets(Player *player, Bullet *bullets) {
    SDL_Rect player_rect = {player->x - PLAYER_WIDTH / 2, player->y - PLAYER_HEIGHT / 2, PLAYER_WIDTH, PLAYER_HEIGHT};
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;
        SDL_Rect bullet_rect = {bullets[i].x - BULLET_RADIUS, bullets[i].y - BULLET_RADIUS, BULLET_RADIUS * 2, BULLET_RADIUS * 2};
        if (SDL_HasIntersection(&bullet_rect, &player_rect)) {
            bullets[i].active = false;
            player->health -= 5;
            return true;
        }
    }
    return false;
}

void player_destroy_texture() {
    free(collision_mask);
    SDL_DestroyTexture(player_texture);
}


void player_update_state(Player *player, Bullet *bullets, PlayerPacket *packet) {
    player->x = packet->x;
    player->y = packet->y;
    player->angle = packet->angle;
    player->health = packet->health;
    player->ammo = packet->ammo;
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].x = packet->bullets[i].x;
        bullets[i].y = packet->bullets[i].y;
        bullets[i].vx = packet->bullets[i].vx;
        bullets[i].vy = packet->bullets[i].vy;
        bullets[i].active = packet->bullets[i].active;
    }
}

void player_prepare_state(Player *player, Bullet *bullets, PlayerPacket *packet) {
    packet->x = player->x;
    packet->y = player->y;
    packet->angle = player->angle;
    packet->health = player->ammo;
    packet->ammo = player->ammo;
    for (int i = 0; i < MAX_BULLETS; i++) {
        packet->bullets[i].x = bullets[i].x;
        packet->bullets[i].y = bullets[i].y;
        packet->bullets[i].vx = bullets[i].vx;
        packet->bullets[i].vy = bullets[i].vy;
        packet->bullets[i].active = bullets[i].active;
    }
}
