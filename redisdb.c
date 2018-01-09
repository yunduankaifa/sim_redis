//
//  redisdb.c
//  teststrcmp
//
//  Created by cai on 2017/12/18.
//  Copyright © 2017年 cai. All rights reserved.
//

#include "redisdb.h"
#include <stdlib.h>
#include <string.h>

#define HT_TABLE_SIZE  256    //简化版，按key的首个字符的ascii码进行hash

redisDb* createDb() {
    redisDb *db;
    db = (redisDb*)malloc(sizeof(redisDb));
    db->dict = malloc(sizeof(dict));
    db->dict->ht.size = DICT_INIT_SIZE;
    db->dict->ht.table = malloc(sizeof(dictEnty*)*HT_TABLE_SIZE);
    return db;
}

unsigned long getHashIndex(sds key) {
    return key[0];
}

dictEnty *isKeyExist(dictEnty *el, sds key) {
    while (el != NULL) {
        if (strcmp(el->key, key) == 0) break;
        else el = el->next;
    }
    
    return el;
}

int dbSetKey(redisDb *db, sds key, sds value) {
    dictEnty *new_enty=NULL, *old_enty=NULL;
    unsigned long index = 0;
    index = getHashIndex(key);
    if (!(old_enty=isKeyExist(db->dict->ht.table[index], key))) {
        new_enty = malloc(sizeof(dictEnty));
        if (!new_enty) return 1;
        new_enty->key = key;
        new_enty->v   = value;
        new_enty->next = db->dict->ht.table[index];
        db->dict->ht.table[index] = new_enty;
        db->dict->ht.used++;
    }
    else {
        old_enty->v = value;
    }
    return 0;
}

sds dbGetKey(redisDb *db, sds key) {
    int index = getHashIndex(key);
    dictEnty *el  = db->dict->ht.table[index];
    while (el != NULL) {
        if (strcmp(el->key, key) == 0) return el->v;
        else el = el->next;
    }
    
    return NULL;
}
