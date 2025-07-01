#include <SDL2/SDL.h>
#include <stdbool.h>
#include "player.h"
#include "bullet.h"
#include "tilemap.h"
#include "hud.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 640

bool running = true;

float camera_x = 0;
float camera_y = 0;

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Shooter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    Player player;
    player_init(&player, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 100);
    Bullet bullets[MAX_BULLETS] = {0};
    tilemap_load(renderer);

    Uint32 last_time = SDL_GetTicks();

    while (running) {
        // Input
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        Uint8 *key_state = SDL_GetKeyboardState(NULL);

        // Update
        Uint32 current_time = SDL_GetTicks();
        const float delta_time = (current_time - last_time) / 1000.0f;
        last_time = current_time;
        player_update(&player, key_state, delta_time);
        bullet_update_all(bullets, key_state, delta_time, player.x, player.y, player.angle, &player.ammo);
        camera_x = player.x - SCREEN_WIDTH / 2;
        camera_y = player.y - SCREEN_HEIGHT / 2;

        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer); // clear screen

        tilemap_render(renderer, camera_x, camera_y);
        player_render(renderer, &player, camera_x, camera_y);
        bullet_render_all(renderer, bullets, camera_x, camera_y);
        hud_render(renderer, &player);

        SDL_RenderPresent(renderer);
    }

    tilemap_unload();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
