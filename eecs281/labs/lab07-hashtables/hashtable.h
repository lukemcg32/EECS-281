// Project Identifier: 2C4A3C53CD5AD45A7BEA3AE5356A3B1622C9D04B
#ifndef HASHTABLE_H
#define HASHTABLE_H

// INSTRUCTIONS:
// fill out the methods in the class below.

// You may assume that the key and value types can be copied and have default
// constructors.

// You can also assume that the key type has (==) and (!=) methods.

// You may assume that Hasher is a functor type with a
// size_t operator()(const Key&) overload.

// The key/value types aren't guaranteed to support any other operations.

// Do not add, remove, or change any of the class's member variables.
// The num_deleted counter is *optional*. You might find it helpful, but it
// is not required to pass the lab assignment.

// Do not change the Bucket type.

// SUBMISSION INSTRUCTIONS:
// Submit this file, by itself, in a .tar.gz.
// Other files will be ignored.

#include <cstdint>
#include <functional> // where std::hash lives
#include <vector>
#include <cassert> // useful for debugging!
#include <stdexcept>

// A bucket's status tells you whether it's empty, occupied, 
// or contains a deleted element.
enum class Status : uint8_t {
    Empty,
    Occupied,
    Deleted
};

template<typename K, typename V, typename Hasher = std::hash<K>>
class HashTable {
    // used by the autograder; do not change/remove.
    friend class Verifier;
public:
    // A bucket has a status, a key, and a value.
    struct Bucket {
        // Do not modify Bucket.
        Status status = Status::Empty;
        K key;
        V val;
    };

    HashTable() {
        buckets.resize(20);
        num_elements = 0;
        num_deleted = 0; 
        // You can use the following to avoid implementing rehash_and_grow().
        // However, you will only pass the AG test cases ending in _C.
        // To pass the rest, start with at most 20 buckets and implement rehash_and_grow().
        //    buckets.resize(10000);
    }

    size_t size() const {
        return num_elements;
    }

    // returns a reference to the value in the bucket with the key, if it
    // already exists. Otherwise, insert it with a default value, and return
    // a reference to the resulting bucket.
V& operator[](const K& key) {
    if (num_elements * 2 > buckets.size()) rehash_and_grow();

    Hasher computeHash;
    size_t bucketIndex = computeHash(key) % buckets.size(); // Use .size() for modulo operation
    size_t initialIndex = bucketIndex;

    while (true) {
        if (buckets[bucketIndex].status == Status::Occupied && buckets[bucketIndex].key == key) {
            // Key found, return its value
            return buckets[bucketIndex].val;
        } else if (buckets[bucketIndex].status == Status::Empty) {
            // Empty spot found, insert new key here
            buckets[bucketIndex].key = key;
            buckets[bucketIndex].val = V{}; // Default construct value
            buckets[bucketIndex].status = Status::Occupied;
            ++num_elements;
            return buckets[bucketIndex].val;
        } else if (buckets[bucketIndex].status == Status::Deleted) {
            size_t nextIndex = (bucketIndex + 1) % buckets.size(); // Use .size() for modulo operation
            while (buckets[nextIndex].status != Status::Empty && nextIndex != initialIndex) {
                if (buckets[nextIndex].status == Status::Occupied && buckets[nextIndex].key == key) return buckets[nextIndex].val;
                nextIndex = (nextIndex + 1) % buckets.size(); // Use .size() for modulo operation
            }

            buckets[bucketIndex].key = key;
            buckets[bucketIndex].val = V{}; // Default construct value
            buckets[bucketIndex].status = Status::Occupied;
            ++num_elements;
            return buckets[bucketIndex].val;
        }

        bucketIndex = (bucketIndex + 1) % buckets.size();
        if (bucketIndex == initialIndex) break;
    }

    // Ideally, execution should never reach here with correct rehashing logic
    throw std::runtime_error("HashTable operator[] failed to insert or find the key");
    return buckets[0].val;  // Dummy return to satisfy the compiler
} // []

    // insert returns whether inserted successfully
    // (if the key already exists in the table, do nothing and return false).
    bool insert(const K& key, const V& val) {
        if (num_elements * 2 > buckets.size()) rehash_and_grow();

        Hasher hashFunction;
        size_t currentBucket = hashFunction(key) % buckets.size();
        size_t firstDeletedBucket = size_t(-1); // Start with an invalid index
        bool found = false;

        for (size_t i = 0; i < buckets.size(); ++i) {
            size_t checkingBucket = (currentBucket + i) % buckets.size();

            if (buckets[checkingBucket].status == Status::Occupied && buckets[checkingBucket].key == key) return false;
            else if (buckets[checkingBucket].status == Status::Deleted && firstDeletedBucket == size_t(-1)) firstDeletedBucket = checkingBucket;
            else if (buckets[checkingBucket].status == Status::Empty) {
                found = true;
                currentBucket = (firstDeletedBucket != size_t(-1)) ? firstDeletedBucket : checkingBucket;
                break;
            } // if/else
        } // for

        if (!found && firstDeletedBucket != size_t(-1)) {
            // If no empty slot was found but a deleted slot was, use the deleted slot
            currentBucket = firstDeletedBucket;
            found = true;
        }

        if (found) {
            buckets[currentBucket].key = key;
            buckets[currentBucket].val = val;
            buckets[currentBucket].status = Status::Occupied;
            ++num_elements;
            return true;
        }

        // Should not reach here with proper rehashing logic in place
        return false; // Failsafe return
    } // insert()


    // erase returns the number of items remove (0 or 1)
    size_t erase(const K& key) {
        Hasher computeHash;
        size_t bucketIndex = computeHash(key) % buckets.size();
        size_t initialIndex = bucketIndex;

        while (true) {
            if (buckets[bucketIndex].status == Status::Empty) return 0;
            if (buckets[bucketIndex].status == Status::Occupied && buckets[bucketIndex].key == key) {
                buckets[bucketIndex].status = Status::Deleted;
                --num_elements;
                return 1; // Indicate success in removing the key
            }
            bucketIndex = (bucketIndex + 1) % buckets.size(); // Proceed to the next bucket

            // If we've looped all the way back to the start, the key does not exist
            if (bucketIndex == initialIndex) return 0;
        } // while
    } // erase()


private:
    size_t num_elements = 0;
    size_t num_deleted = 0; // OPTIONAL: you don't need to use num_deleted to pass
    std::vector<Bucket> buckets;

    void rehash_and_grow() {
        // Double the bucket size for the rehash
        std::vector<Bucket> new_table(buckets.size() * 2);
        Hasher computeHash;

        // Rehashing all occupied buckets into the new bucket array
        for (const auto& bucket : buckets) {
            if (bucket.status == Status::Occupied) {
                size_t newIndex = computeHash(bucket.key) % new_table.size();
                // linear probing
                while (new_table[newIndex].status == Status::Occupied) {
                    newIndex = (newIndex + 1) % new_table.size();
                }
                new_table[newIndex] = bucket;
            }
        } // for
        buckets.swap(new_table);
    } // rehash()

    // You can add methods here if you like.
    // TODO
};

#endif // HASHTABLE_H