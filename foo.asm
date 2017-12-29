
_foo：     檔案格式 elf32-i386


Disassembly of section .text:

00000000 <main>:
#include "user.h"
#include "fcntl.h"

int 
main(int argc, char *argv[])
{
   0:	8d 4c 24 04          	lea    0x4(%esp),%ecx
   4:	83 e4 f0             	and    $0xfffffff0,%esp
   7:	ff 71 fc             	pushl  -0x4(%ecx)
   a:	55                   	push   %ebp
   b:	89 e5                	mov    %esp,%ebp
   d:	57                   	push   %edi
   e:	56                   	push   %esi
   f:	53                   	push   %ebx
  10:	51                   	push   %ecx
  11:	83 ec 18             	sub    $0x18,%esp
  14:	8b 19                	mov    (%ecx),%ebx
  16:	8b 71 04             	mov    0x4(%ecx),%esi
  int k,n,id;
  double x=0,z,d;

  if(argc<2)
  19:	83 fb 01             	cmp    $0x1,%ebx
  1c:	0f 8e b4 00 00 00    	jle    d6 <main+0xd6>
    n = 1;  //default value
  else
    n = atoi( argv[1] ); //from command line
  22:	83 ec 0c             	sub    $0xc,%esp
  25:	ff 76 04             	pushl  0x4(%esi)
  28:	e8 d3 02 00 00       	call   300 <atoi>
  if( n<0 || n>20)
  2d:	83 c4 10             	add    $0x10,%esp
  30:	83 f8 14             	cmp    $0x14,%eax
  double x=0,z,d;

  if(argc<2)
    n = 1;  //default value
  else
    n = atoi( argv[1] ); //from command line
  33:	89 c7                	mov    %eax,%edi
  if( n<0 || n>20)
  35:	0f 86 e6 00 00 00    	jbe    121 <main+0x121>
    n = 2;
  if( argc < 3)
  3b:	83 fb 02             	cmp    $0x2,%ebx
  if(argc<2)
    n = 1;  //default value
  else
    n = atoi( argv[1] ); //from command line
  if( n<0 || n>20)
    n = 2;
  3e:	bf 02 00 00 00       	mov    $0x2,%edi
  if( argc < 3)
  43:	75 70                	jne    b5 <main+0xb5>
    d = 1.0;
  45:	d9 e8                	fld1   
  if(argc<2)
    n = 1;  //default value
  else
    n = atoi( argv[1] ); //from command line
  if( n<0 || n>20)
    n = 2;
  47:	bf 02 00 00 00       	mov    $0x2,%edi
  if( argc < 3)
    d = 1.0;
  4c:	dd 5d e0             	fstpl  -0x20(%ebp)
{
  int k,n,id;
  double x=0,z,d;

  if(argc<2)
    n = 1;  //default value
  4f:	31 db                	xor    %ebx,%ebx
  51:	eb 2d                	jmp    80 <main+0x80>
  53:	90                   	nop
  54:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  for(k = 0; k < n; k++){
    id = fork();
    if( id <0){
      printf(1, "%d failed in fork!\n", getpid());
    }
    else if( id > 0){ //parent
  58:	0f 84 87 00 00 00    	je     e5 <main+0xe5>
      printf(1,"Parent %d creating child %d\n", getpid(), id);
  5e:	e8 8f 03 00 00       	call   3f2 <getpid>
  63:	56                   	push   %esi
  64:	50                   	push   %eax
  else 
    d = atoi( argv[2] );

  x = 0;
  id = 0;
  for(k = 0; k < n; k++){
  65:	83 c3 01             	add    $0x1,%ebx
    id = fork();
    if( id <0){
      printf(1, "%d failed in fork!\n", getpid());
    }
    else if( id > 0){ //parent
      printf(1,"Parent %d creating child %d\n", getpid(), id);
  68:	68 04 08 00 00       	push   $0x804
  6d:	6a 01                	push   $0x1
  6f:	e8 5c 04 00 00       	call   4d0 <printf>
      wait();
  74:	e8 01 03 00 00       	call   37a <wait>
  79:	83 c4 10             	add    $0x10,%esp
  else 
    d = atoi( argv[2] );

  x = 0;
  id = 0;
  for(k = 0; k < n; k++){
  7c:	39 fb                	cmp    %edi,%ebx
  7e:	7d 30                	jge    b0 <main+0xb0>
    id = fork();
  80:	e8 e5 02 00 00       	call   36a <fork>
    if( id <0){
  85:	85 c0                	test   %eax,%eax
    d = atoi( argv[2] );

  x = 0;
  id = 0;
  for(k = 0; k < n; k++){
    id = fork();
  87:	89 c6                	mov    %eax,%esi
    if( id <0){
  89:	79 cd                	jns    58 <main+0x58>
      printf(1, "%d failed in fork!\n", getpid());
  8b:	e8 62 03 00 00       	call   3f2 <getpid>
  90:	83 ec 04             	sub    $0x4,%esp
  else 
    d = atoi( argv[2] );

  x = 0;
  id = 0;
  for(k = 0; k < n; k++){
  93:	83 c3 01             	add    $0x1,%ebx
    id = fork();
    if( id <0){
      printf(1, "%d failed in fork!\n", getpid());
  96:	50                   	push   %eax
  97:	68 f0 07 00 00       	push   $0x7f0
  9c:	6a 01                	push   $0x1
  9e:	e8 2d 04 00 00       	call   4d0 <printf>
  a3:	83 c4 10             	add    $0x10,%esp
  else 
    d = atoi( argv[2] );

  x = 0;
  id = 0;
  for(k = 0; k < n; k++){
  a6:	39 fb                	cmp    %edi,%ebx
  a8:	7c d6                	jl     80 <main+0x80>
  aa:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        x = x + 3.14 * 89.64;  //useless calculations to consume CPU time
      break;
    }
  }
  //cps();
  exit();
  b0:	e8 bd 02 00 00       	call   372 <exit>
  if( n<0 || n>20)
    n = 2;
  if( argc < 3)
    d = 1.0;
  else 
    d = atoi( argv[2] );
  b5:	83 ec 0c             	sub    $0xc,%esp
  b8:	ff 76 08             	pushl  0x8(%esi)
  bb:	e8 40 02 00 00       	call   300 <atoi>
  c0:	89 45 e0             	mov    %eax,-0x20(%ebp)
  c3:	83 c4 10             	add    $0x10,%esp
  c6:	db 45 e0             	fildl  -0x20(%ebp)
  c9:	dd 5d e0             	fstpl  -0x20(%ebp)

  x = 0;
  id = 0;
  for(k = 0; k < n; k++){
  cc:	85 ff                	test   %edi,%edi
  ce:	0f 85 7b ff ff ff    	jne    4f <main+0x4f>
  d4:	eb da                	jmp    b0 <main+0xb0>
  else
    n = atoi( argv[1] ); //from command line
  if( n<0 || n>20)
    n = 2;
  if( argc < 3)
    d = 1.0;
  d6:	d9 e8                	fld1   
{
  int k,n,id;
  double x=0,z,d;

  if(argc<2)
    n = 1;  //default value
  d8:	bf 01 00 00 00       	mov    $0x1,%edi
  else
    n = atoi( argv[1] ); //from command line
  if( n<0 || n>20)
    n = 2;
  if( argc < 3)
    d = 1.0;
  dd:	dd 5d e0             	fstpl  -0x20(%ebp)
  e0:	e9 6a ff ff ff       	jmp    4f <main+0x4f>
    else if( id > 0){ //parent
      printf(1,"Parent %d creating child %d\n", getpid(), id);
      wait();
    }
    else{  //child
      printf(1, "Child %d created\n", getpid());
  e5:	e8 08 03 00 00       	call   3f2 <getpid>
  ea:	52                   	push   %edx
  eb:	50                   	push   %eax
  ec:	68 21 08 00 00       	push   $0x821
  f1:	6a 01                	push   $0x1
  f3:	e8 d8 03 00 00       	call   4d0 <printf>
      for( z = 0; z < 8000000.0; z+=d)
  f8:	d9 ee                	fldz   
    else if( id > 0){ //parent
      printf(1,"Parent %d creating child %d\n", getpid(), id);
      wait();
    }
    else{  //child
      printf(1, "Child %d created\n", getpid());
  fa:	83 c4 10             	add    $0x10,%esp
      for( z = 0; z < 8000000.0; z+=d)
  fd:	d9 05 34 08 00 00    	flds   0x834
 103:	d9 c9                	fxch   %st(1)
 105:	eb 0b                	jmp    112 <main+0x112>
 107:	89 f6                	mov    %esi,%esi
 109:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
 110:	d9 c9                	fxch   %st(1)
 112:	dc 45 e0             	faddl  -0x20(%ebp)
 115:	d9 c9                	fxch   %st(1)
 117:	db e9                	fucomi %st(1),%st
 119:	77 f5                	ja     110 <main+0x110>
 11b:	dd d8                	fstp   %st(0)
 11d:	dd d8                	fstp   %st(0)
 11f:	eb 8f                	jmp    b0 <main+0xb0>
    n = 1;  //default value
  else
    n = atoi( argv[1] ); //from command line
  if( n<0 || n>20)
    n = 2;
  if( argc < 3)
 121:	83 fb 02             	cmp    $0x2,%ebx
 124:	75 8f                	jne    b5 <main+0xb5>
    d = 1.0;
 126:	d9 e8                	fld1   
 128:	dd 5d e0             	fstpl  -0x20(%ebp)
 12b:	eb 9f                	jmp    cc <main+0xcc>
 12d:	66 90                	xchg   %ax,%ax
 12f:	90                   	nop

00000130 <strcpy>:
#include "user.h"
#include "x86.h"

char*
strcpy(char *s, char *t)
{
 130:	55                   	push   %ebp
 131:	89 e5                	mov    %esp,%ebp
 133:	53                   	push   %ebx
 134:	8b 45 08             	mov    0x8(%ebp),%eax
 137:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
 13a:	89 c2                	mov    %eax,%edx
 13c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 140:	83 c1 01             	add    $0x1,%ecx
 143:	0f b6 59 ff          	movzbl -0x1(%ecx),%ebx
 147:	83 c2 01             	add    $0x1,%edx
 14a:	84 db                	test   %bl,%bl
 14c:	88 5a ff             	mov    %bl,-0x1(%edx)
 14f:	75 ef                	jne    140 <strcpy+0x10>
    ;
  return os;
}
 151:	5b                   	pop    %ebx
 152:	5d                   	pop    %ebp
 153:	c3                   	ret    
 154:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 15a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

00000160 <strcmp>:

int
strcmp(const char *p, const char *q)
{
 160:	55                   	push   %ebp
 161:	89 e5                	mov    %esp,%ebp
 163:	56                   	push   %esi
 164:	53                   	push   %ebx
 165:	8b 55 08             	mov    0x8(%ebp),%edx
 168:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  while(*p && *p == *q)
 16b:	0f b6 02             	movzbl (%edx),%eax
 16e:	0f b6 19             	movzbl (%ecx),%ebx
 171:	84 c0                	test   %al,%al
 173:	75 1e                	jne    193 <strcmp+0x33>
 175:	eb 29                	jmp    1a0 <strcmp+0x40>
 177:	89 f6                	mov    %esi,%esi
 179:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
    p++, q++;
 180:	83 c2 01             	add    $0x1,%edx
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
 183:	0f b6 02             	movzbl (%edx),%eax
    p++, q++;
 186:	8d 71 01             	lea    0x1(%ecx),%esi
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
 189:	0f b6 59 01          	movzbl 0x1(%ecx),%ebx
 18d:	84 c0                	test   %al,%al
 18f:	74 0f                	je     1a0 <strcmp+0x40>
 191:	89 f1                	mov    %esi,%ecx
 193:	38 d8                	cmp    %bl,%al
 195:	74 e9                	je     180 <strcmp+0x20>
    p++, q++;
  return (uchar)*p - (uchar)*q;
 197:	29 d8                	sub    %ebx,%eax
}
 199:	5b                   	pop    %ebx
 19a:	5e                   	pop    %esi
 19b:	5d                   	pop    %ebp
 19c:	c3                   	ret    
 19d:	8d 76 00             	lea    0x0(%esi),%esi
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
 1a0:	31 c0                	xor    %eax,%eax
    p++, q++;
  return (uchar)*p - (uchar)*q;
 1a2:	29 d8                	sub    %ebx,%eax
}
 1a4:	5b                   	pop    %ebx
 1a5:	5e                   	pop    %esi
 1a6:	5d                   	pop    %ebp
 1a7:	c3                   	ret    
 1a8:	90                   	nop
 1a9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

000001b0 <strlen>:

uint
strlen(char *s)
{
 1b0:	55                   	push   %ebp
 1b1:	89 e5                	mov    %esp,%ebp
 1b3:	8b 4d 08             	mov    0x8(%ebp),%ecx
  int n;

  for(n = 0; s[n]; n++)
 1b6:	80 39 00             	cmpb   $0x0,(%ecx)
 1b9:	74 12                	je     1cd <strlen+0x1d>
 1bb:	31 d2                	xor    %edx,%edx
 1bd:	8d 76 00             	lea    0x0(%esi),%esi
 1c0:	83 c2 01             	add    $0x1,%edx
 1c3:	80 3c 11 00          	cmpb   $0x0,(%ecx,%edx,1)
 1c7:	89 d0                	mov    %edx,%eax
 1c9:	75 f5                	jne    1c0 <strlen+0x10>
    ;
  return n;
}
 1cb:	5d                   	pop    %ebp
 1cc:	c3                   	ret    
uint
strlen(char *s)
{
  int n;

  for(n = 0; s[n]; n++)
 1cd:	31 c0                	xor    %eax,%eax
    ;
  return n;
}
 1cf:	5d                   	pop    %ebp
 1d0:	c3                   	ret    
 1d1:	eb 0d                	jmp    1e0 <memset>
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

000001e0 <memset>:

void*
memset(void *dst, int c, uint n)
{
 1e0:	55                   	push   %ebp
 1e1:	89 e5                	mov    %esp,%ebp
 1e3:	57                   	push   %edi
 1e4:	8b 55 08             	mov    0x8(%ebp),%edx
}

static inline void
stosb(void *addr, int data, int cnt)
{
  asm volatile("cld; rep stosb" :
 1e7:	8b 4d 10             	mov    0x10(%ebp),%ecx
 1ea:	8b 45 0c             	mov    0xc(%ebp),%eax
 1ed:	89 d7                	mov    %edx,%edi
 1ef:	fc                   	cld    
 1f0:	f3 aa                	rep stos %al,%es:(%edi)
  stosb(dst, c, n);
  return dst;
}
 1f2:	89 d0                	mov    %edx,%eax
 1f4:	5f                   	pop    %edi
 1f5:	5d                   	pop    %ebp
 1f6:	c3                   	ret    
 1f7:	89 f6                	mov    %esi,%esi
 1f9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000200 <strchr>:

char*
strchr(const char *s, char c)
{
 200:	55                   	push   %ebp
 201:	89 e5                	mov    %esp,%ebp
 203:	53                   	push   %ebx
 204:	8b 45 08             	mov    0x8(%ebp),%eax
 207:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  for(; *s; s++)
 20a:	0f b6 10             	movzbl (%eax),%edx
 20d:	84 d2                	test   %dl,%dl
 20f:	74 1d                	je     22e <strchr+0x2e>
    if(*s == c)
 211:	38 d3                	cmp    %dl,%bl
 213:	89 d9                	mov    %ebx,%ecx
 215:	75 0d                	jne    224 <strchr+0x24>
 217:	eb 17                	jmp    230 <strchr+0x30>
 219:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 220:	38 ca                	cmp    %cl,%dl
 222:	74 0c                	je     230 <strchr+0x30>
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
 224:	83 c0 01             	add    $0x1,%eax
 227:	0f b6 10             	movzbl (%eax),%edx
 22a:	84 d2                	test   %dl,%dl
 22c:	75 f2                	jne    220 <strchr+0x20>
    if(*s == c)
      return (char*)s;
  return 0;
 22e:	31 c0                	xor    %eax,%eax
}
 230:	5b                   	pop    %ebx
 231:	5d                   	pop    %ebp
 232:	c3                   	ret    
 233:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 239:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000240 <gets>:

char*
gets(char *buf, int max)
{
 240:	55                   	push   %ebp
 241:	89 e5                	mov    %esp,%ebp
 243:	57                   	push   %edi
 244:	56                   	push   %esi
 245:	53                   	push   %ebx
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
 246:	31 f6                	xor    %esi,%esi
    cc = read(0, &c, 1);
 248:	8d 7d e7             	lea    -0x19(%ebp),%edi
  return 0;
}

char*
gets(char *buf, int max)
{
 24b:	83 ec 1c             	sub    $0x1c,%esp
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
 24e:	eb 29                	jmp    279 <gets+0x39>
    cc = read(0, &c, 1);
 250:	83 ec 04             	sub    $0x4,%esp
 253:	6a 01                	push   $0x1
 255:	57                   	push   %edi
 256:	6a 00                	push   $0x0
 258:	e8 2d 01 00 00       	call   38a <read>
    if(cc < 1)
 25d:	83 c4 10             	add    $0x10,%esp
 260:	85 c0                	test   %eax,%eax
 262:	7e 1d                	jle    281 <gets+0x41>
      break;
    buf[i++] = c;
 264:	0f b6 45 e7          	movzbl -0x19(%ebp),%eax
 268:	8b 55 08             	mov    0x8(%ebp),%edx
 26b:	89 de                	mov    %ebx,%esi
    if(c == '\n' || c == '\r')
 26d:	3c 0a                	cmp    $0xa,%al

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
 26f:	88 44 1a ff          	mov    %al,-0x1(%edx,%ebx,1)
    if(c == '\n' || c == '\r')
 273:	74 1b                	je     290 <gets+0x50>
 275:	3c 0d                	cmp    $0xd,%al
 277:	74 17                	je     290 <gets+0x50>
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
 279:	8d 5e 01             	lea    0x1(%esi),%ebx
 27c:	3b 5d 0c             	cmp    0xc(%ebp),%ebx
 27f:	7c cf                	jl     250 <gets+0x10>
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
 281:	8b 45 08             	mov    0x8(%ebp),%eax
 284:	c6 04 30 00          	movb   $0x0,(%eax,%esi,1)
  return buf;
}
 288:	8d 65 f4             	lea    -0xc(%ebp),%esp
 28b:	5b                   	pop    %ebx
 28c:	5e                   	pop    %esi
 28d:	5f                   	pop    %edi
 28e:	5d                   	pop    %ebp
 28f:	c3                   	ret    
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
 290:	8b 45 08             	mov    0x8(%ebp),%eax
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
 293:	89 de                	mov    %ebx,%esi
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
 295:	c6 04 30 00          	movb   $0x0,(%eax,%esi,1)
  return buf;
}
 299:	8d 65 f4             	lea    -0xc(%ebp),%esp
 29c:	5b                   	pop    %ebx
 29d:	5e                   	pop    %esi
 29e:	5f                   	pop    %edi
 29f:	5d                   	pop    %ebp
 2a0:	c3                   	ret    
 2a1:	eb 0d                	jmp    2b0 <stat>
 2a3:	90                   	nop
 2a4:	90                   	nop
 2a5:	90                   	nop
 2a6:	90                   	nop
 2a7:	90                   	nop
 2a8:	90                   	nop
 2a9:	90                   	nop
 2aa:	90                   	nop
 2ab:	90                   	nop
 2ac:	90                   	nop
 2ad:	90                   	nop
 2ae:	90                   	nop
 2af:	90                   	nop

000002b0 <stat>:

int
stat(char *n, struct stat *st)
{
 2b0:	55                   	push   %ebp
 2b1:	89 e5                	mov    %esp,%ebp
 2b3:	56                   	push   %esi
 2b4:	53                   	push   %ebx
  int fd;
  int r;

  fd = open(n, O_RDONLY);
 2b5:	83 ec 08             	sub    $0x8,%esp
 2b8:	6a 00                	push   $0x0
 2ba:	ff 75 08             	pushl  0x8(%ebp)
 2bd:	e8 f0 00 00 00       	call   3b2 <open>
  if(fd < 0)
 2c2:	83 c4 10             	add    $0x10,%esp
 2c5:	85 c0                	test   %eax,%eax
 2c7:	78 27                	js     2f0 <stat+0x40>
    return -1;
  r = fstat(fd, st);
 2c9:	83 ec 08             	sub    $0x8,%esp
 2cc:	ff 75 0c             	pushl  0xc(%ebp)
 2cf:	89 c3                	mov    %eax,%ebx
 2d1:	50                   	push   %eax
 2d2:	e8 f3 00 00 00       	call   3ca <fstat>
 2d7:	89 c6                	mov    %eax,%esi
  close(fd);
 2d9:	89 1c 24             	mov    %ebx,(%esp)
 2dc:	e8 b9 00 00 00       	call   39a <close>
  return r;
 2e1:	83 c4 10             	add    $0x10,%esp
 2e4:	89 f0                	mov    %esi,%eax
}
 2e6:	8d 65 f8             	lea    -0x8(%ebp),%esp
 2e9:	5b                   	pop    %ebx
 2ea:	5e                   	pop    %esi
 2eb:	5d                   	pop    %ebp
 2ec:	c3                   	ret    
 2ed:	8d 76 00             	lea    0x0(%esi),%esi
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
 2f0:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
 2f5:	eb ef                	jmp    2e6 <stat+0x36>
 2f7:	89 f6                	mov    %esi,%esi
 2f9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000300 <atoi>:
  return r;
}

int
atoi(const char *s)
{
 300:	55                   	push   %ebp
 301:	89 e5                	mov    %esp,%ebp
 303:	53                   	push   %ebx
 304:	8b 4d 08             	mov    0x8(%ebp),%ecx
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
 307:	0f be 11             	movsbl (%ecx),%edx
 30a:	8d 42 d0             	lea    -0x30(%edx),%eax
 30d:	3c 09                	cmp    $0x9,%al
 30f:	b8 00 00 00 00       	mov    $0x0,%eax
 314:	77 1f                	ja     335 <atoi+0x35>
 316:	8d 76 00             	lea    0x0(%esi),%esi
 319:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
    n = n*10 + *s++ - '0';
 320:	8d 04 80             	lea    (%eax,%eax,4),%eax
 323:	83 c1 01             	add    $0x1,%ecx
 326:	8d 44 42 d0          	lea    -0x30(%edx,%eax,2),%eax
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
 32a:	0f be 11             	movsbl (%ecx),%edx
 32d:	8d 5a d0             	lea    -0x30(%edx),%ebx
 330:	80 fb 09             	cmp    $0x9,%bl
 333:	76 eb                	jbe    320 <atoi+0x20>
    n = n*10 + *s++ - '0';
  return n;
}
 335:	5b                   	pop    %ebx
 336:	5d                   	pop    %ebp
 337:	c3                   	ret    
 338:	90                   	nop
 339:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00000340 <memmove>:

void*
memmove(void *vdst, void *vsrc, int n)
{
 340:	55                   	push   %ebp
 341:	89 e5                	mov    %esp,%ebp
 343:	56                   	push   %esi
 344:	53                   	push   %ebx
 345:	8b 5d 10             	mov    0x10(%ebp),%ebx
 348:	8b 45 08             	mov    0x8(%ebp),%eax
 34b:	8b 75 0c             	mov    0xc(%ebp),%esi
  char *dst, *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
 34e:	85 db                	test   %ebx,%ebx
 350:	7e 14                	jle    366 <memmove+0x26>
 352:	31 d2                	xor    %edx,%edx
 354:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    *dst++ = *src++;
 358:	0f b6 0c 16          	movzbl (%esi,%edx,1),%ecx
 35c:	88 0c 10             	mov    %cl,(%eax,%edx,1)
 35f:	83 c2 01             	add    $0x1,%edx
{
  char *dst, *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
 362:	39 da                	cmp    %ebx,%edx
 364:	75 f2                	jne    358 <memmove+0x18>
    *dst++ = *src++;
  return vdst;
}
 366:	5b                   	pop    %ebx
 367:	5e                   	pop    %esi
 368:	5d                   	pop    %ebp
 369:	c3                   	ret    

0000036a <fork>:
  name: \
    movl $SYS_ ## name, %eax; \
    int $T_SYSCALL; \
    ret

SYSCALL(fork)
 36a:	b8 01 00 00 00       	mov    $0x1,%eax
 36f:	cd 40                	int    $0x40
 371:	c3                   	ret    

00000372 <exit>:
SYSCALL(exit)
 372:	b8 02 00 00 00       	mov    $0x2,%eax
 377:	cd 40                	int    $0x40
 379:	c3                   	ret    

0000037a <wait>:
SYSCALL(wait)
 37a:	b8 03 00 00 00       	mov    $0x3,%eax
 37f:	cd 40                	int    $0x40
 381:	c3                   	ret    

00000382 <pipe>:
SYSCALL(pipe)
 382:	b8 04 00 00 00       	mov    $0x4,%eax
 387:	cd 40                	int    $0x40
 389:	c3                   	ret    

0000038a <read>:
SYSCALL(read)
 38a:	b8 05 00 00 00       	mov    $0x5,%eax
 38f:	cd 40                	int    $0x40
 391:	c3                   	ret    

00000392 <write>:
SYSCALL(write)
 392:	b8 10 00 00 00       	mov    $0x10,%eax
 397:	cd 40                	int    $0x40
 399:	c3                   	ret    

0000039a <close>:
SYSCALL(close)
 39a:	b8 15 00 00 00       	mov    $0x15,%eax
 39f:	cd 40                	int    $0x40
 3a1:	c3                   	ret    

000003a2 <kill>:
SYSCALL(kill)
 3a2:	b8 06 00 00 00       	mov    $0x6,%eax
 3a7:	cd 40                	int    $0x40
 3a9:	c3                   	ret    

000003aa <exec>:
SYSCALL(exec)
 3aa:	b8 07 00 00 00       	mov    $0x7,%eax
 3af:	cd 40                	int    $0x40
 3b1:	c3                   	ret    

000003b2 <open>:
SYSCALL(open)
 3b2:	b8 0f 00 00 00       	mov    $0xf,%eax
 3b7:	cd 40                	int    $0x40
 3b9:	c3                   	ret    

000003ba <mknod>:
SYSCALL(mknod)
 3ba:	b8 11 00 00 00       	mov    $0x11,%eax
 3bf:	cd 40                	int    $0x40
 3c1:	c3                   	ret    

000003c2 <unlink>:
SYSCALL(unlink)
 3c2:	b8 12 00 00 00       	mov    $0x12,%eax
 3c7:	cd 40                	int    $0x40
 3c9:	c3                   	ret    

000003ca <fstat>:
SYSCALL(fstat)
 3ca:	b8 08 00 00 00       	mov    $0x8,%eax
 3cf:	cd 40                	int    $0x40
 3d1:	c3                   	ret    

000003d2 <link>:
SYSCALL(link)
 3d2:	b8 13 00 00 00       	mov    $0x13,%eax
 3d7:	cd 40                	int    $0x40
 3d9:	c3                   	ret    

000003da <mkdir>:
SYSCALL(mkdir)
 3da:	b8 14 00 00 00       	mov    $0x14,%eax
 3df:	cd 40                	int    $0x40
 3e1:	c3                   	ret    

000003e2 <chdir>:
SYSCALL(chdir)
 3e2:	b8 09 00 00 00       	mov    $0x9,%eax
 3e7:	cd 40                	int    $0x40
 3e9:	c3                   	ret    

000003ea <dup>:
SYSCALL(dup)
 3ea:	b8 0a 00 00 00       	mov    $0xa,%eax
 3ef:	cd 40                	int    $0x40
 3f1:	c3                   	ret    

000003f2 <getpid>:
SYSCALL(getpid)
 3f2:	b8 0b 00 00 00       	mov    $0xb,%eax
 3f7:	cd 40                	int    $0x40
 3f9:	c3                   	ret    

000003fa <sbrk>:
SYSCALL(sbrk)
 3fa:	b8 0c 00 00 00       	mov    $0xc,%eax
 3ff:	cd 40                	int    $0x40
 401:	c3                   	ret    

00000402 <sleep>:
SYSCALL(sleep)
 402:	b8 0d 00 00 00       	mov    $0xd,%eax
 407:	cd 40                	int    $0x40
 409:	c3                   	ret    

0000040a <uptime>:
SYSCALL(uptime)
 40a:	b8 0e 00 00 00       	mov    $0xe,%eax
 40f:	cd 40                	int    $0x40
 411:	c3                   	ret    

00000412 <cps>:
SYSCALL(cps)
 412:	b8 16 00 00 00       	mov    $0x16,%eax
 417:	cd 40                	int    $0x40
 419:	c3                   	ret    

0000041a <chpr>:
SYSCALL(chpr)
 41a:	b8 17 00 00 00       	mov    $0x17,%eax
 41f:	cd 40                	int    $0x40
 421:	c3                   	ret    

00000422 <alarm>:
SYSCALL(alarm)
 422:	b8 18 00 00 00       	mov    $0x18,%eax
 427:	cd 40                	int    $0x40
 429:	c3                   	ret    
 42a:	66 90                	xchg   %ax,%ax
 42c:	66 90                	xchg   %ax,%ax
 42e:	66 90                	xchg   %ax,%ax

00000430 <printint>:
  write(fd, &c, 1);
}

static void
printint(int fd, int xx, int base, int sgn)
{
 430:	55                   	push   %ebp
 431:	89 e5                	mov    %esp,%ebp
 433:	57                   	push   %edi
 434:	56                   	push   %esi
 435:	53                   	push   %ebx
 436:	89 c6                	mov    %eax,%esi
 438:	83 ec 3c             	sub    $0x3c,%esp
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
 43b:	8b 5d 08             	mov    0x8(%ebp),%ebx
 43e:	85 db                	test   %ebx,%ebx
 440:	74 7e                	je     4c0 <printint+0x90>
 442:	89 d0                	mov    %edx,%eax
 444:	c1 e8 1f             	shr    $0x1f,%eax
 447:	84 c0                	test   %al,%al
 449:	74 75                	je     4c0 <printint+0x90>
    neg = 1;
    x = -xx;
 44b:	89 d0                	mov    %edx,%eax
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
 44d:	c7 45 c4 01 00 00 00 	movl   $0x1,-0x3c(%ebp)
    x = -xx;
 454:	f7 d8                	neg    %eax
 456:	89 75 c0             	mov    %esi,-0x40(%ebp)
  } else {
    x = xx;
  }

  i = 0;
 459:	31 ff                	xor    %edi,%edi
 45b:	8d 5d d7             	lea    -0x29(%ebp),%ebx
 45e:	89 ce                	mov    %ecx,%esi
 460:	eb 08                	jmp    46a <printint+0x3a>
 462:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  do{
    buf[i++] = digits[x % base];
 468:	89 cf                	mov    %ecx,%edi
 46a:	31 d2                	xor    %edx,%edx
 46c:	8d 4f 01             	lea    0x1(%edi),%ecx
 46f:	f7 f6                	div    %esi
 471:	0f b6 92 40 08 00 00 	movzbl 0x840(%edx),%edx
  }while((x /= base) != 0);
 478:	85 c0                	test   %eax,%eax
    x = xx;
  }

  i = 0;
  do{
    buf[i++] = digits[x % base];
 47a:	88 14 0b             	mov    %dl,(%ebx,%ecx,1)
  }while((x /= base) != 0);
 47d:	75 e9                	jne    468 <printint+0x38>
  if(neg)
 47f:	8b 45 c4             	mov    -0x3c(%ebp),%eax
 482:	8b 75 c0             	mov    -0x40(%ebp),%esi
 485:	85 c0                	test   %eax,%eax
 487:	74 08                	je     491 <printint+0x61>
    buf[i++] = '-';
 489:	c6 44 0d d8 2d       	movb   $0x2d,-0x28(%ebp,%ecx,1)
 48e:	8d 4f 02             	lea    0x2(%edi),%ecx
 491:	8d 7c 0d d7          	lea    -0x29(%ebp,%ecx,1),%edi
 495:	8d 76 00             	lea    0x0(%esi),%esi
 498:	0f b6 07             	movzbl (%edi),%eax
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
 49b:	83 ec 04             	sub    $0x4,%esp
 49e:	83 ef 01             	sub    $0x1,%edi
 4a1:	6a 01                	push   $0x1
 4a3:	53                   	push   %ebx
 4a4:	56                   	push   %esi
 4a5:	88 45 d7             	mov    %al,-0x29(%ebp)
 4a8:	e8 e5 fe ff ff       	call   392 <write>
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);
  if(neg)
    buf[i++] = '-';

  while(--i >= 0)
 4ad:	83 c4 10             	add    $0x10,%esp
 4b0:	39 df                	cmp    %ebx,%edi
 4b2:	75 e4                	jne    498 <printint+0x68>
    putc(fd, buf[i]);
}
 4b4:	8d 65 f4             	lea    -0xc(%ebp),%esp
 4b7:	5b                   	pop    %ebx
 4b8:	5e                   	pop    %esi
 4b9:	5f                   	pop    %edi
 4ba:	5d                   	pop    %ebp
 4bb:	c3                   	ret    
 4bc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
  } else {
    x = xx;
 4c0:	89 d0                	mov    %edx,%eax
  static char digits[] = "0123456789ABCDEF";
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
 4c2:	c7 45 c4 00 00 00 00 	movl   $0x0,-0x3c(%ebp)
 4c9:	eb 8b                	jmp    456 <printint+0x26>
 4cb:	90                   	nop
 4cc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

000004d0 <printf>:
}

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, char *fmt, ...)
{
 4d0:	55                   	push   %ebp
 4d1:	89 e5                	mov    %esp,%ebp
 4d3:	57                   	push   %edi
 4d4:	56                   	push   %esi
 4d5:	53                   	push   %ebx
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
 4d6:	8d 45 10             	lea    0x10(%ebp),%eax
}

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, char *fmt, ...)
{
 4d9:	83 ec 2c             	sub    $0x2c,%esp
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
 4dc:	8b 75 0c             	mov    0xc(%ebp),%esi
}

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, char *fmt, ...)
{
 4df:	8b 7d 08             	mov    0x8(%ebp),%edi
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
 4e2:	89 45 d0             	mov    %eax,-0x30(%ebp)
 4e5:	0f b6 1e             	movzbl (%esi),%ebx
 4e8:	83 c6 01             	add    $0x1,%esi
 4eb:	84 db                	test   %bl,%bl
 4ed:	0f 84 b0 00 00 00    	je     5a3 <printf+0xd3>
 4f3:	31 d2                	xor    %edx,%edx
 4f5:	eb 39                	jmp    530 <printf+0x60>
 4f7:	89 f6                	mov    %esi,%esi
 4f9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
 500:	83 f8 25             	cmp    $0x25,%eax
 503:	89 55 d4             	mov    %edx,-0x2c(%ebp)
        state = '%';
 506:	ba 25 00 00 00       	mov    $0x25,%edx
  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
 50b:	74 18                	je     525 <printf+0x55>
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
 50d:	8d 45 e2             	lea    -0x1e(%ebp),%eax
 510:	83 ec 04             	sub    $0x4,%esp
 513:	88 5d e2             	mov    %bl,-0x1e(%ebp)
 516:	6a 01                	push   $0x1
 518:	50                   	push   %eax
 519:	57                   	push   %edi
 51a:	e8 73 fe ff ff       	call   392 <write>
 51f:	8b 55 d4             	mov    -0x2c(%ebp),%edx
 522:	83 c4 10             	add    $0x10,%esp
 525:	83 c6 01             	add    $0x1,%esi
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
 528:	0f b6 5e ff          	movzbl -0x1(%esi),%ebx
 52c:	84 db                	test   %bl,%bl
 52e:	74 73                	je     5a3 <printf+0xd3>
    c = fmt[i] & 0xff;
    if(state == 0){
 530:	85 d2                	test   %edx,%edx
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;
 532:	0f be cb             	movsbl %bl,%ecx
 535:	0f b6 c3             	movzbl %bl,%eax
    if(state == 0){
 538:	74 c6                	je     500 <printf+0x30>
      if(c == '%'){
        state = '%';
      } else {
        putc(fd, c);
      }
    } else if(state == '%'){
 53a:	83 fa 25             	cmp    $0x25,%edx
 53d:	75 e6                	jne    525 <printf+0x55>
      if(c == 'd'){
 53f:	83 f8 64             	cmp    $0x64,%eax
 542:	0f 84 f8 00 00 00    	je     640 <printf+0x170>
        printint(fd, *ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
 548:	81 e1 f7 00 00 00    	and    $0xf7,%ecx
 54e:	83 f9 70             	cmp    $0x70,%ecx
 551:	74 5d                	je     5b0 <printf+0xe0>
        printint(fd, *ap, 16, 0);
        ap++;
      } else if(c == 's'){
 553:	83 f8 73             	cmp    $0x73,%eax
 556:	0f 84 84 00 00 00    	je     5e0 <printf+0x110>
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
        }
      } else if(c == 'c'){
 55c:	83 f8 63             	cmp    $0x63,%eax
 55f:	0f 84 ea 00 00 00    	je     64f <printf+0x17f>
        putc(fd, *ap);
        ap++;
      } else if(c == '%'){
 565:	83 f8 25             	cmp    $0x25,%eax
 568:	0f 84 c2 00 00 00    	je     630 <printf+0x160>
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
 56e:	8d 45 e7             	lea    -0x19(%ebp),%eax
 571:	83 ec 04             	sub    $0x4,%esp
 574:	c6 45 e7 25          	movb   $0x25,-0x19(%ebp)
 578:	6a 01                	push   $0x1
 57a:	50                   	push   %eax
 57b:	57                   	push   %edi
 57c:	e8 11 fe ff ff       	call   392 <write>
 581:	83 c4 0c             	add    $0xc,%esp
 584:	8d 45 e6             	lea    -0x1a(%ebp),%eax
 587:	88 5d e6             	mov    %bl,-0x1a(%ebp)
 58a:	6a 01                	push   $0x1
 58c:	50                   	push   %eax
 58d:	57                   	push   %edi
 58e:	83 c6 01             	add    $0x1,%esi
 591:	e8 fc fd ff ff       	call   392 <write>
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
 596:	0f b6 5e ff          	movzbl -0x1(%esi),%ebx
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
 59a:	83 c4 10             	add    $0x10,%esp
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
 59d:	31 d2                	xor    %edx,%edx
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
 59f:	84 db                	test   %bl,%bl
 5a1:	75 8d                	jne    530 <printf+0x60>
        putc(fd, c);
      }
      state = 0;
    }
  }
}
 5a3:	8d 65 f4             	lea    -0xc(%ebp),%esp
 5a6:	5b                   	pop    %ebx
 5a7:	5e                   	pop    %esi
 5a8:	5f                   	pop    %edi
 5a9:	5d                   	pop    %ebp
 5aa:	c3                   	ret    
 5ab:	90                   	nop
 5ac:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    } else if(state == '%'){
      if(c == 'd'){
        printint(fd, *ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
        printint(fd, *ap, 16, 0);
 5b0:	83 ec 0c             	sub    $0xc,%esp
 5b3:	b9 10 00 00 00       	mov    $0x10,%ecx
 5b8:	6a 00                	push   $0x0
 5ba:	8b 5d d0             	mov    -0x30(%ebp),%ebx
 5bd:	89 f8                	mov    %edi,%eax
 5bf:	8b 13                	mov    (%ebx),%edx
 5c1:	e8 6a fe ff ff       	call   430 <printint>
        ap++;
 5c6:	89 d8                	mov    %ebx,%eax
 5c8:	83 c4 10             	add    $0x10,%esp
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
 5cb:	31 d2                	xor    %edx,%edx
      if(c == 'd'){
        printint(fd, *ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
        printint(fd, *ap, 16, 0);
        ap++;
 5cd:	83 c0 04             	add    $0x4,%eax
 5d0:	89 45 d0             	mov    %eax,-0x30(%ebp)
 5d3:	e9 4d ff ff ff       	jmp    525 <printf+0x55>
 5d8:	90                   	nop
 5d9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
      } else if(c == 's'){
        s = (char*)*ap;
 5e0:	8b 45 d0             	mov    -0x30(%ebp),%eax
 5e3:	8b 18                	mov    (%eax),%ebx
        ap++;
 5e5:	83 c0 04             	add    $0x4,%eax
 5e8:	89 45 d0             	mov    %eax,-0x30(%ebp)
        if(s == 0)
          s = "(null)";
 5eb:	b8 38 08 00 00       	mov    $0x838,%eax
 5f0:	85 db                	test   %ebx,%ebx
 5f2:	0f 44 d8             	cmove  %eax,%ebx
        while(*s != 0){
 5f5:	0f b6 03             	movzbl (%ebx),%eax
 5f8:	84 c0                	test   %al,%al
 5fa:	74 23                	je     61f <printf+0x14f>
 5fc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 600:	88 45 e3             	mov    %al,-0x1d(%ebp)
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
 603:	8d 45 e3             	lea    -0x1d(%ebp),%eax
 606:	83 ec 04             	sub    $0x4,%esp
 609:	6a 01                	push   $0x1
        ap++;
        if(s == 0)
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
 60b:	83 c3 01             	add    $0x1,%ebx
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
 60e:	50                   	push   %eax
 60f:	57                   	push   %edi
 610:	e8 7d fd ff ff       	call   392 <write>
      } else if(c == 's'){
        s = (char*)*ap;
        ap++;
        if(s == 0)
          s = "(null)";
        while(*s != 0){
 615:	0f b6 03             	movzbl (%ebx),%eax
 618:	83 c4 10             	add    $0x10,%esp
 61b:	84 c0                	test   %al,%al
 61d:	75 e1                	jne    600 <printf+0x130>
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
 61f:	31 d2                	xor    %edx,%edx
 621:	e9 ff fe ff ff       	jmp    525 <printf+0x55>
 626:	8d 76 00             	lea    0x0(%esi),%esi
 629:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
 630:	83 ec 04             	sub    $0x4,%esp
 633:	88 5d e5             	mov    %bl,-0x1b(%ebp)
 636:	8d 45 e5             	lea    -0x1b(%ebp),%eax
 639:	6a 01                	push   $0x1
 63b:	e9 4c ff ff ff       	jmp    58c <printf+0xbc>
      } else {
        putc(fd, c);
      }
    } else if(state == '%'){
      if(c == 'd'){
        printint(fd, *ap, 10, 1);
 640:	83 ec 0c             	sub    $0xc,%esp
 643:	b9 0a 00 00 00       	mov    $0xa,%ecx
 648:	6a 01                	push   $0x1
 64a:	e9 6b ff ff ff       	jmp    5ba <printf+0xea>
 64f:	8b 5d d0             	mov    -0x30(%ebp),%ebx
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
 652:	83 ec 04             	sub    $0x4,%esp
 655:	8b 03                	mov    (%ebx),%eax
 657:	6a 01                	push   $0x1
 659:	88 45 e4             	mov    %al,-0x1c(%ebp)
 65c:	8d 45 e4             	lea    -0x1c(%ebp),%eax
 65f:	50                   	push   %eax
 660:	57                   	push   %edi
 661:	e8 2c fd ff ff       	call   392 <write>
 666:	e9 5b ff ff ff       	jmp    5c6 <printf+0xf6>
 66b:	66 90                	xchg   %ax,%ax
 66d:	66 90                	xchg   %ax,%ax
 66f:	90                   	nop

00000670 <free>:
static Header base;
static Header *freep;

void
free(void *ap)
{
 670:	55                   	push   %ebp
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 671:	a1 e4 0a 00 00       	mov    0xae4,%eax
static Header base;
static Header *freep;

void
free(void *ap)
{
 676:	89 e5                	mov    %esp,%ebp
 678:	57                   	push   %edi
 679:	56                   	push   %esi
 67a:	53                   	push   %ebx
 67b:	8b 5d 08             	mov    0x8(%ebp),%ebx
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 67e:	8b 10                	mov    (%eax),%edx
void
free(void *ap)
{
  Header *bp, *p;

  bp = (Header*)ap - 1;
 680:	8d 4b f8             	lea    -0x8(%ebx),%ecx
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 683:	39 c8                	cmp    %ecx,%eax
 685:	73 19                	jae    6a0 <free+0x30>
 687:	89 f6                	mov    %esi,%esi
 689:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
 690:	39 d1                	cmp    %edx,%ecx
 692:	72 1c                	jb     6b0 <free+0x40>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 694:	39 d0                	cmp    %edx,%eax
 696:	73 18                	jae    6b0 <free+0x40>
static Header base;
static Header *freep;

void
free(void *ap)
{
 698:	89 d0                	mov    %edx,%eax
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 69a:	39 c8                	cmp    %ecx,%eax
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 69c:	8b 10                	mov    (%eax),%edx
free(void *ap)
{
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 69e:	72 f0                	jb     690 <free+0x20>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 6a0:	39 d0                	cmp    %edx,%eax
 6a2:	72 f4                	jb     698 <free+0x28>
 6a4:	39 d1                	cmp    %edx,%ecx
 6a6:	73 f0                	jae    698 <free+0x28>
 6a8:	90                   	nop
 6a9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
      break;
  if(bp + bp->s.size == p->s.ptr){
 6b0:	8b 73 fc             	mov    -0x4(%ebx),%esi
 6b3:	8d 3c f1             	lea    (%ecx,%esi,8),%edi
 6b6:	39 d7                	cmp    %edx,%edi
 6b8:	74 19                	je     6d3 <free+0x63>
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
 6ba:	89 53 f8             	mov    %edx,-0x8(%ebx)
  if(p + p->s.size == bp){
 6bd:	8b 50 04             	mov    0x4(%eax),%edx
 6c0:	8d 34 d0             	lea    (%eax,%edx,8),%esi
 6c3:	39 f1                	cmp    %esi,%ecx
 6c5:	74 23                	je     6ea <free+0x7a>
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
 6c7:	89 08                	mov    %ecx,(%eax)
  freep = p;
 6c9:	a3 e4 0a 00 00       	mov    %eax,0xae4
}
 6ce:	5b                   	pop    %ebx
 6cf:	5e                   	pop    %esi
 6d0:	5f                   	pop    %edi
 6d1:	5d                   	pop    %ebp
 6d2:	c3                   	ret    
  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  if(bp + bp->s.size == p->s.ptr){
    bp->s.size += p->s.ptr->s.size;
 6d3:	03 72 04             	add    0x4(%edx),%esi
 6d6:	89 73 fc             	mov    %esi,-0x4(%ebx)
    bp->s.ptr = p->s.ptr->s.ptr;
 6d9:	8b 10                	mov    (%eax),%edx
 6db:	8b 12                	mov    (%edx),%edx
 6dd:	89 53 f8             	mov    %edx,-0x8(%ebx)
  } else
    bp->s.ptr = p->s.ptr;
  if(p + p->s.size == bp){
 6e0:	8b 50 04             	mov    0x4(%eax),%edx
 6e3:	8d 34 d0             	lea    (%eax,%edx,8),%esi
 6e6:	39 f1                	cmp    %esi,%ecx
 6e8:	75 dd                	jne    6c7 <free+0x57>
    p->s.size += bp->s.size;
 6ea:	03 53 fc             	add    -0x4(%ebx),%edx
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
 6ed:	a3 e4 0a 00 00       	mov    %eax,0xae4
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if(p + p->s.size == bp){
    p->s.size += bp->s.size;
 6f2:	89 50 04             	mov    %edx,0x4(%eax)
    p->s.ptr = bp->s.ptr;
 6f5:	8b 53 f8             	mov    -0x8(%ebx),%edx
 6f8:	89 10                	mov    %edx,(%eax)
  } else
    p->s.ptr = bp;
  freep = p;
}
 6fa:	5b                   	pop    %ebx
 6fb:	5e                   	pop    %esi
 6fc:	5f                   	pop    %edi
 6fd:	5d                   	pop    %ebp
 6fe:	c3                   	ret    
 6ff:	90                   	nop

00000700 <malloc>:
  return freep;
}

void*
malloc(uint nbytes)
{
 700:	55                   	push   %ebp
 701:	89 e5                	mov    %esp,%ebp
 703:	57                   	push   %edi
 704:	56                   	push   %esi
 705:	53                   	push   %ebx
 706:	83 ec 0c             	sub    $0xc,%esp
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 709:	8b 45 08             	mov    0x8(%ebp),%eax
  if((prevp = freep) == 0){
 70c:	8b 15 e4 0a 00 00    	mov    0xae4,%edx
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 712:	8d 78 07             	lea    0x7(%eax),%edi
 715:	c1 ef 03             	shr    $0x3,%edi
 718:	83 c7 01             	add    $0x1,%edi
  if((prevp = freep) == 0){
 71b:	85 d2                	test   %edx,%edx
 71d:	0f 84 a3 00 00 00    	je     7c6 <malloc+0xc6>
 723:	8b 02                	mov    (%edx),%eax
 725:	8b 48 04             	mov    0x4(%eax),%ecx
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
 728:	39 cf                	cmp    %ecx,%edi
 72a:	76 74                	jbe    7a0 <malloc+0xa0>
 72c:	81 ff 00 10 00 00    	cmp    $0x1000,%edi
 732:	be 00 10 00 00       	mov    $0x1000,%esi
 737:	8d 1c fd 00 00 00 00 	lea    0x0(,%edi,8),%ebx
 73e:	0f 43 f7             	cmovae %edi,%esi
 741:	ba 00 80 00 00       	mov    $0x8000,%edx
 746:	81 ff ff 0f 00 00    	cmp    $0xfff,%edi
 74c:	0f 46 da             	cmovbe %edx,%ebx
 74f:	eb 10                	jmp    761 <malloc+0x61>
 751:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
  if((prevp = freep) == 0){
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
 758:	8b 02                	mov    (%edx),%eax
    if(p->s.size >= nunits){
 75a:	8b 48 04             	mov    0x4(%eax),%ecx
 75d:	39 cf                	cmp    %ecx,%edi
 75f:	76 3f                	jbe    7a0 <malloc+0xa0>
        p->s.size = nunits;
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
 761:	39 05 e4 0a 00 00    	cmp    %eax,0xae4
 767:	89 c2                	mov    %eax,%edx
 769:	75 ed                	jne    758 <malloc+0x58>
  char *p;
  Header *hp;

  if(nu < 4096)
    nu = 4096;
  p = sbrk(nu * sizeof(Header));
 76b:	83 ec 0c             	sub    $0xc,%esp
 76e:	53                   	push   %ebx
 76f:	e8 86 fc ff ff       	call   3fa <sbrk>
  if(p == (char*)-1)
 774:	83 c4 10             	add    $0x10,%esp
 777:	83 f8 ff             	cmp    $0xffffffff,%eax
 77a:	74 1c                	je     798 <malloc+0x98>
    return 0;
  hp = (Header*)p;
  hp->s.size = nu;
 77c:	89 70 04             	mov    %esi,0x4(%eax)
  free((void*)(hp + 1));
 77f:	83 ec 0c             	sub    $0xc,%esp
 782:	83 c0 08             	add    $0x8,%eax
 785:	50                   	push   %eax
 786:	e8 e5 fe ff ff       	call   670 <free>
  return freep;
 78b:	8b 15 e4 0a 00 00    	mov    0xae4,%edx
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
      if((p = morecore(nunits)) == 0)
 791:	83 c4 10             	add    $0x10,%esp
 794:	85 d2                	test   %edx,%edx
 796:	75 c0                	jne    758 <malloc+0x58>
        return 0;
 798:	31 c0                	xor    %eax,%eax
 79a:	eb 1c                	jmp    7b8 <malloc+0xb8>
 79c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
      if(p->s.size == nunits)
 7a0:	39 cf                	cmp    %ecx,%edi
 7a2:	74 1c                	je     7c0 <malloc+0xc0>
        prevp->s.ptr = p->s.ptr;
      else {
        p->s.size -= nunits;
 7a4:	29 f9                	sub    %edi,%ecx
 7a6:	89 48 04             	mov    %ecx,0x4(%eax)
        p += p->s.size;
 7a9:	8d 04 c8             	lea    (%eax,%ecx,8),%eax
        p->s.size = nunits;
 7ac:	89 78 04             	mov    %edi,0x4(%eax)
      }
      freep = prevp;
 7af:	89 15 e4 0a 00 00    	mov    %edx,0xae4
      return (void*)(p + 1);
 7b5:	83 c0 08             	add    $0x8,%eax
    }
    if(p == freep)
      if((p = morecore(nunits)) == 0)
        return 0;
  }
}
 7b8:	8d 65 f4             	lea    -0xc(%ebp),%esp
 7bb:	5b                   	pop    %ebx
 7bc:	5e                   	pop    %esi
 7bd:	5f                   	pop    %edi
 7be:	5d                   	pop    %ebp
 7bf:	c3                   	ret    
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
      if(p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
 7c0:	8b 08                	mov    (%eax),%ecx
 7c2:	89 0a                	mov    %ecx,(%edx)
 7c4:	eb e9                	jmp    7af <malloc+0xaf>
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
  if((prevp = freep) == 0){
    base.s.ptr = freep = prevp = &base;
 7c6:	c7 05 e4 0a 00 00 e8 	movl   $0xae8,0xae4
 7cd:	0a 00 00 
 7d0:	c7 05 e8 0a 00 00 e8 	movl   $0xae8,0xae8
 7d7:	0a 00 00 
    base.s.size = 0;
 7da:	b8 e8 0a 00 00       	mov    $0xae8,%eax
 7df:	c7 05 ec 0a 00 00 00 	movl   $0x0,0xaec
 7e6:	00 00 00 
 7e9:	e9 3e ff ff ff       	jmp    72c <malloc+0x2c>
