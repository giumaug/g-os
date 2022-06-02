//PHYSICAL VIDEO MEMORY SIZE 2457600 BYTES
//PHYSICAL VIDEO MEMORY MAPPED AT 4G - 3M

#define FB_VIRT_ADDR 0xFFFFFFFF - 0x300000

void init_fb(multiboot_info_t* mbd);
void draw_char_fb(u8 val, unsigned int index);
void free_fb();

