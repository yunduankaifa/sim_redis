//
//  ae.c
//  teststrcmp
//
//  Created by cai on 2017/12/4.
//  Copyright © 2017年 cai. All rights reserved.
//

#include "ae.h"
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>

#ifdef HAVE_EPOLL
#include "aepoll.c"
#endif

aeEventLoop *aeCreateEventLoop(int setsize) {
    aeEventLoop *el;
    el = malloc(sizeof(aeEventLoop));
    if (el == NULL) goto err;
    
    el->maxfd = -1;
    el->setsize = setsize;
    
    el->events = malloc(sizeof(aeFileEvent)*setsize);
    el->fired = malloc(sizeof(aeFileEvent)*setsize);
    if(el->events == NULL) goto err;
    
    el->timeEvents = NULL;
    el->stop = 0;
    el->beforesleep = NULL;
    if (aeApiCreate(el) == -1) goto err;
    
    return el;
    
    
err:
    if(el) {
        free(el->events);
        free(el);
        
    }
    return NULL;
    
}


int aeCreateFileEvent(aeEventLoop *eventLoop, int fd, int mask, aeFileProc *proc, void *clientData) {
    if (fd > eventLoop->setsize) {
        errno = ERANGE;
        return AE_ERR;
    }
    
    aeFileEvent *fe = &eventLoop->events[fd];
    if (aeApiAddEvent(eventLoop, fd, mask) == -1)
        return AE_ERR;
    fe->mask |= mask;   //为啥要或上之前的呢？ 要是复用了之前的fd，那岂不是会错？ 还是说肯定不会复用以前的？
                        //答：之前可能就存在监听事件，读或者写。 mask标识每个套接字的所有监听事件

    if (fe->mask & AE_READABLE) fe->rfileProc = proc;
    if (fe->mask & AE_WRITABLE) fe->wfileProc = proc;
    fe->clientData = clientData;
    
    if (fd > eventLoop->maxfd) eventLoop->maxfd = fd;
    return AE_OK;
}

void aeDeleteFileEvent(aeEventLoop *eventLoop, int fd, int mask) {
    int j=0;
    eventLoop->events[fd].mask &= ~mask;
    if (fd == eventLoop->maxfd && eventLoop->events[fd].mask==AE_NONE) {
        for (j=eventLoop->maxfd; j>=0; j--) {
            if (eventLoop->events[j].mask != AE_NONE){
                eventLoop->maxfd = j;
                break;
            }
        }
    }

    aeApiRemoveEvent(eventLoop, fd, mask);

}


int aeCreateTimeEvent(aeEventLoop *eventLoop, long long milliseconds) {
    aeTimeEvent *te = malloc(sizeof(aeTimeEvent));
    if (te == NULL) return AE_ERR;
    aeAddMillisecondsToNow(milliseconds,&te->when_sec,&te->when_ms);


    te->timeInterval = milliseconds;
    te->next = eventLoop->timeEvents;
    eventLoop->timeEvents = te;
    return AE_OK;
}


aeTimeEvent *aeGetNearestTimeEvent(aeEventLoop *eventLoop) {
    aeTimeEvent *te = eventLoop->timeEvents, *nearest=NULL;
    while (te != NULL) {
        if (nearest==NULL || te->when_sec<nearest->when_sec || (te->when_sec==nearest->when_sec && te->when_ms<nearest->when_ms))
            nearest = te;
        te=te->next;
    }
    
    return nearest;
}



void aeDeleteTimeEvent(aeEventLoop *eventLoop, aeTimeEvent *timeEvent) {
    
    
}

/*
 * 在当前时间上加上 milliseconds 毫秒，
 * 并且将加上之后的秒数和毫秒数分别保存在 sec 和 ms 指针中。
 */
static void aeAddMillisecondsToNow(long long milliseconds, long *sec, long *ms) {
    long cur_sec, cur_ms, when_sec, when_ms;
    
    // 获取当前时间
    aeGetTime(&cur_sec, &cur_ms);
    
    // 计算增加 milliseconds 之后的秒数和毫秒数
    when_sec = cur_sec + milliseconds/1000;
    when_ms = cur_ms + milliseconds%1000;
    
    // 进位：
    // 如果 when_ms 大于等于 1000
    // 那么将 when_sec 增大一秒
    if (when_ms >= 1000) {
        when_sec ++;
        when_ms -= 1000;
    }
    
    // 保存到指针中
    *sec = when_sec;
    *ms = when_ms;
}

/*
 * 取出当前时间的秒和毫秒，
 * 并分别将它们保存到 seconds 和 milliseconds 参数中
 */
static void aeGetTime(long *seconds, long *milliseconds)
{
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    *seconds = tv.tv_sec;
    *milliseconds = tv.tv_usec/1000;
}






int aeProcessEvents(aeEventLoop *eventLoop, int flags)
{
    int processed = 0, numevents;
    struct timeval *tvp = NULL;
    aeTimeEvent *shortest=NULL;
    long now_sec, now_ms;


    if (!(flags & AE_TIME_EVENTS) && !(flags & AE_FILE_EVENTS)) return 0;
    
    /* Note that we want call select() even if there are no
     * file events to process as long as we want to process time
     * events, in order to sleep until the next time event is ready
     * to fire. */
    if (eventLoop->maxfd != -1 ||
        ((flags & AE_TIME_EVENTS) && !(flags & AE_DONT_WAIT))) {
        int j;
        
        /* Call the multiplexing API, will return only on timeout or when
         * some event fires. */
        shortest = aeGetNearestTimeEvent(eventLoop);
        aeGetTime(&now_sec, &now_ms);
        tvp = malloc(sizeof(struct timeval));
        
        if (shortest) {
            tvp->tv_sec = shortest->when_sec - now_sec;
            if (shortest->when_ms < now_ms) {
                tvp->tv_usec = ((shortest->when_ms+1000) - now_ms)*1000;
                tvp->tv_sec --;
            } else {
                tvp->tv_usec = (shortest->when_ms - now_ms)*1000;
            }
            
            // 时间差小于 0 ，说明事件已经可以执行了，将秒和毫秒设为 0 （不阻塞）
            if (tvp->tv_sec < 0) tvp->tv_sec = 0;
            if (tvp->tv_usec < 0) tvp->tv_usec = 0;
            printf("lastTime:%ld\n", tvp->tv_sec);
        }
        else {
            tvp->tv_sec = 0;
            tvp->tv_usec = 0;
        
        }

        numevents = aeApiPoll(eventLoop, tvp);
        
        /* After sleep callback. */
        if (eventLoop->aftersleep != NULL && flags & AE_CALL_AFTER_SLEEP)
            eventLoop->aftersleep(eventLoop);
        
        for (j = 0; j < numevents; j++) {
            aeFileEvent *fe = &eventLoop->events[eventLoop->fired[j].fd];
            int mask = eventLoop->fired[j].mask;
            int fd = eventLoop->fired[j].fd;
            int rfired = 0;
            
            /* note the fe->mask & mask & ... code: maybe an already processed
             * event removed an element that fired and we still didn't
             * processed, so we check if the event is still valid. */
            if (fe->mask & mask & AE_READABLE) {
                rfired = 1;
                fe->rfileProc(eventLoop,fd,fe->clientData,mask);
            }
            if (fe->mask & mask & AE_WRITABLE) {
                if (!rfired || fe->wfileProc != fe->rfileProc)
                    fe->wfileProc(eventLoop,fd,fe->clientData,mask);
            }
            processed++;
        }
    }

    
    return processed; /* return the number of processed file/time events */
}





void aeMain(aeEventLoop *eventLoop) {
    eventLoop->stop = 0;
    while (!eventLoop->stop) {
        if (eventLoop->beforesleep != NULL)
            eventLoop->beforesleep(eventLoop);
        aeProcessEvents(eventLoop, AE_ALL_EVENTS|AE_CALL_AFTER_SLEEP);
    }
}

