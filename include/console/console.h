#ifndef CONSOLE_H                
#define CONSOLE_H

#include "system.h"
#include "scheduler/process.h"

#define VIDEO_MEM_ADD 0xb8000;
#define SCREEN_FOREGROUND_COLOR 0x07;
#define	SCREEN_BACKGROUND_COLOR 0x0;
#define CHAR_NULL 0x0
#define SCREEN_WIDTH 80
#define SCREEN_LENGTH 25
#define SCREEN_AREA SCREEN_WIDTH*SCREEN_LENGTH
#define INC(x,y,z,w) if ((x+=z)>=y) x=w;

typedef struct s_console_desc 
{
	char *out_buf;
	t_llist* wait_queue; 
	unsigned int out_buf_index;
	unsigned int out_window_start;
	unsigned int out_window_end;
	unsigned int out_buf_len;
	char *video_buf;
	unsigned int video_buf_index;
	unsigned int first_char;
	struct t_process_context* sleeping_process;
	unsigned int is_empty;
	t_spinlock_desc spinlock;
	u32 fg_pgid;
}
t_console_desc;

void init_console(t_console_desc *console_desc,int out_buf_len,int in_buf_len);
void free_console(t_console_desc *console_desc);
char _read_char(t_console_desc *console_desc);
void _write_char(t_console_desc *console_desc,char data);
void _delete_char(t_console_desc *console_desc);
void _echo_char(t_console_desc *console_desc,char data);
void _enable_cursor(t_console_desc *console_desc);
void _disable_cursor(t_console_desc *console_desc);
void _update_cursor(t_console_desc *console_desc);
void _write_char_no_irq(t_console_desc *console_desc,char data);

#endif
