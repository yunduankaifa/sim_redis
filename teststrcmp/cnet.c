//
//  cnet.c
//  teststrcmp
//
//  Created by cai on 2017/12/3.
//  Copyright © 2017年 cai. All rights reserved.
//

#include "cnet.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>


int anetTcpServer(int port, int backlog) {
    struct sockaddr_in sa;
    int s;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        return -1;
    }
    
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    
    if (bind(s, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
        return -1;
    }
    
    if (listen(s, backlog) < 0) {
        return -1;
    }
    
    return s;

}
