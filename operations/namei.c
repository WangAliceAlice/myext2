//
// Created by wanga on 2019/7/7.
//

#include "namei.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/head.h"
#include "../data/diskoperations.h"
#include "inode.h"
#include "bitmap.h"

int check_name(char *name);
int check_exist(struct inode *dir,char *name);
struct inode* name_find_inode(char *path,struct inode* n_inode){
    printf("path == %s\n",path);
    __u32 pwd_inodenum;
    char temp_path[1024]="\0";
    int i=0;
    if (*path=='/'){ //根节点
        int len=strlen(path);
        int len_temp=0;
        int temp_i=0;
        int flag=0;
        for (i=0;i<len;i++){
            if (flag==1){
                temp_path[len_temp++]=path[i];
            }
            if (path[i]=='/'){
                flag=1;
            }

        }
        pwd_inodenum = fs_struct.pwd->i_number;
        printf("pwd_inode=%d\n",pwd_inodenum);
        while (temp_path != NULL){
            printf("temp_path==%s\n",temp_path);
            pwd_inodenum = namefind_inode(temp_path,pwd_inodenum);
            if (pwd_inodenum==-1){
                printf("沒有這樣的文件或目錄\n");
                return NULL;
            }
        }
        get_inode_data(pwd_inodenum,n_inode);
        return n_inode;
    }else {
        strcpy(temp_path,path);
        printf("temp_path = %s\n",temp_path);
        pwd_inodenum = fs_struct.pwd->i_number;
        pwd_inodenum = namefind_inode(temp_path,pwd_inodenum);
        printf("pwd_inodenum = %d\n",pwd_inodenum);
        if (pwd_inodenum == -1){
            printf("没有这样的目录\n");
            return NULL;
        }
        char buffer[128];
        get_inode_data(pwd_inodenum,buffer);
        n_inode = (struct inode *)buffer;
        return n_inode;
        //printf("n_inode. blocks = %d\n",n_inode->ext2_inode.i_blocks);
        //printf("n_inode. block[0] = %d\n",n_inode->ext2_inode.i_block[0]);
    }
}

//获取目录的顺序：首先读出目录文件的内容，由于目录文件在磁盘中的存储方式为ext2_dir_entry_2，所以在通过解析读出来的数据，一个一个的域path
//进行比较，判断在当前目录中是否存在这样的目录文件。
int namefind_inode(char *path ,__u32 pwd_inodenum){
    char buf[128];
    struct inode * cur_inode;
    cur_inode =(struct inode *)buf;

    get_inode_data(pwd_inodenum,buf);
    char buffer[1024];
    struct ext2_dir_entry_2 *dentry;
    int num_dir_blocks = cur_inode->ext2_inode.i_blocks;
    //printf("num_dir_blocks = %d\n",num_dir_blocks);
    int i=0;
    for (;i<num_dir_blocks;i++){
        get_block_data(cur_inode->ext2_inode.i_block[i],buffer);
        //printf("num_dir_block[%d] = %d\n",i,cur_inode->ext2_inode.i_block[i]);
        dentry = (struct ext2_dir_entry_2 *) buffer;
        int j=0;
        int num_of_dir = 1024/sizeof(struct ext2_dir_entry_2);
        for (;j<num_of_dir;j++){
            if (dentry->inode!=0 && strcmp(&(dentry->name[0]),path)==0){
                printf("dentry->inode = %d\n",dentry->inode);
                return dentry->inode;
            }
            dentry++;
        }
    }
    return -1;
}

int check_name(char *name){
    char *p;
    p = name;
    while (*p != '\0'){
        if (*p == '/' || *p == '?' || *p == '*' || *p==' ' || *p=='$' || *p=='@' ) return -1;
        p++;
    }
    return 1;
}

int check_exist(struct inode *dir,char *name){
    char buffer[1024];
    struct ext2_dir_entry_2 *dentry;
    int num_dir_blocks = dir->ext2_inode.i_blocks;
    //printf("num_dir_blocks = %d  name = %s \n ",num_dir_blocks,name);
    int i=0;
    for (;i<num_dir_blocks;i++){
        //printf("i_block[%d] : %d\n",i,dir->ext2_inode.i_block[i]);
        get_block_data(dir->ext2_inode.i_block[i],buffer);
        dentry = (struct ext2_dir_entry_2 *) buffer;
        int j=0;
        int num_of_dir = 1024/sizeof(struct ext2_dir_entry_2);
        for (;j<num_of_dir;j++){
            //printf("dentry->inode == %d  j ==%d  block[%d]==%d \n",dentry->inode,j,i,dir->ext2_inode.i_block[i]);
            if (dentry->inode!=0 && strcmp(dentry->name,name)==0){
                printf("inode = %d  %s \n",dentry->inode,dentry->name);
                return 1;
            }
            dentry++;
        }
    }
    return -1;
}
//在dir目录下创建一个叫name的文件
int create(struct inode *dir,char *name,int len,int mode){
    if (check_name(name) == -1){
        printf("文件名不合法，请重新输入\n");
        return -1;
    }

    if (check_exist(dir,name)==1){
        printf("文件已存在\n");
        return -1;
    }
    mode = 0x0666;
    char buffer[1024];
    struct ext2_dir_entry_2 *dentry;
    int num_dir_blocks = dir->ext2_inode.i_blocks;
    int i=0;
    for (;i<num_dir_blocks;i++){
        get_block_data(dir->ext2_inode.i_block[i],buffer);
        dentry = (struct ext2_dir_entry_2*) buffer;
        int num_of_dir = 1024/sizeof(struct ext2_dir_entry_2);
        int j=0;
        for (;j<num_of_dir;j++){
            if (dentry->inode ==0 ){
                dentry->inode=new_inode();
                //printf("dentry->inode == %d  j ==%d  block[%d]==%d name = %s \n",dentry->inode,j,i,dir->ext2_inode.i_block[i],name);
                dentry->rec_len = len;
                dentry->name_len = len;
                dentry->file_type = mode;
                strcpy(dentry->name,name);
                //对dentry的更新就是对buffer的更新，所以最后再把buffer写入到磁盘之中将可以
                set_block_data(dir->ext2_inode.i_block[i],buffer);
                return 1;
            }
            dentry ++;
        }
    }

    if (num_dir_blocks >=13) {
        printf("磁盘已满\n");
        return -1;
    }
    memset(buffer,'\0',sizeof(buffer));

    dir->ext2_inode.i_blocks++;
    num_dir_blocks = dir->ext2_inode.i_blocks;
    int temp_block_num = new_block();
    dir->ext2_inode.i_block[num_dir_blocks-1] = temp_block_num;
    char buf[128];
    struct inode* buf_inode;
    buf_inode = (struct inode *)buf;
    buf_inode->i_number = dir->i_number;
    buf_inode->i_count = dir->i_count;
    buf_inode->ext2_inode.i_mode = dir->ext2_inode.i_mode;
    buf_inode->ext2_inode.i_blocks = dir->ext2_inode.i_blocks;
    for (i=0;i<buf_inode->ext2_inode.i_blocks;i++)
        buf_inode->ext2_inode.i_block[i] = dir->ext2_inode.i_block[i];
    int num = set_inode_data(dir->i_number,buf);
    //printf("num == %d\n",num);

    dentry = (struct ext2_dir_entry_2 *)buffer;
    dentry->inode=new_inode();
    //printf("dentry->inode == %d\n",dentry->inode);
    dentry->rec_len = len;
    dentry->name_len = len;
    //mode &= (0<<12);
    dentry->file_type = mode;
    strcpy(dentry->name,name);
    set_block_data(dir->ext2_inode.i_block[temp_block_num],buffer);
    return 1;
}
//在dir该目录下创建一个叫name的新的目录
int u_mkdir(struct inode* dir,char *name,int len,int mode){
    if (check_name(name)==-1){
        printf("目录文件名不合法\n");
        return -1;
    }
    if (check_exist(dir,name)==1){
        printf("该目录已存在\n");
        return -1;
    }
    //printf("begin\n");
    char buffer[1024];//读数据
    //mode = 0x0666;
    mode=Contents_File;
    struct inode* m_inode;
    int num;
    char buf[128];//写数据
    m_inode = (struct inode *)buf;
    struct ext2_dir_entry_2 *dentry;
    int num_dir_blocks = dir->ext2_inode.i_blocks;
    //printf("num_dir_blocks :%d\n",num_dir_blocks);
    int i=0;
    for(;i<num_dir_blocks;i++){
        //printf("dir -> ext2_inode.i_block[%d] = %d\n",i,dir->ext2_inode.i_block[i]);
        get_block_data(dir->ext2_inode.i_block[i],buffer);
        dentry = (struct ext2_dir_entry_2 *)buffer;
        int num_of_dir=1024/sizeof(struct ext2_dir_entry_2);
        //printf("num_od_dir :%d\n",num_of_dir);
        int j=0;
        for (;j<num_of_dir;j++){
            //printf("dentry   inode==%d\n",dentry->inode);
            if (dentry->inode==0){
                dentry->inode=new_inode();
                //printf("dir  dentry->inode=%d  j ==%d \n ",dentry->inode,j);
                dentry->rec_len = len;
                dentry->name_len = len;
                mode |= (1<<12);
                dentry->file_type = mode;
                strcpy(dentry->name,name);
                set_block_data(dir->ext2_inode.i_block[i],buffer);
                //在dir目录下填写新建的目录信息，还需要给新的目录项分配数据块，用来存放子目录下的文件信息
                m_inode = (struct inode *)malloc(sizeof(struct inode));
                m_inode->ext2_inode.i_blocks=1;
                m_inode->ext2_inode.i_block[0]=new_block();
                num=set_inode_data(dentry->inode,buf);
                //printf("num == %d\n",num);
                free(m_inode);
                return 1;
            }
            dentry++;
        }
    }
    if (num_dir_blocks >=13) {
        printf("磁盘已满\n");
        return -1;
    }
    memset(buffer,'\0',sizeof(buffer));
    memset(buffer,'\0',sizeof(buf));

    dir->ext2_inode.i_blocks++;
    num_dir_blocks = dir->ext2_inode.i_blocks;
    int temp_block_num = new_block();
    //printf("temp_blcok_num ==%d  num_die_blocks == %d\n",temp_block_num,num_dir_blocks);
    dir->ext2_inode.i_block[num_dir_blocks-1] = temp_block_num;
    char buf2[128];
    struct inode* buf_inode;
    buf_inode = (struct inode *)buf2;
    buf_inode->i_number = dir->i_number;
    buf_inode->i_count = dir->i_count;
    buf_inode->ext2_inode.i_mode = dir->ext2_inode.i_mode;
    buf_inode->ext2_inode.i_blocks = dir->ext2_inode.i_blocks;
    for (i=0;i<num_dir_blocks;i++){
        buf_inode->ext2_inode.i_block[i] = dir->ext2_inode.i_block[i];
        //printf("block[%d]==%d\n",i,buf_inode->ext2_inode.i_block[i] );
    }
    num=set_inode_data(dir->i_number,buf2);
    //printf("num == %d  ok\n",num);

    get_block_data(temp_block_num,buffer);
    dentry = (struct ext2_dir_entry_2 *)buffer;
    dentry->inode=new_inode();
    //printf("dentry->inode == %d\n",dentry->inode);
    dentry->rec_len = len;
    dentry->name_len = len;
    mode |= (1<<12);
    dentry->file_type = mode;
    strcpy(dentry->name,name);
    //printf("i_block = == %d\n",dir->ext2_inode.i_block[num_dir_blocks-1]);
    set_block_data(dir->ext2_inode.i_block[num_dir_blocks-1],buffer);

    m_inode = (struct inode *)malloc (sizeof(struct inode));
    m_inode->ext2_inode.i_blocks =1;
    m_inode->ext2_inode.i_block[0]=new_block();
    num=set_inode_data(dentry->inode,buf);
    //printf("num == %d\n",num);
    free(m_inode);
    return 1;
}


int list_all(struct inode *dir){
    //printf("%d\n",dir->i_number);
    char buffer[1024];
    //char * buffer;
    //buffer=malloc(1024);
    struct ext2_dir_entry_2 *dentry ;
    int num_dir_blocks = dir->ext2_inode.i_blocks;
    int i=0;
    for (;i<num_dir_blocks;i++){
        //printf("%d\n",dir->ext2_inode.i_block[0]);
        get_block_data(dir->ext2_inode.i_block[i],buffer);
       // printf("hahahha\n");
        //printf("%s\n",buffer);
        dentry = (struct ext2_dir_entry_2 *)buffer;
        int num_of_dir = 1024/sizeof(struct ext2_dir_entry_2);
        int j=0;
       // printf("%d\n",num_of_dir);
        for(;j<num_of_dir;j++,dentry++){
            if (dentry->inode ==0) continue;
            //int temp_mode = 0x1000;
            int temp_mode;
            temp_mode = dentry->file_type;
            //printf("%d\n",temp_mode);
            //printf("temp_mode :%X  file_ype : %X\n",temp_mode,dentry->file_type);
            //if ((temp_mode == 0x1000)){
            if((temp_mode==Contents_File)){
                printf("%s\n",dentry->name);
                //printf("\033[;34m %s \033[0m\n",dentry->name);
            }
            else
                 //printf(" else\n");

                //printf("\033[;31m %s \033[0m\n",dentry->name);
            printf("  %s\n",dentry->name);
        }
    }
    return 1;
}

int delete_file (struct inode *dir,char *name){
    char buffer[1024];
    struct ext2_dir_entry_2 *dentry;
    int num_dir_blocks = dir->ext2_inode.i_blocks;
    int i=0;
    for (;i<num_dir_blocks;i++){
        get_block_data(dir->ext2_inode.i_block[i],buffer);
        dentry = (struct ext2_dir_entry_2*)buffer;
        int num_of_dir = 1024/sizeof(struct ext2_dir_entry_2);
        int j=0;
        for (;j<num_of_dir;j++){
            if (dentry->inode!=0 && !strcmp(dentry->name,name)){
                if(free_inode(dentry->inode)==-1){
                    printf("free inode error\n");
                    return -1;
                }
                dentry->inode = 0;
                set_block_data(dir->ext2_inode.i_block[i],buffer);
                return 1;
            }
            dentry++;
        }
    }
    printf("没有这样的文件\n");
    return -1;
}
int change_dir(char *name){
    if (name_find_inode(name,fs_struct.pwd)==NULL){
        printf("不存在这样的目录\n");
        return -1;
    }
    return 0;
}