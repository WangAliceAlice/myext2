//
// Created by wanga on 2019/7/7.
//

#include "inittask.h"
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include "../include/head.h"
#include "initdisk.h"
#include "initmemory.h"
#include "../data/diskoperations.h"


struct inode * getInodeByNum(uint32_t inode){
    //extern struct inode inodesTable_memory[];
    //extern int p_inodeTable;
    for (int i=1; i<=32; i++) {
        if (inodesTable_memory[i].i_number==inode) {
            //printf("%d\n",inodesTable_memory[i].i_number);
            return &inodesTable_memory[i];
        }
    }
    return NULL;
}
void inittask(){
    memset(&task_struct,'\0',sizeof(struct task_struct));
    memset(&fs_struct,'\0',sizeof(struct fs_struct));
    memset(&fs_struct.root,'\0',sizeof(struct inode));
    memset(&fs_struct.pwd,'\0',sizeof(struct inode));
    memset(&files_struct,'\0',sizeof(struct files_struct));
    memset(sys_open,'\0',sizeof(struct file)*100);
    // memset(&file_operations,'\0',sizeof(struct file_operations));

    task_struct.fs=&fs_struct;
    task_struct.files=&files_struct;

    fs_struct.root = getInodeByNum(1);
    fs_struct.pwd = getInodeByNum(2);



   //char a[200];
   //printf("%d\n",fs_struct.pwd->i_number);
   //printf("%d\n",fs_struct.pwd->ext2_inode.i_size);
   //printf("%s\n",&inodesTable_memory[3].ext2_inode.i_block[0]);
   //memcpy(a,&fs_struct.pwd->ext2_inode.i_block[0],200);
   //printf("%s\n",a);

    //fs_struct.pwd=&pwd;
    //fs_struct.root=&root;
}
