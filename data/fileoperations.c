//
// Created by wanga on 2019/7/7.
//

#include "fileoperations.h"
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "../include/head.h"
#include "../operations/namei.h"
#include "../operations/bitmap.h"
#include "diskoperations.h"
#include "../operations/inode.h"
#include "../init/inittask.h"
#include "../init/initmemory.h"
#include "../operations/namei.h"
#include "../init/initdisk.h"


int openfile(char *path)
{
    struct inode *dir =&inodesTable_memory[1];
    char buffer[1024];
    struct ext2_dir_entry_2 *dentry ;
    int num_dir_blocks = dir->ext2_inode.i_blocks;
    int i=0;
    get_block_data(dir->ext2_inode.i_block[i],buffer);
    dentry = (struct ext2_dir_entry_2 *)buffer;
    if( (strcmp(path, "/") == 0 || strcmp(path, "/root/") == 0 || strcmp(path, "/root") == 0))
    {
        files_struct.count++;
        sys_open[1].f_inode=&inodesTable_memory[1];
        sys_open[1].f_pos=0;
        *(files_struct.fd)=&sys_open[1];
        printf("%s",dentry->name);
        printf("has been opened\n");
        return 1;
    }
    else if(path[0]=='/')
    {
        int i=0,count=0;
        for(i=0;i<247;i++)
        {
            if(path[i]=='/')
            {
                count++;
            }
        }
        if(count==2)
        {
            int j=0;
            for(;j<2;j++,dentry++){
                if (dentry->inode ==0) continue;
            }
            sys_open[2].f_inode=&inodesTable_memory[2];
            sys_open[2].f_pos=0;
            *(files_struct.fd)=&sys_open[2];
            printf("%s ",dentry->name);
            printf("has been opened\n");
            return 2;
        }
        else if(count==3)
        {
            int j=0;
            for(;j<3;j++,dentry++){
                if (dentry->inode ==0) continue;
            }
            sys_open[3].f_inode=&inodesTable_memory[3];
            sys_open[3].f_pos=0;
            *(files_struct.fd)=&sys_open[3];
            printf("%s ",dentry->name);
            printf("has been opened\n");
            return 3;
        }
    }
    else if(path[0]=='.'&&path[1]!='.')
    {
        //get_block_data()
    }
    return 0;

}
int readfile(int number){
    struct inode * inode;
    char *s;
    s=malloc(1024);
    inode=getInodeByNum(number);
    get_block_data(inode->ext2_inode.i_block[0],s);
    struct ext2_dir_entry_2 *dentry ;
    dentry = (struct ext2_dir_entry_2 *)s;
    if(dentry->file_type==255)
    {
        printf("目录文件\n");
    }
    else
    {
        printf("%s\n",s);
    }

}

int writefile(char *s,int number)
{
    struct inode * inode;
    inode=getInodeByNum(number);
    set_block_data(inode->ext2_inode.i_block[0],s);
}

int closefile(int fd){
    files_struct.count--;
    fd=-1;
    printf("文件已关闭\n");
    return fd;
}

int createfile(char * filename)
{
    char buffer[1024];
    char buffer1[1024];
    char buffer2[32];
    int flag,inodenum,blocknum;
    struct ext2_dir_entry_2 *dentry ;
    flag=checkexist(filename);
    if(flag ==1)
    {
        inodenum=getfreeinode();
        blocknum=getfreeblock();
        get_block_data(fs_struct.pwd->ext2_inode.i_block[0],buffer);
        dentry = (struct ext2_dir_entry_2 *)buffer;
        dentry->rec_len=4;
        mymemcopy(buffer1,dentry,256);
        dentry++;
        mymemcopy(buffer1+256,dentry,256);
        dentry++;
        mymemcopy(buffer1+512,dentry,256);
        dentry=dentry++;
        dentry->inode=inodenum;
        dentry->file_type=0;
        mymemcopy(dentry->name,filename,247);
        dentry->name_len=strlen(filename);
        dentry->rec_len=0;
        mymemcopy(buffer1+768,dentry,256);
        /*int j=0;
        for(;j<4;j++,dentry++){
            if(dentry->inode==2)
            {
                dentry->rec_len=4;
                mymemcopy(buffer1,dentry,256);
            }
            else if (dentry->inode ==0)
            {
                dentry->inode=inodenum;
                dentry->file_type=0;
                mymemcopy(dentry->name,filename,247);
                dentry->name_len=strlen(filename);
                dentry->rec_len=0;
                mymemcopy(buffer1,dentry,256);

            }
            else
            {
                mymemcopy(buffer1,dentry,256);
            }

        }
         */

        set_block_data(fs_struct.pwd->ext2_inode.i_block[0],buffer1);

        inodesTable_memory[inodenum].ext2_inode.i_block[0]=blocknum;
        inodesTable_memory[inodenum].ext2_inode.i_blocks=1;
        inodesTable_memory[inodenum].ext2_inode.i_size=32;
        inodesTable_memory[inodenum].ext2_inode.i_mode=0;
        mymemcopy(buffer2,&inodesTable_memory[inodenum],32);
        set_inode_data(inodenum,buffer2);
        printf("create file successfully\n");
    }

}

int getfreeblock()
{
    int blocknum;
    for(int i=1;i<1024;i++)
    {
        if(blockmap[i]=='0')
        {
            blocknum=i;
            printf("块号为：");
            printf("%d\n",blocknum);

            return blocknum;
        }

    }
}

int getfreeinode()
{
    int inodenum;
    for(int i=1;i<1024;i++)
    {
        if(inodemap[i]=='0')
        {
            inodenum=i;
            printf("inode号为：");
            printf("%d\n",inodenum);
            return inodenum;
        }

    }
}

int checkexist(char *name)
{
    char buffer[1024];
    struct inode *dir =fs_struct.pwd;
    struct ext2_dir_entry_2 *dentry ;
    int i=0;
    get_block_data(dir->ext2_inode.i_block[i],buffer);
    dentry = (struct ext2_dir_entry_2 *)buffer;
    int j=0;
    for(;j<4;j++,dentry++){
        if (dentry->inode ==0) continue;
        if(dentry->name==name)
        {
            printf("this file already exists\n");
            return 0;
        }
    }
    return 1;
}






const int  find_position(__u32 f_pos,__u32 *num,__u32 *offset){
    *num=f_pos/BLOCK_SIZE;
    *offset=f_pos%BLOCK_SIZE;
    return 0;
}





int u_lseek(struct inode* l_inode,struct file* l_file,int offset,int flag){
    if(!l_file || !l_inode) return -1;
    if (flag==0){
        l_file->f_pos=offset;
    }
    if (flag==1){
        l_file->f_pos+=offset;
    }
    if (flag==2){
        l_file->f_pos=l_inode->ext2_inode.i_size+offset;
    }
    return l_file->f_pos;
}

int sys_mkdir (char *name,int len,int mode){
    return u_mkdir(fs_struct.pwd,name,len,mode);
}