#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <string>

using T = int;

constexpr int N = 1024; // statyczny rozmiar

T A[N][N];
T B[N][N];
T C[N][N];

void func(int tid, int num_threads) {
    int chunk = N / num_threads;
    int lb = tid * chunk;
    int ub = (tid == num_threads - 1) ? N : lb + chunk;

    for (int i = lb; i < ub; ++i) {
        for (int j = 0; j < N; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < N; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main(int argc, char** argv) {
    using namespace std::chrono;

    if (argc < 2) {
        std::cout << "Usage: program num_threads\n";
        return 1;
    }

    int num_threads = std::stoi(argv[1]);

    // inicjalizacja
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) {
            A[i][j] = 1;
            B[i][j] = 1;
        }

    std::vector<std::thread> th;

    const auto start = high_resolution_clock::now();

    for (int i = 0; i < num_threads; ++i) {
        th.emplace_back(func, i, num_threads);
    }

    for (auto& t : th) t.join();

    const auto finish = high_resolution_clock::now();

    auto time = duration_cast<milliseconds>(finish - start);
    std::cout << "Time: " << time.count() << " ms\n";

    return 0;
}