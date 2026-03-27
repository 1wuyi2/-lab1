#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>


double get_time_ms() {
    LARGE_INTEGER freq, t;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t);
    return (double)t.QuadPart * 1000.0 / freq.QuadPart;
}


#define N  1024         
#define REPEAT 100000    


double sum_normal_unrolled(int *a) {
    double sum = 0.0;
    for (int i = 0; i < N; i += 2) {
        sum += a[i];
        sum += a[i + 1];
    }
    return sum;
}


double sum_2way(int *a) {
    double sum1 = 0.0, sum2 = 0.0;
    for (int i = 0; i < N; i += 2) {
        sum1 += a[i];
        sum2 += a[i + 1];
    }
    return sum1 + sum2;
}


double sum_recursive(int *a) {
    int m = N;
    while (m > 1) {
        for (int i = 0; i < m / 2; ++i) {
            a[i] = a[2 * i] + a[2 * i + 1];
        }
        m /= 2;
    }
    return (double)a[0];
}

int main() {

    int *a = (int*)malloc(N * sizeof(int));
    int *a_work = (int*)malloc(N * sizeof(int));
    if (a == NULL || a_work == NULL) {
        printf("Memory allocation failed!\n");
        return -1;
    }

    for (int i = 0; i < N; ++i) {
        a[i] = 1;
    }

    double t1, t2, t3;
    double res1, res2, res3;


    t1 = get_time_ms();
    for (int k = 0; k < REPEAT; ++k) {
        res1 = sum_normal_unrolled(a);
    }
    t1 = (get_time_ms() - t1) / REPEAT;


    t2 = get_time_ms();
    for (int k = 0; k < REPEAT; ++k) {
        res2 = sum_2way(a);
    }
    t2 = (get_time_ms() - t2) / REPEAT;


    t3 = get_time_ms();
    for (int k = 0; k < REPEAT; ++k) {
        memcpy(a_work, a, N * sizeof(int)); 
        res3 = sum_recursive(a_work);
    }
    t3 = (get_time_ms() - t3) / REPEAT;

 
    printf("=== Experiment 2: Sum of N Numbers ===\n");
    printf("Array Size N = %d, Repeat = %d\n", N, REPEAT);
    printf("Normal (unrolled, serial) result: %.0f | Time: %.6f ms\n", res1, t1);
    printf("2-Way parallel result: %.0f | Time: %.6f ms\n", res2, t2);
    printf("Recursive result: %.0f | Time: %.6f ms\n", res3, t3);
    printf("2-Way Speedup: %.2f x\n", t1 / t2);
    printf("Recursive Speedup: %.2f x\n", t1 / t3);

    free(a);
    free(a_work);
    return 0;
}