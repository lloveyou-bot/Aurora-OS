#include "ne2000.h"
#include "../pci/pci.h"
#include "../terminal.h"
#include <stddef.h>

// ====== 本地辅助函数 ======
static void write_dec(unsigned int num) {
    char buf[32];
    int i = 0;
    if (num == 0) {
        terminal_write("0");
        return;
    }
    char temp[32];
    int j = 0;
    while (num > 0) {
        temp[j++] = '0' + (num % 10);
        num /= 10;
    }
    while (j > 0) {
        buf[i++] = temp[--j];
    }
    buf[i] = '\0';
    terminal_write(buf);
}

static void write_hex(unsigned int num, int digits) {
    char hex[] = "0123456789ABCDEF";
    char buf[16];
    int i = 0;
    for (int d = digits - 1; d >= 0; d--) {
        buf[i++] = hex[(num >> (d * 4)) & 0xF];
    }
    buf[i] = '\0';
    terminal_write(buf);
}

static uint16_t ne2000_io_base = 0;
static uint8_t ne2000_mac[6] = {0};
static uint8_t ne2000_irq = 0;
static int ne2000_initialized = 0;

// ====== 修复后的 I/O 函数 ======
static inline void ne_outb(uint16_t offset, uint8_t val) {
    uint16_t port = ne2000_io_base + offset;
    asm volatile("outb %0, %1" : : "a"(val), "d"(port));
}

static inline uint8_t ne_inb(uint16_t offset) {
    uint8_t val;
    uint16_t port = ne2000_io_base + offset;
    asm volatile("inb %1, %0" : "=a"(val) : "d"(port));
    return val;
}

static inline void ne_delay(int count) {
    for (int i = 0; i < count; i++) {
        asm volatile("nop");
    }
}

static void ne_remote_read(uint16_t start, uint16_t len, uint8_t* buffer) {
    ne_outb(NE_RSAR0, start & 0xFF);
    ne_outb(NE_RSAR1, (start >> 8) & 0xFF);
    ne_outb(NE_RBCR0, len & 0xFF);
    ne_outb(NE_RBCR1, (len >> 8) & 0xFF);
    ne_outb(NE_COMMAND, NE_CMD_PAGE0 | NE_CMD_RD);
    for (uint16_t i = 0; i < len; i++) {
        buffer[i] = ne_inb(NE_DATA);
    }
}

static void ne_remote_write(uint16_t start, uint16_t len, const uint8_t* buffer) {
    ne_outb(NE_RSAR0, start & 0xFF);
    ne_outb(NE_RSAR1, (start >> 8) & 0xFF);
    ne_outb(NE_RBCR0, len & 0xFF);
    ne_outb(NE_RBCR1, (len >> 8) & 0xFF);
    ne_outb(NE_COMMAND, NE_CMD_PAGE0 | NE_CMD_RW);
    for (uint16_t i = 0; i < len; i++) {
        ne_outb(NE_DATA, buffer[i]);
    }
}

static void ne2000_reset(void) {
    ne_outb(NE_RST, 0xFF);
    for (int i = 0; i < 1000; i++) {
        ne_delay(100);
        if (ne_inb(NE_RST) == 0) break;
    }
    ne_delay(1000);
}

static void ne2000_read_mac(void) {
    ne_outb(NE_COMMAND, NE_CMD_PAGE1);
    for (int i = 0; i < 6; i++) {
        ne2000_mac[i] = ne_inb(0x10 + i);
    }
    ne_outb(NE_COMMAND, NE_CMD_PAGE0);
}

static int ne2000_has_packet(void) {
    uint8_t curr = ne_inb(NE_CR) & 0x3F;
    uint8_t bnry = ne_inb(NE_BNRY);
    return (curr != bnry);
}

void ne2000_interrupt_handler(void) {
    uint8_t isr = ne_inb(NE_ISR);
    if (isr & NE_ISR_RX) {
        terminal_write("NE2000: Packet received\n");
        ne_outb(NE_ISR, NE_ISR_RX);
    }
    if (isr & NE_ISR_TX) {
        terminal_write("NE2000: Packet sent\n");
        ne_outb(NE_ISR, NE_ISR_TX);
    }
    if (isr & NE_ISR_RX_ERR) {
        terminal_write("NE2000: Receive error\n");
        ne_outb(NE_ISR, NE_ISR_RX_ERR);
    }
    if (isr & NE_ISR_TX_ERR) {
        terminal_write("NE2000: Send error\n");
        ne_outb(NE_ISR, NE_ISR_TX_ERR);
    }
}

void ne2000_init(void) {
    pci_device_t dev;
    terminal_write("NE2000: Initializing...\n");
    
    if (pci_find_device(NE2000_VENDOR, NE2000_DEVICE, &dev) != 0) {
        terminal_write("NE2000: Device not found!\n");
        return;
    }
    
    ne2000_io_base = dev.bar[0] & 0xFFFE;
    ne2000_irq = dev.irq;
    
    terminal_write("NE2000: Found at I/O 0x");
    write_hex(ne2000_io_base, 4);
    terminal_write(", IRQ ");
    write_dec(ne2000_irq);
    terminal_write("\n");
    
    ne2000_reset();
    ne_outb(NE_DCR, 0x41);
    ne_outb(NE_RCR, NE_RCR_BROAD);
    ne_outb(NE_TCR, 0x00);
    ne_outb(NE_PSTART, NE_RX_START_PAGE);
    ne_outb(NE_PSTOP, NE_RX_STOP_PAGE);
    ne_outb(NE_BNRY, NE_RX_START_PAGE);
    ne_outb(NE_TPSR, NE_TX_START_PAGE);
    ne_outb(NE_IMR, NE_ISR_RX | NE_ISR_TX);
    
    ne2000_read_mac();
    
    terminal_write("NE2000: MAC ");
    for (int i = 0; i < 6; i++) {
        write_hex(ne2000_mac[i], 2);
        if (i < 5) terminal_write(":");
    }
    terminal_write("\n");
    
    ne_outb(NE_COMMAND, NE_CMD_PAGE0 | NE_CMD_START);
    ne2000_initialized = 1;
    terminal_write("NE2000: Initialized successfully!\n");
}

void ne2000_send(const uint8_t* data, uint16_t len) {
    if (!ne2000_initialized) return;
    if (len > 1500) len = 1500;
    ne_delay(100);
    ne_remote_write(NE_TX_START_PAGE << 8, len, data);
    ne_outb(NE_TBCR0, len & 0xFF);
    ne_outb(NE_TBCR1, (len >> 8) & 0xFF);
    ne_outb(NE_COMMAND, NE_CMD_PAGE0 | NE_CMD_TX | NE_CMD_START);
}

int ne2000_receive(uint8_t* buffer, uint16_t* len) {
    if (!ne2000_initialized) return -1;
    if (!ne2000_has_packet()) return -1;
    
    uint8_t bnry = ne_inb(NE_BNRY);
    uint8_t header[4];
    ne_remote_read(bnry << 8, 4, header);
    
    uint8_t status = header[0];
    uint8_t next = header[1];
    uint16_t pkt_len = header[2] | (header[3] << 8);
    pkt_len -= 4;
    
    if (status & 0x01) {
        ne_outb(NE_BNRY, next);
        return -1;
    }
    
    if (pkt_len > 0 && buffer != NULL) {
        ne_remote_read((bnry << 8) + 4, pkt_len, buffer);
    }
    *len = pkt_len;
    ne_outb(NE_BNRY, next);
    return 0;
}

void ne2000_get_mac(uint8_t* mac) {
    for (int i = 0; i < 6; i++) {
        mac[i] = ne2000_mac[i];
    }
}

int ne2000_is_initialized(void) {
    return ne2000_initialized;
}