
_sh:     file format elf32-i386


Disassembly of section .text:

00000000 <main>:
  return 0;
}

int
main(void)
{
       0:	55                   	push   %ebp
       1:	89 e5                	mov    %esp,%ebp
       3:	83 e4 f0             	and    $0xfffffff0,%esp
       6:	83 ec 10             	sub    $0x10,%esp
  static char buf[100];
  int fd;

  // Ensure that three file descriptors are open.
  while((fd = open("console", O_RDWR)) >= 0){
       9:	eb 0e                	jmp    19 <main+0x19>
       b:	90                   	nop
       c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    if(fd >= 3){
      10:	83 f8 02             	cmp    $0x2,%eax
      13:	0f 8f d4 00 00 00    	jg     ed <main+0xed>
  while((fd = open("console", O_RDWR)) >= 0){
      19:	c7 44 24 04 02 00 00 	movl   $0x2,0x4(%esp)
      20:	00 
      21:	c7 04 24 39 13 00 00 	movl   $0x1339,(%esp)
      28:	e8 f5 0d 00 00       	call   e22 <open>
      2d:	85 c0                	test   %eax,%eax
      2f:	79 df                	jns    10 <main+0x10>
      31:	eb 23                	jmp    56 <main+0x56>
      33:	90                   	nop
      34:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    }
  }

  // Read and run input commands.
  while(getcmd(buf, sizeof(buf)) >= 0){
    if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' '){
      38:	80 3d 42 19 00 00 20 	cmpb   $0x20,0x1942
      3f:	90                   	nop
      40:	74 67                	je     a9 <main+0xa9>
      42:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
      buf[strlen(buf)-1] = 0;  // chop \n
      if(chdir(buf+3) < 0)
        printf(2, "cannot cd %s\n", buf+3);
      continue;
    }
    if(fork1() == 0)
      48:	e8 43 01 00 00       	call   190 <fork1>
      4d:	85 c0                	test   %eax,%eax
      4f:	74 38                	je     89 <main+0x89>
      runcmd(parsecmd(buf));
    wait();
      51:	e8 94 0d 00 00       	call   dea <wait>
  while(getcmd(buf, sizeof(buf)) >= 0){
      56:	c7 44 24 04 64 00 00 	movl   $0x64,0x4(%esp)
      5d:	00 
      5e:	c7 04 24 40 19 00 00 	movl   $0x1940,(%esp)
      65:	e8 96 00 00 00       	call   100 <getcmd>
      6a:	85 c0                	test   %eax,%eax
      6c:	78 2f                	js     9d <main+0x9d>
    if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' '){
      6e:	80 3d 40 19 00 00 63 	cmpb   $0x63,0x1940
      75:	75 d1                	jne    48 <main+0x48>
      77:	80 3d 41 19 00 00 64 	cmpb   $0x64,0x1941
      7e:	74 b8                	je     38 <main+0x38>
    if(fork1() == 0)
      80:	e8 0b 01 00 00       	call   190 <fork1>
      85:	85 c0                	test   %eax,%eax
      87:	75 c8                	jne    51 <main+0x51>
      runcmd(parsecmd(buf));
      89:	c7 04 24 40 19 00 00 	movl   $0x1940,(%esp)
      90:	e8 ab 0a 00 00       	call   b40 <parsecmd>
      95:	89 04 24             	mov    %eax,(%esp)
      98:	e8 13 01 00 00       	call   1b0 <runcmd>
  }
  exit(0);
      9d:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
      a4:	e8 39 0d 00 00       	call   de2 <exit>
      buf[strlen(buf)-1] = 0;  // chop \n
      a9:	c7 04 24 40 19 00 00 	movl   $0x1940,(%esp)
      b0:	e8 8b 0b 00 00       	call   c40 <strlen>
      if(chdir(buf+3) < 0)
      b5:	c7 04 24 43 19 00 00 	movl   $0x1943,(%esp)
      buf[strlen(buf)-1] = 0;  // chop \n
      bc:	c6 80 3f 19 00 00 00 	movb   $0x0,0x193f(%eax)
      if(chdir(buf+3) < 0)
      c3:	e8 8a 0d 00 00       	call   e52 <chdir>
      c8:	85 c0                	test   %eax,%eax
      ca:	79 8a                	jns    56 <main+0x56>
        printf(2, "cannot cd %s\n", buf+3);
      cc:	c7 44 24 08 43 19 00 	movl   $0x1943,0x8(%esp)
      d3:	00 
      d4:	c7 44 24 04 41 13 00 	movl   $0x1341,0x4(%esp)
      db:	00 
      dc:	c7 04 24 02 00 00 00 	movl   $0x2,(%esp)
      e3:	e8 48 0e 00 00       	call   f30 <printf>
      e8:	e9 69 ff ff ff       	jmp    56 <main+0x56>
      close(fd);
      ed:	89 04 24             	mov    %eax,(%esp)
      f0:	e8 15 0d 00 00       	call   e0a <close>
      break;
      f5:	e9 5c ff ff ff       	jmp    56 <main+0x56>
      fa:	66 90                	xchg   %ax,%ax
      fc:	66 90                	xchg   %ax,%ax
      fe:	66 90                	xchg   %ax,%ax

00000100 <getcmd>:
{
     100:	55                   	push   %ebp
     101:	89 e5                	mov    %esp,%ebp
     103:	56                   	push   %esi
     104:	53                   	push   %ebx
     105:	83 ec 10             	sub    $0x10,%esp
     108:	8b 5d 08             	mov    0x8(%ebp),%ebx
     10b:	8b 75 0c             	mov    0xc(%ebp),%esi
  printf(2, "$ ");
     10e:	c7 44 24 04 98 12 00 	movl   $0x1298,0x4(%esp)
     115:	00 
     116:	c7 04 24 02 00 00 00 	movl   $0x2,(%esp)
     11d:	e8 0e 0e 00 00       	call   f30 <printf>
  memset(buf, 0, nbuf);
     122:	89 74 24 08          	mov    %esi,0x8(%esp)
     126:	89 1c 24             	mov    %ebx,(%esp)
     129:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
     130:	00 
     131:	e8 3a 0b 00 00       	call   c70 <memset>
  gets(buf, nbuf);
     136:	89 74 24 04          	mov    %esi,0x4(%esp)
     13a:	89 1c 24             	mov    %ebx,(%esp)
     13d:	e8 8e 0b 00 00       	call   cd0 <gets>
  if(buf[0] == 0) // EOF
     142:	31 c0                	xor    %eax,%eax
     144:	80 3b 00             	cmpb   $0x0,(%ebx)
     147:	0f 94 c0             	sete   %al
}
     14a:	83 c4 10             	add    $0x10,%esp
     14d:	5b                   	pop    %ebx
  if(buf[0] == 0) // EOF
     14e:	f7 d8                	neg    %eax
}
     150:	5e                   	pop    %esi
     151:	5d                   	pop    %ebp
     152:	c3                   	ret    
     153:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
     159:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000160 <panic>:
}

void
panic(char *s)
{
     160:	55                   	push   %ebp
     161:	89 e5                	mov    %esp,%ebp
     163:	83 ec 18             	sub    $0x18,%esp
  printf(2, "%s\n", s);
     166:	8b 45 08             	mov    0x8(%ebp),%eax
     169:	c7 44 24 04 35 13 00 	movl   $0x1335,0x4(%esp)
     170:	00 
     171:	c7 04 24 02 00 00 00 	movl   $0x2,(%esp)
     178:	89 44 24 08          	mov    %eax,0x8(%esp)
     17c:	e8 af 0d 00 00       	call   f30 <printf>
  exit(0);
     181:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     188:	e8 55 0c 00 00       	call   de2 <exit>
     18d:	8d 76 00             	lea    0x0(%esi),%esi

00000190 <fork1>:
}

int
fork1(void)
{
     190:	55                   	push   %ebp
     191:	89 e5                	mov    %esp,%ebp
     193:	83 ec 18             	sub    $0x18,%esp
  int pid;

  pid = fork();
     196:	e8 3f 0c 00 00       	call   dda <fork>
  if(pid == -1)
     19b:	83 f8 ff             	cmp    $0xffffffff,%eax
     19e:	74 02                	je     1a2 <fork1+0x12>
    panic("fork");
  return pid;
}
     1a0:	c9                   	leave  
     1a1:	c3                   	ret    
    panic("fork");
     1a2:	c7 04 24 9b 12 00 00 	movl   $0x129b,(%esp)
     1a9:	e8 b2 ff ff ff       	call   160 <panic>
     1ae:	66 90                	xchg   %ax,%ax

000001b0 <runcmd>:
{
     1b0:	55                   	push   %ebp
     1b1:	89 e5                	mov    %esp,%ebp
     1b3:	53                   	push   %ebx
     1b4:	83 ec 24             	sub    $0x24,%esp
     1b7:	8b 5d 08             	mov    0x8(%ebp),%ebx
  if(cmd == 0)
     1ba:	85 db                	test   %ebx,%ebx
     1bc:	74 5f                	je     21d <runcmd+0x6d>
  switch(cmd->type){
     1be:	83 3b 05             	cmpl   $0x5,(%ebx)
     1c1:	0f 87 e7 00 00 00    	ja     2ae <runcmd+0xfe>
     1c7:	8b 03                	mov    (%ebx),%eax
     1c9:	ff 24 85 50 13 00 00 	jmp    *0x1350(,%eax,4)
    if(pipe(p) < 0)
     1d0:	8d 45 f0             	lea    -0x10(%ebp),%eax
     1d3:	89 04 24             	mov    %eax,(%esp)
     1d6:	e8 17 0c 00 00       	call   df2 <pipe>
     1db:	85 c0                	test   %eax,%eax
     1dd:	0f 88 d7 00 00 00    	js     2ba <runcmd+0x10a>
    if(fork1() == 0){
     1e3:	e8 a8 ff ff ff       	call   190 <fork1>
     1e8:	85 c0                	test   %eax,%eax
     1ea:	0f 84 2e 01 00 00    	je     31e <runcmd+0x16e>
    if(fork1() == 0){
     1f0:	e8 9b ff ff ff       	call   190 <fork1>
     1f5:	85 c0                	test   %eax,%eax
     1f7:	0f 84 e9 00 00 00    	je     2e6 <runcmd+0x136>
    close(p[0]);
     1fd:	8b 45 f0             	mov    -0x10(%ebp),%eax
     200:	89 04 24             	mov    %eax,(%esp)
     203:	e8 02 0c 00 00       	call   e0a <close>
    close(p[1]);
     208:	8b 45 f4             	mov    -0xc(%ebp),%eax
     20b:	89 04 24             	mov    %eax,(%esp)
     20e:	e8 f7 0b 00 00       	call   e0a <close>
    wait();
     213:	e8 d2 0b 00 00       	call   dea <wait>
    wait();
     218:	e8 cd 0b 00 00       	call   dea <wait>
      exit(0);
     21d:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     224:	e8 b9 0b 00 00       	call   de2 <exit>
    if(fork1() == 0)
     229:	e8 62 ff ff ff       	call   190 <fork1>
     22e:	85 c0                	test   %eax,%eax
     230:	75 eb                	jne    21d <runcmd+0x6d>
     232:	eb 6f                	jmp    2a3 <runcmd+0xf3>
    if(ecmd->argv[0] == 0)
     234:	8b 43 04             	mov    0x4(%ebx),%eax
     237:	85 c0                	test   %eax,%eax
     239:	74 e2                	je     21d <runcmd+0x6d>
    exec(ecmd->argv[0], ecmd->argv);
     23b:	8d 53 04             	lea    0x4(%ebx),%edx
     23e:	89 54 24 04          	mov    %edx,0x4(%esp)
     242:	89 04 24             	mov    %eax,(%esp)
     245:	e8 d0 0b 00 00       	call   e1a <exec>
    printf(2, "exec %s failed\n", ecmd->argv[0]);
     24a:	8b 43 04             	mov    0x4(%ebx),%eax
     24d:	c7 44 24 04 a7 12 00 	movl   $0x12a7,0x4(%esp)
     254:	00 
     255:	c7 04 24 02 00 00 00 	movl   $0x2,(%esp)
     25c:	89 44 24 08          	mov    %eax,0x8(%esp)
     260:	e8 cb 0c 00 00       	call   f30 <printf>
    break;
     265:	eb b6                	jmp    21d <runcmd+0x6d>
    if(fork1() == 0)
     267:	e8 24 ff ff ff       	call   190 <fork1>
     26c:	85 c0                	test   %eax,%eax
     26e:	66 90                	xchg   %ax,%ax
     270:	74 31                	je     2a3 <runcmd+0xf3>
    wait();
     272:	e8 73 0b 00 00       	call   dea <wait>
    runcmd(lcmd->right);
     277:	8b 43 08             	mov    0x8(%ebx),%eax
     27a:	89 04 24             	mov    %eax,(%esp)
     27d:	e8 2e ff ff ff       	call   1b0 <runcmd>
    close(rcmd->fd);
     282:	8b 43 14             	mov    0x14(%ebx),%eax
     285:	89 04 24             	mov    %eax,(%esp)
     288:	e8 7d 0b 00 00       	call   e0a <close>
    if(open(rcmd->file, rcmd->mode) < 0){
     28d:	8b 43 10             	mov    0x10(%ebx),%eax
     290:	89 44 24 04          	mov    %eax,0x4(%esp)
     294:	8b 43 08             	mov    0x8(%ebx),%eax
     297:	89 04 24             	mov    %eax,(%esp)
     29a:	e8 83 0b 00 00       	call   e22 <open>
     29f:	85 c0                	test   %eax,%eax
     2a1:	78 23                	js     2c6 <runcmd+0x116>
      runcmd(bcmd->cmd);
     2a3:	8b 43 04             	mov    0x4(%ebx),%eax
     2a6:	89 04 24             	mov    %eax,(%esp)
     2a9:	e8 02 ff ff ff       	call   1b0 <runcmd>
    panic("runcmd");
     2ae:	c7 04 24 a0 12 00 00 	movl   $0x12a0,(%esp)
     2b5:	e8 a6 fe ff ff       	call   160 <panic>
      panic("pipe");
     2ba:	c7 04 24 c7 12 00 00 	movl   $0x12c7,(%esp)
     2c1:	e8 9a fe ff ff       	call   160 <panic>
      printf(2, "open %s failed\n", rcmd->file);
     2c6:	8b 43 08             	mov    0x8(%ebx),%eax
     2c9:	c7 44 24 04 b7 12 00 	movl   $0x12b7,0x4(%esp)
     2d0:	00 
     2d1:	c7 04 24 02 00 00 00 	movl   $0x2,(%esp)
     2d8:	89 44 24 08          	mov    %eax,0x8(%esp)
     2dc:	e8 4f 0c 00 00       	call   f30 <printf>
     2e1:	e9 37 ff ff ff       	jmp    21d <runcmd+0x6d>
      close(0);
     2e6:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     2ed:	e8 18 0b 00 00       	call   e0a <close>
      dup(p[0]);
     2f2:	8b 45 f0             	mov    -0x10(%ebp),%eax
     2f5:	89 04 24             	mov    %eax,(%esp)
     2f8:	e8 5d 0b 00 00       	call   e5a <dup>
      close(p[0]);
     2fd:	8b 45 f0             	mov    -0x10(%ebp),%eax
     300:	89 04 24             	mov    %eax,(%esp)
     303:	e8 02 0b 00 00       	call   e0a <close>
      close(p[1]);
     308:	8b 45 f4             	mov    -0xc(%ebp),%eax
     30b:	89 04 24             	mov    %eax,(%esp)
     30e:	e8 f7 0a 00 00       	call   e0a <close>
      runcmd(pcmd->right);
     313:	8b 43 08             	mov    0x8(%ebx),%eax
     316:	89 04 24             	mov    %eax,(%esp)
     319:	e8 92 fe ff ff       	call   1b0 <runcmd>
      close(1);
     31e:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     325:	e8 e0 0a 00 00       	call   e0a <close>
      dup(p[1]);
     32a:	8b 45 f4             	mov    -0xc(%ebp),%eax
     32d:	89 04 24             	mov    %eax,(%esp)
     330:	e8 25 0b 00 00       	call   e5a <dup>
      close(p[0]);
     335:	8b 45 f0             	mov    -0x10(%ebp),%eax
     338:	89 04 24             	mov    %eax,(%esp)
     33b:	e8 ca 0a 00 00       	call   e0a <close>
      close(p[1]);
     340:	8b 45 f4             	mov    -0xc(%ebp),%eax
     343:	89 04 24             	mov    %eax,(%esp)
     346:	e8 bf 0a 00 00       	call   e0a <close>
      runcmd(pcmd->left);
     34b:	8b 43 04             	mov    0x4(%ebx),%eax
     34e:	89 04 24             	mov    %eax,(%esp)
     351:	e8 5a fe ff ff       	call   1b0 <runcmd>
     356:	8d 76 00             	lea    0x0(%esi),%esi
     359:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000360 <execcmd>:
//PAGEBREAK!
// Constructors

struct cmd*
execcmd(void)
{
     360:	55                   	push   %ebp
     361:	89 e5                	mov    %esp,%ebp
     363:	53                   	push   %ebx
     364:	83 ec 14             	sub    $0x14,%esp
  struct execcmd *cmd;

  cmd = malloc(sizeof(*cmd));
     367:	c7 04 24 54 00 00 00 	movl   $0x54,(%esp)
     36e:	e8 3d 0e 00 00       	call   11b0 <malloc>
  memset(cmd, 0, sizeof(*cmd));
     373:	c7 44 24 08 54 00 00 	movl   $0x54,0x8(%esp)
     37a:	00 
     37b:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
     382:	00 
  cmd = malloc(sizeof(*cmd));
     383:	89 c3                	mov    %eax,%ebx
  memset(cmd, 0, sizeof(*cmd));
     385:	89 04 24             	mov    %eax,(%esp)
     388:	e8 e3 08 00 00       	call   c70 <memset>
  cmd->type = EXEC;
  return (struct cmd*)cmd;
}
     38d:	89 d8                	mov    %ebx,%eax
  cmd->type = EXEC;
     38f:	c7 03 01 00 00 00    	movl   $0x1,(%ebx)
}
     395:	83 c4 14             	add    $0x14,%esp
     398:	5b                   	pop    %ebx
     399:	5d                   	pop    %ebp
     39a:	c3                   	ret    
     39b:	90                   	nop
     39c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

000003a0 <redircmd>:

struct cmd*
redircmd(struct cmd *subcmd, char *file, char *efile, int mode, int fd)
{
     3a0:	55                   	push   %ebp
     3a1:	89 e5                	mov    %esp,%ebp
     3a3:	53                   	push   %ebx
     3a4:	83 ec 14             	sub    $0x14,%esp
  struct redircmd *cmd;

  cmd = malloc(sizeof(*cmd));
     3a7:	c7 04 24 18 00 00 00 	movl   $0x18,(%esp)
     3ae:	e8 fd 0d 00 00       	call   11b0 <malloc>
  memset(cmd, 0, sizeof(*cmd));
     3b3:	c7 44 24 08 18 00 00 	movl   $0x18,0x8(%esp)
     3ba:	00 
     3bb:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
     3c2:	00 
     3c3:	89 04 24             	mov    %eax,(%esp)
  cmd = malloc(sizeof(*cmd));
     3c6:	89 c3                	mov    %eax,%ebx
  memset(cmd, 0, sizeof(*cmd));
     3c8:	e8 a3 08 00 00       	call   c70 <memset>
  cmd->type = REDIR;
  cmd->cmd = subcmd;
     3cd:	8b 45 08             	mov    0x8(%ebp),%eax
  cmd->type = REDIR;
     3d0:	c7 03 02 00 00 00    	movl   $0x2,(%ebx)
  cmd->cmd = subcmd;
     3d6:	89 43 04             	mov    %eax,0x4(%ebx)
  cmd->file = file;
     3d9:	8b 45 0c             	mov    0xc(%ebp),%eax
     3dc:	89 43 08             	mov    %eax,0x8(%ebx)
  cmd->efile = efile;
     3df:	8b 45 10             	mov    0x10(%ebp),%eax
     3e2:	89 43 0c             	mov    %eax,0xc(%ebx)
  cmd->mode = mode;
     3e5:	8b 45 14             	mov    0x14(%ebp),%eax
     3e8:	89 43 10             	mov    %eax,0x10(%ebx)
  cmd->fd = fd;
     3eb:	8b 45 18             	mov    0x18(%ebp),%eax
     3ee:	89 43 14             	mov    %eax,0x14(%ebx)
  return (struct cmd*)cmd;
}
     3f1:	83 c4 14             	add    $0x14,%esp
     3f4:	89 d8                	mov    %ebx,%eax
     3f6:	5b                   	pop    %ebx
     3f7:	5d                   	pop    %ebp
     3f8:	c3                   	ret    
     3f9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00000400 <pipecmd>:

struct cmd*
pipecmd(struct cmd *left, struct cmd *right)
{
     400:	55                   	push   %ebp
     401:	89 e5                	mov    %esp,%ebp
     403:	53                   	push   %ebx
     404:	83 ec 14             	sub    $0x14,%esp
  struct pipecmd *cmd;

  cmd = malloc(sizeof(*cmd));
     407:	c7 04 24 0c 00 00 00 	movl   $0xc,(%esp)
     40e:	e8 9d 0d 00 00       	call   11b0 <malloc>
  memset(cmd, 0, sizeof(*cmd));
     413:	c7 44 24 08 0c 00 00 	movl   $0xc,0x8(%esp)
     41a:	00 
     41b:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
     422:	00 
     423:	89 04 24             	mov    %eax,(%esp)
  cmd = malloc(sizeof(*cmd));
     426:	89 c3                	mov    %eax,%ebx
  memset(cmd, 0, sizeof(*cmd));
     428:	e8 43 08 00 00       	call   c70 <memset>
  cmd->type = PIPE;
  cmd->left = left;
     42d:	8b 45 08             	mov    0x8(%ebp),%eax
  cmd->type = PIPE;
     430:	c7 03 03 00 00 00    	movl   $0x3,(%ebx)
  cmd->left = left;
     436:	89 43 04             	mov    %eax,0x4(%ebx)
  cmd->right = right;
     439:	8b 45 0c             	mov    0xc(%ebp),%eax
     43c:	89 43 08             	mov    %eax,0x8(%ebx)
  return (struct cmd*)cmd;
}
     43f:	83 c4 14             	add    $0x14,%esp
     442:	89 d8                	mov    %ebx,%eax
     444:	5b                   	pop    %ebx
     445:	5d                   	pop    %ebp
     446:	c3                   	ret    
     447:	89 f6                	mov    %esi,%esi
     449:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000450 <listcmd>:

struct cmd*
listcmd(struct cmd *left, struct cmd *right)
{
     450:	55                   	push   %ebp
     451:	89 e5                	mov    %esp,%ebp
     453:	53                   	push   %ebx
     454:	83 ec 14             	sub    $0x14,%esp
  struct listcmd *cmd;

  cmd = malloc(sizeof(*cmd));
     457:	c7 04 24 0c 00 00 00 	movl   $0xc,(%esp)
     45e:	e8 4d 0d 00 00       	call   11b0 <malloc>
  memset(cmd, 0, sizeof(*cmd));
     463:	c7 44 24 08 0c 00 00 	movl   $0xc,0x8(%esp)
     46a:	00 
     46b:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
     472:	00 
     473:	89 04 24             	mov    %eax,(%esp)
  cmd = malloc(sizeof(*cmd));
     476:	89 c3                	mov    %eax,%ebx
  memset(cmd, 0, sizeof(*cmd));
     478:	e8 f3 07 00 00       	call   c70 <memset>
  cmd->type = LIST;
  cmd->left = left;
     47d:	8b 45 08             	mov    0x8(%ebp),%eax
  cmd->type = LIST;
     480:	c7 03 04 00 00 00    	movl   $0x4,(%ebx)
  cmd->left = left;
     486:	89 43 04             	mov    %eax,0x4(%ebx)
  cmd->right = right;
     489:	8b 45 0c             	mov    0xc(%ebp),%eax
     48c:	89 43 08             	mov    %eax,0x8(%ebx)
  return (struct cmd*)cmd;
}
     48f:	83 c4 14             	add    $0x14,%esp
     492:	89 d8                	mov    %ebx,%eax
     494:	5b                   	pop    %ebx
     495:	5d                   	pop    %ebp
     496:	c3                   	ret    
     497:	89 f6                	mov    %esi,%esi
     499:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

000004a0 <backcmd>:

struct cmd*
backcmd(struct cmd *subcmd)
{
     4a0:	55                   	push   %ebp
     4a1:	89 e5                	mov    %esp,%ebp
     4a3:	53                   	push   %ebx
     4a4:	83 ec 14             	sub    $0x14,%esp
  struct backcmd *cmd;

  cmd = malloc(sizeof(*cmd));
     4a7:	c7 04 24 08 00 00 00 	movl   $0x8,(%esp)
     4ae:	e8 fd 0c 00 00       	call   11b0 <malloc>
  memset(cmd, 0, sizeof(*cmd));
     4b3:	c7 44 24 08 08 00 00 	movl   $0x8,0x8(%esp)
     4ba:	00 
     4bb:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
     4c2:	00 
     4c3:	89 04 24             	mov    %eax,(%esp)
  cmd = malloc(sizeof(*cmd));
     4c6:	89 c3                	mov    %eax,%ebx
  memset(cmd, 0, sizeof(*cmd));
     4c8:	e8 a3 07 00 00       	call   c70 <memset>
  cmd->type = BACK;
  cmd->cmd = subcmd;
     4cd:	8b 45 08             	mov    0x8(%ebp),%eax
  cmd->type = BACK;
     4d0:	c7 03 05 00 00 00    	movl   $0x5,(%ebx)
  cmd->cmd = subcmd;
     4d6:	89 43 04             	mov    %eax,0x4(%ebx)
  return (struct cmd*)cmd;
}
     4d9:	83 c4 14             	add    $0x14,%esp
     4dc:	89 d8                	mov    %ebx,%eax
     4de:	5b                   	pop    %ebx
     4df:	5d                   	pop    %ebp
     4e0:	c3                   	ret    
     4e1:	eb 0d                	jmp    4f0 <gettoken>
     4e3:	90                   	nop
     4e4:	90                   	nop
     4e5:	90                   	nop
     4e6:	90                   	nop
     4e7:	90                   	nop
     4e8:	90                   	nop
     4e9:	90                   	nop
     4ea:	90                   	nop
     4eb:	90                   	nop
     4ec:	90                   	nop
     4ed:	90                   	nop
     4ee:	90                   	nop
     4ef:	90                   	nop

000004f0 <gettoken>:
char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>&;()";

int
gettoken(char **ps, char *es, char **q, char **eq)
{
     4f0:	55                   	push   %ebp
     4f1:	89 e5                	mov    %esp,%ebp
     4f3:	57                   	push   %edi
     4f4:	56                   	push   %esi
     4f5:	53                   	push   %ebx
     4f6:	83 ec 1c             	sub    $0x1c,%esp
  char *s;
  int ret;

  s = *ps;
     4f9:	8b 45 08             	mov    0x8(%ebp),%eax
{
     4fc:	8b 5d 0c             	mov    0xc(%ebp),%ebx
     4ff:	8b 75 10             	mov    0x10(%ebp),%esi
  s = *ps;
     502:	8b 38                	mov    (%eax),%edi
  while(s < es && strchr(whitespace, *s))
     504:	39 df                	cmp    %ebx,%edi
     506:	72 0f                	jb     517 <gettoken+0x27>
     508:	eb 24                	jmp    52e <gettoken+0x3e>
     50a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    s++;
     510:	83 c7 01             	add    $0x1,%edi
  while(s < es && strchr(whitespace, *s))
     513:	39 df                	cmp    %ebx,%edi
     515:	74 17                	je     52e <gettoken+0x3e>
     517:	0f be 07             	movsbl (%edi),%eax
     51a:	c7 04 24 20 19 00 00 	movl   $0x1920,(%esp)
     521:	89 44 24 04          	mov    %eax,0x4(%esp)
     525:	e8 66 07 00 00       	call   c90 <strchr>
     52a:	85 c0                	test   %eax,%eax
     52c:	75 e2                	jne    510 <gettoken+0x20>
  if(q)
     52e:	85 f6                	test   %esi,%esi
     530:	74 02                	je     534 <gettoken+0x44>
    *q = s;
     532:	89 3e                	mov    %edi,(%esi)
  ret = *s;
     534:	0f b6 0f             	movzbl (%edi),%ecx
     537:	0f be f1             	movsbl %cl,%esi
  switch(*s){
     53a:	80 f9 29             	cmp    $0x29,%cl
  ret = *s;
     53d:	89 f0                	mov    %esi,%eax
  switch(*s){
     53f:	7f 4f                	jg     590 <gettoken+0xa0>
     541:	80 f9 28             	cmp    $0x28,%cl
     544:	7d 55                	jge    59b <gettoken+0xab>
     546:	84 c9                	test   %cl,%cl
     548:	0f 85 ca 00 00 00    	jne    618 <gettoken+0x128>
    ret = 'a';
    while(s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    break;
  }
  if(eq)
     54e:	8b 45 14             	mov    0x14(%ebp),%eax
     551:	85 c0                	test   %eax,%eax
     553:	74 05                	je     55a <gettoken+0x6a>
    *eq = s;
     555:	8b 45 14             	mov    0x14(%ebp),%eax
     558:	89 38                	mov    %edi,(%eax)

  while(s < es && strchr(whitespace, *s))
     55a:	39 df                	cmp    %ebx,%edi
     55c:	72 09                	jb     567 <gettoken+0x77>
     55e:	eb 1e                	jmp    57e <gettoken+0x8e>
    s++;
     560:	83 c7 01             	add    $0x1,%edi
  while(s < es && strchr(whitespace, *s))
     563:	39 df                	cmp    %ebx,%edi
     565:	74 17                	je     57e <gettoken+0x8e>
     567:	0f be 07             	movsbl (%edi),%eax
     56a:	c7 04 24 20 19 00 00 	movl   $0x1920,(%esp)
     571:	89 44 24 04          	mov    %eax,0x4(%esp)
     575:	e8 16 07 00 00       	call   c90 <strchr>
     57a:	85 c0                	test   %eax,%eax
     57c:	75 e2                	jne    560 <gettoken+0x70>
  *ps = s;
     57e:	8b 45 08             	mov    0x8(%ebp),%eax
     581:	89 38                	mov    %edi,(%eax)
  return ret;
}
     583:	83 c4 1c             	add    $0x1c,%esp
     586:	89 f0                	mov    %esi,%eax
     588:	5b                   	pop    %ebx
     589:	5e                   	pop    %esi
     58a:	5f                   	pop    %edi
     58b:	5d                   	pop    %ebp
     58c:	c3                   	ret    
     58d:	8d 76 00             	lea    0x0(%esi),%esi
  switch(*s){
     590:	80 f9 3e             	cmp    $0x3e,%cl
     593:	75 0b                	jne    5a0 <gettoken+0xb0>
    if(*s == '>'){
     595:	80 7f 01 3e          	cmpb   $0x3e,0x1(%edi)
     599:	74 6d                	je     608 <gettoken+0x118>
    s++;
     59b:	83 c7 01             	add    $0x1,%edi
     59e:	eb ae                	jmp    54e <gettoken+0x5e>
  switch(*s){
     5a0:	7f 56                	jg     5f8 <gettoken+0x108>
     5a2:	83 e9 3b             	sub    $0x3b,%ecx
     5a5:	80 f9 01             	cmp    $0x1,%cl
     5a8:	76 f1                	jbe    59b <gettoken+0xab>
    while(s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
     5aa:	39 fb                	cmp    %edi,%ebx
     5ac:	77 2b                	ja     5d9 <gettoken+0xe9>
     5ae:	66 90                	xchg   %ax,%ax
     5b0:	eb 3b                	jmp    5ed <gettoken+0xfd>
     5b2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
     5b8:	0f be 07             	movsbl (%edi),%eax
     5bb:	c7 04 24 18 19 00 00 	movl   $0x1918,(%esp)
     5c2:	89 44 24 04          	mov    %eax,0x4(%esp)
     5c6:	e8 c5 06 00 00       	call   c90 <strchr>
     5cb:	85 c0                	test   %eax,%eax
     5cd:	75 1e                	jne    5ed <gettoken+0xfd>
      s++;
     5cf:	83 c7 01             	add    $0x1,%edi
    while(s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
     5d2:	39 df                	cmp    %ebx,%edi
     5d4:	74 17                	je     5ed <gettoken+0xfd>
     5d6:	0f be 07             	movsbl (%edi),%eax
     5d9:	89 44 24 04          	mov    %eax,0x4(%esp)
     5dd:	c7 04 24 20 19 00 00 	movl   $0x1920,(%esp)
     5e4:	e8 a7 06 00 00       	call   c90 <strchr>
     5e9:	85 c0                	test   %eax,%eax
     5eb:	74 cb                	je     5b8 <gettoken+0xc8>
    ret = 'a';
     5ed:	be 61 00 00 00       	mov    $0x61,%esi
     5f2:	e9 57 ff ff ff       	jmp    54e <gettoken+0x5e>
     5f7:	90                   	nop
  switch(*s){
     5f8:	80 f9 7c             	cmp    $0x7c,%cl
     5fb:	74 9e                	je     59b <gettoken+0xab>
     5fd:	8d 76 00             	lea    0x0(%esi),%esi
     600:	eb a8                	jmp    5aa <gettoken+0xba>
     602:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
      s++;
     608:	83 c7 02             	add    $0x2,%edi
      ret = '+';
     60b:	be 2b 00 00 00       	mov    $0x2b,%esi
     610:	e9 39 ff ff ff       	jmp    54e <gettoken+0x5e>
     615:	8d 76 00             	lea    0x0(%esi),%esi
  switch(*s){
     618:	80 f9 26             	cmp    $0x26,%cl
     61b:	75 8d                	jne    5aa <gettoken+0xba>
     61d:	e9 79 ff ff ff       	jmp    59b <gettoken+0xab>
     622:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
     629:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000630 <peek>:

int
peek(char **ps, char *es, char *toks)
{
     630:	55                   	push   %ebp
     631:	89 e5                	mov    %esp,%ebp
     633:	57                   	push   %edi
     634:	56                   	push   %esi
     635:	53                   	push   %ebx
     636:	83 ec 1c             	sub    $0x1c,%esp
     639:	8b 7d 08             	mov    0x8(%ebp),%edi
     63c:	8b 75 0c             	mov    0xc(%ebp),%esi
  char *s;

  s = *ps;
     63f:	8b 1f                	mov    (%edi),%ebx
  while(s < es && strchr(whitespace, *s))
     641:	39 f3                	cmp    %esi,%ebx
     643:	72 0a                	jb     64f <peek+0x1f>
     645:	eb 1f                	jmp    666 <peek+0x36>
     647:	90                   	nop
    s++;
     648:	83 c3 01             	add    $0x1,%ebx
  while(s < es && strchr(whitespace, *s))
     64b:	39 f3                	cmp    %esi,%ebx
     64d:	74 17                	je     666 <peek+0x36>
     64f:	0f be 03             	movsbl (%ebx),%eax
     652:	c7 04 24 20 19 00 00 	movl   $0x1920,(%esp)
     659:	89 44 24 04          	mov    %eax,0x4(%esp)
     65d:	e8 2e 06 00 00       	call   c90 <strchr>
     662:	85 c0                	test   %eax,%eax
     664:	75 e2                	jne    648 <peek+0x18>
  *ps = s;
     666:	89 1f                	mov    %ebx,(%edi)
  return *s && strchr(toks, *s);
     668:	0f be 13             	movsbl (%ebx),%edx
     66b:	31 c0                	xor    %eax,%eax
     66d:	84 d2                	test   %dl,%dl
     66f:	74 17                	je     688 <peek+0x58>
     671:	8b 45 10             	mov    0x10(%ebp),%eax
     674:	89 54 24 04          	mov    %edx,0x4(%esp)
     678:	89 04 24             	mov    %eax,(%esp)
     67b:	e8 10 06 00 00       	call   c90 <strchr>
     680:	85 c0                	test   %eax,%eax
     682:	0f 95 c0             	setne  %al
     685:	0f b6 c0             	movzbl %al,%eax
}
     688:	83 c4 1c             	add    $0x1c,%esp
     68b:	5b                   	pop    %ebx
     68c:	5e                   	pop    %esi
     68d:	5f                   	pop    %edi
     68e:	5d                   	pop    %ebp
     68f:	c3                   	ret    

00000690 <parseredirs>:
  return cmd;
}

struct cmd*
parseredirs(struct cmd *cmd, char **ps, char *es)
{
     690:	55                   	push   %ebp
     691:	89 e5                	mov    %esp,%ebp
     693:	57                   	push   %edi
     694:	56                   	push   %esi
     695:	53                   	push   %ebx
     696:	83 ec 3c             	sub    $0x3c,%esp
     699:	8b 75 0c             	mov    0xc(%ebp),%esi
     69c:	8b 5d 10             	mov    0x10(%ebp),%ebx
     69f:	90                   	nop
  int tok;
  char *q, *eq;

  while(peek(ps, es, "<>")){
     6a0:	c7 44 24 08 e9 12 00 	movl   $0x12e9,0x8(%esp)
     6a7:	00 
     6a8:	89 5c 24 04          	mov    %ebx,0x4(%esp)
     6ac:	89 34 24             	mov    %esi,(%esp)
     6af:	e8 7c ff ff ff       	call   630 <peek>
     6b4:	85 c0                	test   %eax,%eax
     6b6:	0f 84 9c 00 00 00    	je     758 <parseredirs+0xc8>
    tok = gettoken(ps, es, 0, 0);
     6bc:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
     6c3:	00 
     6c4:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
     6cb:	00 
     6cc:	89 5c 24 04          	mov    %ebx,0x4(%esp)
     6d0:	89 34 24             	mov    %esi,(%esp)
     6d3:	e8 18 fe ff ff       	call   4f0 <gettoken>
    if(gettoken(ps, es, &q, &eq) != 'a')
     6d8:	89 5c 24 04          	mov    %ebx,0x4(%esp)
     6dc:	89 34 24             	mov    %esi,(%esp)
    tok = gettoken(ps, es, 0, 0);
     6df:	89 c7                	mov    %eax,%edi
    if(gettoken(ps, es, &q, &eq) != 'a')
     6e1:	8d 45 e4             	lea    -0x1c(%ebp),%eax
     6e4:	89 44 24 0c          	mov    %eax,0xc(%esp)
     6e8:	8d 45 e0             	lea    -0x20(%ebp),%eax
     6eb:	89 44 24 08          	mov    %eax,0x8(%esp)
     6ef:	e8 fc fd ff ff       	call   4f0 <gettoken>
     6f4:	83 f8 61             	cmp    $0x61,%eax
     6f7:	75 6a                	jne    763 <parseredirs+0xd3>
      panic("missing file for redirection");
    switch(tok){
     6f9:	83 ff 3c             	cmp    $0x3c,%edi
     6fc:	74 42                	je     740 <parseredirs+0xb0>
     6fe:	83 ff 3e             	cmp    $0x3e,%edi
     701:	74 05                	je     708 <parseredirs+0x78>
     703:	83 ff 2b             	cmp    $0x2b,%edi
     706:	75 98                	jne    6a0 <parseredirs+0x10>
      break;
    case '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY|O_CREATE, 1);
      break;
    case '+':  // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY|O_CREATE, 1);
     708:	c7 44 24 10 01 00 00 	movl   $0x1,0x10(%esp)
     70f:	00 
     710:	c7 44 24 0c 01 02 00 	movl   $0x201,0xc(%esp)
     717:	00 
     718:	8b 45 e4             	mov    -0x1c(%ebp),%eax
     71b:	89 44 24 08          	mov    %eax,0x8(%esp)
     71f:	8b 45 e0             	mov    -0x20(%ebp),%eax
     722:	89 44 24 04          	mov    %eax,0x4(%esp)
     726:	8b 45 08             	mov    0x8(%ebp),%eax
     729:	89 04 24             	mov    %eax,(%esp)
     72c:	e8 6f fc ff ff       	call   3a0 <redircmd>
     731:	89 45 08             	mov    %eax,0x8(%ebp)
      break;
     734:	e9 67 ff ff ff       	jmp    6a0 <parseredirs+0x10>
     739:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
     740:	c7 44 24 10 00 00 00 	movl   $0x0,0x10(%esp)
     747:	00 
     748:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
     74f:	00 
     750:	eb c6                	jmp    718 <parseredirs+0x88>
     752:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    }
  }
  return cmd;
}
     758:	8b 45 08             	mov    0x8(%ebp),%eax
     75b:	83 c4 3c             	add    $0x3c,%esp
     75e:	5b                   	pop    %ebx
     75f:	5e                   	pop    %esi
     760:	5f                   	pop    %edi
     761:	5d                   	pop    %ebp
     762:	c3                   	ret    
      panic("missing file for redirection");
     763:	c7 04 24 cc 12 00 00 	movl   $0x12cc,(%esp)
     76a:	e8 f1 f9 ff ff       	call   160 <panic>
     76f:	90                   	nop

00000770 <parseexec>:
  return cmd;
}

struct cmd*
parseexec(char **ps, char *es)
{
     770:	55                   	push   %ebp
     771:	89 e5                	mov    %esp,%ebp
     773:	57                   	push   %edi
     774:	56                   	push   %esi
     775:	53                   	push   %ebx
     776:	83 ec 3c             	sub    $0x3c,%esp
     779:	8b 75 08             	mov    0x8(%ebp),%esi
     77c:	8b 7d 0c             	mov    0xc(%ebp),%edi
  char *q, *eq;
  int tok, argc;
  struct execcmd *cmd;
  struct cmd *ret;

  if(peek(ps, es, "("))
     77f:	c7 44 24 08 ec 12 00 	movl   $0x12ec,0x8(%esp)
     786:	00 
     787:	89 34 24             	mov    %esi,(%esp)
     78a:	89 7c 24 04          	mov    %edi,0x4(%esp)
     78e:	e8 9d fe ff ff       	call   630 <peek>
     793:	85 c0                	test   %eax,%eax
     795:	0f 85 a5 00 00 00    	jne    840 <parseexec+0xd0>
    return parseblock(ps, es);

  ret = execcmd();
     79b:	e8 c0 fb ff ff       	call   360 <execcmd>
  cmd = (struct execcmd*)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
     7a0:	89 7c 24 08          	mov    %edi,0x8(%esp)
     7a4:	89 74 24 04          	mov    %esi,0x4(%esp)
     7a8:	89 04 24             	mov    %eax,(%esp)
  ret = execcmd();
     7ab:	89 c3                	mov    %eax,%ebx
     7ad:	89 45 cc             	mov    %eax,-0x34(%ebp)
  ret = parseredirs(ret, ps, es);
     7b0:	e8 db fe ff ff       	call   690 <parseredirs>
  argc = 0;
     7b5:	c7 45 d4 00 00 00 00 	movl   $0x0,-0x2c(%ebp)
  ret = parseredirs(ret, ps, es);
     7bc:	89 45 d0             	mov    %eax,-0x30(%ebp)
  while(!peek(ps, es, "|)&;")){
     7bf:	eb 1d                	jmp    7de <parseexec+0x6e>
     7c1:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    if(argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
     7c8:	8b 45 d0             	mov    -0x30(%ebp),%eax
     7cb:	89 7c 24 08          	mov    %edi,0x8(%esp)
     7cf:	89 74 24 04          	mov    %esi,0x4(%esp)
     7d3:	89 04 24             	mov    %eax,(%esp)
     7d6:	e8 b5 fe ff ff       	call   690 <parseredirs>
     7db:	89 45 d0             	mov    %eax,-0x30(%ebp)
  while(!peek(ps, es, "|)&;")){
     7de:	c7 44 24 08 03 13 00 	movl   $0x1303,0x8(%esp)
     7e5:	00 
     7e6:	89 7c 24 04          	mov    %edi,0x4(%esp)
     7ea:	89 34 24             	mov    %esi,(%esp)
     7ed:	e8 3e fe ff ff       	call   630 <peek>
     7f2:	85 c0                	test   %eax,%eax
     7f4:	75 62                	jne    858 <parseexec+0xe8>
    if((tok=gettoken(ps, es, &q, &eq)) == 0)
     7f6:	8d 45 e4             	lea    -0x1c(%ebp),%eax
     7f9:	89 44 24 0c          	mov    %eax,0xc(%esp)
     7fd:	8d 45 e0             	lea    -0x20(%ebp),%eax
     800:	89 44 24 08          	mov    %eax,0x8(%esp)
     804:	89 7c 24 04          	mov    %edi,0x4(%esp)
     808:	89 34 24             	mov    %esi,(%esp)
     80b:	e8 e0 fc ff ff       	call   4f0 <gettoken>
     810:	85 c0                	test   %eax,%eax
     812:	74 44                	je     858 <parseexec+0xe8>
    if(tok != 'a')
     814:	83 f8 61             	cmp    $0x61,%eax
     817:	75 61                	jne    87a <parseexec+0x10a>
    cmd->argv[argc] = q;
     819:	8b 45 e0             	mov    -0x20(%ebp),%eax
     81c:	83 c3 04             	add    $0x4,%ebx
    argc++;
     81f:	83 45 d4 01          	addl   $0x1,-0x2c(%ebp)
    cmd->argv[argc] = q;
     823:	89 03                	mov    %eax,(%ebx)
    cmd->eargv[argc] = eq;
     825:	8b 45 e4             	mov    -0x1c(%ebp),%eax
     828:	89 43 28             	mov    %eax,0x28(%ebx)
    if(argc >= MAXARGS)
     82b:	83 7d d4 0a          	cmpl   $0xa,-0x2c(%ebp)
     82f:	75 97                	jne    7c8 <parseexec+0x58>
      panic("too many args");
     831:	c7 04 24 f5 12 00 00 	movl   $0x12f5,(%esp)
     838:	e8 23 f9 ff ff       	call   160 <panic>
     83d:	8d 76 00             	lea    0x0(%esi),%esi
    return parseblock(ps, es);
     840:	89 7c 24 04          	mov    %edi,0x4(%esp)
     844:	89 34 24             	mov    %esi,(%esp)
     847:	e8 84 01 00 00       	call   9d0 <parseblock>
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  return ret;
}
     84c:	83 c4 3c             	add    $0x3c,%esp
     84f:	5b                   	pop    %ebx
     850:	5e                   	pop    %esi
     851:	5f                   	pop    %edi
     852:	5d                   	pop    %ebp
     853:	c3                   	ret    
     854:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
     858:	8b 45 cc             	mov    -0x34(%ebp),%eax
     85b:	8b 55 d4             	mov    -0x2c(%ebp),%edx
     85e:	8d 04 90             	lea    (%eax,%edx,4),%eax
  cmd->argv[argc] = 0;
     861:	c7 40 04 00 00 00 00 	movl   $0x0,0x4(%eax)
  cmd->eargv[argc] = 0;
     868:	c7 40 2c 00 00 00 00 	movl   $0x0,0x2c(%eax)
  return ret;
     86f:	8b 45 d0             	mov    -0x30(%ebp),%eax
}
     872:	83 c4 3c             	add    $0x3c,%esp
     875:	5b                   	pop    %ebx
     876:	5e                   	pop    %esi
     877:	5f                   	pop    %edi
     878:	5d                   	pop    %ebp
     879:	c3                   	ret    
      panic("syntax");
     87a:	c7 04 24 ee 12 00 00 	movl   $0x12ee,(%esp)
     881:	e8 da f8 ff ff       	call   160 <panic>
     886:	8d 76 00             	lea    0x0(%esi),%esi
     889:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000890 <parsepipe>:
{
     890:	55                   	push   %ebp
     891:	89 e5                	mov    %esp,%ebp
     893:	57                   	push   %edi
     894:	56                   	push   %esi
     895:	53                   	push   %ebx
     896:	83 ec 1c             	sub    $0x1c,%esp
     899:	8b 5d 08             	mov    0x8(%ebp),%ebx
     89c:	8b 75 0c             	mov    0xc(%ebp),%esi
  cmd = parseexec(ps, es);
     89f:	89 1c 24             	mov    %ebx,(%esp)
     8a2:	89 74 24 04          	mov    %esi,0x4(%esp)
     8a6:	e8 c5 fe ff ff       	call   770 <parseexec>
  if(peek(ps, es, "|")){
     8ab:	c7 44 24 08 08 13 00 	movl   $0x1308,0x8(%esp)
     8b2:	00 
     8b3:	89 74 24 04          	mov    %esi,0x4(%esp)
     8b7:	89 1c 24             	mov    %ebx,(%esp)
  cmd = parseexec(ps, es);
     8ba:	89 c7                	mov    %eax,%edi
  if(peek(ps, es, "|")){
     8bc:	e8 6f fd ff ff       	call   630 <peek>
     8c1:	85 c0                	test   %eax,%eax
     8c3:	75 0b                	jne    8d0 <parsepipe+0x40>
}
     8c5:	83 c4 1c             	add    $0x1c,%esp
     8c8:	89 f8                	mov    %edi,%eax
     8ca:	5b                   	pop    %ebx
     8cb:	5e                   	pop    %esi
     8cc:	5f                   	pop    %edi
     8cd:	5d                   	pop    %ebp
     8ce:	c3                   	ret    
     8cf:	90                   	nop
    gettoken(ps, es, 0, 0);
     8d0:	89 74 24 04          	mov    %esi,0x4(%esp)
     8d4:	89 1c 24             	mov    %ebx,(%esp)
     8d7:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
     8de:	00 
     8df:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
     8e6:	00 
     8e7:	e8 04 fc ff ff       	call   4f0 <gettoken>
    cmd = pipecmd(cmd, parsepipe(ps, es));
     8ec:	89 74 24 04          	mov    %esi,0x4(%esp)
     8f0:	89 1c 24             	mov    %ebx,(%esp)
     8f3:	e8 98 ff ff ff       	call   890 <parsepipe>
     8f8:	89 7d 08             	mov    %edi,0x8(%ebp)
     8fb:	89 45 0c             	mov    %eax,0xc(%ebp)
}
     8fe:	83 c4 1c             	add    $0x1c,%esp
     901:	5b                   	pop    %ebx
     902:	5e                   	pop    %esi
     903:	5f                   	pop    %edi
     904:	5d                   	pop    %ebp
    cmd = pipecmd(cmd, parsepipe(ps, es));
     905:	e9 f6 fa ff ff       	jmp    400 <pipecmd>
     90a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

00000910 <parseline>:
{
     910:	55                   	push   %ebp
     911:	89 e5                	mov    %esp,%ebp
     913:	57                   	push   %edi
     914:	56                   	push   %esi
     915:	53                   	push   %ebx
     916:	83 ec 1c             	sub    $0x1c,%esp
     919:	8b 5d 08             	mov    0x8(%ebp),%ebx
     91c:	8b 75 0c             	mov    0xc(%ebp),%esi
  cmd = parsepipe(ps, es);
     91f:	89 1c 24             	mov    %ebx,(%esp)
     922:	89 74 24 04          	mov    %esi,0x4(%esp)
     926:	e8 65 ff ff ff       	call   890 <parsepipe>
     92b:	89 c7                	mov    %eax,%edi
  while(peek(ps, es, "&")){
     92d:	eb 27                	jmp    956 <parseline+0x46>
     92f:	90                   	nop
    gettoken(ps, es, 0, 0);
     930:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
     937:	00 
     938:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
     93f:	00 
     940:	89 74 24 04          	mov    %esi,0x4(%esp)
     944:	89 1c 24             	mov    %ebx,(%esp)
     947:	e8 a4 fb ff ff       	call   4f0 <gettoken>
    cmd = backcmd(cmd);
     94c:	89 3c 24             	mov    %edi,(%esp)
     94f:	e8 4c fb ff ff       	call   4a0 <backcmd>
     954:	89 c7                	mov    %eax,%edi
  while(peek(ps, es, "&")){
     956:	c7 44 24 08 0a 13 00 	movl   $0x130a,0x8(%esp)
     95d:	00 
     95e:	89 74 24 04          	mov    %esi,0x4(%esp)
     962:	89 1c 24             	mov    %ebx,(%esp)
     965:	e8 c6 fc ff ff       	call   630 <peek>
     96a:	85 c0                	test   %eax,%eax
     96c:	75 c2                	jne    930 <parseline+0x20>
  if(peek(ps, es, ";")){
     96e:	c7 44 24 08 06 13 00 	movl   $0x1306,0x8(%esp)
     975:	00 
     976:	89 74 24 04          	mov    %esi,0x4(%esp)
     97a:	89 1c 24             	mov    %ebx,(%esp)
     97d:	e8 ae fc ff ff       	call   630 <peek>
     982:	85 c0                	test   %eax,%eax
     984:	75 0a                	jne    990 <parseline+0x80>
}
     986:	83 c4 1c             	add    $0x1c,%esp
     989:	89 f8                	mov    %edi,%eax
     98b:	5b                   	pop    %ebx
     98c:	5e                   	pop    %esi
     98d:	5f                   	pop    %edi
     98e:	5d                   	pop    %ebp
     98f:	c3                   	ret    
    gettoken(ps, es, 0, 0);
     990:	89 74 24 04          	mov    %esi,0x4(%esp)
     994:	89 1c 24             	mov    %ebx,(%esp)
     997:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
     99e:	00 
     99f:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
     9a6:	00 
     9a7:	e8 44 fb ff ff       	call   4f0 <gettoken>
    cmd = listcmd(cmd, parseline(ps, es));
     9ac:	89 74 24 04          	mov    %esi,0x4(%esp)
     9b0:	89 1c 24             	mov    %ebx,(%esp)
     9b3:	e8 58 ff ff ff       	call   910 <parseline>
     9b8:	89 7d 08             	mov    %edi,0x8(%ebp)
     9bb:	89 45 0c             	mov    %eax,0xc(%ebp)
}
     9be:	83 c4 1c             	add    $0x1c,%esp
     9c1:	5b                   	pop    %ebx
     9c2:	5e                   	pop    %esi
     9c3:	5f                   	pop    %edi
     9c4:	5d                   	pop    %ebp
    cmd = listcmd(cmd, parseline(ps, es));
     9c5:	e9 86 fa ff ff       	jmp    450 <listcmd>
     9ca:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

000009d0 <parseblock>:
{
     9d0:	55                   	push   %ebp
     9d1:	89 e5                	mov    %esp,%ebp
     9d3:	57                   	push   %edi
     9d4:	56                   	push   %esi
     9d5:	53                   	push   %ebx
     9d6:	83 ec 1c             	sub    $0x1c,%esp
     9d9:	8b 5d 08             	mov    0x8(%ebp),%ebx
     9dc:	8b 75 0c             	mov    0xc(%ebp),%esi
  if(!peek(ps, es, "("))
     9df:	c7 44 24 08 ec 12 00 	movl   $0x12ec,0x8(%esp)
     9e6:	00 
     9e7:	89 1c 24             	mov    %ebx,(%esp)
     9ea:	89 74 24 04          	mov    %esi,0x4(%esp)
     9ee:	e8 3d fc ff ff       	call   630 <peek>
     9f3:	85 c0                	test   %eax,%eax
     9f5:	74 76                	je     a6d <parseblock+0x9d>
  gettoken(ps, es, 0, 0);
     9f7:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
     9fe:	00 
     9ff:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
     a06:	00 
     a07:	89 74 24 04          	mov    %esi,0x4(%esp)
     a0b:	89 1c 24             	mov    %ebx,(%esp)
     a0e:	e8 dd fa ff ff       	call   4f0 <gettoken>
  cmd = parseline(ps, es);
     a13:	89 74 24 04          	mov    %esi,0x4(%esp)
     a17:	89 1c 24             	mov    %ebx,(%esp)
     a1a:	e8 f1 fe ff ff       	call   910 <parseline>
  if(!peek(ps, es, ")"))
     a1f:	c7 44 24 08 28 13 00 	movl   $0x1328,0x8(%esp)
     a26:	00 
     a27:	89 74 24 04          	mov    %esi,0x4(%esp)
     a2b:	89 1c 24             	mov    %ebx,(%esp)
  cmd = parseline(ps, es);
     a2e:	89 c7                	mov    %eax,%edi
  if(!peek(ps, es, ")"))
     a30:	e8 fb fb ff ff       	call   630 <peek>
     a35:	85 c0                	test   %eax,%eax
     a37:	74 40                	je     a79 <parseblock+0xa9>
  gettoken(ps, es, 0, 0);
     a39:	89 74 24 04          	mov    %esi,0x4(%esp)
     a3d:	89 1c 24             	mov    %ebx,(%esp)
     a40:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
     a47:	00 
     a48:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
     a4f:	00 
     a50:	e8 9b fa ff ff       	call   4f0 <gettoken>
  cmd = parseredirs(cmd, ps, es);
     a55:	89 74 24 08          	mov    %esi,0x8(%esp)
     a59:	89 5c 24 04          	mov    %ebx,0x4(%esp)
     a5d:	89 3c 24             	mov    %edi,(%esp)
     a60:	e8 2b fc ff ff       	call   690 <parseredirs>
}
     a65:	83 c4 1c             	add    $0x1c,%esp
     a68:	5b                   	pop    %ebx
     a69:	5e                   	pop    %esi
     a6a:	5f                   	pop    %edi
     a6b:	5d                   	pop    %ebp
     a6c:	c3                   	ret    
    panic("parseblock");
     a6d:	c7 04 24 0c 13 00 00 	movl   $0x130c,(%esp)
     a74:	e8 e7 f6 ff ff       	call   160 <panic>
    panic("syntax - missing )");
     a79:	c7 04 24 17 13 00 00 	movl   $0x1317,(%esp)
     a80:	e8 db f6 ff ff       	call   160 <panic>
     a85:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
     a89:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000a90 <nulterminate>:

// NUL-terminate all the counted strings.
struct cmd*
nulterminate(struct cmd *cmd)
{
     a90:	55                   	push   %ebp
     a91:	89 e5                	mov    %esp,%ebp
     a93:	53                   	push   %ebx
     a94:	83 ec 14             	sub    $0x14,%esp
     a97:	8b 5d 08             	mov    0x8(%ebp),%ebx
  struct execcmd *ecmd;
  struct listcmd *lcmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if(cmd == 0)
     a9a:	85 db                	test   %ebx,%ebx
     a9c:	0f 84 8e 00 00 00    	je     b30 <nulterminate+0xa0>
    return 0;

  switch(cmd->type){
     aa2:	83 3b 05             	cmpl   $0x5,(%ebx)
     aa5:	77 49                	ja     af0 <nulterminate+0x60>
     aa7:	8b 03                	mov    (%ebx),%eax
     aa9:	ff 24 85 68 13 00 00 	jmp    *0x1368(,%eax,4)
    nulterminate(pcmd->right);
    break;

  case LIST:
    lcmd = (struct listcmd*)cmd;
    nulterminate(lcmd->left);
     ab0:	8b 43 04             	mov    0x4(%ebx),%eax
     ab3:	89 04 24             	mov    %eax,(%esp)
     ab6:	e8 d5 ff ff ff       	call   a90 <nulterminate>
    nulterminate(lcmd->right);
     abb:	8b 43 08             	mov    0x8(%ebx),%eax
     abe:	89 04 24             	mov    %eax,(%esp)
     ac1:	e8 ca ff ff ff       	call   a90 <nulterminate>
    break;
     ac6:	89 d8                	mov    %ebx,%eax
    bcmd = (struct backcmd*)cmd;
    nulterminate(bcmd->cmd);
    break;
  }
  return cmd;
}
     ac8:	83 c4 14             	add    $0x14,%esp
     acb:	5b                   	pop    %ebx
     acc:	5d                   	pop    %ebp
     acd:	c3                   	ret    
     ace:	66 90                	xchg   %ax,%ax
    for(i=0; ecmd->argv[i]; i++)
     ad0:	8b 4b 04             	mov    0x4(%ebx),%ecx
     ad3:	89 d8                	mov    %ebx,%eax
     ad5:	85 c9                	test   %ecx,%ecx
     ad7:	74 17                	je     af0 <nulterminate+0x60>
     ad9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
      *ecmd->eargv[i] = 0;
     ae0:	8b 50 2c             	mov    0x2c(%eax),%edx
     ae3:	83 c0 04             	add    $0x4,%eax
     ae6:	c6 02 00             	movb   $0x0,(%edx)
    for(i=0; ecmd->argv[i]; i++)
     ae9:	8b 50 04             	mov    0x4(%eax),%edx
     aec:	85 d2                	test   %edx,%edx
     aee:	75 f0                	jne    ae0 <nulterminate+0x50>
}
     af0:	83 c4 14             	add    $0x14,%esp
  switch(cmd->type){
     af3:	89 d8                	mov    %ebx,%eax
}
     af5:	5b                   	pop    %ebx
     af6:	5d                   	pop    %ebp
     af7:	c3                   	ret    
    nulterminate(bcmd->cmd);
     af8:	8b 43 04             	mov    0x4(%ebx),%eax
     afb:	89 04 24             	mov    %eax,(%esp)
     afe:	e8 8d ff ff ff       	call   a90 <nulterminate>
}
     b03:	83 c4 14             	add    $0x14,%esp
    break;
     b06:	89 d8                	mov    %ebx,%eax
}
     b08:	5b                   	pop    %ebx
     b09:	5d                   	pop    %ebp
     b0a:	c3                   	ret    
     b0b:	90                   	nop
     b0c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    nulterminate(rcmd->cmd);
     b10:	8b 43 04             	mov    0x4(%ebx),%eax
     b13:	89 04 24             	mov    %eax,(%esp)
     b16:	e8 75 ff ff ff       	call   a90 <nulterminate>
    *rcmd->efile = 0;
     b1b:	8b 43 0c             	mov    0xc(%ebx),%eax
     b1e:	c6 00 00             	movb   $0x0,(%eax)
}
     b21:	83 c4 14             	add    $0x14,%esp
    break;
     b24:	89 d8                	mov    %ebx,%eax
}
     b26:	5b                   	pop    %ebx
     b27:	5d                   	pop    %ebp
     b28:	c3                   	ret    
     b29:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
    return 0;
     b30:	31 c0                	xor    %eax,%eax
     b32:	eb 94                	jmp    ac8 <nulterminate+0x38>
     b34:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
     b3a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

00000b40 <parsecmd>:
{
     b40:	55                   	push   %ebp
     b41:	89 e5                	mov    %esp,%ebp
     b43:	56                   	push   %esi
     b44:	53                   	push   %ebx
     b45:	83 ec 10             	sub    $0x10,%esp
  es = s + strlen(s);
     b48:	8b 5d 08             	mov    0x8(%ebp),%ebx
     b4b:	89 1c 24             	mov    %ebx,(%esp)
     b4e:	e8 ed 00 00 00       	call   c40 <strlen>
     b53:	01 c3                	add    %eax,%ebx
  cmd = parseline(&s, es);
     b55:	8d 45 08             	lea    0x8(%ebp),%eax
     b58:	89 5c 24 04          	mov    %ebx,0x4(%esp)
     b5c:	89 04 24             	mov    %eax,(%esp)
     b5f:	e8 ac fd ff ff       	call   910 <parseline>
  peek(&s, es, "");
     b64:	c7 44 24 08 b6 12 00 	movl   $0x12b6,0x8(%esp)
     b6b:	00 
     b6c:	89 5c 24 04          	mov    %ebx,0x4(%esp)
  cmd = parseline(&s, es);
     b70:	89 c6                	mov    %eax,%esi
  peek(&s, es, "");
     b72:	8d 45 08             	lea    0x8(%ebp),%eax
     b75:	89 04 24             	mov    %eax,(%esp)
     b78:	e8 b3 fa ff ff       	call   630 <peek>
  if(s != es){
     b7d:	8b 45 08             	mov    0x8(%ebp),%eax
     b80:	39 d8                	cmp    %ebx,%eax
     b82:	75 11                	jne    b95 <parsecmd+0x55>
  nulterminate(cmd);
     b84:	89 34 24             	mov    %esi,(%esp)
     b87:	e8 04 ff ff ff       	call   a90 <nulterminate>
}
     b8c:	83 c4 10             	add    $0x10,%esp
     b8f:	89 f0                	mov    %esi,%eax
     b91:	5b                   	pop    %ebx
     b92:	5e                   	pop    %esi
     b93:	5d                   	pop    %ebp
     b94:	c3                   	ret    
    printf(2, "leftovers: %s\n", s);
     b95:	89 44 24 08          	mov    %eax,0x8(%esp)
     b99:	c7 44 24 04 2a 13 00 	movl   $0x132a,0x4(%esp)
     ba0:	00 
     ba1:	c7 04 24 02 00 00 00 	movl   $0x2,(%esp)
     ba8:	e8 83 03 00 00       	call   f30 <printf>
    panic("syntax");
     bad:	c7 04 24 ee 12 00 00 	movl   $0x12ee,(%esp)
     bb4:	e8 a7 f5 ff ff       	call   160 <panic>
     bb9:	66 90                	xchg   %ax,%ax
     bbb:	66 90                	xchg   %ax,%ax
     bbd:	66 90                	xchg   %ax,%ax
     bbf:	90                   	nop

00000bc0 <strcpy>:
#include "user.h"
#include "x86.h"

char*
strcpy(char *s, const char *t)
{
     bc0:	55                   	push   %ebp
     bc1:	89 e5                	mov    %esp,%ebp
     bc3:	8b 45 08             	mov    0x8(%ebp),%eax
     bc6:	8b 4d 0c             	mov    0xc(%ebp),%ecx
     bc9:	53                   	push   %ebx
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
     bca:	89 c2                	mov    %eax,%edx
     bcc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
     bd0:	83 c1 01             	add    $0x1,%ecx
     bd3:	0f b6 59 ff          	movzbl -0x1(%ecx),%ebx
     bd7:	83 c2 01             	add    $0x1,%edx
     bda:	84 db                	test   %bl,%bl
     bdc:	88 5a ff             	mov    %bl,-0x1(%edx)
     bdf:	75 ef                	jne    bd0 <strcpy+0x10>
    ;
  return os;
}
     be1:	5b                   	pop    %ebx
     be2:	5d                   	pop    %ebp
     be3:	c3                   	ret    
     be4:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
     bea:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

00000bf0 <strcmp>:

int
strcmp(const char *p, const char *q)
{
     bf0:	55                   	push   %ebp
     bf1:	89 e5                	mov    %esp,%ebp
     bf3:	8b 55 08             	mov    0x8(%ebp),%edx
     bf6:	53                   	push   %ebx
     bf7:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  while(*p && *p == *q)
     bfa:	0f b6 02             	movzbl (%edx),%eax
     bfd:	84 c0                	test   %al,%al
     bff:	74 2d                	je     c2e <strcmp+0x3e>
     c01:	0f b6 19             	movzbl (%ecx),%ebx
     c04:	38 d8                	cmp    %bl,%al
     c06:	74 0e                	je     c16 <strcmp+0x26>
     c08:	eb 2b                	jmp    c35 <strcmp+0x45>
     c0a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
     c10:	38 c8                	cmp    %cl,%al
     c12:	75 15                	jne    c29 <strcmp+0x39>
    p++, q++;
     c14:	89 d9                	mov    %ebx,%ecx
     c16:	83 c2 01             	add    $0x1,%edx
  while(*p && *p == *q)
     c19:	0f b6 02             	movzbl (%edx),%eax
    p++, q++;
     c1c:	8d 59 01             	lea    0x1(%ecx),%ebx
  while(*p && *p == *q)
     c1f:	0f b6 49 01          	movzbl 0x1(%ecx),%ecx
     c23:	84 c0                	test   %al,%al
     c25:	75 e9                	jne    c10 <strcmp+0x20>
     c27:	31 c0                	xor    %eax,%eax
  return (uchar)*p - (uchar)*q;
     c29:	29 c8                	sub    %ecx,%eax
}
     c2b:	5b                   	pop    %ebx
     c2c:	5d                   	pop    %ebp
     c2d:	c3                   	ret    
     c2e:	0f b6 09             	movzbl (%ecx),%ecx
  while(*p && *p == *q)
     c31:	31 c0                	xor    %eax,%eax
     c33:	eb f4                	jmp    c29 <strcmp+0x39>
     c35:	0f b6 cb             	movzbl %bl,%ecx
     c38:	eb ef                	jmp    c29 <strcmp+0x39>
     c3a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

00000c40 <strlen>:

uint
strlen(const char *s)
{
     c40:	55                   	push   %ebp
     c41:	89 e5                	mov    %esp,%ebp
     c43:	8b 4d 08             	mov    0x8(%ebp),%ecx
  int n;

  for(n = 0; s[n]; n++)
     c46:	80 39 00             	cmpb   $0x0,(%ecx)
     c49:	74 12                	je     c5d <strlen+0x1d>
     c4b:	31 d2                	xor    %edx,%edx
     c4d:	8d 76 00             	lea    0x0(%esi),%esi
     c50:	83 c2 01             	add    $0x1,%edx
     c53:	80 3c 11 00          	cmpb   $0x0,(%ecx,%edx,1)
     c57:	89 d0                	mov    %edx,%eax
     c59:	75 f5                	jne    c50 <strlen+0x10>
    ;
  return n;
}
     c5b:	5d                   	pop    %ebp
     c5c:	c3                   	ret    
  for(n = 0; s[n]; n++)
     c5d:	31 c0                	xor    %eax,%eax
}
     c5f:	5d                   	pop    %ebp
     c60:	c3                   	ret    
     c61:	eb 0d                	jmp    c70 <memset>
     c63:	90                   	nop
     c64:	90                   	nop
     c65:	90                   	nop
     c66:	90                   	nop
     c67:	90                   	nop
     c68:	90                   	nop
     c69:	90                   	nop
     c6a:	90                   	nop
     c6b:	90                   	nop
     c6c:	90                   	nop
     c6d:	90                   	nop
     c6e:	90                   	nop
     c6f:	90                   	nop

00000c70 <memset>:

void*
memset(void *dst, int c, uint n)
{
     c70:	55                   	push   %ebp
     c71:	89 e5                	mov    %esp,%ebp
     c73:	8b 55 08             	mov    0x8(%ebp),%edx
     c76:	57                   	push   %edi
}

static inline void
stosb(void *addr, int data, int cnt)
{
  asm volatile("cld; rep stosb" :
     c77:	8b 4d 10             	mov    0x10(%ebp),%ecx
     c7a:	8b 45 0c             	mov    0xc(%ebp),%eax
     c7d:	89 d7                	mov    %edx,%edi
     c7f:	fc                   	cld    
     c80:	f3 aa                	rep stos %al,%es:(%edi)
  stosb(dst, c, n);
  return dst;
}
     c82:	89 d0                	mov    %edx,%eax
     c84:	5f                   	pop    %edi
     c85:	5d                   	pop    %ebp
     c86:	c3                   	ret    
     c87:	89 f6                	mov    %esi,%esi
     c89:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000c90 <strchr>:

char*
strchr(const char *s, char c)
{
     c90:	55                   	push   %ebp
     c91:	89 e5                	mov    %esp,%ebp
     c93:	8b 45 08             	mov    0x8(%ebp),%eax
     c96:	53                   	push   %ebx
     c97:	8b 55 0c             	mov    0xc(%ebp),%edx
  for(; *s; s++)
     c9a:	0f b6 18             	movzbl (%eax),%ebx
     c9d:	84 db                	test   %bl,%bl
     c9f:	74 1d                	je     cbe <strchr+0x2e>
    if(*s == c)
     ca1:	38 d3                	cmp    %dl,%bl
     ca3:	89 d1                	mov    %edx,%ecx
     ca5:	75 0d                	jne    cb4 <strchr+0x24>
     ca7:	eb 17                	jmp    cc0 <strchr+0x30>
     ca9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
     cb0:	38 ca                	cmp    %cl,%dl
     cb2:	74 0c                	je     cc0 <strchr+0x30>
  for(; *s; s++)
     cb4:	83 c0 01             	add    $0x1,%eax
     cb7:	0f b6 10             	movzbl (%eax),%edx
     cba:	84 d2                	test   %dl,%dl
     cbc:	75 f2                	jne    cb0 <strchr+0x20>
      return (char*)s;
  return 0;
     cbe:	31 c0                	xor    %eax,%eax
}
     cc0:	5b                   	pop    %ebx
     cc1:	5d                   	pop    %ebp
     cc2:	c3                   	ret    
     cc3:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
     cc9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000cd0 <gets>:

char*
gets(char *buf, int max)
{
     cd0:	55                   	push   %ebp
     cd1:	89 e5                	mov    %esp,%ebp
     cd3:	57                   	push   %edi
     cd4:	56                   	push   %esi
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
     cd5:	31 f6                	xor    %esi,%esi
{
     cd7:	53                   	push   %ebx
     cd8:	83 ec 2c             	sub    $0x2c,%esp
    cc = read(0, &c, 1);
     cdb:	8d 7d e7             	lea    -0x19(%ebp),%edi
  for(i=0; i+1 < max; ){
     cde:	eb 31                	jmp    d11 <gets+0x41>
    cc = read(0, &c, 1);
     ce0:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
     ce7:	00 
     ce8:	89 7c 24 04          	mov    %edi,0x4(%esp)
     cec:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     cf3:	e8 02 01 00 00       	call   dfa <read>
    if(cc < 1)
     cf8:	85 c0                	test   %eax,%eax
     cfa:	7e 1d                	jle    d19 <gets+0x49>
      break;
    buf[i++] = c;
     cfc:	0f b6 45 e7          	movzbl -0x19(%ebp),%eax
  for(i=0; i+1 < max; ){
     d00:	89 de                	mov    %ebx,%esi
    buf[i++] = c;
     d02:	8b 55 08             	mov    0x8(%ebp),%edx
    if(c == '\n' || c == '\r')
     d05:	3c 0d                	cmp    $0xd,%al
    buf[i++] = c;
     d07:	88 44 1a ff          	mov    %al,-0x1(%edx,%ebx,1)
    if(c == '\n' || c == '\r')
     d0b:	74 0c                	je     d19 <gets+0x49>
     d0d:	3c 0a                	cmp    $0xa,%al
     d0f:	74 08                	je     d19 <gets+0x49>
  for(i=0; i+1 < max; ){
     d11:	8d 5e 01             	lea    0x1(%esi),%ebx
     d14:	3b 5d 0c             	cmp    0xc(%ebp),%ebx
     d17:	7c c7                	jl     ce0 <gets+0x10>
      break;
  }
  buf[i] = '\0';
     d19:	8b 45 08             	mov    0x8(%ebp),%eax
     d1c:	c6 04 30 00          	movb   $0x0,(%eax,%esi,1)
  return buf;
}
     d20:	83 c4 2c             	add    $0x2c,%esp
     d23:	5b                   	pop    %ebx
     d24:	5e                   	pop    %esi
     d25:	5f                   	pop    %edi
     d26:	5d                   	pop    %ebp
     d27:	c3                   	ret    
     d28:	90                   	nop
     d29:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00000d30 <stat>:

int
stat(const char *n, struct stat *st)
{
     d30:	55                   	push   %ebp
     d31:	89 e5                	mov    %esp,%ebp
     d33:	56                   	push   %esi
     d34:	53                   	push   %ebx
     d35:	83 ec 10             	sub    $0x10,%esp
  int fd;
  int r;

  fd = open(n, O_RDONLY);
     d38:	8b 45 08             	mov    0x8(%ebp),%eax
     d3b:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
     d42:	00 
     d43:	89 04 24             	mov    %eax,(%esp)
     d46:	e8 d7 00 00 00       	call   e22 <open>
  if(fd < 0)
     d4b:	85 c0                	test   %eax,%eax
  fd = open(n, O_RDONLY);
     d4d:	89 c3                	mov    %eax,%ebx
  if(fd < 0)
     d4f:	78 27                	js     d78 <stat+0x48>
    return -1;
  r = fstat(fd, st);
     d51:	8b 45 0c             	mov    0xc(%ebp),%eax
     d54:	89 1c 24             	mov    %ebx,(%esp)
     d57:	89 44 24 04          	mov    %eax,0x4(%esp)
     d5b:	e8 da 00 00 00       	call   e3a <fstat>
  close(fd);
     d60:	89 1c 24             	mov    %ebx,(%esp)
  r = fstat(fd, st);
     d63:	89 c6                	mov    %eax,%esi
  close(fd);
     d65:	e8 a0 00 00 00       	call   e0a <close>
  return r;
     d6a:	89 f0                	mov    %esi,%eax
}
     d6c:	83 c4 10             	add    $0x10,%esp
     d6f:	5b                   	pop    %ebx
     d70:	5e                   	pop    %esi
     d71:	5d                   	pop    %ebp
     d72:	c3                   	ret    
     d73:	90                   	nop
     d74:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    return -1;
     d78:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
     d7d:	eb ed                	jmp    d6c <stat+0x3c>
     d7f:	90                   	nop

00000d80 <atoi>:

int
atoi(const char *s)
{
     d80:	55                   	push   %ebp
     d81:	89 e5                	mov    %esp,%ebp
     d83:	8b 4d 08             	mov    0x8(%ebp),%ecx
     d86:	53                   	push   %ebx
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
     d87:	0f be 11             	movsbl (%ecx),%edx
     d8a:	8d 42 d0             	lea    -0x30(%edx),%eax
     d8d:	3c 09                	cmp    $0x9,%al
  n = 0;
     d8f:	b8 00 00 00 00       	mov    $0x0,%eax
  while('0' <= *s && *s <= '9')
     d94:	77 17                	ja     dad <atoi+0x2d>
     d96:	66 90                	xchg   %ax,%ax
    n = n*10 + *s++ - '0';
     d98:	83 c1 01             	add    $0x1,%ecx
     d9b:	8d 04 80             	lea    (%eax,%eax,4),%eax
     d9e:	8d 44 42 d0          	lea    -0x30(%edx,%eax,2),%eax
  while('0' <= *s && *s <= '9')
     da2:	0f be 11             	movsbl (%ecx),%edx
     da5:	8d 5a d0             	lea    -0x30(%edx),%ebx
     da8:	80 fb 09             	cmp    $0x9,%bl
     dab:	76 eb                	jbe    d98 <atoi+0x18>
  return n;
}
     dad:	5b                   	pop    %ebx
     dae:	5d                   	pop    %ebp
     daf:	c3                   	ret    

00000db0 <memmove>:

void*
memmove(void *vdst, const void *vsrc, int n)
{
     db0:	55                   	push   %ebp
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
     db1:	31 d2                	xor    %edx,%edx
{
     db3:	89 e5                	mov    %esp,%ebp
     db5:	56                   	push   %esi
     db6:	8b 45 08             	mov    0x8(%ebp),%eax
     db9:	53                   	push   %ebx
     dba:	8b 5d 10             	mov    0x10(%ebp),%ebx
     dbd:	8b 75 0c             	mov    0xc(%ebp),%esi
  while(n-- > 0)
     dc0:	85 db                	test   %ebx,%ebx
     dc2:	7e 12                	jle    dd6 <memmove+0x26>
     dc4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    *dst++ = *src++;
     dc8:	0f b6 0c 16          	movzbl (%esi,%edx,1),%ecx
     dcc:	88 0c 10             	mov    %cl,(%eax,%edx,1)
     dcf:	83 c2 01             	add    $0x1,%edx
  while(n-- > 0)
     dd2:	39 da                	cmp    %ebx,%edx
     dd4:	75 f2                	jne    dc8 <memmove+0x18>
  return vdst;
}
     dd6:	5b                   	pop    %ebx
     dd7:	5e                   	pop    %esi
     dd8:	5d                   	pop    %ebp
     dd9:	c3                   	ret    

00000dda <fork>:
  name: \
    movl $SYS_ ## name, %eax; \
    int $T_SYSCALL; \
    ret

SYSCALL(fork)
     dda:	b8 01 00 00 00       	mov    $0x1,%eax
     ddf:	cd 40                	int    $0x40
     de1:	c3                   	ret    

00000de2 <exit>:
SYSCALL(exit)
     de2:	b8 02 00 00 00       	mov    $0x2,%eax
     de7:	cd 40                	int    $0x40
     de9:	c3                   	ret    

00000dea <wait>:
SYSCALL(wait)
     dea:	b8 03 00 00 00       	mov    $0x3,%eax
     def:	cd 40                	int    $0x40
     df1:	c3                   	ret    

00000df2 <pipe>:
SYSCALL(pipe)
     df2:	b8 04 00 00 00       	mov    $0x4,%eax
     df7:	cd 40                	int    $0x40
     df9:	c3                   	ret    

00000dfa <read>:
SYSCALL(read)
     dfa:	b8 05 00 00 00       	mov    $0x5,%eax
     dff:	cd 40                	int    $0x40
     e01:	c3                   	ret    

00000e02 <write>:
SYSCALL(write)
     e02:	b8 10 00 00 00       	mov    $0x10,%eax
     e07:	cd 40                	int    $0x40
     e09:	c3                   	ret    

00000e0a <close>:
SYSCALL(close)
     e0a:	b8 15 00 00 00       	mov    $0x15,%eax
     e0f:	cd 40                	int    $0x40
     e11:	c3                   	ret    

00000e12 <kill>:
SYSCALL(kill)
     e12:	b8 06 00 00 00       	mov    $0x6,%eax
     e17:	cd 40                	int    $0x40
     e19:	c3                   	ret    

00000e1a <exec>:
SYSCALL(exec)
     e1a:	b8 07 00 00 00       	mov    $0x7,%eax
     e1f:	cd 40                	int    $0x40
     e21:	c3                   	ret    

00000e22 <open>:
SYSCALL(open)
     e22:	b8 0f 00 00 00       	mov    $0xf,%eax
     e27:	cd 40                	int    $0x40
     e29:	c3                   	ret    

00000e2a <mknod>:
SYSCALL(mknod)
     e2a:	b8 11 00 00 00       	mov    $0x11,%eax
     e2f:	cd 40                	int    $0x40
     e31:	c3                   	ret    

00000e32 <unlink>:
SYSCALL(unlink)
     e32:	b8 12 00 00 00       	mov    $0x12,%eax
     e37:	cd 40                	int    $0x40
     e39:	c3                   	ret    

00000e3a <fstat>:
SYSCALL(fstat)
     e3a:	b8 08 00 00 00       	mov    $0x8,%eax
     e3f:	cd 40                	int    $0x40
     e41:	c3                   	ret    

00000e42 <link>:
SYSCALL(link)
     e42:	b8 13 00 00 00       	mov    $0x13,%eax
     e47:	cd 40                	int    $0x40
     e49:	c3                   	ret    

00000e4a <mkdir>:
SYSCALL(mkdir)
     e4a:	b8 14 00 00 00       	mov    $0x14,%eax
     e4f:	cd 40                	int    $0x40
     e51:	c3                   	ret    

00000e52 <chdir>:
SYSCALL(chdir)
     e52:	b8 09 00 00 00       	mov    $0x9,%eax
     e57:	cd 40                	int    $0x40
     e59:	c3                   	ret    

00000e5a <dup>:
SYSCALL(dup)
     e5a:	b8 0a 00 00 00       	mov    $0xa,%eax
     e5f:	cd 40                	int    $0x40
     e61:	c3                   	ret    

00000e62 <getpid>:
SYSCALL(getpid)
     e62:	b8 0b 00 00 00       	mov    $0xb,%eax
     e67:	cd 40                	int    $0x40
     e69:	c3                   	ret    

00000e6a <sbrk>:
SYSCALL(sbrk)
     e6a:	b8 0c 00 00 00       	mov    $0xc,%eax
     e6f:	cd 40                	int    $0x40
     e71:	c3                   	ret    

00000e72 <sleep>:
SYSCALL(sleep)
     e72:	b8 0d 00 00 00       	mov    $0xd,%eax
     e77:	cd 40                	int    $0x40
     e79:	c3                   	ret    

00000e7a <uptime>:
SYSCALL(uptime)
     e7a:	b8 0e 00 00 00       	mov    $0xe,%eax
     e7f:	cd 40                	int    $0x40
     e81:	c3                   	ret    
     e82:	66 90                	xchg   %ax,%ax
     e84:	66 90                	xchg   %ax,%ax
     e86:	66 90                	xchg   %ax,%ax
     e88:	66 90                	xchg   %ax,%ax
     e8a:	66 90                	xchg   %ax,%ax
     e8c:	66 90                	xchg   %ax,%ax
     e8e:	66 90                	xchg   %ax,%ax

00000e90 <printint>:
  write(fd, &c, 1);
}

static void
printint(int fd, int xx, int base, int sgn)
{
     e90:	55                   	push   %ebp
     e91:	89 e5                	mov    %esp,%ebp
     e93:	57                   	push   %edi
     e94:	56                   	push   %esi
     e95:	89 c6                	mov    %eax,%esi
     e97:	53                   	push   %ebx
     e98:	83 ec 4c             	sub    $0x4c,%esp
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
     e9b:	8b 5d 08             	mov    0x8(%ebp),%ebx
     e9e:	85 db                	test   %ebx,%ebx
     ea0:	74 09                	je     eab <printint+0x1b>
     ea2:	89 d0                	mov    %edx,%eax
     ea4:	c1 e8 1f             	shr    $0x1f,%eax
     ea7:	84 c0                	test   %al,%al
     ea9:	75 75                	jne    f20 <printint+0x90>
    neg = 1;
    x = -xx;
  } else {
    x = xx;
     eab:	89 d0                	mov    %edx,%eax
  neg = 0;
     ead:	c7 45 c4 00 00 00 00 	movl   $0x0,-0x3c(%ebp)
     eb4:	89 75 c0             	mov    %esi,-0x40(%ebp)
  }

  i = 0;
     eb7:	31 ff                	xor    %edi,%edi
     eb9:	89 ce                	mov    %ecx,%esi
     ebb:	8d 5d d7             	lea    -0x29(%ebp),%ebx
     ebe:	eb 02                	jmp    ec2 <printint+0x32>
  do{
    buf[i++] = digits[x % base];
     ec0:	89 cf                	mov    %ecx,%edi
     ec2:	31 d2                	xor    %edx,%edx
     ec4:	f7 f6                	div    %esi
     ec6:	8d 4f 01             	lea    0x1(%edi),%ecx
     ec9:	0f b6 92 87 13 00 00 	movzbl 0x1387(%edx),%edx
  }while((x /= base) != 0);
     ed0:	85 c0                	test   %eax,%eax
    buf[i++] = digits[x % base];
     ed2:	88 14 0b             	mov    %dl,(%ebx,%ecx,1)
  }while((x /= base) != 0);
     ed5:	75 e9                	jne    ec0 <printint+0x30>
  if(neg)
     ed7:	8b 55 c4             	mov    -0x3c(%ebp),%edx
    buf[i++] = digits[x % base];
     eda:	89 c8                	mov    %ecx,%eax
     edc:	8b 75 c0             	mov    -0x40(%ebp),%esi
  if(neg)
     edf:	85 d2                	test   %edx,%edx
     ee1:	74 08                	je     eeb <printint+0x5b>
    buf[i++] = '-';
     ee3:	8d 4f 02             	lea    0x2(%edi),%ecx
     ee6:	c6 44 05 d8 2d       	movb   $0x2d,-0x28(%ebp,%eax,1)

  while(--i >= 0)
     eeb:	8d 79 ff             	lea    -0x1(%ecx),%edi
     eee:	66 90                	xchg   %ax,%ax
     ef0:	0f b6 44 3d d8       	movzbl -0x28(%ebp,%edi,1),%eax
     ef5:	83 ef 01             	sub    $0x1,%edi
  write(fd, &c, 1);
     ef8:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
     eff:	00 
     f00:	89 5c 24 04          	mov    %ebx,0x4(%esp)
     f04:	89 34 24             	mov    %esi,(%esp)
     f07:	88 45 d7             	mov    %al,-0x29(%ebp)
     f0a:	e8 f3 fe ff ff       	call   e02 <write>
  while(--i >= 0)
     f0f:	83 ff ff             	cmp    $0xffffffff,%edi
     f12:	75 dc                	jne    ef0 <printint+0x60>
    putc(fd, buf[i]);
}
     f14:	83 c4 4c             	add    $0x4c,%esp
     f17:	5b                   	pop    %ebx
     f18:	5e                   	pop    %esi
     f19:	5f                   	pop    %edi
     f1a:	5d                   	pop    %ebp
     f1b:	c3                   	ret    
     f1c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    x = -xx;
     f20:	89 d0                	mov    %edx,%eax
     f22:	f7 d8                	neg    %eax
    neg = 1;
     f24:	c7 45 c4 01 00 00 00 	movl   $0x1,-0x3c(%ebp)
     f2b:	eb 87                	jmp    eb4 <printint+0x24>
     f2d:	8d 76 00             	lea    0x0(%esi),%esi

00000f30 <printf>:

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, const char *fmt, ...)
{
     f30:	55                   	push   %ebp
     f31:	89 e5                	mov    %esp,%ebp
     f33:	57                   	push   %edi
  char *s;
  int c, i, state;
  uint *ap;

  state = 0;
     f34:	31 ff                	xor    %edi,%edi
{
     f36:	56                   	push   %esi
     f37:	53                   	push   %ebx
     f38:	83 ec 3c             	sub    $0x3c,%esp
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
     f3b:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  ap = (uint*)(void*)&fmt + 1;
     f3e:	8d 45 10             	lea    0x10(%ebp),%eax
{
     f41:	8b 75 08             	mov    0x8(%ebp),%esi
  ap = (uint*)(void*)&fmt + 1;
     f44:	89 45 d4             	mov    %eax,-0x2c(%ebp)
  for(i = 0; fmt[i]; i++){
     f47:	0f b6 13             	movzbl (%ebx),%edx
     f4a:	83 c3 01             	add    $0x1,%ebx
     f4d:	84 d2                	test   %dl,%dl
     f4f:	75 39                	jne    f8a <printf+0x5a>
     f51:	e9 c2 00 00 00       	jmp    1018 <printf+0xe8>
     f56:	66 90                	xchg   %ax,%ax
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
     f58:	83 fa 25             	cmp    $0x25,%edx
     f5b:	0f 84 bf 00 00 00    	je     1020 <printf+0xf0>
  write(fd, &c, 1);
     f61:	8d 45 e2             	lea    -0x1e(%ebp),%eax
     f64:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
     f6b:	00 
     f6c:	89 44 24 04          	mov    %eax,0x4(%esp)
     f70:	89 34 24             	mov    %esi,(%esp)
        state = '%';
      } else {
        putc(fd, c);
     f73:	88 55 e2             	mov    %dl,-0x1e(%ebp)
  write(fd, &c, 1);
     f76:	e8 87 fe ff ff       	call   e02 <write>
     f7b:	83 c3 01             	add    $0x1,%ebx
  for(i = 0; fmt[i]; i++){
     f7e:	0f b6 53 ff          	movzbl -0x1(%ebx),%edx
     f82:	84 d2                	test   %dl,%dl
     f84:	0f 84 8e 00 00 00    	je     1018 <printf+0xe8>
    if(state == 0){
     f8a:	85 ff                	test   %edi,%edi
    c = fmt[i] & 0xff;
     f8c:	0f be c2             	movsbl %dl,%eax
    if(state == 0){
     f8f:	74 c7                	je     f58 <printf+0x28>
      }
    } else if(state == '%'){
     f91:	83 ff 25             	cmp    $0x25,%edi
     f94:	75 e5                	jne    f7b <printf+0x4b>
      if(c == 'd'){
     f96:	83 fa 64             	cmp    $0x64,%edx
     f99:	0f 84 31 01 00 00    	je     10d0 <printf+0x1a0>
        printint(fd, *ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
     f9f:	25 f7 00 00 00       	and    $0xf7,%eax
     fa4:	83 f8 70             	cmp    $0x70,%eax
     fa7:	0f 84 83 00 00 00    	je     1030 <printf+0x100>
        printint(fd, *ap, 16, 0);
        ap++;
      } else if(c == 's'){
     fad:	83 fa 73             	cmp    $0x73,%edx
     fb0:	0f 84 a2 00 00 00    	je     1058 <printf+0x128>
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
        }
      } else if(c == 'c'){
     fb6:	83 fa 63             	cmp    $0x63,%edx
     fb9:	0f 84 35 01 00 00    	je     10f4 <printf+0x1c4>
        putc(fd, *ap);
        ap++;
      } else if(c == '%'){
     fbf:	83 fa 25             	cmp    $0x25,%edx
     fc2:	0f 84 e0 00 00 00    	je     10a8 <printf+0x178>
  write(fd, &c, 1);
     fc8:	8d 45 e6             	lea    -0x1a(%ebp),%eax
     fcb:	83 c3 01             	add    $0x1,%ebx
     fce:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
     fd5:	00 
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
     fd6:	31 ff                	xor    %edi,%edi
  write(fd, &c, 1);
     fd8:	89 44 24 04          	mov    %eax,0x4(%esp)
     fdc:	89 34 24             	mov    %esi,(%esp)
     fdf:	89 55 d0             	mov    %edx,-0x30(%ebp)
     fe2:	c6 45 e6 25          	movb   $0x25,-0x1a(%ebp)
     fe6:	e8 17 fe ff ff       	call   e02 <write>
        putc(fd, c);
     feb:	8b 55 d0             	mov    -0x30(%ebp),%edx
  write(fd, &c, 1);
     fee:	8d 45 e7             	lea    -0x19(%ebp),%eax
     ff1:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
     ff8:	00 
     ff9:	89 44 24 04          	mov    %eax,0x4(%esp)
     ffd:	89 34 24             	mov    %esi,(%esp)
        putc(fd, c);
    1000:	88 55 e7             	mov    %dl,-0x19(%ebp)
  write(fd, &c, 1);
    1003:	e8 fa fd ff ff       	call   e02 <write>
  for(i = 0; fmt[i]; i++){
    1008:	0f b6 53 ff          	movzbl -0x1(%ebx),%edx
    100c:	84 d2                	test   %dl,%dl
    100e:	0f 85 76 ff ff ff    	jne    f8a <printf+0x5a>
    1014:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    }
  }
}
    1018:	83 c4 3c             	add    $0x3c,%esp
    101b:	5b                   	pop    %ebx
    101c:	5e                   	pop    %esi
    101d:	5f                   	pop    %edi
    101e:	5d                   	pop    %ebp
    101f:	c3                   	ret    
        state = '%';
    1020:	bf 25 00 00 00       	mov    $0x25,%edi
    1025:	e9 51 ff ff ff       	jmp    f7b <printf+0x4b>
    102a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        printint(fd, *ap, 16, 0);
    1030:	8b 45 d4             	mov    -0x2c(%ebp),%eax
    1033:	b9 10 00 00 00       	mov    $0x10,%ecx
      state = 0;
    1038:	31 ff                	xor    %edi,%edi
        printint(fd, *ap, 16, 0);
    103a:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1041:	8b 10                	mov    (%eax),%edx
    1043:	89 f0                	mov    %esi,%eax
    1045:	e8 46 fe ff ff       	call   e90 <printint>
        ap++;
    104a:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
    104e:	e9 28 ff ff ff       	jmp    f7b <printf+0x4b>
    1053:	90                   	nop
    1054:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
        s = (char*)*ap;
    1058:	8b 45 d4             	mov    -0x2c(%ebp),%eax
        ap++;
    105b:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
        s = (char*)*ap;
    105f:	8b 38                	mov    (%eax),%edi
          s = "(null)";
    1061:	b8 80 13 00 00       	mov    $0x1380,%eax
    1066:	85 ff                	test   %edi,%edi
    1068:	0f 44 f8             	cmove  %eax,%edi
        while(*s != 0){
    106b:	0f b6 07             	movzbl (%edi),%eax
    106e:	84 c0                	test   %al,%al
    1070:	74 2a                	je     109c <printf+0x16c>
    1072:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    1078:	88 45 e3             	mov    %al,-0x1d(%ebp)
  write(fd, &c, 1);
    107b:	8d 45 e3             	lea    -0x1d(%ebp),%eax
          s++;
    107e:	83 c7 01             	add    $0x1,%edi
  write(fd, &c, 1);
    1081:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
    1088:	00 
    1089:	89 44 24 04          	mov    %eax,0x4(%esp)
    108d:	89 34 24             	mov    %esi,(%esp)
    1090:	e8 6d fd ff ff       	call   e02 <write>
        while(*s != 0){
    1095:	0f b6 07             	movzbl (%edi),%eax
    1098:	84 c0                	test   %al,%al
    109a:	75 dc                	jne    1078 <printf+0x148>
      state = 0;
    109c:	31 ff                	xor    %edi,%edi
    109e:	e9 d8 fe ff ff       	jmp    f7b <printf+0x4b>
    10a3:	90                   	nop
    10a4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  write(fd, &c, 1);
    10a8:	8d 45 e5             	lea    -0x1b(%ebp),%eax
      state = 0;
    10ab:	31 ff                	xor    %edi,%edi
  write(fd, &c, 1);
    10ad:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
    10b4:	00 
    10b5:	89 44 24 04          	mov    %eax,0x4(%esp)
    10b9:	89 34 24             	mov    %esi,(%esp)
    10bc:	c6 45 e5 25          	movb   $0x25,-0x1b(%ebp)
    10c0:	e8 3d fd ff ff       	call   e02 <write>
    10c5:	e9 b1 fe ff ff       	jmp    f7b <printf+0x4b>
    10ca:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        printint(fd, *ap, 10, 1);
    10d0:	8b 45 d4             	mov    -0x2c(%ebp),%eax
    10d3:	b9 0a 00 00 00       	mov    $0xa,%ecx
      state = 0;
    10d8:	66 31 ff             	xor    %di,%di
        printint(fd, *ap, 10, 1);
    10db:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    10e2:	8b 10                	mov    (%eax),%edx
    10e4:	89 f0                	mov    %esi,%eax
    10e6:	e8 a5 fd ff ff       	call   e90 <printint>
        ap++;
    10eb:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
    10ef:	e9 87 fe ff ff       	jmp    f7b <printf+0x4b>
        putc(fd, *ap);
    10f4:	8b 45 d4             	mov    -0x2c(%ebp),%eax
      state = 0;
    10f7:	31 ff                	xor    %edi,%edi
        putc(fd, *ap);
    10f9:	8b 00                	mov    (%eax),%eax
  write(fd, &c, 1);
    10fb:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
    1102:	00 
    1103:	89 34 24             	mov    %esi,(%esp)
        putc(fd, *ap);
    1106:	88 45 e4             	mov    %al,-0x1c(%ebp)
  write(fd, &c, 1);
    1109:	8d 45 e4             	lea    -0x1c(%ebp),%eax
    110c:	89 44 24 04          	mov    %eax,0x4(%esp)
    1110:	e8 ed fc ff ff       	call   e02 <write>
        ap++;
    1115:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
    1119:	e9 5d fe ff ff       	jmp    f7b <printf+0x4b>
    111e:	66 90                	xchg   %ax,%ax

00001120 <free>:
static Header base;
static Header *freep;

void
free(void *ap)
{
    1120:	55                   	push   %ebp
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    1121:	a1 a4 19 00 00       	mov    0x19a4,%eax
{
    1126:	89 e5                	mov    %esp,%ebp
    1128:	57                   	push   %edi
    1129:	56                   	push   %esi
    112a:	53                   	push   %ebx
    112b:	8b 5d 08             	mov    0x8(%ebp),%ebx
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
    112e:	8b 08                	mov    (%eax),%ecx
  bp = (Header*)ap - 1;
    1130:	8d 53 f8             	lea    -0x8(%ebx),%edx
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    1133:	39 d0                	cmp    %edx,%eax
    1135:	72 11                	jb     1148 <free+0x28>
    1137:	90                   	nop
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
    1138:	39 c8                	cmp    %ecx,%eax
    113a:	72 04                	jb     1140 <free+0x20>
    113c:	39 ca                	cmp    %ecx,%edx
    113e:	72 10                	jb     1150 <free+0x30>
    1140:	89 c8                	mov    %ecx,%eax
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    1142:	39 d0                	cmp    %edx,%eax
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
    1144:	8b 08                	mov    (%eax),%ecx
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    1146:	73 f0                	jae    1138 <free+0x18>
    1148:	39 ca                	cmp    %ecx,%edx
    114a:	72 04                	jb     1150 <free+0x30>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
    114c:	39 c8                	cmp    %ecx,%eax
    114e:	72 f0                	jb     1140 <free+0x20>
      break;
  if(bp + bp->s.size == p->s.ptr){
    1150:	8b 73 fc             	mov    -0x4(%ebx),%esi
    1153:	8d 3c f2             	lea    (%edx,%esi,8),%edi
    1156:	39 cf                	cmp    %ecx,%edi
    1158:	74 1e                	je     1178 <free+0x58>
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
    115a:	89 4b f8             	mov    %ecx,-0x8(%ebx)
  if(p + p->s.size == bp){
    115d:	8b 48 04             	mov    0x4(%eax),%ecx
    1160:	8d 34 c8             	lea    (%eax,%ecx,8),%esi
    1163:	39 f2                	cmp    %esi,%edx
    1165:	74 28                	je     118f <free+0x6f>
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
    1167:	89 10                	mov    %edx,(%eax)
  freep = p;
    1169:	a3 a4 19 00 00       	mov    %eax,0x19a4
}
    116e:	5b                   	pop    %ebx
    116f:	5e                   	pop    %esi
    1170:	5f                   	pop    %edi
    1171:	5d                   	pop    %ebp
    1172:	c3                   	ret    
    1173:	90                   	nop
    1174:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    bp->s.size += p->s.ptr->s.size;
    1178:	03 71 04             	add    0x4(%ecx),%esi
    117b:	89 73 fc             	mov    %esi,-0x4(%ebx)
    bp->s.ptr = p->s.ptr->s.ptr;
    117e:	8b 08                	mov    (%eax),%ecx
    1180:	8b 09                	mov    (%ecx),%ecx
    1182:	89 4b f8             	mov    %ecx,-0x8(%ebx)
  if(p + p->s.size == bp){
    1185:	8b 48 04             	mov    0x4(%eax),%ecx
    1188:	8d 34 c8             	lea    (%eax,%ecx,8),%esi
    118b:	39 f2                	cmp    %esi,%edx
    118d:	75 d8                	jne    1167 <free+0x47>
    p->s.size += bp->s.size;
    118f:	03 4b fc             	add    -0x4(%ebx),%ecx
  freep = p;
    1192:	a3 a4 19 00 00       	mov    %eax,0x19a4
    p->s.size += bp->s.size;
    1197:	89 48 04             	mov    %ecx,0x4(%eax)
    p->s.ptr = bp->s.ptr;
    119a:	8b 53 f8             	mov    -0x8(%ebx),%edx
    119d:	89 10                	mov    %edx,(%eax)
}
    119f:	5b                   	pop    %ebx
    11a0:	5e                   	pop    %esi
    11a1:	5f                   	pop    %edi
    11a2:	5d                   	pop    %ebp
    11a3:	c3                   	ret    
    11a4:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    11aa:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

000011b0 <malloc>:
  return freep;
}

void*
malloc(uint nbytes)
{
    11b0:	55                   	push   %ebp
    11b1:	89 e5                	mov    %esp,%ebp
    11b3:	57                   	push   %edi
    11b4:	56                   	push   %esi
    11b5:	53                   	push   %ebx
    11b6:	83 ec 1c             	sub    $0x1c,%esp
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
    11b9:	8b 45 08             	mov    0x8(%ebp),%eax
  if((prevp = freep) == 0){
    11bc:	8b 1d a4 19 00 00    	mov    0x19a4,%ebx
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
    11c2:	8d 48 07             	lea    0x7(%eax),%ecx
    11c5:	c1 e9 03             	shr    $0x3,%ecx
  if((prevp = freep) == 0){
    11c8:	85 db                	test   %ebx,%ebx
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
    11ca:	8d 71 01             	lea    0x1(%ecx),%esi
  if((prevp = freep) == 0){
    11cd:	0f 84 9b 00 00 00    	je     126e <malloc+0xbe>
    11d3:	8b 13                	mov    (%ebx),%edx
    11d5:	8b 7a 04             	mov    0x4(%edx),%edi
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
    11d8:	39 fe                	cmp    %edi,%esi
    11da:	76 64                	jbe    1240 <malloc+0x90>
    11dc:	8d 04 f5 00 00 00 00 	lea    0x0(,%esi,8),%eax
  if(nu < 4096)
    11e3:	bb 00 80 00 00       	mov    $0x8000,%ebx
    11e8:	89 45 e4             	mov    %eax,-0x1c(%ebp)
    11eb:	eb 0e                	jmp    11fb <malloc+0x4b>
    11ed:	8d 76 00             	lea    0x0(%esi),%esi
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    11f0:	8b 02                	mov    (%edx),%eax
    if(p->s.size >= nunits){
    11f2:	8b 78 04             	mov    0x4(%eax),%edi
    11f5:	39 fe                	cmp    %edi,%esi
    11f7:	76 4f                	jbe    1248 <malloc+0x98>
    11f9:	89 c2                	mov    %eax,%edx
        p->s.size = nunits;
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
    11fb:	3b 15 a4 19 00 00    	cmp    0x19a4,%edx
    1201:	75 ed                	jne    11f0 <malloc+0x40>
  if(nu < 4096)
    1203:	8b 45 e4             	mov    -0x1c(%ebp),%eax
    1206:	81 fe 00 10 00 00    	cmp    $0x1000,%esi
    120c:	bf 00 10 00 00       	mov    $0x1000,%edi
    1211:	0f 43 fe             	cmovae %esi,%edi
    1214:	0f 42 c3             	cmovb  %ebx,%eax
  p = sbrk(nu * sizeof(Header));
    1217:	89 04 24             	mov    %eax,(%esp)
    121a:	e8 4b fc ff ff       	call   e6a <sbrk>
  if(p == (char*)-1)
    121f:	83 f8 ff             	cmp    $0xffffffff,%eax
    1222:	74 18                	je     123c <malloc+0x8c>
  hp->s.size = nu;
    1224:	89 78 04             	mov    %edi,0x4(%eax)
  free((void*)(hp + 1));
    1227:	83 c0 08             	add    $0x8,%eax
    122a:	89 04 24             	mov    %eax,(%esp)
    122d:	e8 ee fe ff ff       	call   1120 <free>
  return freep;
    1232:	8b 15 a4 19 00 00    	mov    0x19a4,%edx
      if((p = morecore(nunits)) == 0)
    1238:	85 d2                	test   %edx,%edx
    123a:	75 b4                	jne    11f0 <malloc+0x40>
        return 0;
    123c:	31 c0                	xor    %eax,%eax
    123e:	eb 20                	jmp    1260 <malloc+0xb0>
    if(p->s.size >= nunits){
    1240:	89 d0                	mov    %edx,%eax
    1242:	89 da                	mov    %ebx,%edx
    1244:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      if(p->s.size == nunits)
    1248:	39 fe                	cmp    %edi,%esi
    124a:	74 1c                	je     1268 <malloc+0xb8>
        p->s.size -= nunits;
    124c:	29 f7                	sub    %esi,%edi
    124e:	89 78 04             	mov    %edi,0x4(%eax)
        p += p->s.size;
    1251:	8d 04 f8             	lea    (%eax,%edi,8),%eax
        p->s.size = nunits;
    1254:	89 70 04             	mov    %esi,0x4(%eax)
      freep = prevp;
    1257:	89 15 a4 19 00 00    	mov    %edx,0x19a4
      return (void*)(p + 1);
    125d:	83 c0 08             	add    $0x8,%eax
  }
}
    1260:	83 c4 1c             	add    $0x1c,%esp
    1263:	5b                   	pop    %ebx
    1264:	5e                   	pop    %esi
    1265:	5f                   	pop    %edi
    1266:	5d                   	pop    %ebp
    1267:	c3                   	ret    
        prevp->s.ptr = p->s.ptr;
    1268:	8b 08                	mov    (%eax),%ecx
    126a:	89 0a                	mov    %ecx,(%edx)
    126c:	eb e9                	jmp    1257 <malloc+0xa7>
    base.s.ptr = freep = prevp = &base;
    126e:	c7 05 a4 19 00 00 a8 	movl   $0x19a8,0x19a4
    1275:	19 00 00 
    base.s.size = 0;
    1278:	ba a8 19 00 00       	mov    $0x19a8,%edx
    base.s.ptr = freep = prevp = &base;
    127d:	c7 05 a8 19 00 00 a8 	movl   $0x19a8,0x19a8
    1284:	19 00 00 
    base.s.size = 0;
    1287:	c7 05 ac 19 00 00 00 	movl   $0x0,0x19ac
    128e:	00 00 00 
    1291:	e9 46 ff ff ff       	jmp    11dc <malloc+0x2c>
