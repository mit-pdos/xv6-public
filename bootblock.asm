
bootblock.o:     file format elf32-i386


Disassembly of section .text:

00007c00 <start>:
# with %cs=0 %ip=7c00.

.code16                       # Assemble for 16-bit mode
.globl start
start:
  cli                         # BIOS enabled interrupts; disable
    7c00:	fa                   	cli    

  # Zero data segment registers DS, ES, and SS.
  xorw    %ax,%ax             # Set %ax to zero
    7c01:	31 c0                	xor    %eax,%eax
  movw    %ax,%ds             # -> Data Segment
    7c03:	8e d8                	mov    %eax,%ds
  movw    %ax,%es             # -> Extra Segment
    7c05:	8e c0                	mov    %eax,%es
  movw    %ax,%ss             # -> Stack Segment
    7c07:	8e d0                	mov    %eax,%ss

00007c09 <seta20.1>:

  # Physical address line A20 is tied to zero so that the first PCs 
  # with 2 MB would run software that assumed 1 MB.  Undo that.
seta20.1:
  inb     $0x64,%al               # Wait for not busy
    7c09:	e4 64                	in     $0x64,%al
  testb   $0x2,%al
    7c0b:	a8 02                	test   $0x2,%al
  jnz     seta20.1
    7c0d:	75 fa                	jne    7c09 <seta20.1>

  movb    $0xd1,%al               # 0xd1 -> port 0x64
    7c0f:	b0 d1                	mov    $0xd1,%al
  outb    %al,$0x64
    7c11:	e6 64                	out    %al,$0x64

00007c13 <seta20.2>:

seta20.2:
  inb     $0x64,%al               # Wait for not busy
    7c13:	e4 64                	in     $0x64,%al
  testb   $0x2,%al
    7c15:	a8 02                	test   $0x2,%al
  jnz     seta20.2
    7c17:	75 fa                	jne    7c13 <seta20.2>

  movb    $0xdf,%al               # 0xdf -> port 0x60
    7c19:	b0 df                	mov    $0xdf,%al
  outb    %al,$0x60
    7c1b:	e6 60                	out    %al,$0x60

  # Switch from real to protected mode.  Use a bootstrap GDT that makes
  # virtual addresses map directly to physical addresses so that the
  # effective memory map doesn't change during the transition.
  lgdt    gdtdesc
    7c1d:	0f 01 16             	lgdtl  (%esi)
    7c20:	78 7c                	js     7c9e <readsect+0xe>
  movl    %cr0, %eax
    7c22:	0f 20 c0             	mov    %cr0,%eax
  orl     $CR0_PE, %eax
    7c25:	66 83 c8 01          	or     $0x1,%ax
  movl    %eax, %cr0
    7c29:	0f 22 c0             	mov    %eax,%cr0

//PAGEBREAK!
  # Complete the transition to 32-bit protected mode by using a long jmp
  # to reload %cs and %eip.  The segment descriptors are set up with no
  # translation, so that the mapping is still the identity mapping.
  ljmp    $(SEG_KCODE<<3), $start32
    7c2c:	ea                   	.byte 0xea
    7c2d:	31 7c 08 00          	xor    %edi,0x0(%eax,%ecx,1)

00007c31 <start32>:

.code32  # Tell assembler to generate 32-bit code now.
start32:
  # Set up the protected-mode data segment registers
  movw    $(SEG_KDATA<<3), %ax    # Our data segment selector
    7c31:	66 b8 10 00          	mov    $0x10,%ax
  movw    %ax, %ds                # -> DS: Data Segment
    7c35:	8e d8                	mov    %eax,%ds
  movw    %ax, %es                # -> ES: Extra Segment
    7c37:	8e c0                	mov    %eax,%es
  movw    %ax, %ss                # -> SS: Stack Segment
    7c39:	8e d0                	mov    %eax,%ss
  movw    $0, %ax                 # Zero segments not ready for use
    7c3b:	66 b8 00 00          	mov    $0x0,%ax
  movw    %ax, %fs                # -> FS
    7c3f:	8e e0                	mov    %eax,%fs
  movw    %ax, %gs                # -> GS
    7c41:	8e e8                	mov    %eax,%gs

  # Set up the stack pointer and call into C.
  movl    $start, %esp
    7c43:	bc 00 7c 00 00       	mov    $0x7c00,%esp
  call    bootmain
    7c48:	e8 e2 00 00 00       	call   7d2f <bootmain>

  # If bootmain returns (it shouldn't), trigger a Bochs
  # breakpoint if running under Bochs, then loop.
  movw    $0x8a00, %ax            # 0x8a00 -> port 0x8a00
    7c4d:	66 b8 00 8a          	mov    $0x8a00,%ax
  movw    %ax, %dx
    7c51:	66 89 c2             	mov    %ax,%dx
  outw    %ax, %dx
    7c54:	66 ef                	out    %ax,(%dx)
  movw    $0x8ae0, %ax            # 0x8ae0 -> port 0x8a00
    7c56:	66 b8 e0 8a          	mov    $0x8ae0,%ax
  outw    %ax, %dx
    7c5a:	66 ef                	out    %ax,(%dx)

00007c5c <spin>:
spin:
  jmp     spin
    7c5c:	eb fe                	jmp    7c5c <spin>
    7c5e:	66 90                	xchg   %ax,%ax

00007c60 <gdt>:
	...
    7c68:	ff                   	(bad)  
    7c69:	ff 00                	incl   (%eax)
    7c6b:	00 00                	add    %al,(%eax)
    7c6d:	9a cf 00 ff ff 00 00 	lcall  $0x0,$0xffff00cf
    7c74:	00                   	.byte 0x0
    7c75:	92                   	xchg   %eax,%edx
    7c76:	cf                   	iret   
	...

00007c78 <gdtdesc>:
    7c78:	17                   	pop    %ss
    7c79:	00 60 7c             	add    %ah,0x7c(%eax)
	...

00007c7e <waitdisk>:
  entry();
}

void
waitdisk(void)
{
    7c7e:	55                   	push   %ebp
    7c7f:	89 e5                	mov    %esp,%ebp
static inline uchar
inb(ushort port)
{
  uchar data;

  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
    7c81:	ba f7 01 00 00       	mov    $0x1f7,%edx
    7c86:	ec                   	in     (%dx),%al
  // Wait for disk ready.
  while((inb(0x1F7) & 0xC0) != 0x40)
    7c87:	83 e0 c0             	and    $0xffffffc0,%eax
    7c8a:	3c 40                	cmp    $0x40,%al
    7c8c:	75 f8                	jne    7c86 <waitdisk+0x8>
    ;
}
    7c8e:	5d                   	pop    %ebp
    7c8f:	c3                   	ret    

00007c90 <readsect>:

// Read a single sector at offset into dst.
void
readsect(void *dst, uint offset)
{
    7c90:	55                   	push   %ebp
    7c91:	89 e5                	mov    %esp,%ebp
    7c93:	57                   	push   %edi
    7c94:	53                   	push   %ebx
    7c95:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  // Issue command.
  waitdisk();
    7c98:	e8 e1 ff ff ff       	call   7c7e <waitdisk>
}

static inline void
outb(ushort port, uchar data)
{
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
    7c9d:	ba f2 01 00 00       	mov    $0x1f2,%edx
    7ca2:	b8 01 00 00 00       	mov    $0x1,%eax
    7ca7:	ee                   	out    %al,(%dx)
    7ca8:	b2 f3                	mov    $0xf3,%dl
    7caa:	89 d8                	mov    %ebx,%eax
    7cac:	ee                   	out    %al,(%dx)
    7cad:	0f b6 c7             	movzbl %bh,%eax
    7cb0:	b2 f4                	mov    $0xf4,%dl
    7cb2:	ee                   	out    %al,(%dx)
  outb(0x1F2, 1);   // count = 1
  outb(0x1F3, offset);
  outb(0x1F4, offset >> 8);
  outb(0x1F5, offset >> 16);
    7cb3:	89 d8                	mov    %ebx,%eax
    7cb5:	c1 e8 10             	shr    $0x10,%eax
    7cb8:	b2 f5                	mov    $0xf5,%dl
    7cba:	ee                   	out    %al,(%dx)
  outb(0x1F6, (offset >> 24) | 0xE0);
    7cbb:	c1 eb 18             	shr    $0x18,%ebx
    7cbe:	89 d8                	mov    %ebx,%eax
    7cc0:	83 c8 e0             	or     $0xffffffe0,%eax
    7cc3:	b2 f6                	mov    $0xf6,%dl
    7cc5:	ee                   	out    %al,(%dx)
    7cc6:	b2 f7                	mov    $0xf7,%dl
    7cc8:	b8 20 00 00 00       	mov    $0x20,%eax
    7ccd:	ee                   	out    %al,(%dx)
  outb(0x1F7, 0x20);  // cmd 0x20 - read sectors

  // Read data.
  waitdisk();
    7cce:	e8 ab ff ff ff       	call   7c7e <waitdisk>
  asm volatile("cld; rep insl" :
    7cd3:	8b 7d 08             	mov    0x8(%ebp),%edi
    7cd6:	b9 80 00 00 00       	mov    $0x80,%ecx
    7cdb:	ba f0 01 00 00       	mov    $0x1f0,%edx
    7ce0:	fc                   	cld    
    7ce1:	f3 6d                	rep insl (%dx),%es:(%edi)
  insl(0x1F0, dst, SECTSIZE/4);
}
    7ce3:	5b                   	pop    %ebx
    7ce4:	5f                   	pop    %edi
    7ce5:	5d                   	pop    %ebp
    7ce6:	c3                   	ret    

00007ce7 <readseg>:

// Read 'count' bytes at 'offset' from kernel into physical address 'pa'.
// Might copy more than asked.
void
readseg(uchar* pa, uint count, uint offset)
{
    7ce7:	55                   	push   %ebp
    7ce8:	89 e5                	mov    %esp,%ebp
    7cea:	57                   	push   %edi
    7ceb:	56                   	push   %esi
    7cec:	53                   	push   %ebx
    7ced:	83 ec 08             	sub    $0x8,%esp
    7cf0:	8b 5d 08             	mov    0x8(%ebp),%ebx
    7cf3:	8b 75 10             	mov    0x10(%ebp),%esi
  uchar* epa;

  epa = pa + count;
    7cf6:	89 df                	mov    %ebx,%edi
    7cf8:	03 7d 0c             	add    0xc(%ebp),%edi

  // Round down to sector boundary.
  pa -= offset % SECTSIZE;
    7cfb:	89 f0                	mov    %esi,%eax
    7cfd:	25 ff 01 00 00       	and    $0x1ff,%eax
    7d02:	29 c3                	sub    %eax,%ebx

  // Translate from bytes to sectors; kernel starts at sector 1.
  offset = (offset / SECTSIZE) + 1;
    7d04:	c1 ee 09             	shr    $0x9,%esi
    7d07:	83 c6 01             	add    $0x1,%esi

  // If this is too slow, we could read lots of sectors at a time.
  // We'd write more to memory than asked, but it doesn't matter --
  // we load in increasing order.
  for(; pa < epa; pa += SECTSIZE, offset++)
    7d0a:	39 df                	cmp    %ebx,%edi
    7d0c:	76 19                	jbe    7d27 <readseg+0x40>
    readsect(pa, offset);
    7d0e:	89 74 24 04          	mov    %esi,0x4(%esp)
    7d12:	89 1c 24             	mov    %ebx,(%esp)
    7d15:	e8 76 ff ff ff       	call   7c90 <readsect>
  for(; pa < epa; pa += SECTSIZE, offset++)
    7d1a:	81 c3 00 02 00 00    	add    $0x200,%ebx
    7d20:	83 c6 01             	add    $0x1,%esi
    7d23:	39 df                	cmp    %ebx,%edi
    7d25:	77 e7                	ja     7d0e <readseg+0x27>
}
    7d27:	83 c4 08             	add    $0x8,%esp
    7d2a:	5b                   	pop    %ebx
    7d2b:	5e                   	pop    %esi
    7d2c:	5f                   	pop    %edi
    7d2d:	5d                   	pop    %ebp
    7d2e:	c3                   	ret    

00007d2f <bootmain>:
{
    7d2f:	55                   	push   %ebp
    7d30:	89 e5                	mov    %esp,%ebp
    7d32:	57                   	push   %edi
    7d33:	56                   	push   %esi
    7d34:	53                   	push   %ebx
    7d35:	83 ec 1c             	sub    $0x1c,%esp
  readseg((uchar*)elf, 4096, 0);
    7d38:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
    7d3f:	00 
    7d40:	c7 44 24 04 00 10 00 	movl   $0x1000,0x4(%esp)
    7d47:	00 
    7d48:	c7 04 24 00 00 01 00 	movl   $0x10000,(%esp)
    7d4f:	e8 93 ff ff ff       	call   7ce7 <readseg>
  if(elf->magic != ELF_MAGIC)
    7d54:	81 3d 00 00 01 00 7f 	cmpl   $0x464c457f,0x10000
    7d5b:	45 4c 46 
    7d5e:	75 57                	jne    7db7 <bootmain+0x88>
  ph = (struct proghdr*)((uchar*)elf + elf->phoff);
    7d60:	a1 1c 00 01 00       	mov    0x1001c,%eax
    7d65:	8d 98 00 00 01 00    	lea    0x10000(%eax),%ebx
  eph = ph + elf->phnum;
    7d6b:	0f b7 35 2c 00 01 00 	movzwl 0x1002c,%esi
    7d72:	c1 e6 05             	shl    $0x5,%esi
    7d75:	01 de                	add    %ebx,%esi
  for(; ph < eph; ph++){
    7d77:	39 f3                	cmp    %esi,%ebx
    7d79:	73 36                	jae    7db1 <bootmain+0x82>
    pa = (uchar*)ph->paddr;
    7d7b:	8b 7b 0c             	mov    0xc(%ebx),%edi
    readseg(pa, ph->filesz, ph->off);
    7d7e:	8b 43 04             	mov    0x4(%ebx),%eax
    7d81:	89 44 24 08          	mov    %eax,0x8(%esp)
    7d85:	8b 43 10             	mov    0x10(%ebx),%eax
    7d88:	89 44 24 04          	mov    %eax,0x4(%esp)
    7d8c:	89 3c 24             	mov    %edi,(%esp)
    7d8f:	e8 53 ff ff ff       	call   7ce7 <readseg>
    if(ph->memsz > ph->filesz)
    7d94:	8b 4b 14             	mov    0x14(%ebx),%ecx
    7d97:	8b 43 10             	mov    0x10(%ebx),%eax
    7d9a:	39 c1                	cmp    %eax,%ecx
    7d9c:	76 0c                	jbe    7daa <bootmain+0x7b>
      stosb(pa + ph->filesz, 0, ph->memsz - ph->filesz);
    7d9e:	01 c7                	add    %eax,%edi
    7da0:	29 c1                	sub    %eax,%ecx
}

static inline void
stosb(void *addr, int data, int cnt)
{
  asm volatile("cld; rep stosb" :
    7da2:	b8 00 00 00 00       	mov    $0x0,%eax
    7da7:	fc                   	cld    
    7da8:	f3 aa                	rep stos %al,%es:(%edi)
  for(; ph < eph; ph++){
    7daa:	83 c3 20             	add    $0x20,%ebx
    7dad:	39 de                	cmp    %ebx,%esi
    7daf:	77 ca                	ja     7d7b <bootmain+0x4c>
  entry();
    7db1:	ff 15 18 00 01 00    	call   *0x10018
}
    7db7:	83 c4 1c             	add    $0x1c,%esp
    7dba:	5b                   	pop    %ebx
    7dbb:	5e                   	pop    %esi
    7dbc:	5f                   	pop    %edi
    7dbd:	5d                   	pop    %ebp
    7dbe:	c3                   	ret    
