//
//  ae.h
//  teststrcmp
//
//  Created by cai on 2017/12/4.
//  Copyright © 2017年 cai. All rights reserved.
//事件管理。 事件初始化，

#ifndef ae_h
#define ae_h

#include <stdio.h>
#include <stdlib.h>


#endif /* ae_h */

#ifdef __linux__
#define HAVE_EPOLL 1
#endif

#if (defined(__APPLE__) && defined(MAC_OS_X_VERSION_10_6)) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined (__NetBSD__)
#define HAVE_KQUEUE 1
#endif

#ifdef HAVE_EPOLL
#include "aepoll.c"
#endif


#define AE_OK 0
#define AE_ERR -1

#define AE_NONE 0
#define AE_READABLE 1
#define AE_WRITABLE 2

#define AE_FILE_EVENTS 1
#define AE_TIME_EVENTS 2
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)
#define AE_DONT_WAIT 4
#define AE_CALL_AFTER_SLEEP 8

#define AE_NOMORE -1
#define AE_DELETED_EVENT_ID -1


struct aeEventLoop;

/* Types and data structures
 *
 * 事件接口
 */
typedef void aeFileProc(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask);
typedef int aeTimeProc(struct aeEventLoop *eventLoop, long long id, void *clientData);
typedef void aeEventFinalizerProc(struct aeEventLoop *eventLoop, void *clientData);
typedef void aeBeforeSleepProc(struct aeEventLoop *eventLoop);


typedef struct aeFileEvent {
    
    // 监听事件类型掩码，
    // 值可以是 AE_READABLE 或 AE_WRITABLE ，
    // 或者 AE_READABLE | AE_WRITABLE
    int mask; /* one of AE_(READABLE|WRITABLE) */
    
    // 读事件处理器
    aeFileProc *rfileProc;
    
    // 写事件处理器
    aeFileProc *wfileProc;
    
    // 多路复用库的私有数据
    void *clientData;
    
} aeFileEvent;


/* A fired event
 *
 * 已就绪事件
 */
typedef struct aeFiredEvent {
    
    // 已就绪文件描述符
    int fd;
    
    // 事件类型掩码，
    // 值可以是 AE_READABLE 或 AE_WRITABLE
    // 或者是两者的或
    int mask;
    
} aeFiredEvent;

typedef struct aeEventLoop {
    // 目前已注册的最大描述符
    int maxfd;   /* highest file descriptor currently registered */
    
    // 目前已追踪的最大描述符
    int setsize; /* max number of file descriptors tracked */
    
    // 最后一次执行时间事件的时间
    time_t lastTime;     /* Used to detect system clock skew */
    
    // 已注册的文件事件
    aeFileEvent *events; /* Registered events */
    
    // 已就绪的文件事件
    aeFiredEvent *fired; /* Fired events */
    
    
    // 事件处理器的开关
    int stop;
    
    // 多路复用库的私有数据
    void *apidata; /* This is used for polling API specific data */
    
    // 在处理事件前要执行的函数
    aeBeforeSleepProc *beforesleep;
    aeBeforeSleepProc *aftersleep;


} aeEventLoop;


aeEventLoop *aeCreateEventLoop(int setSize);
int aeCreateFileEvent(aeEventLoop *eventLoop, int fd, int mask, aeFileProc *proc, void *clientData);
int aeDeleteFileEvent(aeEventLoop *eventLoop, aeFileEvent *event);
int aeProcessEvents(aeEventLoop *eventLoop, int flags);
void aeMain(aeEventLoop *eventLoop);






