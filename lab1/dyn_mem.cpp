// Kompilacja g++ -O3 dyn_mem.cpp -o dynamic
// O2 - umiarkowana optymalizacja
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <string>
#include <cstdlib>

using T = int;

void func(int tid, T** A, T** B, T** C, int N, int num_threads) {
    int chunk = N / num_threads;
    int lb = tid * chunk;
    int ub = (tid == num_threads - 1) ? N : lb + chunk;

    for (int i = lb; i < ub; ++i) {
        for (int j = 0; j < N; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < N; ++k) {
                C[i][j] += A[i][k] * B[k][j]; // ❗ brak transpozycji
                //T sum = 0;                    // <---- Lekka optymalizacja dostępności pamięci
                //for (int k = 0; k < N; ++k) {
                //    sum += A[i][k] * B[k][j];
                //}
                //C[i][j] = sum;

            }
        }
    }
}

int main(int argc, char** argv) {

    if (argc < 3) {
        std::cout << "Usage: program N num_threads\n";
        return 1;
    }

    int N = std::stoi(argv[1]);
    int num_threads = std::stoi(argv[2]);

    std::vector<std::thread> th;

    T** A = new T * [N];
    T** B = new T * [N];
    T** C = new T * [N];

    //a = new T[N * N]();
    //b = new T[N * N]();
    //c = new T[N * N]();

    T* a = (T*)aligned_alloc(64, N * N * sizeof(T));    // Zrobić też bez aligned
    T* b = (T*)aligned_alloc(64, N * N * sizeof(T));
    T* c = (T*)aligned_alloc(64, N * N * sizeof(T));

    for (int i = 0; i < N; ++i) {
        A[i] = a + i * N;
        B[i] = b + i * N;
        C[i] = c + i * N;
    }

    // inicjalizacja
    for (int i = 0; i < N * N; ++i) {
        a[i] = 1;
        b[i] = 1;
        c[i] = 0;
    }

    const auto start = std::chrono::steady_clock::now();

    // mnożenie (bez transpozycji)
    for (int i = 0; i < num_threads; ++i) {
        th.emplace_back(func, i, A, B, C, N, num_threads);
    }

    for (auto& t : th) t.join();

    const auto finish = std::chrono::steady_clock::now();

    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << "Time: " << time.count() << " ms\n";

    delete[] A;
    delete[] B;
    delete[] C;

    free(a);
    free(b);
    free(c);

    return 0;
}