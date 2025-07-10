#include "hud.h"
#include "utils.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

static SDL_Texture *bullet_tex = NULL;
static SDL_Texture *health_tex = NULL;
static TTF_Font *font = NULL;

static bool blood_screen = false;
static float blood_start = 0.0f;
const float BLOOD_DURATION = 300.0f;
float blood_time = 0.0f;

void hud_init(SDL_Renderer *renderer) {
    if (TTF_Init() != 0) {
        SDL_Log("TTF_Init error: %s", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    bullet_tex = IMG_LoadTexture(renderer, "assets/hud/ammo.png");
    health_tex = IMG_LoadTexture(renderer, "assets/hud/health.png");
    font = TTF_OpenFont("assets/fonts/VT323/VT323-Regular.ttf", 20);

    if (!bullet_tex || !health_tex || !font) {
        SDL_Log("HUD init error: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

void hud_destroy() {
    SDL_DestroyTexture(bullet_tex);
    SDL_DestroyTexture(health_tex);
    TTF_CloseFont(font);
    TTF_Quit();
}

void hud_render(SDL_Renderer *renderer, Player *player) {
    SDL_Rect bullet_rect = {650, 5, 16, 32};
    SDL_Rect health_rect = {70, 5, 32, 32};

    SDL_RenderCopy(renderer, bullet_tex, NULL, &bullet_rect);
    SDL_RenderCopy(renderer, health_tex, NULL, &health_rect);

    char ammo_text[16];
    snprintf(ammo_text, sizeof(ammo_text), "%d", player->ammo);
    SDL_Color white = {255, 255, 255, 255};

    SDL_Surface *ammo_surface = TTF_RenderText_Blended(font, ammo_text, white);
    SDL_Texture *ammo_texture = SDL_CreateTextureFromSurface(renderer, ammo_surface);
    SDL_Rect ammo_rect = {bullet_rect.x + 36, bullet_rect.y + 6, ammo_surface->w, ammo_surface->h};
    SDL_RenderCopy(renderer, ammo_texture, NULL, &ammo_rect);
    SDL_FreeSurface(ammo_surface);
    SDL_DestroyTexture(ammo_texture);

    char hp_text[16];
    snprintf(hp_text, sizeof(hp_text), "%d", player->health);
    SDL_Surface *health_surface = TTF_RenderText_Blended(font, hp_text, white);
    SDL_Texture *health_texture = SDL_CreateTextureFromSurface(renderer, health_surface);
    SDL_Rect hp_rect = {health_rect.x + 36, health_rect.y + 6, health_surface->w, health_surface->h};
    SDL_RenderCopy(renderer, health_texture, NULL, &hp_rect);
    SDL_FreeSurface(health_surface);
    SDL_DestroyTexture(health_texture);

    if (blood_screen) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        float alpha = 100 - 100 * blood_time / BLOOD_DURATION;
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, alpha);
        SDL_Rect blood_rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &blood_rect);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    }
}

void hud_set_blood_screen(bool blood) {
    if (blood) {
        blood_screen = true;
        blood_start = SDL_GetTicks();
    } else {
        blood_time = SDL_GetTicks() - blood_start;
        if (blood_time >= BLOOD_DURATION) {
            blood_screen = false;
        }
    }
}