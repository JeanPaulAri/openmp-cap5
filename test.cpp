#include <iostream>
#include <stdio.h>
#include <omp.h>

using namespace std;

void print();
int main(){
    #pragma omp parallel num_threads(6)
    print();

    return 0;
}

void print(){
    int tn;
    tn = omp_get_thread_num();
    printf("Hello World from thread -> %d\n", tn);
    
}