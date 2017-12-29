
_foo:     file format elf32-i386


Disassembly of section .text:

00000000 <main>:
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
  int k, n, id;
  double x = 0, z ,d;

  int *memory=0;
  memory=memory;
  if(argc < 2)
  19:	83 fb 01             	cmp    $0x1,%ebx
  1c:	0f 8e 93 00 00 00    	jle    b5 <main+0xb5>
    n = 1;
  else 
    n = atoi (argv[1]);
  22:	83 ec 0c             	sub    $0xc,%esp
  25:	ff 76 04             	pushl  0x4(%esi)
  28:	e8 43 03 00 00       	call   370 <atoi>
    if( n < 0 || n > 20)
  2d:	83 c4 10             	add    $0x10,%esp
  30:	83 f8 14             	cmp    $0x14,%eax
  int *memory=0;
  memory=memory;
  if(argc < 2)
    n = 1;
  else 
    n = atoi (argv[1]);
  33:	89 c7                	mov    %eax,%edi
    if( n < 0 || n > 20)
  35:	0f 86 0e 01 00 00    	jbe    149 <main+0x149>
      n = 2;

  if (argc < 3 )
  3b:	83 fb 02             	cmp    $0x2,%ebx
  3e:	0f 85 bd 00 00 00    	jne    101 <main+0x101>
  if (argc < 4 )
    m = 10;
  else
   m = atoi (argv[3]);
   
   memory=malloc(m);
  44:	83 ec 0c             	sub    $0xc,%esp
  if(argc < 2)
    n = 1;
  else 
    n = atoi (argv[1]);
    if( n < 0 || n > 20)
      n = 2;
  47:	bf 02 00 00 00       	mov    $0x2,%edi
  if (argc < 4 )
    m = 10;
  else
   m = atoi (argv[3]);
   
   memory=malloc(m);
  4c:	6a 0a                	push   $0xa
  4e:	e8 1d 07 00 00       	call   770 <malloc>
    n = atoi (argv[1]);
    if( n < 0 || n > 20)
      n = 2;

  if (argc < 3 )
    d = 1.0;
  53:	d9 e8                	fld1   
  if (argc < 4 )
    m = 10;
  else
   m = atoi (argv[3]);
   
   memory=malloc(m);
  55:	83 c4 10             	add    $0x10,%esp
    n = atoi (argv[1]);
    if( n < 0 || n > 20)
      n = 2;

  if (argc < 3 )
    d = 1.0;
  58:	dd 5d e0             	fstpl  -0x20(%ebp)
  if(argc < 2)
    n = 1;
  else 
    n = atoi (argv[1]);
    if( n < 0 || n > 20)
      n = 2;
  5b:	31 db                	xor    %ebx,%ebx
  5d:	eb 25                	jmp    84 <main+0x84>
  5f:	90                   	nop
  id = 0;
  for( k = 0; k < n; k++ ){
    id = fork();
    if ( id < 0 ){
      printf(1,"%d failed in fork!\n",getpid());
    }else if ( id > 0 ){
  60:	74 6c                	je     ce <main+0xce>
      printf(1,"Parent %d creating child %d\n",getpid(),id);
  62:	e8 fb 03 00 00       	call   462 <getpid>
  67:	56                   	push   %esi
  68:	50                   	push   %eax
   memory=malloc(m);


  x = 0;
  id = 0;
  for( k = 0; k < n; k++ ){
  69:	83 c3 01             	add    $0x1,%ebx
    id = fork();
    if ( id < 0 ){
      printf(1,"%d failed in fork!\n",getpid());
    }else if ( id > 0 ){
      printf(1,"Parent %d creating child %d\n",getpid(),id);
  6c:	68 74 08 00 00       	push   $0x874
  71:	6a 01                	push   $0x1
  73:	e8 c8 04 00 00       	call   540 <printf>
      wait();   
  78:	e8 6d 03 00 00       	call   3ea <wait>
  7d:	83 c4 10             	add    $0x10,%esp
   memory=malloc(m);


  x = 0;
  id = 0;
  for( k = 0; k < n; k++ ){
  80:	39 fb                	cmp    %edi,%ebx
  82:	7d 2c                	jge    b0 <main+0xb0>
    id = fork();
  84:	e8 51 03 00 00       	call   3da <fork>
    if ( id < 0 ){
  89:	85 c0                	test   %eax,%eax


  x = 0;
  id = 0;
  for( k = 0; k < n; k++ ){
    id = fork();
  8b:	89 c6                	mov    %eax,%esi
    if ( id < 0 ){
  8d:	79 d1                	jns    60 <main+0x60>
      printf(1,"%d failed in fork!\n",getpid());
  8f:	e8 ce 03 00 00       	call   462 <getpid>
  94:	83 ec 04             	sub    $0x4,%esp
   memory=malloc(m);


  x = 0;
  id = 0;
  for( k = 0; k < n; k++ ){
  97:	83 c3 01             	add    $0x1,%ebx
    id = fork();
    if ( id < 0 ){
      printf(1,"%d failed in fork!\n",getpid());
  9a:	50                   	push   %eax
  9b:	68 60 08 00 00       	push   $0x860
  a0:	6a 01                	push   $0x1
  a2:	e8 99 04 00 00       	call   540 <printf>
  a7:	83 c4 10             	add    $0x10,%esp
   memory=malloc(m);


  x = 0;
  id = 0;
  for( k = 0; k < n; k++ ){
  aa:	39 fb                	cmp    %edi,%ebx
  ac:	7c d6                	jl     84 <main+0x84>
  ae:	66 90                	xchg   %ax,%ax
      for(z=0;z<8000000.0; z+=d)
        x = x + 3.14 * 89.64;
      break;
    }
  }
  exit();
  b0:	e8 2d 03 00 00       	call   3e2 <exit>
  if (argc < 4 )
    m = 10;
  else
   m = atoi (argv[3]);
   
   memory=malloc(m);
  b5:	83 ec 0c             	sub    $0xc,%esp
  double x = 0, z ,d;

  int *memory=0;
  memory=memory;
  if(argc < 2)
    n = 1;
  b8:	bf 01 00 00 00       	mov    $0x1,%edi
  if (argc < 4 )
    m = 10;
  else
   m = atoi (argv[3]);
   
   memory=malloc(m);
  bd:	6a 0a                	push   $0xa
  bf:	e8 ac 06 00 00       	call   770 <malloc>
    n = atoi (argv[1]);
    if( n < 0 || n > 20)
      n = 2;

  if (argc < 3 )
    d = 1.0;
  c4:	d9 e8                	fld1   
  if (argc < 4 )
    m = 10;
  else
   m = atoi (argv[3]);
   
   memory=malloc(m);
  c6:	83 c4 10             	add    $0x10,%esp
    n = atoi (argv[1]);
    if( n < 0 || n > 20)
      n = 2;

  if (argc < 3 )
    d = 1.0;
  c9:	dd 5d e0             	fstpl  -0x20(%ebp)
  cc:	eb 8d                	jmp    5b <main+0x5b>
      printf(1,"%d failed in fork!\n",getpid());
    }else if ( id > 0 ){
      printf(1,"Parent %d creating child %d\n",getpid(),id);
      wait();   
    }else {
      printf(1,"Child %d created\n",getpid());
  ce:	e8 8f 03 00 00       	call   462 <getpid>
  d3:	52                   	push   %edx
  d4:	50                   	push   %eax
  d5:	68 91 08 00 00       	push   $0x891
  da:	6a 01                	push   $0x1
  dc:	e8 5f 04 00 00       	call   540 <printf>
      for(z=0;z<8000000.0; z+=d)
  e1:	d9 ee                	fldz   
      printf(1,"%d failed in fork!\n",getpid());
    }else if ( id > 0 ){
      printf(1,"Parent %d creating child %d\n",getpid(),id);
      wait();   
    }else {
      printf(1,"Child %d created\n",getpid());
  e3:	83 c4 10             	add    $0x10,%esp
      for(z=0;z<8000000.0; z+=d)
  e6:	d9 05 a4 08 00 00    	flds   0x8a4
  ec:	d9 c9                	fxch   %st(1)
  ee:	eb 02                	jmp    f2 <main+0xf2>
  f0:	d9 c9                	fxch   %st(1)
  f2:	dc 45 e0             	faddl  -0x20(%ebp)
  f5:	d9 c9                	fxch   %st(1)
  f7:	db e9                	fucomi %st(1),%st
  f9:	77 f5                	ja     f0 <main+0xf0>
  fb:	dd d8                	fstp   %st(0)
  fd:	dd d8                	fstp   %st(0)
  ff:	eb af                	jmp    b0 <main+0xb0>
      n = 2;

  if (argc < 3 )
    d = 1.0;
  else
    d = atoi (argv[2]);
 101:	83 ec 0c             	sub    $0xc,%esp
 104:	ff 76 08             	pushl  0x8(%esi)
 107:	e8 64 02 00 00       	call   370 <atoi>
 10c:	89 45 e0             	mov    %eax,-0x20(%ebp)
  
  int m;
  if (argc < 4 )
 10f:	83 c4 10             	add    $0x10,%esp
 112:	83 fb 03             	cmp    $0x3,%ebx
      n = 2;

  if (argc < 3 )
    d = 1.0;
  else
    d = atoi (argv[2]);
 115:	db 45 e0             	fildl  -0x20(%ebp)
 118:	dd 5d e0             	fstpl  -0x20(%ebp)
  
  int m;
  if (argc < 4 )
 11b:	74 60                	je     17d <main+0x17d>
  if(argc < 2)
    n = 1;
  else 
    n = atoi (argv[1]);
    if( n < 0 || n > 20)
      n = 2;
 11d:	bf 02 00 00 00       	mov    $0x2,%edi
  
  int m;
  if (argc < 4 )
    m = 10;
  else
   m = atoi (argv[3]);
 122:	83 ec 0c             	sub    $0xc,%esp
 125:	ff 76 0c             	pushl  0xc(%esi)
 128:	e8 43 02 00 00       	call   370 <atoi>
 12d:	83 c4 10             	add    $0x10,%esp
   
   memory=malloc(m);
 130:	83 ec 0c             	sub    $0xc,%esp
 133:	50                   	push   %eax
 134:	e8 37 06 00 00       	call   770 <malloc>


  x = 0;
  id = 0;
  for( k = 0; k < n; k++ ){
 139:	83 c4 10             	add    $0x10,%esp
 13c:	85 ff                	test   %edi,%edi
 13e:	0f 85 17 ff ff ff    	jne    5b <main+0x5b>
 144:	e9 67 ff ff ff       	jmp    b0 <main+0xb0>
  else 
    n = atoi (argv[1]);
    if( n < 0 || n > 20)
      n = 2;

  if (argc < 3 )
 149:	83 fb 02             	cmp    $0x2,%ebx
 14c:	75 0c                	jne    15a <main+0x15a>
    d = 1.0;
 14e:	d9 e8                	fld1   
  else
    d = atoi (argv[2]);
  
  int m;
  if (argc < 4 )
    m = 10;
 150:	b8 0a 00 00 00       	mov    $0xa,%eax
    n = atoi (argv[1]);
    if( n < 0 || n > 20)
      n = 2;

  if (argc < 3 )
    d = 1.0;
 155:	dd 5d e0             	fstpl  -0x20(%ebp)
 158:	eb d6                	jmp    130 <main+0x130>
  else
    d = atoi (argv[2]);
 15a:	83 ec 0c             	sub    $0xc,%esp
 15d:	ff 76 08             	pushl  0x8(%esi)
 160:	e8 0b 02 00 00       	call   370 <atoi>
 165:	89 45 e0             	mov    %eax,-0x20(%ebp)
  
  int m;
  if (argc < 4 )
 168:	83 c4 10             	add    $0x10,%esp
 16b:	83 fb 03             	cmp    $0x3,%ebx
      n = 2;

  if (argc < 3 )
    d = 1.0;
  else
    d = atoi (argv[2]);
 16e:	db 45 e0             	fildl  -0x20(%ebp)
  
  int m;
  if (argc < 4 )
    m = 10;
 171:	b8 0a 00 00 00       	mov    $0xa,%eax
      n = 2;

  if (argc < 3 )
    d = 1.0;
  else
    d = atoi (argv[2]);
 176:	dd 5d e0             	fstpl  -0x20(%ebp)
  
  int m;
  if (argc < 4 )
 179:	7f a7                	jg     122 <main+0x122>
 17b:	eb b3                	jmp    130 <main+0x130>
    m = 10;
  else
   m = atoi (argv[3]);
   
   memory=malloc(m);
 17d:	83 ec 0c             	sub    $0xc,%esp
  if(argc < 2)
    n = 1;
  else 
    n = atoi (argv[1]);
    if( n < 0 || n > 20)
      n = 2;
 180:	bf 02 00 00 00       	mov    $0x2,%edi
  if (argc < 4 )
    m = 10;
  else
   m = atoi (argv[3]);
   
   memory=malloc(m);
 185:	6a 0a                	push   $0xa
 187:	e8 e4 05 00 00       	call   770 <malloc>
 18c:	83 c4 10             	add    $0x10,%esp
 18f:	e9 c7 fe ff ff       	jmp    5b <main+0x5b>
 194:	66 90                	xchg   %ax,%ax
 196:	66 90                	xchg   %ax,%ax
 198:	66 90                	xchg   %ax,%ax
 19a:	66 90                	xchg   %ax,%ax
 19c:	66 90                	xchg   %ax,%ax
 19e:	66 90                	xchg   %ax,%ax

000001a0 <strcpy>:
 1a0:	55                   	push   %ebp
 1a1:	89 e5                	mov    %esp,%ebp
 1a3:	53                   	push   %ebx
 1a4:	8b 45 08             	mov    0x8(%ebp),%eax
 1a7:	8b 4d 0c             	mov    0xc(%ebp),%ecx
 1aa:	89 c2                	mov    %eax,%edx
 1ac:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 1b0:	83 c1 01             	add    $0x1,%ecx
 1b3:	0f b6 59 ff          	movzbl -0x1(%ecx),%ebx
 1b7:	83 c2 01             	add    $0x1,%edx
 1ba:	84 db                	test   %bl,%bl
 1bc:	88 5a ff             	mov    %bl,-0x1(%edx)
 1bf:	75 ef                	jne    1b0 <strcpy+0x10>
 1c1:	5b                   	pop    %ebx
 1c2:	5d                   	pop    %ebp
 1c3:	c3                   	ret    
 1c4:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 1ca:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

000001d0 <strcmp>:
 1d0:	55                   	push   %ebp
 1d1:	89 e5                	mov    %esp,%ebp
 1d3:	56                   	push   %esi
 1d4:	53                   	push   %ebx
 1d5:	8b 55 08             	mov    0x8(%ebp),%edx
 1d8:	8b 4d 0c             	mov    0xc(%ebp),%ecx
 1db:	0f b6 02             	movzbl (%edx),%eax
 1de:	0f b6 19             	movzbl (%ecx),%ebx
 1e1:	84 c0                	test   %al,%al
 1e3:	75 1e                	jne    203 <strcmp+0x33>
 1e5:	eb 29                	jmp    210 <strcmp+0x40>
 1e7:	89 f6                	mov    %esi,%esi
 1e9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
 1f0:	83 c2 01             	add    $0x1,%edx
 1f3:	0f b6 02             	movzbl (%edx),%eax
 1f6:	8d 71 01             	lea    0x1(%ecx),%esi
 1f9:	0f b6 59 01          	movzbl 0x1(%ecx),%ebx
 1fd:	84 c0                	test   %al,%al
 1ff:	74 0f                	je     210 <strcmp+0x40>
 201:	89 f1                	mov    %esi,%ecx
 203:	38 d8                	cmp    %bl,%al
 205:	74 e9                	je     1f0 <strcmp+0x20>
 207:	29 d8                	sub    %ebx,%eax
 209:	5b                   	pop    %ebx
 20a:	5e                   	pop    %esi
 20b:	5d                   	pop    %ebp
 20c:	c3                   	ret    
 20d:	8d 76 00             	lea    0x0(%esi),%esi
 210:	31 c0                	xor    %eax,%eax
 212:	29 d8                	sub    %ebx,%eax
 214:	5b                   	pop    %ebx
 215:	5e                   	pop    %esi
 216:	5d                   	pop    %ebp
 217:	c3                   	ret    
 218:	90                   	nop
 219:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00000220 <strlen>:
 220:	55                   	push   %ebp
 221:	89 e5                	mov    %esp,%ebp
 223:	8b 4d 08             	mov    0x8(%ebp),%ecx
 226:	80 39 00             	cmpb   $0x0,(%ecx)
 229:	74 12                	je     23d <strlen+0x1d>
 22b:	31 d2                	xor    %edx,%edx
 22d:	8d 76 00             	lea    0x0(%esi),%esi
 230:	83 c2 01             	add    $0x1,%edx
 233:	80 3c 11 00          	cmpb   $0x0,(%ecx,%edx,1)
 237:	89 d0                	mov    %edx,%eax
 239:	75 f5                	jne    230 <strlen+0x10>
 23b:	5d                   	pop    %ebp
 23c:	c3                   	ret    
 23d:	31 c0                	xor    %eax,%eax
 23f:	5d                   	pop    %ebp
 240:	c3                   	ret    
 241:	eb 0d                	jmp    250 <memset>
 243:	90                   	nop
 244:	90                   	nop
 245:	90                   	nop
 246:	90                   	nop
 247:	90                   	nop
 248:	90                   	nop
 249:	90                   	nop
 24a:	90                   	nop
 24b:	90                   	nop
 24c:	90                   	nop
 24d:	90                   	nop
 24e:	90                   	nop
 24f:	90                   	nop

00000250 <memset>:
 250:	55                   	push   %ebp
 251:	89 e5                	mov    %esp,%ebp
 253:	57                   	push   %edi
 254:	8b 55 08             	mov    0x8(%ebp),%edx
 257:	8b 4d 10             	mov    0x10(%ebp),%ecx
 25a:	8b 45 0c             	mov    0xc(%ebp),%eax
 25d:	89 d7                	mov    %edx,%edi
 25f:	fc                   	cld    
 260:	f3 aa                	rep stos %al,%es:(%edi)
 262:	89 d0                	mov    %edx,%eax
 264:	5f                   	pop    %edi
 265:	5d                   	pop    %ebp
 266:	c3                   	ret    
 267:	89 f6                	mov    %esi,%esi
 269:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000270 <strchr>:
 270:	55                   	push   %ebp
 271:	89 e5                	mov    %esp,%ebp
 273:	53                   	push   %ebx
 274:	8b 45 08             	mov    0x8(%ebp),%eax
 277:	8b 5d 0c             	mov    0xc(%ebp),%ebx
 27a:	0f b6 10             	movzbl (%eax),%edx
 27d:	84 d2                	test   %dl,%dl
 27f:	74 1d                	je     29e <strchr+0x2e>
 281:	38 d3                	cmp    %dl,%bl
 283:	89 d9                	mov    %ebx,%ecx
 285:	75 0d                	jne    294 <strchr+0x24>
 287:	eb 17                	jmp    2a0 <strchr+0x30>
 289:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 290:	38 ca                	cmp    %cl,%dl
 292:	74 0c                	je     2a0 <strchr+0x30>
 294:	83 c0 01             	add    $0x1,%eax
 297:	0f b6 10             	movzbl (%eax),%edx
 29a:	84 d2                	test   %dl,%dl
 29c:	75 f2                	jne    290 <strchr+0x20>
 29e:	31 c0                	xor    %eax,%eax
 2a0:	5b                   	pop    %ebx
 2a1:	5d                   	pop    %ebp
 2a2:	c3                   	ret    
 2a3:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 2a9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

000002b0 <gets>:
 2b0:	55                   	push   %ebp
 2b1:	89 e5                	mov    %esp,%ebp
 2b3:	57                   	push   %edi
 2b4:	56                   	push   %esi
 2b5:	53                   	push   %ebx
 2b6:	31 f6                	xor    %esi,%esi
 2b8:	8d 7d e7             	lea    -0x19(%ebp),%edi
 2bb:	83 ec 1c             	sub    $0x1c,%esp
 2be:	eb 29                	jmp    2e9 <gets+0x39>
 2c0:	83 ec 04             	sub    $0x4,%esp
 2c3:	6a 01                	push   $0x1
 2c5:	57                   	push   %edi
 2c6:	6a 00                	push   $0x0
 2c8:	e8 2d 01 00 00       	call   3fa <read>
 2cd:	83 c4 10             	add    $0x10,%esp
 2d0:	85 c0                	test   %eax,%eax
 2d2:	7e 1d                	jle    2f1 <gets+0x41>
 2d4:	0f b6 45 e7          	movzbl -0x19(%ebp),%eax
 2d8:	8b 55 08             	mov    0x8(%ebp),%edx
 2db:	89 de                	mov    %ebx,%esi
 2dd:	3c 0a                	cmp    $0xa,%al
 2df:	88 44 1a ff          	mov    %al,-0x1(%edx,%ebx,1)
 2e3:	74 1b                	je     300 <gets+0x50>
 2e5:	3c 0d                	cmp    $0xd,%al
 2e7:	74 17                	je     300 <gets+0x50>
 2e9:	8d 5e 01             	lea    0x1(%esi),%ebx
 2ec:	3b 5d 0c             	cmp    0xc(%ebp),%ebx
 2ef:	7c cf                	jl     2c0 <gets+0x10>
 2f1:	8b 45 08             	mov    0x8(%ebp),%eax
 2f4:	c6 04 30 00          	movb   $0x0,(%eax,%esi,1)
 2f8:	8d 65 f4             	lea    -0xc(%ebp),%esp
 2fb:	5b                   	pop    %ebx
 2fc:	5e                   	pop    %esi
 2fd:	5f                   	pop    %edi
 2fe:	5d                   	pop    %ebp
 2ff:	c3                   	ret    
 300:	8b 45 08             	mov    0x8(%ebp),%eax
 303:	89 de                	mov    %ebx,%esi
 305:	c6 04 30 00          	movb   $0x0,(%eax,%esi,1)
 309:	8d 65 f4             	lea    -0xc(%ebp),%esp
 30c:	5b                   	pop    %ebx
 30d:	5e                   	pop    %esi
 30e:	5f                   	pop    %edi
 30f:	5d                   	pop    %ebp
 310:	c3                   	ret    
 311:	eb 0d                	jmp    320 <stat>
 313:	90                   	nop
 314:	90                   	nop
 315:	90                   	nop
 316:	90                   	nop
 317:	90                   	nop
 318:	90                   	nop
 319:	90                   	nop
 31a:	90                   	nop
 31b:	90                   	nop
 31c:	90                   	nop
 31d:	90                   	nop
 31e:	90                   	nop
 31f:	90                   	nop

00000320 <stat>:
 320:	55                   	push   %ebp
 321:	89 e5                	mov    %esp,%ebp
 323:	56                   	push   %esi
 324:	53                   	push   %ebx
 325:	83 ec 08             	sub    $0x8,%esp
 328:	6a 00                	push   $0x0
 32a:	ff 75 08             	pushl  0x8(%ebp)
 32d:	e8 f0 00 00 00       	call   422 <open>
 332:	83 c4 10             	add    $0x10,%esp
 335:	85 c0                	test   %eax,%eax
 337:	78 27                	js     360 <stat+0x40>
 339:	83 ec 08             	sub    $0x8,%esp
 33c:	ff 75 0c             	pushl  0xc(%ebp)
 33f:	89 c3                	mov    %eax,%ebx
 341:	50                   	push   %eax
 342:	e8 f3 00 00 00       	call   43a <fstat>
 347:	89 c6                	mov    %eax,%esi
 349:	89 1c 24             	mov    %ebx,(%esp)
 34c:	e8 b9 00 00 00       	call   40a <close>
 351:	83 c4 10             	add    $0x10,%esp
 354:	89 f0                	mov    %esi,%eax
 356:	8d 65 f8             	lea    -0x8(%ebp),%esp
 359:	5b                   	pop    %ebx
 35a:	5e                   	pop    %esi
 35b:	5d                   	pop    %ebp
 35c:	c3                   	ret    
 35d:	8d 76 00             	lea    0x0(%esi),%esi
 360:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
 365:	eb ef                	jmp    356 <stat+0x36>
 367:	89 f6                	mov    %esi,%esi
 369:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000370 <atoi>:
 370:	55                   	push   %ebp
 371:	89 e5                	mov    %esp,%ebp
 373:	53                   	push   %ebx
 374:	8b 4d 08             	mov    0x8(%ebp),%ecx
 377:	0f be 11             	movsbl (%ecx),%edx
 37a:	8d 42 d0             	lea    -0x30(%edx),%eax
 37d:	3c 09                	cmp    $0x9,%al
 37f:	b8 00 00 00 00       	mov    $0x0,%eax
 384:	77 1f                	ja     3a5 <atoi+0x35>
 386:	8d 76 00             	lea    0x0(%esi),%esi
 389:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
 390:	8d 04 80             	lea    (%eax,%eax,4),%eax
 393:	83 c1 01             	add    $0x1,%ecx
 396:	8d 44 42 d0          	lea    -0x30(%edx,%eax,2),%eax
 39a:	0f be 11             	movsbl (%ecx),%edx
 39d:	8d 5a d0             	lea    -0x30(%edx),%ebx
 3a0:	80 fb 09             	cmp    $0x9,%bl
 3a3:	76 eb                	jbe    390 <atoi+0x20>
 3a5:	5b                   	pop    %ebx
 3a6:	5d                   	pop    %ebp
 3a7:	c3                   	ret    
 3a8:	90                   	nop
 3a9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

000003b0 <memmove>:
 3b0:	55                   	push   %ebp
 3b1:	89 e5                	mov    %esp,%ebp
 3b3:	56                   	push   %esi
 3b4:	53                   	push   %ebx
 3b5:	8b 5d 10             	mov    0x10(%ebp),%ebx
 3b8:	8b 45 08             	mov    0x8(%ebp),%eax
 3bb:	8b 75 0c             	mov    0xc(%ebp),%esi
 3be:	85 db                	test   %ebx,%ebx
 3c0:	7e 14                	jle    3d6 <memmove+0x26>
 3c2:	31 d2                	xor    %edx,%edx
 3c4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 3c8:	0f b6 0c 16          	movzbl (%esi,%edx,1),%ecx
 3cc:	88 0c 10             	mov    %cl,(%eax,%edx,1)
 3cf:	83 c2 01             	add    $0x1,%edx
 3d2:	39 da                	cmp    %ebx,%edx
 3d4:	75 f2                	jne    3c8 <memmove+0x18>
 3d6:	5b                   	pop    %ebx
 3d7:	5e                   	pop    %esi
 3d8:	5d                   	pop    %ebp
 3d9:	c3                   	ret    

000003da <fork>:
 3da:	b8 01 00 00 00       	mov    $0x1,%eax
 3df:	cd 40                	int    $0x40
 3e1:	c3                   	ret    

000003e2 <exit>:
 3e2:	b8 02 00 00 00       	mov    $0x2,%eax
 3e7:	cd 40                	int    $0x40
 3e9:	c3                   	ret    

000003ea <wait>:
 3ea:	b8 03 00 00 00       	mov    $0x3,%eax
 3ef:	cd 40                	int    $0x40
 3f1:	c3                   	ret    

000003f2 <pipe>:
 3f2:	b8 04 00 00 00       	mov    $0x4,%eax
 3f7:	cd 40                	int    $0x40
 3f9:	c3                   	ret    

000003fa <read>:
 3fa:	b8 05 00 00 00       	mov    $0x5,%eax
 3ff:	cd 40                	int    $0x40
 401:	c3                   	ret    

00000402 <write>:
 402:	b8 10 00 00 00       	mov    $0x10,%eax
 407:	cd 40                	int    $0x40
 409:	c3                   	ret    

0000040a <close>:
 40a:	b8 15 00 00 00       	mov    $0x15,%eax
 40f:	cd 40                	int    $0x40
 411:	c3                   	ret    

00000412 <kill>:
 412:	b8 06 00 00 00       	mov    $0x6,%eax
 417:	cd 40                	int    $0x40
 419:	c3                   	ret    

0000041a <exec>:
 41a:	b8 07 00 00 00       	mov    $0x7,%eax
 41f:	cd 40                	int    $0x40
 421:	c3                   	ret    

00000422 <open>:
 422:	b8 0f 00 00 00       	mov    $0xf,%eax
 427:	cd 40                	int    $0x40
 429:	c3                   	ret    

0000042a <mknod>:
 42a:	b8 11 00 00 00       	mov    $0x11,%eax
 42f:	cd 40                	int    $0x40
 431:	c3                   	ret    

00000432 <unlink>:
 432:	b8 12 00 00 00       	mov    $0x12,%eax
 437:	cd 40                	int    $0x40
 439:	c3                   	ret    

0000043a <fstat>:
 43a:	b8 08 00 00 00       	mov    $0x8,%eax
 43f:	cd 40                	int    $0x40
 441:	c3                   	ret    

00000442 <link>:
 442:	b8 13 00 00 00       	mov    $0x13,%eax
 447:	cd 40                	int    $0x40
 449:	c3                   	ret    

0000044a <mkdir>:
 44a:	b8 14 00 00 00       	mov    $0x14,%eax
 44f:	cd 40                	int    $0x40
 451:	c3                   	ret    

00000452 <chdir>:
 452:	b8 09 00 00 00       	mov    $0x9,%eax
 457:	cd 40                	int    $0x40
 459:	c3                   	ret    

0000045a <dup>:
 45a:	b8 0a 00 00 00       	mov    $0xa,%eax
 45f:	cd 40                	int    $0x40
 461:	c3                   	ret    

00000462 <getpid>:
 462:	b8 0b 00 00 00       	mov    $0xb,%eax
 467:	cd 40                	int    $0x40
 469:	c3                   	ret    

0000046a <sbrk>:
 46a:	b8 0c 00 00 00       	mov    $0xc,%eax
 46f:	cd 40                	int    $0x40
 471:	c3                   	ret    

00000472 <sleep>:
 472:	b8 0d 00 00 00       	mov    $0xd,%eax
 477:	cd 40                	int    $0x40
 479:	c3                   	ret    

0000047a <uptime>:
 47a:	b8 0e 00 00 00       	mov    $0xe,%eax
 47f:	cd 40                	int    $0x40
 481:	c3                   	ret    

00000482 <cps>:
 482:	b8 16 00 00 00       	mov    $0x16,%eax
 487:	cd 40                	int    $0x40
 489:	c3                   	ret    

0000048a <chpr>:
 48a:	b8 17 00 00 00       	mov    $0x17,%eax
 48f:	cd 40                	int    $0x40
 491:	c3                   	ret    

00000492 <chdl>:
 492:	b8 18 00 00 00       	mov    $0x18,%eax
 497:	cd 40                	int    $0x40
 499:	c3                   	ret    
 49a:	66 90                	xchg   %ax,%ax
 49c:	66 90                	xchg   %ax,%ax
 49e:	66 90                	xchg   %ax,%ax

000004a0 <printint>:
 4a0:	55                   	push   %ebp
 4a1:	89 e5                	mov    %esp,%ebp
 4a3:	57                   	push   %edi
 4a4:	56                   	push   %esi
 4a5:	53                   	push   %ebx
 4a6:	89 c6                	mov    %eax,%esi
 4a8:	83 ec 3c             	sub    $0x3c,%esp
 4ab:	8b 5d 08             	mov    0x8(%ebp),%ebx
 4ae:	85 db                	test   %ebx,%ebx
 4b0:	74 7e                	je     530 <printint+0x90>
 4b2:	89 d0                	mov    %edx,%eax
 4b4:	c1 e8 1f             	shr    $0x1f,%eax
 4b7:	84 c0                	test   %al,%al
 4b9:	74 75                	je     530 <printint+0x90>
 4bb:	89 d0                	mov    %edx,%eax
 4bd:	c7 45 c4 01 00 00 00 	movl   $0x1,-0x3c(%ebp)
 4c4:	f7 d8                	neg    %eax
 4c6:	89 75 c0             	mov    %esi,-0x40(%ebp)
 4c9:	31 ff                	xor    %edi,%edi
 4cb:	8d 5d d7             	lea    -0x29(%ebp),%ebx
 4ce:	89 ce                	mov    %ecx,%esi
 4d0:	eb 08                	jmp    4da <printint+0x3a>
 4d2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 4d8:	89 cf                	mov    %ecx,%edi
 4da:	31 d2                	xor    %edx,%edx
 4dc:	8d 4f 01             	lea    0x1(%edi),%ecx
 4df:	f7 f6                	div    %esi
 4e1:	0f b6 92 b0 08 00 00 	movzbl 0x8b0(%edx),%edx
 4e8:	85 c0                	test   %eax,%eax
 4ea:	88 14 0b             	mov    %dl,(%ebx,%ecx,1)
 4ed:	75 e9                	jne    4d8 <printint+0x38>
 4ef:	8b 45 c4             	mov    -0x3c(%ebp),%eax
 4f2:	8b 75 c0             	mov    -0x40(%ebp),%esi
 4f5:	85 c0                	test   %eax,%eax
 4f7:	74 08                	je     501 <printint+0x61>
 4f9:	c6 44 0d d8 2d       	movb   $0x2d,-0x28(%ebp,%ecx,1)
 4fe:	8d 4f 02             	lea    0x2(%edi),%ecx
 501:	8d 7c 0d d7          	lea    -0x29(%ebp,%ecx,1),%edi
 505:	8d 76 00             	lea    0x0(%esi),%esi
 508:	0f b6 07             	movzbl (%edi),%eax
 50b:	83 ec 04             	sub    $0x4,%esp
 50e:	83 ef 01             	sub    $0x1,%edi
 511:	6a 01                	push   $0x1
 513:	53                   	push   %ebx
 514:	56                   	push   %esi
 515:	88 45 d7             	mov    %al,-0x29(%ebp)
 518:	e8 e5 fe ff ff       	call   402 <write>
 51d:	83 c4 10             	add    $0x10,%esp
 520:	39 df                	cmp    %ebx,%edi
 522:	75 e4                	jne    508 <printint+0x68>
 524:	8d 65 f4             	lea    -0xc(%ebp),%esp
 527:	5b                   	pop    %ebx
 528:	5e                   	pop    %esi
 529:	5f                   	pop    %edi
 52a:	5d                   	pop    %ebp
 52b:	c3                   	ret    
 52c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 530:	89 d0                	mov    %edx,%eax
 532:	c7 45 c4 00 00 00 00 	movl   $0x0,-0x3c(%ebp)
 539:	eb 8b                	jmp    4c6 <printint+0x26>
 53b:	90                   	nop
 53c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00000540 <printf>:
 540:	55                   	push   %ebp
 541:	89 e5                	mov    %esp,%ebp
 543:	57                   	push   %edi
 544:	56                   	push   %esi
 545:	53                   	push   %ebx
 546:	8d 45 10             	lea    0x10(%ebp),%eax
 549:	83 ec 2c             	sub    $0x2c,%esp
 54c:	8b 75 0c             	mov    0xc(%ebp),%esi
 54f:	8b 7d 08             	mov    0x8(%ebp),%edi
 552:	89 45 d0             	mov    %eax,-0x30(%ebp)
 555:	0f b6 1e             	movzbl (%esi),%ebx
 558:	83 c6 01             	add    $0x1,%esi
 55b:	84 db                	test   %bl,%bl
 55d:	0f 84 b0 00 00 00    	je     613 <printf+0xd3>
 563:	31 d2                	xor    %edx,%edx
 565:	eb 39                	jmp    5a0 <printf+0x60>
 567:	89 f6                	mov    %esi,%esi
 569:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
 570:	83 f8 25             	cmp    $0x25,%eax
 573:	89 55 d4             	mov    %edx,-0x2c(%ebp)
 576:	ba 25 00 00 00       	mov    $0x25,%edx
 57b:	74 18                	je     595 <printf+0x55>
 57d:	8d 45 e2             	lea    -0x1e(%ebp),%eax
 580:	83 ec 04             	sub    $0x4,%esp
 583:	88 5d e2             	mov    %bl,-0x1e(%ebp)
 586:	6a 01                	push   $0x1
 588:	50                   	push   %eax
 589:	57                   	push   %edi
 58a:	e8 73 fe ff ff       	call   402 <write>
 58f:	8b 55 d4             	mov    -0x2c(%ebp),%edx
 592:	83 c4 10             	add    $0x10,%esp
 595:	83 c6 01             	add    $0x1,%esi
 598:	0f b6 5e ff          	movzbl -0x1(%esi),%ebx
 59c:	84 db                	test   %bl,%bl
 59e:	74 73                	je     613 <printf+0xd3>
 5a0:	85 d2                	test   %edx,%edx
 5a2:	0f be cb             	movsbl %bl,%ecx
 5a5:	0f b6 c3             	movzbl %bl,%eax
 5a8:	74 c6                	je     570 <printf+0x30>
 5aa:	83 fa 25             	cmp    $0x25,%edx
 5ad:	75 e6                	jne    595 <printf+0x55>
 5af:	83 f8 64             	cmp    $0x64,%eax
 5b2:	0f 84 f8 00 00 00    	je     6b0 <printf+0x170>
 5b8:	81 e1 f7 00 00 00    	and    $0xf7,%ecx
 5be:	83 f9 70             	cmp    $0x70,%ecx
 5c1:	74 5d                	je     620 <printf+0xe0>
 5c3:	83 f8 73             	cmp    $0x73,%eax
 5c6:	0f 84 84 00 00 00    	je     650 <printf+0x110>
 5cc:	83 f8 63             	cmp    $0x63,%eax
 5cf:	0f 84 ea 00 00 00    	je     6bf <printf+0x17f>
 5d5:	83 f8 25             	cmp    $0x25,%eax
 5d8:	0f 84 c2 00 00 00    	je     6a0 <printf+0x160>
 5de:	8d 45 e7             	lea    -0x19(%ebp),%eax
 5e1:	83 ec 04             	sub    $0x4,%esp
 5e4:	c6 45 e7 25          	movb   $0x25,-0x19(%ebp)
 5e8:	6a 01                	push   $0x1
 5ea:	50                   	push   %eax
 5eb:	57                   	push   %edi
 5ec:	e8 11 fe ff ff       	call   402 <write>
 5f1:	83 c4 0c             	add    $0xc,%esp
 5f4:	8d 45 e6             	lea    -0x1a(%ebp),%eax
 5f7:	88 5d e6             	mov    %bl,-0x1a(%ebp)
 5fa:	6a 01                	push   $0x1
 5fc:	50                   	push   %eax
 5fd:	57                   	push   %edi
 5fe:	83 c6 01             	add    $0x1,%esi
 601:	e8 fc fd ff ff       	call   402 <write>
 606:	0f b6 5e ff          	movzbl -0x1(%esi),%ebx
 60a:	83 c4 10             	add    $0x10,%esp
 60d:	31 d2                	xor    %edx,%edx
 60f:	84 db                	test   %bl,%bl
 611:	75 8d                	jne    5a0 <printf+0x60>
 613:	8d 65 f4             	lea    -0xc(%ebp),%esp
 616:	5b                   	pop    %ebx
 617:	5e                   	pop    %esi
 618:	5f                   	pop    %edi
 619:	5d                   	pop    %ebp
 61a:	c3                   	ret    
 61b:	90                   	nop
 61c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 620:	83 ec 0c             	sub    $0xc,%esp
 623:	b9 10 00 00 00       	mov    $0x10,%ecx
 628:	6a 00                	push   $0x0
 62a:	8b 5d d0             	mov    -0x30(%ebp),%ebx
 62d:	89 f8                	mov    %edi,%eax
 62f:	8b 13                	mov    (%ebx),%edx
 631:	e8 6a fe ff ff       	call   4a0 <printint>
 636:	89 d8                	mov    %ebx,%eax
 638:	83 c4 10             	add    $0x10,%esp
 63b:	31 d2                	xor    %edx,%edx
 63d:	83 c0 04             	add    $0x4,%eax
 640:	89 45 d0             	mov    %eax,-0x30(%ebp)
 643:	e9 4d ff ff ff       	jmp    595 <printf+0x55>
 648:	90                   	nop
 649:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 650:	8b 45 d0             	mov    -0x30(%ebp),%eax
 653:	8b 18                	mov    (%eax),%ebx
 655:	83 c0 04             	add    $0x4,%eax
 658:	89 45 d0             	mov    %eax,-0x30(%ebp)
 65b:	b8 a8 08 00 00       	mov    $0x8a8,%eax
 660:	85 db                	test   %ebx,%ebx
 662:	0f 44 d8             	cmove  %eax,%ebx
 665:	0f b6 03             	movzbl (%ebx),%eax
 668:	84 c0                	test   %al,%al
 66a:	74 23                	je     68f <printf+0x14f>
 66c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 670:	88 45 e3             	mov    %al,-0x1d(%ebp)
 673:	8d 45 e3             	lea    -0x1d(%ebp),%eax
 676:	83 ec 04             	sub    $0x4,%esp
 679:	6a 01                	push   $0x1
 67b:	83 c3 01             	add    $0x1,%ebx
 67e:	50                   	push   %eax
 67f:	57                   	push   %edi
 680:	e8 7d fd ff ff       	call   402 <write>
 685:	0f b6 03             	movzbl (%ebx),%eax
 688:	83 c4 10             	add    $0x10,%esp
 68b:	84 c0                	test   %al,%al
 68d:	75 e1                	jne    670 <printf+0x130>
 68f:	31 d2                	xor    %edx,%edx
 691:	e9 ff fe ff ff       	jmp    595 <printf+0x55>
 696:	8d 76 00             	lea    0x0(%esi),%esi
 699:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
 6a0:	83 ec 04             	sub    $0x4,%esp
 6a3:	88 5d e5             	mov    %bl,-0x1b(%ebp)
 6a6:	8d 45 e5             	lea    -0x1b(%ebp),%eax
 6a9:	6a 01                	push   $0x1
 6ab:	e9 4c ff ff ff       	jmp    5fc <printf+0xbc>
 6b0:	83 ec 0c             	sub    $0xc,%esp
 6b3:	b9 0a 00 00 00       	mov    $0xa,%ecx
 6b8:	6a 01                	push   $0x1
 6ba:	e9 6b ff ff ff       	jmp    62a <printf+0xea>
 6bf:	8b 5d d0             	mov    -0x30(%ebp),%ebx
 6c2:	83 ec 04             	sub    $0x4,%esp
 6c5:	8b 03                	mov    (%ebx),%eax
 6c7:	6a 01                	push   $0x1
 6c9:	88 45 e4             	mov    %al,-0x1c(%ebp)
 6cc:	8d 45 e4             	lea    -0x1c(%ebp),%eax
 6cf:	50                   	push   %eax
 6d0:	57                   	push   %edi
 6d1:	e8 2c fd ff ff       	call   402 <write>
 6d6:	e9 5b ff ff ff       	jmp    636 <printf+0xf6>
 6db:	66 90                	xchg   %ax,%ax
 6dd:	66 90                	xchg   %ax,%ax
 6df:	90                   	nop

000006e0 <free>:
 6e0:	55                   	push   %ebp
 6e1:	a1 54 0b 00 00       	mov    0xb54,%eax
 6e6:	89 e5                	mov    %esp,%ebp
 6e8:	57                   	push   %edi
 6e9:	56                   	push   %esi
 6ea:	53                   	push   %ebx
 6eb:	8b 5d 08             	mov    0x8(%ebp),%ebx
 6ee:	8b 10                	mov    (%eax),%edx
 6f0:	8d 4b f8             	lea    -0x8(%ebx),%ecx
 6f3:	39 c8                	cmp    %ecx,%eax
 6f5:	73 19                	jae    710 <free+0x30>
 6f7:	89 f6                	mov    %esi,%esi
 6f9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
 700:	39 d1                	cmp    %edx,%ecx
 702:	72 1c                	jb     720 <free+0x40>
 704:	39 d0                	cmp    %edx,%eax
 706:	73 18                	jae    720 <free+0x40>
 708:	89 d0                	mov    %edx,%eax
 70a:	39 c8                	cmp    %ecx,%eax
 70c:	8b 10                	mov    (%eax),%edx
 70e:	72 f0                	jb     700 <free+0x20>
 710:	39 d0                	cmp    %edx,%eax
 712:	72 f4                	jb     708 <free+0x28>
 714:	39 d1                	cmp    %edx,%ecx
 716:	73 f0                	jae    708 <free+0x28>
 718:	90                   	nop
 719:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 720:	8b 73 fc             	mov    -0x4(%ebx),%esi
 723:	8d 3c f1             	lea    (%ecx,%esi,8),%edi
 726:	39 d7                	cmp    %edx,%edi
 728:	74 19                	je     743 <free+0x63>
 72a:	89 53 f8             	mov    %edx,-0x8(%ebx)
 72d:	8b 50 04             	mov    0x4(%eax),%edx
 730:	8d 34 d0             	lea    (%eax,%edx,8),%esi
 733:	39 f1                	cmp    %esi,%ecx
 735:	74 23                	je     75a <free+0x7a>
 737:	89 08                	mov    %ecx,(%eax)
 739:	a3 54 0b 00 00       	mov    %eax,0xb54
 73e:	5b                   	pop    %ebx
 73f:	5e                   	pop    %esi
 740:	5f                   	pop    %edi
 741:	5d                   	pop    %ebp
 742:	c3                   	ret    
 743:	03 72 04             	add    0x4(%edx),%esi
 746:	89 73 fc             	mov    %esi,-0x4(%ebx)
 749:	8b 10                	mov    (%eax),%edx
 74b:	8b 12                	mov    (%edx),%edx
 74d:	89 53 f8             	mov    %edx,-0x8(%ebx)
 750:	8b 50 04             	mov    0x4(%eax),%edx
 753:	8d 34 d0             	lea    (%eax,%edx,8),%esi
 756:	39 f1                	cmp    %esi,%ecx
 758:	75 dd                	jne    737 <free+0x57>
 75a:	03 53 fc             	add    -0x4(%ebx),%edx
 75d:	a3 54 0b 00 00       	mov    %eax,0xb54
 762:	89 50 04             	mov    %edx,0x4(%eax)
 765:	8b 53 f8             	mov    -0x8(%ebx),%edx
 768:	89 10                	mov    %edx,(%eax)
 76a:	5b                   	pop    %ebx
 76b:	5e                   	pop    %esi
 76c:	5f                   	pop    %edi
 76d:	5d                   	pop    %ebp
 76e:	c3                   	ret    
 76f:	90                   	nop

00000770 <malloc>:
 770:	55                   	push   %ebp
 771:	89 e5                	mov    %esp,%ebp
 773:	57                   	push   %edi
 774:	56                   	push   %esi
 775:	53                   	push   %ebx
 776:	83 ec 0c             	sub    $0xc,%esp
 779:	8b 45 08             	mov    0x8(%ebp),%eax
 77c:	8b 15 54 0b 00 00    	mov    0xb54,%edx
 782:	8d 78 07             	lea    0x7(%eax),%edi
 785:	c1 ef 03             	shr    $0x3,%edi
 788:	83 c7 01             	add    $0x1,%edi
 78b:	85 d2                	test   %edx,%edx
 78d:	0f 84 a3 00 00 00    	je     836 <malloc+0xc6>
 793:	8b 02                	mov    (%edx),%eax
 795:	8b 48 04             	mov    0x4(%eax),%ecx
 798:	39 cf                	cmp    %ecx,%edi
 79a:	76 74                	jbe    810 <malloc+0xa0>
 79c:	81 ff 00 10 00 00    	cmp    $0x1000,%edi
 7a2:	be 00 10 00 00       	mov    $0x1000,%esi
 7a7:	8d 1c fd 00 00 00 00 	lea    0x0(,%edi,8),%ebx
 7ae:	0f 43 f7             	cmovae %edi,%esi
 7b1:	ba 00 80 00 00       	mov    $0x8000,%edx
 7b6:	81 ff ff 0f 00 00    	cmp    $0xfff,%edi
 7bc:	0f 46 da             	cmovbe %edx,%ebx
 7bf:	eb 10                	jmp    7d1 <malloc+0x61>
 7c1:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 7c8:	8b 02                	mov    (%edx),%eax
 7ca:	8b 48 04             	mov    0x4(%eax),%ecx
 7cd:	39 cf                	cmp    %ecx,%edi
 7cf:	76 3f                	jbe    810 <malloc+0xa0>
 7d1:	39 05 54 0b 00 00    	cmp    %eax,0xb54
 7d7:	89 c2                	mov    %eax,%edx
 7d9:	75 ed                	jne    7c8 <malloc+0x58>
 7db:	83 ec 0c             	sub    $0xc,%esp
 7de:	53                   	push   %ebx
 7df:	e8 86 fc ff ff       	call   46a <sbrk>
 7e4:	83 c4 10             	add    $0x10,%esp
 7e7:	83 f8 ff             	cmp    $0xffffffff,%eax
 7ea:	74 1c                	je     808 <malloc+0x98>
 7ec:	89 70 04             	mov    %esi,0x4(%eax)
 7ef:	83 ec 0c             	sub    $0xc,%esp
 7f2:	83 c0 08             	add    $0x8,%eax
 7f5:	50                   	push   %eax
 7f6:	e8 e5 fe ff ff       	call   6e0 <free>
 7fb:	8b 15 54 0b 00 00    	mov    0xb54,%edx
 801:	83 c4 10             	add    $0x10,%esp
 804:	85 d2                	test   %edx,%edx
 806:	75 c0                	jne    7c8 <malloc+0x58>
 808:	31 c0                	xor    %eax,%eax
 80a:	eb 1c                	jmp    828 <malloc+0xb8>
 80c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 810:	39 cf                	cmp    %ecx,%edi
 812:	74 1c                	je     830 <malloc+0xc0>
 814:	29 f9                	sub    %edi,%ecx
 816:	89 48 04             	mov    %ecx,0x4(%eax)
 819:	8d 04 c8             	lea    (%eax,%ecx,8),%eax
 81c:	89 78 04             	mov    %edi,0x4(%eax)
 81f:	89 15 54 0b 00 00    	mov    %edx,0xb54
 825:	83 c0 08             	add    $0x8,%eax
 828:	8d 65 f4             	lea    -0xc(%ebp),%esp
 82b:	5b                   	pop    %ebx
 82c:	5e                   	pop    %esi
 82d:	5f                   	pop    %edi
 82e:	5d                   	pop    %ebp
 82f:	c3                   	ret    
 830:	8b 08                	mov    (%eax),%ecx
 832:	89 0a                	mov    %ecx,(%edx)
 834:	eb e9                	jmp    81f <malloc+0xaf>
 836:	c7 05 54 0b 00 00 58 	movl   $0xb58,0xb54
 83d:	0b 00 00 
 840:	c7 05 58 0b 00 00 58 	movl   $0xb58,0xb58
 847:	0b 00 00 
 84a:	b8 58 0b 00 00       	mov    $0xb58,%eax
 84f:	c7 05 5c 0b 00 00 00 	movl   $0x0,0xb5c
 856:	00 00 00 
 859:	e9 3e ff ff ff       	jmp    79c <malloc+0x2c>
