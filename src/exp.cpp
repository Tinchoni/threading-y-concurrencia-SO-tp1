
#include <iostream>
#include <time.h>
#include "./HashMapConcurrente.hpp"


int main(int argc, char **argv) {
    struct timespec start, end;

    // inicia reloj
    clock_gettime(CLOCK_MONOTONIC, &start);
    std::ios_base::sync_with_stdio(false);

    // corre funcion

    // detiene reloj
    clock_gettime(CLOCK_MONOTONIC, &end); 

    // calcula tiempo transcurrido
    double time_taken; 
    time_taken = (end.tv_sec - start.tv_sec) * 1e9; 
    time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9; 

    
    std::cout << "Time taken by program is : " << std::fixed 
         << time_taken; 
    std::cout << " sec" << std::endl;

	return 0;
}