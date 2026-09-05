// kernel/src/fs.h
#ifndef FS_H
#define FS_H

#define MAX_FILES       32
#define MAX_FILENAME    32
#define MAX_FILE_SIZE   4096

// 文件类型
#define FILE_TYPE_DIR   0
#define FILE_TYPE_FILE  1

// 文件条目
typedef struct {
    char name[MAX_FILENAME];
    int type;           // 0=目录, 1=文件
    int size;           // 文件大小（字节）
    int start_sector;   // 起始扇区
    int parent;         // 父目录索引（-1表示根目录）
} file_entry_t;

// 文件系统
typedef struct {
    file_entry_t files[MAX_FILES];
    int file_count;
    int current_dir;    // 当前目录索引
    char disk[1024 * 1024];  // 1MB 虚拟磁盘
} filesystem_t;

// 文件系统函数
void fs_init();
int fs_create_file(const char* name);
int fs_create_dir(const char* name);
int fs_delete(const char* name);
int fs_open(const char* name);
int fs_read(int fd, char* buf, int size);
int fs_write(int fd, const char* buf, int size);
void fs_list_files();
void fs_cd(const char* name);
void fs_pwd();

// 获取当前目录内容
file_entry_t* fs_get_children(int* count);

// 获取文件系统信息
int fs_get_file_count();
file_entry_t* fs_get_file(int index);

#endif