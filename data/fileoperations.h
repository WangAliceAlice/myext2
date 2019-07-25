//
// Created by wanga on 2019/7/7.
//

#ifndef MYEXT2_FILEOPERATIONS_H
#define MYEXT2_FILEOPERATIONS_H

#include "../include/head.h"


extern int closefile(int fd);//关闭文件
extern const int find_position(__u32 f_pos,__u32 *num,__u32 *offset);//找到当前文件所在块号域块内偏移
extern int u_lseek(struct inode* l_inode,struct file* l_file,int offset,int flag);
int sys_mkdir (char *name,int len,int mode);
int readfile(int number);
int writefile(char *c,int number);
int openfile(char *path);
int createfile(char * name);
int checkexist(char *name);
int getfreeinode();
int getfreeblock();
#endif //MYEXT2_FILEOPERATIONS_H
