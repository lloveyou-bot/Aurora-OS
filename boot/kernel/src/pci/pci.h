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
