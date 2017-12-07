//
//  networking.c
//  teststrcmp
//
//  Created by cai on 2017/12/5.
//  Copyright © 2017年 cai. All rights reserved.
//

#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include "ae.h"
#include "cnet.h"
#include "server.h"




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
       //  serverLog(LL_VERBOSE,"Accepted %s:%d", cip, cport);
        acceptCommonHandler(cfd,0,cip);
    }

}
