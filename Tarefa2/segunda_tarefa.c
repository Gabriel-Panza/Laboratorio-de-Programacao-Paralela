#include <stdio.h>
#include <string.h>
#include "mpi.h"

int main(int argc, char *argv[]) { /* mpi_funcoes.c  */
    int meu_ranque, num_procs, origem, destino, etiq=0, ret; 
    char mensagem[100];
    MPI_Status estado;
    
    /* Inicia o MPI. Em caso de erro aborta o programa */
    ret = MPI_Init(&argc, &argv);
    if (ret != MPI_SUCCESS) {
        printf("Erro ao iniciar o programa MPI. Abortando.\n");
        MPI_Abort(MPI_COMM_WORLD, ret);
    }
    /* Obtém o ranque e número de processos em execução */
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    
    if (meu_ranque != 0){
        sprintf(mensagem, "Processo %d esta vivo!", meu_ranque);
        destino = 0;
        MPI_Send(mensagem, strlen(mensagem)+1, MPI_CHAR, destino, etiq, MPI_COMM_WORLD);
    }
    else{
        for (origem=1; origem<num_procs; origem++){
            MPI_Recv(mensagem, 100, MPI_CHAR, MPI_ANY_SOURCE, etiq, MPI_COMM_WORLD, &estado);
            printf("%s\n", mensagem);
        }
    }
    /* Finaliza o MPI */
    MPI_Finalize();
    return(0);
}
