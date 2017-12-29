
_ln：     檔案格式 elf32-i386


Disassembly of section .text:

00000000 <main>:
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
   0:	8d 4c 24 04          	lea    0x4(%esp),%ecx
   4:	83 e4 f0             	and    $0xfffffff0,%esp
  if(argc != 3){
   7:	83 39 03             	cmpl   $0x3,(%ecx)
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
   a:	ff 71 fc             	pushl  -0x4(%ecx)
   d:	55                   	push   %ebp
   e:	89 e5                	mov    %esp,%ebp
  10:	53                   	push   %ebx
  11:	51                   	push   %ecx
  12:	8b 59 04             	mov    0x4(%ecx),%ebx
  if(argc != 3){
  15:	74 14                	je     2b <main+0x2b>
    printf(2, "Usage: ln old new\n");
  17:	83 ec 08             	sub    $0x8,%esp
  1a:	68 20 07 00 00       	push   $0x720
  1f:	6a 02                	push   $0x2
  21:	e8 da 03 00 00       	call   400 <printf>
    exit();
  26:	e8 77 02 00 00       	call   2a2 <exit>
  }
  if(link(argv[1], argv[2]) < 0)
  2b:	50                   	push   %eax
  2c:	50                   	push   %eax
  2d:	ff 73 08             	pushl  0x8(%ebx)
  30:	ff 73 04             	pushl  0x4(%ebx)
  33:	e8 ca 02 00 00       	call   302 <link>
  38:	83 c4 10             	add    $0x10,%esp
  3b:	85 c0                	test   %eax,%eax
  3d:	78 05                	js     44 <main+0x44>
    printf(2, "link %s %s: failed\n", argv[1], argv[2]);
  exit();
  3f:	e8 5e 02 00 00       	call   2a2 <exit>
  if(argc != 3){
    printf(2, "Usage: ln old new\n");
    exit();
  }
  if(link(argv[1], argv[2]) < 0)
    printf(2, "link %s %s: failed\n", argv[1], argv[2]);
  44:	ff 73 08             	pushl  0x8(%ebx)
  47:	ff 73 04             	pushl  0x4(%ebx)
  4a:	68 33 07 00 00       	push   $0x733
  4f:	6a 02                	push   $0x2
  51:	e8 aa 03 00 00       	call   400 <printf>
  56:	83 c4 10             	add    $0x10,%esp
  59:	eb e4                	jmp    3f <main+0x3f>
  5b:	66 90                	xchg   %ax,%ax
  5d:	66 90                	xchg   %ax,%ax
  5f:	90                   	nop

00000060 <strcpy>:
#include "user.h"
#include "x86.h"

char*
strcpy(char *s, char *t)
{
  60:	55                   	push   %ebp
  61:	89 e5                	mov    %esp,%ebp
  63:	53                   	push   %ebx
  64:	8b 45 08             	mov    0x8(%ebp),%eax
  67:	8b 4d 0c             	mov    0xc(%ebp),%ecx
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
  93:	56                   	push   %esi
  94:	53                   	push   %ebx
  95:	8b 55 08             	mov    0x8(%ebp),%edx
  98:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  while(*p && *p == *q)
  9b:	0f b6 02             	movzbl (%edx),%eax
  9e:	0f b6 19             	movzbl (%ecx),%ebx
  a1:	84 c0                	test   %al,%al
  a3:	75 1e                	jne    c3 <strcmp+0x33>
  a5:	eb 29                	jmp    d0 <strcmp+0x40>
  a7:	89 f6                	mov    %esi,%esi
  a9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
    p++, q++;
  b0:	83 c2 01             	add    $0x1,%edx
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
  b3:	0f b6 02             	movzbl (%edx),%eax
    p++, q++;
  b6:	8d 71 01             	lea    0x1(%ecx),%esi
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
  b9:	0f b6 59 01          	movzbl 0x1(%ecx),%ebx
  bd:	84 c0                	test   %al,%al
  bf:	74 0f                	je     d0 <strcmp+0x40>
  c1:	89 f1                	mov    %esi,%ecx
  c3:	38 d8                	cmp    %bl,%al
  c5:	74 e9                	je     b0 <strcmp+0x20>
    p++, q++;
  return (uchar)*p - (uchar)*q;
  c7:	29 d8                	sub    %ebx,%eax
}
  c9:	5b                   	pop    %ebx
  ca:	5e                   	pop    %esi
  cb:	5d                   	pop    %ebp
  cc:	c3                   	ret    
  cd:	8d 76 00             	lea    0x0(%esi),%esi
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
  d0:	31 c0                	xor    %eax,%eax
    p++, q++;
  return (uchar)*p - (uchar)*q;
  d2:	29 d8                	sub    %ebx,%eax
}
  d4:	5b                   	pop    %ebx
  d5:	5e                   	pop    %esi
  d6:	5d                   	pop    %ebp
  d7:	c3                   	ret    
  d8:	90                   	nop
  d9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

000000e0 <strlen>:

uint
strlen(char *s)
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
uint
strlen(char *s)
{
  int n;

  for(n = 0; s[n]; n++)
  fd:	31 c0                	xor    %eax,%eax
    ;
  return n;
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
 113:	57                   	push   %edi
 114:	8b 55 08             	mov    0x8(%ebp),%edx
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
 133:	53                   	push   %ebx
 134:	8b 45 08             	mov    0x8(%ebp),%eax
 137:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  for(; *s; s++)
 13a:	0f b6 10             	movzbl (%eax),%edx
 13d:	84 d2                	test   %dl,%dl
 13f:	74 1d                	je     15e <strchr+0x2e>
    if(*s == c)
 141:	38 d3                	cmp    %dl,%bl
 143:	89 d9                	mov    %ebx,%ecx
 145:	75 0d                	jne    154 <strchr+0x24>
 147:	eb 17                	jmp    160 <strchr+0x30>
 149:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 150:	38 ca                	cmp    %cl,%dl
 152:	74 0c                	je     160 <strchr+0x30>
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
 154:	83 c0 01             	add    $0x1,%eax
 157:	0f b6 10             	movzbl (%eax),%edx
 15a:	84 d2                	test   %dl,%dl
 15c:	75 f2                	jne    150 <strchr+0x20>
    if(*s == c)
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
 175:	53                   	push   %ebx
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
 176:	31 f6                	xor    %esi,%esi
    cc = read(0, &c, 1);
 178:	8d 7d e7             	lea    -0x19(%ebp),%edi
  return 0;
}

char*
gets(char *buf, int max)
{
 17b:	83 ec 1c             	sub    $0x1c,%esp
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
 17e:	eb 29                	jmp    1a9 <gets+0x39>
    cc = read(0, &c, 1);
 180:	83 ec 04             	sub    $0x4,%esp
 183:	6a 01                	push   $0x1
 185:	57                   	push   %edi
 186:	6a 00                	push   $0x0
 188:	e8 2d 01 00 00       	call   2ba <read>
    if(cc < 1)
 18d:	83 c4 10             	add    $0x10,%esp
 190:	85 c0                	test   %eax,%eax
 192:	7e 1d                	jle    1b1 <gets+0x41>
      break;
    buf[i++] = c;
 194:	0f b6 45 e7          	movzbl -0x19(%ebp),%eax
 198:	8b 55 08             	mov    0x8(%ebp),%edx
 19b:	89 de                	mov    %ebx,%esi
    if(c == '\n' || c == '\r')
 19d:	3c 0a                	cmp    $0xa,%al

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
 19f:	88 44 1a ff          	mov    %al,-0x1(%edx,%ebx,1)
    if(c == '\n' || c == '\r')
 1a3:	74 1b                	je     1c0 <gets+0x50>
 1a5:	3c 0d                	cmp    $0xd,%al
 1a7:	74 17                	je     1c0 <gets+0x50>
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
 1a9:	8d 5e 01             	lea    0x1(%esi),%ebx
 1ac:	3b 5d 0c             	cmp    0xc(%ebp),%ebx
 1af:	7c cf                	jl     180 <gets+0x10>
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
 1b1:	8b 45 08             	mov    0x8(%ebp),%eax
 1b4:	c6 04 30 00          	movb   $0x0,(%eax,%esi,1)
  return buf;
}
 1b8:	8d 65 f4             	lea    -0xc(%ebp),%esp
 1bb:	5b                   	pop    %ebx
 1bc:	5e                   	pop    %esi
 1bd:	5f                   	pop    %edi
 1be:	5d                   	pop    %ebp
 1bf:	c3                   	ret    
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
 1c0:	8b 45 08             	mov    0x8(%ebp),%eax
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
 1c3:	89 de                	mov    %ebx,%esi
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
 1c5:	c6 04 30 00          	movb   $0x0,(%eax,%esi,1)
  return buf;
}
 1c9:	8d 65 f4             	lea    -0xc(%ebp),%esp
 1cc:	5b                   	pop    %ebx
 1cd:	5e                   	pop    %esi
 1ce:	5f                   	pop    %edi
 1cf:	5d                   	pop    %ebp
 1d0:	c3                   	ret    
 1d1:	eb 0d                	jmp    1e0 <stat>
 1d3:	90                   	nop
 1d4:	90                   	nop
 1d5:	90                   	nop
 1d6:	90                   	nop
 1d7:	90                   	nop
 1d8:	90                   	nop
 1d9:	90                   	nop
 1da:	90                   	nop
 1db:	90                   	nop
 1dc:	90                   	nop
 1dd:	90                   	nop
 1de:	90                   	nop
 1df:	90                   	nop

000001e0 <stat>:

int
stat(char *n, struct stat *st)
{
 1e0:	55                   	push   %ebp
 1e1:	89 e5                	mov    %esp,%ebp
 1e3:	56                   	push   %esi
 1e4:	53                   	push   %ebx
  int fd;
  int r;

  fd = open(n, O_RDONLY);
 1e5:	83 ec 08             	sub    $0x8,%esp
 1e8:	6a 00                	push   $0x0
 1ea:	ff 75 08             	pushl  0x8(%ebp)
 1ed:	e8 f0 00 00 00       	call   2e2 <open>
  if(fd < 0)
 1f2:	83 c4 10             	add    $0x10,%esp
 1f5:	85 c0                	test   %eax,%eax
 1f7:	78 27                	js     220 <stat+0x40>
    return -1;
  r = fstat(fd, st);
 1f9:	83 ec 08             	sub    $0x8,%esp
 1fc:	ff 75 0c             	pushl  0xc(%ebp)
 1ff:	89 c3                	mov    %eax,%ebx
 201:	50                   	push   %eax
 202:	e8 f3 00 00 00       	call   2fa <fstat>
 207:	89 c6                	mov    %eax,%esi
  close(fd);
 209:	89 1c 24             	mov    %ebx,(%esp)
 20c:	e8 b9 00 00 00       	call   2ca <close>
  return r;
 211:	83 c4 10             	add    $0x10,%esp
 214:	89 f0                	mov    %esi,%eax
}
 216:	8d 65 f8             	lea    -0x8(%ebp),%esp
 219:	5b                   	pop    %ebx
 21a:	5e                   	pop    %esi
 21b:	5d                   	pop    %ebp
 21c:	c3                   	ret    
 21d:	8d 76 00             	lea    0x0(%esi),%esi
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
 220:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
 225:	eb ef                	jmp    216 <stat+0x36>
 227:	89 f6                	mov    %esi,%esi
 229:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000230 <atoi>:
  return r;
}

int
atoi(const char *s)
{
 230:	55                   	push   %ebp
 231:	89 e5                	mov    %esp,%ebp
 233:	53                   	push   %ebx
 234:	8b 4d 08             	mov    0x8(%ebp),%ecx
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
 237:	0f be 11             	movsbl (%ecx),%edx
 23a:	8d 42 d0             	lea    -0x30(%edx),%eax
 23d:	3c 09                	cmp    $0x9,%al
 23f:	b8 00 00 00 00       	mov    $0x0,%eax
 244:	77 1f                	ja     265 <atoi+0x35>
 246:	8d 76 00             	lea    0x0(%esi),%esi
 249:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
    n = n*10 + *s++ - '0';
 250:	8d 04 80             	lea    (%eax,%eax,4),%eax
 253:	83 c1 01             	add    $0x1,%ecx
 256:	8d 44 42 d0          	lea    -0x30(%edx,%eax,2),%eax
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
 25a:	0f be 11             	movsbl (%ecx),%edx
 25d:	8d 5a d0             	lea    -0x30(%edx),%ebx
 260:	80 fb 09             	cmp    $0x9,%bl
 263:	76 eb                	jbe    250 <atoi+0x20>
    n = n*10 + *s++ - '0';
  return n;
}
 265:	5b                   	pop    %ebx
 266:	5d                   	pop    %ebp
 267:	c3                   	ret    
 268:	90                   	nop
 269:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00000270 <memmove>:

void*
memmove(void *vdst, void *vsrc, int n)
{
 270:	55                   	push   %ebp
 271:	89 e5                	mov    %esp,%ebp
 273:	56                   	push   %esi
 274:	53                   	push   %ebx
 275:	8b 5d 10             	mov    0x10(%ebp),%ebx
 278:	8b 45 08             	mov    0x8(%ebp),%eax
 27b:	8b 75 0c             	mov    0xc(%ebp),%esi
  char *dst, *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
 27e:	85 db                	test   %ebx,%ebx
 280:	7e 14                	jle    296 <memmove+0x26>
 282:	31 d2                	xor    %edx,%edx
 284:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    *dst++ = *src++;
 288:	0f b6 0c 16          	movzbl (%esi,%edx,1),%ecx
 28c:	88 0c 10             	mov    %cl,(%eax,%edx,1)
 28f:	83 c2 01             	add    $0x1,%edx
{
  char *dst, *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
 292:	39 da                	cmp    %ebx,%edx
 294:	75 f2                	jne    288 <memmove+0x18>
    *dst++ = *src++;
  return vdst;
}
 296:	5b                   	pop    %ebx
 297:	5e                   	pop    %esi
 298:	5d                   	pop    %ebp
 299:	c3                   	ret    

0000029a <fork>:
  name: \
    movl $SYS_ ## name, %eax; \
    int $T_SYSCALL; \
    ret

SYSCALL(fork)
 29a:	b8 01 00 00 00       	mov    $0x1,%eax
 29f:	cd 40                	int    $0x40
 2a1:	c3                   	ret    

000002a2 <exit>:
SYSCALL(exit)
 2a2:	b8 02 00 00 00       	mov    $0x2,%eax
 2a7:	cd 40                	int    $0x40
 2a9:	c3                   	ret    

000002aa <wait>:
SYSCALL(wait)
 2aa:	b8 03 00 00 00       	mov    $0x3,%eax
 2af:	cd 40                	int    $0x40
 2b1:	c3                   	ret    

000002b2 <pipe>:
SYSCALL(pipe)
 2b2:	b8 04 00 00 00       	mov    $0x4,%eax
 2b7:	cd 40                	int    $0x40
 2b9:	c3                   	ret    

000002ba <read>:
SYSCALL(read)
 2ba:	b8 05 00 00 00       	mov    $0x5,%eax
 2bf:	cd 40                	int    $0x40
 2c1:	c3                   	ret    

000002c2 <write>:
SYSCALL(write)
 2c2:	b8 10 00 00 00       	mov    $0x10,%eax
 2c7:	cd 40                	int    $0x40
 2c9:	c3                   	ret    

000002ca <close>:
SYSCALL(close)
 2ca:	b8 15 00 00 00       	mov    $0x15,%eax
 2cf:	cd 40                	int    $0x40
 2d1:	c3                   	ret    

000002d2 <kill>:
SYSCALL(kill)
 2d2:	b8 06 00 00 00       	mov    $0x6,%eax
 2d7:	cd 40                	int    $0x40
 2d9:	c3                   	ret    

000002da <exec>:
SYSCALL(exec)
 2da:	b8 07 00 00 00       	mov    $0x7,%eax
 2df:	cd 40                	int    $0x40
 2e1:	c3                   	ret    

000002e2 <open>:
SYSCALL(open)
 2e2:	b8 0f 00 00 00       	mov    $0xf,%eax
 2e7:	cd 40                	int    $0x40
 2e9:	c3                   	ret    

000002ea <mknod>:
SYSCALL(mknod)
 2ea:	b8 11 00 00 00       	mov    $0x11,%eax
 2ef:	cd 40                	int    $0x40
 2f1:	c3                   	ret    

000002f2 <unlink>:
SYSCALL(unlink)
 2f2:	b8 12 00 00 00       	mov    $0x12,%eax
 2f7:	cd 40                	int    $0x40
 2f9:	c3                   	ret    

000002fa <fstat>:
SYSCALL(fstat)
 2fa:	b8 08 00 00 00       	mov    $0x8,%eax
 2ff:	cd 40                	int    $0x40
 301:	c3                   	ret    

00000302 <link>:
SYSCALL(link)
 302:	b8 13 00 00 00       	mov    $0x13,%eax
 307:	cd 40                	int    $0x40
 309:	c3                   	ret    

0000030a <mkdir>:
SYSCALL(mkdir)
 30a:	b8 14 00 00 00       	mov    $0x14,%eax
 30f:	cd 40                	int    $0x40
 311:	c3                   	ret    

00000312 <chdir>:
SYSCALL(chdir)
 312:	b8 09 00 00 00       	mov    $0x9,%eax
 317:	cd 40                	int    $0x40
 319:	c3                   	ret    

0000031a <dup>:
SYSCALL(dup)
 31a:	b8 0a 00 00 00       	mov    $0xa,%eax
 31f:	cd 40                	int    $0x40
 321:	c3                   	ret    

00000322 <getpid>:
SYSCALL(getpid)
 322:	b8 0b 00 00 00       	mov    $0xb,%eax
 327:	cd 40                	int    $0x40
 329:	c3                   	ret    

0000032a <sbrk>:
SYSCALL(sbrk)
 32a:	b8 0c 00 00 00       	mov    $0xc,%eax
 32f:	cd 40                	int    $0x40
 331:	c3                   	ret    

00000332 <sleep>:
SYSCALL(sleep)
 332:	b8 0d 00 00 00       	mov    $0xd,%eax
 337:	cd 40                	int    $0x40
 339:	c3                   	ret    

0000033a <uptime>:
SYSCALL(uptime)
 33a:	b8 0e 00 00 00       	mov    $0xe,%eax
 33f:	cd 40                	int    $0x40
 341:	c3                   	ret    

00000342 <cps>:
SYSCALL(cps)
 342:	b8 16 00 00 00       	mov    $0x16,%eax
 347:	cd 40                	int    $0x40
 349:	c3                   	ret    

0000034a <chpr>:
SYSCALL(chpr)
 34a:	b8 17 00 00 00       	mov    $0x17,%eax
 34f:	cd 40                	int    $0x40
 351:	c3                   	ret    

00000352 <chdl>:
SYSCALL(chdl)
 352:	b8 18 00 00 00       	mov    $0x18,%eax
 357:	cd 40                	int    $0x40
 359:	c3                   	ret    
 35a:	66 90                	xchg   %ax,%ax
 35c:	66 90                	xchg   %ax,%ax
 35e:	66 90                	xchg   %ax,%ax

00000360 <printint>:
  write(fd, &c, 1);
}

static void
printint(int fd, int xx, int base, int sgn)
{
 360:	55                   	push   %ebp
 361:	89 e5                	mov    %esp,%ebp
 363:	57                   	push   %edi
 364:	56                   	push   %esi
 365:	53                   	push   %ebx
 366:	89 c6                	mov    %eax,%esi
 368:	83 ec 3c             	sub    $0x3c,%esp
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
 36b:	8b 5d 08             	mov    0x8(%ebp),%ebx
 36e:	85 db                	test   %ebx,%ebx
 370:	74 7e                	je     3f0 <printint+0x90>
 372:	89 d0                	mov    %edx,%eax
 374:	c1 e8 1f             	shr    $0x1f,%eax
 377:	84 c0                	test   %al,%al
 379:	74 75                	je     3f0 <printint+0x90>
    neg = 1;
    x = -xx;
 37b:	89 d0                	mov    %edx,%eax
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
 37d:	c7 45 c4 01 00 00 00 	movl   $0x1,-0x3c(%ebp)
    x = -xx;
 384:	f7 d8                	neg    %eax
 386:	89 75 c0             	mov    %esi,-0x40(%ebp)
  } else {
    x = xx;
  }

  i = 0;
 389:	31 ff                	xor    %edi,%edi
 38b:	8d 5d d7             	lea    -0x29(%ebp),%ebx
 38e:	89 ce                	mov    %ecx,%esi
 390:	eb 08                	jmp    39a <printint+0x3a>
 392:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  do{
    buf[i++] = digits[x % base];
 398:	89 cf                	mov    %ecx,%edi
 39a:	31 d2                	xor    %edx,%edx
 39c:	8d 4f 01             	lea    0x1(%edi),%ecx
 39f:	f7 f6                	div    %esi
 3a1:	0f b6 92 50 07 00 00 	movzbl 0x750(%edx),%edx
  }while((x /= base) != 0);
 3a8:	85 c0                	test   %eax,%eax
    x = xx;
  }

  i = 0;
  do{
    buf[i++] = digits[x % base];
 3aa:	88 14 0b             	mov    %dl,(%ebx,%ecx,1)
  }while((x /= base) != 0);
 3ad:	75 e9                	jne    398 <printint+0x38>
  if(neg)
 3af:	8b 45 c4             	mov    -0x3c(%ebp),%eax
 3b2:	8b 75 c0             	mov    -0x40(%ebp),%esi
 3b5:	85 c0                	test   %eax,%eax
 3b7:	74 08                	je     3c1 <printint+0x61>
    buf[i++] = '-';
 3b9:	c6 44 0d d8 2d       	movb   $0x2d,-0x28(%ebp,%ecx,1)
 3be:	8d 4f 02             	lea    0x2(%edi),%ecx
 3c1:	8d 7c 0d d7          	lea    -0x29(%ebp,%ecx,1),%edi
 3c5:	8d 76 00             	lea    0x0(%esi),%esi
 3c8:	0f b6 07             	movzbl (%edi),%eax
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
 3cb:	83 ec 04             	sub    $0x4,%esp
 3ce:	83 ef 01             	sub    $0x1,%edi
 3d1:	6a 01                	push   $0x1
 3d3:	53                   	push   %ebx
 3d4:	56                   	push   %esi
 3d5:	88 45 d7             	mov    %al,-0x29(%ebp)
 3d8:	e8 e5 fe ff ff       	call   2c2 <write>
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);
  if(neg)
    buf[i++] = '-';

  while(--i >= 0)
 3dd:	83 c4 10             	add    $0x10,%esp
 3e0:	39 df                	cmp    %ebx,%edi
 3e2:	75 e4                	jne    3c8 <printint+0x68>
    putc(fd, buf[i]);
}
 3e4:	8d 65 f4             	lea    -0xc(%ebp),%esp
 3e7:	5b                   	pop    %ebx
 3e8:	5e                   	pop    %esi
 3e9:	5f                   	pop    %edi
 3ea:	5d                   	pop    %ebp
 3eb:	c3                   	ret    
 3ec:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
  } else {
    x = xx;
 3f0:	89 d0                	mov    %edx,%eax
  static char digits[] = "0123456789ABCDEF";
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
 3f2:	c7 45 c4 00 00 00 00 	movl   $0x0,-0x3c(%ebp)
 3f9:	eb 8b                	jmp    386 <printint+0x26>
 3fb:	90                   	nop
 3fc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00000400 <printf>:
}

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, char *fmt, ...)
{
 400:	55                   	push   %ebp
 401:	89 e5                	mov    %esp,%ebp
 403:	57                   	push   %edi
 404:	56                   	push   %esi
 405:	53                   	push   %ebx
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
 406:	8d 45 10             	lea    0x10(%ebp),%eax
}

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, char *fmt, ...)
{
 409:	83 ec 2c             	sub    $0x2c,%esp
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
 40c:	8b 75 0c             	mov    0xc(%ebp),%esi
}

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, char *fmt, ...)
{
 40f:	8b 7d 08             	mov    0x8(%ebp),%edi
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
 412:	89 45 d0             	mov    %eax,-0x30(%ebp)
 415:	0f b6 1e             	movzbl (%esi),%ebx
 418:	83 c6 01             	add    $0x1,%esi
 41b:	84 db                	test   %bl,%bl
 41d:	0f 84 b0 00 00 00    	je     4d3 <printf+0xd3>
 423:	31 d2                	xor    %edx,%edx
 425:	eb 39                	jmp    460 <printf+0x60>
 427:	89 f6                	mov    %esi,%esi
 429:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
 430:	83 f8 25             	cmp    $0x25,%eax
 433:	89 55 d4             	mov    %edx,-0x2c(%ebp)
        state = '%';
 436:	ba 25 00 00 00       	mov    $0x25,%edx
  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
 43b:	74 18                	je     455 <printf+0x55>
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
 43d:	8d 45 e2             	lea    -0x1e(%ebp),%eax
 440:	83 ec 04             	sub    $0x4,%esp
 443:	88 5d e2             	mov    %bl,-0x1e(%ebp)
 446:	6a 01                	push   $0x1
 448:	50                   	push   %eax
 449:	57                   	push   %edi
 44a:	e8 73 fe ff ff       	call   2c2 <write>
 44f:	8b 55 d4             	mov    -0x2c(%ebp),%edx
 452:	83 c4 10             	add    $0x10,%esp
 455:	83 c6 01             	add    $0x1,%esi
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
 458:	0f b6 5e ff          	movzbl -0x1(%esi),%ebx
 45c:	84 db                	test   %bl,%bl
 45e:	74 73                	je     4d3 <printf+0xd3>
    c = fmt[i] & 0xff;
    if(state == 0){
 460:	85 d2                	test   %edx,%edx
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;
 462:	0f be cb             	movsbl %bl,%ecx
 465:	0f b6 c3             	movzbl %bl,%eax
    if(state == 0){
 468:	74 c6                	je     430 <printf+0x30>
      if(c == '%'){
        state = '%';
      } else {
        putc(fd, c);
      }
    } else if(state == '%'){
 46a:	83 fa 25             	cmp    $0x25,%edx
 46d:	75 e6                	jne    455 <printf+0x55>
      if(c == 'd'){
 46f:	83 f8 64             	cmp    $0x64,%eax
 472:	0f 84 f8 00 00 00    	je     570 <printf+0x170>
        printint(fd, *ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
 478:	81 e1 f7 00 00 00    	and    $0xf7,%ecx
 47e:	83 f9 70             	cmp    $0x70,%ecx
 481:	74 5d                	je     4e0 <printf+0xe0>
        printint(fd, *ap, 16, 0);
        ap++;
      } else if(c == 's'){
 483:	83 f8 73             	cmp    $0x73,%eax
 486:	0f 84 84 00 00 00    	je     510 <printf+0x110>
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
        }
      } else if(c == 'c'){
 48c:	83 f8 63             	cmp    $0x63,%eax
 48f:	0f 84 ea 00 00 00    	je     57f <printf+0x17f>
        putc(fd, *ap);
        ap++;
      } else if(c == '%'){
 495:	83 f8 25             	cmp    $0x25,%eax
 498:	0f 84 c2 00 00 00    	je     560 <printf+0x160>
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
 49e:	8d 45 e7             	lea    -0x19(%ebp),%eax
 4a1:	83 ec 04             	sub    $0x4,%esp
 4a4:	c6 45 e7 25          	movb   $0x25,-0x19(%ebp)
 4a8:	6a 01                	push   $0x1
 4aa:	50                   	push   %eax
 4ab:	57                   	push   %edi
 4ac:	e8 11 fe ff ff       	call   2c2 <write>
 4b1:	83 c4 0c             	add    $0xc,%esp
 4b4:	8d 45 e6             	lea    -0x1a(%ebp),%eax
 4b7:	88 5d e6             	mov    %bl,-0x1a(%ebp)
 4ba:	6a 01                	push   $0x1
 4bc:	50                   	push   %eax
 4bd:	57                   	push   %edi
 4be:	83 c6 01             	add    $0x1,%esi
 4c1:	e8 fc fd ff ff       	call   2c2 <write>
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
 4c6:	0f b6 5e ff          	movzbl -0x1(%esi),%ebx
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
 4ca:	83 c4 10             	add    $0x10,%esp
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
 4cd:	31 d2                	xor    %edx,%edx
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
 4cf:	84 db                	test   %bl,%bl
 4d1:	75 8d                	jne    460 <printf+0x60>
        putc(fd, c);
      }
      state = 0;
    }
  }
}
 4d3:	8d 65 f4             	lea    -0xc(%ebp),%esp
 4d6:	5b                   	pop    %ebx
 4d7:	5e                   	pop    %esi
 4d8:	5f                   	pop    %edi
 4d9:	5d                   	pop    %ebp
 4da:	c3                   	ret    
 4db:	90                   	nop
 4dc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    } else if(state == '%'){
      if(c == 'd'){
        printint(fd, *ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
        printint(fd, *ap, 16, 0);
 4e0:	83 ec 0c             	sub    $0xc,%esp
 4e3:	b9 10 00 00 00       	mov    $0x10,%ecx
 4e8:	6a 00                	push   $0x0
 4ea:	8b 5d d0             	mov    -0x30(%ebp),%ebx
 4ed:	89 f8                	mov    %edi,%eax
 4ef:	8b 13                	mov    (%ebx),%edx
 4f1:	e8 6a fe ff ff       	call   360 <printint>
        ap++;
 4f6:	89 d8                	mov    %ebx,%eax
 4f8:	83 c4 10             	add    $0x10,%esp
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
 4fb:	31 d2                	xor    %edx,%edx
      if(c == 'd'){
        printint(fd, *ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
        printint(fd, *ap, 16, 0);
        ap++;
 4fd:	83 c0 04             	add    $0x4,%eax
 500:	89 45 d0             	mov    %eax,-0x30(%ebp)
 503:	e9 4d ff ff ff       	jmp    455 <printf+0x55>
 508:	90                   	nop
 509:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
      } else if(c == 's'){
        s = (char*)*ap;
 510:	8b 45 d0             	mov    -0x30(%ebp),%eax
 513:	8b 18                	mov    (%eax),%ebx
        ap++;
 515:	83 c0 04             	add    $0x4,%eax
 518:	89 45 d0             	mov    %eax,-0x30(%ebp)
        if(s == 0)
          s = "(null)";
 51b:	b8 47 07 00 00       	mov    $0x747,%eax
 520:	85 db                	test   %ebx,%ebx
 522:	0f 44 d8             	cmove  %eax,%ebx
        while(*s != 0){
 525:	0f b6 03             	movzbl (%ebx),%eax
 528:	84 c0                	test   %al,%al
 52a:	74 23                	je     54f <printf+0x14f>
 52c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 530:	88 45 e3             	mov    %al,-0x1d(%ebp)
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
 533:	8d 45 e3             	lea    -0x1d(%ebp),%eax
 536:	83 ec 04             	sub    $0x4,%esp
 539:	6a 01                	push   $0x1
        ap++;
        if(s == 0)
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
 53b:	83 c3 01             	add    $0x1,%ebx
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
 53e:	50                   	push   %eax
 53f:	57                   	push   %edi
 540:	e8 7d fd ff ff       	call   2c2 <write>
      } else if(c == 's'){
        s = (char*)*ap;
        ap++;
        if(s == 0)
          s = "(null)";
        while(*s != 0){
 545:	0f b6 03             	movzbl (%ebx),%eax
 548:	83 c4 10             	add    $0x10,%esp
 54b:	84 c0                	test   %al,%al
 54d:	75 e1                	jne    530 <printf+0x130>
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
 54f:	31 d2                	xor    %edx,%edx
 551:	e9 ff fe ff ff       	jmp    455 <printf+0x55>
 556:	8d 76 00             	lea    0x0(%esi),%esi
 559:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
 560:	83 ec 04             	sub    $0x4,%esp
 563:	88 5d e5             	mov    %bl,-0x1b(%ebp)
 566:	8d 45 e5             	lea    -0x1b(%ebp),%eax
 569:	6a 01                	push   $0x1
 56b:	e9 4c ff ff ff       	jmp    4bc <printf+0xbc>
      } else {
        putc(fd, c);
      }
    } else if(state == '%'){
      if(c == 'd'){
        printint(fd, *ap, 10, 1);
 570:	83 ec 0c             	sub    $0xc,%esp
 573:	b9 0a 00 00 00       	mov    $0xa,%ecx
 578:	6a 01                	push   $0x1
 57a:	e9 6b ff ff ff       	jmp    4ea <printf+0xea>
 57f:	8b 5d d0             	mov    -0x30(%ebp),%ebx
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
 582:	83 ec 04             	sub    $0x4,%esp
 585:	8b 03                	mov    (%ebx),%eax
 587:	6a 01                	push   $0x1
 589:	88 45 e4             	mov    %al,-0x1c(%ebp)
 58c:	8d 45 e4             	lea    -0x1c(%ebp),%eax
 58f:	50                   	push   %eax
 590:	57                   	push   %edi
 591:	e8 2c fd ff ff       	call   2c2 <write>
 596:	e9 5b ff ff ff       	jmp    4f6 <printf+0xf6>
 59b:	66 90                	xchg   %ax,%ax
 59d:	66 90                	xchg   %ax,%ax
 59f:	90                   	nop

000005a0 <free>:
static Header base;
static Header *freep;

void
free(void *ap)
{
 5a0:	55                   	push   %ebp
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 5a1:	a1 ec 09 00 00       	mov    0x9ec,%eax
static Header base;
static Header *freep;

void
free(void *ap)
{
 5a6:	89 e5                	mov    %esp,%ebp
 5a8:	57                   	push   %edi
 5a9:	56                   	push   %esi
 5aa:	53                   	push   %ebx
 5ab:	8b 5d 08             	mov    0x8(%ebp),%ebx
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 5ae:	8b 10                	mov    (%eax),%edx
void
free(void *ap)
{
  Header *bp, *p;

  bp = (Header*)ap - 1;
 5b0:	8d 4b f8             	lea    -0x8(%ebx),%ecx
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 5b3:	39 c8                	cmp    %ecx,%eax
 5b5:	73 19                	jae    5d0 <free+0x30>
 5b7:	89 f6                	mov    %esi,%esi
 5b9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
 5c0:	39 d1                	cmp    %edx,%ecx
 5c2:	72 1c                	jb     5e0 <free+0x40>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 5c4:	39 d0                	cmp    %edx,%eax
 5c6:	73 18                	jae    5e0 <free+0x40>
static Header base;
static Header *freep;

void
free(void *ap)
{
 5c8:	89 d0                	mov    %edx,%eax
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 5ca:	39 c8                	cmp    %ecx,%eax
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 5cc:	8b 10                	mov    (%eax),%edx
free(void *ap)
{
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 5ce:	72 f0                	jb     5c0 <free+0x20>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 5d0:	39 d0                	cmp    %edx,%eax
 5d2:	72 f4                	jb     5c8 <free+0x28>
 5d4:	39 d1                	cmp    %edx,%ecx
 5d6:	73 f0                	jae    5c8 <free+0x28>
 5d8:	90                   	nop
 5d9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
      break;
  if(bp + bp->s.size == p->s.ptr){
 5e0:	8b 73 fc             	mov    -0x4(%ebx),%esi
 5e3:	8d 3c f1             	lea    (%ecx,%esi,8),%edi
 5e6:	39 d7                	cmp    %edx,%edi
 5e8:	74 19                	je     603 <free+0x63>
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
 5ea:	89 53 f8             	mov    %edx,-0x8(%ebx)
  if(p + p->s.size == bp){
 5ed:	8b 50 04             	mov    0x4(%eax),%edx
 5f0:	8d 34 d0             	lea    (%eax,%edx,8),%esi
 5f3:	39 f1                	cmp    %esi,%ecx
 5f5:	74 23                	je     61a <free+0x7a>
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
 5f7:	89 08                	mov    %ecx,(%eax)
  freep = p;
 5f9:	a3 ec 09 00 00       	mov    %eax,0x9ec
}
 5fe:	5b                   	pop    %ebx
 5ff:	5e                   	pop    %esi
 600:	5f                   	pop    %edi
 601:	5d                   	pop    %ebp
 602:	c3                   	ret    
  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  if(bp + bp->s.size == p->s.ptr){
    bp->s.size += p->s.ptr->s.size;
 603:	03 72 04             	add    0x4(%edx),%esi
 606:	89 73 fc             	mov    %esi,-0x4(%ebx)
    bp->s.ptr = p->s.ptr->s.ptr;
 609:	8b 10                	mov    (%eax),%edx
 60b:	8b 12                	mov    (%edx),%edx
 60d:	89 53 f8             	mov    %edx,-0x8(%ebx)
  } else
    bp->s.ptr = p->s.ptr;
  if(p + p->s.size == bp){
 610:	8b 50 04             	mov    0x4(%eax),%edx
 613:	8d 34 d0             	lea    (%eax,%edx,8),%esi
 616:	39 f1                	cmp    %esi,%ecx
 618:	75 dd                	jne    5f7 <free+0x57>
    p->s.size += bp->s.size;
 61a:	03 53 fc             	add    -0x4(%ebx),%edx
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
 61d:	a3 ec 09 00 00       	mov    %eax,0x9ec
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if(p + p->s.size == bp){
    p->s.size += bp->s.size;
 622:	89 50 04             	mov    %edx,0x4(%eax)
    p->s.ptr = bp->s.ptr;
 625:	8b 53 f8             	mov    -0x8(%ebx),%edx
 628:	89 10                	mov    %edx,(%eax)
  } else
    p->s.ptr = bp;
  freep = p;
}
 62a:	5b                   	pop    %ebx
 62b:	5e                   	pop    %esi
 62c:	5f                   	pop    %edi
 62d:	5d                   	pop    %ebp
 62e:	c3                   	ret    
 62f:	90                   	nop

00000630 <malloc>:
  return freep;
}

void*
malloc(uint nbytes)
{
 630:	55                   	push   %ebp
 631:	89 e5                	mov    %esp,%ebp
 633:	57                   	push   %edi
 634:	56                   	push   %esi
 635:	53                   	push   %ebx
 636:	83 ec 0c             	sub    $0xc,%esp
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 639:	8b 45 08             	mov    0x8(%ebp),%eax
  if((prevp = freep) == 0){
 63c:	8b 15 ec 09 00 00    	mov    0x9ec,%edx
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 642:	8d 78 07             	lea    0x7(%eax),%edi
 645:	c1 ef 03             	shr    $0x3,%edi
 648:	83 c7 01             	add    $0x1,%edi
  if((prevp = freep) == 0){
 64b:	85 d2                	test   %edx,%edx
 64d:	0f 84 a3 00 00 00    	je     6f6 <malloc+0xc6>
 653:	8b 02                	mov    (%edx),%eax
 655:	8b 48 04             	mov    0x4(%eax),%ecx
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
 658:	39 cf                	cmp    %ecx,%edi
 65a:	76 74                	jbe    6d0 <malloc+0xa0>
 65c:	81 ff 00 10 00 00    	cmp    $0x1000,%edi
 662:	be 00 10 00 00       	mov    $0x1000,%esi
 667:	8d 1c fd 00 00 00 00 	lea    0x0(,%edi,8),%ebx
 66e:	0f 43 f7             	cmovae %edi,%esi
 671:	ba 00 80 00 00       	mov    $0x8000,%edx
 676:	81 ff ff 0f 00 00    	cmp    $0xfff,%edi
 67c:	0f 46 da             	cmovbe %edx,%ebx
 67f:	eb 10                	jmp    691 <malloc+0x61>
 681:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
  if((prevp = freep) == 0){
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
 688:	8b 02                	mov    (%edx),%eax
    if(p->s.size >= nunits){
 68a:	8b 48 04             	mov    0x4(%eax),%ecx
 68d:	39 cf                	cmp    %ecx,%edi
 68f:	76 3f                	jbe    6d0 <malloc+0xa0>
        p->s.size = nunits;
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
 691:	39 05 ec 09 00 00    	cmp    %eax,0x9ec
 697:	89 c2                	mov    %eax,%edx
 699:	75 ed                	jne    688 <malloc+0x58>
  char *p;
  Header *hp;

  if(nu < 4096)
    nu = 4096;
  p = sbrk(nu * sizeof(Header));
 69b:	83 ec 0c             	sub    $0xc,%esp
 69e:	53                   	push   %ebx
 69f:	e8 86 fc ff ff       	call   32a <sbrk>
  if(p == (char*)-1)
 6a4:	83 c4 10             	add    $0x10,%esp
 6a7:	83 f8 ff             	cmp    $0xffffffff,%eax
 6aa:	74 1c                	je     6c8 <malloc+0x98>
    return 0;
  hp = (Header*)p;
  hp->s.size = nu;
 6ac:	89 70 04             	mov    %esi,0x4(%eax)
  free((void*)(hp + 1));
 6af:	83 ec 0c             	sub    $0xc,%esp
 6b2:	83 c0 08             	add    $0x8,%eax
 6b5:	50                   	push   %eax
 6b6:	e8 e5 fe ff ff       	call   5a0 <free>
  return freep;
 6bb:	8b 15 ec 09 00 00    	mov    0x9ec,%edx
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
      if((p = morecore(nunits)) == 0)
 6c1:	83 c4 10             	add    $0x10,%esp
 6c4:	85 d2                	test   %edx,%edx
 6c6:	75 c0                	jne    688 <malloc+0x58>
        return 0;
 6c8:	31 c0                	xor    %eax,%eax
 6ca:	eb 1c                	jmp    6e8 <malloc+0xb8>
 6cc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
      if(p->s.size == nunits)
 6d0:	39 cf                	cmp    %ecx,%edi
 6d2:	74 1c                	je     6f0 <malloc+0xc0>
        prevp->s.ptr = p->s.ptr;
      else {
        p->s.size -= nunits;
 6d4:	29 f9                	sub    %edi,%ecx
 6d6:	89 48 04             	mov    %ecx,0x4(%eax)
        p += p->s.size;
 6d9:	8d 04 c8             	lea    (%eax,%ecx,8),%eax
        p->s.size = nunits;
 6dc:	89 78 04             	mov    %edi,0x4(%eax)
      }
      freep = prevp;
 6df:	89 15 ec 09 00 00    	mov    %edx,0x9ec
      return (void*)(p + 1);
 6e5:	83 c0 08             	add    $0x8,%eax
    }
    if(p == freep)
      if((p = morecore(nunits)) == 0)
        return 0;
  }
}
 6e8:	8d 65 f4             	lea    -0xc(%ebp),%esp
 6eb:	5b                   	pop    %ebx
 6ec:	5e                   	pop    %esi
 6ed:	5f                   	pop    %edi
 6ee:	5d                   	pop    %ebp
 6ef:	c3                   	ret    
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
      if(p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
 6f0:	8b 08                	mov    (%eax),%ecx
 6f2:	89 0a                	mov    %ecx,(%edx)
 6f4:	eb e9                	jmp    6df <malloc+0xaf>
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
  if((prevp = freep) == 0){
    base.s.ptr = freep = prevp = &base;
 6f6:	c7 05 ec 09 00 00 f0 	movl   $0x9f0,0x9ec
 6fd:	09 00 00 
 700:	c7 05 f0 09 00 00 f0 	movl   $0x9f0,0x9f0
 707:	09 00 00 
    base.s.size = 0;
 70a:	b8 f0 09 00 00       	mov    $0x9f0,%eax
 70f:	c7 05 f4 09 00 00 00 	movl   $0x0,0x9f4
 716:	00 00 00 
 719:	e9 3e ff ff ff       	jmp    65c <malloc+0x2c>
