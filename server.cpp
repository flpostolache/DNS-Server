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

//functie care determina maximul dintre 2 numere
int max(int a, int b)
{
    if (a > b)
    {
        return a;
    }
    return b;
}

//functie care transforma un vector de caractere intr-un string
string convertToString(char *a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size && a[i] != '\0' && a[i] != '\n'; i++)
    {
        s = s + a[i];
    }
    return s;
}

int main(int argc, char *argv[])
{
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);
    int portno, socket_udp;

    struct sockaddr_in serv_addr;
    int n, i, ret, este_inca_deschis = 1, r;
    int bytes_trimisi, bytes_ramas_de_trimis, bytes_primiti, bytes_ramas_de_primit;

    
    unordered_map<string, vector<string>> map_host_ip;
    unordered_map<string, string> map_ip_host;
    unordered_map<string, vector<string>> map_host_mail;

    packet m;
    uint8_t msg_type;

    fd_set read_total_fds;
    fd_set tmp_total_fds;
    int fdmax_total;

    if (argc < 2)
    {
        usage(argv[0]);
    }

    FD_ZERO(&read_total_fds);
    FD_ZERO(&tmp_total_fds);

    //creez socket-ul pe care se vor astepta mesaje de tip UDP
    socket_udp = socket(AF_INET, SOCK_DGRAM, 0);
    DIE(socket_udp < 0, "socket_udp");

    //convertesc portul primit ca parametru
    portno = atoi(argv[1]);
    DIE(portno == 0, "atoi");

    //completez datele serverului
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    //leg datele de identificare a serverului de socket-ul pe care se vor primi informatii UDP
    ret = bind(socket_udp, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr));
    DIE(ret < 0, "bind_udp");

    //adaug socket-ul in lista de socketi pe care pot primi informatii
    FD_SET(socket_udp, &read_total_fds);
    //adaug si stdin-ul pentru a putea citi date de la tastatura
    FD_SET(0, &read_total_fds);
    fdmax_total = socket_udp;

    printf("S-a deschis serverul UDP\n");
    while (este_inca_deschis)
    {
        tmp_total_fds = read_total_fds;

        //selectez socket-ul pe care s-au primit informatii
        ret = select(fdmax_total + 1, &tmp_total_fds, NULL, NULL, NULL);
        DIE(ret < 0, "select");

        for (i = 0; i <= fdmax_total; i++)
        {
            if (FD_ISSET(i, &tmp_total_fds))
            {
                if (i == 0)
                {
                    char buffer[100];
                    memset(buffer, 0, 100);
                    fgets(buffer, 99, stdin);
                    //daca este orice altceva in afara de exit, ignor ceea ce primesc de la tastatura
                    if (strncmp(buffer, "quit", 4) == 0)
                    {
                        //serverul se va inchide
                        este_inca_deschis = 0;
                        //inchid toti socketii din read_total_fds si clientii vor primi automat un mesaj cu 0 biti
                        //ceea ce ii va determina sa se inchida
                        for (int j = 0; j <= fdmax_total; j++)
                        {
                            if (FD_ISSET(j, &read_total_fds) && j != 0 && j != socket_udp)
                            {
                                FD_CLR(j, &read_total_fds);
                                close(j);
                            }
                        }
                        break;
                    }
                    if(strncmp(buffer, "info", 4) == 0){
                        //afisez toate informatiile din sistem
                        printf("Trebuie sa afisez informatii\n");
                    }
                }
                else if (i == socket_udp)
                {
                    //printf("Am primit un mesaj UDP\n");
                    char informatii_udp[1025];
                    
                    struct sockaddr_in cli_addr;
                    memset(&cli_addr, 0, sizeof(sockaddr_in));
                    socklen_t socklen = sizeof(cli_addr);
                    memset(informatii_udp, 0, 1025);

                    //primesc un mesaj de tip UDP
                    int r = recvfrom(socket_udp, informatii_udp, 1025, 0, (struct sockaddr *)&cli_addr, &socklen);
                    if (r < 0)
                    {
                        printf("Am primit un mesaj UDP\n");
                    }
                    if (strncmp(informatii_udp, "add_address", 11) == 0)
                    {
                        char *aux;
                        aux = strtok(informatii_udp, " ");
                        aux = strtok(NULL," ");
                        string hostmane = convertToString(aux,5000);
                        //cout << hostmane << endl;
                        aux = strtok(NULL, " ");
                        string ip = convertToString(aux, 50000);
                        //cout<< ip << endl;
                        if(map_host_ip.find(hostmane) == map_host_ip.end()){
                            vector<string> tmp;
                            tmp.push_back(ip);
                            map_host_ip.insert(make_pair(hostmane, tmp));
                        }
                        else{
                            map_host_ip[hostmane].push_back(ip);
                        }
                        /*unordered_map<string,vector<string>> ::iterator itr;
                        for (itr = map_host_ip.begin(); itr != map_host_ip.end(); ++itr) {
                            //cout << '\t' << itr->first<< '\t' << itr->second << '\n';
                            cout<< itr->first << ":";
                            for(auto k = itr->second.begin(); k != itr->second.end();++k)
                                cout << *k <<" ";
                            cout<< "\n";
                        }*/
                    }
                    if(strncmp(informatii_udp, "add_name", 8) == 0){
                        char *aux;
                        aux = strtok(informatii_udp, " ");
                        aux = strtok(NULL," ");
                        string hostmane = convertToString(aux,5000000);
                        //cout << hostmane << endl;
                        aux = strtok(NULL, " ");
                        string ip = convertToString(aux, 500000);
                        //cout<< ip << endl;
                        if(map_ip_host.find(hostmane) == map_ip_host.end()){
                    
                            map_ip_host.insert(make_pair(hostmane, ip));
                        }
                        else{
                            //map_host_ip[hostmane].push_back(ip);
                            map_ip_host.erase(hostmane);
                            map_ip_host.insert(make_pair(hostmane,ip));
                        }
                        /*unordered_map<string,string> ::iterator itr;
                        for (itr = map_ip_host.begin(); itr != map_ip_host.end(); ++itr) {
                            //cout << '\t' << itr->first<< '\t' << itr->second << '\n';
                            cout<< itr->first << " : " << itr->second;
                        
                        }*/

                    }
                    if (strncmp(informatii_udp, "add_mail", 8) == 0)
                    {
                        char *aux;
                        aux = strtok(informatii_udp, " ");
                        aux = strtok(NULL," ");
                        string hostmane = convertToString(aux,5000);
                        //cout << hostmane << endl;
                        aux = strtok(NULL, " ");
                        string mail = convertToString(aux, 50000);
                        //cout<< ip << endl;
                        if(map_host_mail.find(hostmane) == map_host_mail.end()){
                            vector<string> tmp;
                            tmp.push_back(mail);
                            map_host_mail.insert(make_pair(hostmane, tmp));
                        }
                        else{
                            map_host_mail[hostmane].push_back(mail);
                        }
                        unordered_map<string,vector<string>> ::iterator itr;
                        for (itr = map_host_mail.begin(); itr != map_host_mail.end(); ++itr) {
                            //cout << '\t' << itr->first<< '\t' << itr->second << '\n';
                            cout<< itr->first << ":";
                            for(auto k = itr->second.begin(); k != itr->second.end();++k)
                                cout << *k <<" ";
                            cout<< "\n";
                        }
                    }
                    if (strncmp(informatii_udp, "get_address", 11) == 0)
                    {
                        char mesaj_de_trimisi[1025];
                        memset(mesaj_de_trimisi, 0, 1025);
                        char *aux;
                        aux = strtok(informatii_udp, " ");
                        aux = strtok(NULL," ");
                        string hostmane = convertToString(aux,5000);
                        //cout<< hostmane;
                        if(map_host_ip.find(hostmane) == map_host_ip.end()){
                            strcat(mesaj_de_trimisi,"Nu avem nici un ip cu denumirea aceasta");
                            int s = sendto(socket_udp, mesaj_de_trimisi, sizeof(mesaj_de_trimisi), 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
                            DIE(s < 0, "Eroare de trimitere");
                        }
                        else{
                            for(auto k = map_host_ip[hostmane].begin(); k != map_host_ip[hostmane].end(); k++){
                                strcat(mesaj_de_trimisi, (*k).c_str());
                                strcat(mesaj_de_trimisi," ");
                            }
                            //printf("%s\n", mesaj_de_trimisi);
                            int s = sendto(socket_udp, mesaj_de_trimisi, sizeof(mesaj_de_trimisi), 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
                            DIE(s < 0, "Eroare de trimitere");
                        }
                    }
                    if (strncmp(informatii_udp, "get_name", 8) == 0)
                    {
                        char mesaj_de_trimisi[1025];
                        memset(mesaj_de_trimisi, 0, 1025);
                        char *aux;
                        aux = strtok(informatii_udp, " ");
                        aux = strtok(NULL," ");
                        string ip = convertToString(aux,5000);
                        //cout<< hostmane;
                        if(map_ip_host.find(ip) == map_ip_host.end()){
                            strcat(mesaj_de_trimisi,"Nu avem nici un ip cu denumirea aceasta");
                            int s = sendto(socket_udp, mesaj_de_trimisi, sizeof(mesaj_de_trimisi), 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
                            DIE(s < 0, "Eroare de trimitere");
                        }
                        else{
                            //for(auto k = map_ip_host[ip].begin(); k != map_host_ip[hostmane].end(); k++){
                                strcat(mesaj_de_trimisi, map_ip_host[ip].c_str());
                                //strcat(mesaj_de_trimisi," ");
                            //printf("%s\n", mesaj_de_trimisi);
                            int s = sendto(socket_udp, mesaj_de_trimisi, sizeof(mesaj_de_trimisi), 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
                            DIE(s < 0, "Eroare de trimitere");
                        }
                    }
                    if (strncmp(informatii_udp, "get_mail", 8) == 0)
                    {
                        char mesaj_de_trimisi[1025];
                        memset(mesaj_de_trimisi, 0, 1025);
                        char *aux;
                        aux = strtok(informatii_udp, " ");
                        aux = strtok(NULL," ");
                        string hostmane = convertToString(aux,5000);
                        //cout<< hostmane;
                        if(map_host_mail.find(hostmane) == map_host_mail.end()){
                            strcat(mesaj_de_trimisi,"Nu avem nici un ip cu denumirea aceasta");
                            int s = sendto(socket_udp, mesaj_de_trimisi, sizeof(mesaj_de_trimisi), 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
                            DIE(s < 0, "Eroare de trimitere");
                        }
                        else{
                            for(auto k = map_host_mail[hostmane].begin(); k != map_host_mail[hostmane].end(); k++){
                                strcat(mesaj_de_trimisi, (*k).c_str());
                                strcat(mesaj_de_trimisi," ");
                            }
                            //printf("%s\n", mesaj_de_trimisi);
                            int s = sendto(socket_udp, mesaj_de_trimisi, sizeof(mesaj_de_trimisi), 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
                            DIE(s < 0, "Eroare de trimitere");
                        }
                    }

                }
            }
        }
    }

    close(socket_udp);
    close(0);
    return 0;
}