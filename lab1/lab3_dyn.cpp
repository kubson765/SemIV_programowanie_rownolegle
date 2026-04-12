#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <string>

using T = int;

T ** A, ** B, ** C, ** BT;
T *a, *b, *c, *bt;

void func(const int tid, T** A, T** BT, T** C, int N, int num_threads) {
    int lb = tid * (N / num_threads);	// lower bound
    int ub = lb + (N / num_threads) - 1;	// upper bound

    for (int i{ lb }; i <= ub; ++i) {
        for (int j{ 0 }; j < N; ++j) {
            for (int k{ 0 }; k < N; ++k) {
                C[i][j] += A[i][k] * BT[j][k];
            }
        }
    }
}


int main(int argc, char** argv) {

	const int num_threads = std::__cxx11::stoi(argv[2]);
	const int N = std::__cxx11::stoi(argv[1]);

    std::vector<std::thread> th;

	A = new T * [N]();
	B = new T * [N]();
	C = new T * [N]();
    BT = new T * [N]();

	a = (T*)aligned_alloc(64, N * N * sizeof(T));
	b = (T*)aligned_alloc(64, N * N * sizeof(T));
	c = (T*)aligned_alloc(64, N * N * sizeof(T));
    bt = (T*)aligned_alloc(64, N * N * sizeof(T));

    for (int i{ 0 }; i < N; ++i) {
        A[i] = a + i * N;
        B[i] = b + i * N;
        C[i] = c + i * N;
        BT[i] = bt + i * N;
    }

    const auto start{ std::chrono::steady_clock::now() };

    for (int i{ 0 }; i < N; ++i) {
        for (int j{ 0 }; j < N; ++j) {
            BT[j][i] = B[i][j];
        }   
        std::cout << i;
    }

    for (int i{ 0 }; i < num_threads; ++i) {
        th.push_back(std::thread(func, i, A, BT, C, N, num_threads));
    }
    for (auto& t : th) t.join();

    const auto finish{ std::chrono::steady_clock::now() };


    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << "Time: " << time.count() << "ms" << std::endl;

    delete[] A;
    delete[] B;
    delete[] C;
    delete[] BT;
    free(a);
    free(b);
    free(c);
    free(bt);

    return 0;
}


 

//template <typename T>
//void run_test(string type_name) {
//    // Alokacja wskaŸników
//    T** A = new T*[N]();
//    T** B = new T*[N]();
//    T** C = new T*[N]();
// 
//    // Alokacja ci¹g³ych bloków pamiêci (dla wydajnoœci)
//    T* dataA = new T[N * N]();
//    T* dataB = new T[N * N]();
//    T* dataC = new T[N * N](); // Inicjalizacja zerami
// 
//    for (int i = 0; i < N; i++) {
//        A[i] = dataA + i * N;
//        B[i] = dataB + i * N;
//        C[i] = dataC + i * N;
//    }
// 
//    // Inicjalizacja danymi
//    for (int i = 0; i < N * N; i++) {
//        dataA[i] = static_cast<T>(1.5);
//        dataB[i] = static_cast<T>(2.0);
//    }
// 
//    vector<thread> th;
//    cout << "Test dla typu: " << type_name << " (" << N << "x" << N << ")..." << endl;
//   
//    auto start = high_resolution_clock::now();
// 
//    for (int i = 0; i < nr_threads; ++i) {
//        th.push_back(thread(multiply_segment<T>, i, A, B, C, N, nr_threads));
//    }
// 
//    
// 
//    auto finish = high_resolution_clock::now();
//    duration<double> elapsed = finish - start;
// 
//    cout << "Czas: " << elapsed.count() << " s" << endl;
//    cout << "Wynik C[0][0]: " << C[0][0] << endl << endl;
// 
//    // Sprz¹tanie
//    delete[] A; delete[] B; delete[] C;
//    delete[] dataA; delete[] dataB; delete[] dataC;
//}
// 
//int main() {
//    // Mo¿esz teraz ³atwo przetestowaæ ró¿ne typy danych
//    run_test<int>("int");
//    run_test<double>("double");
// 
//    return 0;
//}