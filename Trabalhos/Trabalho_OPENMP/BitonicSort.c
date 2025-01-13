#define THREADS 1

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <limits.h> // Para INT_MAX e INT_MIN

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void bitonic_merge(int *arr, int low, int count, int dir) {
    if (count > 1) {
        int mid = count / 2;

        #pragma omp parallel for
        for (int i = low; i < low + mid; i++) {
            if ((dir == 1 && arr[i] > arr[i + mid]) ||
                (dir == 0 && arr[i] < arr[i + mid])) {
                swap(&arr[i], &arr[i + mid]);
            }
        }
        bitonic_merge(arr, low, mid, dir);
        bitonic_merge(arr, low + mid, mid, dir);
    }   
}

void bitonic_sort(int *arr, int low, int count, int dir) {
    if (count > 1) {
        int mid = count / 2;

        bitonic_sort(arr, low, mid, 1);
        bitonic_sort(arr, low + mid, mid, 0);
        bitonic_merge(arr, low, count, dir);
    }
}

void print_array(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

// Função para calcular a próxima potência de 2
int next_power_of_two(int n) {
    int power = 1;
    while (power < n) {
        power *= 2;
    }
    return power;
}

int main() {
    int N;
    printf("Digite o número de elementos: ");
    scanf("%d", &N);

    double start_time, end_time;

    int next_power = next_power_of_two(N);
    int *arr = (int *)malloc(next_power * sizeof(int));

    srand(time(NULL));
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        arr[i] = rand() % 500000;
    }

    // Preenche o restante com valores sentinelas (INT_MAX para crescente)
    for (int i = N; i < next_power; i++) {
        arr[i] = INT_MAX;
    }

    printf("Array original:\n");
    print_array(arr, N);

    omp_set_num_threads(THREADS);
    start_time = omp_get_wtime();

    // Particionar o vetor e ordenar cada parte
    int chunk_size = next_power / THREADS;

    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int start = thread_id * chunk_size;
        bitonic_sort(arr, start, chunk_size, 1);
    }

    // Mesclar as partes ordenadas
    for (int step = chunk_size; step < next_power; step *= 2) {
        for (int i = 0; i < next_power; i += 2 * step) {
            bitonic_merge(arr, i, 2 * step, 1);
        }
    }

    end_time = omp_get_wtime();

    printf("Array ordenado:\n");
    print_array(arr, N);
    printf("Tempo de execução: %f segundos\n", end_time - start_time);

    free(arr);
    return 0;
}