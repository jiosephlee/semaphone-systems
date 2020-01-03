#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
union semun {
	int val;		// Value for SETVAL
	struct semid_ds *buf;		//Buffer for IPC_STAT, IPC_SET
	unsigned short *array;	//Array for GETALL, SETALL
	struct seminfo *__buf;	//Buffer for IPC_INFO (Linux-specific)
};
#define SHMKEY 23444
#define SEMKEY 99918

int shmd,sd,fd;
struct sembuf sem_1;

int main() {
  sem_1.sem_num=0;
  sem_1.sem_op = -1;

  //catching for errors
  printf("trying to get in...\n");
  sd=semget(SEMKEY,1,0);
  if(sd < 0){
    printf("semaphore Error: %s\n", strerror(sd));
    return -1;
  }
  semop(sd,&sem_1,1);
  shmd=shmget(SHMKEY,sizeof(char *),0);
  if(shmd < 0){
    printf("shared memory Error: %s\n", strerror(shmd));
    return -1;
  }
  fd = open("story.txt",O_WRONLY|O_APPEND);
  if(fd < 0){
    printf("file Error: %s\n", strerror(fd));
    return -1;
  }

  //grabbing last line and requesting new line
  printf("AHHH\n");
  char * lastline = shmat(shmd,0,0);
  printf("Last line: %s\n",lastline);
  char newline[1000];
  printf("Please enter the next line of your story:\n");
  fgets(newline,1000,stdin);
  write(fd,newline,strlen(newline));
  printf("\n");
  close(fd);
  shmdt(data);
  sem_1.sem_op=1;
  semop(sd,&sem_1,1);
  return 0;

}
