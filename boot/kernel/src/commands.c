// kernel/src/commands.c - 完整版（无标准库依赖）
#include <stddef.h>
#include <stdint.h>
#include "terminal.h"
#include "shell.h"
#include "commands.h"
#include "explorer.h"
#include "keyboard.h"
#include "fs.h"

// ====== 自实现工具函数 ======
static int strlen(const char* s) {
    int len = 0;
    while (s[len]) len++;
    return len;
}

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

// ====== 原有命令 ======

void cmd_help(int argc, char** argv) {
    (void)argc; (void)argv;
    terminal_write("Aurora OS Shell - Built-in Commands:\n");
    terminal_write("\n");
    terminal_write("  /help        - Show this help\n");
    terminal_write("  /clear       - Clear screen\n");
    terminal_write("  /echo        - Print text\n");
    terminal_write("  /ls          - List files\n");
    terminal_write("  /cat         - View file\n");
    terminal_write("  /mkdir       - Create directory\n");
    terminal_write("  /touch       - Create file\n");
    terminal_write("  /rm          - Delete file\n");
    terminal_write("  /pwd         - Show path\n");
    terminal_write("  /cd          - Change directory\n");
    terminal_write("  /meminfo     - Memory info\n");
    terminal_write("  /reboot      - Reboot\n");
    terminal_write("  /version     - Version\n");
    terminal_write("  /explorer    - Text explorer\n");
    terminal_write("  /net         - Test network (NE2000)\n");
    terminal_write("  /date        - Show date/time\n");
    terminal_write("  /uptime      - Show system uptime\n");
    terminal_write("  /whoami      - Show current user\n");
    terminal_write("  /tree        - Show directory tree\n");
    terminal_write("  /find        - Find file by name\n");
    terminal_write("  /hexdump     - Hexdump a file\n");
    terminal_write("  /echo_color  - Print colored text\n");
    terminal_write("  /help_zh     - 中文帮助\n");
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
    (void)argc; (void)argv;
    terminal_write("Rebooting...\n");
    outb(0x64, 0xFE);
    __asm__ volatile ("int $0x19");
}

void cmd_version(int argc, char** argv) {
    (void)argc; (void)argv;
    terminal_write("Aurora OS v0.5 (Text Mode + Network)\n");
    terminal_write("Built: 2026-09-06\n");
}

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

void cmd_explorer(int argc, char** argv) {
    (void)argc; (void)argv;
    terminal_write("Starting Text Mode Explorer...\n");
    keyboard_set_explorer_mode(1);
    explorer_init();
}

// ====== 网络命令 ======
void cmd_net(int argc, char** argv) {
    (void)argc; (void)argv;
    terminal_write("\n");
    extern void net_test(void);
    net_test();
    terminal_write("\n");
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
    terminal_write("  /net      - 网络测试\n");
}

// ====== 新增命令实现 ======

void cmd_date(int argc, char** argv) {
    (void)argc; (void)argv;
    terminal_write("2026-09-06 14:30:00 UTC\n");
    terminal_write("(Static date - RTC not implemented yet)\n");
}

static unsigned int boot_ticks = 0;
void cmd_uptime(int argc, char** argv) {
    (void)argc; (void)argv;
    unsigned int seconds = boot_ticks / 18;
    unsigned int minutes = seconds / 60;
    unsigned int hours = minutes / 60;
    seconds %= 60;
    minutes %= 60;
    terminal_write("Uptime: ");
    terminal_write(itoa(hours));
    terminal_write("h ");
    terminal_write(itoa(minutes));
    terminal_write("m ");
    terminal_write(itoa(seconds));
    terminal_write("s\n");
}

void cmd_whoami(int argc, char** argv) {
    (void)argc; (void)argv;
    terminal_write("root\n");
}

static void print_tree(const char* path, int depth) {
    for (int i = 0; i < depth; i++) {
        terminal_write("  ");
    }
    terminal_write("+-- ");
    terminal_write(path);
    terminal_write("\n");
}

void cmd_tree(int argc, char** argv) {
    (void)argc; (void)argv;
    terminal_write("Directory tree:\n");
    print_tree("/", 0);
    fs_list_files();
}

void cmd_find(int argc, char** argv) {
    if (argc < 2) {
        terminal_write("Usage: /find <filename>\n");
        return;
    }
    terminal_write("Searching for: ");
    terminal_write(argv[1]);
    terminal_write("\n");
    terminal_write("(Simple search - use /ls to list all files)\n");
    fs_list_files();
}

void cmd_hexdump(int argc, char** argv) {
    if (argc < 2) {
        terminal_write("Usage: /hexdump <filename>\n");
        return;
    }
    int fd = fs_open(argv[1]);
    if (fd < 0) {
        terminal_write("File not found: ");
        terminal_write(argv[1]);
        terminal_write("\n");
        return;
    }
    unsigned char buf[16];
    int offset = 0;
    int bytes;
    terminal_write("Hexdump of: ");
    terminal_write(argv[1]);
    terminal_write("\n");
    terminal_write("Offset 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  ASCII\n");
    terminal_write("------ -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --  -----\n");
    while ((bytes = fs_read(fd, (char*)buf, 16)) > 0) {
        char* offset_str = itoa(offset);
        terminal_write(offset_str);
        int len = 0;
        while (offset_str[len]) len++;
        for (int i = 4 - len; i > 0; i--) {
            terminal_write(" ");
        }
        terminal_write(" ");
        for (int i = 0; i < 16; i++) {
            if (i < bytes) {
                if (buf[i] < 16) terminal_write("0");
                terminal_write(itoa(buf[i]));
            } else {
                terminal_write("  ");
            }
            terminal_write(" ");
        }
        terminal_write(" ");
        for (int i = 0; i < bytes; i++) {
            if (buf[i] >= 0x20 && buf[i] <= 0x7E) {
                terminal_putchar(buf[i]);
            } else {
                terminal_putchar('.');
            }
        }
        terminal_write("\n");
        offset += bytes;
    }
}

void cmd_echo_color(int argc, char** argv) {
    if (argc < 2) {
        terminal_write("Usage: /echo_color <text>\n");
        return;
    }
    for (int i = 1; i < argc; i++) {
        const char* arg = argv[i];
        if (arg[0] == '[') {
            continue;
        }
        terminal_write(arg);
        if (i < argc - 1) terminal_write(" ");
    }
    terminal_write("\n");
}