#include <stdio.h>
#include <omp.h>
#define N 256

int main()
{
    int A[N];
    int B[N];

    for (int i = 0; i < N; i++)
    {
        A[i] = i;
        B[i] = i * 2;
    }

    int *p = A;
    int *q = A;

    #pragma omp parallel for
    for (int i= 0; i < N; i++) {
        p[i] = p[i] + 1;
        q[i] = q[i] + p[i];
    }
    return 0;
}