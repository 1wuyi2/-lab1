#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <math.h>

double get_time_ms() {
    LARGE_INTEGER freq, t;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t);
    return (double)t.QuadPart * 1000.0 / freq.QuadPart;
}

#define N 1024
double b[N][N];
double a[N];
double sum_normal[N];
double sum_opt[N];

void normal() {
    for (int i = 0; i < N; ++i) {
        sum_normal[i] = 0.0;
        for (int j = 0; j < N; ++j) {
            sum_normal[i] += b[j][i] * a[j];
        }
    }
}

void optimized() {
    memset(sum_opt, 0, sizeof(sum_opt));
    for (int j = 0; j < N; ++j) {
        for (int i = 0; i < N; ++i) {
            sum_opt[i] += b[j][i] * a[j];
        }
    }
}

int check() {
    for (int i = 0; i < N; ++i) {
        if (fabs(sum_normal[i] - sum_opt[i]) > 1e-6) return 0;
    }
    return 1;
}

int main() {
    for (int j = 0; j < N; ++j) {
        a[j] = j;
        for (int i = 0; i < N; ++i) {
            b[j][i] = i + j;
        }
    }

    const int repeat =200;
    double t1, t2;

    t1 = get_time_ms();
    for (int k = 0; k < repeat; ++k) normal();
    t1 = (get_time_ms() - t1) / repeat;

    t2 = get_time_ms();
    for (int k = 0; k < repeat; ++k) optimized();
    t2 = (get_time_ms() - t2) / repeat;

    
    printf("=== Experiment 1: Matrix-Vector Multiplication ===\n");
    printf("Matrix Size N = %d\n", N);
    printf("Normal Time: %.4f ms\n", t1);
    printf("Optimized Time: %.4f ms\n", t2);
    printf("Speedup Ratio: %.2f x\n", t1 / t2);

    return 0;
}