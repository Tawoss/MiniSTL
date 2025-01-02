#ifndef ALGOBASE_H
#define ALGOBASE_H


#include <iostream>
#include "iterator.h"
#include "mystring.h"
#include <cstring>
#include "type_traits.h"

namespace mystl{

//遍历判断迭代器是否相等
template<class iter1, class iter2>
bool equal(iter1 first1, iter1 last1, iter2 first2){
	for(;first1!=last1;++first1,++first2){
		if(*first1!=*first2)
			return false;
	}
	return true;
}
template<class iter1, class iter2>
bool equal(iter1 first1, iter1 last1, iter2 first2, iter2 last2){
	for(;first1!=last1&&first2!=last2;++first1,++first2){
		if(*first1!=*first2)
			return false;
	}
	if(first1!=last1||first2!=last2)
		return false;
	return true;
}

//允许指定p作为比较依据，取代元素型别提供的小于操作符
template<class iter1, class iter2, class BinaryPredicate>
bool equal(iter1 first1, iter1 last1, iter2 first2, BinaryPredicate p){
	for(;first1!=last1;++first1,++first2){
		if(!p(*first1,*first2))
			return false;
	}
	return true;
}

//fill将first～last间的值替换为val
template<class iter, class T>
void fill(iter first, iter last, const T& val){
	for(;first!=last;++first)
		*first=val;
}
//fill_n在first修改后面n个元素的值为val
template<class iter, class size, class T>
iter fill_n(iter first, size n, const T& val){
	for(;n>0;--n,++first)
		*first=val;
	return first;
}

//iter_swap交换两个迭代器的元素
template<class iter1, class iter2>
inline void iter_swap(iter1 a, iter2 b){
	__iter_swap(a,b,value_type(a));
}
//使用iterator_traits中的value_type()获取迭代器的数据类型
template<class iter1, class iter2, class T>
inline void __iter_swap(iter1 a, iter2 b, T*){
	T temp=*a;
	*a=*b;
	*b=temp;
}

//返回两个变量中最大、最小值
template<class T>
inline const T& max(const T& a, const T& b){
	return a<b?b:a;
}
template<class T>
inline const T& min(const T& a, const T& b){
	return a<b?a:b;
}
//comp重载小于运算符
template<class T, class Compare>
inline const T& max(const T& a, const T& b, Compare comp){
	return comp(a,b)?b:a;
}
template<class T, class Compare>
inline const T& min(const T& a, const T& b, Compare comp){
	return comp(a,b)?a:b;
}

//mismatch返回两个序列的第一个不匹配点，返回一对迭代器
template<class iter1, class iter2>
std::pair<iter1,iter2> mismatch(iter1 first1, iter1 last1, iter2 first2){
	while(first1!=last1 && *first1==*first2){
		++first1;
		++first2;
	}
	return std::pair<iter1,iter2>(first1,first2);
}
//指定p作为比较标准
template<class iter1, class iter2, class BinaryPredicate>
std::pair<iter1,iter2> mismatch(iter1 first1, iter1 last1, iter2 first2, BinaryPredicate p){
	while(first1!=last1 && p(*first1,*first2)){
		++first1;
		++first2;
	}
	return std::pair<iter1,iter2>(first1,first2);
}

// lexicographical_compare
// 以字典序排列对两个序列进行比较，当在某个位置发现第一组不相等元素时，有下列几种情况：
// (1)如果第一序列的元素较小，返回 true ，否则返回 false
// (2)如果到达 last1 而尚未到达 last2 返回 true
// (3)如果到达 last2 而尚未到达 last1 返回 false
// (4)如果同时到达 last1 和 last2 返回 false
template <class iter1, class iter2>
bool lexicographical_compare(iter1 first1, iter1 last1,
                             iter2 first2, iter2 last2)
{
  for (; first1 != last1 && first2 != last2; ++first1, ++first2)
  {
    if (*first1 < *first2)
      return true;
    if (*first2 < *first1)
      return false;
  }
  return first1 == last1 && first2 != last2;
}
//允许指定p作为比较依据，取代元素型别提供的小于操作符
template <class iter1, class iter2, class BinaryPredicate>
bool lexicographical_compare(iter1 first1, iter1 last1,
                             iter2 first2, iter2 last2, BinaryPredicate p)
{
  for (; first1 != last1 && first2 != last2; ++first1, ++first2)
  {
    if (p(*first1, *first2))
      return true;
    if (p(*first2, *first1))
      return false;
  }
  return first1 == last1 && first2 != last2;
}
//针对原声指针const unsigned char*的特化版lexicographical_compare
inline bool lexicographical_compare(const unsigned char* first1,
									const unsigned char* last1,
									const unsigned char* first2,
									const unsigned char* last2)
{
	const std::size_t len1=last1-first1;
	const std::size_t len2=last2-first2;

	const int result=std::memcmp(first1,first2,mystl::min(len1,len2));
	//result==0 first1和first2前n个字节相同，   result<0 first1<first2，   result>0 first1>first2
	return result!=0?result<0:len1<len2;
}

//swap交换两个变量的值
template<class T>
inline void swap(T& a, T& b){
    T temp=a;
    a=b;
    b=temp;
}

//****************************************copy()**********************************************

//__copy_match_n定义random access iterator的copy方式， 以iterator 的distance决定循环执行的次数
template<class inputiter,class outputiter, class distance>
inline outputiter __copy_match_n(inputiter first, inputiter last, outputiter result, distance*){
	//以迭代器之间的距离n决定循环执行的次数，速度比判断迭代器是否相等更快
	for(distance n=last-first;n>0;--n,++first,++result)
		*result=*first;
	return result;
}

//__copy_match_t定义 指针所指对象具备trivial assignment operator的copy方式
template<class T>
inline T* __copy_match_t(const T* first, const T* last, T* result, __true_type){
	std::memmove(result,first,sizeof(T)*(last-first));
	return result;
}

//__copy_match_t定义 指针所指对象具备non-trivial assignment operator的copy方式
template<class T>
inline T* __copy_match_t(const T* first, const T* last, T* result, __false_type){
	//虽然不具备trivial assignment operator，但是原生指针依然是random access iterator， 派送给copy_match_n处理
	return __copy_match_n(first,last,result,distance_type(first));
}

//__copy_match 的InputIterator版本  识别input_iterator_tag
template<class inputiter,class outputiter>
inline outputiter __copy_match(inputiter first, inputiter last, outputiter result, input_iterator_tag){
	//通过判断迭代器是否相同来决定循环是否继续，较慢
	for(;first!=last;++first,++result)
		*result=*first;
	return result;
}

//__copy_match 的RandomAccessIterator版本  识别random_access_iterator_tag
template<class inputiter,class outputiter>
inline outputiter __copy_match(inputiter first, inputiter last, outputiter result, random_access_iterator_tag){
	return __copy_match_n(first,last,result,distance_type(first));
}

//_copy_dispatch决定要向下执行的函数，这里是完全泛化版本
template<class inputiter,class outputiter>
struct _copy_dispatch{
	outputiter operator()(inputiter first, inputiter last, outputiter result){
		return __copy_match(first,last,result,iterator_category(first));
	}
};

//_copy_dispatch偏特化版本，第一个参数是const T*， 第二个是T*
template<class T>
struct _copy_dispatch<const T*,T*>{
	T* operator()(const T* first, const T* last, T* result){
		typedef typename __type_traits<T>::has_trivial_assignment_operator t;
		return __copy_match_t(first,last,result,t());
	}
};

//_copy_dispatch偏特化版本，两个参数都是T*
template<class T>
struct _copy_dispatch<T*,T*>{
	T* operator()(T* first, T* last, T* result){
		typedef typename __type_traits<T>::has_trivial_assignment_operator t;
		return __copy_match_t(first,last,result,t());
	}
};

//copy函数调用copy_dispatch来进一步决定调用什么函数
template<class inputiter,class outputiter>
inline outputiter copy(inputiter first, inputiter last, outputiter result){
	return _copy_dispatch<inputiter,outputiter>()(first,last,result);
}

//copy函数针对const char* 特化版
inline char* copy(const char* first, const char* last, char* result){
	std::memmove(result,first,(last-first));
	return result+(last-first);
}

//copy函数针对const wchar_t* 特化版
inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result){
	std::memmove(result,first,sizeof(wchar_t)*(last-first));
	return result+(last-first);
}


//*****************************************copy_backward()****************************************//

//__copy_backward_match_n定义random access iterator的copy方式， 以iterator 的distance决定循环执行的次数
template<class inputiter,class outputiter, class distance>
inline outputiter __copy_backward_match_n(inputiter first, inputiter last, outputiter result, distance*){
	//std::cout<<"        __copy_backward_match_n()"<<std::endl;
	//以迭代器之间的距离n决定循环执行的次数，速度比判断迭代器是否相等更快
	for(distance n=last-first;n>0;--n)
		*--result=*--last;
	return result;
}

//__copy_backward_match_t定义 指针所指对象具备trivial assignment operator的copy_backward方式
template<class T>
inline T* __copy_backward_match_t(const T* first, const T* last, T* result, __true_type){
	//std::cout<< "        _copy_backward_match_t(trivial assignment operator)"<<std::endl;
	std::memmove(result-(last-first),first,sizeof(T)*(last-first));
	return result;
}

//__copy_backward_match_t定义 指针所指对象具备non-trivial assignment operator的copy_backward方式
template<class T>
inline T* __copy_backward_match_t(const T* first, const T* last, T* result, __false_type){
	//std::cout<< "        _copy_backward_match_t(non-trivial assignment operator)"<<std::endl;
	//虽然不具备trivial assignment operator，但是原生指针依然是random access iterator， 派送给copy_backward_match_n处理
	return __copy_backward_match_n(first,last,result,distance_type(first));
}

//__copy_backward_match 的InputIterator版本  识别input_iterator_tag
template<class inputiter,class outputiter>
inline outputiter __copy_backward_match(inputiter first, inputiter last, outputiter result, bidirectional_iterator_tag){
	//std::cout<<"        __copy_backward_match(bidirectional_iterator_tag)"<<std::endl;
	//通过判断迭代器是否相同来决定循环是否继续，较慢
	while(first!=last)
		*--result=*--first;
	return result;
}

//__copy_backward_match 的RandomAccessIterator版本  识别random_access_iterator_tag
template<class inputiter,class outputiter>
inline outputiter __copy_backward_match(inputiter first, inputiter last, outputiter result, random_access_iterator_tag){
	//std::cout<<"        __copy_backward_match(random_access_iterator_tag)"<<std::endl;
	return __copy_backward_match_n(first,last,result,distance_type(first));
}

//_copy_backward_dispatch完全泛化版本
template<class inputiter, class outputiter>
struct _copy_backward_dispatch{
	outputiter operator()(inputiter first, inputiter last, outputiter result){
		//std::cout<< "    _copy_backward_dispatch()"<<std::endl;
		return __copy_backward_match(first,last,result,iterator_category(first));
	}
};

//_copy_backward_dispatch偏特化版本，两个参数都是T*
template<class T>
struct _copy_backward_dispatch<T*,T*>{
	T* operator()(T* first, T* last, T* result){
		//std::cout<< "    _copy_backward_dispatch(T*, T*)"<<std::endl;
		typedef typename __type_traits<T>::has_trivial_assignment_operator t;
		return __copy_backward_match_t(first,last,result,t());
	}
};

//_copy_backward_dispatch偏特化版本，第一个参数是const T*， 第二个是T*
template<class T>
struct _copy_backward_dispatch<const T*,T*>{
	T* operator()(T* first, T* last, T* result){
		//std::cout<< "    _copy_backward_dispatch(const T*, T*)"<<std::endl;
		typedef typename __type_traits<T>::has_trivial_assignment_operator t;
		return __copy_backward_match_t(first,last,result,t());
	}
};

//copy_backward函数调用copy_backward_dispatch来进一步决定调用什么函数
template<class inputiter, class outputiter>
inline outputiter copy_backward(inputiter first, inputiter last, outputiter result){
	//检查是否是双向迭代器
	// if(iterator_category(first)!=bidirectional_iterator_tag||iterator_category(result)!=bidirectional_iterator_tag)
	// 	std::__throw_invalid_argument("iterator need to be bidirectional iterator in copy_backward()");
	//std::cout<<"copy_backward()"<<std::endl;
	return _copy_backward_dispatch<inputiter,outputiter>()(first,last,result);
}

//copy_backward函数针对const char* 特化版
inline char* copy_backward(const char* first, const char* last, char* result){
	//std::cout<< "_copy_backward(const char*)"<<std::endl;
	std::memmove(result-(last-first),first,(last-first));
	return result-(last-first);
}

//copy_backward函数针对const wchar_t* 特化版
inline wchar_t* copy_backward(const wchar_t* first, const wchar_t* last, wchar_t* result){
	//std::cout<< "_copy_backward(const wchar_t*)"<<std::endl;
	std::memmove(result-(last-first),first,sizeof(wchar_t)*(last-first));
	return result-(last-first);
}

}

#endif

