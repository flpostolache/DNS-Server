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

using namespace std;

typedef struct {
    char payload[1600];
} packet;

struct mesaj_cu_int{
    uint8_t semn;
    uint32_t numar;
} __attribute__((packed));

struct informatii{
    uint32_t adresa_ip;
    uint16_t port;
    char topic[50];
    uint8_t tip_date;
} __attribute__((packed));

struct mesaj_cu_short_real{
    uint16_t numar;
} __attribute__((packed));

struct mesaj_cu_float{
    uint8_t semn;
    uint32_t numar;
    uint8_t putere;
} __attribute__((packed));

struct mesaj_cu_string{
    char text[1501];
} __attribute__((packed));

typedef struct client{
    int socket_pe_care_este_conectat;
    vector<string> abonamente_SF;
    vector<packet> pachete_in_asteptare;
}client;