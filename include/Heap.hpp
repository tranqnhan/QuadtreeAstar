#pragma once

#include "ankerl/unordered_dense.h"
#include <functional>
#include <vector>

template<typename T>
class Heap {
public:
    Heap(std::function<bool(const T&, const T&)> compare);

    bool Push(T item, int unique_id);

    // get top item of heap
    const T& TopItem() const ;

    // get top unique item id of heap
    int TopItemID() const ;
    
    // removes top of heap
    void Pop();

    unsigned int GetSize() const {
        return size;
    }

    ~Heap();

private:
    std::vector<T> heap;
    
    std::vector<int> hid2id;
    ankerl::unordered_dense::map<int, unsigned int> id2hid; 

    unsigned int size;

    // compare: a is before b in the heap
    // return true if swap a and b, return false if no swap
    std::function<bool(const T& a, const T& b)> compare;

    void Swap(unsigned int hida, unsigned int hidb);

    // Move item up (because its priority increases)
    void Up(int uniqueId);
};


template<typename T>
Heap<T>::Heap(std::function<bool(const T&, const T&)> compare) {
    this->size = 0;
    this->compare = compare;
}


template<typename T>
bool Heap<T>::Push(T item, int uniqueId) {
    auto id2hidIterator = this->id2hid.find(uniqueId);
    if (id2hidIterator != this->id2hid.end()) {
        if (this->compare(this->heap[id2hidIterator->second], item)) {
            this->heap[id2hidIterator->second] = std::move(item);
            Up(uniqueId);
            return true;
        }
        return false;
    }

    if (size < this->heap.size()) {
        this->heap[size] = std::move(item);
        this->hid2id[size] = uniqueId;
    } else {
        this->heap.emplace_back(std::move(item));
        this->hid2id.emplace_back(uniqueId);
    }

    this->id2hid.emplace(uniqueId, size);

    unsigned int cindex = this->size;
    unsigned int pindex = (cindex - 1) / 2;
    
    while (cindex != 0 && this->compare(this->heap[pindex], this->heap[cindex])) {
        this->Swap(cindex, pindex);
        cindex = pindex;
        pindex = (cindex - 1) / 2;
    }

    this->size++;
    return true;
}

template<typename T>
void Heap<T>::Pop() {
    if (this->size == 0) return;

    this->id2hid.erase(this->hid2id[0]);
    this->heap[0] = std::move(this->heap[this->size - 1]);
    this->hid2id[0] = this->hid2id[this->size - 1];
    this->id2hid.insert_or_assign(this->hid2id[this->size - 1], 0);

    this->size--;

    unsigned int pindex = 0;
    unsigned int lindex = 0;
    unsigned int rindex = 0;
    unsigned int cindex = 0;

    while (2 * pindex + 1 < this->size) {
        lindex = 2 * pindex + 1;
        rindex = 2 * pindex + 2;

        cindex = lindex;
        if (rindex < this->size && this->compare(this->heap[lindex], this->heap[rindex])) {
            cindex = rindex;
        }
        if (this->compare(this->heap[pindex], this->heap[cindex])) {
            this->Swap(pindex, cindex);
            pindex = cindex;
        } else {
            break;
        }
    }
}

template<typename T>
const T& Heap<T>::TopItem() const {
    return std::move(this->heap[0]);
}

template<typename T>
int Heap<T>::TopItemID() const {
    return this->hid2id[0];
}

template<typename T>
void Heap<T>::Up(int unique_id) {
    unsigned int cindex = id2hid.find(unique_id)->second;
    unsigned int pindex = (cindex - 1) / 2;
    while (cindex != 0 && this->compare(this->heap[pindex], this->heap[cindex])) {
        this->Swap(pindex, cindex);
        cindex = pindex;
        pindex = (cindex - 1) / 2;
    }
}

template<typename T>
void Heap<T>::Swap(unsigned int hida, unsigned int hidb) {
    T a = std::move(this->heap[hida]);
    this->heap[hida] = std::move(this->heap[hidb]);
    this->heap[hidb] = std::move(a);

    int uqida = this->hid2id[hida];
    this->hid2id[hida] = this->hid2id[hidb];
    this->hid2id[hidb] = uqida;

    this->id2hid.insert_or_assign(this->hid2id[hidb], hidb);
    this->id2hid.insert_or_assign(this->hid2id[hida], hida);
}

template<typename T>
Heap<T>::~Heap() {
    this->heap.clear();
}