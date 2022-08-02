#ifndef FRAMEBUFFER_H                
#define FRAMEBUFFER_H

//PHYSICAL VIDEO MEMORY SIZE 2457600 BYTES
//PHYSICAL VIDEO MEMORY MAPPED AT 4G - 3M

#include "multiboot.h"
#include "system.h"

#define FB_VIRT_ADDR 0xFFFFFFFF - 0x300000
#define FONT font9x18
#define FONT_WIDTH 9
#define FONT_HEIGHT 18
#define CURSOR 95

void init_fb(multiboot_info_t* mbd);
void draw_char_fb(unsigned int index, u8 val);
void update_cursor_fb(unsigned int index);
void free_fb();

#endif

