#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

void  ClientProcess(int []);

int  main(int  argc, char *argv[])
{
  int    ShmID;
  int    *ShmPTR;
  pid_t  pid;
  int    status;
  srandom(getpid());
  
  /*
  if (argc != 5) {
      printf("Use: %s #1 #2 #3 #4\n", argv[0]);
      exit(1);
  }
  */

  ShmID = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
  if (ShmID < 0) {
      printf("*** shmget error (server) ***\n");
      exit(1);
  }
  printf("Server has received a shared memory of two integers...\n");

  ShmPTR = (int *) shmat(ShmID, NULL, 0);
  if (*ShmPTR == -1) {
      printf("*** shmat error (server) ***\n");
      exit(1);
  }
  printf("Server has attached the shared memory...\n");

  ShmPTR[0] = 0;
  ShmPTR[1] = 0;
  

  //printf("Server has filled %d, %d in shared memory...\n", ShmPTR[0], ShmPTR[1]);
  printf("Server is about to fork a child process...\n");
  
  pid = fork();
  if (pid > 0) {
    int j;
    printf("Orig Bank Account = %d \n", ShmPTR[0]);

    for (j = 0; j < 25; j++){
      sleep(random() % 6);
      //int account = ShmPTR[0];
      //int turn = ShmPTR[1];
      while (ShmPTR[1] != 0);
      if (ShmPTR[0] <= 100){
        int balance = random() % 101;
        //printf("time for deposit: %d \n", balance);
        if (balance % 2 == 0){
          ShmPTR[0] += balance;
          printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, ShmPTR[0]);
        }
        else{
          printf("Dear old Dad: Doesn't have any money to give\n");
        }
      }
      else{
        printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", ShmPTR[0]);
      }
      ShmPTR[1] = 1;
    }
  }
  else if (pid == 0) {
      ClientProcess(ShmPTR);
      exit(0);
  }
  else{
    printf("*** fork error (server) ***\n");
    exit(1);
  }

  wait(&status);
  printf("Server has detected the completion of its child...\n");
  shmdt((void *) ShmPTR);
  printf("Server has detached its shared memory...\n");
  shmctl(ShmID, IPC_RMID, NULL);
  printf("Server has removed its shared memory...\n");
  printf("Server exits...\n");
  exit(0);
}

void  ClientProcess(int  SharedMem[])
{
  //printf("   Client process started\n");
  //printf("   Client found %d %d in shared memory\n", SharedMem[0], SharedMem[1]);
  int i;
  for (i = 0; i < 25; i++){
    int balance = rand () % 51;
    sleep(random() % 6);
    while (SharedMem[1] != 1);
      printf("    Poor Student needs $%d\n", balance);
      if (balance <= SharedMem[0]){
        SharedMem[0] -= balance;
        printf("    Poor Student: Withdraws $%d / Balance = $%d\n", balance, SharedMem[0]);
      }
      else{
        printf("    Poor Student: Not Enough Cash ($%d)\n", SharedMem[0]);
      }
    SharedMem[1] = 0;
  }
  printf("   Client is about to exit\n");
}