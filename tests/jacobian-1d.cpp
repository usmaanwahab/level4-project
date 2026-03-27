#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 1000
#define ITER 10

int main()
{
    double *a = (double *)malloc(sizeof(double) * N);
    double *b = (double *)malloc(sizeof(double) * N);

    for (int i = 0; i < N; i++)
    {
        a[i] = i * 1.0;
        b[i] = 0.0;
    }

    for (int t = 0; t < ITER; t++)
    {
#pragma omp parallel for
        for (int i = 1; i < N - 1; i++)
        {
            b[i] = 0.5 * (a[i - 1] + a[i + 1]);
        }

#pragma omp parallel for
        for (int i = 0; i < N; i++)
        {
            a[i] = b[i];
        }
    }
    free(a);
    free(b);
    return 0;
}