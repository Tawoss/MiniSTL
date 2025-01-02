#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <iostream>
namespace mystl{

// lbound_dispatch 的 random_access_iterator_tag 版本
template <class RandomIter, class T>
RandomIter lbound_dispatch(RandomIter first, RandomIter last, const T& value, random_access_iterator_tag){
    auto len = last - first;
    auto half = len;
    RandomIter middle;
    while (len > 0)
    {
        half = len >> 1;
        middle = first + half;
        if (*middle < value)
        {
        first = middle + 1;
        len = len - half - 1;
        }
        else
        {
        len = half;
        }
    }
    return first;
}

template <class ForwardIter, class T>
ForwardIter lower_bound(ForwardIter first, ForwardIter last, const T& value){
    return lbound_dispatch(first, last, value, iterator_category(first));
}

// 重载版本使用函数对象 comp 代替比较操作
// lbound_dispatch 的 forward_iterator_tag 版本
template <class ForwardIter, class T, class Compared>
ForwardIter lbound_dispatch(ForwardIter first, ForwardIter last, const T& value, forward_iterator_tag, Compared comp){
    auto len = mystl::distance(first, last);
    auto half = len;
    ForwardIter middle;
    while (len > 0)
    {
        half = len >> 1;
        middle = first;
        mystl::advance(middle, half);
        if (comp(*middle, value))
        {
        first = middle;
        ++first;
        len = len - half - 1;
        }
        else
        {
        len = half;
        }
    }
    return first;
}

// lbound_dispatch 的 random_access_iterator_tag 版本
template <class RandomIter, class T, class Compared>
RandomIter lbound_dispatch(RandomIter first, RandomIter last, const T& value, random_access_iterator_tag, Compared comp){
    auto len = last - first;
    auto half = len;
    RandomIter middle;
    while (len > 0)
    {
        half = len >> 1;
        middle = first + half;
        if (comp(*middle, value))
        {
        first = middle + 1;
        len = len - half - 1;
        }
        else
        {
        len = half;
        }
    }
    return first;
}

template <class ForwardIter, class T, class Compared>
ForwardIter lower_bound(ForwardIter first, ForwardIter last, const T& value, Compared comp){
    return lbound_dispatch(first, last, value, iterator_category(first), comp);
}


}

#endif
