#include "framebuffer/charset.h"
#include "framebuffer/framebuffer.h"

static char* fb = NULL;
static u32 fb_width;
static u32 fb_height;
static u32 fb_bpp;
static u32 fb_bytes;
static u32 fb_size;
static u32 fb_text_width;
static u32 fb_text_height;
static u32 fb_text_area;

void init_fb(multiboot_info_t* mbd)
{
	int i;
	long fb_virt_addr = FB_VIRT_ADDR;
	long fb_phy_addr = mbd->framebuffer_addr;
	fb = FB_VIRT_ADDR;
	
	fb_width = mbd->framebuffer_width;
	fb_height = mbd->framebuffer_height;
	fb_bpp = mbd->framebuffer_bpp;
	fb_bytes = fb_bpp / 8;
	
	fb_text_width = fb_width / FONT_WIDTH;
	fb_text_height = fb_height / FONT_HEIGHT;
	fb_text_area = fb_text_width * fb_text_height;

	fb_size = fb_width * fb_height * fb_bytes;
	map_vm_mem(system.master_page_dir, fb_virt_addr, fb_phy_addr, (fb_size + PAGE_SIZE), 7);
	//SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) system.master_page_dir)))
	
	for (i = 0; i < fb_size; i++)
	{
		fb[i] = 0;
	}
}

static void put_pixel(int x, int y, u8 c)
{
    unsigned int pix_offset = (4 * x) + (4 * y) * fb_width;
    
    *((char*)(fb + pix_offset)) = (0xff * c);
    *((char*)(fb + pix_offset + 1)) =  (0xff * c);
    *((char*)(fb + pix_offset + 2)) = (0xff * c);
    *((char*)(fb + pix_offset + 3)) = 0;
}

void free_fb()
{
	umap_vm_mem(system.master_page_dir, FB_VIRT_ADDR, (fb_size + PAGE_SIZE), 1);
}

void draw_char_fb(unsigned int index, u8 val) 
{
	unsigned short cx, cy;
	unsigned short x, y;
	short glyph;
	short low_glyph;
	short hi_glyph;
	int offset;
	int set;
	
	index = index % fb_text_area;
	y = (index / fb_text_width) * FONT_HEIGHT;
	x = (index % fb_text_width) * FONT_WIDTH;
	
    for (cy = 0; cy < FONT_HEIGHT; cy++) 
    {
		low_glyph = FONT[val - 32][cy * 2];
		hi_glyph = FONT[val - 32][cy + 1];
		glyph = (hi_glyph << 8) + low_glyph;
        for (cx = 0; cx < FONT_WIDTH; cx++) 
        {	
			set = glyph & 1 << cx;
			if (set  > 0)
			{ 
				put_pixel(x + cx, y + cy, 1);
			} 
			else  
			{
				put_pixel(x + cx, y + cy, 0);
			}
        }
    }
}

void update_cursor_fb(unsigned int index)
{
	draw_char_fb(index, CURSOR);
}
