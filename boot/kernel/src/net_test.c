#include "pci/pci.h"
#include "net/ne2000.h"
#include "terminal.h"

void net_test(void) {
    terminal_write("\n=== Network Test ===\n");
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
        terminal_write("NE2000: Test ARP packet sent!\n");
    }
    terminal_write("=== End of Network Test ===\n\n");
}