#ifndef NETWORK_H
#define NETWORK_H

#include "bullet.h"

#include <stdbool.h>

void set_connection(bool *is_host, int *port, char **ip, char **argv, int argc);
bool network_init(bool is_host, const char *ip, int port);
void network_shutdown();

typedef struct {
    float x, y;
    float angle;
    int health;
    int ammo;
    struct {
        float x, y;
        float vx, vy;
        bool active;
    } bullets[MAX_BULLETS];
    /*struct {
        bool active;
    } opponent_bullets_update[MAX_BULLETS];*/
} PlayerPacket;

bool network_send_player_packet(PlayerPacket *packet);
bool network_recv_player_packet(PlayerPacket *packet);

#endif //NETWORK_H
