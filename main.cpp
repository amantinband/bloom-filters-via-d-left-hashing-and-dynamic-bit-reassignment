#include <iostream>
#include <cstdlib>
#include <ctime>
#include "BloomFilter.h"

int main() {
    BloomFilter  bloom_filter(5); //bloom filter with 5 sub-tables
    unsigned int random;
    const int    NUMBER_OF_ELEMENTS = 200;
    const int    MAX_RAND_RANGE     = 100000;

    std::ios_base::fmtflags f(cout.flags());
    srand(time(0));

    /* add NUMBER_OF_ELEMENTS to the bloom filter */
    for (int i=0; i<NUMBER_OF_ELEMENTS; i++) {
        random = static_cast<unsigned int>(rand() % MAX_RAND_RANGE);
        bloom_filter.insert(reinterpret_cast<const char *>(&random));
    }

    /* check number of false positives */
    int n=0;
    for (int i=1; i<NUMBER_OF_ELEMENTS; i++) {
        random = static_cast<unsigned int>(rand() % MAX_RAND_RANGE) + 2*MAX_RAND_RANGE;
        if (bloom_filter.query(reinterpret_cast<const char *>(&random)) == 1) {
            n++;
        }
    }

    cout.flags(f);
    cout << bloom_filter << endl;
    cout << "False positive percentage:" << endl;
    cout << static_cast<double>(n)/NUMBER_OF_ELEMENTS << endl;
    return 0;
}
