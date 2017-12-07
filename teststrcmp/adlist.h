//
//  adlist.h
//  teststrcmp
//
//  Created by cai on 2017/12/4.
//  Copyright © 2017年 cai. All rights reserved.
//

#ifndef adlist_h
#define adlist_h

#include <stdio.h>

#endif /* adlist_h */

struct listNode {
    struct listNode *next, *pre;
    void *value;

};


struct list {
    struct listNode *head, *tail;
    int cur_len;
    
    
};


struct listNode *listAdd(struct list *l, void *data);

