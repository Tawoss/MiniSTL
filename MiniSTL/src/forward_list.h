#ifndef FORWARD_LIST_H
#define FORWARD_LIST_H

#include <iostream>
#include "iterator.h"
#include "algobase.h"
#include "utils.h"

template<class T>
struct myflist_node{
    typedef T                           valType;
    typedef myflist_node<valType>*      pointer;
    pointer   next;
    valType   data;

    //默认构造
    myflist_node()=default;
    //拷贝构造
    myflist_node(const myflist_node& other):next(other.next),data(other.data){}
    //传入参数拷贝构造
    myflist_node(const T& val): data(val) {}
    //移动构造
    myflist_node(myflist_node&& other):next(mystl::move(other.next)),data(mystl::move(other.data)){}
    //传入参数移动构造
    myflist_node(T&& val):data(mystl::move(val)) {}
};
template<class T>
struct myflist_iterator:public mystl::iterator<mystl::forward_iterator_tag, T>{
    //stl标准要求的内嵌型别定义
    //typedef mystl::forward_iterator_tag             iteartor_category;
    typedef T                                       value_type;
    typedef __PTRDIFF_TYPE__                        difference_type;
    typedef value_type*                             pointer;
    typedef value_type&                             reference;

    typedef myflist_node<value_type>*                node_ptr;
    typedef myflist_iterator<T>                      self;

    node_ptr  node;

    //默认构造
    myflist_iterator():node(){}

    //传入参数为一个list_node
    myflist_iterator(node_ptr x): node(x){}

    //拷贝构造
    myflist_iterator(const myflist_iterator& other): node(other.node) {}


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

    //重载比较运算符
    bool operator==(const self& other) const {return node==other.node;}
    bool operator!=(const self& other) const {return node!=other.node;}
};

template<class T>
struct myflist{
    //mylist类的内嵌型别定义
    typedef T                                               my_valType;
    typedef myflist_node<my_valType>                        my_node;
    typedef my_node*                                        node_ptr;
    typedef my_valType&                                     my_reference;
    typedef myflist_iterator<my_valType>                    my_iterator;
    typedef const myflist_iterator<my_valType>              const_my_iterator;
    typedef std::size_t                                     size_type;
    typedef __PTRDIFF_TYPE__                                difference_type;

    node_ptr _head;

    //默认构造函数
    myflist(){
        default_init();
    }

    //初始化容器大小为n，值为默认值
    myflist(size_type n){
        default_init();
        fill_init(n,my_valType());
    }

    //range构造函数
    template<class iter>
    myflist(iter first,iter last){
        default_init();
        range_init(first,last);
    }

    //初始化容器大小为n，值为val
    myflist(size_type n, const my_valType& val){
        default_init();
        fill_init(n,val);
    }

    //拷贝构造
    myflist(const myflist& other){
        default_init();
        range_init(other.cbegin(),other.cend());
    }

    //移动构造
    myflist(myflist&& other)
    :_head(other._head)
    {
        other._head=nullptr;
    }

    //支持initializer_list构造
    myflist(std::initializer_list<my_valType> ilist){
        default_init();
        range_init(ilist.begin(),ilist.end());
    }

    //重载operator=  拷贝赋值
    myflist& operator=(myflist& other){
        if(this==&other)
            return *this;
        _head=other._head;
        return *this;
    }
    //重载operator=  移动赋值
    myflist& operator=(myflist&& other){
        if(this==&other)
            return *this;
        _head=other._head;
        other._head=nullptr;
        return *this;
    }

    //默认初始化容器
    void default_init(){
        _head=create_node(my_valType());
        _head->next=nullptr;
    }

    //初始化容器，大小为n,值为val
    void fill_init(size_type n,const my_valType& val){
        for(;n>0;--n)
            push_front(val);
    }

    //以两个迭代器range来初始化容器
    template<class iter>
    void range_init(iter first, iter last){
        my_iterator prev=before_begin();
        while(first!=last){
            m_insert_after(prev,*first);
            ++prev;
            ++first;
        }
    }

    //创建node函数,自动转换参数为list的元素类型
    template<typename... Args>
    node_ptr create_node(Args&&... args){
        node_ptr tem=new myflist_node<my_valType>();
        new (&tem->data) my_valType(std::forward<Args>(args)...);
        return tem;
    }

    //移动插入元素到pos之后
    template<typename... Args>
    my_iterator m_insert_after(const_my_iterator pos, Args... args){
        node_ptr temp=create_node(std::forward<Args>(args)...);
        temp->next=pos.node->next;
        pos.node->next=temp;
        return temp;
    }

    //返回一个前向迭代器，其指向容器中第一个元素之前的位置
    my_iterator before_begin(){
        return my_iterator(_head);
    }
    //和 before_begin() 功能相同，只不过在其基础上，增加了 const 属性，不能用于修改元素
    const_my_iterator cbefore_begin(){
        return const_my_iterator(_head);
    }

    //返回一个前向迭代器，其指向容器中第一个元素的位置
    my_iterator begin(){
        return my_iterator(_head->next);
    }
    //和 begin() 功能相同，只不过在其基础上，增加了 const 属性，不能用于修改元素
    const_my_iterator cbegin() const {
        return const_my_iterator(_head->next);
    }

    //返回一个前向迭代器，其指向容器中最后一个元素之后的位置
    my_iterator end(){
        return my_iterator(nullptr);
    }
    //和 end() 功能相同，只不过在其基础上，增加了 const 属性，不能用于修改元素
    const_my_iterator cend() const {
        return const_my_iterator(nullptr);
    }

    //判断容器是否为空
    bool empty(){
        return _head->next==nullptr;
    }

    //返回容器能够存储的理论最大元素数量
    size_type max_size()const {
        return size_type(-1)/sizeof(my_valType);
    }

    //返回第一个元素的引用
    my_reference front(){
        return *begin();
    }

    //assign()将容器中的元素替换为range中的元素
    template<class iter>
    void assign(iter first, iter last){
        clear();
        range_init(first,last);
    }

     //在容器头部插入一个元素
    void push_front(const my_valType& val){
        node_ptr temp=create_node(val);

        temp->next=_head->next;
        _head->next=temp;
    }

    //在容器头部生成一个元素。该函数和 push_front() 的功能相同，但效率更高
    template<class... Args>
    void emplace_front(Args&&... args){
        m_insert_after(cbefore_begin(),std::forward<Args>(args)...);
    }

    //删除容器头部的一个元素
    void pop_front(){
        erase_after(_head);
    }

    //在指定位置之后插入一个新元素，并返回一个指向新元素的迭代器。和 insert_after() 的功能相同，但效率更高
    template<class... Args>
    my_iterator emplace_after(const_my_iterator pos, Args&&... args){
        my_iterator ret=m_insert_after(pos,std::forward<Args>(args)...);
        return ret;
    }

    //在指定位置之后插入一个新元素，并返回一个指向新元素的迭代器
    my_iterator insert_after(const_my_iterator pos,const my_valType& val){
        my_iterator ret = m_insert_after(pos,val);
        return ret;
    }
    //移动插入
    my_iterator insert_after(const_my_iterator pos,my_valType&& val){
        my_iterator ret = m_insert_after(pos,mystl::move(val));
        return ret;
    }
    //在指定位置之后插入n个值为val的节点
    my_iterator insert_after(const_my_iterator pos, size_type n, const my_valType& val){
        my_iterator ret;
        for(;n>0;--n){
            ret=m_insert_after(pos,val);
        }
        return ret;
    }
    //在指定位置插入range中的元素
    template<class iter>
    my_iterator insert_after(const_my_iterator pos,iter first,iter last){
        my_iterator ppos=(my_iterator)pos;
        while(first!=last){
            ppos=m_insert_after(ppos,*first);
            ++first;
        }
        return ppos;
    }
    //支持 initializer list插入
    my_iterator insert_after(const_my_iterator pos,std::initializer_list<my_valType> ilist){
        return insert_after(pos,ilist.begin(),ilist.end());
    }

    //删除容器中某个指定位置或区域内的所有元素
    //删除pos处后一个位置的元素
    my_iterator erase_after(const_my_iterator pos){
        my_iterator ret=(my_iterator)pos;
        node_ptr tem=ret.node->next;
        ret.node->next=ret.node->next->next;
        delete tem;
        ++ret;
        return ret;
    }
    //删除(pos,last)中的元素
    my_iterator erase_after(const_my_iterator pos, const_my_iterator last){
        difference_type len=distance(pos,last);
        for(;len>1;--len){
            erase_after(pos);
        }
        my_iterator ret=++(my_iterator)pos;
        return ret;
    }

    //swap交换两个容器   类型必须一样，size可以不同
    void swap(myflist<my_valType>& other){
        mystl::swap(this->_head, other._head);
    }

    //resize()改变实际元素的个数,超过原来容器的部分用val补齐
    void resize(size_type n, const my_valType& val){
        my_iterator start=before_begin();
        my_iterator next=begin();
        size_type len=0;
        for(;next!=end()&&len<n;++start,++next,++len);
        if(len==n)
            erase_after(start,end());
        else
            insert_after(start,n-len,val);

    }

    //clear()删除容器中的所有节点
    void clear(){
        //cur一开始定位在容器的第一个元素，循环遍历释放所有节点
        node_ptr cur=_head->next;
        node_ptr next=cur->next;
        while(cur!=nullptr){
            delete cur;
            cur=next;
            if(next!=nullptr)
                next=next->next;
        }
        //初始化endBlanknode和_size的状态
        default_init();
    }

    //tansfer将first~last中的所有元素移动到pos之后
    void transfer_after(my_iterator pos, my_iterator first, my_iterator last){
        //找到要转移的第一个元素
        my_iterator start=first;
        ++start;
        //找到要转移的最后一个元素
        my_iterator end=first;
        my_iterator next=end;
        ++next;
        while(next!=last){
            ++end;
            ++next;
        }
        //剪切到新位置
        first.node->next=last.node;
        end.node->next=pos.node->next;
        pos.node->next=start.node;
    }

    //把整个other list移动到pos处    2 params
    void splice_after(const_my_iterator pos, myflist<my_valType>& other){
        transfer_after(pos, other.before_begin(),other.end());
    }
    //把迭代器i所指的节点移动到pos处     3 params
    void splice_after(const_my_iterator pos, myflist<my_valType>& other, const_my_iterator i){
        //找到要转移的最后一个元素
        my_iterator prev=other.before_begin();
        my_iterator next=other.begin();
        while(next!=i){
            ++prev;
            ++next;
        }
        ++next;
        transfer_after(pos,prev,next);
    }
    //把迭代器first～last范围中的节点移动到pos处   4 params open range
    void splice_after(const_my_iterator pos, myflist<my_valType>& other, const_my_iterator first, const_my_iterator last){
        transfer_after(pos,first,last);
    }

    //remove(val)删除值为val的节点
    void remove(const my_valType& val){
        my_iterator cur=begin();
        my_iterator prev=before_begin();
        while(cur.node!=end().node){
            if(*cur==val){
                cur=erase_after(prev);
            }
            else{
                ++cur;
                ++prev;
            }
        }
    }

    //remove_if()删除满足条件的节点
    template<class predicate>
    void remove_if(predicate p){
        my_iterator cur=begin();
        my_iterator prev=before_begin();
        while(cur.node!=end().node){
            if(p(*cur)){
                cur=erase_after(prev);
            }
            else{
                ++cur;
                ++prev;
            }
        }
    }

    //删除容器中相邻的重复元素，只保留一个
    void unique(){
        my_iterator prev=begin();
        my_iterator cur=prev;
        ++cur;
        while(cur.node!=end().node){
            if(*cur==*prev){
                cur=erase_after(prev);
            }
            else{
                ++cur;
                ++prev;
            }
        }
    }

    //合并两个事先已排好序的 forward_list 容器，并且合并之后的 forward_list 容器依然是有序的
    void merge(myflist<my_valType>& other){
        my_iterator prev1=before_begin();
        my_iterator first1=begin();
        my_iterator last1=end();
        my_iterator prev2=other.before_begin();
        my_iterator first2=other.begin();
        my_iterator last2=other.end();

        while(first1 != last1 && first2 != last2)
            if(*first2<*first1){
                my_iterator next=first2;
                ++next;
                transfer_after(prev1,prev2,next);
                ++prev1;
                first2=next;
            }
            else{
                ++first1;
                ++prev1;
            }
        //first2比first1更长，把other尾部未处理的节点接到last1处
        if(first2!=last2)
            transfer_after(prev1,prev2,last2);
    }

    //自定义比较标准
    template<class Compare>
    void merge(myflist<my_valType>& other,Compare comp){
        my_iterator prev1=before_begin();
        my_iterator first1=begin();
        my_iterator last1=end();
        my_iterator prev2=other.before_begin();
        my_iterator first2=other.begin();
        my_iterator last2=other.end();

        while(first1 != last1 && first2 != last2)
            if(comp(*first2,*first1)){
                my_iterator next=first2;
                ++next;
                transfer_after(prev1,prev2,next);
                ++prev1;
                first2=next;
            }
            else{
                ++first1;
                ++prev1;
            }
        //first2比first1更长，把other尾部未处理的节点接到last1处
        if(first2!=last2)
            transfer_after(prev1,prev2,last2);
    }

    //通过更改容器中元素的位置，将它们进行排序
    void sort(){
        //容器中为空或者只有一个元素时不需要操作
        if(_head->next==nullptr||_head->next->next==nullptr) return;

        //创建中介存储，counter[i]中能够存储2^i个元素，fill=counter存储的最高位+1，fill初始为0,fill<logn
        myflist<my_valType> carry;
        myflist<my_valType> counter[64];
        int fill=0;
        //外层循环O(n)   内层循环O(logn)     复杂度O(nlogn)
        while(!empty()){
            //每次先从list中取一个元素    O(n)
            carry.splice_after(carry.before_begin(), *this, begin());
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
        if(_head->next==nullptr||_head->next->next==nullptr) return;

        myflist<my_valType> carry;
        myflist<my_valType> counter[64];

        int fill=0;
        while(!empty()){
            carry.splice_after(carry.before_begin(), *this, begin());
            int i=0;
            while(i<fill&&!counter[i].empty()){
                carry.merge(counter[i],comp);
                ++i;
            }
            //把carry携带的元素放进当前空位counter[i]
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
        if(_head->next==nullptr||_head->next->next==nullptr) return;

        //定位第一个元素
        my_iterator first=before_begin();
        ++first;
        my_iterator prev=first;
        my_iterator next=first;
        ++next;++next;
        while(next!=end()){
            //把prev-next之间的元素放到list首部
            transfer_after(before_begin(),prev,next);
            ++next;
        }
        transfer_after(before_begin(),prev,next);
    }

};


#endif
