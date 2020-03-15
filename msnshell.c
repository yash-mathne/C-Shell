#include "common.h"

char *supported_commands[] = {"cd","ls","pwd","clear","echo","pinfo","setenv","unsetenv"};
int (*supported_functions[])(char**) = {&msnshell_cd,&msnshell_ls,&msnshell_pwd,&msnshell_clear,&msnshell_echo,&msnshell_pinfo,&msnshell_setenv,&msnshell_unsetenv};

pid_t current_pid = -100;
pid_t shell_pid ;
char bgproc[100000][256];
char homedir[BUF_SIZE];
int num_proc = 0;
int bg_proc_pids[256];
char cur_proc_name[256];

void checkctrl_C(int sig)
{
  pid_t check_pid = getpid();
  if(check_pid != shell_pid)
		return;
	if(current_pid > 0)
		kill(current_pid, SIGINT);
	signal(SIGINT, checkctrl_C);
  current_pid=-1;
}

void checkctrl_Z(int sig)
{
	if(getpid() != shell_pid)
		return;
	if(current_pid > 0)
	{
		kill(current_pid, SIGTTIN);
		kill(current_pid, SIGTSTP);
    num_proc++;
    bg_proc_pids[num_proc] = current_pid;
    strcpy(bgproc[current_pid],cur_proc_name);
	  printf("+[%d] %d %s\n",num_proc,current_pid,bgproc[current_pid]);
	}
	signal(SIGTSTP, checkctrl_Z);
}

void childHandler(int sig)
{
  int check;
  for(int p=1;p<=num_proc;)
  {
    int ret_pid = waitpid(bg_proc_pids[p],&check,WNOHANG|WUNTRACED);
    if(bg_proc_pids[p] == ret_pid)// process terminated
    {
      if(WIFEXITED(check))
         fprintf(stderr,"%sBackground process: %s and pid %d exited normally with exit status %d%s\n",BLUE,bgproc[ret_pid],ret_pid,WEXITSTATUS(check),DEF);
      else if (WIFSIGNALED(check))
         fprintf(stderr,"%sBackground process: %s and pid %d exited abnormally with exit status %d%s\n",BLUE,bgproc[ret_pid],ret_pid,WTERMSIG(check),DEF);
      // delete that process from storage
      for(int i=p;i<num_proc;++i)
         bg_proc_pids[i]=bg_proc_pids[i+1];
      strcpy(bgproc[ret_pid],"");
    //  reduce the number of background processes
      num_proc-=1;
    }
    else
      p+=1;
  }
 signal(SIGCHLD, childHandler);
}


void shell_print_prompt()
{
  char *username = getenv("USER");
  char pwd[BUF_SIZE], hostname[BUF_SIZE];
  if(gethostname(hostname,sizeof(hostname))==-1)
      fprintf(stderr,"Error getting hostname\n");
  if(getcwd(pwd, sizeof(pwd)) == NULL)
      fprintf(stderr,"Error getting current path\n");
  char *displaypath = display_path(pwd);
  printf("%s<%s@%s:%s>%s ",RED,username,hostname,displaypath,DEF);
}

char **separate_acc_to(char *input, char* delim)
{
  char *arg;
  char **args = malloc(ARG_BUF_SIZ * sizeof(char*));
  int bfsz = ARG_BUF_SIZ, pos = 0;
  alloc_error_d(args);
  arg = strtok(input, delim);
  if(arg!=NULL)
  {
    do {
      args[pos++] = arg;
      arg = strtok(NULL,delim);
      if (pos >= bfsz)
       {
        bfsz += ARG_BUF_SIZ;
        args = realloc(args, bfsz * sizeof(char*));
        alloc_error_d(args);
       }
      continue;
      } while(arg!=NULL);
  }
  args[pos] = NULL;
  return args;
}

char *read_input()
{
  int bfsz = BUF_SIZE ;
  char *buffer = malloc(sizeof(char) * bfsz);
  int inpch,pos=0;
  alloc_error_s(buffer);
  do {
    inpch = getchar();
    if (inpch == '\n' || inpch == EOF || inpch == '\r')
           {
             buffer[pos] = '\0';
             return buffer;
           }
    else
             buffer[pos++] = inpch;
    if (pos >= bfsz) {
          bfsz += BUF_SIZE;
          buffer = realloc(buffer,sizeof(char) * bfsz);
          alloc_error_s(buffer);
    }
  } while(1);
}

void alloc_error_d(char **buffer)
{
  if (!buffer)
    {
      perror("Error allocating memory");
      exit(EXIT_FAILURE);
    }
}

void alloc_error_s(char *buffer)
{
  if (!buffer)
   {
     perror("Error allocating memory");
     exit(EXIT_FAILURE);
   }
}

// convert relative path to absolute path
char *return_abs(char *relpath)
{
  char *returnpath = (char*)malloc(256*sizeof(char));
  if(relpath[0]=='~')
  {
    strcpy(returnpath,homedir);
    strcat(returnpath,"/");
    if(strlen(relpath)>2)
    {
      relpath++;
      relpath++;
      strcat(returnpath,relpath);
    }
  }
  else
     strcpy(returnpath,relpath);
  return returnpath;
}

// the path to display after considering home path as ~
char *display_path(char *pwd)
{
  int home_size,cur_size;
  home_size = strlen(homedir), cur_size = strlen(pwd);
  if(home_size == cur_size)
  {
    if(!strcmp(homedir,pwd))
        return "~";
    else
        return pwd;
  }
  if(home_size > cur_size)
        return pwd;
  else{
    int i,flag=0;
    for(i=0;i<strlen(homedir);++i)
    {
      if(homedir[i]!=pwd[i])
        {
          flag=1;
          break;
        }
    }
    if (flag==0){
      char *disp = (char*)malloc(100*sizeof(char));
      alloc_error_s(disp);
      disp[0] = '~';
      disp[1] = '/';
      int size_left = (cur_size-home_size+1);
      for(int i=2;i<size_left;++i)
          disp[i] = pwd[home_size+i-1];
      return disp;
      }
      else
      return pwd;
  }
}


// launch commands
int command_launch(char** arguments,int nargs,int bg)
{
    if (nargs==0) return 0;
 	
    if( !strcmp(arguments[0], "quit") ){
      printf("Thanks for using MSNshell! See you soon.\n");
      exit(0);
    }

    if(!strcmp(arguments[0],"cd"))
    {
      msnshell_cd(arguments);
      return 1;
    }

    if(!strcmp(arguments[0],"overkill"))
    {
      overkill();
      return 1;
    }

    if(!strcmp(arguments[0],"fg"))
    {
      fg(arguments,nargs);
      return 1;
    }

    if(!strcmp(arguments[0],"bg"))
    {
      bgp(arguments,nargs);
      return 1;
    }

    if(!strcmp(arguments[0],"remindme")){
       remindme(arguments);
       return 1;
    }

    if(!strcmp(arguments[0],"clock")){
      clock_disp(arguments);
      return 1;
    }

     int status,state;
     int pid, wpid;
     if ((pid = fork()) < 0)
     {
       perror("Error while forking process");
       return 0;
     }

     if(pid==0)
        {
      // assign all child processes the group id of the parent so that they receive the same signal handler
          if(bg==1){
            if(setpgid(0,0)!=0)
            {
              fprintf(stderr,"Unable to start process in background\n");
              exit(1);
            }
          }

           signal(SIGINT, checkctrl_C);
           int i,fd=0,in=0,out=0,append=0;
           char input_file[ARG_BUF_SIZ];
           char output_files[30][ARG_BUF_SIZ];
           char append_files[30][ARG_BUF_SIZ];

          for(i=0;i<nargs;++i)
            {
              if(!strcmp(arguments[i],"<"))
                {
                   in=1;//set the input flag
                   strcpy(input_file,arguments[i+1]);//copy the file to be taken as input
                   arguments[i]=NULL;
               }

              else if(!strcmp(arguments[i],">"))
                {
                    strcpy(output_files[out],arguments[i+1]);//copy the file to be given output
                    arguments[i]=NULL;
                    out+=1;
                }
              else if(!strcmp(arguments[i],">>"))
                {
                    strcpy(append_files[out],arguments[i+1]);//copy the file to be given output
                    arguments[i]=NULL;
                    append+=1;
                }
            }

              if(in==1)
              {
                if((fd = open(input_file, O_RDONLY))<0)
                     perror("msnShell:Error opening input file");
                if(dup2(fd,0) < 0)
                    perror("msnShell:During input duping");
                close(fd);
              }

              for (i = 0; i < out; ++i)
              {
                fd = open(output_files[i], O_WRONLY | O_TRUNC | O_CREAT , 0644);
                if(fd<0)
                      perror("msnShell:Error opening output file");
                int dup = dup2(fd,1);
                if(dup<0)
                       perror("msnShell:During output duping");
                close(fd);
              }

              for (i = 0; i < append; ++i)
              {
                if((fd = open(append_files[i], O_WRONLY | O_APPEND | O_CREAT, 0644))<0)
                      perror("msnShell:Error opening the append file");
                if(dup2(fd,1)<0)
                      perror("Error - output(append) duping");
                close(fd);
              }

              if(!strcmp(arguments[0],"jobs"))
              {
                show_jobs();
                exit(EXIT_FAILURE);
              }

              if(!strcmp(arguments[0],"kjob"))
              {
                kjob(arguments,nargs);
                exit(EXIT_FAILURE);
              }

              if(!strcmp(arguments[0],"ls"))
              {
                msnshell_ls(arguments,nargs);
                exit(EXIT_FAILURE);
              }

              if(!strcmp(arguments[0],"echo"))
              {
                msnshell_echo(arguments,nargs);
                exit(EXIT_FAILURE);
              }

              if(!strcmp(arguments[0],"pwd"))
              {
                msnshell_pwd(arguments);
                exit(EXIT_FAILURE);
              }
              // check if one of the builtins
              if(!(!strcmp(arguments[0],"ls")||!strcmp(arguments[0],"echo")||!strcmp(arguments[0],"pwd")))
              {
                int coms = sizeof(supported_commands)/sizeof(char*);
                for(int i=0;i<coms;++i)
                {
                  if(strcmp(arguments[0],supported_commands[i])==0)
                     return (*supported_functions[i])(arguments);
                }
              }
             // call exec on the child process if not builtin
             if (execvp(arguments[0], arguments) < 0){
                    perror("Error while running exec on command");
                    exit(0);
                  }
              //just as safety measure
              exit(EXIT_FAILURE);
       }
         // background process(parent body)
    else if(pid!=0 && bg==1)
       {
          // store the pid of the process put in background
          num_proc++;
          bg_proc_pids[num_proc] = pid;
          // store the name of the process
          strcpy(bgproc[pid],arguments[0]);
          return 1;
       }
         // foreground process(parent body)
    else if (pid!=0 && bg==0)
       {
          current_pid = pid;
          strcpy(cur_proc_name,arguments[0]);
          strcpy(bgproc[pid],arguments[0]);
          wpid = waitpid(pid, &state, WUNTRACED);
          while(1)
          {
             if (!WIFEXITED(state) && !WIFSIGNALED(state) && !(WIFSTOPPED(state)))
                  {
                     wpid = waitpid (pid, &state, WUNTRACED);
                     continue;
                  }
                 else
                    break;
           }
           return 1;
        }
   }


void shell_loop()
{
  char **commands,**arguments,**piped_commands;
  char *user_input;
  signal(SIGTSTP, checkctrl_Z);
  signal(SIGINT, checkctrl_C);
  signal(SIGCHLD, childHandler);

  while(1){
    // print the shell prompt along with the path of current working directory
    current_pid = -67;//some negative value
    shell_print_prompt();
    // take user input
    user_input = read_input();
    // separate the input given into commands using commands delimiter
    commands = separate_acc_to(user_input,COMMANDS_DELIM);
    // now separate the individual commands into arguments
    int i=0,status=0;
    while(commands[i]!=NULL)
    {
      piped_commands = separate_acc_to(commands[i++],PIPE_DELIM);
      if(piped_commands[1]!=NULL)
      {
        int j=0,fd;
        int pipe_arr[2];
        for(j=0;piped_commands[j]!=NULL;j++)
        {
          // separate the arguments in the basis of arg delimiters
          arguments = separate_acc_to(piped_commands[j],ARGS_DELIM);
          // calculate number of arguments and determine wheter background process
          int nargs,bg=0,pipe_proc;
          for (nargs = 0; arguments[nargs] != NULL; nargs++)
            continue;

          if(!strcmp(arguments[nargs-1],"&")){
            bg=1;
            arguments[nargs-1]=NULL;
            nargs--;
          }
          // create pipe
          pipe(pipe_arr);
          if( (pipe_proc = fork()) < 0)
               perror("msnShell:Error forking process");

          else if(pipe_proc==0)
          {
            dup2(fd,0);
            if(piped_commands[j+1]!=NULL)
                dup2(pipe_arr[1],1);
            close(pipe_arr[0]);
            command_launch(arguments,nargs,bg);
            exit(2);
          }
          else if(pipe_proc>0)
          {
            wait(NULL);
            close(pipe_arr[1]);
            fd = pipe_arr[0];
            i++;
          }
        }
      }
      else
      {
        int nargs,bg=0;
        arguments = separate_acc_to(piped_commands[0],ARGS_DELIM);
        for (nargs = 0; arguments[nargs] != NULL; nargs++)
               continue;
        if(!strcmp(arguments[nargs-1],"&")){
          bg=1;
          arguments[nargs-1]=NULL;
          nargs--;
        }
        command_launch(arguments,nargs,bg);
      }
      free(arguments);
    }
    free(commands);
    free(user_input);
  //  check_for_finished();
  }
}


int main()
{
  printf("\e[1;1H\e[2J");
  getcwd(homedir,sizeof(homedir));
  shell_pid = getpid();
  shell_loop();
  return 0;
}
