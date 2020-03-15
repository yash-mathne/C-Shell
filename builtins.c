#include "common.h"
// pwd command
extern char homedir[BUF_SIZE];
int msnshell_pwd(char** arguments)
{
  char pwd[BUF_SIZE];
  if(getcwd(pwd, sizeof(pwd)) == NULL)
      perror("Error getting current path");
  printf("%s\n",pwd);
  return 1;
}

// clear screen
int msnshell_clear(char **arguments){
  printf("\e[1;1H\e[2J");
  return 1;
}

// echo command implementation
int msnshell_echo(char **arguments,int nargs)
{
  int i;
  for(i=1;i<nargs;i++)
  {
      if(arguments[i]==NULL)
        break;

      // if environment variable
      if(arguments[i][0]=='$')
      {
        char *env ;
        env = getenv(arguments[i]+1);
        if(env!=NULL)
            fprintf(stdout,"%s ",env);
      }
      else
      {
        int j=0;
        while(arguments[i][j]!='\0')
        {
          if(arguments[i][j] != '\"' && arguments[i][j]!= '\'' )
              fprintf(stdout,"%c",arguments[i][j]);
          j++;
        }
        fprintf(stdout," ");
      }
  }
   fprintf(stdout,"\n");
   return 1;
}

// cd command
int msnshell_cd(char** arguments)
{
  if (arguments[2] != NULL){
      perror("Error: cd takes only one argument");
      return 1;
  }
  else if (arguments[1] == NULL || !strcmp(arguments[1],"~")){
      chdir(homedir);
      return 1;
  }
  else{
    if(chdir(return_abs(arguments[1]))!=0){
        perror("Error changing directory");
        return 1;
      }
   }
}
