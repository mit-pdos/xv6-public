
_zombie:     file format elf32-i386


Disassembly of section .text:

00000000 <main>:
#include "stat.h"
#include "user.h"

int
main(void)
{
   0:	55                   	push   %ebp
   1:	89 e5                	mov    %esp,%ebp
   3:	83 e4 f0             	and    $0xfffffff0,%esp
   6:	83 ec 10             	sub    $0x10,%esp
  if(fork() > 0)
   9:	e8 3c 02 00 00       	call   24a <fork>
   e:	85 c0                	test   %eax,%eax
  10:	7e 0c                	jle    1e <main+0x1e>
    sleep(5);  // Let child exit before parent.
  12:	c7 04 24 05 00 00 00 	movl   $0x5,(%esp)
  19:	e8 c4 02 00 00       	call   2e2 <sleep>
  exit(0);
  1e:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  25:	e8 28 02 00 00       	call   252 <exit>
  2a:	66 90                	xchg   %ax,%ax
  2c:	66 90                	xchg   %ax,%ax
  2e:	66 90                	xchg   %ax,%ax

00000030 <strcpy>:
#include "user.h"
#include "x86.h"

char*
strcpy(char *s, const char *t)
{
  30:	55                   	push   %ebp
  31:	89 e5                	mov    %esp,%ebp
  33:	8b 45 08             	mov    0x8(%ebp),%eax
  36:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  39:	53                   	push   %ebx
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
  3a:	89 c2                	mov    %eax,%edx
  3c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  40:	83 c1 01             	add    $0x1,%ecx
  43:	0f b6 59 ff          	movzbl -0x1(%ecx),%ebx
  47:	83 c2 01             	add    $0x1,%edx
  4a:	84 db                	test   %bl,%bl
  4c:	88 5a ff             	mov    %bl,-0x1(%edx)
  4f:	75 ef                	jne    40 <strcpy+0x10>
    ;
  return os;
}
  51:	5b                   	pop    %ebx
  52:	5d                   	pop    %ebp
  53:	c3                   	ret    
  54:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  5a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

00000060 <strcmp>:

int
strcmp(const char *p, const char *q)
{
  60:	55                   	push   %ebp
  61:	89 e5                	mov    %esp,%ebp
  63:	8b 55 08             	mov    0x8(%ebp),%edx
  66:	53                   	push   %ebx
  67:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  while(*p && *p == *q)
  6a:	0f b6 02             	movzbl (%edx),%eax
  6d:	84 c0                	test   %al,%al
  6f:	74 2d                	je     9e <strcmp+0x3e>
  71:	0f b6 19             	movzbl (%ecx),%ebx
  74:	38 d8                	cmp    %bl,%al
  76:	74 0e                	je     86 <strcmp+0x26>
  78:	eb 2b                	jmp    a5 <strcmp+0x45>
  7a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  80:	38 c8                	cmp    %cl,%al
  82:	75 15                	jne    99 <strcmp+0x39>
    p++, q++;
  84:	89 d9                	mov    %ebx,%ecx
  86:	83 c2 01             	add    $0x1,%edx
  while(*p && *p == *q)
  89:	0f b6 02             	movzbl (%edx),%eax
    p++, q++;
  8c:	8d 59 01             	lea    0x1(%ecx),%ebx
  while(*p && *p == *q)
  8f:	0f b6 49 01          	movzbl 0x1(%ecx),%ecx
  93:	84 c0                	test   %al,%al
  95:	75 e9                	jne    80 <strcmp+0x20>
  97:	31 c0                	xor    %eax,%eax
  return (uchar)*p - (uchar)*q;
  99:	29 c8                	sub    %ecx,%eax
}
  9b:	5b                   	pop    %ebx
  9c:	5d                   	pop    %ebp
  9d:	c3                   	ret    
  9e:	0f b6 09             	movzbl (%ecx),%ecx
  while(*p && *p == *q)
  a1:	31 c0                	xor    %eax,%eax
  a3:	eb f4                	jmp    99 <strcmp+0x39>
  a5:	0f b6 cb             	movzbl %bl,%ecx
  a8:	eb ef                	jmp    99 <strcmp+0x39>
  aa:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

000000b0 <strlen>:

uint
strlen(const char *s)
{
  b0:	55                   	push   %ebp
  b1:	89 e5                	mov    %esp,%ebp
  b3:	8b 4d 08             	mov    0x8(%ebp),%ecx
  int n;

  for(n = 0; s[n]; n++)
  b6:	80 39 00             	cmpb   $0x0,(%ecx)
  b9:	74 12                	je     cd <strlen+0x1d>
  bb:	31 d2                	xor    %edx,%edx
  bd:	8d 76 00             	lea    0x0(%esi),%esi
  c0:	83 c2 01             	add    $0x1,%edx
  c3:	80 3c 11 00          	cmpb   $0x0,(%ecx,%edx,1)
  c7:	89 d0                	mov    %edx,%eax
  c9:	75 f5                	jne    c0 <strlen+0x10>
    ;
  return n;
}
  cb:	5d                   	pop    %ebp
  cc:	c3                   	ret    
  for(n = 0; s[n]; n++)
  cd:	31 c0                	xor    %eax,%eax
}
  cf:	5d                   	pop    %ebp
  d0:	c3                   	ret    
  d1:	eb 0d                	jmp    e0 <memset>
  d3:	90                   	nop
  d4:	90                   	nop
  d5:	90                   	nop
  d6:	90                   	nop
  d7:	90                   	nop
  d8:	90                   	nop
  d9:	90                   	nop
  da:	90                   	nop
  db:	90                   	nop
  dc:	90                   	nop
  dd:	90                   	nop
  de:	90                   	nop
  df:	90                   	nop

000000e0 <memset>:

void*
memset(void *dst, int c, uint n)
{
  e0:	55                   	push   %ebp
  e1:	89 e5                	mov    %esp,%ebp
  e3:	8b 55 08             	mov    0x8(%ebp),%edx
  e6:	57                   	push   %edi
}

static inline void
stosb(void *addr, int data, int cnt)
{
  asm volatile("cld; rep stosb" :
  e7:	8b 4d 10             	mov    0x10(%ebp),%ecx
  ea:	8b 45 0c             	mov    0xc(%ebp),%eax
  ed:	89 d7                	mov    %edx,%edi
  ef:	fc                   	cld    
  f0:	f3 aa                	rep stos %al,%es:(%edi)
  stosb(dst, c, n);
  return dst;
}
  f2:	89 d0                	mov    %edx,%eax
  f4:	5f                   	pop    %edi
  f5:	5d                   	pop    %ebp
  f6:	c3                   	ret    
  f7:	89 f6                	mov    %esi,%esi
  f9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000100 <strchr>:

char*
strchr(const char *s, char c)
{
 100:	55                   	push   %ebp
 101:	89 e5                	mov    %esp,%ebp
 103:	8b 45 08             	mov    0x8(%ebp),%eax
 106:	53                   	push   %ebx
 107:	8b 55 0c             	mov    0xc(%ebp),%edx
  for(; *s; s++)
 10a:	0f b6 18             	movzbl (%eax),%ebx
 10d:	84 db                	test   %bl,%bl
 10f:	74 1d                	je     12e <strchr+0x2e>
    if(*s == c)
 111:	38 d3                	cmp    %dl,%bl
 113:	89 d1                	mov    %edx,%ecx
 115:	75 0d                	jne    124 <strchr+0x24>
 117:	eb 17                	jmp    130 <strchr+0x30>
 119:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 120:	38 ca                	cmp    %cl,%dl
 122:	74 0c                	je     130 <strchr+0x30>
  for(; *s; s++)
 124:	83 c0 01             	add    $0x1,%eax
 127:	0f b6 10             	movzbl (%eax),%edx
 12a:	84 d2                	test   %dl,%dl
 12c:	75 f2                	jne    120 <strchr+0x20>
      return (char*)s;
  return 0;
 12e:	31 c0                	xor    %eax,%eax
}
 130:	5b                   	pop    %ebx
 131:	5d                   	pop    %ebp
 132:	c3                   	ret    
 133:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 139:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000140 <gets>:

char*
gets(char *buf, int max)
{
 140:	55                   	push   %ebp
 141:	89 e5                	mov    %esp,%ebp
 143:	57                   	push   %edi
 144:	56                   	push   %esi
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
 145:	31 f6                	xor    %esi,%esi
{
 147:	53                   	push   %ebx
 148:	83 ec 2c             	sub    $0x2c,%esp
    cc = read(0, &c, 1);
 14b:	8d 7d e7             	lea    -0x19(%ebp),%edi
  for(i=0; i+1 < max; ){
 14e:	eb 31                	jmp    181 <gets+0x41>
    cc = read(0, &c, 1);
 150:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 157:	00 
 158:	89 7c 24 04          	mov    %edi,0x4(%esp)
 15c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
 163:	e8 02 01 00 00       	call   26a <read>
    if(cc < 1)
 168:	85 c0                	test   %eax,%eax
 16a:	7e 1d                	jle    189 <gets+0x49>
      break;
    buf[i++] = c;
 16c:	0f b6 45 e7          	movzbl -0x19(%ebp),%eax
  for(i=0; i+1 < max; ){
 170:	89 de                	mov    %ebx,%esi
    buf[i++] = c;
 172:	8b 55 08             	mov    0x8(%ebp),%edx
    if(c == '\n' || c == '\r')
 175:	3c 0d                	cmp    $0xd,%al
    buf[i++] = c;
 177:	88 44 1a ff          	mov    %al,-0x1(%edx,%ebx,1)
    if(c == '\n' || c == '\r')
 17b:	74 0c                	je     189 <gets+0x49>
 17d:	3c 0a                	cmp    $0xa,%al
 17f:	74 08                	je     189 <gets+0x49>
  for(i=0; i+1 < max; ){
 181:	8d 5e 01             	lea    0x1(%esi),%ebx
 184:	3b 5d 0c             	cmp    0xc(%ebp),%ebx
 187:	7c c7                	jl     150 <gets+0x10>
      break;
  }
  buf[i] = '\0';
 189:	8b 45 08             	mov    0x8(%ebp),%eax
 18c:	c6 04 30 00          	movb   $0x0,(%eax,%esi,1)
  return buf;
}
 190:	83 c4 2c             	add    $0x2c,%esp
 193:	5b                   	pop    %ebx
 194:	5e                   	pop    %esi
 195:	5f                   	pop    %edi
 196:	5d                   	pop    %ebp
 197:	c3                   	ret    
 198:	90                   	nop
 199:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

000001a0 <stat>:

int
stat(const char *n, struct stat *st)
{
 1a0:	55                   	push   %ebp
 1a1:	89 e5                	mov    %esp,%ebp
 1a3:	56                   	push   %esi
 1a4:	53                   	push   %ebx
 1a5:	83 ec 10             	sub    $0x10,%esp
  int fd;
  int r;

  fd = open(n, O_RDONLY);
 1a8:	8b 45 08             	mov    0x8(%ebp),%eax
 1ab:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
 1b2:	00 
 1b3:	89 04 24             	mov    %eax,(%esp)
 1b6:	e8 d7 00 00 00       	call   292 <open>
  if(fd < 0)
 1bb:	85 c0                	test   %eax,%eax
  fd = open(n, O_RDONLY);
 1bd:	89 c3                	mov    %eax,%ebx
  if(fd < 0)
 1bf:	78 27                	js     1e8 <stat+0x48>
    return -1;
  r = fstat(fd, st);
 1c1:	8b 45 0c             	mov    0xc(%ebp),%eax
 1c4:	89 1c 24             	mov    %ebx,(%esp)
 1c7:	89 44 24 04          	mov    %eax,0x4(%esp)
 1cb:	e8 da 00 00 00       	call   2aa <fstat>
  close(fd);
 1d0:	89 1c 24             	mov    %ebx,(%esp)
  r = fstat(fd, st);
 1d3:	89 c6                	mov    %eax,%esi
  close(fd);
 1d5:	e8 a0 00 00 00       	call   27a <close>
  return r;
 1da:	89 f0                	mov    %esi,%eax
}
 1dc:	83 c4 10             	add    $0x10,%esp
 1df:	5b                   	pop    %ebx
 1e0:	5e                   	pop    %esi
 1e1:	5d                   	pop    %ebp
 1e2:	c3                   	ret    
 1e3:	90                   	nop
 1e4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    return -1;
 1e8:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
 1ed:	eb ed                	jmp    1dc <stat+0x3c>
 1ef:	90                   	nop

000001f0 <atoi>:

int
atoi(const char *s)
{
 1f0:	55                   	push   %ebp
 1f1:	89 e5                	mov    %esp,%ebp
 1f3:	8b 4d 08             	mov    0x8(%ebp),%ecx
 1f6:	53                   	push   %ebx
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
 1f7:	0f be 11             	movsbl (%ecx),%edx
 1fa:	8d 42 d0             	lea    -0x30(%edx),%eax
 1fd:	3c 09                	cmp    $0x9,%al
  n = 0;
 1ff:	b8 00 00 00 00       	mov    $0x0,%eax
  while('0' <= *s && *s <= '9')
 204:	77 17                	ja     21d <atoi+0x2d>
 206:	66 90                	xchg   %ax,%ax
    n = n*10 + *s++ - '0';
 208:	83 c1 01             	add    $0x1,%ecx
 20b:	8d 04 80             	lea    (%eax,%eax,4),%eax
 20e:	8d 44 42 d0          	lea    -0x30(%edx,%eax,2),%eax
  while('0' <= *s && *s <= '9')
 212:	0f be 11             	movsbl (%ecx),%edx
 215:	8d 5a d0             	lea    -0x30(%edx),%ebx
 218:	80 fb 09             	cmp    $0x9,%bl
 21b:	76 eb                	jbe    208 <atoi+0x18>
  return n;
}
 21d:	5b                   	pop    %ebx
 21e:	5d                   	pop    %ebp
 21f:	c3                   	ret    

00000220 <memmove>:

void*
memmove(void *vdst, const void *vsrc, int n)
{
 220:	55                   	push   %ebp
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
 221:	31 d2                	xor    %edx,%edx
{
 223:	89 e5                	mov    %esp,%ebp
 225:	56                   	push   %esi
 226:	8b 45 08             	mov    0x8(%ebp),%eax
 229:	53                   	push   %ebx
 22a:	8b 5d 10             	mov    0x10(%ebp),%ebx
 22d:	8b 75 0c             	mov    0xc(%ebp),%esi
  while(n-- > 0)
 230:	85 db                	test   %ebx,%ebx
 232:	7e 12                	jle    246 <memmove+0x26>
 234:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    *dst++ = *src++;
 238:	0f b6 0c 16          	movzbl (%esi,%edx,1),%ecx
 23c:	88 0c 10             	mov    %cl,(%eax,%edx,1)
 23f:	83 c2 01             	add    $0x1,%edx
  while(n-- > 0)
 242:	39 da                	cmp    %ebx,%edx
 244:	75 f2                	jne    238 <memmove+0x18>
  return vdst;
}
 246:	5b                   	pop    %ebx
 247:	5e                   	pop    %esi
 248:	5d                   	pop    %ebp
 249:	c3                   	ret    

0000024a <fork>:
 24a:	b8 01 00 00 00       	mov    $0x1,%eax
 24f:	cd 40                	int    $0x40
 251:	c3                   	ret    

00000252 <exit>:
 252:	b8 02 00 00 00       	mov    $0x2,%eax
 257:	cd 40                	int    $0x40
 259:	c3                   	ret    

0000025a <wait>:
 25a:	b8 03 00 00 00       	mov    $0x3,%eax
 25f:	cd 40                	int    $0x40
 261:	c3                   	ret    

00000262 <pipe>:
 262:	b8 04 00 00 00       	mov    $0x4,%eax
 267:	cd 40                	int    $0x40
 269:	c3                   	ret    

0000026a <read>:
 26a:	b8 05 00 00 00       	mov    $0x5,%eax
 26f:	cd 40                	int    $0x40
 271:	c3                   	ret    

00000272 <write>:
 272:	b8 10 00 00 00       	mov    $0x10,%eax
 277:	cd 40                	int    $0x40
 279:	c3                   	ret    

0000027a <close>:
 27a:	b8 15 00 00 00       	mov    $0x15,%eax
 27f:	cd 40                	int    $0x40
 281:	c3                   	ret    

00000282 <kill>:
 282:	b8 06 00 00 00       	mov    $0x6,%eax
 287:	cd 40                	int    $0x40
 289:	c3                   	ret    

0000028a <exec>:
 28a:	b8 07 00 00 00       	mov    $0x7,%eax
 28f:	cd 40                	int    $0x40
 291:	c3                   	ret    

00000292 <open>:
 292:	b8 0f 00 00 00       	mov    $0xf,%eax
 297:	cd 40                	int    $0x40
 299:	c3                   	ret    

0000029a <mknod>:
 29a:	b8 11 00 00 00       	mov    $0x11,%eax
 29f:	cd 40                	int    $0x40
 2a1:	c3                   	ret    

000002a2 <unlink>:
 2a2:	b8 12 00 00 00       	mov    $0x12,%eax
 2a7:	cd 40                	int    $0x40
 2a9:	c3                   	ret    

000002aa <fstat>:
 2aa:	b8 08 00 00 00       	mov    $0x8,%eax
 2af:	cd 40                	int    $0x40
 2b1:	c3                   	ret    

000002b2 <link>:
 2b2:	b8 13 00 00 00       	mov    $0x13,%eax
 2b7:	cd 40                	int    $0x40
 2b9:	c3                   	ret    

000002ba <mkdir>:
 2ba:	b8 14 00 00 00       	mov    $0x14,%eax
 2bf:	cd 40                	int    $0x40
 2c1:	c3                   	ret    

000002c2 <chdir>:
 2c2:	b8 09 00 00 00       	mov    $0x9,%eax
 2c7:	cd 40                	int    $0x40
 2c9:	c3                   	ret    

000002ca <dup>:
 2ca:	b8 0a 00 00 00       	mov    $0xa,%eax
 2cf:	cd 40                	int    $0x40
 2d1:	c3                   	ret    

000002d2 <getpid>:
 2d2:	b8 0b 00 00 00       	mov    $0xb,%eax
 2d7:	cd 40                	int    $0x40
 2d9:	c3                   	ret    

000002da <sbrk>:
 2da:	b8 0c 00 00 00       	mov    $0xc,%eax
 2df:	cd 40                	int    $0x40
 2e1:	c3                   	ret    

000002e2 <sleep>:
 2e2:	b8 0d 00 00 00       	mov    $0xd,%eax
 2e7:	cd 40                	int    $0x40
 2e9:	c3                   	ret    

000002ea <uptime>:
 2ea:	b8 0e 00 00 00       	mov    $0xe,%eax
 2ef:	cd 40                	int    $0x40
 2f1:	c3                   	ret    
 2f2:	66 90                	xchg   %ax,%ax
 2f4:	66 90                	xchg   %ax,%ax
 2f6:	66 90                	xchg   %ax,%ax
 2f8:	66 90                	xchg   %ax,%ax
 2fa:	66 90                	xchg   %ax,%ax
 2fc:	66 90                	xchg   %ax,%ax
 2fe:	66 90                	xchg   %ax,%ax

00000300 <printint>:
  write(fd, &c, 1);
}

static void
printint(int fd, int xx, int base, int sgn)
{
 300:	55                   	push   %ebp
 301:	89 e5                	mov    %esp,%ebp
 303:	57                   	push   %edi
 304:	56                   	push   %esi
 305:	89 c6                	mov    %eax,%esi
 307:	53                   	push   %ebx
 308:	83 ec 4c             	sub    $0x4c,%esp
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
 30b:	8b 5d 08             	mov    0x8(%ebp),%ebx
 30e:	85 db                	test   %ebx,%ebx
 310:	74 09                	je     31b <printint+0x1b>
 312:	89 d0                	mov    %edx,%eax
 314:	c1 e8 1f             	shr    $0x1f,%eax
 317:	84 c0                	test   %al,%al
 319:	75 75                	jne    390 <printint+0x90>
    neg = 1;
    x = -xx;
  } else {
    x = xx;
 31b:	89 d0                	mov    %edx,%eax
  neg = 0;
 31d:	c7 45 c4 00 00 00 00 	movl   $0x0,-0x3c(%ebp)
 324:	89 75 c0             	mov    %esi,-0x40(%ebp)
  }

  i = 0;
 327:	31 ff                	xor    %edi,%edi
 329:	89 ce                	mov    %ecx,%esi
 32b:	8d 5d d7             	lea    -0x29(%ebp),%ebx
 32e:	eb 02                	jmp    332 <printint+0x32>
  do{
    buf[i++] = digits[x % base];
 330:	89 cf                	mov    %ecx,%edi
 332:	31 d2                	xor    %edx,%edx
 334:	f7 f6                	div    %esi
 336:	8d 4f 01             	lea    0x1(%edi),%ecx
 339:	0f b6 92 0d 07 00 00 	movzbl 0x70d(%edx),%edx
  }while((x /= base) != 0);
 340:	85 c0                	test   %eax,%eax
    buf[i++] = digits[x % base];
 342:	88 14 0b             	mov    %dl,(%ebx,%ecx,1)
  }while((x /= base) != 0);
 345:	75 e9                	jne    330 <printint+0x30>
  if(neg)
 347:	8b 55 c4             	mov    -0x3c(%ebp),%edx
    buf[i++] = digits[x % base];
 34a:	89 c8                	mov    %ecx,%eax
 34c:	8b 75 c0             	mov    -0x40(%ebp),%esi
  if(neg)
 34f:	85 d2                	test   %edx,%edx
 351:	74 08                	je     35b <printint+0x5b>
    buf[i++] = '-';
 353:	8d 4f 02             	lea    0x2(%edi),%ecx
 356:	c6 44 05 d8 2d       	movb   $0x2d,-0x28(%ebp,%eax,1)

  while(--i >= 0)
 35b:	8d 79 ff             	lea    -0x1(%ecx),%edi
 35e:	66 90                	xchg   %ax,%ax
 360:	0f b6 44 3d d8       	movzbl -0x28(%ebp,%edi,1),%eax
 365:	83 ef 01             	sub    $0x1,%edi
  write(fd, &c, 1);
 368:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 36f:	00 
 370:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 374:	89 34 24             	mov    %esi,(%esp)
 377:	88 45 d7             	mov    %al,-0x29(%ebp)
 37a:	e8 f3 fe ff ff       	call   272 <write>
  while(--i >= 0)
 37f:	83 ff ff             	cmp    $0xffffffff,%edi
 382:	75 dc                	jne    360 <printint+0x60>
    putc(fd, buf[i]);
}
 384:	83 c4 4c             	add    $0x4c,%esp
 387:	5b                   	pop    %ebx
 388:	5e                   	pop    %esi
 389:	5f                   	pop    %edi
 38a:	5d                   	pop    %ebp
 38b:	c3                   	ret    
 38c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    x = -xx;
 390:	89 d0                	mov    %edx,%eax
 392:	f7 d8                	neg    %eax
    neg = 1;
 394:	c7 45 c4 01 00 00 00 	movl   $0x1,-0x3c(%ebp)
 39b:	eb 87                	jmp    324 <printint+0x24>
 39d:	8d 76 00             	lea    0x0(%esi),%esi

000003a0 <printf>:

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, const char *fmt, ...)
{
 3a0:	55                   	push   %ebp
 3a1:	89 e5                	mov    %esp,%ebp
 3a3:	57                   	push   %edi
  char *s;
  int c, i, state;
  uint *ap;

  state = 0;
 3a4:	31 ff                	xor    %edi,%edi
{
 3a6:	56                   	push   %esi
 3a7:	53                   	push   %ebx
 3a8:	83 ec 3c             	sub    $0x3c,%esp
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
 3ab:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  ap = (uint*)(void*)&fmt + 1;
 3ae:	8d 45 10             	lea    0x10(%ebp),%eax
{
 3b1:	8b 75 08             	mov    0x8(%ebp),%esi
  ap = (uint*)(void*)&fmt + 1;
 3b4:	89 45 d4             	mov    %eax,-0x2c(%ebp)
  for(i = 0; fmt[i]; i++){
 3b7:	0f b6 13             	movzbl (%ebx),%edx
 3ba:	83 c3 01             	add    $0x1,%ebx
 3bd:	84 d2                	test   %dl,%dl
 3bf:	75 39                	jne    3fa <printf+0x5a>
 3c1:	e9 c2 00 00 00       	jmp    488 <printf+0xe8>
 3c6:	66 90                	xchg   %ax,%ax
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
 3c8:	83 fa 25             	cmp    $0x25,%edx
 3cb:	0f 84 bf 00 00 00    	je     490 <printf+0xf0>
  write(fd, &c, 1);
 3d1:	8d 45 e2             	lea    -0x1e(%ebp),%eax
 3d4:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 3db:	00 
 3dc:	89 44 24 04          	mov    %eax,0x4(%esp)
 3e0:	89 34 24             	mov    %esi,(%esp)
        state = '%';
      } else {
        putc(fd, c);
 3e3:	88 55 e2             	mov    %dl,-0x1e(%ebp)
  write(fd, &c, 1);
 3e6:	e8 87 fe ff ff       	call   272 <write>
 3eb:	83 c3 01             	add    $0x1,%ebx
  for(i = 0; fmt[i]; i++){
 3ee:	0f b6 53 ff          	movzbl -0x1(%ebx),%edx
 3f2:	84 d2                	test   %dl,%dl
 3f4:	0f 84 8e 00 00 00    	je     488 <printf+0xe8>
    if(state == 0){
 3fa:	85 ff                	test   %edi,%edi
    c = fmt[i] & 0xff;
 3fc:	0f be c2             	movsbl %dl,%eax
    if(state == 0){
 3ff:	74 c7                	je     3c8 <printf+0x28>
      }
    } else if(state == '%'){
 401:	83 ff 25             	cmp    $0x25,%edi
 404:	75 e5                	jne    3eb <printf+0x4b>
      if(c == 'd'){
 406:	83 fa 64             	cmp    $0x64,%edx
 409:	0f 84 31 01 00 00    	je     540 <printf+0x1a0>
        printint(fd, *ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
 40f:	25 f7 00 00 00       	and    $0xf7,%eax
 414:	83 f8 70             	cmp    $0x70,%eax
 417:	0f 84 83 00 00 00    	je     4a0 <printf+0x100>
        printint(fd, *ap, 16, 0);
        ap++;
      } else if(c == 's'){
 41d:	83 fa 73             	cmp    $0x73,%edx
 420:	0f 84 a2 00 00 00    	je     4c8 <printf+0x128>
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
        }
      } else if(c == 'c'){
 426:	83 fa 63             	cmp    $0x63,%edx
 429:	0f 84 35 01 00 00    	je     564 <printf+0x1c4>
        putc(fd, *ap);
        ap++;
      } else if(c == '%'){
 42f:	83 fa 25             	cmp    $0x25,%edx
 432:	0f 84 e0 00 00 00    	je     518 <printf+0x178>
  write(fd, &c, 1);
 438:	8d 45 e6             	lea    -0x1a(%ebp),%eax
 43b:	83 c3 01             	add    $0x1,%ebx
 43e:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 445:	00 
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
 446:	31 ff                	xor    %edi,%edi
  write(fd, &c, 1);
 448:	89 44 24 04          	mov    %eax,0x4(%esp)
 44c:	89 34 24             	mov    %esi,(%esp)
 44f:	89 55 d0             	mov    %edx,-0x30(%ebp)
 452:	c6 45 e6 25          	movb   $0x25,-0x1a(%ebp)
 456:	e8 17 fe ff ff       	call   272 <write>
        putc(fd, c);
 45b:	8b 55 d0             	mov    -0x30(%ebp),%edx
  write(fd, &c, 1);
 45e:	8d 45 e7             	lea    -0x19(%ebp),%eax
 461:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 468:	00 
 469:	89 44 24 04          	mov    %eax,0x4(%esp)
 46d:	89 34 24             	mov    %esi,(%esp)
        putc(fd, c);
 470:	88 55 e7             	mov    %dl,-0x19(%ebp)
  write(fd, &c, 1);
 473:	e8 fa fd ff ff       	call   272 <write>
  for(i = 0; fmt[i]; i++){
 478:	0f b6 53 ff          	movzbl -0x1(%ebx),%edx
 47c:	84 d2                	test   %dl,%dl
 47e:	0f 85 76 ff ff ff    	jne    3fa <printf+0x5a>
 484:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    }
  }
}
 488:	83 c4 3c             	add    $0x3c,%esp
 48b:	5b                   	pop    %ebx
 48c:	5e                   	pop    %esi
 48d:	5f                   	pop    %edi
 48e:	5d                   	pop    %ebp
 48f:	c3                   	ret    
        state = '%';
 490:	bf 25 00 00 00       	mov    $0x25,%edi
 495:	e9 51 ff ff ff       	jmp    3eb <printf+0x4b>
 49a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        printint(fd, *ap, 16, 0);
 4a0:	8b 45 d4             	mov    -0x2c(%ebp),%eax
 4a3:	b9 10 00 00 00       	mov    $0x10,%ecx
      state = 0;
 4a8:	31 ff                	xor    %edi,%edi
        printint(fd, *ap, 16, 0);
 4aa:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
 4b1:	8b 10                	mov    (%eax),%edx
 4b3:	89 f0                	mov    %esi,%eax
 4b5:	e8 46 fe ff ff       	call   300 <printint>
        ap++;
 4ba:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
 4be:	e9 28 ff ff ff       	jmp    3eb <printf+0x4b>
 4c3:	90                   	nop
 4c4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
        s = (char*)*ap;
 4c8:	8b 45 d4             	mov    -0x2c(%ebp),%eax
        ap++;
 4cb:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
        s = (char*)*ap;
 4cf:	8b 38                	mov    (%eax),%edi
          s = "(null)";
 4d1:	b8 06 07 00 00       	mov    $0x706,%eax
 4d6:	85 ff                	test   %edi,%edi
 4d8:	0f 44 f8             	cmove  %eax,%edi
        while(*s != 0){
 4db:	0f b6 07             	movzbl (%edi),%eax
 4de:	84 c0                	test   %al,%al
 4e0:	74 2a                	je     50c <printf+0x16c>
 4e2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 4e8:	88 45 e3             	mov    %al,-0x1d(%ebp)
  write(fd, &c, 1);
 4eb:	8d 45 e3             	lea    -0x1d(%ebp),%eax
          s++;
 4ee:	83 c7 01             	add    $0x1,%edi
  write(fd, &c, 1);
 4f1:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 4f8:	00 
 4f9:	89 44 24 04          	mov    %eax,0x4(%esp)
 4fd:	89 34 24             	mov    %esi,(%esp)
 500:	e8 6d fd ff ff       	call   272 <write>
        while(*s != 0){
 505:	0f b6 07             	movzbl (%edi),%eax
 508:	84 c0                	test   %al,%al
 50a:	75 dc                	jne    4e8 <printf+0x148>
      state = 0;
 50c:	31 ff                	xor    %edi,%edi
 50e:	e9 d8 fe ff ff       	jmp    3eb <printf+0x4b>
 513:	90                   	nop
 514:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  write(fd, &c, 1);
 518:	8d 45 e5             	lea    -0x1b(%ebp),%eax
      state = 0;
 51b:	31 ff                	xor    %edi,%edi
  write(fd, &c, 1);
 51d:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 524:	00 
 525:	89 44 24 04          	mov    %eax,0x4(%esp)
 529:	89 34 24             	mov    %esi,(%esp)
 52c:	c6 45 e5 25          	movb   $0x25,-0x1b(%ebp)
 530:	e8 3d fd ff ff       	call   272 <write>
 535:	e9 b1 fe ff ff       	jmp    3eb <printf+0x4b>
 53a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        printint(fd, *ap, 10, 1);
 540:	8b 45 d4             	mov    -0x2c(%ebp),%eax
 543:	b9 0a 00 00 00       	mov    $0xa,%ecx
      state = 0;
 548:	66 31 ff             	xor    %di,%di
        printint(fd, *ap, 10, 1);
 54b:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 552:	8b 10                	mov    (%eax),%edx
 554:	89 f0                	mov    %esi,%eax
 556:	e8 a5 fd ff ff       	call   300 <printint>
        ap++;
 55b:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
 55f:	e9 87 fe ff ff       	jmp    3eb <printf+0x4b>
        putc(fd, *ap);
 564:	8b 45 d4             	mov    -0x2c(%ebp),%eax
      state = 0;
 567:	31 ff                	xor    %edi,%edi
        putc(fd, *ap);
 569:	8b 00                	mov    (%eax),%eax
  write(fd, &c, 1);
 56b:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 572:	00 
 573:	89 34 24             	mov    %esi,(%esp)
        putc(fd, *ap);
 576:	88 45 e4             	mov    %al,-0x1c(%ebp)
  write(fd, &c, 1);
 579:	8d 45 e4             	lea    -0x1c(%ebp),%eax
 57c:	89 44 24 04          	mov    %eax,0x4(%esp)
 580:	e8 ed fc ff ff       	call   272 <write>
        ap++;
 585:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
 589:	e9 5d fe ff ff       	jmp    3eb <printf+0x4b>
 58e:	66 90                	xchg   %ax,%ax

00000590 <free>:
static Header base;
static Header *freep;

void
free(void *ap)
{
 590:	55                   	push   %ebp
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 591:	a1 84 09 00 00       	mov    0x984,%eax
{
 596:	89 e5                	mov    %esp,%ebp
 598:	57                   	push   %edi
 599:	56                   	push   %esi
 59a:	53                   	push   %ebx
 59b:	8b 5d 08             	mov    0x8(%ebp),%ebx
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 59e:	8b 08                	mov    (%eax),%ecx
  bp = (Header*)ap - 1;
 5a0:	8d 53 f8             	lea    -0x8(%ebx),%edx
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 5a3:	39 d0                	cmp    %edx,%eax
 5a5:	72 11                	jb     5b8 <free+0x28>
 5a7:	90                   	nop
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 5a8:	39 c8                	cmp    %ecx,%eax
 5aa:	72 04                	jb     5b0 <free+0x20>
 5ac:	39 ca                	cmp    %ecx,%edx
 5ae:	72 10                	jb     5c0 <free+0x30>
 5b0:	89 c8                	mov    %ecx,%eax
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 5b2:	39 d0                	cmp    %edx,%eax
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 5b4:	8b 08                	mov    (%eax),%ecx
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 5b6:	73 f0                	jae    5a8 <free+0x18>
 5b8:	39 ca                	cmp    %ecx,%edx
 5ba:	72 04                	jb     5c0 <free+0x30>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 5bc:	39 c8                	cmp    %ecx,%eax
 5be:	72 f0                	jb     5b0 <free+0x20>
      break;
  if(bp + bp->s.size == p->s.ptr){
 5c0:	8b 73 fc             	mov    -0x4(%ebx),%esi
 5c3:	8d 3c f2             	lea    (%edx,%esi,8),%edi
 5c6:	39 cf                	cmp    %ecx,%edi
 5c8:	74 1e                	je     5e8 <free+0x58>
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
 5ca:	89 4b f8             	mov    %ecx,-0x8(%ebx)
  if(p + p->s.size == bp){
 5cd:	8b 48 04             	mov    0x4(%eax),%ecx
 5d0:	8d 34 c8             	lea    (%eax,%ecx,8),%esi
 5d3:	39 f2                	cmp    %esi,%edx
 5d5:	74 28                	je     5ff <free+0x6f>
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
 5d7:	89 10                	mov    %edx,(%eax)
  freep = p;
 5d9:	a3 84 09 00 00       	mov    %eax,0x984
}
 5de:	5b                   	pop    %ebx
 5df:	5e                   	pop    %esi
 5e0:	5f                   	pop    %edi
 5e1:	5d                   	pop    %ebp
 5e2:	c3                   	ret    
 5e3:	90                   	nop
 5e4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    bp->s.size += p->s.ptr->s.size;
 5e8:	03 71 04             	add    0x4(%ecx),%esi
 5eb:	89 73 fc             	mov    %esi,-0x4(%ebx)
    bp->s.ptr = p->s.ptr->s.ptr;
 5ee:	8b 08                	mov    (%eax),%ecx
 5f0:	8b 09                	mov    (%ecx),%ecx
 5f2:	89 4b f8             	mov    %ecx,-0x8(%ebx)
  if(p + p->s.size == bp){
 5f5:	8b 48 04             	mov    0x4(%eax),%ecx
 5f8:	8d 34 c8             	lea    (%eax,%ecx,8),%esi
 5fb:	39 f2                	cmp    %esi,%edx
 5fd:	75 d8                	jne    5d7 <free+0x47>
    p->s.size += bp->s.size;
 5ff:	03 4b fc             	add    -0x4(%ebx),%ecx
  freep = p;
 602:	a3 84 09 00 00       	mov    %eax,0x984
    p->s.size += bp->s.size;
 607:	89 48 04             	mov    %ecx,0x4(%eax)
    p->s.ptr = bp->s.ptr;
 60a:	8b 53 f8             	mov    -0x8(%ebx),%edx
 60d:	89 10                	mov    %edx,(%eax)
}
 60f:	5b                   	pop    %ebx
 610:	5e                   	pop    %esi
 611:	5f                   	pop    %edi
 612:	5d                   	pop    %ebp
 613:	c3                   	ret    
 614:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 61a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

00000620 <malloc>:
  return freep;
}

void*
malloc(uint nbytes)
{
 620:	55                   	push   %ebp
 621:	89 e5                	mov    %esp,%ebp
 623:	57                   	push   %edi
 624:	56                   	push   %esi
 625:	53                   	push   %ebx
 626:	83 ec 1c             	sub    $0x1c,%esp
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 629:	8b 45 08             	mov    0x8(%ebp),%eax
  if((prevp = freep) == 0){
 62c:	8b 1d 84 09 00 00    	mov    0x984,%ebx
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 632:	8d 48 07             	lea    0x7(%eax),%ecx
 635:	c1 e9 03             	shr    $0x3,%ecx
  if((prevp = freep) == 0){
 638:	85 db                	test   %ebx,%ebx
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 63a:	8d 71 01             	lea    0x1(%ecx),%esi
  if((prevp = freep) == 0){
 63d:	0f 84 9b 00 00 00    	je     6de <malloc+0xbe>
 643:	8b 13                	mov    (%ebx),%edx
 645:	8b 7a 04             	mov    0x4(%edx),%edi
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
 648:	39 fe                	cmp    %edi,%esi
 64a:	76 64                	jbe    6b0 <malloc+0x90>
 64c:	8d 04 f5 00 00 00 00 	lea    0x0(,%esi,8),%eax
  if(nu < 4096)
 653:	bb 00 80 00 00       	mov    $0x8000,%ebx
 658:	89 45 e4             	mov    %eax,-0x1c(%ebp)
 65b:	eb 0e                	jmp    66b <malloc+0x4b>
 65d:	8d 76 00             	lea    0x0(%esi),%esi
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
 660:	8b 02                	mov    (%edx),%eax
    if(p->s.size >= nunits){
 662:	8b 78 04             	mov    0x4(%eax),%edi
 665:	39 fe                	cmp    %edi,%esi
 667:	76 4f                	jbe    6b8 <malloc+0x98>
 669:	89 c2                	mov    %eax,%edx
        p->s.size = nunits;
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
 66b:	3b 15 84 09 00 00    	cmp    0x984,%edx
 671:	75 ed                	jne    660 <malloc+0x40>
  if(nu < 4096)
 673:	8b 45 e4             	mov    -0x1c(%ebp),%eax
 676:	81 fe 00 10 00 00    	cmp    $0x1000,%esi
 67c:	bf 00 10 00 00       	mov    $0x1000,%edi
 681:	0f 43 fe             	cmovae %esi,%edi
 684:	0f 42 c3             	cmovb  %ebx,%eax
  p = sbrk(nu * sizeof(Header));
 687:	89 04 24             	mov    %eax,(%esp)
 68a:	e8 4b fc ff ff       	call   2da <sbrk>
  if(p == (char*)-1)
 68f:	83 f8 ff             	cmp    $0xffffffff,%eax
 692:	74 18                	je     6ac <malloc+0x8c>
  hp->s.size = nu;
 694:	89 78 04             	mov    %edi,0x4(%eax)
  free((void*)(hp + 1));
 697:	83 c0 08             	add    $0x8,%eax
 69a:	89 04 24             	mov    %eax,(%esp)
 69d:	e8 ee fe ff ff       	call   590 <free>
  return freep;
 6a2:	8b 15 84 09 00 00    	mov    0x984,%edx
      if((p = morecore(nunits)) == 0)
 6a8:	85 d2                	test   %edx,%edx
 6aa:	75 b4                	jne    660 <malloc+0x40>
        return 0;
 6ac:	31 c0                	xor    %eax,%eax
 6ae:	eb 20                	jmp    6d0 <malloc+0xb0>
    if(p->s.size >= nunits){
 6b0:	89 d0                	mov    %edx,%eax
 6b2:	89 da                	mov    %ebx,%edx
 6b4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      if(p->s.size == nunits)
 6b8:	39 fe                	cmp    %edi,%esi
 6ba:	74 1c                	je     6d8 <malloc+0xb8>
        p->s.size -= nunits;
 6bc:	29 f7                	sub    %esi,%edi
 6be:	89 78 04             	mov    %edi,0x4(%eax)
        p += p->s.size;
 6c1:	8d 04 f8             	lea    (%eax,%edi,8),%eax
        p->s.size = nunits;
 6c4:	89 70 04             	mov    %esi,0x4(%eax)
      freep = prevp;
 6c7:	89 15 84 09 00 00    	mov    %edx,0x984
      return (void*)(p + 1);
 6cd:	83 c0 08             	add    $0x8,%eax
  }
}
 6d0:	83 c4 1c             	add    $0x1c,%esp
 6d3:	5b                   	pop    %ebx
 6d4:	5e                   	pop    %esi
 6d5:	5f                   	pop    %edi
 6d6:	5d                   	pop    %ebp
 6d7:	c3                   	ret    
        prevp->s.ptr = p->s.ptr;
 6d8:	8b 08                	mov    (%eax),%ecx
 6da:	89 0a                	mov    %ecx,(%edx)
 6dc:	eb e9                	jmp    6c7 <malloc+0xa7>
    base.s.ptr = freep = prevp = &base;
 6de:	c7 05 84 09 00 00 88 	movl   $0x988,0x984
 6e5:	09 00 00 
    base.s.size = 0;
 6e8:	ba 88 09 00 00       	mov    $0x988,%edx
    base.s.ptr = freep = prevp = &base;
 6ed:	c7 05 88 09 00 00 88 	movl   $0x988,0x988
 6f4:	09 00 00 
    base.s.size = 0;
 6f7:	c7 05 8c 09 00 00 00 	movl   $0x0,0x98c
 6fe:	00 00 00 
 701:	e9 46 ff ff ff       	jmp    64c <malloc+0x2c>
