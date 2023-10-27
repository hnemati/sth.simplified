#ifndef _MINIC_TYPES_H_
#define _MINIC_TYPES_H_

/* this is usefull when testing: dont hide internal functions when TEST */
#ifdef TEST
 #define PRIVATE /* NOTHING */
#else
 #define PRIVATE static
#endif


typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define BOOL int
#define FALSE 0
#define TRUE 1
#define NULL 0

#define BASE_REG volatile uint32_t *



typedef enum return_value_ {
    RV_OK = 0,
    RV_BAD_ARG = -127,
    RV_BAD_PERM,
    RV_BAD_OTHER
} return_value;


/* data types */
typedef return_value (*cpu_callback)(uint32_t, uint32_t, uint32_t);


/* size = 2 dword */
typedef struct {
    uint32_t size, size_prev;
} heap_marker_t;

typedef struct {
    uint32_t size, end;
    uint32_t user; /* user data, could fore example be a lock */
    heap_marker_t *memory;
} heap_ctxt_t;


#endif /* _MINIC_TYPES_H_ */
