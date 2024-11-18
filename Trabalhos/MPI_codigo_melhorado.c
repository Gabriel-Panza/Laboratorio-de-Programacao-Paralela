#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "libMatriz/matriz.h"

// Função para trocar linhas
void swapRows(double** matrix, int N, int row1, int row2) {
    for (int j = 0; j < N + 1; j++) {
        double temp = matrix[row1][j];
        matrix[row1][j] = matrix[row2][j];
        matrix[row2][j] = temp;
    }
}

int main(int argc, char **argv) {
    int rank, size, N;
    double **matrix;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Tamanho para a matriz quadrática = ");
        scanf("%d", &N);
        matrix = criar_matriz(N);
        imprimirMatriz(matrix,N);
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Início da medição de tempo
    double start_time = MPI_Wtime();

    // Distribuir a matriz entre todos os processos
    for (int i = 0; i < N; i++) {
        MPI_Bcast(matrix[i], N + 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    for (int k = 0; k < N-1; k++) {
        // Processo raiz realiza o pivotamento parcial
        if (rank == 0) {
            int maxRow = k;
            for (int i = k + 1; i < N; i++) {
                if (fabs(matrix[i][k]) > fabs(matrix[maxRow][k])) {
                    maxRow = i;
                }
            }

            // Trocar linhas se necessário
            if (maxRow != k) {
                swapRows(matrix, N, k, maxRow);
            }

            // Verifica se o pivô ainda é zero
            if (fabs(matrix[k][k]) < 1e-9) {
                printf("Erro: Matriz singular, pivô na linha %d é zero.\n", k);
                MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
            }
        }

        // Broadcast do pivô e da linha correspondente
        MPI_Bcast(matrix[k], N + 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // Distribuir o pivô para todos os processos
        double pivo;
        double *linha, *linha_pivo;
        if(rank==0){
            pivo = matrix[k][k];
        } 
        MPI_Bcast(&pivo, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        
        // Atualizar as linhas abaixo do pivô
        double fator;
        for (int i = k+1; i < N; i++) {
            if (rank==0){
                fator = matrix[i][k]/pivo;
                linha = matrix[i];
                linha_pivo = matrix[k];
            }
            MPI_Bcast(&fator, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            MPI_Bcast(&linha, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            MPI_Bcast(&linha_pivo, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            for (int j = rank; j < N + 1; j+=size) {
                linha[j] -= fator * linha_pivo[j];
            }
        }
        // Sincronizar a matriz entre os processos
        for (int i = k + 1; i < N; i++) {
            MPI_Bcast(matrix[i], N + 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }
    }

    // Substituição retroativa paralela
    double result[N];
    double local_result;

    for (int i = 0; i < N; i++) {
        result[i] = 0.0;
    }

    for (int i = N - 1; i >= 0; i--) {
        local_result = matrix[i][N];
        for (int j = i + 1; j < N; j++) {
            local_result -= matrix[i][j] * result[j];
        }
        local_result /= matrix[i][i];
        MPI_Allreduce(&local_result, &result[i], 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    }

    // Imprimir a solução no processo raiz
    if (rank == 0) {
        printf("\nSolution:\n");
        for (int i = 0; i < N; i++) {
            printf("x[%d] = %8.4f\n", i, result[i]);
        }
    }

    // Fim da medição de tempo
    double end_time = MPI_Wtime();

    // Exibe o tempo total de execução
    if (rank == 0) {
        printf("\nExecution Time: %f seconds\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}