#include <SDL2/SDL.h>
#include <stdbool.h>
#include "player.h"
#include "bullet.h"
#include "tilemap.h"
#include "hud.h"
#include "network.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 640


float camera_x = 0;
float camera_y = 0;

int main(int argc, char *argv[]) {

    if (argc < 3) {
        printf("Usage: %s host <port> %s client <ip> <port>\n", argv[0], argv[0]);
        return 1;
    }

    bool running = true;
    bool is_host = false;
    const char *ip = NULL;
    int port = 0;

    set_connection(&is_host, &port, &ip, argv, argc);

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
    Player remote_player;

    player_init(&player, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 100);
    player_init(&remote_player, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 150);

    Bullet bullets[MAX_BULLETS] = {0};
    Bullet remote_bullets[MAX_BULLETS] = {0};

    tilemap_load(renderer);

    Uint32 last_time = SDL_GetTicks();

    PlayerPacket local_packet = {0};
    PlayerPacket remote_packet = {0};

    while (running) {
        if (!is_host) {
            if (network_recv_player_packet(&remote_packet)) {
                remote_player.x = remote_packet.x;
                remote_player.y = remote_packet.y;
                remote_player.angle = remote_packet.angle;
                remote_player.health = remote_packet.health;
                remote_player.ammo = remote_packet.ammo;
                for (int i = 0; i < MAX_BULLETS; i++) {
                    remote_bullets[i].x = remote_packet.bullets[i].x;
                    remote_bullets[i].y = remote_packet.bullets[i].y;
                    remote_bullets[i].vx = remote_packet.bullets[i].vx;
                    remote_bullets[i].vy = remote_packet.bullets[i].vy;
                    remote_bullets[i].active = remote_packet.bullets[i].active;
                }
            }
        }

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
        bullet_update_all(bullets, key_state, delta_time, player.x, player.y, player.angle, &player.ammo);
        camera_x = player.x - SCREEN_WIDTH / 2;
        camera_y = player.y - SCREEN_HEIGHT / 2;

        // Send state
        local_packet.x = player.x;
        local_packet.y = player.y;
        local_packet.angle = player.angle;
        local_packet.health = player.health;
        local_packet.ammo = player.ammo;
        for (int i = 0; i < MAX_BULLETS; i++) {
            local_packet.bullets[i].x = bullets[i].x;
            local_packet.bullets[i].y = bullets[i].y;
            local_packet.bullets[i].vx = bullets[i].vx;
            local_packet.bullets[i].vy = bullets[i].vy;
            local_packet.bullets[i].active = bullets[i].active;
        }
        network_send_player_packet(&local_packet);

        if (is_host) {
            if (network_recv_player_packet(&remote_packet)) {
                remote_player.x = remote_packet.x;
                remote_player.y = remote_packet.y;
                remote_player.angle = remote_packet.angle;
                remote_player.health = remote_packet.health;
                remote_player.ammo = remote_packet.ammo;
                for (int i = 0; i < MAX_BULLETS; i++) {
                    remote_bullets[i].x = remote_packet.bullets[i].x;
                    remote_bullets[i].y = remote_packet.bullets[i].y;
                    remote_bullets[i].vx = remote_packet.bullets[i].vx;
                    remote_bullets[i].vy = remote_packet.bullets[i].vy;
                    remote_bullets[i].active = remote_packet.bullets[i].active;
                }
            }
        }
        player_update(&player, key_state, delta_time);

        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer); // clear screen

        tilemap_render(renderer, camera_x, camera_y);
        player_render(renderer, &player, camera_x, camera_y);
        player_render(renderer, &remote_player, camera_x, camera_y);
        bullet_render_all(renderer, bullets, camera_x, camera_y);
        bullet_render_all(renderer, remote_bullets, camera_x, camera_y);
        hud_render(renderer, &player);

        SDL_RenderPresent(renderer);
    }

    tilemap_unload();
    network_shutdown();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
