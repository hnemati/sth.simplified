
#include "types.h"
#include "soc_interrupt.h"
#include "lib.h"

#define IRQ_COUNT 32

cpu_callback interrupt_handlers[IRQ_COUNT];

static void default_handler(uint32_t r0, uint32_t r1, uint32_t r2)
{
   printf("DEFAULT INTERRUPT HANDLER %x:%x:%x\n", r0, r1, r2);
}

/* 
 */
int cpu_irq_get_count()
{
    return IRQ_COUNT;
}
void cpu_irq_set_enable(int number, BOOL enable)
{
    aic_registers *aic = (aic_registers *) AIC_BASE;
    if(number < 0 || number >= IRQ_COUNT) return;
    
    if(enable)
        aic->iecr = (1UL << number);
    else 
        aic->idcr = (1UL << number);
    
    /* OVP fix */
    aic->iecr = 0;    
    aic->idcr = 0;    
}
void cpu_irq_set_handler(int number, cpu_callback handler)
{
    aic_registers *aic = (aic_registers *) AIC_BASE;
    if(number < 0 || number >= IRQ_COUNT) return;
    
    if(!handler)
        handler = (cpu_callback)default_handler;
    
    aic->svr[number] = (uint32_t) handler;
}

void cpu_irq_acknowledge(int number)
{
    aic_registers *aic = (aic_registers *) AIC_BASE;
    if(number < 0 || number >= IRQ_COUNT) return;
    aic->eoicr = (1UL << number);
}

void soc_interrupt_set_configuration(int number, int priority, 
                                     BOOL polarity,
                                     BOOL level_sensetive)
{
    aic_registers *aic = (aic_registers *) AIC_BASE;
    uint32_t tmp;
    if(number < 0 || number >= IRQ_COUNT) return;
    
    tmp = priority & 7;
    if(polarity) tmp |= (1UL << 6);
    if(!level_sensetive) tmp |= (1UL << 5);
    
    aic->smr[number] = tmp;
    
}

void cpu_irq_get_current()
{
    /* TODO */
}


void soc_interrupt_init()
{
    int i;
    
    /* disable and clear interrupts */
    aic_registers *aic = (aic_registers *) AIC_BASE;
    aic->idcr = -1; /* disable all */
    aic->iccr = -1; /* clear all */
    
    /* disable each individual interrupt and set the default handler */
    for(i = 0; i < cpu_irq_get_count(); i++) {
        cpu_irq_set_enable(i, FALSE);
        cpu_irq_set_handler(i, (cpu_callback)default_handler);
    }
    
    /* TODO: set relocated interrupt base */
}

