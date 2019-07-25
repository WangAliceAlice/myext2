cmake_minimum_required(VERSION 3.10)
project(myext2)

set(CMAKE_CXX_STANDARD 14)

add_executable(myext2 init/initdisk.h
               init/initdisk.c
               include/head.h
               init/initmemory.h
               init/initmemory.c
               init/inittask.h
               init/inittask.c
        operations/namei.c
        operations/namei.h
        data/diskoperations.c
        data/diskoperations.h
        operations/bitmap.c
        operations/bitmap.h
        operations/super.c
        operations/super.h
        operations/inode.c
        operations/inode.h
               main.c
        data/fileoperations.c
        data/fileoperations.h)