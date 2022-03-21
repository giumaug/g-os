#define LAPIC_BASE 0xfee00000;
#define LAPIC_DFR  0x00e0;
#define LAPIC_LDR  0x00d0;
#define LAPIC_TPR  0x0080;
#define LAPIC_SVR  0x00f0;
#define LAPIC_DCR 0x03e0     
#define LAPIC_TMR 0x0320
#define LAPIC_TMR_ONESHOT 0x0
#define LAPIC_TMR_M (1 << 16)
#define LAPIC_ICR 0x0380


#define LAPIC_SVR_APIC_EN = 0x100;
#define LAPIC_LVT_DV_1 0xb
#define LAPIC_LVT_DV_2 0x0
#define LAPIC_LVT_DV_4 0x1     
#define LAPIC_LVT_DV_8 0x2
#define LAPIC_LVT_DV_16 0x3
#define LAPIC_LVT_DV_32 0x8
#define LAPIC_LVT_DV_64 0x9
#define LAPIC_LVT_DV_128 0xa
 

void init_lapic()
{
	u32 val;

	// Clear Task Priority register; this enables all LAPIC interrupts
	val = read_config_word(LAPIC_TPR);
	val &= ~0xff;
	write_config_word(LAPIC_TPR, val);
	// Enable the LAPIC
	val = read_config_word(LAPIC_SVR);
    val |= LAPIC_SVR_APIC_EN;
	write_config_word(LAPIC_SVR, val);
	init_timer();
	start_timer();
}

init_timer()
{
	unsigned int max_timer_count = 0xffffffff;
	for (divisor = 2; divisor <= 128; divisor *= 2)
	{
		set_timer_divisor(divisor);
		write_config_word(LAPIC_LVT_TMR, LAPIC_LVT_TMR_ONESHOT | LAPIC_LVT_TMR_M);
		write_config_word(LAPIC_ICR, max_timer_count);

		WriteLAPIC(LAPIC_LVT_TR, LAPIC_LVT_TM_ONESHOT | LAPIC_LVT_M);
        WriteLAPIC(LAPIC_LVT_ICR, maxLAPICTimerCount);

	}

}

void set_timer_divisor(int divisor)
{
	int dv;
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

void InitTimer()
    {
        /*
         * First step is to calibrate the LAPIC's frequency - this is CPU/bus speed
         * dependant. We scale the divisor from 2..128 so that we cover all available
         * CPU ranges (inspired by FreeBSD)
         */
        int divisor = 2;
        for (/* nothing */; divisor <= 128; divisor *= 2) {
            SetLAPICTimerDivisor(divisor);
            WriteLAPIC(LAPIC_LVT_TR, LAPIC_LVT_TM_ONESHOT | LAPIC_LVT_M);
            WriteLAPIC(LAPIC_LVT_ICR, maxLAPICTimerCount);
            delay(1000);
            lapic_freq = maxLAPICTimerCount - ReadLAPIC(LAPIC_LVT_CCR);
            if (lapic_freq != maxLAPICTimerCount)
                break;
        }
        if (divisor > 128)
            panic("lapic divisor too large");
        kprintf("lapic: divisor %d, frequency %d Hz\n", divisor, lapic_freq);
        StartLAPICPeriodicTimer();
    }

static u32 read_config_word()
{

}

static void write_config_word()
{

}




