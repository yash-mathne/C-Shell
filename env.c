#include "common.h"
// setting environment variables
int msnshell_setenv(char **arguments)
{
  int nargs=0;
  while(arguments[nargs]!=NULL)
    nargs++;
  if(nargs<=1 || nargs>=4 )
  {
    fprintf(stderr,"%s%s%s%s",RED,"Invalid number of arguments to setenv",DEF,"\n");
    if(nargs<=1)
    {
      fprintf(stderr,"%s%s%s%s",BLUE,"Too few arguments,usage: setenv variable [newvalue]",DEF,"\n");
    }
    if(nargs>=4)
    {
      fprintf(stderr,"%s%s%s%s",BLUE,"Too many arguments,usage: setenv variable [newvalue]",DEF,"\n");
    }
  }
  else
  {
    if(nargs==2)
    {
      if(setenv(arguments[1],"",1) != 0)
      {
        fprintf(stderr,"%s%s%s%s",RED,"Error setting environment variable",DEF,"\n");
        perror("msnShell:setenv");
      }
    }
    else if(nargs==3)
    {
      if(setenv(arguments[1],arguments[2],1) != 0)
      {
        fprintf(stderr,"%s%s%s%s",RED,"Error setting environment variable",DEF,"\n");
        perror("msnShell:setenv");
      }
    }
  }
  return 1;
}

// unsetting environment variables
int msnshell_unsetenv(char **arguments)
{
  int nargs=0;

  while(arguments[nargs]!=NULL){
    nargs++;
  }
  if(nargs<=1)
  {
    fprintf(stderr,"%s%s%s%s",RED,"Invalid number of arguments to setenv",DEF,"\n");
    fprintf(stderr,"%s%s%s%s",BLUE,"Too few arguments,usage: setenv variable [newvalue]",DEF,"\n");
  }
  else if(nargs>=3)
  {
    fprintf(stderr,"%s%s%s%s",RED,"Invalid number of arguments to setenv",DEF,"\n");
    fprintf(stderr,"%s%s%s%s",BLUE,"Too many arguments,usage: setenv variable [newvalue]",DEF,"\n");
  }
  else
  {
    if(unsetenv(arguments[1])==-1)
    {
      fprintf(stderr,"%s%s%s%s",RED,"Error unsetting environment variable",DEF,"\n");
      perror("msnShell:unsetenv");
    }
  }
  return 1;
}
