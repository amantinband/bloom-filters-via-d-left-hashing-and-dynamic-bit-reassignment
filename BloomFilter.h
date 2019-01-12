//
// Created by amichai on 06/01/19.
//

#ifndef DLEFT_HASHING_AND_DYNAMIC_BIT_REASSIGNMENT_COUNTING_BLOOM_FILTER_BLOOMFILTER_H
#define DLEFT_HASHING_AND_DYNAMIC_BIT_REASSIGNMENT_COUNTING_BLOOM_FILTER_BLOOMFILTER_H

using namespace std;
#include <cstring>
#include <exception>

class BloomFilter {
public:
    explicit BloomFilter(unsigned int number_of_tables=3, unsigned int buckets_per_table=16, unsigned int bits_per_bucket=60, unsigned int log_max_elements_in_bucket=4);
    void insert(const char *element);
    bool query(const char *element);

    friend ostream& operator<<(ostream& ostream, BloomFilter bloom_filter);
    struct InvalidAlignmentException : public exception {
        const char * what() const noexcept {
            return "bits per bucket + log max elements in bucket must be byte aligned";
        }
    };
    struct InvalidLogMaxElementsInBucketException : public exception {
        const char * what() const noexcept {
            return "log max elements in bucket must be between 0 and 8";
        }
    };
    struct InvalidNumberOfBitsPerBucketException : public exception {
        const char * what() const noexcept {
            return "number of bits per bucket cannot exceed sizeof(size_t)";
        }
    };
    struct OverflowException : public exception {
        const char * what() const noexcept {
            return "overflow detected!";
        }
    };

private:
    static const unsigned int BITS_IN_BYTE = 8;
    unsigned int number_of_tables;
    unsigned int buckets_per_table;
    unsigned int bits_per_bucket;
    unsigned int log_max_elements_in_bucket;
    unsigned char *table;

    unsigned int get_table_index_with_emptiest_bucket(const char *element);
    unsigned int get_number_of_elements_in_bucket(unsigned int table_index, size_t bucket_index);
    unsigned int get_table_size_in_bytes();
    bool element_is_in_bloom_filter(const char *element);
    bool bucket_contains_element(unsigned int table_index, size_t bucket_index, const char *element);
    size_t get_fingerprint(const char *element, unsigned int length);
    size_t get_bitmask(unsigned int size);
    size_t get_bucket(unsigned int table_index, size_t bucket_index);
    unsigned int get_bucket_size_in_bytes();
    size_t get_bucket_index(const char *element);
    size_t number_of_lost_bits(unsigned int fingerprint_length, unsigned int number_of_elements);
};


#endif //DLEFT_HASHING_AND_DYNAMIC_BIT_REASSIGNMENT_COUNTING_BLOOM_FILTER_BLOOMFILTER_H
