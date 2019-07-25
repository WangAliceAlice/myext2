//
// Created by wanga on 2019/7/7.
//

#ifndef MYEXT2_INODE_H
#define MYEXT2_INODE_H

#include <stdint.h>
//#include "../init/initmemory.h"
#include "../include/head.h"
extern int free_inode(int inode_num);//释放一个inode
extern int new_inode();//获取一个inode
void analyse_pathTo_inode(char * path,struct inode ** inode,char * ret_name);
void find_inode(uint32_t inodeNum,struct inode ** inode);
void get_inodeNum_from_content(char * name , struct inode * inode,int * ret_inode_num);
#endif //MYEXT2_INODE_H
