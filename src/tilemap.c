#include "tilemap.h"
#include <stdio.h>

static int **map = NULL;
static int map_width = 0;
static int map_height = 0;
static SDL_Texture *wall_texture = NULL;
static SDL_Texture *floor_texture = NULL;

static SDL_Texture *load_texture(SDL_Renderer *renderer, const char *path) {
    SDL_Surface *surface = SDL_LoadBMP(path);
    if (!surface) {
        printf("Unable to load image %s! SDL Error: %s\n", path, SDL_GetError());
        return NULL;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
    }
    return texture;
};

void get_map_size(const char *filename, int *width, int *height) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Unable to load map from file %s! SDL Error: %s\n", filename, SDL_GetError());
        *width = *height = 0;
        return;
    }

    int w = 0, h = 0;
    int current_line_length = 0;
    int c;

    while ((c = fgetc(f)) != EOF) {
        if (c == '\n') {
            if (current_line_length > w) {
                w = current_line_length;
            }
            current_line_length = 0;
            h++;
        } else {
            current_line_length++;
        }
    }

    if (current_line_length > 0) {
        if (current_line_length > w) {
            w = current_line_length;
        }
        h++;
    }
    fclose(f);
    *width = w;
    *height = h;
}

void tilemap_load(SDL_Renderer *renderer) {
    wall_texture = load_texture(renderer, "./assets/textures/wall.bmp");
    floor_texture = load_texture(renderer, "./assets/textures/floor.bmp");

    get_map_size("./assets/map/map.txt", &map_width, &map_height);
    map = malloc(sizeof(int*) * map_height);
    for (int y = 0; y < map_height; y++) {
        map[y] = malloc(sizeof(int) * map_width);
    }

    FILE *f = fopen("./assets/map/map.txt", "r");
    if (!f) {
        printf("Unable to load map.txt in tilemap.c!");
        return;
    }

    for (int y = 0; y < map_height; y++) {
        for (int x = 0; x < map_width; x++) {
            char c = fgetc(f);
            if (c == '\n') {
                c = fgetc(f);
            }
            if (c == '#') {
                map[y][x] = 1;
            } else {
                map[y][x] = 0;
            }
        }
    }
    fclose(f);
}



void tilemap_render(SDL_Renderer *renderer, float camera_x, float camera_y) {
    SDL_Rect dest;
    dest.w = TILE_SIZE;
    dest.h = TILE_SIZE;

    for (int y = 0; y < map_height; y++) {
        for (int x = 0; x < map_width; x++) {
            dest.x = x * TILE_SIZE - camera_x;
            dest.y = y * TILE_SIZE - camera_y;

            if (map[y][x] == 1) {
                SDL_RenderCopy(renderer, wall_texture, NULL, &dest);
            } else {
                SDL_RenderCopy(renderer, floor_texture, NULL, &dest);
            }
        }
    }
}

int tilemap_get_tile(int world_x, int world_y) {
    const int tx = world_x / TILE_SIZE;
    const int ty = world_y / TILE_SIZE;
    if (tx >= 0 && ty >= 0 && tx < map_width && ty < map_height) {
        return map[ty][tx];
    }
    return 0;
}

bool tilemap_is_colliding(int world_x, int world_y) {
    return tilemap_get_tile(world_x, world_y) == 1;
}

void tilemap_unload() {
    if (wall_texture) {
        SDL_DestroyTexture(wall_texture);
    }
    if (floor_texture) {
        SDL_DestroyTexture(floor_texture);
    }
    for (int y = 0; y < map_height; y++) {
        free(map[y]);
    }
    free(map);
    map = NULL;
}