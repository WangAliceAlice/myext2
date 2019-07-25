//
// Created by wanga on 2019/7/7.
//

#ifndef MYEXT2_SUPER_H
#define MYEXT2_SUPER_H

#include "../include/head.h"
extern int get_super_block_data(struct ext2_super_block *super_block);//从磁盘中读取超级快结构存储到super_block中
extern int set_super_block_data(struct ext2_super_block * super_block);//把super_block设置到磁盘中
#endif //MYEXT2_SUPER_H
