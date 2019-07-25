//
// Created by wanga on 2019/7/7.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/head.h"
#include "init/inittask.h"
#include "operations/namei.h"
#include "init/initdisk.h"
#include "init/initmemory.h"
#include "data/fileoperations.h"
#include "operations/bitmap.h"
#include "data/diskoperations.h"
#include "operations/inode.h"
#include "operations/super.h"
//extern struct inode root;

int main(int argc,char *agrv[]){
    init();
    loadFileSysFromDisk();
    inittask();
    printf("--------------------------------------------------------------------------------------------\n");
    printf("   1 ls 2 open file 3 write file 4 read file  5 close file   6 create file 7 quit \n");
    printf("--------------------------------------------------------------------------------------------\n");
    int num;
    //char com[1000];
    char * com;
    com=malloc(247);
    int fd=-1;
    while (1){
        scanf ("%d",&num);
        if (num==1){
            list_all(fs_struct.pwd);
        }
        else if (num==2){
            printf("输入文件路径：");
            scanf ("%s",com);
            fd=openfile(com);
        }

        else if (num==3){
            if (fd==-1){
                printf("文件尚未打开\n");
                continue;
            }
            printf("输入要写入的数据：");
            scanf ("%s",com);
            writefile(com,fd);
        }
        else if(num==4){
            if (fd==-1){
                printf("文件尚未打开\n");
                continue;
            }
            readfile(fd);
        }
        else if(num == 5){
            fd =closefile(fd);

        }

        else if(num==6)
        {
            printf("输入文件名：");
            scanf("%s",com);
            createfile(com);
        }
        else if(num==7){
            return 0;
        }
        else {
            printf("此数字下无操作\n");
        }

    }
}
