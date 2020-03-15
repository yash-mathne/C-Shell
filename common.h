#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <pwd.h>
#include <fcntl.h>
#include <grp.h>
#include <time.h>
#define BUF_SIZE 1024
#define ARG_BUF_SIZ 64
#define COMMANDS_DELIM ";\n"
#define ARGS_DELIM " \r\n\t"
#define PIPE_DELIM "|"
#define GREEN "\x1b[92m"
#define BLUE "\x1b[94m"
#define DEF "\x1B[0m"
#define CYAN "\x1b[96m"
#define RED "\x1b[0;31m"

int msnshell_cd(char** arguments);
int msnshell_ls(char** arguments,int nargs);
int msnshell_pwd(char** arguments);
int msnshell_clear(char ** arguments);
int msnshell_exit(char ** arguments);
int msnshell_echo(char **arguments,int nargs);
int msnshell_pinfo(char **arguments);
int msnshell_setenv(char **arguments);
int msnshell_unsetenv(char **arguments);
char *return_abs(char *relpath);
char *display_path(char *pwd);
void alloc_error_d(char **buffer);
void alloc_error_s(char *buffer);
void clock_disp(char ** arguments);
char **separate_acc_to(char *input, char* delim);
int take_hidden_comp(const char *s1, const char *s2);
int filesort(const struct dirent **file1, const struct dirent **file2);
int filter_hidden(const struct dirent *file);
void file_name_print(struct dirent* file,char* end);
void long_list(char* path,int all);
void short_list(char *path,int all);
void shell_print_prompt();
char *read_input();
int command_launch (char** arguments,int nargs,int bg);
void show_jobs();
void checkctrl_C(int sig);
void checkctrl_Z(int sig);
void childHandler(int sig);
void overkill();
void kjob(char ** arguments,int nargs);
void fg(char ** arguments,int nargs);
void bgp(char ** arguments,int nargs);
