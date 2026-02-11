#include <stdio.h>
#include <omp.h>

#define N 8

int shared_counter = 0; // shared variable, external node

int main() {
    int a[N];

    // Initialize array
    for (int i = 0; i < N; i++) a[i] = i;

    // First parallel region writes to shared_counter
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        a[i] += 1;
        shared_counter += 1; // <-- potential race
    }

    // Second parallel region also writes to shared_counter
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        a[i] *= 2;
        shared_counter += 1; // <-- same shared variable
    }

    printf("shared_counter = %d\n", shared_counter);
    return 0;
}
