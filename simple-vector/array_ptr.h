#pragma once

#include <cassert>
#include <cstdlib>
#include <utility>


template <typename Type>
class ArrayPtr {
public:
    // Инициализирует ArrayPtr нулевым указателем
    ArrayPtr() = default;

    // Создаёт в куче массив из size элементов типа Type.
    // Если size == 0, поле raw_ptr_ должно быть равно nullptr
    explicit ArrayPtr(size_t size) : raw_ptr_(size == 0 ? nullptr : new Type[size]) {
    }

    // Конструктор из сырого указателя, хранящего адрес массива в куче либо nullptr
    explicit ArrayPtr(Type* raw_ptr) noexcept : raw_ptr_(raw_ptr) {
    }

    // Запрещаем копирование
    ArrayPtr(const ArrayPtr&) = delete;

    // Конструктор перемещения
    ArrayPtr(ArrayPtr&& other) noexcept {
        raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);
    }

    // Высвобождает память по окончанию жизненного цикла объекта
    ~ArrayPtr() {
        delete[] raw_ptr_;
    }

    // Запрещаем присваивание копированием
    ArrayPtr& operator=(const ArrayPtr&) = delete;

    // Присваивание пермещением
    ArrayPtr& operator=(ArrayPtr&& other) noexcept {
        if (this != &other) {
            raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);
        }
        return *this;
    }

    // Прекращает владением массивом в памяти, возвращает значение адреса массива
    // После вызова метода указатель на массив должен обнулиться
    [[nodiscard]] Type* Release() noexcept {
        return std::exchange(raw_ptr_, nullptr);
    }

    // Возвращает ссылку на элемент массива с индексом index
    Type& operator[](size_t index) noexcept {
        assert(raw_ptr_ != nullptr);
        return raw_ptr_[index];
    }

    // Возвращает константную ссылку на элемент массива с индексом index
    const Type& operator[](size_t index) const noexcept {
        assert(raw_ptr_ != nullptr);
        return raw_ptr_[index];
    }

    // Возвращает true, если указатель ненулевой, и false в противном случае
    explicit operator bool() const {
        return raw_ptr_ != nullptr;
    }

    // Возвращает значение сырого указателя, хранящего адрес начала массива
    Type* Get() const noexcept {
        return raw_ptr_;
    }

    // Обменивается значениям указателя на массив с объектом other
    void swap(ArrayPtr& other) noexcept {
        std::swap(raw_ptr_, other.raw_ptr_);
    }

private:
    Type* raw_ptr_ = nullptr;   // < сырой указатель на массив
};