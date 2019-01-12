//
// Created by amichai on 06/01/19.
//

#include <functional>
#include <iostream>
#include <cmath>
#include <sstream>
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
    size_t       bucket_index                             = get_bucket_index(element);
    unsigned int destination_table_index                  = get_table_index_with_emptiest_bucket(element);
    unsigned int number_of_elements_in_destination_bucket = get_number_of_elements_in_bucket(destination_table_index, bucket_index);
    unsigned int new_fingerprint_length                   = bits_per_bucket/(number_of_elements_in_destination_bucket+1);
    unsigned int old_fingerprint_length                   = number_of_elements_in_destination_bucket == 0 ? 0 : bits_per_bucket/number_of_elements_in_destination_bucket;
    unsigned int table_size_in_bytes                      = get_table_size_in_bytes();
    size_t       old_bucket                               = get_bucket(destination_table_index, bucket_index);
    size_t       element_fingerprint                      = get_fingerprint(element, number_of_elements_in_destination_bucket+1);
    size_t       new_bucket                               = 0;

    if (number_of_elements_in_destination_bucket > (static_cast<size_t>(1)<<log_max_elements_in_bucket)) {
        throw OverflowException();
    }
    /* dynamic bit reassignment */
    new_bucket |= number_of_elements_in_destination_bucket+1;
    new_bucket <<= number_of_lost_bits(new_fingerprint_length, number_of_elements_in_destination_bucket + 1);
    for (unsigned int i=0; i<number_of_elements_in_destination_bucket; i++) {
        new_bucket <<= new_fingerprint_length;
        new_bucket |= (old_bucket&get_bitmask(old_fingerprint_length))%(static_cast<size_t>(1)<<new_fingerprint_length);
        old_bucket >>= old_fingerprint_length;
    }
    new_bucket <<= new_fingerprint_length;
    new_bucket |= element_fingerprint;

    /* replace old bucket */
    for (unsigned int i=0; i<get_bucket_size_in_bytes(); i++) {
        table[destination_table_index*table_size_in_bytes+bucket_index*get_bucket_size_in_bytes()+i] = static_cast<unsigned char>(0xFF&(new_bucket >> ((get_bucket_size_in_bytes()-1-i)*BITS_IN_BYTE)));
    }
//    cout << hex << new_bucket << endl;
}

unsigned int BloomFilter::get_table_index_with_emptiest_bucket(const char *element) {
    size_t bucket_index = (hash<string>{}(element))%buckets_per_table;
    unsigned int min_bucket_element = INT32_MAX;
    unsigned int table_index_with_min_bucket = 0;

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
    return table[table_index*get_table_size_in_bytes()+bucket_index*get_bucket_size_in_bytes()]>>(BITS_IN_BYTE-log_max_elements_in_bucket);
}

unsigned int BloomFilter::get_table_size_in_bytes() {
    return buckets_per_table*(bits_per_bucket+log_max_elements_in_bucket)/BITS_IN_BYTE;
}

bool BloomFilter::element_is_in_bloom_filter(const char *element) {
    size_t bucket_index = get_bucket_index(element);

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

    size_t element_fingerprint = get_fingerprint(element, number_of_elements_in_bucket);

    size_t bucket = get_bucket(table_index, bucket_index);

    /* extract fingerprint from bucket */
    auto element_size = static_cast<unsigned int>(floor(bits_per_bucket / number_of_elements_in_bucket));

    for (unsigned int i=0; i<number_of_elements_in_bucket; i++) {
        if ((bucket&get_bitmask(element_size)) == element_fingerprint) {
            return true;
        }
        bucket >>= element_size;
    }
    return false;
}

size_t BloomFilter::get_fingerprint(const char *element, unsigned int number_of_elements_in_bucket) {
    size_t fingerprint_length = static_cast<size_t>(static_cast<size_t>(1) << (bits_per_bucket-1));
    size_t fingerprint = hash<string>{}(element)%(fingerprint_length);
    for (unsigned int i=2; i<number_of_elements_in_bucket+1; i++) {
        fingerprint_length = static_cast<size_t>(static_cast<size_t>(1) << (bits_per_bucket / i));
        fingerprint = fingerprint%fingerprint_length;
    }
    return fingerprint;
}

size_t BloomFilter::get_bitmask(unsigned int size) {
    return (((~(static_cast<size_t>(0))) << (sizeof(size_t) * BITS_IN_BYTE - size)) >> (sizeof(size_t) * BITS_IN_BYTE - size));
}

size_t BloomFilter::get_bucket(unsigned int table_index, size_t bucket_index) {
    size_t bucket = 0;

    /* extract entire bucket from table */
    for (unsigned int i=0; i<sizeof(size_t); i++) {
        bucket <<= BITS_IN_BYTE;
        bucket |= table[table_index*get_table_size_in_bytes() + bucket_index*get_bucket_size_in_bytes() + i];
    }
    bucket >>= (sizeof(size_t)*BITS_IN_BYTE-log_max_elements_in_bucket-bits_per_bucket);
    return bucket;
}

bool BloomFilter::query(const char *element) {
    return element_is_in_bloom_filter(element);
}

unsigned int BloomFilter::get_bucket_size_in_bytes() {
    return (bits_per_bucket+log_max_elements_in_bucket)/BITS_IN_BYTE;
}

size_t BloomFilter::get_bucket_index(const char *element) {
    return (hash<string>{}(element))%buckets_per_table;
}

size_t BloomFilter::number_of_lost_bits(unsigned int fingerprint_length, unsigned int number_of_elements) {
    return bits_per_bucket-(fingerprint_length*number_of_elements);
}

string center(const string &s, int n) {
    stringstream ss;
    for (unsigned int i=0; i<(n-s.length())/2; i++) {
        ss << " ";
    }
    ss << s;
    for (unsigned int i=0; i<ceil(n-s.length())/2; i++) {
        ss << " ";
    }
    return ss.str();
}
ostream &operator<<(ostream &ostream, BloomFilter bloom_filter) {
    const unsigned int column_width = bloom_filter.bits_per_bucket/4 + 9;


    ostream << "     |";
    for (unsigned int table_index=0; table_index<bloom_filter.number_of_tables; table_index++) {
        ostream << center("table " + to_string(table_index), column_width) <<"|";
    }
    ostream << endl;
    ostream << "-----+";
    for (unsigned int table_index=0; table_index<bloom_filter.number_of_tables; table_index++) {
        for (unsigned int i=0; i<column_width; i++) {
            ostream << "-";
        }
        ostream << "+";
    }
    ostream << endl;
    for (unsigned int bucket_index=0; bucket_index<bloom_filter.buckets_per_table; bucket_index++) {
        cout << center(to_string(bucket_index),5) << "|";
        for (unsigned int table_index=0; table_index<bloom_filter.number_of_tables; table_index++) {
            size_t bucket = bloom_filter.get_bucket(table_index, bucket_index);
            auto number_of_elements_in_bucket = static_cast<unsigned int>(bucket >> (sizeof(size_t)*BloomFilter::BITS_IN_BYTE - bloom_filter.log_max_elements_in_bucket));
            stringstream ss;
            ss << hex << number_of_elements_in_bucket << " " << hex << bucket;
            ostream << center(ss.str(), column_width);
            ostream << "|";
        }
        ostream << endl;
    }
    return ostream;
}
