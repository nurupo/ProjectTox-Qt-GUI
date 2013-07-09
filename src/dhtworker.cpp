/*
    Copyright (C) 2013 by Maxim Biro <nurupo.contributions@gmail.com>
    
    This file is part of NFK Lobby.
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    
    See the COPYING file for more details.
*/

#include "dhtworker.hpp"
extern "C" {
#include "core/DHT.h"
}

#include <QThread>
#include <QDebug>

DhtWorker::DhtWorker()
{
}

DhtWorker::~DhtWorker()
{
#ifdef WIN32
    WSACleanup();
#endif
}

bool DhtWorker::init(int bindPort)
{
    srand(time(NULL));
    int randdomnum = rand();
    memcpy(self_client_id, &randdomnum, 4);

#ifdef WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != NO_ERROR) {
        return false;
    }
#endif

    //addfriend(self_client_id);

    //initialize our socket
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    //Set socket nonblocking
#ifdef WIN32
    //I think this works for windows
    u_long mode = 1;
    //ioctl(sock, FIONBIO, &mode);
    ioctlsocket(sock, FIONBIO, &mode);
#else
    fcntl(sock, F_SETFL, O_NONBLOCK, 1);
#endif

    //Bind our socket to port PORT and address 0.0.0.0
    ADDR addr = {AF_INET, htons(33445), {{0}}};
    perror("Initialization");
    bind(sock, (struct sockaddr*)&addr, sizeof(addr));

    IP_Port bootstrap_ip_port;
    bootstrap_ip_port.port = htons(bindPort);
    bootstrap_ip_port.ip.i = inet_addr("127.0.0.1");
    bootstrap(bootstrap_ip_port);

    return true;
}

void DhtWorker::run()
{
    IP_Port ip_port;
    char data[MAX_UDP_PACKET_SIZE];
    uint32_t length;
    uint32_t i;

    while (1) {
        doDHT();
qDebug() << QString(self_client_id);
        while (recievepacket(&ip_port, data, &length) != -1) {
            if (DHT_recvpacket(data, length, ip_port)) {
                printf("UNHANDLED PACKET RECEIVED\nLENGTH:%u\nCONTENTS:\n", length);
                printf("--------------------BEGIN-----------------------------\n");
                for(i = 0; i < length; i++)
                {
                    if(data[i] < 16)
                        printf("0");
                    printf("%X",data[i]);
                }
                printf("\n--------------------END-----------------------------\n\n\n");
            } else {
                printf("Received handled packet with length: %u\n", length);
            }
        }
        //print_clientlist();
        //print_friendlist();
        QThread::msleep(300);
    }
}
