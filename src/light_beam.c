#include "light_beam.h"

static SDL_Texture *light_mask = NULL;

int texture_width = 0;
int texture_height = 0;
int screen_width = 0;
int screen_height = 0;

void light_beam_init(SDL_Renderer *renderer, int width, int height) {
    if (light_mask != NULL) {
        return;
    }

    const int diag = (int)ceilf(sqrtf(((float)(width * width) + (float)(height * height))));

    // Blank black texture
    light_mask = SDL_CreateTexture(renderer,
         SDL_PIXELFORMAT_RGBA8888,
         SDL_TEXTUREACCESS_TARGET,
         diag, diag);

    texture_width = diag;
    texture_height = diag;
    screen_width = width;
    screen_height = height;

    SDL_SetTextureBlendMode(light_mask, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, light_mask);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Light cone params
    const int segments = 16;
    const float cone_angle = M_PI / 1.5f; // 120 deg view
    const int cx = diag / 2;
    const int cy = diag / 2;

    // Set of triangles build cone
    SDL_Vertex vertices[segments + 2];
    // Center vertice of the cone
    vertices[0].position.x = cx;
    vertices[0].position.y = cy;
    vertices[0].color = (SDL_Color){0, 0, 0, 0};

    for (int i = 0; i <= segments; i++) {
        const float angle = 0.0f;
        const float radius = 350.0f;
        // Calculating angle of segment.
        const float a = angle - cone_angle / 2.0f + i * (cone_angle / segments);
        const float x = cx + cosf(a) * radius; // x position of the vertice
        const float y = cy + sinf(a) * radius; // y position of the vertice

        // Distance from the center
        const float dx = x - cx;
        const float dy = y - cy;
        const float dist = sqrtf(dx * dx + dy * dy);
        // Gradient
        Uint8 alpha = (Uint8)(250.0f * (dist / radius));
        if (alpha > 250) alpha = 250;

        vertices[i + 1].position.x = x;
        vertices[i + 1].position.y = y;
        vertices[i + 1].color = (SDL_Color){0, 0, 0, alpha};
    }

    for (int i = 1; i < segments + 1; i++) {
        SDL_RenderGeometry(renderer, NULL,
            (SDL_Vertex[]){vertices[0], vertices[i], vertices[i + 1]},
            3, NULL, 0);
    }

    SDL_SetRenderTarget(renderer, NULL);
}


void light_beam_render(SDL_Renderer *renderer, float player_angle) {
    if (!light_mask) {
        return;
    }
    const float angle = (float)(player_angle * (180.0f / M_PI));

    const SDL_FRect dst = {
        (screen_width - texture_width) / 2.0f, (screen_height - texture_height) / 2.0f, (float)texture_width, (float)texture_height
    };

    SDL_RenderCopyExF(renderer, light_mask, NULL, &dst, angle, &(SDL_FPoint){texture_width / 2.0f, texture_height / 2.0f}, SDL_FLIP_NONE);
}

void light_beam_destroy() {
    if (light_mask) {
        SDL_DestroyTexture(light_mask);
        light_mask = NULL;
    }
}
