#include <omp.h>  
#include <stdio.h>  
 
int main(int argc, char *argv[]) {
   printf("Fora = %d\n", omp_in_parallel( ));
   printf("Número de processadores = %d\n", omp_get_num_procs( ));
   #pragma omp parallel num_threads(8)  
   {  
  	    int tid = omp_get_thread_num(); 
  	    printf("Olá da thread %d\n", tid);
        printf("Dentro da thread %d = %d\n", tid, omp_in_parallel( )); 
   }
   return(0);
}