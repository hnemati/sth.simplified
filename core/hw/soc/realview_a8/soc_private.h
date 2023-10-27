
#ifndef _SOC_PRIVATE_H_
#define _SOC_PRIVATE_H_

extern void soc_uart_init();
extern void soc_timer_init();
extern void soc_irq_init();
extern void soc_init();


extern void soc_interrupt_set_configuration(int number, int priority);


#endif /* _SOC_PRIVATE_H_ */
