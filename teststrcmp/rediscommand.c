//
//  rediscommand.c
//  teststrcmp
//
//  Created by cai on 2017/12/18.
//  Copyright © 2017年 cai. All rights reserved.
//

#include "rediscommand.h"

#include <stdio.h>

struct redisCommand redisCommandTable[] = {
    {"get",getCommand,2,"rF",0,NULL,1,1,1,0,0},
    {"set",setCommand,-3,"wm",0,NULL,1,1,1,0,0},
};

//从内存取value，并给客户端写执行结果
int getCommand(client *c) {
    sds msg;
    msg = dbGetKey(server.db, c->argv[1]);
    if (msg == NULL) {
        msg = sdscat(msg, "(nil)\0");
    }
    if(send(c->fd, msg, sdslen(msg), 0)<0) {
        return 1;
    }
    else return 0;
}

//存入内存，并且给客户端写执行结果

int setCommand(client *c) {
    sds msg = sdsempty(); 
    if (!dbSetKey(server.db, c->argv[1], c->argv[2])) {
        msg = sdscat(msg, "ok!\0");
    } else {
        msg = sdscat(msg, "key already exist!\0");
    }
    
    if (send(c->fd, msg, sdslen(msg), 0) < 0 ) {
        return 1;
    }
    else return 0;
    
}



redisCommandProc* getCommandProc(sds command) {
    int numCommands = sizeof(redisCommandTable)/sizeof(struct redisCommand);
    int i=0;
    
    for(i=0; i< numCommands; i++) {
        if (strcmp(redisCommandTable[i].name, command) == 0) {
            return redisCommandTable[i].proc;
        }
    }
    
    return NULL;
    
    
}

redisCommand* createCommand (client *c) {
    redisCommand *ci;
    ci = malloc(sizeof(redisCommand));
    ci->name = c->argv[0];
    if ( (ci->proc = getCommandProc(ci->name)) != NULL ) {
        return ci;
    }
    else
        return NULL;
}





