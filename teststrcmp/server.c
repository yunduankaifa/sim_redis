//
//  server.c
//  teststrcmp
//
//  Created by cai on 2017/12/3.
//  Copyright © 2017年 cai. All rights reserved.
//

#include "server.h"

int main (int argc, char *argv[]) {
    server.port = DEFAULT_PORT;
    server.tcp_backlog = TCP_BACKLOG;
    server.listenfd = anetTcpServer(server.port, server.tcp_backlog);
    if (server.listenfd < 0) {
        return -1;
    }
    
    
    
}
