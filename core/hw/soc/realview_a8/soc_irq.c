#include "hw.h"

#include "soc.h"
#include "soc_private.h"

#define IRQ_COUNT 96
#define BASE_GIC0 0x1E000000

#define CPU_CONTROL (0x0000 / 4)
#define CPU_PRI_MSK (0x0004 / 4) /* priority mask */
#define CPU_BPR     (0x0008 / 4) /* binary point */
#define CPU_ICCIAR	(0x000c / 4) /* int ack */
#define CPU_ICCEOIR (0x0010 / 4) /* end of interrupt */
#define CPU_PRIO	(0x0014 / 4) /* run priority */

#define DIST_CONTROL  (0x1000 / 4)
#define DIST_TYPE     (0x1004 / 4)
#define DIST_ICDISRn  (0x1080 / 4) /* security */
#define DIST_ICDISERn (0x1100 / 4) /* set */
#define DIST_ICDICERn (0x1180 / 4) /* clear */
#define DIST_CDISPRn  (0x1200 / 4) /* pending */
#define DIST_ICDISRn  (0x1280 / 4) /* pending clear */
#define DIST_ICDABRn  (0x1300 / 4) /* active  */
#define DIST_ICDIPRn  (0x1400 / 4) /* priority */
#define DIST_ICDIPTRn (0x1800 / 4) /* target */
#define DIST_ICDICRn  (0x1C00 / 4) /* config */

static cpu_callback irq_handlers[IRQ_COUNT];

static void default_handler(uint32_t r0, uint32_t r1, uint32_t r2)
{
   printf("DEFAULT INTERRUPT HANDLER %x:%x:%x\n", r0, r1, r2);
}


int cpu_irq_get_count()
{
    return IRQ_COUNT;
}

void cpu_irq_set_enable(int number, BOOL enable)
{
    volatile uint32_t *gic = (uint32_t *) BASE_GIC0;
    gic[(enable ? DIST_ICDISERn : DIST_ICDICERn) + (number / 32)] = 1UL << (number & 31);
}

void cpu_irq_set_handler(int number, cpu_callback handler)
{
    irq_handlers[number] = handler;
}

void cpu_irq_set_config(int number, int priority)
{
    volatile uint32_t *gic = (uint32_t *) BASE_GIC0;
    volatile uint8_t *prio = (uint8_t *) &gic[DIST_ICDIPRn];

    prio[number] = priority;
}

void cpu_irq_set_priority(int priority)
{
    volatile uint32_t *gic = (uint32_t *) BASE_GIC0;
    gic[CPU_PRI_MSK] = priority;
}

void cpu_irq_dump()
{

    volatile uint32_t *gic = (uint32_t *) BASE_GIC0;
    printf("CPU control=%x mask=%x ack=%x eoi=%x prio=%x\n",
           gic[CPU_CONTROL], gic[CPU_PRI_MSK],
           gic[CPU_ICCIAR], gic[CPU_PRIO]);

    printf("DIST control=%x type=%x\n",
           gic[DIST_CONTROL], gic[DIST_TYPE]);

    printf("DIST enabled = %x %x %x\n",
           gic[DIST_ICDISERn + 0], gic[DIST_ICDISERn + 1], gic[DIST_ICDISERn + 2]);

    printf("DIST pending = %x %x %x\n",
           gic[DIST_CDISPRn + 0], gic[DIST_CDISPRn + 1], gic[DIST_CDISPRn + 2]);

    printf("DIST active  = %x %x %x\n",
           gic[DIST_ICDABRn + 0], gic[DIST_ICDABRn + 1], gic[DIST_ICDABRn + 2]);

}

void soc_irq_init()
{
    volatile uint32_t *gic = (uint32_t *) BASE_GIC0;
    int i;

    gic[DIST_CONTROL] = 0;
    gic[CPU_CONTROL] = 0;

    for(i = 0; i < IRQ_COUNT; i++)
        irq_handlers[i] = default_handler;

    gic[DIST_CONTROL] = 1;
    gic[CPU_PRI_MSK] = 0xFF;
    gic[CPU_BPR] = 0;
    gic[CPU_CONTROL] = 0x201;
}


void cpu_irq_handler()
{
    volatile uint32_t *gic = (uint32_t *) BASE_GIC0;
    uint32_t irq;


    for(;;) {
        /* what irq did just happen ? */
        irq = gic[CPU_ICCIAR] & 0x3FF;

        /* ignore spurious interrupts and others */
        if(irq >= 1022)
            return;

        /* out of range irq? */
        if(irq >= IRQ_COUNT) {
            printf("Unknown irq?\n");
            for(;;);
        }

        /* acknowledge */
        gic[CPU_ICCEOIR] = irq;

        /* call handler */
        if(irq_handlers[irq])
            irq_handlers[irq](0, 0, 0);
    }
}
