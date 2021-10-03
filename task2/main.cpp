#include <omp.h>
#include <iostream>

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "requiers number as inp arg" << std::endl;
    return -1;
  }

  int n = std::stoi(argv[1]);
  std::cout << "input number : " << n << std::endl;
  int sum = 0;

  #pragma omp parallel for reduction(+:sum) schedule(auto)
    for (unsigned i = 1; i <= n; ++i)
      sum += i;
  std::cout << "result sum : " << sum << std::endl;

  return 0;
}
