#include "menu.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "network.h"

#define MAX_INPUT 32

bool menu_select_role(SDL_Renderer *renderer, bool *is_host);
void menu_get_user_input(SDL_Renderer *renderer, char *descriptor, char *user_input);
void menu_waiting_for_connection(SDL_Renderer *renderer);

TTF_Font *font;
SDL_Color text_color;

static SDL_Texture *render_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) {
        SDL_Log("TTF_RenderText_Solid error: %s", TTF_GetError());
        return NULL;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
};

bool menu_run(SDL_Renderer *renderer, Menu_Config *config) {
    if (TTF_Init() != 0) {
        SDL_Log("TTF_Init error: %s", TTF_GetError());
        return false;
    }

    font = TTF_OpenFont("assets/fonts/VT323/VT323-Regular.ttf", 24);
    if (!font) {
        SDL_Log("TTF_OpenFont error: %s", TTF_GetError());
        return false;
    }
    text_color = (SDL_Color){255, 255, 255, 255};

    if (!menu_select_role(renderer, &config->is_host)) {
        return false;
    }

    char port_char[MAX_INPUT + 1];
    menu_get_user_input(renderer, "port: ", port_char);
    config->port = atoi(port_char);
    if (!config->is_host) {
        menu_get_user_input(renderer, "IP: ", config->ip);
    }

    menu_waiting_for_connection(renderer);
    network_init(config->is_host, config->ip, config->port);

    TTF_CloseFont(font);
    TTF_Quit();
    return true;
}

bool menu_select_role(SDL_Renderer *renderer, bool *is_host) {
    const char *options[] = {"Host", "Client"};
    int selected = 0;
    bool running = true;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return false;
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        selected = (selected - 1 + 2) % 2;
                        break;
                    case SDLK_DOWN:
                        selected = (selected + 1) % 2;
                        break;
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER:
                        *is_host = (selected == 0);
                        running = false;
                        break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < 2; i++) {
            char buffer[MAX_INPUT];
            snprintf(buffer, sizeof(buffer), "%s %s", (i == selected ? ">": " "), options[i]);

            SDL_Texture *texture = render_text(renderer, font, buffer, text_color);
            if (texture) {
                int texWidth = 0, texHeight = 0;
                SDL_QueryTexture(texture, NULL, NULL, &texWidth, &texHeight);
                SDL_Rect dst_rect = {100, 100 + i * 40, texWidth, texHeight};
                SDL_RenderCopy(renderer, texture, NULL, &dst_rect);
                SDL_DestroyTexture(texture);
            }
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // 60 FPS
    }
    return true;
}

void menu_get_user_input(SDL_Renderer *renderer, char *descriptor, char *user_input) {
    char input_text[MAX_INPUT + 1] = {0};
    bool running = true;
    int input_length = 0;

    SDL_StartTextInput();

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        running = false;
                    } else if (event.key.keysym.sym == SDLK_BACKSPACE && input_length > 0) {
                        input_text[--input_length] = '\0';
                    } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                        input_text[0] = '\0';
                        running = false;
                    }
                    break;
                case SDL_TEXTINPUT:
                    if (input_length < MAX_INPUT - 1) {
                        strncat(input_text, event.text.text, MAX_INPUT - 1);
                        input_length = strlen(input_text);
                    }
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        char display_text[strlen(descriptor) + strlen(input_text) + 3];
        snprintf(display_text, sizeof(display_text), "%s%s", descriptor, input_text);

        SDL_Texture *texture = render_text(renderer, font, display_text, text_color);
        if (texture) {
            int texWidth = 0, texHeight = 0;
            SDL_QueryTexture(texture, NULL, NULL, &texWidth, &texHeight);
            SDL_Rect dst_rect = {50, 50, texWidth, texHeight};
            SDL_RenderCopy(renderer, texture, NULL, &dst_rect);
            SDL_DestroyTexture(texture);
        }
        SDL_RenderPresent(renderer);
    }
    strncpy(user_input, input_text, MAX_INPUT);
    user_input[MAX_INPUT] = '\0';

    SDL_StopTextInput();
}

void menu_waiting_for_connection(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Texture *texture = render_text(renderer, font, "Waiting for connection...", text_color);
    if (!texture) {
        SDL_Log("Error creating waiting for connection texture");
        return;
    }
    int texWidth = 0, texHeight = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texWidth, &texHeight);
    SDL_Rect dst_rect = {50, 50, texWidth, texHeight};
    SDL_RenderCopy(renderer, texture, NULL, &dst_rect);
    SDL_DestroyTexture(texture);
    SDL_RenderPresent(renderer);
}

bool menu_display_game_over_menu(SDL_Renderer *renderer, int player_health) {
    if (TTF_Init() != 0) {
        SDL_Log("TTF_Init error: %s", TTF_GetError());
        return false;
    }
    font = TTF_OpenFont("assets/fonts/VT323/VT323-Regular.ttf", 24);
    if (!font) {
        SDL_Log("TTF_OpenFont error: %s", TTF_GetError());
        return false;
    }
    text_color = (SDL_Color){255, 255, 255, 255};

    const char *options[] = {"Continue", "Exit"};
    int selected = 0;

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case (SDLK_DOWN):
                        selected = (selected + 1) % 2;
                        break;
                    case SDLK_UP:
                        selected = (selected + 1) % 2;
                        break;
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER:
                        running = false;
                        break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < 2; i++) {
            char buffer[MAX_INPUT];
            snprintf(buffer, sizeof(buffer), "%s %s", (i == selected ? ">": " "), options[i]);

            SDL_Texture *texture = render_text(renderer, font, buffer, text_color);
            if (texture) {
                int texWidth = 0, texHeight = 0;
                SDL_QueryTexture(texture, NULL, NULL, &texWidth, &texHeight);
                SDL_Rect dst_rect = {100, 100 + i * 40, texWidth, texHeight};
                SDL_RenderCopy(renderer, texture, NULL, &dst_rect);
                SDL_DestroyTexture(texture);
            }
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    TTF_CloseFont(font);
    TTF_Quit();
    return selected == 0;
}
