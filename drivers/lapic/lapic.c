#include "timer.h"
#include "asm.h"  
#include "idt.h" 
#include "virtual_memory/vm.h"
#include "drivers/lapic/lapic.h"
#include "drivers/pit/8253.h" 

static u32 interrupt_counter = 0;

extern int counter;

static void init_timer();
static void int_handler_lapic();
static u32 read_reg(u32 reg_offset);
static void write_reg(u32 reg_offset, u32 val);
static void set_timer_divisor(int divisor);

void enable_irq_line()
{
        //FAKE JUST TO COMPILE
}

void disable_irq_line()
{
        //FAKE JUST TO COMPILE
}

void init_lapic()
{
	u32 val;
	
	map_vm_mem(system.master_page_dir, LAPIC_BASE, LAPIC_BASE, PAGE_SIZE,3);
	init_pit();
	// Clear Task Priority register; this enables all LAPIC interrupts
	val = read_reg(LAPIC_TPR);
	val &= ~0xff;
	write_reg(LAPIC_TPR, val);
	// Enable the LAPIC
	val = read_reg(LAPIC_SVR);
    val |= LAPIC_SVR_APIC_EN;
	write_reg(LAPIC_SVR, val);
	STI
	init_timer();
	CLI
	free_pit();
}

static void init_timer()
{
	static struct t_i_desc i_desc;
	t_timer* timer = NULL;
	u32 lapic_freq;
	u32 max_timer_count = 0xffffffff;
	u32 count;
	u32 divisor;

	timer = timer_init(0, NULL, NULL, NULL);
	for (divisor = 2; divisor <= 128; divisor *= 2)
	{
		set_timer_divisor(divisor);
		write_reg(LAPIC_TMR, LAPIC_TMR_ONESHOT | LAPIC_TMR_M);
		write_reg(LAPIC_ICR, max_timer_count);
		timer_set(timer, ONE_SEC_DELAY);
		while (timer->val > 0 );
		lapic_freq = max_timer_count - read_reg(LAPIC_CCR);
		if (lapic_freq != max_timer_count) 
		{
			break;
		}
	}
	if (divisor > 128)
	{
		panic();
	}

	i_desc.baseLow = ((int) &int_handler_lapic) & 0xFFFF;
	i_desc.selector = 0x8;
	i_desc.flags = 0x08e00;
	i_desc.baseHi =((int) &int_handler_lapic) >> 0x10;
	set_idt_entry(0x38, &i_desc);

	count = lapic_freq / TICK_FRQ; //lapic_freq / count = TICK_FRQ
	write_reg(LAPIC_TMR, LAPIC_TMR_PERIODIC | LAPIC_TMR_VECTOR);
	write_reg(LAPIC_ICR, count);
}

static void set_timer_divisor(int divisor)
{
	unsigned int dv;
	switch (divisor) 
	{
     	case 1:
     		dv = LAPIC_LVT_DV_1;
			break;
       	case 2:
       		dv = LAPIC_LVT_DV_2;
			break;
       	case 4:
      		dv = LAPIC_LVT_DV_4;
			break;
       	case 8:
       		dv = LAPIC_LVT_DV_8;
			break;
      	case 16:
     		dv = LAPIC_LVT_DV_16;
			break;
      	case 32:
     		dv = LAPIC_LVT_DV_32;
			break;
      	case 64:
     		dv = LAPIC_LVT_DV_64;
			break;
    	case 128:
     		dv = LAPIC_LVT_DV_128;
			break;
   		default:
      		panic();
			break;
	}
	write_reg(LAPIC_DCR, dv);
}

static u32 read_reg(u32 reg_offset)
{
	unsigned char* address = NULL;
	u32 val;

	address = LAPIC_BASE + reg_offset;
	val = *((volatile u32*)(address));
	return val;
}

static void write_reg(u32 reg_offset, u32 val)
{
	unsigned char* address = NULL;

	address = LAPIC_BASE + reg_offset;
	(*((volatile u32*)(address))) = (val);
}

static void int_handler_lapic()
{
	struct t_processor_reg processor_reg;
	struct t_processor_reg _processor_reg;

	SAVE_PROCESSOR_REG
	SWITCH_DS_TO_KERNEL_MODE
	
	if (interrupt_counter == 0)
	{
			interrupt_counter = 0;
	}

	interrupt_counter++;
	if(interrupt_counter == 1000)
	{
		interrupt_counter = 0;
	}
	_processor_reg = processor_reg;
	EOI_TO_LAPIC
	RESTORE_PROCESSOR_REG                                                                                   
	RET_FROM_INT_HANDLER
}
