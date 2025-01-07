#include <stdio.h>
#include <omp.h>
#define ARESTAS 10
#define VERTICES 7

int main(int argc, char *argv[]) { /* omp_atomic.c  */
int i,j,k;
int grau[VERTICES]={0,0,0,0,0,0,0};
int matriz_adj[7][7] = {{0,1,1,0,0,0,0},
                        {1,0,1,1,0,0,0},
                        {1,1,0,1,1,1,0},
                        {0,1,1,0,1,1,0},
                        {0,0,1,1,0,0,1},
                        {0,0,1,1,0,0,0},
                        {0,0,0,0,1,0,0}};
   
    omp_set_num_threads(4);

    #pragma omp parallel for schedule (dynamic) private(j,k)
    for (j = 0; j< VERTICES; j++){
        for (k = 0; k< VERTICES; k++){
            if (matriz_adj[j][k] == 1) {
                #pragma omp atomic
                grau[j]++;
            }  
        }
    }
    for (i = 0; i < VERTICES; i++)
   	    printf("Grau do vértice %d = %d \n",i, grau[i]);
   	return(0);
}
