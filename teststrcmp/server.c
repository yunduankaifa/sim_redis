//
//  server.c
//  teststrcmp
//
//  Created by cai on 2017/12/3.
//  Copyright © 2017年 cai. All rights reserved.
//

#include "server.h"
#include "networking.c"




#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include "ae.h"
#include "cnet.h"


#define MAX_ACCEPTS_PER_CALL 1000

void acceptTcpHandler(aeEventLoop *el, int fd, void *clientData, int mask) {
    int cport, cfd=-1, max = MAX_ACCEPTS_PER_CALL;
    char cip[NET_IP_STR_LEN];
    
    while(max--) {
        cfd = anetTcpAccept(server.neterr, fd, cip, sizeof(cip), &cport);
        if (cfd == ANET_ERR) {
            if (errno != EWOULDBLOCK)
            /*          serverLog(LL_WARNING,
             "Accepting client connection: %s", server.neterr);
             */
                return;
        }
        printf("conn succed!\n");
        //  serverLog(LL_VERBOSE,"Accepted %s:%d", cip, cport);
        //      acceptCommonHandler(cfd,0,cip);
    }
    
}







struct redisServer server;

int main (int argc, char *argv[]) {
    server.port = DEFAULT_PORT;
    server.tcp_backlog = TCP_BACKLOG;
    server.maxclients = CONFIG_BINDADDR_MAX;
    server.el = aeCreateEventLoop(server.maxclients+CONFIG_FDSET_INCR);

    server.listenfd = anetTcpServer(server.port, server.tcp_backlog);
    if (server.listenfd < 0) {
        return -1;
    }
    
    if (aeCreateFileEvent(server.el, server.listenfd, AE_READABLE, acceptTcpHandler, NULL) == AE_ERR) {
        return -1;
    }
    
    aeMain(server.el);
    
    
}

