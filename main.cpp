#include <iostream>
#include "BloomFilter.h"
#include <cstdlib>
#include <ctime>

int main() {
    srand(time(0));
    BloomFilter bloom_filter(1000);
    std::ios_base::fmtflags f(cout.flags());
    int number_of_elements = 1000;

    unsigned int random;
    for (int i=1; i<number_of_elements; i++) {
        random = static_cast<unsigned int>(rand() % 1000000);
        bloom_filter.insert(reinterpret_cast<const char *>(&random));
    }
    int n=0;
    for (int i=500000; i<500000+number_of_elements; i++){
        random = static_cast<unsigned int>(rand() % 1000000) + 1000000;
        if (bloom_filter.query(reinterpret_cast<const char *>(&random)) == 1) {
            n++;
        }
    }
    cout.flags(f);
    cout << n << endl;
    cout << (1.0*n)/number_of_elements << endl;
    return 0;
}

