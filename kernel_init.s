.global init

# setting up the Multiboot header - see GRUB docs for details

.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum required

.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.set INIT_STACK_SIZE,0x1000
.set TSS_SIZE, 0x68 
 
.comm INIT_STACK,INIT_STACK_SIZE,32   
.comm TSS,TSS_SIZE, 32

#t_init_data
.comm TSS_ADD,8

# Global Descriptor Table (GDT)
 
gdt_data: 
	.long 0x0 			# null descriptor
	.long 0x0 
 
# gdt kernel code segment:		# code descriptor
	.word 0xFFFF 			# limit low
	.word 0x0 			# base low
	.byte 0x0 			# base middle
	.byte 0B10011010 		# access
	.byte 0B11001111 		# granularity
	.byte 0x0 			# base high

# gdt user  code segment:		# code descriptor
	.word 0xFFFF 			# limit low
	.word 0x0 			# base low
	.byte 0x0 			# base middle
	.byte 0B11111010 		# access
	.byte 0B11001111 		# granularity
	.byte 0x0 			# base high

 
# gdt kernel data segment:		# data descriptor
	.word 0x0FFFF 			# limit low (Same as code)
	.word 0x0 			# base low
	.byte 0x0 			# base middle
	.byte 0B10010010 		# access
	.byte 0B11001111 		# granularity
	.byte 0x0			# base high

# gdt user data segment:		# data descriptor
	.word 0x0FFFF 			# limit low (Same as code)
	.word 0x0 			# base low
	.byte 0x0 			# base middle
	.byte 0B11110010 		# access
	.byte 0B11001111 		# granularity
	.byte 0x0			# base high

# gdt tss segment:
tss_desc:
	.word 0x68 			# limit low (Same as code)
	.word 0x0  		        # base low
	.byte 0x0               	# base middle
	.byte 0B10001001 		# access
	.byte 0B00000000 		# granularity
	.byte 0x0	                # base high

#da aggingere
 
end_of_gdt:
toc: 
	.word end_of_gdt - gdt_data - 1 
	.long gdt_data 			
init: 
	mov 	$TSS,%eax;
	mov 	$0xFFFF,%ebx;
	and 	%ebx,%eax;
	mov 	%ax,tss_desc+2;

	mov 	$TSS,%eax;
	mov 	$0xFF0000,%ebx;
	and 	%ebx,%eax;
	shr	$0x10,%eax;
	mov 	%al,tss_desc+4;

	mov 	$TSS,%eax;
	mov 	$0xFF000000,%ebx;
	and 	%ebx,%eax;
	shr	$0x18,%eax;
	mov 	%al,tss_desc+7;

# Inizialize GDT

	lgdt 	toc			

# Go into pmode		
					
	mov	%cr0,%eax		
	or	$0x10001,%eax
	mov	%eax,%cr0
	jmp	$0x08,$pmode		
 
# ENTRY POINT FOR PMODE
 
pmode:
	mov	$0x18,%cx	    	
	mov	%cx,%ds
	mov	%cx,%ss
	mov	%cx,%es
	mov 	$(TSS+0x08),%ecx;# 50 SS
	mov	%ecx,TSS_ADD;
	mov 	$(TSS+0x04),%ecx;# 38 ESP
	mov	%ecx,(TSS_ADD+4);
	mov     $TSS_ADD,%ecx;

# Stack grows from botton to up (push decrement address stack,pop increment)
	mov   	$(INIT_STACK+INIT_STACK_SIZE),%esp;
#load tss       
	mov 	$0x28,%ax;
        ltr 	%ax;	
	push 	%ecx;   	
	push  	%ebx                       	
   	push  	%eax               	
	call  	kmain            		
	cli

# Halt machine should kernel return

hang:
   	hlt                   
   	jmp   	hang










