//
// Created by wanga on 2019/7/6.
//

#ifndef MYEXT2_INITDISK_H
#define MYEXT2_INITDISK_H

#include <stdio.h>

void init();//初始化各个block
void setsuperblock(FILE * file);//初始化superBlock和调用初始化root目录函数
void setblockbit(FILE * file);//初始化block位图
void setinodebit(FILE * file);//初始化inode位图
void setinodetable(FILE * file,int num[]);//初始化inodeTble
void setdata(FILE * file);
void mymemcopy(char*des,char *sor,int count);

#endif //MYEXT2_INITDISK_H
