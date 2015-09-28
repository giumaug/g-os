void init_8254x(t_i8254x* i8254x)
{
	struct t_i_desc i_desc;

	i8254x->mem_base=read_pci_config_word(I8254X_BUS,I8254X_SLOT,I8254X_FUNC,I8254X_MEM_BASE);
	i8254x->irq_line=read_pci_config_word(I8254X_BUS,I8254X_SLOT,I8254X_FUNC,I8254X_IRQ_LINE);
	read_mac_i8254x(i8254x);
	start_link_i8254x(i8254x);

	--------qui

	i_desc.baseLow=((int)&int_handler_ata) & 0xFFFF;
	i_desc.selector=0x8;
	i_desc.flags=0x08e00;
	i_desc.baseHi=((int)&int_handler_ata)>>0x10;
	set_idt_entry(0x20+i8254x->irq_line,&int_handler_i8254x);



}

void int_handler_i8254x()
{

}

void send_packet_i8254x()
{

}

static send_commnad_i8254x()
{

}

static read_mac_i8254x(t_i8254x* i8254x)
{

}

static rx_init_i8254x()
{

}

static tx_init_i8254x()
{

}

static start_link_i8254x(t_i8254x* i8254x)
{

}
