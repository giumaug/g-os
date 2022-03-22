#include "lapic.h"
#include "timer.h"

void init_lapic()
{
	u32 val;

	init_pit();
	// Clear Task Priority register; this enables all LAPIC interrupts
	val = read_config_word(LAPIC_TPR);
	val &= ~0xff;
	write_config_word(LAPIC_TPR, val);
	// Enable the LAPIC
	val = read_config_word(LAPIC_SVR);
    val |= LAPIC_SVR_APIC_EN;
	write_config_word(LAPIC_SVR, val);
	STI
	init_timer();
	CLI
	free_pit();
	start_timer();
}

init_timer()
{
	t_timer timer = NULL;
	u32 freq;
	u32 max_timer_count = 0xffffffff;

	t_timer timer = timer_init(0, NULL, NULL, NULL);
	for (divisor = 2; divisor <= 128; divisor *= 2)
	{
		set_timer_divisor(divisor);
		write_config_word(LAPIC_LVT_TMR, LAPIC_LVT_TMR_ONESHOT | LAPIC_LVT_TMR_M);
		write_config_word(LAPIC_ICR, max_timer_count);
		timer_set(timer, ONE_SEC_DELAY);
		while (timer->val > 0 );
		freq = max_timer_count - read_config_word(LAPIC_CCR);
		if (freq != max_timer_count) 
		{
			break;
		}
	}
	if (divisor > 128)
	{
		panic();
	}
}

void set_timer_divisor(int divisor)
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
	write_config_word(LAPIC_LVT_DCR, dv);
}

static u32 read_config_word()
{

}

static void write_config_word()
{

}
