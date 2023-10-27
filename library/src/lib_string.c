
/*
 * unoptimized libc functions
 * 
 * NOTE: these functions are all untested!
 * 
 */

#include <minic.h>

void *memcpy(void *dst, void const *src, int len)
{
    char * pDst = (char *) dst;
    char const * pSrc = (char const *) src;
    while (len--)
    {
        *pDst++ = *pSrc++;
    }
    return (dst);    
}

void * memset ( void * ptr, int value, int num )
{
    char * tmp = (char *) ptr;
    while(num--)
        *tmp++ = value;
    return ptr;
}
