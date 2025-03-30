#include <iostream>

extern "C" void multiply(int* A, int* B, int* C, int M, int N, int K);

int main() {
    const int M = 2, N = 3, K = 2;
    int A[M * N] = {1, 2, 3, 4, 5, 6};
    int B[N * K] = {7, 8, 9, 10, 11, 12};
    int C[M * K] = {0};

    multiply(A, B, C, M, N, K);
    return 0;
}

