
_hello:     file format elf32-i386


Disassembly of section .text:

00000000 <main>:
#include "types.h"
#include "user.h"

int main(int argc, char **argv)
{
   0:	55                   	push   %ebp
   1:	89 e5                	mov    %esp,%ebp
   3:	83 e4 f0             	and    $0xfffffff0,%esp
   6:	83 ec 20             	sub    $0x20,%esp
	int exitStatus = 10000000;
	printf(1, "Waiting...\n");
   9:	c7 44 24 04 36 07 00 	movl   $0x736,0x4(%esp)
  10:	00 
  11:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
	int exitStatus = 10000000;
  18:	c7 44 24 1c 80 96 98 	movl   $0x989680,0x1c(%esp)
  1f:	00 
	printf(1, "Waiting...\n");
  20:	e8 ab 03 00 00       	call   3d0 <printf>
	wait(&exitStatus);
  25:	8d 44 24 1c          	lea    0x1c(%esp),%eax
  29:	89 04 24             	mov    %eax,(%esp)
  2c:	e8 59 02 00 00       	call   28a <wait>
	printf(1, "Hello World!\n");
  31:	c7 44 24 04 42 07 00 	movl   $0x742,0x4(%esp)
  38:	00 
  39:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
  40:	e8 8b 03 00 00       	call   3d0 <printf>
	exit(0);
  45:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  4c:	e8 31 02 00 00       	call   282 <exit>
  51:	66 90                	xchg   %ax,%ax
  53:	66 90                	xchg   %ax,%ax
  55:	66 90                	xchg   %ax,%ax
  57:	66 90                	xchg   %ax,%ax
  59:	66 90                	xchg   %ax,%ax
  5b:	66 90                	xchg   %ax,%ax
  5d:	66 90                	xchg   %ax,%ax
  5f:	90                   	nop

00000060 <strcpy>:
#include "user.h"
#include "x86.h"

char*
strcpy(char *s, const char *t)
{
  60:	55                   	push   %ebp
  61:	89 e5                	mov    %esp,%ebp
  63:	8b 45 08             	mov    0x8(%ebp),%eax
  66:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  69:	53                   	push   %ebx
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
  6a:	89 c2                	mov    %eax,%edx
  6c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  70:	83 c1 01             	add    $0x1,%ecx
  73:	0f b6 59 ff          	movzbl -0x1(%ecx),%ebx
  77:	83 c2 01             	add    $0x1,%edx
  7a:	84 db                	test   %bl,%bl
  7c:	88 5a ff             	mov    %bl,-0x1(%edx)
  7f:	75 ef                	jne    70 <strcpy+0x10>
    ;
  return os;
}
  81:	5b                   	pop    %ebx
  82:	5d                   	pop    %ebp
  83:	c3                   	ret    
  84:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  8a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

00000090 <strcmp>:

int
strcmp(const char *p, const char *q)
{
  90:	55                   	push   %ebp
  91:	89 e5                	mov    %esp,%ebp
  93:	8b 55 08             	mov    0x8(%ebp),%edx
  96:	53                   	push   %ebx
  97:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  while(*p && *p == *q)
  9a:	0f b6 02             	movzbl (%edx),%eax
  9d:	84 c0                	test   %al,%al
  9f:	74 2d                	je     ce <strcmp+0x3e>
  a1:	0f b6 19             	movzbl (%ecx),%ebx
  a4:	38 d8                	cmp    %bl,%al
  a6:	74 0e                	je     b6 <strcmp+0x26>
  a8:	eb 2b                	jmp    d5 <strcmp+0x45>
  aa:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  b0:	38 c8                	cmp    %cl,%al
  b2:	75 15                	jne    c9 <strcmp+0x39>
    p++, q++;
  b4:	89 d9                	mov    %ebx,%ecx
  b6:	83 c2 01             	add    $0x1,%edx
  while(*p && *p == *q)
  b9:	0f b6 02             	movzbl (%edx),%eax
    p++, q++;
  bc:	8d 59 01             	lea    0x1(%ecx),%ebx
  while(*p && *p == *q)
  bf:	0f b6 49 01          	movzbl 0x1(%ecx),%ecx
  c3:	84 c0                	test   %al,%al
  c5:	75 e9                	jne    b0 <strcmp+0x20>
  c7:	31 c0                	xor    %eax,%eax
  return (uchar)*p - (uchar)*q;
  c9:	29 c8                	sub    %ecx,%eax
}
  cb:	5b                   	pop    %ebx
  cc:	5d                   	pop    %ebp
  cd:	c3                   	ret    
  ce:	0f b6 09             	movzbl (%ecx),%ecx
  while(*p && *p == *q)
  d1:	31 c0                	xor    %eax,%eax
  d3:	eb f4                	jmp    c9 <strcmp+0x39>
  d5:	0f b6 cb             	movzbl %bl,%ecx
  d8:	eb ef                	jmp    c9 <strcmp+0x39>
  da:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

000000e0 <strlen>:

uint
strlen(const char *s)
{
  e0:	55                   	push   %ebp
  e1:	89 e5                	mov    %esp,%ebp
  e3:	8b 4d 08             	mov    0x8(%ebp),%ecx
  int n;

  for(n = 0; s[n]; n++)
  e6:	80 39 00             	cmpb   $0x0,(%ecx)
  e9:	74 12                	je     fd <strlen+0x1d>
  eb:	31 d2                	xor    %edx,%edx
  ed:	8d 76 00             	lea    0x0(%esi),%esi
  f0:	83 c2 01             	add    $0x1,%edx
  f3:	80 3c 11 00          	cmpb   $0x0,(%ecx,%edx,1)
  f7:	89 d0                	mov    %edx,%eax
  f9:	75 f5                	jne    f0 <strlen+0x10>
    ;
  return n;
}
  fb:	5d                   	pop    %ebp
  fc:	c3                   	ret    
  for(n = 0; s[n]; n++)
  fd:	31 c0                	xor    %eax,%eax
}
  ff:	5d                   	pop    %ebp
 100:	c3                   	ret    
 101:	eb 0d                	jmp    110 <memset>
 103:	90                   	nop
 104:	90                   	nop
 105:	90                   	nop
 106:	90                   	nop
 107:	90                   	nop
 108:	90                   	nop
 109:	90                   	nop
 10a:	90                   	nop
 10b:	90                   	nop
 10c:	90                   	nop
 10d:	90                   	nop
 10e:	90                   	nop
 10f:	90                   	nop

00000110 <memset>:

void*
memset(void *dst, int c, uint n)
{
 110:	55                   	push   %ebp
 111:	89 e5                	mov    %esp,%ebp
 113:	8b 55 08             	mov    0x8(%ebp),%edx
 116:	57                   	push   %edi
}

static inline void
stosb(void *addr, int data, int cnt)
{
  asm volatile("cld; rep stosb" :
 117:	8b 4d 10             	mov    0x10(%ebp),%ecx
 11a:	8b 45 0c             	mov    0xc(%ebp),%eax
 11d:	89 d7                	mov    %edx,%edi
 11f:	fc                   	cld    
 120:	f3 aa                	rep stos %al,%es:(%edi)
  stosb(dst, c, n);
  return dst;
}
 122:	89 d0                	mov    %edx,%eax
 124:	5f                   	pop    %edi
 125:	5d                   	pop    %ebp
 126:	c3                   	ret    
 127:	89 f6                	mov    %esi,%esi
 129:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000130 <strchr>:

char*
strchr(const char *s, char c)
{
 130:	55                   	push   %ebp
 131:	89 e5                	mov    %esp,%ebp
 133:	8b 45 08             	mov    0x8(%ebp),%eax
 136:	53                   	push   %ebx
 137:	8b 55 0c             	mov    0xc(%ebp),%edx
  for(; *s; s++)
 13a:	0f b6 18             	movzbl (%eax),%ebx
 13d:	84 db                	test   %bl,%bl
 13f:	74 1d                	je     15e <strchr+0x2e>
    if(*s == c)
 141:	38 d3                	cmp    %dl,%bl
 143:	89 d1                	mov    %edx,%ecx
 145:	75 0d                	jne    154 <strchr+0x24>
 147:	eb 17                	jmp    160 <strchr+0x30>
 149:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 150:	38 ca                	cmp    %cl,%dl
 152:	74 0c                	je     160 <strchr+0x30>
  for(; *s; s++)
 154:	83 c0 01             	add    $0x1,%eax
 157:	0f b6 10             	movzbl (%eax),%edx
 15a:	84 d2                	test   %dl,%dl
 15c:	75 f2                	jne    150 <strchr+0x20>
      return (char*)s;
  return 0;
 15e:	31 c0                	xor    %eax,%eax
}
 160:	5b                   	pop    %ebx
 161:	5d                   	pop    %ebp
 162:	c3                   	ret    
 163:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 169:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000170 <gets>:

char*
gets(char *buf, int max)
{
 170:	55                   	push   %ebp
 171:	89 e5                	mov    %esp,%ebp
 173:	57                   	push   %edi
 174:	56                   	push   %esi
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
 175:	31 f6                	xor    %esi,%esi
{
 177:	53                   	push   %ebx
 178:	83 ec 2c             	sub    $0x2c,%esp
    cc = read(0, &c, 1);
 17b:	8d 7d e7             	lea    -0x19(%ebp),%edi
  for(i=0; i+1 < max; ){
 17e:	eb 31                	jmp    1b1 <gets+0x41>
    cc = read(0, &c, 1);
 180:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 187:	00 
 188:	89 7c 24 04          	mov    %edi,0x4(%esp)
 18c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
 193:	e8 02 01 00 00       	call   29a <read>
    if(cc < 1)
 198:	85 c0                	test   %eax,%eax
 19a:	7e 1d                	jle    1b9 <gets+0x49>
      break;
    buf[i++] = c;
 19c:	0f b6 45 e7          	movzbl -0x19(%ebp),%eax
  for(i=0; i+1 < max; ){
 1a0:	89 de                	mov    %ebx,%esi
    buf[i++] = c;
 1a2:	8b 55 08             	mov    0x8(%ebp),%edx
    if(c == '\n' || c == '\r')
 1a5:	3c 0d                	cmp    $0xd,%al
    buf[i++] = c;
 1a7:	88 44 1a ff          	mov    %al,-0x1(%edx,%ebx,1)
    if(c == '\n' || c == '\r')
 1ab:	74 0c                	je     1b9 <gets+0x49>
 1ad:	3c 0a                	cmp    $0xa,%al
 1af:	74 08                	je     1b9 <gets+0x49>
  for(i=0; i+1 < max; ){
 1b1:	8d 5e 01             	lea    0x1(%esi),%ebx
 1b4:	3b 5d 0c             	cmp    0xc(%ebp),%ebx
 1b7:	7c c7                	jl     180 <gets+0x10>
      break;
  }
  buf[i] = '\0';
 1b9:	8b 45 08             	mov    0x8(%ebp),%eax
 1bc:	c6 04 30 00          	movb   $0x0,(%eax,%esi,1)
  return buf;
}
 1c0:	83 c4 2c             	add    $0x2c,%esp
 1c3:	5b                   	pop    %ebx
 1c4:	5e                   	pop    %esi
 1c5:	5f                   	pop    %edi
 1c6:	5d                   	pop    %ebp
 1c7:	c3                   	ret    
 1c8:	90                   	nop
 1c9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

000001d0 <stat>:

int
stat(const char *n, struct stat *st)
{
 1d0:	55                   	push   %ebp
 1d1:	89 e5                	mov    %esp,%ebp
 1d3:	56                   	push   %esi
 1d4:	53                   	push   %ebx
 1d5:	83 ec 10             	sub    $0x10,%esp
  int fd;
  int r;

  fd = open(n, O_RDONLY);
 1d8:	8b 45 08             	mov    0x8(%ebp),%eax
 1db:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
 1e2:	00 
 1e3:	89 04 24             	mov    %eax,(%esp)
 1e6:	e8 d7 00 00 00       	call   2c2 <open>
  if(fd < 0)
 1eb:	85 c0                	test   %eax,%eax
  fd = open(n, O_RDONLY);
 1ed:	89 c3                	mov    %eax,%ebx
  if(fd < 0)
 1ef:	78 27                	js     218 <stat+0x48>
    return -1;
  r = fstat(fd, st);
 1f1:	8b 45 0c             	mov    0xc(%ebp),%eax
 1f4:	89 1c 24             	mov    %ebx,(%esp)
 1f7:	89 44 24 04          	mov    %eax,0x4(%esp)
 1fb:	e8 da 00 00 00       	call   2da <fstat>
  close(fd);
 200:	89 1c 24             	mov    %ebx,(%esp)
  r = fstat(fd, st);
 203:	89 c6                	mov    %eax,%esi
  close(fd);
 205:	e8 a0 00 00 00       	call   2aa <close>
  return r;
 20a:	89 f0                	mov    %esi,%eax
}
 20c:	83 c4 10             	add    $0x10,%esp
 20f:	5b                   	pop    %ebx
 210:	5e                   	pop    %esi
 211:	5d                   	pop    %ebp
 212:	c3                   	ret    
 213:	90                   	nop
 214:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    return -1;
 218:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
 21d:	eb ed                	jmp    20c <stat+0x3c>
 21f:	90                   	nop

00000220 <atoi>:

int
atoi(const char *s)
{
 220:	55                   	push   %ebp
 221:	89 e5                	mov    %esp,%ebp
 223:	8b 4d 08             	mov    0x8(%ebp),%ecx
 226:	53                   	push   %ebx
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
 227:	0f be 11             	movsbl (%ecx),%edx
 22a:	8d 42 d0             	lea    -0x30(%edx),%eax
 22d:	3c 09                	cmp    $0x9,%al
  n = 0;
 22f:	b8 00 00 00 00       	mov    $0x0,%eax
  while('0' <= *s && *s <= '9')
 234:	77 17                	ja     24d <atoi+0x2d>
 236:	66 90                	xchg   %ax,%ax
    n = n*10 + *s++ - '0';
 238:	83 c1 01             	add    $0x1,%ecx
 23b:	8d 04 80             	lea    (%eax,%eax,4),%eax
 23e:	8d 44 42 d0          	lea    -0x30(%edx,%eax,2),%eax
  while('0' <= *s && *s <= '9')
 242:	0f be 11             	movsbl (%ecx),%edx
 245:	8d 5a d0             	lea    -0x30(%edx),%ebx
 248:	80 fb 09             	cmp    $0x9,%bl
 24b:	76 eb                	jbe    238 <atoi+0x18>
  return n;
}
 24d:	5b                   	pop    %ebx
 24e:	5d                   	pop    %ebp
 24f:	c3                   	ret    

00000250 <memmove>:

void*
memmove(void *vdst, const void *vsrc, int n)
{
 250:	55                   	push   %ebp
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
 251:	31 d2                	xor    %edx,%edx
{
 253:	89 e5                	mov    %esp,%ebp
 255:	56                   	push   %esi
 256:	8b 45 08             	mov    0x8(%ebp),%eax
 259:	53                   	push   %ebx
 25a:	8b 5d 10             	mov    0x10(%ebp),%ebx
 25d:	8b 75 0c             	mov    0xc(%ebp),%esi
  while(n-- > 0)
 260:	85 db                	test   %ebx,%ebx
 262:	7e 12                	jle    276 <memmove+0x26>
 264:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    *dst++ = *src++;
 268:	0f b6 0c 16          	movzbl (%esi,%edx,1),%ecx
 26c:	88 0c 10             	mov    %cl,(%eax,%edx,1)
 26f:	83 c2 01             	add    $0x1,%edx
  while(n-- > 0)
 272:	39 da                	cmp    %ebx,%edx
 274:	75 f2                	jne    268 <memmove+0x18>
  return vdst;
}
 276:	5b                   	pop    %ebx
 277:	5e                   	pop    %esi
 278:	5d                   	pop    %ebp
 279:	c3                   	ret    

0000027a <fork>:
 27a:	b8 01 00 00 00       	mov    $0x1,%eax
 27f:	cd 40                	int    $0x40
 281:	c3                   	ret    

00000282 <exit>:
 282:	b8 02 00 00 00       	mov    $0x2,%eax
 287:	cd 40                	int    $0x40
 289:	c3                   	ret    

0000028a <wait>:
 28a:	b8 03 00 00 00       	mov    $0x3,%eax
 28f:	cd 40                	int    $0x40
 291:	c3                   	ret    

00000292 <pipe>:
 292:	b8 04 00 00 00       	mov    $0x4,%eax
 297:	cd 40                	int    $0x40
 299:	c3                   	ret    

0000029a <read>:
 29a:	b8 05 00 00 00       	mov    $0x5,%eax
 29f:	cd 40                	int    $0x40
 2a1:	c3                   	ret    

000002a2 <write>:
 2a2:	b8 10 00 00 00       	mov    $0x10,%eax
 2a7:	cd 40                	int    $0x40
 2a9:	c3                   	ret    

000002aa <close>:
 2aa:	b8 15 00 00 00       	mov    $0x15,%eax
 2af:	cd 40                	int    $0x40
 2b1:	c3                   	ret    

000002b2 <kill>:
 2b2:	b8 06 00 00 00       	mov    $0x6,%eax
 2b7:	cd 40                	int    $0x40
 2b9:	c3                   	ret    

000002ba <exec>:
 2ba:	b8 07 00 00 00       	mov    $0x7,%eax
 2bf:	cd 40                	int    $0x40
 2c1:	c3                   	ret    

000002c2 <open>:
 2c2:	b8 0f 00 00 00       	mov    $0xf,%eax
 2c7:	cd 40                	int    $0x40
 2c9:	c3                   	ret    

000002ca <mknod>:
 2ca:	b8 11 00 00 00       	mov    $0x11,%eax
 2cf:	cd 40                	int    $0x40
 2d1:	c3                   	ret    

000002d2 <unlink>:
 2d2:	b8 12 00 00 00       	mov    $0x12,%eax
 2d7:	cd 40                	int    $0x40
 2d9:	c3                   	ret    

000002da <fstat>:
 2da:	b8 08 00 00 00       	mov    $0x8,%eax
 2df:	cd 40                	int    $0x40
 2e1:	c3                   	ret    

000002e2 <link>:
 2e2:	b8 13 00 00 00       	mov    $0x13,%eax
 2e7:	cd 40                	int    $0x40
 2e9:	c3                   	ret    

000002ea <mkdir>:
 2ea:	b8 14 00 00 00       	mov    $0x14,%eax
 2ef:	cd 40                	int    $0x40
 2f1:	c3                   	ret    

000002f2 <chdir>:
 2f2:	b8 09 00 00 00       	mov    $0x9,%eax
 2f7:	cd 40                	int    $0x40
 2f9:	c3                   	ret    

000002fa <dup>:
 2fa:	b8 0a 00 00 00       	mov    $0xa,%eax
 2ff:	cd 40                	int    $0x40
 301:	c3                   	ret    

00000302 <getpid>:
 302:	b8 0b 00 00 00       	mov    $0xb,%eax
 307:	cd 40                	int    $0x40
 309:	c3                   	ret    

0000030a <sbrk>:
 30a:	b8 0c 00 00 00       	mov    $0xc,%eax
 30f:	cd 40                	int    $0x40
 311:	c3                   	ret    

00000312 <sleep>:
 312:	b8 0d 00 00 00       	mov    $0xd,%eax
 317:	cd 40                	int    $0x40
 319:	c3                   	ret    

0000031a <uptime>:
 31a:	b8 0e 00 00 00       	mov    $0xe,%eax
 31f:	cd 40                	int    $0x40
 321:	c3                   	ret    
 322:	66 90                	xchg   %ax,%ax
 324:	66 90                	xchg   %ax,%ax
 326:	66 90                	xchg   %ax,%ax
 328:	66 90                	xchg   %ax,%ax
 32a:	66 90                	xchg   %ax,%ax
 32c:	66 90                	xchg   %ax,%ax
 32e:	66 90                	xchg   %ax,%ax

00000330 <printint>:
  write(fd, &c, 1);
}

static void
printint(int fd, int xx, int base, int sgn)
{
 330:	55                   	push   %ebp
 331:	89 e5                	mov    %esp,%ebp
 333:	57                   	push   %edi
 334:	56                   	push   %esi
 335:	89 c6                	mov    %eax,%esi
 337:	53                   	push   %ebx
 338:	83 ec 4c             	sub    $0x4c,%esp
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
 33b:	8b 5d 08             	mov    0x8(%ebp),%ebx
 33e:	85 db                	test   %ebx,%ebx
 340:	74 09                	je     34b <printint+0x1b>
 342:	89 d0                	mov    %edx,%eax
 344:	c1 e8 1f             	shr    $0x1f,%eax
 347:	84 c0                	test   %al,%al
 349:	75 75                	jne    3c0 <printint+0x90>
    neg = 1;
    x = -xx;
  } else {
    x = xx;
 34b:	89 d0                	mov    %edx,%eax
  neg = 0;
 34d:	c7 45 c4 00 00 00 00 	movl   $0x0,-0x3c(%ebp)
 354:	89 75 c0             	mov    %esi,-0x40(%ebp)
  }

  i = 0;
 357:	31 ff                	xor    %edi,%edi
 359:	89 ce                	mov    %ecx,%esi
 35b:	8d 5d d7             	lea    -0x29(%ebp),%ebx
 35e:	eb 02                	jmp    362 <printint+0x32>
  do{
    buf[i++] = digits[x % base];
 360:	89 cf                	mov    %ecx,%edi
 362:	31 d2                	xor    %edx,%edx
 364:	f7 f6                	div    %esi
 366:	8d 4f 01             	lea    0x1(%edi),%ecx
 369:	0f b6 92 57 07 00 00 	movzbl 0x757(%edx),%edx
  }while((x /= base) != 0);
 370:	85 c0                	test   %eax,%eax
    buf[i++] = digits[x % base];
 372:	88 14 0b             	mov    %dl,(%ebx,%ecx,1)
  }while((x /= base) != 0);
 375:	75 e9                	jne    360 <printint+0x30>
  if(neg)
 377:	8b 55 c4             	mov    -0x3c(%ebp),%edx
    buf[i++] = digits[x % base];
 37a:	89 c8                	mov    %ecx,%eax
 37c:	8b 75 c0             	mov    -0x40(%ebp),%esi
  if(neg)
 37f:	85 d2                	test   %edx,%edx
 381:	74 08                	je     38b <printint+0x5b>
    buf[i++] = '-';
 383:	8d 4f 02             	lea    0x2(%edi),%ecx
 386:	c6 44 05 d8 2d       	movb   $0x2d,-0x28(%ebp,%eax,1)

  while(--i >= 0)
 38b:	8d 79 ff             	lea    -0x1(%ecx),%edi
 38e:	66 90                	xchg   %ax,%ax
 390:	0f b6 44 3d d8       	movzbl -0x28(%ebp,%edi,1),%eax
 395:	83 ef 01             	sub    $0x1,%edi
  write(fd, &c, 1);
 398:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 39f:	00 
 3a0:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 3a4:	89 34 24             	mov    %esi,(%esp)
 3a7:	88 45 d7             	mov    %al,-0x29(%ebp)
 3aa:	e8 f3 fe ff ff       	call   2a2 <write>
  while(--i >= 0)
 3af:	83 ff ff             	cmp    $0xffffffff,%edi
 3b2:	75 dc                	jne    390 <printint+0x60>
    putc(fd, buf[i]);
}
 3b4:	83 c4 4c             	add    $0x4c,%esp
 3b7:	5b                   	pop    %ebx
 3b8:	5e                   	pop    %esi
 3b9:	5f                   	pop    %edi
 3ba:	5d                   	pop    %ebp
 3bb:	c3                   	ret    
 3bc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    x = -xx;
 3c0:	89 d0                	mov    %edx,%eax
 3c2:	f7 d8                	neg    %eax
    neg = 1;
 3c4:	c7 45 c4 01 00 00 00 	movl   $0x1,-0x3c(%ebp)
 3cb:	eb 87                	jmp    354 <printint+0x24>
 3cd:	8d 76 00             	lea    0x0(%esi),%esi

000003d0 <printf>:

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, const char *fmt, ...)
{
 3d0:	55                   	push   %ebp
 3d1:	89 e5                	mov    %esp,%ebp
 3d3:	57                   	push   %edi
  char *s;
  int c, i, state;
  uint *ap;

  state = 0;
 3d4:	31 ff                	xor    %edi,%edi
{
 3d6:	56                   	push   %esi
 3d7:	53                   	push   %ebx
 3d8:	83 ec 3c             	sub    $0x3c,%esp
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
 3db:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  ap = (uint*)(void*)&fmt + 1;
 3de:	8d 45 10             	lea    0x10(%ebp),%eax
{
 3e1:	8b 75 08             	mov    0x8(%ebp),%esi
  ap = (uint*)(void*)&fmt + 1;
 3e4:	89 45 d4             	mov    %eax,-0x2c(%ebp)
  for(i = 0; fmt[i]; i++){
 3e7:	0f b6 13             	movzbl (%ebx),%edx
 3ea:	83 c3 01             	add    $0x1,%ebx
 3ed:	84 d2                	test   %dl,%dl
 3ef:	75 39                	jne    42a <printf+0x5a>
 3f1:	e9 c2 00 00 00       	jmp    4b8 <printf+0xe8>
 3f6:	66 90                	xchg   %ax,%ax
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
 3f8:	83 fa 25             	cmp    $0x25,%edx
 3fb:	0f 84 bf 00 00 00    	je     4c0 <printf+0xf0>
  write(fd, &c, 1);
 401:	8d 45 e2             	lea    -0x1e(%ebp),%eax
 404:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 40b:	00 
 40c:	89 44 24 04          	mov    %eax,0x4(%esp)
 410:	89 34 24             	mov    %esi,(%esp)
        state = '%';
      } else {
        putc(fd, c);
 413:	88 55 e2             	mov    %dl,-0x1e(%ebp)
  write(fd, &c, 1);
 416:	e8 87 fe ff ff       	call   2a2 <write>
 41b:	83 c3 01             	add    $0x1,%ebx
  for(i = 0; fmt[i]; i++){
 41e:	0f b6 53 ff          	movzbl -0x1(%ebx),%edx
 422:	84 d2                	test   %dl,%dl
 424:	0f 84 8e 00 00 00    	je     4b8 <printf+0xe8>
    if(state == 0){
 42a:	85 ff                	test   %edi,%edi
    c = fmt[i] & 0xff;
 42c:	0f be c2             	movsbl %dl,%eax
    if(state == 0){
 42f:	74 c7                	je     3f8 <printf+0x28>
      }
    } else if(state == '%'){
 431:	83 ff 25             	cmp    $0x25,%edi
 434:	75 e5                	jne    41b <printf+0x4b>
      if(c == 'd'){
 436:	83 fa 64             	cmp    $0x64,%edx
 439:	0f 84 31 01 00 00    	je     570 <printf+0x1a0>
        printint(fd, *ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
 43f:	25 f7 00 00 00       	and    $0xf7,%eax
 444:	83 f8 70             	cmp    $0x70,%eax
 447:	0f 84 83 00 00 00    	je     4d0 <printf+0x100>
        printint(fd, *ap, 16, 0);
        ap++;
      } else if(c == 's'){
 44d:	83 fa 73             	cmp    $0x73,%edx
 450:	0f 84 a2 00 00 00    	je     4f8 <printf+0x128>
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
        }
      } else if(c == 'c'){
 456:	83 fa 63             	cmp    $0x63,%edx
 459:	0f 84 35 01 00 00    	je     594 <printf+0x1c4>
        putc(fd, *ap);
        ap++;
      } else if(c == '%'){
 45f:	83 fa 25             	cmp    $0x25,%edx
 462:	0f 84 e0 00 00 00    	je     548 <printf+0x178>
  write(fd, &c, 1);
 468:	8d 45 e6             	lea    -0x1a(%ebp),%eax
 46b:	83 c3 01             	add    $0x1,%ebx
 46e:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 475:	00 
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
 476:	31 ff                	xor    %edi,%edi
  write(fd, &c, 1);
 478:	89 44 24 04          	mov    %eax,0x4(%esp)
 47c:	89 34 24             	mov    %esi,(%esp)
 47f:	89 55 d0             	mov    %edx,-0x30(%ebp)
 482:	c6 45 e6 25          	movb   $0x25,-0x1a(%ebp)
 486:	e8 17 fe ff ff       	call   2a2 <write>
        putc(fd, c);
 48b:	8b 55 d0             	mov    -0x30(%ebp),%edx
  write(fd, &c, 1);
 48e:	8d 45 e7             	lea    -0x19(%ebp),%eax
 491:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 498:	00 
 499:	89 44 24 04          	mov    %eax,0x4(%esp)
 49d:	89 34 24             	mov    %esi,(%esp)
        putc(fd, c);
 4a0:	88 55 e7             	mov    %dl,-0x19(%ebp)
  write(fd, &c, 1);
 4a3:	e8 fa fd ff ff       	call   2a2 <write>
  for(i = 0; fmt[i]; i++){
 4a8:	0f b6 53 ff          	movzbl -0x1(%ebx),%edx
 4ac:	84 d2                	test   %dl,%dl
 4ae:	0f 85 76 ff ff ff    	jne    42a <printf+0x5a>
 4b4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    }
  }
}
 4b8:	83 c4 3c             	add    $0x3c,%esp
 4bb:	5b                   	pop    %ebx
 4bc:	5e                   	pop    %esi
 4bd:	5f                   	pop    %edi
 4be:	5d                   	pop    %ebp
 4bf:	c3                   	ret    
        state = '%';
 4c0:	bf 25 00 00 00       	mov    $0x25,%edi
 4c5:	e9 51 ff ff ff       	jmp    41b <printf+0x4b>
 4ca:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        printint(fd, *ap, 16, 0);
 4d0:	8b 45 d4             	mov    -0x2c(%ebp),%eax
 4d3:	b9 10 00 00 00       	mov    $0x10,%ecx
      state = 0;
 4d8:	31 ff                	xor    %edi,%edi
        printint(fd, *ap, 16, 0);
 4da:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
 4e1:	8b 10                	mov    (%eax),%edx
 4e3:	89 f0                	mov    %esi,%eax
 4e5:	e8 46 fe ff ff       	call   330 <printint>
        ap++;
 4ea:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
 4ee:	e9 28 ff ff ff       	jmp    41b <printf+0x4b>
 4f3:	90                   	nop
 4f4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
        s = (char*)*ap;
 4f8:	8b 45 d4             	mov    -0x2c(%ebp),%eax
        ap++;
 4fb:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
        s = (char*)*ap;
 4ff:	8b 38                	mov    (%eax),%edi
          s = "(null)";
 501:	b8 50 07 00 00       	mov    $0x750,%eax
 506:	85 ff                	test   %edi,%edi
 508:	0f 44 f8             	cmove  %eax,%edi
        while(*s != 0){
 50b:	0f b6 07             	movzbl (%edi),%eax
 50e:	84 c0                	test   %al,%al
 510:	74 2a                	je     53c <printf+0x16c>
 512:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 518:	88 45 e3             	mov    %al,-0x1d(%ebp)
  write(fd, &c, 1);
 51b:	8d 45 e3             	lea    -0x1d(%ebp),%eax
          s++;
 51e:	83 c7 01             	add    $0x1,%edi
  write(fd, &c, 1);
 521:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 528:	00 
 529:	89 44 24 04          	mov    %eax,0x4(%esp)
 52d:	89 34 24             	mov    %esi,(%esp)
 530:	e8 6d fd ff ff       	call   2a2 <write>
        while(*s != 0){
 535:	0f b6 07             	movzbl (%edi),%eax
 538:	84 c0                	test   %al,%al
 53a:	75 dc                	jne    518 <printf+0x148>
      state = 0;
 53c:	31 ff                	xor    %edi,%edi
 53e:	e9 d8 fe ff ff       	jmp    41b <printf+0x4b>
 543:	90                   	nop
 544:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  write(fd, &c, 1);
 548:	8d 45 e5             	lea    -0x1b(%ebp),%eax
      state = 0;
 54b:	31 ff                	xor    %edi,%edi
  write(fd, &c, 1);
 54d:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 554:	00 
 555:	89 44 24 04          	mov    %eax,0x4(%esp)
 559:	89 34 24             	mov    %esi,(%esp)
 55c:	c6 45 e5 25          	movb   $0x25,-0x1b(%ebp)
 560:	e8 3d fd ff ff       	call   2a2 <write>
 565:	e9 b1 fe ff ff       	jmp    41b <printf+0x4b>
 56a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        printint(fd, *ap, 10, 1);
 570:	8b 45 d4             	mov    -0x2c(%ebp),%eax
 573:	b9 0a 00 00 00       	mov    $0xa,%ecx
      state = 0;
 578:	66 31 ff             	xor    %di,%di
        printint(fd, *ap, 10, 1);
 57b:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 582:	8b 10                	mov    (%eax),%edx
 584:	89 f0                	mov    %esi,%eax
 586:	e8 a5 fd ff ff       	call   330 <printint>
        ap++;
 58b:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
 58f:	e9 87 fe ff ff       	jmp    41b <printf+0x4b>
        putc(fd, *ap);
 594:	8b 45 d4             	mov    -0x2c(%ebp),%eax
      state = 0;
 597:	31 ff                	xor    %edi,%edi
        putc(fd, *ap);
 599:	8b 00                	mov    (%eax),%eax
  write(fd, &c, 1);
 59b:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 5a2:	00 
 5a3:	89 34 24             	mov    %esi,(%esp)
        putc(fd, *ap);
 5a6:	88 45 e4             	mov    %al,-0x1c(%ebp)
  write(fd, &c, 1);
 5a9:	8d 45 e4             	lea    -0x1c(%ebp),%eax
 5ac:	89 44 24 04          	mov    %eax,0x4(%esp)
 5b0:	e8 ed fc ff ff       	call   2a2 <write>
        ap++;
 5b5:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
 5b9:	e9 5d fe ff ff       	jmp    41b <printf+0x4b>
 5be:	66 90                	xchg   %ax,%ax

000005c0 <free>:
static Header base;
static Header *freep;

void
free(void *ap)
{
 5c0:	55                   	push   %ebp
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 5c1:	a1 cc 09 00 00       	mov    0x9cc,%eax
{
 5c6:	89 e5                	mov    %esp,%ebp
 5c8:	57                   	push   %edi
 5c9:	56                   	push   %esi
 5ca:	53                   	push   %ebx
 5cb:	8b 5d 08             	mov    0x8(%ebp),%ebx
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 5ce:	8b 08                	mov    (%eax),%ecx
  bp = (Header*)ap - 1;
 5d0:	8d 53 f8             	lea    -0x8(%ebx),%edx
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 5d3:	39 d0                	cmp    %edx,%eax
 5d5:	72 11                	jb     5e8 <free+0x28>
 5d7:	90                   	nop
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 5d8:	39 c8                	cmp    %ecx,%eax
 5da:	72 04                	jb     5e0 <free+0x20>
 5dc:	39 ca                	cmp    %ecx,%edx
 5de:	72 10                	jb     5f0 <free+0x30>
 5e0:	89 c8                	mov    %ecx,%eax
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 5e2:	39 d0                	cmp    %edx,%eax
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 5e4:	8b 08                	mov    (%eax),%ecx
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 5e6:	73 f0                	jae    5d8 <free+0x18>
 5e8:	39 ca                	cmp    %ecx,%edx
 5ea:	72 04                	jb     5f0 <free+0x30>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 5ec:	39 c8                	cmp    %ecx,%eax
 5ee:	72 f0                	jb     5e0 <free+0x20>
      break;
  if(bp + bp->s.size == p->s.ptr){
 5f0:	8b 73 fc             	mov    -0x4(%ebx),%esi
 5f3:	8d 3c f2             	lea    (%edx,%esi,8),%edi
 5f6:	39 cf                	cmp    %ecx,%edi
 5f8:	74 1e                	je     618 <free+0x58>
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
 5fa:	89 4b f8             	mov    %ecx,-0x8(%ebx)
  if(p + p->s.size == bp){
 5fd:	8b 48 04             	mov    0x4(%eax),%ecx
 600:	8d 34 c8             	lea    (%eax,%ecx,8),%esi
 603:	39 f2                	cmp    %esi,%edx
 605:	74 28                	je     62f <free+0x6f>
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
 607:	89 10                	mov    %edx,(%eax)
  freep = p;
 609:	a3 cc 09 00 00       	mov    %eax,0x9cc
}
 60e:	5b                   	pop    %ebx
 60f:	5e                   	pop    %esi
 610:	5f                   	pop    %edi
 611:	5d                   	pop    %ebp
 612:	c3                   	ret    
 613:	90                   	nop
 614:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    bp->s.size += p->s.ptr->s.size;
 618:	03 71 04             	add    0x4(%ecx),%esi
 61b:	89 73 fc             	mov    %esi,-0x4(%ebx)
    bp->s.ptr = p->s.ptr->s.ptr;
 61e:	8b 08                	mov    (%eax),%ecx
 620:	8b 09                	mov    (%ecx),%ecx
 622:	89 4b f8             	mov    %ecx,-0x8(%ebx)
  if(p + p->s.size == bp){
 625:	8b 48 04             	mov    0x4(%eax),%ecx
 628:	8d 34 c8             	lea    (%eax,%ecx,8),%esi
 62b:	39 f2                	cmp    %esi,%edx
 62d:	75 d8                	jne    607 <free+0x47>
    p->s.size += bp->s.size;
 62f:	03 4b fc             	add    -0x4(%ebx),%ecx
  freep = p;
 632:	a3 cc 09 00 00       	mov    %eax,0x9cc
    p->s.size += bp->s.size;
 637:	89 48 04             	mov    %ecx,0x4(%eax)
    p->s.ptr = bp->s.ptr;
 63a:	8b 53 f8             	mov    -0x8(%ebx),%edx
 63d:	89 10                	mov    %edx,(%eax)
}
 63f:	5b                   	pop    %ebx
 640:	5e                   	pop    %esi
 641:	5f                   	pop    %edi
 642:	5d                   	pop    %ebp
 643:	c3                   	ret    
 644:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 64a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

00000650 <malloc>:
  return freep;
}

void*
malloc(uint nbytes)
{
 650:	55                   	push   %ebp
 651:	89 e5                	mov    %esp,%ebp
 653:	57                   	push   %edi
 654:	56                   	push   %esi
 655:	53                   	push   %ebx
 656:	83 ec 1c             	sub    $0x1c,%esp
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 659:	8b 45 08             	mov    0x8(%ebp),%eax
  if((prevp = freep) == 0){
 65c:	8b 1d cc 09 00 00    	mov    0x9cc,%ebx
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 662:	8d 48 07             	lea    0x7(%eax),%ecx
 665:	c1 e9 03             	shr    $0x3,%ecx
  if((prevp = freep) == 0){
 668:	85 db                	test   %ebx,%ebx
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 66a:	8d 71 01             	lea    0x1(%ecx),%esi
  if((prevp = freep) == 0){
 66d:	0f 84 9b 00 00 00    	je     70e <malloc+0xbe>
 673:	8b 13                	mov    (%ebx),%edx
 675:	8b 7a 04             	mov    0x4(%edx),%edi
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
 678:	39 fe                	cmp    %edi,%esi
 67a:	76 64                	jbe    6e0 <malloc+0x90>
 67c:	8d 04 f5 00 00 00 00 	lea    0x0(,%esi,8),%eax
  if(nu < 4096)
 683:	bb 00 80 00 00       	mov    $0x8000,%ebx
 688:	89 45 e4             	mov    %eax,-0x1c(%ebp)
 68b:	eb 0e                	jmp    69b <malloc+0x4b>
 68d:	8d 76 00             	lea    0x0(%esi),%esi
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
 690:	8b 02                	mov    (%edx),%eax
    if(p->s.size >= nunits){
 692:	8b 78 04             	mov    0x4(%eax),%edi
 695:	39 fe                	cmp    %edi,%esi
 697:	76 4f                	jbe    6e8 <malloc+0x98>
 699:	89 c2                	mov    %eax,%edx
        p->s.size = nunits;
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
 69b:	3b 15 cc 09 00 00    	cmp    0x9cc,%edx
 6a1:	75 ed                	jne    690 <malloc+0x40>
  if(nu < 4096)
 6a3:	8b 45 e4             	mov    -0x1c(%ebp),%eax
 6a6:	81 fe 00 10 00 00    	cmp    $0x1000,%esi
 6ac:	bf 00 10 00 00       	mov    $0x1000,%edi
 6b1:	0f 43 fe             	cmovae %esi,%edi
 6b4:	0f 42 c3             	cmovb  %ebx,%eax
  p = sbrk(nu * sizeof(Header));
 6b7:	89 04 24             	mov    %eax,(%esp)
 6ba:	e8 4b fc ff ff       	call   30a <sbrk>
  if(p == (char*)-1)
 6bf:	83 f8 ff             	cmp    $0xffffffff,%eax
 6c2:	74 18                	je     6dc <malloc+0x8c>
  hp->s.size = nu;
 6c4:	89 78 04             	mov    %edi,0x4(%eax)
  free((void*)(hp + 1));
 6c7:	83 c0 08             	add    $0x8,%eax
 6ca:	89 04 24             	mov    %eax,(%esp)
 6cd:	e8 ee fe ff ff       	call   5c0 <free>
  return freep;
 6d2:	8b 15 cc 09 00 00    	mov    0x9cc,%edx
      if((p = morecore(nunits)) == 0)
 6d8:	85 d2                	test   %edx,%edx
 6da:	75 b4                	jne    690 <malloc+0x40>
        return 0;
 6dc:	31 c0                	xor    %eax,%eax
 6de:	eb 20                	jmp    700 <malloc+0xb0>
    if(p->s.size >= nunits){
 6e0:	89 d0                	mov    %edx,%eax
 6e2:	89 da                	mov    %ebx,%edx
 6e4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      if(p->s.size == nunits)
 6e8:	39 fe                	cmp    %edi,%esi
 6ea:	74 1c                	je     708 <malloc+0xb8>
        p->s.size -= nunits;
 6ec:	29 f7                	sub    %esi,%edi
 6ee:	89 78 04             	mov    %edi,0x4(%eax)
        p += p->s.size;
 6f1:	8d 04 f8             	lea    (%eax,%edi,8),%eax
        p->s.size = nunits;
 6f4:	89 70 04             	mov    %esi,0x4(%eax)
      freep = prevp;
 6f7:	89 15 cc 09 00 00    	mov    %edx,0x9cc
      return (void*)(p + 1);
 6fd:	83 c0 08             	add    $0x8,%eax
  }
}
 700:	83 c4 1c             	add    $0x1c,%esp
 703:	5b                   	pop    %ebx
 704:	5e                   	pop    %esi
 705:	5f                   	pop    %edi
 706:	5d                   	pop    %ebp
 707:	c3                   	ret    
        prevp->s.ptr = p->s.ptr;
 708:	8b 08                	mov    (%eax),%ecx
 70a:	89 0a                	mov    %ecx,(%edx)
 70c:	eb e9                	jmp    6f7 <malloc+0xa7>
    base.s.ptr = freep = prevp = &base;
 70e:	c7 05 cc 09 00 00 d0 	movl   $0x9d0,0x9cc
 715:	09 00 00 
    base.s.size = 0;
 718:	ba d0 09 00 00       	mov    $0x9d0,%edx
    base.s.ptr = freep = prevp = &base;
 71d:	c7 05 d0 09 00 00 d0 	movl   $0x9d0,0x9d0
 724:	09 00 00 
    base.s.size = 0;
 727:	c7 05 d4 09 00 00 00 	movl   $0x0,0x9d4
 72e:	00 00 00 
 731:	e9 46 ff ff ff       	jmp    67c <malloc+0x2c>
