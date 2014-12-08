#pragma once
#include "../declarations.h"

uint16_t pci_read_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void pci_write_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint16_t data);
struct pci_control
{
    uint16_t vendor; uint16_t id;
    uint16_t command; uint16_t status;
    uint8_t prog; uint8_t rev; uint8_t c_code; uint8_t c_sub;
    uint8_t time; uint8_t size; uint8_t h_type; uint8_t bist;
    uint32_t bar0;
    uint32_t bar1;
    uint32_t bar2;
    uint32_t bar3;
    uint32_t bar4;
    uint32_t bar5;
    uint32_t cis;
    uint16_t sub_vendor; uint16_t sub_id;
    uint32_t rom;
    uint32_t res;
    uint32_t res1;
    uint32_t res2;
} __attribute__((packed));

union pciu
{
    struct pci_control pci;
    uint16_t flat[32];
};

typedef struct meta_pci
{
    uint8_t bus; uint8_t slot; uint8_t func; uint8_t pad;
    union pciu pci;
}PCI;

PCI* devices;
uint32_t device_count;

PCI pci_scan_slot(uint8_t bus, uint8_t slot, uint8_t func);
void pci_print_pci(PCI pci);

void pci_init();
void pci_print_all();