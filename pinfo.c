#include "common.h"
// pinfo command
int msnshell_pinfo(char **arguments)
{
  int pid;
  char stat;
  long unsigned int memory;
  char exec_path[BUF_SIZE];
  char proc_path[BUF_SIZE];
  char cur_path[BUF_SIZE];
  if(arguments[1]!=NULL)
  {
    strcpy(proc_path,"/proc");
    strcat(proc_path,"/");
    strcat(proc_path,arguments[1]);
  }
  else
  {
    int proc_pid = getpid();
    sprintf(proc_path,"/proc/%d",proc_pid);
  }

  strcpy(cur_path,proc_path);
  strcat(cur_path,"/stat");
  FILE *status = fopen(cur_path,"r");
  if(status==NULL){
    fprintf(stdout,"No such process exists");
    return 1;
  }
  else{
    fscanf(status,"%d %*s %c %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %lu %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d",&pid,&stat,&memory);
    fclose(status);
  }
  memory/=1024;
  strcpy(cur_path,proc_path);
  strcat(cur_path,"/exe");
  int p;
  if((p=readlink(cur_path,exec_path,sizeof(exec_path)))==-1)
     fprintf(stderr,"Error getting executable path for the process\n");
  else
      exec_path[p]='\0';
  // now print all values gathered
  printf("pid -- %d\nProcess Status -- %c\nMemory -- %lu kB\n",pid,stat,memory);
  char *relpath = display_path(exec_path);
  fprintf(stdout,"Executable Path -- %s\n",relpath);
  return 1;
}
