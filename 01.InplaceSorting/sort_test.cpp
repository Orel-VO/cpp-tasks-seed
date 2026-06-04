#include <algorithm>
#include <random>
#include <vector>

#include <gtest/gtest.h>

// А вот тут collvalue.h не надо, берите просто инты
#include "sorting.h"

// Но проверьте, что сортировки таки работают...
namespace
{
std::vector<int> sorted_copy(std::vector<int> v)
{
    std::sort(v.begin(), v.end());
    return v;
}

template <typename SortFn>
void expect_sort_matches_std(std::vector<int> data, SortFn sort_fn)
{
    const std::vector<int> expected = sorted_copy(data);
    sort_fn(data.begin(), data.end());
    EXPECT_EQ(data, expected);
}
} // namespace

TEST(BubbleSort, Empty)
{
    std::vector<int> v;
    bubble_sort(v.begin(), v.end());
    EXPECT_TRUE(v.empty());
}

TEST(BubbleSort, Single)
{
    std::vector<int> v = {42};
    bubble_sort(v.begin(), v.end());
    EXPECT_EQ(v, (std::vector<int> {42}));
}

TEST(BubbleSort, AlreadySorted)
{
    expect_sort_matches_std({1, 2, 3, 4}, [](auto b, auto e)
    {
        bubble_sort(b, e);
    });
}

TEST(BubbleSort, Reverse)
{
    expect_sort_matches_std({4, 3, 2, 1}, [](auto b, auto e)
    {
        bubble_sort(b, e);
    });
}

TEST(BubbleSort, Duplicates)
{
    expect_sort_matches_std({3, 1, 4, 1, 5, 9, 2, 6, 5}, [](auto b, auto e)
    {
        bubble_sort(b, e);
    });
}

TEST(QuickSort, Empty)
{
    std::vector<int> v;
    quick_sort(v.begin(), v.end());
    EXPECT_TRUE(v.empty());
}

TEST(QuickSort, Single)
{
    std::vector<int> v = {7};
    quick_sort(v.begin(), v.end());
    EXPECT_EQ(v, (std::vector<int> {7}));
}

TEST(QuickSort, AlreadySorted)
{
    expect_sort_matches_std({1, 2, 3, 4, 5}, [](auto b, auto e)
    {
        quick_sort(b, e);
    });
}

TEST(QuickSort, Reverse)
{
    expect_sort_matches_std({9, 8, 7, 6, 5}, [](auto b, auto e)
    {
        quick_sort(b, e);
    });
}

TEST(QuickSort, Duplicates)
{
    expect_sort_matches_std({2, 2, 2, 1, 0, 1}, [](auto b, auto e)
    {
        quick_sort(b, e);
    });
}

TEST(SortingIntegration, RandomDataVariousSizesBubble)
{
    std::mt19937 gen(42U);
    const int sizes[] = {0, 1, 2, 3, 10, 50, 100};
    for (int n : sizes)
    {
        std::vector<int> data(static_cast<std::size_t>(n));
        for (int& x : data)
        {
            x = static_cast<int>(gen() & 0x7fffffff);
        }
        expect_sort_matches_std(std::move(data), [](auto b, auto e)
        {
            bubble_sort(b, e);
        });
    }
}

TEST(SortingIntegration, RandomDataVariousSizesQuick)
{
    std::mt19937 gen(99U);
    const int sizes[] = {0, 1, 2, 3, 10, 50, 100};
    for (int n : sizes)
    {
        std::vector<int> data(static_cast<std::size_t>(n));
        for (int& x : data)
        {
            x = static_cast<int>(gen() & 0x7fffffff);
        }
        expect_sort_matches_std(std::move(data), [](auto b, auto e)
        {
            quick_sort(b, e);
        });
    }
}
