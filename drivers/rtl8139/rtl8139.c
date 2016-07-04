#include "drivers/i8254x/i8254x.h"
#include "virtual_memory/vm.h"

static void write_rtl8139(t_i8254x* i8254x,u32 address,u32 value)
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

static start_link_rtl8139(t_i8254x* i8254x);

static u32 read_rtl8139(t_i8254x* i8254x,u32 address)
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

static void read_mac_rtl8139(t_i8254x* i8254x)
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

static rx_init_rtl8139(t_i8254x* i8254x)
{
	int i;
	t_rx_desc_i8254x* rx_desc;
	char* data_buffer;
	
	rx_desc=kmalloc(16 + sizeof(t_rx_desc_i8254x) * NUM_RX_DESC);
	rx_desc = ((u32)rx_desc + 16) - ((u32)rx_desc % 16);
	for (i=0;i<NUM_RX_DESC;i++)
	{
		data_buffer=kmalloc(MTU_ETH);
		rx_desc[i].hi_addr=0;
		rx_desc[i].low_addr=FROM_VIRT_TO_PHY((u32)data_buffer);
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

static tx_init_rtl8139(t_i8254x* i8254x)
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
	write_i8254x(i8254x,TDLEN_REG,NUM_TX_DESC*16);
	i8254x->tx_cur=0;

	write_i8254x(i8254x,THD_REG,0);
	write_i8254x(i8254x,TDT_REG,0);
	write_i8254x(i8254x,TCTRL_REG,(TCTL_EN| TCTL_PSP | (15 << TCTL_CT_SHIFT) | (64 << TCTL_COLD_SHIFT) | TCTL_RTLC));
}

static start_link_rtl8139(t_i8254x* i8254x)
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

t_i8254x* init_rtl8139()
{
	struct t_process_context* current_process_context;
	t_rtl8139* rtl8139=NULL;
	struct t_i_desc i_desc;
	u32* bar0;

	rtl8139=kmalloc(sizeof(t_rtl8139));
	bar0=read_pci_config_word(RTL8139_BUS,RTL8139_SLOT,RTL8139_FUNC,RTL8139_BAR0);
	if ((u32)bar0 & 0x1) 
	{
		rtl8139->io_base=(u32)bar0 & 0xFFFC;
		rtl8139->mem_base=NULL;
		rtl8139->bar_type=1;
	}
	else 
	{
		rtl8139->mem_base=RTL8139_VIRT_BAR0_MEM;
		rtl8139->io_base=NULL;
		rtl8139->bar_type=0;
		CURRENT_PROCESS_CONTEXT(current_process_context);
		map_vm_mem(current_process_context->page_dir,RTL8139_VIRT_BAR0_MEM,(((u32) (bar0)) & 0xFFFFFFF0),RTL8139_VIRT_BAR0_MEM_SIZE,3);
		SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) current_process_context->page_dir))) 
	}

	rtl8139->irq_line=read_pci_config_word(I8254X_BUS,I8254X_SLOT,I8254X_FUNC,I8254X_IRQ_LINE-3) & 0xFF;
	read_mac_rtl8139(rtl8139);
	start_link_rtl8139(rtl8139);

	i_desc.baseLow=((int)&int_handler_rtl8139) & 0xFFFF;
	i_desc.selector=0x8;
	i_desc.flags=0x08e00;
	i_desc.baseHi=((int)&int_handler_rtl8139)>>0x10;
	set_idt_entry(0x20+rtl8139->irq_line,&i_desc);

	reset_multicast_array(i8254x);
	rx_init_i8254x(i8254x);
	tx_init_i8254x(i8254x);

	write_i8254x(i8254x,REG_IMS,(IMS_RXT0 | IMS_RXO));
	read_i8254x(i8254x,REG_ICR);
	return i8254x;
}

void free_rtl8139(t_i8254x* i8254x)
{
    //need to release all buffers
	kfree(i8254x);
}

void int_handler_rtl8139()
{
	U_N_INT(3) u_3;
	u_3 tail;
	u32 rx_buf_size;
	int diff;
	t_i8254x* i8254x;
	struct t_processor_reg processor_reg;
	u16 cur;
	u16 old_cur;
	u32 status;
	u32 head;
	t_rx_desc_i8254x* rx_desc;
	char* frame_addr;
	u32 low_addr;
	u32 hi_addr;
	u16 frame_len;
	t_data_sckt_buf* data_sckt_buf;
	char* data_buffer;

	SAVE_PROCESSOR_REG
	i8254x=system.network_desc->dev;
	disable_irq_line(i8254x->irq_line);
	DISABLE_PREEMPTION
	EOI_TO_SLAVE_PIC
	EOI_TO_MASTER_PIC
//	STI occhio!!!!

	static int int_count=0;
	int_count++;

	//printk("in ... \n");
	//printk("int num=%d \n",int_count);

//	old_cur=7;//i8254x->rx_cur;
//	//tail.val=old_cur+NUM_RX_DESC-1;
//	head=read_i8254x(i8254x,RHD_REG);
//	//u32 tmp=read_i8254x(i8254x,RHD_REG);
//	printk("head is:%d \n",head);
//	printk("tail is:%d \n",old_cur);
//
//	diff=head-old_cur;
//	//abs
//	rx_buf_size=diff * ((diff>0) - (diff<0));
//	if(rx_buf_size<2)
//	{
//		write_i8254x(i8254x,RDT_REG,1);
//		goto exit;
//	}

	status=read_i8254x(i8254x,REG_ICR);


	if (status==0) 
	{
		printk("alert!!!!! \n");
	}

	if (status & ICR_LSC)
	{
		start_link_i8254x(i8254x);
	}
	else if(status & ICR_RXO)
	{
		printk("overrun!!!! \n");
	}
	else if (status & ICR_RXT0)
	{
		cur=i8254x->rx_cur;
		rx_desc=i8254x->rx_desc;
		while(rx_desc[cur].status & 0x1)
		{
			printk("flush from int \n");
			//I use 32 bit addressing
			low_addr=rx_desc[cur].low_addr;
			hi_addr=rx_desc[cur].hi_addr;
			frame_addr=FROM_PHY_TO_VIRT(low_addr);
			frame_len=rx_desc[cur].length;

			data_sckt_buf=alloc_void_sckt();
			data_sckt_buf->mac_hdr=frame_addr;
			data_sckt_buf->data=frame_addr;
			data_sckt_buf->data_len=frame_len;

			data_buffer=kmalloc(MTU_ETH);
			rx_desc[cur].hi_addr=0;
			rx_desc[cur].low_addr=FROM_VIRT_TO_PHY((u32)data_buffer);
			rx_desc[cur].status=0;
			rx_desc[cur].length=0;
			rx_desc[cur].checksum=0;
			rx_desc[cur].errors=0;
			rx_desc[cur].special=0;

			enqueue_sckt(system.network_desc->rx_queue,data_sckt_buf);
			rx_desc[cur].status=0;
			old_cur=cur;
			cur =(cur + 1) % NUM_RX_DESC;
		}
	}
	i8254x->rx_cur=cur;
	//write_i8254x(i8254x,RDT_REG,old_cur);
	//printk("out ... \n");
exit:
	enable_irq_line(i8254x->irq_line);
	ENABLE_PREEMPTION
	EXIT_INT_HANDLER(0,processor_reg)
}



void testx()
{
	int index=0;
	int found=0;
	int i;
	t_i8254x* i8254x;
	struct t_processor_reg processor_reg;
	u16 cur;
	u16 old_cur;
	u32 status;
	u32 head;
	t_rx_desc_i8254x* rx_desc;
	char* frame_addr;
	u32 low_addr;
	u32 hi_addr;
	u16 frame_len;
	t_data_sckt_buf* data_sckt_buf;
	char* data_buffer;

	i8254x=system.network_desc->dev;
	cur=i8254x->rx_cur;
	rx_desc=i8254x->rx_desc;
	//while(rx_desc[cur].status & 0x1)
	for (i=0;i<NUM_RX_DESC;i++)
	{
		if (rx_desc[i].status & 0x1)
		{
			found=1;
			index++;
			printk("flush \n");
			//I use 32 bit addressing
			low_addr=rx_desc[cur].low_addr;
			hi_addr=rx_desc[cur].hi_addr;
			frame_addr=FROM_PHY_TO_VIRT(low_addr);
			frame_len=rx_desc[i].length;

			data_sckt_buf=alloc_void_sckt();
			data_sckt_buf->mac_hdr=frame_addr;
			data_sckt_buf->data=frame_addr;
			data_sckt_buf->data_len=frame_len;

			data_buffer=kmalloc(MTU_ETH);
			rx_desc[i].hi_addr=0;
			rx_desc[i].low_addr=FROM_VIRT_TO_PHY((u32)data_buffer);
			rx_desc[i].status=0;
			rx_desc[i].length=0;
			rx_desc[i].checksum=0;
			rx_desc[i].errors=0;
			rx_desc[i].special=0;

			enqueue_sckt(system.network_desc->rx_queue,data_sckt_buf);
			rx_desc[i].status=0;
			//old_cur=i;
			//cur =(cur + 1) % NUM_RX_DESC;
		}
	}
	if (found==1)
	{
		if (index==5)
		{
			printk("detected overrun!!!\n");
		}
		i8254x->rx_cur=cur;
		write_i8254x(i8254x,RDT_REG,0);
	}
}

void send_packet_rtl8139(t_i8254x* i8254x,void* frame_addr,u16 frame_len)
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
	//DON'T WAIT TO AVOID RACE WITH INTERRUPT HANDLER?????
	long i=0;
//	printk("in... \n");
//	for (i=0;i<=1000000000;i++)
//	{
//		if (i==99) 
//		{
//			printk("tt \n");
//		}
//	}
//	printk("out... \n");
	//while(!(tx_desc[cur].status & 0xff));
}

