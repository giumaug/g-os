#include "drivers/i8254x/i8254x.h"
#include "virtual_memory/vm.h"
#include "drivers/lapic/lapic.h"

#define EEPROM_IMPLEMENTED 0

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

static start_link_i8254x(t_i8254x* i8254x);

static u32 read_i8254x(t_i8254x* i8254x,u32 address)
{
	u32 val;
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

	if (EEPROM_IMPLEMENTED)
	{
		tmp=read_eeprom_i8254x(i8254x,2);	
		i8254x->mac_addr.lo=(tmp>>8) | (tmp<<8);
	
		tmp=read_eeprom_i8254x(i8254x,1);	
		i8254x->mac_addr.mi=(tmp>>8) | (tmp<<8);

		tmp=read_eeprom_i8254x(i8254x,0);	
		i8254x->mac_addr.hi=(tmp>>8) | (tmp<<8);
	}
	else
	{
		//Manual MAC
		i8254x->mac_addr.lo = 0x0000;
		i8254x->mac_addr.mi = 0xa000;
		i8254x->mac_addr.hi = 0x0017;
	}
}

void int_handler_i8254x();

static rx_init_i8254x(t_i8254x* i8254x)
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
	write_i8254x(i8254x,TDLEN_REG,NUM_TX_DESC*16);
	i8254x->tx_cur=0;

	write_i8254x(i8254x,THD_REG,0);
	write_i8254x(i8254x,TDT_REG,0);
	write_i8254x(i8254x,TCTRL_REG,(TCTL_EN| TCTL_PSP | (15 << TCTL_CT_SHIFT) | (64 << TCTL_COLD_SHIFT) | TCTL_RTLC));
	//82567LF-2 ONLY
    write_i8254x(i8254x,0x410,  0x00702008);
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
	}

	i8254x->irq_line=read_pci_config_word(I8254X_BUS,I8254X_SLOT,I8254X_FUNC,I8254X_IRQ_LINE-3) & 0xFF;
	read_mac_i8254x(i8254x);
	start_link_i8254x(i8254x);

	i_desc.baseLow=((int)&int_handler_i8254x) & 0xFFFF;
	i_desc.selector=0x8;
	i_desc.flags=0x08e00;
	i_desc.baseHi=((int)&int_handler_i8254x)>>0x10;
	set_idt_entry(0x30,&i_desc);

	reset_multicast_array(i8254x);
	rx_init_i8254x(i8254x);
	tx_init_i8254x(i8254x);
	write_i8254x(i8254x,REG_IMS,(IMS_RXT0 | IMS_RXO));
	return i8254x;
}

void free_8254x(t_i8254x* i8254x)
{
	//need to release all buffers
	kfree(i8254x);
}

void int_handler_i8254x()
{
	struct t_process_context* tmp;
	t_i8254x* i8254x;
	u16 cur;
	u16 old_cur;
	u32 status;
	t_rx_desc_i8254x* rx_desc;
	char* frame_addr;
	u32 low_addr;
	u32 hi_addr;
	u16 frame_len;
	t_data_sckt_buf* data_sckt_buf;
	char* data_buffer;
	struct t_processor_reg processor_reg;
	u32 int_count = 0;

	SAVE_PROCESSOR_REG
	i8254x=system.network_desc->dev;
	mask_entry(16);
	DISABLE_PREEMPTION
	EOI_TO_LAPIC
	STI

	status=read_i8254x(i8254x,REG_ICR);
	if (status & ICR_LSC)
	{
		start_link_i8254x(i8254x);
		printk("Link status changed!!! \n");
	}
	else if(status & ICR_RXO)
	{
		printk("Overrun!!!! \n");
	}
	else if (status & ICR_RXT0)
	{
		cur=i8254x->rx_cur;
		rx_desc=i8254x->rx_desc;
		old_cur=cur;
		while(rx_desc[cur].status & 0x1)
		{
			int_count++;
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

			CLI
			enqueue_sckt(system.network_desc->rx_queue,data_sckt_buf);
			STI
			
			rx_desc[cur].status=0;
			old_cur=cur;
			cur =(cur + 1) % NUM_RX_DESC;
			i8254x->rx_cur=cur;
			write_i8254x(i8254x,RDT_REG,old_cur);
		}
	}
exit:
	unmask_entry(16);
	ENABLE_PREEMPTION
	CLI
	system.flush_network = 1;
	//--EXIT_INT_HANDLER(0,processor_reg)

	static struct t_process_context _current_process_context;                                                  	
	static struct t_process_context _old_process_context;                                                      	
	static struct t_process_context _new_process_context;	                                                        
	static struct t_processor_reg _processor_reg;                                                                   
	static unsigned int _action2; 
	static u8 stop = 0;                                                                              
                                                                                                                 
	CLI                                                                                                             
	if (system.int_path_count == 0 && system.force_scheduling == 0 && system.flush_network == 1)                    
	{                                                                                                               
                        system.flush_network = 0;                                                                       
			dequeue_packet(system.network_desc);                                                            
			equeue_packet(system.network_desc);                                                             
                        system.flush_network = 1;                                                                       
	}                                                                                                               
	_action2=0;                                                                                      
	_current_process_context=*(struct t_process_context*)system.process_info->current_process->val;                 
	_old_process_context=_current_process_context;                                                                  
	_processor_reg=processor_reg;                                                                                   
	if (system.force_scheduling == 1 && 0 == 0 && system.int_path_count == 0)                                  
	{                                                                                                               
		_action2 = 1;                                                                                           
		if (_current_process_context.proc_status == EXITING)                                                    
		{                                                                                                       
			_action2 = 2;                                                                                   
		}                                                                                                       
	}                                                                                                               
                                                                                                                        
	if (_action2>0)                                                                                                 
	{	system.force_scheduling = 0;
		stop = 0;                                                                            
		while(!stop)                                                                                             
		{                                                                                                       
			schedule(&_current_process_context,&_processor_reg);                                            
			_new_process_context = *(struct t_process_context*) system.process_info->current_process->val;  
			if (_new_process_context.sig_num == SIGINT)                                                    
			{                                                                                              
				_exit(0);
				free_vm_process(&_new_process_context);                                                         
				buddy_free_page(system.buddy_desc,FROM_PHY_TO_VIRT(_new_process_context.phy_kernel_stack));           
			}                                                                                               
			else                                                                                            
			{                                                                                               
				stop = 1;                                                                               
			}                                                                                               
		}       
		if (_new_process_context.pid != _old_process_context.pid)                                               
		{                                                                                                       
				_processor_reg=_new_process_context.processor_reg;                                      
		}                                                                                                       
		SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) _new_process_context.page_dir)))                       
		DO_STACK_FRAME(_processor_reg.esp-8);                                                                   
                                                                                                                        
		if (_action2==2)                                                                                        
		{                                                                                                       
			DO_STACK_FRAME(_processor_reg.esp-8);                                                           
			free_vm_process(&_old_process_context);                                                         
			buddy_free_page(system.buddy_desc,FROM_PHY_TO_VIRT(_old_process_context.phy_kernel_stack));     
		}                                                                                                       
		RESTORE_PROCESSOR_REG                                                                                   
		EXIT_SYSCALL_HANDLER                                                                                    
	}                                                                                                          	
	else                                                                                                       	
	{                                                                                                               
		RESTORE_PROCESSOR_REG                                                                                   
		RET_FROM_INT_HANDLER                                                                                    
	}
}

void send_packet_i8254x(t_i8254x* i8254x,void* frame_addr,u16 frame_len)
{
	u32 phy_frame_addr;
	u16 cur;
	t_tx_desc_i8254x* tx_desc;
	
	//TMP TEST
	SAVE_IF_STATUS
	CLI

	cur=i8254x->tx_cur;
	tx_desc=i8254x->tx_desc;
	phy_frame_addr=FROM_VIRT_TO_PHY(frame_addr);

	tx_desc[cur].low_addr=phy_frame_addr;
	tx_desc[cur].hi_addr=0;
	tx_desc[cur].length=frame_len;
	tx_desc[cur].cso=0;
 	tx_desc[cur].cmd=CMD_EOP | CMD_RS | CMD_IFCS;
	tx_desc[cur].status=0;
	tx_desc[cur].css=0;
	tx_desc[cur].special=0;
	
	i8254x->tx_cur = (cur + 1) % NUM_TX_DESC;	
	write_i8254x(i8254x,TDT_REG,i8254x->tx_cur);
	//DON'T WAIT TO AVOID RACE WITH INTERRUPT HANDLER?????
	RESTORE_IF_STATUS
}

void pooling()
{
	struct t_process_context* tmp;
	t_i8254x* i8254x;
	u16 cur;
	u16 old_cur;
	u32 status;
	t_rx_desc_i8254x* rx_desc;
	char* frame_addr;
	u32 low_addr;
	u32 hi_addr;
	u16 frame_len;
	t_data_sckt_buf* data_sckt_buf;
	char* data_buffer;
	struct t_processor_reg processor_reg;
	u32 int_count = 0;

	i8254x=system.network_desc->dev;
	status=read_i8254x(i8254x,REG_ICR);
	cur=i8254x->rx_cur;
	rx_desc=i8254x->rx_desc;
	old_cur=cur;
    while(rx_desc[cur].status & 0x1)
	{
		int_count++;
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
		i8254x->rx_cur=cur;
		write_i8254x(i8254x,RDT_REG,old_cur);
	}
}
