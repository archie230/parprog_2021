#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <omp.h>
#include <vector>
#include <algorithm>

bool is_simple(uint64_t num) {
    if (num <= 2)
        return true;

    for (size_t i = 2; i * i <= num; ++i)
        if (num % i == 0)
            return false;
    return true;
}

std::vector<uint64_t> simple_search(size_t n) {
    std::vector<uint64_t> res;
    
    #pragma omp parallel for
    for (size_t i = 1; i <= n; ++i)
        if (is_simple(i))
        #pragma omp critical
            res.push_back(i);
    return res;
}

int main(int argc, char **argv) {
    double x = 1;
    size_t limit = std::atoi(argv[1]);

    auto res = simple_search(limit);
    std::sort(res.begin(), res.end());
    std::cout << "simple numbers until " << limit << std::endl;
    for (auto i : res) 
        std::cout << i << " ";
    std::cout << std::endl;

    return 0;
}