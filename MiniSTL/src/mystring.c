void* mymemcopy(void* des, const void* src, std::size_t n){
    char* pdes=(char*)des;
    char* psrc=(char*)src;
    void* ret=(char*)des;
    while(n>0){
        *pdes=*psrc;
        ++psrc;
        ++pdes;
        --n;
    }
    return ret;
}

void* mymemmove(void* des, const void* src, std::size_t n){
    char* pdes=(char*)des;
    char* psrc=(char*)src;
    void* ret=des;
    if (src > des)
	{
		//顺顺序
		while (n--)
		{
			*pdes = *psrc;
			pdes = pdes + 1;
			psrc = psrc + 1;
		}
	}
	else
	{
		//逆顺序
		while (n--)
		{
			*(pdes+n) = *(psrc + n);
		}
	}
    return ret;
}

int mymemcomp(const void* iter1, const void* iter2, std::size_t n){
    if(!n) return 0;
    while(n--&&*(char*)iter1==*(char*)iter2){
        iter1=(unsigned char*)iter1+1;
        iter2=(unsigned char*)iter2+1;
    }
    return *(unsigned char*)iter1 - *(unsigned char*)iter2;
}
