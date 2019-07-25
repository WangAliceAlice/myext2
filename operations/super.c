//
// Created by wanga on 2019/7/7.
//

#include "super.h"
#include"../include/head.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#define ext2_disk "/mnt/c/Users/wanga/CLionProjects/myext2/diskfile"

int get_super_block_data(struct ext2_super_block *super_block){
    int fd;
    if((fd = open(ext2_disk,O_RDONLY))==-1){
        printf("read ext2_super_block error\n");
        return 0;
    }
    if(lseek(fd,1024,SEEK_SET)==-1){
        printf("set seek ext2_super_block error\n");
        return 0;
    }
    int num=read(fd,super_block,sizeof(struct ext2_super_block));
    close(fd);
    return num;
}

int set_super_block_data(struct ext2_super_block * super_block){
    int fd;
    if((fd = open(ext2_disk,O_RDWR))==-1){
        printf("set ext2_super_block error\n");
        return 0;
    }
    lseek (fd,1024,SEEK_SET);
    int num=write(fd,super_block,sizeof(struct ext2_super_block));
    close(fd);
    return num;
}