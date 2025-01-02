#ifndef MY_SET_H
#define MY_SET_H

#include "RB_tree.h"

//参数一代表键值类型，参数二代表键值的比较方式，缺省使用 std::less
template<class Key, class Compare = std::less<Key>>
struct my_set{
    //定义内部树
    typedef my_RBtree<Key,Compare>      my_tree;
    my_tree     mytree;

    //定义内嵌型别
    typedef Key                     my_keyType;
    typedef Key                     my_valType;
    typedef Compare                 key_compare;
    typedef Compare                 val_compare;

    typedef typename my_tree::pointer                   my_pointer;
    typedef typename my_tree::const_pointer             const_my_pointer;
    typedef typename my_tree::reference                 my_reference;
    typedef typename my_tree::const_reference           const_my_reference;
    typedef typename my_tree::Node                      Node;
    typedef typename my_tree::iterator                  iterator;
    typedef typename my_tree::const_iterator            const_iterator;
    typedef typename my_tree::reverse_iterator          reverse_iterator;
    typedef typename my_tree::const_reverse_iterator    const_reverse_iterator;
    typedef typename my_tree::size_type                 size_type;

    //默认构造
    my_set() = default;

    //range构造函数
    template<class iter>
    my_set(iter first, iter last)
    :mytree()
    {
        mytree.rb_insert_range_unique(first,last);
    }

    //拷贝构造
    my_set(const my_set& other): mytree(other.mytree) {}

    //移动构造
    my_set(my_set&& other): mytree(mystl::move(other.mytree)) {}

    //initializer list构造
    my_set(std::initializer_list<my_valType> ilist)
    : mytree()
    {
        mytree.rb_insert_range_unique(ilist.begin(), ilist.end());
    }

    //拷贝赋值
    my_set& operator=(const my_set& other){
        mytree=other.mytree;
        return *this;
    }
    //移动赋值
    my_set& operator=(my_set&& other){
        mytree=mystl::move(other.mytree);
        return *this;
    }


    // 迭代器相关
    //返回指向容器中第一个（已排好序的第一个）元素的双向迭代器
    iterator               begin()         noexcept { return mytree.begin(); }
    const_iterator         begin()   const noexcept { return mytree.begin(); }
    const_iterator         cbegin()  const noexcept { return begin(); }

    //返回指向容器最后一个元素（已排好序的最后一个）所在位置后一个位置的双向迭代器
    iterator               end()           noexcept { return mytree.end(); }
    const_iterator         end()     const noexcept { return mytree.end(); }
    const_iterator         cend()    const noexcept { return end(); }

    //返回指向最后一个（已排好序的最后一个）元素的反向双向迭代器
    reverse_iterator       rbegin()        noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin()  const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const noexcept { return rbegin(); }

    //返回指向第一个（已排好序的第一个）元素所在位置前一个位置的反向双向迭代器
    reverse_iterator       rend()          noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend()    const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend()   const noexcept { return rend(); }

    // 容量相关
    bool                   empty()    const noexcept { return mytree.empty(); }
    size_type              size()     const noexcept { return mytree.size(); }
    size_type              max_size() const noexcept { return mytree.max_size(); }

    //swap
    void swap(my_set& other) noexcept { mytree.swap(other.mytree); }

    //set相关操作
    //在 set 容器中查找值为 val 的元素   调用rbtree的成员方法
    iterator find(const my_valType& val) { return mytree.find(val); }
    const_iterator find(const my_valType& val) const { return mytree.find(val); }

    //在当前 set 容器中，查找值为 val 的元素的个数，并返回
    size_type count(const my_valType& val) { return mytree.count_unique(val); }

    //返回一个指向当前 set 容器中第一个大于或等于 val 的元素的双向迭代器
    iterator lower_bound(const my_valType& val) { return mytree.lower_bound(val); }
    const_iterator lower_bound(const my_valType& val) const { return mytree.lower_bound(val); }

    //返回一个指向当前 set 容器中第一个大于 val 的元素的双向迭代器
    iterator upper_bound(const my_valType& val) { return mytree.upper_bound(val); }
    const_iterator upper_bound(const my_valType& val) const { return mytree.upper_bound(val); }

    //equal_range 该方法返回一个 pair 对象 pair.first 和 lower_bound 方法的返回值等价，pair.second 和 upper_bound 方法的返回值等价
    std::pair<iterator,iterator> equal_range(const my_valType& val) { return mytree.equal_range_unique(val); }
    std::pair<const_iterator,const_iterator> equal_range(const my_valType& val) const { return mytree.equal_range_unique(val); }

    //插入删除
    //插入值为val的节点     拷贝
    iterator insert(const my_valType& val) { return mytree.emplace_unique(val); }
    //插入值为val的节点     移动
    iterator insert(my_valType&& val) { return mytree.emplace_unique(mystl::move(val)); }
    //给定插入位置在pos附近     拷贝
    iterator insert(iterator pos, const my_valType& val) { return mytree.emplace_hint_unique(pos,val); }
    //给定插入位置在pos附近     移动
    iterator insert(iterator pos, my_valType&& val) { return mytree.emplace_hint_unique(pos,mystl::move(val)); }
    //给定range插入
    template<class iter>
    void insert(iter first, iter last) { mytree.rb_insert_range_unique(first,last); }

    //initializer_list插入
    void insert(std::initializer_list<my_valType> ilist) { mytree.rb_insert_range_unique(ilist.begin(),ilist.end()); }

    //删除迭代器所指位置节点
    void erase(const_iterator pos) { mytree.rb_erase(pos.node, mytree.root()); }
    //删除容器中值为val的节点
    size_type erase(const my_valType& val) { return mytree.erase_val_unique(val); }
    //删除range中迭代器所指节点
    void erase(const_iterator first, const_iterator last) { mytree.erase_range(first,last); }
    //清空容器
    void clear(){ erase(begin(),end()); }

};

#endif

