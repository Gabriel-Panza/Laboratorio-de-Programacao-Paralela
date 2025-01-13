#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

int main(int argc, char **argv) {
    int N, nthreads;
    long int i;
    double dot = 0.0;
    clock_t start, end;
    
    /* The number of threads is the first input parameter */
    nthreads = atoi(argv[1]);
    
    /* The number of elements is the second input parameter */
    N = atoi(argv[2]);
    int* a = (int*) malloc(sizeof(int)*N);
    int* b = (int*) malloc(sizeof(int)*N);
    
    /* could be read from a file */
    for ( i = 0; i < N; i++){
        a[i] = 2;
        b[i] = 5;
    }

    start = clock();
    /* Multiplication parallel for */
    #pragma omp simd reduction(+:dot)
    for(i= 0; i< N; i++){
        dot += a[i] * b[i];
    }
    end = clock();
    
    double time_taken = (double)(end - start) / (double)CLOCKS_PER_SEC;
    printf("Resultado: dot = %9.3f com tempo de %f segundos\n", dot, time_taken);
    
    free(a);
    free(b);
}