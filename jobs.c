#include "common.h"

extern pid_t current_pid;
extern pid_t shell_pid ;
extern char bgproc[100000][256];
extern char homedir[BUF_SIZE];
extern int num_proc;
extern int bg_proc_pids[256];
extern char cur_proc_name[256];

void overkill()
{
  for(int i=1;i<=num_proc;++i)
  {
    int kpid = bg_proc_pids[i];
    strcpy(bgproc[kpid],"");
    kill(kpid,SIGINT);
  }
  num_proc=0;
}

// send a signal to a process as specified by user
void kjob(char ** arguments,int nargs)
{
  if(nargs!=3)
    fprintf(stderr,"Invalid number of arguments provided\n");
  else{
    int job_num,signal_num;
    job_num = atoi(arguments[1]);
    signal_num = atoi(arguments[2]);
    if(job_num>num_proc)
        fprintf(stderr,"Invalid job number\n");
    else{
          printf("%sKilled process %s with pid %d%s\n",RED,bgproc[bg_proc_pids[job_num]],bg_proc_pids[job_num],DEF);
          kill(bg_proc_pids[job_num],signal_num);
        }
      }
}

// bring a background process to foreground
void fg(char ** arguments,int nargs)
{
  if(nargs!=2)
  {
    fprintf(stderr,"Invalid number of arguments provided\n");
  }
  else
  {
    int job_num = atoi(arguments[1]);
    if(job_num<=num_proc)
    {
      //get the pid of the background process being brought to foreground
       pid_t job_pid = bg_proc_pids[job_num];
       printf("continued process %s with pid %d\n",bgproc[bg_proc_pids[job_num]],bg_proc_pids[job_num]);
       //delete it from the list of background processes
       for(int i=job_num;i<num_proc;++i)
         bg_proc_pids[i]=bg_proc_pids[i+1];
       num_proc-=1;
       //send a SIGCONT signal to the pid obtained
       kill(job_pid,SIGCONT);
       current_pid = job_pid;
       strcpy(cur_proc_name,bgproc[job_pid]);
       strcpy(bgproc[job_pid],"");
       int wpid,state;
       wpid = waitpid(job_pid, &state, WUNTRACED);
       while(1)
       {
          if (!WIFEXITED(state) && !WIFSIGNALED(state) && !(WIFSTOPPED(state)))
           {
              wpid = waitpid (job_pid, &state, WUNTRACED);
              continue;
           }
          else
           {
             break;
           }
       }
     }
   }
}

//changes a stopped background job to a running background job
void bgp(char ** arguments,int nargs)
{
  if(nargs!=2)
  {
    fprintf(stderr,"Invalid number of arguments provided\n");
  }
  else{
    int job_num = atoi(arguments[1]);
    if(job_num<=num_proc)
    {
      pid_t job_pid = bg_proc_pids[job_num];
      printf("Continued process %s with pid %d\n",bgproc[bg_proc_pids[job_num]],bg_proc_pids[job_num]);
      kill(job_pid,SIGCONT);
    }
  }
}

void show_jobs()
{
  for(int z=1;z<=num_proc;z++)
  {
      char path[100];
      sprintf(path,"/proc/%d",bg_proc_pids[z]);
      strcat(path,"/status");
      FILE *status ;
      status = fopen(path,"r");
      char line[256];
      int y,size=0;
      for(y=1;y<=3;y++)
         fgets(line,sizeof(line),status);
      line[strlen(line)-1] =  ' ';
      printf("%s[%d] %s %s [%d]%s\n",CYAN,z,line,bgproc[bg_proc_pids[z]],bg_proc_pids[z],DEF);
      fclose(status);
  }
}
