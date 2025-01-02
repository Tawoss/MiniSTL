#ifndef TEST_H
#define TEST_H
#include "../src/vector.h"
#include <gtest/gtest.h>
//#include <vector>
TEST(VectorTest, ConstructorTest){
    int a[5] = {1,2,3,4,5};
    //构造函数  test
    my_vector<int> v1;
    EXPECT_EQ(v1.size(), 0);
    my_vector<int> v2(10);
    EXPECT_EQ(v2.size(), 10);
    EXPECT_EQ(*v2.begin(), 0);
    my_vector<int> v3(size_t(10), 1);
    EXPECT_EQ(v3.size(), 10);
    EXPECT_EQ(*v3.begin(), 1);
    my_vector<int> v4(a, a + 5);
    EXPECT_EQ(v4.size(), 5);
    EXPECT_EQ(*v4.begin(), 1);
    EXPECT_EQ(v4[4], 5);
    my_vector<int> v5(v2);
    EXPECT_EQ(v5.size(), 10);
    my_vector<int> v6(mystl::move(v2));
    EXPECT_EQ(v6.size(), 10);
    EXPECT_EQ(v2.size(), 0);
    my_vector<int> v7{ 1,2,3,4,5,6,7,8,9 };
    EXPECT_EQ(v7.size(), 9);
    EXPECT_EQ(*v7.begin(), 1);
    EXPECT_EQ(*(v7.end()-1), 9);
}
TEST(VectorTest, AssignmentTest){
    my_vector<int> v3(size_t(10), 1);
    //opearator=  test
    my_vector<int> v8, v9, v10;
    v8 = v3;
    EXPECT_EQ(v8.size(), 10);
    EXPECT_EQ(*(v8.end()-1), 1);
    v9 = mystl::move(v3);
    EXPECT_EQ(v9.size(), 10);
    EXPECT_EQ(*(v9.end()-1), 1);
    EXPECT_EQ(v3.size(), 0);
    v10 = {1,2,3,4,5};
    EXPECT_EQ(v10.size(), 5);
    EXPECT_EQ(*v10.begin(), 1);
    EXPECT_EQ(*(v10.end()-1), 5);
}
TEST(VectorTest, BeginEndTest){
    my_vector<int> v10{1,2,3,4,5};
    //data begin rbegin
    EXPECT_EQ(*v10.data(),1);
    EXPECT_EQ(*v10.begin(),1);
    EXPECT_EQ(*v10.rbegin(),5);
    //end rend
    EXPECT_EQ(*(v10.end()-1),5);
    EXPECT_EQ(*(v10.rend()-1),1);
}
TEST(VectorTest, ResizeTest){
    my_vector<int> v10{1,2,3,4,5};
    //resize
    v10.resize(6,8);
    EXPECT_EQ(v10[5],8);
    v10.resize(3,3);
    EXPECT_EQ(v10.size(),3);
    EXPECT_EQ(v10[2],3);
}
TEST(VectorTest, EmptyTest){
    my_vector<int> v10{1,2,3};
    my_vector<int> emp;
    //empty
    EXPECT_TRUE(emp.empty());
    EXPECT_FALSE(v10.empty());
}
TEST(VectorTest, ReserveShrinkToFitTest){
    my_vector<int> v10{1,2,3,4,5};
    //reserve  capacity
    size_t c=v10.capacity();
    size_t new_c=2*c;
    v10.reserve(new_c);
    EXPECT_EQ(v10.capacity(),new_c);
    EXPECT_GT(v10.capacity(),v10.size());
    //shrink_to_fit
    v10.shrink_to_fit();
    EXPECT_EQ(v10.size(),v10.capacity());
}
TEST(VectorTest, AtFrontBackTest){
    my_vector<int> v10{1,2,3,4,5};
    //at
    EXPECT_EQ(v10.at(0),*v10.begin());
    //front back
    EXPECT_EQ(v10.front(),*v10.begin());
    EXPECT_EQ(v10.back(),*v10.rbegin());
}
TEST(VectorTest, AssignTest){
    my_vector<int> v10;
    //assign
    std::initializer_list<int> t{1,2,3,4,5,6,7};
    v10.assign(t.begin(),t.end());
    EXPECT_EQ(v10.size(),7);
    EXPECT_EQ(*v10.begin(),1);
    EXPECT_EQ(*v10.rbegin(),7);
}
TEST(VectorTest, PushPopBackTest){
    my_vector<int> v10{1,2,3,4,5,6,7};
    //push_back   pop_back
    v10.pop_back();
    EXPECT_EQ(v10.size(),6);
    EXPECT_EQ(*v10.rbegin(),6);
    v10.push_back(10);
    v10.push_back(11);
    EXPECT_EQ(v10.size(),8);
    EXPECT_EQ(*v10.rbegin(),11);
    EXPECT_EQ(*(v10.rbegin()+1),10);
}
TEST(VectorTest, InsertTest){
    my_vector<int> v10{1,2,3,4,5};
    //在指定位置出插入1个值为val的元素
    auto it=v10.begin()+2;
    v10.insert(it, 50);
    EXPECT_EQ(v10.size(),6);
    EXPECT_EQ(v10[2],50);
    //在指定位置插入多个值为val的元素
    it=v10.begin()+2;
    v10.insert(it,size_t(3),20);
    EXPECT_EQ(v10.size(),9);
    EXPECT_EQ(v10[2],20);
    EXPECT_EQ(v10[4],20);
    EXPECT_EQ(v10[5],50);
    //在指定位置插入range中的元素
    int ia[5]={11,12,13,14,15};
    it=v10.begin()+2;
    v10.insert(it,ia,ia+5);
    EXPECT_EQ(v10.size(),14);
    EXPECT_EQ(v10[2],11);
    EXPECT_EQ(v10[6],15);
    EXPECT_EQ(v10[7],20);
    for(int i=0;i<10000;++i)
        v10.insert(v10.cbegin(),i);
}
TEST(VectorTest, EmplaceTest){
    my_vector<int> v10{1,2,3,4,5};
    //在指定位置出插入1个值为val的元素
    auto it=v10.begin()+2;
    v10.emplace(it,10);
    EXPECT_EQ(v10.size(),6);
    EXPECT_EQ(v10[2],10);
    for(int i=0;i<10000;++i)
        v10.emplace(v10.cbegin(),5);
}
TEST(VectorTest, EraseTest){
    my_vector<int> v10{1,2,3,4,5};
    //删除指定位置的元素
    auto it=v10.begin();
    v10.erase(it);
    EXPECT_EQ(v10.size(),4);
    EXPECT_EQ(v10.at(0),2);
    //删除两个迭代器表示的range中的元素
    it=v10.begin(); ++it;
    auto it2=v10.begin(); ++it2;++it2;++it2;
    v10.erase(it,it2);
    EXPECT_EQ(v10.size(),2);
    EXPECT_EQ(v10.at(0),2);
    EXPECT_EQ(v10.at(1),5);
}
TEST(VectorTest, ClearTest){
    my_vector<int> v10{1,2,3,4,5};
    v10.clear();
    EXPECT_EQ(v10.size(),0);
}
TEST(VectorTest, SwapTest){
    my_vector<int> v10{1,2,3,4,5};
    my_vector<int> v11{7};
    v10.swap(v11);
    EXPECT_EQ(v10.size(),1);
    EXPECT_EQ(v11.size(),5);
}
#endif
