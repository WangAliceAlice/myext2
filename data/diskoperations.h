//
// Created by wanga on 2019/7/7.
//

#ifndef MYEXT2_DISKOPERATIONS_H
#define MYEXT2_DISKOPERATIONS_H
//ext2_disk.c:实现对磁盘文件的操作，读写block和读写inode
int get_block_data(unsigned int  num,char *buffer);
int set_block_data(unsigned int  num,char *buffer);
int get_inode_data(unsigned int  num,char * inode);
extern int set_inode_data(unsigned int  num,char *inode);
#endif //MYEXT2_DISKOPERATIONS_H
