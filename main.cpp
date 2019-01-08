#include <iostream>
#include "BloomFilter.h"

const char *generate_string(int n);

int main() {
    BloomFilter bloom_filter;
    std::ios_base::fmtflags f(cout.flags());
    for (int i=1; i<115204; i++) {
        bloom_filter.insert(generate_string(i));
    }
    int max = 0;
    for (int i=1; i<115204; i++) {
        if (bloom_filter.query(generate_string(i)) == 0) {
            cout.flags(f);
            cout <<  i << endl;
//            if (bloom_filter.get_number_of_elements_in_bucket(generate_string(i)) > max) {
//                max = bloom_filter.get_number_of_elements_in_bucket(generate_string(i));
//            }
        }
    }
    cout << max << endl;
    return 0;
}

const char *generate_string(int n) {
    string res;
    for (int i=0; i<n; i++) {
        res += 'b';
    }
    auto *buf = new char[res.length()+1];
    strcpy(buf, res.c_str());
    return buf;
}