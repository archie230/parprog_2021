#include <omp.h>
#include <iostream>
#include <vector>
#include <cassert>

#define ARR_SIZE 100000

int main() {
  // исходный массв a=[1..ARR_SIZE]
  std::vector<double> a(ARR_SIZE);
  for (int i=1; i<=ARR_SIZE; ++i)
    a[i-1] = i;
  // новый массив
  std::vector<double> b(ARR_SIZE);
  // массив для теста, что все отработало ок
  std::vector<double> b_ref(ARR_SIZE);
  
  for (int i=1; i<ARR_SIZE-1; ++i)
    b_ref[i] = (a[i-1]+a[i]+a[i+1])/3.0;
  b_ref[0] = (a[1]+a[2])/2.0;
  b_ref[ARR_SIZE-1] = (a[ARR_SIZE - 1] + a[ARR_SIZE - 2]) / 2.0;
    
  std::cout << "array_size: " << ARR_SIZE <<  std::endl;

  #pragma omp parallel for shared(b) 
  for (int i=1; i<ARR_SIZE-1; ++i)
    b[i] = (a[i-1]+a[i]+a[i+1])/3.0;
  b[0] = (a[1]+a[2])/2.0;
  b[ARR_SIZE-1] = (a[ARR_SIZE - 1] + a[ARR_SIZE - 2]) / 2.0;

  assert(b == b_ref);
    
  std::cout << "output array and reference array are equal!" << std::endl;
    
  return 0;
}
