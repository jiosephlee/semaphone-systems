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
#define SHMKEY 23445
#define SEMKEY 99911


int main() {
  int shmd,semd,fd;
  struct sembuf sem;
  sem.sem_num=0;
  sem.sem_op = -1;
  sem.sem_flg = SEM_UNDO;

  //catching for errors
  printf("trying to get in...\n");
  semd=semget(SEMKEY,1,0);
  if(semd < 0){
    printf("semaphore Error: %s\n", strerror(semd));
    return -1;
  }
  semop(semd,&sem,1);

  shmd=shmget(SHMKEY,sizeof(int),0);
  if(shmd < 0){
    printf("shared memory Error: %s\n", strerror(shmd));
    return -1;
  }

  fd = open("story.txt",O_WRONLY|O_APPEND);
  if(fd < 0){
    printf("file Error: %s\n", strerror(fd));
    return -1;
  }

  //grabbing last line
  char * data = shmat(shmd,0,0);
  printf("Last line: %s\n",data);
  //grabs new line
  printf("Please enter the next line of your story:\n");
  fgets(data,sizeof(data),stdin);
  write(fd,data,strlen(data));
  printf("\n");
  close(fd);
  shmdt(data);
  sem.sem_op=1;
  semop(semd,&sem,1);
  return 0;

}
