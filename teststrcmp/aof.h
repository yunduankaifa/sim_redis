//
//  aof.h
//  teststrcmp
//
//  Created by cai on 2018/1/6.
//  Copyright © 2018年 cai. All rights reserved.
//

#ifndef aof_h
#define aof_h

#include <stdio.h>
#include "sds.h"
#include "redisdb.h"

int createAofHandler(const char *const filePath);
int writeAofBufferToFile(const sds aof_buf);
int loadAofFile();

#endif /* aof_h */
