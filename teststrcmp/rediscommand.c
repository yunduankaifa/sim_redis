//
//  rediscommand.c
//  teststrcmp
//
//  Created by cai on 2017/12/14.
//  Copyright © 2017年 cai. All rights reserved.
//

#include <stdio.h>
#include "server.h"
typedef int redisCommandProc(client *c);
typedef void redisGetKeysProc();
int getCommand();
int setCommand();

typedef struct redisCommand {
    char *name;
    redisCommandProc *proc;
    int arity;
    char *sflags; /* Flags as string representation, one char per flag. */
    int flags;    /* The actual flags, obtained from the 'sflags' field. */
    /* Use a function to determine keys arguments in a command line.
     * Used for Redis Cluster redirect. */
    redisGetKeysProc *getkeys_proc;
    /* What keys should be loaded in background when calling this command? */
    int firstkey; /* The first argument that's a key (0 = no keys) */
    int lastkey;  /* The last argument that's a key */
    int keystep;  /* The step between first and last key */
    long long microseconds, calls;
} redisCommand;

struct redisCommand redisCommandTable[] = {
    {"get",getCommand,2,"rF",0,NULL,1,1,1,0,0},
    {"set",setCommand,-3,"wm",0,NULL,1,1,1,0,0},
};

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





