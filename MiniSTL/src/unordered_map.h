#ifndef MY_UNORDERED_MAP_H
#define MY_UNORDERED_MAP_H

#include "hashtable.h"

template<class Key, class T, class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>>
struct myunordered_map{
    //定义hashtable
    typedef my_hashtable<std::pair<const Key, T>,Hash,KeyEqual>    myhash_table;
    //使用 hashtable 的型别
    typedef typename myhash_table::key_type             key_type;
    typedef typename myhash_table::mapped_type          mapped_type;
    typedef typename myhash_table::my_valType           my_valType;
    typedef typename myhash_table::hasher               hasher;
    typedef typename myhash_table::keyequal             keyequal;
    typedef typename myhash_table::size_type            size_type;
    typedef typename myhash_table::difference_type      difference_type;
    typedef typename myhash_table::pointer              pointer;
    typedef typename myhash_table::const_pointer        const_pointer;
    typedef typename myhash_table::reference            reference;
    typedef typename myhash_table::const_reference      const_reference;
    typedef typename myhash_table::iterator             iterator;
    typedef typename myhash_table::const_iterator       const_iterator;

    //定义内部数据成员 hashtable
    myhash_table        my_ht;

    //默认构造
    myunordered_map():my_ht(100,Hash(), KeyEqual()){}
    //传入bucketcount, Hash函数, KeyEqual函数构造
    myunordered_map(size_type bucket_count, const Hash& hash=Hash(), const KeyEqual& equal=KeyEqual()):my_ht(bucket_count,hash,equal){}
    //传入range, bucketcount, Hash函数, KeyEqual函数构造
    template <class iter>
    myunordered_map(iter first, iter last, const size_type bucket_count = 100, const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual())
        : my_ht(mystl::max(bucket_count, static_cast<size_type>(mystl::distance(first, last))), hash, equal)
    {
        for (; first != last; ++first)
            my_ht.hash_insert_val_unique(*first);
    }
    //传入初始化列表, bucketcount, Hash函数, KeyEqual函数构造
    myunordered_map(std::initializer_list<my_valType> ilist, const size_type bucket_count = 100, const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual())
        : my_ht(mystl::max(bucket_count, static_cast<size_type>(ilist.size())), hash, equal)
    {
        for (auto first=ilist.begin(), last=ilist.end(); first != last; ++first)
            my_ht.hash_insert_val_unique(*first);
    }
    //拷贝构造
    myunordered_map(const myunordered_map& other):my_ht(other.my_ht) {}
    //移动构造
    myunordered_map(myunordered_map&& other):my_ht(mystl::move(other.my_ht)) {}

    //拷贝赋值
    myunordered_map& operator=(const myunordered_map& other){
        my_ht=other.my_ht;
        return *this;
    }
    //移动赋值
    myunordered_map& operator=(myunordered_map&& other){
        my_ht=mystl::move(other.my_ht);
        return *this;
    }
    //初始化列表赋值
    myunordered_map& operator=(std::initializer_list<my_valType> ilist){
        //清空原来的容器
        my_ht.clear();
        //增加hash bucket数量
        my_ht.reserve(ilist.size());
        //在新容器中插入ilist中的元素
        for (auto first=ilist.begin(), last=ilist.end(); first != last; ++first)
            my_ht.hash_insert_val_unique(*first);
        return *this;
    }

    // 迭代器相关
    iterator       begin()        noexcept { return my_ht.begin(); }
    const_iterator cbegin() const noexcept { return my_ht.cbegin(); }

    iterator       end()          noexcept { return my_ht.end(); }
    const_iterator cend()   const noexcept { return my_ht.cend(); }

    // 容量相关
    bool       empty()    const noexcept { return my_ht.empty(); }
    size_type  size()     const noexcept { return my_ht.size(); }
    size_type  max_size() const noexcept { return my_ht.max_size(); }
    //bucket相关
    size_type  bucket(const key_type& key) { return my_ht.bucket(key); }
    size_type  bucket_count() const noexcept { return my_ht.bucket_count(); }
    size_type  max_bucket_count() const noexcept { return my_ht.max_bucket_count(); }
    size_type  bucket_size(size_type n) const noexcept { return my_ht._bucket_size(n); }

    //功能函数
    //equal_range  返回一个 pair 对象，其包含 2 个迭代器，用于表明当前容器中值为 key 的元素所在的范围
    std::pair<iterator,iterator>   equal_range(const key_type& key) { return my_ht.equal_range_unique(key); }

    //返回 unordered_set 容器中当前的负载因子
    float load_factor() const noexcept { return (float)my_ht.size() / (float)my_ht.bucket_count(); }

    float max_load_factor() const noexcept { return 1.0; }

    //将当前容器底层使用桶的数量设置为 n
    void rehash(size_type n) { my_ht.rehash(n); }

    //将存储桶的数量（也就是 bucket_count() 方法的返回值）设置为至少容纳 count 个元素（不超过最大负载因子）所需的数量，并重新整理容器
    void reserve(size_type n) { my_ht.reserve(n); }

    //返回当前容器使用的哈希函数对象
    hasher hash_function() { return my_ht.hash_function(); }

    //返回键值为key的节点数量
    size_type count(const key_type& key) { return my_ht.count(key); }

    //找到键值为key的节点
    iterator find(const key_type& key) { return my_ht.find(key); }


    //插入操作
    //emplace
    template<class ...Args>
    std::pair<iterator, bool>  emplace(Args&& ...args) { return my_ht.emplace_unique(mystl::forward<Args>(args)...); }
    template<class ...Args>
    std::pair<iterator, bool>  emplace_hint(const_iterator pos,Args&& ...args) { return my_ht.emplace_unique_hint(pos,mystl::forward<Args>(args)...); }

    //insert
    std::pair<iterator,bool>    insert(const my_valType& val) { return my_ht.insert_unique(val); }
    std::pair<iterator,bool>    insert(my_valType&& val) { return my_ht.emplace_unique(mystl::move(val)); }
    iterator                    insert(const_iterator pos, const my_valType& val) { return my_ht.insert_unique_hint(val); }
    iterator                    insert(const_iterator pos, my_valType&& val) { return my_ht.emplace_unique_hint(mystl::move(val)); }
    template<class iter> void   insert(iter first, iter last) { my_ht.insert_unique(first,last); }
    void                        insert(std::initializer_list<my_valType> ilist) { my_ht.insert_unique(ilist.begin(),ilist.end()); }

    //删除操作
    iterator  erase(const_iterator pos){ return my_ht.erase(pos); }
    iterator  erase(const_iterator first, const_iterator last) { return my_ht.erase(first,last); }
    size_type erase(const key_type& key) { return my_ht.erase_unique(key); }

    //清空容器
    void clear() { my_ht.clear(); }


    //访问相关
    //找到 map 容器中 key 键对应的值，如果找不到，该函数会引发 out_of_range 异常
    mapped_type& at(const key_type& key){
        iterator iter=find(key);
        if(iter==end())
            std::__throw_out_of_range_fmt("map<%zu,val> no such key found", key);
        return (*iter).second;
    }
    const mapped_type& at(const key_type& key) const {
        iterator iter=find(key);
        if(iter==end())
            std::__throw_out_of_range_fmt("map<%zu,val> no such key found", key);
        return (*iter).second;
    }

    //重载operator[]
    mapped_type& operator[](const key_type& key){
        iterator iter=my_ht.find(key);
        if(iter.cur_node==nullptr)
            iter=my_ht.emplace_unique(mystl::make_pair(key,T{})).first;
        return (*iter).second;
    }
    mapped_type& operator[](key_type&& key){
        iterator iter=my_ht.find(key);
        if(iter.cur_node==nullptr)
            iter = my_ht.emplace_unique(mystl::make_pair(mystl::move(key),T{})).first;
        return (*iter).second;
    }
};


#endif
