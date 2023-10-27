
#include <types.h>
#include <hw.h>

/* this should have been declared in board code */

extern memory_layout_entry * memory_padr_layout;

memory_layout_entry *mem_padr_lookup(uint32_t padr)
{
    memory_layout_entry * tmp = (memory_layout_entry*)(&memory_padr_layout);
    uint32_t page = ADR_TO_PAGE(padr);


    for(;;) {
        if(!tmp) break;

        if(page >= tmp->page_start && page <= tmp->page_end) return tmp;
        if(tmp->flags & MLF_LAST) break;
        tmp++;
    }
    return 0; /* not found */
}
