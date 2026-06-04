#ifndef COLLVALUE_H
#define COLLVALUE_H

// Header-only

template <typename T>
class CollectingValue
{
public:
    static inline size_t comps = 0; // С С++17 можно!
    static inline size_t swaps = 0;
    static inline size_t moves = 0;

    T value;

    // Конструктор по умолчанию
    explicit CollectingValue(T val = T()) : value(val) {}

    // Копирование CollectingValue a = b;
    CollectingValue(const CollectingValue& other) : value(other.value)
    {
        moves++;
    }

    // Перемещение CollectingValue a = std::move(b); -- любит std::sort!
    CollectingValue(CollectingValue&& other) noexcept : value(std::move(other.value))
    {
        moves++;
    }

    // a = b;
    CollectingValue& operator=(const CollectingValue& other)
    {
        if (this != &other)
        {
            value = other.value;
            moves++;
        }
        return *this;
    }

    // a = std::move(b);
    CollectingValue& operator=(CollectingValue&& other) noexcept
    {
        if (this != &other)
        {
            value = std::move(other.value);
            moves++;
        }
        return *this;
    }

    // a < b
    bool operator<(const CollectingValue& other) const
    {
        comps++;
        return value < other.value;
    }

    // Для std::iota
    CollectingValue& operator++()
    {
        ++value;
        return *this;
    }

    static void reset_stats()
    {
        comps = 0;
        swaps = 0;
        moves = 0;
    }
};

/*
Внутри std::sort (упрощенно)
template <class RandomIt> void sort(RandomIt first, RandomIt last) {
    // ...
    using std::swap; // 1. Делаем std::swap видимым (страховка)
    swap(*i, *j);    // 2. Вызываем swap
    // ...
}
*/
// Кастомный swap, будет найдена автоматически при вызове std::swap для CollectingValue
template <typename T>
void swap(CollectingValue<T>& a, CollectingValue<T>& b)
{
    CollectingValue<T>::swaps++;
    std::swap(a.value, b.value);
}

#endif // COLLVALUE_H
