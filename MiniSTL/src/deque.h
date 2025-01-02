#ifndef MY_DEQUE_H
#define MY_DEQUE_H

#include <iostream>
#include "iterator.h"
#include "algobase.h"
#include "utils.h"

#define deque_map_init_size 8
//deque buffer size设置为8
inline std::size_t __deque_buf_size(std::size_t sz){
    return sz < 512 ? size_t(512 / sz) : size_t(1);
}

template<class T>
struct mydeque_iterator:public mystl::iterator<mystl::random_access_iterator_tag, T>{
    //stl标准要求的内嵌型别定义
    typedef mystl::random_access_iterator_tag           iteartor_category;
    typedef T                                           value_type;
    typedef __PTRDIFF_TYPE__                            difference_type;
    typedef value_type*                                 pointer;
    typedef value_type&                                 reference;
    typedef value_type*                                 value_pointer;
    typedef value_type**                                map_pointer;
    typedef mydeque_iterator<value_type>                iterator;
    typedef mydeque_iterator                            self;
    typedef std::size_t                                 size_type;

    // 迭代器所含成员数据
    value_pointer cur;    // 指向所在缓冲区的当前元素
    value_pointer first;  // 指向所在缓冲区的头部
    value_pointer last;   // 指向所在缓冲区的尾部
    map_pointer   node;   // 缓冲区所在节点

    //返回缓冲区大小
    static size_type buffer_size() noexcept { return __deque_buf_size(sizeof(value_type)); }

    // 默认构造函数
    mydeque_iterator() noexcept
        :cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}
    //拷贝构造函数
    mydeque_iterator(const iterator& rhs)
        :cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node){}
    //移动构造函数
    mydeque_iterator(iterator&& rhs) noexcept
        :cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node)
    {
        rhs.cur = nullptr;
        rhs.first = nullptr;
        rhs.last = nullptr;
        rhs.node = nullptr;
    }
    //以缓冲区构造
    mydeque_iterator(value_pointer v, map_pointer n)
        :cur(v), first(*n), last(*n + buffer_size()), node(n) {}
    // 转到另一个缓冲区
    void set_node(map_pointer new_node)
    {
        node = new_node;
        first = *new_node;
        last = first + buffer_size();
    }

    //重载引用和解引用操作符
    reference operator*() const { return *cur; }
    pointer  operator->() const { return  cur; }

    //重载运算符
    //-iterator
    difference_type operator-(const self& x) const{
        return static_cast<difference_type>(buffer_size()) * (node - x.node) + (cur - first) - (x.cur - x.first);
    }
    //前置++
    self& operator++(){
        ++cur;
        // 如果到达缓冲区的尾
        if (cur == last){
            set_node(node + 1);
            cur = first;
        }
        return *this;
    }
    //后置++
    self operator++(int){
        self tmp = *this;
        ++*this;
        return tmp;
    }
    //前置--
    self& operator--(){
        // 如果到达缓冲区的头
        if (cur == first){
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }
    //后置--
    self operator--(int){
        self tmp = *this;
        --*this;
        return tmp;
    }
    //+=
    self& operator+=(difference_type n){
        const difference_type offset=n+(cur-first);
        //仍在当前缓冲区
        if(offset>=0 && offset<difference_type(buffer_size()))
            cur+=n;
        //超出当前缓冲区
        else{
            const difference_type node_offset = offset>0 ? offset/difference_type(buffer_size())
            :-difference_type((-offset - 1)/ buffer_size())-1;
            //重新定位缓冲区
            set_node(node+node_offset);
            //更新cur
            cur=first+(offset-node_offset*difference_type(buffer_size()));
        }
        return *this;
    }
    //-=
    self& operator-=(difference_type n){
        return *this+=-n;
    }
    //+n
    self operator+(difference_type n) const {
        self tmp = *this;
        return tmp += n;
    }
    //-n
    self operator-(difference_type n) const{
        self tmp = *this;
        return tmp -= n;
    }

    //重载比较操作符
    bool operator==(const self& rhs) const { return cur == rhs.cur; }
    bool operator< (const self& rhs) const { return node == rhs.node ? (cur < rhs.cur) : (node < rhs.node); }
    bool operator!=(const self& rhs) const { return !(*this == rhs); }
    bool operator> (const self& rhs) const { return rhs < *this; }
    bool operator<=(const self& rhs) const { return !(rhs < *this); }
    bool operator>=(const self& rhs) const { return !(*this < rhs); }
    //重载[]操作符
    reference operator[](difference_type n) const { return *(*this + n); }
    //重载=操作符  拷贝
    self& operator=(const iterator& rhs){
        if (this != &rhs){
            cur = rhs.cur;
            first = rhs.first;
            last = rhs.last;
            node = rhs.node;
        }
        return *this;
    }

    //重载=操作符  移动
    self& operator=(iterator&& rhs){
        if (this != &rhs){
            cur = rhs.cur;
            first = rhs.first;
            last = rhs.last;
            node = rhs.node;
            rhs.cur = nullptr;
            rhs.first = nullptr;
            rhs.last = nullptr;
            rhs.node = nullptr;
        }
        return *this;
    }

};

template<class T>
struct mydeque{
    //内嵌型别
    typedef T                                                   my_valType;
    typedef my_valType*                                         my_pointer;
    typedef const my_valType*                                   const_my_pointer;
    typedef my_valType&                                         my_reference;
    typedef const my_valType&                                   const_my_reference;
    typedef my_valType**                                        my_map_pointer;
    typedef const my_valType**                                  const_my_map_pointer;
    typedef std::size_t                                         size_type;
    typedef __PTRDIFF_TYPE__                                    difference_type;
    //迭代器
    typedef mydeque_iterator<my_valType>                        my_iterator;
    typedef const mydeque_iterator<my_valType>                  const_my_iterator;
    typedef mystl::reverse_iterator<my_iterator>                my_reverse_iterator;
    typedef const mystl::reverse_iterator<my_iterator>          const_my_reverse_iterator;

    // 用以下四个数据来表现一个 deque
    my_iterator       begin_;     // 指向第一个节点
    my_iterator       end_;       // 指向最后一个节点
    my_map_pointer    map_;       // 指向一块 map，map 中的每个元素都是一个指针，指向一个缓冲区
    size_type         map_size_;  // map 内指针的数目

    //返回缓冲区大小
    static size_type buffer_size() noexcept { return __deque_buf_size(sizeof(my_valType)); }

    //默认构造函数
    mydeque(){
        fill_init(0,my_valType());
    }
    //初始化大小为n的默认容器
    mydeque(size_type n) {
        fill_init(n,my_valType());
    }
    //初始化大小为n的值为val的容器
    mydeque(size_type n, const my_valType& val) {
        fill_init(n,my_valType(val));
    }
    //以两个迭代器range构造
    template<class iter>
    mydeque(iter first, iter last){
        range_init(first,last);
    }
    //拷贝构造
    mydeque(const mydeque& other){
        range_init(other.cbegin(),other.cend());
    }
    //initializer_list构造
    mydeque(std::initializer_list<my_valType> ilist){
        range_init(ilist.begin(),ilist.end());
    }
    //移动构造
    mydeque(mydeque&& other)
    :begin_(mystl::move(other.begin_)),
    end_(mystl::move(other.end_)),
    map_(other.map_),
    map_size_(other.map_size_)
    {
        other.map_=nullptr;
        other.map_size_=0;
    }

    //重载operator=  拷贝赋值
    mydeque& operator=(mydeque& other){
        if(this==&other)
            return *this;
        if(size()>=other.size()){
            erase(mystl::copy(other.begin(),other.end(),begin()),end());
        }
        else{
            my_iterator first_end=other.begin()+size();
            mystl::copy(other.begin(),first_end,begin());
            insert(end(),first_end,other.end());
        }
        return *this;
    }
    //重载operator=  移动赋值
    mydeque& operator=(mydeque&& other){
        clear();
        begin_=mystl::move(other.begin_);
        end_=mystl::move(other.end_);
        map_=other.map_;
        map_size_=other.map_size_;
        other.map_=nullptr;
        other.map_size_=0;
        return *this;
    }

    //初始化
    //初始化容器，大小为n,值为val
    void fill_init(size_type n,const my_valType& val){
        map_init(n);
        if(n==0) return;
        for(my_map_pointer cur=begin_.node;cur<end_.node;++cur){
            mystl::fill_range(*cur,*cur+buffer_size(),val);
        }
        mystl::fill_range(end_.first,end_.cur,val);
    }

    //以两个迭代器range来初始化容器
    template<class iter>
    void range_init(iter first, iter last){
        size_type elems_num=mystl::distance(first,last);
        map_init(elems_num);
        mystl::copy(first,last,begin_.cur);
    }

    //初始化map存储n个元素
    void map_init(size_type n){
        const size_type need_buffer=n/buffer_size()+1;
        map_size_=mystl::max(size_type(deque_map_init_size), need_buffer+2);
        //创建map，保证异常安全
        try{
            map_ = create_map(map_size_);
        }
        catch (...){
            map_ = nullptr;
            map_size_ = 0;
            throw;
        }
        // 让 nstart 和 nfinish 都指向 map_ 最中央的区域，方便向头尾扩充
        my_map_pointer nstart = map_ + (map_size_ - need_buffer) / 2;
        my_map_pointer nfinish = nstart + need_buffer - 1;

        try{
            create_buffer(nstart, nfinish);
        }
        catch (...){
            if(map_!=nullptr)
                 delete(map_);
            map_ = nullptr;
            map_size_ = 0;
            throw;
        }
        begin_.set_node(nstart);
        end_.set_node(nfinish);
        begin_.cur = begin_.first;
        end_.cur = end_.first + (n%buffer_size());
    }

    //迭代器相关操作
    //返回指向容器中第一个元素的迭代器      const 和 non-const
    my_iterator begin() noexcept { return begin_; }
    const_my_iterator cbegin() const noexcept { return const_my_iterator(begin_); }
    //返回指向最后一个元素的反向迭代器      const 和 non-const
    my_reverse_iterator rbegin() noexcept { return my_reverse_iterator(end()); }
    const_my_reverse_iterator crbegin() const noexcept { return const_my_reverse_iterator(end()); }

    //返回指向容器最后一个元素所在位置后一个位置的迭代器        const 和 non-const
    my_iterator end() noexcept { return end_; }
    const_my_iterator cend() const noexcept { return const_my_iterator(end_); }
    //返回指向第一个元素所在位置前一个位置的反向双向迭代器       const 和 non-const
    my_reverse_iterator rend() noexcept { return my_reverse_iterator(begin()); }
    const_my_reverse_iterator crend() const noexcept { return const_my_reverse_iterator(begin()); }

    //插入
    //emplace_front
    template<class ...Args>
    void emplace_front(Args&&... args){
        if(begin_.cur!=begin_.first){
            new (begin_.cur-1) my_valType(mystl::forward<Args>(args)...);
            --begin_.cur;
        }
        else{
            require_capacity_front(1);
            --begin_;
            new (begin_.cur) my_valType(mystl::forward<Args>(args)...);
        }
    }
    //emplace_back
    template<class ...Args>
    void emplace_back(Args&&... args){
        if(end_.cur!=end_.last-1){
            new (end_.cur) my_valType(mystl::forward<Args>(args)...);
            ++end_.cur;
        }
        else{
            require_capacity_back(1);
            new (end_.cur) my_valType(mystl::forward<Args>(args)...);
            ++end_;
        }
    }
    //emplace
    template<class ...Args>
    my_iterator emplace(my_iterator pos, Args&&... args){
        if (pos.cur == begin_.cur){
            emplace_front(mystl::forward<Args>(args)...);
            return begin_;
        }
        else if (pos.cur == end_.cur){
            emplace_back(mystl::forward<Args>(args)...);
            return end_ - 1;
        }
        return insert_at(pos, mystl::forward<Args>(args)...);
    }

    //push_back   在容器尾部插入一个元素
    void push_back(const my_valType& val){
        if(end_.cur!=end_.last-1){
            new (end_.cur) my_valType(val);
            ++end_.cur;
        }
        else{
            require_capacity_back(1);
            new (end_.cur) my_valType(val);
            ++end_;
        }
    }
    //push_front   在容器首部插入一个元素
    void push_front(const my_valType& val){
        if(begin_.cur!=begin_.first){
            new (begin_.cur-1) my_valType(val);
            --begin_.cur;
        }
        else{
            require_capacity_front(1);
            --begin_;
            new (begin_.cur) my_valType(val);
        }
    }
    //在指定位置插入一个值为val的元素
    my_iterator insert(const_my_iterator pos,const my_valType& val){
        return insert_at(pos,val);
    }
    //移动操作
    my_iterator insert(const_my_iterator pos, my_valType&& val){
        return insert_at(pos,mystl::move(val));
    }
    //在指定位置插入n个值为val的元素
    my_iterator insert(const_my_iterator pos, size_type n, const my_valType& val){
        my_iterator ret=pos;
        for(int i=0;i<n;++i)
            ret=insert_at(pos,val);
        return ret;
    }
    //在指定位置插入first-last范围中的元素
    template<class iter>
    my_iterator insert(const_my_iterator pos, iter first, iter last){
        auto ret=pos;++ret;
        --last;
        my_iterator cur=pos;
        while(first!=last){
            cur=insert(cur,*last);
            --last;
        }
        insert(cur,*last);
        return ret;
    }
    //在指定位置插入initializer_list
    my_iterator insert(const_my_iterator pos,std::initializer_list<my_valType> ilist){
        return insert(pos,ilist.begin(),ilist.end());
    }
    //在pos处插入元素
    template<class... Args>
    my_iterator insert_at(my_iterator pos, Args&&... args){
        const size_type elems_before = pos - begin_;
        if(elems_before==0){
            emplace_front(mystl::forward<Args>(args)...);
            return begin_;
        }
        my_valType value_copy = my_valType(mystl::forward<Args>(args)...);

        if(elems_before < size()/2){
            //更靠近首部， 从首部开始， 移动元素更少
            emplace_front(front());
            my_iterator front=begin_;
            ++front;
            my_iterator start=front;
            ++start;
            pos=begin_+elems_before;
            my_iterator position=pos;
            ++position;
            mystl::copy(start,position,front);
        }
        else{
            //更靠近尾部， 从尾部开始， 移动元素更少
            emplace_back(back());
            my_iterator back=end_;
            --back;
            my_iterator end=back;
            --end;
            pos=begin_+elems_before;
            mystl::copy_backward(pos,end,back);
        }
        *pos=value_copy;
        return pos;
    }

    //assign
    //assign针对input_iterator
    template<class iter>
    void copy_assign(iter first, iter last, mystl::input_iterator_tag){
        my_iterator start=begin();
        my_iterator finish=end();
        for(;first!=last&&start!=finish;++first,++start)
            *start=*first;
        if(start!=finish)
            erase(start,finish);
        else
            insert(end(),first,last);
    }
    //assign针对random_access_iterator
    template<class iter>
    void copy_assign(iter first, iter last, mystl::random_access_iterator_tag){
        size_type len=last-first;
        if(len<size()){
            erase(mystl::copy(first,last,begin()),end());
        }
        else{
            iter first_end=first;
            mystl::advance(first_end,size());
            mystl::copy(first,first_end,begin());
            insert(end(),first_end,last);
        }
    }

    //修改容器元素为n个值为val的元素
    void assign(size_type n, const my_valType& val){
        if(n>size()){
            mystl::fill(begin(),end(),val);
            insert(end(),n-size(),val);
        }
        else{
            erase(begin()+n,end());
            mystl::fill(begin(),end(),val);
        }
    }
    //修改容器元素为first-last所包含的元素
    template<class iter>
    void assign(iter first, iter last){
        copy_assign(first,last,mystl::iterator_category(first));
    }
    //修改容器元素为initializer_list所包含的元素
    void assign(std::initializer_list<my_valType> ilist){
        assign(ilist.begin(),ilist.end());
    }

    //swap
    void swap(mydeque& other){
        if(this==&other)
            return;
        mystl::swap(begin_,other.begin_);
        mystl::swap(end_,other.end_);
        mystl::swap(map_,other.map_);
        mystl::swap(map_size_,other.map_size_);
    }

    //删除
    //pop_back移除容器尾部的元素  进行异常检查
    void pop_back(){
        if(size()==0)
            std::__throw_out_of_range("the container is empty cannot pop_back");
        if(end_.cur!=end_.first){
            --end_.cur;
            //delete end_.cur;
            end_.cur->~my_valType();
        }
        else{
            --end_;
            //delete end_.cur;
            end_.cur->~my_valType();
            //缓冲区为空，删除缓冲区
            delete_buffer(end_.node+1,end_.node+1);
        }
    }
    //pop_front在序列的头部添加一个元素
    void pop_front(){
        if(size()==0)
            std::__throw_out_of_range("the container is empty cannot pop_back");
        if(begin_.cur!=begin_.last-1){
            //delete(&*begin_.cur);
            begin_.cur->~my_valType();
            ++begin_.cur;
        }
        else{
            //delete(&*begin_.cur);
            begin_.cur->~my_valType();
            ++begin_;
            delete_buffer(begin_.node-1,begin_.node-1);
        }
    }
    //erase(pos)   根据离容器两端哪端更近来决定从哪一端开始移动元素
    my_iterator erase(const_my_iterator pos){
        my_iterator next=pos;
        ++next;
        const size_type elems_before=pos-begin_;
        if(elems_before < size()/2){
            //把[begin_, pos)前移一位
            mystl::copy_backward(begin_,pos,next);
            pop_front();
        }
        else{
            mystl::copy(next,end_,pos);
            pop_back();
        }
        return begin_+elems_before;
    }
    //erase(first, last) 采用copy整体删除[first,last)
    my_iterator erase(const_my_iterator first, const_my_iterator last){
        if(first==begin_&&last==end_){
            clear();
            return end_;
        }
        const size_type len=last-first;
        const size_type elems_before=first-begin_;
        //离首部更近,从首部开始移动元素
        if(elems_before<=(size()-len)/2){
            mystl::copy_backward(begin_,first,last);
            my_iterator new_begin=begin_+len;
            my_pointer cur;
            while(begin_.cur!=new_begin.cur){
                cur=begin_.cur;
                cur->~my_valType();
                ++begin_;
            }
            begin_=new_begin;
        }
        else{
            mystl::copy(last,end_,first);
            my_iterator new_end=end_-len;
            my_pointer cur;
            while(new_end.cur!=end_.cur){
                cur=end_.cur;
                cur->~my_valType();
                --end_;
            }
            end_=new_end;
        }
        return begin_+elems_before;
    }
    //clear清空容器
    void clear(){
        //释放头尾节点中间的缓冲区
        for(my_map_pointer cur=begin_.node+1;cur < end_.node;++cur){
            delete(*cur);
        }
        //有两个以上的缓冲区，释放了中间的之后，释放首尾缓冲区
        if(begin_.node!=end_.node){
            for(;begin_.cur!=begin_.last;++begin_.cur)
                begin_.cur->~my_valType();
            for(;end_.first!=end_.cur;++end_.first)
                end_.first->~my_valType();
            //mystl::destroy(begin_.cur,begin_.last);
            //mystl::destroy(end_.first,end_.cur);
        }
        else{
            my_pointer start1=begin_.cur;
            my_pointer end1=end_.cur;
            for(;begin_.cur!=end_.cur;++begin_.cur)
                begin_.cur->~my_valType();
            //mystl::destroy(begin_.cur,end_.cur);
        }
        shrink_to_fit();
        end_=begin_;
        map_init(0);
    }


    //容量相关操作
    //判断容器是否为空
    bool empty() noexcept { return begin_==end_; }
    //返回容器中元素的个数 operator-被重载过
    size_type size() noexcept { return end_-begin_; }
    //返回容器最大容纳数
    size_type max_size() noexcept { return size_type(-1);}
    //改变容器元素个数为n，容器原来没有的部分用val补齐
    void resize(size_type n, const my_valType& val){
        size_type len = size();
        if(n<len)
            erase(begin_+n,end_);
        else
            insert(end_,n-len,val);
    }
    //改变容器元素个数为n，容器原来没有的部分用默认值补齐
    void resize(size_type n) { resize(n,my_valType()); }
    //将内存减少到等于当前元素实际所使用的大小
    void shrink_to_fit() noexcept {
        //释放map_start —— begin_.node  之间的预备空间
        for(my_map_pointer map_start=map_;map_start<begin_.node;++map_start){
            if(*map_start==nullptr)
                continue;
            delete(*map_start);
            *map_start=nullptr;
        }
        //释放end_.node —— map_end  之间的预备空间
        my_map_pointer map_end=map_+map_size_;
        for(my_map_pointer end=end_.node+1;end<map_end;++end){
            if(*end==nullptr)
                continue;
            delete(*end);
            *end=nullptr;
        }

    };

    // 访问元素相关操作
    //返回首元素的引用
    my_reference front() { return *begin_; }
    const_my_reference front() const { return const_my_reference(*begin_); }
    //返回尾元素的引用
    my_reference back() { return *(end_-1); }
    const_my_reference back() const { return const_my_reference(*(end_-1)); }
    //重载[]操作符
    my_reference operator[](size_type n) { return begin_[n]; }
    const_my_reference operator[](size_type n) const { return const_my_reference(begin_[n]); }
    //at返回第n个元素, 进行边界检查
    my_reference at(size_type n) {
        if(n>=size())
            std::__throw_out_of_range_fmt("at(%zu) is out of range", n);

        return (*this)[n];
    }
    const_my_reference at(size_type n) const {
        if(n>=size())
            std::__throw_out_of_range_fmt("at(%zu) is out of range", n);

        return const_my_reference((*this)[n]);
    }

    //根据需要的空间大小n来定位到下一块map
    void require_capacity_back(size_type n){
        //尾部缓存区节点不够存储n个
        if(size_type(end_.last-end_.cur-1)<n){
            //计算需要多少缓冲区节点
            const size_type need_buffer = (n - (end_.last - end_.cur - 1)) / buffer_size() + 1;
            //map空间不够
            if(need_buffer>size_type((map_+map_size_)-end_.node-1)){
                reallocate_map_at_back(need_buffer);
                return;
            }
            //map的空间足够开辟新的缓冲区节点
            create_buffer(end_.node+1,end_.node+need_buffer);
        }
    }

    void require_capacity_front(size_type n){
        const size_type need_buffer = (n - (begin_.cur - begin_.first)) / buffer_size() + 1;
        if (need_buffer > static_cast<size_type>(begin_.node - map_)){
            reallocate_map_at_front(need_buffer);
            return;
        }
        create_buffer(begin_.node - need_buffer, begin_.node - 1);
    }
    //根据所需缓冲区节点数来创建缓冲区
    void reallocate_map_at_back(size_type need_buffer){
        const size_type new_map_size=mystl::max(map_size_<<1, map_size_ + need_buffer+deque_map_init_size);
        my_map_pointer new_map=create_map(new_map_size);
        const size_type old_buffer_num=end_.node-begin_.node+1;
        const size_type new_buffer_num=old_buffer_num+need_buffer;
        //将新的map中的指针指向原来的缓冲区节点，并开辟新建立的缓冲区节点
        //复制旧节点
        my_map_pointer begin=new_map+((new_map_size-new_buffer_num)/2);
        my_map_pointer mid=begin+old_buffer_num;
        my_map_pointer end=mid+need_buffer;
        for(my_map_pointer new_begin=begin, old_begin=begin_.node;new_begin!=mid;++new_begin,++old_begin)
            *new_begin=*old_begin;
        //开辟新建立的缓冲区  就是根据need_buffer建立的缓冲区
        create_buffer(mid,end-1);

        //更新deque成员数据
        //删除旧的map
        if(map_!=nullptr)
             delete(map_);
        map_=new_map;
        map_size_=new_map_size;
        begin_ = my_iterator(*begin + (begin_.cur - begin_.first), begin);
        end_ = my_iterator(*(mid - 1) + (end_.cur - end_.first), mid - 1);
    }

    void reallocate_map_at_front(size_type need_buffer){
        const size_type new_map_size=mystl::max(map_size_<<1, map_size_ + need_buffer+deque_map_init_size);
        my_map_pointer new_map=create_map(new_map_size);
        const size_type old_buffer_num=end_.node-begin_.node+1;
        const size_type new_buffer_num=old_buffer_num+need_buffer;
        //将新的map中的指针指向原来的缓冲区节点，并开辟新建立的缓冲区节点
        my_map_pointer begin=new_map+((new_map_size-new_buffer_num)/2);
        my_map_pointer mid=begin+need_buffer;
        my_map_pointer end=mid+old_buffer_num;
        //开辟新建立的缓冲区  就是根据need_buffer建立的缓冲区
        create_buffer(begin,mid-1);
        //复制旧节点
        for(my_map_pointer new_begin=mid, old_begin=begin_.node;new_begin!=end;++new_begin,++old_begin)
            *new_begin=*old_begin;
        //更新deque成员数据
        //删除旧的map
        if(map_!=nullptr)
             delete(map_);
        map_=new_map;
        map_size_=new_map_size;
        begin_ = my_iterator(*mid + (begin_.cur - begin_.first), mid);
        end_ = my_iterator(*(end - 1) + (end_.cur - end_.first), end - 1);
    }

    //创建所需大小的map,map中的buffer初始化为nullptr
    my_map_pointer create_map(size_type n){
        my_map_pointer mp=static_cast<my_valType**>(::operator new(n * sizeof(my_valType*)));
        for (size_type i = 0; i < n; ++i)
            *(mp + i) = nullptr;
        return mp;
    }

    //创建map中start-finish指针所指的缓冲区节点
    void create_buffer(my_map_pointer start, my_map_pointer finish){
        my_map_pointer cur;
        try{
            //循环创建缓冲区节点
            for (cur = start; cur <= finish; ++cur){
                *cur = static_cast<my_valType*>(::operator new(buffer_size() * sizeof(my_valType)));
            }
        }
        catch (...){
            //出现异常返回最初状态
            while (cur != start){
                --cur;
                if(*cur==nullptr)
                    continue;
                 delete(*cur);
                *cur = nullptr;
            }
            throw;
        }
    }

    //删除map中位于start-end中的缓冲区
    void delete_buffer(my_map_pointer start, my_map_pointer end){
        for(my_map_pointer n=start;n<=end;++n){
            //避免释放空指针
            if(*n!=nullptr)
                 delete(*n);
            *n=nullptr;
        }
    }

};

#endif
