#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>
#include <vector>

// ------------------------------------------------------------
// HashTable<V>
// A hash table built from scratch (separate chaining via singly
// linked lists that we manage ourselves -- not std::unordered_map).
// Keys are always std::string. Average case O(1) insert/search/remove.
// Automatically resizes (rehashes) once load factor exceeds 0.7.
// ------------------------------------------------------------
template <typename V>
class HashTable {
private:
    struct Node {
        std::string key;
        V value;
        Node* next;
        Node(const std::string& k, const V& v) : key(k), value(v), next(nullptr) {}
    };

    std::vector<Node*> buckets;
    int capacity;
    int count;

    // djb2 string hash
    int hashFunction(const std::string& key) const {
        unsigned long hash = 5381;
        for (unsigned char c : key)
            hash = ((hash << 5) + hash) + c;   // hash * 33 + c
        return (int)(hash % (unsigned long)capacity);
    }

    void rehash() {
        std::vector<Node*> oldBuckets = buckets;
        int oldCapacity = capacity;
        capacity = capacity * 2 + 1;
        buckets.assign(capacity, nullptr);
        count = 0;

        for (int i = 0; i < oldCapacity; ++i) {
            Node* cur = oldBuckets[i];
            while (cur) {
                insert(cur->key, cur->value);
                Node* toDelete = cur;
                cur = cur->next;
                delete toDelete;
            }
        }
    }

public:
    HashTable(int cap = 101) : capacity(cap), count(0) {
        buckets.assign(capacity, nullptr);
    }

    ~HashTable() {
        clear();
    }

    void clear() {
        for (int i = 0; i < capacity; ++i) {
            Node* cur = buckets[i];
            while (cur) {
                Node* toDelete = cur;
                cur = cur->next;
                delete toDelete;
            }
            buckets[i] = nullptr;
        }
        count = 0;
    }

    void insert(const std::string& key, const V& value) {
        if (capacity > 0 && (double)(count + 1) / capacity > 0.7) rehash();

        int idx = hashFunction(key);
        Node* cur = buckets[idx];
        while (cur) {
            if (cur->key == key) { cur->value = value; return; } // update
            cur = cur->next;
        }
        Node* n = new Node(key, value);
        n->next = buckets[idx];
        buckets[idx] = n;
        count++;
    }

    bool search(const std::string& key, V& outValue) const {
        int idx = hashFunction(key);
        Node* cur = buckets[idx];
        while (cur) {
            if (cur->key == key) { outValue = cur->value; return true; }
            cur = cur->next;
        }
        return false;
    }

    bool contains(const std::string& key) const {
        int idx = hashFunction(key);
        Node* cur = buckets[idx];
        while (cur) {
            if (cur->key == key) return true;
            cur = cur->next;
        }
        return false;
    }

    bool remove(const std::string& key) {
        int idx = hashFunction(key);
        Node* cur = buckets[idx];
        Node* prev = nullptr;
        while (cur) {
            if (cur->key == key) {
                if (prev) prev->next = cur->next;
                else buckets[idx] = cur->next;
                delete cur;
                count--;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }

    std::vector<V> getAllValues() const {
        std::vector<V> result;
        result.reserve(count);
        for (int i = 0; i < capacity; ++i) {
            Node* cur = buckets[i];
            while (cur) {
                result.push_back(cur->value);
                cur = cur->next;
            }
        }
        return result;
    }

    int size() const { return count; }
    int bucketCount() const { return capacity; }
};

#endif
