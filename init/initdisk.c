//
// Created by wanga on 2019/7/6.
//

#include "initdisk.h"
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <malloc.h>
#include "../include/head.h"
void mymemcopy(char*des,char *sor,int count)
{
    for(int i=0;i<count;i++)
    {
        des[0]=sor[0];
        des++;
        sor++;
    }

}
//初始化各个block
void init(){
    //打开文件流：
    FILE * file = NULL;
    if ((file = fopen("/mnt/c/Users/wanga/CLionProjects/myext2/diskfile", "w+"))==NULL) {
        printf("fopen异常\n");
    }
    setsuperblock(file);
    setblockbit(file);
    setinodebit(file);
    setdata(file);
}
//初始化superBlock
void setsuperblock(FILE * file){
    printf("初始化superblock:\n");
    struct ext2_super_block superblock;
    memset(&superblock,'\0',sizeof(struct ext2_super_block));
    superblock.s_inode_size=32;//i节点大小
    superblock.s_inodes_count=32;//inode节点总数
    superblock.s_blocks_count=36;//1kb一个block
    superblock.s_r_blocks_count=16;//为超级用户保留块数
    superblock.s_free_blocks_count=13;//空闲block
    superblock.s_free_inodes_count=29;//空闲inode
    superblock.s_first_data_block=20;//第一个block
    superblock.s_log_block_size=1024;//block的大小 1024B
    superblock.s_imap_first_block = 2;  //第一个inode位图块块号
    superblock.s_zmap_first_block = 1;  //第一个block位图块块号
    char buf[SUPER_STRUCT_SIZE] = "";
    memcpy(buf, &superblock, sizeof(superblock));
    //写superblock到模拟硬盘文件中
    if (ftell(file)!=0) {
        rewind(file);//这里保证每次superblock都在开头
    }
    if (fwrite(&buf, BLOCK_SIZE, 1, file)==-1) {
        printf("fwrite异常！\n");
    }
    printf("superblock硬盘中初始化成功\n");
}

//初始化block位图
void setblockbit(FILE * file){
    printf("初始化blockBit:\n");
    if (ftell(file)!=BLOCK_SIZE) {
        fseek(file, BLOCK_SIZE, SEEK_SET);//移动指针到1k处，写入数据
    }
    char buff[BLOCK_SIZE];
    memset(buff, '0', BLOCK_SIZE);   //全部设置为0
    //设置前22位为1

    for(int i=1;i<23;i++)
    {
        buff[i]='1';
    }
    if (fwrite(&buff, BLOCK_SIZE, 1, file)==-1) {
        printf("fwrite异常！\n");
    }
    printf("blockBit硬盘中初始化成功\n");

}
//初始化inode位图
void setinodebit(FILE * file){
    printf("初始化inodeBit:\n");
    if (ftell(file)!=BLOCK_SIZE*2) {
        fseek(file, 2*BLOCK_SIZE, SEEK_SET);//移动指针到2k处，写入数据
    }
    //置空该区域
    char buff[BLOCK_SIZE];
    memset(buff, '0', sizeof(buff));
    //设置 前3个已使用
    buff[1]='1';
    buff[2]='1';
    buff[3]='1';
    //写入
    if (fwrite(&buff, BLOCK_SIZE, 1, file)==-1) {
        printf("fwrite异常！\n");
    }
    printf("inodeBit硬盘中初始化成功\n");

}

//初始化inodeTble
void setinodetable(FILE * file,int num[]){
    printf("初始化inodeTable区域:\n");
    if (ftell(file)!=BLOCK_SIZE*3+INODE_STRUCT_SIZE) {
        fseek(file, 3*BLOCK_SIZE+INODE_STRUCT_SIZE, SEEK_SET);//移动指针到3k+32处，写入数据
    }
    //置空该区域
    char buff[BLOCK_SIZE];
    memset(buff, 0, sizeof(buff));
    /*需要初始化三个inode*/
    //1.root目录文件
    struct ext2_inode inode;
    inode.i_mode = (Contents_File << 12)+1;  //高4位表示文件类型，所以左移12位,级别1
    inode.i_size = num[0];   //注意：以字节记！！！！
    inode.i_blocks = 1;   //文件所占block数量
    inode.i_block[0] = 20;   //root块号暂定为35，即非保留区的第1个数据块
//    char buff_inode[128] = "";
//    memcpy(buff_inode, &inode, sizeof(inode));
    memcpy(buff, &inode, 32);

    //2.bin目录文件
    struct ext2_inode inode1;
    inode1.i_mode = (Contents_File << 12)+1;  //高4位表示文件类型，所以左移12位，级别1
    inode1.i_size = num[1];   //注意：以字节记！！！！
    inode1.i_blocks = 1;   //文件所占block数量
    inode1.i_block[0] = 21;   //bin块号暂定为36，即非保留区的第2个数据块
//    char buff_inode1[128] = "";
//    memcpy(buff_inode1, &inode1, sizeof(inode1));
    memcpy(buff+32, &inode1, 32);
    //3.test.c 普通文件
    struct ext2_inode inode2;
    inode2.i_mode = (Ordinary_File << 12)+3;  //高4位表示文件类型，所以左移12位,级别3
    inode2.i_size = num[2];   //注意：以字节记！！！！
    inode2.i_blocks = 1;   //文件所占block数量
    inode2.i_block[0] = 22;   //bin块号暂定为37，即非保留区的第3个数据块
//    char buff_inode2[128] = "";
//    memcpy(buff_inode2, &inode2, sizeof(inode2));
    memcpy(buff+64, &inode2, 32);

    if (fwrite(&buff, BLOCK_SIZE, 1, file)==-1) {
        printf("fwrite异常！\n");
    }
    printf("inodeTable区域硬盘中初始化成功\n");

}
//初始化数据
void setdata(FILE * file){
    if (ftell(file)!=BLOCK_SIZE*20) {
        fseek(file, 20*BLOCK_SIZE, SEEK_SET);//移动指针到20k处，写入数据
    }
    /*
     root文件内容设置
     */
    //char buff[BLOCK_SIZE*3];  //root目录文件总大小 1K
    char *buff1,*buff2,*buff3,* buff4,*buff5,*buff6,*buff7,*buff8,*buff9;
    buff1=malloc(256);
    //1.写入"."
    struct ext2_dir_entry_2 dentry_this;
    dentry_this.inode = 1;   //inode号 root文件inode 即自己
    char this[247] = ".";
    memcpy(dentry_this.name, this, 247);   //文件名字
    //printf("%s\n",dentry_this.name);
    dentry_this.name_len = 1;   //文件名字长度
    dentry_this.file_type = (uint8_t)Contents_File;   //文件类型
    dentry_this.rec_len = 4;   //目录数 因为指向自己
    /*mymemcopy(buff1,&dentry_this,256);
    struct ext2_dir_entry_2 *www;
    www=(struct ext2_dir_entry_2*)buff1;
    printf("%d\n",www->file_type);
     */

    //printf("%d\n", sizeof(dentry_this));
   // memcpy(file,dentry_this.name, 247);
    //fseek(file, 20*BLOCK_SIZE, SEEK_SET);
   //printf("%d\n", sizeof(file));
   //char *a;
   //a=malloc(256);
   //int count=fread(&a,247,1,&file);
    //printf("%d\n",count);
    mymemcopy(buff1, &dentry_this, DIR_ENTRY_SIZE);//写入buff
    if (fwrite(buff1, 256,1, file)==-1) {
        printf("fwrite异常！\n");
    }
    fflush(file);

    //".."目录项
    if (ftell(file)!=BLOCK_SIZE*20+256) {
        fseek(file, 20*BLOCK_SIZE+256, SEEK_SET);//移动指针到20k处，写入数据
    }
    buff2=malloc(256);
    struct ext2_dir_entry_2 dentry_father;
    dentry_father.inode = 1;   //inode号 root文件inode 即自己
    char father[247] = "..";
    memcpy(dentry_father.name, father, 247);   //文件名字
    dentry_father.name_len = 2;   //文件名字长度
    dentry_father.file_type = (uint8_t)Contents_File;   //文件类型
    dentry_father.rec_len = 4;   //目录数 因为是根目录，也指向自己
    //memcpy(buff+DIR_ENTRY_SIZE, &dentry_father, DIR_ENTRY_SIZE);//写入buff
    mymemcopy(buff2, &dentry_father, DIR_ENTRY_SIZE);//写入buff
    if (fwrite(buff2, 256,1, file)==-1) {
        printf("fwrite异常！\n");
    }
    fflush(file);
    //2.写入bin目录项
    if (ftell(file)!=BLOCK_SIZE*20+512) {
        fseek(file, 20*BLOCK_SIZE+512, SEEK_SET);//移动指针到20k处，写入数据
    }
    buff3=malloc(256);
    struct ext2_dir_entry_2 dentry_bin;
    dentry_bin.inode = 2;   //inode号
    char bin[247] = "bin";
    memcpy(dentry_bin.name, bin, 247);   //文件名字
    dentry_bin.name_len = 3;   //文件名字长度
    dentry_bin.file_type = (uint8_t)Contents_File;   //文件类型
    dentry_bin.rec_len = 2;   //目录数
    //memcpy(buff+DIR_ENTRY_SIZE*2, &dentry_bin, DIR_ENTRY_SIZE);//写入buff
    mymemcopy(buff3, &dentry_bin, DIR_ENTRY_SIZE);//写入buff
    if (fwrite(buff3, 256,1, file)==-1) {
        printf("fwrite异常！\n");
    }
    fflush(file);
    //3.test文件目录项
    if (ftell(file)!=BLOCK_SIZE*20+768) {
        fseek(file, 20*BLOCK_SIZE+768, SEEK_SET);//移动指针到20k处，写入数据
    }
    buff4=malloc(256);
    struct ext2_dir_entry_2 dentry_test;
    dentry_test.inode = 3;   //inode号
    char test[247] = "test";
    memcpy(dentry_test.name, test, 247);   //文件名字
    dentry_test.name_len = 6;   //文件名字长度
    dentry_test.file_type = (uint8_t)Ordinary_File;   //文件类型
    dentry_test.rec_len = 0;   //目录数
    //memcpy(buff+DIR_ENTRY_SIZE*3, &dentry_test, DIR_ENTRY_SIZE);//写入buff
    mymemcopy(buff4, &dentry_test, DIR_ENTRY_SIZE);//写入buff
    if (fwrite(buff4, 256,1, file)==-1) {
        printf("fwrite异常！\n");
    }
    fflush(file);
    /*
     bin 内容设置
     */
    //char buff_bin[BLOCK_SIZE];
    //1.写入"."
    if (ftell(file)!=BLOCK_SIZE*20+1024) {
        fseek(file, 20*BLOCK_SIZE+1024, SEEK_SET);//移动指针到20k处，写入数据
    }
    buff5=malloc(256);
    struct ext2_dir_entry_2 dentry_bin_this;
    dentry_bin_this.inode = 2;   //inode号
    char bin_this[247] = ".";
    memcpy(dentry_bin_this.name, bin_this, 247);   //文件名字
    dentry_bin_this.name_len = 1;   //文件名字长度
    dentry_bin_this.file_type = (uint8_t)Contents_File;   //文件类型
    dentry_bin_this.rec_len = 3;   //目录数 因为指向自己
    mymemcopy(buff5, &dentry_bin_this, DIR_ENTRY_SIZE);//写入buff_bin
    if (fwrite(buff5, 256,1, file)==-1) {
        printf("fwrite异常！\n");
    }
    fflush(file);
    //".."目录项
    if (ftell(file)!=BLOCK_SIZE*20+1280) {
        fseek(file, 20*BLOCK_SIZE+1280, SEEK_SET);//移动指针到20k处，写入数据
    }
    buff6=malloc(256);
    struct ext2_dir_entry_2 dentry_bin_father;
    dentry_bin_father.inode = 1;   //inode号
    char bin_father[247] = "..";
    memcpy(dentry_bin_father.name, bin_father, 247);   //文件名字
    dentry_bin_father.name_len = 2;   //文件名字长度
    dentry_bin_father.file_type = (uint8_t)Contents_File;   //文件类型
    dentry_bin_father.rec_len = 4;   //目录数
    //memcpy(buff_bin+DIR_ENTRY_SIZE, &dentry_bin_father, DIR_ENTRY_SIZE);//写入buff_bin
    //memcpy(buff+BLOCK_SIZE, buff_bin, BLOCK_SIZE);//buff_bin写入buff
    mymemcopy(buff6, &dentry_bin_father, DIR_ENTRY_SIZE);//写入buff_bin
    if (fwrite(buff6, 256,1, file)==-1) {
        printf("fwrite异常！\n");
    }
    fflush(file);
    //memcpy(buff, buff_bin, BLOCK_SIZE);//buff_bin写入buff

    //test目录项
    if (ftell(file)!=BLOCK_SIZE*20+1536) {
        fseek(file, 20*BLOCK_SIZE+1536, SEEK_SET);//移动指针到20k处，写入数据
    }
    buff8=malloc(256);
    struct ext2_dir_entry_2 dentry_bin_test;
    dentry_bin_test.inode = 3;   //inode号
    char bin_test[247] = "test";
    memcpy(dentry_bin_test.name, bin_test, 247);   //文件名字
    dentry_bin_test.name_len = 1;   //文件名字长度
    dentry_bin_test.file_type = (uint8_t)Ordinary_File;   //文件类型
    dentry_bin_test.rec_len = 0;   //目录数 因为指向自己
    mymemcopy(buff8, &dentry_bin_test, DIR_ENTRY_SIZE);//写入buff_bin
    if (fwrite(buff8, 256,1, file)==-1) {
        printf("fwrite异常！\n");
    }
    fflush(file);

    //空的一项
    if (ftell(file)!=BLOCK_SIZE*20+1792) {
        fseek(file, 20*BLOCK_SIZE+1792, SEEK_SET);//移动指针到20k处，写入数据
    }
    buff9=malloc(256);
    struct ext2_dir_entry_2 dentry_bin_next;
    dentry_bin_next.inode = 0;   //inode号
    mymemcopy(buff9, &dentry_bin_next, DIR_ENTRY_SIZE);//写入buff_bin
    if (fwrite(buff9, 256,1, file)==-1) {
        printf("fwrite异常！\n");
    }
    fflush(file);

    /*
     hello.c 内容设置
     */
    if (ftell(file)!=BLOCK_SIZE*20+2048) {
        fseek(file, 20*BLOCK_SIZE+2048, SEEK_SET);//移动指针到20k处，写入数据
    }
    //printf("%d\n",ftell(file));
    buff7=malloc(1024);
    //char buff_hello[BLOCK_SIZE];
    char helloFile[1024] = "Hello World!";
    //buff_hello[BLOCK_SIZE-1] = 'H';   //用来debug
    //memcpy(buff7, helloFile, 512);
    //printf("%s\n",helloFile);
    //printf("%s\n",buff_hello);
    //memcpy(buff+BLOCK_SIZE*2, buff_hello, BLOCK_SIZE);//buff_hello写入buff
    mymemcopy(buff7, helloFile, BLOCK_SIZE);//buff_hello写入buff
    //printf("%s\n",buff7);
    //printf("%s\n",buff+BLOCK_SIZE*2);
    //printf("%s\n",buff);
    /*struct ext2_dir_entry_2 *www;
    www=(struct ext2_dir_entry_2*)buff7;
    printf("%d\n",www->file_type);
     */
     //printf("%s\n",buff7);
    //4.写入硬盘数据区
    if (fwrite(buff7, BLOCK_SIZE,1, file)==-1) {
        printf("fwrite异常！\n");
    }
    fflush(file);
    //fseek(file, 22*BLOCK_SIZE, SEEK_SET);
    //char a[1024];
    //fread(a,1024,1,file);
    //printf("%s\n",a);
    /*memcpy(file,buff7,strlen(buff7)+1);
    printf("%s\n",buff7);
    fseek(file, 20*BLOCK_SIZE+2048, SEEK_SET);
    char *a;
    a=malloc(1025);
    printf("%d\n",strlen(file));
    memcpy(a,file,strlen(file)+1);
    printf("%s\n",a);
    /*调用写入inodeTable函数*/
    int num[3] = {BLOCK_SIZE,BLOCK_SIZE,BLOCK_SIZE};
    setinodetable(file,num);
    //printf("%d\n",ftell(file));
    /*fseek(file, 20*BLOCK_SIZE+2048, SEEK_SET);
    char *a;
    a=malloc(1024);
    int n=fread(&a,1024,1,file);
    printf("内容是%s\n",a);
    */

    //lseek(file,4*1024,SEEK_SET);
    //char a [2048];
    //int count=read(a,file,2048);
    //printf("%d\n",count);
    //printf("%s\n",a);


}