#include <omp.h>
#include <iostream>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <algorithm>

void dump_matrix(std::vector<std::vector<double>> &m) {
    // python style dump
    for (int i = 0; i < m.size(); ++i) {
        if (i==0)
            std::cout << "[[";
        else
            std::cout << "[";
        for (int j = 0; j < m[0].size(); ++j)
            if (j < m[0].size()-1)
                std::cout << m[i][j] << ", ";
            else
                std::cout << m[i][j];
        if (i==m.size()-1)
            std::cout << "]]" << std::endl;
        else
            std::cout << "]," << std::endl;
    }
}

void random_init_matrix(std::vector<std::vector<double>> &m) {
    // randomly initialize matrix values
    for (auto &row : m) {
        std::transform(row.begin(), row.end(), row.begin(), [](double x) {
            return static_cast<double>(std::rand() % 1000) / 1000.0;
        });
    }
}

std::vector<std::vector<double>> create_random_matrix(int shape_0, int shape_1) {
    // create randomly initialized matrix [shape_0, shape_1]
    std::vector<std::vector<double>> M(shape_0);
    for (auto& row : M)
        row.resize(shape_1);
    random_init_matrix(M);
    return M;
}

double dot(const std::vector<double>& a, const std::vector<double>& b) {
    double res = 0.0;
    for (size_t i = 0; i < a.size(); ++i)
        res += a[i] * b[i];
    return res;
}

std::vector<std::vector<double>> transpose(const std::vector<std::vector<double>>& m) {
    int m_shape_0 = m.size();
    int m_shape_1 = m[0].size();
    auto m_T = create_random_matrix(m_shape_1, m_shape_0);
    for (int i=0; i<m_shape_1; ++i) {
        for (int j=0; j<m_shape_0; j++)
            m_T[i][j] = m[j][i];
    }
    return m_T;
}

std::vector<std::vector<double>> matmul(std::vector<std::vector<double>> &A, std::vector<std::vector<double>> &B) {
    // matrix A -- vector of vectors of shape [n, m]
    // matrix B -- vector of vectors of shape [m, k]
    auto B_T = transpose(B);
    size_t n = A.size();
    size_t m = A[0].size();
    size_t k = B_T.size();
    size_t len = n*k;
    size_t omp_num_threads = std::atoi(std::getenv("OMP_NUM_THREADS"));
    // разбиваем res матрицу на chunk_size, каждый поток будет делать примерно chunk_size скалярных произведений
    size_t chunk_size = len / omp_num_threads + (len % omp_num_threads != 0);
    auto res = create_random_matrix(n, k);
        
    #pragma omp parallel
    {
        int me = omp_get_thread_num();
        size_t begin = me * chunk_size;
        size_t end = std::min(begin + chunk_size, len);

        for (size_t i = begin; i < end; ++i) {
            size_t row = i / k;
            size_t col = i % k;
            res[row][col] = dot(A[row], B_T[col]);
        }
    }
    return res;
}

int main() {
    int N = 3;
    int M = 4;
    int K = 5;
    auto A = create_random_matrix(N, M);
    auto B = create_random_matrix(M, K);
    
    std::cout << "matrix A: " << std::endl;
    dump_matrix(A);
    std::cout << "matrix B: " << std::endl;
    dump_matrix(B);
    
    auto res = matmul(A, B);
    
    std::cout << "A*B: " << std::endl;
    dump_matrix(res);
    
    return 0;
}
