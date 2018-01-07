//
//  aof.c
//  teststrcmp
//
//  Created by cai on 2018/1/6.
//  Copyright © 2018年 cai. All rights reserved.
//

#include "aof.h"
#include "sds.h"
#include <errno.h>
#include <unistd.h>
//#include <sys/file.h>



FILE *aof_handler=NULL;

int createAofHandler(const char *const filePath) {
    aof_handler = fopen(filePath, "a");
    if (!aof_handler) return errno;
    return 0;
}

//写入文件并同步。 需要改造成可以设置频次
int writeAofBufferToFile(const sds aof_buf) {
    if (!aof_handler) {
       return 1;
        
    }
    
    if (sdslen(aof_buf)==0) return 0;
    if (fputs(aof_buf, aof_handler)) return 0;
    else return 1;
}


