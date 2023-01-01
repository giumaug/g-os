#include "asm.h"
#include "scheduler/process.h"
#include "memory_manager/kmalloc.h"
#include "drivers/kbc/8042.h"
#include "console/console.h"

static void update_cursor(unsigned int curs_pos);
static void scroll(t_console_desc *console_desc);

void init_console(t_console_desc* console_desc,
				  void (*f)(u32, char),
				  void (*g)(),
				  int out_buf_len,
				  int in_buf_len)
{
	int i, j = 0;
	SPINLOCK_INIT(console_desc->spinlock);
	console_desc->out_buf = kmalloc(out_buf_len);
	console_desc->out_buf_len = out_buf_len;
	console_desc->out_buf_index = -1;
	console_desc->video_buf_index = -1;
	console_desc->write_char = f;
	console_desc->update_cursor = g;
	console_desc->first_char = 1;
	console_desc->sleeping_process = NULL;
	console_desc->is_empty = 1;
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

	while (!(data=read_buf()));
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
		scroll(console_desc);
	}
	INC(console_desc->video_buf_index,(SCREEN_AREA),1,0);
	console_desc->write_char(console_desc->video_buf_index, console_desc->out_buf[console_desc->out_buf_index]);
}

void _write_char(t_console_desc *console_desc,char data)
{
	unsigned int to_end_line;
	unsigned int i;

	SPINLOCK_LOCK(console_desc->spinlock);
	if (data=='\n')
	{
		to_end_line=SCREEN_WIDTH -1 - (console_desc->out_buf_index %  SCREEN_WIDTH);
		for (i=0;i<to_end_line;i++) write_out_buf(console_desc, CHAR_NULL);
	}
	else write_out_buf(console_desc,data);
	SPINLOCK_UNLOCK(console_desc->spinlock);
}

void _write_char_no_irq(t_console_desc *console_desc, char data)
{
	unsigned int to_end_line;
	unsigned int i;

	SAVE_IF_STATUS
	CLI
	if (data=='\n')
	{
		to_end_line = SCREEN_WIDTH -1 - (console_desc->out_buf_index %  SCREEN_WIDTH);
		for (i = 0; i < to_end_line; i++) write_out_buf(console_desc, CHAR_NULL);
	}
	else write_out_buf(console_desc, data);
	RESTORE_IF_STATUS
}

void _echo_char(t_console_desc *console_desc,char data)
{
	_write_char(console_desc,data);	
	_update_cursor(console_desc);
}

void _delete_char(t_console_desc *console_desc)
{
	SPINLOCK_LOCK(console_desc->spinlock);
	//if (console_desc->out_buf_index/SCREEN_WIDTH==(console_desc->out_buf_index+1)/SCREEN_WIDTH)
	{
		console_desc->write_char(console_desc->video_buf_index + 1 , CHAR_NULL);
		console_desc->out_buf[console_desc->out_buf_index--]= CHAR_NULL;
		console_desc->write_char(console_desc->video_buf_index--, CHAR_NULL);
	}
	SPINLOCK_UNLOCK(console_desc->spinlock);
}

void _enable_cursor(t_console_desc *console_desc)
{
	_update_cursor(console_desc);
}

void _disable_cursor(t_console_desc *console_desc)
{
	_update_cursor(console_desc);
}

void _update_cursor(t_console_desc* console_desc)
{	
	SPINLOCK_LOCK(console_desc->spinlock);
	if (console_desc->out_buf_index == console_desc->out_window_end)
	{
		scroll(console_desc);
	}
	
	unsigned int cursor_position = console_desc->video_buf_index;
	INC(cursor_position, console_desc->out_buf_len, 1, 0);
    console_desc->update_cursor(cursor_position);
	SPINLOCK_UNLOCK(console_desc->spinlock);
}

static void scroll(t_console_desc *console_desc)
{
	unsigned int i;
	unsigned int index_1;
	unsigned int index_2;
	
	INC(console_desc->out_window_start,console_desc->out_buf_len,SCREEN_WIDTH,0);
	INC(console_desc->out_window_end,console_desc->out_buf_len,SCREEN_WIDTH,SCREEN_WIDTH-1);
	
	index_1 = console_desc->out_window_start-1;
	index_2 = 0;
	for (i = 0; i < SCREEN_AREA - SCREEN_WIDTH; i++)
	{	
		INC(index_1,console_desc->out_buf_len,1,0);
		console_desc->write_char(index_2++, console_desc->out_buf[index_1]);
	}
	console_desc->video_buf_index = index_2 - 1;
	for (i = 0;i < SCREEN_WIDTH; i++)
	{
		console_desc->write_char(index_2++, CHAR_NULL);
	}
}

