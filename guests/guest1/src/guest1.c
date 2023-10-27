#include "hyper.h"

void handler(void ) __attribute__ ((section ("HANDLERSC")));

int x;

void _main(void) {
	x = 0;
    // printf("Enter guest1\n");
	while (1) {
		x += 1;
		asm("mov  r0, %[value] \n\t"
	    	:
			:[value] "r" (x)/*input*/
	        : /* No clobbers */);
		ISSUE_HYPERCALL(HYPERCALL_MESSAGE);
		//asm("swi #0x1");
	}
}

void handler(void){
	asm("mov  %[result],r0 \n\t"
    	:[result] "=r" (x)
		: /*input*/
        : /* No clobbers */);
	ISSUE_HYPERCALL(HYPERCALL_RELEASE);
	//asm("swi #0x0");
}
