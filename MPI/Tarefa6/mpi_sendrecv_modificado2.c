#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) { /* mpi_sendrecv.c */
    int meu_ranque, num_procs;
    int dados, dados_locais;
    int destino, origem, etiq = 0;
    MPI_Status estado;
    int raiz = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    
    /* Se o número de processos não for igual a 3, então aborta */
    if (num_procs != 3) { 
        if (meu_ranque == raiz)
            printf("Por favor, execute com exatamente 3 processos\n");
        MPI_Finalize();
        exit(0);
    }

    /* Valor a ser enviado */
    dados = meu_ranque * 2 + 1;
    printf("Processo %d: tem dado %d\n", meu_ranque, dados);

    /* Determina o destino e a origem */
    destino = (meu_ranque + 2) % 3;
    origem = (meu_ranque - 2 + 3) % 3;

    if (meu_ranque == 0) {
        MPI_Sendrecv(&dados, 1, MPI_INT, destino, etiq, &dados_locais, 1, MPI_INT, origem, etiq, MPI_COMM_WORLD, &estado);
    } else if (meu_ranque == 2) {
        MPI_Recv(&dados_locais, 1, MPI_INT, origem, etiq, MPI_COMM_WORLD, &estado);
        if (dados > dados_locais)
            dados_locais = dados;
    } else if (meu_ranque == 1) {
        if (dados_locais < dados) 
            dados_locais = dados;
        MPI_Send(&dados_locais, 1, MPI_INT, destino, etiq, MPI_COMM_WORLD);
    }

    printf("Processo %d: Possui o valor %d no final de tudo!\n", meu_ranque, dados_locais);

    MPI_Finalize();
    return 0;
}
