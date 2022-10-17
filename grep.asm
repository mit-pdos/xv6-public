
_grep:     file format elf32-i386


Disassembly of section .text:

00000000 <main>:
  }
}

int
main(int argc, char *argv[])
{
   0:	55                   	push   %ebp
   1:	89 e5                	mov    %esp,%ebp
   3:	57                   	push   %edi
   4:	56                   	push   %esi
   5:	53                   	push   %ebx
   6:	83 e4 f0             	and    $0xfffffff0,%esp
   9:	83 ec 10             	sub    $0x10,%esp
   c:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  int fd, i;
  char *pattern;

  if(argc <= 1){
   f:	83 7d 08 01          	cmpl   $0x1,0x8(%ebp)
  13:	0f 8e a0 00 00 00    	jle    b9 <main+0xb9>
    printf(2, "usage: grep pattern [file ...]\n");
    exit(0);
  }
  pattern = argv[1];
  19:	8b 43 04             	mov    0x4(%ebx),%eax
  1c:	83 c3 08             	add    $0x8,%ebx

  if(argc <= 2){
  1f:	be 02 00 00 00       	mov    $0x2,%esi
  24:	83 7d 08 02          	cmpl   $0x2,0x8(%ebp)
  pattern = argv[1];
  28:	89 44 24 0c          	mov    %eax,0xc(%esp)
  if(argc <= 2){
  2c:	74 6f                	je     9d <main+0x9d>
  2e:	66 90                	xchg   %ax,%ax
    grep(pattern, 0);
    exit(0);
  }

  for(i = 2; i < argc; i++){
    if((fd = open(argv[i], 0)) < 0){
  30:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
  37:	00 
  38:	8b 03                	mov    (%ebx),%eax
  3a:	89 04 24             	mov    %eax,(%esp)
  3d:	e8 50 05 00 00       	call   592 <open>
  42:	85 c0                	test   %eax,%eax
  44:	89 c7                	mov    %eax,%edi
  46:	78 2f                	js     77 <main+0x77>
      printf(1, "grep: cannot open %s\n", argv[i]);
      exit(0);
    }
    grep(pattern, fd);
  48:	89 44 24 04          	mov    %eax,0x4(%esp)
  4c:	8b 44 24 0c          	mov    0xc(%esp),%eax
  for(i = 2; i < argc; i++){
  50:	83 c6 01             	add    $0x1,%esi
  53:	83 c3 04             	add    $0x4,%ebx
    grep(pattern, fd);
  56:	89 04 24             	mov    %eax,(%esp)
  59:	e8 c2 01 00 00       	call   220 <grep>
    close(fd);
  5e:	89 3c 24             	mov    %edi,(%esp)
  61:	e8 14 05 00 00       	call   57a <close>
  for(i = 2; i < argc; i++){
  66:	39 75 08             	cmp    %esi,0x8(%ebp)
  69:	7f c5                	jg     30 <main+0x30>
  }
  exit(0);
  6b:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  72:	e8 db 04 00 00       	call   552 <exit>
      printf(1, "grep: cannot open %s\n", argv[i]);
  77:	8b 03                	mov    (%ebx),%eax
  79:	c7 44 24 04 28 0a 00 	movl   $0xa28,0x4(%esp)
  80:	00 
  81:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
  88:	89 44 24 08          	mov    %eax,0x8(%esp)
  8c:	e8 0f 06 00 00       	call   6a0 <printf>
      exit(0);
  91:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  98:	e8 b5 04 00 00       	call   552 <exit>
    grep(pattern, 0);
  9d:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
  a4:	00 
  a5:	89 04 24             	mov    %eax,(%esp)
  a8:	e8 73 01 00 00       	call   220 <grep>
    exit(0);
  ad:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  b4:	e8 99 04 00 00       	call   552 <exit>
    printf(2, "usage: grep pattern [file ...]\n");
  b9:	c7 44 24 04 08 0a 00 	movl   $0xa08,0x4(%esp)
  c0:	00 
  c1:	c7 04 24 02 00 00 00 	movl   $0x2,(%esp)
  c8:	e8 d3 05 00 00       	call   6a0 <printf>
    exit(0);
  cd:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  d4:	e8 79 04 00 00       	call   552 <exit>
  d9:	66 90                	xchg   %ax,%ax
  db:	66 90                	xchg   %ax,%ax
  dd:	66 90                	xchg   %ax,%ax
  df:	90                   	nop

000000e0 <matchstar>:
  return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
  e0:	55                   	push   %ebp
  e1:	89 e5                	mov    %esp,%ebp
  e3:	57                   	push   %edi
  e4:	56                   	push   %esi
  e5:	53                   	push   %ebx
  e6:	83 ec 1c             	sub    $0x1c,%esp
  e9:	8b 75 08             	mov    0x8(%ebp),%esi
  ec:	8b 7d 0c             	mov    0xc(%ebp),%edi
  ef:	8b 5d 10             	mov    0x10(%ebp),%ebx
  f2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  do{  // a * matches zero or more instances
    if(matchhere(re, text))
  f8:	89 5c 24 04          	mov    %ebx,0x4(%esp)
  fc:	89 3c 24             	mov    %edi,(%esp)
  ff:	e8 3c 00 00 00       	call   140 <matchhere>
 104:	85 c0                	test   %eax,%eax
 106:	75 20                	jne    128 <matchstar+0x48>
      return 1;
  }while(*text!='\0' && (*text++==c || c=='.'));
 108:	0f be 13             	movsbl (%ebx),%edx
 10b:	84 d2                	test   %dl,%dl
 10d:	74 0c                	je     11b <matchstar+0x3b>
 10f:	83 c3 01             	add    $0x1,%ebx
 112:	39 f2                	cmp    %esi,%edx
 114:	74 e2                	je     f8 <matchstar+0x18>
 116:	83 fe 2e             	cmp    $0x2e,%esi
 119:	74 dd                	je     f8 <matchstar+0x18>
  return 0;
}
 11b:	83 c4 1c             	add    $0x1c,%esp
 11e:	5b                   	pop    %ebx
 11f:	5e                   	pop    %esi
 120:	5f                   	pop    %edi
 121:	5d                   	pop    %ebp
 122:	c3                   	ret    
 123:	90                   	nop
 124:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 128:	83 c4 1c             	add    $0x1c,%esp
      return 1;
 12b:	b8 01 00 00 00       	mov    $0x1,%eax
}
 130:	5b                   	pop    %ebx
 131:	5e                   	pop    %esi
 132:	5f                   	pop    %edi
 133:	5d                   	pop    %ebp
 134:	c3                   	ret    
 135:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 139:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000140 <matchhere>:
{
 140:	55                   	push   %ebp
 141:	89 e5                	mov    %esp,%ebp
 143:	53                   	push   %ebx
 144:	83 ec 14             	sub    $0x14,%esp
 147:	8b 55 08             	mov    0x8(%ebp),%edx
 14a:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  if(re[0] == '\0')
 14d:	0f be 02             	movsbl (%edx),%eax
 150:	84 c0                	test   %al,%al
 152:	75 20                	jne    174 <matchhere+0x34>
 154:	eb 42                	jmp    198 <matchhere+0x58>
 156:	66 90                	xchg   %ax,%ax
  if(*text!='\0' && (re[0]=='.' || re[0]==*text))
 158:	0f b6 19             	movzbl (%ecx),%ebx
 15b:	84 db                	test   %bl,%bl
 15d:	74 31                	je     190 <matchhere+0x50>
 15f:	3c 2e                	cmp    $0x2e,%al
 161:	74 04                	je     167 <matchhere+0x27>
 163:	38 d8                	cmp    %bl,%al
 165:	75 29                	jne    190 <matchhere+0x50>
    return matchhere(re+1, text+1);
 167:	83 c2 01             	add    $0x1,%edx
  if(re[0] == '\0')
 16a:	0f be 02             	movsbl (%edx),%eax
    return matchhere(re+1, text+1);
 16d:	83 c1 01             	add    $0x1,%ecx
  if(re[0] == '\0')
 170:	84 c0                	test   %al,%al
 172:	74 24                	je     198 <matchhere+0x58>
  if(re[1] == '*')
 174:	0f b6 5a 01          	movzbl 0x1(%edx),%ebx
 178:	80 fb 2a             	cmp    $0x2a,%bl
 17b:	74 2b                	je     1a8 <matchhere+0x68>
  if(re[0] == '$' && re[1] == '\0')
 17d:	3c 24                	cmp    $0x24,%al
 17f:	75 d7                	jne    158 <matchhere+0x18>
 181:	84 db                	test   %bl,%bl
 183:	74 3c                	je     1c1 <matchhere+0x81>
  if(*text!='\0' && (re[0]=='.' || re[0]==*text))
 185:	0f b6 19             	movzbl (%ecx),%ebx
 188:	84 db                	test   %bl,%bl
 18a:	75 d7                	jne    163 <matchhere+0x23>
 18c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  return 0;
 190:	31 c0                	xor    %eax,%eax
}
 192:	83 c4 14             	add    $0x14,%esp
 195:	5b                   	pop    %ebx
 196:	5d                   	pop    %ebp
 197:	c3                   	ret    
 198:	83 c4 14             	add    $0x14,%esp
    return 1;
 19b:	b8 01 00 00 00       	mov    $0x1,%eax
}
 1a0:	5b                   	pop    %ebx
 1a1:	5d                   	pop    %ebp
 1a2:	c3                   	ret    
 1a3:	90                   	nop
 1a4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    return matchstar(re[0], re+2, text);
 1a8:	83 c2 02             	add    $0x2,%edx
 1ab:	89 4c 24 08          	mov    %ecx,0x8(%esp)
 1af:	89 54 24 04          	mov    %edx,0x4(%esp)
 1b3:	89 04 24             	mov    %eax,(%esp)
 1b6:	e8 25 ff ff ff       	call   e0 <matchstar>
}
 1bb:	83 c4 14             	add    $0x14,%esp
 1be:	5b                   	pop    %ebx
 1bf:	5d                   	pop    %ebp
 1c0:	c3                   	ret    
    return *text == '\0';
 1c1:	31 c0                	xor    %eax,%eax
 1c3:	80 39 00             	cmpb   $0x0,(%ecx)
 1c6:	0f 94 c0             	sete   %al
 1c9:	eb c7                	jmp    192 <matchhere+0x52>
 1cb:	90                   	nop
 1cc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

000001d0 <match>:
{
 1d0:	55                   	push   %ebp
 1d1:	89 e5                	mov    %esp,%ebp
 1d3:	56                   	push   %esi
 1d4:	53                   	push   %ebx
 1d5:	83 ec 10             	sub    $0x10,%esp
 1d8:	8b 75 08             	mov    0x8(%ebp),%esi
 1db:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  if(re[0] == '^')
 1de:	80 3e 5e             	cmpb   $0x5e,(%esi)
 1e1:	75 0e                	jne    1f1 <match+0x21>
 1e3:	eb 28                	jmp    20d <match+0x3d>
 1e5:	8d 76 00             	lea    0x0(%esi),%esi
  }while(*text++ != '\0');
 1e8:	83 c3 01             	add    $0x1,%ebx
 1eb:	80 7b ff 00          	cmpb   $0x0,-0x1(%ebx)
 1ef:	74 15                	je     206 <match+0x36>
    if(matchhere(re, text))
 1f1:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 1f5:	89 34 24             	mov    %esi,(%esp)
 1f8:	e8 43 ff ff ff       	call   140 <matchhere>
 1fd:	85 c0                	test   %eax,%eax
 1ff:	74 e7                	je     1e8 <match+0x18>
      return 1;
 201:	b8 01 00 00 00       	mov    $0x1,%eax
}
 206:	83 c4 10             	add    $0x10,%esp
 209:	5b                   	pop    %ebx
 20a:	5e                   	pop    %esi
 20b:	5d                   	pop    %ebp
 20c:	c3                   	ret    
    return matchhere(re+1, text);
 20d:	83 c6 01             	add    $0x1,%esi
 210:	89 75 08             	mov    %esi,0x8(%ebp)
}
 213:	83 c4 10             	add    $0x10,%esp
 216:	5b                   	pop    %ebx
 217:	5e                   	pop    %esi
 218:	5d                   	pop    %ebp
    return matchhere(re+1, text);
 219:	e9 22 ff ff ff       	jmp    140 <matchhere>
 21e:	66 90                	xchg   %ax,%ax

00000220 <grep>:
{
 220:	55                   	push   %ebp
 221:	89 e5                	mov    %esp,%ebp
 223:	57                   	push   %edi
 224:	56                   	push   %esi
 225:	53                   	push   %ebx
 226:	83 ec 1c             	sub    $0x1c,%esp
 229:	8b 75 08             	mov    0x8(%ebp),%esi
  m = 0;
 22c:	c7 45 e4 00 00 00 00 	movl   $0x0,-0x1c(%ebp)
 233:	90                   	nop
 234:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  while((n = read(fd, buf+m, sizeof(buf)-m-1)) > 0){
 238:	8b 55 e4             	mov    -0x1c(%ebp),%edx
 23b:	b8 ff 03 00 00       	mov    $0x3ff,%eax
 240:	29 d0                	sub    %edx,%eax
 242:	89 44 24 08          	mov    %eax,0x8(%esp)
 246:	89 d0                	mov    %edx,%eax
 248:	05 c0 0d 00 00       	add    $0xdc0,%eax
 24d:	89 44 24 04          	mov    %eax,0x4(%esp)
 251:	8b 45 0c             	mov    0xc(%ebp),%eax
 254:	89 04 24             	mov    %eax,(%esp)
 257:	e8 0e 03 00 00       	call   56a <read>
 25c:	85 c0                	test   %eax,%eax
 25e:	0f 8e b8 00 00 00    	jle    31c <grep+0xfc>
    m += n;
 264:	01 45 e4             	add    %eax,-0x1c(%ebp)
    p = buf;
 267:	bb c0 0d 00 00       	mov    $0xdc0,%ebx
    buf[m] = '\0';
 26c:	8b 45 e4             	mov    -0x1c(%ebp),%eax
 26f:	c6 80 c0 0d 00 00 00 	movb   $0x0,0xdc0(%eax)
 276:	66 90                	xchg   %ax,%ax
    while((q = strchr(p, '\n')) != 0){
 278:	c7 44 24 04 0a 00 00 	movl   $0xa,0x4(%esp)
 27f:	00 
 280:	89 1c 24             	mov    %ebx,(%esp)
 283:	e8 78 01 00 00       	call   400 <strchr>
 288:	85 c0                	test   %eax,%eax
 28a:	89 c7                	mov    %eax,%edi
 28c:	74 42                	je     2d0 <grep+0xb0>
      *q = 0;
 28e:	c6 07 00             	movb   $0x0,(%edi)
      if(match(pattern, p)){
 291:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 295:	89 34 24             	mov    %esi,(%esp)
 298:	e8 33 ff ff ff       	call   1d0 <match>
 29d:	85 c0                	test   %eax,%eax
 29f:	75 07                	jne    2a8 <grep+0x88>
 2a1:	8d 5f 01             	lea    0x1(%edi),%ebx
 2a4:	eb d2                	jmp    278 <grep+0x58>
 2a6:	66 90                	xchg   %ax,%ax
        *q = '\n';
 2a8:	c6 07 0a             	movb   $0xa,(%edi)
        write(1, p, q+1 - p);
 2ab:	83 c7 01             	add    $0x1,%edi
 2ae:	89 f8                	mov    %edi,%eax
 2b0:	29 d8                	sub    %ebx,%eax
 2b2:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 2b6:	89 fb                	mov    %edi,%ebx
 2b8:	89 44 24 08          	mov    %eax,0x8(%esp)
 2bc:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 2c3:	e8 aa 02 00 00       	call   572 <write>
 2c8:	eb ae                	jmp    278 <grep+0x58>
 2ca:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    if(p == buf)
 2d0:	81 fb c0 0d 00 00    	cmp    $0xdc0,%ebx
 2d6:	74 38                	je     310 <grep+0xf0>
    if(m > 0){
 2d8:	8b 45 e4             	mov    -0x1c(%ebp),%eax
 2db:	85 c0                	test   %eax,%eax
 2dd:	0f 8e 55 ff ff ff    	jle    238 <grep+0x18>
      m -= p - buf;
 2e3:	b8 c0 0d 00 00       	mov    $0xdc0,%eax
 2e8:	29 d8                	sub    %ebx,%eax
 2ea:	01 45 e4             	add    %eax,-0x1c(%ebp)
      memmove(buf, p, m);
 2ed:	8b 45 e4             	mov    -0x1c(%ebp),%eax
 2f0:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 2f4:	c7 04 24 c0 0d 00 00 	movl   $0xdc0,(%esp)
 2fb:	89 44 24 08          	mov    %eax,0x8(%esp)
 2ff:	e8 1c 02 00 00       	call   520 <memmove>
 304:	e9 2f ff ff ff       	jmp    238 <grep+0x18>
 309:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
      m = 0;
 310:	c7 45 e4 00 00 00 00 	movl   $0x0,-0x1c(%ebp)
 317:	e9 1c ff ff ff       	jmp    238 <grep+0x18>
}
 31c:	83 c4 1c             	add    $0x1c,%esp
 31f:	5b                   	pop    %ebx
 320:	5e                   	pop    %esi
 321:	5f                   	pop    %edi
 322:	5d                   	pop    %ebp
 323:	c3                   	ret    
 324:	66 90                	xchg   %ax,%ax
 326:	66 90                	xchg   %ax,%ax
 328:	66 90                	xchg   %ax,%ax
 32a:	66 90                	xchg   %ax,%ax
 32c:	66 90                	xchg   %ax,%ax
 32e:	66 90                	xchg   %ax,%ax

00000330 <strcpy>:
#include "user.h"
#include "x86.h"

char*
strcpy(char *s, const char *t)
{
 330:	55                   	push   %ebp
 331:	89 e5                	mov    %esp,%ebp
 333:	8b 45 08             	mov    0x8(%ebp),%eax
 336:	8b 4d 0c             	mov    0xc(%ebp),%ecx
 339:	53                   	push   %ebx
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
 33a:	89 c2                	mov    %eax,%edx
 33c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 340:	83 c1 01             	add    $0x1,%ecx
 343:	0f b6 59 ff          	movzbl -0x1(%ecx),%ebx
 347:	83 c2 01             	add    $0x1,%edx
 34a:	84 db                	test   %bl,%bl
 34c:	88 5a ff             	mov    %bl,-0x1(%edx)
 34f:	75 ef                	jne    340 <strcpy+0x10>
    ;
  return os;
}
 351:	5b                   	pop    %ebx
 352:	5d                   	pop    %ebp
 353:	c3                   	ret    
 354:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 35a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

00000360 <strcmp>:

int
strcmp(const char *p, const char *q)
{
 360:	55                   	push   %ebp
 361:	89 e5                	mov    %esp,%ebp
 363:	8b 55 08             	mov    0x8(%ebp),%edx
 366:	53                   	push   %ebx
 367:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  while(*p && *p == *q)
 36a:	0f b6 02             	movzbl (%edx),%eax
 36d:	84 c0                	test   %al,%al
 36f:	74 2d                	je     39e <strcmp+0x3e>
 371:	0f b6 19             	movzbl (%ecx),%ebx
 374:	38 d8                	cmp    %bl,%al
 376:	74 0e                	je     386 <strcmp+0x26>
 378:	eb 2b                	jmp    3a5 <strcmp+0x45>
 37a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 380:	38 c8                	cmp    %cl,%al
 382:	75 15                	jne    399 <strcmp+0x39>
    p++, q++;
 384:	89 d9                	mov    %ebx,%ecx
 386:	83 c2 01             	add    $0x1,%edx
  while(*p && *p == *q)
 389:	0f b6 02             	movzbl (%edx),%eax
    p++, q++;
 38c:	8d 59 01             	lea    0x1(%ecx),%ebx
  while(*p && *p == *q)
 38f:	0f b6 49 01          	movzbl 0x1(%ecx),%ecx
 393:	84 c0                	test   %al,%al
 395:	75 e9                	jne    380 <strcmp+0x20>
 397:	31 c0                	xor    %eax,%eax
  return (uchar)*p - (uchar)*q;
 399:	29 c8                	sub    %ecx,%eax
}
 39b:	5b                   	pop    %ebx
 39c:	5d                   	pop    %ebp
 39d:	c3                   	ret    
 39e:	0f b6 09             	movzbl (%ecx),%ecx
  while(*p && *p == *q)
 3a1:	31 c0                	xor    %eax,%eax
 3a3:	eb f4                	jmp    399 <strcmp+0x39>
 3a5:	0f b6 cb             	movzbl %bl,%ecx
 3a8:	eb ef                	jmp    399 <strcmp+0x39>
 3aa:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

000003b0 <strlen>:

uint
strlen(const char *s)
{
 3b0:	55                   	push   %ebp
 3b1:	89 e5                	mov    %esp,%ebp
 3b3:	8b 4d 08             	mov    0x8(%ebp),%ecx
  int n;

  for(n = 0; s[n]; n++)
 3b6:	80 39 00             	cmpb   $0x0,(%ecx)
 3b9:	74 12                	je     3cd <strlen+0x1d>
 3bb:	31 d2                	xor    %edx,%edx
 3bd:	8d 76 00             	lea    0x0(%esi),%esi
 3c0:	83 c2 01             	add    $0x1,%edx
 3c3:	80 3c 11 00          	cmpb   $0x0,(%ecx,%edx,1)
 3c7:	89 d0                	mov    %edx,%eax
 3c9:	75 f5                	jne    3c0 <strlen+0x10>
    ;
  return n;
}
 3cb:	5d                   	pop    %ebp
 3cc:	c3                   	ret    
  for(n = 0; s[n]; n++)
 3cd:	31 c0                	xor    %eax,%eax
}
 3cf:	5d                   	pop    %ebp
 3d0:	c3                   	ret    
 3d1:	eb 0d                	jmp    3e0 <memset>
 3d3:	90                   	nop
 3d4:	90                   	nop
 3d5:	90                   	nop
 3d6:	90                   	nop
 3d7:	90                   	nop
 3d8:	90                   	nop
 3d9:	90                   	nop
 3da:	90                   	nop
 3db:	90                   	nop
 3dc:	90                   	nop
 3dd:	90                   	nop
 3de:	90                   	nop
 3df:	90                   	nop

000003e0 <memset>:

void*
memset(void *dst, int c, uint n)
{
 3e0:	55                   	push   %ebp
 3e1:	89 e5                	mov    %esp,%ebp
 3e3:	8b 55 08             	mov    0x8(%ebp),%edx
 3e6:	57                   	push   %edi
}

static inline void
stosb(void *addr, int data, int cnt)
{
  asm volatile("cld; rep stosb" :
 3e7:	8b 4d 10             	mov    0x10(%ebp),%ecx
 3ea:	8b 45 0c             	mov    0xc(%ebp),%eax
 3ed:	89 d7                	mov    %edx,%edi
 3ef:	fc                   	cld    
 3f0:	f3 aa                	rep stos %al,%es:(%edi)
  stosb(dst, c, n);
  return dst;
}
 3f2:	89 d0                	mov    %edx,%eax
 3f4:	5f                   	pop    %edi
 3f5:	5d                   	pop    %ebp
 3f6:	c3                   	ret    
 3f7:	89 f6                	mov    %esi,%esi
 3f9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000400 <strchr>:

char*
strchr(const char *s, char c)
{
 400:	55                   	push   %ebp
 401:	89 e5                	mov    %esp,%ebp
 403:	8b 45 08             	mov    0x8(%ebp),%eax
 406:	53                   	push   %ebx
 407:	8b 55 0c             	mov    0xc(%ebp),%edx
  for(; *s; s++)
 40a:	0f b6 18             	movzbl (%eax),%ebx
 40d:	84 db                	test   %bl,%bl
 40f:	74 1d                	je     42e <strchr+0x2e>
    if(*s == c)
 411:	38 d3                	cmp    %dl,%bl
 413:	89 d1                	mov    %edx,%ecx
 415:	75 0d                	jne    424 <strchr+0x24>
 417:	eb 17                	jmp    430 <strchr+0x30>
 419:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 420:	38 ca                	cmp    %cl,%dl
 422:	74 0c                	je     430 <strchr+0x30>
  for(; *s; s++)
 424:	83 c0 01             	add    $0x1,%eax
 427:	0f b6 10             	movzbl (%eax),%edx
 42a:	84 d2                	test   %dl,%dl
 42c:	75 f2                	jne    420 <strchr+0x20>
      return (char*)s;
  return 0;
 42e:	31 c0                	xor    %eax,%eax
}
 430:	5b                   	pop    %ebx
 431:	5d                   	pop    %ebp
 432:	c3                   	ret    
 433:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 439:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000440 <gets>:

char*
gets(char *buf, int max)
{
 440:	55                   	push   %ebp
 441:	89 e5                	mov    %esp,%ebp
 443:	57                   	push   %edi
 444:	56                   	push   %esi
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
 445:	31 f6                	xor    %esi,%esi
{
 447:	53                   	push   %ebx
 448:	83 ec 2c             	sub    $0x2c,%esp
    cc = read(0, &c, 1);
 44b:	8d 7d e7             	lea    -0x19(%ebp),%edi
  for(i=0; i+1 < max; ){
 44e:	eb 31                	jmp    481 <gets+0x41>
    cc = read(0, &c, 1);
 450:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 457:	00 
 458:	89 7c 24 04          	mov    %edi,0x4(%esp)
 45c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
 463:	e8 02 01 00 00       	call   56a <read>
    if(cc < 1)
 468:	85 c0                	test   %eax,%eax
 46a:	7e 1d                	jle    489 <gets+0x49>
      break;
    buf[i++] = c;
 46c:	0f b6 45 e7          	movzbl -0x19(%ebp),%eax
  for(i=0; i+1 < max; ){
 470:	89 de                	mov    %ebx,%esi
    buf[i++] = c;
 472:	8b 55 08             	mov    0x8(%ebp),%edx
    if(c == '\n' || c == '\r')
 475:	3c 0d                	cmp    $0xd,%al
    buf[i++] = c;
 477:	88 44 1a ff          	mov    %al,-0x1(%edx,%ebx,1)
    if(c == '\n' || c == '\r')
 47b:	74 0c                	je     489 <gets+0x49>
 47d:	3c 0a                	cmp    $0xa,%al
 47f:	74 08                	je     489 <gets+0x49>
  for(i=0; i+1 < max; ){
 481:	8d 5e 01             	lea    0x1(%esi),%ebx
 484:	3b 5d 0c             	cmp    0xc(%ebp),%ebx
 487:	7c c7                	jl     450 <gets+0x10>
      break;
  }
  buf[i] = '\0';
 489:	8b 45 08             	mov    0x8(%ebp),%eax
 48c:	c6 04 30 00          	movb   $0x0,(%eax,%esi,1)
  return buf;
}
 490:	83 c4 2c             	add    $0x2c,%esp
 493:	5b                   	pop    %ebx
 494:	5e                   	pop    %esi
 495:	5f                   	pop    %edi
 496:	5d                   	pop    %ebp
 497:	c3                   	ret    
 498:	90                   	nop
 499:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

000004a0 <stat>:

int
stat(const char *n, struct stat *st)
{
 4a0:	55                   	push   %ebp
 4a1:	89 e5                	mov    %esp,%ebp
 4a3:	56                   	push   %esi
 4a4:	53                   	push   %ebx
 4a5:	83 ec 10             	sub    $0x10,%esp
  int fd;
  int r;

  fd = open(n, O_RDONLY);
 4a8:	8b 45 08             	mov    0x8(%ebp),%eax
 4ab:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
 4b2:	00 
 4b3:	89 04 24             	mov    %eax,(%esp)
 4b6:	e8 d7 00 00 00       	call   592 <open>
  if(fd < 0)
 4bb:	85 c0                	test   %eax,%eax
  fd = open(n, O_RDONLY);
 4bd:	89 c3                	mov    %eax,%ebx
  if(fd < 0)
 4bf:	78 27                	js     4e8 <stat+0x48>
    return -1;
  r = fstat(fd, st);
 4c1:	8b 45 0c             	mov    0xc(%ebp),%eax
 4c4:	89 1c 24             	mov    %ebx,(%esp)
 4c7:	89 44 24 04          	mov    %eax,0x4(%esp)
 4cb:	e8 da 00 00 00       	call   5aa <fstat>
  close(fd);
 4d0:	89 1c 24             	mov    %ebx,(%esp)
  r = fstat(fd, st);
 4d3:	89 c6                	mov    %eax,%esi
  close(fd);
 4d5:	e8 a0 00 00 00       	call   57a <close>
  return r;
 4da:	89 f0                	mov    %esi,%eax
}
 4dc:	83 c4 10             	add    $0x10,%esp
 4df:	5b                   	pop    %ebx
 4e0:	5e                   	pop    %esi
 4e1:	5d                   	pop    %ebp
 4e2:	c3                   	ret    
 4e3:	90                   	nop
 4e4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    return -1;
 4e8:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
 4ed:	eb ed                	jmp    4dc <stat+0x3c>
 4ef:	90                   	nop

000004f0 <atoi>:

int
atoi(const char *s)
{
 4f0:	55                   	push   %ebp
 4f1:	89 e5                	mov    %esp,%ebp
 4f3:	8b 4d 08             	mov    0x8(%ebp),%ecx
 4f6:	53                   	push   %ebx
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
 4f7:	0f be 11             	movsbl (%ecx),%edx
 4fa:	8d 42 d0             	lea    -0x30(%edx),%eax
 4fd:	3c 09                	cmp    $0x9,%al
  n = 0;
 4ff:	b8 00 00 00 00       	mov    $0x0,%eax
  while('0' <= *s && *s <= '9')
 504:	77 17                	ja     51d <atoi+0x2d>
 506:	66 90                	xchg   %ax,%ax
    n = n*10 + *s++ - '0';
 508:	83 c1 01             	add    $0x1,%ecx
 50b:	8d 04 80             	lea    (%eax,%eax,4),%eax
 50e:	8d 44 42 d0          	lea    -0x30(%edx,%eax,2),%eax
  while('0' <= *s && *s <= '9')
 512:	0f be 11             	movsbl (%ecx),%edx
 515:	8d 5a d0             	lea    -0x30(%edx),%ebx
 518:	80 fb 09             	cmp    $0x9,%bl
 51b:	76 eb                	jbe    508 <atoi+0x18>
  return n;
}
 51d:	5b                   	pop    %ebx
 51e:	5d                   	pop    %ebp
 51f:	c3                   	ret    

00000520 <memmove>:

void*
memmove(void *vdst, const void *vsrc, int n)
{
 520:	55                   	push   %ebp
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
 521:	31 d2                	xor    %edx,%edx
{
 523:	89 e5                	mov    %esp,%ebp
 525:	56                   	push   %esi
 526:	8b 45 08             	mov    0x8(%ebp),%eax
 529:	53                   	push   %ebx
 52a:	8b 5d 10             	mov    0x10(%ebp),%ebx
 52d:	8b 75 0c             	mov    0xc(%ebp),%esi
  while(n-- > 0)
 530:	85 db                	test   %ebx,%ebx
 532:	7e 12                	jle    546 <memmove+0x26>
 534:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    *dst++ = *src++;
 538:	0f b6 0c 16          	movzbl (%esi,%edx,1),%ecx
 53c:	88 0c 10             	mov    %cl,(%eax,%edx,1)
 53f:	83 c2 01             	add    $0x1,%edx
  while(n-- > 0)
 542:	39 da                	cmp    %ebx,%edx
 544:	75 f2                	jne    538 <memmove+0x18>
  return vdst;
}
 546:	5b                   	pop    %ebx
 547:	5e                   	pop    %esi
 548:	5d                   	pop    %ebp
 549:	c3                   	ret    

0000054a <fork>:
 54a:	b8 01 00 00 00       	mov    $0x1,%eax
 54f:	cd 40                	int    $0x40
 551:	c3                   	ret    

00000552 <exit>:
 552:	b8 02 00 00 00       	mov    $0x2,%eax
 557:	cd 40                	int    $0x40
 559:	c3                   	ret    

0000055a <wait>:
 55a:	b8 03 00 00 00       	mov    $0x3,%eax
 55f:	cd 40                	int    $0x40
 561:	c3                   	ret    

00000562 <pipe>:
 562:	b8 04 00 00 00       	mov    $0x4,%eax
 567:	cd 40                	int    $0x40
 569:	c3                   	ret    

0000056a <read>:
 56a:	b8 05 00 00 00       	mov    $0x5,%eax
 56f:	cd 40                	int    $0x40
 571:	c3                   	ret    

00000572 <write>:
 572:	b8 10 00 00 00       	mov    $0x10,%eax
 577:	cd 40                	int    $0x40
 579:	c3                   	ret    

0000057a <close>:
 57a:	b8 15 00 00 00       	mov    $0x15,%eax
 57f:	cd 40                	int    $0x40
 581:	c3                   	ret    

00000582 <kill>:
 582:	b8 06 00 00 00       	mov    $0x6,%eax
 587:	cd 40                	int    $0x40
 589:	c3                   	ret    

0000058a <exec>:
 58a:	b8 07 00 00 00       	mov    $0x7,%eax
 58f:	cd 40                	int    $0x40
 591:	c3                   	ret    

00000592 <open>:
 592:	b8 0f 00 00 00       	mov    $0xf,%eax
 597:	cd 40                	int    $0x40
 599:	c3                   	ret    

0000059a <mknod>:
 59a:	b8 11 00 00 00       	mov    $0x11,%eax
 59f:	cd 40                	int    $0x40
 5a1:	c3                   	ret    

000005a2 <unlink>:
 5a2:	b8 12 00 00 00       	mov    $0x12,%eax
 5a7:	cd 40                	int    $0x40
 5a9:	c3                   	ret    

000005aa <fstat>:
 5aa:	b8 08 00 00 00       	mov    $0x8,%eax
 5af:	cd 40                	int    $0x40
 5b1:	c3                   	ret    

000005b2 <link>:
 5b2:	b8 13 00 00 00       	mov    $0x13,%eax
 5b7:	cd 40                	int    $0x40
 5b9:	c3                   	ret    

000005ba <mkdir>:
 5ba:	b8 14 00 00 00       	mov    $0x14,%eax
 5bf:	cd 40                	int    $0x40
 5c1:	c3                   	ret    

000005c2 <chdir>:
 5c2:	b8 09 00 00 00       	mov    $0x9,%eax
 5c7:	cd 40                	int    $0x40
 5c9:	c3                   	ret    

000005ca <dup>:
 5ca:	b8 0a 00 00 00       	mov    $0xa,%eax
 5cf:	cd 40                	int    $0x40
 5d1:	c3                   	ret    

000005d2 <getpid>:
 5d2:	b8 0b 00 00 00       	mov    $0xb,%eax
 5d7:	cd 40                	int    $0x40
 5d9:	c3                   	ret    

000005da <sbrk>:
 5da:	b8 0c 00 00 00       	mov    $0xc,%eax
 5df:	cd 40                	int    $0x40
 5e1:	c3                   	ret    

000005e2 <sleep>:
 5e2:	b8 0d 00 00 00       	mov    $0xd,%eax
 5e7:	cd 40                	int    $0x40
 5e9:	c3                   	ret    

000005ea <uptime>:
 5ea:	b8 0e 00 00 00       	mov    $0xe,%eax
 5ef:	cd 40                	int    $0x40
 5f1:	c3                   	ret    
 5f2:	66 90                	xchg   %ax,%ax
 5f4:	66 90                	xchg   %ax,%ax
 5f6:	66 90                	xchg   %ax,%ax
 5f8:	66 90                	xchg   %ax,%ax
 5fa:	66 90                	xchg   %ax,%ax
 5fc:	66 90                	xchg   %ax,%ax
 5fe:	66 90                	xchg   %ax,%ax

00000600 <printint>:
  write(fd, &c, 1);
}

static void
printint(int fd, int xx, int base, int sgn)
{
 600:	55                   	push   %ebp
 601:	89 e5                	mov    %esp,%ebp
 603:	57                   	push   %edi
 604:	56                   	push   %esi
 605:	89 c6                	mov    %eax,%esi
 607:	53                   	push   %ebx
 608:	83 ec 4c             	sub    $0x4c,%esp
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
 60b:	8b 5d 08             	mov    0x8(%ebp),%ebx
 60e:	85 db                	test   %ebx,%ebx
 610:	74 09                	je     61b <printint+0x1b>
 612:	89 d0                	mov    %edx,%eax
 614:	c1 e8 1f             	shr    $0x1f,%eax
 617:	84 c0                	test   %al,%al
 619:	75 75                	jne    690 <printint+0x90>
    neg = 1;
    x = -xx;
  } else {
    x = xx;
 61b:	89 d0                	mov    %edx,%eax
  neg = 0;
 61d:	c7 45 c4 00 00 00 00 	movl   $0x0,-0x3c(%ebp)
 624:	89 75 c0             	mov    %esi,-0x40(%ebp)
  }

  i = 0;
 627:	31 ff                	xor    %edi,%edi
 629:	89 ce                	mov    %ecx,%esi
 62b:	8d 5d d7             	lea    -0x29(%ebp),%ebx
 62e:	eb 02                	jmp    632 <printint+0x32>
  do{
    buf[i++] = digits[x % base];
 630:	89 cf                	mov    %ecx,%edi
 632:	31 d2                	xor    %edx,%edx
 634:	f7 f6                	div    %esi
 636:	8d 4f 01             	lea    0x1(%edi),%ecx
 639:	0f b6 92 45 0a 00 00 	movzbl 0xa45(%edx),%edx
  }while((x /= base) != 0);
 640:	85 c0                	test   %eax,%eax
    buf[i++] = digits[x % base];
 642:	88 14 0b             	mov    %dl,(%ebx,%ecx,1)
  }while((x /= base) != 0);
 645:	75 e9                	jne    630 <printint+0x30>
  if(neg)
 647:	8b 55 c4             	mov    -0x3c(%ebp),%edx
    buf[i++] = digits[x % base];
 64a:	89 c8                	mov    %ecx,%eax
 64c:	8b 75 c0             	mov    -0x40(%ebp),%esi
  if(neg)
 64f:	85 d2                	test   %edx,%edx
 651:	74 08                	je     65b <printint+0x5b>
    buf[i++] = '-';
 653:	8d 4f 02             	lea    0x2(%edi),%ecx
 656:	c6 44 05 d8 2d       	movb   $0x2d,-0x28(%ebp,%eax,1)

  while(--i >= 0)
 65b:	8d 79 ff             	lea    -0x1(%ecx),%edi
 65e:	66 90                	xchg   %ax,%ax
 660:	0f b6 44 3d d8       	movzbl -0x28(%ebp,%edi,1),%eax
 665:	83 ef 01             	sub    $0x1,%edi
  write(fd, &c, 1);
 668:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 66f:	00 
 670:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 674:	89 34 24             	mov    %esi,(%esp)
 677:	88 45 d7             	mov    %al,-0x29(%ebp)
 67a:	e8 f3 fe ff ff       	call   572 <write>
  while(--i >= 0)
 67f:	83 ff ff             	cmp    $0xffffffff,%edi
 682:	75 dc                	jne    660 <printint+0x60>
    putc(fd, buf[i]);
}
 684:	83 c4 4c             	add    $0x4c,%esp
 687:	5b                   	pop    %ebx
 688:	5e                   	pop    %esi
 689:	5f                   	pop    %edi
 68a:	5d                   	pop    %ebp
 68b:	c3                   	ret    
 68c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    x = -xx;
 690:	89 d0                	mov    %edx,%eax
 692:	f7 d8                	neg    %eax
    neg = 1;
 694:	c7 45 c4 01 00 00 00 	movl   $0x1,-0x3c(%ebp)
 69b:	eb 87                	jmp    624 <printint+0x24>
 69d:	8d 76 00             	lea    0x0(%esi),%esi

000006a0 <printf>:

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, const char *fmt, ...)
{
 6a0:	55                   	push   %ebp
 6a1:	89 e5                	mov    %esp,%ebp
 6a3:	57                   	push   %edi
  char *s;
  int c, i, state;
  uint *ap;

  state = 0;
 6a4:	31 ff                	xor    %edi,%edi
{
 6a6:	56                   	push   %esi
 6a7:	53                   	push   %ebx
 6a8:	83 ec 3c             	sub    $0x3c,%esp
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
 6ab:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  ap = (uint*)(void*)&fmt + 1;
 6ae:	8d 45 10             	lea    0x10(%ebp),%eax
{
 6b1:	8b 75 08             	mov    0x8(%ebp),%esi
  ap = (uint*)(void*)&fmt + 1;
 6b4:	89 45 d4             	mov    %eax,-0x2c(%ebp)
  for(i = 0; fmt[i]; i++){
 6b7:	0f b6 13             	movzbl (%ebx),%edx
 6ba:	83 c3 01             	add    $0x1,%ebx
 6bd:	84 d2                	test   %dl,%dl
 6bf:	75 39                	jne    6fa <printf+0x5a>
 6c1:	e9 c2 00 00 00       	jmp    788 <printf+0xe8>
 6c6:	66 90                	xchg   %ax,%ax
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
 6c8:	83 fa 25             	cmp    $0x25,%edx
 6cb:	0f 84 bf 00 00 00    	je     790 <printf+0xf0>
  write(fd, &c, 1);
 6d1:	8d 45 e2             	lea    -0x1e(%ebp),%eax
 6d4:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 6db:	00 
 6dc:	89 44 24 04          	mov    %eax,0x4(%esp)
 6e0:	89 34 24             	mov    %esi,(%esp)
        state = '%';
      } else {
        putc(fd, c);
 6e3:	88 55 e2             	mov    %dl,-0x1e(%ebp)
  write(fd, &c, 1);
 6e6:	e8 87 fe ff ff       	call   572 <write>
 6eb:	83 c3 01             	add    $0x1,%ebx
  for(i = 0; fmt[i]; i++){
 6ee:	0f b6 53 ff          	movzbl -0x1(%ebx),%edx
 6f2:	84 d2                	test   %dl,%dl
 6f4:	0f 84 8e 00 00 00    	je     788 <printf+0xe8>
    if(state == 0){
 6fa:	85 ff                	test   %edi,%edi
    c = fmt[i] & 0xff;
 6fc:	0f be c2             	movsbl %dl,%eax
    if(state == 0){
 6ff:	74 c7                	je     6c8 <printf+0x28>
      }
    } else if(state == '%'){
 701:	83 ff 25             	cmp    $0x25,%edi
 704:	75 e5                	jne    6eb <printf+0x4b>
      if(c == 'd'){
 706:	83 fa 64             	cmp    $0x64,%edx
 709:	0f 84 31 01 00 00    	je     840 <printf+0x1a0>
        printint(fd, *ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
 70f:	25 f7 00 00 00       	and    $0xf7,%eax
 714:	83 f8 70             	cmp    $0x70,%eax
 717:	0f 84 83 00 00 00    	je     7a0 <printf+0x100>
        printint(fd, *ap, 16, 0);
        ap++;
      } else if(c == 's'){
 71d:	83 fa 73             	cmp    $0x73,%edx
 720:	0f 84 a2 00 00 00    	je     7c8 <printf+0x128>
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
        }
      } else if(c == 'c'){
 726:	83 fa 63             	cmp    $0x63,%edx
 729:	0f 84 35 01 00 00    	je     864 <printf+0x1c4>
        putc(fd, *ap);
        ap++;
      } else if(c == '%'){
 72f:	83 fa 25             	cmp    $0x25,%edx
 732:	0f 84 e0 00 00 00    	je     818 <printf+0x178>
  write(fd, &c, 1);
 738:	8d 45 e6             	lea    -0x1a(%ebp),%eax
 73b:	83 c3 01             	add    $0x1,%ebx
 73e:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 745:	00 
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
 746:	31 ff                	xor    %edi,%edi
  write(fd, &c, 1);
 748:	89 44 24 04          	mov    %eax,0x4(%esp)
 74c:	89 34 24             	mov    %esi,(%esp)
 74f:	89 55 d0             	mov    %edx,-0x30(%ebp)
 752:	c6 45 e6 25          	movb   $0x25,-0x1a(%ebp)
 756:	e8 17 fe ff ff       	call   572 <write>
        putc(fd, c);
 75b:	8b 55 d0             	mov    -0x30(%ebp),%edx
  write(fd, &c, 1);
 75e:	8d 45 e7             	lea    -0x19(%ebp),%eax
 761:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 768:	00 
 769:	89 44 24 04          	mov    %eax,0x4(%esp)
 76d:	89 34 24             	mov    %esi,(%esp)
        putc(fd, c);
 770:	88 55 e7             	mov    %dl,-0x19(%ebp)
  write(fd, &c, 1);
 773:	e8 fa fd ff ff       	call   572 <write>
  for(i = 0; fmt[i]; i++){
 778:	0f b6 53 ff          	movzbl -0x1(%ebx),%edx
 77c:	84 d2                	test   %dl,%dl
 77e:	0f 85 76 ff ff ff    	jne    6fa <printf+0x5a>
 784:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    }
  }
}
 788:	83 c4 3c             	add    $0x3c,%esp
 78b:	5b                   	pop    %ebx
 78c:	5e                   	pop    %esi
 78d:	5f                   	pop    %edi
 78e:	5d                   	pop    %ebp
 78f:	c3                   	ret    
        state = '%';
 790:	bf 25 00 00 00       	mov    $0x25,%edi
 795:	e9 51 ff ff ff       	jmp    6eb <printf+0x4b>
 79a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        printint(fd, *ap, 16, 0);
 7a0:	8b 45 d4             	mov    -0x2c(%ebp),%eax
 7a3:	b9 10 00 00 00       	mov    $0x10,%ecx
      state = 0;
 7a8:	31 ff                	xor    %edi,%edi
        printint(fd, *ap, 16, 0);
 7aa:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
 7b1:	8b 10                	mov    (%eax),%edx
 7b3:	89 f0                	mov    %esi,%eax
 7b5:	e8 46 fe ff ff       	call   600 <printint>
        ap++;
 7ba:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
 7be:	e9 28 ff ff ff       	jmp    6eb <printf+0x4b>
 7c3:	90                   	nop
 7c4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
        s = (char*)*ap;
 7c8:	8b 45 d4             	mov    -0x2c(%ebp),%eax
        ap++;
 7cb:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
        s = (char*)*ap;
 7cf:	8b 38                	mov    (%eax),%edi
          s = "(null)";
 7d1:	b8 3e 0a 00 00       	mov    $0xa3e,%eax
 7d6:	85 ff                	test   %edi,%edi
 7d8:	0f 44 f8             	cmove  %eax,%edi
        while(*s != 0){
 7db:	0f b6 07             	movzbl (%edi),%eax
 7de:	84 c0                	test   %al,%al
 7e0:	74 2a                	je     80c <printf+0x16c>
 7e2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 7e8:	88 45 e3             	mov    %al,-0x1d(%ebp)
  write(fd, &c, 1);
 7eb:	8d 45 e3             	lea    -0x1d(%ebp),%eax
          s++;
 7ee:	83 c7 01             	add    $0x1,%edi
  write(fd, &c, 1);
 7f1:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 7f8:	00 
 7f9:	89 44 24 04          	mov    %eax,0x4(%esp)
 7fd:	89 34 24             	mov    %esi,(%esp)
 800:	e8 6d fd ff ff       	call   572 <write>
        while(*s != 0){
 805:	0f b6 07             	movzbl (%edi),%eax
 808:	84 c0                	test   %al,%al
 80a:	75 dc                	jne    7e8 <printf+0x148>
      state = 0;
 80c:	31 ff                	xor    %edi,%edi
 80e:	e9 d8 fe ff ff       	jmp    6eb <printf+0x4b>
 813:	90                   	nop
 814:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  write(fd, &c, 1);
 818:	8d 45 e5             	lea    -0x1b(%ebp),%eax
      state = 0;
 81b:	31 ff                	xor    %edi,%edi
  write(fd, &c, 1);
 81d:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 824:	00 
 825:	89 44 24 04          	mov    %eax,0x4(%esp)
 829:	89 34 24             	mov    %esi,(%esp)
 82c:	c6 45 e5 25          	movb   $0x25,-0x1b(%ebp)
 830:	e8 3d fd ff ff       	call   572 <write>
 835:	e9 b1 fe ff ff       	jmp    6eb <printf+0x4b>
 83a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        printint(fd, *ap, 10, 1);
 840:	8b 45 d4             	mov    -0x2c(%ebp),%eax
 843:	b9 0a 00 00 00       	mov    $0xa,%ecx
      state = 0;
 848:	66 31 ff             	xor    %di,%di
        printint(fd, *ap, 10, 1);
 84b:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 852:	8b 10                	mov    (%eax),%edx
 854:	89 f0                	mov    %esi,%eax
 856:	e8 a5 fd ff ff       	call   600 <printint>
        ap++;
 85b:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
 85f:	e9 87 fe ff ff       	jmp    6eb <printf+0x4b>
        putc(fd, *ap);
 864:	8b 45 d4             	mov    -0x2c(%ebp),%eax
      state = 0;
 867:	31 ff                	xor    %edi,%edi
        putc(fd, *ap);
 869:	8b 00                	mov    (%eax),%eax
  write(fd, &c, 1);
 86b:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 872:	00 
 873:	89 34 24             	mov    %esi,(%esp)
        putc(fd, *ap);
 876:	88 45 e4             	mov    %al,-0x1c(%ebp)
  write(fd, &c, 1);
 879:	8d 45 e4             	lea    -0x1c(%ebp),%eax
 87c:	89 44 24 04          	mov    %eax,0x4(%esp)
 880:	e8 ed fc ff ff       	call   572 <write>
        ap++;
 885:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
 889:	e9 5d fe ff ff       	jmp    6eb <printf+0x4b>
 88e:	66 90                	xchg   %ax,%ax

00000890 <free>:
static Header base;
static Header *freep;

void
free(void *ap)
{
 890:	55                   	push   %ebp
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 891:	a1 a0 0d 00 00       	mov    0xda0,%eax
{
 896:	89 e5                	mov    %esp,%ebp
 898:	57                   	push   %edi
 899:	56                   	push   %esi
 89a:	53                   	push   %ebx
 89b:	8b 5d 08             	mov    0x8(%ebp),%ebx
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 89e:	8b 08                	mov    (%eax),%ecx
  bp = (Header*)ap - 1;
 8a0:	8d 53 f8             	lea    -0x8(%ebx),%edx
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 8a3:	39 d0                	cmp    %edx,%eax
 8a5:	72 11                	jb     8b8 <free+0x28>
 8a7:	90                   	nop
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 8a8:	39 c8                	cmp    %ecx,%eax
 8aa:	72 04                	jb     8b0 <free+0x20>
 8ac:	39 ca                	cmp    %ecx,%edx
 8ae:	72 10                	jb     8c0 <free+0x30>
 8b0:	89 c8                	mov    %ecx,%eax
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 8b2:	39 d0                	cmp    %edx,%eax
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 8b4:	8b 08                	mov    (%eax),%ecx
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 8b6:	73 f0                	jae    8a8 <free+0x18>
 8b8:	39 ca                	cmp    %ecx,%edx
 8ba:	72 04                	jb     8c0 <free+0x30>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 8bc:	39 c8                	cmp    %ecx,%eax
 8be:	72 f0                	jb     8b0 <free+0x20>
      break;
  if(bp + bp->s.size == p->s.ptr){
 8c0:	8b 73 fc             	mov    -0x4(%ebx),%esi
 8c3:	8d 3c f2             	lea    (%edx,%esi,8),%edi
 8c6:	39 cf                	cmp    %ecx,%edi
 8c8:	74 1e                	je     8e8 <free+0x58>
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
 8ca:	89 4b f8             	mov    %ecx,-0x8(%ebx)
  if(p + p->s.size == bp){
 8cd:	8b 48 04             	mov    0x4(%eax),%ecx
 8d0:	8d 34 c8             	lea    (%eax,%ecx,8),%esi
 8d3:	39 f2                	cmp    %esi,%edx
 8d5:	74 28                	je     8ff <free+0x6f>
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
 8d7:	89 10                	mov    %edx,(%eax)
  freep = p;
 8d9:	a3 a0 0d 00 00       	mov    %eax,0xda0
}
 8de:	5b                   	pop    %ebx
 8df:	5e                   	pop    %esi
 8e0:	5f                   	pop    %edi
 8e1:	5d                   	pop    %ebp
 8e2:	c3                   	ret    
 8e3:	90                   	nop
 8e4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    bp->s.size += p->s.ptr->s.size;
 8e8:	03 71 04             	add    0x4(%ecx),%esi
 8eb:	89 73 fc             	mov    %esi,-0x4(%ebx)
    bp->s.ptr = p->s.ptr->s.ptr;
 8ee:	8b 08                	mov    (%eax),%ecx
 8f0:	8b 09                	mov    (%ecx),%ecx
 8f2:	89 4b f8             	mov    %ecx,-0x8(%ebx)
  if(p + p->s.size == bp){
 8f5:	8b 48 04             	mov    0x4(%eax),%ecx
 8f8:	8d 34 c8             	lea    (%eax,%ecx,8),%esi
 8fb:	39 f2                	cmp    %esi,%edx
 8fd:	75 d8                	jne    8d7 <free+0x47>
    p->s.size += bp->s.size;
 8ff:	03 4b fc             	add    -0x4(%ebx),%ecx
  freep = p;
 902:	a3 a0 0d 00 00       	mov    %eax,0xda0
    p->s.size += bp->s.size;
 907:	89 48 04             	mov    %ecx,0x4(%eax)
    p->s.ptr = bp->s.ptr;
 90a:	8b 53 f8             	mov    -0x8(%ebx),%edx
 90d:	89 10                	mov    %edx,(%eax)
}
 90f:	5b                   	pop    %ebx
 910:	5e                   	pop    %esi
 911:	5f                   	pop    %edi
 912:	5d                   	pop    %ebp
 913:	c3                   	ret    
 914:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 91a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

00000920 <malloc>:
  return freep;
}

void*
malloc(uint nbytes)
{
 920:	55                   	push   %ebp
 921:	89 e5                	mov    %esp,%ebp
 923:	57                   	push   %edi
 924:	56                   	push   %esi
 925:	53                   	push   %ebx
 926:	83 ec 1c             	sub    $0x1c,%esp
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 929:	8b 45 08             	mov    0x8(%ebp),%eax
  if((prevp = freep) == 0){
 92c:	8b 1d a0 0d 00 00    	mov    0xda0,%ebx
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 932:	8d 48 07             	lea    0x7(%eax),%ecx
 935:	c1 e9 03             	shr    $0x3,%ecx
  if((prevp = freep) == 0){
 938:	85 db                	test   %ebx,%ebx
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 93a:	8d 71 01             	lea    0x1(%ecx),%esi
  if((prevp = freep) == 0){
 93d:	0f 84 9b 00 00 00    	je     9de <malloc+0xbe>
 943:	8b 13                	mov    (%ebx),%edx
 945:	8b 7a 04             	mov    0x4(%edx),%edi
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
 948:	39 fe                	cmp    %edi,%esi
 94a:	76 64                	jbe    9b0 <malloc+0x90>
 94c:	8d 04 f5 00 00 00 00 	lea    0x0(,%esi,8),%eax
  if(nu < 4096)
 953:	bb 00 80 00 00       	mov    $0x8000,%ebx
 958:	89 45 e4             	mov    %eax,-0x1c(%ebp)
 95b:	eb 0e                	jmp    96b <malloc+0x4b>
 95d:	8d 76 00             	lea    0x0(%esi),%esi
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
 960:	8b 02                	mov    (%edx),%eax
    if(p->s.size >= nunits){
 962:	8b 78 04             	mov    0x4(%eax),%edi
 965:	39 fe                	cmp    %edi,%esi
 967:	76 4f                	jbe    9b8 <malloc+0x98>
 969:	89 c2                	mov    %eax,%edx
        p->s.size = nunits;
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
 96b:	3b 15 a0 0d 00 00    	cmp    0xda0,%edx
 971:	75 ed                	jne    960 <malloc+0x40>
  if(nu < 4096)
 973:	8b 45 e4             	mov    -0x1c(%ebp),%eax
 976:	81 fe 00 10 00 00    	cmp    $0x1000,%esi
 97c:	bf 00 10 00 00       	mov    $0x1000,%edi
 981:	0f 43 fe             	cmovae %esi,%edi
 984:	0f 42 c3             	cmovb  %ebx,%eax
  p = sbrk(nu * sizeof(Header));
 987:	89 04 24             	mov    %eax,(%esp)
 98a:	e8 4b fc ff ff       	call   5da <sbrk>
  if(p == (char*)-1)
 98f:	83 f8 ff             	cmp    $0xffffffff,%eax
 992:	74 18                	je     9ac <malloc+0x8c>
  hp->s.size = nu;
 994:	89 78 04             	mov    %edi,0x4(%eax)
  free((void*)(hp + 1));
 997:	83 c0 08             	add    $0x8,%eax
 99a:	89 04 24             	mov    %eax,(%esp)
 99d:	e8 ee fe ff ff       	call   890 <free>
  return freep;
 9a2:	8b 15 a0 0d 00 00    	mov    0xda0,%edx
      if((p = morecore(nunits)) == 0)
 9a8:	85 d2                	test   %edx,%edx
 9aa:	75 b4                	jne    960 <malloc+0x40>
        return 0;
 9ac:	31 c0                	xor    %eax,%eax
 9ae:	eb 20                	jmp    9d0 <malloc+0xb0>
    if(p->s.size >= nunits){
 9b0:	89 d0                	mov    %edx,%eax
 9b2:	89 da                	mov    %ebx,%edx
 9b4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      if(p->s.size == nunits)
 9b8:	39 fe                	cmp    %edi,%esi
 9ba:	74 1c                	je     9d8 <malloc+0xb8>
        p->s.size -= nunits;
 9bc:	29 f7                	sub    %esi,%edi
 9be:	89 78 04             	mov    %edi,0x4(%eax)
        p += p->s.size;
 9c1:	8d 04 f8             	lea    (%eax,%edi,8),%eax
        p->s.size = nunits;
 9c4:	89 70 04             	mov    %esi,0x4(%eax)
      freep = prevp;
 9c7:	89 15 a0 0d 00 00    	mov    %edx,0xda0
      return (void*)(p + 1);
 9cd:	83 c0 08             	add    $0x8,%eax
  }
}
 9d0:	83 c4 1c             	add    $0x1c,%esp
 9d3:	5b                   	pop    %ebx
 9d4:	5e                   	pop    %esi
 9d5:	5f                   	pop    %edi
 9d6:	5d                   	pop    %ebp
 9d7:	c3                   	ret    
        prevp->s.ptr = p->s.ptr;
 9d8:	8b 08                	mov    (%eax),%ecx
 9da:	89 0a                	mov    %ecx,(%edx)
 9dc:	eb e9                	jmp    9c7 <malloc+0xa7>
    base.s.ptr = freep = prevp = &base;
 9de:	c7 05 a0 0d 00 00 a4 	movl   $0xda4,0xda0
 9e5:	0d 00 00 
    base.s.size = 0;
 9e8:	ba a4 0d 00 00       	mov    $0xda4,%edx
    base.s.ptr = freep = prevp = &base;
 9ed:	c7 05 a4 0d 00 00 a4 	movl   $0xda4,0xda4
 9f4:	0d 00 00 
    base.s.size = 0;
 9f7:	c7 05 a8 0d 00 00 00 	movl   $0x0,0xda8
 9fe:	00 00 00 
 a01:	e9 46 ff ff ff       	jmp    94c <malloc+0x2c>
