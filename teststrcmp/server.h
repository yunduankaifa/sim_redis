//
//  server.h
//  teststrcmp
//
//  Created by cai on 2017/12/3.
//  Copyright © 2017年 cai. All rights reserved.
//

#ifndef server_h
#define server_h

#include <stdio.h>
//#include "adlist.c"
#include "cnet.h"

#endif /* server_h */

#define DEFAULT_PORT  8811
#define TCP_BACKLOG   500

struct redisServer {
    int listenfd;
//    struct list *client;
    int port;
    int tcp_backlog;


};

struct redisServer server;

