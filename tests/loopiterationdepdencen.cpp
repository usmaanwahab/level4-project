#include <stdio.h>
#include <omp.h>

int main() {
    int n = 10;
    int arr[n];
    int sum = 0;

    arr[0] = 0;
    #pragma omp parallel for
    for (int i = 1; i < n; i++) {
        arr[i] = (arr[i-1] + i) * 2;
    }
    
    return 0;
}