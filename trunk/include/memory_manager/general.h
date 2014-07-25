#ifndef MEMORY_GENERAL_H                
#define MEMORY_GENERAL_H

#define PAGE_SIZE 0x1000
#define PHY_MEM_SIZE 0xC800000 //0x3200000
//Consider system with 200M.Buddy takes 185M starting from 15M 
//0xD00000
#define MEM_START_ADDR 0x1700000
#define MEM_END_ADDR PHY_MEM_SIZE
#define ALLOCATED_MEM (MEM_END_ADDR - MEM_START_ADDR)
#define POOL_START_ADDR MEM_START_ADDR
#define POOL_END_ADDR (POOL_START_ADDR + ((ALLOCATED_MEM/10)-1))
#define POOL_NUM 16 
#define MEM_TO_POOL ((POOL_END_ADDR - POOL_START_ADDR)/POOL_NUM)
//#define BUDDY_START_ADDR (MEM_START_ADDR+(ALLOCATED_MEM/10))
#define _BUDDY_START_ADDR (MEM_START_ADDR+(ALLOCATED_MEM/10))
#define BUDDY_START_ADDR ((( _BUDDY_START_ADDR % 0x1000) !=0) ? ((( _BUDDY_START_ADDR+0x1000) - (( _BUDDY_START_ADDR+0x1000) % 0x1000))) : ( _BUDDY_START_ADDR % 0x1000))
#define BUDDY_END_ADDR MEM_END_ADDR
#define BUDDY_MEM_SIZE (BUDDY_END_ADDR - BUDDY_START_ADDR)

#endif
