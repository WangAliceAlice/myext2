//
// Created by wanga on 2019/7/7.
//

#include "initmemory.h"
#include <stdlib.h>
#include <stdint.h>
#include "../include/head.h"
#include "initdisk.h"
/*加载硬盘中数据*/
/*结构体列表*/
int p_inodeTable = 1;   /*永远指向inodeTable末尾,0处不填数据,为了保证和硬盘inodetable对齐*/
//char inodeMap_memory[4];/*定义全局inode位图，因为最多有32个inode*/
//char blockMap_memory[64];/*定义全局block位图，因为硬盘容量32块，一个位映射一个块*/
/*函数列表*/
void loadSuperBlock(FILE * file);/*加载superBlock*/
void loadInodeTable(FILE * file);/*加载inodeTable*/
void loadInode_bit(FILE * file);/*加载inode位图*/
void loadBlock_bit(FILE * file);/*加载块位图*/
/*
 从硬盘中引导文件系统
 （模拟BOOT的过程）
 */
void loadFileSysFromDisk(void){
    /*
     1.加载superblock
     2.加载blockmap
     3.加载inodemap
     4.加载inodetable
     5.加载其他（根目录文件结构等）
     */

    //打开文件：
    FILE * file = NULL;
    if ((file = fopen("/mnt/c/Users/wanga/CLionProjects/myext2/diskfile", "r+"))==NULL) {
        printf("fopen异常\n");
    }
    //1.加载superblock
    loadSuperBlock(file);
    //2.加载inodeTable
    loadInodeTable(file);
    //3.加载imap
    loadInode_bit(file);
    //4.加载block位图
    loadBlock_bit(file);

    fclose(file);
}
/*
 加载superblock函数
 注意要添加一些变量在结尾，所以使用memory族的结构体
 */
void loadSuperBlock(FILE * file){
    printf("开始加载superBlock\n");

    if (ftell(file)!=0) {
        rewind(file);
    }
    if (fread(&superBlock_memory, SUPER_STRUCT_SIZE, 1, file)<1) {
        printf("fread error\n");
    }
    //写入部分数据
    printf("superBlock加载完毕\n");
}
/*
 加载inodeTable
 */
void loadInodeTable(FILE * file){
    printf("开始加载inodeTable\n");
    //通过superBlock拿到inodeTable首地址:(以字节为单位)
    uint32_t inodeTable_addr_disk = 3*1024+32;//(superBlock_memory.ext2_super_block.s_imap_first_block+1)*superBlock_memory.ext2_super_block.s_log_block_size+superBlock_memory.ext2_super_block.s_inode_size;
    //inodeSize 以字节为单位
    uint32_t inodeSize = superBlock_memory.ext2_super_block.s_inode_size;
    //printf("%d\n",inodeSize);
    //已使用的inode数量
    uint32_t count = superBlock_memory.ext2_super_block.s_inodes_count-superBlock_memory.ext2_super_block.s_free_inodes_count;
    //printf("%d\n",count);
    //跳转到inodeTable开始处
    if (ftell(file)!=inodeTable_addr_disk) {
        fseek(file, inodeTable_addr_disk, SEEK_SET);
    }
    for (int i=0; i<count; i++) {
        struct inode inode_tmp;
        inode_tmp.i_number = i+1;   /*inode号*/
        if (fread(&inode_tmp, inodeSize, 1, file)<1) {
            printf("fread error\n");
        }
        inodesTable_memory[inode_tmp.i_number]=inode_tmp;
        //inode_tmp.i_count = 0;   /*访问此节点的进程数，暂时为0*/
        //inodesTable_memory[p_inodeTable] = inode_tmp;
        //p_inodeTable+=1;
    }
    /*for(int i=1;i<count+1;i++)
    {
        printf("%d\n",inodesTable_memory[i].ext2_inode.i_block[0]);
    }
     */
    printf("inodeTable加载完毕\n");
}
/*
 加载inode位图
 */
void loadInode_bit(FILE * file){
    printf("开始加载inode位图\n");
    char * buffer;
    buffer=malloc(1024);
    uint32_t imap_addr =2048; //superBlock_memory.ext2_super_block.s_imap_first_block*superBlock_memory.ext2_super_block.s_log_block_size;
    //跳转到imap开始处
    if (ftell(file)!=imap_addr) {
        fseek(file, imap_addr, SEEK_SET);
    }

    if (fread(buffer, sizeof(inodemap), 1, file)<1) {
        printf("fread error\n");
    }
    mymemcopy(inodemap,buffer,1024);

    printf("inode位图加载完成\n");
}
/*
 加载block位图
 */
void loadBlock_bit(FILE * file) {
    printf("开始加载block位图\n");
    uint32_t blockMap_addr = 1024;//superBlock_memory.ext2_super_block.s_zmap_first_block*superBlock_memory.ext2_super_block.s_log_block_size;
    //跳转到zmap开始处
    if (ftell(file)!=blockMap_addr) {
        fseek(file, blockMap_addr, SEEK_SET);
    }
    //这里size只需要64字节大小
    if (fread(blockmap, sizeof(blockmap), 1, file) < 1) {
        printf("fread error\n");
    }
    printf("block位图加载完成\n");
}