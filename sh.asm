
_sh:     file format elf32-i386


Disassembly of section .text:

00000000 <main>:
    return -1;
  return 0;
}

int main(void)
{
       0:	55                   	push   %ebp
       1:	89 e5                	mov    %esp,%ebp
       3:	83 e4 f0             	and    $0xfffffff0,%esp
       6:	83 ec 10             	sub    $0x10,%esp
  static char buf[100];
  int fd;

  // Ensure that three file descriptors are open.
  while ((fd = open("console", O_RDWR)) >= 0)
       9:	eb 0e                	jmp    19 <main+0x19>
       b:	90                   	nop
       c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  {
    if (fd >= 3)
      10:	83 f8 02             	cmp    $0x2,%eax
      13:	0f 8f db 00 00 00    	jg     f4 <main+0xf4>
  while ((fd = open("console", O_RDWR)) >= 0)
      19:	c7 44 24 04 02 00 00 	movl   $0x2,0x4(%esp)
      20:	00 
      21:	c7 04 24 59 13 00 00 	movl   $0x1359,(%esp)
      28:	e8 15 0e 00 00       	call   e42 <open>
      2d:	85 c0                	test   %eax,%eax
      2f:	79 df                	jns    10 <main+0x10>
      31:	eb 2a                	jmp    5d <main+0x5d>
      33:	90                   	nop
      34:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  }

  // Read and run input commands.
  while (getcmd(buf, sizeof(buf)) >= 0)
  {
    if (buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' ')
      38:	80 3d 62 19 00 00 20 	cmpb   $0x20,0x1962
      3f:	90                   	nop
      40:	74 6e                	je     b0 <main+0xb0>
      42:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
      buf[strlen(buf) - 1] = 0; // chop \n
      if (chdir(buf + 3) < 0)
        printf(2, "cannot cd %s\n", buf + 3);
      continue;
    }
    if (fork1() == 0)
      48:	e8 53 01 00 00       	call   1a0 <fork1>
      4d:	85 c0                	test   %eax,%eax
      4f:	74 3f                	je     90 <main+0x90>
      runcmd(parsecmd(buf));
    wait(0);
      51:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
      58:	e8 ad 0d 00 00       	call   e0a <wait>
  while (getcmd(buf, sizeof(buf)) >= 0)
      5d:	c7 44 24 04 64 00 00 	movl   $0x64,0x4(%esp)
      64:	00 
      65:	c7 04 24 60 19 00 00 	movl   $0x1960,(%esp)
      6c:	e8 9f 00 00 00       	call   110 <getcmd>
      71:	85 c0                	test   %eax,%eax
      73:	78 2f                	js     a4 <main+0xa4>
    if (buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' ')
      75:	80 3d 60 19 00 00 63 	cmpb   $0x63,0x1960
      7c:	75 ca                	jne    48 <main+0x48>
      7e:	80 3d 61 19 00 00 64 	cmpb   $0x64,0x1961
      85:	74 b1                	je     38 <main+0x38>
    if (fork1() == 0)
      87:	e8 14 01 00 00       	call   1a0 <fork1>
      8c:	85 c0                	test   %eax,%eax
      8e:	75 c1                	jne    51 <main+0x51>
      runcmd(parsecmd(buf));
      90:	c7 04 24 60 19 00 00 	movl   $0x1960,(%esp)
      97:	e8 c4 0a 00 00       	call   b60 <parsecmd>
      9c:	89 04 24             	mov    %eax,(%esp)
      9f:	e8 1c 01 00 00       	call   1c0 <runcmd>
    ;
  }
  exit(0);
      a4:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
      ab:	e8 52 0d 00 00       	call   e02 <exit>
      buf[strlen(buf) - 1] = 0; // chop \n
      b0:	c7 04 24 60 19 00 00 	movl   $0x1960,(%esp)
      b7:	e8 a4 0b 00 00       	call   c60 <strlen>
      if (chdir(buf + 3) < 0)
      bc:	c7 04 24 63 19 00 00 	movl   $0x1963,(%esp)
      buf[strlen(buf) - 1] = 0; // chop \n
      c3:	c6 80 5f 19 00 00 00 	movb   $0x0,0x195f(%eax)
      if (chdir(buf + 3) < 0)
      ca:	e8 a3 0d 00 00       	call   e72 <chdir>
      cf:	85 c0                	test   %eax,%eax
      d1:	79 8a                	jns    5d <main+0x5d>
        printf(2, "cannot cd %s\n", buf + 3);
      d3:	c7 44 24 08 63 19 00 	movl   $0x1963,0x8(%esp)
      da:	00 
      db:	c7 44 24 04 61 13 00 	movl   $0x1361,0x4(%esp)
      e2:	00 
      e3:	c7 04 24 02 00 00 00 	movl   $0x2,(%esp)
      ea:	e8 61 0e 00 00       	call   f50 <printf>
      ef:	e9 69 ff ff ff       	jmp    5d <main+0x5d>
      close(fd);
      f4:	89 04 24             	mov    %eax,(%esp)
      f7:	e8 2e 0d 00 00       	call   e2a <close>
      fc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      break;
     100:	e9 58 ff ff ff       	jmp    5d <main+0x5d>
     105:	66 90                	xchg   %ax,%ax
     107:	66 90                	xchg   %ax,%ax
     109:	66 90                	xchg   %ax,%ax
     10b:	66 90                	xchg   %ax,%ax
     10d:	66 90                	xchg   %ax,%ax
     10f:	90                   	nop

00000110 <getcmd>:
{
     110:	55                   	push   %ebp
     111:	89 e5                	mov    %esp,%ebp
     113:	56                   	push   %esi
     114:	53                   	push   %ebx
     115:	83 ec 10             	sub    $0x10,%esp
     118:	8b 5d 08             	mov    0x8(%ebp),%ebx
     11b:	8b 75 0c             	mov    0xc(%ebp),%esi
  printf(2, "$ ");
     11e:	c7 44 24 04 b8 12 00 	movl   $0x12b8,0x4(%esp)
     125:	00 
     126:	c7 04 24 02 00 00 00 	movl   $0x2,(%esp)
     12d:	e8 1e 0e 00 00       	call   f50 <printf>
  memset(buf, 0, nbuf);
     132:	89 74 24 08          	mov    %esi,0x8(%esp)
     136:	89 1c 24             	mov    %ebx,(%esp)
     139:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
     140:	00 
     141:	e8 4a 0b 00 00       	call   c90 <memset>
  gets(buf, nbuf);
     146:	89 74 24 04          	mov    %esi,0x4(%esp)
     14a:	89 1c 24             	mov    %ebx,(%esp)
     14d:	e8 9e 0b 00 00       	call   cf0 <gets>
  if (buf[0] == 0) // EOF
     152:	31 c0                	xor    %eax,%eax
     154:	80 3b 00             	cmpb   $0x0,(%ebx)
     157:	0f 94 c0             	sete   %al
}
     15a:	83 c4 10             	add    $0x10,%esp
     15d:	5b                   	pop    %ebx
  if (buf[0] == 0) // EOF
     15e:	f7 d8                	neg    %eax
}
     160:	5e                   	pop    %esi
     161:	5d                   	pop    %ebp
     162:	c3                   	ret    
     163:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
     169:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000170 <panic>:
}

void panic(char *s)
{
     170:	55                   	push   %ebp
     171:	89 e5                	mov    %esp,%ebp
     173:	83 ec 18             	sub    $0x18,%esp
  printf(2, "%s\n", s);
     176:	8b 45 08             	mov    0x8(%ebp),%eax
     179:	c7 44 24 04 55 13 00 	movl   $0x1355,0x4(%esp)
     180:	00 
     181:	c7 04 24 02 00 00 00 	movl   $0x2,(%esp)
     188:	89 44 24 08          	mov    %eax,0x8(%esp)
     18c:	e8 bf 0d 00 00       	call   f50 <printf>
  exit(0);
     191:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     198:	e8 65 0c 00 00       	call   e02 <exit>
     19d:	8d 76 00             	lea    0x0(%esi),%esi

000001a0 <fork1>:
}

int fork1(void)
{
     1a0:	55                   	push   %ebp
     1a1:	89 e5                	mov    %esp,%ebp
     1a3:	83 ec 18             	sub    $0x18,%esp
  int pid;

  pid = fork();
     1a6:	e8 4f 0c 00 00       	call   dfa <fork>
  if (pid == -1)
     1ab:	83 f8 ff             	cmp    $0xffffffff,%eax
     1ae:	74 02                	je     1b2 <fork1+0x12>
    panic("fork");
  return pid;
}
     1b0:	c9                   	leave  
     1b1:	c3                   	ret    
    panic("fork");
     1b2:	c7 04 24 bb 12 00 00 	movl   $0x12bb,(%esp)
     1b9:	e8 b2 ff ff ff       	call   170 <panic>
     1be:	66 90                	xchg   %ax,%ax

000001c0 <runcmd>:
{
     1c0:	55                   	push   %ebp
     1c1:	89 e5                	mov    %esp,%ebp
     1c3:	53                   	push   %ebx
     1c4:	83 ec 24             	sub    $0x24,%esp
     1c7:	8b 5d 08             	mov    0x8(%ebp),%ebx
  if (cmd == 0)
     1ca:	85 db                	test   %ebx,%ebx
     1cc:	74 6d                	je     23b <runcmd+0x7b>
  switch (cmd->type)
     1ce:	83 3b 05             	cmpl   $0x5,(%ebx)
     1d1:	0f 87 fe 00 00 00    	ja     2d5 <runcmd+0x115>
     1d7:	8b 03                	mov    (%ebx),%eax
     1d9:	ff 24 85 70 13 00 00 	jmp    *0x1370(,%eax,4)
    if (pipe(p) < 0)
     1e0:	8d 45 f0             	lea    -0x10(%ebp),%eax
     1e3:	89 04 24             	mov    %eax,(%esp)
     1e6:	e8 27 0c 00 00       	call   e12 <pipe>
     1eb:	85 c0                	test   %eax,%eax
     1ed:	0f 88 ee 00 00 00    	js     2e1 <runcmd+0x121>
    if (fork1() == 0)
     1f3:	e8 a8 ff ff ff       	call   1a0 <fork1>
     1f8:	85 c0                	test   %eax,%eax
     1fa:	0f 84 45 01 00 00    	je     345 <runcmd+0x185>
    if (fork1() == 0)
     200:	e8 9b ff ff ff       	call   1a0 <fork1>
     205:	85 c0                	test   %eax,%eax
     207:	0f 84 00 01 00 00    	je     30d <runcmd+0x14d>
    close(p[0]);
     20d:	8b 45 f0             	mov    -0x10(%ebp),%eax
     210:	89 04 24             	mov    %eax,(%esp)
     213:	e8 12 0c 00 00       	call   e2a <close>
    close(p[1]);
     218:	8b 45 f4             	mov    -0xc(%ebp),%eax
     21b:	89 04 24             	mov    %eax,(%esp)
     21e:	e8 07 0c 00 00       	call   e2a <close>
    wait(0);
     223:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     22a:	e8 db 0b 00 00       	call   e0a <wait>
    wait(0);
     22f:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     236:	e8 cf 0b 00 00       	call   e0a <wait>
      exit(0);
     23b:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     242:	e8 bb 0b 00 00       	call   e02 <exit>
    if (fork1() == 0)
     247:	e8 54 ff ff ff       	call   1a0 <fork1>
     24c:	85 c0                	test   %eax,%eax
     24e:	75 eb                	jne    23b <runcmd+0x7b>
     250:	eb 78                	jmp    2ca <runcmd+0x10a>
    if (ecmd->argv[0] == 0)
     252:	8b 43 04             	mov    0x4(%ebx),%eax
     255:	85 c0                	test   %eax,%eax
     257:	74 e2                	je     23b <runcmd+0x7b>
    exec(ecmd->argv[0], ecmd->argv);
     259:	8d 53 04             	lea    0x4(%ebx),%edx
     25c:	89 54 24 04          	mov    %edx,0x4(%esp)
     260:	89 04 24             	mov    %eax,(%esp)
     263:	e8 d2 0b 00 00       	call   e3a <exec>
    printf(2, "exec %s failed\n", ecmd->argv[0]);
     268:	8b 43 04             	mov    0x4(%ebx),%eax
     26b:	c7 44 24 04 c7 12 00 	movl   $0x12c7,0x4(%esp)
     272:	00 
     273:	c7 04 24 02 00 00 00 	movl   $0x2,(%esp)
     27a:	89 44 24 08          	mov    %eax,0x8(%esp)
     27e:	e8 cd 0c 00 00       	call   f50 <printf>
    break;
     283:	eb b6                	jmp    23b <runcmd+0x7b>
    if (fork1() == 0)
     285:	e8 16 ff ff ff       	call   1a0 <fork1>
     28a:	85 c0                	test   %eax,%eax
     28c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
     290:	74 38                	je     2ca <runcmd+0x10a>
    wait(0);
     292:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     299:	e8 6c 0b 00 00       	call   e0a <wait>
    runcmd(lcmd->right);
     29e:	8b 43 08             	mov    0x8(%ebx),%eax
     2a1:	89 04 24             	mov    %eax,(%esp)
     2a4:	e8 17 ff ff ff       	call   1c0 <runcmd>
    close(rcmd->fd);
     2a9:	8b 43 14             	mov    0x14(%ebx),%eax
     2ac:	89 04 24             	mov    %eax,(%esp)
     2af:	e8 76 0b 00 00       	call   e2a <close>
    if (open(rcmd->file, rcmd->mode) < 0)
     2b4:	8b 43 10             	mov    0x10(%ebx),%eax
     2b7:	89 44 24 04          	mov    %eax,0x4(%esp)
     2bb:	8b 43 08             	mov    0x8(%ebx),%eax
     2be:	89 04 24             	mov    %eax,(%esp)
     2c1:	e8 7c 0b 00 00       	call   e42 <open>
     2c6:	85 c0                	test   %eax,%eax
     2c8:	78 23                	js     2ed <runcmd+0x12d>
      runcmd(bcmd->cmd);
     2ca:	8b 43 04             	mov    0x4(%ebx),%eax
     2cd:	89 04 24             	mov    %eax,(%esp)
     2d0:	e8 eb fe ff ff       	call   1c0 <runcmd>
    panic("runcmd");
     2d5:	c7 04 24 c0 12 00 00 	movl   $0x12c0,(%esp)
     2dc:	e8 8f fe ff ff       	call   170 <panic>
      panic("pipe");
     2e1:	c7 04 24 e7 12 00 00 	movl   $0x12e7,(%esp)
     2e8:	e8 83 fe ff ff       	call   170 <panic>
      printf(2, "open %s failed\n", rcmd->file);
     2ed:	8b 43 08             	mov    0x8(%ebx),%eax
     2f0:	c7 44 24 04 d7 12 00 	movl   $0x12d7,0x4(%esp)
     2f7:	00 
     2f8:	c7 04 24 02 00 00 00 	movl   $0x2,(%esp)
     2ff:	89 44 24 08          	mov    %eax,0x8(%esp)
     303:	e8 48 0c 00 00       	call   f50 <printf>
     308:	e9 2e ff ff ff       	jmp    23b <runcmd+0x7b>
      close(0);
     30d:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     314:	e8 11 0b 00 00       	call   e2a <close>
      dup(p[0]);
     319:	8b 45 f0             	mov    -0x10(%ebp),%eax
     31c:	89 04 24             	mov    %eax,(%esp)
     31f:	e8 56 0b 00 00       	call   e7a <dup>
      close(p[0]);
     324:	8b 45 f0             	mov    -0x10(%ebp),%eax
     327:	89 04 24             	mov    %eax,(%esp)
     32a:	e8 fb 0a 00 00       	call   e2a <close>
      close(p[1]);
     32f:	8b 45 f4             	mov    -0xc(%ebp),%eax
     332:	89 04 24             	mov    %eax,(%esp)
     335:	e8 f0 0a 00 00       	call   e2a <close>
      runcmd(pcmd->right);
     33a:	8b 43 08             	mov    0x8(%ebx),%eax
     33d:	89 04 24             	mov    %eax,(%esp)
     340:	e8 7b fe ff ff       	call   1c0 <runcmd>
      close(1);
     345:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     34c:	e8 d9 0a 00 00       	call   e2a <close>
      dup(p[1]);
     351:	8b 45 f4             	mov    -0xc(%ebp),%eax
     354:	89 04 24             	mov    %eax,(%esp)
     357:	e8 1e 0b 00 00       	call   e7a <dup>
      close(p[0]);
     35c:	8b 45 f0             	mov    -0x10(%ebp),%eax
     35f:	89 04 24             	mov    %eax,(%esp)
     362:	e8 c3 0a 00 00       	call   e2a <close>
      close(p[1]);
     367:	8b 45 f4             	mov    -0xc(%ebp),%eax
     36a:	89 04 24             	mov    %eax,(%esp)
     36d:	e8 b8 0a 00 00       	call   e2a <close>
      runcmd(pcmd->left);
     372:	8b 43 04             	mov    0x4(%ebx),%eax
     375:	89 04 24             	mov    %eax,(%esp)
     378:	e8 43 fe ff ff       	call   1c0 <runcmd>
     37d:	8d 76 00             	lea    0x0(%esi),%esi

00000380 <execcmd>:
// PAGEBREAK!
//  Constructors

struct cmd *
execcmd(void)
{
     380:	55                   	push   %ebp
     381:	89 e5                	mov    %esp,%ebp
     383:	53                   	push   %ebx
     384:	83 ec 14             	sub    $0x14,%esp
  struct execcmd *cmd;

  cmd = malloc(sizeof(*cmd));
     387:	c7 04 24 54 00 00 00 	movl   $0x54,(%esp)
     38e:	e8 3d 0e 00 00       	call   11d0 <malloc>
  memset(cmd, 0, sizeof(*cmd));
     393:	c7 44 24 08 54 00 00 	movl   $0x54,0x8(%esp)
     39a:	00 
     39b:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
     3a2:	00 
  cmd = malloc(sizeof(*cmd));
     3a3:	89 c3                	mov    %eax,%ebx
  memset(cmd, 0, sizeof(*cmd));
     3a5:	89 04 24             	mov    %eax,(%esp)
     3a8:	e8 e3 08 00 00       	call   c90 <memset>
  cmd->type = EXEC;
  return (struct cmd *)cmd;
}
     3ad:	89 d8                	mov    %ebx,%eax
  cmd->type = EXEC;
     3af:	c7 03 01 00 00 00    	movl   $0x1,(%ebx)
}
     3b5:	83 c4 14             	add    $0x14,%esp
     3b8:	5b                   	pop    %ebx
     3b9:	5d                   	pop    %ebp
     3ba:	c3                   	ret    
     3bb:	90                   	nop
     3bc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

000003c0 <redircmd>:

struct cmd *
redircmd(struct cmd *subcmd, char *file, char *efile, int mode, int fd)
{
     3c0:	55                   	push   %ebp
     3c1:	89 e5                	mov    %esp,%ebp
     3c3:	53                   	push   %ebx
     3c4:	83 ec 14             	sub    $0x14,%esp
  struct redircmd *cmd;

  cmd = malloc(sizeof(*cmd));
     3c7:	c7 04 24 18 00 00 00 	movl   $0x18,(%esp)
     3ce:	e8 fd 0d 00 00       	call   11d0 <malloc>
  memset(cmd, 0, sizeof(*cmd));
     3d3:	c7 44 24 08 18 00 00 	movl   $0x18,0x8(%esp)
     3da:	00 
     3db:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
     3e2:	00 
     3e3:	89 04 24             	mov    %eax,(%esp)
  cmd = malloc(sizeof(*cmd));
     3e6:	89 c3                	mov    %eax,%ebx
  memset(cmd, 0, sizeof(*cmd));
     3e8:	e8 a3 08 00 00       	call   c90 <memset>
  cmd->type = REDIR;
  cmd->cmd = subcmd;
     3ed:	8b 45 08             	mov    0x8(%ebp),%eax
  cmd->type = REDIR;
     3f0:	c7 03 02 00 00 00    	movl   $0x2,(%ebx)
  cmd->cmd = subcmd;
     3f6:	89 43 04             	mov    %eax,0x4(%ebx)
  cmd->file = file;
     3f9:	8b 45 0c             	mov    0xc(%ebp),%eax
     3fc:	89 43 08             	mov    %eax,0x8(%ebx)
  cmd->efile = efile;
     3ff:	8b 45 10             	mov    0x10(%ebp),%eax
     402:	89 43 0c             	mov    %eax,0xc(%ebx)
  cmd->mode = mode;
     405:	8b 45 14             	mov    0x14(%ebp),%eax
     408:	89 43 10             	mov    %eax,0x10(%ebx)
  cmd->fd = fd;
     40b:	8b 45 18             	mov    0x18(%ebp),%eax
     40e:	89 43 14             	mov    %eax,0x14(%ebx)
  return (struct cmd *)cmd;
}
     411:	83 c4 14             	add    $0x14,%esp
     414:	89 d8                	mov    %ebx,%eax
     416:	5b                   	pop    %ebx
     417:	5d                   	pop    %ebp
     418:	c3                   	ret    
     419:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00000420 <pipecmd>:

struct cmd *
pipecmd(struct cmd *left, struct cmd *right)
{
     420:	55                   	push   %ebp
     421:	89 e5                	mov    %esp,%ebp
     423:	53                   	push   %ebx
     424:	83 ec 14             	sub    $0x14,%esp
  struct pipecmd *cmd;

  cmd = malloc(sizeof(*cmd));
     427:	c7 04 24 0c 00 00 00 	movl   $0xc,(%esp)
     42e:	e8 9d 0d 00 00       	call   11d0 <malloc>
  memset(cmd, 0, sizeof(*cmd));
     433:	c7 44 24 08 0c 00 00 	movl   $0xc,0x8(%esp)
     43a:	00 
     43b:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
     442:	00 
     443:	89 04 24             	mov    %eax,(%esp)
  cmd = malloc(sizeof(*cmd));
     446:	89 c3                	mov    %eax,%ebx
  memset(cmd, 0, sizeof(*cmd));
     448:	e8 43 08 00 00       	call   c90 <memset>
  cmd->type = PIPE;
  cmd->left = left;
     44d:	8b 45 08             	mov    0x8(%ebp),%eax
  cmd->type = PIPE;
     450:	c7 03 03 00 00 00    	movl   $0x3,(%ebx)
  cmd->left = left;
     456:	89 43 04             	mov    %eax,0x4(%ebx)
  cmd->right = right;
     459:	8b 45 0c             	mov    0xc(%ebp),%eax
     45c:	89 43 08             	mov    %eax,0x8(%ebx)
  return (struct cmd *)cmd;
}
     45f:	83 c4 14             	add    $0x14,%esp
     462:	89 d8                	mov    %ebx,%eax
     464:	5b                   	pop    %ebx
     465:	5d                   	pop    %ebp
     466:	c3                   	ret    
     467:	89 f6                	mov    %esi,%esi
     469:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000470 <listcmd>:

struct cmd *
listcmd(struct cmd *left, struct cmd *right)
{
     470:	55                   	push   %ebp
     471:	89 e5                	mov    %esp,%ebp
     473:	53                   	push   %ebx
     474:	83 ec 14             	sub    $0x14,%esp
  struct listcmd *cmd;

  cmd = malloc(sizeof(*cmd));
     477:	c7 04 24 0c 00 00 00 	movl   $0xc,(%esp)
     47e:	e8 4d 0d 00 00       	call   11d0 <malloc>
  memset(cmd, 0, sizeof(*cmd));
     483:	c7 44 24 08 0c 00 00 	movl   $0xc,0x8(%esp)
     48a:	00 
     48b:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
     492:	00 
     493:	89 04 24             	mov    %eax,(%esp)
  cmd = malloc(sizeof(*cmd));
     496:	89 c3                	mov    %eax,%ebx
  memset(cmd, 0, sizeof(*cmd));
     498:	e8 f3 07 00 00       	call   c90 <memset>
  cmd->type = LIST;
  cmd->left = left;
     49d:	8b 45 08             	mov    0x8(%ebp),%eax
  cmd->type = LIST;
     4a0:	c7 03 04 00 00 00    	movl   $0x4,(%ebx)
  cmd->left = left;
     4a6:	89 43 04             	mov    %eax,0x4(%ebx)
  cmd->right = right;
     4a9:	8b 45 0c             	mov    0xc(%ebp),%eax
     4ac:	89 43 08             	mov    %eax,0x8(%ebx)
  return (struct cmd *)cmd;
}
     4af:	83 c4 14             	add    $0x14,%esp
     4b2:	89 d8                	mov    %ebx,%eax
     4b4:	5b                   	pop    %ebx
     4b5:	5d                   	pop    %ebp
     4b6:	c3                   	ret    
     4b7:	89 f6                	mov    %esi,%esi
     4b9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

000004c0 <backcmd>:

struct cmd *
backcmd(struct cmd *subcmd)
{
     4c0:	55                   	push   %ebp
     4c1:	89 e5                	mov    %esp,%ebp
     4c3:	53                   	push   %ebx
     4c4:	83 ec 14             	sub    $0x14,%esp
  struct backcmd *cmd;

  cmd = malloc(sizeof(*cmd));
     4c7:	c7 04 24 08 00 00 00 	movl   $0x8,(%esp)
     4ce:	e8 fd 0c 00 00       	call   11d0 <malloc>
  memset(cmd, 0, sizeof(*cmd));
     4d3:	c7 44 24 08 08 00 00 	movl   $0x8,0x8(%esp)
     4da:	00 
     4db:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
     4e2:	00 
     4e3:	89 04 24             	mov    %eax,(%esp)
  cmd = malloc(sizeof(*cmd));
     4e6:	89 c3                	mov    %eax,%ebx
  memset(cmd, 0, sizeof(*cmd));
     4e8:	e8 a3 07 00 00       	call   c90 <memset>
  cmd->type = BACK;
  cmd->cmd = subcmd;
     4ed:	8b 45 08             	mov    0x8(%ebp),%eax
  cmd->type = BACK;
     4f0:	c7 03 05 00 00 00    	movl   $0x5,(%ebx)
  cmd->cmd = subcmd;
     4f6:	89 43 04             	mov    %eax,0x4(%ebx)
  return (struct cmd *)cmd;
}
     4f9:	83 c4 14             	add    $0x14,%esp
     4fc:	89 d8                	mov    %ebx,%eax
     4fe:	5b                   	pop    %ebx
     4ff:	5d                   	pop    %ebp
     500:	c3                   	ret    
     501:	eb 0d                	jmp    510 <gettoken>
     503:	90                   	nop
     504:	90                   	nop
     505:	90                   	nop
     506:	90                   	nop
     507:	90                   	nop
     508:	90                   	nop
     509:	90                   	nop
     50a:	90                   	nop
     50b:	90                   	nop
     50c:	90                   	nop
     50d:	90                   	nop
     50e:	90                   	nop
     50f:	90                   	nop

00000510 <gettoken>:

char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>&;()";

int gettoken(char **ps, char *es, char **q, char **eq)
{
     510:	55                   	push   %ebp
     511:	89 e5                	mov    %esp,%ebp
     513:	57                   	push   %edi
     514:	56                   	push   %esi
     515:	53                   	push   %ebx
     516:	83 ec 1c             	sub    $0x1c,%esp
  char *s;
  int ret;

  s = *ps;
     519:	8b 45 08             	mov    0x8(%ebp),%eax
{
     51c:	8b 5d 0c             	mov    0xc(%ebp),%ebx
     51f:	8b 75 10             	mov    0x10(%ebp),%esi
  s = *ps;
     522:	8b 38                	mov    (%eax),%edi
  while (s < es && strchr(whitespace, *s))
     524:	39 df                	cmp    %ebx,%edi
     526:	72 0f                	jb     537 <gettoken+0x27>
     528:	eb 24                	jmp    54e <gettoken+0x3e>
     52a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    s++;
     530:	83 c7 01             	add    $0x1,%edi
  while (s < es && strchr(whitespace, *s))
     533:	39 df                	cmp    %ebx,%edi
     535:	74 17                	je     54e <gettoken+0x3e>
     537:	0f be 07             	movsbl (%edi),%eax
     53a:	c7 04 24 40 19 00 00 	movl   $0x1940,(%esp)
     541:	89 44 24 04          	mov    %eax,0x4(%esp)
     545:	e8 66 07 00 00       	call   cb0 <strchr>
     54a:	85 c0                	test   %eax,%eax
     54c:	75 e2                	jne    530 <gettoken+0x20>
  if (q)
     54e:	85 f6                	test   %esi,%esi
     550:	74 02                	je     554 <gettoken+0x44>
    *q = s;
     552:	89 3e                	mov    %edi,(%esi)
  ret = *s;
     554:	0f b6 0f             	movzbl (%edi),%ecx
     557:	0f be f1             	movsbl %cl,%esi
  switch (*s)
     55a:	80 f9 29             	cmp    $0x29,%cl
  ret = *s;
     55d:	89 f0                	mov    %esi,%eax
  switch (*s)
     55f:	7f 4f                	jg     5b0 <gettoken+0xa0>
     561:	80 f9 28             	cmp    $0x28,%cl
     564:	7d 55                	jge    5bb <gettoken+0xab>
     566:	84 c9                	test   %cl,%cl
     568:	0f 85 ca 00 00 00    	jne    638 <gettoken+0x128>
    ret = 'a';
    while (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    break;
  }
  if (eq)
     56e:	8b 45 14             	mov    0x14(%ebp),%eax
     571:	85 c0                	test   %eax,%eax
     573:	74 05                	je     57a <gettoken+0x6a>
    *eq = s;
     575:	8b 45 14             	mov    0x14(%ebp),%eax
     578:	89 38                	mov    %edi,(%eax)

  while (s < es && strchr(whitespace, *s))
     57a:	39 df                	cmp    %ebx,%edi
     57c:	72 09                	jb     587 <gettoken+0x77>
     57e:	eb 1e                	jmp    59e <gettoken+0x8e>
    s++;
     580:	83 c7 01             	add    $0x1,%edi
  while (s < es && strchr(whitespace, *s))
     583:	39 df                	cmp    %ebx,%edi
     585:	74 17                	je     59e <gettoken+0x8e>
     587:	0f be 07             	movsbl (%edi),%eax
     58a:	c7 04 24 40 19 00 00 	movl   $0x1940,(%esp)
     591:	89 44 24 04          	mov    %eax,0x4(%esp)
     595:	e8 16 07 00 00       	call   cb0 <strchr>
     59a:	85 c0                	test   %eax,%eax
     59c:	75 e2                	jne    580 <gettoken+0x70>
  *ps = s;
     59e:	8b 45 08             	mov    0x8(%ebp),%eax
     5a1:	89 38                	mov    %edi,(%eax)
  return ret;
}
     5a3:	83 c4 1c             	add    $0x1c,%esp
     5a6:	89 f0                	mov    %esi,%eax
     5a8:	5b                   	pop    %ebx
     5a9:	5e                   	pop    %esi
     5aa:	5f                   	pop    %edi
     5ab:	5d                   	pop    %ebp
     5ac:	c3                   	ret    
     5ad:	8d 76 00             	lea    0x0(%esi),%esi
  switch (*s)
     5b0:	80 f9 3e             	cmp    $0x3e,%cl
     5b3:	75 0b                	jne    5c0 <gettoken+0xb0>
    if (*s == '>')
     5b5:	80 7f 01 3e          	cmpb   $0x3e,0x1(%edi)
     5b9:	74 6d                	je     628 <gettoken+0x118>
    s++;
     5bb:	83 c7 01             	add    $0x1,%edi
     5be:	eb ae                	jmp    56e <gettoken+0x5e>
  switch (*s)
     5c0:	7f 56                	jg     618 <gettoken+0x108>
     5c2:	83 e9 3b             	sub    $0x3b,%ecx
     5c5:	80 f9 01             	cmp    $0x1,%cl
     5c8:	76 f1                	jbe    5bb <gettoken+0xab>
    while (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
     5ca:	39 fb                	cmp    %edi,%ebx
     5cc:	77 2b                	ja     5f9 <gettoken+0xe9>
     5ce:	66 90                	xchg   %ax,%ax
     5d0:	eb 3b                	jmp    60d <gettoken+0xfd>
     5d2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
     5d8:	0f be 07             	movsbl (%edi),%eax
     5db:	c7 04 24 38 19 00 00 	movl   $0x1938,(%esp)
     5e2:	89 44 24 04          	mov    %eax,0x4(%esp)
     5e6:	e8 c5 06 00 00       	call   cb0 <strchr>
     5eb:	85 c0                	test   %eax,%eax
     5ed:	75 1e                	jne    60d <gettoken+0xfd>
      s++;
     5ef:	83 c7 01             	add    $0x1,%edi
    while (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
     5f2:	39 df                	cmp    %ebx,%edi
     5f4:	74 17                	je     60d <gettoken+0xfd>
     5f6:	0f be 07             	movsbl (%edi),%eax
     5f9:	89 44 24 04          	mov    %eax,0x4(%esp)
     5fd:	c7 04 24 40 19 00 00 	movl   $0x1940,(%esp)
     604:	e8 a7 06 00 00       	call   cb0 <strchr>
     609:	85 c0                	test   %eax,%eax
     60b:	74 cb                	je     5d8 <gettoken+0xc8>
    ret = 'a';
     60d:	be 61 00 00 00       	mov    $0x61,%esi
     612:	e9 57 ff ff ff       	jmp    56e <gettoken+0x5e>
     617:	90                   	nop
  switch (*s)
     618:	80 f9 7c             	cmp    $0x7c,%cl
     61b:	74 9e                	je     5bb <gettoken+0xab>
     61d:	8d 76 00             	lea    0x0(%esi),%esi
     620:	eb a8                	jmp    5ca <gettoken+0xba>
     622:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
      s++;
     628:	83 c7 02             	add    $0x2,%edi
      ret = '+';
     62b:	be 2b 00 00 00       	mov    $0x2b,%esi
     630:	e9 39 ff ff ff       	jmp    56e <gettoken+0x5e>
     635:	8d 76 00             	lea    0x0(%esi),%esi
  switch (*s)
     638:	80 f9 26             	cmp    $0x26,%cl
     63b:	75 8d                	jne    5ca <gettoken+0xba>
     63d:	e9 79 ff ff ff       	jmp    5bb <gettoken+0xab>
     642:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
     649:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000650 <peek>:

int peek(char **ps, char *es, char *toks)
{
     650:	55                   	push   %ebp
     651:	89 e5                	mov    %esp,%ebp
     653:	57                   	push   %edi
     654:	56                   	push   %esi
     655:	53                   	push   %ebx
     656:	83 ec 1c             	sub    $0x1c,%esp
     659:	8b 7d 08             	mov    0x8(%ebp),%edi
     65c:	8b 75 0c             	mov    0xc(%ebp),%esi
  char *s;

  s = *ps;
     65f:	8b 1f                	mov    (%edi),%ebx
  while (s < es && strchr(whitespace, *s))
     661:	39 f3                	cmp    %esi,%ebx
     663:	72 0a                	jb     66f <peek+0x1f>
     665:	eb 1f                	jmp    686 <peek+0x36>
     667:	90                   	nop
    s++;
     668:	83 c3 01             	add    $0x1,%ebx
  while (s < es && strchr(whitespace, *s))
     66b:	39 f3                	cmp    %esi,%ebx
     66d:	74 17                	je     686 <peek+0x36>
     66f:	0f be 03             	movsbl (%ebx),%eax
     672:	c7 04 24 40 19 00 00 	movl   $0x1940,(%esp)
     679:	89 44 24 04          	mov    %eax,0x4(%esp)
     67d:	e8 2e 06 00 00       	call   cb0 <strchr>
     682:	85 c0                	test   %eax,%eax
     684:	75 e2                	jne    668 <peek+0x18>
  *ps = s;
     686:	89 1f                	mov    %ebx,(%edi)
  return *s && strchr(toks, *s);
     688:	0f be 13             	movsbl (%ebx),%edx
     68b:	31 c0                	xor    %eax,%eax
     68d:	84 d2                	test   %dl,%dl
     68f:	74 17                	je     6a8 <peek+0x58>
     691:	8b 45 10             	mov    0x10(%ebp),%eax
     694:	89 54 24 04          	mov    %edx,0x4(%esp)
     698:	89 04 24             	mov    %eax,(%esp)
     69b:	e8 10 06 00 00       	call   cb0 <strchr>
     6a0:	85 c0                	test   %eax,%eax
     6a2:	0f 95 c0             	setne  %al
     6a5:	0f b6 c0             	movzbl %al,%eax
}
     6a8:	83 c4 1c             	add    $0x1c,%esp
     6ab:	5b                   	pop    %ebx
     6ac:	5e                   	pop    %esi
     6ad:	5f                   	pop    %edi
     6ae:	5d                   	pop    %ebp
     6af:	c3                   	ret    

000006b0 <parseredirs>:
  return cmd;
}

struct cmd *
parseredirs(struct cmd *cmd, char **ps, char *es)
{
     6b0:	55                   	push   %ebp
     6b1:	89 e5                	mov    %esp,%ebp
     6b3:	57                   	push   %edi
     6b4:	56                   	push   %esi
     6b5:	53                   	push   %ebx
     6b6:	83 ec 3c             	sub    $0x3c,%esp
     6b9:	8b 75 0c             	mov    0xc(%ebp),%esi
     6bc:	8b 5d 10             	mov    0x10(%ebp),%ebx
     6bf:	90                   	nop
  int tok;
  char *q, *eq;

  while (peek(ps, es, "<>"))
     6c0:	c7 44 24 08 09 13 00 	movl   $0x1309,0x8(%esp)
     6c7:	00 
     6c8:	89 5c 24 04          	mov    %ebx,0x4(%esp)
     6cc:	89 34 24             	mov    %esi,(%esp)
     6cf:	e8 7c ff ff ff       	call   650 <peek>
     6d4:	85 c0                	test   %eax,%eax
     6d6:	0f 84 9c 00 00 00    	je     778 <parseredirs+0xc8>
  {
    tok = gettoken(ps, es, 0, 0);
     6dc:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
     6e3:	00 
     6e4:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
     6eb:	00 
     6ec:	89 5c 24 04          	mov    %ebx,0x4(%esp)
     6f0:	89 34 24             	mov    %esi,(%esp)
     6f3:	e8 18 fe ff ff       	call   510 <gettoken>
    if (gettoken(ps, es, &q, &eq) != 'a')
     6f8:	89 5c 24 04          	mov    %ebx,0x4(%esp)
     6fc:	89 34 24             	mov    %esi,(%esp)
    tok = gettoken(ps, es, 0, 0);
     6ff:	89 c7                	mov    %eax,%edi
    if (gettoken(ps, es, &q, &eq) != 'a')
     701:	8d 45 e4             	lea    -0x1c(%ebp),%eax
     704:	89 44 24 0c          	mov    %eax,0xc(%esp)
     708:	8d 45 e0             	lea    -0x20(%ebp),%eax
     70b:	89 44 24 08          	mov    %eax,0x8(%esp)
     70f:	e8 fc fd ff ff       	call   510 <gettoken>
     714:	83 f8 61             	cmp    $0x61,%eax
     717:	75 6a                	jne    783 <parseredirs+0xd3>
      panic("missing file for redirection");
    switch (tok)
     719:	83 ff 3c             	cmp    $0x3c,%edi
     71c:	74 42                	je     760 <parseredirs+0xb0>
     71e:	83 ff 3e             	cmp    $0x3e,%edi
     721:	74 05                	je     728 <parseredirs+0x78>
     723:	83 ff 2b             	cmp    $0x2b,%edi
     726:	75 98                	jne    6c0 <parseredirs+0x10>
      break;
    case '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE, 1);
      break;
    case '+': // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE, 1);
     728:	c7 44 24 10 01 00 00 	movl   $0x1,0x10(%esp)
     72f:	00 
     730:	c7 44 24 0c 01 02 00 	movl   $0x201,0xc(%esp)
     737:	00 
     738:	8b 45 e4             	mov    -0x1c(%ebp),%eax
     73b:	89 44 24 08          	mov    %eax,0x8(%esp)
     73f:	8b 45 e0             	mov    -0x20(%ebp),%eax
     742:	89 44 24 04          	mov    %eax,0x4(%esp)
     746:	8b 45 08             	mov    0x8(%ebp),%eax
     749:	89 04 24             	mov    %eax,(%esp)
     74c:	e8 6f fc ff ff       	call   3c0 <redircmd>
     751:	89 45 08             	mov    %eax,0x8(%ebp)
      break;
     754:	e9 67 ff ff ff       	jmp    6c0 <parseredirs+0x10>
     759:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
     760:	c7 44 24 10 00 00 00 	movl   $0x0,0x10(%esp)
     767:	00 
     768:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
     76f:	00 
     770:	eb c6                	jmp    738 <parseredirs+0x88>
     772:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    }
  }
  return cmd;
}
     778:	8b 45 08             	mov    0x8(%ebp),%eax
     77b:	83 c4 3c             	add    $0x3c,%esp
     77e:	5b                   	pop    %ebx
     77f:	5e                   	pop    %esi
     780:	5f                   	pop    %edi
     781:	5d                   	pop    %ebp
     782:	c3                   	ret    
      panic("missing file for redirection");
     783:	c7 04 24 ec 12 00 00 	movl   $0x12ec,(%esp)
     78a:	e8 e1 f9 ff ff       	call   170 <panic>
     78f:	90                   	nop

00000790 <parseexec>:
  return cmd;
}

struct cmd *
parseexec(char **ps, char *es)
{
     790:	55                   	push   %ebp
     791:	89 e5                	mov    %esp,%ebp
     793:	57                   	push   %edi
     794:	56                   	push   %esi
     795:	53                   	push   %ebx
     796:	83 ec 3c             	sub    $0x3c,%esp
     799:	8b 75 08             	mov    0x8(%ebp),%esi
     79c:	8b 7d 0c             	mov    0xc(%ebp),%edi
  char *q, *eq;
  int tok, argc;
  struct execcmd *cmd;
  struct cmd *ret;

  if (peek(ps, es, "("))
     79f:	c7 44 24 08 0c 13 00 	movl   $0x130c,0x8(%esp)
     7a6:	00 
     7a7:	89 34 24             	mov    %esi,(%esp)
     7aa:	89 7c 24 04          	mov    %edi,0x4(%esp)
     7ae:	e8 9d fe ff ff       	call   650 <peek>
     7b3:	85 c0                	test   %eax,%eax
     7b5:	0f 85 a5 00 00 00    	jne    860 <parseexec+0xd0>
    return parseblock(ps, es);

  ret = execcmd();
     7bb:	e8 c0 fb ff ff       	call   380 <execcmd>
  cmd = (struct execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
     7c0:	89 7c 24 08          	mov    %edi,0x8(%esp)
     7c4:	89 74 24 04          	mov    %esi,0x4(%esp)
     7c8:	89 04 24             	mov    %eax,(%esp)
  ret = execcmd();
     7cb:	89 c3                	mov    %eax,%ebx
     7cd:	89 45 cc             	mov    %eax,-0x34(%ebp)
  ret = parseredirs(ret, ps, es);
     7d0:	e8 db fe ff ff       	call   6b0 <parseredirs>
  argc = 0;
     7d5:	c7 45 d4 00 00 00 00 	movl   $0x0,-0x2c(%ebp)
  ret = parseredirs(ret, ps, es);
     7dc:	89 45 d0             	mov    %eax,-0x30(%ebp)
  while (!peek(ps, es, "|)&;"))
     7df:	eb 1d                	jmp    7fe <parseexec+0x6e>
     7e1:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    if (argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
     7e8:	8b 45 d0             	mov    -0x30(%ebp),%eax
     7eb:	89 7c 24 08          	mov    %edi,0x8(%esp)
     7ef:	89 74 24 04          	mov    %esi,0x4(%esp)
     7f3:	89 04 24             	mov    %eax,(%esp)
     7f6:	e8 b5 fe ff ff       	call   6b0 <parseredirs>
     7fb:	89 45 d0             	mov    %eax,-0x30(%ebp)
  while (!peek(ps, es, "|)&;"))
     7fe:	c7 44 24 08 23 13 00 	movl   $0x1323,0x8(%esp)
     805:	00 
     806:	89 7c 24 04          	mov    %edi,0x4(%esp)
     80a:	89 34 24             	mov    %esi,(%esp)
     80d:	e8 3e fe ff ff       	call   650 <peek>
     812:	85 c0                	test   %eax,%eax
     814:	75 62                	jne    878 <parseexec+0xe8>
    if ((tok = gettoken(ps, es, &q, &eq)) == 0)
     816:	8d 45 e4             	lea    -0x1c(%ebp),%eax
     819:	89 44 24 0c          	mov    %eax,0xc(%esp)
     81d:	8d 45 e0             	lea    -0x20(%ebp),%eax
     820:	89 44 24 08          	mov    %eax,0x8(%esp)
     824:	89 7c 24 04          	mov    %edi,0x4(%esp)
     828:	89 34 24             	mov    %esi,(%esp)
     82b:	e8 e0 fc ff ff       	call   510 <gettoken>
     830:	85 c0                	test   %eax,%eax
     832:	74 44                	je     878 <parseexec+0xe8>
    if (tok != 'a')
     834:	83 f8 61             	cmp    $0x61,%eax
     837:	75 61                	jne    89a <parseexec+0x10a>
    cmd->argv[argc] = q;
     839:	8b 45 e0             	mov    -0x20(%ebp),%eax
     83c:	83 c3 04             	add    $0x4,%ebx
    argc++;
     83f:	83 45 d4 01          	addl   $0x1,-0x2c(%ebp)
    cmd->argv[argc] = q;
     843:	89 03                	mov    %eax,(%ebx)
    cmd->eargv[argc] = eq;
     845:	8b 45 e4             	mov    -0x1c(%ebp),%eax
     848:	89 43 28             	mov    %eax,0x28(%ebx)
    if (argc >= MAXARGS)
     84b:	83 7d d4 0a          	cmpl   $0xa,-0x2c(%ebp)
     84f:	75 97                	jne    7e8 <parseexec+0x58>
      panic("too many args");
     851:	c7 04 24 15 13 00 00 	movl   $0x1315,(%esp)
     858:	e8 13 f9 ff ff       	call   170 <panic>
     85d:	8d 76 00             	lea    0x0(%esi),%esi
    return parseblock(ps, es);
     860:	89 7c 24 04          	mov    %edi,0x4(%esp)
     864:	89 34 24             	mov    %esi,(%esp)
     867:	e8 84 01 00 00       	call   9f0 <parseblock>
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  return ret;
}
     86c:	83 c4 3c             	add    $0x3c,%esp
     86f:	5b                   	pop    %ebx
     870:	5e                   	pop    %esi
     871:	5f                   	pop    %edi
     872:	5d                   	pop    %ebp
     873:	c3                   	ret    
     874:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
     878:	8b 45 cc             	mov    -0x34(%ebp),%eax
     87b:	8b 55 d4             	mov    -0x2c(%ebp),%edx
     87e:	8d 04 90             	lea    (%eax,%edx,4),%eax
  cmd->argv[argc] = 0;
     881:	c7 40 04 00 00 00 00 	movl   $0x0,0x4(%eax)
  cmd->eargv[argc] = 0;
     888:	c7 40 2c 00 00 00 00 	movl   $0x0,0x2c(%eax)
  return ret;
     88f:	8b 45 d0             	mov    -0x30(%ebp),%eax
}
     892:	83 c4 3c             	add    $0x3c,%esp
     895:	5b                   	pop    %ebx
     896:	5e                   	pop    %esi
     897:	5f                   	pop    %edi
     898:	5d                   	pop    %ebp
     899:	c3                   	ret    
      panic("syntax");
     89a:	c7 04 24 0e 13 00 00 	movl   $0x130e,(%esp)
     8a1:	e8 ca f8 ff ff       	call   170 <panic>
     8a6:	8d 76 00             	lea    0x0(%esi),%esi
     8a9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

000008b0 <parsepipe>:
{
     8b0:	55                   	push   %ebp
     8b1:	89 e5                	mov    %esp,%ebp
     8b3:	57                   	push   %edi
     8b4:	56                   	push   %esi
     8b5:	53                   	push   %ebx
     8b6:	83 ec 1c             	sub    $0x1c,%esp
     8b9:	8b 5d 08             	mov    0x8(%ebp),%ebx
     8bc:	8b 75 0c             	mov    0xc(%ebp),%esi
  cmd = parseexec(ps, es);
     8bf:	89 1c 24             	mov    %ebx,(%esp)
     8c2:	89 74 24 04          	mov    %esi,0x4(%esp)
     8c6:	e8 c5 fe ff ff       	call   790 <parseexec>
  if (peek(ps, es, "|"))
     8cb:	c7 44 24 08 28 13 00 	movl   $0x1328,0x8(%esp)
     8d2:	00 
     8d3:	89 74 24 04          	mov    %esi,0x4(%esp)
     8d7:	89 1c 24             	mov    %ebx,(%esp)
  cmd = parseexec(ps, es);
     8da:	89 c7                	mov    %eax,%edi
  if (peek(ps, es, "|"))
     8dc:	e8 6f fd ff ff       	call   650 <peek>
     8e1:	85 c0                	test   %eax,%eax
     8e3:	75 0b                	jne    8f0 <parsepipe+0x40>
}
     8e5:	83 c4 1c             	add    $0x1c,%esp
     8e8:	89 f8                	mov    %edi,%eax
     8ea:	5b                   	pop    %ebx
     8eb:	5e                   	pop    %esi
     8ec:	5f                   	pop    %edi
     8ed:	5d                   	pop    %ebp
     8ee:	c3                   	ret    
     8ef:	90                   	nop
    gettoken(ps, es, 0, 0);
     8f0:	89 74 24 04          	mov    %esi,0x4(%esp)
     8f4:	89 1c 24             	mov    %ebx,(%esp)
     8f7:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
     8fe:	00 
     8ff:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
     906:	00 
     907:	e8 04 fc ff ff       	call   510 <gettoken>
    cmd = pipecmd(cmd, parsepipe(ps, es));
     90c:	89 74 24 04          	mov    %esi,0x4(%esp)
     910:	89 1c 24             	mov    %ebx,(%esp)
     913:	e8 98 ff ff ff       	call   8b0 <parsepipe>
     918:	89 7d 08             	mov    %edi,0x8(%ebp)
     91b:	89 45 0c             	mov    %eax,0xc(%ebp)
}
     91e:	83 c4 1c             	add    $0x1c,%esp
     921:	5b                   	pop    %ebx
     922:	5e                   	pop    %esi
     923:	5f                   	pop    %edi
     924:	5d                   	pop    %ebp
    cmd = pipecmd(cmd, parsepipe(ps, es));
     925:	e9 f6 fa ff ff       	jmp    420 <pipecmd>
     92a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

00000930 <parseline>:
{
     930:	55                   	push   %ebp
     931:	89 e5                	mov    %esp,%ebp
     933:	57                   	push   %edi
     934:	56                   	push   %esi
     935:	53                   	push   %ebx
     936:	83 ec 1c             	sub    $0x1c,%esp
     939:	8b 5d 08             	mov    0x8(%ebp),%ebx
     93c:	8b 75 0c             	mov    0xc(%ebp),%esi
  cmd = parsepipe(ps, es);
     93f:	89 1c 24             	mov    %ebx,(%esp)
     942:	89 74 24 04          	mov    %esi,0x4(%esp)
     946:	e8 65 ff ff ff       	call   8b0 <parsepipe>
     94b:	89 c7                	mov    %eax,%edi
  while (peek(ps, es, "&"))
     94d:	eb 27                	jmp    976 <parseline+0x46>
     94f:	90                   	nop
    gettoken(ps, es, 0, 0);
     950:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
     957:	00 
     958:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
     95f:	00 
     960:	89 74 24 04          	mov    %esi,0x4(%esp)
     964:	89 1c 24             	mov    %ebx,(%esp)
     967:	e8 a4 fb ff ff       	call   510 <gettoken>
    cmd = backcmd(cmd);
     96c:	89 3c 24             	mov    %edi,(%esp)
     96f:	e8 4c fb ff ff       	call   4c0 <backcmd>
     974:	89 c7                	mov    %eax,%edi
  while (peek(ps, es, "&"))
     976:	c7 44 24 08 2a 13 00 	movl   $0x132a,0x8(%esp)
     97d:	00 
     97e:	89 74 24 04          	mov    %esi,0x4(%esp)
     982:	89 1c 24             	mov    %ebx,(%esp)
     985:	e8 c6 fc ff ff       	call   650 <peek>
     98a:	85 c0                	test   %eax,%eax
     98c:	75 c2                	jne    950 <parseline+0x20>
  if (peek(ps, es, ";"))
     98e:	c7 44 24 08 26 13 00 	movl   $0x1326,0x8(%esp)
     995:	00 
     996:	89 74 24 04          	mov    %esi,0x4(%esp)
     99a:	89 1c 24             	mov    %ebx,(%esp)
     99d:	e8 ae fc ff ff       	call   650 <peek>
     9a2:	85 c0                	test   %eax,%eax
     9a4:	75 0a                	jne    9b0 <parseline+0x80>
}
     9a6:	83 c4 1c             	add    $0x1c,%esp
     9a9:	89 f8                	mov    %edi,%eax
     9ab:	5b                   	pop    %ebx
     9ac:	5e                   	pop    %esi
     9ad:	5f                   	pop    %edi
     9ae:	5d                   	pop    %ebp
     9af:	c3                   	ret    
    gettoken(ps, es, 0, 0);
     9b0:	89 74 24 04          	mov    %esi,0x4(%esp)
     9b4:	89 1c 24             	mov    %ebx,(%esp)
     9b7:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
     9be:	00 
     9bf:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
     9c6:	00 
     9c7:	e8 44 fb ff ff       	call   510 <gettoken>
    cmd = listcmd(cmd, parseline(ps, es));
     9cc:	89 74 24 04          	mov    %esi,0x4(%esp)
     9d0:	89 1c 24             	mov    %ebx,(%esp)
     9d3:	e8 58 ff ff ff       	call   930 <parseline>
     9d8:	89 7d 08             	mov    %edi,0x8(%ebp)
     9db:	89 45 0c             	mov    %eax,0xc(%ebp)
}
     9de:	83 c4 1c             	add    $0x1c,%esp
     9e1:	5b                   	pop    %ebx
     9e2:	5e                   	pop    %esi
     9e3:	5f                   	pop    %edi
     9e4:	5d                   	pop    %ebp
    cmd = listcmd(cmd, parseline(ps, es));
     9e5:	e9 86 fa ff ff       	jmp    470 <listcmd>
     9ea:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

000009f0 <parseblock>:
{
     9f0:	55                   	push   %ebp
     9f1:	89 e5                	mov    %esp,%ebp
     9f3:	57                   	push   %edi
     9f4:	56                   	push   %esi
     9f5:	53                   	push   %ebx
     9f6:	83 ec 1c             	sub    $0x1c,%esp
     9f9:	8b 5d 08             	mov    0x8(%ebp),%ebx
     9fc:	8b 75 0c             	mov    0xc(%ebp),%esi
  if (!peek(ps, es, "("))
     9ff:	c7 44 24 08 0c 13 00 	movl   $0x130c,0x8(%esp)
     a06:	00 
     a07:	89 1c 24             	mov    %ebx,(%esp)
     a0a:	89 74 24 04          	mov    %esi,0x4(%esp)
     a0e:	e8 3d fc ff ff       	call   650 <peek>
     a13:	85 c0                	test   %eax,%eax
     a15:	74 76                	je     a8d <parseblock+0x9d>
  gettoken(ps, es, 0, 0);
     a17:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
     a1e:	00 
     a1f:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
     a26:	00 
     a27:	89 74 24 04          	mov    %esi,0x4(%esp)
     a2b:	89 1c 24             	mov    %ebx,(%esp)
     a2e:	e8 dd fa ff ff       	call   510 <gettoken>
  cmd = parseline(ps, es);
     a33:	89 74 24 04          	mov    %esi,0x4(%esp)
     a37:	89 1c 24             	mov    %ebx,(%esp)
     a3a:	e8 f1 fe ff ff       	call   930 <parseline>
  if (!peek(ps, es, ")"))
     a3f:	c7 44 24 08 48 13 00 	movl   $0x1348,0x8(%esp)
     a46:	00 
     a47:	89 74 24 04          	mov    %esi,0x4(%esp)
     a4b:	89 1c 24             	mov    %ebx,(%esp)
  cmd = parseline(ps, es);
     a4e:	89 c7                	mov    %eax,%edi
  if (!peek(ps, es, ")"))
     a50:	e8 fb fb ff ff       	call   650 <peek>
     a55:	85 c0                	test   %eax,%eax
     a57:	74 40                	je     a99 <parseblock+0xa9>
  gettoken(ps, es, 0, 0);
     a59:	89 74 24 04          	mov    %esi,0x4(%esp)
     a5d:	89 1c 24             	mov    %ebx,(%esp)
     a60:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
     a67:	00 
     a68:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
     a6f:	00 
     a70:	e8 9b fa ff ff       	call   510 <gettoken>
  cmd = parseredirs(cmd, ps, es);
     a75:	89 74 24 08          	mov    %esi,0x8(%esp)
     a79:	89 5c 24 04          	mov    %ebx,0x4(%esp)
     a7d:	89 3c 24             	mov    %edi,(%esp)
     a80:	e8 2b fc ff ff       	call   6b0 <parseredirs>
}
     a85:	83 c4 1c             	add    $0x1c,%esp
     a88:	5b                   	pop    %ebx
     a89:	5e                   	pop    %esi
     a8a:	5f                   	pop    %edi
     a8b:	5d                   	pop    %ebp
     a8c:	c3                   	ret    
    panic("parseblock");
     a8d:	c7 04 24 2c 13 00 00 	movl   $0x132c,(%esp)
     a94:	e8 d7 f6 ff ff       	call   170 <panic>
    panic("syntax - missing )");
     a99:	c7 04 24 37 13 00 00 	movl   $0x1337,(%esp)
     aa0:	e8 cb f6 ff ff       	call   170 <panic>
     aa5:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
     aa9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000ab0 <nulterminate>:

// NUL-terminate all the counted strings.
struct cmd *
nulterminate(struct cmd *cmd)
{
     ab0:	55                   	push   %ebp
     ab1:	89 e5                	mov    %esp,%ebp
     ab3:	53                   	push   %ebx
     ab4:	83 ec 14             	sub    $0x14,%esp
     ab7:	8b 5d 08             	mov    0x8(%ebp),%ebx
  struct execcmd *ecmd;
  struct listcmd *lcmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if (cmd == 0)
     aba:	85 db                	test   %ebx,%ebx
     abc:	0f 84 8e 00 00 00    	je     b50 <nulterminate+0xa0>
    return 0;

  switch (cmd->type)
     ac2:	83 3b 05             	cmpl   $0x5,(%ebx)
     ac5:	77 49                	ja     b10 <nulterminate+0x60>
     ac7:	8b 03                	mov    (%ebx),%eax
     ac9:	ff 24 85 88 13 00 00 	jmp    *0x1388(,%eax,4)
    nulterminate(pcmd->right);
    break;

  case LIST:
    lcmd = (struct listcmd *)cmd;
    nulterminate(lcmd->left);
     ad0:	8b 43 04             	mov    0x4(%ebx),%eax
     ad3:	89 04 24             	mov    %eax,(%esp)
     ad6:	e8 d5 ff ff ff       	call   ab0 <nulterminate>
    nulterminate(lcmd->right);
     adb:	8b 43 08             	mov    0x8(%ebx),%eax
     ade:	89 04 24             	mov    %eax,(%esp)
     ae1:	e8 ca ff ff ff       	call   ab0 <nulterminate>
    break;
     ae6:	89 d8                	mov    %ebx,%eax
    bcmd = (struct backcmd *)cmd;
    nulterminate(bcmd->cmd);
    break;
  }
  return cmd;
}
     ae8:	83 c4 14             	add    $0x14,%esp
     aeb:	5b                   	pop    %ebx
     aec:	5d                   	pop    %ebp
     aed:	c3                   	ret    
     aee:	66 90                	xchg   %ax,%ax
    for (i = 0; ecmd->argv[i]; i++)
     af0:	8b 4b 04             	mov    0x4(%ebx),%ecx
     af3:	89 d8                	mov    %ebx,%eax
     af5:	85 c9                	test   %ecx,%ecx
     af7:	74 17                	je     b10 <nulterminate+0x60>
     af9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
      *ecmd->eargv[i] = 0;
     b00:	8b 50 2c             	mov    0x2c(%eax),%edx
     b03:	83 c0 04             	add    $0x4,%eax
     b06:	c6 02 00             	movb   $0x0,(%edx)
    for (i = 0; ecmd->argv[i]; i++)
     b09:	8b 50 04             	mov    0x4(%eax),%edx
     b0c:	85 d2                	test   %edx,%edx
     b0e:	75 f0                	jne    b00 <nulterminate+0x50>
}
     b10:	83 c4 14             	add    $0x14,%esp
  switch (cmd->type)
     b13:	89 d8                	mov    %ebx,%eax
}
     b15:	5b                   	pop    %ebx
     b16:	5d                   	pop    %ebp
     b17:	c3                   	ret    
    nulterminate(bcmd->cmd);
     b18:	8b 43 04             	mov    0x4(%ebx),%eax
     b1b:	89 04 24             	mov    %eax,(%esp)
     b1e:	e8 8d ff ff ff       	call   ab0 <nulterminate>
}
     b23:	83 c4 14             	add    $0x14,%esp
    break;
     b26:	89 d8                	mov    %ebx,%eax
}
     b28:	5b                   	pop    %ebx
     b29:	5d                   	pop    %ebp
     b2a:	c3                   	ret    
     b2b:	90                   	nop
     b2c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    nulterminate(rcmd->cmd);
     b30:	8b 43 04             	mov    0x4(%ebx),%eax
     b33:	89 04 24             	mov    %eax,(%esp)
     b36:	e8 75 ff ff ff       	call   ab0 <nulterminate>
    *rcmd->efile = 0;
     b3b:	8b 43 0c             	mov    0xc(%ebx),%eax
     b3e:	c6 00 00             	movb   $0x0,(%eax)
}
     b41:	83 c4 14             	add    $0x14,%esp
    break;
     b44:	89 d8                	mov    %ebx,%eax
}
     b46:	5b                   	pop    %ebx
     b47:	5d                   	pop    %ebp
     b48:	c3                   	ret    
     b49:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
    return 0;
     b50:	31 c0                	xor    %eax,%eax
     b52:	eb 94                	jmp    ae8 <nulterminate+0x38>
     b54:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
     b5a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

00000b60 <parsecmd>:
{
     b60:	55                   	push   %ebp
     b61:	89 e5                	mov    %esp,%ebp
     b63:	56                   	push   %esi
     b64:	53                   	push   %ebx
     b65:	83 ec 10             	sub    $0x10,%esp
  es = s + strlen(s);
     b68:	8b 5d 08             	mov    0x8(%ebp),%ebx
     b6b:	89 1c 24             	mov    %ebx,(%esp)
     b6e:	e8 ed 00 00 00       	call   c60 <strlen>
     b73:	01 c3                	add    %eax,%ebx
  cmd = parseline(&s, es);
     b75:	8d 45 08             	lea    0x8(%ebp),%eax
     b78:	89 5c 24 04          	mov    %ebx,0x4(%esp)
     b7c:	89 04 24             	mov    %eax,(%esp)
     b7f:	e8 ac fd ff ff       	call   930 <parseline>
  peek(&s, es, "");
     b84:	c7 44 24 08 d6 12 00 	movl   $0x12d6,0x8(%esp)
     b8b:	00 
     b8c:	89 5c 24 04          	mov    %ebx,0x4(%esp)
  cmd = parseline(&s, es);
     b90:	89 c6                	mov    %eax,%esi
  peek(&s, es, "");
     b92:	8d 45 08             	lea    0x8(%ebp),%eax
     b95:	89 04 24             	mov    %eax,(%esp)
     b98:	e8 b3 fa ff ff       	call   650 <peek>
  if (s != es)
     b9d:	8b 45 08             	mov    0x8(%ebp),%eax
     ba0:	39 d8                	cmp    %ebx,%eax
     ba2:	75 11                	jne    bb5 <parsecmd+0x55>
  nulterminate(cmd);
     ba4:	89 34 24             	mov    %esi,(%esp)
     ba7:	e8 04 ff ff ff       	call   ab0 <nulterminate>
}
     bac:	83 c4 10             	add    $0x10,%esp
     baf:	89 f0                	mov    %esi,%eax
     bb1:	5b                   	pop    %ebx
     bb2:	5e                   	pop    %esi
     bb3:	5d                   	pop    %ebp
     bb4:	c3                   	ret    
    printf(2, "leftovers: %s\n", s);
     bb5:	89 44 24 08          	mov    %eax,0x8(%esp)
     bb9:	c7 44 24 04 4a 13 00 	movl   $0x134a,0x4(%esp)
     bc0:	00 
     bc1:	c7 04 24 02 00 00 00 	movl   $0x2,(%esp)
     bc8:	e8 83 03 00 00       	call   f50 <printf>
    panic("syntax");
     bcd:	c7 04 24 0e 13 00 00 	movl   $0x130e,(%esp)
     bd4:	e8 97 f5 ff ff       	call   170 <panic>
     bd9:	66 90                	xchg   %ax,%ax
     bdb:	66 90                	xchg   %ax,%ax
     bdd:	66 90                	xchg   %ax,%ax
     bdf:	90                   	nop

00000be0 <strcpy>:
#include "user.h"
#include "x86.h"

char*
strcpy(char *s, const char *t)
{
     be0:	55                   	push   %ebp
     be1:	89 e5                	mov    %esp,%ebp
     be3:	8b 45 08             	mov    0x8(%ebp),%eax
     be6:	8b 4d 0c             	mov    0xc(%ebp),%ecx
     be9:	53                   	push   %ebx
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
     bea:	89 c2                	mov    %eax,%edx
     bec:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
     bf0:	83 c1 01             	add    $0x1,%ecx
     bf3:	0f b6 59 ff          	movzbl -0x1(%ecx),%ebx
     bf7:	83 c2 01             	add    $0x1,%edx
     bfa:	84 db                	test   %bl,%bl
     bfc:	88 5a ff             	mov    %bl,-0x1(%edx)
     bff:	75 ef                	jne    bf0 <strcpy+0x10>
    ;
  return os;
}
     c01:	5b                   	pop    %ebx
     c02:	5d                   	pop    %ebp
     c03:	c3                   	ret    
     c04:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
     c0a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

00000c10 <strcmp>:

int
strcmp(const char *p, const char *q)
{
     c10:	55                   	push   %ebp
     c11:	89 e5                	mov    %esp,%ebp
     c13:	8b 55 08             	mov    0x8(%ebp),%edx
     c16:	53                   	push   %ebx
     c17:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  while(*p && *p == *q)
     c1a:	0f b6 02             	movzbl (%edx),%eax
     c1d:	84 c0                	test   %al,%al
     c1f:	74 2d                	je     c4e <strcmp+0x3e>
     c21:	0f b6 19             	movzbl (%ecx),%ebx
     c24:	38 d8                	cmp    %bl,%al
     c26:	74 0e                	je     c36 <strcmp+0x26>
     c28:	eb 2b                	jmp    c55 <strcmp+0x45>
     c2a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
     c30:	38 c8                	cmp    %cl,%al
     c32:	75 15                	jne    c49 <strcmp+0x39>
    p++, q++;
     c34:	89 d9                	mov    %ebx,%ecx
     c36:	83 c2 01             	add    $0x1,%edx
  while(*p && *p == *q)
     c39:	0f b6 02             	movzbl (%edx),%eax
    p++, q++;
     c3c:	8d 59 01             	lea    0x1(%ecx),%ebx
  while(*p && *p == *q)
     c3f:	0f b6 49 01          	movzbl 0x1(%ecx),%ecx
     c43:	84 c0                	test   %al,%al
     c45:	75 e9                	jne    c30 <strcmp+0x20>
     c47:	31 c0                	xor    %eax,%eax
  return (uchar)*p - (uchar)*q;
     c49:	29 c8                	sub    %ecx,%eax
}
     c4b:	5b                   	pop    %ebx
     c4c:	5d                   	pop    %ebp
     c4d:	c3                   	ret    
     c4e:	0f b6 09             	movzbl (%ecx),%ecx
  while(*p && *p == *q)
     c51:	31 c0                	xor    %eax,%eax
     c53:	eb f4                	jmp    c49 <strcmp+0x39>
     c55:	0f b6 cb             	movzbl %bl,%ecx
     c58:	eb ef                	jmp    c49 <strcmp+0x39>
     c5a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

00000c60 <strlen>:

uint
strlen(const char *s)
{
     c60:	55                   	push   %ebp
     c61:	89 e5                	mov    %esp,%ebp
     c63:	8b 4d 08             	mov    0x8(%ebp),%ecx
  int n;

  for(n = 0; s[n]; n++)
     c66:	80 39 00             	cmpb   $0x0,(%ecx)
     c69:	74 12                	je     c7d <strlen+0x1d>
     c6b:	31 d2                	xor    %edx,%edx
     c6d:	8d 76 00             	lea    0x0(%esi),%esi
     c70:	83 c2 01             	add    $0x1,%edx
     c73:	80 3c 11 00          	cmpb   $0x0,(%ecx,%edx,1)
     c77:	89 d0                	mov    %edx,%eax
     c79:	75 f5                	jne    c70 <strlen+0x10>
    ;
  return n;
}
     c7b:	5d                   	pop    %ebp
     c7c:	c3                   	ret    
  for(n = 0; s[n]; n++)
     c7d:	31 c0                	xor    %eax,%eax
}
     c7f:	5d                   	pop    %ebp
     c80:	c3                   	ret    
     c81:	eb 0d                	jmp    c90 <memset>
     c83:	90                   	nop
     c84:	90                   	nop
     c85:	90                   	nop
     c86:	90                   	nop
     c87:	90                   	nop
     c88:	90                   	nop
     c89:	90                   	nop
     c8a:	90                   	nop
     c8b:	90                   	nop
     c8c:	90                   	nop
     c8d:	90                   	nop
     c8e:	90                   	nop
     c8f:	90                   	nop

00000c90 <memset>:

void*
memset(void *dst, int c, uint n)
{
     c90:	55                   	push   %ebp
     c91:	89 e5                	mov    %esp,%ebp
     c93:	8b 55 08             	mov    0x8(%ebp),%edx
     c96:	57                   	push   %edi
}

static inline void
stosb(void *addr, int data, int cnt)
{
  asm volatile("cld; rep stosb" :
     c97:	8b 4d 10             	mov    0x10(%ebp),%ecx
     c9a:	8b 45 0c             	mov    0xc(%ebp),%eax
     c9d:	89 d7                	mov    %edx,%edi
     c9f:	fc                   	cld    
     ca0:	f3 aa                	rep stos %al,%es:(%edi)
  stosb(dst, c, n);
  return dst;
}
     ca2:	89 d0                	mov    %edx,%eax
     ca4:	5f                   	pop    %edi
     ca5:	5d                   	pop    %ebp
     ca6:	c3                   	ret    
     ca7:	89 f6                	mov    %esi,%esi
     ca9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000cb0 <strchr>:

char*
strchr(const char *s, char c)
{
     cb0:	55                   	push   %ebp
     cb1:	89 e5                	mov    %esp,%ebp
     cb3:	8b 45 08             	mov    0x8(%ebp),%eax
     cb6:	53                   	push   %ebx
     cb7:	8b 55 0c             	mov    0xc(%ebp),%edx
  for(; *s; s++)
     cba:	0f b6 18             	movzbl (%eax),%ebx
     cbd:	84 db                	test   %bl,%bl
     cbf:	74 1d                	je     cde <strchr+0x2e>
    if(*s == c)
     cc1:	38 d3                	cmp    %dl,%bl
     cc3:	89 d1                	mov    %edx,%ecx
     cc5:	75 0d                	jne    cd4 <strchr+0x24>
     cc7:	eb 17                	jmp    ce0 <strchr+0x30>
     cc9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
     cd0:	38 ca                	cmp    %cl,%dl
     cd2:	74 0c                	je     ce0 <strchr+0x30>
  for(; *s; s++)
     cd4:	83 c0 01             	add    $0x1,%eax
     cd7:	0f b6 10             	movzbl (%eax),%edx
     cda:	84 d2                	test   %dl,%dl
     cdc:	75 f2                	jne    cd0 <strchr+0x20>
      return (char*)s;
  return 0;
     cde:	31 c0                	xor    %eax,%eax
}
     ce0:	5b                   	pop    %ebx
     ce1:	5d                   	pop    %ebp
     ce2:	c3                   	ret    
     ce3:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
     ce9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000cf0 <gets>:

char*
gets(char *buf, int max)
{
     cf0:	55                   	push   %ebp
     cf1:	89 e5                	mov    %esp,%ebp
     cf3:	57                   	push   %edi
     cf4:	56                   	push   %esi
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
     cf5:	31 f6                	xor    %esi,%esi
{
     cf7:	53                   	push   %ebx
     cf8:	83 ec 2c             	sub    $0x2c,%esp
    cc = read(0, &c, 1);
     cfb:	8d 7d e7             	lea    -0x19(%ebp),%edi
  for(i=0; i+1 < max; ){
     cfe:	eb 31                	jmp    d31 <gets+0x41>
    cc = read(0, &c, 1);
     d00:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
     d07:	00 
     d08:	89 7c 24 04          	mov    %edi,0x4(%esp)
     d0c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     d13:	e8 02 01 00 00       	call   e1a <read>
    if(cc < 1)
     d18:	85 c0                	test   %eax,%eax
     d1a:	7e 1d                	jle    d39 <gets+0x49>
      break;
    buf[i++] = c;
     d1c:	0f b6 45 e7          	movzbl -0x19(%ebp),%eax
  for(i=0; i+1 < max; ){
     d20:	89 de                	mov    %ebx,%esi
    buf[i++] = c;
     d22:	8b 55 08             	mov    0x8(%ebp),%edx
    if(c == '\n' || c == '\r')
     d25:	3c 0d                	cmp    $0xd,%al
    buf[i++] = c;
     d27:	88 44 1a ff          	mov    %al,-0x1(%edx,%ebx,1)
    if(c == '\n' || c == '\r')
     d2b:	74 0c                	je     d39 <gets+0x49>
     d2d:	3c 0a                	cmp    $0xa,%al
     d2f:	74 08                	je     d39 <gets+0x49>
  for(i=0; i+1 < max; ){
     d31:	8d 5e 01             	lea    0x1(%esi),%ebx
     d34:	3b 5d 0c             	cmp    0xc(%ebp),%ebx
     d37:	7c c7                	jl     d00 <gets+0x10>
      break;
  }
  buf[i] = '\0';
     d39:	8b 45 08             	mov    0x8(%ebp),%eax
     d3c:	c6 04 30 00          	movb   $0x0,(%eax,%esi,1)
  return buf;
}
     d40:	83 c4 2c             	add    $0x2c,%esp
     d43:	5b                   	pop    %ebx
     d44:	5e                   	pop    %esi
     d45:	5f                   	pop    %edi
     d46:	5d                   	pop    %ebp
     d47:	c3                   	ret    
     d48:	90                   	nop
     d49:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00000d50 <stat>:

int
stat(const char *n, struct stat *st)
{
     d50:	55                   	push   %ebp
     d51:	89 e5                	mov    %esp,%ebp
     d53:	56                   	push   %esi
     d54:	53                   	push   %ebx
     d55:	83 ec 10             	sub    $0x10,%esp
  int fd;
  int r;

  fd = open(n, O_RDONLY);
     d58:	8b 45 08             	mov    0x8(%ebp),%eax
     d5b:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
     d62:	00 
     d63:	89 04 24             	mov    %eax,(%esp)
     d66:	e8 d7 00 00 00       	call   e42 <open>
  if(fd < 0)
     d6b:	85 c0                	test   %eax,%eax
  fd = open(n, O_RDONLY);
     d6d:	89 c3                	mov    %eax,%ebx
  if(fd < 0)
     d6f:	78 27                	js     d98 <stat+0x48>
    return -1;
  r = fstat(fd, st);
     d71:	8b 45 0c             	mov    0xc(%ebp),%eax
     d74:	89 1c 24             	mov    %ebx,(%esp)
     d77:	89 44 24 04          	mov    %eax,0x4(%esp)
     d7b:	e8 da 00 00 00       	call   e5a <fstat>
  close(fd);
     d80:	89 1c 24             	mov    %ebx,(%esp)
  r = fstat(fd, st);
     d83:	89 c6                	mov    %eax,%esi
  close(fd);
     d85:	e8 a0 00 00 00       	call   e2a <close>
  return r;
     d8a:	89 f0                	mov    %esi,%eax
}
     d8c:	83 c4 10             	add    $0x10,%esp
     d8f:	5b                   	pop    %ebx
     d90:	5e                   	pop    %esi
     d91:	5d                   	pop    %ebp
     d92:	c3                   	ret    
     d93:	90                   	nop
     d94:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    return -1;
     d98:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
     d9d:	eb ed                	jmp    d8c <stat+0x3c>
     d9f:	90                   	nop

00000da0 <atoi>:

int
atoi(const char *s)
{
     da0:	55                   	push   %ebp
     da1:	89 e5                	mov    %esp,%ebp
     da3:	8b 4d 08             	mov    0x8(%ebp),%ecx
     da6:	53                   	push   %ebx
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
     da7:	0f be 11             	movsbl (%ecx),%edx
     daa:	8d 42 d0             	lea    -0x30(%edx),%eax
     dad:	3c 09                	cmp    $0x9,%al
  n = 0;
     daf:	b8 00 00 00 00       	mov    $0x0,%eax
  while('0' <= *s && *s <= '9')
     db4:	77 17                	ja     dcd <atoi+0x2d>
     db6:	66 90                	xchg   %ax,%ax
    n = n*10 + *s++ - '0';
     db8:	83 c1 01             	add    $0x1,%ecx
     dbb:	8d 04 80             	lea    (%eax,%eax,4),%eax
     dbe:	8d 44 42 d0          	lea    -0x30(%edx,%eax,2),%eax
  while('0' <= *s && *s <= '9')
     dc2:	0f be 11             	movsbl (%ecx),%edx
     dc5:	8d 5a d0             	lea    -0x30(%edx),%ebx
     dc8:	80 fb 09             	cmp    $0x9,%bl
     dcb:	76 eb                	jbe    db8 <atoi+0x18>
  return n;
}
     dcd:	5b                   	pop    %ebx
     dce:	5d                   	pop    %ebp
     dcf:	c3                   	ret    

00000dd0 <memmove>:

void*
memmove(void *vdst, const void *vsrc, int n)
{
     dd0:	55                   	push   %ebp
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
     dd1:	31 d2                	xor    %edx,%edx
{
     dd3:	89 e5                	mov    %esp,%ebp
     dd5:	56                   	push   %esi
     dd6:	8b 45 08             	mov    0x8(%ebp),%eax
     dd9:	53                   	push   %ebx
     dda:	8b 5d 10             	mov    0x10(%ebp),%ebx
     ddd:	8b 75 0c             	mov    0xc(%ebp),%esi
  while(n-- > 0)
     de0:	85 db                	test   %ebx,%ebx
     de2:	7e 12                	jle    df6 <memmove+0x26>
     de4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    *dst++ = *src++;
     de8:	0f b6 0c 16          	movzbl (%esi,%edx,1),%ecx
     dec:	88 0c 10             	mov    %cl,(%eax,%edx,1)
     def:	83 c2 01             	add    $0x1,%edx
  while(n-- > 0)
     df2:	39 da                	cmp    %ebx,%edx
     df4:	75 f2                	jne    de8 <memmove+0x18>
  return vdst;
}
     df6:	5b                   	pop    %ebx
     df7:	5e                   	pop    %esi
     df8:	5d                   	pop    %ebp
     df9:	c3                   	ret    

00000dfa <fork>:
     dfa:	b8 01 00 00 00       	mov    $0x1,%eax
     dff:	cd 40                	int    $0x40
     e01:	c3                   	ret    

00000e02 <exit>:
     e02:	b8 02 00 00 00       	mov    $0x2,%eax
     e07:	cd 40                	int    $0x40
     e09:	c3                   	ret    

00000e0a <wait>:
     e0a:	b8 03 00 00 00       	mov    $0x3,%eax
     e0f:	cd 40                	int    $0x40
     e11:	c3                   	ret    

00000e12 <pipe>:
     e12:	b8 04 00 00 00       	mov    $0x4,%eax
     e17:	cd 40                	int    $0x40
     e19:	c3                   	ret    

00000e1a <read>:
     e1a:	b8 05 00 00 00       	mov    $0x5,%eax
     e1f:	cd 40                	int    $0x40
     e21:	c3                   	ret    

00000e22 <write>:
     e22:	b8 10 00 00 00       	mov    $0x10,%eax
     e27:	cd 40                	int    $0x40
     e29:	c3                   	ret    

00000e2a <close>:
     e2a:	b8 15 00 00 00       	mov    $0x15,%eax
     e2f:	cd 40                	int    $0x40
     e31:	c3                   	ret    

00000e32 <kill>:
     e32:	b8 06 00 00 00       	mov    $0x6,%eax
     e37:	cd 40                	int    $0x40
     e39:	c3                   	ret    

00000e3a <exec>:
     e3a:	b8 07 00 00 00       	mov    $0x7,%eax
     e3f:	cd 40                	int    $0x40
     e41:	c3                   	ret    

00000e42 <open>:
     e42:	b8 0f 00 00 00       	mov    $0xf,%eax
     e47:	cd 40                	int    $0x40
     e49:	c3                   	ret    

00000e4a <mknod>:
     e4a:	b8 11 00 00 00       	mov    $0x11,%eax
     e4f:	cd 40                	int    $0x40
     e51:	c3                   	ret    

00000e52 <unlink>:
     e52:	b8 12 00 00 00       	mov    $0x12,%eax
     e57:	cd 40                	int    $0x40
     e59:	c3                   	ret    

00000e5a <fstat>:
     e5a:	b8 08 00 00 00       	mov    $0x8,%eax
     e5f:	cd 40                	int    $0x40
     e61:	c3                   	ret    

00000e62 <link>:
     e62:	b8 13 00 00 00       	mov    $0x13,%eax
     e67:	cd 40                	int    $0x40
     e69:	c3                   	ret    

00000e6a <mkdir>:
     e6a:	b8 14 00 00 00       	mov    $0x14,%eax
     e6f:	cd 40                	int    $0x40
     e71:	c3                   	ret    

00000e72 <chdir>:
     e72:	b8 09 00 00 00       	mov    $0x9,%eax
     e77:	cd 40                	int    $0x40
     e79:	c3                   	ret    

00000e7a <dup>:
     e7a:	b8 0a 00 00 00       	mov    $0xa,%eax
     e7f:	cd 40                	int    $0x40
     e81:	c3                   	ret    

00000e82 <getpid>:
     e82:	b8 0b 00 00 00       	mov    $0xb,%eax
     e87:	cd 40                	int    $0x40
     e89:	c3                   	ret    

00000e8a <sbrk>:
     e8a:	b8 0c 00 00 00       	mov    $0xc,%eax
     e8f:	cd 40                	int    $0x40
     e91:	c3                   	ret    

00000e92 <sleep>:
     e92:	b8 0d 00 00 00       	mov    $0xd,%eax
     e97:	cd 40                	int    $0x40
     e99:	c3                   	ret    

00000e9a <uptime>:
     e9a:	b8 0e 00 00 00       	mov    $0xe,%eax
     e9f:	cd 40                	int    $0x40
     ea1:	c3                   	ret    
     ea2:	66 90                	xchg   %ax,%ax
     ea4:	66 90                	xchg   %ax,%ax
     ea6:	66 90                	xchg   %ax,%ax
     ea8:	66 90                	xchg   %ax,%ax
     eaa:	66 90                	xchg   %ax,%ax
     eac:	66 90                	xchg   %ax,%ax
     eae:	66 90                	xchg   %ax,%ax

00000eb0 <printint>:
  write(fd, &c, 1);
}

static void
printint(int fd, int xx, int base, int sgn)
{
     eb0:	55                   	push   %ebp
     eb1:	89 e5                	mov    %esp,%ebp
     eb3:	57                   	push   %edi
     eb4:	56                   	push   %esi
     eb5:	89 c6                	mov    %eax,%esi
     eb7:	53                   	push   %ebx
     eb8:	83 ec 4c             	sub    $0x4c,%esp
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
     ebb:	8b 5d 08             	mov    0x8(%ebp),%ebx
     ebe:	85 db                	test   %ebx,%ebx
     ec0:	74 09                	je     ecb <printint+0x1b>
     ec2:	89 d0                	mov    %edx,%eax
     ec4:	c1 e8 1f             	shr    $0x1f,%eax
     ec7:	84 c0                	test   %al,%al
     ec9:	75 75                	jne    f40 <printint+0x90>
    neg = 1;
    x = -xx;
  } else {
    x = xx;
     ecb:	89 d0                	mov    %edx,%eax
  neg = 0;
     ecd:	c7 45 c4 00 00 00 00 	movl   $0x0,-0x3c(%ebp)
     ed4:	89 75 c0             	mov    %esi,-0x40(%ebp)
  }

  i = 0;
     ed7:	31 ff                	xor    %edi,%edi
     ed9:	89 ce                	mov    %ecx,%esi
     edb:	8d 5d d7             	lea    -0x29(%ebp),%ebx
     ede:	eb 02                	jmp    ee2 <printint+0x32>
  do{
    buf[i++] = digits[x % base];
     ee0:	89 cf                	mov    %ecx,%edi
     ee2:	31 d2                	xor    %edx,%edx
     ee4:	f7 f6                	div    %esi
     ee6:	8d 4f 01             	lea    0x1(%edi),%ecx
     ee9:	0f b6 92 a7 13 00 00 	movzbl 0x13a7(%edx),%edx
  }while((x /= base) != 0);
     ef0:	85 c0                	test   %eax,%eax
    buf[i++] = digits[x % base];
     ef2:	88 14 0b             	mov    %dl,(%ebx,%ecx,1)
  }while((x /= base) != 0);
     ef5:	75 e9                	jne    ee0 <printint+0x30>
  if(neg)
     ef7:	8b 55 c4             	mov    -0x3c(%ebp),%edx
    buf[i++] = digits[x % base];
     efa:	89 c8                	mov    %ecx,%eax
     efc:	8b 75 c0             	mov    -0x40(%ebp),%esi
  if(neg)
     eff:	85 d2                	test   %edx,%edx
     f01:	74 08                	je     f0b <printint+0x5b>
    buf[i++] = '-';
     f03:	8d 4f 02             	lea    0x2(%edi),%ecx
     f06:	c6 44 05 d8 2d       	movb   $0x2d,-0x28(%ebp,%eax,1)

  while(--i >= 0)
     f0b:	8d 79 ff             	lea    -0x1(%ecx),%edi
     f0e:	66 90                	xchg   %ax,%ax
     f10:	0f b6 44 3d d8       	movzbl -0x28(%ebp,%edi,1),%eax
     f15:	83 ef 01             	sub    $0x1,%edi
  write(fd, &c, 1);
     f18:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
     f1f:	00 
     f20:	89 5c 24 04          	mov    %ebx,0x4(%esp)
     f24:	89 34 24             	mov    %esi,(%esp)
     f27:	88 45 d7             	mov    %al,-0x29(%ebp)
     f2a:	e8 f3 fe ff ff       	call   e22 <write>
  while(--i >= 0)
     f2f:	83 ff ff             	cmp    $0xffffffff,%edi
     f32:	75 dc                	jne    f10 <printint+0x60>
    putc(fd, buf[i]);
}
     f34:	83 c4 4c             	add    $0x4c,%esp
     f37:	5b                   	pop    %ebx
     f38:	5e                   	pop    %esi
     f39:	5f                   	pop    %edi
     f3a:	5d                   	pop    %ebp
     f3b:	c3                   	ret    
     f3c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    x = -xx;
     f40:	89 d0                	mov    %edx,%eax
     f42:	f7 d8                	neg    %eax
    neg = 1;
     f44:	c7 45 c4 01 00 00 00 	movl   $0x1,-0x3c(%ebp)
     f4b:	eb 87                	jmp    ed4 <printint+0x24>
     f4d:	8d 76 00             	lea    0x0(%esi),%esi

00000f50 <printf>:

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, const char *fmt, ...)
{
     f50:	55                   	push   %ebp
     f51:	89 e5                	mov    %esp,%ebp
     f53:	57                   	push   %edi
  char *s;
  int c, i, state;
  uint *ap;

  state = 0;
     f54:	31 ff                	xor    %edi,%edi
{
     f56:	56                   	push   %esi
     f57:	53                   	push   %ebx
     f58:	83 ec 3c             	sub    $0x3c,%esp
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
     f5b:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  ap = (uint*)(void*)&fmt + 1;
     f5e:	8d 45 10             	lea    0x10(%ebp),%eax
{
     f61:	8b 75 08             	mov    0x8(%ebp),%esi
  ap = (uint*)(void*)&fmt + 1;
     f64:	89 45 d4             	mov    %eax,-0x2c(%ebp)
  for(i = 0; fmt[i]; i++){
     f67:	0f b6 13             	movzbl (%ebx),%edx
     f6a:	83 c3 01             	add    $0x1,%ebx
     f6d:	84 d2                	test   %dl,%dl
     f6f:	75 39                	jne    faa <printf+0x5a>
     f71:	e9 c2 00 00 00       	jmp    1038 <printf+0xe8>
     f76:	66 90                	xchg   %ax,%ax
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
     f78:	83 fa 25             	cmp    $0x25,%edx
     f7b:	0f 84 bf 00 00 00    	je     1040 <printf+0xf0>
  write(fd, &c, 1);
     f81:	8d 45 e2             	lea    -0x1e(%ebp),%eax
     f84:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
     f8b:	00 
     f8c:	89 44 24 04          	mov    %eax,0x4(%esp)
     f90:	89 34 24             	mov    %esi,(%esp)
        state = '%';
      } else {
        putc(fd, c);
     f93:	88 55 e2             	mov    %dl,-0x1e(%ebp)
  write(fd, &c, 1);
     f96:	e8 87 fe ff ff       	call   e22 <write>
     f9b:	83 c3 01             	add    $0x1,%ebx
  for(i = 0; fmt[i]; i++){
     f9e:	0f b6 53 ff          	movzbl -0x1(%ebx),%edx
     fa2:	84 d2                	test   %dl,%dl
     fa4:	0f 84 8e 00 00 00    	je     1038 <printf+0xe8>
    if(state == 0){
     faa:	85 ff                	test   %edi,%edi
    c = fmt[i] & 0xff;
     fac:	0f be c2             	movsbl %dl,%eax
    if(state == 0){
     faf:	74 c7                	je     f78 <printf+0x28>
      }
    } else if(state == '%'){
     fb1:	83 ff 25             	cmp    $0x25,%edi
     fb4:	75 e5                	jne    f9b <printf+0x4b>
      if(c == 'd'){
     fb6:	83 fa 64             	cmp    $0x64,%edx
     fb9:	0f 84 31 01 00 00    	je     10f0 <printf+0x1a0>
        printint(fd, *ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
     fbf:	25 f7 00 00 00       	and    $0xf7,%eax
     fc4:	83 f8 70             	cmp    $0x70,%eax
     fc7:	0f 84 83 00 00 00    	je     1050 <printf+0x100>
        printint(fd, *ap, 16, 0);
        ap++;
      } else if(c == 's'){
     fcd:	83 fa 73             	cmp    $0x73,%edx
     fd0:	0f 84 a2 00 00 00    	je     1078 <printf+0x128>
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
        }
      } else if(c == 'c'){
     fd6:	83 fa 63             	cmp    $0x63,%edx
     fd9:	0f 84 35 01 00 00    	je     1114 <printf+0x1c4>
        putc(fd, *ap);
        ap++;
      } else if(c == '%'){
     fdf:	83 fa 25             	cmp    $0x25,%edx
     fe2:	0f 84 e0 00 00 00    	je     10c8 <printf+0x178>
  write(fd, &c, 1);
     fe8:	8d 45 e6             	lea    -0x1a(%ebp),%eax
     feb:	83 c3 01             	add    $0x1,%ebx
     fee:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
     ff5:	00 
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
     ff6:	31 ff                	xor    %edi,%edi
  write(fd, &c, 1);
     ff8:	89 44 24 04          	mov    %eax,0x4(%esp)
     ffc:	89 34 24             	mov    %esi,(%esp)
     fff:	89 55 d0             	mov    %edx,-0x30(%ebp)
    1002:	c6 45 e6 25          	movb   $0x25,-0x1a(%ebp)
    1006:	e8 17 fe ff ff       	call   e22 <write>
        putc(fd, c);
    100b:	8b 55 d0             	mov    -0x30(%ebp),%edx
  write(fd, &c, 1);
    100e:	8d 45 e7             	lea    -0x19(%ebp),%eax
    1011:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
    1018:	00 
    1019:	89 44 24 04          	mov    %eax,0x4(%esp)
    101d:	89 34 24             	mov    %esi,(%esp)
        putc(fd, c);
    1020:	88 55 e7             	mov    %dl,-0x19(%ebp)
  write(fd, &c, 1);
    1023:	e8 fa fd ff ff       	call   e22 <write>
  for(i = 0; fmt[i]; i++){
    1028:	0f b6 53 ff          	movzbl -0x1(%ebx),%edx
    102c:	84 d2                	test   %dl,%dl
    102e:	0f 85 76 ff ff ff    	jne    faa <printf+0x5a>
    1034:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    }
  }
}
    1038:	83 c4 3c             	add    $0x3c,%esp
    103b:	5b                   	pop    %ebx
    103c:	5e                   	pop    %esi
    103d:	5f                   	pop    %edi
    103e:	5d                   	pop    %ebp
    103f:	c3                   	ret    
        state = '%';
    1040:	bf 25 00 00 00       	mov    $0x25,%edi
    1045:	e9 51 ff ff ff       	jmp    f9b <printf+0x4b>
    104a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        printint(fd, *ap, 16, 0);
    1050:	8b 45 d4             	mov    -0x2c(%ebp),%eax
    1053:	b9 10 00 00 00       	mov    $0x10,%ecx
      state = 0;
    1058:	31 ff                	xor    %edi,%edi
        printint(fd, *ap, 16, 0);
    105a:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1061:	8b 10                	mov    (%eax),%edx
    1063:	89 f0                	mov    %esi,%eax
    1065:	e8 46 fe ff ff       	call   eb0 <printint>
        ap++;
    106a:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
    106e:	e9 28 ff ff ff       	jmp    f9b <printf+0x4b>
    1073:	90                   	nop
    1074:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
        s = (char*)*ap;
    1078:	8b 45 d4             	mov    -0x2c(%ebp),%eax
        ap++;
    107b:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
        s = (char*)*ap;
    107f:	8b 38                	mov    (%eax),%edi
          s = "(null)";
    1081:	b8 a0 13 00 00       	mov    $0x13a0,%eax
    1086:	85 ff                	test   %edi,%edi
    1088:	0f 44 f8             	cmove  %eax,%edi
        while(*s != 0){
    108b:	0f b6 07             	movzbl (%edi),%eax
    108e:	84 c0                	test   %al,%al
    1090:	74 2a                	je     10bc <printf+0x16c>
    1092:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    1098:	88 45 e3             	mov    %al,-0x1d(%ebp)
  write(fd, &c, 1);
    109b:	8d 45 e3             	lea    -0x1d(%ebp),%eax
          s++;
    109e:	83 c7 01             	add    $0x1,%edi
  write(fd, &c, 1);
    10a1:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
    10a8:	00 
    10a9:	89 44 24 04          	mov    %eax,0x4(%esp)
    10ad:	89 34 24             	mov    %esi,(%esp)
    10b0:	e8 6d fd ff ff       	call   e22 <write>
        while(*s != 0){
    10b5:	0f b6 07             	movzbl (%edi),%eax
    10b8:	84 c0                	test   %al,%al
    10ba:	75 dc                	jne    1098 <printf+0x148>
      state = 0;
    10bc:	31 ff                	xor    %edi,%edi
    10be:	e9 d8 fe ff ff       	jmp    f9b <printf+0x4b>
    10c3:	90                   	nop
    10c4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  write(fd, &c, 1);
    10c8:	8d 45 e5             	lea    -0x1b(%ebp),%eax
      state = 0;
    10cb:	31 ff                	xor    %edi,%edi
  write(fd, &c, 1);
    10cd:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
    10d4:	00 
    10d5:	89 44 24 04          	mov    %eax,0x4(%esp)
    10d9:	89 34 24             	mov    %esi,(%esp)
    10dc:	c6 45 e5 25          	movb   $0x25,-0x1b(%ebp)
    10e0:	e8 3d fd ff ff       	call   e22 <write>
    10e5:	e9 b1 fe ff ff       	jmp    f9b <printf+0x4b>
    10ea:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        printint(fd, *ap, 10, 1);
    10f0:	8b 45 d4             	mov    -0x2c(%ebp),%eax
    10f3:	b9 0a 00 00 00       	mov    $0xa,%ecx
      state = 0;
    10f8:	66 31 ff             	xor    %di,%di
        printint(fd, *ap, 10, 1);
    10fb:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1102:	8b 10                	mov    (%eax),%edx
    1104:	89 f0                	mov    %esi,%eax
    1106:	e8 a5 fd ff ff       	call   eb0 <printint>
        ap++;
    110b:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
    110f:	e9 87 fe ff ff       	jmp    f9b <printf+0x4b>
        putc(fd, *ap);
    1114:	8b 45 d4             	mov    -0x2c(%ebp),%eax
      state = 0;
    1117:	31 ff                	xor    %edi,%edi
        putc(fd, *ap);
    1119:	8b 00                	mov    (%eax),%eax
  write(fd, &c, 1);
    111b:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
    1122:	00 
    1123:	89 34 24             	mov    %esi,(%esp)
        putc(fd, *ap);
    1126:	88 45 e4             	mov    %al,-0x1c(%ebp)
  write(fd, &c, 1);
    1129:	8d 45 e4             	lea    -0x1c(%ebp),%eax
    112c:	89 44 24 04          	mov    %eax,0x4(%esp)
    1130:	e8 ed fc ff ff       	call   e22 <write>
        ap++;
    1135:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
    1139:	e9 5d fe ff ff       	jmp    f9b <printf+0x4b>
    113e:	66 90                	xchg   %ax,%ax

00001140 <free>:
static Header base;
static Header *freep;

void
free(void *ap)
{
    1140:	55                   	push   %ebp
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    1141:	a1 c4 19 00 00       	mov    0x19c4,%eax
{
    1146:	89 e5                	mov    %esp,%ebp
    1148:	57                   	push   %edi
    1149:	56                   	push   %esi
    114a:	53                   	push   %ebx
    114b:	8b 5d 08             	mov    0x8(%ebp),%ebx
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
    114e:	8b 08                	mov    (%eax),%ecx
  bp = (Header*)ap - 1;
    1150:	8d 53 f8             	lea    -0x8(%ebx),%edx
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    1153:	39 d0                	cmp    %edx,%eax
    1155:	72 11                	jb     1168 <free+0x28>
    1157:	90                   	nop
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
    1158:	39 c8                	cmp    %ecx,%eax
    115a:	72 04                	jb     1160 <free+0x20>
    115c:	39 ca                	cmp    %ecx,%edx
    115e:	72 10                	jb     1170 <free+0x30>
    1160:	89 c8                	mov    %ecx,%eax
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    1162:	39 d0                	cmp    %edx,%eax
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
    1164:	8b 08                	mov    (%eax),%ecx
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    1166:	73 f0                	jae    1158 <free+0x18>
    1168:	39 ca                	cmp    %ecx,%edx
    116a:	72 04                	jb     1170 <free+0x30>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
    116c:	39 c8                	cmp    %ecx,%eax
    116e:	72 f0                	jb     1160 <free+0x20>
      break;
  if(bp + bp->s.size == p->s.ptr){
    1170:	8b 73 fc             	mov    -0x4(%ebx),%esi
    1173:	8d 3c f2             	lea    (%edx,%esi,8),%edi
    1176:	39 cf                	cmp    %ecx,%edi
    1178:	74 1e                	je     1198 <free+0x58>
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
    117a:	89 4b f8             	mov    %ecx,-0x8(%ebx)
  if(p + p->s.size == bp){
    117d:	8b 48 04             	mov    0x4(%eax),%ecx
    1180:	8d 34 c8             	lea    (%eax,%ecx,8),%esi
    1183:	39 f2                	cmp    %esi,%edx
    1185:	74 28                	je     11af <free+0x6f>
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
    1187:	89 10                	mov    %edx,(%eax)
  freep = p;
    1189:	a3 c4 19 00 00       	mov    %eax,0x19c4
}
    118e:	5b                   	pop    %ebx
    118f:	5e                   	pop    %esi
    1190:	5f                   	pop    %edi
    1191:	5d                   	pop    %ebp
    1192:	c3                   	ret    
    1193:	90                   	nop
    1194:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    bp->s.size += p->s.ptr->s.size;
    1198:	03 71 04             	add    0x4(%ecx),%esi
    119b:	89 73 fc             	mov    %esi,-0x4(%ebx)
    bp->s.ptr = p->s.ptr->s.ptr;
    119e:	8b 08                	mov    (%eax),%ecx
    11a0:	8b 09                	mov    (%ecx),%ecx
    11a2:	89 4b f8             	mov    %ecx,-0x8(%ebx)
  if(p + p->s.size == bp){
    11a5:	8b 48 04             	mov    0x4(%eax),%ecx
    11a8:	8d 34 c8             	lea    (%eax,%ecx,8),%esi
    11ab:	39 f2                	cmp    %esi,%edx
    11ad:	75 d8                	jne    1187 <free+0x47>
    p->s.size += bp->s.size;
    11af:	03 4b fc             	add    -0x4(%ebx),%ecx
  freep = p;
    11b2:	a3 c4 19 00 00       	mov    %eax,0x19c4
    p->s.size += bp->s.size;
    11b7:	89 48 04             	mov    %ecx,0x4(%eax)
    p->s.ptr = bp->s.ptr;
    11ba:	8b 53 f8             	mov    -0x8(%ebx),%edx
    11bd:	89 10                	mov    %edx,(%eax)
}
    11bf:	5b                   	pop    %ebx
    11c0:	5e                   	pop    %esi
    11c1:	5f                   	pop    %edi
    11c2:	5d                   	pop    %ebp
    11c3:	c3                   	ret    
    11c4:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    11ca:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

000011d0 <malloc>:
  return freep;
}

void*
malloc(uint nbytes)
{
    11d0:	55                   	push   %ebp
    11d1:	89 e5                	mov    %esp,%ebp
    11d3:	57                   	push   %edi
    11d4:	56                   	push   %esi
    11d5:	53                   	push   %ebx
    11d6:	83 ec 1c             	sub    $0x1c,%esp
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
    11d9:	8b 45 08             	mov    0x8(%ebp),%eax
  if((prevp = freep) == 0){
    11dc:	8b 1d c4 19 00 00    	mov    0x19c4,%ebx
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
    11e2:	8d 48 07             	lea    0x7(%eax),%ecx
    11e5:	c1 e9 03             	shr    $0x3,%ecx
  if((prevp = freep) == 0){
    11e8:	85 db                	test   %ebx,%ebx
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
    11ea:	8d 71 01             	lea    0x1(%ecx),%esi
  if((prevp = freep) == 0){
    11ed:	0f 84 9b 00 00 00    	je     128e <malloc+0xbe>
    11f3:	8b 13                	mov    (%ebx),%edx
    11f5:	8b 7a 04             	mov    0x4(%edx),%edi
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
    11f8:	39 fe                	cmp    %edi,%esi
    11fa:	76 64                	jbe    1260 <malloc+0x90>
    11fc:	8d 04 f5 00 00 00 00 	lea    0x0(,%esi,8),%eax
  if(nu < 4096)
    1203:	bb 00 80 00 00       	mov    $0x8000,%ebx
    1208:	89 45 e4             	mov    %eax,-0x1c(%ebp)
    120b:	eb 0e                	jmp    121b <malloc+0x4b>
    120d:	8d 76 00             	lea    0x0(%esi),%esi
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    1210:	8b 02                	mov    (%edx),%eax
    if(p->s.size >= nunits){
    1212:	8b 78 04             	mov    0x4(%eax),%edi
    1215:	39 fe                	cmp    %edi,%esi
    1217:	76 4f                	jbe    1268 <malloc+0x98>
    1219:	89 c2                	mov    %eax,%edx
        p->s.size = nunits;
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
    121b:	3b 15 c4 19 00 00    	cmp    0x19c4,%edx
    1221:	75 ed                	jne    1210 <malloc+0x40>
  if(nu < 4096)
    1223:	8b 45 e4             	mov    -0x1c(%ebp),%eax
    1226:	81 fe 00 10 00 00    	cmp    $0x1000,%esi
    122c:	bf 00 10 00 00       	mov    $0x1000,%edi
    1231:	0f 43 fe             	cmovae %esi,%edi
    1234:	0f 42 c3             	cmovb  %ebx,%eax
  p = sbrk(nu * sizeof(Header));
    1237:	89 04 24             	mov    %eax,(%esp)
    123a:	e8 4b fc ff ff       	call   e8a <sbrk>
  if(p == (char*)-1)
    123f:	83 f8 ff             	cmp    $0xffffffff,%eax
    1242:	74 18                	je     125c <malloc+0x8c>
  hp->s.size = nu;
    1244:	89 78 04             	mov    %edi,0x4(%eax)
  free((void*)(hp + 1));
    1247:	83 c0 08             	add    $0x8,%eax
    124a:	89 04 24             	mov    %eax,(%esp)
    124d:	e8 ee fe ff ff       	call   1140 <free>
  return freep;
    1252:	8b 15 c4 19 00 00    	mov    0x19c4,%edx
      if((p = morecore(nunits)) == 0)
    1258:	85 d2                	test   %edx,%edx
    125a:	75 b4                	jne    1210 <malloc+0x40>
        return 0;
    125c:	31 c0                	xor    %eax,%eax
    125e:	eb 20                	jmp    1280 <malloc+0xb0>
    if(p->s.size >= nunits){
    1260:	89 d0                	mov    %edx,%eax
    1262:	89 da                	mov    %ebx,%edx
    1264:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      if(p->s.size == nunits)
    1268:	39 fe                	cmp    %edi,%esi
    126a:	74 1c                	je     1288 <malloc+0xb8>
        p->s.size -= nunits;
    126c:	29 f7                	sub    %esi,%edi
    126e:	89 78 04             	mov    %edi,0x4(%eax)
        p += p->s.size;
    1271:	8d 04 f8             	lea    (%eax,%edi,8),%eax
        p->s.size = nunits;
    1274:	89 70 04             	mov    %esi,0x4(%eax)
      freep = prevp;
    1277:	89 15 c4 19 00 00    	mov    %edx,0x19c4
      return (void*)(p + 1);
    127d:	83 c0 08             	add    $0x8,%eax
  }
}
    1280:	83 c4 1c             	add    $0x1c,%esp
    1283:	5b                   	pop    %ebx
    1284:	5e                   	pop    %esi
    1285:	5f                   	pop    %edi
    1286:	5d                   	pop    %ebp
    1287:	c3                   	ret    
        prevp->s.ptr = p->s.ptr;
    1288:	8b 08                	mov    (%eax),%ecx
    128a:	89 0a                	mov    %ecx,(%edx)
    128c:	eb e9                	jmp    1277 <malloc+0xa7>
    base.s.ptr = freep = prevp = &base;
    128e:	c7 05 c4 19 00 00 c8 	movl   $0x19c8,0x19c4
    1295:	19 00 00 
    base.s.size = 0;
    1298:	ba c8 19 00 00       	mov    $0x19c8,%edx
    base.s.ptr = freep = prevp = &base;
    129d:	c7 05 c8 19 00 00 c8 	movl   $0x19c8,0x19c8
    12a4:	19 00 00 
    base.s.size = 0;
    12a7:	c7 05 cc 19 00 00 00 	movl   $0x0,0x19cc
    12ae:	00 00 00 
    12b1:	e9 46 ff ff ff       	jmp    11fc <malloc+0x2c>
