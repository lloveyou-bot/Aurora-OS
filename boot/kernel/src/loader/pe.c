// kernel/src/loader/pe.c
#include "../fs.h"
#include "../terminal.h"

int pe_load(const char* filename, void** entry) {
    int fd = fs_open(filename);
    if (fd < 0) {
        terminal_write("File not found: ");
        terminal_write(filename);
        terminal_write("\n");
        return -1;
    }
    
    // 简化版本：只返回错误，表示 PE 加载器尚未实现
    terminal_write("PE loader not fully implemented yet.\n");
    return -1;
}