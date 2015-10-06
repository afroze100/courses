#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int* A;

int* B;

int* C;
static int arr_size;
int* shm_ptr;

void init ()

{

int i;
	
for(i=0; i < arr_size; i++)
	
{	
A[i] = rand() % 10;
		
B[i] = rand() % 10;
	
}

}


void printarray(char name, int* array)

{
 int i;	
	
printf("%c = ", name);
	
for(i = 0; i < arr_size; i++)
		
printf("%d, ",array[i]);
	
printf("\n");

}


int main(void)
{
pid_t pid[4];
int i;
int shm_id;      /* shared memory ID */
int w,x,y,z;
srand(time(NULL));

/////////////////////////////////////////////////////////////////////
	
printf("Enter the size of the arrays\n");
scanf("%d", &arr_size);

shm_id = shmget(IPC_PRIVATE, arr_size*sizeof(int), IPC_CREAT | 0666);  ///// requesting for shared memory
shm_ptr = (int *) shmat(shm_id, NULL, 0);                              ///// attaching to shared memory
	
A = (int*) malloc(arr_size * sizeof(int));
	
B = (int*) malloc(arr_size * sizeof(int));	
	
C = (int*) malloc(arr_size * sizeof(int));

init();	
	w = arr_size/4;
        x = arr_size/2;
        y = arr_size*(3/4);
        z = arr_size;

printf("The initial arrays are\n");	
printarray('A',A);
printarray('B',B);


if (shm_id < 0)
{
printf("shmget error\n");
exit(0);
}

for(i=0;i<4;i++)
{

pid[i]=fork();

if(pid[i]==0)          /* condition for child process */
{
   break;
}
else
 
wait();         // Parent put to Sleep....to allow child processes to complete first!!
}

//////////////////////////////////////////////////////////// FIRST CHILD

if(pid[0]==0)
{ 
  C=shm_ptr;
 for(i=0;i<w;i++)
   { 
     C[i] = A[i]+B[i];
     shm_ptr++;                   ///// the pointer increments one step
   }
printf("I am child number 0 with pid %d, and i have completed my task\n",getpid());
exit(0);
}

//////////////////////////////////////////////////////////// SECOND CHILD

if(pid[1]==0)
{ 
 C=shm_ptr;
 for(i=w;i<x;i++)
   { 
     C[i] = A[i]+B[i];
     shm_ptr++;
   } 
printf("I am child number 1 with pid %d, and i have completed my task\n",getpid());
exit(0);
}

//////////////////////////////////////////////////////////// THIRD CHILD

if(pid[2]==0)
{ 
C=shm_ptr;
 for(i=x;i<y;i++)
   { 
     C[i] = A[i]+B[i];
      shm_ptr++;
   }
printf("I am child number 2 with pid %d, and i have completed my task\n",getpid());
exit(0);
}

///////////////////////////////////////////////////////////// FOURTH CHILD

if(pid[3]==0)
{ 
C=shm_ptr;
 for(i=y;i<z;i++)
   { 
     C[i] = A[i]+B[i];
     shm_ptr++;
   }
printf("I am child number 3 with pid %d, and i have completed my task\n",getpid());
exit(0);
}

/////////////////////////////////////////////////////////BACK IN PARENT PROCESS

printarray('C',shm_ptr);
shmdt (shm_ptr);
shmctl (shm_id, IPC_RMID, NULL);
exit(0);
}

