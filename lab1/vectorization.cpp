// Jak kompilowaæ: 
// g++ -O3 -march=native -fopenmp vectorization.cpp 
// icpx -O3 -march=core-avx2 -Rpass=loop-vectorize -Rpass-analysis=loop-vectorize mm.cpp  -qopt-report=max -qopt-report-phase=vec
// g++ -O3 -fopenmp -march=native -fopt-info-vec-optimized -fopt-info-vec-missed mm.cpp
// clang++ -O3 -march=native   -Rpass=loop-vectorize   -Rpass-missed=loop-vectorize   -Rpass-analysis=loop-vectorize   main_dyn2.cpp -o program 

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <string>
#include <cstdlib>

using T = int;


void func(const int tid, T* a, T* bt, T* c, int N, int num_threads) {     // Sprawdziæ jeszcze __restrict__
    int lb = tid * (N / num_threads);	// lower bound
    int ub = lb + (N / num_threads) - 1;	// upper bound

    for (int i = lb; i < ub; ++i) {
        for (int j = 0; j < N; ++j) {

            T sum = 0;

            T* rowA = a + i * N;
            T* rowBT = bt + j * N;

            #pragma omp simd reduction(+:sum)       // w³¹czyæ/wy³¹czyæ t¹ linijkê
            for (int k = 0; k < N; ++k) {
                sum += rowA[k] * rowBT[k];
            }

            c[i * N + j] = sum;
        }
    }

}


int main(int argc, char** argv) {       // Sprawdziæ te¿ z podwójnym wskaŸnikiem

    if (argc < 3) {
        std::cout << "Usage: program N num_threads\n";
        return 1;
    }

    const int num_threads = std::stoi(argv[2]);
    const int N = std::stoi(argv[1]);

    std::vector<std::thread> th;

    //T** A, ** B, ** C, ** BT;
    //A = new T * [N]();
    //B = new T * [N]();
    //C = new T * [N]();
    //BT = new T * [N]();

    T* a, * b, * c, * bt;

    a = (T*)aligned_alloc(64, N * N * sizeof(T));
    b = (T*)aligned_alloc(64, N * N * sizeof(T));
    c = (T*)aligned_alloc(64, N * N * sizeof(T));
    bt = (T*)aligned_alloc(64, N * N * sizeof(T));

    //for (int i{ 0 }; i < N; ++i) {
    //    A[i] = a + i * N;
    //    B[i] = b + i * N;
    //    C[i] = c + i * N;
    //    BT[i] = bt + i * N;
    //}
    for (int i = 0; i < N * N; ++i) {
        a[i] = 1;
        b[i] = 1;
        c[i] = 0;
    }

    const auto start{ std::chrono::steady_clock::now() };

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            bt[j * N + i] = b[i * N + j];
        }
    }
        //std::cout << i;

    for (int i{ 0 }; i < num_threads; ++i) {
        th.push_back(std::thread(func, i, a, bt, c, N, num_threads));
    }
    for (auto& t : th) t.join();

    const auto finish{ std::chrono::steady_clock::now() };


    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << "Time: " << time.count() << "ms" << std::endl;

    //delete[] A;
    //delete[] B;
    //delete[] C;
    //delete[] BT;
    free(a);
    free(b);
    free(c);
    free(bt);

    return 0;
}


