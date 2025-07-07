#ifndef MENU_H
#define MENU_H

#include <SDL_render.h>
#include <stdbool.h>

typedef struct {
    bool is_host;
    int port;
    char ip[32];
} Menu_Config;

bool menu_run(SDL_Renderer *renderer, Menu_Config *menu_config);

#endif //MENU_H
