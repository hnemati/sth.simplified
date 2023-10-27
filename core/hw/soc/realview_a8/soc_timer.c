
#include "hw.h"

#include "soc.h"
#include "soc_private.h"

/* ARM Dual-Timer SP804 r1p2 */

#define BASE_TIMER0 0x10011000
#define IRQ_TIMER0 36

#define TIMER_LOAD    0
#define TIMER_VALUE	  1
#define TIMER_CONTROL 2
#define TIMER_INTCLR  3
#define TIMER_MASKED  5

cpu_callback tick_handler = 0;

static void tick_handler_stub(uint32_t r0, uint32_t r1, uint32_t r2)
{
    volatile uint32_t *timer = (uint32_t *) BASE_TIMER0;
    timer[TIMER_INTCLR] = 1;

    if(tick_handler)
        tick_handler(r0, r1, r2);

    // printf("%d %d %x\n", timer[TIMER_LOAD], timer[TIMER_VALUE], timer[TIMER_CONTROL]);

}


void timer_tick_start(cpu_callback handler)
{
    volatile uint32_t *timer = (uint32_t *) BASE_TIMER0;

    tick_handler = handler;
    timer[TIMER_LOAD] = 2000;
    timer[TIMER_VALUE] = 2000;
    timer[TIMER_CONTROL] = 0xE0;
}

void soc_timer_init()
{
    volatile uint32_t *timer = (uint32_t *) BASE_TIMER0;
    timer[TIMER_CONTROL] = 0;

    cpu_irq_set_handler(IRQ_TIMER0, (cpu_callback)tick_handler_stub);
    cpu_irq_set_config(IRQ_TIMER0, 6);
    cpu_irq_set_enable(IRQ_TIMER0, TRUE);
}
