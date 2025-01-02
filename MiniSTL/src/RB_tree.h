#ifndef MY_RB_TREE_H
#define MY_RB_TREE_H

#include <iostream>
#include "iterator.h"
#include "algobase.h"
#include "utils.h"

enum RBtree_color { red = false, black = true };

//萃取node元素属性， 是map， key type!=mapped type     是set key type==mapped type
//默认为set
template<class T, bool>
struct my_RBtree_value_traits_imp{
    typedef T key_type;
    typedef T mapped_type;
    typedef T value_type;
    //获取键值
    template <class Ty>
    static const key_type& get_key(const Ty& value){ return value; }
};
//如果传入参数是pair则为map
template<class T>
struct my_RBtree_value_traits_imp<T,true>{
    typedef typename std::remove_cv<typename T::first_type>::type key_type;
    typedef typename T::second_type                               mapped_type;
    typedef T                                                     value_type;
    //获取键值
    template <class Ty>
    static const key_type& get_key(const Ty& value){ return value.first; }
};
template<class T>
struct my_RBtree_value_traits{
    //type_traits里面写有判断是否为pair的函数
    static constexpr bool is_map = is_pair<T>::value;
    typedef my_RBtree_value_traits_imp<T, is_map> value_traits_type;

    typedef typename value_traits_type::key_type    key_type;
    typedef typename value_traits_type::mapped_type mapped_type;
    typedef typename value_traits_type::value_type  value_type;
    //获取键值
    template <class Ty>
    static const key_type& get_key(const Ty& value){ return value_traits_type::get_key(value); }
};

//根据萃取机决定是什么node
template<class T>
struct my_RBtree_node{
    typedef my_RBtree_value_traits<T>               valuetraits;
    typedef typename valuetraits::key_type           key_type;
    typedef typename valuetraits::mapped_type        mapped_type;
    typedef typename valuetraits::value_type         value_type;

public:
    my_RBtree_node<T>*      left;           //左指针
    my_RBtree_node<T>*      right;          //右指针
    my_RBtree_node<T>*      parent;         //双亲指针
    RBtree_color            color;          //颜色标记位
    // key_type                key;            //节点键值
    // mapped_type             mapped_val;     //键值对应val
    value_type              val;            //节点pair

    my_RBtree_node() = default;
    //传参构造构造函数
    my_RBtree_node(const T& kv)
    :left(nullptr),
    right(nullptr),
    parent(nullptr),
    val(kv),
    color(red){}

    my_RBtree_node(const key_type& k, const mapped_type& v)
    :left(nullptr),
    right(nullptr),
    parent(nullptr),
    val(mystl::make_pair(k,v)),
    color(red){}

    //获取键值
    const key_type& get_key(){ return valuetraits::get_key(val); }
};

//树的相关算法
//返回_root子树中最小的节点
template<class node_ptr>
static node_ptr rb_minimum(node_ptr _root) noexcept {
    if(_root==nullptr)
        return _root;
    while(_root->left!=nullptr)
        _root=_root->left;
    return _root;
}
//返回_root子树中最大的节点
template<class node_ptr>
static node_ptr rb_maximum(node_ptr _root) noexcept {
    if(_root==nullptr)
        return _root;
    while(_root->right!=nullptr)
        _root=_root->right;
    return _root;
}
//判断node是否为左子结点, 返回false则为右子结点
template<class node_ptr>
bool rb_is_left_child(node_ptr node) noexcept {
    return node==node->parent->left;
}
//判断node颜色是否为红， 返回false则为黑
template<class node_ptr>
bool rb_is_red(node_ptr node) noexcept {
    return node->color==red;
}
//设置节点颜色为红
template<class node_ptr>
bool rb_set_red(node_ptr node) noexcept {
    return node->color=red;
}
//设置节点颜色为黑
template<class node_ptr>
bool rb_set_black(node_ptr node) noexcept {
    return node->color=black;
}
//寻找下一个节点
template <class node_ptr>
node_ptr rb_successor(node_ptr node) noexcept {
    if (node->right != nullptr)
        return rb_minimum(node->right);
    while (!rb_is_left_child(node))
        node = node->parent;
    return node->parent;
}

//左单旋
template<class node_ptr>
void rb_rotate_left(node_ptr x, node_ptr& _root ){
    node_ptr y = x->right;
    //把b作为x右子树
    x->right=y->left;
    if(y->left!=nullptr)
        y->left->parent=x;
    //把y作为p的子树
    y->parent=x->parent;

    //如果x是根节点, y成为根节点
    if(x==_root)
        _root=y;
    else if(rb_is_left_child(x)){
        x->parent->left=y;
    }
    else{
        x->parent->right=y;
    }
    y->left=x;
    x->parent=y;
}
//右单旋
template<class node_ptr>
void rb_rotate_right(node_ptr x, node_ptr& _root ){
    node_ptr y = x->left;
    //把b作为x右子树
    x->left=y->right;
    if(y->right!=nullptr)
        y->right->parent=x;
    //把y作为p的子树
    y->parent=x->parent;

    //如果x是根节点, y成为根节点
    if(x==_root)
        _root=y;
    else if(rb_is_left_child(x)){
        x->parent->left=y;
    }
    else{
        x->parent->right=y;
    }
    y->right=x;
    x->parent=y;
}

// RB_Tree 规则:
//   1.每个结点不是红色就是黑色
//   2.根结点是黑色的
//   3.如果一个结点是红色的，则它的两个孩子结点是黑色的
//   4.对于每个结点，从该节点到其后代叶子结点的简单路径上，均包含相同数目的黑色结点
//   5.每个叶子结点都是黑色的（此处的叶子结点指的是空结点）

// 插入节点后使 rb tree 重新平衡，参数一为新增节点，参数二为根节点
// 插入节点的五种情况
// case 1: 新增节点位于根节点，令新增节点为黑
// case 2: 新增节点的父节点为黑，没有破坏平衡，直接返回
// case 3: 父节点和叔叔节点都为红，令父节点和叔叔节点为黑，祖父节点为红，
//         然后令祖父节点为当前节点，继续处理
// case 4: 父节点为红，叔叔节点为 NIL 或黑色，父节点为左（右）孩子，当前节点为右（左）孩子，
//         让父节点成为当前节点，再以当前节点为支点左（右）旋
// case 5: 父节点为红，叔叔节点为 NIL 或黑色，父节点为左（右）孩子，当前节点为左（右）孩子，
//         让父节点变为黑色，祖父节点变为红色，以祖父节点为支点右（左）旋
template<class node_ptr>
void rb_insert_fixup(node_ptr x, node_ptr& _root){
    rb_set_red(x);     //新增节点设置为红色
    //循环检查规则直到x为根节点或者x的父节点不为红色, 并处理case3，4，5
    while (x!=_root && rb_is_red(x->parent)){
        //父节点是左子结点
        if(rb_is_left_child(x->parent)){
            node_ptr uncle=x->parent->parent->right;
            //处理case3
            if(uncle!=nullptr && rb_is_red(uncle)){
                rb_set_black(x->parent);
                rb_set_black(uncle);
                x=x->parent->parent;
                rb_set_red(x);
            }
            else{
                //叔叔节点为 NIL 或黑色,  父节点是左孩子， 当前节点为右孩子， 转换成case5
                if(!rb_is_left_child(x)){
                    x=x->parent;
                    rb_rotate_left(x, _root);
                }
                //处理case5
                rb_set_black(x->parent);
                rb_set_red(x->parent->parent);
                rb_rotate_right(x->parent->parent, _root);
                break;
            }
        }
        //父节点是右子结点
        else{
            node_ptr uncle=x->parent->parent->left;
            //处理case3
            if(uncle!=nullptr && rb_is_red(uncle)){
                rb_set_black(x->parent);
                rb_set_black(uncle);
                x=x->parent->parent;
                rb_set_red(x);
            }
            else{
                //叔叔节点为 NIL 或黑色,  父节点是右孩子， 当前节点为左孩子， 转换成case5
                if(rb_is_left_child(x)){
                    x=x->parent;
                    rb_rotate_right(x, _root);
                }
                //处理case5
                rb_set_black(x->parent);
                rb_set_red(x->parent->parent);
                rb_rotate_left(x->parent->parent,_root);
                break;
            }
        }
    }
    rb_set_black(_root);
}

// 删除节点后使 rb tree 重新平衡
template<class node_ptr>
void rb_erase_fixup(node_ptr x, node_ptr x_p, node_ptr& _root){
    // case 1: 兄弟节点为红色，令父节点为红，兄弟节点为黑，进行左（右）旋，继续处理
    // case 2: 兄弟节点为黑色，且两个子节点都为黑色或 NIL，令兄弟节点为红，父节点成为当前节点，继续处理
    // case 3: 兄弟节点为黑色，左子节点为红色或 NIL，右子节点为黑色或 NIL，
    //         令兄弟节点为红，兄弟节点的左子节点为黑，以兄弟节点为支点右（左）旋，继续处理
    // case 4: 兄弟节点为黑色，右子节点为红色，令兄弟节点为父节点的颜色，父节点为黑色，兄弟节点的右子节点
    //         为黑色，以父节点为支点左（右）旋，树的性质调整完成，算法结束
    while(x!=_root && (x==nullptr||!rb_is_red(x))){
        //x为左子结点
        if(x==x_p->left){
            node_ptr bro=x_p->right;
            //把case1转换为case2，3，4中的一种
            if(rb_is_red(bro)){
                rb_set_red(x_p);
                rb_set_black(bro);
                rb_rotate_left(x_p,_root);
                bro=x_p->right;
            }
            //处理case2
            if( (bro->left==nullptr||!rb_is_red(bro->left)) && (bro->right==nullptr||!rb_is_red(bro->right)) ){
                rb_set_red(bro);
                x=x_p;
                x_p=x_p->parent;
            }
            //剩下的情况两个子结点必有一个为红色，处理case3
            else{
                if(bro->right==nullptr||!rb_is_red(bro->right)){
                    if(bro->left!=nullptr)
                        rb_set_black(bro->left);
                    rb_set_red(bro);
                    rb_rotate_right(bro, _root);
                    bro=x_p->right;
                }
                //case3转换为case4  继续处理case4
                bro->color=x_p->color;
                rb_set_black(x_p);
                if(bro->right!=nullptr)
                    rb_set_black(bro->right);
                rb_rotate_left(x_p, _root);
                break;
            }
        }
        //x为右子结点
        else{
            node_ptr bro=x_p->left;
            //把case1转换为case2，3，4中的一种
            if(rb_is_red(bro)){
                rb_set_red(x_p);
                rb_set_black(bro);
                rb_rotate_right(x_p,_root);
                bro=x_p->left;
            }
            //处理case2
            if( (bro->left==nullptr||!rb_is_red(bro->left)) && (bro->right==nullptr||!rb_is_red(bro->right)) ){
                rb_set_red(bro);
                x=x_p;
                x_p=x_p->parent;
            }
            //剩下的情况两个子结点必有一个为红色，处理case3
            else{
                if(bro->left==nullptr||!rb_is_red(bro->left)){
                    if(bro->right!=nullptr)
                        rb_set_black(bro->right);
                    rb_set_red(bro);
                    rb_rotate_left(bro, _root);
                    bro=x_p->left;
                }
                //case3转换为case4  继续处理case4
                bro->color=x_p->color;
                rb_set_black(x_p);
                if(bro->left!=nullptr)
                    rb_set_black(bro->left);
                rb_rotate_right(x_p, _root);
                break;
            }
        }
    }
    if(x!=nullptr)
        rb_set_black(x);
}

//rb tree 迭代器
template<class T>
struct my_RBtree_iterator:public mystl::iterator<mystl::bidirectional_iterator_tag, T>{
    //迭代器内嵌型别定义
    typedef T                                    my_valType;
    typedef my_valType*                          my_pointer;
    typedef my_valType&                          my_reference;
    typedef my_RBtree_node<T>*                   node_ptr;

    typedef my_RBtree_iterator<T>                iterator;
    typedef const my_RBtree_iterator<T>          const_iterator;
    typedef iterator                             self;

    //iterator 要指向一个rbtree节点
    node_ptr    node;

    //默认构造函数
    my_RBtree_iterator() noexcept {}
    //传入参数node构造函数
    my_RBtree_iterator(node_ptr x) noexcept : node(x) {}
    //拷贝构造函数
    my_RBtree_iterator(const iterator& other) noexcept : node(other.node){}

    //迭代器前进  也就是排好序后的下一个元素
    void increment(){
        if(node->right!=nullptr)
            node=rb_minimum(node->right);
        else{
            // 不存在右子树,那么分为两种情况：自底往上搜索,当前节点为父节点的左孩子的时候,父节点就是后继节点
            // 第二种情况:tem为header节点了,那么node就是最后的后继节点. 简言之node为最小节点且往上回溯,一直为父节点的右孩子,直到    变为父节点,_y为其右孩子
            node_ptr tem=node->parent;
            //找到比当前节点值更小的  最高位的节点
            while(tem->right==node){
                node=tem;
                tem=tem->parent;
            }
            //处理只有一个根节点，然后++的情况（只有一根节点，header.right==root）
            if(node->right!=tem)
                node=tem;
        }
    }
    //迭代器后退
    void decrement(){
        //header 节点返回end
        if(rb_is_red(node)&&node->parent->parent==node)
            node=node->right;
        else if(node->left!=nullptr)
            node=rb_maximum(node->left);
        else{
            node_ptr tem=node->parent;
            //找到比当前节点值更大的  最高位的节点
            while(tem->left==node){
                node=tem;
                tem=tem->parent;
            }
            node=tem;
        }
    }

    //重载引用解引用
    my_reference operator*() const noexcept { return node->val; }
    my_pointer operator->() const noexcept { return &(operator*()); }

    //重载运算符
    self& operator++(){
        increment();
        return *this;
    }
    self operator++(int){
        self tem=*this;
        increment();
        return tem;
    }
    self& operator--(){
        decrement();
        return *this;
    }
    self operator--(int){
        self tem=*this;
        decrement();
        return tem;
    }

    bool operator==(const iterator& other) const noexcept {return node==other.node;}
    bool operator!=(const iterator& other) const noexcept {return node!=other.node;}
};

template<class T, class Compare>
struct my_RBtree{
    //内嵌型别定义
    typedef my_RBtree_value_traits<T>                   value_traits;
    typedef std::size_t                                 size_type;
    typedef typename value_traits::key_type             key_type;
    typedef typename value_traits::mapped_type          mapped_type;
    typedef typename value_traits::value_type           my_valType;
    typedef Compare                                     key_compare;
    typedef my_RBtree<T,key_compare>                    my_tree;
    typedef my_RBtree_node<T>                           Node;
    typedef my_RBtree_node<T>*                          node_ptr;
    typedef my_valType*                                 pointer;
    typedef const my_valType*                           const_pointer;
    typedef my_valType&                                 reference;
    typedef const my_valType&                           const_reference;
    typedef my_RBtree_iterator<T>                       iterator;
    typedef const my_RBtree_iterator<T>                 const_iterator;
    typedef mystl::reverse_iterator<iterator>           reverse_iterator;
    typedef const mystl::reverse_iterator<iterator>     const_reverse_iterator;

    //RBtree成员
    size_type       node_count;
    node_ptr        _header;
    key_compare     comp;

    //默认初始化
    void default_init(){
        _header=new Node();
        _header->left=_header;
        _header->right=_header;
        root() = nullptr;
        node_count=0;
    }

    //重置整颗树
    void reset(){
        node_count=0;
        _header=nullptr;
    }
    //默认构造
    my_RBtree() noexcept {
        default_init();
    };
    //复制构造
    my_RBtree(const my_tree& other) noexcept
    :node_count(other.node_count),
    _header(other._header),
    comp(other.comp) {}

    //移动构造
    my_RBtree(my_tree&& other) noexcept
    :node_count(other.node_count),
    _header(other._header),
    comp(other.comp)
    {
        other.reset();
    }
    //拷贝赋值
    my_tree& operator=(const my_tree& other) noexcept {
        node_count=other.node_count;
        _header=other._header;
        comp=other.comp;
        return *this;
    }
    //移动赋值
    my_tree& operator=(my_tree&& other) noexcept {
        if(this!=&other){
            node_count=other.node_count;
            _header=other._header;
            comp=other.comp;

            other.reset();
        }
        return *this;
    }

    //返回根节点
    node_ptr& root() noexcept { return _header->parent; }
    node_ptr& leftmost() const noexcept { return _header->left; }
    node_ptr& rightmost() const noexcept { return _header->right; }
    // 迭代器相关操作
    //begin返回最小元素
    iterator                begin() noexcept {
        return iterator(_header->left);
    }
    const_iterator          begin() const noexcept {
        return const_iterator(_header->left);
    }
    iterator                cbegin() const noexcept { return begin(); }
    //end返回最大元素
    iterator                end() noexcept {
        return iterator(_header);
    }
    const_iterator          end() const noexcept {
        return const_iterator(_header);
    }
    iterator                cend() const noexcept { return end(); }

    //反向迭代器
    reverse_iterator        rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator  crbegin() const noexcept { return const_reverse_iterator(end()); }
    reverse_iterator        rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator  crend() const noexcept { return const_reverse_iterator(begin()); }

    // 容量相关操作
    bool      empty()    const noexcept { return node_count == 0; }
    size_type size()     const noexcept { return node_count; }
    size_type max_size() const noexcept { return static_cast<size_type>(-1); }

    //插入删除
    //插入节点, 允许重复值
    iterator rb_insert_multi(node_ptr node, node_ptr& _root){
        node_ptr p=nullptr;
        node_ptr cur=_root;
        //找到要插入的节点所在位置, 允许重复值
        while(cur!=nullptr){
            p=cur;
            if(comp(node->get_key(),cur->get_key()))
                cur=cur->left;
            else
                cur=cur->right;
        }
        //设置node父节点
        node->parent=p;
        //连接node父节点和node
        node->parent=p;
        if(p!=nullptr){
            if(comp(node->get_key(),p->get_key())){
                p->left=node;
                if(p==_header->left)
                    _header->left=node;
            }
            else{
                p->right=node;
                if(p==_header->right)
                    _header->right=node;
            }
        }
        else{
            _root=_header->left=_header->right=node;
            _root->parent=_header;
        }
        rb_insert_fixup(node,_root);
        ++node_count;
        return iterator(node);
    }
    //插入节点, 不允许重复值
    iterator rb_insert_unique(node_ptr node, node_ptr& _root){
        node_ptr p=nullptr;
        node_ptr cur=_root;
        //找到要插入的节点所在位置, 不允许重复值
        while(cur!=nullptr){
            p=cur;
            if(node->val==cur->val){
                std::cout<<"node already exists"<<std::endl;
                return nullptr;
            }
            if(comp(node->get_key(),cur->get_key()))
                cur=cur->left;
            else
                cur=cur->right;
        }
        //设置node父节点
        node->parent=p;
        if(p!=nullptr){
            if(comp(node->get_key(),p->get_key())){
                p->left=node;
                if(p==_header->left)
                    _header->left=node;
            }
            else{
                p->right=node;
                if(p==_header->right)
                    _header->right=node;
            }
        }
        else{
            _root=_header->left=_header->right=node;
            _root->parent=_header;
        }

        rb_insert_fixup(node,_root);
        ++node_count;
        return iterator(node);
    }

    //range插入  不允许重复值
    template<class iter>
    void rb_insert_range_unique(iter first, iter last){
        size_type n=mystl::distance(first,last);
        while(n>0){
            emplace_unique(*first);
            --n;
            ++first;
        }
    }
    //range插入  允许重复值
    template<class iter>
    void rb_insert_range_multi(iter first, iter last){
        size_type n=mystl::distance(first,last);
        while(n>0){
            emplace_multi(*first);
            --n;
            ++first;
        }
    }

    //给定插入位置在pos附近
    iterator rb_insert_unique_hint(iterator pos, node_ptr node, node_ptr& _root){
        node_ptr p=nullptr;
        node_ptr cur=pos.node;
        //从pos开始，找到要插入的节点所在位置, 不允许重复值
        while(cur!=nullptr){
            p=cur;
            if(node->val==cur->val){
                std::cout<<"node already exists"<<std::endl;
                return pos;
            }
            if(comp(node->get_key(),cur->get_key()))
                cur=cur->left;
            else
                cur=cur->right;
        }
        //设置node父节点
        node->parent=p;
        //连接node父节点和node
        if(p!=nullptr){
            if(comp(node->get_key(),p->get_key())){
                p->left=node;
                if(p==_header->left)
                    _header->left=node;
            }
            else{
                p->right=node;
                if(p==_header->right)
                    _header->right=node;
            }
        }
        else
            _root=_header->left=_header->right=node;
        rb_insert_fixup(node,_root);
        ++node_count;
        return iterator(node);
    }
    //给定插入位置在pos附近
    iterator rb_insert_multi_hint(iterator pos, node_ptr node, node_ptr& _root){
        node_ptr p=nullptr;
        node_ptr cur=pos.node;
        //从pos开始，找到要插入的节点所在位置, 不允许重复值
        while(cur!=nullptr){
            p=cur;
            if(comp(node->get_key(),cur->get_key()))
                cur=cur->left;
            else
                cur=cur->right;
        }
        //设置node父节点
        node->parent=p;
        //连接node父节点和node
        node->parent=p;
        if(p!=nullptr){
            if(comp(node->get_key(),p->get_key())){
                p->left=node;
                if(p==_header->left)
                    _header->left=node;
            }
            else{
                p->right=node;
                if(p==_header->right)
                    _header->right=node;
            }
        }
        else
            _root=_header->left=_header->right=node;
        rb_insert_fixup(node,_root);
        ++node_count;
        return iterator(node);
    }

    //emplace插入, 不允许重复值
    template <class... Args>
    iterator emplace_unique(Args&&... args){
        node_ptr newnode=new Node(mystl::forward<Args>(args)...);
        return rb_insert_unique(newnode,root());
    }
    //emplace插入, 允许重复值
    template <class... Args>
    iterator emplace_multi(Args&&... args){
        node_ptr newnode=new Node(mystl::forward<Args>(args)...);
        return rb_insert_multi(newnode,root());
    }

    //emplace_hint 给定插入位置在pos附近  不允许重复值
    template <class... Args>
    iterator emplace_hint_unique (const_iterator pos, Args&&... args){
        node_ptr newnode=new Node(mystl::forward<Args>(args)...);
        return rb_insert_unique_hint(pos, newnode, root());
    }
    //emplace_hint 给定插入位置在pos附近  允许重复值
    template <class... Args>
    iterator emplace_hint_multi (const_iterator pos, Args&&... args){
        node_ptr newnode=new Node(mystl::forward<Args>(args)...);
        return rb_insert_multi_hint(pos, newnode, root());
    }

    //删除某个节点
    void rb_erase(node_ptr z, node_ptr& _root){
        //y是可能的替换节点, 指向最终要删除的节点
        node_ptr y=(z->left==nullptr||z->right==nullptr)? z:rb_successor(z);
        // x 是 y 的一个独子节点或 nullptr
        node_ptr x= y->left==nullptr ? y->right:y->left;
        //定义x父节点
        node_ptr x_p=nullptr;
        // y != z 说明 z 有两个非空子节点，此时 y 指向 z 右子树的最左节点（y没有左子结点）  x 指向 y 的右子节点。
        // 用 y 顶替 z 的位置，用 x 顶替 y 的位置，最后用 y 指向 z
        if(y!=z){
            //y替换z, 把z的左子树作为y的左子树
            z->left->parent=y;
            y->left=z->left;
            //y不是z的右子结点, 说明y位于z右子结点的左子树末端
            if(y!=z->right){
                x_p=y->parent;
                //x替换y, y作为最左节点， 没有左子树,只有右子树x， 把x作为y->parent的左子树
                if(x!=nullptr)
                    x->parent=y->parent;
                y->parent->left=x;
                //y替换z, 把z的右子树作为y的右子树
                y->right=z->right;
                z->right->parent=y;
            }
            //y是z的右子结点， 说明z的右子结点没有左子树, 只需把z->parent的孩子换成y即可
            else{
                x_p=y;
            }

            //连接z->parent与y
            if(_root==z)
                _root=y;
            else if(rb_is_left_child(z))
                z->parent->left=y;
            else
                z->parent->right=y;
            y->parent=z->parent;
            //
            mystl::swap(y->color,z->color);
            y=z;
        }
        //y==z  y已经指向要删除的元素  说明z最多只有一个孩子   x为z的孩子， 只需把x作为替换z的节点
        else{
            x_p=y->parent;
            //x不为空，  设置x的父节点为y，z的父节点
            if(x!=nullptr)
                x->parent=y->parent;

            //连接z->parent与x
            if(_root==z)
                _root=x;
            else if(rb_is_left_child(z))
                z->parent->left=x;
            else
                z->parent->right=x;

            if(_header->left==z)
                _header->left= x==nullptr? x_p:rb_minimum(x);
            if(_header->right==z)
                _header->right= x==nullptr? x_p:rb_maximum(x);
        }

        // 此时，y 指向要删除的节点，x 为y替代节点，从 x 节点开始调整。
        // 如果删除的节点为红色，树的性质没有被破坏，否则按照以下情况调整（x 为左子节点为例）：
        if(!rb_is_red(y)){
            rb_erase_fixup(x,x_p,_root);
        }

        --node_count;
    }

    //删除某个节点以及他的子树
    void erase_since(node_ptr& x){
        if(x==nullptr)
            return;

        if(_header->left==rb_minimum(x))
            _header->left=x->parent;
        if(_header->right==rb_maximum(x))
            _header->right=x->parent;

        while (x != nullptr){
            erase_since(x->right);
            auto y = x->left;
            (&x->val)->~T();
            if(x!=nullptr)
                delete(x);
            x = y;
            --node_count;
        }
    }

    //删除值为val的节点
    size_type erase_val_unique(const key_type& val) {
        iterator it=find(val);
        if(it==end())
            return 0;
        rb_erase(it.node,root());
        return 1;
    }
    size_type erase_val_multi(const key_type& val) {
        std::pair<iterator,iterator> pa=equal_range_multi(val);
        size_type erased_num=mystl::distance(pa.first,pa.second);
        erase_range(pa.first,pa.second);
        return erased_num;
    }

    //删除range中迭代器所指节点
    void erase_range(iterator first, iterator last){
        size_type n=mystl::distance(first,last);
        iterator next=first;++next;
        while(n>0){
            rb_erase(first.node,root());
            --n;
            first=next;
            if(next!=end())
                next=++next;
        }
    }

    //删除树中所有元素
    void clear(){
        if(root()==nullptr)
            return;
        erase_since(root());
    }

    //在树中查找键值为 val 的元素，如果成功找到，则返回指向该元素的双向迭代器；反之，则返回和 end() 方法一样的迭代器
    iterator find(const key_type& val){
        node_ptr cur=root();
        while(cur!=nullptr){
            if(cur->get_key()==val)
                return iterator(cur);
            else if(comp(cur->get_key(),val))
                cur=cur->right;
            else
                cur=cur->left;
        }
        return end();
    }

    const_iterator find(const key_type& val) const {
        node_ptr cur=root();
        while(cur!=nullptr){
            if(cur->get_key()==val)
                return iterator(cur);
            else if(comp(cur->get_key(),val))
                cur=cur->right;
            else
                cur=cur->left;
        }
        return end();
    }

    //返回一个指向当前树中第一个大于或等于 val 的元素的双向迭代器
    iterator lower_bound(const key_type& val){
        node_ptr ret=_header;
        node_ptr cur=root();
        while(cur!=nullptr){
            if(comp(cur->get_key(),val)){
                cur=cur->right;
            }
            else{
                ret=cur;
                cur=cur->left;
            }
        }
        return iterator(ret);
    }

    const_iterator lower_bound(const key_type& val) const {
        node_ptr ret=_header;
        node_ptr cur=root();
        while(cur!=nullptr){
            if(comp(cur->get_key(),val)){
                cur=cur->right;
            }
            else{
                ret=cur;
                cur=cur->left;
            }
        }
        return iterator(ret);
    }

    //返回一个指向当前树中第一个大于 val 的元素的双向迭代器
    iterator upper_bound(const key_type& val){
        node_ptr ret=_header;
        node_ptr cur=root();
        while(cur!=nullptr){
            if(comp(val,cur->get_key())){
                ret=cur;
                cur=cur->left;
            }
            else{
                cur=cur->right;
            }
        }
        return iterator(ret);
    }

    const_iterator upper_bound(const key_type& val) const {
        node_ptr ret=_header;
        node_ptr cur=root();
        while(cur!=nullptr){
            if(comp(val,cur->get_key())){
                ret=cur;
                cur=cur->left;
            }
            else{
                cur=cur->right;
            }
        }
        return const_iterator(ret);
    }

    //计算树中值为val的元素有多少个
    size_type count_unique(const key_type& val) noexcept { return find(val)==end()? 0:1; }
    size_type count_multi(const key_type& val) noexcept {
        std::pair<iterator,iterator> pa=equal_range_multi(val);
        return size_type(mystl::distance(pa.first,pa.second));
    }


    //equal_range 该方法返回一个 pair 对象 pair.first 和 lower_bound 方法的返回值等价，pair.second 和 upper_bound 方法的返回值等价
    std::pair<iterator,iterator>             equal_range_unique(const key_type& val){
        iterator it=lower_bound(val);
        iterator next=it;
        ++next;
        return next==end()? mystl::make_pair(it,it):mystl::make_pair(it,next);
    }

    std::pair<const_iterator,const_iterator> equal_range_unique(const key_type& val) const {
        iterator it=lower_bound(val);
        iterator next=it;
        ++next;
        return next==end()? mystl::make_pair(it,it):mystl::make_pair(it,next);
    }

    std::pair<iterator,iterator>             equal_range_multi(const key_type& val){
        return mystl::make_pair(lower_bound(val),upper_bound(val));
    }

    std::pair<const_iterator,const_iterator> equal_range_multi(const key_type& val) const {
        return mystl::make_pair(lower_bound(val),upper_bound(val));
    }

    //交换两棵树
    void swap(my_tree& other) noexcept {
        if(this==&other)
            return;
        mystl::swap(root(), other.root());
        mystl::swap(node_count, other.node_count);
        mystl::swap(_header->left, other._header->left);
        mystl::swap(_header->right, other._header->right);
    }

    //

};

#endif
