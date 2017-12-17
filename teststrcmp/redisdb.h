//
//  redisdb.h
//  teststrcmp
//
//  Created by cai on 2017/12/18.
//  Copyright © 2017年 cai. All rights reserved.
//

#ifndef redisdb_h
#define redisdb_h

#include <stdio.h>
#include "sds.h"

#define DICT_INIT_SIZE  100

typedef unsigned int computeHashKey(sds key);

typedef struct dictEnty {
    sds key;
    sds v;
    struct dictEnty *next;
} dictEnty;

typedef struct dictht{
    computeHashKey* comfunc;
    dictEnty **table;
    int size;
    int used;
} dictht;


typedef struct dict {
    dictht ht;
    
} dict;

// db
typedef struct redisDb {
    int id;
    dict *dict;
} redisDb;

int initDb(redisDb* db) ;

unsigned long getHashIndex(sds key);

int isKeyExist(dictEnty *el, sds key);
int dbSetKey(redisDb *db, sds key, sds value);




#endif /* redisdb_h */
