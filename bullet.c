#include "bullet.h"
#include <math.h>

#include "tilemap.h"

#define BULLET_SPEED 400.f;
#define BULLET_RADIUS 3.0f

void bullet_fire(Bullet bullets[], float x, float y, float angle) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].x = x;
            bullets[i].y = y;
            bullets[i].vx = cosf(angle) * BULLET_SPEED;
            bullets[i].vy = sinf(angle) * BULLET_SPEED;
            bullets[i].active = true;
            break;
        }
    }
}

void bullet_update_all(Bullet bullets[], const Uint8 *key_state, float delta_time, float x, float y, float angle, int *player_ammo) {
    static bool prev_space = false;
    bool current_space = key_state[SDL_SCANCODE_SPACE];
    if (*player_ammo <= 0) {
        current_space = false;
    }

    if (current_space && !prev_space) {
        float muzzle_x = x + cosf(angle) * 20.0f;
        float muzzle_y = y + sinf(angle) * 20.0f;
        bullet_fire(bullets, muzzle_x, muzzle_y, angle);
        (*player_ammo)--;
        printf("player ammo: %d\n", *player_ammo);
    }

    prev_space = current_space;

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].x += bullets[i].vx * delta_time;
            bullets[i].y += bullets[i].vy * delta_time;

            if (tilemap_is_colliding(bullets[i].x, bullets[i].y)) {
                bullets[i].active = false;
            }

            if (bullets[i].x < 0 || bullets[i].x > 800 ||
                bullets[i].y < 0 || bullets[i].y > 600) {
                bullets[i].active = false;
            }
        }
    }
}

void bullet_render_all(SDL_Renderer *renderer, Bullet bullets[], float camera_x, float camera_y) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            SDL_Rect bullet_rect = {
                (int)(bullets[i].x - BULLET_RADIUS - camera_x),
                (int)(bullets[i].y - BULLET_RADIUS - camera_y),
                (int) BULLET_RADIUS * 2,
                (int) BULLET_RADIUS * 2
            };
            SDL_RenderFillRect(renderer, &bullet_rect);
        }
    }
}