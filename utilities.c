#include "common.h"
// clock command
void clock_disp(char ** arguments)
{
  char rtc_path[100];
  char file_read[256];
  char **args;

  strcpy(rtc_path,"/proc/driver/rtc");
  FILE *status = fopen(rtc_path,"r");
  if(status==NULL)
    {
      perror("RTC log files not found");
      return 1;
    }
   else
    {
      int tt=0;
      while(tt<=atoi(arguments[4]))
      {
        FILE *status = fopen(rtc_path,"r");
        fgets(file_read,256,status);
        args = separate_acc_to(file_read,ARGS_DELIM);
        printf("%s ",args[2]);
        fgets(file_read,256,status);
        args = separate_acc_to(file_read,ARGS_DELIM);
        printf("%s\n",args[2]);
        sleep(atoi(arguments[2]));
        fclose(status);
        tt+=atoi(arguments[2]);
      }
   }
  return 1;
}

//remindme bonus
void remindme(char **arguments)
{
  pid_t pid, wpid;
  if ((pid = fork()) < 0)
    perror("Error while forking process");
  if(pid==0){
    sleep(atoi(arguments[1]));
    printf("\nReminder: ");
    int arg=2;
    while(arguments[arg]!=NULL){
        printf("%s ",arguments[arg]);
        arg++;
      }
    printf("\n");
  }
  return 1;
}
