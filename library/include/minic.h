
#ifndef _MINIC_LIB_H_
#define _MINIC_LIB_H_

#include <minic_types.h>


extern void printf(char *fmt, ...);
extern void *memcpy(void *dst, void const *src, int len);
extern void * memset ( void * ptr, int value, int num );

extern void heap_init(heap_ctxt_t *heap, uint32_t size, void *memory);
extern void *heap_alloc(heap_ctxt_t *heap, int size);
extern int heap_free(heap_ctxt_t *heap, void *adr);


#endif /* _LIB_H_ */
