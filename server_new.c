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

      printf("\n\n\t\t\033[1;31mS\033[1;32mH\033[1;33mA\033[1;34mR\033[1;35mE\033[1;36mD\033[1;37m M\033[1;38mE\033[1;39mM\033[0;32mO\033[0;33mR\033[0;34mY\033[0m\n");
      printf("\n\033[1;35m[INFO]\033[0m \033[5mServer working.\033[0m\n");
      printf("\033[0m");
      /* Создадим область разделяемой памяти */
      if((shmid = shmget(key, sizeof(Message),PERM | IPC_CREAT)) < 0) {//возвращает дескриптор разделяемой памяти
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
        // Этот системный вызов  и, идентификатор которого задан значением аргумента key(1),
        // и возвращает неотрицательный целочисленный дескриптор. Если key = (макрос) IPC_PRIVATE, системный вызов создает набор семафоров,
        // который будет использоваться исключительно вызывающим процессом для синхронизации родительского и порожденных процессов.
        // Если flag (3) = 0, системный вызов прерывает свою работу, если нет набора семафоров с идентификатором key;
        // в противном случае возвращает дескриптор этого набора семафоров. Если процессу необходимо создать новый набор с идентификатором key,
        // то значение flag должно представлять собой результат побитового сложения константы IPC_CREAT и числовых значений прав доступа для чтения и записи.
        // Значение num_sem(2) может быть равно нулю, если IPC_CREAT в flag не указан, или числу семафоров в создаваемом наборе.
      if((semid = semget(key, 1, PERM | IPC_CREAT)) < 0) {
          perror("\033[1;31m[ERROR] Can not creat sem.");
          printf("\033[0m");
          return 1;
        }

      g = popen("ps -A|grep 'tty'|awk '{print $1}'","r");

      if(!(g)){
           perror("\033[1;31m[ERROR] popen.");
           printf("\033[0m");
           return 1;
       }

       tail = fread(s, 1, sizeof(s), g);
       s[tail] = '\0';
       //printf("%s",s);
       strncpy(msgptr->buff, s, sizeof(s));
       pclose(g);
       /* блокируем*/
       // С помощью этого системного вызова можно изменять значение одного или нескольких семафоров в наборе с дескриптором semfd(1)
       // и/или проверять равенство их значений нулю. Аргумент op (2)— это указатель на массив объектов типа struct sembuf,
       // описанной в заголовке <sys/sem.h>, каждый из которых задает одну операцию (запрос или изменение значения).
       // len (3)— показывает, сколько элементов имеется в массиве, указанном op.
       if(semop(semid,lock,1) < 0){ //op – указатель на массив объектов 1 -  количество элиментов в массиве 1
            perror("\033[1;31m[ERROR] Can not wait client.");
            printf("\033[0m");
            return 1;
       }
       //схему добавить с семоф и один сем
       /* Отключимся от области */
       if(shmdt(msgptr) < 0){
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
       if(semctl(semid, 0, IPC_RMID) < 0) { //0 - индекс номер семафора
            perror("\033[1;31m[ERROR] Can not delete sem.");
            printf("\033[0m");
            return 1;
       }
       printf("\n\033[1;35m[INFO]\033[0m \033[5mClient stop working.\033[0m\n");
       printf("\033[0m");
       return 0;
}
