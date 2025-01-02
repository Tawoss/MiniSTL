#ifndef MY_UTILS_H
#define MY_UTILS_H

#include<iostream>

namespace mystl{

//move
template <class T>
constexpr typename std::remove_reference<T>::type&& move(T&& arg) noexcept {
    return static_cast<typename std::remove_reference<T>::type&&>(arg);
}

//forward
template <class T>
constexpr T&& forward(typename std::remove_reference<T>::type& arg) noexcept{
    return static_cast<T&&>(arg);
}

template <class T>
constexpr T&& forward(typename std::remove_reference<T>::type&& arg) noexcept{
    //源码对T进行了是否是左值引用的检查 is_lvalue_reference<T>::value
    return static_cast<T&&>(arg);
}

//把迭代器first-last范围赋值为val
template<class iter,class T>
void fill_range(iter first, iter last, const T& val){
    iter cur=first;
    try{
        for(;cur!=last;++cur)
            new(&*cur) T(val);
    }
    catch(...){
        for(;first!=cur;++(*cur))
            ::operator delete(&*first);
    }
}

// 全局函数，让两个数据成为一个 pair
template <class Ty1, class Ty2>
std::pair<Ty1, Ty2> make_pair(Ty1&& first, Ty2&& second){
  return std::pair<Ty1, Ty2>(mystl::forward<Ty1>(first), mystl::forward<Ty2>(second));
}

}

#endif


