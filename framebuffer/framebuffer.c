#include "framebuffer/charset.h"
#include "framebuffer/framebuffer.h"

static char* fb = NULL;
static u32 fb_width;
static u32 fb_height;
static u32 fb_bpp;
static u32 fb_bytes;
static u32 fb_size;
static u8 fb_text_width;
static u8 fb_text_height;

void init_fb(multiboot_info_t* mbd)
{
/*
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
	ascii_characters[95] = UNDERSCORE__10x14;

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
*/

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

	fb_size = fb_width * fb_height * fb_bytes;
	map_vm_mem(system.master_page_dir, fb_virt_addr, fb_phy_addr, (fb_size + PAGE_SIZE), 7);
	//SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) system.master_page_dir)))
	
	for (i = 0; i < fb_size; i++)
	{
		//fb[i] = 0;
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

//void render(char *bitmap) {
//    int x,y;
//    int set;
//    int mask;
//    for (x=0; x < 8; x++) {
//        for (y=0; y < 8; y++) {
//            set = bitmap[x] & 1 << y;
//            printf("%c", set ? 'X' : ' ');
//        }
//        printf("\n");
//    }
//}


void draw_char_fb(unsigned int index, u8 val) 
{
	unsigned short cx, cy;
	unsigned short x, y;
	short glyph;
	short low_glyph;
	short hi_glyph;
	int offset;
	
	int set;
	
	int pippo_1 = FONT_HEIGHT;
	int pippo_2 = FONT_WIDTH;
	if (pippo_1 == pippo_2) set = 0;
	
	y = (index / fb_text_width) * FONT_HEIGHT;
	x = (index % fb_text_width) * FONT_WIDTH;
	
	//102 x 42
	//index = 12
	//cahr 10 x 14
	
	//1024x600 = 128 x 75 (with 8x8 font)
	
	
	
	
    for (cy = 0; cy < FONT_HEIGHT; cy++) 
    {
		low_glyph = font8x16_basic[0][cy * 2];
		hi_glyph = font8x16_basic[0][cy + 1];
		glyph = (hi_glyph << 8) + low_glyph;
		//glyph = FONT[0][cx];
        for (cx = 0; cx < FONT_WIDTH; cx++) 
        {	
			//set = (font8x16_basic[0][cy]) & 1 << cx;
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
