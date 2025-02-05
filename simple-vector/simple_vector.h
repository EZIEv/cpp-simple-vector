#pragma once

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <utility>

#include "array_ptr.h"


// Структура для хранения информации о вместимости контейнера
struct Capacity {
    size_t capacity;
};

// Функция для создания объекта Capacity с заданной вместимостью
Capacity Reserve(size_t new_capacity) {
    return {new_capacity};
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    // Конструктор по умолчанию, создает пустой вектор с нулевой вместимостью
    SimpleVector() noexcept : size_(0), capacity_(0) {}
    
    // Конструктор с резервированием памяти
    explicit SimpleVector(Capacity capacity) : ptr_(capacity.capacity), size_(0), capacity_(capacity.capacity) {}

    // Конструктор с заданным размером, элементы инициализируются значением по умолчанию
    explicit SimpleVector(size_t size) : ptr_(size), size_(size), capacity_(size) {
        std::fill(ptr_.Get(), ptr_.Get() + size, Type{});
    }

    // Конструктор с заданным размером и значением
    SimpleVector(size_t size, const Type& value) : ptr_(size), size_(size), capacity_(size) {
        std::fill(ptr_.Get(), ptr_.Get() + size, value);
    }

    // Конструктор из списка инициализации
    SimpleVector(std::initializer_list<Type> init) : ptr_(init.size()), size_(init.size()), capacity_(init.size()) {
        std::copy(init.begin(), init.end(), ptr_.Get());
    }
    
    // Конструктор копирования
    SimpleVector(const SimpleVector& other) : ptr_(other.GetSize()), size_(other.GetSize()), capacity_(other.GetSize()) {
        std::copy(other.begin(), other.end(), ptr_.Get());
    }
    
    // Конструктор перемещения
    SimpleVector(SimpleVector&& other) : size_(std::exchange(other.size_, 0)), capacity_(std::exchange(other.capacity_, 0)) {
        ptr_.swap(other.ptr_);
    }
    
    // Оператор присваивания копированием
    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            SimpleVector<Type> copy(rhs);
            this->swap(copy);
        }
        return *this;
    }
    
    /// Оператор присваивания перемещением
    SimpleVector& operator=(SimpleVector&& rhs) {
        if (this != &rhs) {
            ptr_.swap(rhs.ptr_);
            size_ = std::exchange(rhs.size_, 0);
            capacity_ = std::exchange(rhs.capacity_, 0);
        }
        return *this;
    }

    // Возвращает текущий размер вектора
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает текущую вместимость вектора
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Проверяет, пуст ли вектор
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Оператор доступа по индексу
    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return ptr_[index];
    }

    // Оператор доступа по индексу (константная версия)
    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return ptr_[index];
    }

    // Доступ по индексу с проверкой границ
    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index is out of range");
        }
        return ptr_[index];
    }

    // Доступ по индексу с проверкой границ (константная версия)
    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index is out of range");
        }
        return ptr_[index];
    }

    // Очищает вектор
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер вектора
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

    // Возвращает итератор на начало вектора
    Iterator begin() noexcept {
        return ptr_.Get();
    }

    // Возвращает итератор на конец вектора
    Iterator end() noexcept {
        return ptr_.Get() + size_;
    }

    // Возвращает константный итератор на начало вектора
    ConstIterator begin() const noexcept {
        return ptr_.Get();
    }

    // Возвращает константный итератор на конец вектора
    ConstIterator end() const noexcept {
        return ptr_.Get() + size_;
    }
    
    // Возвращает константный итератор на начало вектора
    ConstIterator cbegin() const noexcept {
        return ptr_.Get();
    }

    // Возвращает константный итератор на конец вектора
    ConstIterator cend() const noexcept {
        return ptr_.Get() + size_;
    }

    // Добавляет элемент в конец вектора
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
    
    // Вставляет элемент на указанную позицию
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
    
    // Удаляет последний элемент
    void PopBack() noexcept {
        assert(size_ > 0);
        --size_;
    }

    // Удаляет элемент с указанной позиции
    Iterator Erase(ConstIterator pos) {
        assert(size_ > 0);
        size_t index = static_cast<size_t>(pos - ptr_.Get());
        assert(index < size_);

        std::copy(std::make_move_iterator(ptr_.Get() + index + 1), std::make_move_iterator(ptr_.Get() + size_), ptr_.Get() + index);
        --size_;
        
        return ptr_.Get() + index;
    }
    
    // Обменивается содержимым векторов
    void swap(SimpleVector& other) noexcept {
        ptr_.swap(other.ptr_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }
    
    // Резервирует память для хранения заданного количества элементов
    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ArrayPtr<Type> copy_ptr(new_capacity);
            std::copy(std::make_move_iterator(ptr_.Get()), std::make_move_iterator(ptr_.Get() + size_), copy_ptr.Get());
            ptr_.swap(copy_ptr);
            capacity_ = new_capacity;
        }
    }

private:
    ArrayPtr<Type> ptr_; // < Указатель на динамический массив
    size_t size_;        // < Количество элементов в векторе
    size_t capacity_;    // < Вместимость вектора
};

// Операторы сравнения
// ==, !=, <, >, <=, >=
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