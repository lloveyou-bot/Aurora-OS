#include "pci.h"
#include "../terminal.h"
#include <stddef.h>

#define PCI_CONFIG_ADDR  0xCF8
#define PCI_CONFIG_DATA  0xCFC

// 端口 I/O 辅助
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

// 读取 PCI 配置空间
uint32_t pci_read(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address = (1 << 31)
                     | ((uint32_t)bus << 16)
                     | ((uint32_t)slot << 11)
                     | ((uint32_t)func << 8)
                     | (offset & 0xFC);
    outl(PCI_CONFIG_ADDR, address);
    return inl(PCI_CONFIG_DATA);
}

// 写入 PCI 配置空间
void pci_write(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value) {
    uint32_t address = (1 << 31)
                     | ((uint32_t)bus << 16)
                     | ((uint32_t)slot << 11)
                     | ((uint32_t)func << 8)
                     | (offset & 0xFC);
    outl(PCI_CONFIG_ADDR, address);
    outl(PCI_CONFIG_DATA, value);
}

// 扫描一个设备
int pci_scan_device(uint8_t bus, uint8_t slot, uint8_t func, pci_device_t* dev) {
    uint32_t vendor_device = pci_read(bus, slot, func, 0);
    uint16_t vendor_id = vendor_device & 0xFFFF;
    uint16_t device_id = (vendor_device >> 16) & 0xFFFF;
    
    if (vendor_id == 0xFFFF) return -1;  // 无效设备
    
    dev->vendor_id = vendor_id;
    dev->device_id = device_id;
    dev->bus = bus;
    dev->slot = slot;
    dev->func = func;
    
    // 读取 6 个 BAR（基地址寄存器）
    for (int i = 0; i < 6; i++) {
        dev->bar[i] = pci_read(bus, slot, func, 0x10 + i * 4);
    }
    
    // 读取 IRQ 线
    dev->irq = pci_read(bus, slot, func, 0x3C) & 0xFF;
    
    return 0;
}

// 扫描全部 PCI 总线
void pci_scan_all(void) {
    terminal_writestring("PCI: Scanning bus...\n");
    
    int device_count = 0;
    
    for (int bus = 0; bus < 256; bus++) {
        for (int slot = 0; slot < 32; slot++) {
            // 检查设备是否存在（只检查 func 0）
            uint32_t vendor_device = pci_read(bus, slot, 0, 0);
            uint16_t vendor_id = vendor_device & 0xFFFF;
            
            if (vendor_id == 0xFFFF) continue;  // 无效槽位
            
            // 检查是否是多功能设备
            uint32_t header = pci_read(bus, slot, 0, 0x0C);
            int is_multi = (header >> 16) & 0x80;
            
            int max_func = is_multi ? 7 : 0;
            
            for (int func = 0; func <= max_func; func++) {
                pci_device_t dev;
                if (pci_scan_device(bus, slot, func, &dev) == 0) {
                    device_count++;
                    
                    // 打印设备信息
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
                    
                    // 打印 BAR0
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

// 根据 vendor/device id 查找设备
int pci_find_device(uint16_t vendor_id, uint16_t device_id, pci_device_t* out) {
    for (int bus = 0; bus < 256; bus++) {
        for (int slot = 0; slot < 32; slot++) {
            uint32_t vendor_device = pci_read(bus, slot, 0, 0);
            uint16_t v = vendor_device & 0xFFFF;
            uint16_t d = (vendor_device >> 16) & 0xFFFF;
            
            if (v == vendor_id && d == device_id) {
                // 找到设备，只检查 func 0
                if (pci_scan_device(bus, slot, 0, out) == 0) {
                    return 0;
                }
            }
            
            // 检查多功能设备
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

// 读取单个 BAR（方便使用）
uint32_t pci_read_bar(uint8_t bus, uint8_t slot, uint8_t func, int bar_index) {
    if (bar_index < 0 || bar_index > 5) return 0;
    return pci_read(bus, slot, func, 0x10 + bar_index * 4);
}