#include <iostream>
#include "BloomFilter.h"

const char *generate_string(int n);

int main() {
    BloomFilter bloom_filter;
    std::ios_base::fmtflags f(cout.flags());
    int a;
    int number_of_elements = 300;
    for (int i=1; i<number_of_elements; i++) {
        bloom_filter.insert(reinterpret_cast<const char *>(&i));
    }
    int max = 0;
    for (int i=1; i<number_of_elements; i++) {
        if (bloom_filter.query(reinterpret_cast<const char *>(&i)) == 0) {
            cout.flags(f);
            cout <<  i << endl;
        }
    }
    int n=0;
    for (int i=500000; i<500000+number_of_elements; i++){
        if (bloom_filter.query(reinterpret_cast<const char *>(&i)) == 1) {
            n++;
        }

    }
    cout.flags(f);
    cout << n << endl;
    cout << (1.0*n)/number_of_elements << endl;
    return 0;
}

