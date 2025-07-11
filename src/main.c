#define SDL_MAIN_HANDLED
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#include "utils.h"
#include "player.h"
#include "bullet.h"
#include "tilemap.h"
#include "hud.h"
#include "network.h"
#include "light_beam.h"
#include "menu.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
Player player;
Player opponent;
Bullet bullets[MAX_BULLETS] = {0};
Bullet remote_bullets[MAX_BULLETS] = {0};
PlayerPacket local_packet = {0};
PlayerPacket remote_packet = {0};
bool running = true;
float camera_x = 0;
float camera_y = 0;
Uint32 last_time = 0;
char *ip = NULL;
int port = 0;
Menu_Config menu_config = {0};

void init();
void loop();
void clean();
void set_SDL();
void set_players();
bool check_game_over(Player *player, Player *opponent);
void render();
void reset_state();
void update_state(const Uint8 *key_state);

int main() {
    init();
    if (!menu_run(renderer, &menu_config)) {
        SDL_Log("Menu Error: %s", SDL_GetError());
        return 1;
    };
    last_time = SDL_GetTicks();
    loop();
    clean();
    return 0;
}

void init() {
    set_SDL();
    set_players();
    tilemap_load(renderer);
    light_beam_init(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    hud_init(renderer);
}

void loop() {
    while (running) {
        // If you are client, receive host state
        if (!menu_config.is_host && network_recv_player_packet(&remote_packet)) {
            player_update_state(&opponent, remote_bullets, &remote_packet);
        }

        // Input
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        const Uint8 *key_state = SDL_GetKeyboardState(NULL);

        //update
        update_state(key_state);

        // Send state
        player_prepare_state(&player, bullets, &local_packet);
        network_send_player_packet(&local_packet);

        // If you are host, receive opponent state
        if (menu_config.is_host && network_recv_player_packet(&remote_packet)) {
            player_update_state(&opponent, remote_bullets, &remote_packet);
        }

        if (check_game_over(&player, &opponent)) {
            menu_display_game_over_menu(renderer, player.health);
            if (menu_display_game_over_menu(renderer, player.health)) {
                reset_state();
                printf("health: %d %d, ammo: %d %d", player.health, opponent.health, player.ammo, opponent.ammo);
            } else {
                return;
            }
        }
        // Render
        render();
    }
}

void set_SDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("Failed to init SDL_image: %s", IMG_GetError());
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

void clean() {
    tilemap_unload();
    light_beam_destroy();
    player_destroy_texture();
    hud_destroy();
    network_shutdown();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

void set_players() {
    player_init(&player, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 100);
    player_init(&opponent, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 150);
    player_set_textures(renderer, true);
    player_set_textures(renderer, false);
}

void update_state(const Uint8 *key_state) {
    const Uint32 current_time = SDL_GetTicks();
    const float delta_time = (float)(current_time - last_time) / 1000.0f;
    last_time = current_time;
    player_update(&player, key_state, delta_time, remote_bullets);
    player_check_collision_with_bullets(&opponent, bullets);
    hud_set_blood_screen(player_check_collision_with_bullets(&player, remote_bullets));
    bullet_update_all(bullets, key_state, delta_time, player.x, player.y, player.angle, &player.ammo);
    camera_x = player.x - SCREEN_WIDTH / 2.0;
    camera_y = player.y - SCREEN_HEIGHT / 2.0;
}

bool check_game_over(Player *player, Player *opponent) {
    if (player->health == 0 || opponent->health == 0) {
        if (player->health > 0) {
            printf("You won!");
        } else {
            printf("You loose!");
        }
        return true;
    }
    return false;
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

void reset_state() {
    player.health = 100;
    player.ammo = 100;
    player.x = SCREEN_WIDTH / 2;
    player.y = SCREEN_HEIGHT / 2 + 150;
}
