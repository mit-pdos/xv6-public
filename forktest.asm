
_forktest:     file format elf32-i386


Disassembly of section .text:

00000000 <main>:

  printf(1, "fork test OK\n");
}

int main(void)
{
   0:	55                   	push   %ebp
   1:	89 e5                	mov    %esp,%ebp
   3:	83 e4 f0             	and    $0xfffffff0,%esp
   6:	83 ec 10             	sub    $0x10,%esp
  forktest();
   9:	e8 42 00 00 00       	call   50 <forktest>
  exit(0);
   e:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  15:	e8 58 03 00 00       	call   372 <exit>
  1a:	66 90                	xchg   %ax,%ax
  1c:	66 90                	xchg   %ax,%ax
  1e:	66 90                	xchg   %ax,%ax

00000020 <printf>:
{
  20:	55                   	push   %ebp
  21:	89 e5                	mov    %esp,%ebp
  23:	53                   	push   %ebx
  24:	83 ec 14             	sub    $0x14,%esp
  27:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  write(fd, s, strlen(s));
  2a:	89 1c 24             	mov    %ebx,(%esp)
  2d:	e8 9e 01 00 00       	call   1d0 <strlen>
  32:	89 5c 24 04          	mov    %ebx,0x4(%esp)
  36:	89 44 24 08          	mov    %eax,0x8(%esp)
  3a:	8b 45 08             	mov    0x8(%ebp),%eax
  3d:	89 04 24             	mov    %eax,(%esp)
  40:	e8 4d 03 00 00       	call   392 <write>
}
  45:	83 c4 14             	add    $0x14,%esp
  48:	5b                   	pop    %ebx
  49:	5d                   	pop    %ebp
  4a:	c3                   	ret    
  4b:	90                   	nop
  4c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00000050 <forktest>:
{
  50:	55                   	push   %ebp
  51:	89 e5                	mov    %esp,%ebp
  53:	53                   	push   %ebx
  for (n = 0; n < N; n++)
  54:	31 db                	xor    %ebx,%ebx
{
  56:	83 ec 14             	sub    $0x14,%esp
  printf(1, "fork test\n");
  59:	c7 44 24 04 14 04 00 	movl   $0x414,0x4(%esp)
  60:	00 
  61:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
  68:	e8 b3 ff ff ff       	call   20 <printf>
  6d:	eb 13                	jmp    82 <forktest+0x32>
  6f:	90                   	nop
    if (pid == 0)
  70:	0f 84 a6 00 00 00    	je     11c <forktest+0xcc>
  for (n = 0; n < N; n++)
  76:	83 c3 01             	add    $0x1,%ebx
  79:	81 fb e8 03 00 00    	cmp    $0x3e8,%ebx
  7f:	90                   	nop
  80:	74 56                	je     d8 <forktest+0x88>
    pid = fork();
  82:	e8 e3 02 00 00       	call   36a <fork>
    if (pid < 0)
  87:	85 c0                	test   %eax,%eax
  89:	79 e5                	jns    70 <forktest+0x20>
  for (; n > 0; n--)
  8b:	85 db                	test   %ebx,%ebx
  8d:	8d 76 00             	lea    0x0(%esi),%esi
  90:	74 1b                	je     ad <forktest+0x5d>
  92:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    if (wait(0) < 0)
  98:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  9f:	e8 d6 02 00 00       	call   37a <wait>
  a4:	85 c0                	test   %eax,%eax
  a6:	78 60                	js     108 <forktest+0xb8>
  for (; n > 0; n--)
  a8:	83 eb 01             	sub    $0x1,%ebx
  ab:	75 eb                	jne    98 <forktest+0x48>
  if (wait(0) != -1)
  ad:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  b4:	e8 c1 02 00 00       	call   37a <wait>
  b9:	83 f8 ff             	cmp    $0xffffffff,%eax
  bc:	75 6a                	jne    128 <forktest+0xd8>
  printf(1, "fork test OK\n");
  be:	c7 44 24 04 46 04 00 	movl   $0x446,0x4(%esp)
  c5:	00 
  c6:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
  cd:	e8 4e ff ff ff       	call   20 <printf>
}
  d2:	83 c4 14             	add    $0x14,%esp
  d5:	5b                   	pop    %ebx
  d6:	5d                   	pop    %ebp
  d7:	c3                   	ret    
  write(fd, s, strlen(s));
  d8:	c7 04 24 54 04 00 00 	movl   $0x454,(%esp)
  df:	e8 ec 00 00 00       	call   1d0 <strlen>
  e4:	c7 44 24 04 54 04 00 	movl   $0x454,0x4(%esp)
  eb:	00 
  ec:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
  f3:	89 44 24 08          	mov    %eax,0x8(%esp)
  f7:	e8 96 02 00 00       	call   392 <write>
    exit(0);
  fc:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
 103:	e8 6a 02 00 00       	call   372 <exit>
      printf(1, "wait stopped early\n");
 108:	c7 44 24 04 1f 04 00 	movl   $0x41f,0x4(%esp)
 10f:	00 
 110:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 117:	e8 04 ff ff ff       	call   20 <printf>
      exit(0);
 11c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
 123:	e8 4a 02 00 00       	call   372 <exit>
    printf(1, "wait got too many\n");
 128:	c7 44 24 04 33 04 00 	movl   $0x433,0x4(%esp)
 12f:	00 
 130:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 137:	e8 e4 fe ff ff       	call   20 <printf>
    exit(0);
 13c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
 143:	e8 2a 02 00 00       	call   372 <exit>
 148:	66 90                	xchg   %ax,%ax
 14a:	66 90                	xchg   %ax,%ax
 14c:	66 90                	xchg   %ax,%ax
 14e:	66 90                	xchg   %ax,%ax

00000150 <strcpy>:
#include "user.h"
#include "x86.h"

char*
strcpy(char *s, const char *t)
{
 150:	55                   	push   %ebp
 151:	89 e5                	mov    %esp,%ebp
 153:	8b 45 08             	mov    0x8(%ebp),%eax
 156:	8b 4d 0c             	mov    0xc(%ebp),%ecx
 159:	53                   	push   %ebx
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
 15a:	89 c2                	mov    %eax,%edx
 15c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 160:	83 c1 01             	add    $0x1,%ecx
 163:	0f b6 59 ff          	movzbl -0x1(%ecx),%ebx
 167:	83 c2 01             	add    $0x1,%edx
 16a:	84 db                	test   %bl,%bl
 16c:	88 5a ff             	mov    %bl,-0x1(%edx)
 16f:	75 ef                	jne    160 <strcpy+0x10>
    ;
  return os;
}
 171:	5b                   	pop    %ebx
 172:	5d                   	pop    %ebp
 173:	c3                   	ret    
 174:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 17a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

00000180 <strcmp>:

int
strcmp(const char *p, const char *q)
{
 180:	55                   	push   %ebp
 181:	89 e5                	mov    %esp,%ebp
 183:	8b 55 08             	mov    0x8(%ebp),%edx
 186:	53                   	push   %ebx
 187:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  while(*p && *p == *q)
 18a:	0f b6 02             	movzbl (%edx),%eax
 18d:	84 c0                	test   %al,%al
 18f:	74 2d                	je     1be <strcmp+0x3e>
 191:	0f b6 19             	movzbl (%ecx),%ebx
 194:	38 d8                	cmp    %bl,%al
 196:	74 0e                	je     1a6 <strcmp+0x26>
 198:	eb 2b                	jmp    1c5 <strcmp+0x45>
 19a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 1a0:	38 c8                	cmp    %cl,%al
 1a2:	75 15                	jne    1b9 <strcmp+0x39>
    p++, q++;
 1a4:	89 d9                	mov    %ebx,%ecx
 1a6:	83 c2 01             	add    $0x1,%edx
  while(*p && *p == *q)
 1a9:	0f b6 02             	movzbl (%edx),%eax
    p++, q++;
 1ac:	8d 59 01             	lea    0x1(%ecx),%ebx
  while(*p && *p == *q)
 1af:	0f b6 49 01          	movzbl 0x1(%ecx),%ecx
 1b3:	84 c0                	test   %al,%al
 1b5:	75 e9                	jne    1a0 <strcmp+0x20>
 1b7:	31 c0                	xor    %eax,%eax
  return (uchar)*p - (uchar)*q;
 1b9:	29 c8                	sub    %ecx,%eax
}
 1bb:	5b                   	pop    %ebx
 1bc:	5d                   	pop    %ebp
 1bd:	c3                   	ret    
 1be:	0f b6 09             	movzbl (%ecx),%ecx
  while(*p && *p == *q)
 1c1:	31 c0                	xor    %eax,%eax
 1c3:	eb f4                	jmp    1b9 <strcmp+0x39>
 1c5:	0f b6 cb             	movzbl %bl,%ecx
 1c8:	eb ef                	jmp    1b9 <strcmp+0x39>
 1ca:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

000001d0 <strlen>:

uint
strlen(const char *s)
{
 1d0:	55                   	push   %ebp
 1d1:	89 e5                	mov    %esp,%ebp
 1d3:	8b 4d 08             	mov    0x8(%ebp),%ecx
  int n;

  for(n = 0; s[n]; n++)
 1d6:	80 39 00             	cmpb   $0x0,(%ecx)
 1d9:	74 12                	je     1ed <strlen+0x1d>
 1db:	31 d2                	xor    %edx,%edx
 1dd:	8d 76 00             	lea    0x0(%esi),%esi
 1e0:	83 c2 01             	add    $0x1,%edx
 1e3:	80 3c 11 00          	cmpb   $0x0,(%ecx,%edx,1)
 1e7:	89 d0                	mov    %edx,%eax
 1e9:	75 f5                	jne    1e0 <strlen+0x10>
    ;
  return n;
}
 1eb:	5d                   	pop    %ebp
 1ec:	c3                   	ret    
  for(n = 0; s[n]; n++)
 1ed:	31 c0                	xor    %eax,%eax
}
 1ef:	5d                   	pop    %ebp
 1f0:	c3                   	ret    
 1f1:	eb 0d                	jmp    200 <memset>
 1f3:	90                   	nop
 1f4:	90                   	nop
 1f5:	90                   	nop
 1f6:	90                   	nop
 1f7:	90                   	nop
 1f8:	90                   	nop
 1f9:	90                   	nop
 1fa:	90                   	nop
 1fb:	90                   	nop
 1fc:	90                   	nop
 1fd:	90                   	nop
 1fe:	90                   	nop
 1ff:	90                   	nop

00000200 <memset>:

void*
memset(void *dst, int c, uint n)
{
 200:	55                   	push   %ebp
 201:	89 e5                	mov    %esp,%ebp
 203:	8b 55 08             	mov    0x8(%ebp),%edx
 206:	57                   	push   %edi
}

static inline void
stosb(void *addr, int data, int cnt)
{
  asm volatile("cld; rep stosb" :
 207:	8b 4d 10             	mov    0x10(%ebp),%ecx
 20a:	8b 45 0c             	mov    0xc(%ebp),%eax
 20d:	89 d7                	mov    %edx,%edi
 20f:	fc                   	cld    
 210:	f3 aa                	rep stos %al,%es:(%edi)
  stosb(dst, c, n);
  return dst;
}
 212:	89 d0                	mov    %edx,%eax
 214:	5f                   	pop    %edi
 215:	5d                   	pop    %ebp
 216:	c3                   	ret    
 217:	89 f6                	mov    %esi,%esi
 219:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000220 <strchr>:

char*
strchr(const char *s, char c)
{
 220:	55                   	push   %ebp
 221:	89 e5                	mov    %esp,%ebp
 223:	8b 45 08             	mov    0x8(%ebp),%eax
 226:	53                   	push   %ebx
 227:	8b 55 0c             	mov    0xc(%ebp),%edx
  for(; *s; s++)
 22a:	0f b6 18             	movzbl (%eax),%ebx
 22d:	84 db                	test   %bl,%bl
 22f:	74 1d                	je     24e <strchr+0x2e>
    if(*s == c)
 231:	38 d3                	cmp    %dl,%bl
 233:	89 d1                	mov    %edx,%ecx
 235:	75 0d                	jne    244 <strchr+0x24>
 237:	eb 17                	jmp    250 <strchr+0x30>
 239:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 240:	38 ca                	cmp    %cl,%dl
 242:	74 0c                	je     250 <strchr+0x30>
  for(; *s; s++)
 244:	83 c0 01             	add    $0x1,%eax
 247:	0f b6 10             	movzbl (%eax),%edx
 24a:	84 d2                	test   %dl,%dl
 24c:	75 f2                	jne    240 <strchr+0x20>
      return (char*)s;
  return 0;
 24e:	31 c0                	xor    %eax,%eax
}
 250:	5b                   	pop    %ebx
 251:	5d                   	pop    %ebp
 252:	c3                   	ret    
 253:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 259:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000260 <gets>:

char*
gets(char *buf, int max)
{
 260:	55                   	push   %ebp
 261:	89 e5                	mov    %esp,%ebp
 263:	57                   	push   %edi
 264:	56                   	push   %esi
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
 265:	31 f6                	xor    %esi,%esi
{
 267:	53                   	push   %ebx
 268:	83 ec 2c             	sub    $0x2c,%esp
    cc = read(0, &c, 1);
 26b:	8d 7d e7             	lea    -0x19(%ebp),%edi
  for(i=0; i+1 < max; ){
 26e:	eb 31                	jmp    2a1 <gets+0x41>
    cc = read(0, &c, 1);
 270:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 277:	00 
 278:	89 7c 24 04          	mov    %edi,0x4(%esp)
 27c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
 283:	e8 02 01 00 00       	call   38a <read>
    if(cc < 1)
 288:	85 c0                	test   %eax,%eax
 28a:	7e 1d                	jle    2a9 <gets+0x49>
      break;
    buf[i++] = c;
 28c:	0f b6 45 e7          	movzbl -0x19(%ebp),%eax
  for(i=0; i+1 < max; ){
 290:	89 de                	mov    %ebx,%esi
    buf[i++] = c;
 292:	8b 55 08             	mov    0x8(%ebp),%edx
    if(c == '\n' || c == '\r')
 295:	3c 0d                	cmp    $0xd,%al
    buf[i++] = c;
 297:	88 44 1a ff          	mov    %al,-0x1(%edx,%ebx,1)
    if(c == '\n' || c == '\r')
 29b:	74 0c                	je     2a9 <gets+0x49>
 29d:	3c 0a                	cmp    $0xa,%al
 29f:	74 08                	je     2a9 <gets+0x49>
  for(i=0; i+1 < max; ){
 2a1:	8d 5e 01             	lea    0x1(%esi),%ebx
 2a4:	3b 5d 0c             	cmp    0xc(%ebp),%ebx
 2a7:	7c c7                	jl     270 <gets+0x10>
      break;
  }
  buf[i] = '\0';
 2a9:	8b 45 08             	mov    0x8(%ebp),%eax
 2ac:	c6 04 30 00          	movb   $0x0,(%eax,%esi,1)
  return buf;
}
 2b0:	83 c4 2c             	add    $0x2c,%esp
 2b3:	5b                   	pop    %ebx
 2b4:	5e                   	pop    %esi
 2b5:	5f                   	pop    %edi
 2b6:	5d                   	pop    %ebp
 2b7:	c3                   	ret    
 2b8:	90                   	nop
 2b9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

000002c0 <stat>:

int
stat(const char *n, struct stat *st)
{
 2c0:	55                   	push   %ebp
 2c1:	89 e5                	mov    %esp,%ebp
 2c3:	56                   	push   %esi
 2c4:	53                   	push   %ebx
 2c5:	83 ec 10             	sub    $0x10,%esp
  int fd;
  int r;

  fd = open(n, O_RDONLY);
 2c8:	8b 45 08             	mov    0x8(%ebp),%eax
 2cb:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
 2d2:	00 
 2d3:	89 04 24             	mov    %eax,(%esp)
 2d6:	e8 d7 00 00 00       	call   3b2 <open>
  if(fd < 0)
 2db:	85 c0                	test   %eax,%eax
  fd = open(n, O_RDONLY);
 2dd:	89 c3                	mov    %eax,%ebx
  if(fd < 0)
 2df:	78 27                	js     308 <stat+0x48>
    return -1;
  r = fstat(fd, st);
 2e1:	8b 45 0c             	mov    0xc(%ebp),%eax
 2e4:	89 1c 24             	mov    %ebx,(%esp)
 2e7:	89 44 24 04          	mov    %eax,0x4(%esp)
 2eb:	e8 da 00 00 00       	call   3ca <fstat>
  close(fd);
 2f0:	89 1c 24             	mov    %ebx,(%esp)
  r = fstat(fd, st);
 2f3:	89 c6                	mov    %eax,%esi
  close(fd);
 2f5:	e8 a0 00 00 00       	call   39a <close>
  return r;
 2fa:	89 f0                	mov    %esi,%eax
}
 2fc:	83 c4 10             	add    $0x10,%esp
 2ff:	5b                   	pop    %ebx
 300:	5e                   	pop    %esi
 301:	5d                   	pop    %ebp
 302:	c3                   	ret    
 303:	90                   	nop
 304:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    return -1;
 308:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
 30d:	eb ed                	jmp    2fc <stat+0x3c>
 30f:	90                   	nop

00000310 <atoi>:

int
atoi(const char *s)
{
 310:	55                   	push   %ebp
 311:	89 e5                	mov    %esp,%ebp
 313:	8b 4d 08             	mov    0x8(%ebp),%ecx
 316:	53                   	push   %ebx
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
 317:	0f be 11             	movsbl (%ecx),%edx
 31a:	8d 42 d0             	lea    -0x30(%edx),%eax
 31d:	3c 09                	cmp    $0x9,%al
  n = 0;
 31f:	b8 00 00 00 00       	mov    $0x0,%eax
  while('0' <= *s && *s <= '9')
 324:	77 17                	ja     33d <atoi+0x2d>
 326:	66 90                	xchg   %ax,%ax
    n = n*10 + *s++ - '0';
 328:	83 c1 01             	add    $0x1,%ecx
 32b:	8d 04 80             	lea    (%eax,%eax,4),%eax
 32e:	8d 44 42 d0          	lea    -0x30(%edx,%eax,2),%eax
  while('0' <= *s && *s <= '9')
 332:	0f be 11             	movsbl (%ecx),%edx
 335:	8d 5a d0             	lea    -0x30(%edx),%ebx
 338:	80 fb 09             	cmp    $0x9,%bl
 33b:	76 eb                	jbe    328 <atoi+0x18>
  return n;
}
 33d:	5b                   	pop    %ebx
 33e:	5d                   	pop    %ebp
 33f:	c3                   	ret    

00000340 <memmove>:

void*
memmove(void *vdst, const void *vsrc, int n)
{
 340:	55                   	push   %ebp
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
 341:	31 d2                	xor    %edx,%edx
{
 343:	89 e5                	mov    %esp,%ebp
 345:	56                   	push   %esi
 346:	8b 45 08             	mov    0x8(%ebp),%eax
 349:	53                   	push   %ebx
 34a:	8b 5d 10             	mov    0x10(%ebp),%ebx
 34d:	8b 75 0c             	mov    0xc(%ebp),%esi
  while(n-- > 0)
 350:	85 db                	test   %ebx,%ebx
 352:	7e 12                	jle    366 <memmove+0x26>
 354:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    *dst++ = *src++;
 358:	0f b6 0c 16          	movzbl (%esi,%edx,1),%ecx
 35c:	88 0c 10             	mov    %cl,(%eax,%edx,1)
 35f:	83 c2 01             	add    $0x1,%edx
  while(n-- > 0)
 362:	39 da                	cmp    %ebx,%edx
 364:	75 f2                	jne    358 <memmove+0x18>
  return vdst;
}
 366:	5b                   	pop    %ebx
 367:	5e                   	pop    %esi
 368:	5d                   	pop    %ebp
 369:	c3                   	ret    

0000036a <fork>:
 36a:	b8 01 00 00 00       	mov    $0x1,%eax
 36f:	cd 40                	int    $0x40
 371:	c3                   	ret    

00000372 <exit>:
 372:	b8 02 00 00 00       	mov    $0x2,%eax
 377:	cd 40                	int    $0x40
 379:	c3                   	ret    

0000037a <wait>:
 37a:	b8 03 00 00 00       	mov    $0x3,%eax
 37f:	cd 40                	int    $0x40
 381:	c3                   	ret    

00000382 <pipe>:
 382:	b8 04 00 00 00       	mov    $0x4,%eax
 387:	cd 40                	int    $0x40
 389:	c3                   	ret    

0000038a <read>:
 38a:	b8 05 00 00 00       	mov    $0x5,%eax
 38f:	cd 40                	int    $0x40
 391:	c3                   	ret    

00000392 <write>:
 392:	b8 10 00 00 00       	mov    $0x10,%eax
 397:	cd 40                	int    $0x40
 399:	c3                   	ret    

0000039a <close>:
 39a:	b8 15 00 00 00       	mov    $0x15,%eax
 39f:	cd 40                	int    $0x40
 3a1:	c3                   	ret    

000003a2 <kill>:
 3a2:	b8 06 00 00 00       	mov    $0x6,%eax
 3a7:	cd 40                	int    $0x40
 3a9:	c3                   	ret    

000003aa <exec>:
 3aa:	b8 07 00 00 00       	mov    $0x7,%eax
 3af:	cd 40                	int    $0x40
 3b1:	c3                   	ret    

000003b2 <open>:
 3b2:	b8 0f 00 00 00       	mov    $0xf,%eax
 3b7:	cd 40                	int    $0x40
 3b9:	c3                   	ret    

000003ba <mknod>:
 3ba:	b8 11 00 00 00       	mov    $0x11,%eax
 3bf:	cd 40                	int    $0x40
 3c1:	c3                   	ret    

000003c2 <unlink>:
 3c2:	b8 12 00 00 00       	mov    $0x12,%eax
 3c7:	cd 40                	int    $0x40
 3c9:	c3                   	ret    

000003ca <fstat>:
 3ca:	b8 08 00 00 00       	mov    $0x8,%eax
 3cf:	cd 40                	int    $0x40
 3d1:	c3                   	ret    

000003d2 <link>:
 3d2:	b8 13 00 00 00       	mov    $0x13,%eax
 3d7:	cd 40                	int    $0x40
 3d9:	c3                   	ret    

000003da <mkdir>:
 3da:	b8 14 00 00 00       	mov    $0x14,%eax
 3df:	cd 40                	int    $0x40
 3e1:	c3                   	ret    

000003e2 <chdir>:
 3e2:	b8 09 00 00 00       	mov    $0x9,%eax
 3e7:	cd 40                	int    $0x40
 3e9:	c3                   	ret    

000003ea <dup>:
 3ea:	b8 0a 00 00 00       	mov    $0xa,%eax
 3ef:	cd 40                	int    $0x40
 3f1:	c3                   	ret    

000003f2 <getpid>:
 3f2:	b8 0b 00 00 00       	mov    $0xb,%eax
 3f7:	cd 40                	int    $0x40
 3f9:	c3                   	ret    

000003fa <sbrk>:
 3fa:	b8 0c 00 00 00       	mov    $0xc,%eax
 3ff:	cd 40                	int    $0x40
 401:	c3                   	ret    

00000402 <sleep>:
 402:	b8 0d 00 00 00       	mov    $0xd,%eax
 407:	cd 40                	int    $0x40
 409:	c3                   	ret    

0000040a <uptime>:
 40a:	b8 0e 00 00 00       	mov    $0xe,%eax
 40f:	cd 40                	int    $0x40
 411:	c3                   	ret    
