#include "asm.h" 
#include "idt.h"
#include "virtual_memory/vm.h"
#include "drivers/ata/ata.h"

static int race=0;

void static int_handler_ata();

void init_ata(t_device_desc* device_desc)
{	
	struct t_i_desc i_desc;
	
	i_desc.baseLow=((int)&int_handler_ata) & 0xFFFF;
	i_desc.selector=0x8;
	i_desc.flags=0x08e00;
	i_desc.baseHi=((int)&int_handler_ata)>>0x10;
	set_idt_entry(0x2E,&i_desc);
	device_desc->read=_read_28_ata;
	device_desc->write=_write_28_ata;
	device_desc->p_read=_p_read_28_ata;
	device_desc->p_write=_p_write_28_ata;
	device_desc->status=DEVICE_IDLE;
	sem_init(&device_desc->mutex,1);
	sem_init(&device_desc->sem,0);
}

void free_ata(t_device_desc* device_desc)
{
	device_desc->status=DEVICE_IDLE;
	sem_down(&device_desc->mutex);
}

void int_handler_ata()
{
	struct t_processor_reg processor_reg;
	t_io_request* io_request;
	struct t_process_context* process_context;
	struct t_process_context* current_process_context;

	SAVE_PROCESSOR_REG
	disable_irq_line(14);
	DISABLE_PREEMPTION
	EOI_TO_SLAVE_PIC
	EOI_TO_MASTER_PIC
	STI

	io_request=system.device_desc->serving_request;
	process_context=io_request->process_context;
	CURRENT_PROCESS_CONTEXT(current_process_context);

	if (system.device_desc->status!=POOLING_MODE)
	{
		sem_up(&io_request->device_desc->sem);
	}
	if (current_process_context->pid != process_context->pid) 
	{
	 	system.force_scheduling = 1;
	}
	system.device_desc->status=DEVICE_IDLE;
	enable_irq_line(14);
	ENABLE_PREEMPTION
//	EXIT_INT_HANDLER(0,processor_reg)
	int action = 0;
	static struct t_process_context _current_process_context;                                                  		
	static struct t_process_context _old_process_context;                                                      		
	static struct t_process_context _new_process_context;	                                                   		
	static struct t_processor_reg _processor_reg;                                                              		
	static unsigned int _action2;                                                                              		
                                                                                                                   		
	CLI                                                                                                        		
	_action2=action;                                                                                           		
	_current_process_context=*(struct t_process_context*)system.process_info->current_process->val;             		
	_old_process_context=_current_process_context;                                                             		
	_processor_reg=processor_reg;                                                                              		
	if (system.force_scheduling == 1 && action == 0 && system.int_path_count == 0)                                          
	{                                                                                                                       
		_action2 = 1;                                                                                                   
	}                                                                                                                       
	system.force_scheduling = 0;                                                                                            
                                                                                                                                
	if (_action2>0)                                                                                            		
	{                                                                                                          		
		schedule(&_current_process_context,&_processor_reg);                                               		
		_new_process_context=*(struct t_process_context*)system.process_info->current_process->val;         		
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

static unsigned int _read_write_28_ata(t_io_request* io_request)
{
	int i;
	t_device_desc* device_desc;
	t_io_request* pending_request;
	t_llist_node* node;
	int k=0;
	int s;
	
	device_desc=io_request->device_desc;
	//Entrypoint mutual exclusion region
	sem_down(&device_desc->mutex);
	device_desc->status=DEVICE_BUSY;
	system.device_desc->serving_request=io_request;
	
	out(0xE0 | (io_request->lba >> 24),0x1F6);
	out((unsigned char)io_request->sector_count,0x1F2);
	out((unsigned char)io_request->lba,0x1F3);
	out((unsigned char)(io_request->lba >> 8),0x1F4);
	out((unsigned char)(io_request->lba >> 16),0x1F5);
	out(io_request->command,0x1F7);
	
	for (k=0;k<1000;k++);

	//to fix
	if (io_request->command==WRITE_28)
	{
		for (i=0;i<256;i++)
		{  
			//out(*(char*)io_request->io_buffer++,0x1F0); 
			outw((unsigned short)57,0x1F0);
		}
	}
	
	//one interrupt for each block
	for (k=0;k<io_request->sector_count;k++)
	{
		//semaphore to avoid race with interrupt handler
		sem_down(&device_desc->sem);

		if ((in(0x1F7)&1))
		{
			device_desc->status=DEVICE_IDLE;
			panic();
			return -1;
		}
		if (io_request->command==READ_28)
		{
			for (i=0;i<512;i+=2)
			{  
				unsigned short val=inw(0x1F0);
				((char*)io_request->io_buffer)[i+(512*k)]=(val&0xff);
				((char*)io_request->io_buffer)[i+1+(512*k)]=(val>>0x8);
			}
		i=0;
		}
	}
	//Endpoint mutual exclusion region
	sem_up(&device_desc->mutex);	
	return 0;
}

//MODIFIED VERSION TO TEST PERFORMANCE WITH MULTISECTORE READS (SEE _read_test(t_ext2* ext2) in ext2.c)
static unsigned int ___read_write_28_ata(t_io_request* io_request)
{
	int i;
	t_device_desc* device_desc;
	t_io_request* pending_request;
	t_llist_node* node;
	int k=0;
	int s;
	
	device_desc=io_request->device_desc;
	//Entrypoint mutual exclusion region
	sem_down(&device_desc->mutex);
	race++;
	
	device_desc->status=DEVICE_BUSY;
	system.device_desc->serving_request=io_request;
	
        out(2, 0x3F6);
	out(0xE0 | (io_request->lba >> 24),0x1F6);
	//io_request->sector_count=2;
	out((unsigned char)io_request->sector_count,0x1F2);
	out((unsigned char)io_request->lba,0x1F3);
	out((unsigned char)(io_request->lba >> 8),0x1F4);
	out((unsigned char)(io_request->lba >> 16),0x1F5);
	//out(io_request->command,0x1F7);
	if (io_request->command==READ_28) 
	{
		out(0xc4,0x1F7);
	}
	for (k=0;k<1000;k++);

	//to fix
	if (io_request->command==WRITE_28)
	{
		for (i=0;i<256;i++)
		{  
			//out(*(char*)io_request->io_buffer++,0x1F0); 
			outw((unsigned short)57,0x1F0);
		}
	}
	
	//one interrupt for each block
	for (k=0;k<io_request->sector_count;k++)
	{
		//semaphore to avoid race with interrupt handler
		sem_down(&device_desc->sem);

		while ((in(0x1F7)&0x83)!=0);

		if ((in(0x1F7)&1))
		{
			device_desc->status=DEVICE_IDLE;
			panic();
			return -1;
		}
		if (io_request->command==READ_28)
		{
			for (i=0;i<512;i+=2)
			{  
				unsigned short val=inw(0x1F0);
				((char*)io_request->io_buffer)[i+(512*k)]=(val&0xff);
				((char*)io_request->io_buffer)[i+1+(512*k)]=(val>>0x8);
			}
		i=0;
		}
	}
	race--;
	if (race>0)
	{
		panic();
	}
	//Endpoint mutual exclusion region
	sem_up(&device_desc->mutex);	
	return 0;
}

static unsigned int _p_read_write_28_ata(t_io_request* io_request)
{
	int i;
	t_device_desc* device_desc;
	t_io_request* pending_request;
	t_llist_node* node;
	t_spinlock_desc spinlock;
	int k=0;

	SPINLOCK_INIT(spinlock);
	device_desc=io_request->device_desc;
	
	//Entrypoint mutual exclusion region
	SPINLOCK_LOCK(spinlock);
	
	device_desc->status=DEVICE_BUSY || POOLING_MODE;
	system.device_desc->serving_request=io_request;

	out(0x2,0x3F6);
	out(0xE0 | (io_request->lba >> 24),0x1F6);
	out((unsigned char)io_request->sector_count,0x1F2);
	out((unsigned char)io_request->lba,0x1F3);
	out((unsigned char)(io_request->lba >> 8),0x1F4);
	out((unsigned char)(io_request->lba >> 16),0x1F5);
	out(io_request->command,0x1F7);
	for (k=0;k<1000;k++);

	//to fix
	if (io_request->command==WRITE_28)
	{
		for (i=0;i<256;i++)
		{  
			//out(*(char*)io_request->io_buffer++,0x1F0); 
			outw((unsigned short)57,0x1F0);
		}
	}
	while (in(0x1F7)&0x80);

	if ((in(0x1F7)&0x21))
	{
		device_desc->status=DEVICE_IDLE;
		panic();
		return -1;
	}

	device_desc->status=DEVICE_IDLE;
	
	if (io_request->command==READ_28)
	{
		for (i=0;i<(512*io_request->sector_count);i+=2)
		{  
			unsigned short val=inw(0x1F0);
			((char*)io_request->io_buffer)[i]=(val&0xff);
			((char*)io_request->io_buffer)[i+1]=(val>>0x8);
		}
	}
	out(0x0,0x3F6);
	//Exitpoint mutual exclusion region
	SPINLOCK_UNLOCK(spinlock);
	return 0;
}

unsigned int _read_28_ata(t_io_request* io_request)
{
	io_request->command=READ_28;
	return _read_write_28_ata(io_request);	
}

unsigned int _write_28_ata(t_io_request* io_request)
{
	io_request->command=WRITE_28;
	return _read_write_28_ata(io_request);	
}

unsigned int _p_read_28_ata(t_io_request* io_request)
{
	io_request->command=READ_28;
	return _p_read_write_28_ata(io_request);	
}

unsigned int _p_write_28_ata(t_io_request* io_request)
{
	io_request->command=WRITE_28;
	return _p_read_write_28_ata(io_request);	
}



int _flush_ata_pending_request()
{
	int ret=0;
	if (system.device_desc->serving_request->process_context!=NULL)
	{
		_awake(system.device_desc->serving_request->process_context);
		ret=1;
	}
	return ret;
}
