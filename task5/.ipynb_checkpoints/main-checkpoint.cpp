#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <omp.h>
#include <vector>

double exp(double x0, size_t len) {
    size_t jobs = std::atoi(std::getenv("OMP_NUM_THREADS"));
    size_t chunk = len / jobs + (len % jobs != 0);
    double res = 0.0;
    #pragma omp parallel
    {
        auto me = omp_get_thread_num();
        size_t begin = me * chunk;
        size_t end = std::min(begin + chunk, len);

        double fact = 1;
        double x = 1;
        for (size_t i = 1; i <= begin; ++i) {
            fact /= i;
            x *= x0;
        }

        double tmp = x * fact;
        for (size_t i = begin + 1; i < end; ++i) {
            fact /= i;
            x *= x0;
            tmp += x * fact;
        }

        #pragma omp atomic
        res += tmp;
    }
    return res;
}

int main(int argc, char **argv) {
    double x = 1;
    size_t len = std::atoi(argv[1]);

    double res = exp(x, len);
    std::cout << "tailor series partial sum for " << len << "th element" << std::endl;
    std::cout << "exp(" << x <<  ") ~ " << res << '\n';

    return 0;
}