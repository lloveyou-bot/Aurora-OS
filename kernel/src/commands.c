// kernel/src/commands.c - 完整命令实现（禁用图形）
#include "terminal.h"
#include "shell.h"
#include "commands.h"
#include "explorer.h"
#include "keyboard.h"
#include "fs.h"
#include "graphics/graphics.h"

#include <stddef.h>

#define NULL 0

// ========== 工具函数 ==========

static int str_equal(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++; b++;
    }
    return *a == *b;
}

static char* itoa(int num) {
    static char buf[32];
    int i = 0;
    if (num == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return buf;
    }
    if (num < 0) {
        buf[i++] = '-';
        num = -num;
    }
    char temp[32];
    int j = 0;
    while (num > 0) {
        temp[j++] = '0' + (num % 10);
        num /= 10;
    }
    while (j > 0) {
        buf[i++] = temp[--j];
    }
    buf[i] = '\0';
    return buf;
}

static unsigned int memory_used = 0;
static unsigned int memory_total = 4 * 1024 * 1024;

static inline void outb(unsigned short port, unsigned char data) {
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

// ========== 基础命令 ==========

void cmd_help(int argc, char** argv) {
    (void)argc; (void)argv;
    terminal_write("Aurora OS Shell - Built-in Commands:\n");
    terminal_write("\n");
    terminal_write("  /help        - Show this help\n");
    terminal_write("  /clear       - Clear screen\n");
    terminal_write("  /echo        - Print text\n");
    terminal_write("\n");
    terminal_write("  [File System]\n");
    terminal_write("  /ls          - List files\n");
    terminal_write("  /cat         - View file\n");
    terminal_write("  /mkdir       - Create directory\n");
    terminal_write("  /touch       - Create file\n");
    terminal_write("  /rm          - Delete file\n");
    terminal_write("  /pwd         - Show path\n");
    terminal_write("  /cd          - Change directory\n");
    terminal_write("\n");
    terminal_write("  [System]\n");
    terminal_write("  /meminfo     - Memory info\n");
    terminal_write("  /reboot      - Reboot\n");
    terminal_write("  /version     - Version\n");
    terminal_write("\n");
    terminal_write("  [Graphics]\n");
    terminal_write("  /explorer    - File explorer\n");
    terminal_write("  /desktop     - Show desktop (coming soon)\n");
    terminal_write("\n");
    terminal_write("  [Programs]\n");
    terminal_write("  /run <file>  - Run program (coming soon)\n");
}

void cmd_clear(int argc, char** argv) {
    (void)argc; (void)argv;
    terminal_clear();
}

void cmd_echo(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        terminal_write(argv[i]);
        if (i < argc - 1) terminal_write(" ");
    }
    terminal_write("\n");
}

// ========== 系统命令 ==========

void cmd_meminfo(int argc, char** argv) {
    (void)argc; (void)argv;
    terminal_write("Memory Info:\n");
    terminal_write("  Total: ");
    terminal_write(itoa(memory_total));
    terminal_write(" bytes\n");
    terminal_write("  Used:  ");
    terminal_write(itoa(memory_used));
    terminal_write(" bytes\n");
    terminal_write("  Free:  ");
    terminal_write(itoa(memory_total - memory_used));
    terminal_write(" bytes\n");
}

void cmd_reboot(int argc, char** argv) {
    (void)argc;
    
    int graphic_mode = 0;
    for (int i = 1; i < argc; i++) {
        if (str_equal(argv[i], "--graphic")) {
            graphic_mode = 1;
            break;
        }
    }
    
    if (graphic_mode) {
        terminal_write("Starting GUI mode...\n");
        terminal_write("Desktop coming soon!\n");
    } else {
        terminal_write("Rebooting...\n");
        outb(0x64, 0xFE);
        __asm__ volatile ("int $0x19");
    }
}

void cmd_version(int argc, char** argv) {
    (void)argc; (void)argv;
    terminal_write("Aurora OS v0.4\n");
    terminal_write("Built: 2026-09-05\n");
}

// ========== 文件系统命令 ==========

void cmd_ls(int argc, char** argv) {
    (void)argc; (void)argv;
    fs_list_files();
}

void cmd_cat(int argc, char** argv) {
    if (argc < 2) {
        terminal_write("Usage: /cat <filename>\n");
        return;
    }
    int fd = fs_open(argv[1]);
    if (fd < 0) {
        terminal_write("File not found: ");
        terminal_write(argv[1]);
        terminal_write("\n");
        return;
    }
    char buf[256];
    int bytes = fs_read(fd, buf, 255);
    if (bytes > 0) {
        buf[bytes] = '\0';
        terminal_write(buf);
        terminal_write("\n");
    }
}

void cmd_mkdir(int argc, char** argv) {
    if (argc < 2) {
        terminal_write("Usage: /mkdir <dirname>\n");
        return;
    }
    if (fs_create_dir(argv[1]) == 0) {
        terminal_write("Directory created: ");
        terminal_write(argv[1]);
        terminal_write("\n");
    } else {
        terminal_write("Failed to create directory\n");
    }
}

void cmd_touch(int argc, char** argv) {
    if (argc < 2) {
        terminal_write("Usage: /touch <filename>\n");
        return;
    }
    if (fs_create_file(argv[1]) == 0) {
        terminal_write("File created: ");
        terminal_write(argv[1]);
        terminal_write("\n");
    } else {
        terminal_write("Failed to create file\n");
    }
}

void cmd_rm(int argc, char** argv) {
    if (argc < 2) {
        terminal_write("Usage: /rm <filename>\n");
        return;
    }
    if (fs_delete(argv[1]) == 0) {
        terminal_write("Deleted: ");
        terminal_write(argv[1]);
        terminal_write("\n");
    } else {
        terminal_write("Failed to delete: ");
        terminal_write(argv[1]);
        terminal_write("\n");
    }
}

void cmd_pwd(int argc, char** argv) {
    (void)argc; (void)argv;
    fs_pwd();
    terminal_write("\n");
}

void cmd_cd(int argc, char** argv) {
    if (argc < 2) {
        terminal_write("Usage: /cd <dirname>\n");
        return;
    }
    fs_cd(argv[1]);
}

// ========== 图形命令 ==========

void cmd_explorer(int argc, char** argv) {
    (void)argc; (void)argv;
    terminal_write("Starting Text Mode Explorer...\n");
    keyboard_set_explorer_mode(1);
    explorer_init();
}

// ========== 占位命令 ==========

void cmd_desktop(int argc, char** argv) {
    (void)argc; (void)argv;
    terminal_write("Desktop: Coming soon!\n");
}

void cmd_wm(int argc, char** argv) {
    (void)argc; (void)argv;
    terminal_write("Window Manager: Coming soon!\n");
}

void cmd_run(int argc, char** argv) {
    if (argc < 2) {
        terminal_write("Usage: /run <filename>\n");
        return;
    }
    terminal_write("Program loader: Coming soon!\n");
}

void cmd_help_zh(int argc, char** argv) {
    (void)argc; (void)argv;
    terminal_write("Aurora OS 命令列表:\n");
    terminal_write("  /help     - 显示帮助\n");
    terminal_write("  /clear    - 清屏\n");
    terminal_write("  /echo     - 打印文字\n");
    terminal_write("  /ls       - 列出文件\n");
    terminal_write("  /cat      - 查看文件\n");
    terminal_write("  /mkdir    - 创建目录\n");
    terminal_write("  /touch    - 创建文件\n");
    terminal_write("  /rm       - 删除文件\n");
    terminal_write("  /pwd      - 显示路径\n");
    terminal_write("  /cd       - 切换目录\n");
    terminal_write("  /meminfo  - 内存信息\n");
    terminal_write("  /reboot   - 重启系统\n");
    terminal_write("  /version  - 版本信息\n");
    terminal_write("  /explorer - 文件管理器\n");
}

// ========== 占位函数（供 shell.c 引用） ==========

void cmd_browser(int argc, char** argv) {
    (void)argc; (void)argv;
    terminal_write("Browser: Coming soon!\n");
}