#ifndef PCI_H                
#define PCI_H

#include "system.h"

u16 read_pci_config_word(u8 bus, u8 slot,u8 func,u8 offset);

#endif
