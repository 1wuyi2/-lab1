#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

// 高精度计时函数（毫秒）
double get_time_ms() {
    LARGE_INTEGER freq, t;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t);
    return (double)t.QuadPart * 1000.0 / freq.QuadPart;
}

// 实验参数
#define N  1024         // 数组大小（2的幂）
#define REPEAT 100000    // 重复执行次数，提高计时精度

// 平凡算法：2路循环展开，但保持串行数据依赖（单累加器）
double sum_normal_unrolled(int *a) {
    double sum = 0.0;
    for (int i = 0; i < N; i += 2) {
        sum += a[i];
        sum += a[i + 1];
    }
    return sum;
}

// 2路并行算法：两个独立累加器，消除数据依赖
double sum_2way(int *a) {
    double sum1 = 0.0, sum2 = 0.0;
    for (int i = 0; i < N; i += 2) {
        sum1 += a[i];
        sum2 += a[i + 1];
    }
    return sum1 + sum2;
}

// 递归分治算法：原地归并（两两相加，逐层缩小）
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
    // 分配两个数组：原始数据和工作副本
    int *a = (int*)malloc(N * sizeof(int));
    int *a_work = (int*)malloc(N * sizeof(int));
    if (a == NULL || a_work == NULL) {
        printf("Memory allocation failed!\n");
        return -1;
    }

    // 初始化数组（简单值，可自行修改）
    for (int i = 0; i < N; ++i) {
        a[i] = 1;
    }

    double t1, t2, t3;
    double res1, res2, res3;

    // 1. 平凡算法（展开但串行依赖）
    t1 = get_time_ms();
    for (int k = 0; k < REPEAT; ++k) {
        res1 = sum_normal_unrolled(a);
    }
    t1 = (get_time_ms() - t1) / REPEAT;

    // 2. 2路并行算法
    t2 = get_time_ms();
    for (int k = 0; k < REPEAT; ++k) {
        res2 = sum_2way(a);
    }
    t2 = (get_time_ms() - t2) / REPEAT;

    // 3. 递归分治算法（每次使用原始数据副本，保证公平）
    t3 = get_time_ms();
    for (int k = 0; k < REPEAT; ++k) {
        memcpy(a_work, a, N * sizeof(int));  // 复制初始数据
        res3 = sum_recursive(a_work);
    }
    t3 = (get_time_ms() - t3) / REPEAT;

    // 输出结果
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