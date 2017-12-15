//
//  cnet.h
//  teststrcmp
//
//  Created by cai on 2017/12/3.
//  Copyright © 2017年 cai. All rights reserved.
//

#ifndef cnet_h
#define cnet_h

#include <stdio.h>
#include <sys/socket.h>


#define ANET_OK 0
#define ANET_ERR -1
#define ANET_ERR_LEN 256

/* Flags used with certain functions. */
#define ANET_NONE 0
#define ANET_IP_ONLY (1<<0)

#if defined(__sun) || defined(_AIX)
#define AF_LOCAL AF_UNIX
#endif

#ifdef _AIX
#undef ip_len
#endif



int anetTcpServer(int port, int backlog);
int anetTcpAccept(char *err, int s, char *ip, size_t ip_len, int *port);
static int anetGenericAccept(char *err, int s, struct sockaddr *sa, socklen_t *len);
static void anetSetError(char *err, const char *fmt, ...);
int anetNonBlock(char *err, int fd);
int anetBlock(char *err, int fd);
int anetEnableTcpNoDelay(char *err, int fd);
int anetDisableTcpNoDelay(char *err, int fd);
int anetKeepAlive(char *err, int fd, int interval);




#endif /* cnet_h */
