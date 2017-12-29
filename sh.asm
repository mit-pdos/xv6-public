
_sh：     檔案格式 elf32-i386


Disassembly of section .text:

00000000 <main>:
  return 0;
}

int
main(void)
{
       0:	8d 4c 24 04          	lea    0x4(%esp),%ecx
       4:	83 e4 f0             	and    $0xfffffff0,%esp
       7:	ff 71 fc             	pushl  -0x4(%ecx)
       a:	55                   	push   %ebp
       b:	89 e5                	mov    %esp,%ebp
       d:	51                   	push   %ecx
       e:	83 ec 04             	sub    $0x4,%esp
  static char buf[100];
  int fd;

  // Ensure that three file descriptors are open.
  while((fd = open("console", O_RDWR)) >= 0){
      11:	eb 0e                	jmp    21 <main+0x21>
      13:	90                   	nop
      14:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    if(fd >= 3){
      18:	83 f8 02             	cmp    $0x2,%eax
      1b:	0f 8f c3 00 00 00    	jg     e4 <main+0xe4>
{
  static char buf[100];
  int fd;

  // Ensure that three file descriptors are open.
  while((fd = open("console", O_RDWR)) >= 0){
      21:	83 ec 08             	sub    $0x8,%esp
      24:	6a 02                	push   $0x2
      26:	68 41 12 00 00       	push   $0x1241
      2b:	e8 32 0d 00 00       	call   d62 <open>
      30:	83 c4 10             	add    $0x10,%esp
      33:	85 c0                	test   %eax,%eax
      35:	79 e1                	jns    18 <main+0x18>
      37:	eb 2e                	jmp    67 <main+0x67>
      39:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
    }
  }

  // Read and run input commands.
  while(getcmd(buf, sizeof(buf)) >= 0){
    if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' '){
      40:	80 3d 62 18 00 00 20 	cmpb   $0x20,0x1862
      47:	74 5d                	je     a6 <main+0xa6>
      49:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
int
fork1(void)
{
  int pid;

  pid = fork();
      50:	e8 c5 0c 00 00       	call   d1a <fork>
  if(pid == -1)
      55:	83 f8 ff             	cmp    $0xffffffff,%eax
      58:	74 3f                	je     99 <main+0x99>
      buf[strlen(buf)-1] = 0;  // chop \n
      if(chdir(buf+3) < 0)
        printf(2, "cannot cd %s\n", buf+3);
      continue;
    }
    if(fork1() == 0)
      5a:	85 c0                	test   %eax,%eax
      5c:	0f 84 98 00 00 00    	je     fa <main+0xfa>
      runcmd(parsecmd(buf));
    wait();
      62:	e8 c3 0c 00 00       	call   d2a <wait>
      break;
    }
  }

  // Read and run input commands.
  while(getcmd(buf, sizeof(buf)) >= 0){
      67:	83 ec 08             	sub    $0x8,%esp
      6a:	6a 64                	push   $0x64
      6c:	68 60 18 00 00       	push   $0x1860
      71:	e8 9a 00 00 00       	call   110 <getcmd>
      76:	83 c4 10             	add    $0x10,%esp
      79:	85 c0                	test   %eax,%eax
      7b:	78 78                	js     f5 <main+0xf5>
    if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' '){
      7d:	80 3d 60 18 00 00 63 	cmpb   $0x63,0x1860
      84:	75 ca                	jne    50 <main+0x50>
      86:	80 3d 61 18 00 00 64 	cmpb   $0x64,0x1861
      8d:	74 b1                	je     40 <main+0x40>
int
fork1(void)
{
  int pid;

  pid = fork();
      8f:	e8 86 0c 00 00       	call   d1a <fork>
  if(pid == -1)
      94:	83 f8 ff             	cmp    $0xffffffff,%eax
      97:	75 c1                	jne    5a <main+0x5a>
    panic("fork");
      99:	83 ec 0c             	sub    $0xc,%esp
      9c:	68 ca 11 00 00       	push   $0x11ca
      a1:	e8 ba 00 00 00       	call   160 <panic>

  // Read and run input commands.
  while(getcmd(buf, sizeof(buf)) >= 0){
    if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' '){
      // Chdir must be called by the parent, not the child.
      buf[strlen(buf)-1] = 0;  // chop \n
      a6:	83 ec 0c             	sub    $0xc,%esp
      a9:	68 60 18 00 00       	push   $0x1860
      ae:	e8 ad 0a 00 00       	call   b60 <strlen>
      if(chdir(buf+3) < 0)
      b3:	c7 04 24 63 18 00 00 	movl   $0x1863,(%esp)

  // Read and run input commands.
  while(getcmd(buf, sizeof(buf)) >= 0){
    if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' '){
      // Chdir must be called by the parent, not the child.
      buf[strlen(buf)-1] = 0;  // chop \n
      ba:	c6 80 5f 18 00 00 00 	movb   $0x0,0x185f(%eax)
      if(chdir(buf+3) < 0)
      c1:	e8 cc 0c 00 00       	call   d92 <chdir>
      c6:	83 c4 10             	add    $0x10,%esp
      c9:	85 c0                	test   %eax,%eax
      cb:	79 9a                	jns    67 <main+0x67>
        printf(2, "cannot cd %s\n", buf+3);
      cd:	50                   	push   %eax
      ce:	68 63 18 00 00       	push   $0x1863
      d3:	68 49 12 00 00       	push   $0x1249
      d8:	6a 02                	push   $0x2
      da:	e8 a1 0d 00 00       	call   e80 <printf>
      df:	83 c4 10             	add    $0x10,%esp
      e2:	eb 83                	jmp    67 <main+0x67>
  int fd;

  // Ensure that three file descriptors are open.
  while((fd = open("console", O_RDWR)) >= 0){
    if(fd >= 3){
      close(fd);
      e4:	83 ec 0c             	sub    $0xc,%esp
      e7:	50                   	push   %eax
      e8:	e8 5d 0c 00 00       	call   d4a <close>
      break;
      ed:	83 c4 10             	add    $0x10,%esp
      f0:	e9 72 ff ff ff       	jmp    67 <main+0x67>
    }
    if(fork1() == 0)
      runcmd(parsecmd(buf));
    wait();
  }
  exit();
      f5:	e8 28 0c 00 00       	call   d22 <exit>
      if(chdir(buf+3) < 0)
        printf(2, "cannot cd %s\n", buf+3);
      continue;
    }
    if(fork1() == 0)
      runcmd(parsecmd(buf));
      fa:	83 ec 0c             	sub    $0xc,%esp
      fd:	68 60 18 00 00       	push   $0x1860
     102:	e8 69 09 00 00       	call   a70 <parsecmd>
     107:	89 04 24             	mov    %eax,(%esp)
     10a:	e8 71 00 00 00       	call   180 <runcmd>
     10f:	90                   	nop

00000110 <getcmd>:
  exit();
}

int
getcmd(char *buf, int nbuf)
{
     110:	55                   	push   %ebp
     111:	89 e5                	mov    %esp,%ebp
     113:	56                   	push   %esi
     114:	53                   	push   %ebx
     115:	8b 75 0c             	mov    0xc(%ebp),%esi
     118:	8b 5d 08             	mov    0x8(%ebp),%ebx
  printf(2, "$ ");
     11b:	83 ec 08             	sub    $0x8,%esp
     11e:	68 a0 11 00 00       	push   $0x11a0
     123:	6a 02                	push   $0x2
     125:	e8 56 0d 00 00       	call   e80 <printf>
  memset(buf, 0, nbuf);
     12a:	83 c4 0c             	add    $0xc,%esp
     12d:	56                   	push   %esi
     12e:	6a 00                	push   $0x0
     130:	53                   	push   %ebx
     131:	e8 5a 0a 00 00       	call   b90 <memset>
  gets(buf, nbuf);
     136:	58                   	pop    %eax
     137:	5a                   	pop    %edx
     138:	56                   	push   %esi
     139:	53                   	push   %ebx
     13a:	e8 b1 0a 00 00       	call   bf0 <gets>
     13f:	83 c4 10             	add    $0x10,%esp
     142:	31 c0                	xor    %eax,%eax
     144:	80 3b 00             	cmpb   $0x0,(%ebx)
     147:	0f 94 c0             	sete   %al
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}
     14a:	8d 65 f8             	lea    -0x8(%ebp),%esp
     14d:	f7 d8                	neg    %eax
     14f:	5b                   	pop    %ebx
     150:	5e                   	pop    %esi
     151:	5d                   	pop    %ebp
     152:	c3                   	ret    
     153:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
     159:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000160 <panic>:
  exit();
}

void
panic(char *s)
{
     160:	55                   	push   %ebp
     161:	89 e5                	mov    %esp,%ebp
     163:	83 ec 0c             	sub    $0xc,%esp
  printf(2, "%s\n", s);
     166:	ff 75 08             	pushl  0x8(%ebp)
     169:	68 3d 12 00 00       	push   $0x123d
     16e:	6a 02                	push   $0x2
     170:	e8 0b 0d 00 00       	call   e80 <printf>
  exit();
     175:	e8 a8 0b 00 00       	call   d22 <exit>
     17a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

00000180 <runcmd>:
struct cmd *parsecmd(char*);

// Execute cmd.  Never returns.
void
runcmd(struct cmd *cmd)
{
     180:	55                   	push   %ebp
     181:	89 e5                	mov    %esp,%ebp
     183:	53                   	push   %ebx
     184:	83 ec 14             	sub    $0x14,%esp
     187:	8b 5d 08             	mov    0x8(%ebp),%ebx
  struct execcmd *ecmd;
  struct listcmd *lcmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if(cmd == 0)
     18a:	85 db                	test   %ebx,%ebx
     18c:	74 76                	je     204 <runcmd+0x84>
    exit();

  switch(cmd->type){
     18e:	83 3b 05             	cmpl   $0x5,(%ebx)
     191:	0f 87 f8 00 00 00    	ja     28f <runcmd+0x10f>
     197:	8b 03                	mov    (%ebx),%eax
     199:	ff 24 85 58 12 00 00 	jmp    *0x1258(,%eax,4)
    runcmd(lcmd->right);
    break;

  case PIPE:
    pcmd = (struct pipecmd*)cmd;
    if(pipe(p) < 0)
     1a0:	8d 45 f0             	lea    -0x10(%ebp),%eax
     1a3:	83 ec 0c             	sub    $0xc,%esp
     1a6:	50                   	push   %eax
     1a7:	e8 86 0b 00 00       	call   d32 <pipe>
     1ac:	83 c4 10             	add    $0x10,%esp
     1af:	85 c0                	test   %eax,%eax
     1b1:	0f 88 07 01 00 00    	js     2be <runcmd+0x13e>
int
fork1(void)
{
  int pid;

  pid = fork();
     1b7:	e8 5e 0b 00 00       	call   d1a <fork>
  if(pid == -1)
     1bc:	83 f8 ff             	cmp    $0xffffffff,%eax
     1bf:	0f 84 d7 00 00 00    	je     29c <runcmd+0x11c>

  case PIPE:
    pcmd = (struct pipecmd*)cmd;
    if(pipe(p) < 0)
      panic("pipe");
    if(fork1() == 0){
     1c5:	85 c0                	test   %eax,%eax
     1c7:	0f 84 fe 00 00 00    	je     2cb <runcmd+0x14b>
int
fork1(void)
{
  int pid;

  pid = fork();
     1cd:	e8 48 0b 00 00       	call   d1a <fork>
  if(pid == -1)
     1d2:	83 f8 ff             	cmp    $0xffffffff,%eax
     1d5:	0f 84 c1 00 00 00    	je     29c <runcmd+0x11c>
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->left);
    }
    if(fork1() == 0){
     1db:	85 c0                	test   %eax,%eax
     1dd:	0f 84 16 01 00 00    	je     2f9 <runcmd+0x179>
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->right);
    }
    close(p[0]);
     1e3:	83 ec 0c             	sub    $0xc,%esp
     1e6:	ff 75 f0             	pushl  -0x10(%ebp)
     1e9:	e8 5c 0b 00 00       	call   d4a <close>
    close(p[1]);
     1ee:	58                   	pop    %eax
     1ef:	ff 75 f4             	pushl  -0xc(%ebp)
     1f2:	e8 53 0b 00 00       	call   d4a <close>
    wait();
     1f7:	e8 2e 0b 00 00       	call   d2a <wait>
    wait();
     1fc:	e8 29 0b 00 00       	call   d2a <wait>
    break;
     201:	83 c4 10             	add    $0x10,%esp
  struct listcmd *lcmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if(cmd == 0)
    exit();
     204:	e8 19 0b 00 00       	call   d22 <exit>
int
fork1(void)
{
  int pid;

  pid = fork();
     209:	e8 0c 0b 00 00       	call   d1a <fork>
  if(pid == -1)
     20e:	83 f8 ff             	cmp    $0xffffffff,%eax
     211:	0f 84 85 00 00 00    	je     29c <runcmd+0x11c>
    wait();
    break;

  case BACK:
    bcmd = (struct backcmd*)cmd;
    if(fork1() == 0)
     217:	85 c0                	test   %eax,%eax
     219:	75 e9                	jne    204 <runcmd+0x84>
     21b:	eb 49                	jmp    266 <runcmd+0xe6>
  default:
    panic("runcmd");

  case EXEC:
    ecmd = (struct execcmd*)cmd;
    if(ecmd->argv[0] == 0)
     21d:	8b 43 04             	mov    0x4(%ebx),%eax
     220:	85 c0                	test   %eax,%eax
     222:	74 e0                	je     204 <runcmd+0x84>
      exit();
    exec(ecmd->argv[0], ecmd->argv);
     224:	52                   	push   %edx
     225:	52                   	push   %edx
     226:	8d 53 04             	lea    0x4(%ebx),%edx
     229:	52                   	push   %edx
     22a:	50                   	push   %eax
     22b:	e8 2a 0b 00 00       	call   d5a <exec>
    printf(2, "exec %s failed\n", ecmd->argv[0]);
     230:	83 c4 0c             	add    $0xc,%esp
     233:	ff 73 04             	pushl  0x4(%ebx)
     236:	68 aa 11 00 00       	push   $0x11aa
     23b:	6a 02                	push   $0x2
     23d:	e8 3e 0c 00 00       	call   e80 <printf>
    break;
     242:	83 c4 10             	add    $0x10,%esp
     245:	eb bd                	jmp    204 <runcmd+0x84>

  case REDIR:
    rcmd = (struct redircmd*)cmd;
    close(rcmd->fd);
     247:	83 ec 0c             	sub    $0xc,%esp
     24a:	ff 73 14             	pushl  0x14(%ebx)
     24d:	e8 f8 0a 00 00       	call   d4a <close>
    if(open(rcmd->file, rcmd->mode) < 0){
     252:	59                   	pop    %ecx
     253:	58                   	pop    %eax
     254:	ff 73 10             	pushl  0x10(%ebx)
     257:	ff 73 08             	pushl  0x8(%ebx)
     25a:	e8 03 0b 00 00       	call   d62 <open>
     25f:	83 c4 10             	add    $0x10,%esp
     262:	85 c0                	test   %eax,%eax
     264:	78 43                	js     2a9 <runcmd+0x129>
    break;

  case BACK:
    bcmd = (struct backcmd*)cmd;
    if(fork1() == 0)
      runcmd(bcmd->cmd);
     266:	83 ec 0c             	sub    $0xc,%esp
     269:	ff 73 04             	pushl  0x4(%ebx)
     26c:	e8 0f ff ff ff       	call   180 <runcmd>
int
fork1(void)
{
  int pid;

  pid = fork();
     271:	e8 a4 0a 00 00       	call   d1a <fork>
  if(pid == -1)
     276:	83 f8 ff             	cmp    $0xffffffff,%eax
     279:	74 21                	je     29c <runcmd+0x11c>
    runcmd(rcmd->cmd);
    break;

  case LIST:
    lcmd = (struct listcmd*)cmd;
    if(fork1() == 0)
     27b:	85 c0                	test   %eax,%eax
     27d:	74 e7                	je     266 <runcmd+0xe6>
      runcmd(lcmd->left);
    wait();
     27f:	e8 a6 0a 00 00       	call   d2a <wait>
    runcmd(lcmd->right);
     284:	83 ec 0c             	sub    $0xc,%esp
     287:	ff 73 08             	pushl  0x8(%ebx)
     28a:	e8 f1 fe ff ff       	call   180 <runcmd>
  if(cmd == 0)
    exit();

  switch(cmd->type){
  default:
    panic("runcmd");
     28f:	83 ec 0c             	sub    $0xc,%esp
     292:	68 a3 11 00 00       	push   $0x11a3
     297:	e8 c4 fe ff ff       	call   160 <panic>
{
  int pid;

  pid = fork();
  if(pid == -1)
    panic("fork");
     29c:	83 ec 0c             	sub    $0xc,%esp
     29f:	68 ca 11 00 00       	push   $0x11ca
     2a4:	e8 b7 fe ff ff       	call   160 <panic>

  case REDIR:
    rcmd = (struct redircmd*)cmd;
    close(rcmd->fd);
    if(open(rcmd->file, rcmd->mode) < 0){
      printf(2, "open %s failed\n", rcmd->file);
     2a9:	52                   	push   %edx
     2aa:	ff 73 08             	pushl  0x8(%ebx)
     2ad:	68 ba 11 00 00       	push   $0x11ba
     2b2:	6a 02                	push   $0x2
     2b4:	e8 c7 0b 00 00       	call   e80 <printf>
      exit();
     2b9:	e8 64 0a 00 00       	call   d22 <exit>
    break;

  case PIPE:
    pcmd = (struct pipecmd*)cmd;
    if(pipe(p) < 0)
      panic("pipe");
     2be:	83 ec 0c             	sub    $0xc,%esp
     2c1:	68 cf 11 00 00       	push   $0x11cf
     2c6:	e8 95 fe ff ff       	call   160 <panic>
    if(fork1() == 0){
      close(1);
     2cb:	83 ec 0c             	sub    $0xc,%esp
     2ce:	6a 01                	push   $0x1
     2d0:	e8 75 0a 00 00       	call   d4a <close>
      dup(p[1]);
     2d5:	58                   	pop    %eax
     2d6:	ff 75 f4             	pushl  -0xc(%ebp)
     2d9:	e8 bc 0a 00 00       	call   d9a <dup>
      close(p[0]);
     2de:	58                   	pop    %eax
     2df:	ff 75 f0             	pushl  -0x10(%ebp)
     2e2:	e8 63 0a 00 00       	call   d4a <close>
      close(p[1]);
     2e7:	58                   	pop    %eax
     2e8:	ff 75 f4             	pushl  -0xc(%ebp)
     2eb:	e8 5a 0a 00 00       	call   d4a <close>
      runcmd(pcmd->left);
     2f0:	58                   	pop    %eax
     2f1:	ff 73 04             	pushl  0x4(%ebx)
     2f4:	e8 87 fe ff ff       	call   180 <runcmd>
    }
    if(fork1() == 0){
      close(0);
     2f9:	83 ec 0c             	sub    $0xc,%esp
     2fc:	6a 00                	push   $0x0
     2fe:	e8 47 0a 00 00       	call   d4a <close>
      dup(p[0]);
     303:	5a                   	pop    %edx
     304:	ff 75 f0             	pushl  -0x10(%ebp)
     307:	e8 8e 0a 00 00       	call   d9a <dup>
      close(p[0]);
     30c:	59                   	pop    %ecx
     30d:	ff 75 f0             	pushl  -0x10(%ebp)
     310:	e8 35 0a 00 00       	call   d4a <close>
      close(p[1]);
     315:	58                   	pop    %eax
     316:	ff 75 f4             	pushl  -0xc(%ebp)
     319:	e8 2c 0a 00 00       	call   d4a <close>
      runcmd(pcmd->right);
     31e:	58                   	pop    %eax
     31f:	ff 73 08             	pushl  0x8(%ebx)
     322:	e8 59 fe ff ff       	call   180 <runcmd>
     327:	89 f6                	mov    %esi,%esi
     329:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000330 <fork1>:
  exit();
}

int
fork1(void)
{
     330:	55                   	push   %ebp
     331:	89 e5                	mov    %esp,%ebp
     333:	83 ec 08             	sub    $0x8,%esp
  int pid;

  pid = fork();
     336:	e8 df 09 00 00       	call   d1a <fork>
  if(pid == -1)
     33b:	83 f8 ff             	cmp    $0xffffffff,%eax
     33e:	74 02                	je     342 <fork1+0x12>
    panic("fork");
  return pid;
}
     340:	c9                   	leave  
     341:	c3                   	ret    
{
  int pid;

  pid = fork();
  if(pid == -1)
    panic("fork");
     342:	83 ec 0c             	sub    $0xc,%esp
     345:	68 ca 11 00 00       	push   $0x11ca
     34a:	e8 11 fe ff ff       	call   160 <panic>
     34f:	90                   	nop

00000350 <execcmd>:
//PAGEBREAK!
// Constructors

struct cmd*
execcmd(void)
{
     350:	55                   	push   %ebp
     351:	89 e5                	mov    %esp,%ebp
     353:	53                   	push   %ebx
     354:	83 ec 10             	sub    $0x10,%esp
  struct execcmd *cmd;

  cmd = malloc(sizeof(*cmd));
     357:	6a 54                	push   $0x54
     359:	e8 52 0d 00 00       	call   10b0 <malloc>
  memset(cmd, 0, sizeof(*cmd));
     35e:	83 c4 0c             	add    $0xc,%esp
struct cmd*
execcmd(void)
{
  struct execcmd *cmd;

  cmd = malloc(sizeof(*cmd));
     361:	89 c3                	mov    %eax,%ebx
  memset(cmd, 0, sizeof(*cmd));
     363:	6a 54                	push   $0x54
     365:	6a 00                	push   $0x0
     367:	50                   	push   %eax
     368:	e8 23 08 00 00       	call   b90 <memset>
  cmd->type = EXEC;
     36d:	c7 03 01 00 00 00    	movl   $0x1,(%ebx)
  return (struct cmd*)cmd;
}
     373:	89 d8                	mov    %ebx,%eax
     375:	8b 5d fc             	mov    -0x4(%ebp),%ebx
     378:	c9                   	leave  
     379:	c3                   	ret    
     37a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

00000380 <redircmd>:

struct cmd*
redircmd(struct cmd *subcmd, char *file, char *efile, int mode, int fd)
{
     380:	55                   	push   %ebp
     381:	89 e5                	mov    %esp,%ebp
     383:	53                   	push   %ebx
     384:	83 ec 10             	sub    $0x10,%esp
  struct redircmd *cmd;

  cmd = malloc(sizeof(*cmd));
     387:	6a 18                	push   $0x18
     389:	e8 22 0d 00 00       	call   10b0 <malloc>
  memset(cmd, 0, sizeof(*cmd));
     38e:	83 c4 0c             	add    $0xc,%esp
struct cmd*
redircmd(struct cmd *subcmd, char *file, char *efile, int mode, int fd)
{
  struct redircmd *cmd;

  cmd = malloc(sizeof(*cmd));
     391:	89 c3                	mov    %eax,%ebx
  memset(cmd, 0, sizeof(*cmd));
     393:	6a 18                	push   $0x18
     395:	6a 00                	push   $0x0
     397:	50                   	push   %eax
     398:	e8 f3 07 00 00       	call   b90 <memset>
  cmd->type = REDIR;
  cmd->cmd = subcmd;
     39d:	8b 45 08             	mov    0x8(%ebp),%eax
{
  struct redircmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = REDIR;
     3a0:	c7 03 02 00 00 00    	movl   $0x2,(%ebx)
  cmd->cmd = subcmd;
     3a6:	89 43 04             	mov    %eax,0x4(%ebx)
  cmd->file = file;
     3a9:	8b 45 0c             	mov    0xc(%ebp),%eax
     3ac:	89 43 08             	mov    %eax,0x8(%ebx)
  cmd->efile = efile;
     3af:	8b 45 10             	mov    0x10(%ebp),%eax
     3b2:	89 43 0c             	mov    %eax,0xc(%ebx)
  cmd->mode = mode;
     3b5:	8b 45 14             	mov    0x14(%ebp),%eax
     3b8:	89 43 10             	mov    %eax,0x10(%ebx)
  cmd->fd = fd;
     3bb:	8b 45 18             	mov    0x18(%ebp),%eax
     3be:	89 43 14             	mov    %eax,0x14(%ebx)
  return (struct cmd*)cmd;
}
     3c1:	89 d8                	mov    %ebx,%eax
     3c3:	8b 5d fc             	mov    -0x4(%ebp),%ebx
     3c6:	c9                   	leave  
     3c7:	c3                   	ret    
     3c8:	90                   	nop
     3c9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

000003d0 <pipecmd>:

struct cmd*
pipecmd(struct cmd *left, struct cmd *right)
{
     3d0:	55                   	push   %ebp
     3d1:	89 e5                	mov    %esp,%ebp
     3d3:	53                   	push   %ebx
     3d4:	83 ec 10             	sub    $0x10,%esp
  struct pipecmd *cmd;

  cmd = malloc(sizeof(*cmd));
     3d7:	6a 0c                	push   $0xc
     3d9:	e8 d2 0c 00 00       	call   10b0 <malloc>
  memset(cmd, 0, sizeof(*cmd));
     3de:	83 c4 0c             	add    $0xc,%esp
struct cmd*
pipecmd(struct cmd *left, struct cmd *right)
{
  struct pipecmd *cmd;

  cmd = malloc(sizeof(*cmd));
     3e1:	89 c3                	mov    %eax,%ebx
  memset(cmd, 0, sizeof(*cmd));
     3e3:	6a 0c                	push   $0xc
     3e5:	6a 00                	push   $0x0
     3e7:	50                   	push   %eax
     3e8:	e8 a3 07 00 00       	call   b90 <memset>
  cmd->type = PIPE;
  cmd->left = left;
     3ed:	8b 45 08             	mov    0x8(%ebp),%eax
{
  struct pipecmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = PIPE;
     3f0:	c7 03 03 00 00 00    	movl   $0x3,(%ebx)
  cmd->left = left;
     3f6:	89 43 04             	mov    %eax,0x4(%ebx)
  cmd->right = right;
     3f9:	8b 45 0c             	mov    0xc(%ebp),%eax
     3fc:	89 43 08             	mov    %eax,0x8(%ebx)
  return (struct cmd*)cmd;
}
     3ff:	89 d8                	mov    %ebx,%eax
     401:	8b 5d fc             	mov    -0x4(%ebp),%ebx
     404:	c9                   	leave  
     405:	c3                   	ret    
     406:	8d 76 00             	lea    0x0(%esi),%esi
     409:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000410 <listcmd>:

struct cmd*
listcmd(struct cmd *left, struct cmd *right)
{
     410:	55                   	push   %ebp
     411:	89 e5                	mov    %esp,%ebp
     413:	53                   	push   %ebx
     414:	83 ec 10             	sub    $0x10,%esp
  struct listcmd *cmd;

  cmd = malloc(sizeof(*cmd));
     417:	6a 0c                	push   $0xc
     419:	e8 92 0c 00 00       	call   10b0 <malloc>
  memset(cmd, 0, sizeof(*cmd));
     41e:	83 c4 0c             	add    $0xc,%esp
struct cmd*
listcmd(struct cmd *left, struct cmd *right)
{
  struct listcmd *cmd;

  cmd = malloc(sizeof(*cmd));
     421:	89 c3                	mov    %eax,%ebx
  memset(cmd, 0, sizeof(*cmd));
     423:	6a 0c                	push   $0xc
     425:	6a 00                	push   $0x0
     427:	50                   	push   %eax
     428:	e8 63 07 00 00       	call   b90 <memset>
  cmd->type = LIST;
  cmd->left = left;
     42d:	8b 45 08             	mov    0x8(%ebp),%eax
{
  struct listcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = LIST;
     430:	c7 03 04 00 00 00    	movl   $0x4,(%ebx)
  cmd->left = left;
     436:	89 43 04             	mov    %eax,0x4(%ebx)
  cmd->right = right;
     439:	8b 45 0c             	mov    0xc(%ebp),%eax
     43c:	89 43 08             	mov    %eax,0x8(%ebx)
  return (struct cmd*)cmd;
}
     43f:	89 d8                	mov    %ebx,%eax
     441:	8b 5d fc             	mov    -0x4(%ebp),%ebx
     444:	c9                   	leave  
     445:	c3                   	ret    
     446:	8d 76 00             	lea    0x0(%esi),%esi
     449:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000450 <backcmd>:

struct cmd*
backcmd(struct cmd *subcmd)
{
     450:	55                   	push   %ebp
     451:	89 e5                	mov    %esp,%ebp
     453:	53                   	push   %ebx
     454:	83 ec 10             	sub    $0x10,%esp
  struct backcmd *cmd;

  cmd = malloc(sizeof(*cmd));
     457:	6a 08                	push   $0x8
     459:	e8 52 0c 00 00       	call   10b0 <malloc>
  memset(cmd, 0, sizeof(*cmd));
     45e:	83 c4 0c             	add    $0xc,%esp
struct cmd*
backcmd(struct cmd *subcmd)
{
  struct backcmd *cmd;

  cmd = malloc(sizeof(*cmd));
     461:	89 c3                	mov    %eax,%ebx
  memset(cmd, 0, sizeof(*cmd));
     463:	6a 08                	push   $0x8
     465:	6a 00                	push   $0x0
     467:	50                   	push   %eax
     468:	e8 23 07 00 00       	call   b90 <memset>
  cmd->type = BACK;
  cmd->cmd = subcmd;
     46d:	8b 45 08             	mov    0x8(%ebp),%eax
{
  struct backcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = BACK;
     470:	c7 03 05 00 00 00    	movl   $0x5,(%ebx)
  cmd->cmd = subcmd;
     476:	89 43 04             	mov    %eax,0x4(%ebx)
  return (struct cmd*)cmd;
}
     479:	89 d8                	mov    %ebx,%eax
     47b:	8b 5d fc             	mov    -0x4(%ebp),%ebx
     47e:	c9                   	leave  
     47f:	c3                   	ret    

00000480 <gettoken>:
char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>&;()";

int
gettoken(char **ps, char *es, char **q, char **eq)
{
     480:	55                   	push   %ebp
     481:	89 e5                	mov    %esp,%ebp
     483:	57                   	push   %edi
     484:	56                   	push   %esi
     485:	53                   	push   %ebx
     486:	83 ec 0c             	sub    $0xc,%esp
  char *s;
  int ret;

  s = *ps;
     489:	8b 45 08             	mov    0x8(%ebp),%eax
char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>&;()";

int
gettoken(char **ps, char *es, char **q, char **eq)
{
     48c:	8b 5d 0c             	mov    0xc(%ebp),%ebx
     48f:	8b 75 10             	mov    0x10(%ebp),%esi
  char *s;
  int ret;

  s = *ps;
     492:	8b 38                	mov    (%eax),%edi
  while(s < es && strchr(whitespace, *s))
     494:	39 df                	cmp    %ebx,%edi
     496:	72 13                	jb     4ab <gettoken+0x2b>
     498:	eb 29                	jmp    4c3 <gettoken+0x43>
     49a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    s++;
     4a0:	83 c7 01             	add    $0x1,%edi
{
  char *s;
  int ret;

  s = *ps;
  while(s < es && strchr(whitespace, *s))
     4a3:	39 fb                	cmp    %edi,%ebx
     4a5:	0f 84 ed 00 00 00    	je     598 <gettoken+0x118>
     4ab:	0f be 07             	movsbl (%edi),%eax
     4ae:	83 ec 08             	sub    $0x8,%esp
     4b1:	50                   	push   %eax
     4b2:	68 48 18 00 00       	push   $0x1848
     4b7:	e8 f4 06 00 00       	call   bb0 <strchr>
     4bc:	83 c4 10             	add    $0x10,%esp
     4bf:	85 c0                	test   %eax,%eax
     4c1:	75 dd                	jne    4a0 <gettoken+0x20>
    s++;
  if(q)
     4c3:	85 f6                	test   %esi,%esi
     4c5:	74 02                	je     4c9 <gettoken+0x49>
    *q = s;
     4c7:	89 3e                	mov    %edi,(%esi)
  ret = *s;
     4c9:	0f be 37             	movsbl (%edi),%esi
     4cc:	89 f1                	mov    %esi,%ecx
     4ce:	89 f0                	mov    %esi,%eax
  switch(*s){
     4d0:	80 f9 29             	cmp    $0x29,%cl
     4d3:	7f 5b                	jg     530 <gettoken+0xb0>
     4d5:	80 f9 28             	cmp    $0x28,%cl
     4d8:	7d 61                	jge    53b <gettoken+0xbb>
     4da:	84 c9                	test   %cl,%cl
     4dc:	0f 85 de 00 00 00    	jne    5c0 <gettoken+0x140>
    ret = 'a';
    while(s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    break;
  }
  if(eq)
     4e2:	8b 55 14             	mov    0x14(%ebp),%edx
     4e5:	85 d2                	test   %edx,%edx
     4e7:	74 05                	je     4ee <gettoken+0x6e>
    *eq = s;
     4e9:	8b 45 14             	mov    0x14(%ebp),%eax
     4ec:	89 38                	mov    %edi,(%eax)

  while(s < es && strchr(whitespace, *s))
     4ee:	39 fb                	cmp    %edi,%ebx
     4f0:	77 0d                	ja     4ff <gettoken+0x7f>
     4f2:	eb 23                	jmp    517 <gettoken+0x97>
     4f4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    s++;
     4f8:	83 c7 01             	add    $0x1,%edi
    break;
  }
  if(eq)
    *eq = s;

  while(s < es && strchr(whitespace, *s))
     4fb:	39 fb                	cmp    %edi,%ebx
     4fd:	74 18                	je     517 <gettoken+0x97>
     4ff:	0f be 07             	movsbl (%edi),%eax
     502:	83 ec 08             	sub    $0x8,%esp
     505:	50                   	push   %eax
     506:	68 48 18 00 00       	push   $0x1848
     50b:	e8 a0 06 00 00       	call   bb0 <strchr>
     510:	83 c4 10             	add    $0x10,%esp
     513:	85 c0                	test   %eax,%eax
     515:	75 e1                	jne    4f8 <gettoken+0x78>
    s++;
  *ps = s;
     517:	8b 45 08             	mov    0x8(%ebp),%eax
     51a:	89 38                	mov    %edi,(%eax)
  return ret;
}
     51c:	8d 65 f4             	lea    -0xc(%ebp),%esp
     51f:	89 f0                	mov    %esi,%eax
     521:	5b                   	pop    %ebx
     522:	5e                   	pop    %esi
     523:	5f                   	pop    %edi
     524:	5d                   	pop    %ebp
     525:	c3                   	ret    
     526:	8d 76 00             	lea    0x0(%esi),%esi
     529:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
  while(s < es && strchr(whitespace, *s))
    s++;
  if(q)
    *q = s;
  ret = *s;
  switch(*s){
     530:	80 f9 3e             	cmp    $0x3e,%cl
     533:	75 0b                	jne    540 <gettoken+0xc0>
  case '<':
    s++;
    break;
  case '>':
    s++;
    if(*s == '>'){
     535:	80 7f 01 3e          	cmpb   $0x3e,0x1(%edi)
     539:	74 75                	je     5b0 <gettoken+0x130>
  case '&':
  case '<':
    s++;
    break;
  case '>':
    s++;
     53b:	83 c7 01             	add    $0x1,%edi
     53e:	eb a2                	jmp    4e2 <gettoken+0x62>
  while(s < es && strchr(whitespace, *s))
    s++;
  if(q)
    *q = s;
  ret = *s;
  switch(*s){
     540:	7f 5e                	jg     5a0 <gettoken+0x120>
     542:	83 e9 3b             	sub    $0x3b,%ecx
     545:	80 f9 01             	cmp    $0x1,%cl
     548:	76 f1                	jbe    53b <gettoken+0xbb>
      s++;
    }
    break;
  default:
    ret = 'a';
    while(s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
     54a:	39 fb                	cmp    %edi,%ebx
     54c:	77 24                	ja     572 <gettoken+0xf2>
     54e:	eb 7c                	jmp    5cc <gettoken+0x14c>
     550:	0f be 07             	movsbl (%edi),%eax
     553:	83 ec 08             	sub    $0x8,%esp
     556:	50                   	push   %eax
     557:	68 40 18 00 00       	push   $0x1840
     55c:	e8 4f 06 00 00       	call   bb0 <strchr>
     561:	83 c4 10             	add    $0x10,%esp
     564:	85 c0                	test   %eax,%eax
     566:	75 1f                	jne    587 <gettoken+0x107>
      s++;
     568:	83 c7 01             	add    $0x1,%edi
      s++;
    }
    break;
  default:
    ret = 'a';
    while(s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
     56b:	39 fb                	cmp    %edi,%ebx
     56d:	74 5b                	je     5ca <gettoken+0x14a>
     56f:	0f be 07             	movsbl (%edi),%eax
     572:	83 ec 08             	sub    $0x8,%esp
     575:	50                   	push   %eax
     576:	68 48 18 00 00       	push   $0x1848
     57b:	e8 30 06 00 00       	call   bb0 <strchr>
     580:	83 c4 10             	add    $0x10,%esp
     583:	85 c0                	test   %eax,%eax
     585:	74 c9                	je     550 <gettoken+0xd0>
      ret = '+';
      s++;
    }
    break;
  default:
    ret = 'a';
     587:	be 61 00 00 00       	mov    $0x61,%esi
     58c:	e9 51 ff ff ff       	jmp    4e2 <gettoken+0x62>
     591:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
     598:	89 df                	mov    %ebx,%edi
     59a:	e9 24 ff ff ff       	jmp    4c3 <gettoken+0x43>
     59f:	90                   	nop
  while(s < es && strchr(whitespace, *s))
    s++;
  if(q)
    *q = s;
  ret = *s;
  switch(*s){
     5a0:	80 f9 7c             	cmp    $0x7c,%cl
     5a3:	74 96                	je     53b <gettoken+0xbb>
     5a5:	eb a3                	jmp    54a <gettoken+0xca>
     5a7:	89 f6                	mov    %esi,%esi
     5a9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
    break;
  case '>':
    s++;
    if(*s == '>'){
      ret = '+';
      s++;
     5b0:	83 c7 02             	add    $0x2,%edi
    s++;
    break;
  case '>':
    s++;
    if(*s == '>'){
      ret = '+';
     5b3:	be 2b 00 00 00       	mov    $0x2b,%esi
     5b8:	e9 25 ff ff ff       	jmp    4e2 <gettoken+0x62>
     5bd:	8d 76 00             	lea    0x0(%esi),%esi
  while(s < es && strchr(whitespace, *s))
    s++;
  if(q)
    *q = s;
  ret = *s;
  switch(*s){
     5c0:	80 f9 26             	cmp    $0x26,%cl
     5c3:	75 85                	jne    54a <gettoken+0xca>
     5c5:	e9 71 ff ff ff       	jmp    53b <gettoken+0xbb>
     5ca:	89 df                	mov    %ebx,%edi
    ret = 'a';
    while(s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    break;
  }
  if(eq)
     5cc:	8b 45 14             	mov    0x14(%ebp),%eax
     5cf:	be 61 00 00 00       	mov    $0x61,%esi
     5d4:	85 c0                	test   %eax,%eax
     5d6:	0f 85 0d ff ff ff    	jne    4e9 <gettoken+0x69>
     5dc:	e9 36 ff ff ff       	jmp    517 <gettoken+0x97>
     5e1:	eb 0d                	jmp    5f0 <peek>
     5e3:	90                   	nop
     5e4:	90                   	nop
     5e5:	90                   	nop
     5e6:	90                   	nop
     5e7:	90                   	nop
     5e8:	90                   	nop
     5e9:	90                   	nop
     5ea:	90                   	nop
     5eb:	90                   	nop
     5ec:	90                   	nop
     5ed:	90                   	nop
     5ee:	90                   	nop
     5ef:	90                   	nop

000005f0 <peek>:
  return ret;
}

int
peek(char **ps, char *es, char *toks)
{
     5f0:	55                   	push   %ebp
     5f1:	89 e5                	mov    %esp,%ebp
     5f3:	57                   	push   %edi
     5f4:	56                   	push   %esi
     5f5:	53                   	push   %ebx
     5f6:	83 ec 0c             	sub    $0xc,%esp
     5f9:	8b 7d 08             	mov    0x8(%ebp),%edi
     5fc:	8b 75 0c             	mov    0xc(%ebp),%esi
  char *s;

  s = *ps;
     5ff:	8b 1f                	mov    (%edi),%ebx
  while(s < es && strchr(whitespace, *s))
     601:	39 f3                	cmp    %esi,%ebx
     603:	72 12                	jb     617 <peek+0x27>
     605:	eb 28                	jmp    62f <peek+0x3f>
     607:	89 f6                	mov    %esi,%esi
     609:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
    s++;
     610:	83 c3 01             	add    $0x1,%ebx
peek(char **ps, char *es, char *toks)
{
  char *s;

  s = *ps;
  while(s < es && strchr(whitespace, *s))
     613:	39 de                	cmp    %ebx,%esi
     615:	74 18                	je     62f <peek+0x3f>
     617:	0f be 03             	movsbl (%ebx),%eax
     61a:	83 ec 08             	sub    $0x8,%esp
     61d:	50                   	push   %eax
     61e:	68 48 18 00 00       	push   $0x1848
     623:	e8 88 05 00 00       	call   bb0 <strchr>
     628:	83 c4 10             	add    $0x10,%esp
     62b:	85 c0                	test   %eax,%eax
     62d:	75 e1                	jne    610 <peek+0x20>
    s++;
  *ps = s;
     62f:	89 1f                	mov    %ebx,(%edi)
  return *s && strchr(toks, *s);
     631:	0f be 13             	movsbl (%ebx),%edx
     634:	31 c0                	xor    %eax,%eax
     636:	84 d2                	test   %dl,%dl
     638:	74 17                	je     651 <peek+0x61>
     63a:	83 ec 08             	sub    $0x8,%esp
     63d:	52                   	push   %edx
     63e:	ff 75 10             	pushl  0x10(%ebp)
     641:	e8 6a 05 00 00       	call   bb0 <strchr>
     646:	83 c4 10             	add    $0x10,%esp
     649:	85 c0                	test   %eax,%eax
     64b:	0f 95 c0             	setne  %al
     64e:	0f b6 c0             	movzbl %al,%eax
}
     651:	8d 65 f4             	lea    -0xc(%ebp),%esp
     654:	5b                   	pop    %ebx
     655:	5e                   	pop    %esi
     656:	5f                   	pop    %edi
     657:	5d                   	pop    %ebp
     658:	c3                   	ret    
     659:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00000660 <parseredirs>:
  return cmd;
}

struct cmd*
parseredirs(struct cmd *cmd, char **ps, char *es)
{
     660:	55                   	push   %ebp
     661:	89 e5                	mov    %esp,%ebp
     663:	57                   	push   %edi
     664:	56                   	push   %esi
     665:	53                   	push   %ebx
     666:	83 ec 1c             	sub    $0x1c,%esp
     669:	8b 75 0c             	mov    0xc(%ebp),%esi
     66c:	8b 5d 10             	mov    0x10(%ebp),%ebx
     66f:	90                   	nop
  int tok;
  char *q, *eq;

  while(peek(ps, es, "<>")){
     670:	83 ec 04             	sub    $0x4,%esp
     673:	68 f1 11 00 00       	push   $0x11f1
     678:	53                   	push   %ebx
     679:	56                   	push   %esi
     67a:	e8 71 ff ff ff       	call   5f0 <peek>
     67f:	83 c4 10             	add    $0x10,%esp
     682:	85 c0                	test   %eax,%eax
     684:	74 6a                	je     6f0 <parseredirs+0x90>
    tok = gettoken(ps, es, 0, 0);
     686:	6a 00                	push   $0x0
     688:	6a 00                	push   $0x0
     68a:	53                   	push   %ebx
     68b:	56                   	push   %esi
     68c:	e8 ef fd ff ff       	call   480 <gettoken>
     691:	89 c7                	mov    %eax,%edi
    if(gettoken(ps, es, &q, &eq) != 'a')
     693:	8d 45 e4             	lea    -0x1c(%ebp),%eax
     696:	50                   	push   %eax
     697:	8d 45 e0             	lea    -0x20(%ebp),%eax
     69a:	50                   	push   %eax
     69b:	53                   	push   %ebx
     69c:	56                   	push   %esi
     69d:	e8 de fd ff ff       	call   480 <gettoken>
     6a2:	83 c4 20             	add    $0x20,%esp
     6a5:	83 f8 61             	cmp    $0x61,%eax
     6a8:	75 51                	jne    6fb <parseredirs+0x9b>
      panic("missing file for redirection");
    switch(tok){
     6aa:	83 ff 3c             	cmp    $0x3c,%edi
     6ad:	74 31                	je     6e0 <parseredirs+0x80>
     6af:	83 ff 3e             	cmp    $0x3e,%edi
     6b2:	74 05                	je     6b9 <parseredirs+0x59>
     6b4:	83 ff 2b             	cmp    $0x2b,%edi
     6b7:	75 b7                	jne    670 <parseredirs+0x10>
      break;
    case '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY|O_CREATE, 1);
      break;
    case '+':  // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY|O_CREATE, 1);
     6b9:	83 ec 0c             	sub    $0xc,%esp
     6bc:	6a 01                	push   $0x1
     6be:	68 01 02 00 00       	push   $0x201
     6c3:	ff 75 e4             	pushl  -0x1c(%ebp)
     6c6:	ff 75 e0             	pushl  -0x20(%ebp)
     6c9:	ff 75 08             	pushl  0x8(%ebp)
     6cc:	e8 af fc ff ff       	call   380 <redircmd>
      break;
     6d1:	83 c4 20             	add    $0x20,%esp
      break;
    case '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY|O_CREATE, 1);
      break;
    case '+':  // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY|O_CREATE, 1);
     6d4:	89 45 08             	mov    %eax,0x8(%ebp)
      break;
     6d7:	eb 97                	jmp    670 <parseredirs+0x10>
     6d9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
    tok = gettoken(ps, es, 0, 0);
    if(gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    switch(tok){
    case '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
     6e0:	83 ec 0c             	sub    $0xc,%esp
     6e3:	6a 00                	push   $0x0
     6e5:	6a 00                	push   $0x0
     6e7:	eb da                	jmp    6c3 <parseredirs+0x63>
     6e9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
      cmd = redircmd(cmd, q, eq, O_WRONLY|O_CREATE, 1);
      break;
    }
  }
  return cmd;
}
     6f0:	8b 45 08             	mov    0x8(%ebp),%eax
     6f3:	8d 65 f4             	lea    -0xc(%ebp),%esp
     6f6:	5b                   	pop    %ebx
     6f7:	5e                   	pop    %esi
     6f8:	5f                   	pop    %edi
     6f9:	5d                   	pop    %ebp
     6fa:	c3                   	ret    
  char *q, *eq;

  while(peek(ps, es, "<>")){
    tok = gettoken(ps, es, 0, 0);
    if(gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
     6fb:	83 ec 0c             	sub    $0xc,%esp
     6fe:	68 d4 11 00 00       	push   $0x11d4
     703:	e8 58 fa ff ff       	call   160 <panic>
     708:	90                   	nop
     709:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00000710 <parseexec>:
  return cmd;
}

struct cmd*
parseexec(char **ps, char *es)
{
     710:	55                   	push   %ebp
     711:	89 e5                	mov    %esp,%ebp
     713:	57                   	push   %edi
     714:	56                   	push   %esi
     715:	53                   	push   %ebx
     716:	83 ec 30             	sub    $0x30,%esp
     719:	8b 75 08             	mov    0x8(%ebp),%esi
     71c:	8b 7d 0c             	mov    0xc(%ebp),%edi
  char *q, *eq;
  int tok, argc;
  struct execcmd *cmd;
  struct cmd *ret;

  if(peek(ps, es, "("))
     71f:	68 f4 11 00 00       	push   $0x11f4
     724:	57                   	push   %edi
     725:	56                   	push   %esi
     726:	e8 c5 fe ff ff       	call   5f0 <peek>
     72b:	83 c4 10             	add    $0x10,%esp
     72e:	85 c0                	test   %eax,%eax
     730:	0f 85 9a 00 00 00    	jne    7d0 <parseexec+0xc0>
    return parseblock(ps, es);

  ret = execcmd();
     736:	e8 15 fc ff ff       	call   350 <execcmd>
  cmd = (struct execcmd*)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
     73b:	83 ec 04             	sub    $0x4,%esp
  struct cmd *ret;

  if(peek(ps, es, "("))
    return parseblock(ps, es);

  ret = execcmd();
     73e:	89 c3                	mov    %eax,%ebx
     740:	89 45 cc             	mov    %eax,-0x34(%ebp)
  cmd = (struct execcmd*)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
     743:	57                   	push   %edi
     744:	56                   	push   %esi
     745:	8d 5b 04             	lea    0x4(%ebx),%ebx
     748:	50                   	push   %eax
     749:	e8 12 ff ff ff       	call   660 <parseredirs>
     74e:	83 c4 10             	add    $0x10,%esp
     751:	89 45 d0             	mov    %eax,-0x30(%ebp)
    return parseblock(ps, es);

  ret = execcmd();
  cmd = (struct execcmd*)ret;

  argc = 0;
     754:	c7 45 d4 00 00 00 00 	movl   $0x0,-0x2c(%ebp)
     75b:	eb 16                	jmp    773 <parseexec+0x63>
     75d:	8d 76 00             	lea    0x0(%esi),%esi
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    if(argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
     760:	83 ec 04             	sub    $0x4,%esp
     763:	57                   	push   %edi
     764:	56                   	push   %esi
     765:	ff 75 d0             	pushl  -0x30(%ebp)
     768:	e8 f3 fe ff ff       	call   660 <parseredirs>
     76d:	83 c4 10             	add    $0x10,%esp
     770:	89 45 d0             	mov    %eax,-0x30(%ebp)
  ret = execcmd();
  cmd = (struct execcmd*)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  while(!peek(ps, es, "|)&;")){
     773:	83 ec 04             	sub    $0x4,%esp
     776:	68 0b 12 00 00       	push   $0x120b
     77b:	57                   	push   %edi
     77c:	56                   	push   %esi
     77d:	e8 6e fe ff ff       	call   5f0 <peek>
     782:	83 c4 10             	add    $0x10,%esp
     785:	85 c0                	test   %eax,%eax
     787:	75 5f                	jne    7e8 <parseexec+0xd8>
    if((tok=gettoken(ps, es, &q, &eq)) == 0)
     789:	8d 45 e4             	lea    -0x1c(%ebp),%eax
     78c:	50                   	push   %eax
     78d:	8d 45 e0             	lea    -0x20(%ebp),%eax
     790:	50                   	push   %eax
     791:	57                   	push   %edi
     792:	56                   	push   %esi
     793:	e8 e8 fc ff ff       	call   480 <gettoken>
     798:	83 c4 10             	add    $0x10,%esp
     79b:	85 c0                	test   %eax,%eax
     79d:	74 49                	je     7e8 <parseexec+0xd8>
      break;
    if(tok != 'a')
     79f:	83 f8 61             	cmp    $0x61,%eax
     7a2:	75 66                	jne    80a <parseexec+0xfa>
      panic("syntax");
    cmd->argv[argc] = q;
     7a4:	8b 45 e0             	mov    -0x20(%ebp),%eax
    cmd->eargv[argc] = eq;
    argc++;
     7a7:	83 45 d4 01          	addl   $0x1,-0x2c(%ebp)
     7ab:	83 c3 04             	add    $0x4,%ebx
  while(!peek(ps, es, "|)&;")){
    if((tok=gettoken(ps, es, &q, &eq)) == 0)
      break;
    if(tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
     7ae:	89 43 fc             	mov    %eax,-0x4(%ebx)
    cmd->eargv[argc] = eq;
     7b1:	8b 45 e4             	mov    -0x1c(%ebp),%eax
     7b4:	89 43 24             	mov    %eax,0x24(%ebx)
    argc++;
     7b7:	8b 45 d4             	mov    -0x2c(%ebp),%eax
    if(argc >= MAXARGS)
     7ba:	83 f8 0a             	cmp    $0xa,%eax
     7bd:	75 a1                	jne    760 <parseexec+0x50>
      panic("too many args");
     7bf:	83 ec 0c             	sub    $0xc,%esp
     7c2:	68 fd 11 00 00       	push   $0x11fd
     7c7:	e8 94 f9 ff ff       	call   160 <panic>
     7cc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  int tok, argc;
  struct execcmd *cmd;
  struct cmd *ret;

  if(peek(ps, es, "("))
    return parseblock(ps, es);
     7d0:	83 ec 08             	sub    $0x8,%esp
     7d3:	57                   	push   %edi
     7d4:	56                   	push   %esi
     7d5:	e8 56 01 00 00       	call   930 <parseblock>
     7da:	83 c4 10             	add    $0x10,%esp
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  return ret;
}
     7dd:	8d 65 f4             	lea    -0xc(%ebp),%esp
     7e0:	5b                   	pop    %ebx
     7e1:	5e                   	pop    %esi
     7e2:	5f                   	pop    %edi
     7e3:	5d                   	pop    %ebp
     7e4:	c3                   	ret    
     7e5:	8d 76 00             	lea    0x0(%esi),%esi
     7e8:	8b 45 cc             	mov    -0x34(%ebp),%eax
     7eb:	8b 55 d4             	mov    -0x2c(%ebp),%edx
     7ee:	8d 04 90             	lea    (%eax,%edx,4),%eax
    argc++;
    if(argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
     7f1:	c7 40 04 00 00 00 00 	movl   $0x0,0x4(%eax)
  cmd->eargv[argc] = 0;
     7f8:	c7 40 2c 00 00 00 00 	movl   $0x0,0x2c(%eax)
     7ff:	8b 45 d0             	mov    -0x30(%ebp),%eax
  return ret;
}
     802:	8d 65 f4             	lea    -0xc(%ebp),%esp
     805:	5b                   	pop    %ebx
     806:	5e                   	pop    %esi
     807:	5f                   	pop    %edi
     808:	5d                   	pop    %ebp
     809:	c3                   	ret    
  ret = parseredirs(ret, ps, es);
  while(!peek(ps, es, "|)&;")){
    if((tok=gettoken(ps, es, &q, &eq)) == 0)
      break;
    if(tok != 'a')
      panic("syntax");
     80a:	83 ec 0c             	sub    $0xc,%esp
     80d:	68 f6 11 00 00       	push   $0x11f6
     812:	e8 49 f9 ff ff       	call   160 <panic>
     817:	89 f6                	mov    %esi,%esi
     819:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000820 <parsepipe>:
  return cmd;
}

struct cmd*
parsepipe(char **ps, char *es)
{
     820:	55                   	push   %ebp
     821:	89 e5                	mov    %esp,%ebp
     823:	57                   	push   %edi
     824:	56                   	push   %esi
     825:	53                   	push   %ebx
     826:	83 ec 14             	sub    $0x14,%esp
     829:	8b 5d 08             	mov    0x8(%ebp),%ebx
     82c:	8b 75 0c             	mov    0xc(%ebp),%esi
  struct cmd *cmd;

  cmd = parseexec(ps, es);
     82f:	56                   	push   %esi
     830:	53                   	push   %ebx
     831:	e8 da fe ff ff       	call   710 <parseexec>
  if(peek(ps, es, "|")){
     836:	83 c4 0c             	add    $0xc,%esp
struct cmd*
parsepipe(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parseexec(ps, es);
     839:	89 c7                	mov    %eax,%edi
  if(peek(ps, es, "|")){
     83b:	68 10 12 00 00       	push   $0x1210
     840:	56                   	push   %esi
     841:	53                   	push   %ebx
     842:	e8 a9 fd ff ff       	call   5f0 <peek>
     847:	83 c4 10             	add    $0x10,%esp
     84a:	85 c0                	test   %eax,%eax
     84c:	75 12                	jne    860 <parsepipe+0x40>
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  return cmd;
}
     84e:	8d 65 f4             	lea    -0xc(%ebp),%esp
     851:	89 f8                	mov    %edi,%eax
     853:	5b                   	pop    %ebx
     854:	5e                   	pop    %esi
     855:	5f                   	pop    %edi
     856:	5d                   	pop    %ebp
     857:	c3                   	ret    
     858:	90                   	nop
     859:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
{
  struct cmd *cmd;

  cmd = parseexec(ps, es);
  if(peek(ps, es, "|")){
    gettoken(ps, es, 0, 0);
     860:	6a 00                	push   $0x0
     862:	6a 00                	push   $0x0
     864:	56                   	push   %esi
     865:	53                   	push   %ebx
     866:	e8 15 fc ff ff       	call   480 <gettoken>
    cmd = pipecmd(cmd, parsepipe(ps, es));
     86b:	58                   	pop    %eax
     86c:	5a                   	pop    %edx
     86d:	56                   	push   %esi
     86e:	53                   	push   %ebx
     86f:	e8 ac ff ff ff       	call   820 <parsepipe>
     874:	89 7d 08             	mov    %edi,0x8(%ebp)
     877:	89 45 0c             	mov    %eax,0xc(%ebp)
     87a:	83 c4 10             	add    $0x10,%esp
  }
  return cmd;
}
     87d:	8d 65 f4             	lea    -0xc(%ebp),%esp
     880:	5b                   	pop    %ebx
     881:	5e                   	pop    %esi
     882:	5f                   	pop    %edi
     883:	5d                   	pop    %ebp
  struct cmd *cmd;

  cmd = parseexec(ps, es);
  if(peek(ps, es, "|")){
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
     884:	e9 47 fb ff ff       	jmp    3d0 <pipecmd>
     889:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00000890 <parseline>:
  return cmd;
}

struct cmd*
parseline(char **ps, char *es)
{
     890:	55                   	push   %ebp
     891:	89 e5                	mov    %esp,%ebp
     893:	57                   	push   %edi
     894:	56                   	push   %esi
     895:	53                   	push   %ebx
     896:	83 ec 14             	sub    $0x14,%esp
     899:	8b 5d 08             	mov    0x8(%ebp),%ebx
     89c:	8b 75 0c             	mov    0xc(%ebp),%esi
  struct cmd *cmd;

  cmd = parsepipe(ps, es);
     89f:	56                   	push   %esi
     8a0:	53                   	push   %ebx
     8a1:	e8 7a ff ff ff       	call   820 <parsepipe>
  while(peek(ps, es, "&")){
     8a6:	83 c4 10             	add    $0x10,%esp
struct cmd*
parseline(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parsepipe(ps, es);
     8a9:	89 c7                	mov    %eax,%edi
  while(peek(ps, es, "&")){
     8ab:	eb 1b                	jmp    8c8 <parseline+0x38>
     8ad:	8d 76 00             	lea    0x0(%esi),%esi
    gettoken(ps, es, 0, 0);
     8b0:	6a 00                	push   $0x0
     8b2:	6a 00                	push   $0x0
     8b4:	56                   	push   %esi
     8b5:	53                   	push   %ebx
     8b6:	e8 c5 fb ff ff       	call   480 <gettoken>
    cmd = backcmd(cmd);
     8bb:	89 3c 24             	mov    %edi,(%esp)
     8be:	e8 8d fb ff ff       	call   450 <backcmd>
     8c3:	83 c4 10             	add    $0x10,%esp
     8c6:	89 c7                	mov    %eax,%edi
parseline(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parsepipe(ps, es);
  while(peek(ps, es, "&")){
     8c8:	83 ec 04             	sub    $0x4,%esp
     8cb:	68 12 12 00 00       	push   $0x1212
     8d0:	56                   	push   %esi
     8d1:	53                   	push   %ebx
     8d2:	e8 19 fd ff ff       	call   5f0 <peek>
     8d7:	83 c4 10             	add    $0x10,%esp
     8da:	85 c0                	test   %eax,%eax
     8dc:	75 d2                	jne    8b0 <parseline+0x20>
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  if(peek(ps, es, ";")){
     8de:	83 ec 04             	sub    $0x4,%esp
     8e1:	68 0e 12 00 00       	push   $0x120e
     8e6:	56                   	push   %esi
     8e7:	53                   	push   %ebx
     8e8:	e8 03 fd ff ff       	call   5f0 <peek>
     8ed:	83 c4 10             	add    $0x10,%esp
     8f0:	85 c0                	test   %eax,%eax
     8f2:	75 0c                	jne    900 <parseline+0x70>
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  return cmd;
}
     8f4:	8d 65 f4             	lea    -0xc(%ebp),%esp
     8f7:	89 f8                	mov    %edi,%eax
     8f9:	5b                   	pop    %ebx
     8fa:	5e                   	pop    %esi
     8fb:	5f                   	pop    %edi
     8fc:	5d                   	pop    %ebp
     8fd:	c3                   	ret    
     8fe:	66 90                	xchg   %ax,%ax
  while(peek(ps, es, "&")){
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  if(peek(ps, es, ";")){
    gettoken(ps, es, 0, 0);
     900:	6a 00                	push   $0x0
     902:	6a 00                	push   $0x0
     904:	56                   	push   %esi
     905:	53                   	push   %ebx
     906:	e8 75 fb ff ff       	call   480 <gettoken>
    cmd = listcmd(cmd, parseline(ps, es));
     90b:	58                   	pop    %eax
     90c:	5a                   	pop    %edx
     90d:	56                   	push   %esi
     90e:	53                   	push   %ebx
     90f:	e8 7c ff ff ff       	call   890 <parseline>
     914:	89 7d 08             	mov    %edi,0x8(%ebp)
     917:	89 45 0c             	mov    %eax,0xc(%ebp)
     91a:	83 c4 10             	add    $0x10,%esp
  }
  return cmd;
}
     91d:	8d 65 f4             	lea    -0xc(%ebp),%esp
     920:	5b                   	pop    %ebx
     921:	5e                   	pop    %esi
     922:	5f                   	pop    %edi
     923:	5d                   	pop    %ebp
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  if(peek(ps, es, ";")){
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
     924:	e9 e7 fa ff ff       	jmp    410 <listcmd>
     929:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00000930 <parseblock>:
  return cmd;
}

struct cmd*
parseblock(char **ps, char *es)
{
     930:	55                   	push   %ebp
     931:	89 e5                	mov    %esp,%ebp
     933:	57                   	push   %edi
     934:	56                   	push   %esi
     935:	53                   	push   %ebx
     936:	83 ec 10             	sub    $0x10,%esp
     939:	8b 5d 08             	mov    0x8(%ebp),%ebx
     93c:	8b 75 0c             	mov    0xc(%ebp),%esi
  struct cmd *cmd;

  if(!peek(ps, es, "("))
     93f:	68 f4 11 00 00       	push   $0x11f4
     944:	56                   	push   %esi
     945:	53                   	push   %ebx
     946:	e8 a5 fc ff ff       	call   5f0 <peek>
     94b:	83 c4 10             	add    $0x10,%esp
     94e:	85 c0                	test   %eax,%eax
     950:	74 4a                	je     99c <parseblock+0x6c>
    panic("parseblock");
  gettoken(ps, es, 0, 0);
     952:	6a 00                	push   $0x0
     954:	6a 00                	push   $0x0
     956:	56                   	push   %esi
     957:	53                   	push   %ebx
     958:	e8 23 fb ff ff       	call   480 <gettoken>
  cmd = parseline(ps, es);
     95d:	58                   	pop    %eax
     95e:	5a                   	pop    %edx
     95f:	56                   	push   %esi
     960:	53                   	push   %ebx
     961:	e8 2a ff ff ff       	call   890 <parseline>
  if(!peek(ps, es, ")"))
     966:	83 c4 0c             	add    $0xc,%esp
  struct cmd *cmd;

  if(!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
     969:	89 c7                	mov    %eax,%edi
  if(!peek(ps, es, ")"))
     96b:	68 30 12 00 00       	push   $0x1230
     970:	56                   	push   %esi
     971:	53                   	push   %ebx
     972:	e8 79 fc ff ff       	call   5f0 <peek>
     977:	83 c4 10             	add    $0x10,%esp
     97a:	85 c0                	test   %eax,%eax
     97c:	74 2b                	je     9a9 <parseblock+0x79>
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
     97e:	6a 00                	push   $0x0
     980:	6a 00                	push   $0x0
     982:	56                   	push   %esi
     983:	53                   	push   %ebx
     984:	e8 f7 fa ff ff       	call   480 <gettoken>
  cmd = parseredirs(cmd, ps, es);
     989:	83 c4 0c             	add    $0xc,%esp
     98c:	56                   	push   %esi
     98d:	53                   	push   %ebx
     98e:	57                   	push   %edi
     98f:	e8 cc fc ff ff       	call   660 <parseredirs>
  return cmd;
}
     994:	8d 65 f4             	lea    -0xc(%ebp),%esp
     997:	5b                   	pop    %ebx
     998:	5e                   	pop    %esi
     999:	5f                   	pop    %edi
     99a:	5d                   	pop    %ebp
     99b:	c3                   	ret    
parseblock(char **ps, char *es)
{
  struct cmd *cmd;

  if(!peek(ps, es, "("))
    panic("parseblock");
     99c:	83 ec 0c             	sub    $0xc,%esp
     99f:	68 14 12 00 00       	push   $0x1214
     9a4:	e8 b7 f7 ff ff       	call   160 <panic>
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  if(!peek(ps, es, ")"))
    panic("syntax - missing )");
     9a9:	83 ec 0c             	sub    $0xc,%esp
     9ac:	68 1f 12 00 00       	push   $0x121f
     9b1:	e8 aa f7 ff ff       	call   160 <panic>
     9b6:	8d 76 00             	lea    0x0(%esi),%esi
     9b9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

000009c0 <nulterminate>:
}

// NUL-terminate all the counted strings.
struct cmd*
nulterminate(struct cmd *cmd)
{
     9c0:	55                   	push   %ebp
     9c1:	89 e5                	mov    %esp,%ebp
     9c3:	53                   	push   %ebx
     9c4:	83 ec 04             	sub    $0x4,%esp
     9c7:	8b 5d 08             	mov    0x8(%ebp),%ebx
  struct execcmd *ecmd;
  struct listcmd *lcmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if(cmd == 0)
     9ca:	85 db                	test   %ebx,%ebx
     9cc:	0f 84 96 00 00 00    	je     a68 <nulterminate+0xa8>
    return 0;

  switch(cmd->type){
     9d2:	83 3b 05             	cmpl   $0x5,(%ebx)
     9d5:	77 48                	ja     a1f <nulterminate+0x5f>
     9d7:	8b 03                	mov    (%ebx),%eax
     9d9:	ff 24 85 70 12 00 00 	jmp    *0x1270(,%eax,4)
    nulterminate(pcmd->right);
    break;

  case LIST:
    lcmd = (struct listcmd*)cmd;
    nulterminate(lcmd->left);
     9e0:	83 ec 0c             	sub    $0xc,%esp
     9e3:	ff 73 04             	pushl  0x4(%ebx)
     9e6:	e8 d5 ff ff ff       	call   9c0 <nulterminate>
    nulterminate(lcmd->right);
     9eb:	58                   	pop    %eax
     9ec:	ff 73 08             	pushl  0x8(%ebx)
     9ef:	e8 cc ff ff ff       	call   9c0 <nulterminate>
    break;
     9f4:	83 c4 10             	add    $0x10,%esp
     9f7:	89 d8                	mov    %ebx,%eax
    bcmd = (struct backcmd*)cmd;
    nulterminate(bcmd->cmd);
    break;
  }
  return cmd;
}
     9f9:	8b 5d fc             	mov    -0x4(%ebp),%ebx
     9fc:	c9                   	leave  
     9fd:	c3                   	ret    
     9fe:	66 90                	xchg   %ax,%ax
    return 0;

  switch(cmd->type){
  case EXEC:
    ecmd = (struct execcmd*)cmd;
    for(i=0; ecmd->argv[i]; i++)
     a00:	8b 4b 04             	mov    0x4(%ebx),%ecx
     a03:	8d 43 2c             	lea    0x2c(%ebx),%eax
     a06:	85 c9                	test   %ecx,%ecx
     a08:	74 15                	je     a1f <nulterminate+0x5f>
     a0a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
      *ecmd->eargv[i] = 0;
     a10:	8b 10                	mov    (%eax),%edx
     a12:	83 c0 04             	add    $0x4,%eax
     a15:	c6 02 00             	movb   $0x0,(%edx)
    return 0;

  switch(cmd->type){
  case EXEC:
    ecmd = (struct execcmd*)cmd;
    for(i=0; ecmd->argv[i]; i++)
     a18:	8b 50 d8             	mov    -0x28(%eax),%edx
     a1b:	85 d2                	test   %edx,%edx
     a1d:	75 f1                	jne    a10 <nulterminate+0x50>
  struct redircmd *rcmd;

  if(cmd == 0)
    return 0;

  switch(cmd->type){
     a1f:	89 d8                	mov    %ebx,%eax
    bcmd = (struct backcmd*)cmd;
    nulterminate(bcmd->cmd);
    break;
  }
  return cmd;
}
     a21:	8b 5d fc             	mov    -0x4(%ebp),%ebx
     a24:	c9                   	leave  
     a25:	c3                   	ret    
     a26:	8d 76 00             	lea    0x0(%esi),%esi
     a29:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
    nulterminate(lcmd->right);
    break;

  case BACK:
    bcmd = (struct backcmd*)cmd;
    nulterminate(bcmd->cmd);
     a30:	83 ec 0c             	sub    $0xc,%esp
     a33:	ff 73 04             	pushl  0x4(%ebx)
     a36:	e8 85 ff ff ff       	call   9c0 <nulterminate>
    break;
     a3b:	89 d8                	mov    %ebx,%eax
     a3d:	83 c4 10             	add    $0x10,%esp
  }
  return cmd;
}
     a40:	8b 5d fc             	mov    -0x4(%ebp),%ebx
     a43:	c9                   	leave  
     a44:	c3                   	ret    
     a45:	8d 76 00             	lea    0x0(%esi),%esi
      *ecmd->eargv[i] = 0;
    break;

  case REDIR:
    rcmd = (struct redircmd*)cmd;
    nulterminate(rcmd->cmd);
     a48:	83 ec 0c             	sub    $0xc,%esp
     a4b:	ff 73 04             	pushl  0x4(%ebx)
     a4e:	e8 6d ff ff ff       	call   9c0 <nulterminate>
    *rcmd->efile = 0;
     a53:	8b 43 0c             	mov    0xc(%ebx),%eax
    break;
     a56:	83 c4 10             	add    $0x10,%esp
    break;

  case REDIR:
    rcmd = (struct redircmd*)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
     a59:	c6 00 00             	movb   $0x0,(%eax)
    break;
     a5c:	89 d8                	mov    %ebx,%eax
    bcmd = (struct backcmd*)cmd;
    nulterminate(bcmd->cmd);
    break;
  }
  return cmd;
}
     a5e:	8b 5d fc             	mov    -0x4(%ebp),%ebx
     a61:	c9                   	leave  
     a62:	c3                   	ret    
     a63:	90                   	nop
     a64:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  struct listcmd *lcmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if(cmd == 0)
    return 0;
     a68:	31 c0                	xor    %eax,%eax
     a6a:	eb 8d                	jmp    9f9 <nulterminate+0x39>
     a6c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00000a70 <parsecmd>:
struct cmd *parseexec(char**, char*);
struct cmd *nulterminate(struct cmd*);

struct cmd*
parsecmd(char *s)
{
     a70:	55                   	push   %ebp
     a71:	89 e5                	mov    %esp,%ebp
     a73:	56                   	push   %esi
     a74:	53                   	push   %ebx
  char *es;
  struct cmd *cmd;

  es = s + strlen(s);
     a75:	8b 5d 08             	mov    0x8(%ebp),%ebx
     a78:	83 ec 0c             	sub    $0xc,%esp
     a7b:	53                   	push   %ebx
     a7c:	e8 df 00 00 00       	call   b60 <strlen>
  cmd = parseline(&s, es);
     a81:	59                   	pop    %ecx
parsecmd(char *s)
{
  char *es;
  struct cmd *cmd;

  es = s + strlen(s);
     a82:	01 c3                	add    %eax,%ebx
  cmd = parseline(&s, es);
     a84:	8d 45 08             	lea    0x8(%ebp),%eax
     a87:	5e                   	pop    %esi
     a88:	53                   	push   %ebx
     a89:	50                   	push   %eax
     a8a:	e8 01 fe ff ff       	call   890 <parseline>
     a8f:	89 c6                	mov    %eax,%esi
  peek(&s, es, "");
     a91:	8d 45 08             	lea    0x8(%ebp),%eax
     a94:	83 c4 0c             	add    $0xc,%esp
     a97:	68 b9 11 00 00       	push   $0x11b9
     a9c:	53                   	push   %ebx
     a9d:	50                   	push   %eax
     a9e:	e8 4d fb ff ff       	call   5f0 <peek>
  if(s != es){
     aa3:	8b 45 08             	mov    0x8(%ebp),%eax
     aa6:	83 c4 10             	add    $0x10,%esp
     aa9:	39 c3                	cmp    %eax,%ebx
     aab:	75 12                	jne    abf <parsecmd+0x4f>
    printf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
     aad:	83 ec 0c             	sub    $0xc,%esp
     ab0:	56                   	push   %esi
     ab1:	e8 0a ff ff ff       	call   9c0 <nulterminate>
  return cmd;
}
     ab6:	8d 65 f8             	lea    -0x8(%ebp),%esp
     ab9:	89 f0                	mov    %esi,%eax
     abb:	5b                   	pop    %ebx
     abc:	5e                   	pop    %esi
     abd:	5d                   	pop    %ebp
     abe:	c3                   	ret    

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  if(s != es){
    printf(2, "leftovers: %s\n", s);
     abf:	52                   	push   %edx
     ac0:	50                   	push   %eax
     ac1:	68 32 12 00 00       	push   $0x1232
     ac6:	6a 02                	push   $0x2
     ac8:	e8 b3 03 00 00       	call   e80 <printf>
    panic("syntax");
     acd:	c7 04 24 f6 11 00 00 	movl   $0x11f6,(%esp)
     ad4:	e8 87 f6 ff ff       	call   160 <panic>
     ad9:	66 90                	xchg   %ax,%ax
     adb:	66 90                	xchg   %ax,%ax
     add:	66 90                	xchg   %ax,%ax
     adf:	90                   	nop

00000ae0 <strcpy>:
#include "user.h"
#include "x86.h"

char*
strcpy(char *s, char *t)
{
     ae0:	55                   	push   %ebp
     ae1:	89 e5                	mov    %esp,%ebp
     ae3:	53                   	push   %ebx
     ae4:	8b 45 08             	mov    0x8(%ebp),%eax
     ae7:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
     aea:	89 c2                	mov    %eax,%edx
     aec:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
     af0:	83 c1 01             	add    $0x1,%ecx
     af3:	0f b6 59 ff          	movzbl -0x1(%ecx),%ebx
     af7:	83 c2 01             	add    $0x1,%edx
     afa:	84 db                	test   %bl,%bl
     afc:	88 5a ff             	mov    %bl,-0x1(%edx)
     aff:	75 ef                	jne    af0 <strcpy+0x10>
    ;
  return os;
}
     b01:	5b                   	pop    %ebx
     b02:	5d                   	pop    %ebp
     b03:	c3                   	ret    
     b04:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
     b0a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

00000b10 <strcmp>:

int
strcmp(const char *p, const char *q)
{
     b10:	55                   	push   %ebp
     b11:	89 e5                	mov    %esp,%ebp
     b13:	56                   	push   %esi
     b14:	53                   	push   %ebx
     b15:	8b 55 08             	mov    0x8(%ebp),%edx
     b18:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  while(*p && *p == *q)
     b1b:	0f b6 02             	movzbl (%edx),%eax
     b1e:	0f b6 19             	movzbl (%ecx),%ebx
     b21:	84 c0                	test   %al,%al
     b23:	75 1e                	jne    b43 <strcmp+0x33>
     b25:	eb 29                	jmp    b50 <strcmp+0x40>
     b27:	89 f6                	mov    %esi,%esi
     b29:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
    p++, q++;
     b30:	83 c2 01             	add    $0x1,%edx
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
     b33:	0f b6 02             	movzbl (%edx),%eax
    p++, q++;
     b36:	8d 71 01             	lea    0x1(%ecx),%esi
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
     b39:	0f b6 59 01          	movzbl 0x1(%ecx),%ebx
     b3d:	84 c0                	test   %al,%al
     b3f:	74 0f                	je     b50 <strcmp+0x40>
     b41:	89 f1                	mov    %esi,%ecx
     b43:	38 d8                	cmp    %bl,%al
     b45:	74 e9                	je     b30 <strcmp+0x20>
    p++, q++;
  return (uchar)*p - (uchar)*q;
     b47:	29 d8                	sub    %ebx,%eax
}
     b49:	5b                   	pop    %ebx
     b4a:	5e                   	pop    %esi
     b4b:	5d                   	pop    %ebp
     b4c:	c3                   	ret    
     b4d:	8d 76 00             	lea    0x0(%esi),%esi
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
     b50:	31 c0                	xor    %eax,%eax
    p++, q++;
  return (uchar)*p - (uchar)*q;
     b52:	29 d8                	sub    %ebx,%eax
}
     b54:	5b                   	pop    %ebx
     b55:	5e                   	pop    %esi
     b56:	5d                   	pop    %ebp
     b57:	c3                   	ret    
     b58:	90                   	nop
     b59:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00000b60 <strlen>:

uint
strlen(char *s)
{
     b60:	55                   	push   %ebp
     b61:	89 e5                	mov    %esp,%ebp
     b63:	8b 4d 08             	mov    0x8(%ebp),%ecx
  int n;

  for(n = 0; s[n]; n++)
     b66:	80 39 00             	cmpb   $0x0,(%ecx)
     b69:	74 12                	je     b7d <strlen+0x1d>
     b6b:	31 d2                	xor    %edx,%edx
     b6d:	8d 76 00             	lea    0x0(%esi),%esi
     b70:	83 c2 01             	add    $0x1,%edx
     b73:	80 3c 11 00          	cmpb   $0x0,(%ecx,%edx,1)
     b77:	89 d0                	mov    %edx,%eax
     b79:	75 f5                	jne    b70 <strlen+0x10>
    ;
  return n;
}
     b7b:	5d                   	pop    %ebp
     b7c:	c3                   	ret    
uint
strlen(char *s)
{
  int n;

  for(n = 0; s[n]; n++)
     b7d:	31 c0                	xor    %eax,%eax
    ;
  return n;
}
     b7f:	5d                   	pop    %ebp
     b80:	c3                   	ret    
     b81:	eb 0d                	jmp    b90 <memset>
     b83:	90                   	nop
     b84:	90                   	nop
     b85:	90                   	nop
     b86:	90                   	nop
     b87:	90                   	nop
     b88:	90                   	nop
     b89:	90                   	nop
     b8a:	90                   	nop
     b8b:	90                   	nop
     b8c:	90                   	nop
     b8d:	90                   	nop
     b8e:	90                   	nop
     b8f:	90                   	nop

00000b90 <memset>:

void*
memset(void *dst, int c, uint n)
{
     b90:	55                   	push   %ebp
     b91:	89 e5                	mov    %esp,%ebp
     b93:	57                   	push   %edi
     b94:	8b 55 08             	mov    0x8(%ebp),%edx
}

static inline void
stosb(void *addr, int data, int cnt)
{
  asm volatile("cld; rep stosb" :
     b97:	8b 4d 10             	mov    0x10(%ebp),%ecx
     b9a:	8b 45 0c             	mov    0xc(%ebp),%eax
     b9d:	89 d7                	mov    %edx,%edi
     b9f:	fc                   	cld    
     ba0:	f3 aa                	rep stos %al,%es:(%edi)
  stosb(dst, c, n);
  return dst;
}
     ba2:	89 d0                	mov    %edx,%eax
     ba4:	5f                   	pop    %edi
     ba5:	5d                   	pop    %ebp
     ba6:	c3                   	ret    
     ba7:	89 f6                	mov    %esi,%esi
     ba9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000bb0 <strchr>:

char*
strchr(const char *s, char c)
{
     bb0:	55                   	push   %ebp
     bb1:	89 e5                	mov    %esp,%ebp
     bb3:	53                   	push   %ebx
     bb4:	8b 45 08             	mov    0x8(%ebp),%eax
     bb7:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  for(; *s; s++)
     bba:	0f b6 10             	movzbl (%eax),%edx
     bbd:	84 d2                	test   %dl,%dl
     bbf:	74 1d                	je     bde <strchr+0x2e>
    if(*s == c)
     bc1:	38 d3                	cmp    %dl,%bl
     bc3:	89 d9                	mov    %ebx,%ecx
     bc5:	75 0d                	jne    bd4 <strchr+0x24>
     bc7:	eb 17                	jmp    be0 <strchr+0x30>
     bc9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
     bd0:	38 ca                	cmp    %cl,%dl
     bd2:	74 0c                	je     be0 <strchr+0x30>
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
     bd4:	83 c0 01             	add    $0x1,%eax
     bd7:	0f b6 10             	movzbl (%eax),%edx
     bda:	84 d2                	test   %dl,%dl
     bdc:	75 f2                	jne    bd0 <strchr+0x20>
    if(*s == c)
      return (char*)s;
  return 0;
     bde:	31 c0                	xor    %eax,%eax
}
     be0:	5b                   	pop    %ebx
     be1:	5d                   	pop    %ebp
     be2:	c3                   	ret    
     be3:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
     be9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000bf0 <gets>:

char*
gets(char *buf, int max)
{
     bf0:	55                   	push   %ebp
     bf1:	89 e5                	mov    %esp,%ebp
     bf3:	57                   	push   %edi
     bf4:	56                   	push   %esi
     bf5:	53                   	push   %ebx
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
     bf6:	31 f6                	xor    %esi,%esi
    cc = read(0, &c, 1);
     bf8:	8d 7d e7             	lea    -0x19(%ebp),%edi
  return 0;
}

char*
gets(char *buf, int max)
{
     bfb:	83 ec 1c             	sub    $0x1c,%esp
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
     bfe:	eb 29                	jmp    c29 <gets+0x39>
    cc = read(0, &c, 1);
     c00:	83 ec 04             	sub    $0x4,%esp
     c03:	6a 01                	push   $0x1
     c05:	57                   	push   %edi
     c06:	6a 00                	push   $0x0
     c08:	e8 2d 01 00 00       	call   d3a <read>
    if(cc < 1)
     c0d:	83 c4 10             	add    $0x10,%esp
     c10:	85 c0                	test   %eax,%eax
     c12:	7e 1d                	jle    c31 <gets+0x41>
      break;
    buf[i++] = c;
     c14:	0f b6 45 e7          	movzbl -0x19(%ebp),%eax
     c18:	8b 55 08             	mov    0x8(%ebp),%edx
     c1b:	89 de                	mov    %ebx,%esi
    if(c == '\n' || c == '\r')
     c1d:	3c 0a                	cmp    $0xa,%al

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
     c1f:	88 44 1a ff          	mov    %al,-0x1(%edx,%ebx,1)
    if(c == '\n' || c == '\r')
     c23:	74 1b                	je     c40 <gets+0x50>
     c25:	3c 0d                	cmp    $0xd,%al
     c27:	74 17                	je     c40 <gets+0x50>
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
     c29:	8d 5e 01             	lea    0x1(%esi),%ebx
     c2c:	3b 5d 0c             	cmp    0xc(%ebp),%ebx
     c2f:	7c cf                	jl     c00 <gets+0x10>
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
     c31:	8b 45 08             	mov    0x8(%ebp),%eax
     c34:	c6 04 30 00          	movb   $0x0,(%eax,%esi,1)
  return buf;
}
     c38:	8d 65 f4             	lea    -0xc(%ebp),%esp
     c3b:	5b                   	pop    %ebx
     c3c:	5e                   	pop    %esi
     c3d:	5f                   	pop    %edi
     c3e:	5d                   	pop    %ebp
     c3f:	c3                   	ret    
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
     c40:	8b 45 08             	mov    0x8(%ebp),%eax
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
     c43:	89 de                	mov    %ebx,%esi
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
     c45:	c6 04 30 00          	movb   $0x0,(%eax,%esi,1)
  return buf;
}
     c49:	8d 65 f4             	lea    -0xc(%ebp),%esp
     c4c:	5b                   	pop    %ebx
     c4d:	5e                   	pop    %esi
     c4e:	5f                   	pop    %edi
     c4f:	5d                   	pop    %ebp
     c50:	c3                   	ret    
     c51:	eb 0d                	jmp    c60 <stat>
     c53:	90                   	nop
     c54:	90                   	nop
     c55:	90                   	nop
     c56:	90                   	nop
     c57:	90                   	nop
     c58:	90                   	nop
     c59:	90                   	nop
     c5a:	90                   	nop
     c5b:	90                   	nop
     c5c:	90                   	nop
     c5d:	90                   	nop
     c5e:	90                   	nop
     c5f:	90                   	nop

00000c60 <stat>:

int
stat(char *n, struct stat *st)
{
     c60:	55                   	push   %ebp
     c61:	89 e5                	mov    %esp,%ebp
     c63:	56                   	push   %esi
     c64:	53                   	push   %ebx
  int fd;
  int r;

  fd = open(n, O_RDONLY);
     c65:	83 ec 08             	sub    $0x8,%esp
     c68:	6a 00                	push   $0x0
     c6a:	ff 75 08             	pushl  0x8(%ebp)
     c6d:	e8 f0 00 00 00       	call   d62 <open>
  if(fd < 0)
     c72:	83 c4 10             	add    $0x10,%esp
     c75:	85 c0                	test   %eax,%eax
     c77:	78 27                	js     ca0 <stat+0x40>
    return -1;
  r = fstat(fd, st);
     c79:	83 ec 08             	sub    $0x8,%esp
     c7c:	ff 75 0c             	pushl  0xc(%ebp)
     c7f:	89 c3                	mov    %eax,%ebx
     c81:	50                   	push   %eax
     c82:	e8 f3 00 00 00       	call   d7a <fstat>
     c87:	89 c6                	mov    %eax,%esi
  close(fd);
     c89:	89 1c 24             	mov    %ebx,(%esp)
     c8c:	e8 b9 00 00 00       	call   d4a <close>
  return r;
     c91:	83 c4 10             	add    $0x10,%esp
     c94:	89 f0                	mov    %esi,%eax
}
     c96:	8d 65 f8             	lea    -0x8(%ebp),%esp
     c99:	5b                   	pop    %ebx
     c9a:	5e                   	pop    %esi
     c9b:	5d                   	pop    %ebp
     c9c:	c3                   	ret    
     c9d:	8d 76 00             	lea    0x0(%esi),%esi
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
     ca0:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
     ca5:	eb ef                	jmp    c96 <stat+0x36>
     ca7:	89 f6                	mov    %esi,%esi
     ca9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000cb0 <atoi>:
  return r;
}

int
atoi(const char *s)
{
     cb0:	55                   	push   %ebp
     cb1:	89 e5                	mov    %esp,%ebp
     cb3:	53                   	push   %ebx
     cb4:	8b 4d 08             	mov    0x8(%ebp),%ecx
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
     cb7:	0f be 11             	movsbl (%ecx),%edx
     cba:	8d 42 d0             	lea    -0x30(%edx),%eax
     cbd:	3c 09                	cmp    $0x9,%al
     cbf:	b8 00 00 00 00       	mov    $0x0,%eax
     cc4:	77 1f                	ja     ce5 <atoi+0x35>
     cc6:	8d 76 00             	lea    0x0(%esi),%esi
     cc9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
    n = n*10 + *s++ - '0';
     cd0:	8d 04 80             	lea    (%eax,%eax,4),%eax
     cd3:	83 c1 01             	add    $0x1,%ecx
     cd6:	8d 44 42 d0          	lea    -0x30(%edx,%eax,2),%eax
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
     cda:	0f be 11             	movsbl (%ecx),%edx
     cdd:	8d 5a d0             	lea    -0x30(%edx),%ebx
     ce0:	80 fb 09             	cmp    $0x9,%bl
     ce3:	76 eb                	jbe    cd0 <atoi+0x20>
    n = n*10 + *s++ - '0';
  return n;
}
     ce5:	5b                   	pop    %ebx
     ce6:	5d                   	pop    %ebp
     ce7:	c3                   	ret    
     ce8:	90                   	nop
     ce9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00000cf0 <memmove>:

void*
memmove(void *vdst, void *vsrc, int n)
{
     cf0:	55                   	push   %ebp
     cf1:	89 e5                	mov    %esp,%ebp
     cf3:	56                   	push   %esi
     cf4:	53                   	push   %ebx
     cf5:	8b 5d 10             	mov    0x10(%ebp),%ebx
     cf8:	8b 45 08             	mov    0x8(%ebp),%eax
     cfb:	8b 75 0c             	mov    0xc(%ebp),%esi
  char *dst, *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
     cfe:	85 db                	test   %ebx,%ebx
     d00:	7e 14                	jle    d16 <memmove+0x26>
     d02:	31 d2                	xor    %edx,%edx
     d04:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    *dst++ = *src++;
     d08:	0f b6 0c 16          	movzbl (%esi,%edx,1),%ecx
     d0c:	88 0c 10             	mov    %cl,(%eax,%edx,1)
     d0f:	83 c2 01             	add    $0x1,%edx
{
  char *dst, *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
     d12:	39 da                	cmp    %ebx,%edx
     d14:	75 f2                	jne    d08 <memmove+0x18>
    *dst++ = *src++;
  return vdst;
}
     d16:	5b                   	pop    %ebx
     d17:	5e                   	pop    %esi
     d18:	5d                   	pop    %ebp
     d19:	c3                   	ret    

00000d1a <fork>:
  name: \
    movl $SYS_ ## name, %eax; \
    int $T_SYSCALL; \
    ret

SYSCALL(fork)
     d1a:	b8 01 00 00 00       	mov    $0x1,%eax
     d1f:	cd 40                	int    $0x40
     d21:	c3                   	ret    

00000d22 <exit>:
SYSCALL(exit)
     d22:	b8 02 00 00 00       	mov    $0x2,%eax
     d27:	cd 40                	int    $0x40
     d29:	c3                   	ret    

00000d2a <wait>:
SYSCALL(wait)
     d2a:	b8 03 00 00 00       	mov    $0x3,%eax
     d2f:	cd 40                	int    $0x40
     d31:	c3                   	ret    

00000d32 <pipe>:
SYSCALL(pipe)
     d32:	b8 04 00 00 00       	mov    $0x4,%eax
     d37:	cd 40                	int    $0x40
     d39:	c3                   	ret    

00000d3a <read>:
SYSCALL(read)
     d3a:	b8 05 00 00 00       	mov    $0x5,%eax
     d3f:	cd 40                	int    $0x40
     d41:	c3                   	ret    

00000d42 <write>:
SYSCALL(write)
     d42:	b8 10 00 00 00       	mov    $0x10,%eax
     d47:	cd 40                	int    $0x40
     d49:	c3                   	ret    

00000d4a <close>:
SYSCALL(close)
     d4a:	b8 15 00 00 00       	mov    $0x15,%eax
     d4f:	cd 40                	int    $0x40
     d51:	c3                   	ret    

00000d52 <kill>:
SYSCALL(kill)
     d52:	b8 06 00 00 00       	mov    $0x6,%eax
     d57:	cd 40                	int    $0x40
     d59:	c3                   	ret    

00000d5a <exec>:
SYSCALL(exec)
     d5a:	b8 07 00 00 00       	mov    $0x7,%eax
     d5f:	cd 40                	int    $0x40
     d61:	c3                   	ret    

00000d62 <open>:
SYSCALL(open)
     d62:	b8 0f 00 00 00       	mov    $0xf,%eax
     d67:	cd 40                	int    $0x40
     d69:	c3                   	ret    

00000d6a <mknod>:
SYSCALL(mknod)
     d6a:	b8 11 00 00 00       	mov    $0x11,%eax
     d6f:	cd 40                	int    $0x40
     d71:	c3                   	ret    

00000d72 <unlink>:
SYSCALL(unlink)
     d72:	b8 12 00 00 00       	mov    $0x12,%eax
     d77:	cd 40                	int    $0x40
     d79:	c3                   	ret    

00000d7a <fstat>:
SYSCALL(fstat)
     d7a:	b8 08 00 00 00       	mov    $0x8,%eax
     d7f:	cd 40                	int    $0x40
     d81:	c3                   	ret    

00000d82 <link>:
SYSCALL(link)
     d82:	b8 13 00 00 00       	mov    $0x13,%eax
     d87:	cd 40                	int    $0x40
     d89:	c3                   	ret    

00000d8a <mkdir>:
SYSCALL(mkdir)
     d8a:	b8 14 00 00 00       	mov    $0x14,%eax
     d8f:	cd 40                	int    $0x40
     d91:	c3                   	ret    

00000d92 <chdir>:
SYSCALL(chdir)
     d92:	b8 09 00 00 00       	mov    $0x9,%eax
     d97:	cd 40                	int    $0x40
     d99:	c3                   	ret    

00000d9a <dup>:
SYSCALL(dup)
     d9a:	b8 0a 00 00 00       	mov    $0xa,%eax
     d9f:	cd 40                	int    $0x40
     da1:	c3                   	ret    

00000da2 <getpid>:
SYSCALL(getpid)
     da2:	b8 0b 00 00 00       	mov    $0xb,%eax
     da7:	cd 40                	int    $0x40
     da9:	c3                   	ret    

00000daa <sbrk>:
SYSCALL(sbrk)
     daa:	b8 0c 00 00 00       	mov    $0xc,%eax
     daf:	cd 40                	int    $0x40
     db1:	c3                   	ret    

00000db2 <sleep>:
SYSCALL(sleep)
     db2:	b8 0d 00 00 00       	mov    $0xd,%eax
     db7:	cd 40                	int    $0x40
     db9:	c3                   	ret    

00000dba <uptime>:
SYSCALL(uptime)
     dba:	b8 0e 00 00 00       	mov    $0xe,%eax
     dbf:	cd 40                	int    $0x40
     dc1:	c3                   	ret    

00000dc2 <cps>:
SYSCALL(cps)
     dc2:	b8 16 00 00 00       	mov    $0x16,%eax
     dc7:	cd 40                	int    $0x40
     dc9:	c3                   	ret    

00000dca <chpr>:
SYSCALL(chpr)
     dca:	b8 17 00 00 00       	mov    $0x17,%eax
     dcf:	cd 40                	int    $0x40
     dd1:	c3                   	ret    

00000dd2 <chdl>:
SYSCALL(chdl)
     dd2:	b8 18 00 00 00       	mov    $0x18,%eax
     dd7:	cd 40                	int    $0x40
     dd9:	c3                   	ret    
     dda:	66 90                	xchg   %ax,%ax
     ddc:	66 90                	xchg   %ax,%ax
     dde:	66 90                	xchg   %ax,%ax

00000de0 <printint>:
  write(fd, &c, 1);
}

static void
printint(int fd, int xx, int base, int sgn)
{
     de0:	55                   	push   %ebp
     de1:	89 e5                	mov    %esp,%ebp
     de3:	57                   	push   %edi
     de4:	56                   	push   %esi
     de5:	53                   	push   %ebx
     de6:	89 c6                	mov    %eax,%esi
     de8:	83 ec 3c             	sub    $0x3c,%esp
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
     deb:	8b 5d 08             	mov    0x8(%ebp),%ebx
     dee:	85 db                	test   %ebx,%ebx
     df0:	74 7e                	je     e70 <printint+0x90>
     df2:	89 d0                	mov    %edx,%eax
     df4:	c1 e8 1f             	shr    $0x1f,%eax
     df7:	84 c0                	test   %al,%al
     df9:	74 75                	je     e70 <printint+0x90>
    neg = 1;
    x = -xx;
     dfb:	89 d0                	mov    %edx,%eax
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
     dfd:	c7 45 c4 01 00 00 00 	movl   $0x1,-0x3c(%ebp)
    x = -xx;
     e04:	f7 d8                	neg    %eax
     e06:	89 75 c0             	mov    %esi,-0x40(%ebp)
  } else {
    x = xx;
  }

  i = 0;
     e09:	31 ff                	xor    %edi,%edi
     e0b:	8d 5d d7             	lea    -0x29(%ebp),%ebx
     e0e:	89 ce                	mov    %ecx,%esi
     e10:	eb 08                	jmp    e1a <printint+0x3a>
     e12:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  do{
    buf[i++] = digits[x % base];
     e18:	89 cf                	mov    %ecx,%edi
     e1a:	31 d2                	xor    %edx,%edx
     e1c:	8d 4f 01             	lea    0x1(%edi),%ecx
     e1f:	f7 f6                	div    %esi
     e21:	0f b6 92 90 12 00 00 	movzbl 0x1290(%edx),%edx
  }while((x /= base) != 0);
     e28:	85 c0                	test   %eax,%eax
    x = xx;
  }

  i = 0;
  do{
    buf[i++] = digits[x % base];
     e2a:	88 14 0b             	mov    %dl,(%ebx,%ecx,1)
  }while((x /= base) != 0);
     e2d:	75 e9                	jne    e18 <printint+0x38>
  if(neg)
     e2f:	8b 45 c4             	mov    -0x3c(%ebp),%eax
     e32:	8b 75 c0             	mov    -0x40(%ebp),%esi
     e35:	85 c0                	test   %eax,%eax
     e37:	74 08                	je     e41 <printint+0x61>
    buf[i++] = '-';
     e39:	c6 44 0d d8 2d       	movb   $0x2d,-0x28(%ebp,%ecx,1)
     e3e:	8d 4f 02             	lea    0x2(%edi),%ecx
     e41:	8d 7c 0d d7          	lea    -0x29(%ebp,%ecx,1),%edi
     e45:	8d 76 00             	lea    0x0(%esi),%esi
     e48:	0f b6 07             	movzbl (%edi),%eax
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
     e4b:	83 ec 04             	sub    $0x4,%esp
     e4e:	83 ef 01             	sub    $0x1,%edi
     e51:	6a 01                	push   $0x1
     e53:	53                   	push   %ebx
     e54:	56                   	push   %esi
     e55:	88 45 d7             	mov    %al,-0x29(%ebp)
     e58:	e8 e5 fe ff ff       	call   d42 <write>
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);
  if(neg)
    buf[i++] = '-';

  while(--i >= 0)
     e5d:	83 c4 10             	add    $0x10,%esp
     e60:	39 df                	cmp    %ebx,%edi
     e62:	75 e4                	jne    e48 <printint+0x68>
    putc(fd, buf[i]);
}
     e64:	8d 65 f4             	lea    -0xc(%ebp),%esp
     e67:	5b                   	pop    %ebx
     e68:	5e                   	pop    %esi
     e69:	5f                   	pop    %edi
     e6a:	5d                   	pop    %ebp
     e6b:	c3                   	ret    
     e6c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
  } else {
    x = xx;
     e70:	89 d0                	mov    %edx,%eax
  static char digits[] = "0123456789ABCDEF";
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
     e72:	c7 45 c4 00 00 00 00 	movl   $0x0,-0x3c(%ebp)
     e79:	eb 8b                	jmp    e06 <printint+0x26>
     e7b:	90                   	nop
     e7c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00000e80 <printf>:
}

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, char *fmt, ...)
{
     e80:	55                   	push   %ebp
     e81:	89 e5                	mov    %esp,%ebp
     e83:	57                   	push   %edi
     e84:	56                   	push   %esi
     e85:	53                   	push   %ebx
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
     e86:	8d 45 10             	lea    0x10(%ebp),%eax
}

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, char *fmt, ...)
{
     e89:	83 ec 2c             	sub    $0x2c,%esp
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
     e8c:	8b 75 0c             	mov    0xc(%ebp),%esi
}

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, char *fmt, ...)
{
     e8f:	8b 7d 08             	mov    0x8(%ebp),%edi
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
     e92:	89 45 d0             	mov    %eax,-0x30(%ebp)
     e95:	0f b6 1e             	movzbl (%esi),%ebx
     e98:	83 c6 01             	add    $0x1,%esi
     e9b:	84 db                	test   %bl,%bl
     e9d:	0f 84 b0 00 00 00    	je     f53 <printf+0xd3>
     ea3:	31 d2                	xor    %edx,%edx
     ea5:	eb 39                	jmp    ee0 <printf+0x60>
     ea7:	89 f6                	mov    %esi,%esi
     ea9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
     eb0:	83 f8 25             	cmp    $0x25,%eax
     eb3:	89 55 d4             	mov    %edx,-0x2c(%ebp)
        state = '%';
     eb6:	ba 25 00 00 00       	mov    $0x25,%edx
  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
     ebb:	74 18                	je     ed5 <printf+0x55>
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
     ebd:	8d 45 e2             	lea    -0x1e(%ebp),%eax
     ec0:	83 ec 04             	sub    $0x4,%esp
     ec3:	88 5d e2             	mov    %bl,-0x1e(%ebp)
     ec6:	6a 01                	push   $0x1
     ec8:	50                   	push   %eax
     ec9:	57                   	push   %edi
     eca:	e8 73 fe ff ff       	call   d42 <write>
     ecf:	8b 55 d4             	mov    -0x2c(%ebp),%edx
     ed2:	83 c4 10             	add    $0x10,%esp
     ed5:	83 c6 01             	add    $0x1,%esi
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
     ed8:	0f b6 5e ff          	movzbl -0x1(%esi),%ebx
     edc:	84 db                	test   %bl,%bl
     ede:	74 73                	je     f53 <printf+0xd3>
    c = fmt[i] & 0xff;
    if(state == 0){
     ee0:	85 d2                	test   %edx,%edx
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;
     ee2:	0f be cb             	movsbl %bl,%ecx
     ee5:	0f b6 c3             	movzbl %bl,%eax
    if(state == 0){
     ee8:	74 c6                	je     eb0 <printf+0x30>
      if(c == '%'){
        state = '%';
      } else {
        putc(fd, c);
      }
    } else if(state == '%'){
     eea:	83 fa 25             	cmp    $0x25,%edx
     eed:	75 e6                	jne    ed5 <printf+0x55>
      if(c == 'd'){
     eef:	83 f8 64             	cmp    $0x64,%eax
     ef2:	0f 84 f8 00 00 00    	je     ff0 <printf+0x170>
        printint(fd, *ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
     ef8:	81 e1 f7 00 00 00    	and    $0xf7,%ecx
     efe:	83 f9 70             	cmp    $0x70,%ecx
     f01:	74 5d                	je     f60 <printf+0xe0>
        printint(fd, *ap, 16, 0);
        ap++;
      } else if(c == 's'){
     f03:	83 f8 73             	cmp    $0x73,%eax
     f06:	0f 84 84 00 00 00    	je     f90 <printf+0x110>
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
        }
      } else if(c == 'c'){
     f0c:	83 f8 63             	cmp    $0x63,%eax
     f0f:	0f 84 ea 00 00 00    	je     fff <printf+0x17f>
        putc(fd, *ap);
        ap++;
      } else if(c == '%'){
     f15:	83 f8 25             	cmp    $0x25,%eax
     f18:	0f 84 c2 00 00 00    	je     fe0 <printf+0x160>
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
     f1e:	8d 45 e7             	lea    -0x19(%ebp),%eax
     f21:	83 ec 04             	sub    $0x4,%esp
     f24:	c6 45 e7 25          	movb   $0x25,-0x19(%ebp)
     f28:	6a 01                	push   $0x1
     f2a:	50                   	push   %eax
     f2b:	57                   	push   %edi
     f2c:	e8 11 fe ff ff       	call   d42 <write>
     f31:	83 c4 0c             	add    $0xc,%esp
     f34:	8d 45 e6             	lea    -0x1a(%ebp),%eax
     f37:	88 5d e6             	mov    %bl,-0x1a(%ebp)
     f3a:	6a 01                	push   $0x1
     f3c:	50                   	push   %eax
     f3d:	57                   	push   %edi
     f3e:	83 c6 01             	add    $0x1,%esi
     f41:	e8 fc fd ff ff       	call   d42 <write>
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
     f46:	0f b6 5e ff          	movzbl -0x1(%esi),%ebx
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
     f4a:	83 c4 10             	add    $0x10,%esp
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
     f4d:	31 d2                	xor    %edx,%edx
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
     f4f:	84 db                	test   %bl,%bl
     f51:	75 8d                	jne    ee0 <printf+0x60>
        putc(fd, c);
      }
      state = 0;
    }
  }
}
     f53:	8d 65 f4             	lea    -0xc(%ebp),%esp
     f56:	5b                   	pop    %ebx
     f57:	5e                   	pop    %esi
     f58:	5f                   	pop    %edi
     f59:	5d                   	pop    %ebp
     f5a:	c3                   	ret    
     f5b:	90                   	nop
     f5c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    } else if(state == '%'){
      if(c == 'd'){
        printint(fd, *ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
        printint(fd, *ap, 16, 0);
     f60:	83 ec 0c             	sub    $0xc,%esp
     f63:	b9 10 00 00 00       	mov    $0x10,%ecx
     f68:	6a 00                	push   $0x0
     f6a:	8b 5d d0             	mov    -0x30(%ebp),%ebx
     f6d:	89 f8                	mov    %edi,%eax
     f6f:	8b 13                	mov    (%ebx),%edx
     f71:	e8 6a fe ff ff       	call   de0 <printint>
        ap++;
     f76:	89 d8                	mov    %ebx,%eax
     f78:	83 c4 10             	add    $0x10,%esp
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
     f7b:	31 d2                	xor    %edx,%edx
      if(c == 'd'){
        printint(fd, *ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
        printint(fd, *ap, 16, 0);
        ap++;
     f7d:	83 c0 04             	add    $0x4,%eax
     f80:	89 45 d0             	mov    %eax,-0x30(%ebp)
     f83:	e9 4d ff ff ff       	jmp    ed5 <printf+0x55>
     f88:	90                   	nop
     f89:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
      } else if(c == 's'){
        s = (char*)*ap;
     f90:	8b 45 d0             	mov    -0x30(%ebp),%eax
     f93:	8b 18                	mov    (%eax),%ebx
        ap++;
     f95:	83 c0 04             	add    $0x4,%eax
     f98:	89 45 d0             	mov    %eax,-0x30(%ebp)
        if(s == 0)
          s = "(null)";
     f9b:	b8 88 12 00 00       	mov    $0x1288,%eax
     fa0:	85 db                	test   %ebx,%ebx
     fa2:	0f 44 d8             	cmove  %eax,%ebx
        while(*s != 0){
     fa5:	0f b6 03             	movzbl (%ebx),%eax
     fa8:	84 c0                	test   %al,%al
     faa:	74 23                	je     fcf <printf+0x14f>
     fac:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
     fb0:	88 45 e3             	mov    %al,-0x1d(%ebp)
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
     fb3:	8d 45 e3             	lea    -0x1d(%ebp),%eax
     fb6:	83 ec 04             	sub    $0x4,%esp
     fb9:	6a 01                	push   $0x1
        ap++;
        if(s == 0)
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
     fbb:	83 c3 01             	add    $0x1,%ebx
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
     fbe:	50                   	push   %eax
     fbf:	57                   	push   %edi
     fc0:	e8 7d fd ff ff       	call   d42 <write>
      } else if(c == 's'){
        s = (char*)*ap;
        ap++;
        if(s == 0)
          s = "(null)";
        while(*s != 0){
     fc5:	0f b6 03             	movzbl (%ebx),%eax
     fc8:	83 c4 10             	add    $0x10,%esp
     fcb:	84 c0                	test   %al,%al
     fcd:	75 e1                	jne    fb0 <printf+0x130>
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
     fcf:	31 d2                	xor    %edx,%edx
     fd1:	e9 ff fe ff ff       	jmp    ed5 <printf+0x55>
     fd6:	8d 76 00             	lea    0x0(%esi),%esi
     fd9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
     fe0:	83 ec 04             	sub    $0x4,%esp
     fe3:	88 5d e5             	mov    %bl,-0x1b(%ebp)
     fe6:	8d 45 e5             	lea    -0x1b(%ebp),%eax
     fe9:	6a 01                	push   $0x1
     feb:	e9 4c ff ff ff       	jmp    f3c <printf+0xbc>
      } else {
        putc(fd, c);
      }
    } else if(state == '%'){
      if(c == 'd'){
        printint(fd, *ap, 10, 1);
     ff0:	83 ec 0c             	sub    $0xc,%esp
     ff3:	b9 0a 00 00 00       	mov    $0xa,%ecx
     ff8:	6a 01                	push   $0x1
     ffa:	e9 6b ff ff ff       	jmp    f6a <printf+0xea>
     fff:	8b 5d d0             	mov    -0x30(%ebp),%ebx
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
    1002:	83 ec 04             	sub    $0x4,%esp
    1005:	8b 03                	mov    (%ebx),%eax
    1007:	6a 01                	push   $0x1
    1009:	88 45 e4             	mov    %al,-0x1c(%ebp)
    100c:	8d 45 e4             	lea    -0x1c(%ebp),%eax
    100f:	50                   	push   %eax
    1010:	57                   	push   %edi
    1011:	e8 2c fd ff ff       	call   d42 <write>
    1016:	e9 5b ff ff ff       	jmp    f76 <printf+0xf6>
    101b:	66 90                	xchg   %ax,%ax
    101d:	66 90                	xchg   %ax,%ax
    101f:	90                   	nop

00001020 <free>:
static Header base;
static Header *freep;

void
free(void *ap)
{
    1020:	55                   	push   %ebp
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    1021:	a1 c4 18 00 00       	mov    0x18c4,%eax
static Header base;
static Header *freep;

void
free(void *ap)
{
    1026:	89 e5                	mov    %esp,%ebp
    1028:	57                   	push   %edi
    1029:	56                   	push   %esi
    102a:	53                   	push   %ebx
    102b:	8b 5d 08             	mov    0x8(%ebp),%ebx
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
    102e:	8b 10                	mov    (%eax),%edx
void
free(void *ap)
{
  Header *bp, *p;

  bp = (Header*)ap - 1;
    1030:	8d 4b f8             	lea    -0x8(%ebx),%ecx
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    1033:	39 c8                	cmp    %ecx,%eax
    1035:	73 19                	jae    1050 <free+0x30>
    1037:	89 f6                	mov    %esi,%esi
    1039:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
    1040:	39 d1                	cmp    %edx,%ecx
    1042:	72 1c                	jb     1060 <free+0x40>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
    1044:	39 d0                	cmp    %edx,%eax
    1046:	73 18                	jae    1060 <free+0x40>
static Header base;
static Header *freep;

void
free(void *ap)
{
    1048:	89 d0                	mov    %edx,%eax
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    104a:	39 c8                	cmp    %ecx,%eax
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
    104c:	8b 10                	mov    (%eax),%edx
free(void *ap)
{
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    104e:	72 f0                	jb     1040 <free+0x20>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
    1050:	39 d0                	cmp    %edx,%eax
    1052:	72 f4                	jb     1048 <free+0x28>
    1054:	39 d1                	cmp    %edx,%ecx
    1056:	73 f0                	jae    1048 <free+0x28>
    1058:	90                   	nop
    1059:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
      break;
  if(bp + bp->s.size == p->s.ptr){
    1060:	8b 73 fc             	mov    -0x4(%ebx),%esi
    1063:	8d 3c f1             	lea    (%ecx,%esi,8),%edi
    1066:	39 d7                	cmp    %edx,%edi
    1068:	74 19                	je     1083 <free+0x63>
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
    106a:	89 53 f8             	mov    %edx,-0x8(%ebx)
  if(p + p->s.size == bp){
    106d:	8b 50 04             	mov    0x4(%eax),%edx
    1070:	8d 34 d0             	lea    (%eax,%edx,8),%esi
    1073:	39 f1                	cmp    %esi,%ecx
    1075:	74 23                	je     109a <free+0x7a>
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
    1077:	89 08                	mov    %ecx,(%eax)
  freep = p;
    1079:	a3 c4 18 00 00       	mov    %eax,0x18c4
}
    107e:	5b                   	pop    %ebx
    107f:	5e                   	pop    %esi
    1080:	5f                   	pop    %edi
    1081:	5d                   	pop    %ebp
    1082:	c3                   	ret    
  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  if(bp + bp->s.size == p->s.ptr){
    bp->s.size += p->s.ptr->s.size;
    1083:	03 72 04             	add    0x4(%edx),%esi
    1086:	89 73 fc             	mov    %esi,-0x4(%ebx)
    bp->s.ptr = p->s.ptr->s.ptr;
    1089:	8b 10                	mov    (%eax),%edx
    108b:	8b 12                	mov    (%edx),%edx
    108d:	89 53 f8             	mov    %edx,-0x8(%ebx)
  } else
    bp->s.ptr = p->s.ptr;
  if(p + p->s.size == bp){
    1090:	8b 50 04             	mov    0x4(%eax),%edx
    1093:	8d 34 d0             	lea    (%eax,%edx,8),%esi
    1096:	39 f1                	cmp    %esi,%ecx
    1098:	75 dd                	jne    1077 <free+0x57>
    p->s.size += bp->s.size;
    109a:	03 53 fc             	add    -0x4(%ebx),%edx
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
    109d:	a3 c4 18 00 00       	mov    %eax,0x18c4
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if(p + p->s.size == bp){
    p->s.size += bp->s.size;
    10a2:	89 50 04             	mov    %edx,0x4(%eax)
    p->s.ptr = bp->s.ptr;
    10a5:	8b 53 f8             	mov    -0x8(%ebx),%edx
    10a8:	89 10                	mov    %edx,(%eax)
  } else
    p->s.ptr = bp;
  freep = p;
}
    10aa:	5b                   	pop    %ebx
    10ab:	5e                   	pop    %esi
    10ac:	5f                   	pop    %edi
    10ad:	5d                   	pop    %ebp
    10ae:	c3                   	ret    
    10af:	90                   	nop

000010b0 <malloc>:
  return freep;
}

void*
malloc(uint nbytes)
{
    10b0:	55                   	push   %ebp
    10b1:	89 e5                	mov    %esp,%ebp
    10b3:	57                   	push   %edi
    10b4:	56                   	push   %esi
    10b5:	53                   	push   %ebx
    10b6:	83 ec 0c             	sub    $0xc,%esp
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
    10b9:	8b 45 08             	mov    0x8(%ebp),%eax
  if((prevp = freep) == 0){
    10bc:	8b 15 c4 18 00 00    	mov    0x18c4,%edx
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
    10c2:	8d 78 07             	lea    0x7(%eax),%edi
    10c5:	c1 ef 03             	shr    $0x3,%edi
    10c8:	83 c7 01             	add    $0x1,%edi
  if((prevp = freep) == 0){
    10cb:	85 d2                	test   %edx,%edx
    10cd:	0f 84 a3 00 00 00    	je     1176 <malloc+0xc6>
    10d3:	8b 02                	mov    (%edx),%eax
    10d5:	8b 48 04             	mov    0x4(%eax),%ecx
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
    10d8:	39 cf                	cmp    %ecx,%edi
    10da:	76 74                	jbe    1150 <malloc+0xa0>
    10dc:	81 ff 00 10 00 00    	cmp    $0x1000,%edi
    10e2:	be 00 10 00 00       	mov    $0x1000,%esi
    10e7:	8d 1c fd 00 00 00 00 	lea    0x0(,%edi,8),%ebx
    10ee:	0f 43 f7             	cmovae %edi,%esi
    10f1:	ba 00 80 00 00       	mov    $0x8000,%edx
    10f6:	81 ff ff 0f 00 00    	cmp    $0xfff,%edi
    10fc:	0f 46 da             	cmovbe %edx,%ebx
    10ff:	eb 10                	jmp    1111 <malloc+0x61>
    1101:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
  if((prevp = freep) == 0){
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    1108:	8b 02                	mov    (%edx),%eax
    if(p->s.size >= nunits){
    110a:	8b 48 04             	mov    0x4(%eax),%ecx
    110d:	39 cf                	cmp    %ecx,%edi
    110f:	76 3f                	jbe    1150 <malloc+0xa0>
        p->s.size = nunits;
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
    1111:	39 05 c4 18 00 00    	cmp    %eax,0x18c4
    1117:	89 c2                	mov    %eax,%edx
    1119:	75 ed                	jne    1108 <malloc+0x58>
  char *p;
  Header *hp;

  if(nu < 4096)
    nu = 4096;
  p = sbrk(nu * sizeof(Header));
    111b:	83 ec 0c             	sub    $0xc,%esp
    111e:	53                   	push   %ebx
    111f:	e8 86 fc ff ff       	call   daa <sbrk>
  if(p == (char*)-1)
    1124:	83 c4 10             	add    $0x10,%esp
    1127:	83 f8 ff             	cmp    $0xffffffff,%eax
    112a:	74 1c                	je     1148 <malloc+0x98>
    return 0;
  hp = (Header*)p;
  hp->s.size = nu;
    112c:	89 70 04             	mov    %esi,0x4(%eax)
  free((void*)(hp + 1));
    112f:	83 ec 0c             	sub    $0xc,%esp
    1132:	83 c0 08             	add    $0x8,%eax
    1135:	50                   	push   %eax
    1136:	e8 e5 fe ff ff       	call   1020 <free>
  return freep;
    113b:	8b 15 c4 18 00 00    	mov    0x18c4,%edx
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
      if((p = morecore(nunits)) == 0)
    1141:	83 c4 10             	add    $0x10,%esp
    1144:	85 d2                	test   %edx,%edx
    1146:	75 c0                	jne    1108 <malloc+0x58>
        return 0;
    1148:	31 c0                	xor    %eax,%eax
    114a:	eb 1c                	jmp    1168 <malloc+0xb8>
    114c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
      if(p->s.size == nunits)
    1150:	39 cf                	cmp    %ecx,%edi
    1152:	74 1c                	je     1170 <malloc+0xc0>
        prevp->s.ptr = p->s.ptr;
      else {
        p->s.size -= nunits;
    1154:	29 f9                	sub    %edi,%ecx
    1156:	89 48 04             	mov    %ecx,0x4(%eax)
        p += p->s.size;
    1159:	8d 04 c8             	lea    (%eax,%ecx,8),%eax
        p->s.size = nunits;
    115c:	89 78 04             	mov    %edi,0x4(%eax)
      }
      freep = prevp;
    115f:	89 15 c4 18 00 00    	mov    %edx,0x18c4
      return (void*)(p + 1);
    1165:	83 c0 08             	add    $0x8,%eax
    }
    if(p == freep)
      if((p = morecore(nunits)) == 0)
        return 0;
  }
}
    1168:	8d 65 f4             	lea    -0xc(%ebp),%esp
    116b:	5b                   	pop    %ebx
    116c:	5e                   	pop    %esi
    116d:	5f                   	pop    %edi
    116e:	5d                   	pop    %ebp
    116f:	c3                   	ret    
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
      if(p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
    1170:	8b 08                	mov    (%eax),%ecx
    1172:	89 0a                	mov    %ecx,(%edx)
    1174:	eb e9                	jmp    115f <malloc+0xaf>
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
  if((prevp = freep) == 0){
    base.s.ptr = freep = prevp = &base;
    1176:	c7 05 c4 18 00 00 c8 	movl   $0x18c8,0x18c4
    117d:	18 00 00 
    1180:	c7 05 c8 18 00 00 c8 	movl   $0x18c8,0x18c8
    1187:	18 00 00 
    base.s.size = 0;
    118a:	b8 c8 18 00 00       	mov    $0x18c8,%eax
    118f:	c7 05 cc 18 00 00 00 	movl   $0x0,0x18cc
    1196:	00 00 00 
    1199:	e9 3e ff ff ff       	jmp    10dc <malloc+0x2c>
