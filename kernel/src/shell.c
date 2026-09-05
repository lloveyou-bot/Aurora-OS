// kernel/src/shell.c
#include "terminal.h"
#include "shell.h"
#include "commands.h"
#include <stddef.h>

#define NULL 0

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
extern void cmd_browser(int argc, char** argv);
extern void cmd_desktop(int argc, char** argv);
extern void cmd_wm(int argc, char** argv);
extern void cmd_run(int argc, char** argv);
extern void cmd_help_zh(int argc, char** argv);

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
    {"browser", "Web browser", cmd_browser},
    {"desktop", "Desktop", cmd_desktop},
    {"wm", "Window manager", cmd_wm},
    {"run", "Run program", cmd_run},
    {"help_zh", "中文帮助", cmd_help_zh},
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
    terminal_write("Aurora OS v0.4\n");
    terminal_write("Type '/help' for available commands\n\n");
    terminal_write("AuroraOS> ");
}

void shell_process_char(char c) {
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