#include "framebuffer.h"
#include "charset.h"

static u8 CHAR_WIDTH = 10;
static u8 CHAR_HEIGHT = 14; 
static u8* ascii_characters_BIG[128];
static u8* ascii_characters_SMALL[128];
static u8* numbers_BIG[10];
static u8* numbers_small[10];

static char* fb = NULL;
static u32 fb_width;
static u32 fb_height;
static u32 fb_bpp;
static u32 fb_bytes;
static u32 fb_size;

void init_fb(multiboot_info_t* mbd)
{
	ascii_characters[32] = SPACE__10x14;
	ascii_characters[48] = AR0__10x14;
	ascii_characters[49] = AR1__10x14;
	ascii_characters[50] = AR2__10x14;
	ascii_characters[51] = AR3__10x14;
	ascii_characters[52] = AR4__10x14;
	ascii_characters[53] = AR5__10x14;
	ascii_characters[54] = AR6__10x14;
	ascii_characters[55] = AR7__10x14;
	ascii_characters[56] = AR8__10x14;
	ascii_characters[57] = AR9__10x14;
	ascii_characters[58] = COLON__10x14;
	ascii_characters[65] = A__10x14;
	ascii_characters[66] = B__10x14;
	ascii_characters[67] = C__10x14;
	ascii_characters[68] = D__10x14;
	ascii_characters[69] = E__10x14;
	ascii_characters[70] = F__10x14;
	ascii_characters[71] = G__10x14;
	ascii_characters[72] = H__10x14;
	ascii_characters[73] = I__10x14;
	ascii_characters[74] = J__10x14;
	ascii_characters[75] = K__10x14;
	ascii_characters[76] = L__10x14;
	ascii_characters[77] = M__10x14;
	ascii_characters[78] = N__10x14;
	ascii_characters[79] = O__10x14;
	ascii_characters[80] = P__10x14;
	ascii_characters[81] = Q__10x14;
	ascii_characters[82] = R__10x14;
	ascii_characters[83] = S__10x14;
	ascii_characters[84] = T__10x14;
	ascii_characters[85] = U__10x14;
	ascii_characters[86] = V__10x14;
	ascii_characters[87] = W__10x14;
	ascii_characters[88] = X__10x14;
	ascii_characters[89] = Y__10x14;
	ascii_characters[90] = Z__10x14;

	numbers[0] = AR0__10x14;
	numbers[1] = AR1__10x14;
	numbers[2] = AR2__10x14;
	numbers[3] = AR3__10x14;
	numbers[4] = AR4__10x14;
	numbers[5] = AR5__10x14;
	numbers[6] = AR6__10x14;
	numbers[7] = AR7__10x14;
	numbers[8] = AR8__10x14;
	numbers[9] = AR9__10x14;

	int i;
	long fb_virt_addr = FB_VIRT_ADDR;
	long fb_phy_addr = mbd->framebuffer_addr;
	char* fb = FB_VIRT_ADDR;
	
	fb_width = mbd->framebuffer_width;
	fb_height = mbd->framebuffer_height;
	fb_bpp = mbd->framebuffer_bpp;
	fb_bytes = fb_bpp / 8; 

	fb_size = fb_width * fb_height * fb_bytes;
	map_vm_mem(system.master_page_dir, fb_virt_addr, fb_phy_addr, (fb_size + PAGE_SIZE), 7);
	//SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int)page_dir)))
	
	fbdata = fb_virt_addr;
	for (i = 0; i < fb_data_size; i++)
	{
		fbdata[i] = 0;
	}
}

static void put_pixel(int x, int y, u8 c)
{
    unsigned int pix_offset = (4 * x) + (4 * y) * vinfo.xres;
    
    *((char*)(fbp + pix_offset)) = (0xff * c);
    *((char*)(fbp + pix_offset + 1)) =  (0xff * c);
    *((char*)(fbp + pix_offset + 2)) = (0xff + c);
    *((char*)(fbp + pix_offset + 3)) = 0;
}

void free_fb()
{
	umap_vm_mem(system.master_page_dir, FB_VIRT_ADDR, KERNEL_STACK_SIZE, (fb_size + PAGE_SIZE), 1);	
}

void draw_char_fb(u8 val, unsigned int index) 
{
	unsigned short cx, cy;
	unsigned short x, y;
	char* glyph;
	
	y = index / fb_width;
	x = index % fb_width;
	glyph = ascii_characters_BIG[val];
	
    for (cy = 0; cy < CHAR_HEIGHT; cy++) 
    {
        for (cx = 0; cx < CHAR_WIDTH; cx++) 
        {
			if (glyph[cy * CHAR_WIDTH + cx] > 0)
			{ 
				put_pixel(x+cx, y+cy, 1);
			} 
			else  
			{
				put_pixel(x+cx, y+cy, 0);
			}
        }
    }
}
