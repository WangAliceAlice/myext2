//
// Created by wanga on 2019/7/7.
//

#ifndef MYEXT2_BITMAP_H
#define MYEXT2_BITMAP_H

#include "../include/head.h"

//bitmap.c : 关于位图的一些列操作

extern int set_bitmap(__u32 *bitmap,int block_num,int flag);//将一个bitmap中的某一位设置为0或1
extern int get_free_bitmap(__u32 *bitmap);//在位图中寻找一个空闲块或者inode
extern int get_inode_bitmap(char *bitmap);//从磁盘获得inode位图
extern int set_inode_bitmap(char *bitmap);//将inode位图保存到磁盘中
extern int get_block_bitmap(char *bitmap);//从磁盘中获得block位图
extern int set_block_bitmap(char *bitmap);//将block位图保存到磁盘中
extern int free_block(int block_num);//释放某一个block，需要将该block位图设置为0
extern int new_block();//获取一个空的block，并且设置位图为1

#endif //MYEXT2_BITMAP_H
