// kernel/src/shell.h
#ifndef SHELL_H
#define SHELL_H

#define CMD_BUF_SIZE   256
#define MAX_ARGS       16
#define MAX_CMD_HIST   16

typedef struct {
    const char* name;
    const char* desc;
    void (*handler)(int argc, char** argv);
} command_t;

void shell_init();
void shell_process_char(char c);
void shell_run();

#endif