#include "pci.h"
#include "port.h"
#include "terminal.h"
#include "../paging/paging.h"

uint16_t pci_read_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    uint32_t bus32 = (uint32_t)bus;
    uint32_t slot32 = (uint32_t)slot;
    uint32_t func32 = (uint32_t)func;

    uint32_t device = (uint32_t)((bus32 << 16) | (slot32 << 11) | (func32 << 8) | (offset & 0xFC) | 0x80000000);

    send_long_to(0xCF8, device);

    return (uint16_t)((get_long_from(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
}

void pci_write_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint16_t data)
{
    uint32_t bus32 = (uint32_t)bus;
    uint32_t slot32 = (uint32_t)slot;
    uint32_t func32 = (uint32_t)func;

    uint32_t device = (uint32_t)((bus32 << 16) | (slot32 << 11) | (func32 << 8) | (offset & 0xFC) | 0x80000000);

    send_long_to(0xCF8, device);

    send_long_to(0xCFC, data);
}

PCI pci_scan_slot(uint8_t bus, uint8_t slot, uint8_t func)
{
    PCI pci;
    if ((pci.pci.flat[0] = pci_read_config(bus, slot, func,0)) != 0xffff)
    {
        for (uint32_t i = 0; i < 32; i++)
            pci.pci.flat[i] = pci_read_config(bus, slot, func, i*2);
        pci.bus = bus;
        pci.slot = slot;
        pci.func = func;
        pci.pad = 0xDE;
    }
    return pci;
}

void pci_write_pci(PCI pci)
{
    uint8_t bus = pci.bus;
    uint8_t slot = pci.slot;
    uint8_t func = pci.func;
    for (uint32_t i = 0; i < 32; i++)
        pci_write_config(bus, slot, func, i*2, pci.pci.flat[i]);
}

void pci_print_pci(PCI pci)
{
    printf("PCI: b:%d s:%d f:%d  v:%h, d:%h, prog %d, rev %d, c_code %d, c_sub %d, h %b\n"
                                                                , pci.bus
                                                                , pci.slot
                                                                , pci.func
                                                                , pci.pci.pci.vendor
                                                                , pci.pci.pci.id
                                                                , pci.pci.pci.prog
                                                                , pci.pci.pci.rev
                                                                , pci.pci.pci.c_code
                                                                , pci.pci.pci.c_sub
                                                                , pci.pci.pci.h_type);
}

void pci_init()
{

    paging_map_new_to_virtual(0xc0007000);
    devices = paging_get_pci_map();

    device_count = 0;

    for (uint16_t bus = 0; bus < 256; bus++)
    {
        for (uint8_t slot = 0; slot < 8; slot++)
        {
            uint8_t func = 0;
            PCI pci = pci_scan_slot((uint8_t)bus, slot, func);
            if (pci.pci.pci.vendor != 0xffff)
            {
                devices[device_count++] = pci;
                if ((pci.pci.pci.h_type & 0x80) != 0)
                {
                    for (func = 1; func < 8; func++)
                    {
                        pci = pci_scan_slot((uint8_t)bus, slot, func);
                        if (pci.pci.pci.vendor != 0xffff)
                        {
                            devices[device_count++] = pci;
                        }
                    }
                }
            }
        }
    }
    return;
    //pci_print_all();
}

void pci_print_all()
{
    for (uint32_t i = 0; i < device_count; i++)
    {
        pci_print_pci(devices[i]);
    }
    printf("got %d devices\n", device_count);
}