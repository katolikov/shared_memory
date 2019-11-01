#include <sys/types.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include "shmem.h"


int main(){

      Message* msgptr;
      key_t key = 2002;
      int shmid, semid;
      FILE *g;
      int tail;
      char s[MAXBUFF];

      printf("\n\033[1;35m[INFO] Sever working.\n");
      printf("\033[0m");
      /* Создадим область разделяемой памяти */
      if((shmid = shmget(key, sizeof(Message),PERM | IPC_CREAT)) < 0) {
          perror("\033[1;31m[ERROR] Can't create memory.");
          printf("\033[0m");
          return 1;
        }
        /* Присоединим ее */
      if((msgptr = (Message*)shmat(shmid, 0, 0)) < 0) {
          perror("\033[1;31m[ERROR] Conneting error.");
          printf("\033[0m");
          return 1;
        }
     /* Создадим группу из двух семафоров:
        Первый семафор - для синхронизации работы
        с разделяемой памятью. Второй семафор -
        для синхронизации выполнения процессов */
      if((semid = semget(key, 2, PERM | IPC_CREAT)) < 0) {
          perror("\033[1;31m[ERROR] Can not creat sem.");
          printf("\033[0m");
          return 1;
        }

      g = popen("ps -A|grep 'tty'| awk '{print $1}'|cat","r");
      if(!(g)){
           perror("\033[1;31m[ERROR] popen.");
           printf("\033[0m");
           return 1;
       }
       
       tail = fread(s, 1, sizeof(g), g);
       //printf("%lu\n", sizeof(g));
       s[tail] = '\0';
       pclose(g);
       strncpy(msgptr->buff, s, sizeof(s));

       /* Ждем, пока клиент начнет работу и заблокирует разделяемую память */
       if(semop(semid, &proc_wait[0], 1) < 0) {
            perror("\033[1;31m[ERROR] Can not wait client.");
            printf("\033[0m");
            return 1;
       }
       /* Отключимся от области */
       if(shmdt(msgptr) < 0) {
            perror("\033[1;31m[ERROR] shmdt.");
            printf("\033[0m");
            return 1;
       }
       /* Удалим созданные объекты IPC */
       if(shmctl(shmid, IPC_RMID, 0) < 0) {
            perror("\033[1;31m[ERROR] Can not delete memory.");
            printf("\033[0m");
            return 1;
       }
       if(semctl(semid, 0, IPC_RMID) < 0) {
            perror("\033[1;31m[ERROR] Can not delete sem.");
            printf("\033[0m");
            return 1;
       }
       printf("\n\033[1;35m[INFO] Client stop working.\n");
       printf("\033[0m");
       return 0;
}
