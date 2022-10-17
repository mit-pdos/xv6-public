
_init:     file format elf32-i386


Disassembly of section .text:

00000000 <main>:
#include "fcntl.h"

char *argv[] = {"sh", 0};

int main(void)
{
   0:	55                   	push   %ebp
   1:	89 e5                	mov    %esp,%ebp
   3:	53                   	push   %ebx
   4:	83 e4 f0             	and    $0xfffffff0,%esp
   7:	83 ec 10             	sub    $0x10,%esp
  int pid, wpid;

  if (open("console", O_RDWR) < 0)
   a:	c7 44 24 04 02 00 00 	movl   $0x2,0x4(%esp)
  11:	00 
  12:	c7 04 24 f6 07 00 00 	movl   $0x7f6,(%esp)
  19:	e8 64 03 00 00       	call   382 <open>
  1e:	85 c0                	test   %eax,%eax
  20:	0f 88 c0 00 00 00    	js     e6 <main+0xe6>
  {
    mknod("console", 1, 1);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  26:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  2d:	e8 88 03 00 00       	call   3ba <dup>
  dup(0); // stderr
  32:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  39:	e8 7c 03 00 00       	call   3ba <dup>
  3e:	66 90                	xchg   %ax,%ax

  for (;;)
  {
    printf(1, "init: starting sh\n");
  40:	c7 44 24 04 fe 07 00 	movl   $0x7fe,0x4(%esp)
  47:	00 
  48:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
  4f:	e8 3c 04 00 00       	call   490 <printf>
    pid = fork();
  54:	e8 e1 02 00 00       	call   33a <fork>
    if (pid < 0)
  59:	85 c0                	test   %eax,%eax
    pid = fork();
  5b:	89 c3                	mov    %eax,%ebx
    if (pid < 0)
  5d:	78 33                	js     92 <main+0x92>
  5f:	90                   	nop
    {
      printf(1, "init: fork failed\n");
      exit(0);
    }
    if (pid == 0)
  60:	74 50                	je     b2 <main+0xb2>
  62:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    {
      exec("sh", argv);
      printf(1, "init: exec sh failed\n");
      exit(0);
    }
    while ((wpid = wait(0)) >= 0 && wpid != pid)
  68:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  6f:	e8 d6 02 00 00       	call   34a <wait>
  74:	85 c0                	test   %eax,%eax
  76:	78 c8                	js     40 <main+0x40>
  78:	39 d8                	cmp    %ebx,%eax
  7a:	74 c4                	je     40 <main+0x40>
      printf(1, "zombie!\n");
  7c:	c7 44 24 04 3d 08 00 	movl   $0x83d,0x4(%esp)
  83:	00 
  84:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
  8b:	e8 00 04 00 00       	call   490 <printf>
  90:	eb d6                	jmp    68 <main+0x68>
      printf(1, "init: fork failed\n");
  92:	c7 44 24 04 11 08 00 	movl   $0x811,0x4(%esp)
  99:	00 
  9a:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
  a1:	e8 ea 03 00 00       	call   490 <printf>
      exit(0);
  a6:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  ad:	e8 90 02 00 00       	call   342 <exit>
      exec("sh", argv);
  b2:	c7 44 24 04 c4 0a 00 	movl   $0xac4,0x4(%esp)
  b9:	00 
  ba:	c7 04 24 24 08 00 00 	movl   $0x824,(%esp)
  c1:	e8 b4 02 00 00       	call   37a <exec>
      printf(1, "init: exec sh failed\n");
  c6:	c7 44 24 04 27 08 00 	movl   $0x827,0x4(%esp)
  cd:	00 
  ce:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
  d5:	e8 b6 03 00 00       	call   490 <printf>
      exit(0);
  da:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  e1:	e8 5c 02 00 00       	call   342 <exit>
    mknod("console", 1, 1);
  e6:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
  ed:	00 
  ee:	c7 44 24 04 01 00 00 	movl   $0x1,0x4(%esp)
  f5:	00 
  f6:	c7 04 24 f6 07 00 00 	movl   $0x7f6,(%esp)
  fd:	e8 88 02 00 00       	call   38a <mknod>
    open("console", O_RDWR);
 102:	c7 44 24 04 02 00 00 	movl   $0x2,0x4(%esp)
 109:	00 
 10a:	c7 04 24 f6 07 00 00 	movl   $0x7f6,(%esp)
 111:	e8 6c 02 00 00       	call   382 <open>
 116:	e9 0b ff ff ff       	jmp    26 <main+0x26>
 11b:	66 90                	xchg   %ax,%ax
 11d:	66 90                	xchg   %ax,%ax
 11f:	90                   	nop

00000120 <strcpy>:
#include "user.h"
#include "x86.h"

char*
strcpy(char *s, const char *t)
{
 120:	55                   	push   %ebp
 121:	89 e5                	mov    %esp,%ebp
 123:	8b 45 08             	mov    0x8(%ebp),%eax
 126:	8b 4d 0c             	mov    0xc(%ebp),%ecx
 129:	53                   	push   %ebx
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
 12a:	89 c2                	mov    %eax,%edx
 12c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 130:	83 c1 01             	add    $0x1,%ecx
 133:	0f b6 59 ff          	movzbl -0x1(%ecx),%ebx
 137:	83 c2 01             	add    $0x1,%edx
 13a:	84 db                	test   %bl,%bl
 13c:	88 5a ff             	mov    %bl,-0x1(%edx)
 13f:	75 ef                	jne    130 <strcpy+0x10>
    ;
  return os;
}
 141:	5b                   	pop    %ebx
 142:	5d                   	pop    %ebp
 143:	c3                   	ret    
 144:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 14a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

00000150 <strcmp>:

int
strcmp(const char *p, const char *q)
{
 150:	55                   	push   %ebp
 151:	89 e5                	mov    %esp,%ebp
 153:	8b 55 08             	mov    0x8(%ebp),%edx
 156:	53                   	push   %ebx
 157:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  while(*p && *p == *q)
 15a:	0f b6 02             	movzbl (%edx),%eax
 15d:	84 c0                	test   %al,%al
 15f:	74 2d                	je     18e <strcmp+0x3e>
 161:	0f b6 19             	movzbl (%ecx),%ebx
 164:	38 d8                	cmp    %bl,%al
 166:	74 0e                	je     176 <strcmp+0x26>
 168:	eb 2b                	jmp    195 <strcmp+0x45>
 16a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 170:	38 c8                	cmp    %cl,%al
 172:	75 15                	jne    189 <strcmp+0x39>
    p++, q++;
 174:	89 d9                	mov    %ebx,%ecx
 176:	83 c2 01             	add    $0x1,%edx
  while(*p && *p == *q)
 179:	0f b6 02             	movzbl (%edx),%eax
    p++, q++;
 17c:	8d 59 01             	lea    0x1(%ecx),%ebx
  while(*p && *p == *q)
 17f:	0f b6 49 01          	movzbl 0x1(%ecx),%ecx
 183:	84 c0                	test   %al,%al
 185:	75 e9                	jne    170 <strcmp+0x20>
 187:	31 c0                	xor    %eax,%eax
  return (uchar)*p - (uchar)*q;
 189:	29 c8                	sub    %ecx,%eax
}
 18b:	5b                   	pop    %ebx
 18c:	5d                   	pop    %ebp
 18d:	c3                   	ret    
 18e:	0f b6 09             	movzbl (%ecx),%ecx
  while(*p && *p == *q)
 191:	31 c0                	xor    %eax,%eax
 193:	eb f4                	jmp    189 <strcmp+0x39>
 195:	0f b6 cb             	movzbl %bl,%ecx
 198:	eb ef                	jmp    189 <strcmp+0x39>
 19a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

000001a0 <strlen>:

uint
strlen(const char *s)
{
 1a0:	55                   	push   %ebp
 1a1:	89 e5                	mov    %esp,%ebp
 1a3:	8b 4d 08             	mov    0x8(%ebp),%ecx
  int n;

  for(n = 0; s[n]; n++)
 1a6:	80 39 00             	cmpb   $0x0,(%ecx)
 1a9:	74 12                	je     1bd <strlen+0x1d>
 1ab:	31 d2                	xor    %edx,%edx
 1ad:	8d 76 00             	lea    0x0(%esi),%esi
 1b0:	83 c2 01             	add    $0x1,%edx
 1b3:	80 3c 11 00          	cmpb   $0x0,(%ecx,%edx,1)
 1b7:	89 d0                	mov    %edx,%eax
 1b9:	75 f5                	jne    1b0 <strlen+0x10>
    ;
  return n;
}
 1bb:	5d                   	pop    %ebp
 1bc:	c3                   	ret    
  for(n = 0; s[n]; n++)
 1bd:	31 c0                	xor    %eax,%eax
}
 1bf:	5d                   	pop    %ebp
 1c0:	c3                   	ret    
 1c1:	eb 0d                	jmp    1d0 <memset>
 1c3:	90                   	nop
 1c4:	90                   	nop
 1c5:	90                   	nop
 1c6:	90                   	nop
 1c7:	90                   	nop
 1c8:	90                   	nop
 1c9:	90                   	nop
 1ca:	90                   	nop
 1cb:	90                   	nop
 1cc:	90                   	nop
 1cd:	90                   	nop
 1ce:	90                   	nop
 1cf:	90                   	nop

000001d0 <memset>:

void*
memset(void *dst, int c, uint n)
{
 1d0:	55                   	push   %ebp
 1d1:	89 e5                	mov    %esp,%ebp
 1d3:	8b 55 08             	mov    0x8(%ebp),%edx
 1d6:	57                   	push   %edi
}

static inline void
stosb(void *addr, int data, int cnt)
{
  asm volatile("cld; rep stosb" :
 1d7:	8b 4d 10             	mov    0x10(%ebp),%ecx
 1da:	8b 45 0c             	mov    0xc(%ebp),%eax
 1dd:	89 d7                	mov    %edx,%edi
 1df:	fc                   	cld    
 1e0:	f3 aa                	rep stos %al,%es:(%edi)
  stosb(dst, c, n);
  return dst;
}
 1e2:	89 d0                	mov    %edx,%eax
 1e4:	5f                   	pop    %edi
 1e5:	5d                   	pop    %ebp
 1e6:	c3                   	ret    
 1e7:	89 f6                	mov    %esi,%esi
 1e9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

000001f0 <strchr>:

char*
strchr(const char *s, char c)
{
 1f0:	55                   	push   %ebp
 1f1:	89 e5                	mov    %esp,%ebp
 1f3:	8b 45 08             	mov    0x8(%ebp),%eax
 1f6:	53                   	push   %ebx
 1f7:	8b 55 0c             	mov    0xc(%ebp),%edx
  for(; *s; s++)
 1fa:	0f b6 18             	movzbl (%eax),%ebx
 1fd:	84 db                	test   %bl,%bl
 1ff:	74 1d                	je     21e <strchr+0x2e>
    if(*s == c)
 201:	38 d3                	cmp    %dl,%bl
 203:	89 d1                	mov    %edx,%ecx
 205:	75 0d                	jne    214 <strchr+0x24>
 207:	eb 17                	jmp    220 <strchr+0x30>
 209:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 210:	38 ca                	cmp    %cl,%dl
 212:	74 0c                	je     220 <strchr+0x30>
  for(; *s; s++)
 214:	83 c0 01             	add    $0x1,%eax
 217:	0f b6 10             	movzbl (%eax),%edx
 21a:	84 d2                	test   %dl,%dl
 21c:	75 f2                	jne    210 <strchr+0x20>
      return (char*)s;
  return 0;
 21e:	31 c0                	xor    %eax,%eax
}
 220:	5b                   	pop    %ebx
 221:	5d                   	pop    %ebp
 222:	c3                   	ret    
 223:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 229:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000230 <gets>:

char*
gets(char *buf, int max)
{
 230:	55                   	push   %ebp
 231:	89 e5                	mov    %esp,%ebp
 233:	57                   	push   %edi
 234:	56                   	push   %esi
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
 235:	31 f6                	xor    %esi,%esi
{
 237:	53                   	push   %ebx
 238:	83 ec 2c             	sub    $0x2c,%esp
    cc = read(0, &c, 1);
 23b:	8d 7d e7             	lea    -0x19(%ebp),%edi
  for(i=0; i+1 < max; ){
 23e:	eb 31                	jmp    271 <gets+0x41>
    cc = read(0, &c, 1);
 240:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 247:	00 
 248:	89 7c 24 04          	mov    %edi,0x4(%esp)
 24c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
 253:	e8 02 01 00 00       	call   35a <read>
    if(cc < 1)
 258:	85 c0                	test   %eax,%eax
 25a:	7e 1d                	jle    279 <gets+0x49>
      break;
    buf[i++] = c;
 25c:	0f b6 45 e7          	movzbl -0x19(%ebp),%eax
  for(i=0; i+1 < max; ){
 260:	89 de                	mov    %ebx,%esi
    buf[i++] = c;
 262:	8b 55 08             	mov    0x8(%ebp),%edx
    if(c == '\n' || c == '\r')
 265:	3c 0d                	cmp    $0xd,%al
    buf[i++] = c;
 267:	88 44 1a ff          	mov    %al,-0x1(%edx,%ebx,1)
    if(c == '\n' || c == '\r')
 26b:	74 0c                	je     279 <gets+0x49>
 26d:	3c 0a                	cmp    $0xa,%al
 26f:	74 08                	je     279 <gets+0x49>
  for(i=0; i+1 < max; ){
 271:	8d 5e 01             	lea    0x1(%esi),%ebx
 274:	3b 5d 0c             	cmp    0xc(%ebp),%ebx
 277:	7c c7                	jl     240 <gets+0x10>
      break;
  }
  buf[i] = '\0';
 279:	8b 45 08             	mov    0x8(%ebp),%eax
 27c:	c6 04 30 00          	movb   $0x0,(%eax,%esi,1)
  return buf;
}
 280:	83 c4 2c             	add    $0x2c,%esp
 283:	5b                   	pop    %ebx
 284:	5e                   	pop    %esi
 285:	5f                   	pop    %edi
 286:	5d                   	pop    %ebp
 287:	c3                   	ret    
 288:	90                   	nop
 289:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00000290 <stat>:

int
stat(const char *n, struct stat *st)
{
 290:	55                   	push   %ebp
 291:	89 e5                	mov    %esp,%ebp
 293:	56                   	push   %esi
 294:	53                   	push   %ebx
 295:	83 ec 10             	sub    $0x10,%esp
  int fd;
  int r;

  fd = open(n, O_RDONLY);
 298:	8b 45 08             	mov    0x8(%ebp),%eax
 29b:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
 2a2:	00 
 2a3:	89 04 24             	mov    %eax,(%esp)
 2a6:	e8 d7 00 00 00       	call   382 <open>
  if(fd < 0)
 2ab:	85 c0                	test   %eax,%eax
  fd = open(n, O_RDONLY);
 2ad:	89 c3                	mov    %eax,%ebx
  if(fd < 0)
 2af:	78 27                	js     2d8 <stat+0x48>
    return -1;
  r = fstat(fd, st);
 2b1:	8b 45 0c             	mov    0xc(%ebp),%eax
 2b4:	89 1c 24             	mov    %ebx,(%esp)
 2b7:	89 44 24 04          	mov    %eax,0x4(%esp)
 2bb:	e8 da 00 00 00       	call   39a <fstat>
  close(fd);
 2c0:	89 1c 24             	mov    %ebx,(%esp)
  r = fstat(fd, st);
 2c3:	89 c6                	mov    %eax,%esi
  close(fd);
 2c5:	e8 a0 00 00 00       	call   36a <close>
  return r;
 2ca:	89 f0                	mov    %esi,%eax
}
 2cc:	83 c4 10             	add    $0x10,%esp
 2cf:	5b                   	pop    %ebx
 2d0:	5e                   	pop    %esi
 2d1:	5d                   	pop    %ebp
 2d2:	c3                   	ret    
 2d3:	90                   	nop
 2d4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    return -1;
 2d8:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
 2dd:	eb ed                	jmp    2cc <stat+0x3c>
 2df:	90                   	nop

000002e0 <atoi>:

int
atoi(const char *s)
{
 2e0:	55                   	push   %ebp
 2e1:	89 e5                	mov    %esp,%ebp
 2e3:	8b 4d 08             	mov    0x8(%ebp),%ecx
 2e6:	53                   	push   %ebx
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
 2e7:	0f be 11             	movsbl (%ecx),%edx
 2ea:	8d 42 d0             	lea    -0x30(%edx),%eax
 2ed:	3c 09                	cmp    $0x9,%al
  n = 0;
 2ef:	b8 00 00 00 00       	mov    $0x0,%eax
  while('0' <= *s && *s <= '9')
 2f4:	77 17                	ja     30d <atoi+0x2d>
 2f6:	66 90                	xchg   %ax,%ax
    n = n*10 + *s++ - '0';
 2f8:	83 c1 01             	add    $0x1,%ecx
 2fb:	8d 04 80             	lea    (%eax,%eax,4),%eax
 2fe:	8d 44 42 d0          	lea    -0x30(%edx,%eax,2),%eax
  while('0' <= *s && *s <= '9')
 302:	0f be 11             	movsbl (%ecx),%edx
 305:	8d 5a d0             	lea    -0x30(%edx),%ebx
 308:	80 fb 09             	cmp    $0x9,%bl
 30b:	76 eb                	jbe    2f8 <atoi+0x18>
  return n;
}
 30d:	5b                   	pop    %ebx
 30e:	5d                   	pop    %ebp
 30f:	c3                   	ret    

00000310 <memmove>:

void*
memmove(void *vdst, const void *vsrc, int n)
{
 310:	55                   	push   %ebp
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
 311:	31 d2                	xor    %edx,%edx
{
 313:	89 e5                	mov    %esp,%ebp
 315:	56                   	push   %esi
 316:	8b 45 08             	mov    0x8(%ebp),%eax
 319:	53                   	push   %ebx
 31a:	8b 5d 10             	mov    0x10(%ebp),%ebx
 31d:	8b 75 0c             	mov    0xc(%ebp),%esi
  while(n-- > 0)
 320:	85 db                	test   %ebx,%ebx
 322:	7e 12                	jle    336 <memmove+0x26>
 324:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    *dst++ = *src++;
 328:	0f b6 0c 16          	movzbl (%esi,%edx,1),%ecx
 32c:	88 0c 10             	mov    %cl,(%eax,%edx,1)
 32f:	83 c2 01             	add    $0x1,%edx
  while(n-- > 0)
 332:	39 da                	cmp    %ebx,%edx
 334:	75 f2                	jne    328 <memmove+0x18>
  return vdst;
}
 336:	5b                   	pop    %ebx
 337:	5e                   	pop    %esi
 338:	5d                   	pop    %ebp
 339:	c3                   	ret    

0000033a <fork>:
 33a:	b8 01 00 00 00       	mov    $0x1,%eax
 33f:	cd 40                	int    $0x40
 341:	c3                   	ret    

00000342 <exit>:
 342:	b8 02 00 00 00       	mov    $0x2,%eax
 347:	cd 40                	int    $0x40
 349:	c3                   	ret    

0000034a <wait>:
 34a:	b8 03 00 00 00       	mov    $0x3,%eax
 34f:	cd 40                	int    $0x40
 351:	c3                   	ret    

00000352 <pipe>:
 352:	b8 04 00 00 00       	mov    $0x4,%eax
 357:	cd 40                	int    $0x40
 359:	c3                   	ret    

0000035a <read>:
 35a:	b8 05 00 00 00       	mov    $0x5,%eax
 35f:	cd 40                	int    $0x40
 361:	c3                   	ret    

00000362 <write>:
 362:	b8 10 00 00 00       	mov    $0x10,%eax
 367:	cd 40                	int    $0x40
 369:	c3                   	ret    

0000036a <close>:
 36a:	b8 15 00 00 00       	mov    $0x15,%eax
 36f:	cd 40                	int    $0x40
 371:	c3                   	ret    

00000372 <kill>:
 372:	b8 06 00 00 00       	mov    $0x6,%eax
 377:	cd 40                	int    $0x40
 379:	c3                   	ret    

0000037a <exec>:
 37a:	b8 07 00 00 00       	mov    $0x7,%eax
 37f:	cd 40                	int    $0x40
 381:	c3                   	ret    

00000382 <open>:
 382:	b8 0f 00 00 00       	mov    $0xf,%eax
 387:	cd 40                	int    $0x40
 389:	c3                   	ret    

0000038a <mknod>:
 38a:	b8 11 00 00 00       	mov    $0x11,%eax
 38f:	cd 40                	int    $0x40
 391:	c3                   	ret    

00000392 <unlink>:
 392:	b8 12 00 00 00       	mov    $0x12,%eax
 397:	cd 40                	int    $0x40
 399:	c3                   	ret    

0000039a <fstat>:
 39a:	b8 08 00 00 00       	mov    $0x8,%eax
 39f:	cd 40                	int    $0x40
 3a1:	c3                   	ret    

000003a2 <link>:
 3a2:	b8 13 00 00 00       	mov    $0x13,%eax
 3a7:	cd 40                	int    $0x40
 3a9:	c3                   	ret    

000003aa <mkdir>:
 3aa:	b8 14 00 00 00       	mov    $0x14,%eax
 3af:	cd 40                	int    $0x40
 3b1:	c3                   	ret    

000003b2 <chdir>:
 3b2:	b8 09 00 00 00       	mov    $0x9,%eax
 3b7:	cd 40                	int    $0x40
 3b9:	c3                   	ret    

000003ba <dup>:
 3ba:	b8 0a 00 00 00       	mov    $0xa,%eax
 3bf:	cd 40                	int    $0x40
 3c1:	c3                   	ret    

000003c2 <getpid>:
 3c2:	b8 0b 00 00 00       	mov    $0xb,%eax
 3c7:	cd 40                	int    $0x40
 3c9:	c3                   	ret    

000003ca <sbrk>:
 3ca:	b8 0c 00 00 00       	mov    $0xc,%eax
 3cf:	cd 40                	int    $0x40
 3d1:	c3                   	ret    

000003d2 <sleep>:
 3d2:	b8 0d 00 00 00       	mov    $0xd,%eax
 3d7:	cd 40                	int    $0x40
 3d9:	c3                   	ret    

000003da <uptime>:
 3da:	b8 0e 00 00 00       	mov    $0xe,%eax
 3df:	cd 40                	int    $0x40
 3e1:	c3                   	ret    
 3e2:	66 90                	xchg   %ax,%ax
 3e4:	66 90                	xchg   %ax,%ax
 3e6:	66 90                	xchg   %ax,%ax
 3e8:	66 90                	xchg   %ax,%ax
 3ea:	66 90                	xchg   %ax,%ax
 3ec:	66 90                	xchg   %ax,%ax
 3ee:	66 90                	xchg   %ax,%ax

000003f0 <printint>:
  write(fd, &c, 1);
}

static void
printint(int fd, int xx, int base, int sgn)
{
 3f0:	55                   	push   %ebp
 3f1:	89 e5                	mov    %esp,%ebp
 3f3:	57                   	push   %edi
 3f4:	56                   	push   %esi
 3f5:	89 c6                	mov    %eax,%esi
 3f7:	53                   	push   %ebx
 3f8:	83 ec 4c             	sub    $0x4c,%esp
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
 3fb:	8b 5d 08             	mov    0x8(%ebp),%ebx
 3fe:	85 db                	test   %ebx,%ebx
 400:	74 09                	je     40b <printint+0x1b>
 402:	89 d0                	mov    %edx,%eax
 404:	c1 e8 1f             	shr    $0x1f,%eax
 407:	84 c0                	test   %al,%al
 409:	75 75                	jne    480 <printint+0x90>
    neg = 1;
    x = -xx;
  } else {
    x = xx;
 40b:	89 d0                	mov    %edx,%eax
  neg = 0;
 40d:	c7 45 c4 00 00 00 00 	movl   $0x0,-0x3c(%ebp)
 414:	89 75 c0             	mov    %esi,-0x40(%ebp)
  }

  i = 0;
 417:	31 ff                	xor    %edi,%edi
 419:	89 ce                	mov    %ecx,%esi
 41b:	8d 5d d7             	lea    -0x29(%ebp),%ebx
 41e:	eb 02                	jmp    422 <printint+0x32>
  do{
    buf[i++] = digits[x % base];
 420:	89 cf                	mov    %ecx,%edi
 422:	31 d2                	xor    %edx,%edx
 424:	f7 f6                	div    %esi
 426:	8d 4f 01             	lea    0x1(%edi),%ecx
 429:	0f b6 92 4d 08 00 00 	movzbl 0x84d(%edx),%edx
  }while((x /= base) != 0);
 430:	85 c0                	test   %eax,%eax
    buf[i++] = digits[x % base];
 432:	88 14 0b             	mov    %dl,(%ebx,%ecx,1)
  }while((x /= base) != 0);
 435:	75 e9                	jne    420 <printint+0x30>
  if(neg)
 437:	8b 55 c4             	mov    -0x3c(%ebp),%edx
    buf[i++] = digits[x % base];
 43a:	89 c8                	mov    %ecx,%eax
 43c:	8b 75 c0             	mov    -0x40(%ebp),%esi
  if(neg)
 43f:	85 d2                	test   %edx,%edx
 441:	74 08                	je     44b <printint+0x5b>
    buf[i++] = '-';
 443:	8d 4f 02             	lea    0x2(%edi),%ecx
 446:	c6 44 05 d8 2d       	movb   $0x2d,-0x28(%ebp,%eax,1)

  while(--i >= 0)
 44b:	8d 79 ff             	lea    -0x1(%ecx),%edi
 44e:	66 90                	xchg   %ax,%ax
 450:	0f b6 44 3d d8       	movzbl -0x28(%ebp,%edi,1),%eax
 455:	83 ef 01             	sub    $0x1,%edi
  write(fd, &c, 1);
 458:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 45f:	00 
 460:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 464:	89 34 24             	mov    %esi,(%esp)
 467:	88 45 d7             	mov    %al,-0x29(%ebp)
 46a:	e8 f3 fe ff ff       	call   362 <write>
  while(--i >= 0)
 46f:	83 ff ff             	cmp    $0xffffffff,%edi
 472:	75 dc                	jne    450 <printint+0x60>
    putc(fd, buf[i]);
}
 474:	83 c4 4c             	add    $0x4c,%esp
 477:	5b                   	pop    %ebx
 478:	5e                   	pop    %esi
 479:	5f                   	pop    %edi
 47a:	5d                   	pop    %ebp
 47b:	c3                   	ret    
 47c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    x = -xx;
 480:	89 d0                	mov    %edx,%eax
 482:	f7 d8                	neg    %eax
    neg = 1;
 484:	c7 45 c4 01 00 00 00 	movl   $0x1,-0x3c(%ebp)
 48b:	eb 87                	jmp    414 <printint+0x24>
 48d:	8d 76 00             	lea    0x0(%esi),%esi

00000490 <printf>:

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, const char *fmt, ...)
{
 490:	55                   	push   %ebp
 491:	89 e5                	mov    %esp,%ebp
 493:	57                   	push   %edi
  char *s;
  int c, i, state;
  uint *ap;

  state = 0;
 494:	31 ff                	xor    %edi,%edi
{
 496:	56                   	push   %esi
 497:	53                   	push   %ebx
 498:	83 ec 3c             	sub    $0x3c,%esp
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
 49b:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  ap = (uint*)(void*)&fmt + 1;
 49e:	8d 45 10             	lea    0x10(%ebp),%eax
{
 4a1:	8b 75 08             	mov    0x8(%ebp),%esi
  ap = (uint*)(void*)&fmt + 1;
 4a4:	89 45 d4             	mov    %eax,-0x2c(%ebp)
  for(i = 0; fmt[i]; i++){
 4a7:	0f b6 13             	movzbl (%ebx),%edx
 4aa:	83 c3 01             	add    $0x1,%ebx
 4ad:	84 d2                	test   %dl,%dl
 4af:	75 39                	jne    4ea <printf+0x5a>
 4b1:	e9 c2 00 00 00       	jmp    578 <printf+0xe8>
 4b6:	66 90                	xchg   %ax,%ax
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
 4b8:	83 fa 25             	cmp    $0x25,%edx
 4bb:	0f 84 bf 00 00 00    	je     580 <printf+0xf0>
  write(fd, &c, 1);
 4c1:	8d 45 e2             	lea    -0x1e(%ebp),%eax
 4c4:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 4cb:	00 
 4cc:	89 44 24 04          	mov    %eax,0x4(%esp)
 4d0:	89 34 24             	mov    %esi,(%esp)
        state = '%';
      } else {
        putc(fd, c);
 4d3:	88 55 e2             	mov    %dl,-0x1e(%ebp)
  write(fd, &c, 1);
 4d6:	e8 87 fe ff ff       	call   362 <write>
 4db:	83 c3 01             	add    $0x1,%ebx
  for(i = 0; fmt[i]; i++){
 4de:	0f b6 53 ff          	movzbl -0x1(%ebx),%edx
 4e2:	84 d2                	test   %dl,%dl
 4e4:	0f 84 8e 00 00 00    	je     578 <printf+0xe8>
    if(state == 0){
 4ea:	85 ff                	test   %edi,%edi
    c = fmt[i] & 0xff;
 4ec:	0f be c2             	movsbl %dl,%eax
    if(state == 0){
 4ef:	74 c7                	je     4b8 <printf+0x28>
      }
    } else if(state == '%'){
 4f1:	83 ff 25             	cmp    $0x25,%edi
 4f4:	75 e5                	jne    4db <printf+0x4b>
      if(c == 'd'){
 4f6:	83 fa 64             	cmp    $0x64,%edx
 4f9:	0f 84 31 01 00 00    	je     630 <printf+0x1a0>
        printint(fd, *ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
 4ff:	25 f7 00 00 00       	and    $0xf7,%eax
 504:	83 f8 70             	cmp    $0x70,%eax
 507:	0f 84 83 00 00 00    	je     590 <printf+0x100>
        printint(fd, *ap, 16, 0);
        ap++;
      } else if(c == 's'){
 50d:	83 fa 73             	cmp    $0x73,%edx
 510:	0f 84 a2 00 00 00    	je     5b8 <printf+0x128>
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
        }
      } else if(c == 'c'){
 516:	83 fa 63             	cmp    $0x63,%edx
 519:	0f 84 35 01 00 00    	je     654 <printf+0x1c4>
        putc(fd, *ap);
        ap++;
      } else if(c == '%'){
 51f:	83 fa 25             	cmp    $0x25,%edx
 522:	0f 84 e0 00 00 00    	je     608 <printf+0x178>
  write(fd, &c, 1);
 528:	8d 45 e6             	lea    -0x1a(%ebp),%eax
 52b:	83 c3 01             	add    $0x1,%ebx
 52e:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 535:	00 
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
 536:	31 ff                	xor    %edi,%edi
  write(fd, &c, 1);
 538:	89 44 24 04          	mov    %eax,0x4(%esp)
 53c:	89 34 24             	mov    %esi,(%esp)
 53f:	89 55 d0             	mov    %edx,-0x30(%ebp)
 542:	c6 45 e6 25          	movb   $0x25,-0x1a(%ebp)
 546:	e8 17 fe ff ff       	call   362 <write>
        putc(fd, c);
 54b:	8b 55 d0             	mov    -0x30(%ebp),%edx
  write(fd, &c, 1);
 54e:	8d 45 e7             	lea    -0x19(%ebp),%eax
 551:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 558:	00 
 559:	89 44 24 04          	mov    %eax,0x4(%esp)
 55d:	89 34 24             	mov    %esi,(%esp)
        putc(fd, c);
 560:	88 55 e7             	mov    %dl,-0x19(%ebp)
  write(fd, &c, 1);
 563:	e8 fa fd ff ff       	call   362 <write>
  for(i = 0; fmt[i]; i++){
 568:	0f b6 53 ff          	movzbl -0x1(%ebx),%edx
 56c:	84 d2                	test   %dl,%dl
 56e:	0f 85 76 ff ff ff    	jne    4ea <printf+0x5a>
 574:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    }
  }
}
 578:	83 c4 3c             	add    $0x3c,%esp
 57b:	5b                   	pop    %ebx
 57c:	5e                   	pop    %esi
 57d:	5f                   	pop    %edi
 57e:	5d                   	pop    %ebp
 57f:	c3                   	ret    
        state = '%';
 580:	bf 25 00 00 00       	mov    $0x25,%edi
 585:	e9 51 ff ff ff       	jmp    4db <printf+0x4b>
 58a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        printint(fd, *ap, 16, 0);
 590:	8b 45 d4             	mov    -0x2c(%ebp),%eax
 593:	b9 10 00 00 00       	mov    $0x10,%ecx
      state = 0;
 598:	31 ff                	xor    %edi,%edi
        printint(fd, *ap, 16, 0);
 59a:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
 5a1:	8b 10                	mov    (%eax),%edx
 5a3:	89 f0                	mov    %esi,%eax
 5a5:	e8 46 fe ff ff       	call   3f0 <printint>
        ap++;
 5aa:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
 5ae:	e9 28 ff ff ff       	jmp    4db <printf+0x4b>
 5b3:	90                   	nop
 5b4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
        s = (char*)*ap;
 5b8:	8b 45 d4             	mov    -0x2c(%ebp),%eax
        ap++;
 5bb:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
        s = (char*)*ap;
 5bf:	8b 38                	mov    (%eax),%edi
          s = "(null)";
 5c1:	b8 46 08 00 00       	mov    $0x846,%eax
 5c6:	85 ff                	test   %edi,%edi
 5c8:	0f 44 f8             	cmove  %eax,%edi
        while(*s != 0){
 5cb:	0f b6 07             	movzbl (%edi),%eax
 5ce:	84 c0                	test   %al,%al
 5d0:	74 2a                	je     5fc <printf+0x16c>
 5d2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 5d8:	88 45 e3             	mov    %al,-0x1d(%ebp)
  write(fd, &c, 1);
 5db:	8d 45 e3             	lea    -0x1d(%ebp),%eax
          s++;
 5de:	83 c7 01             	add    $0x1,%edi
  write(fd, &c, 1);
 5e1:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 5e8:	00 
 5e9:	89 44 24 04          	mov    %eax,0x4(%esp)
 5ed:	89 34 24             	mov    %esi,(%esp)
 5f0:	e8 6d fd ff ff       	call   362 <write>
        while(*s != 0){
 5f5:	0f b6 07             	movzbl (%edi),%eax
 5f8:	84 c0                	test   %al,%al
 5fa:	75 dc                	jne    5d8 <printf+0x148>
      state = 0;
 5fc:	31 ff                	xor    %edi,%edi
 5fe:	e9 d8 fe ff ff       	jmp    4db <printf+0x4b>
 603:	90                   	nop
 604:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  write(fd, &c, 1);
 608:	8d 45 e5             	lea    -0x1b(%ebp),%eax
      state = 0;
 60b:	31 ff                	xor    %edi,%edi
  write(fd, &c, 1);
 60d:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 614:	00 
 615:	89 44 24 04          	mov    %eax,0x4(%esp)
 619:	89 34 24             	mov    %esi,(%esp)
 61c:	c6 45 e5 25          	movb   $0x25,-0x1b(%ebp)
 620:	e8 3d fd ff ff       	call   362 <write>
 625:	e9 b1 fe ff ff       	jmp    4db <printf+0x4b>
 62a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        printint(fd, *ap, 10, 1);
 630:	8b 45 d4             	mov    -0x2c(%ebp),%eax
 633:	b9 0a 00 00 00       	mov    $0xa,%ecx
      state = 0;
 638:	66 31 ff             	xor    %di,%di
        printint(fd, *ap, 10, 1);
 63b:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 642:	8b 10                	mov    (%eax),%edx
 644:	89 f0                	mov    %esi,%eax
 646:	e8 a5 fd ff ff       	call   3f0 <printint>
        ap++;
 64b:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
 64f:	e9 87 fe ff ff       	jmp    4db <printf+0x4b>
        putc(fd, *ap);
 654:	8b 45 d4             	mov    -0x2c(%ebp),%eax
      state = 0;
 657:	31 ff                	xor    %edi,%edi
        putc(fd, *ap);
 659:	8b 00                	mov    (%eax),%eax
  write(fd, &c, 1);
 65b:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 662:	00 
 663:	89 34 24             	mov    %esi,(%esp)
        putc(fd, *ap);
 666:	88 45 e4             	mov    %al,-0x1c(%ebp)
  write(fd, &c, 1);
 669:	8d 45 e4             	lea    -0x1c(%ebp),%eax
 66c:	89 44 24 04          	mov    %eax,0x4(%esp)
 670:	e8 ed fc ff ff       	call   362 <write>
        ap++;
 675:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
 679:	e9 5d fe ff ff       	jmp    4db <printf+0x4b>
 67e:	66 90                	xchg   %ax,%ax

00000680 <free>:
static Header base;
static Header *freep;

void
free(void *ap)
{
 680:	55                   	push   %ebp
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 681:	a1 cc 0a 00 00       	mov    0xacc,%eax
{
 686:	89 e5                	mov    %esp,%ebp
 688:	57                   	push   %edi
 689:	56                   	push   %esi
 68a:	53                   	push   %ebx
 68b:	8b 5d 08             	mov    0x8(%ebp),%ebx
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 68e:	8b 08                	mov    (%eax),%ecx
  bp = (Header*)ap - 1;
 690:	8d 53 f8             	lea    -0x8(%ebx),%edx
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 693:	39 d0                	cmp    %edx,%eax
 695:	72 11                	jb     6a8 <free+0x28>
 697:	90                   	nop
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 698:	39 c8                	cmp    %ecx,%eax
 69a:	72 04                	jb     6a0 <free+0x20>
 69c:	39 ca                	cmp    %ecx,%edx
 69e:	72 10                	jb     6b0 <free+0x30>
 6a0:	89 c8                	mov    %ecx,%eax
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 6a2:	39 d0                	cmp    %edx,%eax
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 6a4:	8b 08                	mov    (%eax),%ecx
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 6a6:	73 f0                	jae    698 <free+0x18>
 6a8:	39 ca                	cmp    %ecx,%edx
 6aa:	72 04                	jb     6b0 <free+0x30>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 6ac:	39 c8                	cmp    %ecx,%eax
 6ae:	72 f0                	jb     6a0 <free+0x20>
      break;
  if(bp + bp->s.size == p->s.ptr){
 6b0:	8b 73 fc             	mov    -0x4(%ebx),%esi
 6b3:	8d 3c f2             	lea    (%edx,%esi,8),%edi
 6b6:	39 cf                	cmp    %ecx,%edi
 6b8:	74 1e                	je     6d8 <free+0x58>
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
 6ba:	89 4b f8             	mov    %ecx,-0x8(%ebx)
  if(p + p->s.size == bp){
 6bd:	8b 48 04             	mov    0x4(%eax),%ecx
 6c0:	8d 34 c8             	lea    (%eax,%ecx,8),%esi
 6c3:	39 f2                	cmp    %esi,%edx
 6c5:	74 28                	je     6ef <free+0x6f>
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
 6c7:	89 10                	mov    %edx,(%eax)
  freep = p;
 6c9:	a3 cc 0a 00 00       	mov    %eax,0xacc
}
 6ce:	5b                   	pop    %ebx
 6cf:	5e                   	pop    %esi
 6d0:	5f                   	pop    %edi
 6d1:	5d                   	pop    %ebp
 6d2:	c3                   	ret    
 6d3:	90                   	nop
 6d4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    bp->s.size += p->s.ptr->s.size;
 6d8:	03 71 04             	add    0x4(%ecx),%esi
 6db:	89 73 fc             	mov    %esi,-0x4(%ebx)
    bp->s.ptr = p->s.ptr->s.ptr;
 6de:	8b 08                	mov    (%eax),%ecx
 6e0:	8b 09                	mov    (%ecx),%ecx
 6e2:	89 4b f8             	mov    %ecx,-0x8(%ebx)
  if(p + p->s.size == bp){
 6e5:	8b 48 04             	mov    0x4(%eax),%ecx
 6e8:	8d 34 c8             	lea    (%eax,%ecx,8),%esi
 6eb:	39 f2                	cmp    %esi,%edx
 6ed:	75 d8                	jne    6c7 <free+0x47>
    p->s.size += bp->s.size;
 6ef:	03 4b fc             	add    -0x4(%ebx),%ecx
  freep = p;
 6f2:	a3 cc 0a 00 00       	mov    %eax,0xacc
    p->s.size += bp->s.size;
 6f7:	89 48 04             	mov    %ecx,0x4(%eax)
    p->s.ptr = bp->s.ptr;
 6fa:	8b 53 f8             	mov    -0x8(%ebx),%edx
 6fd:	89 10                	mov    %edx,(%eax)
}
 6ff:	5b                   	pop    %ebx
 700:	5e                   	pop    %esi
 701:	5f                   	pop    %edi
 702:	5d                   	pop    %ebp
 703:	c3                   	ret    
 704:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 70a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

00000710 <malloc>:
  return freep;
}

void*
malloc(uint nbytes)
{
 710:	55                   	push   %ebp
 711:	89 e5                	mov    %esp,%ebp
 713:	57                   	push   %edi
 714:	56                   	push   %esi
 715:	53                   	push   %ebx
 716:	83 ec 1c             	sub    $0x1c,%esp
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 719:	8b 45 08             	mov    0x8(%ebp),%eax
  if((prevp = freep) == 0){
 71c:	8b 1d cc 0a 00 00    	mov    0xacc,%ebx
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 722:	8d 48 07             	lea    0x7(%eax),%ecx
 725:	c1 e9 03             	shr    $0x3,%ecx
  if((prevp = freep) == 0){
 728:	85 db                	test   %ebx,%ebx
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 72a:	8d 71 01             	lea    0x1(%ecx),%esi
  if((prevp = freep) == 0){
 72d:	0f 84 9b 00 00 00    	je     7ce <malloc+0xbe>
 733:	8b 13                	mov    (%ebx),%edx
 735:	8b 7a 04             	mov    0x4(%edx),%edi
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
 738:	39 fe                	cmp    %edi,%esi
 73a:	76 64                	jbe    7a0 <malloc+0x90>
 73c:	8d 04 f5 00 00 00 00 	lea    0x0(,%esi,8),%eax
  if(nu < 4096)
 743:	bb 00 80 00 00       	mov    $0x8000,%ebx
 748:	89 45 e4             	mov    %eax,-0x1c(%ebp)
 74b:	eb 0e                	jmp    75b <malloc+0x4b>
 74d:	8d 76 00             	lea    0x0(%esi),%esi
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
 750:	8b 02                	mov    (%edx),%eax
    if(p->s.size >= nunits){
 752:	8b 78 04             	mov    0x4(%eax),%edi
 755:	39 fe                	cmp    %edi,%esi
 757:	76 4f                	jbe    7a8 <malloc+0x98>
 759:	89 c2                	mov    %eax,%edx
        p->s.size = nunits;
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
 75b:	3b 15 cc 0a 00 00    	cmp    0xacc,%edx
 761:	75 ed                	jne    750 <malloc+0x40>
  if(nu < 4096)
 763:	8b 45 e4             	mov    -0x1c(%ebp),%eax
 766:	81 fe 00 10 00 00    	cmp    $0x1000,%esi
 76c:	bf 00 10 00 00       	mov    $0x1000,%edi
 771:	0f 43 fe             	cmovae %esi,%edi
 774:	0f 42 c3             	cmovb  %ebx,%eax
  p = sbrk(nu * sizeof(Header));
 777:	89 04 24             	mov    %eax,(%esp)
 77a:	e8 4b fc ff ff       	call   3ca <sbrk>
  if(p == (char*)-1)
 77f:	83 f8 ff             	cmp    $0xffffffff,%eax
 782:	74 18                	je     79c <malloc+0x8c>
  hp->s.size = nu;
 784:	89 78 04             	mov    %edi,0x4(%eax)
  free((void*)(hp + 1));
 787:	83 c0 08             	add    $0x8,%eax
 78a:	89 04 24             	mov    %eax,(%esp)
 78d:	e8 ee fe ff ff       	call   680 <free>
  return freep;
 792:	8b 15 cc 0a 00 00    	mov    0xacc,%edx
      if((p = morecore(nunits)) == 0)
 798:	85 d2                	test   %edx,%edx
 79a:	75 b4                	jne    750 <malloc+0x40>
        return 0;
 79c:	31 c0                	xor    %eax,%eax
 79e:	eb 20                	jmp    7c0 <malloc+0xb0>
    if(p->s.size >= nunits){
 7a0:	89 d0                	mov    %edx,%eax
 7a2:	89 da                	mov    %ebx,%edx
 7a4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      if(p->s.size == nunits)
 7a8:	39 fe                	cmp    %edi,%esi
 7aa:	74 1c                	je     7c8 <malloc+0xb8>
        p->s.size -= nunits;
 7ac:	29 f7                	sub    %esi,%edi
 7ae:	89 78 04             	mov    %edi,0x4(%eax)
        p += p->s.size;
 7b1:	8d 04 f8             	lea    (%eax,%edi,8),%eax
        p->s.size = nunits;
 7b4:	89 70 04             	mov    %esi,0x4(%eax)
      freep = prevp;
 7b7:	89 15 cc 0a 00 00    	mov    %edx,0xacc
      return (void*)(p + 1);
 7bd:	83 c0 08             	add    $0x8,%eax
  }
}
 7c0:	83 c4 1c             	add    $0x1c,%esp
 7c3:	5b                   	pop    %ebx
 7c4:	5e                   	pop    %esi
 7c5:	5f                   	pop    %edi
 7c6:	5d                   	pop    %ebp
 7c7:	c3                   	ret    
        prevp->s.ptr = p->s.ptr;
 7c8:	8b 08                	mov    (%eax),%ecx
 7ca:	89 0a                	mov    %ecx,(%edx)
 7cc:	eb e9                	jmp    7b7 <malloc+0xa7>
    base.s.ptr = freep = prevp = &base;
 7ce:	c7 05 cc 0a 00 00 d0 	movl   $0xad0,0xacc
 7d5:	0a 00 00 
    base.s.size = 0;
 7d8:	ba d0 0a 00 00       	mov    $0xad0,%edx
    base.s.ptr = freep = prevp = &base;
 7dd:	c7 05 d0 0a 00 00 d0 	movl   $0xad0,0xad0
 7e4:	0a 00 00 
    base.s.size = 0;
 7e7:	c7 05 d4 0a 00 00 00 	movl   $0x0,0xad4
 7ee:	00 00 00 
 7f1:	e9 46 ff ff ff       	jmp    73c <malloc+0x2c>
