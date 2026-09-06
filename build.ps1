# Aurora OS - Network Module Generator
# by Chisa

Write-Host "================================================" -ForegroundColor Cyan
Write-Host "       Aurora OS - Network Module Generator    " -ForegroundColor Cyan
Write-Host "              by Chisa :3                       " -ForegroundColor Cyan
Write-Host "================================================" -ForegroundColor Cyan
Write-Host ""

$ProjectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $ProjectRoot

# Create directories
Write-Host "[1/6] Creating directories..." -ForegroundColor Yellow
New-Item -ItemType Directory -Force -Path "kernel\src\pci" | Out-Null
New-Item -ItemType Directory -Force -Path "kernel\src\net" | Out-Null
Write-Host "[OK] Directories created" -ForegroundColor Green

# Generate pci.h
Write-Host "[2/6] Generating pci.h..." -ForegroundColor Yellow
@'
#ifndef PCI_H
#define PCI_H

#include <stdint.h>

typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t bus;
    uint8_t slot;
    uint8_t func;
    uint32_t bar[6];
    uint8_t irq;
} pci_device_t;

uint32_t pci_read(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void pci_write(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value);
int pci_scan_device(uint8_t bus, uint8_t slot, uint8_t func, pci_device_t* dev);
void pci_scan_all(void);
int pci_find_device(uint16_t vendor_id, uint16_t device_id, pci_device_t* out);
uint32_t pci_read_bar(uint8_t bus, uint8_t slot, uint8_t func, int bar_index);

#endif
'@ | Out-File -FilePath "kernel\src\pci\pci.h" -Encoding ASCII
Write-Host "[OK] pci.h generated" -ForegroundColor Green

# Generate pci.c
Write-Host "[3/6] Generating pci.c..." -ForegroundColor Yellow
@'
#include "pci.h"
#include "../terminal.h"
#include <stddef.h>

#define PCI_CONFIG_ADDR  0xCF8
#define PCI_CONFIG_DATA  0xCFC

static inline void outl(uint16_t port, uint32_t val) {
    asm volatile("outl %0, %1" : : "a"(val), "d"(port));
}

static inline uint32_t inl(uint16_t port) {
    uint32_t val;
    asm volatile("inl %1, %0" : "=a"(val) : "d"(port));
    return val;
}

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "d"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t val;
    asm volatile("inb %1, %0" : "=a"(val) : "d"(port));
    return val;
}

uint32_t pci_read(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address = (1 << 31)
                     | ((uint32_t)bus << 16)
                     | ((uint32_t)slot << 11)
                     | ((uint32_t)func << 8)
                     | (offset & 0xFC);
    outl(PCI_CONFIG_ADDR, address);
    return inl(PCI_CONFIG_DATA);
}

void pci_write(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value) {
    uint32_t address = (1 << 31)
                     | ((uint32_t)bus << 16)
                     | ((uint32_t)slot << 11)
                     | ((uint32_t)func << 8)
                     | (offset & 0xFC);
    outl(PCI_CONFIG_ADDR, address);
    outl(PCI_CONFIG_DATA, value);
}

int pci_scan_device(uint8_t bus, uint8_t slot, uint8_t func, pci_device_t* dev) {
    uint32_t vendor_device = pci_read(bus, slot, func, 0);
    uint16_t vendor_id = vendor_device & 0xFFFF;
    uint16_t device_id = (vendor_device >> 16) & 0xFFFF;
    
    if (vendor_id == 0xFFFF) return -1;
    
    dev->vendor_id = vendor_id;
    dev->device_id = device_id;
    dev->bus = bus;
    dev->slot = slot;
    dev->func = func;
    
    for (int i = 0; i < 6; i++) {
        dev->bar[i] = pci_read(bus, slot, func, 0x10 + i * 4);
    }
    
    dev->irq = pci_read(bus, slot, func, 0x3C) & 0xFF;
    
    return 0;
}

void pci_scan_all(void) {
    terminal_writestring("PCI: Scanning bus...\n");
    int device_count = 0;
    
    for (int bus = 0; bus < 256; bus++) {
        for (int slot = 0; slot < 32; slot++) {
            uint32_t vendor_device = pci_read(bus, slot, 0, 0);
            uint16_t vendor_id = vendor_device & 0xFFFF;
            
            if (vendor_id == 0xFFFF) continue;
            
            uint32_t header = pci_read(bus, slot, 0, 0x0C);
            int is_multi = (header >> 16) & 0x80;
            int max_func = is_multi ? 7 : 0;
            
            for (int func = 0; func <= max_func; func++) {
                pci_device_t dev;
                if (pci_scan_device(bus, slot, func, &dev) == 0) {
                    device_count++;
                    terminal_writestring("  ");
                    terminal_writedec(bus);
                    terminal_writestring(":");
                    terminal_writedec(slot);
                    terminal_writestring(".");
                    terminal_writedec(func);
                    terminal_writestring(" [");
                    terminal_writehex(dev.vendor_id, 4);
                    terminal_writestring(":");
                    terminal_writehex(dev.device_id, 4);
                    terminal_writestring("] IRQ=");
                    terminal_writedec(dev.irq);
                    if (dev.bar[0] != 0) {
                        terminal_writestring(" BAR0=0x");
                        terminal_writehex(dev.bar[0], 8);
                    }
                    terminal_writestring("\n");
                }
            }
        }
    }
    
    terminal_writestring("PCI: Found ");
    terminal_writedec(device_count);
    terminal_writestring(" device(s)\n");
}

int pci_find_device(uint16_t vendor_id, uint16_t device_id, pci_device_t* out) {
    for (int bus = 0; bus < 256; bus++) {
        for (int slot = 0; slot < 32; slot++) {
            uint32_t vendor_device = pci_read(bus, slot, 0, 0);
            uint16_t v = vendor_device & 0xFFFF;
            uint16_t d = (vendor_device >> 16) & 0xFFFF;
            
            if (v == vendor_id && d == device_id) {
                if (pci_scan_device(bus, slot, 0, out) == 0) {
                    return 0;
                }
            }
            
            uint32_t header = pci_read(bus, slot, 0, 0x0C);
            int is_multi = (header >> 16) & 0x80;
            if (is_multi) {
                for (int func = 1; func < 8; func++) {
                    uint32_t vd = pci_read(bus, slot, func, 0);
                    uint16_t v = vd & 0xFFFF;
                    uint16_t d = (vd >> 16) & 0xFFFF;
                    if (v == vendor_id && d == device_id) {
                        if (pci_scan_device(bus, slot, func, out) == 0) {
                            return 0;
                        }
                    }
                }
            }
        }
    }
    return -1;
}

uint32_t pci_read_bar(uint8_t bus, uint8_t slot, uint8_t func, int bar_index) {
    if (bar_index < 0 || bar_index > 5) return 0;
    return pci_read(bus, slot, func, 0x10 + bar_index * 4);
}
'@ | Out-File -FilePath "kernel\src\pci\pci.c" -Encoding ASCII
Write-Host "[OK] pci.c generated" -ForegroundColor Green

# Generate ne2000.h
Write-Host "[4/6] Generating ne2000.h..." -ForegroundColor Yellow
@'
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
'@ | Out-File -FilePath "kernel\src\net\ne2000.h" -Encoding ASCII
Write-Host "[OK] ne2000.h generated" -ForegroundColor Green

# Generate ne2000.c
Write-Host "[5/6] Generating ne2000.c..." -ForegroundColor Yellow
@'
#include "ne2000.h"
#include "../pci/pci.h"
#include "../terminal.h"
#include <stddef.h>

static uint16_t ne2000_io_base = 0;
static uint8_t ne2000_mac[6] = {0};
static uint8_t ne2000_irq = 0;
static int ne2000_initialized = 0;

static inline void ne_outb(uint16_t offset, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "d"(ne2000_io_base + offset));
}

static inline uint8_t ne_inb(uint16_t offset) {
    uint8_t val;
    asm volatile("inb %1, %0" : "=a"(val) : "d"(ne2000_io_base + offset));
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
        terminal_writestring("NE2000: Packet received\n");
        ne_outb(NE_ISR, NE_ISR_RX);
    }
    if (isr & NE_ISR_TX) {
        terminal_writestring("NE2000: Packet sent\n");
        ne_outb(NE_ISR, NE_ISR_TX);
    }
    if (isr & NE_ISR_RX_ERR) {
        terminal_writestring("NE2000: Receive error\n");
        ne_outb(NE_ISR, NE_ISR_RX_ERR);
    }
    if (isr & NE_ISR_TX_ERR) {
        terminal_writestring("NE2000: Send error\n");
        ne_outb(NE_ISR, NE_ISR_TX_ERR);
    }
}

void ne2000_init(void) {
    pci_device_t dev;
    terminal_writestring("NE2000: Initializing...\n");
    
    if (pci_find_device(NE2000_VENDOR, NE2000_DEVICE, &dev) != 0) {
        terminal_writestring("NE2000: Device not found!\n");
        return;
    }
    
    ne2000_io_base = dev.bar[0] & 0xFFFE;
    ne2000_irq = dev.irq;
    
    terminal_writestring("NE2000: Found at I/O 0x");
    terminal_writehex(ne2000_io_base, 4);
    terminal_writestring(", IRQ ");
    terminal_writedec(ne2000_irq);
    terminal_writestring("\n");
    
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
    
    terminal_writestring("NE2000: MAC ");
    for (int i = 0; i < 6; i++) {
        terminal_writehex(ne2000_mac[i], 2);
        if (i < 5) terminal_writestring(":");
    }
    terminal_writestring("\n");
    
    ne_outb(NE_COMMAND, NE_CMD_PAGE0 | NE_CMD_START);
    ne2000_initialized = 1;
    terminal_writestring("NE2000: Initialized successfully!\n");
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
'@ | Out-File -FilePath "kernel\src\net\ne2000.c" -Encoding ASCII
Write-Host "[OK] ne2000.c generated" -ForegroundColor Green

# Generate test file
Write-Host "[6/6] Creating test entry..." -ForegroundColor Yellow
@'
#include "../pci/pci.h"
#include "../net/ne2000.h"
#include "../terminal.h"

void net_test(void) {
    terminal_writestring("\n=== Network Test ===\n");
    pci_scan_all();
    ne2000_init();
    
    if (ne2000_is_initialized()) {
        uint8_t test_packet[42] = {
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x08, 0x06,
            0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x01,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xC0, 0xA8, 0x01, 0x64,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xC0, 0xA8, 0x01, 0x01
        };
        
        uint8_t mac[6];
        ne2000_get_mac(mac);
        for (int i = 0; i < 6; i++) {
            test_packet[6 + i] = mac[i];
            test_packet[22 + i] = mac[i];
        }
        
        ne2000_send(test_packet, 42);
        terminal_writestring("NE2000: Test ARP packet sent!\n");
    }
    terminal_writestring("=== End of Network Test ===\n\n");
}
'@ | Out-File -FilePath "kernel\src\net_test.c" -Encoding ASCII
Write-Host "[OK] net_test.c generated" -ForegroundColor Green

Write-Host ""
Write-Host "================================================" -ForegroundColor Cyan
Write-Host "          [OK] All files generated!            " -ForegroundColor Cyan
Write-Host "================================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Generated files:" -ForegroundColor Yellow
Write-Host "   kernel/src/pci/pci.h"
Write-Host "   kernel/src/pci/pci.c"
Write-Host "   kernel/src/net/ne2000.h"
Write-Host "   kernel/src/net/ne2000.c"
Write-Host "   kernel/src/net_test.c"
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "   1. Add #include \"../net_test.c\" in main.c"
Write-Host "   2. Call net_test() in kmain()"
Write-Host "   3. Run build.ps1 to compile"
Write-Host "   4. Start QEMU to test"
Write-Host ""
Write-Host "QEMU command:" -ForegroundColor Green
Write-Host "   qemu-system-x86_64 -m 64 -vga std -drive file=aurora.img,format=raw,if=floppy -boot a -netdev user,id=net0 -device ne2k_pci,netdev=net0 -no-reboot -no-shutdown"
Write-Host ""
Read-Host "Press Enter to exit"