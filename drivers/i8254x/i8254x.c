#include "drivers/i8254x/i8254x.h"
#include "virtual_memory/vm.h"

static void write_i8254x(t_i8254x* i8254x,u32 address,u32 value)
{
	u32 virt_addr;


	if (i8254x->bar_type==0)
	{
		virt_addr=(u32)i8254x->mem_base+address;
		 (*((volatile u32*)(virt_addr)))=(value);
	}
	else 
	{
		outdw(address,i8254x->io_base);
		outdw(value,i8254x->io_base+4);
	}
}

static u32 read_i8254x(t_i8254x* i8254x,u32 address)
{
	u32 val=0;
	u32 virt_addr;

	if (i8254x->bar_type==0)
	{
		virt_addr=(u32)i8254x->mem_base+address;
		val=*((volatile u32*)(virt_addr));
	}
	else 
	{
		outdw(address,i8254x->io_base);
		val=indw(i8254x->io_base+4);
	}
	return val;
}

static u16 read_eeprom_i8254x(t_i8254x* i8254x,u8 addr)
{
	u32 tmp;
	
	write_i8254x(i8254x,REG_EERD, 1 | ((u32)(addr)<< 8));
	while( !((tmp = read_i8254x(i8254x,REG_EERD)) & (1 << 4)));

	return (u16)((tmp >> 16) & 0xFFFF);
}

static void read_mac_i8254x(t_i8254x* i8254x)
{
	u16 tmp;

	//tmp=read_i8254x(i8254x,0x5400);

	tmp=read_eeprom_i8254x(i8254x,2);	
	i8254x->mac_addr.lo=(tmp>>8) | (tmp<<8);
	
	tmp=read_eeprom_i8254x(i8254x,1);	
	i8254x->mac_addr.mi=(tmp>>8) | (tmp<<8);

	tmp=read_eeprom_i8254x(i8254x,0);	
	i8254x->mac_addr.hi=(tmp>>8) | (tmp<<8);
}

void int_handler_i8254x();

static rx_init_i8254x(t_i8254x* i8254x)
{
	int i;
	t_rx_desc_i8254x* rx_desc;
	t_data_sckt_buf* data_sckt_buf;

	rx_desc=kmalloc(16 + sizeof(t_rx_desc_i8254x) * NUM_RX_DESC);
	rx_desc = ((u32)rx_desc + 16) - ((u32)rx_desc % 16);
	for (i=0;i<NUM_RX_DESC;i++)
	{
		//ALLOC IN ADVANCE BUFFER TO AVOID MEMCPY
		data_sckt_buf=alloc_sckt(MTU_ETH);
		rx_desc[i].hi_addr=0;
		u32 aaa=FROM_VIRT_TO_PHY((u32)rx_desc);
		rx_desc[i].low_addr=FROM_VIRT_TO_PHY((u32)data_sckt_buf->data);
		rx_desc[i].status=0;

		rx_desc[i].length=0;
		rx_desc[i].checksum=0;
		rx_desc[i].errors=0;
		rx_desc[i].special=0;
	}
	i8254x->rx_desc=rx_desc;
	write_i8254x(i8254x,RDBAL_REG,FROM_VIRT_TO_PHY((u32)rx_desc));
	write_i8254x(i8254x,RDBAH_REG,0);
	write_i8254x(i8254x,RDLEN_REG,NUM_RX_DESC*16);
	i8254x->rx_cur=0;
	
	write_i8254x(i8254x,RHD_REG,0);
	write_i8254x(i8254x,RDT_REG,NUM_RX_DESC-1);
	write_i8254x(i8254x,RCTRL_REG, (RCTL_EN | RCTL_SBP | RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC  | RCTL_BSIZE_8192));
}

static tx_init_i8254x(t_i8254x* i8254x)
{
	int i;
	t_tx_desc_i8254x* tx_desc;

	tx_desc=kmalloc(16 + sizeof(t_tx_desc_i8254x) * NUM_TX_DESC);
	tx_desc = ((u32)tx_desc + 16) - ((u32)tx_desc % 16);
	for (i=0;i<NUM_TX_DESC;i++)
	{
		tx_desc[i].hi_addr=0;
		tx_desc[i].low_addr=0;
		tx_desc[i].status=TSTA_DD;
		tx_desc[i].cmd=0;

		tx_desc[i].length=0;
		tx_desc[i].cso=0;
		tx_desc[i].css=0;
		tx_desc[i].special=0;
	}
	i8254x->tx_desc=tx_desc;
	write_i8254x(i8254x,TDBAL_REG,FROM_VIRT_TO_PHY((u32)tx_desc)); //first 4 bit zero
	write_i8254x(i8254x,TDBAH_REG,0);
	write_i8254x(i8254x,TDLEN_REG,NUM_RX_DESC*16);
	i8254x->tx_cur=0;

	write_i8254x(i8254x,THD_REG,0);
	write_i8254x(i8254x,TDT_REG,0);
	write_i8254x(i8254x,TCTRL_REG,(TCTL_EN| TCTL_PSP | (15 << TCTL_CT_SHIFT) | (64 << TCTL_COLD_SHIFT) | TCTL_RTLC));
}

static start_link_i8254x(t_i8254x* i8254x)
{
	u32 current_state;

	current_state=read_i8254x(i8254x,CTRL_REG);
	write_i8254x(i8254x,CTRL_REG,current_state | CTRL_SLU);
}

static reset_multicast_array(t_i8254x* i8254x)
{
	int index;

	for(index = 0; index < 0x80; index++)
	{
		write_i8254x(i8254x,MLTC_TBL_ARRY_REG+index*4,0);
	}
}

t_i8254x* init_8254x()
{
	struct t_process_context* current_process_context;
	t_i8254x* i8254x=NULL;
	struct t_i_desc i_desc;
	u32* bar0;

	i8254x=kmalloc(sizeof(t_i8254x));
	bar0=read_pci_config_word(I8254X_BUS,I8254X_SLOT,I8254X_FUNC,I8254X_BAR0);
	if ((u32)bar0 & 0x1) 
	{
		i8254x->io_base=(u32)bar0 & 0xFFFC;
		i8254x->mem_base=NULL;
		i8254x->bar_type=1;
	}
	else 
	{
		i8254x->mem_base=I8254X_VIRT_BAR0_MEM;
		i8254x->io_base=NULL;
		i8254x->bar_type=0;
		CURRENT_PROCESS_CONTEXT(current_process_context);
		map_vm_mem(current_process_context->page_dir,I8254X_VIRT_BAR0_MEM,(((u32) (bar0)) & 0xFFFFFFF0),I8254X_VIRT_BAR0_MEM_SIZE,3);
		SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) current_process_context->page_dir))) 
//		map_vm_mem(system.master_page_dir,I8254X_VIRT_BAR0_MEM,(((u32) (bar0)) & 0xFFFFFFF0),I8254X_VIRT_BAR0_MEM_SIZE,3);
//		SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) system.master_page_dir))) 
	}

	i8254x->irq_line=read_pci_config_word(I8254X_BUS,I8254X_SLOT,I8254X_FUNC,I8254X_IRQ_LINE-3) & 0xFF;
	read_mac_i8254x(i8254x);
	start_link_i8254x(i8254x);

	i_desc.baseLow=((int)&int_handler_i8254x) & 0xFFFF;
	i_desc.selector=0x8;
	i_desc.flags=0x08e00;
	i_desc.baseHi=((int)&int_handler_i8254x)>>0x10;
	set_idt_entry(0x20+i8254x->irq_line,&i_desc);

	reset_multicast_array(i8254x);
	rx_init_i8254x(i8254x);
	tx_init_i8254x(i8254x);

	write_i8254x(i8254x,REG_IMS,IMS_RXT0);
	read_i8254x(i8254x,REG_ICR);
	return i8254x;
}

void free_8254x(t_i8254x* i8254x)
{
	kfree(i8254x);
}

void int_handler_i8254x()
{
	t_i8254x* i8254x;
	struct t_processor_reg processor_reg;
	u16 cur;
	u16 old_cur;
	u32 status;
	t_rx_desc_i8254x* rx_desc;
	char* frame_addr;
	u32 low_addr;
	u32 hi_addr;
	u16 frame_len;
	t_data_sckt_buf* data_sckt_buf;
	t_sckt_buf_desc* sckt_buf_desc;

	SAVE_PROCESSOR_REG
	i8254x=system.network_desc->dev;
	disable_irq_line(i8254x->irq_line);
	DISABLE_PREEMPTION
	EOI_TO_SLAVE_PIC
	EOI_TO_MASTER_PIC
//	STI

	status=read_i8254x(i8254x,REG_ICR);
	if (status & ICR_LSC)
	{
		start_link_i8254x(i8254x);
	}
	else if (status & ICR_RXT0)
	{
		sckt_buf_desc=system.network_desc->rx_queue;
		cur=i8254x->rx_cur;
		rx_desc=i8254x->rx_desc;
		while(rx_desc[cur].status & 0x1)
		{
			//i use 32 bit addressing
			low_addr=rx_desc[cur].low_addr;
			hi_addr=rx_desc[cur].hi_addr;
			frame_addr=FROM_PHY_TO_VIRT(low_addr);
			frame_len=rx_desc[cur].length;

			//ALLOC IN ADVANCE BUFFER TO AVOID MEMCPY
			data_sckt_buf=frame_addr-MTU_ETH;
			data_sckt_buf=alloc_sckt(frame_len);
			data_sckt_buf->mac_hdr=data_sckt_buf;
			enqueue_sckt(system.network_desc->rx_queue,data_sckt_buf);
			rx_desc[cur].status=0;
			old_cur=cur;
			cur =(cur + 1) % NUM_RX_DESC;
			write_i8254x(i8254x,RDT_REG,old_cur);
		}
	}
	i8254x->rx_cur=cur;
	write_i8254x(i8254x,RDT_REG,old_cur);
	enable_irq_line(i8254x->irq_line);
	ENABLE_PREEMPTION
	EXIT_INT_HANDLER(0,processor_reg)
}

void send_packet_i8254x(t_i8254x* i8254x,void* frame_addr,u16 frame_len)
{
	u32 phy_frame_addr;
	u16 cur;
	t_tx_desc_i8254x* tx_desc;

	cur=i8254x->tx_cur;
	tx_desc=i8254x->tx_desc;
	phy_frame_addr=FROM_VIRT_TO_PHY(frame_addr);

	tx_desc[cur].low_addr=phy_frame_addr;
	tx_desc[cur].hi_addr=0;
	tx_desc[cur].length=frame_len;
	tx_desc[cur].cso=0;
//	tx_desc[cur].cmd=CMD_EOP | CMD_RS | CMD_RPS;
 	tx_desc[cur].cmd=CMD_EOP | CMD_RPS;
	tx_desc[cur].status=0;
	tx_desc[cur].css=0;
	tx_desc[cur].special=0;

	i8254x->tx_cur = (cur + 1) % NUM_TX_DESC;	
	write_i8254x(i8254x,TDT_REG,i8254x->tx_cur);
	while(!(tx_desc[cur].status & 0xff));
}















