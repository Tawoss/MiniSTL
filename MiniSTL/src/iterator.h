#ifndef MY_ITERATOR_H
#define MY_ITERATOR_H

#include "type_traits.h"
namespace mystl{

//五种迭代器类型
using input_iterator_tag = std::input_iterator_tag;
using output_iterator_tag = std::output_iterator_tag;
using forward_iterator_tag = std::forward_iterator_tag;
using bidirectional_iterator_tag = std::bidirectional_iterator_tag;
using random_access_iterator_tag = std::random_access_iterator_tag;

// struct input_iterator_tag {};
// struct output_iterator_tag {};
// struct forward_iterator_tag : public input_iterator_tag {};
// struct bidirectional_iterator_tag : public forward_iterator_tag {};
// struct random_access_iterator_tag : public bidirectional_iterator_tag {};

//iterator模板类，自行开发迭代器应继承自该类
template<class Category,class T, class Distance = __PTRDIFF_TYPE__, class Pointer = T*, class Reference = T&>
struct iterator
{
    //iterator五种相应型别
    typedef Category      iterator_category;
    typedef T             value_type;
    typedef Distance      difference_type;
    typedef Pointer       pointer;
    typedef Reference     reference;
};

//迭代器特性萃取类（traits）
template<class iterator>
struct my_iterator_traits
{
    //萃取类获取iterator型别
    typedef typename iterator::iterator_category     iterator_category;
    typedef typename iterator::value_type            value_type;
    typedef typename iterator::difference_type       difference_type;
    typedef typename iterator::pointer               pointer;
    typedef typename iterator::reference             reference;
};

//针对原生指针（int*， char* .....）的iterator traits特化版
template<class T>
struct my_iterator_traits<T*>
{
    typedef random_access_iterator_tag     iterator_category;
    typedef T                              value_type;
    typedef __PTRDIFF_TYPE__               difference_type;
    typedef T*                             pointer;
    typedef T&                             reference;
};

//针对指向常量的原生指针（const int*， const char* .....）的iterator traits特化版
template<class T>
struct my_iterator_traits<const T*>
{
    typedef random_access_iterator_tag     iterator_category;
    typedef T                              value_type;
    typedef __PTRDIFF_TYPE__               difference_type;
    typedef const T*                       pointer;
    typedef const T&                       reference;
};

//获取iterator的category型别属性，也就是iterator的类别
template<class iterator>
typename my_iterator_traits<iterator>::iterator_category
iterator_category ( const iterator& ) {
    return typename my_iterator_traits<iterator>::iterator_category();
}

//获取iterator的difference_type型别属性
template<class iterator>
typename my_iterator_traits<iterator>::difference_type*
distance_type (const iterator& ) {
    return static_cast<typename my_iterator_traits<iterator>::difference_type*>(0);
}

//获取iterator的value_type型别属性
template<class iterator>
typename my_iterator_traits<iterator>::value_type*
value_type ( const iterator& ) {
    return static_cast<typename my_iterator_traits<iterator>::value_type*>(0);
}

//distance函数计算迭代器之间的距离,有两种，一种计算input iterator tag，  另一种计算 random access iterator tag
//计算Input Iterator之间的距离
template<class InputIterator>
typename my_iterator_traits<InputIterator>::difference_type
distance_match(InputIterator first, InputIterator last, input_iterator_tag) {
    typename my_iterator_traits<InputIterator>::difference_type n=0;
    while(first!=last){
        ++first; ++n;
    }
    return n;
}

//计算Random Access Iterator之间的距离
template<class RandomAccessIterator>
typename my_iterator_traits<RandomAccessIterator>::difference_type
distance_match(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
    return last-first;
}

//distance_match统一调用函数，根据传入iterator的category型别来判断使用哪一种计distance_match的函数
template<class MyInputIterator>
typename my_iterator_traits<MyInputIterator>::difference_type
distance(MyInputIterator first, MyInputIterator last) {
    //iterator_category方法获取传入iterator的category型别
    return distance_match(first,last,iterator_category(first));
}

//advance函数可以让迭代器前进距离n
//Input Iterator的advance函数
template<class InputIterator, class Distance>
void advance_match(InputIterator& i, Distance n, input_iterator_tag){
    while(n--) ++i;
}

//Bidirectional Iteartor的advance函数
template<class BidirectionalIterator, class Distance>
void advance_match(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag){
    if(n>=0)
        while(n--) ++i;
    else
        while(n++) --i;
}

//Random Access Iterator的advance函数
template<class RandomAccessIterator, class Distance>
void advance_match(RandomAccessIterator& i, Distance n, random_access_iterator_tag){
    i+=n;
}

//advance_match统一调用函数，根据传入iterator的category型别来判断使用哪一种advance_match的函数
template<class MyInputIterator, class Distance>
void advance(MyInputIterator& i, Distance n){
    advance_match(i,n,iterator_category(i));
}


//////////////////////////////////////以下是reverse_iterator反向迭代器的实现/////////////////////////////////////

template<class iterator>
class reverse_iterator
{
private:
    iterator current;          //记录对应的正向迭代器
public:
    //定义reverse_iterator的五种内嵌型别
    typedef typename my_iterator_traits<iterator>::iterator_category     iterator_category;
    typedef typename my_iterator_traits<iterator>::value_type            value_type;
    typedef typename my_iterator_traits<iterator>::difference_type       difference_type;
    typedef typename my_iterator_traits<iterator>::pointer               pointer;
    typedef typename my_iterator_traits<iterator>::reference             reference;

    typedef iterator                       iterator_type;      //定义正向迭代器别名
    typedef reverse_iterator<iterator>     self;               //定义反向迭代器别名

    //reverse_iterator默认构造函数
    reverse_iterator(){}
    //reverse_iterator有参构造函数，传入参数分别是正向迭代器或者反向迭代器
    reverse_iterator(iterator_type in_itr) : current(in_itr) {}
    reverse_iterator(const self& in_itr) : current(in_itr.current) {}

    //取出正向迭代器
    iterator_type base() const {
        return current;
    }

    //重载解引用运算符
    reference operator*()const{
        iterator temp=current;
        //返回正向迭代器前一个位置
        return *(--temp);
    }

    //重载成员访问运算符
    pointer operator->()const{
        return &(operator*());
    }

    //++变为--
    self& operator++(){//前置++，//返回引用，是一个可修改的左值
        --current;
        return *this;
    }
    self operator++(int){//后置++
        self temp=*this;
        --current;
        return temp;
    }

    //--变为++
    self& operator--(){//前置--，//返回引用，是一个可修改的左值
        ++current;
        return *this;
    }
    self operator--(int){//后置--
        self temp=*this;
        ++current;
        return temp;
    }

    //+-方向逆转
    self operator+(difference_type n)const{
        return self(current-n);
    }
    self& operator+=(difference_type n){//返回引用，是一个可修改的左值
        current-=n;
        return *this;
    }
    self operator-(difference_type n)const{
        return self(current+n);
    }
    self& operator-=(difference_type n){//返回引用，是一个可修改的左值
        current+=n;
        return *this;
    }

    // this为指向reverse_iterator类的C++指针，*this的*不会使用本类中重载的*，*this表示reverse_iterator类，所
    // 以+会调用本例中重载的+，+返回的是reverse_iterator<iterator>反向迭代器，所以外层的*会调用反向迭代器重载的*
    reference operator[](difference_type n){
        return *(*this+n);
    }
};

//重载比较运算符
template<class iterator>
bool operator==(const reverse_iterator<iterator>& l,const reverse_iterator<iterator>& r){
    return l.base()==r.base();
}

template<class iterator>
bool operator!=(const reverse_iterator<iterator>& l,const reverse_iterator<iterator>& r){
    return !(l==r);
}

// template<class iterator>
// bool operator>(const reverse_iterator<iterator>& l,const reverse_iterator<iterator>& r){
//     return !(r.base()>l.base());
// }

template<class iterator>
bool operator<(const reverse_iterator<iterator>& l,const reverse_iterator<iterator>& r){
    //反向迭代器需要反向，eg. 1,2,3,4,5  反向迭代器1.base()为4，指向5；反向迭代器2.base()为3，指向4
    //1.base()>2.base()，但是由于是反向迭代器，所以反向迭代器2应该大于反向迭代器1
    return r.base() < l.base();
}

template<class iterator>
bool operator>(const reverse_iterator<iterator>& l,const reverse_iterator<iterator>& r){
    //由于重载了operator<， r和l所以这里直接判断r<l，
    return r < l;
}

template<class iterator>
bool operator<=(const reverse_iterator<iterator>& l,const reverse_iterator<iterator>& r){
    return ! (r < l);
}

template<class iterator>
bool operator>=(const reverse_iterator<iterator>& l,const reverse_iterator<iterator>& r){
    return ! (l < r);
}
}
#endif
