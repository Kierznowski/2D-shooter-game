#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
    bool is_host;
    int port;
    char ip[32];
} Menu_Config;

bool menu_run(SDL_Renderer *renderer, Menu_Config *menu_config);
bool menu_display_game_over_menu(SDL_Renderer *renderer, int player_health);

#endif //MENU_H
