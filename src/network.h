#ifndef NETWORK_H
#define NETWORK_H

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h> // socket() etc...
#include <arpa/inet.h> // sockaddr_in struct
#include <unistd.h> // close()
#endif

#include "bullet.h"
#include <stdbool.h>

bool network_init(bool is_host, char *ip, int port);
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
} PlayerPacket;

bool network_send_player_packet(PlayerPacket *packet);
bool network_recv_player_packet(PlayerPacket *packet);

#endif //NETWORK_H
