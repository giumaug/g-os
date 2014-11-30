#ifndef VM_H                
#define VM_H

#define VIRT_MEM_START_ADDR 0xC0000000
#define VIRT_MEM_END_ADDR 0xC0000000+PHY_MEM_SIZE
#define PHY_MEM_START_ADDR  0x100000
#define RAM_DISK_START 0x300000
#define RAM_DISK_END   0xCFFFFF
#define PAGE_TABLE_ENTRY HIGH_MEM_LIMIT/0x1000
#define PAGE_DIR_ENTRY PAGE_TABLE_ENTRY/0x400
#define PROC_VIRT_MEM_START_ADDR 0x100000    //1M - 1G
#define HEAP_VIRT_MEM_START_ADDR 0x40000000  //1G - 2G
#define HEAP_INIT_SIZE 0x100000
#define KERNEL_STACK 0xBFFFF000              //3G-4K hole
#define KERNEL_STACK_SIZE 0x2000          
#define USER_STACK 0xBFFFC000                //3G-4Khole -8K -4Khole    
#define USER_STACK_INIT_SIZE 0x100000

#define FROM_VIRT_TO_PHY(addr) addr-VIRT_MEM_START_ADDR+PHY_MEM_START_ADDR
#define FROM_PHY_TO_VIRT(addr) addr+VIRT_MEM_START_ADDR-PHY_MEM_START_ADDR
//#define KERNEL_STACK 0x1FC000
//#define USER_STACK 0xBFFFF000 //3g-4k security hole 

#define CHECK_MEM_REG(fault_addr,mem_reg) fault_addr>=mem_reg->start_addr && fault_addr<=mem_reg->end_addr 

//void* init_vm_process(void* master_page_dir,unsigned int proc_phy_addr,struct t_process_context* process_context,unsigned int flags);
//void free_vm_process(void* page_dir,unsigned int flags);
void* init_vm_process(struct t_process_context* process_context);
void free_vm_process(struct t_process_context* process_context);
void map_vm_mem(void* page_dir,unsigned int vir_mem_addr,unsigned int phy_mem_addr,int mem_size);
void umap_vm_mem(void* page_dir,unsigned int virt_mem_addr,unsigned int mem_size,unsigned int flush);
void clone_vm_process(void* parent_page_dir);

#endif



