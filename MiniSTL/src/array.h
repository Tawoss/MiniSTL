#ifndef MYSTL_ARRAY_H
#define MYSTL_ARRAY_H

//#include<array>
#include <iostream>
#include "iterator.h"
//using namespace std;

template<typename _T, std::size_t _N>
class Myarray_traits
{
public:
    //内置数组定义
    typedef _T my_arr[_N];
    //返回arr[__n]元素的引用
    static constexpr _T& get_ref(const my_arr& __t, std::size_t __n) noexcept{
        return const_cast<_T&>(__t[__n]); 
    }
    //返回数组的首地址指针
    static constexpr _T* start_ptr(const my_arr __t) noexcept{
        return const_cast<_T*>(__t);
    }
};

//*————————————————————————————————————————
template<typename _T>
struct Myarray_traits<_T, 0>
{
    struct my_arr { };

    //*nullptr 
    static constexpr _T&
    get_ref(const my_arr&, std::size_t) noexcept
    { return *static_cast<_T*>(nullptr);}

    static constexpr _T*
    start_ptr(const my_arr&) noexcept
    { return nullptr; }
};
//*————————————————————————————————————————


template<typename _T, std::size_t _N>
class myarray
{
public:
    typedef std::size_t                      my_sizeType;      //定义数组大小的声明类型
    typedef _T                               my_valType;       //定义数组类型
    typedef my_valType*                      my_iter;          //定义迭代器类型
    typedef mystl::reverse_iterator<my_iter>   my_reverse_iter;  //定义反响迭代器
    typedef const my_valType*                const_my_iter;    //定义const 迭代器
    typedef my_valType*                      my_ptr;           //定义指针类型
    typedef const my_valType*                const_my_ptr;
    typedef my_valType&                      my_ref;           //定义引用类型
    typedef const my_valType&                const_my_ref;     //定义const 引用类型
    typedef Myarray_traits<_T,_N>            my_ArrayTraits;   //定义内置数组类为AT_Type
    typename my_ArrayTraits::my_arr          M_elems;          //定义数组类中的内置数组为M_elems

    //返回数组首地址指针
    my_ptr data(){
        return my_ArrayTraits::start_ptr(M_elems);
    }
    const_my_ptr data() const {
        return my_ArrayTraits::start_ptr(M_elems);
    }
    //begin返回数组首地址元素的迭代器
    my_iter begin(){
        return my_iter(data());
    }
    //end返回数组尾地址元素后一个位置的迭代器
    my_iter end(){
        return my_iter(data()+_N);
    }
    //rbegin()返回数组最后一个元素的反向迭代器
    my_reverse_iter rbegin(){
        return my_reverse_iter(end());
    }
    //rend()返回数组第一个元素前一个位置的的反向迭代器
    my_reverse_iter rend(){
        return my_reverse_iter(begin());
    }
    //cbegin()
    const_my_iter cbegin(){
        return const_my_iter(data());
    }
    //cend()
    const_my_iter cend(){
        return const_my_iter(data()+_N);
    }
    //size(),在array类中，其作用与max_size相同，始终等于_N
    my_sizeType size(){
        return _N;
    }
    //max_size()，作用和size（）相同
    my_sizeType max_size(){
        return _N;
    }
    //empty(), _N为0则返回true
    bool empty(){
        return size()==0;
    }
    //at(_n),返回_n位置处的元素的引用，函数自动检查位置_n是否在允许范围内，不是则抛出out_of_range异常
    my_ref at(my_sizeType _n){
        if(_n>=_N){
            //超出索引范围，抛出out of range错误并终止程序
            std::__throw_out_of_range_fmt("array::at(%zu) is out of range, where _N = %zu",_n,_N);
            //return my_ArrayTraits::get_ref(M_elems,0);
        }
        //if(_n>_N) cerr<<"array["<<_n<<"] out of range"<<endl;
        return my_ArrayTraits::get_ref(M_elems,_n);
    }
    //重载[]运算符，让array支持下标访问
    my_ref operator[](my_sizeType _n){
        return my_ArrayTraits::get_ref(M_elems,_n);
    }
    //front(),返回数组第一个元素的引用，如果数组为空抛出异常
    my_ref front(){
        if(empty()) std::__throw_out_of_range_fmt("front() can not be applied on an empty array");
        return *begin();
    }
    //back(),返回数组第一个元素的直接引用，如果数组为空抛出异常
    my_ref back(){
        if(empty()) std::__throw_out_of_range_fmt("back() can not be applied on an empty array");
        return *end();
    }
    //fill(val), 将val赋值给容器中的每一个元素
    void fill(my_valType val){
        std::fill_n(begin(),size(),val);
    }
    //swap(myarray other), 交换两个数组的元素， 要求两个数组类型和大小必须相同,函数接受的other类型是self的<_T,_N>
    void swap(myarray& other){
        std::swap_ranges(begin(), end(), other.begin());
    }
};
#endif
