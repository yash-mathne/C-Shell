#include "common.h"
//associated ls functions
int take_hidden_comp(const char *s1, const char *s2)
{
		if(s1[0]=='.') s1++;
		if(s2[0]=='.') s2++;
		return strcmp(s1,s2);
}

int filesort(const struct dirent **file1, const struct dirent **file2)
{
  return take_hidden_comp((*file1)->d_name, (*file2)->d_name);
}

int filter_hidden(const struct dirent *file)
{
     return (file->d_name[0] == '.')? 0 : 1;
}
// initially was using to print colors for different file types but removed for implementing redirection
void file_name_print(struct dirent* file,char* end)
{
    if(file->d_type == DT_REG)          // regular file
        printf("%s%s",file->d_name,end);
    else if(file->d_type == DT_DIR)    // a directory
        printf("%s%s",file->d_name,end);
    else                              // unknown file types
        printf("%s%s",file->d_name,end);
}
// ls -l implementation (-a passed as argument)
void long_list(char* path,int all){
    struct dirent **filelist;
    struct stat details;
    int i=0,total=0;
    char timer[14];
    int listn;

    if(all == 1)
       listn = scandir(path, &filelist,0,filesort);
    else
       listn = scandir(path, &filelist, filter_hidden, alphasort);

    if(listn > 0){
      for ( i = 0; i < listn; i++){
        char path_to_file[256];
        strcpy(path_to_file,path);
        strcat(path_to_file,"/");
        strcat(path_to_file,filelist[i]->d_name);
        int t = stat(path_to_file,&details);
        if(t == 0)
      {
          total += details.st_blocks; // block size
          // owner permissions - group permissions - other permissions
          printf("%1s",(S_ISDIR(details.st_mode)) ? "d" : "-");
          printf("%1s",(details.st_mode & S_IWUSR) ? "w" : "-");
          printf("%1s",(details.st_mode & S_IRUSR) ? "r" : "-");
          printf("%1s",(details.st_mode & S_IXUSR) ? "x" : "-");
          printf("%1s",(details.st_mode & S_IRGRP) ? "r" : "-");
          printf("%1s",(details.st_mode & S_IWGRP) ? "w" : "-");
          printf("%1s",(details.st_mode & S_IXGRP) ? "x" : "-");
          printf("%1s",(details.st_mode & S_IWOTH) ? "w" : "-");
          printf("%1s",(details.st_mode & S_IROTH) ? "r" : "-");
          printf("%1s ",(details.st_mode & S_IXOTH) ? "x" : "-");
          // links associated - owner name - group name
          printf("%4ld ",(unsigned long)(details.st_nlink));
          printf("%s ",(getpwuid(details.st_uid))->pw_name);
          printf("%s ",(getgrgid(details.st_gid))->gr_name);
          // file size (bytes) - time modified - name
          printf("%9lld ",(unsigned long long)details.st_size);
          strftime (timer,14,"%h %d %H:%M",localtime(&details.st_mtime));
          printf("%s ",timer);
          file_name_print(filelist[i],"\n");
        }
      free(filelist[i]);
    }
  free(filelist);
  printf("total %d\n",(total/2));
  }
    else
      printf("Empty directory\n");
 }
// ls implementation (-a as argument)
void short_list(char *path,int all)
{
  struct dirent **filelist;
  int i=0,total=0;
  int listn;
  if(all == 1)
     listn = scandir(path, &filelist,0,alphasort);
  else
     listn = scandir(path, &filelist, filter_hidden, alphasort);
  if(listn > 0){
     for ( i = 0; i < listn; i++)        {
          file_name_print(filelist[i],"\n");
          free(filelist[i]);
        }
      free(filelist);
    }
}
// base ls command that calls associated functions according to arguments
int msnshell_ls(char** arguments,int nargs)
{
  char pwd[BUF_SIZE];
   if(getcwd(pwd, sizeof(pwd)) == NULL)
       printf("Error getting current path\n");
  // check the arguments for different flags passed to the ls command
  int i=1,ll=0,al=0,dirpath=0;
  for(i=1;i<nargs;i++)
  {
       if(arguments[i]==NULL)
          break;
        // the place where the redirect would have been will create a break/ null betweek the argument array
       else if(arguments[i][0]=='-')
       {
            if(!strcmp(arguments[i],"-l"))
               ll=1;
            if(!strcmp(arguments[i],"-a"))
               al=1;
            if(!strcmp(arguments[i],"-la") || !strcmp(arguments[i],"-al")){
               ll=1;
               al=1;
            }
       }
      else
      {
          dirpath=1;
          strcpy(pwd,return_abs(arguments[i]));
      }
   }

   if(ll==1)
       long_list(pwd,al);
   else
       short_list(pwd,al);
    return 1;
}
// end of ls implementation
