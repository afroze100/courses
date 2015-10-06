#include <sys/types.h>

#include <stdio.h>

#include <string.h>

#include <stdlib.h>

#include <unistd.h>


#define NUM_CHILD 4




static int arr_size;


int* A;

int* B;

int* C;




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



int main (void)

{		
	
srand(time(NULL));

	
printf("Enter the size of the arrays\n");
	
scanf("%d", &arr_size);


A = (int*) malloc(arr_size * sizeof(int));
	
B = (int*) malloc(arr_size * sizeof(int));	
	
C = (int*) malloc(arr_size * sizeof(int));

	

init();	
	
	

printf("The initial arrays are\n");	
	
printarray('A',A);
	
printarray('B',B);


	/*Write your code here*/
        
int pfd1[2];                  //4 pipes
        
int pfd2[2];
        
int pfd3[2];
        
int pfd4[2]; 

     
       
        
        
        
        
int prid[4];
        
int i;
        
int parid;
        

parid=getpid();                //saving parent id





        

if (pipe(pfd1) == -1)
        
{
        
perror("pipe failed");
          
exit(1);
        
}


        

if (pipe(pfd2) == -1)
        
{
        
perror("pipe failed");
          
exit(1);
        
} 
        

if (pipe(pfd3) == -1)
        
{
        
perror("pipe failed");
          
exit(1);
        
} 

        

if (pipe(pfd4) == -1)
        
{
        
perror("pipe failed");
          
exit(1);
        
}
        

       
        

for(i=0;i<4;i++)                //saving ids of child processes
        
{
       
        
if ((prid[i] = fork()) < 0)
        
{
         
perror("fork failed");
         
exit(1);
        
}
        
       
if(prid[i]==0)
        
{
        
prid[i]=getpid();
        
break;
        
}
       
             
        
}
        

       
     
      


if((getpid()==parid))//only parent will do this
                          
{                                    
          
close(pfd1[1]);                     
          
i=0;                               
                               
read(pfd1[0], C, arr_size);
            
printf("\nparent received following data through pipe1\n");

printarray('C',C);
         
          
close(pfd1[0]);
          
        
}
      

     
       

if((getpid()==parid))      //only parent will do this
         
        
{
          
close(pfd2[1]);
          
           
          
read(pfd2[0], C+((arr_size)/4),arr_size);
          
printf("parent received following data through pipe2\n");
          
         

           
printarray('C',C);
         
          
close(pfd2[0]);
         
         
        
}
        
      
        
          

if((getpid()==parid))        //only parent will do this
         
        
{
          
close(pfd3[1]);
          
          
          
read(pfd3[0], (C+((arr_size)/2)), arr_size);
           
printf("parent received following data through pipe3\n");
          
       
           
printarray('C',C);
          
close(pfd3[0]);
      
         
        
}

       

         
        

if((getpid()==parid))          //only parent will do this
         
        
{
         
close(pfd4[1]);
          
          
          
{ 
          
          
read(pfd4[0], C+(3*(arr_size)/4), arr_size);
          
printf("parent received following data through pipe4\n");
          
         

           
printarray('C',C);
          
}
          
close(pfd4[0]);
          
         
        
}
       
       



        


if((getpid()==prid[0]))        //child 1
        
{
        
close(pfd1[0]);
               
        
for(i=0;i<(arr_size/4);i++) 
        
{
        C[i]=A[i]+B[i];
       
        
}
       
        
write(pfd1[1], C, arr_size);
        
close(pfd1[1]);
 
printf("\nchild process 1 with pid = %d,submitted my work through pipe1",prid[0]);
        
exit(1);
        
}   





       
        

if((getpid()==prid[1]))                    //child 2
        
{
        close(pfd2[0]);
               
        
for(i=(arr_size/4);i<(arr_size/2);i++) 
        
C[i]=A[i]+B[i];

        
write(pfd2[1], C+(arr_size/4),arr_size);
       
        
close(pfd2[1]);
 
printf("\nchild process 2 with pid = %d, submitted my work through pipe 2",prid[1]);

      

exit(1);

        
}  









       
         
        

if((getpid()==prid[2]))                //child 3
        
{
        
close(pfd3[0]);
               
        
for(i=(arr_size/2);i<(3*(arr_size)/4);i++) 
        
C[i]=A[i]+B[i];

        
write(pfd3[1],C+(arr_size/2), arr_size);

        
close(pfd3[1]);

printf("\nchild process 3 with pid = %d, submitted my work through pipe 3",prid[2]);
       
       
        
exit(1);
        
}      
        




        
        

if((getpid()==prid[3]))                      //child 4
        
{
        
close(pfd4[0]);
               
        
for(i=3*(arr_size)/4;i<(arr_size);i++) 
        
C[i]=A[i]+B[i];

        
write(pfd4[1], C+(3*(arr_size)/4),arr_size);
        
close(pfd4[1]);

printf("\nchild process 4 with pid = %d, submitted my work through pipe 4",prid[3]);
       
        
exit(1);
        
        
}      
       
 
     
      
      
      


if(getpid()==parid)                    //parent displays final array
      
printf("parent received final array\n");
      
printarray('C',C);
     
      

     
     

return 0;

}
