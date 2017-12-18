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
    c->db    = malloc(sizeof(redisDb));
    c->querybuf = sdsempty();
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
    free(c->buffer);
    sdsfree(c->pending_querybuf);
    sdsfree(c->querybuf);
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
    redisCommand *ci = createCommand(c);
    if (ci->proc(c)==0) {
        printf("command exe success!\n");
    } else {
        printf("command exe failed!!!\n");
    }
    
}

void readQueryFromClient(aeEventLoop *el, int fd, void *privdata, int mask) {
    client *c = (client*) privdata;
    int nread, readlen;
    size_t qblen;
    UNUSED(el);
    UNUSED(mask);
    
    readlen = PROTO_IOBUF_LEN;
    /* If this is a multi bulk request, and we are processing a bulk reply
     * that is large enough, try to maximize the probability that the query
     * buffer contains exactly the SDS string representing the object, even
     * at the risk of requiring more read(2) calls. This way the function
     * processMultiBulkBuffer() can avoid copying buffers to create the
     * Redis Object representing the argument. */
/*    if (c->reqtype == PROTO_REQ_MULTIBULK && c->multibulklen && c->bulklen != -1
        && c->bulklen >= PROTO_MBULK_BIG_ARG)
    {
        int remaining = (unsigned)(c->bulklen+2)-sdslen(c->querybuf);
        
        if (remaining < readlen) readlen = remaining;
    }
 */
    qblen = sdslen(c->querybuf);
    if (c->querybuf_peak < qblen) c->querybuf_peak = qblen;
    c->querybuf = sdsMakeRoomFor(c->querybuf, readlen);
    
    nread = read(fd, c->querybuf+qblen, readlen);
    printf("%s\n", c->querybuf+qblen );
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

