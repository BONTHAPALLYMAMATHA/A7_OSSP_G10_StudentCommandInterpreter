#ifndef SHELL_H
#define SHELL_H

#define MAX_ARGS 64

void display_prompt(void);
void parse_command(char *input, char *args[]);
void execute_command(char *args[]);
void show_help(void);

#endif

