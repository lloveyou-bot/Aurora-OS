// kernel/src/shell.c
#include "terminal.h"
#include "shell.h"
#include "commands.h"
#include "fs.h"          // 添加 fs.h 解决 fs_list_files 未声明
#include <stddef.h>

// 不需要再定义 NULL，stddef.h 已经定义了

static char cmd_buffer[CMD_BUF_SIZE];
static int cmd_pos = 0;

// 声明外部命令函数
extern void cmd_help(int argc, char** argv);
extern void cmd_clear(int argc, char** argv);
extern void cmd_echo(int argc, char** argv);
extern void cmd_ls(int argc, char** argv);
extern void cmd_cat(int argc, char** argv);
extern void cmd_mkdir(int argc, char** argv);
extern void cmd_touch(int argc, char** argv);
extern void cmd_rm(int argc, char** argv);
extern void cmd_pwd(int argc, char** argv);
extern void cmd_cd(int argc, char** argv);
extern void cmd_meminfo(int argc, char** argv);
extern void cmd_reboot(int argc, char** argv);
extern void cmd_version(int argc, char** argv);
extern void cmd_explorer(int argc, char** argv);
extern void cmd_net(int argc, char** argv);
extern void cmd_help_zh(int argc, char** argv);
extern void cmd_date(int argc, char** argv);
extern void cmd_uptime(int argc, char** argv);
extern void cmd_whoami(int argc, char** argv);
extern void cmd_tree(int argc, char** argv);
extern void cmd_find(int argc, char** argv);
extern void cmd_hexdump(int argc, char** argv);
extern void cmd_echo_color(int argc, char** argv);

// 命令表
static command_t command_table[] = {
    {"help", "Show this help", cmd_help},
    {"clear", "Clear screen", cmd_clear},
    {"echo", "Print text", cmd_echo},
    {"ls", "List files", cmd_ls},
    {"cat", "View file", cmd_cat},
    {"mkdir", "Make directory", cmd_mkdir},
    {"touch", "Create file", cmd_touch},
    {"rm", "Delete file", cmd_rm},
    {"pwd", "Show path", cmd_pwd},
    {"cd", "Change directory", cmd_cd},
    {"meminfo", "Memory info", cmd_meminfo},
    {"reboot", "Reboot", cmd_reboot},
    {"version", "Version", cmd_version},
    {"explorer", "File explorer", cmd_explorer},
    {"net", "Network test (NE2000)", cmd_net},
    {"help_zh", "中文帮助", cmd_help_zh},
    {"date", "Show date/time", cmd_date},
    {"uptime", "Show system uptime", cmd_uptime},
    {"whoami", "Show current user", cmd_whoami},
    {"tree", "Show directory tree", cmd_tree},
    {"find", "Find file by name", cmd_find},
    {"hexdump", "Hexdump a file", cmd_hexdump},
    {"echo_color", "Print colored text", cmd_echo_color},
    {NULL, NULL, NULL}
};

static command_t* find_command(const char* name) {
    if (name[0] == '/') name++;
    for (int i = 0; command_table[i].name != NULL; i++) {
        const char* a = command_table[i].name;
        const char* b = name;
        while (*a && *b) {
            if (*a != *b) break;
            a++; b++;
        }
        if (*a == '\0' && *b == '\0') return &command_table[i];
    }
    return NULL;
}

// ====== Tab 补全功能 ======
static void autocomplete(void) {
    // 检查是否在命令位置（没有空格）
    int has_space = 0;
    for (int i = 0; i < cmd_pos; i++) {
        if (cmd_buffer[i] == ' ') {
            has_space = 1;
            break;
        }
    }
    
    if (has_space) {
        // 文件补全（简单实现：列出匹配的文件）
        terminal_write("\n");
        fs_list_files();
        terminal_write("AuroraOS> ");
        // 重新显示当前输入
        for (int i = 0; i < cmd_pos; i++) {
            terminal_putchar(cmd_buffer[i]);
        }
        return;
    }
    
    // 命令补全
    const char* partial = cmd_buffer;
    int partial_len = 0;
    while (partial[partial_len]) partial_len++;
    
    // 查找匹配的命令
    int match_count = 0;
    int match_index = -1;
    
    for (int i = 0; command_table[i].name != NULL; i++) {
        const char* name = command_table[i].name;
        int match = 1;
        for (int j = 0; j < partial_len; j++) {
            if (name[j] != partial[j]) {
                match = 0;
                break;
            }
        }
        if (match && name[partial_len] != '\0') {
            match_count++;
            if (match_index == -1) {
                match_index = i;
            }
        }
    }
    
    if (match_count == 0) {
        // 没有匹配
        terminal_putchar('\a');  // Beep
        return;
    }
    
    if (match_count == 1) {
        // 唯一匹配，直接补全
        const char* name = command_table[match_index].name;
        // 删除之前的输入
        for (int i = 0; i < cmd_pos; i++) {
            terminal_putchar('\b');
        }
        // 输出完整命令
        cmd_pos = 0;
        while (name[cmd_pos]) {
            cmd_buffer[cmd_pos] = name[cmd_pos];
            terminal_putchar(name[cmd_pos]);
            cmd_pos++;
        }
        terminal_putchar(' ');
        cmd_buffer[cmd_pos] = ' ';
        cmd_pos++;
    } else {
        // 多个匹配，显示选项
        terminal_write("\n");
        for (int i = 0; command_table[i].name != NULL; i++) {
            const char* name = command_table[i].name;
            int match = 1;
            for (int j = 0; j < partial_len; j++) {
                if (name[j] != partial[j]) {
                    match = 0;
                    break;
                }
            }
            if (match) {
                terminal_write("  ");
                terminal_write(name);
                terminal_write("\n");
            }
        }
        terminal_write("AuroraOS> ");
        for (int i = 0; i < cmd_pos; i++) {
            terminal_putchar(cmd_buffer[i]);
        }
    }
}

static int parse_command(char* buf, char** argv) {
    int argc = 0;
    char* p = buf;
    while (*p == ' ') p++;
    while (*p && argc < MAX_ARGS - 1) {
        argv[argc++] = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
    }
    argv[argc] = NULL;
    return argc;
}

static void execute_command(char* buf) {
    char* argv[MAX_ARGS];
    int argc = parse_command(buf, argv);
    if (argc == 0) return;
    command_t* cmd = find_command(argv[0]);
    if (cmd) {
        cmd->handler(argc, argv);
    } else {
        terminal_write("Command not found: ");
        terminal_write(argv[0]);
        terminal_write("\n");
        terminal_write("Type '/help' for available commands\n");
    }
}

void shell_init() {
    cmd_pos = 0;
    cmd_buffer[0] = '\0';
    terminal_clear();
    terminal_write("Aurora OS v0.5 (Text Mode + Network)\n");
    terminal_write("Type '/help' for available commands\n\n");
    terminal_write("AuroraOS> ");
}

void shell_process_char(char c) {
    if (c == '\t') {  // Tab 键
        autocomplete();
        return;
    }
    if (c == '\n' || c == '\r') {
        terminal_putchar('\n');
        cmd_buffer[cmd_pos] = '\0';
        execute_command(cmd_buffer);
        cmd_pos = 0;
        terminal_write("AuroraOS> ");
        return;
    }
    if (c == '\b') {
        if (cmd_pos > 0) {
            cmd_pos--;
            terminal_putchar('\b');
        }
        return;
    }
    if (cmd_pos < CMD_BUF_SIZE - 1 && c >= 0x20 && c <= 0x7E) {
        cmd_buffer[cmd_pos++] = c;
        terminal_putchar(c);
    }
}

void shell_run() {
    while (1) {
        __asm__ volatile ("hlt");
    }
}