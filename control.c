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

union semun us;
struct sembuf sb;

int createStory(){
  printf("creating story...\n\n");
  // create file
  int fd = open("story.txt", O_CREAT | O_TRUNC, 0640);
  if(fd < 0) {
    printf("failed to create a file\n");
    printf("error: %s\n", strerror(errno));
    return -1;
  } else {
    printf("file created\n");
  }
  // create semaphore
  int sd = semget(SEMKEY, 1, IPC_CREAT | IPC_EXCL | 0644);
  if(sd < 0) {
    printf("failed to create a semaphore\n");
    printf("error: %s\n", strerror(errno));
    return -1;
  } else {
    printf("semaphore created\n");
  }
  // create shared Memory
  int shmd = shmget(SHMKEY, sizeof(shmd), IPC_CREAT | IPC_EXCL | 0640);
  if(shmd < 0) {
    printf("failed to create shared memory\n");
    printf("error: %s\n", strerror(errno));
    return -1;
  } else {
    printf("shared memory created\n");
  }
}

int viewStory(){
  int fd = open("story.txt",O_RDONLY);
  if ( fd < 0 ){
    printf("error: %s\n", strerror(fd));
    return -2;
  }
  char story[1000]="\0";
  read(fd,story,1000);
  printf("printing the story so far...\n");
  printf("%s\n",story);
  close(fd);
}

int removeStory(){
      // Print Contents
      viewStory();
      remove("story.txt");
      //Get Semaphore
      int sd = semget(SEMKEY, 1, 0640);
      if(sd < 0){
        printf("semaphore error: %s\n", strerror(sd));
        return -3;
      }
      semop(sd,&sb,1);
      int shmd = shmget(SHMKEY, sizeof(shmd), 0640);
      if (shmd < 0) {
        printf("shared memory error: %s\n", strerror(sd));
        return -3;
      }

      //Remove Shared Memory
      shmctl(shmd, IPC_RMID, 0);
      printf("removed shared memory\n");
      //Remove Semaphore
      semctl(sd, 0, IPC_RMID, 0);
      printf("removed semaphores\n");
    return 1;
}

int execute (char *args[]){
  int debug = 1;
  if(strcmp(args[1],"-c")==0){
    debug = createStory();
  } else if(strcmp(args[1],"-r")==0){
    debug = removeStory();
  } else if(strcmp(args[1],"-v")==0){
    debug = viewStory();
  } else{
      printf("command not found\n");
      debug = -1;
  }
  return debug;
}

int main(int argc, char *argv[]) {
  us.val=1;
  sb.sem_num=0;
  sb.sem_op =-1;
  if(argc <= 1) {
    printf("%s\n", "You may access this story by using the following flags...");
    printf("%s\n", "\"-c\" to create story");
    printf("%s\n", "\"-r\" to remove the story's contents");
    printf("%s\n", "\"-v\" to read story's contents");
  } else {
    int executed = execute(argv);
  }
  return 0;
}
