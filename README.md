# SimpleVector

### Описание

**SimpleVector** — это собственная реализация динамического массива (аналог std::vector), поддерживающая основные операции работы с контейнером: добавление, удаление, изменение размера, резервирование памяти и итерацию.

### Возможности

- Поддержка динамического изменения размера

- Оптимизированное управление памятью

- Конструкторы копирования и перемещения

- Операторы присваивания копированием и перемещением

- Итераторы для удобного перебора элементов

- Методы PushBack, PopBack, Insert, Erase, Resize, Reserve, Clear, swap

- Операторы сравнения (==, !=, <, >, <=, >=)

### Установка

Для использования SimpleVector достаточно включить заголовочный файл в ваш проект:
```cpp
#include "simple_vector.h"
```

### Пример использования
```cpp
#include <iostream>

#include "simple_vector.h"


int main() {
    SimpleVector<int> vec;
    vec.PushBack(10);
    vec.PushBack(20);
    vec.PushBack(30);
    
    for (int value : vec) {
        std::cout << value << " ";
    }
    return 0;
}
```

Вывод:
```console
10 20 30
```

### Требования

Компилятор с поддержкой C++17 и выше