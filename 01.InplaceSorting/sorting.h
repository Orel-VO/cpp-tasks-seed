#ifndef SORTING_H
#define SORTING_H

// Header-only
#include <algorithm>
#include <iterator>

// Для swap
#include "collvalue.h"

// ----------------------------- Пузырёк (например) --------------------------------------
namespace sorting_detail
{
template <typename RandomIt>
RandomIt median_of_three(RandomIt a, RandomIt b, RandomIt c)
{
    if (*a < *b)
    {
        if (*b < *c)
        {
            return b;
        }
        if (*a < *c)
        {
            return c;
        }
        return a;
    }
    if (*a < *c)
    {
        return a;
    }
    if (*b < *c)
    {
        return c;
    }
    return b;
}

template <typename RandomIt>
RandomIt lomuto_partition(RandomIt begin, RandomIt end)
{
    RandomIt last = std::prev(end);
    const auto n = std::distance(begin, end);
    RandomIt mid = begin + (n - 1) / 2;
    RandomIt pivot_it = median_of_three(begin, mid, last);
    std::iter_swap(pivot_it, last);
    RandomIt store = begin;
    for (RandomIt it = begin; it != last; ++it)
    {
        if (*it < *last)
        {
            std::iter_swap(it, store);
            ++store;
        }
    }
    std::iter_swap(store, last);
    return store;
}

template <typename RandomIt>
void quick_sort_impl(RandomIt begin, RandomIt end)
{
    using Diff = typename std::iterator_traits<RandomIt>::difference_type;
    for (;;)
    {
        const Diff n = std::distance(begin, end);
        if (n < 2)
        {
            return;
        }
        if (n == 2)
        {
            RandomIt j = std::next(begin);
            if (*j < *begin)
            {
                std::iter_swap(begin, j);
            }
            return;
        }
        RandomIt p = lomuto_partition(begin, end);
        const Diff left = std::distance(begin, p);
        const Diff right = std::distance(std::next(p), end);
        if (left < right)
        {
            quick_sort_impl(begin, p);
            begin = std::next(p);
        }
        else
        {
            quick_sort_impl(std::next(p), end);
            end = p;
        }
    }
}
} // namespace sorting_detail

template <typename Iterator>
void bubble_sort(Iterator begin, Iterator end)
{
    if (begin == end)
    {
        return;
    }
    for (Iterator i = begin; i != end; ++i)
    {
        bool swapped = false;
        for (Iterator j = begin;; ++j)
        {
            Iterator j_next = std::next(j);
            if (j_next == end)
            {
                break;
            }
            if (*j_next < *j)
            {
                std::iter_swap(j, j_next);
                swapped = true;
            }
        }
        if (!swapped)
        {
            break;
        }
    }
}

// --------------------------- QuickSort (например) ---------------------------------------

template <typename Iterator>
void quick_sort(Iterator begin, Iterator end)
{
    sorting_detail::quick_sort_impl(begin, end);
}

#endif // SORTING_H
