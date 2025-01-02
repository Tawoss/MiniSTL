#ifndef MY_HASHTABLE_H
#define MY_HASHTABLE_H

#include "iterator.h"
#include "vector.h"
#include "utils.h"
#include "algorithm.h"

//萃取node元素属性， 是map， key type!=mapped type     是set key type==mapped type
//默认为set
template<class T, bool>
struct my_HashTable_value_traits_imp{
    typedef T key_type;
    typedef T mapped_type;
    typedef T value_type;
    //获取键值
    template <class Ty>
    static const key_type& get_key(const Ty& value){ return value; }
};

//如果传入参数是pair则为map
template<class T>
struct my_HashTable_value_traits_imp<T,true>{
    typedef typename std::remove_cv<typename T::first_type>::type key_type;
    typedef typename T::second_type                               mapped_type;
    typedef T                                                     value_type;
    //获取键值
    template <class Ty>
    static const key_type& get_key(const Ty& value){ return value.first; }
};

template<class T>
struct my_HashTable_value_traits{
    //type_traits里面写有判断是否为pair的函数
    static constexpr bool is_map = is_pair<T>::value;
    typedef my_HashTable_value_traits_imp<T, is_map> value_traits_type;

    typedef typename value_traits_type::key_type    key_type;
    typedef typename value_traits_type::mapped_type mapped_type;
    typedef typename value_traits_type::value_type  value_type;
    //获取键值
    template <class Ty>
    static const key_type& get_key(const Ty& value){ return value_traits_type::get_key(value); }
};

//定义节点， 节点存储下一个节点的指针 以及 节点本身值
template <class T>
struct hashtable_node
{
    //获取属性
    typedef my_HashTable_value_traits<T>                valuetraits;
    typedef typename valuetraits::key_type              key_type;
    typedef typename valuetraits::mapped_type           mapped_type;
    typedef typename valuetraits::value_type            value_type;
    //定义内部数据成员
    hashtable_node*          next;   // 指向下一个节点
    value_type               val;    // 储存实值

    hashtable_node() = default;
    hashtable_node(const T& n) :next(nullptr), val(n) {}

    hashtable_node(const hashtable_node& other) :next(other.next), val(other.val) {}
    hashtable_node(hashtable_node&& other) :next(other.next), val(mystl::move(other.val)){
        other.next = nullptr;
    }

    //获取键值
    const key_type& get_key(){ return valuetraits::get_key(val); }

};

//hashtable和hashiterator声明
template <class T, class HashFunc, class KeyEqual>
class my_hashtable;

template <class T, class HashFunc, class KeyEqual>
struct hashtable_iterator;

//iterator  类型是forward iterator
template<class T, class HashFunc, class KeyEqual>
struct hashtable_iterator:public mystl::iterator<mystl::forward_iterator_tag,T> {
    //内嵌型别定义
    typedef T                                                   my_valType;
    typedef my_valType*                                         pointer;
    typedef my_valType&                                         reference;
    typedef my_hashtable<T,HashFunc,KeyEqual>                   hashtable;
    typedef hashtable_iterator<T,HashFunc,KeyEqual>             iterator;
    typedef hashtable_node<T>*                                  node_ptr;
    typedef const node_ptr                                      const_node_ptr;
    typedef hashtable*                                          contain_ptr;
    typedef const hashtable*                                    const_contain_ptr;

    typedef std::size_t                                         size_type;
    typedef ptrdiff_t                                           difference_type;

    typedef my_HashTable_value_traits<T>                        value_traits;

    //数据成员
    node_ptr            cur_node;      //迭代器所指节点
    contain_ptr         h_table;    //与hashtable建立联系，方便从一个bucket跳到另一个bucket

    //默认构造
    hashtable_iterator() = default;
    //传入node和hashtable构造
    hashtable_iterator(node_ptr node, contain_ptr table): cur_node(node), h_table(table){}
    //拷贝构造
    hashtable_iterator(const iterator& other): cur_node(other.cur_node), h_table(other.h_table){}

    //拷贝赋值
    iterator& operator=(const iterator& other){
        if(this!=&other){
            cur_node=other.cur_node;
            h_table=other.h_table;
        }
        return *this;
    }

    //重载引用和解引用
    reference operator*() const { return cur_node->val; }
    pointer   operator->() const { return &(operator*()); }

    //重载前置++
    iterator& operator++(){
        const node_ptr old=cur_node;
        cur_node=cur_node->next;
        //到达bucket尾，跳到下一个bucket
        if(!cur_node){
            //记录原来所在bucket
            size_type new_index=h_table->hash(old->get_key());
            //定位下一个bucket起始处
            while(!cur_node && ++new_index < h_table->_bucket_size)
                cur_node=h_table->_buckets[new_index];
        }
        return *this;
    }
    //重载后置++
    iterator operator++(int){
        iterator temp=*this;
        ++*this;
        return temp;
    }

    //重载==和!=
    bool operator==(const iterator& other) const { return cur_node==other.cur_node; }
    bool operator!=(const iterator& other) const { return cur_node!=other.cur_node; }
};

//质数表
#define PRIME_NUM 99
// 1. start with p = 101
// 2. p = next_prime(p * 1.7)
// 3. if p < (2 << 63), go to step 2, otherwise, go to step 4
// 4. end with p = prev_prime(2 << 63 - 1)
static constexpr size_t hashtable_prime_list[] = {
  101ull, 173ull, 263ull, 397ull, 599ull, 907ull, 1361ull, 2053ull, 3083ull,
  4637ull, 6959ull, 10453ull, 15683ull, 23531ull, 35311ull, 52967ull, 79451ull,
  119179ull, 178781ull, 268189ull, 402299ull, 603457ull, 905189ull, 1357787ull,
  2036687ull, 3055043ull, 4582577ull, 6873871ull, 10310819ull, 15466229ull,
  23199347ull, 34799021ull, 52198537ull, 78297827ull, 117446801ull, 176170229ull,
  264255353ull, 396383041ull, 594574583ull, 891861923ull, 1337792887ull,
  2006689337ull, 3010034021ull, 4515051137ull, 6772576709ull, 10158865069ull,
  15238297621ull, 22857446471ull, 34286169707ull, 51429254599ull, 77143881917ull,
  115715822899ull, 173573734363ull, 260360601547ull, 390540902329ull,
  585811353559ull, 878717030339ull, 1318075545511ull, 1977113318311ull,
  2965669977497ull, 4448504966249ull, 6672757449409ull, 10009136174239ull,
  15013704261371ull, 22520556392057ull, 33780834588157ull, 50671251882247ull,
  76006877823377ull, 114010316735089ull, 171015475102649ull, 256523212653977ull,
  384784818980971ull, 577177228471507ull, 865765842707309ull, 1298648764060979ull,
  1947973146091477ull, 2921959719137273ull, 4382939578705967ull, 6574409368058969ull,
  9861614052088471ull, 14792421078132871ull, 22188631617199337ull, 33282947425799017ull,
  49924421138698549ull, 74886631708047827ull, 112329947562071807ull, 168494921343107851ull,
  252742382014661767ull, 379113573021992729ull, 568670359532989111ull, 853005539299483657ull,
  1279508308949225477ull, 1919262463423838231ull, 2878893695135757317ull, 4318340542703636011ull,
  6477510814055453699ull, 9716266221083181299ull, 14574399331624771603ull, 18446744073709551557ull
};

// 找出最接近并大于等于 n 的那个质数
inline std::size_t hashtable_next_prime(size_t n) {
  const size_t* first = hashtable_prime_list;
  const size_t* last = hashtable_prime_list + PRIME_NUM;
  const size_t* pos = mystl::lower_bound(first, last, n);
  return pos == last ? *(last - 1) : *pos;
}

//hashtable
template<class T, class HashFunc, class KeyEqual>
struct my_hashtable{
    //内嵌型别定义
    typedef my_HashTable_value_traits<T>                value_traits;
    typedef typename value_traits::key_type             key_type;
    typedef typename value_traits::mapped_type          mapped_type;
    typedef typename value_traits::value_type           my_valType;
    typedef HashFunc                                    hasher;
    typedef KeyEqual                                    keyequal;

    typedef std::size_t                                 size_type;
    typedef ptrdiff_t                                   difference_type;
    typedef hashtable_node<T>                           node_type;
    typedef node_type*                                  node_ptr;
    typedef my_vector<node_ptr>                         bucket_type;
    typedef my_valType*                                 pointer;
    typedef const my_valType*                           const_pointer;
    typedef my_valType&                                 reference;
    typedef const my_valType&                           const_reference;
    typedef hashtable_iterator<T,HashFunc,KeyEqual>     iterator;
    typedef const iterator                              const_iterator;

    //数据成员
    bucket_type     _buckets;        //是一个vector, 元素为链表
    size_type       _bucket_size;
    size_type       _size;
    hasher          _hash;
    keyequal        _equal;

    //功能函数
    //找到大于n的下一个质数
    size_type next_size(size_type n) const {
        return hashtable_next_prime(n);
    }

    //默认初始化_buckets数组
    void default_init(size_type n){
        size_type bucket_num=next_size(n);
        _buckets.reserve(bucket_num);
        _buckets.insert(_buckets.end(),bucket_num,(node_ptr)0);
        _bucket_size=_buckets.size();
    }

    //判断两个key是否相等
    bool is_equal(const key_type& k1, const key_type& k2) { return _equal(k1,k2); }
    bool is_equal(const key_type& k1, const key_type& k2) const { return _equal(k1,k2); }

    //找到第一个节点
    iterator        m_begin(){
        for(size_type n=0;n<_bucket_size;++n){
            //从头开始，找到不为空的节点就直接返回
            if(_buckets[n])
                return iterator(_buckets[n], this);
        }
        return iterator(nullptr, this);
    }
    const_iterator  m_begin() const {
        for(size_type n=0;n<_bucket_size;++n){
            //从头开始，找到不为空的节点就直接返回
            if(_buckets[n])
                return const_iterator(_buckets[n], this);
        }
        return const_iterator(nullptr, this);
    }

    //hash函数
    size_type       hash(const key_type& key, size_type n) const { return _hash(key) % n; }
    size_type       hash(const key_type& key) const { return _hash(key) % _bucket_size; }

    //create node创建节点
    template<class ...Args>
    node_ptr        create_node(Args&&... args){
        //为节点分配内存
        node_ptr temp=static_cast<node_type*>( ::operator new(sizeof(node_type)) );
        //在&temp->val处直接使用new创建值
        new (temp) node_type(mystl::forward<Args>(args)...);
        temp->next=nullptr;
        return temp;
    }
    //delete node删除节点
    void            delete_node(node_ptr& node){
        if(&(node->val)!=nullptr)
            (&(node->val))->~my_valType();
        delete node;
    }

    //copy_init拷贝另一个hashtable来初始化
    void            copy_init(const my_hashtable& other){
        //拷贝_buckets[]
        _buckets.clear();
        _buckets.reserve(other._buckets.size());
        _buckets.insert(_buckets.end(), other._buckets.size(), (node_ptr)0);
        //遍历other的每一个bucket
        for(size_type n=0;n<other._buckets.size();++n){
            node_ptr cur=other._buckets[n];
            //cur不为空则复制
            if(cur){
                node_ptr copy=create_node(cur->val);
                _buckets[n]=copy;
                //复制该bucket中cur后的所有元素
                for(node_ptr next=cur->next; next; cur = next, next = cur->next){
                    copy->next=create_node(next->val);
                    copy=copy->next;
                }
                //最后一个元素的next置空
                copy->next=nullptr;
            }
        }
        //拷贝_bucket_size 和 _size
        _bucket_size=other._bucket_size;
        _size=other._size;
    }

    //equal_range 返回一个 pair 对象，其包含 2 个迭代器，用于表明当前容器中值为 key 的元素所在的范围
    std::pair<iterator,iterator>  equal_range_unique(const key_type& key){
        const size_type n = hash(key);
        for(node_ptr first = _buckets[n]; first; ++first){
            if(is_equal(first->get_key(), key)){
                //下个位置不为空
                if(first->next)
                    return mystl::make_pair(iterator(first,this), iterator(first->next,this));
                //下个位置为空， 到达_buckets[n]结尾， 寻找下一个bucket
                for(size_type i = n + 1; i<_bucket_size; ++i){
                    if(_buckets[i])
                        return mystl::make_pair(iterator(first,this), iterator(_buckets[i], this));
                }
                //没能找到下一个bucket 返回end()
                return mystl::make_pair(iterator(first, this), end());
            }
        }
        return mystl::make_pair(end(),end());
    }
    std::pair<iterator,iterator>  equal_range_multi(const key_type& key){
        const size_type n = hash(key);
        for(node_ptr first = _buckets[n]; first; ++first){
            if(is_equal(first->get_key(), key)){
                //下个位置不为空
                for(node_ptr next = first->next; next; next=next->next){
                    if(!is_equal(next->get_key(), key))
                        return mystl::make_pair(iterator(first,this), iterator(next,this));
                }

                //下个位置为空， 到达_buckets[n]结尾， 寻找下一个bucket
                for(size_type i = n + 1; i<_bucket_size; ++i){
                    if(_buckets[i])
                        return mystl::make_pair(iterator(first,this), iterator(_buckets[i], this));
                }
                //没能找到下一个bucket 返回end()
                return mystl::make_pair(iterator(first, this), end());
            }
        }
        return mystl::make_pair(end(),end());
    }

    //返回_buckets[n]的链表节点个数
    size_type       bucket_size(size_type n){
        size_type result = 0;
        for (node_ptr cur = _buckets[n]; cur; cur = cur->next)
            ++result;
        return result;
    }
    //返回key所在bucket
    size_type       bucket(const key_type& key)  const { return hash(key); }

    //返回当前容器使用的哈希函数对象
    hasher hash_function() { return _hash; }

    //返回键值为key的节点数量
    size_type count(const key_type& key){
        size_type n = hash(key);
        size_type result = 0;
        for (node_ptr cur = _buckets[n]; cur; cur = cur->next)
            if(is_equal(cur->get_key(), key))
                ++result;
        return result;
    }

    //找到键值为key的节点
    iterator find(const key_type& key){
        size_type n = hash(key);
        for (node_ptr cur = _buckets[n]; cur; cur = cur->next)
            if(is_equal(cur->get_key(), key))
                return iterator(cur,this);
        return iterator(nullptr,this);
    }

    //默认构造
    explicit my_hashtable(size_type bucket_count, const HashFunc& hash=HashFunc(), const KeyEqual& equal=KeyEqual())
    :_size(0), _hash(hash), _equal(equal)
    {
        default_init(bucket_count);
    }
    //range构造
    template<class iter>
    my_hashtable(iter first, iter last, size_type bucket_count, const HashFunc& hash=HashFunc(), const KeyEqual& equal=KeyEqual())
    :_size(mystl::distance(first,last)), _hash(hash), _equal(equal)
    {
        default_init(mystl::max(bucket_count, static_cast<size_type>(mystl::distance(first,last))));
    }
    //拷贝构造
    my_hashtable(const my_hashtable& other)
    :_hash(other._hash), _equal(other._equal)
    {
        copy_init(other);
    }
    //移动构造
    my_hashtable(my_hashtable&& other)
    :_bucket_size(other._bucket_size),
    _size(other._size),
    _hash(other._hash),
    _equal(other._equal)
    {
        _buckets=mystl::move(other._buckets);
        other._bucket_size=0;
        other._size=0;
    }

    //拷贝赋值
    my_hashtable& operator=(const my_hashtable& other){
        if(this!=&other){
            my_hashtable temp(other);
            swap(temp);
        }
        return *this;
    }
    //移动赋值
    my_hashtable& operator=(my_hashtable&& other){
        if(this!=&other){
            my_hashtable temp(mystl::move(other));
            swap(temp);
        }
        return *this;
    }
    //析构函数
    ~my_hashtable(){ clear(); }

    // 迭代器相关操作
    iterator       begin()        noexcept { return m_begin(); }
    const_iterator cbegin() const noexcept { return begin(); }
    iterator       end()          noexcept { return iterator(nullptr, this); }
    const_iterator cend()   const noexcept { return end(); }

    // 容量相关操作
    bool           empty()    const noexcept { return _size == 0; }
    size_type      size()     const noexcept { return _size; }
    size_type      max_size() const noexcept { return static_cast<size_type>(-1); }
    size_type      bucket_count() const noexcept { return _bucket_size; }
    size_type      max_bucket_count() const noexcept { return hashtable_prime_list[PRIME_NUM - 1]; }

    //rehash操作
    //replace_bucket
    void replace_bucket(size_type bucket_count){
        bucket_type bucket(bucket_count);
        //原来的容器不为空, 遍历原来的_buckets来处理所有元素
        if(_size!=0){
            for(size_type i=0;i<_bucket_size;++i){
                //处理_buckets[i]中的每个元素
                for(node_ptr first = _buckets[i]; first; first=first->next){
                    node_ptr temp=create_node(first->val);
                    const size_type n=hash(first->get_key(),bucket_count);
                    node_ptr new_first=bucket[n];
                    bool is_inserted=false;
                    //判断bucket中是否有重复值，有就不再插入(记录is_inserted flag)
                    for(node_ptr cur=new_first; cur; cur=cur->next){
                        if(is_equal(cur->get_key(),first->get_key())){
                            temp->next=cur->next;
                            cur->next=temp;
                            is_inserted=true;
                            break;
                        }
                    }
                    if(!is_inserted){
                        temp->next=new_first;
                        bucket[n]=temp;
                    }
                }
            }
        }
        _buckets.swap(bucket);
        _bucket_size=_buckets.size();
    }
    //判断是否需要rehash
    void rehash_if_need(size_type n){
        if(_size+n>_bucket_size)
            rehash(_size+n);
    }
    //rehash重新对元素进行一遍哈希，插入到新的位置
    void rehash(size_type count){
        size_type new_bucketnum=hashtable_next_prime(count);
        if(new_bucketnum>_bucket_size)
            replace_bucket(new_bucketnum);
        else{
            if( (float)_size / (float)new_bucketnum < 0.75 && (float)new_bucketnum < (float)_bucket_size * 0.75 )
                replace_bucket(new_bucketnum);
        }
    }

    //reserve
    void reserve(size_type count) { rehash(static_cast<size_type>((float)count + 0.5f)); }

    //插入操作
    //通过val插入新节点，键值不允许重复
    std::pair<iterator,bool>     hash_insert_val_unique(const my_valType& val){
        const size_type n=hash(value_traits::get_key(val));
        node_ptr first = _buckets[n];
        //检查是否有重复值， 有则返回未能成功插入的pair
        for(node_ptr cur=first; cur; cur=cur->next){
            if(is_equal(cur->get_key(),value_traits::get_key(val)))
                return mystl::make_pair(iterator(cur,this), false);
        }
        node_ptr temp=create_node(val);
        temp->next=first;
        _buckets[n]=temp;
        ++_size;
        return mystl::make_pair(iterator(temp,this), true);
    }
    //通过val插入新节点，键值允许重复
    iterator                     hash_insert_val_multi(const my_valType& val){
        const size_type n=hash(value_traits::get_key(val));
        node_ptr first=_buckets[n];
        node_ptr temp=create_node(val);
        for(node_ptr cur=first; cur; cur=cur->next){
            if(is_equal(cur->get_key(),value_traits::get_key(val))){
                temp->next=cur->next;
                cur->next=temp;
                ++_size;
                return iterator(temp,this);
            }
        }
        //更新_buckets[n]
        temp->next=first;
        _buckets[n]=temp;
        ++_size;
        return iterator(temp,this);
    }

    //插入值为val的节点， 判断是否需要rehash (支持unique, multi以及移动操作)
    std::pair<iterator,bool>     insert_unique(const my_valType& val){
        rehash_if_need(1);
        return hash_insert_val_unique(val);
    }
    std::pair<iterator,bool>     insert_unique(my_valType&& val){
        return emplace_unique(mystl::move(val));
    }
    iterator                     insert_unique_hint(const_iterator pos, const my_valType& val){ return insert_unique(val).first; }
    iterator                     insert_unique_hint(const_iterator pos, my_valType&& val){ return emplace_unique(mystl::move(val)).first; }

    iterator                     insert_multi(const my_valType& val){
        rehash_if_need(1);
        return hash_insert_val_multi(val);
    }
    iterator                     insert_multi(my_valType&& val){
        return emplace_multi(mystl::move(val));
    }
    iterator                     insert_multi_hint(const_iterator pos, const my_valType& val){ return insert_multi(val); }
    iterator                     insert_multi_hint(const_iterator pos, my_valType&& val){ return emplace_multi(mystl::move(val)); }

    //range插入 根据iter类型判断插入方法(支持unique, multi)
    template<class iter>
    void range_insert_unique(iter first, iter last){
        size_type n= static_cast<size_type>(mystl::distance(first,last));
        rehash_if_need(n);
        for(; first!=last;++first)
            hash_insert_val_unique(*first);
    }
    template<class iter>
    void range_insert_multi(iter first, iter last){
        size_type n= static_cast<size_type>(mystl::distance(first,last));
        rehash_if_need(n);
        for(; first!=last;++first)
            hash_insert_val_multi(*first);
    }
    template<class iter>
    void insert_unique(iter first, iter last){
        range_insert_unique(first, last);
    }
    template<class iter>
    void insert_multi(iter first, iter last){
        range_insert_multi(first, last);
    }

    //insert_node_unique 函数插入node
    std::pair<iterator,bool>     hash_insert_node_unique(node_ptr node){
        const size_type n=hash(node->get_key());
        node_ptr first=_buckets[n];
        //如果_buckets[n]为空， 直接插入node作为链表
        if(first==nullptr){
            _buckets[n]=node;
            ++_size;
            return mystl::make_pair(iterator(node,this),true);
        }
        //检查是否有重复值， 有则返回未能成功插入的pair
        for(node_ptr cur=first; cur; cur=cur->next)
            if(is_equal(cur->get_key(),node->get_key()))
                return mystl::make_pair(iterator(cur,this), false);
        //更新_buckets[n]
        node->next=first;
        _buckets[n]=node;
        ++_size;
        return mystl::make_pair(iterator(node,this), true);
    }
    //insert_node_multi 函数
    iterator                     hash_insert_node_multi(node_ptr node){
        const size_type n=hash(node->get_key());
        node_ptr first=_buckets[n];
        //如果_buckets[n]为空， 直接插入node作为链表
        if(first==nullptr){
            _buckets[n]=node;
            ++_size;
            return iterator(node,this);
        }
        //检查是否有重复值， 有则返回未能成功插入的pair
        for(node_ptr cur=first; cur; cur=cur->next){
            if(is_equal(cur->get_key(),node->get_key())){
                node->next=cur->next;
                cur->next=node;
                ++_size;
                return iterator(node,this);
            }
        }
        //更新_buckets[n]
        node->next=first;
        _buckets[n]=node;
        ++_size;
        return iterator(node,this);
    }

    //就地构造元素，键值不允许重复
    template<class ...Args>
    std::pair<iterator,bool>    emplace_unique(Args&&... args){
        //为节点分配内存
        node_ptr temp=create_node(mystl::forward<Args>(args)...);
        try{
            if(_size+1>_bucket_size)
                rehash(_size+1);
        }catch(...){
            delete_node(temp);
            throw;
        }
        return hash_insert_node_unique(temp);
    }
    template<class ...Args>
    iterator                    emplace_unique_hint(const_iterator pos, Args&&... args) {
        return emplace_unique(mystl::forward<Args>(args)...).first;
    }
    //就地构造元素，键值允许重复
    template<class ...Args>
    iterator                    emplace_multi(Args&&... args){
        //为节点分配内存
        node_ptr temp=create_node(mystl::forward<Args>(args)...);
        try{
            if(_size+1>_bucket_size)
                rehash(_size+1);
        }catch(...){
            delete_node(temp);
            throw;
        }
        return hash_insert_node_multi(temp);
    }
    template<class ...Args>
    iterator                    emplace_multi_hint(const_iterator pos, Args&&... args) {
        return emplace_multi(mystl::forward<Args>(args)...);
    }

    //删除操作
    //删除pos所指节点
    iterator   erase(const_iterator pos){
        node_ptr node=pos.cur_node;
        if(node){
            const size_type n=hash(node->get_key());
            node_ptr cur=_buckets[n];
            node_ptr next=cur->next;
            if(cur==node){
                _buckets[n]=next;
                delete_node(cur);
                --_size;
                return iterator(next,this);
            }
            while(next){
                if(next==node){
                    cur->next=next->next;
                    delete_node(next);
                    --_size;
                    return iterator(cur->next,this);
                }
                else{
                    cur=next;
                    next=cur->next;
                }
            }
        }
        return iterator(nullptr,this);
    }

    // erase_bucket 函数
    // 在第 n 个 bucket 内，删除 [buckets_[n], last) 的节点
    void       erase_bucket(size_type n, node_ptr last){
        node_ptr cur = _buckets[n];
        //遍历删除_buckets[n]中的节点
        while (cur != last){
            node_ptr next = cur->next;
            delete_node(cur);
            cur = next;
            --_size;
        }
        _buckets[n] = last;
    }
    // 在第 n 个 bucket 内，删除 [first, last) 的节点
    void       erase_bucket(size_type n, node_ptr first, node_ptr last){
        node_ptr cur = _buckets[n];
        if (cur == first)
            erase_bucket(n, last);
        else{
            node_ptr next = cur->next;
            //定位first所指节点
            for (; next != first; cur = next, next = cur->next){}
            //遍历删除first-last所指节点
            while (next != last){
                cur->next = next->next;
                delete_node(next);
                next = cur->next;
                --_size;
            }
        }
    }

    //删除first-last所指节点
    iterator   erase(const_iterator first, const_iterator last){
        if(first.cur_node==last.cur_node)
            return iterator(nullptr,this);
        //定位first 和 last 所在的bucket
        size_type first_bucket = first.cur_node ? hash(first.cur_node->get_key()) : _bucket_size;
        size_type last_bucket = last.cur_node ? hash(last.cur_node->get_key()) : _bucket_size;
        //位于同一个bucket
        if(first_bucket==last_bucket)
            erase_bucket(first_bucket, first.cur_node, last.cur_node);
        else{
            //删除first_bucket中的节点
            erase_bucket(first_bucket, first.cur_node, nullptr);
            //删除first+1-last_bucket-1中的节点
            for(size_type n = first_bucket + 1; n < last_bucket; ++n){
                if(_buckets[n]!=nullptr)
                    erase_bucket(n,nullptr);
            }
            //删除last_bucket中的节点
            if(last_bucket!=_bucket_size)
                erase_bucket(last_bucket,last.cur_node);
        }
        return iterator(last.cur_node,this);
    }

    //删除键值为key的节点， 不允许重复
    size_type erase_unique(const key_type& key){
        const size_type n = hash(key);
        node_ptr first = _buckets[n];
        if(first==nullptr)
            return 0;

        //key位于_buckets[n]起始处
        if(is_equal(first->get_key(), key)){
            _buckets[n]=first->next;
            delete_node(first);
            --_size;
            return 1;
        }
        else{
            //遍历后续节点， 查找键值key
            node_ptr next = first->next;
            while(next){
                if(is_equal(next->get_key(), key)){
                    first->next=next->next;
                    delete_node(next);
                    --_size;
                    return 1;
                }
                first=next;
                next=first->next;
            }
        }
        return 0;
    }

    //删除键值为key的节点， 允许重复
    size_type erase_multi(const key_type& key){
        std::pair<iterator, iterator>  p = equal_range_multi(key);
        if(p.first.cur_node){
            erase(p.first, p.second);
            return mystl::distance(p.first,p.second);
        }
        return 0;
    }

    //清空hashtable
    void clear(){
        if(_size==0)
            return;
        //遍历所有bucket
        for(size_type n=0;n<_bucket_size;++n){
            node_ptr cur=_buckets[n];
            //遍历删除_buckets[n]中的节点
            while(cur!=nullptr){
                node_ptr next=cur->next;
                delete_node(cur);
                cur=next;
            }
            _buckets[n]=nullptr;
        }
        _size=0;
    }

    //swap
    void swap(my_hashtable& other) noexcept {
        if (this != &other)
        {
            _buckets.swap(other._buckets);
            mystl::swap(_bucket_size, other._bucket_size);
            mystl::swap(_size, other._size);
            mystl::swap(_hash, other._hash);
            mystl::swap(_equal, other._equal);
        }
    }
};





#endif
