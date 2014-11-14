typedef struct s_mem_regs 
{ 
	u32 start_addr;
	u32 end_addr;
	
} 
t_mem_reg;

void init_mem_regs()
{
	system.mem_regs=new_dllist();
}

void free_mem_regs()
{
	free_llist(system.mem_regs);
}

void create_mem_reg(u32 start_addr,u32 end_addr)
{
	t_mem_reg* mem_reg=null;

	mem_reg=kmalloc(sizeof(t_mem_reg));
	mem_reg->start_addr=start_addr;
	mem_reg->end_addr=end_addr;
	ll_append(system.mem_regs,mem_reg);
}

void delete_mem_reg(t_mem_reg* mem_reg)
{
	ll_delete_node(mem_reg);
	kfree(mem_reg);
}

void enlarge_mem_reg()
{

}

void shrink_mem_reg()
{

}
