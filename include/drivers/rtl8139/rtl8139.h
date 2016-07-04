#define RTL8139_VIRT_BAR0_MEM      0x80000
#define RTL8139_VIRT_BAR0_MEM_SIZE 0x10000

typedef struct s_rtl8139
{
	u8 bar_type;
	u32* mem_base;
	u32 mem_base_size;
	u16 io_base;
	u8 irq_line;
	u32 low_mac;
	u32 hi_mac;
	t_rx_desc_rtl8139* rx_desc;
	t_tx_desc_rtl8139* tx_desc;
	u16 rx_cur;
	u16 tx_cur;
	t_mac_addr mac_addr;
}
t_rtl8139;
