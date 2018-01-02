//
//  adlist.c
//  teststrcmp
//
//  Created by cai on 2017/12/4.
//  Copyright © 2017年 cai. All rights reserved.
//

#include "adlist.h"
#include <stdlib.h>

list *listCreate(void) {
    list *list;
    list = malloc(sizeof(list));
    list->head = NULL;
    list->tail = NULL;
    list->len  = 0;
    return list;
}



list *listAddNodeHead(list *list, void *value) {
    listNode *node = malloc(sizeof(listNode));
    node->value = value;
    if (list->len > 0) {
        node->next  = list->head;
        list->head->pre = node;
        list->head  = node;
        node->pre = NULL;
    } else {
        list->head = node;
        list->tail = node;
        node->pre  = NULL;
        node->next = NULL;
    
    }
    list->len++;
    return list;
}

list *listAddNodeTail(list *list, void *value) {
    listNode *node;
    node  = malloc(sizeof(listNode));
    node->value = value;
    if (list->len > 0) {
        node->pre  = list->tail;
        node->next = NULL;
        list->tail->next = node;
        list->tail = node;
    } else {
        list->head = node;
        list->tail = node;
        node->pre  = NULL;
        node->next = NULL;
    }
    list->len++;
    return list;
    

}

void listDeleteNode(list *list, void *value) {
    listNode *cur = list->head;
    if (cur->value == value) {
        list->head = cur->next;
        if (list->head) list->head->pre = NULL;
    }
    else {
        while(cur != NULL) {
            if (cur->value == value) {
                cur->pre->next = cur->next;
                if (cur->next) cur->next->pre = cur->pre;
            }
            cur = cur->next;
        }
    }
    
    free(cur);
}

