```cpp
#include <stdio.h>
#include <omp.h>
#define N 16
#define ITER 5

volatile int loop_id_0 = 0;
volatile int loop_id_1 = 0;
volatile int loop_id_2 = 0;
volatile int loop_id_3 = 0;

int main() {
    double a[N], b[N];

    for (int i = 0; i < N; i++) {
        a[i] = i * 1.0;
        b[i] = 0.0;
        loop_id_0 += 1;
    }

    for (int t = 0; t < ITER; t++) {
        #pragma omp parallel
        for (int i = 1; i < N - 1; i++) {
            b[i] = 0.5 * (a[i - 1] + a[i + 1]);
            loop_id_1 += 1;
        }

        #pragma omp parallel
        for (int i = 0; i < N; i++) {
            a[i] = b[i];
            loop_id_2 += 1;
        }
        loop_id_3 += 1;
    }
    return 0;
}
```

All known SCC groups are hidden, all floating values starting with an `i` are remnants from removing known SCC nodes. However, generating the image and collapsing edges crashes the graphviz tool. 

---

```cpp
for (int i = 0; i < N; i++) {
	a[i] = i * 1.0;
	b[i] = 0.0;
	loop_id_0 += 1;
}
```

![Jacobian Loop 3](media/jacobian-1d-0.svg)

---

```cpp
for (int t = 0; t < ITER; t++) {
	#pragma omp parallel
	for (int i = 1; i < N - 1; i++) {
		b[i] = 0.5 * (a[i - 1] + a[i + 1]);
		loop_id_1 += 1;
	}

	#pragma omp parallel
	for (int i = 0; i < N; i++) {
		a[i] = b[i];
		loop_id_2 += 1;
	}
	loop_id_3 += 1;
}
```
![Jacobian Loop 3](media/jacobian-1d-1.svg)

---
```cpp
for (int i = 1; i < N - 1; i++) {
	b[i] = 0.5 * (a[i - 1] + a[i + 1]);
	loop_id_1 += 1;
}
```

![Jacobian Loop 3](media/jacobian-1d-2.svg)

---

```c++
for (int i = 0; i < N; i++) {
	a[i] = b[i];
	loop_id_2 += 1;
}
```
![Jacobian Loop 3](media/jacobian-1d-3.svg)