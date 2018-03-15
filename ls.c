#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include	<pwd.h>
#include	<grp.h>
// ls ; -a; -f, -i, -s, -S, -t, -r, -la
#define BBLUE          "\033[1m\033[34m"      /* Bold Blue */
#define GRN            "\033[1m\033[32m"
#define BMAGENTA       "\033[1m\033[35m"
#define BCYAN       "\033[1m\033[36m"
#define RESET          "\x1B[0m"
#define BUFSIZE         256
#define min s
#define size e
struct stat buf_i;
struct stat buf_j;
int t_i,t_j;
void iNode(ino_t ino){
  printf("%ju ", (uintmax_t)ino);
}
/* this is sorting for time */
void ls_t(char* dr[], int size){
  char* buf = malloc(256);
  int i, j;
  for(i=0 ; i < size- 1; i++)
      for(j= i + 1; j < size; j++){
        stat (dr[i], &buf_i);
        stat (dr[j], &buf_j);
        t_i = buf_i.st_ctime;
        t_j = buf_j.st_ctime;
            if(t_i < t_j){
              strcpy(buf,dr[i]);
              strcpy(dr[i],dr[j]);
              strcpy(dr[j],buf);
           }
       }
}
/* sort for name */
void sort(char* dir_names[], int n){
  char* temp = malloc(256);
  int i, j;

  for(i=0 ; i < n - 1; i++)
      for(j= i + 1; j < n; j++){
            if(strcmp(dir_names[i], dir_names[j])>0){
              strcpy(temp,dir_names[i]);
              strcpy(dir_names[i],dir_names[j]);
              strcpy(dir_names[j],temp);
           }
       }
}
/* sort for size */
void ls_S(char* dr[], int size){
  char* buf = malloc(256);
  int i, j;
  for(i=0 ; i < size - 1; i++)
      for(j= i + 1; j < size; j++){
        stat (dr[i], &buf_i);
        stat (dr[j], &buf_j);
        t_i = buf_i.st_size;
        t_j = buf_j.st_size;
            if(t_i < t_j){
              strcpy(buf,dr[i]);
              strcpy(dr[i],dr[j]);
              strcpy(dr[j],buf);
           }
       }
}
/* reverse */
  void reverse(char* dr[], int n) {
  int c, d;
  char* buf[BUFSIZE];
  sort(dr, n);
    for (c = n - 1, d = 0; c >= 0; c--, d++)
      buf[d] = dr[c];
   for (c = 0; c < n; c++)
      dr[c] = buf[c];
}
/* print ls -la */
void ls_la( char *filename, struct stat *info_p )
{
	char	*uid_to_name(), *ctime(), *gid_to_name(), *filemode();
	void	mode_to_letters();
  char modestr[11];

	mode_to_letters( info_p->st_mode, modestr );

	printf( "%s"    , modestr );
	printf( "%4d "  , (int) info_p->st_nlink);
	printf( "%-8s " , uid_to_name(info_p->st_uid) );
	printf( "%-8s " , gid_to_name(info_p->st_gid) );
	printf( "%8ld " , (long)info_p->st_size);
	printf( "%.12s ", 4+ctime(&info_p->st_mtime));
  if(S_ISDIR(info_p->st_mode)){

    printf(BBLUE "%s\n" RESET,filename);
  }else if(access(filename,X_OK) != -1){
  printf(GRN "%s\n" RESET,filename);
}
else
	printf( "%s\n"  , filename );

}
void mode_to_letters( int mode, char str[] )
{
    strcpy( str, "----------" );

    if ( S_ISDIR(mode) )  str[0] = 'd';
    if ( S_ISCHR(mode) )  str[0] = 'c';
    if ( S_ISBLK(mode) )  str[0] = 'b';

    if ( mode & S_IRUSR ) str[1] = 'r';
    if ( mode & S_IWUSR ) str[2] = 'w';
    if ( mode & S_IXUSR ) str[3] = 'x';

    if ( mode & S_IRGRP ) str[4] = 'r';
    if ( mode & S_IWGRP ) str[5] = 'w';
    if ( mode & S_IXGRP ) str[6] = 'x';

    if ( mode & S_IROTH ) str[7] = 'r';
    if ( mode & S_IWOTH ) str[8] = 'w';
    if ( mode & S_IXOTH ) str[9] = 'x';
}
char *uid_to_name( uid_t uid )
/*
 *	returns pointer to username associated with uid, uses getpw()
 */
{
	struct	passwd *getpwuid(), *pw_ptr;
	static  char numstr[10];

	if ( ( pw_ptr = getpwuid( uid ) ) == NULL ){
		sprintf(numstr,"%d", uid);
		return numstr;
	}
	else
		return pw_ptr->pw_name ;
}
char *gid_to_name( gid_t gid )
/*
 *	returns pointer to group number gid. used getgrgid(3)
 */
{
	struct group *getgrgid(), *grp_ptr;
	static  char numstr[10];

	if ( ( grp_ptr = getgrgid(gid) ) == NULL ){
		sprintf(numstr,"%d", gid);
		return numstr;
	}
	else
		return grp_ptr->gr_name;
}
void ls_F(int mode, char* dir_name){
  //directory
  if (S_ISDIR (mode))
      printf ("/");
  //executable
  else if (access(dir_name, X_OK) != -1)
    printf ("*");
  //symbolic link
  else if (S_ISLNK(mode))
    printf ("@");
  //socket
  else if (S_ISSOCK(mode))
    printf ("=");
  //FIFO
  else if ( S_ISFIFO (mode))
    printf("|");

  else printf("");
}


void main(int argc, char const *argv[])
{
    DIR           *dp;
    struct dirent *dirp;
    struct stat buf;
    char *bf2 = malloc(10000);
    char * bf = malloc(10000);
   dp = opendir(".");
   struct winsize w;
   ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
   char buffer [33];
   int sq = 0;
   int mode=0;
   int total = 0;
   char **a = calloc(256, sizeof(char*));
   int sizes[256];
   int pt = 0;
   int i,n;
   for (i = 0;(dirp = readdir(dp)) != NULL; i++) {
      a[i] = malloc(sizeof(dirp->d_name));
      strcpy(a[i], dirp->d_name);
      stat(dirp->d_name, &buf);
  }

 for (i = 0; i < n; ++i) {
     if(strstr(argv[1], "-f") == 0)
       if(!strcmp(".",a[i]) || !strcmp("..",a[i]))
         continue;
      stat(a[i], &buf);
      int siz = buf.st_blocks >> 1;
      total += siz;
      sizes[i] = siz;
  }
    printf("total %d\n",total);

  a[i] = NULL;
  n = i;
  if(strstr(argv[1], "-t"))
    ls_t(a,n);
  if(strstr(argv[1], "-S"))
      ls_S(a,n);
  if(strstr(argv[1], "ls"))
        sort(a,n);
  if(strstr(argv[1], "-r"))
        reverse(a,n);
  if(strstr(argv[1], "-a"))
    sort(a,n);
  if(strstr(argv[1], "-A"))
      sort(a,n);
  //if(strstr(argv[1], "-f"))
//  sort(a,n);

     for (i = 0; i < n; i++) {
         struct stat info;
       if(strstr(argv[1], "-la")){
         if ( stat(a[i], &info) == -1 )
           perror( a[i] );
         else
             ls_la(a[i],&info);
           }

      else{
        if(strstr(argv[1], "-i"))
        {
          printf("%ld ",buf.st_ino);
        }
       if(strstr(argv[1], "-a")==0&&strstr(argv[1], "-f")==0)
       if(!strcmp(".",a[i]) || !strcmp("..",a[i])){
         continue;
       }


      if(strstr(argv[1], "-i")){
      sprintf(buffer, "%ld",buf.st_ino);
      sq += strlen(a[i]) + 7 + strlen(buffer);}
      else if(argv[1], "-s"){
       sprintf(buffer, "%d", sizes[i]);
       sq += strlen(a[i]) + 6 + strlen(buffer);
     }

     else sq+=strlen(a[i]);
       buffer[0] = '\0';
       mode=buf.st_ino;
       stat(a[i], &buf);
       if(strstr(argv[1], "-s")!=0)
        printf("%d ",sizes[i]);
       if(strstr(argv[1], "-f")!=0)
       printf("%s     ", a[i]);

      else if(strstr(argv[1], "-F")!=0){
         if(S_ISDIR(buf.st_mode)){
           printf(BBLUE "%s" RESET,a[i]);
           ls_F(mode,a[i]);
           printf("     ");
         }else if(access(a[i],X_OK) != -1){
           printf(GRN "%s" RESET,a[i]);
           ls_F(mode,a[i]);
           printf("    ");
         }else{
             printf("%s",a[i]);
             ls_F(mode,a[i]);
             printf("     ");
         }
       }
       else{
       if(S_ISDIR(buf.st_mode)){
         printf(BBLUE "%s     " RESET,a[i]);
       }else if(access(a[i],X_OK) != -1){

         printf(GRN "%s     " RESET,a[i]);
       }else{
           printf("%s     ",a[i]);
       }
     }
       if(i + 1 < n && sq + strlen(a[i + 1]) + 6 > w.ws_col){
         printf("\n");
         sq = 0;
       }
     }
   }
     printf("\n");

     closedir(dp);
     exit(0);
 }
