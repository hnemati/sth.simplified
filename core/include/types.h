#ifndef _TYPES_H_
#define _TYPES_H_

#include <minic_types.h>


/* page table definition */
#define PAGE_BITS 20
#define PAGE_SIZE (1UL << PAGE_BITS)
#define PAGE_MASK (PAGE_SIZE-1)

#define ADR_TO_PAGE(adr) ((adr) >> PAGE_BITS)
#define PAGE_TO_ADR(page) ((page) << PAGE_BITS)


/* cpu type */
typedef enum cpu_type_ {
    CT_ARM
} cpu_type;

typedef enum cpu_model_ {
    CM_ARMv5 = 0,
    CM_ARMv6,
    CM_ARMv7a

} cpu_model;

/* memory layout */
typedef struct {
    uint32_t page_start;
    uint32_t page_end;
    uint32_t type;
    uint32_t flags;
} memory_layout_entry;

typedef enum memory_layout_type_ {
    MLT_NONE = 0,
    MLT_HYPER_RAM,
    MLT_HYPER_ROM,
    MLT_USER1_RAM,
    MLT_USER2_RAM,
    MLT_USER_ROM,
    MLT_IO_REG,

} memory_layout_type;

typedef enum memory_layout_flags_ {
    MLF_LAST = (1 << 0),
    MLF_READABLE = (1 << 2),
    MLF_WRITEABLE = (1 << 3)
} memory_layout_flags;

#endif /* _TYPES_H_ */
