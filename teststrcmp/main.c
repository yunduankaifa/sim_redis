//
//  main.c
//  teststrcmp
//
//  Created by cai on 2017/11/28.
//  Copyright © 2017年 cai. All rights reserved.
//

#include <stdio.h>
#include <string.h>

int main(int argc, const char * argv[]) {
    // insert code here...
   // printf("Hello, World!\n");
    int a[5];
    for (int i=0; i<5; i++) {
        if (a[i]>0) {
            printf("yes %d",a[i]);
        }
        else {
            printf("no %d", a[i] );
        }
        
    }
    return 0;
}
