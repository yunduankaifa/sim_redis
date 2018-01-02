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

int aeProcessEvents(aeEventLoop *eventLoop, int flags)
{
    int processed = 0, numevents;
    struct timeval *tvp = NULL;
    /* Nothing to do? return ASAP */
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
        tvp = malloc(sizeof(struct timeval));
        tvp->tv_sec = 10;
        tvp->tv_usec = 0;
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

