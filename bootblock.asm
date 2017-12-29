
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
    7c48:	e8 ee 00 00 00       	call   7d3b <bootmain>

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
    7ca8:	ba f3 01 00 00       	mov    $0x1f3,%edx
    7cad:	89 d8                	mov    %ebx,%eax
    7caf:	ee                   	out    %al,(%dx)
    7cb0:	89 d8                	mov    %ebx,%eax
    7cb2:	c1 e8 08             	shr    $0x8,%eax
    7cb5:	ba f4 01 00 00       	mov    $0x1f4,%edx
    7cba:	ee                   	out    %al,(%dx)
    7cbb:	89 d8                	mov    %ebx,%eax
    7cbd:	c1 e8 10             	shr    $0x10,%eax
    7cc0:	ba f5 01 00 00       	mov    $0x1f5,%edx
    7cc5:	ee                   	out    %al,(%dx)
    7cc6:	89 d8                	mov    %ebx,%eax
    7cc8:	c1 e8 18             	shr    $0x18,%eax
    7ccb:	83 c8 e0             	or     $0xffffffe0,%eax
    7cce:	ba f6 01 00 00       	mov    $0x1f6,%edx
    7cd3:	ee                   	out    %al,(%dx)
    7cd4:	ba f7 01 00 00       	mov    $0x1f7,%edx
    7cd9:	b8 20 00 00 00       	mov    $0x20,%eax
    7cde:	ee                   	out    %al,(%dx)
  outb(0x1F5, offset >> 16);
  outb(0x1F6, (offset >> 24) | 0xE0);
  outb(0x1F7, 0x20);  // cmd 0x20 - read sectors

  // Read data.
  waitdisk();
    7cdf:	e8 9a ff ff ff       	call   7c7e <waitdisk>
}

static inline void
insl(int port, void *addr, int cnt)
{
  asm volatile("cld; rep insl" :
    7ce4:	8b 7d 08             	mov    0x8(%ebp),%edi
    7ce7:	b9 80 00 00 00       	mov    $0x80,%ecx
    7cec:	ba f0 01 00 00       	mov    $0x1f0,%edx
    7cf1:	fc                   	cld    
    7cf2:	f3 6d                	rep insl (%dx),%es:(%edi)
  insl(0x1F0, dst, SECTSIZE/4);
}
    7cf4:	5b                   	pop    %ebx
    7cf5:	5f                   	pop    %edi
    7cf6:	5d                   	pop    %ebp
    7cf7:	c3                   	ret    

00007cf8 <readseg>:

// Read 'count' bytes at 'offset' from kernel into physical address 'pa'.
// Might copy more than asked.
void
readseg(uchar* pa, uint count, uint offset)
{
    7cf8:	55                   	push   %ebp
    7cf9:	89 e5                	mov    %esp,%ebp
    7cfb:	57                   	push   %edi
    7cfc:	56                   	push   %esi
    7cfd:	53                   	push   %ebx
    7cfe:	8b 5d 08             	mov    0x8(%ebp),%ebx
    7d01:	8b 75 10             	mov    0x10(%ebp),%esi
  uchar* epa;

  epa = pa + count;
    7d04:	89 df                	mov    %ebx,%edi
    7d06:	03 7d 0c             	add    0xc(%ebp),%edi

  // Round down to sector boundary.
  pa -= offset % SECTSIZE;
    7d09:	89 f0                	mov    %esi,%eax
    7d0b:	25 ff 01 00 00       	and    $0x1ff,%eax
    7d10:	29 c3                	sub    %eax,%ebx

  // Translate from bytes to sectors; kernel starts at sector 1.
  offset = (offset / SECTSIZE) + 1;
    7d12:	c1 ee 09             	shr    $0x9,%esi
    7d15:	83 c6 01             	add    $0x1,%esi

  // If this is too slow, we could read lots of sectors at a time.
  // We'd write more to memory than asked, but it doesn't matter --
  // we load in increasing order.
  for(; pa < epa; pa += SECTSIZE, offset++)
    7d18:	39 df                	cmp    %ebx,%edi
    7d1a:	76 17                	jbe    7d33 <readseg+0x3b>
    readsect(pa, offset);
    7d1c:	56                   	push   %esi
    7d1d:	53                   	push   %ebx
    7d1e:	e8 6d ff ff ff       	call   7c90 <readsect>
  offset = (offset / SECTSIZE) + 1;

  // If this is too slow, we could read lots of sectors at a time.
  // We'd write more to memory than asked, but it doesn't matter --
  // we load in increasing order.
  for(; pa < epa; pa += SECTSIZE, offset++)
    7d23:	81 c3 00 02 00 00    	add    $0x200,%ebx
    7d29:	83 c6 01             	add    $0x1,%esi
    7d2c:	83 c4 08             	add    $0x8,%esp
    7d2f:	39 df                	cmp    %ebx,%edi
    7d31:	77 e9                	ja     7d1c <readseg+0x24>
    readsect(pa, offset);
}
    7d33:	8d 65 f4             	lea    -0xc(%ebp),%esp
    7d36:	5b                   	pop    %ebx
    7d37:	5e                   	pop    %esi
    7d38:	5f                   	pop    %edi
    7d39:	5d                   	pop    %ebp
    7d3a:	c3                   	ret    

00007d3b <bootmain>:

void readseg(uchar*, uint, uint);

void
bootmain(void)
{
    7d3b:	55                   	push   %ebp
    7d3c:	89 e5                	mov    %esp,%ebp
    7d3e:	57                   	push   %edi
    7d3f:	56                   	push   %esi
    7d40:	53                   	push   %ebx
    7d41:	83 ec 0c             	sub    $0xc,%esp
  uchar* pa;

  elf = (struct elfhdr*)0x10000;  // scratch space

  // Read 1st page off disk
  readseg((uchar*)elf, 4096, 0);
    7d44:	6a 00                	push   $0x0
    7d46:	68 00 10 00 00       	push   $0x1000
    7d4b:	68 00 00 01 00       	push   $0x10000
    7d50:	e8 a3 ff ff ff       	call   7cf8 <readseg>

  // Is this an ELF executable?
  if(elf->magic != ELF_MAGIC)
    7d55:	83 c4 0c             	add    $0xc,%esp
    7d58:	81 3d 00 00 01 00 7f 	cmpl   $0x464c457f,0x10000
    7d5f:	45 4c 46 
    7d62:	75 50                	jne    7db4 <bootmain+0x79>
    return;  // let bootasm.S handle error

  // Load each program segment (ignores ph flags).
  ph = (struct proghdr*)((uchar*)elf + elf->phoff);
    7d64:	a1 1c 00 01 00       	mov    0x1001c,%eax
    7d69:	8d 98 00 00 01 00    	lea    0x10000(%eax),%ebx
  eph = ph + elf->phnum;
    7d6f:	0f b7 35 2c 00 01 00 	movzwl 0x1002c,%esi
    7d76:	c1 e6 05             	shl    $0x5,%esi
    7d79:	01 de                	add    %ebx,%esi
  for(; ph < eph; ph++){
    7d7b:	39 f3                	cmp    %esi,%ebx
    7d7d:	73 2f                	jae    7dae <bootmain+0x73>
    pa = (uchar*)ph->paddr;
    7d7f:	8b 7b 0c             	mov    0xc(%ebx),%edi
    readseg(pa, ph->filesz, ph->off);
    7d82:	ff 73 04             	pushl  0x4(%ebx)
    7d85:	ff 73 10             	pushl  0x10(%ebx)
    7d88:	57                   	push   %edi
    7d89:	e8 6a ff ff ff       	call   7cf8 <readseg>
    if(ph->memsz > ph->filesz)
    7d8e:	8b 4b 14             	mov    0x14(%ebx),%ecx
    7d91:	8b 43 10             	mov    0x10(%ebx),%eax
    7d94:	83 c4 0c             	add    $0xc,%esp
    7d97:	39 c1                	cmp    %eax,%ecx
    7d99:	76 0c                	jbe    7da7 <bootmain+0x6c>
}

static inline void
stosb(void *addr, int data, int cnt)
{
  asm volatile("cld; rep stosb" :
    7d9b:	01 c7                	add    %eax,%edi
    7d9d:	29 c1                	sub    %eax,%ecx
    7d9f:	b8 00 00 00 00       	mov    $0x0,%eax
    7da4:	fc                   	cld    
    7da5:	f3 aa                	rep stos %al,%es:(%edi)
    return;  // let bootasm.S handle error

  // Load each program segment (ignores ph flags).
  ph = (struct proghdr*)((uchar*)elf + elf->phoff);
  eph = ph + elf->phnum;
  for(; ph < eph; ph++){
    7da7:	83 c3 20             	add    $0x20,%ebx
    7daa:	39 de                	cmp    %ebx,%esi
    7dac:	77 d1                	ja     7d7f <bootmain+0x44>
  }

  // Call the entry point from the ELF header.
  // Does not return!
  entry = (void(*)(void))(elf->entry);
  entry();
    7dae:	ff 15 18 00 01 00    	call   *0x10018
}
    7db4:	8d 65 f4             	lea    -0xc(%ebp),%esp
    7db7:	5b                   	pop    %ebx
    7db8:	5e                   	pop    %esi
    7db9:	5f                   	pop    %edi
    7dba:	5d                   	pop    %ebp
    7dbb:	c3                   	ret    
