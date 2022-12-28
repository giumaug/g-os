#include "device.h"

t_device_desc* init_device(u8 device_num, u8 partition_num)
{
	t_device_desc* device_desc = NULL;
	
	device_desc = kmalloc(sizeof(t_device_desc));
	device_desc->num = device_num;
	device_desc->partition_num = partition_num;
	sem_init(&device_desc->mutex, 1);
	sem_init(&device_desc->sem, 0);
	return device_desc;
}

void free_device(t_device_desc* device_desc)
{
	kfree(device_desc);
}

u8 read_config_byte(t_device_desc* device_desc,u32 address)
{
	u32 virt_addr;
	u8 value;

	if (device_desc->dma_pci_bar_type == 0)
	{
		virt_addr = (u32) device_desc->dma_pci_mem_base + address;
		value =  (*((volatile u8*)(virt_addr)));
	}
	else 
	{
		value = in(device_desc->dma_pci_io_base + address);
	}
	return value;
}

u32 read_config_dword(t_device_desc* device_desc,u32 address)
{
	u32 virt_addr;
	u32 value;

	if (device_desc->dma_pci_bar_type == 0)
	{
		virt_addr = (u32) device_desc->dma_pci_mem_base + address;
		value =  (*((volatile u32*)(virt_addr)));
	}
	else 
	{
		value = indw(device_desc->dma_pci_io_base + address);
	}
	return value;
}

void write_config_dword(t_device_desc* device_desc,u8 address,u32 value)
{
	u32 virt_addr;
	u8 reg_value;

	if (device_desc->dma_pci_bar_type == 0)
	{
		virt_addr = (u32) device_desc->dma_pci_mem_base + address;
		 (*((volatile u32*)(virt_addr))) = (value);
	}
	else 
	{
		outdw(value,device_desc->dma_pci_io_base + address);
	}
}

void write_config_byte(t_device_desc* device_desc,u32 address,u8 value)
{
	u32 virt_addr;

	if (device_desc->dma_pci_bar_type == 0)
	{
		virt_addr = (u32) device_desc->dma_pci_mem_base + address;
		 (*((volatile u32*)(virt_addr))) = (value);
	}
	else 
	{
		out(value,device_desc->dma_pci_io_base + address);
	}
}
