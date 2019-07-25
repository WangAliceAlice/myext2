//
// Created by wanga on 2019/7/7.
//

#ifndef MYEXT2_NAMEI_H
#define MYEXT2_NAMEI_H

#include "../include/head.h"
#include "../init/inittask.h"

struct inode* name_find_inode(char *path, struct inode* n_inode);
int namefind_inode(char *path ,__u32 pwd_inodenum);
int create(struct inode *dir,char *name,int len,int mode);
int u_mkdir(struct inode* dir,char *name,int len,int mode);
int list_all(struct inode *dir);
int delete_file (struct inode *dir,char *name);
int change_dir(char *name);
#endif //MYEXT2_NAMEI_H
