#pragma once

#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <algorithm>

template <typename T>
void destroy_all(T* ptr, size_t size) {
    for (size_t i = 0; i < size; i++) {
        ptr[i].~T();
    }
}

template <typename T>
void copy_and_construct(T* dst, T const* src, size_t size) {
    size_t i = 0;

    try {
        for (; i < size; i++) {
            new(dst + i) T(src[i]);
        }
    } catch(...) {
        destroy_all(dst, i);
        throw;
    }
}

template <typename T>
struct vector {
    typedef T* iterator;
    typedef T const* const_iterator;

    vector();                               // O(1) nothrow
    vector(vector const&);                  // O(N) strong
    vector& operator=(vector const& other); // O(N) strong

    ~vector();                              // O(N) nothrow

    T& operator[](size_t i);                // O(1) nothrow
    T const& operator[](size_t i) const;    // O(1) nothrow

    T* data();                              // O(1) nothrow
    T const* data() const;                  // O(1) nothrow
    size_t size() const;                    // O(1) nothrow

    T& front();                             // O(1) nothrow
    T const& front() const;                 // O(1) nothrow

    T& back();                              // O(1) nothrow
    T const& back() const;                  // O(1) nothrow
    void push_back(T const&);               // O(1)* strong
    void pop_back();                        // O(1) nothrow

    bool empty() const;                     // O(1) nothrow

    size_t capacity() const;                // O(1) nothrow
    void reserve(size_t);                   // O(N) strong
    void shrink_to_fit();                   // O(N) strong

    void clear();                           // O(N) nothrow

    void swap(vector&);                     // O(1) nothrow

    iterator begin();                       // O(1) nothrow
    iterator end();                         // O(1) nothrow

    const_iterator begin() const;           // O(1) nothrow
    const_iterator end() const;             // O(1) nothrow

    iterator insert(iterator pos, T const&); // O(N) weak
    iterator insert(const_iterator pos, T const&); // O(N) weak

    iterator erase(iterator pos);           // O(N) weak
    iterator erase(const_iterator pos);     // O(N) weak

    iterator erase(iterator first, iterator last); // O(N) weak
    iterator erase(const_iterator first, const_iterator last); // O(N) weak

private:
    void reserve_if_needed(size_t additional_capacity);
    void resize(size_t);
    void resize_and_insert(size_t size, size_t i, T const&);
    void copy_to_ptr(size_t, T*);
    void move_element(size_t from, size_t to);

    T* data_;
    size_t size_;
    size_t capacity_;
};

template <typename T>
vector<T>::vector()
    : data_(nullptr)
    , size_(0)
    , capacity_(0) {}

template <typename T>
vector<T>::vector(vector<T> const& v)
    : vector() {
    if (v.size() != 0) {
        T* ptr = static_cast<T*>(operator new(v.size() * sizeof(T)));
        copy_and_construct(ptr, v.data(), v.size());

        data_ = ptr;
        capacity_ = v.size();
        size_ = v.size();
    }
}

template <typename T>
vector<T>& vector<T>::operator=(vector<T> const& other) {
    vector<T> copy(other);
    swap(copy);
    return *this;
}

template <typename T>
vector<T>::~vector() {
    destroy_all(data_, size_);
    operator delete(data_);
}

template <typename T>
T& vector<T>::operator[](size_t i) {
    assert(i < size_);
    return data_[i];
}

template <typename T>
T const& vector<T>::operator[](size_t i) const  {
    assert(i < size_);
    return data_[i];
}

template <typename T>
T* vector<T>::data() {
    return data_;
}

template <typename T>
T const* vector<T>::data() const {
    return data_;
}

template <typename T>
size_t vector<T>::size() const  {
    return size_;
}

template <typename T>
T& vector<T>::front() {
    assert(size_ > 0);
    return data_[0];
}

template <typename T>
T const& vector<T>::front() const {
    assert(size_ > 0);
    return data_[0];
}

template <typename T>
T& vector<T>::back() {
    assert(size_ > 0);
    return data_[size_ - 1];
}

template <typename T>
T const& vector<T>::back() const {
    assert(size_ > 0);
    return data_[size_ - 1];
}

template <typename T>
void vector<T>::push_back(T const& e) {
    insert(end(), e);
}

template <typename T>
void vector<T>::pop_back() {
    assert(size_ > 0);
    back().~T();
    size_--;
}

template <typename T>
bool vector<T>::empty() const {
    return size_ == 0;
}

template <typename T>
size_t vector<T>::capacity() const {
    return capacity_;
}

template <typename T>
void vector<T>::reserve(size_t size) {
    if (size > capacity_) {
        resize(size);
    }
}

template <typename T>
void vector<T>::shrink_to_fit() {
    if (capacity_ != size_) {
        resize(size_);
    }
}

template <typename T>
void vector<T>::clear() {
    destroy_all(data_, size_);
    size_ = 0;
}

template <typename T>
void vector<T>::swap(vector<T> &other) {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}

template <typename T>
typename vector<T>::iterator vector<T>::begin() {
    return data_;
}

template <typename T>
typename vector<T>::iterator vector<T>::end() {
    return data_ + size_;
}

template <typename T>
typename vector<T>::const_iterator vector<T>::begin() const {
    return data_;
}

template <typename T>
typename vector<T>::const_iterator vector<T>::end() const {
    return data_ + size_;
}

template <typename T>
typename vector<T>::iterator vector<T>::insert(iterator pos, T const& e) {
    const size_t pos_i = pos - begin();
    if (size_ == capacity_) {
        resize_and_insert(std::max(static_cast<size_t>(1), capacity_ * 2), pos_i, e);
    } else {
        new(data_ + size_) T(e);
        size_++;
        move_element(size_ - 1, pos_i);
    }

    return begin() + pos_i;
}

template <typename T>
typename vector<T>::iterator vector<T>::insert(const_iterator pos, T const& e) {
    return insert(const_cast<iterator>(pos), e);
}

template <typename T>
typename vector<T>::iterator vector<T>::erase(iterator pos) {
    return erase(pos, pos + 1);
}

template <typename T>
typename vector<T>::iterator vector<T>::erase(const_iterator pos) {
    return erase(pos, pos + 1);
}

template <typename T>
typename vector<T>::iterator vector<T>::erase(iterator first, iterator last) {
    return erase(static_cast<const_iterator>(first), static_cast<const_iterator>(last));
}

template <typename T>
typename vector<T>::iterator vector<T>::erase(const_iterator first, const_iterator last) {
    size_t first_after_i = last - begin();
    size_t first_i = first - begin();

    for (size_t i = first_after_i; i < size_; i++) {
        data_[first_i + i - first_after_i] = data_[i];
        data_[i].~T();
    }
    size_ -= last - first;
    return data_+ first_i;
}

template <typename T>
void vector<T>::reserve_if_needed(size_t additional_capacity) {
    size_t new_capacity = std::max(static_cast<size_t>(1), capacity_);
    while (new_capacity < size_ + additional_capacity) {
        new_capacity *= 2;
    }
    reserve(new_capacity);
}

template <typename T>
void vector<T>::resize(size_t size) {
    if (size == 0) {
        operator delete(data_);
        data_ = nullptr;
        capacity_ = 0;
    } else {
        copy_to_ptr(size, static_cast<T*>(operator new(size * sizeof(T))));
    }
}

template <typename T>
void vector<T>::resize_and_insert(size_t size, size_t i, T const& e) {
    T* ptr = static_cast<T*>(operator new(size * sizeof(T)));
    new(ptr + size_) T(e);
    auto placed_element = ptr + size_;
    try {
        copy_to_ptr(size, ptr);
    } catch (...) {
        placed_element->~T();
        throw;
    }
    size_++;
    move_element(size_ - 1, i);
}

template <typename T>
void vector<T>::copy_to_ptr(size_t capacity, T *ptr) {
    try {
        copy_and_construct(ptr, data_, size_);
    } catch(...) {
        std::swap(ptr, data_);
        capacity_ = capacity;
        destroy_all(ptr, size_);
        operator delete(ptr);

        size_ = 0;
        throw;
    }
    std::swap(ptr, data_);
    capacity_ = capacity;
    destroy_all(ptr, size_);
    operator delete(ptr);
}

template <typename T>
void vector<T>::move_element(size_t from, size_t to) {
    assert(from >= to);
    if (from != to) {
        T copy = data_[from];
        for (size_t i = from; i > to; i--) {
            data_[i] = data_[i - 1];
        }
        data_[to] = copy;
    }
}
