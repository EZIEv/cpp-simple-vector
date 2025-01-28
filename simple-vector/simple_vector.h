#pragma once

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <utility>

#include "array_ptr.h"

struct Capacity {
    size_t capacity;
};

Capacity Reserve(size_t new_capacity) {
    return {new_capacity};
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept : size_(0), capacity_(0) {}
    
    explicit SimpleVector(Capacity capacity) : ptr_(capacity.capacity), size_(0), capacity_(capacity.capacity) {}

    explicit SimpleVector(size_t size) : ptr_(size), size_(size), capacity_(size) {
        std::fill(ptr_.Get(), ptr_.Get() + size, Type{});
    }

    SimpleVector(size_t size, const Type& value) : ptr_(size), size_(size), capacity_(size) {
        std::fill(ptr_.Get(), ptr_.Get() + size, value);
    }

    SimpleVector(std::initializer_list<Type> init) : ptr_(init.size()), size_(init.size()), capacity_(init.size()) {
        std::copy(init.begin(), init.end(), ptr_.Get());
    }
    
    SimpleVector(const SimpleVector& other) : ptr_(other.GetSize()), size_(other.GetSize()), capacity_(other.GetSize()) {
        std::copy(other.begin(), other.end(), ptr_.Get());
    }
    
    SimpleVector(SimpleVector&& other) : size_(std::exchange(other.size_, 0)), capacity_(std::exchange(other.capacity_, 0)) {
        ptr_.swap(other.ptr_);
    }
    
    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            SimpleVector<Type> copy(rhs);
            this->swap(copy);
        }

        return *this;
    }
    
    SimpleVector& operator=(SimpleVector&& rhs) {
        if (this != &rhs) {
            ptr_.swap(rhs.ptr_);
            size_ = std::exchange(rhs.size_, 0);
            capacity_ = std::exchange(rhs.capacity_, 0);
        }
        
        return *this;
    }

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return ptr_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return ptr_[index];
    }

    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index is out of range");
        }
        return ptr_[index];
    }

    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index is out of range");
        }
        return ptr_[index];
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void Resize(size_t new_size) {
        if (new_size > capacity_) {
            ArrayPtr<Type> copy_ptr(new_size);

            std::copy(std::make_move_iterator(ptr_.Get()), std::make_move_iterator(ptr_.Get() + size_), copy_ptr.Get());
            std::generate(copy_ptr.Get() + size_, copy_ptr.Get() + new_size, [](){ return Type{}; });
            ptr_.swap(copy_ptr);

            capacity_ = new_size;
        } else if (new_size > size_) {
            std::generate(ptr_.Get() + size_, ptr_.Get() + new_size, [](){ return Type{}; });
        }

        size_ = new_size;
    }

    Iterator begin() noexcept {
        return ptr_.Get();
    }

    Iterator end() noexcept {
        return ptr_.Get() + size_;
    }

    ConstIterator begin() const noexcept {
        return ptr_.Get();
    }

    ConstIterator end() const noexcept {
        return ptr_.Get() + size_;
    }
    
    ConstIterator cbegin() const noexcept {
        return ptr_.Get();
    }

    ConstIterator cend() const noexcept {
        return ptr_.Get() + size_;
    }
    
    void PushBack(Type item) {
        if (size_ == capacity_) {
            ArrayPtr<Type> copy_ptr(capacity_ == 0 ? 1 : capacity_ * 2);

            std::copy(std::make_move_iterator(ptr_.Get()), std::make_move_iterator(ptr_.Get() + size_), copy_ptr.Get());
            ptr_.swap(copy_ptr);

            capacity_ = capacity_ == 0 ? 1 : capacity_ * 2;
        }
        
        ptr_[size_] = std::move(item);
        ++size_;
    }
    
    Iterator Insert(ConstIterator pos, Type value) {
        size_t index = static_cast<size_t>(pos - ptr_.Get());
        assert(index <= size_);

        if (size_ == capacity_) {
            ArrayPtr<Type> copy_ptr(capacity_ == 0 ? 1 : capacity_ * 2);

            std::copy(std::make_move_iterator(ptr_.Get()), std::make_move_iterator(ptr_.Get() + size_), copy_ptr.Get());
            ptr_.swap(copy_ptr);

            capacity_ = capacity_ == 0 ? 1 : capacity_ * 2;
        }
        
        std::copy_backward(std::make_move_iterator(ptr_.Get() + index), std::make_move_iterator(ptr_.Get() + size_), ptr_.Get() + size_ + 1);
        ptr_[index] = std::move(value);
        ++size_;
        
        return ptr_.Get() + index;
    }
    
    void PopBack() noexcept {
        assert(size_ > 0);
        --size_;
    }
    
    Iterator Erase(ConstIterator pos) {
        assert(size_ > 0);
        size_t index = static_cast<size_t>(pos - ptr_.Get());
        assert(index < size_);

        std::copy(std::make_move_iterator(ptr_.Get() + index + 1), std::make_move_iterator(ptr_.Get() + size_), ptr_.Get() + index);
        --size_;
        
        return ptr_.Get() + index;
    }
    
    void swap(SimpleVector& other) noexcept {
        ptr_.swap(other.ptr_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ArrayPtr<Type> copy_ptr(new_capacity);
            std::copy(std::make_move_iterator(ptr_.Get()), std::make_move_iterator(ptr_.Get() + size_), copy_ptr.Get());
            ptr_.swap(copy_ptr);
            capacity_ = new_capacity;
        }
    }

private:
    ArrayPtr<Type> ptr_;
    size_t size_;
    size_t capacity_;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}