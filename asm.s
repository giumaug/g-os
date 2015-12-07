.global out,in,outw,inw,indw,outdw,adjust_kernel_stack,syscall

# void out(unsigend char value,unsigned int address)
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

# unsigned char in(unsigned int address)
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

# void out(unsigend short value,unsigned int address)
outw:
	push   %ebp
 	mov    %esp,%ebp
	push   %edx
	push   %eax
 	mov    0xc(%ebp),%edx  # address
 	mov    0x8(%ebp),%eax  # value
 	out    %ax,%dx
	pop    %eax
	pop    %edx
 	pop    %ebp
 	ret

# unsigned short in(unsigned int address)
inw:
	push   %ebp
 	mov    %esp,%ebp
	push   %edx
	#push   %eax
	mov    $0x0,%eax
 	mov    0x8(%ebp),%edx  # address
 	in     %dx,%ax
	#pop    %eax
	pop    %edx
 	pop    %ebp
 	ret

# void outdw(unsigend int value,unsigned int address)
outdw:
	push   %ebp
 	mov    %esp,%ebp
	push   %edx
	push   %eax
 	mov    0x0a(%ebp),%edx  # address
 	mov    0x6(%ebp),%eax  # value
 	outl    %eax,%dx
	pop    %eax
	pop    %edx
 	pop    %ebp
 	ret

# unsigned int indw(unsigned int address)
indw:
	push   %ebp
 	mov    %esp,%ebp
	push   %edx
	mov    $0x0,%eax
 	mov    0x8(%ebp),%dx  # address
 	inl    %dx,%eax
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
