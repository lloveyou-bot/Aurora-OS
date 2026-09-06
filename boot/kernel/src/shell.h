// kernel/src/shell.h
#ifndef SHELL_H
#define SHELL_H

#define CMD_BUF_SIZE 256
#define MAX_ARGS 32

// 命令结构（只在这里定义一次）
typedef struct {
    const char* name;
    const char* description;
    void (*handler)(int argc, char** argv);
} command_t;

// 函数声明
void shell_init(void);
void shell_process_char(char c);
void shell_run(void);

#endif