//
//  server.c
//  teststrcmp
//
//  Created by cai on 2017/12/3.
//  Copyright © 2017年 cai. All rights reserved.
//
// tag  服务器实现监听功能

#include "server.h"



redisServer server;

int main (int argc, char *argv[]) {
    server.port = DEFAULT_PORT;
    server.tcp_backlog = TCP_BACKLOG;
    server.maxclients = CONFIG_BINDADDR_MAX;
    server.tcpkeepalive = CONFIG_DEFAULT_TCP_KEEPALIVE;
    server.el = aeCreateEventLoop(server.maxclients+CONFIG_FDSET_INCR);
    server.clients = listCreate();
    server.db = createDb();
    server.aof_buffer = sdsempty();

    server.listenfd = anetTcpServer(server.port, server.tcp_backlog);
    if (server.listenfd < 0) {
        return -1;
    }
    if (aeCreateTimeEvent(server.el, 100000000) == AE_ERR) return -1;
    
    if (aeCreateFileEvent(server.el, server.listenfd, AE_READABLE, acceptTcpHandler, NULL) == AE_ERR) {
        return -1;
    }
    
    aeMain(server.el);
    
    
}

