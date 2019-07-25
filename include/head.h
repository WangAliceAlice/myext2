//
// Created by wanga on 2019/7/6.
//

#ifndef MYEXT2_HEAD_H
#define MYEXT2_HEAD_H
#define  __u32  unsigned int
#define  __u16  unsigned short
#define  __u8   unsigned char
#define  __s32  unsigned int
#define  __s16  unsigned short
#define  __s8   unsigned char
#define  __le32 unsigned int
#define  __le16 unsigned short
#define BLOCK_SIZE 1024
#define DIR_NAME_SIZE 247

#define SUPER_STRUCT_SIZE 1024
/* 硬盘超级块结构 */
struct  ext2_super_block{
    __u32   s_inode_size;/*i节点大小*/
    __u32	s_inodes_count;	/* 文件系统中索引节点总数 */
    __u32	s_blocks_count;	/*文件系统中总块数 */
    __u32	s_r_blocks_count;	  /* 为超级用户保留的块数 */
    __u32	s_free_blocks_count;	/*文件系统中空闲块总数 */
    __u32	s_free_inodes_count;	/*文件系统中空闲索引节点总数*/
    __u32	s_first_data_block;	/* 文件系统中第一个数据块 */
    __u32	s_log_block_size;	 /* 用于计算逻辑块大小 */
    __u8    s_imap_first_block;  /*第一个inode位图块号*/
    __u8    s_zmap_first_block;  /*第一个块位图块号*/
};

/*内存超级块结构*/
struct super_block{
    struct ext2_super_block ext2_super_block
};

#define INODE_STRUCT_SIZE 32
/*硬盘inode数据结构*/
#define Ext2_N_BLOCKS 15
struct ext2_inode {
    __u16	i_mode; /* 文件类型和访问权限 */
    __u32	i_size; /* 以字节计的文件大小 */
    __u32	i_blocks; /* 文件所占块数（每块以1024字节计）,一个块大小为1kb */
    __u32	i_block[Ext2_N_BLOCKS];  /* 指向数据块的指针数组 */
};

/*内存inode数据结构*/
struct inode{
    struct ext2_inode ext2_inode;
    __u32 i_number; /* i节点号*/
    __u32 i_count;  /*引用计数*/
};

/*目录文件结构*/
#define DIR_ENTRY_SIZE 256
struct ext2_dir_entry_2 {
    __le32 inode; // 文件入口的inode号，0表示该项未使用
    __u8 file_type; // 文件类型
    char name[247]; // 文件名
    __le16 rec_len; // 目录项长度
    __u8 name_len; // 文件名包含的字符数
};

/*文件数据结构*/
struct file {
    unsigned int f_pos;  /* 文件的当前读写位置 */
    struct inode * f_inode;  /* 指向文件对应的inode */
};

/* 进程控制块 */
struct task_struct {
    int pid;
    struct fs_struct *fs;
    struct files_struct *files;//进程描述符表
};
//记录文件描述符的使用情况
#define NR_OPEN 256
struct files_struct {
    int count ; //使用该表的进程数
    struct file *fd[NR_OPEN];//用户打开文件描述符表
    int next_fd;
};
/*记录文件系统根目录和当前目录*/
struct fs_struct {
    int count;    // 共享此结构的计数值
    unsigned short umask;  // 文件掩码
    struct inode * root, * pwd;  //确定根目录和当前目录inode指针
};

enum FileType{
    Ordinary_File = 0,   //普通文件
    Contents_File = 255,   //目录文件
    Block_File = 2,   //块设备
    ASCII_File = 3,   //字符设备
    Socket_File = 4,   //套接口
    Symbol_File = 5,   //符号连接
    FIFO_File = 6   //FIFO/管道文件
};


#endif //MYEXT2_HEAD_H
