#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include "helpers.h"
#include "structuri.h"

using namespace std;

void usage(char *file)
{
    fprintf(stderr, "Usage: %s server_port\n", file);
    exit(0);
}

int max(int a, int b)
{
    if (a > b)
    {
        return a;
    }
    return b;
}

int main(int argc, char *argv[])
{
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);
    int socketul, n, ret, r;
    struct sockaddr_in serv_addr;
    char id[11];
    packet m;
    uint8_t msg_type;

    if (argc < 3)
    {
        usage(argv[0]);
    }
    //strcpy(id, argv[1]);

    socketul = socket(AF_INET, SOCK_DGRAM, 0);
    DIE(socketul < 0, "socket");

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    ret = inet_aton(argv[1], &addr.sin_addr);
    DIE(ret == 0, "inet_aton");

    ret = connect(socketul, (struct sockaddr *)&addr, sizeof(addr));
    DIE(ret < 0, "connect");

    fd_set read_fds;
    fd_set tmp_fds;
    int fdmax;

    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);
    FD_SET(0, &read_fds);
    FD_SET(socketul, &read_fds);
    fdmax = socketul;

    while (1)
    {
        tmp_fds = read_fds;

        //selectez socket-ul pe care s-au primit informatii
        ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
        DIE(ret < 0, "select");

        if (FD_ISSET(0, &tmp_fds))
        {
            char buffer[1025];
            memset(buffer, 0, 1025);
            fgets(buffer, 1024, stdin);

            if (strncmp(buffer, "quit", 4) == 0)
            {
                break;
            }
            if (strncmp(buffer, "add_address", 11) == 0)
            {
                int s = sendto(socketul, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, sizeof(addr));
                DIE(s < 0, "Eroare de trimitere");
            }
            if (strncmp(buffer, "add_name", 8) == 0)
            {
                int s = sendto(socketul, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, sizeof(addr));
                DIE(s < 0, "Eroare de trimitere");
            }
            if (strncmp(buffer, "add_mail", 8) == 0)
            {
                int s = sendto(socketul, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, sizeof(addr));
                DIE(s < 0, "Eroare de trimitere");
            }
            if (strncmp(buffer, "get_address", 10) == 0)
            {
                int s = sendto(socketul, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, sizeof(addr));
                DIE(s < 0, "Eroare de trimitere");
            }
            if (strncmp(buffer, "get_name", 8) == 0)
            {
                int s = sendto(socketul, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, sizeof(addr));
                DIE(s < 0, "Eroare de trimitere");
            }
            if (strncmp(buffer, "get_mail", 8) == 0)
            {
                int s = sendto(socketul, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, sizeof(addr));
                DIE(s < 0, "Eroare de trimitere");
            }
        }
        if (FD_ISSET(socketul, &tmp_fds))
        {
            //printf("Am primit ceva de la server\n");
            char informatii_udp[1025];

            struct sockaddr_in cli_addr;
            memset(&cli_addr, 0, sizeof(sockaddr_in));
            socklen_t socklen = sizeof(cli_addr);
            memset(informatii_udp, 0, 1025);

            //primesc un mesaj de tip UDP
            int r = recvfrom(socketul, informatii_udp, 1025, 0, (struct sockaddr *)&cli_addr, &socklen);
            printf("%s\n", informatii_udp);
        }
    }

    close(socketul);
    close(0);
    return 0;
}