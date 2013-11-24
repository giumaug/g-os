#include "general.h"
#include "system.h"
#include "asm.h"
#include "scheduler/process.h"
#include "memory_manager/kmalloc.h"
#include "drivers/kbc/8042.h"
#include "console/console.h"

extern t_system system;
static void update_cursor(unsigned int curs_pos);

void init_console(t_console_desc *console_desc,int out_buf_len,int in_buf_len)
{
	int i,j=0;
	SPINLOCK_INIT(console_desc->spinlock);
	SPINLOCK_LOCK(console_desc->spinlock);
	i++;
	SPINLOCK_UNLOCK(console_desc->spinlock);
	console_desc->out_buf=kmalloc(out_buf_len);
	console_desc->out_buf_len=out_buf_len;
	console_desc->out_buf_index=-1;
	console_desc->video_buf=VIDEO_MEM_ADD;
	console_desc->video_buf_index=-1;
	console_desc->first_char=1;
	console_desc->sleeping_process=NULL;
	console_desc->is_empty=1;
	for (i=0;i<SCREEN_AREA;i++)
	{
		console_desc->video_buf[j++]=CHAR_NULL;
		console_desc->video_buf[j++]=SCREEN_FOREGROUND_COLOR;
	}
	return;
}

void free_console(t_console_desc *console_desc)
{
	SPINLOCK_UNLOCK(console_desc->spinlock);
	kfree(console_desc->out_buf);
}

char _read_char(t_console_desc *console_desc)
{
	char data;

	while (!(data=read_buf())) 
	{
		system.active_console_desc->sleeping_process=system.process_info.current_process->val;
		_sleep();
	}	
	return data;	
}

static int write_out_buf(t_console_desc *console_desc,char data)
{
	unsigned int video_index;
	unsigned int offset;
	unsigned int do_scroll=0;
	unsigned int i;
	unsigned int index_1;
	unsigned int index_2;

//	SAVE_IF_STATUS
//	CLI
	SPINLOCK_LOCK(console_desc->spinlock);	
	INC(console_desc->out_buf_index,console_desc->out_buf_len,1,0);
	console_desc->out_buf[console_desc->out_buf_index]=data;
	if (console_desc->out_buf_index==0 && console_desc->first_char) 
	{
		console_desc->out_window_start=0;
		console_desc->out_window_end=SCREEN_AREA-1;
		console_desc->first_char=0;
	}
	else if (console_desc->out_buf_index>console_desc->out_window_end || console_desc->out_buf_index==0)
	{
		INC(console_desc->out_window_start,console_desc->out_buf_len,SCREEN_WIDTH,0);
		INC(console_desc->out_window_end,console_desc->out_buf_len,SCREEN_WIDTH,SCREEN_WIDTH-1);
		do_scroll=1;
	}
	if (do_scroll) 
	{
		index_1=console_desc->out_window_start-1;
		index_2=-1;
		for (i=0;i<SCREEN_AREA-SCREEN_WIDTH;i++)
		{	
			INC(index_1,console_desc->out_buf_len,1,0);
			console_desc->video_buf[index_2++]=SCREEN_FOREGROUND_COLOR;
			console_desc->video_buf[index_2++]=console_desc->out_buf[index_1];
		}
		console_desc->video_buf[index_2++]=SCREEN_FOREGROUND_COLOR;
		console_desc->video_buf[index_2++]=console_desc->out_buf[index_1++];
		console_desc->video_buf_index=index_2-2;
		for (i=0;i<SCREEN_WIDTH-1;i++)
		{
			console_desc->video_buf[index_2++]=SCREEN_FOREGROUND_COLOR;//SCREEN_BACKGROUND_COLOR
			console_desc->video_buf[index_2++]=CHAR_NULL;	
		}
		do_scroll=0;
	}
	INC(console_desc->video_buf_index,(2*SCREEN_AREA),1,0);
	console_desc->video_buf[console_desc->video_buf_index]=console_desc->out_buf[console_desc->out_buf_index];
	console_desc->video_buf_index++;
	console_desc->video_buf[console_desc->video_buf_index]=SCREEN_FOREGROUND_COLOR;
	SPINLOCK_UNLOCK(console_desc->spinlock);
//	RESTORE_IF_STATUS
}

void _write_char(t_console_desc *console_desc,char data)
{
	unsigned int to_end_line;
	unsigned int i;

//	SAVE_IF_STATUS
//	CLI
	SPINLOCK_LOCK(console_desc->spinlock);
	if (data=='\n')
	{
		to_end_line=SCREEN_WIDTH -1 - (console_desc->out_buf_index %  SCREEN_WIDTH);
		for (i=0;i<to_end_line;i++) write_out_buf(console_desc,'\0');
	}
	else write_out_buf(console_desc,data);
	SPINLOCK_UNLOCK(console_desc->spinlock);
//	RESTORE_IF_STATUS
}

void _echo_char(t_console_desc *console_desc,char data)
{
//	SAVE_IF_STATUS
//	CLI
	_write_char(console_desc,data);	
	_update_cursor(console_desc);
//	RESTORE_IF_STATUS
}

void _delete_char(t_console_desc *console_desc)
{
//	SAVE_IF_STATUS
//	CLI
	SPINLOCK_LOCK(console_desc->spinlock);
	if (console_desc->out_buf_index/SCREEN_WIDTH==(console_desc->out_buf_index+1)/SCREEN_WIDTH)
	{
		console_desc->out_buf[--console_desc->out_buf_index]='\0';
		console_desc->video_buf[--console_desc->video_buf_index]=CHAR_NULL;
		console_desc->video_buf[--console_desc->video_buf_index]=SCREEN_FOREGROUND_COLOR;
	}
	SPINLOCK_UNLOCK(console_desc->spinlock);
//	RESTORE_IF_STATUS
}

void _enable_cursor(t_console_desc *console_desc)
{
	_update_cursor(console_desc);
}

void _disable_cursor(t_console_desc *console_desc)
{
	_update_cursor(console_desc);
}

void _update_cursor(t_console_desc *console_desc)
{
//	SAVE_IF_STATUS
//	CLI
	SPINLOCK_LOCK(console_desc->spinlock);
	unsigned int cursor_position=(console_desc->video_buf_index/2)+1;
	out(0x0F,0x3D4);
    	out((unsigned char)(cursor_position&0xFF),0x3D5);
   	out(0x0E,0x3D4);
    	out((unsigned char )((cursor_position>>8)&0xFF),0x3D5);
	SPINLOCK_UNLOCK(console_desc->spinlock);
//	RESTORE_IF_STATUS
}

