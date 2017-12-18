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

int initDb(redisDb* db) {
    db = (redisDb*)malloc(sizeof(redisDb));
    db->dict = malloc(sizeof(dict));
    db->dict->ht.size = DICT_INIT_SIZE;
    db->dict->ht.table = malloc(sizeof(dictEnty*)*HT_TABLE_SIZE);
    if (db->dict->ht.table==NULL) {
        return 1;
    }
    else  return 0;
}

unsigned long getHashIndex(sds key) {
    return key[0];
}

int isKeyExist(dictEnty *el, sds key) {
    while (el != NULL) {
        if (strcmp(el->key, key) == 0) return 1;
        else el = el->next;
    }
    
    return 0;
}

int dbSetKey(redisDb *db, sds key, sds value) {
    dictEnty *enty = malloc(sizeof(dictEnty));
    unsigned long index = 0;
    index = getHashIndex(key);
    if (!isKeyExist(db->dict->ht.table[index], key)) {
        enty->key = key;
        enty->v   = value;
        enty->next = db->dict->ht.table[index];
        db->dict->ht.table[index] = enty;
        db->dict->ht.used++;
        return 0;
    }
    else {
        return 1;
    }
}
