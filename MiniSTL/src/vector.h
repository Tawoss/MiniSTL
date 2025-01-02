#ifndef MY_VECTOR_H
#define MY_VECTOR_H

#include <iostream>
#include "iterator.h"
#include "algobase.h"
#include "utils.h"

template<class T>
struct my_vector{
    typedef T                                               my_valType;
    typedef my_valType*                                     my_pointer;
    typedef const my_valType*                               const_my_pointer;
    typedef my_valType&                                     my_reference;
    typedef const my_valType&                               const_my_reference;
    typedef my_valType*                                     my_iterator;
    typedef const my_valType*                               const_my_iterator;
    typedef mystl::reverse_iterator<my_iterator>            my_reverse_iterator;
    typedef const mystl::reverse_iterator<my_iterator>      const_my_reverse_iterator;
    typedef std::size_t                                     size_type;
    typedef __PTRDIFF_TYPE__                                difference_type;

    //vector的迭代器是原生指针类型 T*
    my_iterator    _start;
    my_iterator    _finish;
    my_iterator    _real_end;

    //默认构造函数
    my_vector():_start(nullptr),_finish(nullptr),_real_end(nullptr){}
    //传入参数为元素个数构造函数
    my_vector(size_type n)
    :_start(nullptr),
    _finish(nullptr),
    _real_end(nullptr)
    {
        while(n>0){
            push_back(my_valType());
            --n;
        }
    }
    my_vector(size_type n, const my_valType& val)
    :_start(nullptr),
    _finish(nullptr),
    _real_end(nullptr)
    {
        while(n>0){
            push_back(val);
            --n;
        }
    }
    //range构造函数
    template<class InputIterator>
    my_vector(InputIterator first,InputIterator last)
    :_start(nullptr),
    _finish(nullptr),
    _real_end(nullptr)
    {
        //range赋值可以封装成函数
        while(first!=last){
            push_back(*first);
            ++first;
        }
    }
    //拷贝构造
    my_vector(const my_vector& other){
        //std::cout<<"拷贝构造"<<std::endl;
        new (this) my_vector(other._start,other._finish);
    }
    //移动构造
    my_vector(my_vector&& other)noexcept
    :_start(other._start),
    _finish(other._finish),
    _real_end(other._real_end)
    {
        //std::cout<<"移动构造"<<std::endl;
        other._start=nullptr;
        other._finish=nullptr;
        other._real_end=nullptr;
    }
    //支持initializer_list构造
    my_vector(std::initializer_list<my_valType> ilist)
    :_start(nullptr),
    _finish(nullptr),
    _real_end(nullptr)
    {
        assign(ilist.begin(), ilist.end());
    }
    //析构函数
    ~my_vector(){
        if(_start){
            delete[] _start;
            _start=_finish=_real_end=nullptr;
        }
    }


    //返回第一个元素迭代器
    my_pointer data(){
        return _start;
    }
    const_my_pointer data() const{
        return _start;
    }
    my_iterator begin(){
        return data();
    }
    const_my_iterator cbegin() const {
        return const_my_iterator(data());
    }

    //返回最后一个元素后一个位置的迭代器
    my_iterator end(){
        return _finish;
    }
    const_my_iterator cend()const {
        return const_my_iterator(data()+size());
    }

    //rbegin()返回数组最后一个元素的反向迭代器
    my_reverse_iterator rbegin(){
        return my_reverse_iterator(end());
    }
    const_my_reverse_iterator crbegin() const {
        return const_my_reverse_iterator(end());
    }

    //rend()返回指向第一个元素所在位置前一个位置的反向迭代器
    my_reverse_iterator rend(){
        return my_reverse_iterator(begin());
    }
    const_my_reverse_iterator crend() const {
        return const_my_reverse_iterator(begin());
    }

    //front()返回第一个元素的引用
    my_reference front(){
        if(empty()){
            std::__throw_out_of_range_fmt("front() error: vector is empty");
        }
        return *_start;
    }

    //back返回最后一个元素的引用
    my_reference back(){
        if(empty()){
            std::__throw_out_of_range_fmt("back() error: vector is empty");
        }
        return *(_finish-1);
    }

    //返回容器的容量
    size_type capacity() const {
        return _real_end-_start;
    }

    //返回容器中元素的个数（ size()<=capacity() ）
    size_type size() const {
        return _finish-_start;
    }

    //返回容器能够存储的理论最大元素数量
    size_type max_size()const {
        return size_type(-1)/sizeof(T);
    }

    //empty()判断容器是否为空
    bool empty(){
        return size()==0;
    }

    //使用索引访问容器中的元素，自动进行边界检查
    my_reference at(size_type i){
        if(i>=size())
            std::__throw_out_of_range_fmt("at( %zu ) out of range",i);
        return (*this)[i];
    }

    //move_insert直接传入参数调用构造函数,在pos处生成元素
    template<class... Args>
    my_iterator move_insert(const_my_iterator pos,  Args... args){

    }

    //emplace在指定的位置直接生成一个元素
    template<typename... Args>
    my_iterator emplace(const_my_iterator pos, Args&&... args){
        // if(pos<_start||pos>_finish)
        //     std::__throw_out_of_range_fmt("emplace(pos, val)  pos is out of range");

        //判断是否需要扩大capacity
        if(_finish==_real_end){
            size_type len=pos-_start;
            size_type newcapacity=capacity()==0? 4:2*capacity();
            reserve(newcapacity);
            pos=_start+len;
        }
        //pos和pos后的元素全部后移一位
        vecmemmove((my_iterator)pos+1, pos, _finish-pos);
        //在pos处构造元素
        new ((void*)pos) my_valType(std::forward<Args>(args)...);
        ++_finish;
        return (my_iterator)pos;
    }

    //在指定位置出插入1个值为val的元素
    my_iterator insert(const_my_iterator pos,const my_valType& val){
        // if(pos<_start||pos>_finish)
        //     std::__throw_out_of_range_fmt("insert( ) out of range");

        //判断是否需要扩大capacity
        if(_finish==_real_end){
            size_type len=pos-_start;
            size_type newcapacity=capacity()==0? 4:2*capacity();
            reserve(newcapacity);
            pos=_start+len;
        }
        difference_type offset=pos-cbegin();
        my_iterator ppos=begin()+offset;
        //pos和pos后的元素全部后移一位
        vecmemmove(ppos+1, pos, _finish-pos);

        *ppos=val;
        ++_finish;
        //返回插入之前pos处的元素的迭代器
        return begin()+offset;
    }
    //移动插入
    my_iterator insert(const_my_iterator pos,my_valType&& val){return emplace(pos,mystl::move(val));}

    //在指定位置插入多个值为val的元素
    my_iterator insert(const_my_iterator pos, size_type n, const my_valType& val){
        if(pos<_start||pos>_finish)
            std::__throw_out_of_range_fmt("insert( ) out of range");

        //判断是否需要扩大capacity
        if(size()+n>capacity()){
            size_type len=pos-_start;
            size_type newcapacity=capacity()==0? n:capacity()+n;
            reserve(newcapacity);
            pos=_start+len;
        }
        difference_type offset=pos-cbegin();
        my_iterator ppos=begin()+offset;
        //pos和pos后的元素移至pos+n处
        vecmemmove(ppos+n, pos, _finish-pos);
        for(int i=0;i<n;++i){
            *(ppos+i)=val;
            ++_finish;
        }
        //返回插入之前pos处的元素的迭代器
        return ppos+n;
    }
    //在指定位置插入range中的元素
    template<class iter,typename = std::_RequireInputIter<iter>>
    my_iterator insert(const_my_iterator pos, iter first, iter last){
        for(;first!=last;++first)
            pos=insert(pos,*first)+1;
        my_iterator ret=(my_iterator)pos;
        return ret;
    }

    //删除指定位置的元素
    my_iterator erase(my_iterator pos){
        if(pos<_start||pos>=_finish)
            std::__throw_out_of_range_fmt("erase( ) out of range");

        vecmemmove(pos,pos+1,(_finish-pos-1));
        --_finish;
        *_finish=0;
        return pos;
    }
    //删除两个迭代器表示的range中的元素
    my_iterator erase(my_iterator first,my_iterator last){
        size_type len=last-first;
        if(first<_start||last>_finish)
            std::__throw_out_of_range_fmt("erase( first, last ) out of range");

        vecmemmove(first,last,(_finish-last));

        for(size_type i=0;i<len;++i){
            --_finish;
            *_finish=0;
        }
        return first;
    }

    //clear()删除所有元素，size变为0
    void clear(){
        erase(begin(),end());
    }

    //resize()改变容器中元素的个数
    void resize(size_type n,const my_valType& val){
        if(n>max_size())
            std::__throw_length_error("n cannot be larger than max_size() in vector::resize(n,val)");

        if(n<size()){
            erase(_start+n,_finish);
        }
        else{
            insert(end(),n-size(),val);
        }
    }

    //释放容器中多余的空间 _finish~_real_end
    void shrink_to_fit(){
        if(_finish<_real_end){
            size_type n=size();
            my_iterator new_start=new my_valType[n];
            //转移元素至新容器
            for(int i=0;i<n;++i){
                    new_start[i]=_start[i];
                }
                //删除旧容器分配的空间
                delete[] _start;
            _start=new_start;
            _finish=_start+n;
            _real_end=_finish;
        }
    }

    //增加容量至c
    void reserve(size_type n){
        if(n>max_size())
            std::__throw_length_error("n cannot be larger than max_size() in vector::reserve(n)");
        if(n>capacity()){
            size_type sz=size();
            my_iterator temp=new my_valType[n];
            vecmemmove(temp,_start,sz);
            _start=temp;
            _finish=_start+sz;
            _real_end=_start+n;
        }
    }

    void init_reserve(size_type n){
        if(n>max_size())
            std::__throw_length_error("n cannot be larger than max_size() in vector::reserve(n)");
        if(n>capacity()){
            size_type sz=size();
            my_iterator temp=new my_valType[n];
            //不转移元素
            _start=temp;
            _finish=_start+sz;
            _real_end=_start+n;
        }
    }

    //在容器尾部插入元素，如果容器元素数量达到容量上限，将容量扩大至原来的两倍
    void push_back(const my_valType& val){
        if(_finish==_real_end){
            size_type newcapcity= capacity()==0? 4:2*capacity();
            reserve(newcapcity);
        }
        *_finish=val;
        ++_finish;
    }
    //移动插入
    void push_back(my_valType&& val){
        emplace(end(),mystl::move(val));
    }
    //弹出最后一个元素（并没有释放空间，只是把size-1。 ）
    void pop_back(){
        if(empty())
            std::__throw_out_of_range_fmt("vector is empty cannot pop_back()");

        --_finish;
        *_finish=0;
    }

    //assign把range中的元素拷贝到自身容器中
    template<class iter>
    void assign(iter first, iter last){
        size_type len=last-first;
        if(len>capacity()){
            //创建新的capacity，删除容器中旧的内容
            size_type newcapacity=capacity()==0? len:capacity()+len;
            init_reserve(newcapacity);
            //把first~last中的元素复制到容器中
            vecmemcopy(_start,first,len);
            _finish=_start+len;
            _real_end=_start+newcapacity;
        }
        else{
            //把first~last中的元素复制到容器中，并改变容器的size（）
            vecmemcopy(_start,first,len);
            _finish=_start+len;
        }
    }

    //拷贝赋值
    my_vector& operator=(const my_vector& other){
        assign(other.cbegin(),other.cend());
        return *this;
    }

    //移动赋值
    my_vector& operator=(my_vector&& other){
        _start=other._start;
        _finish=other._finish;
        _real_end=other._real_end;
        other._start=nullptr;
        other._finish=nullptr;
        other._real_end=nullptr;
        return *this;
    }

    //[]运算符重载
    my_reference operator[](size_type n){
        if(n>=size())
            std::__throw_out_of_range_fmt("vector::at(%zu) is out of range, where _N = %zu",n,size());
        return *(_start + n);
    }

    const_my_reference operator[](size_type n) const{
        if(n>=size())
            std::__throw_out_of_range_fmt("vector::at(%zu) is out of range, where _N = %zu",n,size());
        return *(_start + n);
    }

    //vector的移动操作
    void* vecmemmove(my_valType* des, const my_valType* src, std::size_t n){
        void* ret=des;
        if (src > des)
        {
            //顺顺序
            while (n--)
            {
                *des = *(my_valType*)src;
                des = des + 1;
                src = (my_valType*)src + 1;
            }
        }
        else
        {
            //逆顺序
            while (n--)
            {
                *(des+n) = *((my_valType*)src + n);
            }
        }
        return ret;
    }

    //vector复制操作
    void* vecmemcopy(my_valType* des, const my_valType* src, std::size_t n){
        my_valType* pdes=des;
        my_valType* psrc=(my_valType*)src;
        void* ret=des;
        while(n>0){
            *pdes=*psrc;
            ++psrc;
            ++pdes;
            --n;
        }
        return ret;
    }

    //swap交换容器
    void swap(my_vector& other){
        if(this!=&other){
            mystl::swap(_start,other._start);
            mystl::swap(_finish,other._finish);
            mystl::swap(_real_end,other._real_end);
        }
    }

};

#endif
