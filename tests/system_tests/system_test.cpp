#include <iostream>
#include <vector>
#include <omp.h>

#include "profiler.h"


// System tests that are ran using installed libraries 
// rather than linked directly to main CMake project
// like the unit tests

int main() {

    auto prof_main = prof.start("main");

    const int N = 10000000;
    std::vector<int> my_vector;
    int sum = 0;

    #pragma omp parallel default(none) shared(prof,N,my_vector,sum)
    {
        //
        // 1. Fill vector with values
        //
        auto prof_fill = prof.start("fill vector");
        #pragma omp for schedule(static)
        for (int i = 0; i < N; i++) {
            #pragma omp critical
            my_vector.push_back(i*i);
        }
        prof.stop(prof_fill);

        //
        // 2.Find sum and average
        //
        auto prof_sum = prof.start("sum entries");
        #pragma omp for schedule(static)
        for (int i = 0; i < N; i++) {
            #pragma omp atomic
            sum += my_vector[i];
        }
        prof.stop(prof_sum);

    }

    auto prof_avg = prof.start("calc avg");
    double avg = static_cast<double>(sum) / 
                 static_cast<double>(N);
    prof.stop(prof_avg);
            
    
    // 
    // 3. Print
    //
    std::cout << avg << std::endl;


    prof.stop(prof_main);
    prof.write();
   
    return 0;
}