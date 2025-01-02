#ifndef LIST_H
#define LIST_H

#include<iostream>
#include "iterator.h"
#include "algobase.h"
#include "utils.h"

template<class T>
struct mylist_node{
    typedef T                       valType;
    typedef mylist_node<valType>*     pointer;
    pointer   prev;
    pointer   next;
    valType   data;

    //默认构造
    mylist_node()=default;
    //拷贝构造
    mylist_node(const mylist_node& other):prev(other.prev),next(other.next),data(other.data){}
    //传入参数拷贝构造
    mylist_node(const T& val): data(val) {}
    //移动构造
    mylist_node(mylist_node&& other) noexcept :prev(mystl::move(other.prev)),next(mystl::move(other.next)),data(mystl::move(other.data)) {}
    //传入参数移动构造
    mylist_node(T&& val):data(mystl::move(val)) {}

};

template<class T>
struct mylist_iterator:public mystl::iterator<mystl::bidirectional_iterator_tag, T>{
    //stl标准要求的内嵌型别定义
    typedef mystl::bidirectional_iterator_tag           iteartor_category;
    typedef T                                           value_type;
    typedef __PTRDIFF_TYPE__                            difference_type;
    typedef value_type*                                 pointer;
    typedef value_type&                                 reference;

    typedef mylist_node<value_type>*                    node_ptr;
    typedef mylist_iterator<T>                          self;

    node_ptr  node;

    //默认构造
    mylist_iterator(){}

    //传入参数为一个list_node
    mylist_iterator(node_ptr x): node(x){}

    //拷贝构造
    mylist_iterator(const mylist_iterator& other): node(other.node) {}

    //重载解引用操作符
    reference operator*() const {return node->data;}

    //重载成员指针访问运算符
    pointer operator->() const {return &(operator*());}

    //重载前置++
    self& operator++(){
        node=node->next;
        return *this;
    }
    //重载后置++
    self operator++(int){
        self temp=*this;
        ++*this;
        return temp;
    }

    //重载前置--
    self& operator--(){
        node=node->prev;
        return *this;
    }
    //重载后置--
    self operator--(int){
        self temp=*this;
        --*this;
        return temp;
    }

    //重载比较运算符
    bool operator==(const self& other) const {return node==other.node;}
    bool operator!=(const self& other) const {return node!=other.node;}
};

template<class T>
struct mylist{
    //mylist类的内嵌型别定义
    typedef T                                               my_valType;
    typedef mylist_node<my_valType>                         my_node;
    typedef my_node*                                        node_ptr;
    typedef my_valType&                                     my_reference;
    typedef mylist_iterator<my_valType>                     my_iterator;
    typedef const mylist_iterator<my_valType>               const_my_iterator;
    typedef mystl::reverse_iterator<my_iterator>            my_reverse_iterator;
    typedef const mystl::reverse_iterator<my_iterator>      const_my_reverse_iterator;
    typedef std::size_t                                     size_type;
    typedef __PTRDIFF_TYPE__                                difference_type;

    node_ptr    endBlankNode;
    size_type   _size;

    //默认构造函数
    mylist(){
        default_init();
    }

    //初始化容器大小为n，值为默认值
    mylist(size_type n){
        default_init();
        fill_init(n,my_valType());
    }

    //初始化容器大小为n，值为val
    mylist(size_type n,const my_valType& val){
        default_init();
        fill_init(n,val);
    }

    //range构造函数
    template<class iter>
    mylist(iter first,iter last){
        default_init();
        range_init(first,last);
    }

    //拷贝构造
    mylist(const mylist& other){
        //std::cout<<"copy constructor called"<<std::endl;
        default_init();
        range_init(other.cbegin(),other.cend());
    }

    //移动构造
    mylist(mylist&& other)
    :endBlankNode(other.endBlankNode),
    _size(other._size)
    {
        other.endBlankNode=nullptr;
        other._size=0;
    }

    //支持initializer_list构造
    mylist(std::initializer_list<my_valType> ilist){
        default_init();
        range_init(ilist.begin(),ilist.end());
    }

    //析构函数
    ~mylist(){
        //clear();
    }

    //拷贝赋值
    mylist& operator=(const mylist& other){
        if(this != &other){
            assign(other.cbegin(),other.cend());
        }
        return *this;
    }

    //移动赋值
    mylist& operator=(mylist&& other){
        clear();
        endBlankNode=other.endBlankNode;
        _size=other._size;
        other.endBlankNode=nullptr;
        other._size=0;
        return *this;
    }

    //默认初始化容器
    void default_init(){
        _size=0;
        endBlankNode=create_node(my_valType());
        endBlankNode->prev=endBlankNode;
        endBlankNode->next=endBlankNode;
    }

    //初始化容器，大小为n,值为val
    void fill_init(size_type n,const my_valType& val){
        for(;n>0;--n)
            push_back(val);
    }

    //以两个迭代器range来初始化容器
    template<class iter>
    void range_init(iter first, iter last){
        while(first!=last){
            push_back(*first);
            ++first;
        }
    }

    // //创建node函数,自动转换参数为list的元素类型
    // template<class cT>
    // node_ptr create_node(cT val){
    //     return new mylist_node<my_valType>(val);
    // }
    //
    template<typename... Args>
    node_ptr create_node(Args&&... args){
        node_ptr tem=new mylist_node<my_valType>();
        new (&tem->data) my_valType(std::forward<Args>(args)...);
        return tem;
    }

    //返回容器当前的元素个数
    size_type size(){
        return _size;
    }

    //返回容器能够存储的理论最大元素数量
    size_type max_size()const {
        return size_type(-1)/sizeof(T);
    }

    //判断容器是否为空
    bool empty(){
        return _size==0;
    }

    //返回指向容器中第一个元素的双向迭代器  const 和 non-const
    my_iterator begin(){
        return my_iterator(endBlankNode->next);
    }
    const_my_iterator cbegin() const {
        return const_my_iterator(endBlankNode->next);
    }
    //返回指向最后一个元素的反向双向迭代器    const 和 non-const
    my_reverse_iterator rbegin(){
        return my_reverse_iterator(end());
    }
    const_my_reverse_iterator crbegin(){
        return const_my_reverse_iterator(end());
    }

    //返回指向容器中最后一个元素所在位置的下一个位置的双向迭代器   const 和 non-const
    my_iterator end(){
        return my_iterator(endBlankNode);
    }
    const_my_iterator cend()const{
        return const_my_iterator(endBlankNode);
    }
    //返回指向第一个元素所在位置前一个位置的反向双向迭代器       const 和 non-const
    my_reverse_iterator rend(){
        return my_reverse_iterator(begin());
    }
    const_my_reverse_iterator crend(){
        return const_my_reverse_iterator(begin());
    }

    //返回第一个元素的引用
    my_reference front(){
        return *begin();
    }

    //返回最后一个元素的引用
    my_reference back(){
        return *(--end());
    }

    //assign()将容器中的元素替换为range中的元素
    template<class iter>
    void assign(iter first, iter last){
        clear();
        range_init(first,last);
    }

    //resize()改变实际元素的个数,超过原来容器的部分用val补齐
    void resize(size_type n, const my_valType& val){
        my_iterator start=begin();
        size_type len=0;
        for(;start!=end()&&len<n;++start,++len);
        if(len==n)
            erase(start,end());
        else
            fill_insert(start,n-len,val);

    }
    void resize(size_type n){
        return resize(n,my_valType());
    }

    //swap交换两个容器   类型必须一样，size可以不同
    void swap(mylist<my_valType>& other){
        mystl::swap(this->endBlankNode, other.endBlankNode);
        mystl::swap(this->_size, other._size);
    }

    //在容器尾部插入一个元素
    void push_back(const my_valType& val){
        node_ptr temp=create_node(val);

        temp->prev=endBlankNode->prev;
        temp->next=endBlankNode;
        endBlankNode->prev->next=temp;
        endBlankNode->prev=temp;

        _size++;
    }
    //移动插入
    void push_back(my_valType&& val){
        emplace_back(mystl::move(val));
    }

    //在容器头部插入一个元素
    void push_front(const my_valType& val){
        node_ptr temp=create_node(val);

        temp->prev=endBlankNode;
        temp->next=endBlankNode->next;
        endBlankNode->next->prev=temp;
        endBlankNode->next=temp;

        _size++;
    }

    //删除容器头部的一个元素
    void pop_front(){
        erase(begin());
    }

    //删除容器尾部的一个元素
    void pop_back(){
        erase(--end());
    }

    //emplace()在容器中的指定位置插入元素   该函数和 insert() 功能相同，但效率更高
    template<class... Args>
    my_iterator emplace(const_my_iterator pos, Args... args){
        return move_insert(pos,std::forward<Args>(args)...);
    }

    //在容器尾部直接生成一个元素   该函数和 push_back() 的功能相同，但效率更高。
    template<class... Args>
    void emplace_back(Args... args){
        move_insert(end(),std::forward<Args>(args)...);
    }

    //在容器头部生成一个元素   该函数和 push_front() 的功能相同，但效率更高。
    template<class... Args>
    void emplace_front(Args... args){
        move_insert(begin(),std::forward<Args>(args)...);
    }

    //直接传入参数调用构造函数,在pos处创建node
    template<class... Args>
    my_iterator move_insert(my_iterator pos, Args... args){
        node_ptr temp=create_node(std::forward<Args>(args)...);

        temp->prev=pos.node->prev;
        temp->next=pos.node;
        pos.node->prev->next=temp;
        pos.node->prev=temp;

        _size++;
        return temp;
    }

    //在pos处插入一个node
    my_iterator copy_insert(const_my_iterator pos,const my_valType& val){
        node_ptr temp=create_node(val);

        temp->prev=pos.node->prev;
        temp->next=pos.node;
        pos.node->prev->next=temp;
        pos.node->prev=temp;

        _size++;
        return temp;
    }

    //在pos处插入n个值为val的节点
    my_iterator fill_insert(const_my_iterator pos, size_type n, const my_valType& val){
        auto ret=pos;++ret;
        for(;n>0;--n)
            copy_insert(pos,val);
        return ret;
    }

    //insert在指定位置插入一个元素
    my_iterator insert(my_iterator pos,const my_valType& val) {return copy_insert(pos,val);}
    //移动插入
    my_iterator insert(my_iterator pos,my_valType&& val) {return emplace(pos,mystl::move(val));}

    //在指定位置插入n个值为val的节点
    my_iterator insert(const_my_iterator pos, size_type n, const my_valType& val) {return fill_insert(pos,n,val);}

    //在指定位置插入range中的元素
    template<class iter>
    my_iterator insert(const_my_iterator pos,iter first,iter last){
        auto ret=pos;++ret;
        for(;first!=last;++first)
            copy_insert(pos,*first);
        return ret;
    }
    //支持 initializer list插入
    my_iterator insert(const_my_iterator pos,std::initializer_list<my_valType> ilist){return insert(pos,ilist.begin(),ilist.end());}

    //删除pos处的元素
    my_iterator erase(my_iterator pos){
        //检查容器大小是否为0，是的话报告错误
        if(_size==0)
            std::__throw_out_of_range("you try to delete an element while the list is empty");

        my_iterator ret=++pos;
        pos--;
        pos.node->prev->next=pos.node->next;
        pos.node->next->prev=pos.node->prev;
        _size--;
        delete pos.node;
        return ret;
    }
    //删除range中的元素
    my_iterator erase(my_iterator first, my_iterator last){
        //这里不需要检查是否删除超过容器大小的数量，因为容器是环形结构，两个迭代器始终在list范围中
        //循环删除range中的每个节点
        while(first!=last)
            erase(first++);
        return last;
    }

    //clear()删除容器中的所有节点
    void clear(){
        //cur一开始定位在容器的第一个元素，循环遍历释放所有节点
        node_ptr cur=endBlankNode->next;
        node_ptr next=cur->next;
        while(cur!=endBlankNode){
            delete cur;
            cur=next;
            next=next->next;
        }
        //初始化endBlanknode和_size的状态
        default_init();
    }

    //tansfer将first~last中的所有元素移动到pos之前
    void transfer(my_iterator pos, my_iterator first, my_iterator last){
        //先修改三个节点first.prev  pos.prev  last.prev 的next节点  先保留prev以便后续更改（也可以先prev再next）
        last.node->prev->next=pos.node;
        first.node->prev->next=last.node;
        pos.node->prev->next=first.node;

        //修改三个节点first  pos  last的prev节点
        node_ptr temp=first.node->prev;
        first.node->prev=pos.node->prev;
        pos.node->prev=last.node->prev;
        last.node->prev=temp;
    }

    //把整个other list移动到pos处    2 params
    void splice(const_my_iterator pos, mylist<my_valType>& other) noexcept {
        if(!other.empty())
            transfer(pos,other.begin(),other.end());
        _size+=other.size();
        other._size=0;
    }

    //把迭代器i所指的节点移动到pos处     3 params
    void splice(const_my_iterator pos,mylist<my_valType>& other, const_my_iterator i) noexcept {
        my_iterator j=i;
        ++j;
        //对pos处节点自己进行transfer没有意义，  把j=pos，等于把pos前一个节点放到pos前一个位置，一样没有意义
        if(pos==i||pos==j)
            return;
        transfer(pos,i,j);
        _size++;
        other._size--;
    }

    //把迭代器first～last范围中的节点移动到pos处   4 params
    void splice(const_my_iterator pos, mylist<my_valType>& other, const_my_iterator first, const_my_iterator last) noexcept {
        size_type len=distance(first,last);
        transfer(pos,first,last);
        _size+=len;
        other._size-=len;
    }

    //remove(val)删除值为val的节点
    void remove(const my_valType& val){
        my_iterator cur=begin();
        my_iterator next=cur;
        ++next;
        while(cur.node!=endBlankNode){
            if(*cur==val){
                erase(cur);
            }
            cur=next;
            ++next;
        }
    }

    //remove_if()删除满足条件的节点
    template<class predicate>
    void remove_if(predicate p){
        my_iterator cur=begin();
        my_iterator next=cur;
        ++next;
        while(cur.node!=endBlankNode){
            if(p(*cur)){
                erase(cur);
            }
            cur=next;
            ++next;
        }
    }

    //删除容器中相邻的重复元素，只保留一个
    void unique(){
        my_iterator cur=begin();
        my_iterator next=cur;
        ++next;
        while(cur.node!=endBlankNode){
            if(*cur==*next){
                erase(cur);
            }
            cur=next;
            ++next;
        }
    }

    //合并两个事先已排好序的 list 容器，并且合并之后的 list 容器依然是有序的
    void merge(mylist<my_valType>& other){
        my_iterator first1=begin();
        my_iterator last1=end();
        my_iterator first2=other.begin();
        my_iterator last2=other.end();

        while(first1 != last1 && first2 != last2)
            if(*first2<*first1){
                my_iterator next=first2;
                ++next;
                transfer(first1,first2,next);
                first2=next;
            }
            else
                ++first1;
        //first2比first1更长，把other尾部未处理的节点接到last1处
        if(first2!=last2)
            transfer(last1,first2,last2);
        _size+=other._size;
        other._size=0;
    }
    //自定义比较标准
    template<class Compare>
    void merge(mylist<my_valType>& other, Compare comp){
        my_iterator first1=begin();
        my_iterator last1=end();
        my_iterator first2=other.begin();
        my_iterator last2=other.end();

        while(first1 != last1 && first2 != last2)
            if(comp(*first2,*first1)){
                my_iterator next=first2;
                ++next;
                transfer(first1,first2,next);
                first2=next;
            }
            else
                ++first1;
        //first2比first1更长，把other尾部未处理的节点接到last1处
        if(first2!=last2)
            transfer(last1,first2,last2);
        _size+=other._size;
        other._size=0;
    }

    //通过更改容器中元素的位置，将它们进行排序   O(n)=nlogn
    void sort(){
        //容器中为空或者只有一个元素时不需要操作
        if(endBlankNode->next==endBlankNode||endBlankNode->next->next==endBlankNode)
            return;
        //创建中介存储，counter[i]中能够存储2^i个元素，fill=counter存储的最高位+1，fill初始为0,fill<logn
        mylist<my_valType> carry;
        mylist<my_valType> counter[64];
        int fill=0;
        //外层循环O(n)   内层循环O(logn)     复杂度O(nlogn)
        while(!empty()){
            //每次先从list中取一个元素    O(n)
            carry.splice(carry.begin(),*this,begin());
            int i=0;
            //类似二进制加法，当前位counter[i]要么存2^i个元素，要么为空。
            //如果counter[i]为空直接放进counter[i]，counter[i]不为空，说明要进位，则把counter[i]放
            //进carry（merge按顺序放入），让i加一，开启下一轮循环，判断能否放进counter[i+1]    O(logn)
            while(i<fill&&!counter[i].empty()){
                carry.merge(counter[i]);
                ++i;
            }
            //把carry携带的元素放进当前空位counter[i]
            carry.swap(counter[i]);
            if(i==fill)
                ++fill;
        }
        //每个counter位中存储的元素都是排序好的，再把他们merge到一起即可 merge复杂度O(n)  fill<logn  所以复杂度是O(nlogn)
        for(int i=1;i<fill;++i)
            counter[i].merge(counter[i-1]);
        swap(counter[fill-1]);
    }
    //自定义比较标准
    template<class Compare>
    void sort(Compare comp){
        //容器中为空或者只有一个元素时不需要操作
        if(endBlankNode->next==endBlankNode||endBlankNode->next->next==endBlankNode)
            return;
        //创建中介存储，counter[i]中能够存储2^i个元素，fill=counter存储的最高位+1，fill初始为0,fill<logn
        mylist<my_valType> carry;
        mylist<my_valType> counter[64];
        int fill=0;
        //外层循环O(n)   内层循环O(logn)     复杂度O(nlogn)
        while(!empty()){
            carry.splice(carry.begin(),*this,begin());
            int i=0;
            while(i<fill&&!counter[i].empty()){
                carry.merge(counter[i],comp);
                ++i;
            }
            carry.swap(counter[i]);
            if(i==fill)
                ++fill;
        }
        for(int i=1;i<fill;++i)
            counter[i].merge(counter[i-1],comp);
        swap(counter[fill-1]);
    }

    //反转容器中元素的顺序
    void reverse(){
        //容器中为空或者只有一个元素时不需要操作
        if(endBlankNode->next==endBlankNode||endBlankNode->next->next==endBlankNode)
            return;

        my_iterator cur=begin();
        ++cur;
        while(cur!=end()){
            my_iterator prev=cur;
            ++cur;
            transfer(begin(),prev,cur);
        }
    }

};

#endif
