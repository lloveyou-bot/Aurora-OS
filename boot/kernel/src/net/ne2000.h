#ifndef NE2000_H
#define NE2000_H

#include <stdint.h>

#define NE2000_VENDOR 0x10EC
#define NE2000_DEVICE 0x8029

#define NE_DATA       0x00
#define NE_COMMAND    0x01
#define NE_STATUS     0x02
#define NE_DCR        0x03
#define NE_IMR        0x04
#define NE_ISR        0x05
#define NE_RST        0x06
#define NE_TPSR       0x07
#define NE_TBCR0      0x08
#define NE_TBCR1      0x09
#define NE_PSTART     0x0A
#define NE_PSTOP      0x0B
#define NE_BNRY       0x0C
#define NE_CR         0x0D
#define NE_IMR2       0x0E
#define NE_ISR2       0x0F
#define NE_RSAR0      0x10
#define NE_RSAR1      0x11
#define NE_RBCR0      0x12
#define NE_RBCR1      0x13
#define NE_RCR        0x14
#define NE_TCR        0x15

#define NE_CMD_PAGE0  0x00
#define NE_CMD_PAGE1  0x40
#define NE_CMD_PAGE2  0x80
#define NE_CMD_STOP   0x01
#define NE_CMD_START  0x02
#define NE_CMD_TX     0x04
#define NE_CMD_RD     0x08
#define NE_CMD_RW     0x10

#define NE_ISR_RX     0x01
#define NE_ISR_TX     0x02
#define NE_ISR_RX_ERR 0x04
#define NE_ISR_TX_ERR 0x08
#define NE_ISR_OVER   0x10
#define NE_ISR_CNT    0x20
#define NE_ISR_RDC    0x40

#define NE_RCR_MON    0x01
#define NE_RCR_BROAD  0x04
#define NE_RCR_MULTI  0x08
#define NE_RCR_ERR    0x10
#define NE_RCR_ABORT  0x20

#define NE_TCR_CRC    0x01
#define NE_TCR_LOOP   0x02

#define NE_TX_START_PAGE 0x40
#define NE_RX_START_PAGE 0x46
#define NE_RX_STOP_PAGE  0x60

void ne2000_init(void);
void ne2000_send(const uint8_t* data, uint16_t len);
int ne2000_receive(uint8_t* buffer, uint16_t* len);
void ne2000_interrupt_handler(void);
void ne2000_get_mac(uint8_t* mac);
int ne2000_is_initialized(void);

#endif
