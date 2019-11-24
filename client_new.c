#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
#include "shmem.h"

int main(){

     Message *msgptr;
     key_t key = 2002;
     int shmid, semid;
     char s[MAXBUFF];
     int fd;
     FILE *g;
     struct shmid_ds buf;

     printf("\n\033[1;35m[INFO]\033[0m \033[5mClient working.\033[0m\n");
     /* Получим доступ к разделяемой памяти */
     if ((shmid = shmget(key, sizeof(Message), 0)) < 0) {
         perror("\033[1;31m[ERROR] shmget.");
         printf("\033[0m");
         return 1;
     }
     /* Присоединим ее */
     if ((msgptr = (Message*)shmat(shmid, 0, 0)) < 0) {
         perror("\033[1;31m[ERROR] shmat.");
         printf("\033[0m");
         return 1;
     }
     /* Получим доступ к семафору */
     if ((semid = semget(key, 1, PERM)) < 0) {
         perror("\033[1;31m[ERROR] semget.");
         printf("\033[0m");
         return 1;
     }
     /* разаблокируем разделяемую память */
      if (semop(semid,unlock,1) < 0) {
         perror("\033[1;31m[ERROR] Block memory error.");
         printf("\033[0m");
         return 1;
     }
    
     if(shmctl(shmid, IPC_STAT, &buf) < 0){
         perror("\033[1;31m[ERROR] shmctl.");
         printf("\033[0m");
         return 1;
     }

     strncpy(s, msgptr->buff, MAXBUFF);

     if(strlen(s) < 1){
         printf("\033[1;31m[ERROR] Memory are empty.\n");
         printf("\033[0m");
         return 1;
     }

     if(access("haram", F_OK)==-1){

          printf("\n\033[1;35m[INFO]\033[0m Number pros connecting to memory: \033[1;35m%hu\n\n", buf.shm_nattch);
          fd = creat("haram",0666);
          write(fd,s,strlen(s));
          printf("| STAT | COMMAND |\n");
          system("ps -eAo pid,stat,command| grep -f haram|awk '{print $2,$3, $4, $5}'");
          printf("\033[0m");
          system("rm haram");
          close(fd);

    }
    else printf("\033[1;35m[INFO] Rename your file: haram\n");

    printf("\n\033[1;35m[INFO]\033[0m PROCESS STATE CODES\n\nD \033[1;35muninterruptible sleep (usually IO)\n\033[0mR \033[1;35mrunning or runnable (on run queue)\n\033[0mS \033[1;35minterruptible sleep (waiting for an event to complete)\n\033[0mT \033[1;35mstopped, either by a job control signal or because it is being traced.\n\033[0mW \033[1;35mpaging (not valid since the 2.6.xx kernel)\n\033[0mX \033[1;35mdead (should never be seen)\n\033[0mZ \033[1;35mdefunct ('zombie') process, terminate but not reaped by its parent.\n\nFor \033[0mBSD\033[1;35m formats and when the stat keyword is used, additional characters may\n\033[0m<\033[1;35m high-priority (not nice to other users)\n\033[0mN\033[1;35m low-priority (nice to other users)\n\033[0mL\033[1;35m has pages locked into memory (for real-time and custom IO)\n\033[0ms\033[1;35m is a session leader\n\033[0ml\033[1;35m is multi-threaded (using CLONE_THREAD, like NPTL pthreads do)\n\033[0m+\033[1;35m is in the foreground process group.\n\n");

    /* Отключимся от области */
    if (shmdt(msgptr) < 0) {
        perror("\033[1;31m[ERROR] shmdt.");
        printf("\033[0m");
        return 1;
     }
    printf("\033[0m");
    return 0;
}
