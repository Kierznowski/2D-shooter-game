#include <SDL2/SDL.h>
#include <stdbool.h>
#include "player.h"
#include "bullet.h"
#include "tilemap.h"
#include "hud.h"
#include "network.h"
#include "light_beam.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 640

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
Player player;
Player opponent;
Bullet bullets[MAX_BULLETS] = {0};
Bullet remote_bullets[MAX_BULLETS] = {0};
PlayerPacket local_packet = {0};
PlayerPacket remote_packet = {0};
bool is_host = false;
float camera_x = 0;
float camera_y = 0;

void set_SDL();
void set_players();
void update_opponent_state();
void prepare_player_state();
void render();

int main(int argc, char *argv[]) {

    if (argc < 3) {
        printf("Usage: '%s host <port>' '%s client <ip> <port>'\n", argv[0], argv[0]);
        return 1;
    }

    bool running = true;
    const char *ip = NULL;
    int port = 0;

    set_connection(&is_host, &port, &ip, argv, argc);
    set_SDL();
    set_players();
    tilemap_load(renderer);
    light_beam_init(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);


    Uint32 last_time = SDL_GetTicks();
    while (running) {
        // If you are client receive host state first
        if (!is_host && network_recv_player_packet(&remote_packet)) {
            update_opponent_state();
        }

        // Input
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        const Uint8 *key_state = SDL_GetKeyboardState(NULL);

        // Update
        const Uint32 current_time = SDL_GetTicks();
        const float delta_time = (float)(current_time - last_time) / 1000.0f;
        last_time = current_time;
        player_update(&player, key_state, delta_time);
        bullet_update_all(bullets, key_state, delta_time, player.x, player.y, player.angle, &player.ammo);
        camera_x = player.x - SCREEN_WIDTH / 2.0;
        camera_y = player.y - SCREEN_HEIGHT / 2.0;

        // Send state
        prepare_player_state();
        network_send_player_packet(&local_packet);

        // If you are host receive opponent state later
        if (is_host && network_recv_player_packet(&remote_packet)) {
            update_opponent_state();
        }

        // Render
        render();
    }

    tilemap_unload();
    light_beam_destroy();
    network_shutdown();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void set_SDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    window = SDL_CreateWindow("Shooter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
}

void set_players() {
    player_init(&player, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 100);
    player_init(&opponent, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 150);
}

void update_opponent_state() {
    opponent.x = remote_packet.x;
    opponent.y = remote_packet.y;
    opponent.angle = remote_packet.angle;
    opponent.health = remote_packet.health;
    opponent.ammo = remote_packet.ammo;
    for (int i = 0; i < MAX_BULLETS; i++) {
        remote_bullets[i].x = remote_packet.bullets[i].x;
        remote_bullets[i].y = remote_packet.bullets[i].y;
        remote_bullets[i].vx = remote_packet.bullets[i].vx;
        remote_bullets[i].vy = remote_packet.bullets[i].vy;
        remote_bullets[i].active = remote_packet.bullets[i].active;
    }
}

void prepare_player_state() {
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
}

void render() {
    // clr screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    tilemap_render(renderer, camera_x, camera_y);
    player_render(renderer, &player, camera_x, camera_y, false);
    player_render(renderer, &opponent, camera_x, camera_y, true);
    bullet_render_all(renderer, bullets, camera_x, camera_y);
    bullet_render_all(renderer, remote_bullets, camera_x, camera_y);
    light_beam_render(renderer, player.angle);
    hud_render(renderer, &player);

    SDL_RenderPresent(renderer);
}