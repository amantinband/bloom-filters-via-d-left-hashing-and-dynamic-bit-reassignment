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
```cpp
    explicit BloomFilter(unsigned int number_of_tables=3, unsigned int buckets_per_table=16, unsigned int bits_per_bucket=60, unsigned int log_max_elements_in_bucket=4);
```
* sub-tables = 3
* buckets in sub-table = 16 
* bits in bucket = 60
* log max elements in bucket = 4  

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

The bloom filter's inner structure can be viewed at any time via printing to an output stream
```cpp
ofstream file("my_output_file.dat");
file << bloom_filter;
```
or
```cpp
cout << bloom_filter << endl;
```
## Example
```cpp
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
    for (int i=1; i<NUMBER_OF_ELEMENTS; i++) {
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
    cout << n << endl;
    cout << static_cast<double>(n)/NUMBER_OF_ELEMENTS << endl;
    return 0;
}
```
## Output
```
     |        table 0         |        table 1         |        table 2         |        table 3         |        table 4         |
-----+------------------------+------------------------+------------------------+------------------------+------------------------+
  0  |   3 3e23501d56015aa0   |   2 27dbc8f829908190   |   2 2c634b181b485670   |   2 2f62188034cca250   |   2 20a1750c1302f4a0   |
  1  |   3 3b6e0118c6109e01   |   3 3973c1f3b517d851   |   3 3061115b461753d1   |   2 26f4989c44b2de71   |   2 24b257804f8d88a1   |
  2  |   2 2788d620bbc3edb2   |   2 28b3ff70a0bc6f92   |   2 2c053f2892058eb2   |   1 13f1268ab5c9f8a2   |   1 111215c740465cc2   |
  3  |   3 31b9532c12394473   |   3 3326b3bc0d362133   |   3 3825639961378333   |   3 326ae3d0a53a60a3   |   3 3436936c003144a3   |
  4  |   2 2015f7153b6977a4   |   1 12c0c83946121244   |   1 10ae95fdec1b63e4   |   1 12e4aecec6ffc604   |   1 15f3a13f51cf1914   |
  5  |   3 375e357b1456bf35   |   3 32f4c56ee2527bc5   |   3 3f4a75fbea5e1095   |   2 219881e5655c6395   |   2 2d9c50b574304ec5   |
  6  |   4 42c0deb99f0b0ce6   |   4 42bacebdba634f86   |   4 4e06cf1d96eb1266   |   4 4876d35db9b317b6   |   3 3e3706da6f68a436   |
  7  |   4 4866fbedc9038a27   |   4 42eae739ef73b9e7   |   4 47bce2d5cb1bc897   |   4 4d8af3e1e453c997   |   3 3311873e2d760137   |
  8  |   3 36c8182bb1831d88   |   2 24bd76ea1fb00b78   |   2 20f9ba5a2bbc8088   |   2 2b79b33a394c4048   |   2 298b74e2294ab578   |
  9  |   3 3252397cd5977e19   |   3 3d5d291d3d9c1659   |   2 24b38ef24b3c3959   |   2 24dcd5ee6eeb0fc9   |   2 2a59f2fe6f27cbe9   |
 10  |   4 4a315326a83d1b1a   |   4 42874d6e9a5d5d7a   |   3 3c481a5934afc12a   |   3 3fd6bafe25ac24ba   |   3 372f8a9a92a9adda   |
 11  |   3 3662bb8109b26a8b   |   3 33783b14e2b7b22b   |   2 2d229596f65218ab   |   2 2a890f62c84d5aab   |   2 24e486e2e19b51db   |
 12  |   2 2da2a5770e5760cc   |   2 2312633709479bec   |   1 159be6e2f6be5fbc   |   1 1017e60b23b015cc   |   1 15ae6ce62389b31c   |
 13  |   3 3b91ad7b27d926fd   |   3 390d7dfb63d25a0d   |   3 30214da660dff6fd   |   2 269e6c935cfce55d   |   2 20e7377b675f92dd   |
 14  |   3 313c4e6548e8ab0e   |   3 3ac52eb202e2ff6e   |   3 3d794ee3caeab69e   |   3 399f3e481de3e2ce   |   3 3129aee40ee9cebe   |
 15  |   2 21014267d878723f   |   2 20dc7d6bfe71814f   |   2 2b8a26cbda68140f   |   1 10b08947c0e8476f   |   1 13edb74d36c458af   |

False positive percentage:
0
0
```
