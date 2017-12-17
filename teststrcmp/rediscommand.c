//
//  rediscommand.c
//  teststrcmp
//
//  Created by cai on 2017/12/18.
//  Copyright © 2017年 cai. All rights reserved.
//

#include "rediscommand.h"

#include <stdio.h>


int getCommand(client *c) {
    return 0;
}

int setCommand(client *c) {
    return dbSetKey(c->db, c->argv[1], c->argv[2]);
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





