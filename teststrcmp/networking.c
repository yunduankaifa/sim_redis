//
//  networking.c
//  teststrcmp
//
//  Created by cai on 2017/12/5.
//  Copyright © 2017年 cai. All rights reserved.
//


#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include "ae.h"
#include "cnet.h"
#include "server.h"
//#include "sds.h"
#include "rediscommand.h"


#define UNUSED(x) (void)(x)
void readQueryFromClient(aeEventLoop *el, int fd, void *privdata, int mask);
static void acceptCommonHandler(int fd, int flags, char *ip);

client *createClient(int fd) {
    client *c = NULL;
    c = malloc(sizeof(client));
    c->flags = 0;
    c->querybuf = sdsempty();
    c->querybuf = sdsMakeRoomFor(c->querybuf, PROTO_IOBUF_LEN);
    
    // 查询缓冲区峰值
    c->querybuf_peak = 0;
    c->argc = 0;
    c->argv = NULL;
    if (fd != -1) {
        anetNonBlock(NULL,fd);
        anetEnableTcpNoDelay(NULL,fd);
        if (server.tcpkeepalive)
            anetKeepAlive(NULL,fd,server.tcpkeepalive);
        if (aeCreateFileEvent(server.el,fd,AE_READABLE,
                              readQueryFromClient, c) == AE_ERR)
        {
            close(fd);
            free(c);
            return NULL;
        }
    }
    
    c->fd = fd;
    if (fd != -1) listAddNodeTail(server.clients,c);
    
    return c;
}

void freeClient(client *c) {
    close(c->fd);
    sdsfree(c->pending_querybuf);
    sdsfree(c->querybuf);
    //server clients 去掉 listremove
    //eventloop 里对应的fd对应的事件去掉 =NULL
    //epoll 监听的事件去掉 epoll_ctl
  //  aeApiRemoveEvent(server.eventLoop, c->fd, server.eventLoop->apidata->events[fd].mask);
    
    aeDeleteFileEvent(server.el, c->fd, server.el->events[c->fd].mask);
    listDeleteNode(server.clients, c);
}




#define MAX_ACCEPTS_PER_CALL 1000

void acceptTcpHandler(aeEventLoop *el, int fd, void *clientData, int mask) {
    int cport, cfd=-1, max = MAX_ACCEPTS_PER_CALL;
    char cip[NET_IP_STR_LEN];
    
  //  while(max--) {
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
  
        acceptCommonHandler(cfd,0,cip);
 //   }

}



static void acceptCommonHandler(int fd, int flags, char *ip) {
    client *c;
    if ((c = createClient(fd)) == NULL) {
        close(fd); /* May be already closed, just ignore errors */
        return;
    }
    /* If maxclient directive is set and this is one client more... close the
     * connection. Note that we create the client instead to check before
     * for this condition, since now the socket is already set in non-blocking
     * mode and we can send an error for free using the Kernel I/O */
    if (listLength(server.clients) > server.maxclients) {
        char *err = "-ERR max number of clients reached\r\n";
        
        /* That's a best effort error message, don't check write errors */
        if (write(c->fd,err,strlen(err)) == -1) {
            /* Nothing to do, Just to avoid the warning... */
        }
        server.stat_rejected_conn++;
        freeClient(c);
        return;
    }
    
    /* If the server is running in protected mode (the default) and there
     * is no password set, nor a specific interface is bound, we don't accept
     * requests from non loopback interfaces. Instead we try to explain the
     * user what to do to fix it if needed. */
    server.stat_numconnections++;
    c->flags |= flags;
}

void getArgFromBuffer(sds buffer, sds *argv, int *argc) {
    argv = sdssplitargs(buffer, argc);
}


void processInputBuffer(client *c) {
    if (c == NULL) return;
    c->argv = sdssplitargs(c->querybuf, &(c->argc));
    if (c->argc == 0) return;
    redisCommand *ci = createCommand(c);
    if (ci && !ci->proc(c)) {
        server.aof_buffer = sdscat(server.aof_buffer, c->querybuf);
        if (server.aof_buffer == NULL) perror("allocate for aof_buffer error");
        else printf("aof_buffer: %s\n", server.aof_buffer);
    }
    sdsclear(c->querybuf);
    if (writeAofBufferToFile(server.aof_buffer) !=0 ) {
        perror("writeAofBufferToFile error!\n");
    }
}

void readQueryFromClient(aeEventLoop *el, int fd, void *privdata, int mask) {
    client *c = (client*) privdata;
    int nread=0, readlen;
    UNUSED(el);
    UNUSED(mask);
    
    readlen = PROTO_IOBUF_LEN;

    nread = read(fd, c->querybuf, readlen);
    printf("buffer: %s\n", c->querybuf);
    if (nread == -1) {
        if (errno == EAGAIN) {
            return;
        } else {
  //          serverLog(LL_VERBOSE, "Reading from client: %s",strerror(errno));
            freeClient(c);
            return;
        }
    } else if (nread == 0) {
 //       serverLog(LL_VERBOSE, "Client closed connection");
        freeClient(c);
        return;
    }
    
    processInputBuffer(c);
    
}

