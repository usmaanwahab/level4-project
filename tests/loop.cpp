#include <stdio.h>
#include <omp.h>

int main() {
    int n = 10;
    int arr[n];
    int sum = 0;

    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        arr[i] = i * 2;
    }
    
    return 0;
}