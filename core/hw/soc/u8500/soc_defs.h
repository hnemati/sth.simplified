
#ifndef _SOC_DEFS_H_
#define _SOC_DEFS_H_

/* PRCMU */
#define PRCMU_BASE 0x80157000
#define PRCMU_UARTCLK_MGT 6

#define PRCM_PLLDIV 0
#define PRCM_CLKEN (1UL << 8)
#define PRCM_CLK38 (1UL << 9)
#define PRCM_CLK38SRC (1UL << 10)
#define PRCM_CLK38DIV (1UL << 11)


/* PRCC */
#define PRCC_CLK_RST3_BASE 0x8000F000
#define PRCC_CLK_RST1_BASE 0x8012F000

#define PRCC_PCKEN              0
#define PRCC_PCKDIS             1
#define PRCC_KCKEN              2
#define PRCC_KCKDIS             3
#define PRCC_PCKSR              4
#define PRCC_PKCKSR             5

/* GPIO */
#define GPIO0_BASE 0x8012E000
#define GPIO_AFSLA (0x20 / 4)
#define GPIO_AFSLB (0x24 / 4)

/* UART */
#define BASE_UART0          0x80120000
#define BASE_UART1          0x80121000
#define BASE_UART2          0x80007000

#define UART_DR                0
#define UART_RSR               1
#define UART_FR                6
#define UART_LCRH_RX           7
#define UART_ILPR              8
#define UART_IBRD              9
#define UART_FBRD              10
#define UART_LCRH_TX           11
#define UART_CR                12
#define UART_IFLS              13
#define UART_IMSC              14
#define UART_RIS               15
#define UART_MIS               16
#define UART_ICR               17



#endif /* _SOC_DEFS_H_ */
