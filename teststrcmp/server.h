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
#include "adlist.c"
#include "cnet.h"
#include "ae.h"




#define DEFAULT_PORT  8811
#define TCP_BACKLOG   500
#define CONFIG_BINDADDR_MAX 16
#define CONFIG_MIN_RESERVED_FDS 32
#define CONFIG_FDSET_INCR (CONFIG_MIN_RESERVED_FDS+96)
#define NET_IP_STR_LEN 46


struct redisServer {
    int listenfd;
//    struct list *client;
    int port;
    int tcp_backlog;
    int epfd;
    aeEventLoop *el;
    int maxclients;
    char neterr[ANET_ERR_LEN];
    

};

struct redisServer server;

#endif /* server_h */
