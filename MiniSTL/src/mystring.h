#ifndef MYSTRING_H
#define MYSTRING_H

extern void* mymemcopy(void* des, const void* src, std::size_t n);

extern void* mymemmove(void* des, const void* src, std::size_t n);

extern int mymemcomp(const void* iter1, const void* iter2, std::size_t n);

#endif
