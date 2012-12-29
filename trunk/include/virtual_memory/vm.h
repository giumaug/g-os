#define VIRT_MEM_START_ADDR 0xC0000000
#define VIRT_MEM_END_ADDR 0xC0000000+PHY_MEM_SIZE
#define PHY_MEM_START_ADDR  0x100000
#define RAM_DISK_START 0x300000
#define RAM_DISK_END   0xCFFFFF
#define PAGE_TABLE_ENTRY HIGH_MEM_LIMIT/0x1000
#define PAGE_DIR_ENTRY PAGE_TABLE_ENTRY/0x400
#define PROC_VIRT_MEM_START_ADDR 0x100000
#define FROM_VIRT_TO_PHY(addr) addr-VIRT_MEM_START_ADDR+PHY_MEM_START_ADDR
#define FROM_PHY_TO_VIRT(addr) addr+VIRT_MEM_START_ADDR-PHY_MEM_START_ADDR

void* init_vm_process(void* master_page_dir,unsigned int proc_phy_addr,struct t_process_context* process_context);
void free_vm_process(void* page_dir,int pad);
void map_vm_mem(void* page_dir,unsigned int vir_mem_addr,unsigned int phy_mem_addr,int mem_size);
void umap_vm_mem(void* page_dir,unsigned int virt_mem_addr,unsigned int mem_size);



