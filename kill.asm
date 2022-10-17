
_kill:     file format elf32-i386


Disassembly of section .text:

00000000 <main>:
#include "stat.h"
#include "user.h"

int
main(int argc, char **argv)
{
   0:	55                   	push   %ebp
   1:	89 e5                	mov    %esp,%ebp
   3:	57                   	push   %edi
   4:	56                   	push   %esi
   5:	53                   	push   %ebx
  int i;

  if(argc < 2){
   6:	bb 01 00 00 00       	mov    $0x1,%ebx
{
   b:	83 e4 f0             	and    $0xfffffff0,%esp
   e:	83 ec 10             	sub    $0x10,%esp
  11:	8b 75 08             	mov    0x8(%ebp),%esi
  14:	8b 7d 0c             	mov    0xc(%ebp),%edi
  if(argc < 2){
  17:	83 fe 01             	cmp    $0x1,%esi
  1a:	7e 2a                	jle    46 <main+0x46>
  1c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    printf(2, "usage: kill pid...\n");
    exit(0);
  }
  for(i=1; i<argc; i++)
    kill(atoi(argv[i]));
  20:	8b 04 9f             	mov    (%edi,%ebx,4),%eax
  for(i=1; i<argc; i++)
  23:	83 c3 01             	add    $0x1,%ebx
    kill(atoi(argv[i]));
  26:	89 04 24             	mov    %eax,(%esp)
  29:	e8 02 02 00 00       	call   230 <atoi>
  2e:	89 04 24             	mov    %eax,(%esp)
  31:	e8 8c 02 00 00       	call   2c2 <kill>
  for(i=1; i<argc; i++)
  36:	39 f3                	cmp    %esi,%ebx
  38:	75 e6                	jne    20 <main+0x20>
  exit(0);
  3a:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  41:	e8 4c 02 00 00       	call   292 <exit>
    printf(2, "usage: kill pid...\n");
  46:	c7 44 24 04 46 07 00 	movl   $0x746,0x4(%esp)
  4d:	00 
  4e:	c7 04 24 02 00 00 00 	movl   $0x2,(%esp)
  55:	e8 86 03 00 00       	call   3e0 <printf>
    exit(0);
  5a:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  61:	e8 2c 02 00 00       	call   292 <exit>
  66:	66 90                	xchg   %ax,%ax
  68:	66 90                	xchg   %ax,%ax
  6a:	66 90                	xchg   %ax,%ax
  6c:	66 90                	xchg   %ax,%ax
  6e:	66 90                	xchg   %ax,%ax

00000070 <strcpy>:
#include "user.h"
#include "x86.h"

char*
strcpy(char *s, const char *t)
{
  70:	55                   	push   %ebp
  71:	89 e5                	mov    %esp,%ebp
  73:	8b 45 08             	mov    0x8(%ebp),%eax
  76:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  79:	53                   	push   %ebx
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
  7a:	89 c2                	mov    %eax,%edx
  7c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  80:	83 c1 01             	add    $0x1,%ecx
  83:	0f b6 59 ff          	movzbl -0x1(%ecx),%ebx
  87:	83 c2 01             	add    $0x1,%edx
  8a:	84 db                	test   %bl,%bl
  8c:	88 5a ff             	mov    %bl,-0x1(%edx)
  8f:	75 ef                	jne    80 <strcpy+0x10>
    ;
  return os;
}
  91:	5b                   	pop    %ebx
  92:	5d                   	pop    %ebp
  93:	c3                   	ret    
  94:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  9a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

000000a0 <strcmp>:

int
strcmp(const char *p, const char *q)
{
  a0:	55                   	push   %ebp
  a1:	89 e5                	mov    %esp,%ebp
  a3:	8b 55 08             	mov    0x8(%ebp),%edx
  a6:	53                   	push   %ebx
  a7:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  while(*p && *p == *q)
  aa:	0f b6 02             	movzbl (%edx),%eax
  ad:	84 c0                	test   %al,%al
  af:	74 2d                	je     de <strcmp+0x3e>
  b1:	0f b6 19             	movzbl (%ecx),%ebx
  b4:	38 d8                	cmp    %bl,%al
  b6:	74 0e                	je     c6 <strcmp+0x26>
  b8:	eb 2b                	jmp    e5 <strcmp+0x45>
  ba:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  c0:	38 c8                	cmp    %cl,%al
  c2:	75 15                	jne    d9 <strcmp+0x39>
    p++, q++;
  c4:	89 d9                	mov    %ebx,%ecx
  c6:	83 c2 01             	add    $0x1,%edx
  while(*p && *p == *q)
  c9:	0f b6 02             	movzbl (%edx),%eax
    p++, q++;
  cc:	8d 59 01             	lea    0x1(%ecx),%ebx
  while(*p && *p == *q)
  cf:	0f b6 49 01          	movzbl 0x1(%ecx),%ecx
  d3:	84 c0                	test   %al,%al
  d5:	75 e9                	jne    c0 <strcmp+0x20>
  d7:	31 c0                	xor    %eax,%eax
  return (uchar)*p - (uchar)*q;
  d9:	29 c8                	sub    %ecx,%eax
}
  db:	5b                   	pop    %ebx
  dc:	5d                   	pop    %ebp
  dd:	c3                   	ret    
  de:	0f b6 09             	movzbl (%ecx),%ecx
  while(*p && *p == *q)
  e1:	31 c0                	xor    %eax,%eax
  e3:	eb f4                	jmp    d9 <strcmp+0x39>
  e5:	0f b6 cb             	movzbl %bl,%ecx
  e8:	eb ef                	jmp    d9 <strcmp+0x39>
  ea:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

000000f0 <strlen>:

uint
strlen(const char *s)
{
  f0:	55                   	push   %ebp
  f1:	89 e5                	mov    %esp,%ebp
  f3:	8b 4d 08             	mov    0x8(%ebp),%ecx
  int n;

  for(n = 0; s[n]; n++)
  f6:	80 39 00             	cmpb   $0x0,(%ecx)
  f9:	74 12                	je     10d <strlen+0x1d>
  fb:	31 d2                	xor    %edx,%edx
  fd:	8d 76 00             	lea    0x0(%esi),%esi
 100:	83 c2 01             	add    $0x1,%edx
 103:	80 3c 11 00          	cmpb   $0x0,(%ecx,%edx,1)
 107:	89 d0                	mov    %edx,%eax
 109:	75 f5                	jne    100 <strlen+0x10>
    ;
  return n;
}
 10b:	5d                   	pop    %ebp
 10c:	c3                   	ret    
  for(n = 0; s[n]; n++)
 10d:	31 c0                	xor    %eax,%eax
}
 10f:	5d                   	pop    %ebp
 110:	c3                   	ret    
 111:	eb 0d                	jmp    120 <memset>
 113:	90                   	nop
 114:	90                   	nop
 115:	90                   	nop
 116:	90                   	nop
 117:	90                   	nop
 118:	90                   	nop
 119:	90                   	nop
 11a:	90                   	nop
 11b:	90                   	nop
 11c:	90                   	nop
 11d:	90                   	nop
 11e:	90                   	nop
 11f:	90                   	nop

00000120 <memset>:

void*
memset(void *dst, int c, uint n)
{
 120:	55                   	push   %ebp
 121:	89 e5                	mov    %esp,%ebp
 123:	8b 55 08             	mov    0x8(%ebp),%edx
 126:	57                   	push   %edi
}

static inline void
stosb(void *addr, int data, int cnt)
{
  asm volatile("cld; rep stosb" :
 127:	8b 4d 10             	mov    0x10(%ebp),%ecx
 12a:	8b 45 0c             	mov    0xc(%ebp),%eax
 12d:	89 d7                	mov    %edx,%edi
 12f:	fc                   	cld    
 130:	f3 aa                	rep stos %al,%es:(%edi)
  stosb(dst, c, n);
  return dst;
}
 132:	89 d0                	mov    %edx,%eax
 134:	5f                   	pop    %edi
 135:	5d                   	pop    %ebp
 136:	c3                   	ret    
 137:	89 f6                	mov    %esi,%esi
 139:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000140 <strchr>:

char*
strchr(const char *s, char c)
{
 140:	55                   	push   %ebp
 141:	89 e5                	mov    %esp,%ebp
 143:	8b 45 08             	mov    0x8(%ebp),%eax
 146:	53                   	push   %ebx
 147:	8b 55 0c             	mov    0xc(%ebp),%edx
  for(; *s; s++)
 14a:	0f b6 18             	movzbl (%eax),%ebx
 14d:	84 db                	test   %bl,%bl
 14f:	74 1d                	je     16e <strchr+0x2e>
    if(*s == c)
 151:	38 d3                	cmp    %dl,%bl
 153:	89 d1                	mov    %edx,%ecx
 155:	75 0d                	jne    164 <strchr+0x24>
 157:	eb 17                	jmp    170 <strchr+0x30>
 159:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 160:	38 ca                	cmp    %cl,%dl
 162:	74 0c                	je     170 <strchr+0x30>
  for(; *s; s++)
 164:	83 c0 01             	add    $0x1,%eax
 167:	0f b6 10             	movzbl (%eax),%edx
 16a:	84 d2                	test   %dl,%dl
 16c:	75 f2                	jne    160 <strchr+0x20>
      return (char*)s;
  return 0;
 16e:	31 c0                	xor    %eax,%eax
}
 170:	5b                   	pop    %ebx
 171:	5d                   	pop    %ebp
 172:	c3                   	ret    
 173:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 179:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000180 <gets>:

char*
gets(char *buf, int max)
{
 180:	55                   	push   %ebp
 181:	89 e5                	mov    %esp,%ebp
 183:	57                   	push   %edi
 184:	56                   	push   %esi
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
 185:	31 f6                	xor    %esi,%esi
{
 187:	53                   	push   %ebx
 188:	83 ec 2c             	sub    $0x2c,%esp
    cc = read(0, &c, 1);
 18b:	8d 7d e7             	lea    -0x19(%ebp),%edi
  for(i=0; i+1 < max; ){
 18e:	eb 31                	jmp    1c1 <gets+0x41>
    cc = read(0, &c, 1);
 190:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 197:	00 
 198:	89 7c 24 04          	mov    %edi,0x4(%esp)
 19c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
 1a3:	e8 02 01 00 00       	call   2aa <read>
    if(cc < 1)
 1a8:	85 c0                	test   %eax,%eax
 1aa:	7e 1d                	jle    1c9 <gets+0x49>
      break;
    buf[i++] = c;
 1ac:	0f b6 45 e7          	movzbl -0x19(%ebp),%eax
  for(i=0; i+1 < max; ){
 1b0:	89 de                	mov    %ebx,%esi
    buf[i++] = c;
 1b2:	8b 55 08             	mov    0x8(%ebp),%edx
    if(c == '\n' || c == '\r')
 1b5:	3c 0d                	cmp    $0xd,%al
    buf[i++] = c;
 1b7:	88 44 1a ff          	mov    %al,-0x1(%edx,%ebx,1)
    if(c == '\n' || c == '\r')
 1bb:	74 0c                	je     1c9 <gets+0x49>
 1bd:	3c 0a                	cmp    $0xa,%al
 1bf:	74 08                	je     1c9 <gets+0x49>
  for(i=0; i+1 < max; ){
 1c1:	8d 5e 01             	lea    0x1(%esi),%ebx
 1c4:	3b 5d 0c             	cmp    0xc(%ebp),%ebx
 1c7:	7c c7                	jl     190 <gets+0x10>
      break;
  }
  buf[i] = '\0';
 1c9:	8b 45 08             	mov    0x8(%ebp),%eax
 1cc:	c6 04 30 00          	movb   $0x0,(%eax,%esi,1)
  return buf;
}
 1d0:	83 c4 2c             	add    $0x2c,%esp
 1d3:	5b                   	pop    %ebx
 1d4:	5e                   	pop    %esi
 1d5:	5f                   	pop    %edi
 1d6:	5d                   	pop    %ebp
 1d7:	c3                   	ret    
 1d8:	90                   	nop
 1d9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

000001e0 <stat>:

int
stat(const char *n, struct stat *st)
{
 1e0:	55                   	push   %ebp
 1e1:	89 e5                	mov    %esp,%ebp
 1e3:	56                   	push   %esi
 1e4:	53                   	push   %ebx
 1e5:	83 ec 10             	sub    $0x10,%esp
  int fd;
  int r;

  fd = open(n, O_RDONLY);
 1e8:	8b 45 08             	mov    0x8(%ebp),%eax
 1eb:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
 1f2:	00 
 1f3:	89 04 24             	mov    %eax,(%esp)
 1f6:	e8 d7 00 00 00       	call   2d2 <open>
  if(fd < 0)
 1fb:	85 c0                	test   %eax,%eax
  fd = open(n, O_RDONLY);
 1fd:	89 c3                	mov    %eax,%ebx
  if(fd < 0)
 1ff:	78 27                	js     228 <stat+0x48>
    return -1;
  r = fstat(fd, st);
 201:	8b 45 0c             	mov    0xc(%ebp),%eax
 204:	89 1c 24             	mov    %ebx,(%esp)
 207:	89 44 24 04          	mov    %eax,0x4(%esp)
 20b:	e8 da 00 00 00       	call   2ea <fstat>
  close(fd);
 210:	89 1c 24             	mov    %ebx,(%esp)
  r = fstat(fd, st);
 213:	89 c6                	mov    %eax,%esi
  close(fd);
 215:	e8 a0 00 00 00       	call   2ba <close>
  return r;
 21a:	89 f0                	mov    %esi,%eax
}
 21c:	83 c4 10             	add    $0x10,%esp
 21f:	5b                   	pop    %ebx
 220:	5e                   	pop    %esi
 221:	5d                   	pop    %ebp
 222:	c3                   	ret    
 223:	90                   	nop
 224:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    return -1;
 228:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
 22d:	eb ed                	jmp    21c <stat+0x3c>
 22f:	90                   	nop

00000230 <atoi>:

int
atoi(const char *s)
{
 230:	55                   	push   %ebp
 231:	89 e5                	mov    %esp,%ebp
 233:	8b 4d 08             	mov    0x8(%ebp),%ecx
 236:	53                   	push   %ebx
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
 237:	0f be 11             	movsbl (%ecx),%edx
 23a:	8d 42 d0             	lea    -0x30(%edx),%eax
 23d:	3c 09                	cmp    $0x9,%al
  n = 0;
 23f:	b8 00 00 00 00       	mov    $0x0,%eax
  while('0' <= *s && *s <= '9')
 244:	77 17                	ja     25d <atoi+0x2d>
 246:	66 90                	xchg   %ax,%ax
    n = n*10 + *s++ - '0';
 248:	83 c1 01             	add    $0x1,%ecx
 24b:	8d 04 80             	lea    (%eax,%eax,4),%eax
 24e:	8d 44 42 d0          	lea    -0x30(%edx,%eax,2),%eax
  while('0' <= *s && *s <= '9')
 252:	0f be 11             	movsbl (%ecx),%edx
 255:	8d 5a d0             	lea    -0x30(%edx),%ebx
 258:	80 fb 09             	cmp    $0x9,%bl
 25b:	76 eb                	jbe    248 <atoi+0x18>
  return n;
}
 25d:	5b                   	pop    %ebx
 25e:	5d                   	pop    %ebp
 25f:	c3                   	ret    

00000260 <memmove>:

void*
memmove(void *vdst, const void *vsrc, int n)
{
 260:	55                   	push   %ebp
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
 261:	31 d2                	xor    %edx,%edx
{
 263:	89 e5                	mov    %esp,%ebp
 265:	56                   	push   %esi
 266:	8b 45 08             	mov    0x8(%ebp),%eax
 269:	53                   	push   %ebx
 26a:	8b 5d 10             	mov    0x10(%ebp),%ebx
 26d:	8b 75 0c             	mov    0xc(%ebp),%esi
  while(n-- > 0)
 270:	85 db                	test   %ebx,%ebx
 272:	7e 12                	jle    286 <memmove+0x26>
 274:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    *dst++ = *src++;
 278:	0f b6 0c 16          	movzbl (%esi,%edx,1),%ecx
 27c:	88 0c 10             	mov    %cl,(%eax,%edx,1)
 27f:	83 c2 01             	add    $0x1,%edx
  while(n-- > 0)
 282:	39 da                	cmp    %ebx,%edx
 284:	75 f2                	jne    278 <memmove+0x18>
  return vdst;
}
 286:	5b                   	pop    %ebx
 287:	5e                   	pop    %esi
 288:	5d                   	pop    %ebp
 289:	c3                   	ret    

0000028a <fork>:
 28a:	b8 01 00 00 00       	mov    $0x1,%eax
 28f:	cd 40                	int    $0x40
 291:	c3                   	ret    

00000292 <exit>:
 292:	b8 02 00 00 00       	mov    $0x2,%eax
 297:	cd 40                	int    $0x40
 299:	c3                   	ret    

0000029a <wait>:
 29a:	b8 03 00 00 00       	mov    $0x3,%eax
 29f:	cd 40                	int    $0x40
 2a1:	c3                   	ret    

000002a2 <pipe>:
 2a2:	b8 04 00 00 00       	mov    $0x4,%eax
 2a7:	cd 40                	int    $0x40
 2a9:	c3                   	ret    

000002aa <read>:
 2aa:	b8 05 00 00 00       	mov    $0x5,%eax
 2af:	cd 40                	int    $0x40
 2b1:	c3                   	ret    

000002b2 <write>:
 2b2:	b8 10 00 00 00       	mov    $0x10,%eax
 2b7:	cd 40                	int    $0x40
 2b9:	c3                   	ret    

000002ba <close>:
 2ba:	b8 15 00 00 00       	mov    $0x15,%eax
 2bf:	cd 40                	int    $0x40
 2c1:	c3                   	ret    

000002c2 <kill>:
 2c2:	b8 06 00 00 00       	mov    $0x6,%eax
 2c7:	cd 40                	int    $0x40
 2c9:	c3                   	ret    

000002ca <exec>:
 2ca:	b8 07 00 00 00       	mov    $0x7,%eax
 2cf:	cd 40                	int    $0x40
 2d1:	c3                   	ret    

000002d2 <open>:
 2d2:	b8 0f 00 00 00       	mov    $0xf,%eax
 2d7:	cd 40                	int    $0x40
 2d9:	c3                   	ret    

000002da <mknod>:
 2da:	b8 11 00 00 00       	mov    $0x11,%eax
 2df:	cd 40                	int    $0x40
 2e1:	c3                   	ret    

000002e2 <unlink>:
 2e2:	b8 12 00 00 00       	mov    $0x12,%eax
 2e7:	cd 40                	int    $0x40
 2e9:	c3                   	ret    

000002ea <fstat>:
 2ea:	b8 08 00 00 00       	mov    $0x8,%eax
 2ef:	cd 40                	int    $0x40
 2f1:	c3                   	ret    

000002f2 <link>:
 2f2:	b8 13 00 00 00       	mov    $0x13,%eax
 2f7:	cd 40                	int    $0x40
 2f9:	c3                   	ret    

000002fa <mkdir>:
 2fa:	b8 14 00 00 00       	mov    $0x14,%eax
 2ff:	cd 40                	int    $0x40
 301:	c3                   	ret    

00000302 <chdir>:
 302:	b8 09 00 00 00       	mov    $0x9,%eax
 307:	cd 40                	int    $0x40
 309:	c3                   	ret    

0000030a <dup>:
 30a:	b8 0a 00 00 00       	mov    $0xa,%eax
 30f:	cd 40                	int    $0x40
 311:	c3                   	ret    

00000312 <getpid>:
 312:	b8 0b 00 00 00       	mov    $0xb,%eax
 317:	cd 40                	int    $0x40
 319:	c3                   	ret    

0000031a <sbrk>:
 31a:	b8 0c 00 00 00       	mov    $0xc,%eax
 31f:	cd 40                	int    $0x40
 321:	c3                   	ret    

00000322 <sleep>:
 322:	b8 0d 00 00 00       	mov    $0xd,%eax
 327:	cd 40                	int    $0x40
 329:	c3                   	ret    

0000032a <uptime>:
 32a:	b8 0e 00 00 00       	mov    $0xe,%eax
 32f:	cd 40                	int    $0x40
 331:	c3                   	ret    
 332:	66 90                	xchg   %ax,%ax
 334:	66 90                	xchg   %ax,%ax
 336:	66 90                	xchg   %ax,%ax
 338:	66 90                	xchg   %ax,%ax
 33a:	66 90                	xchg   %ax,%ax
 33c:	66 90                	xchg   %ax,%ax
 33e:	66 90                	xchg   %ax,%ax

00000340 <printint>:
  write(fd, &c, 1);
}

static void
printint(int fd, int xx, int base, int sgn)
{
 340:	55                   	push   %ebp
 341:	89 e5                	mov    %esp,%ebp
 343:	57                   	push   %edi
 344:	56                   	push   %esi
 345:	89 c6                	mov    %eax,%esi
 347:	53                   	push   %ebx
 348:	83 ec 4c             	sub    $0x4c,%esp
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
 34b:	8b 5d 08             	mov    0x8(%ebp),%ebx
 34e:	85 db                	test   %ebx,%ebx
 350:	74 09                	je     35b <printint+0x1b>
 352:	89 d0                	mov    %edx,%eax
 354:	c1 e8 1f             	shr    $0x1f,%eax
 357:	84 c0                	test   %al,%al
 359:	75 75                	jne    3d0 <printint+0x90>
    neg = 1;
    x = -xx;
  } else {
    x = xx;
 35b:	89 d0                	mov    %edx,%eax
  neg = 0;
 35d:	c7 45 c4 00 00 00 00 	movl   $0x0,-0x3c(%ebp)
 364:	89 75 c0             	mov    %esi,-0x40(%ebp)
  }

  i = 0;
 367:	31 ff                	xor    %edi,%edi
 369:	89 ce                	mov    %ecx,%esi
 36b:	8d 5d d7             	lea    -0x29(%ebp),%ebx
 36e:	eb 02                	jmp    372 <printint+0x32>
  do{
    buf[i++] = digits[x % base];
 370:	89 cf                	mov    %ecx,%edi
 372:	31 d2                	xor    %edx,%edx
 374:	f7 f6                	div    %esi
 376:	8d 4f 01             	lea    0x1(%edi),%ecx
 379:	0f b6 92 61 07 00 00 	movzbl 0x761(%edx),%edx
  }while((x /= base) != 0);
 380:	85 c0                	test   %eax,%eax
    buf[i++] = digits[x % base];
 382:	88 14 0b             	mov    %dl,(%ebx,%ecx,1)
  }while((x /= base) != 0);
 385:	75 e9                	jne    370 <printint+0x30>
  if(neg)
 387:	8b 55 c4             	mov    -0x3c(%ebp),%edx
    buf[i++] = digits[x % base];
 38a:	89 c8                	mov    %ecx,%eax
 38c:	8b 75 c0             	mov    -0x40(%ebp),%esi
  if(neg)
 38f:	85 d2                	test   %edx,%edx
 391:	74 08                	je     39b <printint+0x5b>
    buf[i++] = '-';
 393:	8d 4f 02             	lea    0x2(%edi),%ecx
 396:	c6 44 05 d8 2d       	movb   $0x2d,-0x28(%ebp,%eax,1)

  while(--i >= 0)
 39b:	8d 79 ff             	lea    -0x1(%ecx),%edi
 39e:	66 90                	xchg   %ax,%ax
 3a0:	0f b6 44 3d d8       	movzbl -0x28(%ebp,%edi,1),%eax
 3a5:	83 ef 01             	sub    $0x1,%edi
  write(fd, &c, 1);
 3a8:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 3af:	00 
 3b0:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 3b4:	89 34 24             	mov    %esi,(%esp)
 3b7:	88 45 d7             	mov    %al,-0x29(%ebp)
 3ba:	e8 f3 fe ff ff       	call   2b2 <write>
  while(--i >= 0)
 3bf:	83 ff ff             	cmp    $0xffffffff,%edi
 3c2:	75 dc                	jne    3a0 <printint+0x60>
    putc(fd, buf[i]);
}
 3c4:	83 c4 4c             	add    $0x4c,%esp
 3c7:	5b                   	pop    %ebx
 3c8:	5e                   	pop    %esi
 3c9:	5f                   	pop    %edi
 3ca:	5d                   	pop    %ebp
 3cb:	c3                   	ret    
 3cc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    x = -xx;
 3d0:	89 d0                	mov    %edx,%eax
 3d2:	f7 d8                	neg    %eax
    neg = 1;
 3d4:	c7 45 c4 01 00 00 00 	movl   $0x1,-0x3c(%ebp)
 3db:	eb 87                	jmp    364 <printint+0x24>
 3dd:	8d 76 00             	lea    0x0(%esi),%esi

000003e0 <printf>:

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, const char *fmt, ...)
{
 3e0:	55                   	push   %ebp
 3e1:	89 e5                	mov    %esp,%ebp
 3e3:	57                   	push   %edi
  char *s;
  int c, i, state;
  uint *ap;

  state = 0;
 3e4:	31 ff                	xor    %edi,%edi
{
 3e6:	56                   	push   %esi
 3e7:	53                   	push   %ebx
 3e8:	83 ec 3c             	sub    $0x3c,%esp
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
 3eb:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  ap = (uint*)(void*)&fmt + 1;
 3ee:	8d 45 10             	lea    0x10(%ebp),%eax
{
 3f1:	8b 75 08             	mov    0x8(%ebp),%esi
  ap = (uint*)(void*)&fmt + 1;
 3f4:	89 45 d4             	mov    %eax,-0x2c(%ebp)
  for(i = 0; fmt[i]; i++){
 3f7:	0f b6 13             	movzbl (%ebx),%edx
 3fa:	83 c3 01             	add    $0x1,%ebx
 3fd:	84 d2                	test   %dl,%dl
 3ff:	75 39                	jne    43a <printf+0x5a>
 401:	e9 c2 00 00 00       	jmp    4c8 <printf+0xe8>
 406:	66 90                	xchg   %ax,%ax
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
 408:	83 fa 25             	cmp    $0x25,%edx
 40b:	0f 84 bf 00 00 00    	je     4d0 <printf+0xf0>
  write(fd, &c, 1);
 411:	8d 45 e2             	lea    -0x1e(%ebp),%eax
 414:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 41b:	00 
 41c:	89 44 24 04          	mov    %eax,0x4(%esp)
 420:	89 34 24             	mov    %esi,(%esp)
        state = '%';
      } else {
        putc(fd, c);
 423:	88 55 e2             	mov    %dl,-0x1e(%ebp)
  write(fd, &c, 1);
 426:	e8 87 fe ff ff       	call   2b2 <write>
 42b:	83 c3 01             	add    $0x1,%ebx
  for(i = 0; fmt[i]; i++){
 42e:	0f b6 53 ff          	movzbl -0x1(%ebx),%edx
 432:	84 d2                	test   %dl,%dl
 434:	0f 84 8e 00 00 00    	je     4c8 <printf+0xe8>
    if(state == 0){
 43a:	85 ff                	test   %edi,%edi
    c = fmt[i] & 0xff;
 43c:	0f be c2             	movsbl %dl,%eax
    if(state == 0){
 43f:	74 c7                	je     408 <printf+0x28>
      }
    } else if(state == '%'){
 441:	83 ff 25             	cmp    $0x25,%edi
 444:	75 e5                	jne    42b <printf+0x4b>
      if(c == 'd'){
 446:	83 fa 64             	cmp    $0x64,%edx
 449:	0f 84 31 01 00 00    	je     580 <printf+0x1a0>
        printint(fd, *ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
 44f:	25 f7 00 00 00       	and    $0xf7,%eax
 454:	83 f8 70             	cmp    $0x70,%eax
 457:	0f 84 83 00 00 00    	je     4e0 <printf+0x100>
        printint(fd, *ap, 16, 0);
        ap++;
      } else if(c == 's'){
 45d:	83 fa 73             	cmp    $0x73,%edx
 460:	0f 84 a2 00 00 00    	je     508 <printf+0x128>
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
        }
      } else if(c == 'c'){
 466:	83 fa 63             	cmp    $0x63,%edx
 469:	0f 84 35 01 00 00    	je     5a4 <printf+0x1c4>
        putc(fd, *ap);
        ap++;
      } else if(c == '%'){
 46f:	83 fa 25             	cmp    $0x25,%edx
 472:	0f 84 e0 00 00 00    	je     558 <printf+0x178>
  write(fd, &c, 1);
 478:	8d 45 e6             	lea    -0x1a(%ebp),%eax
 47b:	83 c3 01             	add    $0x1,%ebx
 47e:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 485:	00 
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
 486:	31 ff                	xor    %edi,%edi
  write(fd, &c, 1);
 488:	89 44 24 04          	mov    %eax,0x4(%esp)
 48c:	89 34 24             	mov    %esi,(%esp)
 48f:	89 55 d0             	mov    %edx,-0x30(%ebp)
 492:	c6 45 e6 25          	movb   $0x25,-0x1a(%ebp)
 496:	e8 17 fe ff ff       	call   2b2 <write>
        putc(fd, c);
 49b:	8b 55 d0             	mov    -0x30(%ebp),%edx
  write(fd, &c, 1);
 49e:	8d 45 e7             	lea    -0x19(%ebp),%eax
 4a1:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 4a8:	00 
 4a9:	89 44 24 04          	mov    %eax,0x4(%esp)
 4ad:	89 34 24             	mov    %esi,(%esp)
        putc(fd, c);
 4b0:	88 55 e7             	mov    %dl,-0x19(%ebp)
  write(fd, &c, 1);
 4b3:	e8 fa fd ff ff       	call   2b2 <write>
  for(i = 0; fmt[i]; i++){
 4b8:	0f b6 53 ff          	movzbl -0x1(%ebx),%edx
 4bc:	84 d2                	test   %dl,%dl
 4be:	0f 85 76 ff ff ff    	jne    43a <printf+0x5a>
 4c4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    }
  }
}
 4c8:	83 c4 3c             	add    $0x3c,%esp
 4cb:	5b                   	pop    %ebx
 4cc:	5e                   	pop    %esi
 4cd:	5f                   	pop    %edi
 4ce:	5d                   	pop    %ebp
 4cf:	c3                   	ret    
        state = '%';
 4d0:	bf 25 00 00 00       	mov    $0x25,%edi
 4d5:	e9 51 ff ff ff       	jmp    42b <printf+0x4b>
 4da:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        printint(fd, *ap, 16, 0);
 4e0:	8b 45 d4             	mov    -0x2c(%ebp),%eax
 4e3:	b9 10 00 00 00       	mov    $0x10,%ecx
      state = 0;
 4e8:	31 ff                	xor    %edi,%edi
        printint(fd, *ap, 16, 0);
 4ea:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
 4f1:	8b 10                	mov    (%eax),%edx
 4f3:	89 f0                	mov    %esi,%eax
 4f5:	e8 46 fe ff ff       	call   340 <printint>
        ap++;
 4fa:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
 4fe:	e9 28 ff ff ff       	jmp    42b <printf+0x4b>
 503:	90                   	nop
 504:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
        s = (char*)*ap;
 508:	8b 45 d4             	mov    -0x2c(%ebp),%eax
        ap++;
 50b:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
        s = (char*)*ap;
 50f:	8b 38                	mov    (%eax),%edi
          s = "(null)";
 511:	b8 5a 07 00 00       	mov    $0x75a,%eax
 516:	85 ff                	test   %edi,%edi
 518:	0f 44 f8             	cmove  %eax,%edi
        while(*s != 0){
 51b:	0f b6 07             	movzbl (%edi),%eax
 51e:	84 c0                	test   %al,%al
 520:	74 2a                	je     54c <printf+0x16c>
 522:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 528:	88 45 e3             	mov    %al,-0x1d(%ebp)
  write(fd, &c, 1);
 52b:	8d 45 e3             	lea    -0x1d(%ebp),%eax
          s++;
 52e:	83 c7 01             	add    $0x1,%edi
  write(fd, &c, 1);
 531:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 538:	00 
 539:	89 44 24 04          	mov    %eax,0x4(%esp)
 53d:	89 34 24             	mov    %esi,(%esp)
 540:	e8 6d fd ff ff       	call   2b2 <write>
        while(*s != 0){
 545:	0f b6 07             	movzbl (%edi),%eax
 548:	84 c0                	test   %al,%al
 54a:	75 dc                	jne    528 <printf+0x148>
      state = 0;
 54c:	31 ff                	xor    %edi,%edi
 54e:	e9 d8 fe ff ff       	jmp    42b <printf+0x4b>
 553:	90                   	nop
 554:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  write(fd, &c, 1);
 558:	8d 45 e5             	lea    -0x1b(%ebp),%eax
      state = 0;
 55b:	31 ff                	xor    %edi,%edi
  write(fd, &c, 1);
 55d:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 564:	00 
 565:	89 44 24 04          	mov    %eax,0x4(%esp)
 569:	89 34 24             	mov    %esi,(%esp)
 56c:	c6 45 e5 25          	movb   $0x25,-0x1b(%ebp)
 570:	e8 3d fd ff ff       	call   2b2 <write>
 575:	e9 b1 fe ff ff       	jmp    42b <printf+0x4b>
 57a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        printint(fd, *ap, 10, 1);
 580:	8b 45 d4             	mov    -0x2c(%ebp),%eax
 583:	b9 0a 00 00 00       	mov    $0xa,%ecx
      state = 0;
 588:	66 31 ff             	xor    %di,%di
        printint(fd, *ap, 10, 1);
 58b:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 592:	8b 10                	mov    (%eax),%edx
 594:	89 f0                	mov    %esi,%eax
 596:	e8 a5 fd ff ff       	call   340 <printint>
        ap++;
 59b:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
 59f:	e9 87 fe ff ff       	jmp    42b <printf+0x4b>
        putc(fd, *ap);
 5a4:	8b 45 d4             	mov    -0x2c(%ebp),%eax
      state = 0;
 5a7:	31 ff                	xor    %edi,%edi
        putc(fd, *ap);
 5a9:	8b 00                	mov    (%eax),%eax
  write(fd, &c, 1);
 5ab:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 5b2:	00 
 5b3:	89 34 24             	mov    %esi,(%esp)
        putc(fd, *ap);
 5b6:	88 45 e4             	mov    %al,-0x1c(%ebp)
  write(fd, &c, 1);
 5b9:	8d 45 e4             	lea    -0x1c(%ebp),%eax
 5bc:	89 44 24 04          	mov    %eax,0x4(%esp)
 5c0:	e8 ed fc ff ff       	call   2b2 <write>
        ap++;
 5c5:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
 5c9:	e9 5d fe ff ff       	jmp    42b <printf+0x4b>
 5ce:	66 90                	xchg   %ax,%ax

000005d0 <free>:
static Header base;
static Header *freep;

void
free(void *ap)
{
 5d0:	55                   	push   %ebp
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 5d1:	a1 dc 09 00 00       	mov    0x9dc,%eax
{
 5d6:	89 e5                	mov    %esp,%ebp
 5d8:	57                   	push   %edi
 5d9:	56                   	push   %esi
 5da:	53                   	push   %ebx
 5db:	8b 5d 08             	mov    0x8(%ebp),%ebx
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 5de:	8b 08                	mov    (%eax),%ecx
  bp = (Header*)ap - 1;
 5e0:	8d 53 f8             	lea    -0x8(%ebx),%edx
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 5e3:	39 d0                	cmp    %edx,%eax
 5e5:	72 11                	jb     5f8 <free+0x28>
 5e7:	90                   	nop
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 5e8:	39 c8                	cmp    %ecx,%eax
 5ea:	72 04                	jb     5f0 <free+0x20>
 5ec:	39 ca                	cmp    %ecx,%edx
 5ee:	72 10                	jb     600 <free+0x30>
 5f0:	89 c8                	mov    %ecx,%eax
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 5f2:	39 d0                	cmp    %edx,%eax
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 5f4:	8b 08                	mov    (%eax),%ecx
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 5f6:	73 f0                	jae    5e8 <free+0x18>
 5f8:	39 ca                	cmp    %ecx,%edx
 5fa:	72 04                	jb     600 <free+0x30>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 5fc:	39 c8                	cmp    %ecx,%eax
 5fe:	72 f0                	jb     5f0 <free+0x20>
      break;
  if(bp + bp->s.size == p->s.ptr){
 600:	8b 73 fc             	mov    -0x4(%ebx),%esi
 603:	8d 3c f2             	lea    (%edx,%esi,8),%edi
 606:	39 cf                	cmp    %ecx,%edi
 608:	74 1e                	je     628 <free+0x58>
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
 60a:	89 4b f8             	mov    %ecx,-0x8(%ebx)
  if(p + p->s.size == bp){
 60d:	8b 48 04             	mov    0x4(%eax),%ecx
 610:	8d 34 c8             	lea    (%eax,%ecx,8),%esi
 613:	39 f2                	cmp    %esi,%edx
 615:	74 28                	je     63f <free+0x6f>
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
 617:	89 10                	mov    %edx,(%eax)
  freep = p;
 619:	a3 dc 09 00 00       	mov    %eax,0x9dc
}
 61e:	5b                   	pop    %ebx
 61f:	5e                   	pop    %esi
 620:	5f                   	pop    %edi
 621:	5d                   	pop    %ebp
 622:	c3                   	ret    
 623:	90                   	nop
 624:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    bp->s.size += p->s.ptr->s.size;
 628:	03 71 04             	add    0x4(%ecx),%esi
 62b:	89 73 fc             	mov    %esi,-0x4(%ebx)
    bp->s.ptr = p->s.ptr->s.ptr;
 62e:	8b 08                	mov    (%eax),%ecx
 630:	8b 09                	mov    (%ecx),%ecx
 632:	89 4b f8             	mov    %ecx,-0x8(%ebx)
  if(p + p->s.size == bp){
 635:	8b 48 04             	mov    0x4(%eax),%ecx
 638:	8d 34 c8             	lea    (%eax,%ecx,8),%esi
 63b:	39 f2                	cmp    %esi,%edx
 63d:	75 d8                	jne    617 <free+0x47>
    p->s.size += bp->s.size;
 63f:	03 4b fc             	add    -0x4(%ebx),%ecx
  freep = p;
 642:	a3 dc 09 00 00       	mov    %eax,0x9dc
    p->s.size += bp->s.size;
 647:	89 48 04             	mov    %ecx,0x4(%eax)
    p->s.ptr = bp->s.ptr;
 64a:	8b 53 f8             	mov    -0x8(%ebx),%edx
 64d:	89 10                	mov    %edx,(%eax)
}
 64f:	5b                   	pop    %ebx
 650:	5e                   	pop    %esi
 651:	5f                   	pop    %edi
 652:	5d                   	pop    %ebp
 653:	c3                   	ret    
 654:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 65a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

00000660 <malloc>:
  return freep;
}

void*
malloc(uint nbytes)
{
 660:	55                   	push   %ebp
 661:	89 e5                	mov    %esp,%ebp
 663:	57                   	push   %edi
 664:	56                   	push   %esi
 665:	53                   	push   %ebx
 666:	83 ec 1c             	sub    $0x1c,%esp
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 669:	8b 45 08             	mov    0x8(%ebp),%eax
  if((prevp = freep) == 0){
 66c:	8b 1d dc 09 00 00    	mov    0x9dc,%ebx
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 672:	8d 48 07             	lea    0x7(%eax),%ecx
 675:	c1 e9 03             	shr    $0x3,%ecx
  if((prevp = freep) == 0){
 678:	85 db                	test   %ebx,%ebx
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 67a:	8d 71 01             	lea    0x1(%ecx),%esi
  if((prevp = freep) == 0){
 67d:	0f 84 9b 00 00 00    	je     71e <malloc+0xbe>
 683:	8b 13                	mov    (%ebx),%edx
 685:	8b 7a 04             	mov    0x4(%edx),%edi
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
 688:	39 fe                	cmp    %edi,%esi
 68a:	76 64                	jbe    6f0 <malloc+0x90>
 68c:	8d 04 f5 00 00 00 00 	lea    0x0(,%esi,8),%eax
  if(nu < 4096)
 693:	bb 00 80 00 00       	mov    $0x8000,%ebx
 698:	89 45 e4             	mov    %eax,-0x1c(%ebp)
 69b:	eb 0e                	jmp    6ab <malloc+0x4b>
 69d:	8d 76 00             	lea    0x0(%esi),%esi
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
 6a0:	8b 02                	mov    (%edx),%eax
    if(p->s.size >= nunits){
 6a2:	8b 78 04             	mov    0x4(%eax),%edi
 6a5:	39 fe                	cmp    %edi,%esi
 6a7:	76 4f                	jbe    6f8 <malloc+0x98>
 6a9:	89 c2                	mov    %eax,%edx
        p->s.size = nunits;
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
 6ab:	3b 15 dc 09 00 00    	cmp    0x9dc,%edx
 6b1:	75 ed                	jne    6a0 <malloc+0x40>
  if(nu < 4096)
 6b3:	8b 45 e4             	mov    -0x1c(%ebp),%eax
 6b6:	81 fe 00 10 00 00    	cmp    $0x1000,%esi
 6bc:	bf 00 10 00 00       	mov    $0x1000,%edi
 6c1:	0f 43 fe             	cmovae %esi,%edi
 6c4:	0f 42 c3             	cmovb  %ebx,%eax
  p = sbrk(nu * sizeof(Header));
 6c7:	89 04 24             	mov    %eax,(%esp)
 6ca:	e8 4b fc ff ff       	call   31a <sbrk>
  if(p == (char*)-1)
 6cf:	83 f8 ff             	cmp    $0xffffffff,%eax
 6d2:	74 18                	je     6ec <malloc+0x8c>
  hp->s.size = nu;
 6d4:	89 78 04             	mov    %edi,0x4(%eax)
  free((void*)(hp + 1));
 6d7:	83 c0 08             	add    $0x8,%eax
 6da:	89 04 24             	mov    %eax,(%esp)
 6dd:	e8 ee fe ff ff       	call   5d0 <free>
  return freep;
 6e2:	8b 15 dc 09 00 00    	mov    0x9dc,%edx
      if((p = morecore(nunits)) == 0)
 6e8:	85 d2                	test   %edx,%edx
 6ea:	75 b4                	jne    6a0 <malloc+0x40>
        return 0;
 6ec:	31 c0                	xor    %eax,%eax
 6ee:	eb 20                	jmp    710 <malloc+0xb0>
    if(p->s.size >= nunits){
 6f0:	89 d0                	mov    %edx,%eax
 6f2:	89 da                	mov    %ebx,%edx
 6f4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      if(p->s.size == nunits)
 6f8:	39 fe                	cmp    %edi,%esi
 6fa:	74 1c                	je     718 <malloc+0xb8>
        p->s.size -= nunits;
 6fc:	29 f7                	sub    %esi,%edi
 6fe:	89 78 04             	mov    %edi,0x4(%eax)
        p += p->s.size;
 701:	8d 04 f8             	lea    (%eax,%edi,8),%eax
        p->s.size = nunits;
 704:	89 70 04             	mov    %esi,0x4(%eax)
      freep = prevp;
 707:	89 15 dc 09 00 00    	mov    %edx,0x9dc
      return (void*)(p + 1);
 70d:	83 c0 08             	add    $0x8,%eax
  }
}
 710:	83 c4 1c             	add    $0x1c,%esp
 713:	5b                   	pop    %ebx
 714:	5e                   	pop    %esi
 715:	5f                   	pop    %edi
 716:	5d                   	pop    %ebp
 717:	c3                   	ret    
        prevp->s.ptr = p->s.ptr;
 718:	8b 08                	mov    (%eax),%ecx
 71a:	89 0a                	mov    %ecx,(%edx)
 71c:	eb e9                	jmp    707 <malloc+0xa7>
    base.s.ptr = freep = prevp = &base;
 71e:	c7 05 dc 09 00 00 e0 	movl   $0x9e0,0x9dc
 725:	09 00 00 
    base.s.size = 0;
 728:	ba e0 09 00 00       	mov    $0x9e0,%edx
    base.s.ptr = freep = prevp = &base;
 72d:	c7 05 e0 09 00 00 e0 	movl   $0x9e0,0x9e0
 734:	09 00 00 
    base.s.size = 0;
 737:	c7 05 e4 09 00 00 00 	movl   $0x0,0x9e4
 73e:	00 00 00 
 741:	e9 46 ff ff ff       	jmp    68c <malloc+0x2c>
