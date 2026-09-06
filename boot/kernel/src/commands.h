// kernel/src/commands.h
#ifndef COMMANDS_H
#define COMMANDS_H

#include "shell.h"  // 包含 command_t 定义

// 所有命令声明
void cmd_help(int argc, char** argv);
void cmd_clear(int argc, char** argv);
void cmd_echo(int argc, char** argv);
void cmd_ls(int argc, char** argv);
void cmd_cat(int argc, char** argv);
void cmd_mkdir(int argc, char** argv);
void cmd_touch(int argc, char** argv);
void cmd_rm(int argc, char** argv);
void cmd_pwd(int argc, char** argv);
void cmd_cd(int argc, char** argv);
void cmd_meminfo(int argc, char** argv);
void cmd_reboot(int argc, char** argv);
void cmd_version(int argc, char** argv);
void cmd_explorer(int argc, char** argv);
void cmd_net(int argc, char** argv);
void cmd_help_zh(int argc, char** argv);
void cmd_date(int argc, char** argv);
void cmd_uptime(int argc, char** argv);
void cmd_whoami(int argc, char** argv);
void cmd_tree(int argc, char** argv);
void cmd_find(int argc, char** argv);
void cmd_hexdump(int argc, char** argv);
void cmd_echo_color(int argc, char** argv);

#endif