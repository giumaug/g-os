
static void write_i8254x(t_i8254x* i8254x,u32 address,u32 value)
{
	outdw(value,i8254x->mem_base+address);
}

static u32 read_i8254x(t_i8254x* i8254x,u32 adrress)
{
	return indw(i8254x,CTRL_REG);
}

static u16 read_eeprom_i8254x(t_i8254x* i8254x,u8 addr)
{
	u32 tmp;

	write_i8254x(i8254x,REG_EERD, 1 | ((u32)(addr)<< 8));
	while( !((tmp = read_i8254x(i8254x,REG_EERD)) & (1 << 4)));

	return (uint16_t)((tmp >> 16) & 0xFFFF);
}

static void read_mac_i8254x(t_i8254x* i8254x)
{
	u32 tmp;	

	tmp=read_i8254x(i8254x,0);
	tmp=(tmp && 0xf);
	i8254x->low_mac=read_i8254x(i8254x,0);

	tmp=read_i8254x(i8254x,1);
	tmp=(tmp && 0xf)<<8;
	i8254x->low_mac | = tmp;

	tmp=read_i8254x(i8254x,2);
	tmp=(tmp && 0xf)<<16;
	i8254x->low_mac | = tmp;

	tmp=read_i8254x(i8254x,3);
	tmp=(tmp && 0xf)<<24;
	i8254x->low_mac | = tmp;

	tmp=read_i8254x(i8254x,4);
	tmp=(tmp && 0xf);
	i8254x->hi_mac=read_i8254x(i8254x,0);

	tmp=read_i8254x(i8254x,5);
	tmp=(tmp && 0xf)<<8;
	i8254x->hi_mac | = tmp;
}

static rx_init_i8254x(t_i8254x* i8254x)
{
	int i;
	char* rx_desc_ring;
	t_rx_desc_i8254x* rx_desc;

	rx_desc_ring=kmalloc(sizeof(t_rx_desc_i8254x)*NUM_RX_DESC);
	rx_desc=rx_desc_ring;
	for (i=0;i<NUM_RX_DESC;i++)
	{
		rx_desc[i]->hi_addr=0;
		rx_desc[i]->low_addr=kmalloc(RX_BUF_SIZE);
		rx_desc[i]->status=0;
	}
	write_i8254x(i8254x,RDBAL_REG,rx_desc_ring);
	write_i8254x(i8254x,RDBAH_REG,0);
	write_i8254x(i8254x,RDLEN,NUM_RX_DESC*16);
	
	write_i8254x(i8254x,RHD,0);
	write_i8254x(i8254x,RDT,NUM_RX_DESC-1);

	write_i8254x(i8254x,REG_RCTRL, (RCTL_EN | RCTL_SBP | RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC  | RCTL_BSIZE_8192));
}

static tx_init_i8254x(t_i8254x* i8254x)
{
	int i;
	char* tx_desc_ring;
	t_tx_desc_i8254x* tx_desc;

	tx_desc_ring=kmalloc(sizeof(t_tx_desc_i8254x)*NUM_TX_DESC);
	tx_desc=tx_desc_ring;
	for (i=0;i<NUM_TX_DESC;i++)
	{
		tx_desc[i]->hi_addr=0;
		tx_desc[i]->low_addr=kmalloc(RX_BUF_SIZE);no !!!! uso socket_buff addr
		tx_desc[i]->status=TSTA_DD;
		tx_desc[i]->cmd=0;
	}
	write_i8254x(i8254x,TDBAL_REG,rx_desc_ring);
	write_i8254x(i8254x,TDBAH_REG,0);
	write_i8254x(i8254x,TDLEN,NUM_RX_DESC*16);

	write_i8254x(i8254x,THD,0);
	write_i8254x(i8254x,TDT,NUM_RX_DESC-1);

	write_i8254x(i8254x,REG_TCTRL,TCTL_EN,
			    | TCTL_PSP
        		    | (15 << TCTL_CT_SHIFT)
        		    | (64 << TCTL_COLD_SHIFT)
          		    | TCTL_RTLC);
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

void init_8254x(t_i8254x* i8254x)
{
	struct t_i_desc i_desc;

	i8254x->mem_base=read_pci_config_word(I8254X_BUS,I8254X_SLOT,I8254X_FUNC,I8254X_MEM_BASE);
	i8254x->irq_line=read_pci_config_word(I8254X_BUS,I8254X_SLOT,I8254X_FUNC,I8254X_IRQ_LINE);
	read_mac_i8254x(i8254x);
	start_link_i8254x(i8254x);

	i_desc.baseLow=((int)&int_handler_ata) & 0xFFFF;
	i_desc.selector=0x8;
	i_desc.flags=0x08e00;
	i_desc.baseHi=((int)&int_handler_ata)>>0x10;
	set_idt_entry(0x20+i8254x->irq_line,&int_handler_i8254x);

	reset_multicast_array(i8254x);
	rx_init_i8254x(i8254x);
	tx_init_i8254x(i8254x);
}

void free_8254x(t_i8254x* i8254x)
{

}

void int_handler_i8254x(t_i8254x* i8254x)
{
	u32 status;

	status=read_i8254x(i8254x,REG_ICR);
	if (status & ICR_LSC)
	{
		start_link_i8254x(i8254x);
	}
	else if (status & ICR_RXT0)
	{

	}

}

void send_packet_i8254x()
{

}
