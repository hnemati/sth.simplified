
/*
 * @OPTION -fno-builtin-memcpy 
 * @NEED lib_string.c 
 */


// #include <stdio.h>

#include <lib.h>
#include "test.h"

#define SIZE 100

#undef memcpy

void test(test_context_t *ctx)
{
    int i, j, k;
    char buffer1[SIZE];
    char buffer2[SIZE];
    

    test_group_start(ctx, "memcpy");
    {
        for(i = 0; i < SIZE; i++) {
            /* fill buffer with known data: */
            for(j = 0; j < SIZE; j++) {
                buffer1[j] = j | 0x10000;
                buffer2[j] = j | 0x20000;
            }
            
            /* do a copy */
            memcpy(buffer1, buffer2, i);
            
            /* now first i bytes of buffer1 should contain data from buffe 2 */
            for(j = 0; j < SIZE; j++) {
                if(j < i)
                    test_equal(ctx, "memcpy got good data", buffer1[j], buffer2[j]);
                else                    
                    test_equal(ctx, "memcpy didn't touch other data", buffer1[j], j | 0x10000);
            }                    
                    
        }
       
    }
    test_group_end(ctx);    
}
