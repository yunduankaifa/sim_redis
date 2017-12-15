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

#define listLength(l)  ((l)->len)

typedef struct listNode {
    struct listNode *next, *pre;
    void *value;

} listNode;


typedef struct list {
    listNode *head, *tail;
    unsigned long len;
    
    
} list;


struct listNode *listAdd(struct list *l, void *data);
list *listCreate(void);
list *listAddNodeHead(list *list, void *value);
list *listAddNodeTail(list *list, void *value);



#endif /* adlist_h */
