//
// Created by amichai on 06/01/19.
//

#include <functional>
#include <iostream>
#include "BloomFilter.h"

BloomFilter::BloomFilter(unsigned int number_of_tables, unsigned int buckets_per_table, unsigned int bits_per_bucket, unsigned int log_max_elements_in_bucket)
: number_of_tables(number_of_tables), buckets_per_table(buckets_per_table), bits_per_bucket(bits_per_bucket), log_max_elements_in_bucket(log_max_elements_in_bucket) {
    if ((bits_per_bucket+log_max_elements_in_bucket)%BITS_IN_BYTE) {
        throw InvalidAlignmentException();
    }
    if (log_max_elements_in_bucket <=0 || log_max_elements_in_bucket >= BITS_IN_BYTE) {
        throw InvalidLogMaxElementsInBucketException();
    }
    if (bits_per_bucket > sizeof(size_t)*BITS_IN_BYTE) {
        throw InvalidNumberOfBitsPerBucketException();
    }
    table = new unsigned char[number_of_tables*buckets_per_table*(bits_per_bucket+log_max_elements_in_bucket)/BITS_IN_BYTE]();
}

void BloomFilter::insert(const char *element) {
    if (element_is_in_bloom_filter(element)) {
        return;
    }
    unsigned int destinationTable = get_table_index_with_emptiest_bucket(element);
    size_t bucket_index = (hash<string>{}(element))%buckets_per_table;
    increment_bucket_element_counter(destinationTable, bucket_index);
    cout << get_number_of_elements_in_bucket(destinationTable, bucket_index) << endl;
    cout << element << endl;
}

unsigned int BloomFilter::get_table_index_with_emptiest_bucket(const string &element) {
    size_t bucket_index = (hash<string>{}(element))%buckets_per_table;
    unsigned int min_bucket_element = INT32_MAX;
    unsigned int table_index_with_min_bucket;

    for (unsigned int table_index=0; table_index<number_of_tables; table_index++) {
        unsigned int elements_in_bucket = get_number_of_elements_in_bucket(table_index, bucket_index);
        if (elements_in_bucket < min_bucket_element) {
            min_bucket_element = elements_in_bucket;
            table_index_with_min_bucket = table_index;
        }
    }
    return table_index_with_min_bucket;
}

unsigned int BloomFilter::get_number_of_elements_in_bucket(unsigned int table_index, size_t bucket_index) {
    return (unsigned int)(table[table_index*get_table_size_in_bytes()+bucket_index]>>(BITS_IN_BYTE-log_max_elements_in_bucket));
}

unsigned int BloomFilter::get_table_size_in_bytes() {
    return buckets_per_table*(bits_per_bucket+log_max_elements_in_bucket)/BITS_IN_BYTE;
}

bool BloomFilter::element_is_in_bloom_filter(const char *element) {
    size_t bucket_index = hash<string>{}(element)%buckets_per_table;

    for (unsigned int table_index=0; table_index<number_of_tables; table_index++) {
        if (bucket_contains_element(table_index, bucket_index, element)) {
            return true;
        }
    }

    return false;
}

bool BloomFilter::bucket_contains_element(unsigned int table_index, size_t bucket_index, const char *element) {
    unsigned int number_of_elements_in_bucket = get_number_of_elements_in_bucket(table_index, bucket_index);

    if (number_of_elements_in_bucket == 0) {
        return false;
    }

    size_t element_fingerprint = get_fingerprint(element, bits_per_bucket/number_of_elements_in_bucket);

    for (unsigned int i=0; i<number_of_elements_in_bucket; i++) {
        if (get_element_at_index(table_index, bucket_index, i) == element_fingerprint) {
            return true;
        }
    }
    return false;
}

size_t BloomFilter::get_fingerprint(const char *element, unsigned int length) {
    return hash<string>{}(element)%length;
}

size_t BloomFilter::get_element_at_index(unsigned int table_index, size_t bucket_index, unsigned int element_index) {
    size_t fingerprint = 0;

    /* extract entire bucket from table */
    for (unsigned int i=0; i<sizeof(size_t); i++) {
        fingerprint <<= BITS_IN_BYTE;
        fingerprint |= table[table_index*get_table_size_in_bytes() + bucket_index + i];
    }
    unsigned int number_of_elements_in_bucket = get_number_of_elements_in_bucket(table_index, bucket_index);
    unsigned int element_size = bits_per_bucket/number_of_elements_in_bucket;

    /* extract fingerprint from bucket */
    fingerprint <<= log_max_elements_in_bucket;
    fingerprint <<= ((number_of_elements_in_bucket-element_index)*element_size);
    return fingerprint>>(log_max_elements_in_bucket + ((number_of_elements_in_bucket-1)*element_size));
}

void BloomFilter::increment_bucket_element_counter(unsigned int table_index, size_t bucket_index) {
    auto bucket_offset_in_table = static_cast<unsigned int>(table_index * get_table_size_in_bytes() + bucket_index);
    unsigned char first_byte_in_bucket = table[bucket_offset_in_table]>>(BITS_IN_BYTE-log_max_elements_in_bucket);
    unsigned int current_number_of_elements = get_number_of_elements_in_bucket(table_index, bucket_index);
    table[bucket_offset_in_table] = static_cast<unsigned char>(
            ((first_byte_in_bucket << (BITS_IN_BYTE - log_max_elements_in_bucket)) >> (BITS_IN_BYTE - log_max_elements_in_bucket)) | (++current_number_of_elements)<<(BITS_IN_BYTE-log_max_elements_in_bucket));

}
