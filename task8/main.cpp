#include "CLI11.hpp"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <omp.h>
#include <vector>
#include <algorithm>
#include <string>
#include <cassert>
#include <utility>

enum EasySortedReturn {
    FWD = 0,
    BCKWD = 1,
    NONSORTED = 2
};

bool check_sorted(const std::vector<int> &a) {
    for(size_t i = 1; i < a.size(); ++i)
        if (a[i-1] > a[i])
            return false;
    return true;
}

bool check_reverse_sorted(const std::vector<int> &a) {
    for(size_t i = 1; i < a.size(); ++i)
        if (a[i-1] < a[i])
            return false;
    return true;
}

EasySortedReturn easy_sorted_check(const std::vector<int>& v) {
    if (check_sorted(v))
        return EasySortedReturn::FWD;
    
    if (check_reverse_sorted(v))
        return EasySortedReturn::BCKWD;

    return EasySortedReturn::NONSORTED;
}

void qsort(std::vector<int> &a, size_t begin, size_t end, uint16_t current_depth, uint16_t max_depth) {
    if (begin >= end) return;
    size_t len = end + 1 - begin;
    if (len == 2) {
        if (a[begin] > a[end])
            std::swap(a[begin], a[end]);
        return;
    }
    int pivot = a[std::rand() % len + begin];
    int pivot_index = begin;
    for (size_t i = begin; i <= end; ++i)
        if (a[i] < pivot) {
            std::swap(a[pivot_index], a[i]);
            ++pivot_index;
        }
    if (current_depth == max_depth) {
        if (pivot_index > begin)
            qsort(a, begin, pivot_index - 1, current_depth, max_depth);
        if (a[pivot_index] == pivot)
            ++pivot_index;
        qsort(a, pivot_index, end, current_depth, max_depth);
    } else
        #pragma omp parallel sections
        {
            #pragma omp section
            {
                if (pivot_index > begin)
                    qsort(a, begin, pivot_index - 1, current_depth + 1, max_depth);
            }
            #pragma omp section
            {
                if (a[pivot_index] == pivot)
                    ++pivot_index;
                qsort(a, pivot_index, end, current_depth + 1, max_depth);
            }
        }
}

int main(int argc, char **argv) {
    int inp_len = 0;
    uint16_t depth = 8;
    bool verbose = false;
    std::string file_name = "";

    CLI::App app;
    app.add_option("--inp_len", inp_len, "input array len");
    app.add_option("--depth", depth, "max recursion depth");
    app.add_option("--file_name", file_name, "input file name");
    app.add_flag("--verbose", verbose, "Output sorted array");
    CLI11_PARSE(app, argc, argv);

    omp_set_max_active_levels(depth);

    std::vector<int> arr;
    if (file_name.size() > 0) {
        std::ifstream f(file_name);
            while (!f.eof()) {
                int x;
                f >> x;
                arr.push_back(x);
            }
    } 
    else {
        assert(inp_len > 0);
        arr.resize(inp_len);  
        for (int i = 0; i < arr.size(); ++i)
            std::cin >> arr[i];
    }

    EasySortedReturn easy_sorted = easy_sorted_check(arr);

    switch (easy_sorted) {
        case EasySortedReturn::FWD:
            std::cout << "corner case: " << "array already sorted" << std::endl;
            break;

        case EasySortedReturn::BCKWD:
            std::reverse(arr.begin(), arr.end());
            std::cout << "corner case: " << "array already backward sorted" << std::endl;
            break;
        
        default:
            qsort(arr, 0, arr.size() - 1, 0, depth);
    }

    
    std::cout << (check_sorted(arr) ? "array is sorted\n" : "array is not sorted\n");
    if (verbose) {
        for (auto a: arr)
            std::cout << a << ' ';
        std::cout << '\n';
    }
    return 0;
}