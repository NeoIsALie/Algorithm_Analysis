#include <iostream>
#include <vector>
#include <numeric>
#include <random>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <functional>
#include <fstream>
#include <string>

std::vector<int> generateRandomVector(int size)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 30);

    std::vector<int> vec (size, 0);

    for (size_t i = 0; i < size; i++)
        vec[i] = dist(rng);

    return vec;
}

template <typename RandomAccessIterator>
void bubble_sort(RandomAccessIterator begin, RandomAccessIterator end) {
  bool swapped = true;
  while (begin != end-- && swapped) {
    swapped = false;
    for (auto i = begin; i != end; ++i) {
      if (*(i + 1) < *i) {
        std::iter_swap(i, i + 1);
        swapped = true;
      }
    }
  }
}

void bubbleSort(std::vector<int> vec)
{
      int temp;
      bool swapped;
      for (size_t i = 0; i < vec.size(); i++)
      {
         swapped = false;
         for (size_t j = 0; j < vec.size() - i - 1; j++)
         {
            if (vec[j] > vec[j + 1])
            {
                temp = vec[j];
                vec[j] = vec[j + 1];
                vec[j + 1] = temp;
                swapped = true;
            }
         }
         if (!swapped)
            break;
      }
}

template<typename RandomAccessIterator, typename Order>
 void mergesort(RandomAccessIterator first, RandomAccessIterator last, Order order)
{
  if (last - first > 1)
  {
    RandomAccessIterator middle = first + (last - first) / 2;
    mergesort(first, middle, order);
    mergesort(middle, last, order);
    std::inplace_merge(first, middle, last, order);
  }
}

template<typename RandomAccessIterator>
 void mergesort(RandomAccessIterator first, RandomAccessIterator last)
{
  mergesort(first, last, std::less<typename std::iterator_traits<RandomAccessIterator>::value_type>());
}

template<typename T>
 T median(T t1, T t2, T t3)
{
  if (t1 < t2)
  {
    if (t2 < t3)
      return t2;
    else if (t1 < t3)
      return t3;
    else
      return t1;
  }
  else
  {
    if (t1 < t3)
      return t1;
    else if (t2 < t3)
      return t3;
    else
      return t2;
  }
}

template<typename Order> struct non_strict_op:
  public std::binary_function<typename Order::second_argument_type,
                              typename Order::first_argument_type,
                              bool>
{
  non_strict_op(Order o): order(o) {}
  bool operator()(typename Order::second_argument_type arg1,
                  typename Order::first_argument_type arg2) const
  {
    return !order(arg2, arg1);
  }
private:
  Order order;
};

template<typename Order> non_strict_op<Order> non_strict(Order o)
{
  return non_strict_op<Order>(o);
}

template<typename RandomAccessIterator,
         typename Order>
 void quicksort(RandomAccessIterator first, RandomAccessIterator last, Order order)
{
  if (first != last && first+1 != last)
  {
    typedef typename std::iterator_traits<RandomAccessIterator>::value_type value_type;
    RandomAccessIterator mid = first + (last - first)/2;
    value_type pivot = median(*first, *mid, *(last-1));
    RandomAccessIterator split1 = std::partition(first, last, std::bind2nd(order, pivot));
    RandomAccessIterator split2 = std::partition(split1, last, std::bind2nd(non_strict(order), pivot));
    quicksort(first, split1, order);
    quicksort(split2, last, order);
  }
}

template<typename RandomAccessIterator>
 void quicksort(RandomAccessIterator first, RandomAccessIterator last)
{
  quicksort(first, last, std::less<typename std::iterator_traits<RandomAccessIterator>::value_type>());
}

template<typename RandomAccessIterator,
         typename Order>
 void quicksortSimple(RandomAccessIterator first, RandomAccessIterator last, Order order)
{
  if (last - first > 1)
  {
    RandomAccessIterator split = std::partition(first+1, last, std::bind2nd(order, *first));
    std::iter_swap(first, split-1);
    quicksort(first, split-1, order);
    quicksort(split, last, order);
  }
}

template<typename RandomAccessIterator>
 void quicksortSimple(RandomAccessIterator first, RandomAccessIterator last)
{
  quicksortSimple(first, last, std::less<typename std::iterator_traits<RandomAccessIterator>::value_type>());
}
