```cpp
#include <stdio.h>

#include <omp.h>

#define N 16
#define ITER 5

int main() {
    double a[N], b[N];

    for (int i = 0; i < N; i++) {
        a[i] = i * 1.0;
        b[i] = 0.0;
    }

    for (int t = 0; t < ITER; t++) {
        #pragma omp parallel
        for
        for (int i = 1; i < N - 1; i++) {
            b[i] = 0.5 * (a[i - 1] + a[i + 1]);
        }

        #pragma omp parallel
        for
        for (int i = 0; i < N; i++) {
            a[i] = b[i];
        }
    }
    return 0;
}
```

All known SCC groups are hidden, all floating values starting with an `i` are remnants from removing known SCC nodes. However, generating the image and collapsing edges crashes the graphviz tool. 

![Jacobian Loop 3](media/jacobian-1d-0.svg)
![Jacobian Loop 3](media/jacobian-1d-1.svg)
![Jacobian Loop 3](media/jacobian-1d-2.svg)
![Jacobian Loop 3](media/jacobian-1d-3.svg)