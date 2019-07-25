//
// Created by wanga on 2019/7/7.
//

#ifndef MYEXT2_INITTASK_H
#define MYEXT2_INITTASK_H

#include <stdio.h>
#include <stdint.h>
#include "../include/head.h"
struct task_struct task_struct;
struct fs_struct fs_struct;
struct files_struct files_struct ;
struct file sys_open[100];//系统打开文件表
void inittask();
struct inode * getInodeByNum(uint32_t inode);
#endif //MYEXT2_INITTASK_H
