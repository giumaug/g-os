.global out,in,adjust_kernel_stack,syscall

# void out(unsigend int value,unsigned int address)
out:
	push   %ebp
 	mov    %esp,%ebp
	push   %edx
	push   %eax
 	mov    0xc(%ebp),%edx  # address
 	mov    0x8(%ebp),%eax  # value
 	out    %al,%dx
	pop    %eax
	pop    %edx
 	pop    %ebp
 	ret

# unsigned int in(unsigned int address)
in:
	push   %ebp
 	mov    %esp,%ebp
	push   %edx
	#push   %eax
	mov    $0x0,%eax
 	mov    0x8(%ebp),%edx  # address
 	in     %dx,%al
	#pop    %eax
	pop    %edx
 	pop    %ebp
 	ret  

switch_to_user_mode_old:
	push   %ebp
 	mov    %esp,%ebp
 	mov    $0x20,%ax 	
	mov    %ax,%ds
	mov    %ax,%ss
	mov    %ax,%es
	jmp	$0x10,$next
next: 	pop    %ebp
 	ret

#adjust_kernel_stack:
#	push   %ebp
# 	mov    %esp,%ebp
#	push   %eax
#	push   %ebx
#	push   %ecx
#	mov    0x8(%ebp),%eax	 # stack address
#       mov    0xc(%ebp),%ecx	 # entry point offset
#       movl   $0x23,-0x4(%eax)	 # ss
#	mov    %eax,%ebx
#	sub    $0x10000,%ebx
#	movl   %ebx,-0x8(%eax)   # esp
#	movl   $0x206,-0xc(%eax) # elfags put eflags value at start of process_0
#	movl   $0x13,-0x10(%eax) # cs
##	add    $process_0,%ecx  
#	mov    %ecx,-0x14(%eax)  # eip
#       movl   $0x0,-0x18(%eax)	 # fake ebp
#	pop    %ecx
#       pop    %ebx
#	pop    %eax
#	pop    %ebp
#	ret

# void syscall(unsigend int syscall_num,int* params)
#syscall:
#	push   %ebp
# 	mov    %esp,%ebp
#	push   %eax
#	push   %ebx
# 	mov    0xc(%ebp),%ebx  # params
# 	mov    0x8(%ebp),%eax  # syscall_num
# 	int    $0x80
#	pop    %ebx
#	pop    %eax
# 	pop    %ebp
# 	ret

                     
	
	

