// kernel/src/commands.h
#ifndef COMMANDS_H
#define COMMANDS_H

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
void cmd_browser(int argc, char** argv);
void cmd_desktop(int argc, char** argv);
void cmd_wm(int argc, char** argv);
void cmd_run(int argc, char** argv);
void cmd_help_zh(int argc, char** argv);

#endif