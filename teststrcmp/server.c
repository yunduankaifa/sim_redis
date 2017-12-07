//
//  server.c
//  teststrcmp
//
//  Created by cai on 2017/12/3.
//  Copyright © 2017年 cai. All rights reserved.
//

#include "server.h"



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

