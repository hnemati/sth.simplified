#include "hw.h"
#include "soc_interrupt.h"

cpu_callback tick_handler = 0;
void tick_handler_stub(uint32_t r0, uint32_t r1, uint32_t r2)
{
    
    /* TODO */
#if 0    
    /* ack the timer interrupt */    
    timer_registers *timer = (timer_registers *) TIMER_BASE;
    timer->channels[1].sr; /* this ack the irq */
    /* call tick handler */
    if(tick_handler)
        tick_handler(r0, r1, r2);
#endif    
}

/*
 * use timer 1 for system tick.
 * the reason we use this one is because the other
 * aren't working in OVP...
 */
void timer_tick_start(cpu_callback handler)
{
    /* TODO */
}

void timer_tick_stop()
{
    /* TODO */
}

void soc_timer_init()
{
    /* TODO */
}
