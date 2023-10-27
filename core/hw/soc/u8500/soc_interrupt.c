
#include <hw.h>

#if 0

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
    /* TODO */
}

void cpu_irq_get_current()
{
    /* TODO */
}

#endif

void soc_interrupt_init()
{
    /* relocate the exception vector vector */
    extern uint32_t _interrupt_vector_table;
    COP_WRITE(COP_SYSTEM, COP_SYSTEM_RELOCATE_EXCEPTION_VECTOR, &_interrupt_vector_table);
}

