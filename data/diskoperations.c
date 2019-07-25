//
// Created by wanga on 2019/7/7.
//

#include "diskoperations.h"
#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "../include/head.h"
//从num块中读取数据存放到buffer中
int get_block_data(unsigned int  num,char *buffer){
    //printf("%d\n",num);
    if(!buffer){
        printf("get block data error\n");
        return 0;
    }
    //确定文件当前位置:超级块1kb,inode位图1kb，块位图1kb，inode节点表1kb，块位置
    unsigned int  file_cur = 1024 * num;
    int fd;
    if ((fd = open("/mnt/c/Users/wanga/CLionProjects/myext2/diskfile",O_RDONLY )) == -1) {
        printf("get ext2_disk open error \n");
        return 0;
    }
    if (lseek(fd,file_cur,SEEK_SET)==-1){
        printf("get ext2_disk lseek error\n");
        return 0;
    }
    int count;
    if ((count = read(fd,buffer,1024))==0){
        printf("get ext2_disk read error\n");
        return 0;
    }
    //printf("%d\n",count);
    //struct ext2_dir_entry_2 *t;
    //t=(struct ext2_dir_entry_2*)buffer;
    //printf("%s\n",t->name);
    close (fd);
    return count;


}


int set_block_data(unsigned int  num,char *buffer){
    if(!buffer){
        printf("set block data error\n");
        return 0;
    }
    unsigned int file_cur = 1024 * num;
    int fd;
    if ((fd = open("/mnt/c/Users/wanga/CLionProjects/myext2/diskfile" , O_RDWR )) == -1) {
        printf("set ext2_disk open error \n");
        return 0;
    }
    if (lseek(fd,file_cur,SEEK_SET)==-1){
        printf("set ext2_disk lseek error\n");
        return 0;
    }
    int count;
    if ((count = write(fd,buffer,1024))==0){
        printf("set ext2_disk read error\n");
        return 0;
    }
    close (fd);
    return count;
}

int get_inode_data(unsigned int  num,char * inode){
    int fd;
    __u32 file_cur=1024*3 + 32 * num;
    if ((fd = open("/mnt/c/Users/wanga/CLionProjects/myext2/diskfile" , O_RDONLY )) == -1) {
        printf("get ext2_disk inode  open error \n");
        return 0;
    }
    if (lseek(fd,file_cur,SEEK_SET)==-1){
        printf("get ext2_disk inode  lseek error\n");
        return 0;
    }
    int count ;
    if( (count = read(fd,inode,32))==0){
        printf("get ext2_disk inode read error\n");
        return 0;
    }
    ((struct inode *)inode)->i_number = num;
    close (fd);
    return count;
}

int set_inode_data(unsigned int  num,char *inode){
    int fd;
    __u32 file_cur=1024*3 +32 + 32 * num;
    if ((fd = open("/mnt/c/Users/wanga/CLionProjects/myext2/diskfile" , O_RDWR )) == -1) {
        printf("set ext2_disk inode  open error \n");
        return 0;
    }
    if (lseek(fd,file_cur,SEEK_SET)==-1){
        printf("set ext2_disk inode  lseek error\n");
        return 0;
    }
    int count ;
    count = write(fd,inode,32);
    close (fd);
    return count;
}