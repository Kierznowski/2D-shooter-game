#include "network.h"

#include <stdio.h>

#ifdef _WIN32
static WSADATA wsa_data;
#endif

static int sock_fd = -1;

bool network_init(bool is_host, char *ip, int port) {
#ifdef _WIN32
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        return false;
    }
#endif

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket failed");
        return false;
    }

    if (is_host) {
        // Server code
        struct sockaddr_in serv_addr = {
            .sin_family = AF_INET,
            .sin_addr.s_addr = INADDR_ANY,
            .sin_port = htons(port),
        };

        if (bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("bind failed");
            return false;
        }
        if (listen(sock_fd, 1) < 0) {
            perror("listen failed");
            return false;
        }

        printf("Host listening for connection on port %d...\n", port);
        int client_fd = accept(sock_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept failed");
            return false;
        }

        close(sock_fd);
        sock_fd = client_fd;
        printf("Client connected to the host!\n");


    } else {
        // client code
        struct sockaddr_in serv_addr = {
            .sin_family = AF_INET,
            .sin_port = htons(port),
        };

        if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
            perror("inet_pton failed");
            return false;
        }

        if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("connect failed");
            return false;
        }

        printf("Client connected to the server at %s:%d!\n", ip, port);
    }
    return true;
}

void network_shutdown() {
#ifdef _WIN32
    WSACleanup();
    closesocket(sock_fd);
#else
    if (sock_fd >= 0) {
        close(sock_fd);
        sock_fd = -1;
    }
#endif
}

bool network_send_all(int sock, const void *buffer, size_t length) {
    size_t total_sent = 0;
    const char *buf = (const char *)buffer;

    while (total_sent < length) {
        ssize_t sent = send(sock, buf + total_sent, length - total_sent, 0);
        if (sent <= 0) {
            perror("send failed");
            return false;
        }
        total_sent += sent;
    }
    return true;
}

bool network_recv_all(int sock, void *buffer, size_t length) {
    size_t total_received = 0;
    char *buf = (char *)buffer;

    while (total_received < length) {
        ssize_t received = recv(sock, buf + total_received, length - total_received, 0);
        if (received <= 0) {
            if (received == 0) {
                printf("Connection closed by peer\n");
            } else {
                perror("recv failed");
            }
            return false;
        }
        total_received += received;
    }
    return true;
}

bool network_send_player_packet(PlayerPacket *packet) {
    return network_send_all(sock_fd, packet, sizeof(PlayerPacket));
}

bool network_recv_player_packet(PlayerPacket *packet) {
    return network_recv_all(sock_fd, packet, sizeof(PlayerPacket));
}
