#include <iostream>
#include <omp.h>
#include <vector>
#include <string>

const char hello_world[] = "Hello, world!";

int main(int argc, char** argv) {

    int nworkers = sizeof(hello_world);
    unsigned next_id = 0;

    std::cout << "threads num: " << nworkers << " equals to len of: " << hello_world << std::endl; 

    #pragma omp parallel shared(hello_world, next_id) num_threads(nworkers)
    {
        int me = omp_get_thread_num();
        while (next_id != me);
        std::cout << hello_world[me];
        ++next_id;
        #pragma omp barrier
        while (next_id != me + 1);
        std::cout << std::endl << me;
        --next_id;
    }
    std::cout << std::endl;
    
    return 0;
}
