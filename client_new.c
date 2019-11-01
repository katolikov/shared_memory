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

     printf("\n\033[1;35m[INFO] Client working.\n");
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
     if ((semid = semget(key, 2, PERM)) < 0) {
         perror("\033[1;31m[ERROR] semget.");
         printf("\033[0m");
         return 1;
     }
     /* Заблокируем разделяемую память */
     if (semop(semid, &mem_lock[0], 2) < 0) {
         perror("\033[1;31m[ERROR] Block memory error.");
         printf("\033[0m");
         return 1;
     }

     /* Уведомим сервер о начале работы */
     if (semop(semid, &proc_start[0], 1) < 0) {
         perror("\033[1;31m[ERROR] Server don't understand.");
         printf("\033[0m");
         return 1;
     }

     /* Освободим разделяемую память */
     if(shmctl(shmid, IPC_STAT, &buf) < 0){
         perror("\033[1;31m[ERROR] shmctl.");
         printf("\033[0m");
         return 1;
     }

     strncpy(s, msgptr->buff, strlen(s));

     if(strlen(s) < 1){
         printf("\033[1;31m[ERROR] Memory are empty.\n");
         printf("\033[0m");
         return 1;
     }
     if (semop(semid, &mem_unlock[0], 1) < 0){
         perror("\033[1;31m[ERROR] Unlock memory error.");
         printf("\033[0m");
         return 1;
     }

     if(access("haram", F_OK)==-1){

          printf("\n\033[1;35m[INFO] Number pros connecting to memory: %hu\n", buf.shm_nattch);
          fd = creat("haram",0666);
          write(fd,s,strlen(s));
          printf("|STAT|\n");
          system("ps -Al | grep -f haram | awk '{print $11}'|sed 's/.$//'");
          system("rm haram");
          close(fd);

      }
    else printf("\033[1;35m[INFO] Rename your file: haram\n");
     /* Отключимся от области */
    if (shmdt(msgptr) < 0) {
        perror("\033[1;31m[ERROR] shmdt.");
        printf("\033[0m");
        return 1;
     }
    printf("\033[0m");
    return 0;
}
