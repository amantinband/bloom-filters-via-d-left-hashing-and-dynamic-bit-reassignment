# Bloom Filters via d-Left Hashing and Dynamic Bit Reassignment  

## This code implements the data structure presented in [this article](https://pdfs.semanticscholar.org/c969/d09434e1b3326053e76fc466b62402942d06.pdf)

To use this code first include the file BloomFilter.h  
```cpp
#include "BloomFilter.h"
```

Create an instance of the class  
```cpp
BloomFilter bloom_filter;
```
By default, these are the values used for the bloom filter:
* sub-tables = 3
* buckets in sub-table = 16 
* bits in bucket = 60
* log max elements in bucket = 4  

```cpp

    explicit BloomFilter(unsigned int number_of_tables=3, unsigned int buckets_per_table=16, unsigned int bits_per_bucket=60, unsigned int log_max_elements_in_bucket=4);
```
These values can be modified during initialization of the bloom filter    
```cpp
BloomFilter bloom_filter(1000); // create bloom filter with 1000 sub-tables

BloomFilter bloom_filter(1000, 50); // create bloom filter with 1000 sub-tables with 50 buckets each

BloomFilter bloom_filter(1000, 50, 34); // create bloom filter with 1000 sub-tables with 50 buckets each and 34 bits in each bucket  

BloomFilter bloom_filter(1000, 50, 30, 2); // create bloom filter with 1000 sub-tables with 50 buckets each, 30 bits in each bucket and a maximum of 2^2 elements per bucket
```

Inserting elements to the bloom filter can be done using the `insert` function  
```cpp
bloom_filter.insert("hey there honey");
```

The bloom filter expects to receive char* but other types can be added with a cast  
```cpp
unsigned int random = rand()%MAX;
bloom_filter.insert(reinterpret_cast<const char *>(&random));
```

Queries can be done using the `query` function  
```cpp
bloom_filter.query("hey there honey"); //returns true
```


## Example
```cpp
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "BloomFilter.h"

int main() {
    BloomFilter  bloom_filter(300); //bloom filter with 300 sub-tables
    unsigned int random;
    const int    NUMBER_OF_ELEMENTS = 10000;
    const int    MAX_RAND_RANGE     = 1000000;

    std::ios_base::fmtflags f(cout.flags());
    srand(time(0));

    /* add NUMBER_OF_ELEMENTS to the bloom filter */
    for (int i=1; i<NUMBER_OF_ELEMENTS; i++) {
        random = static_cast<unsigned int>(rand() % MAX_RAND_RANGE);
        bloom_filter.insert(reinterpret_cast<const char *>(&random));
    }

    /* check number of false positives */
    int n=0;
    for (int i=1; i<NUMBER_OF_ELEMENTS; i++) {
        random = static_cast<unsigned int>(rand() % MAX_RAND_RANGE) + MAX_RAND_RANGE;
        if (bloom_filter.query(reinterpret_cast<const char *>(&random)) == 1) {
            n++;
        }
    }

    cout.flags(f);
    cout << "False positive percentage:" << endl;
    cout << static_cast<double>(n)/NUMBER_OF_ELEMENTS << endl;
    return 0;
}
```
