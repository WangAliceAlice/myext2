//
// Created by wanga on 2019/7/7.
//

#ifndef MYEXT2_INITMEMORY_H
#define MYEXT2_INITMEMORY_H
#include<stdio.h>
#include "../include/head.h"

struct super_block superBlock_memory;/*定义全局superBlock结构体(内存中)*/
struct inode inodesTable_memory[32];/*定义全局inodeTable(内存中)*/
char inodemap[1024];
char blockmap[1024];
void loadFileSysFromDisk(void);/*加载硬盘中数据*/
#endif //MYEXT2_INITMEMORY_H
