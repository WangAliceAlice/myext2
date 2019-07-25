//
// Created by wanga on 2019/7/7.
//

#include "inode.h"
#include "super.h"
#include"../include/head.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "bitmap.h"
#include "../init/initmemory.h"
#include "../init/inittask.h"
#include "../data/diskoperations.h"

#define ext2_disk "/mnt/c/Users/wanga/CLionProjects/simple_ext2/myext2/diskfile"
//struct ext2_super_block super_block;
//extern __u32 inodemap[1];
//extern __u32 blockmap[1];

int free_inode(int inode_num){
    if(get_inode_bitmap((char *)inodemap)==-1) {
        printf("get_inode_bitmap error\n");
        return -1;
    }
    if(set_bitmap(inodemap,inode_num,0)==-1){
        printf("set 0 inodemap error\n");
        return -1;
    }
    if(set_inode_bitmap((char *)inodemap)==-1){
        printf("set_inode_bitmap error\n");
        return -1;
    }
    get_super_block_data(&superBlock_memory);
    //printf("super_block free_inode ;%d \n ",super_block.s_free_inodes_count);
    superBlock_memory.ext2_super_block.s_free_inodes_count++;
    set_super_block_data(&superBlock_memory);
    return 1;
}

int new_inode(){
    if(get_inode_bitmap((char *)inodemap)==-1) {
        printf("get_inode_bitmap error\n");
        return -1;
    }
    int inode_num;
    if((inode_num=get_free_bitmap((__u32 *)inodemap))==-1){
        printf("get_free_bitmap error\n");
        return -1;
    }
    //printf("inode_num :%d \n",inode_num);
    if(set_bitmap(inodemap,inode_num,1)==-1){
        printf("set 1 bitmap error\n");
        return -1;
    }
    if(set_inode_bitmap((char *)inodemap)==-1){
        printf("set_inode_bitmap error\n");
        return -1;
    }
    get_super_block_data(&superBlock_memory);
    //printf("super_block free inode : %d\n",super_block.s_free_inodes_count);
    superBlock_memory.ext2_super_block.s_free_inodes_count--;
    set_super_block_data(&superBlock_memory);
    return inode_num;
}

/*通过inumber找inode结构体地址*/
void find_inode(uint32_t inodeNum,struct inode ** inode){
    extern struct inode inodesTable_memory[];
    extern int p_inodeTable;
    for(int i=1;i<=p_inodeTable;i++) {//从1号i节点开始查找
        if (inodesTable_memory[i].i_number==inodeNum) {
            *inode = &inodesTable_memory[i];
            return;
        }
    }
    inode = NULL;
}



void analyse_pathTo_inode(char * path,struct inode ** inode,char * ret_name) {
    //printf("dfgvbjhfb\n");

    //ret_name是最顶层的目录名
    if (strcmp(path, "/") == 0 || strcmp(path, "/root/") == 0 || strcmp(path, "/root") == 0) {

        struct inode *inode_root = NULL;
        find_inode(1, &inode_root);//通过inode号拿到curr
        *inode = inode_root;
        printf("%d\n",inode_root->i_number);
        /*if (ret_name != NULL) {
            strncpy(ret_name, "/", 1);
        }
         */
        //strncpy(ret_name, "/", 1);
        ret_name="/";
        printf("%s\n",ret_name);
        return ;
    }
    int p_begin = 0, p_end = 0, tmp = 0;
    struct inode *currentInode = NULL;
    if (path[0] == '/') {
        p_begin += 1;
        tmp += 1;
        find_inode(1, &currentInode);//通过inode号拿到curr
    }

    char name[DIR_NAME_SIZE];
    //extern struct User *currentUser;

    if (currentInode == NULL) {
         struct task_struct *currentTask;
      currentInode = currentTask->fs->pwd;//当前目录inode号,不在根目录中
    }

    int ret_inodeNum;
    while (path[tmp] != '\0') {//结束标志
        while (path[tmp] != '/' && path[tmp] != '\0') {
            tmp += 1;
        }
        p_end = tmp;
        memset(name, '\0', DIR_NAME_SIZE);//清空字符缓存区
        strncpy(name, path + p_begin, p_end - p_begin);
        printf("%s\n",name);

        //在当前目录中找name文件,并返回inode号
        //get_inodeNum_from_content(name, currentInode, &ret_inodeNum);//从当前目录查找
        //没有该目录项就返回-1
        /*if (ret_inodeNum == -1) {
            *inode = NULL;
            if (ret_name != NULL) {
                strncpy(ret_name, path + p_begin, p_end - p_begin);
            }
            printf("%s\n",ret_name);
            return;
        }
        //再次改变currentinode，找当前目录之后的内容
        find_inode(ret_inodeNum, &currentInode);
        if (path[tmp] == '\0') {//没有‘/’的情况
            if (tmp == strlen(path)) {//顶层目录项
                *inode = currentInode;
                if (ret_name != NULL) {
                    strncpy(ret_name, path + p_begin, p_end - p_begin);
                }
            }
        } else {
            if (tmp == strlen(path) - 1) {//顶层目录项
                *inode = currentInode;
                if (ret_name != NULL) {
                    strncpy(ret_name, path + p_begin, p_end - p_begin);
                }
            }
        }
         */
        p_begin = p_end + 1;
        tmp = p_begin;
    }
    printf("%s\n",ret_name);

}


/*从目录中找inodenum，通过文件名*/
void get_inodeNum_from_content(char * name , struct inode * inode,int * ret_inode_num){
    //1.先拷贝一个block到缓冲，没找到再拷贝下一个
    FILE * file = NULL;
    if ((file = fopen("/mnt/c/Users/wanga/CLionProjects/myext2/diskfile", "r"))==NULL) {
        printf("fopen异常\n");
    }
    struct ext2_dir_entry_2 dentry;
    char buff[BLOCK_SIZE];
    //    uint32_t more_size_256 = (inode->i_size%BLOCK_SIZE)/256;
    for (int i=0; i<inode->ext2_inode.i_blocks; i++) {
        if (ftell(file)!=inode->ext2_inode.i_block[i]*BLOCK_SIZE) {
            fseek(file, inode->ext2_inode.i_block[i]*BLOCK_SIZE, SEEK_SET);
        }
        //每次读一个block大小
        if (fread(buff, BLOCK_SIZE, 1, file)<1) {
            printf("fread异常\n");
        }
        for (int j=0; j<BLOCK_SIZE/DIR_ENTRY_SIZE; j++) {
            memcpy(&dentry, buff+j*DIR_ENTRY_SIZE, DIR_ENTRY_SIZE);
            if (strcmp(dentry.name, name)==0) {
                if (dentry.inode==0) {
                    printf("该目录项已删除\n");
                    *ret_inode_num = 0;
                    fclose(file);
                    return;
                }
                *ret_inode_num = dentry.inode;
                printf("%d\n",ret_inode_num);
                fclose(file);
                return;
            }
        }
        //找不到该目录的情况:
        *ret_inode_num = -1;
    }
    fclose(file);
}