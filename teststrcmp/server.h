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
#include <unistd.h>
#include "adlist.h"
#include "cnet.h"
#include "ae.h"
#include "sds.h"
#include "redisdb.h"

#define DEFAULT_PORT  8811
#define TCP_BACKLOG   500
#define CONFIG_BINDADDR_MAX 16
#define CONFIG_MIN_RESERVED_FDS 32
#define CONFIG_FDSET_INCR (CONFIG_MIN_RESERVED_FDS+96)
#define NET_IP_STR_LEN 46
#define CONFIG_DEFAULT_TCP_KEEPALIVE 300
#define PROTO_IOBUF_LEN         (1024*16)  /* Generic I/O buffer size */

#define AOF_FILE_PATH   "aof_file.txt"

typedef long long mstime_t;

void acceptTcpHandler(aeEventLoop *el, int fd, void *clientData, int mask);

typedef struct client {
    int fd;
    
    char *buffer;
    int flags;
    sds querybuf;           /* Buffer we use to accumulate client queries. */
    sds pending_querybuf;   /* If this is a master, this buffer represents the
                             yet not applied replication stream that we
                             are receiving from the master. */
    size_t querybuf_peak;   /* Recent (100ms or more) peak of querybuf size. */
    sds *argv;
    int argc;
    
} client;



typedef struct redisServer {
    int listenfd;
    redisDb *db;
//    struct list *client;
    int port;
    int tcp_backlog;
    int epfd;
    aeEventLoop *el;
    int maxclients;
    char neterr[ANET_ERR_LEN];
    int tcpkeepalive;
    list *clients;
    
    sds aof_buffer;
    unsigned long stat_rejected_conn;
    unsigned long stat_numconnections;
    
} redisServer;





extern redisServer server;
#endif /* server_h */
