
_usertests:     file format elf32-i386


Disassembly of section .text:

00000000 <main>:
  return randstate;
}

int
main(int argc, char *argv[])
{
       0:	55                   	push   %ebp
       1:	89 e5                	mov    %esp,%ebp
       3:	83 e4 f0             	and    $0xfffffff0,%esp
       6:	83 ec 10             	sub    $0x10,%esp
  printf(1, "usertests starting\n");
       9:	c7 44 24 04 36 53 00 	movl   $0x5336,0x4(%esp)
      10:	00 
      11:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
      18:	e8 c3 3f 00 00       	call   3fe0 <printf>

  if(open("usertests.ran", 0) >= 0){
      1d:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
      24:	00 
      25:	c7 04 24 4a 53 00 00 	movl   $0x534a,(%esp)
      2c:	e8 a1 3e 00 00       	call   3ed2 <open>
      31:	85 c0                	test   %eax,%eax
      33:	78 20                	js     55 <main+0x55>
    printf(1, "already ran user tests -- rebuild fs.img\n");
      35:	c7 44 24 04 b4 5a 00 	movl   $0x5ab4,0x4(%esp)
      3c:	00 
      3d:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
      44:	e8 97 3f 00 00       	call   3fe0 <printf>
    exit(0);
      49:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
      50:	e8 3d 3e 00 00       	call   3e92 <exit>
  }
  close(open("usertests.ran", O_CREATE));
      55:	c7 44 24 04 00 02 00 	movl   $0x200,0x4(%esp)
      5c:	00 
      5d:	c7 04 24 4a 53 00 00 	movl   $0x534a,(%esp)
      64:	e8 69 3e 00 00       	call   3ed2 <open>
      69:	89 04 24             	mov    %eax,(%esp)
      6c:	e8 49 3e 00 00       	call   3eba <close>

  argptest();
      71:	e8 4a 3b 00 00       	call   3bc0 <argptest>
  createdelete();
      76:	e8 05 12 00 00       	call   1280 <createdelete>
      7b:	90                   	nop
      7c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  linkunlink();
      80:	e8 3b 1c 00 00       	call   1cc0 <linkunlink>
  concreate();
      85:	e8 f6 18 00 00       	call   1980 <concreate>
  fourfiles();
      8a:	e8 b1 0f 00 00       	call   1040 <fourfiles>
      8f:	90                   	nop
  sharedfd();
      90:	e8 bb 0d 00 00       	call   e50 <sharedfd>

  bigargtest();
      95:	e8 86 37 00 00       	call   3820 <bigargtest>
  bigwrite();
      9a:	e8 21 27 00 00       	call   27c0 <bigwrite>
      9f:	90                   	nop
  bigargtest();
      a0:	e8 7b 37 00 00       	call   3820 <bigargtest>
  bsstest();
      a5:	e8 f6 36 00 00       	call   37a0 <bsstest>
  sbrktest();
      aa:	e8 31 32 00 00       	call   32e0 <sbrktest>
      af:	90                   	nop
  validatetest();
      b0:	e8 3b 36 00 00       	call   36f0 <validatetest>

  opentest();
      b5:	e8 e6 02 00 00       	call   3a0 <opentest>
  writetest();
      ba:	e8 81 03 00 00       	call   440 <writetest>
      bf:	90                   	nop
  writetest1();
      c0:	e8 4b 05 00 00       	call   610 <writetest1>
  createtest();
      c5:	e8 06 07 00 00       	call   7d0 <createtest>

  openiputtest();
      ca:	e8 01 02 00 00       	call   2d0 <openiputtest>
      cf:	90                   	nop
  exitiputtest();
      d0:	e8 2b 01 00 00       	call   200 <exitiputtest>
  iputtest();
      d5:	e8 66 00 00 00       	call   140 <iputtest>

  mem();
      da:	e8 91 0c 00 00       	call   d70 <mem>
      df:	90                   	nop
  pipe1();
      e0:	e8 bb 08 00 00       	call   9a0 <pipe1>
  preempt();
      e5:	e8 96 0a 00 00       	call   b80 <preempt>
  exitwait();
      ea:	e8 f1 0b 00 00       	call   ce0 <exitwait>
      ef:	90                   	nop

  rmdot();
      f0:	e8 8b 2b 00 00       	call   2c80 <rmdot>
  fourteen();
      f5:	e8 06 2a 00 00       	call   2b00 <fourteen>
  bigfile();
      fa:	e8 d1 27 00 00       	call   28d0 <bigfile>
      ff:	90                   	nop
  subdir();
     100:	e8 3b 1e 00 00       	call   1f40 <subdir>
  linktest();
     105:	e8 d6 15 00 00       	call   16e0 <linktest>
  unlinkread();
     10a:	e8 d1 13 00 00       	call   14e0 <unlinkread>
     10f:	90                   	nop
  dirfile();
     110:	e8 3b 2d 00 00       	call   2e50 <dirfile>
  iref();
     115:	e8 b6 2f 00 00       	call   30d0 <iref>
  forktest();
     11a:	e8 e1 30 00 00       	call   3200 <forktest>
     11f:	90                   	nop
  bigdir(); // slow
     120:	e8 bb 1c 00 00       	call   1de0 <bigdir>

  uio();
     125:	e8 06 3a 00 00       	call   3b30 <uio>

  exectest();
     12a:	e8 11 08 00 00       	call   940 <exectest>

  exit(0);
     12f:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     136:	e8 57 3d 00 00       	call   3e92 <exit>
     13b:	66 90                	xchg   %ax,%ax
     13d:	66 90                	xchg   %ax,%ax
     13f:	90                   	nop

00000140 <iputtest>:
{
     140:	55                   	push   %ebp
     141:	89 e5                	mov    %esp,%ebp
     143:	83 ec 18             	sub    $0x18,%esp
  printf(stdout, "iput test\n");
     146:	a1 c4 63 00 00       	mov    0x63c4,%eax
     14b:	c7 44 24 04 dc 43 00 	movl   $0x43dc,0x4(%esp)
     152:	00 
     153:	89 04 24             	mov    %eax,(%esp)
     156:	e8 85 3e 00 00       	call   3fe0 <printf>
  if(mkdir("iputdir") < 0){
     15b:	c7 04 24 6f 43 00 00 	movl   $0x436f,(%esp)
     162:	e8 93 3d 00 00       	call   3efa <mkdir>
     167:	85 c0                	test   %eax,%eax
     169:	78 47                	js     1b2 <iputtest+0x72>
  if(chdir("iputdir") < 0){
     16b:	c7 04 24 6f 43 00 00 	movl   $0x436f,(%esp)
     172:	e8 8b 3d 00 00       	call   3f02 <chdir>
     177:	85 c0                	test   %eax,%eax
     179:	78 6c                	js     1e7 <iputtest+0xa7>
  if(unlink("../iputdir") < 0){
     17b:	c7 04 24 6c 43 00 00 	movl   $0x436c,(%esp)
     182:	e8 5b 3d 00 00       	call   3ee2 <unlink>
     187:	85 c0                	test   %eax,%eax
     189:	78 52                	js     1dd <iputtest+0x9d>
  if(chdir("/") < 0){
     18b:	c7 04 24 91 43 00 00 	movl   $0x4391,(%esp)
     192:	e8 6b 3d 00 00       	call   3f02 <chdir>
     197:	85 c0                	test   %eax,%eax
     199:	78 38                	js     1d3 <iputtest+0x93>
  printf(stdout, "iput test ok\n");
     19b:	a1 c4 63 00 00       	mov    0x63c4,%eax
     1a0:	c7 44 24 04 14 44 00 	movl   $0x4414,0x4(%esp)
     1a7:	00 
     1a8:	89 04 24             	mov    %eax,(%esp)
     1ab:	e8 30 3e 00 00       	call   3fe0 <printf>
}
     1b0:	c9                   	leave  
     1b1:	c3                   	ret    
    printf(stdout, "mkdir failed\n");
     1b2:	c7 44 24 04 48 43 00 	movl   $0x4348,0x4(%esp)
     1b9:	00 
    printf(stdout, "chdir iputdir failed\n");
     1ba:	a1 c4 63 00 00       	mov    0x63c4,%eax
     1bf:	89 04 24             	mov    %eax,(%esp)
     1c2:	e8 19 3e 00 00       	call   3fe0 <printf>
    exit(0);
     1c7:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     1ce:	e8 bf 3c 00 00       	call   3e92 <exit>
    printf(stdout, "chdir / failed\n");
     1d3:	c7 44 24 04 93 43 00 	movl   $0x4393,0x4(%esp)
     1da:	00 
     1db:	eb dd                	jmp    1ba <iputtest+0x7a>
    printf(stdout, "unlink ../iputdir failed\n");
     1dd:	c7 44 24 04 77 43 00 	movl   $0x4377,0x4(%esp)
     1e4:	00 
     1e5:	eb d3                	jmp    1ba <iputtest+0x7a>
    printf(stdout, "chdir iputdir failed\n");
     1e7:	c7 44 24 04 56 43 00 	movl   $0x4356,0x4(%esp)
     1ee:	00 
     1ef:	eb c9                	jmp    1ba <iputtest+0x7a>
     1f1:	eb 0d                	jmp    200 <exitiputtest>
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

00000200 <exitiputtest>:
{
     200:	55                   	push   %ebp
     201:	89 e5                	mov    %esp,%ebp
     203:	83 ec 18             	sub    $0x18,%esp
  printf(stdout, "exitiput test\n");
     206:	a1 c4 63 00 00       	mov    0x63c4,%eax
     20b:	c7 44 24 04 a3 43 00 	movl   $0x43a3,0x4(%esp)
     212:	00 
     213:	89 04 24             	mov    %eax,(%esp)
     216:	e8 c5 3d 00 00       	call   3fe0 <printf>
  pid = fork();
     21b:	e8 6a 3c 00 00       	call   3e8a <fork>
  if(pid < 0){
     220:	85 c0                	test   %eax,%eax
     222:	0f 88 89 00 00 00    	js     2b1 <exitiputtest+0xb1>
  if(pid == 0){
     228:	75 46                	jne    270 <exitiputtest+0x70>
    if(mkdir("iputdir") < 0){
     22a:	c7 04 24 6f 43 00 00 	movl   $0x436f,(%esp)
     231:	e8 c4 3c 00 00       	call   3efa <mkdir>
     236:	85 c0                	test   %eax,%eax
     238:	0f 88 87 00 00 00    	js     2c5 <exitiputtest+0xc5>
    if(chdir("iputdir") < 0){
     23e:	c7 04 24 6f 43 00 00 	movl   $0x436f,(%esp)
     245:	e8 b8 3c 00 00       	call   3f02 <chdir>
     24a:	85 c0                	test   %eax,%eax
     24c:	78 6d                	js     2bb <exitiputtest+0xbb>
    if(unlink("../iputdir") < 0){
     24e:	c7 04 24 6c 43 00 00 	movl   $0x436c,(%esp)
     255:	e8 88 3c 00 00       	call   3ee2 <unlink>
     25a:	85 c0                	test   %eax,%eax
     25c:	78 32                	js     290 <exitiputtest+0x90>
      exit(0);
     25e:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     265:	e8 28 3c 00 00       	call   3e92 <exit>
     26a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  wait();
     270:	e8 25 3c 00 00       	call   3e9a <wait>
  printf(stdout, "exitiput test ok\n");
     275:	a1 c4 63 00 00       	mov    0x63c4,%eax
     27a:	c7 44 24 04 c6 43 00 	movl   $0x43c6,0x4(%esp)
     281:	00 
     282:	89 04 24             	mov    %eax,(%esp)
     285:	e8 56 3d 00 00       	call   3fe0 <printf>
}
     28a:	c9                   	leave  
     28b:	c3                   	ret    
     28c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      printf(stdout, "unlink ../iputdir failed\n");
     290:	c7 44 24 04 77 43 00 	movl   $0x4377,0x4(%esp)
     297:	00 
      printf(stdout, "mkdir failed\n");
     298:	a1 c4 63 00 00       	mov    0x63c4,%eax
     29d:	89 04 24             	mov    %eax,(%esp)
     2a0:	e8 3b 3d 00 00       	call   3fe0 <printf>
      exit(0);
     2a5:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     2ac:	e8 e1 3b 00 00       	call   3e92 <exit>
    printf(stdout, "fork failed\n");
     2b1:	c7 44 24 04 89 52 00 	movl   $0x5289,0x4(%esp)
     2b8:	00 
     2b9:	eb dd                	jmp    298 <exitiputtest+0x98>
      printf(stdout, "child chdir failed\n");
     2bb:	c7 44 24 04 b2 43 00 	movl   $0x43b2,0x4(%esp)
     2c2:	00 
     2c3:	eb d3                	jmp    298 <exitiputtest+0x98>
      printf(stdout, "mkdir failed\n");
     2c5:	c7 44 24 04 48 43 00 	movl   $0x4348,0x4(%esp)
     2cc:	00 
     2cd:	eb c9                	jmp    298 <exitiputtest+0x98>
     2cf:	90                   	nop

000002d0 <openiputtest>:
{
     2d0:	55                   	push   %ebp
     2d1:	89 e5                	mov    %esp,%ebp
     2d3:	83 ec 18             	sub    $0x18,%esp
  printf(stdout, "openiput test\n");
     2d6:	a1 c4 63 00 00       	mov    0x63c4,%eax
     2db:	c7 44 24 04 d8 43 00 	movl   $0x43d8,0x4(%esp)
     2e2:	00 
     2e3:	89 04 24             	mov    %eax,(%esp)
     2e6:	e8 f5 3c 00 00       	call   3fe0 <printf>
  if(mkdir("oidir") < 0){
     2eb:	c7 04 24 e7 43 00 00 	movl   $0x43e7,(%esp)
     2f2:	e8 03 3c 00 00       	call   3efa <mkdir>
     2f7:	85 c0                	test   %eax,%eax
     2f9:	0f 88 8b 00 00 00    	js     38a <openiputtest+0xba>
  pid = fork();
     2ff:	e8 86 3b 00 00       	call   3e8a <fork>
  if(pid < 0){
     304:	85 c0                	test   %eax,%eax
     306:	78 78                	js     380 <openiputtest+0xb0>
  if(pid == 0){
     308:	75 3e                	jne    348 <openiputtest+0x78>
    int fd = open("oidir", O_RDWR);
     30a:	c7 44 24 04 02 00 00 	movl   $0x2,0x4(%esp)
     311:	00 
     312:	c7 04 24 e7 43 00 00 	movl   $0x43e7,(%esp)
     319:	e8 b4 3b 00 00       	call   3ed2 <open>
    if(fd >= 0){
     31e:	85 c0                	test   %eax,%eax
     320:	78 15                	js     337 <openiputtest+0x67>
      printf(stdout, "open directory for write succeeded\n");
     322:	c7 44 24 04 6c 53 00 	movl   $0x536c,0x4(%esp)
     329:	00 
    printf(stdout, "fork failed\n");
     32a:	a1 c4 63 00 00       	mov    0x63c4,%eax
     32f:	89 04 24             	mov    %eax,(%esp)
     332:	e8 a9 3c 00 00       	call   3fe0 <printf>
    exit(0);
     337:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     33e:	e8 4f 3b 00 00       	call   3e92 <exit>
     343:	90                   	nop
     344:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  sleep(1);
     348:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     34f:	e8 ce 3b 00 00       	call   3f22 <sleep>
  if(unlink("oidir") != 0){
     354:	c7 04 24 e7 43 00 00 	movl   $0x43e7,(%esp)
     35b:	e8 82 3b 00 00       	call   3ee2 <unlink>
     360:	85 c0                	test   %eax,%eax
     362:	75 30                	jne    394 <openiputtest+0xc4>
  wait();
     364:	e8 31 3b 00 00       	call   3e9a <wait>
  printf(stdout, "openiput test ok\n");
     369:	a1 c4 63 00 00       	mov    0x63c4,%eax
     36e:	c7 44 24 04 10 44 00 	movl   $0x4410,0x4(%esp)
     375:	00 
     376:	89 04 24             	mov    %eax,(%esp)
     379:	e8 62 3c 00 00       	call   3fe0 <printf>
}
     37e:	c9                   	leave  
     37f:	c3                   	ret    
    printf(stdout, "fork failed\n");
     380:	c7 44 24 04 89 52 00 	movl   $0x5289,0x4(%esp)
     387:	00 
     388:	eb a0                	jmp    32a <openiputtest+0x5a>
    printf(stdout, "mkdir oidir failed\n");
     38a:	c7 44 24 04 ed 43 00 	movl   $0x43ed,0x4(%esp)
     391:	00 
     392:	eb 96                	jmp    32a <openiputtest+0x5a>
    printf(stdout, "unlink failed\n");
     394:	c7 44 24 04 01 44 00 	movl   $0x4401,0x4(%esp)
     39b:	00 
     39c:	eb 8c                	jmp    32a <openiputtest+0x5a>
     39e:	66 90                	xchg   %ax,%ax

000003a0 <opentest>:
{
     3a0:	55                   	push   %ebp
     3a1:	89 e5                	mov    %esp,%ebp
     3a3:	83 ec 18             	sub    $0x18,%esp
  printf(stdout, "open test\n");
     3a6:	a1 c4 63 00 00       	mov    0x63c4,%eax
     3ab:	c7 44 24 04 22 44 00 	movl   $0x4422,0x4(%esp)
     3b2:	00 
     3b3:	89 04 24             	mov    %eax,(%esp)
     3b6:	e8 25 3c 00 00       	call   3fe0 <printf>
  fd = open("echo", 0);
     3bb:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
     3c2:	00 
     3c3:	c7 04 24 2d 44 00 00 	movl   $0x442d,(%esp)
     3ca:	e8 03 3b 00 00       	call   3ed2 <open>
  if(fd < 0){
     3cf:	85 c0                	test   %eax,%eax
     3d1:	78 37                	js     40a <opentest+0x6a>
  close(fd);
     3d3:	89 04 24             	mov    %eax,(%esp)
     3d6:	e8 df 3a 00 00       	call   3eba <close>
  fd = open("doesnotexist", 0);
     3db:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
     3e2:	00 
     3e3:	c7 04 24 45 44 00 00 	movl   $0x4445,(%esp)
     3ea:	e8 e3 3a 00 00       	call   3ed2 <open>
  if(fd >= 0){
     3ef:	85 c0                	test   %eax,%eax
     3f1:	79 38                	jns    42b <opentest+0x8b>
  printf(stdout, "open test ok\n");
     3f3:	a1 c4 63 00 00       	mov    0x63c4,%eax
     3f8:	c7 44 24 04 70 44 00 	movl   $0x4470,0x4(%esp)
     3ff:	00 
     400:	89 04 24             	mov    %eax,(%esp)
     403:	e8 d8 3b 00 00       	call   3fe0 <printf>
}
     408:	c9                   	leave  
     409:	c3                   	ret    
    printf(stdout, "open echo failed!\n");
     40a:	c7 44 24 04 32 44 00 	movl   $0x4432,0x4(%esp)
     411:	00 
    printf(stdout, "open doesnotexist succeeded!\n");
     412:	a1 c4 63 00 00       	mov    0x63c4,%eax
     417:	89 04 24             	mov    %eax,(%esp)
     41a:	e8 c1 3b 00 00       	call   3fe0 <printf>
    exit(0);
     41f:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     426:	e8 67 3a 00 00       	call   3e92 <exit>
    printf(stdout, "open doesnotexist succeeded!\n");
     42b:	c7 44 24 04 52 44 00 	movl   $0x4452,0x4(%esp)
     432:	00 
     433:	eb dd                	jmp    412 <opentest+0x72>
     435:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
     439:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000440 <writetest>:
{
     440:	55                   	push   %ebp
     441:	89 e5                	mov    %esp,%ebp
     443:	56                   	push   %esi
     444:	53                   	push   %ebx
     445:	83 ec 10             	sub    $0x10,%esp
  printf(stdout, "small file test\n");
     448:	a1 c4 63 00 00       	mov    0x63c4,%eax
     44d:	c7 44 24 04 7e 44 00 	movl   $0x447e,0x4(%esp)
     454:	00 
     455:	89 04 24             	mov    %eax,(%esp)
     458:	e8 83 3b 00 00       	call   3fe0 <printf>
  fd = open("small", O_CREATE|O_RDWR);
     45d:	c7 44 24 04 02 02 00 	movl   $0x202,0x4(%esp)
     464:	00 
     465:	c7 04 24 8f 44 00 00 	movl   $0x448f,(%esp)
     46c:	e8 61 3a 00 00       	call   3ed2 <open>
  if(fd >= 0){
     471:	85 c0                	test   %eax,%eax
  fd = open("small", O_CREATE|O_RDWR);
     473:	89 c6                	mov    %eax,%esi
  if(fd >= 0){
     475:	0f 88 8b 01 00 00    	js     606 <writetest+0x1c6>
    printf(stdout, "creat small succeeded; ok\n");
     47b:	a1 c4 63 00 00       	mov    0x63c4,%eax
  for(i = 0; i < 100; i++){
     480:	31 db                	xor    %ebx,%ebx
    printf(stdout, "creat small succeeded; ok\n");
     482:	c7 44 24 04 95 44 00 	movl   $0x4495,0x4(%esp)
     489:	00 
     48a:	89 04 24             	mov    %eax,(%esp)
     48d:	e8 4e 3b 00 00       	call   3fe0 <printf>
     492:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    if(write(fd, "aaaaaaaaaa", 10) != 10){
     498:	c7 44 24 08 0a 00 00 	movl   $0xa,0x8(%esp)
     49f:	00 
     4a0:	c7 44 24 04 cc 44 00 	movl   $0x44cc,0x4(%esp)
     4a7:	00 
     4a8:	89 34 24             	mov    %esi,(%esp)
     4ab:	e8 02 3a 00 00       	call   3eb2 <write>
     4b0:	83 f8 0a             	cmp    $0xa,%eax
     4b3:	0f 85 e5 00 00 00    	jne    59e <writetest+0x15e>
    if(write(fd, "bbbbbbbbbb", 10) != 10){
     4b9:	c7 44 24 08 0a 00 00 	movl   $0xa,0x8(%esp)
     4c0:	00 
     4c1:	c7 44 24 04 d7 44 00 	movl   $0x44d7,0x4(%esp)
     4c8:	00 
     4c9:	89 34 24             	mov    %esi,(%esp)
     4cc:	e8 e1 39 00 00       	call   3eb2 <write>
     4d1:	83 f8 0a             	cmp    $0xa,%eax
     4d4:	0f 85 e9 00 00 00    	jne    5c3 <writetest+0x183>
  for(i = 0; i < 100; i++){
     4da:	83 c3 01             	add    $0x1,%ebx
     4dd:	83 fb 64             	cmp    $0x64,%ebx
     4e0:	75 b6                	jne    498 <writetest+0x58>
  printf(stdout, "writes ok\n");
     4e2:	a1 c4 63 00 00       	mov    0x63c4,%eax
     4e7:	c7 44 24 04 e2 44 00 	movl   $0x44e2,0x4(%esp)
     4ee:	00 
     4ef:	89 04 24             	mov    %eax,(%esp)
     4f2:	e8 e9 3a 00 00       	call   3fe0 <printf>
  close(fd);
     4f7:	89 34 24             	mov    %esi,(%esp)
     4fa:	e8 bb 39 00 00       	call   3eba <close>
  fd = open("small", O_RDONLY);
     4ff:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
     506:	00 
     507:	c7 04 24 8f 44 00 00 	movl   $0x448f,(%esp)
     50e:	e8 bf 39 00 00       	call   3ed2 <open>
  if(fd >= 0){
     513:	85 c0                	test   %eax,%eax
  fd = open("small", O_RDONLY);
     515:	89 c3                	mov    %eax,%ebx
  if(fd >= 0){
     517:	0f 88 b4 00 00 00    	js     5d1 <writetest+0x191>
    printf(stdout, "open small succeeded ok\n");
     51d:	a1 c4 63 00 00       	mov    0x63c4,%eax
     522:	c7 44 24 04 ed 44 00 	movl   $0x44ed,0x4(%esp)
     529:	00 
     52a:	89 04 24             	mov    %eax,(%esp)
     52d:	e8 ae 3a 00 00       	call   3fe0 <printf>
  i = read(fd, buf, 2000);
     532:	c7 44 24 08 d0 07 00 	movl   $0x7d0,0x8(%esp)
     539:	00 
     53a:	c7 44 24 04 a0 8b 00 	movl   $0x8ba0,0x4(%esp)
     541:	00 
     542:	89 1c 24             	mov    %ebx,(%esp)
     545:	e8 60 39 00 00       	call   3eaa <read>
  if(i == 2000){
     54a:	3d d0 07 00 00       	cmp    $0x7d0,%eax
     54f:	0f 85 9d 00 00 00    	jne    5f2 <writetest+0x1b2>
    printf(stdout, "read succeeded ok\n");
     555:	a1 c4 63 00 00       	mov    0x63c4,%eax
     55a:	c7 44 24 04 21 45 00 	movl   $0x4521,0x4(%esp)
     561:	00 
     562:	89 04 24             	mov    %eax,(%esp)
     565:	e8 76 3a 00 00       	call   3fe0 <printf>
  close(fd);
     56a:	89 1c 24             	mov    %ebx,(%esp)
     56d:	e8 48 39 00 00       	call   3eba <close>
  if(unlink("small") < 0){
     572:	c7 04 24 8f 44 00 00 	movl   $0x448f,(%esp)
     579:	e8 64 39 00 00       	call   3ee2 <unlink>
     57e:	85 c0                	test   %eax,%eax
     580:	78 7a                	js     5fc <writetest+0x1bc>
  printf(stdout, "small file test ok\n");
     582:	a1 c4 63 00 00       	mov    0x63c4,%eax
     587:	c7 44 24 04 49 45 00 	movl   $0x4549,0x4(%esp)
     58e:	00 
     58f:	89 04 24             	mov    %eax,(%esp)
     592:	e8 49 3a 00 00       	call   3fe0 <printf>
}
     597:	83 c4 10             	add    $0x10,%esp
     59a:	5b                   	pop    %ebx
     59b:	5e                   	pop    %esi
     59c:	5d                   	pop    %ebp
     59d:	c3                   	ret    
      printf(stdout, "error: write aa %d new file failed\n", i);
     59e:	89 5c 24 08          	mov    %ebx,0x8(%esp)
     5a2:	c7 44 24 04 90 53 00 	movl   $0x5390,0x4(%esp)
     5a9:	00 
      printf(stdout, "error: write bb %d new file failed\n", i);
     5aa:	a1 c4 63 00 00       	mov    0x63c4,%eax
     5af:	89 04 24             	mov    %eax,(%esp)
     5b2:	e8 29 3a 00 00       	call   3fe0 <printf>
      exit(0);
     5b7:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     5be:	e8 cf 38 00 00       	call   3e92 <exit>
      printf(stdout, "error: write bb %d new file failed\n", i);
     5c3:	89 5c 24 08          	mov    %ebx,0x8(%esp)
     5c7:	c7 44 24 04 b4 53 00 	movl   $0x53b4,0x4(%esp)
     5ce:	00 
     5cf:	eb d9                	jmp    5aa <writetest+0x16a>
    printf(stdout, "error: open small failed!\n");
     5d1:	c7 44 24 04 06 45 00 	movl   $0x4506,0x4(%esp)
     5d8:	00 
     5d9:	a1 c4 63 00 00       	mov    0x63c4,%eax
     5de:	89 04 24             	mov    %eax,(%esp)
     5e1:	e8 fa 39 00 00       	call   3fe0 <printf>
    exit(0);
     5e6:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     5ed:	e8 a0 38 00 00       	call   3e92 <exit>
    printf(stdout, "read failed\n");
     5f2:	c7 44 24 04 4d 48 00 	movl   $0x484d,0x4(%esp)
     5f9:	00 
     5fa:	eb dd                	jmp    5d9 <writetest+0x199>
    printf(stdout, "unlink small failed\n");
     5fc:	c7 44 24 04 34 45 00 	movl   $0x4534,0x4(%esp)
     603:	00 
     604:	eb d3                	jmp    5d9 <writetest+0x199>
    printf(stdout, "error: creat small failed!\n");
     606:	c7 44 24 04 b0 44 00 	movl   $0x44b0,0x4(%esp)
     60d:	00 
     60e:	eb c9                	jmp    5d9 <writetest+0x199>

00000610 <writetest1>:
{
     610:	55                   	push   %ebp
     611:	89 e5                	mov    %esp,%ebp
     613:	56                   	push   %esi
     614:	53                   	push   %ebx
     615:	83 ec 10             	sub    $0x10,%esp
  printf(stdout, "big files test\n");
     618:	a1 c4 63 00 00       	mov    0x63c4,%eax
     61d:	c7 44 24 04 5d 45 00 	movl   $0x455d,0x4(%esp)
     624:	00 
     625:	89 04 24             	mov    %eax,(%esp)
     628:	e8 b3 39 00 00       	call   3fe0 <printf>
  fd = open("big", O_CREATE|O_RDWR);
     62d:	c7 44 24 04 02 02 00 	movl   $0x202,0x4(%esp)
     634:	00 
     635:	c7 04 24 d7 45 00 00 	movl   $0x45d7,(%esp)
     63c:	e8 91 38 00 00       	call   3ed2 <open>
  if(fd < 0){
     641:	85 c0                	test   %eax,%eax
  fd = open("big", O_CREATE|O_RDWR);
     643:	89 c6                	mov    %eax,%esi
  if(fd < 0){
     645:	0f 88 6f 01 00 00    	js     7ba <writetest1+0x1aa>
     64b:	31 db                	xor    %ebx,%ebx
     64d:	8d 76 00             	lea    0x0(%esi),%esi
    if(write(fd, buf, 512) != 512){
     650:	c7 44 24 08 00 02 00 	movl   $0x200,0x8(%esp)
     657:	00 
     658:	c7 44 24 04 a0 8b 00 	movl   $0x8ba0,0x4(%esp)
     65f:	00 
     660:	89 34 24             	mov    %esi,(%esp)
    ((int*)buf)[0] = i;
     663:	89 1d a0 8b 00 00    	mov    %ebx,0x8ba0
    if(write(fd, buf, 512) != 512){
     669:	e8 44 38 00 00       	call   3eb2 <write>
     66e:	3d 00 02 00 00       	cmp    $0x200,%eax
     673:	0f 85 b2 00 00 00    	jne    72b <writetest1+0x11b>
  for(i = 0; i < MAXFILE; i++){
     679:	83 c3 01             	add    $0x1,%ebx
     67c:	81 fb 8c 00 00 00    	cmp    $0x8c,%ebx
     682:	75 cc                	jne    650 <writetest1+0x40>
  close(fd);
     684:	89 34 24             	mov    %esi,(%esp)
     687:	e8 2e 38 00 00       	call   3eba <close>
  fd = open("big", O_RDONLY);
     68c:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
     693:	00 
     694:	c7 04 24 d7 45 00 00 	movl   $0x45d7,(%esp)
     69b:	e8 32 38 00 00       	call   3ed2 <open>
  if(fd < 0){
     6a0:	85 c0                	test   %eax,%eax
  fd = open("big", O_RDONLY);
     6a2:	89 c6                	mov    %eax,%esi
  if(fd < 0){
     6a4:	0f 88 ef 00 00 00    	js     799 <writetest1+0x189>
     6aa:	31 db                	xor    %ebx,%ebx
     6ac:	eb 1d                	jmp    6cb <writetest1+0xbb>
     6ae:	66 90                	xchg   %ax,%ax
    } else if(i != 512){
     6b0:	3d 00 02 00 00       	cmp    $0x200,%eax
     6b5:	0f 85 be 00 00 00    	jne    779 <writetest1+0x169>
    if(((int*)buf)[0] != n){
     6bb:	a1 a0 8b 00 00       	mov    0x8ba0,%eax
     6c0:	39 d8                	cmp    %ebx,%eax
     6c2:	0f 85 88 00 00 00    	jne    750 <writetest1+0x140>
    n++;
     6c8:	83 c3 01             	add    $0x1,%ebx
    i = read(fd, buf, 512);
     6cb:	c7 44 24 08 00 02 00 	movl   $0x200,0x8(%esp)
     6d2:	00 
     6d3:	c7 44 24 04 a0 8b 00 	movl   $0x8ba0,0x4(%esp)
     6da:	00 
     6db:	89 34 24             	mov    %esi,(%esp)
     6de:	e8 c7 37 00 00       	call   3eaa <read>
    if(i == 0){
     6e3:	85 c0                	test   %eax,%eax
     6e5:	75 c9                	jne    6b0 <writetest1+0xa0>
      if(n == MAXFILE - 1){
     6e7:	81 fb 8b 00 00 00    	cmp    $0x8b,%ebx
     6ed:	0f 84 94 00 00 00    	je     787 <writetest1+0x177>
  close(fd);
     6f3:	89 34 24             	mov    %esi,(%esp)
     6f6:	e8 bf 37 00 00       	call   3eba <close>
  if(unlink("big") < 0){
     6fb:	c7 04 24 d7 45 00 00 	movl   $0x45d7,(%esp)
     702:	e8 db 37 00 00       	call   3ee2 <unlink>
     707:	85 c0                	test   %eax,%eax
     709:	0f 88 b5 00 00 00    	js     7c4 <writetest1+0x1b4>
  printf(stdout, "big files ok\n");
     70f:	a1 c4 63 00 00       	mov    0x63c4,%eax
     714:	c7 44 24 04 fe 45 00 	movl   $0x45fe,0x4(%esp)
     71b:	00 
     71c:	89 04 24             	mov    %eax,(%esp)
     71f:	e8 bc 38 00 00       	call   3fe0 <printf>
}
     724:	83 c4 10             	add    $0x10,%esp
     727:	5b                   	pop    %ebx
     728:	5e                   	pop    %esi
     729:	5d                   	pop    %ebp
     72a:	c3                   	ret    
      printf(stdout, "error: write big file failed\n", i);
     72b:	89 5c 24 08          	mov    %ebx,0x8(%esp)
     72f:	c7 44 24 04 87 45 00 	movl   $0x4587,0x4(%esp)
     736:	00 
        printf(stdout, "read only %d blocks from big", n);
     737:	a1 c4 63 00 00       	mov    0x63c4,%eax
     73c:	89 04 24             	mov    %eax,(%esp)
     73f:	e8 9c 38 00 00       	call   3fe0 <printf>
        exit(0);
     744:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     74b:	e8 42 37 00 00       	call   3e92 <exit>
      printf(stdout, "read content of block %d is %d\n",
     750:	89 44 24 0c          	mov    %eax,0xc(%esp)
     754:	a1 c4 63 00 00       	mov    0x63c4,%eax
     759:	89 5c 24 08          	mov    %ebx,0x8(%esp)
     75d:	c7 44 24 04 d8 53 00 	movl   $0x53d8,0x4(%esp)
     764:	00 
     765:	89 04 24             	mov    %eax,(%esp)
     768:	e8 73 38 00 00       	call   3fe0 <printf>
      exit(0);
     76d:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     774:	e8 19 37 00 00       	call   3e92 <exit>
      printf(stdout, "read failed %d\n", i);
     779:	89 44 24 08          	mov    %eax,0x8(%esp)
     77d:	c7 44 24 04 db 45 00 	movl   $0x45db,0x4(%esp)
     784:	00 
     785:	eb b0                	jmp    737 <writetest1+0x127>
        printf(stdout, "read only %d blocks from big", n);
     787:	c7 44 24 08 8b 00 00 	movl   $0x8b,0x8(%esp)
     78e:	00 
     78f:	c7 44 24 04 be 45 00 	movl   $0x45be,0x4(%esp)
     796:	00 
     797:	eb 9e                	jmp    737 <writetest1+0x127>
    printf(stdout, "error: open big failed!\n");
     799:	c7 44 24 04 a5 45 00 	movl   $0x45a5,0x4(%esp)
     7a0:	00 
     7a1:	a1 c4 63 00 00       	mov    0x63c4,%eax
     7a6:	89 04 24             	mov    %eax,(%esp)
     7a9:	e8 32 38 00 00       	call   3fe0 <printf>
    exit(0);
     7ae:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     7b5:	e8 d8 36 00 00       	call   3e92 <exit>
    printf(stdout, "error: creat big failed!\n");
     7ba:	c7 44 24 04 6d 45 00 	movl   $0x456d,0x4(%esp)
     7c1:	00 
     7c2:	eb dd                	jmp    7a1 <writetest1+0x191>
    printf(stdout, "unlink big failed\n");
     7c4:	c7 44 24 04 eb 45 00 	movl   $0x45eb,0x4(%esp)
     7cb:	00 
     7cc:	eb d3                	jmp    7a1 <writetest1+0x191>
     7ce:	66 90                	xchg   %ax,%ax

000007d0 <createtest>:
{
     7d0:	55                   	push   %ebp
     7d1:	89 e5                	mov    %esp,%ebp
     7d3:	53                   	push   %ebx
  name[2] = '\0';
     7d4:	bb 30 00 00 00       	mov    $0x30,%ebx
{
     7d9:	83 ec 14             	sub    $0x14,%esp
  printf(stdout, "many creates, followed by unlink test\n");
     7dc:	a1 c4 63 00 00       	mov    0x63c4,%eax
     7e1:	c7 44 24 04 f8 53 00 	movl   $0x53f8,0x4(%esp)
     7e8:	00 
     7e9:	89 04 24             	mov    %eax,(%esp)
     7ec:	e8 ef 37 00 00       	call   3fe0 <printf>
  name[0] = 'a';
     7f1:	c6 05 a0 ab 00 00 61 	movb   $0x61,0xaba0
  name[2] = '\0';
     7f8:	c6 05 a2 ab 00 00 00 	movb   $0x0,0xaba2
     7ff:	90                   	nop
    fd = open(name, O_CREATE|O_RDWR);
     800:	c7 44 24 04 02 02 00 	movl   $0x202,0x4(%esp)
     807:	00 
     808:	c7 04 24 a0 ab 00 00 	movl   $0xaba0,(%esp)
    name[1] = '0' + i;
     80f:	88 1d a1 ab 00 00    	mov    %bl,0xaba1
     815:	83 c3 01             	add    $0x1,%ebx
    fd = open(name, O_CREATE|O_RDWR);
     818:	e8 b5 36 00 00       	call   3ed2 <open>
    close(fd);
     81d:	89 04 24             	mov    %eax,(%esp)
     820:	e8 95 36 00 00       	call   3eba <close>
  for(i = 0; i < 52; i++){
     825:	80 fb 64             	cmp    $0x64,%bl
     828:	75 d6                	jne    800 <createtest+0x30>
  name[0] = 'a';
     82a:	c6 05 a0 ab 00 00 61 	movb   $0x61,0xaba0
  name[2] = '\0';
     831:	bb 30 00 00 00       	mov    $0x30,%ebx
     836:	c6 05 a2 ab 00 00 00 	movb   $0x0,0xaba2
     83d:	8d 76 00             	lea    0x0(%esi),%esi
    name[1] = '0' + i;
     840:	88 1d a1 ab 00 00    	mov    %bl,0xaba1
     846:	83 c3 01             	add    $0x1,%ebx
    unlink(name);
     849:	c7 04 24 a0 ab 00 00 	movl   $0xaba0,(%esp)
     850:	e8 8d 36 00 00       	call   3ee2 <unlink>
  for(i = 0; i < 52; i++){
     855:	80 fb 64             	cmp    $0x64,%bl
     858:	75 e6                	jne    840 <createtest+0x70>
  printf(stdout, "many creates, followed by unlink; ok\n");
     85a:	a1 c4 63 00 00       	mov    0x63c4,%eax
     85f:	c7 44 24 04 20 54 00 	movl   $0x5420,0x4(%esp)
     866:	00 
     867:	89 04 24             	mov    %eax,(%esp)
     86a:	e8 71 37 00 00       	call   3fe0 <printf>
}
     86f:	83 c4 14             	add    $0x14,%esp
     872:	5b                   	pop    %ebx
     873:	5d                   	pop    %ebp
     874:	c3                   	ret    
     875:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
     879:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00000880 <dirtest>:
{
     880:	55                   	push   %ebp
     881:	89 e5                	mov    %esp,%ebp
     883:	83 ec 18             	sub    $0x18,%esp
  printf(stdout, "mkdir test\n");
     886:	a1 c4 63 00 00       	mov    0x63c4,%eax
     88b:	c7 44 24 04 0c 46 00 	movl   $0x460c,0x4(%esp)
     892:	00 
     893:	89 04 24             	mov    %eax,(%esp)
     896:	e8 45 37 00 00       	call   3fe0 <printf>
  if(mkdir("dir0") < 0){
     89b:	c7 04 24 18 46 00 00 	movl   $0x4618,(%esp)
     8a2:	e8 53 36 00 00       	call   3efa <mkdir>
     8a7:	85 c0                	test   %eax,%eax
     8a9:	78 47                	js     8f2 <dirtest+0x72>
  if(chdir("dir0") < 0){
     8ab:	c7 04 24 18 46 00 00 	movl   $0x4618,(%esp)
     8b2:	e8 4b 36 00 00       	call   3f02 <chdir>
     8b7:	85 c0                	test   %eax,%eax
     8b9:	78 6c                	js     927 <dirtest+0xa7>
  if(chdir("..") < 0){
     8bb:	c7 04 24 bd 4b 00 00 	movl   $0x4bbd,(%esp)
     8c2:	e8 3b 36 00 00       	call   3f02 <chdir>
     8c7:	85 c0                	test   %eax,%eax
     8c9:	78 52                	js     91d <dirtest+0x9d>
  if(unlink("dir0") < 0){
     8cb:	c7 04 24 18 46 00 00 	movl   $0x4618,(%esp)
     8d2:	e8 0b 36 00 00       	call   3ee2 <unlink>
     8d7:	85 c0                	test   %eax,%eax
     8d9:	78 38                	js     913 <dirtest+0x93>
  printf(stdout, "mkdir test ok\n");
     8db:	a1 c4 63 00 00       	mov    0x63c4,%eax
     8e0:	c7 44 24 04 55 46 00 	movl   $0x4655,0x4(%esp)
     8e7:	00 
     8e8:	89 04 24             	mov    %eax,(%esp)
     8eb:	e8 f0 36 00 00       	call   3fe0 <printf>
}
     8f0:	c9                   	leave  
     8f1:	c3                   	ret    
    printf(stdout, "mkdir failed\n");
     8f2:	c7 44 24 04 48 43 00 	movl   $0x4348,0x4(%esp)
     8f9:	00 
    printf(stdout, "chdir dir0 failed\n");
     8fa:	a1 c4 63 00 00       	mov    0x63c4,%eax
     8ff:	89 04 24             	mov    %eax,(%esp)
     902:	e8 d9 36 00 00       	call   3fe0 <printf>
    exit(0);
     907:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     90e:	e8 7f 35 00 00       	call   3e92 <exit>
    printf(stdout, "unlink dir0 failed\n");
     913:	c7 44 24 04 41 46 00 	movl   $0x4641,0x4(%esp)
     91a:	00 
     91b:	eb dd                	jmp    8fa <dirtest+0x7a>
    printf(stdout, "chdir .. failed\n");
     91d:	c7 44 24 04 30 46 00 	movl   $0x4630,0x4(%esp)
     924:	00 
     925:	eb d3                	jmp    8fa <dirtest+0x7a>
    printf(stdout, "chdir dir0 failed\n");
     927:	c7 44 24 04 1d 46 00 	movl   $0x461d,0x4(%esp)
     92e:	00 
     92f:	eb c9                	jmp    8fa <dirtest+0x7a>
     931:	eb 0d                	jmp    940 <exectest>
     933:	90                   	nop
     934:	90                   	nop
     935:	90                   	nop
     936:	90                   	nop
     937:	90                   	nop
     938:	90                   	nop
     939:	90                   	nop
     93a:	90                   	nop
     93b:	90                   	nop
     93c:	90                   	nop
     93d:	90                   	nop
     93e:	90                   	nop
     93f:	90                   	nop

00000940 <exectest>:
{
     940:	55                   	push   %ebp
     941:	89 e5                	mov    %esp,%ebp
     943:	83 ec 18             	sub    $0x18,%esp
  printf(stdout, "exec test\n");
     946:	a1 c4 63 00 00       	mov    0x63c4,%eax
     94b:	c7 44 24 04 64 46 00 	movl   $0x4664,0x4(%esp)
     952:	00 
     953:	89 04 24             	mov    %eax,(%esp)
     956:	e8 85 36 00 00       	call   3fe0 <printf>
  if(exec("echo", echoargv) < 0){
     95b:	c7 44 24 04 c8 63 00 	movl   $0x63c8,0x4(%esp)
     962:	00 
     963:	c7 04 24 2d 44 00 00 	movl   $0x442d,(%esp)
     96a:	e8 5b 35 00 00       	call   3eca <exec>
     96f:	85 c0                	test   %eax,%eax
     971:	78 02                	js     975 <exectest+0x35>
}
     973:	c9                   	leave  
     974:	c3                   	ret    
    printf(stdout, "exec echo failed\n");
     975:	a1 c4 63 00 00       	mov    0x63c4,%eax
     97a:	c7 44 24 04 6f 46 00 	movl   $0x466f,0x4(%esp)
     981:	00 
     982:	89 04 24             	mov    %eax,(%esp)
     985:	e8 56 36 00 00       	call   3fe0 <printf>
    exit(0);
     98a:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     991:	e8 fc 34 00 00       	call   3e92 <exit>
     996:	8d 76 00             	lea    0x0(%esi),%esi
     999:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

000009a0 <pipe1>:
{
     9a0:	55                   	push   %ebp
     9a1:	89 e5                	mov    %esp,%ebp
     9a3:	57                   	push   %edi
     9a4:	56                   	push   %esi
     9a5:	53                   	push   %ebx
     9a6:	83 ec 2c             	sub    $0x2c,%esp
  if(pipe(fds) != 0){
     9a9:	8d 45 e0             	lea    -0x20(%ebp),%eax
     9ac:	89 04 24             	mov    %eax,(%esp)
     9af:	e8 ee 34 00 00       	call   3ea2 <pipe>
     9b4:	85 c0                	test   %eax,%eax
     9b6:	0f 85 55 01 00 00    	jne    b11 <pipe1+0x171>
  pid = fork();
     9bc:	e8 c9 34 00 00       	call   3e8a <fork>
  if(pid == 0){
     9c1:	83 f8 00             	cmp    $0x0,%eax
     9c4:	0f 84 93 00 00 00    	je     a5d <pipe1+0xbd>
  } else if(pid > 0){
     9ca:	0f 8e 61 01 00 00    	jle    b31 <pipe1+0x191>
    close(fds[1]);
     9d0:	8b 45 e4             	mov    -0x1c(%ebp),%eax
    cc = 1;
     9d3:	bf 01 00 00 00       	mov    $0x1,%edi
  seq = 0;
     9d8:	31 db                	xor    %ebx,%ebx
    close(fds[1]);
     9da:	89 04 24             	mov    %eax,(%esp)
     9dd:	e8 d8 34 00 00       	call   3eba <close>
    total = 0;
     9e2:	c7 45 d4 00 00 00 00 	movl   $0x0,-0x2c(%ebp)
    while((n = read(fds[0], buf, cc)) > 0){
     9e9:	8b 45 e0             	mov    -0x20(%ebp),%eax
     9ec:	89 7c 24 08          	mov    %edi,0x8(%esp)
     9f0:	c7 44 24 04 a0 8b 00 	movl   $0x8ba0,0x4(%esp)
     9f7:	00 
     9f8:	89 04 24             	mov    %eax,(%esp)
     9fb:	e8 aa 34 00 00       	call   3eaa <read>
     a00:	85 c0                	test   %eax,%eax
     a02:	0f 8e ba 00 00 00    	jle    ac2 <pipe1+0x122>
     a08:	89 d9                	mov    %ebx,%ecx
     a0a:	8d 34 03             	lea    (%ebx,%eax,1),%esi
     a0d:	f7 d9                	neg    %ecx
     a0f:	eb 09                	jmp    a1a <pipe1+0x7a>
     a11:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
        if((buf[i] & 0xff) != (seq++ & 0xff)){
     a18:	89 d3                	mov    %edx,%ebx
     a1a:	38 9c 0b a0 8b 00 00 	cmp    %bl,0x8ba0(%ebx,%ecx,1)
     a21:	8d 53 01             	lea    0x1(%ebx),%edx
     a24:	75 1b                	jne    a41 <pipe1+0xa1>
      for(i = 0; i < n; i++){
     a26:	39 f2                	cmp    %esi,%edx
     a28:	75 ee                	jne    a18 <pipe1+0x78>
      cc = cc * 2;
     a2a:	01 ff                	add    %edi,%edi
        if((buf[i] & 0xff) != (seq++ & 0xff)){
     a2c:	89 f3                	mov    %esi,%ebx
      total += n;
     a2e:	01 45 d4             	add    %eax,-0x2c(%ebp)
        cc = sizeof(buf);
     a31:	81 ff 01 20 00 00    	cmp    $0x2001,%edi
     a37:	b8 00 20 00 00       	mov    $0x2000,%eax
     a3c:	0f 43 f8             	cmovae %eax,%edi
     a3f:	eb a8                	jmp    9e9 <pipe1+0x49>
          printf(1, "pipe1 oops 2\n");
     a41:	c7 44 24 04 9e 46 00 	movl   $0x469e,0x4(%esp)
     a48:	00 
     a49:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     a50:	e8 8b 35 00 00       	call   3fe0 <printf>
}
     a55:	83 c4 2c             	add    $0x2c,%esp
     a58:	5b                   	pop    %ebx
     a59:	5e                   	pop    %esi
     a5a:	5f                   	pop    %edi
     a5b:	5d                   	pop    %ebp
     a5c:	c3                   	ret    
    close(fds[0]);
     a5d:	8b 45 e0             	mov    -0x20(%ebp),%eax
  seq = 0;
     a60:	31 f6                	xor    %esi,%esi
    close(fds[0]);
     a62:	89 04 24             	mov    %eax,(%esp)
     a65:	e8 50 34 00 00       	call   3eba <close>
     a6a:	89 f0                	mov    %esi,%eax
{
     a6c:	89 f3                	mov    %esi,%ebx
     a6e:	8d 96 09 04 00 00    	lea    0x409(%esi),%edx
     a74:	f7 d8                	neg    %eax
     a76:	66 90                	xchg   %ax,%ax
        buf[i] = seq++;
     a78:	88 9c 18 a0 8b 00 00 	mov    %bl,0x8ba0(%eax,%ebx,1)
     a7f:	83 c3 01             	add    $0x1,%ebx
      for(i = 0; i < 1033; i++)
     a82:	39 d3                	cmp    %edx,%ebx
     a84:	75 f2                	jne    a78 <pipe1+0xd8>
      if(write(fds[1], buf, 1033) != 1033){
     a86:	8b 45 e4             	mov    -0x1c(%ebp),%eax
     a89:	89 de                	mov    %ebx,%esi
     a8b:	c7 44 24 08 09 04 00 	movl   $0x409,0x8(%esp)
     a92:	00 
     a93:	c7 44 24 04 a0 8b 00 	movl   $0x8ba0,0x4(%esp)
     a9a:	00 
     a9b:	89 04 24             	mov    %eax,(%esp)
     a9e:	e8 0f 34 00 00       	call   3eb2 <write>
     aa3:	3d 09 04 00 00       	cmp    $0x409,%eax
     aa8:	0f 85 a3 00 00 00    	jne    b51 <pipe1+0x1b1>
    for(n = 0; n < 5; n++){
     aae:	81 fb 2d 14 00 00    	cmp    $0x142d,%ebx
     ab4:	75 b4                	jne    a6a <pipe1+0xca>
      exit(0);
     ab6:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     abd:	e8 d0 33 00 00       	call   3e92 <exit>
    if(total != 5 * 1033){
     ac2:	81 7d d4 2d 14 00 00 	cmpl   $0x142d,-0x2c(%ebp)
     ac9:	75 29                	jne    af4 <pipe1+0x154>
    close(fds[0]);
     acb:	8b 45 e0             	mov    -0x20(%ebp),%eax
     ace:	89 04 24             	mov    %eax,(%esp)
     ad1:	e8 e4 33 00 00       	call   3eba <close>
    wait();
     ad6:	e8 bf 33 00 00       	call   3e9a <wait>
  printf(1, "pipe1 ok\n");
     adb:	c7 44 24 04 c3 46 00 	movl   $0x46c3,0x4(%esp)
     ae2:	00 
     ae3:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     aea:	e8 f1 34 00 00       	call   3fe0 <printf>
     aef:	e9 61 ff ff ff       	jmp    a55 <pipe1+0xb5>
      printf(1, "pipe1 oops 3 total %d\n", total);
     af4:	8b 45 d4             	mov    -0x2c(%ebp),%eax
     af7:	c7 44 24 04 ac 46 00 	movl   $0x46ac,0x4(%esp)
     afe:	00 
     aff:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     b06:	89 44 24 08          	mov    %eax,0x8(%esp)
     b0a:	e8 d1 34 00 00       	call   3fe0 <printf>
     b0f:	eb a5                	jmp    ab6 <pipe1+0x116>
    printf(1, "pipe() failed\n");
     b11:	c7 44 24 04 81 46 00 	movl   $0x4681,0x4(%esp)
     b18:	00 
     b19:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     b20:	e8 bb 34 00 00       	call   3fe0 <printf>
    exit(0);
     b25:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     b2c:	e8 61 33 00 00       	call   3e92 <exit>
    printf(1, "fork() failed\n");
     b31:	c7 44 24 04 cd 46 00 	movl   $0x46cd,0x4(%esp)
     b38:	00 
     b39:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     b40:	e8 9b 34 00 00       	call   3fe0 <printf>
    exit(0);
     b45:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     b4c:	e8 41 33 00 00       	call   3e92 <exit>
        printf(1, "pipe1 oops 1\n");
     b51:	c7 44 24 04 90 46 00 	movl   $0x4690,0x4(%esp)
     b58:	00 
     b59:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     b60:	e8 7b 34 00 00       	call   3fe0 <printf>
        exit(0);
     b65:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     b6c:	e8 21 33 00 00       	call   3e92 <exit>
     b71:	eb 0d                	jmp    b80 <preempt>
     b73:	90                   	nop
     b74:	90                   	nop
     b75:	90                   	nop
     b76:	90                   	nop
     b77:	90                   	nop
     b78:	90                   	nop
     b79:	90                   	nop
     b7a:	90                   	nop
     b7b:	90                   	nop
     b7c:	90                   	nop
     b7d:	90                   	nop
     b7e:	90                   	nop
     b7f:	90                   	nop

00000b80 <preempt>:
{
     b80:	55                   	push   %ebp
     b81:	89 e5                	mov    %esp,%ebp
     b83:	57                   	push   %edi
     b84:	56                   	push   %esi
     b85:	53                   	push   %ebx
     b86:	83 ec 2c             	sub    $0x2c,%esp
  printf(1, "preempt: ");
     b89:	c7 44 24 04 dc 46 00 	movl   $0x46dc,0x4(%esp)
     b90:	00 
     b91:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     b98:	e8 43 34 00 00       	call   3fe0 <printf>
  pid1 = fork();
     b9d:	e8 e8 32 00 00       	call   3e8a <fork>
  if(pid1 == 0)
     ba2:	85 c0                	test   %eax,%eax
  pid1 = fork();
     ba4:	89 c7                	mov    %eax,%edi
  if(pid1 == 0)
     ba6:	75 02                	jne    baa <preempt+0x2a>
     ba8:	eb fe                	jmp    ba8 <preempt+0x28>
     baa:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  pid2 = fork();
     bb0:	e8 d5 32 00 00       	call   3e8a <fork>
  if(pid2 == 0)
     bb5:	85 c0                	test   %eax,%eax
  pid2 = fork();
     bb7:	89 c6                	mov    %eax,%esi
  if(pid2 == 0)
     bb9:	75 02                	jne    bbd <preempt+0x3d>
     bbb:	eb fe                	jmp    bbb <preempt+0x3b>
  pipe(pfds);
     bbd:	8d 45 e0             	lea    -0x20(%ebp),%eax
     bc0:	89 04 24             	mov    %eax,(%esp)
     bc3:	e8 da 32 00 00       	call   3ea2 <pipe>
  pid3 = fork();
     bc8:	e8 bd 32 00 00       	call   3e8a <fork>
  if(pid3 == 0){
     bcd:	85 c0                	test   %eax,%eax
  pid3 = fork();
     bcf:	89 c3                	mov    %eax,%ebx
  if(pid3 == 0){
     bd1:	75 4c                	jne    c1f <preempt+0x9f>
    close(pfds[0]);
     bd3:	8b 45 e0             	mov    -0x20(%ebp),%eax
     bd6:	89 04 24             	mov    %eax,(%esp)
     bd9:	e8 dc 32 00 00       	call   3eba <close>
    if(write(pfds[1], "x", 1) != 1)
     bde:	8b 45 e4             	mov    -0x1c(%ebp),%eax
     be1:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
     be8:	00 
     be9:	c7 44 24 04 a1 4c 00 	movl   $0x4ca1,0x4(%esp)
     bf0:	00 
     bf1:	89 04 24             	mov    %eax,(%esp)
     bf4:	e8 b9 32 00 00       	call   3eb2 <write>
     bf9:	83 f8 01             	cmp    $0x1,%eax
     bfc:	74 14                	je     c12 <preempt+0x92>
      printf(1, "preempt write error");
     bfe:	c7 44 24 04 e6 46 00 	movl   $0x46e6,0x4(%esp)
     c05:	00 
     c06:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     c0d:	e8 ce 33 00 00       	call   3fe0 <printf>
    close(pfds[1]);
     c12:	8b 45 e4             	mov    -0x1c(%ebp),%eax
     c15:	89 04 24             	mov    %eax,(%esp)
     c18:	e8 9d 32 00 00       	call   3eba <close>
     c1d:	eb fe                	jmp    c1d <preempt+0x9d>
  close(pfds[1]);
     c1f:	8b 45 e4             	mov    -0x1c(%ebp),%eax
     c22:	89 04 24             	mov    %eax,(%esp)
     c25:	e8 90 32 00 00       	call   3eba <close>
  if(read(pfds[0], buf, sizeof(buf)) != 1){
     c2a:	8b 45 e0             	mov    -0x20(%ebp),%eax
     c2d:	c7 44 24 08 00 20 00 	movl   $0x2000,0x8(%esp)
     c34:	00 
     c35:	c7 44 24 04 a0 8b 00 	movl   $0x8ba0,0x4(%esp)
     c3c:	00 
     c3d:	89 04 24             	mov    %eax,(%esp)
     c40:	e8 65 32 00 00       	call   3eaa <read>
     c45:	83 f8 01             	cmp    $0x1,%eax
     c48:	74 1c                	je     c66 <preempt+0xe6>
    printf(1, "preempt read error");
     c4a:	c7 44 24 04 fa 46 00 	movl   $0x46fa,0x4(%esp)
     c51:	00 
     c52:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     c59:	e8 82 33 00 00       	call   3fe0 <printf>
}
     c5e:	83 c4 2c             	add    $0x2c,%esp
     c61:	5b                   	pop    %ebx
     c62:	5e                   	pop    %esi
     c63:	5f                   	pop    %edi
     c64:	5d                   	pop    %ebp
     c65:	c3                   	ret    
  close(pfds[0]);
     c66:	8b 45 e0             	mov    -0x20(%ebp),%eax
     c69:	89 04 24             	mov    %eax,(%esp)
     c6c:	e8 49 32 00 00       	call   3eba <close>
  printf(1, "kill... ");
     c71:	c7 44 24 04 0d 47 00 	movl   $0x470d,0x4(%esp)
     c78:	00 
     c79:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     c80:	e8 5b 33 00 00       	call   3fe0 <printf>
  kill(pid1);
     c85:	89 3c 24             	mov    %edi,(%esp)
     c88:	e8 35 32 00 00       	call   3ec2 <kill>
  kill(pid2);
     c8d:	89 34 24             	mov    %esi,(%esp)
     c90:	e8 2d 32 00 00       	call   3ec2 <kill>
  kill(pid3);
     c95:	89 1c 24             	mov    %ebx,(%esp)
     c98:	e8 25 32 00 00       	call   3ec2 <kill>
  printf(1, "wait... ");
     c9d:	c7 44 24 04 16 47 00 	movl   $0x4716,0x4(%esp)
     ca4:	00 
     ca5:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     cac:	e8 2f 33 00 00       	call   3fe0 <printf>
  wait();
     cb1:	e8 e4 31 00 00       	call   3e9a <wait>
  wait();
     cb6:	e8 df 31 00 00       	call   3e9a <wait>
     cbb:	90                   	nop
     cbc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  wait();
     cc0:	e8 d5 31 00 00       	call   3e9a <wait>
  printf(1, "preempt ok\n");
     cc5:	c7 44 24 04 1f 47 00 	movl   $0x471f,0x4(%esp)
     ccc:	00 
     ccd:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     cd4:	e8 07 33 00 00       	call   3fe0 <printf>
     cd9:	eb 83                	jmp    c5e <preempt+0xde>
     cdb:	90                   	nop
     cdc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00000ce0 <exitwait>:
{
     ce0:	55                   	push   %ebp
     ce1:	89 e5                	mov    %esp,%ebp
     ce3:	56                   	push   %esi
     ce4:	be 64 00 00 00       	mov    $0x64,%esi
     ce9:	53                   	push   %ebx
     cea:	83 ec 10             	sub    $0x10,%esp
     ced:	eb 13                	jmp    d02 <exitwait+0x22>
     cef:	90                   	nop
    if(pid){
     cf0:	74 71                	je     d63 <exitwait+0x83>
      if(wait() != pid){
     cf2:	e8 a3 31 00 00       	call   3e9a <wait>
     cf7:	39 d8                	cmp    %ebx,%eax
     cf9:	75 2d                	jne    d28 <exitwait+0x48>
  for(i = 0; i < 100; i++){
     cfb:	83 ee 01             	sub    $0x1,%esi
     cfe:	66 90                	xchg   %ax,%ax
     d00:	74 46                	je     d48 <exitwait+0x68>
    pid = fork();
     d02:	e8 83 31 00 00       	call   3e8a <fork>
    if(pid < 0){
     d07:	85 c0                	test   %eax,%eax
    pid = fork();
     d09:	89 c3                	mov    %eax,%ebx
    if(pid < 0){
     d0b:	79 e3                	jns    cf0 <exitwait+0x10>
      printf(1, "fork failed\n");
     d0d:	c7 44 24 04 89 52 00 	movl   $0x5289,0x4(%esp)
     d14:	00 
     d15:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     d1c:	e8 bf 32 00 00       	call   3fe0 <printf>
}
     d21:	83 c4 10             	add    $0x10,%esp
     d24:	5b                   	pop    %ebx
     d25:	5e                   	pop    %esi
     d26:	5d                   	pop    %ebp
     d27:	c3                   	ret    
        printf(1, "wait wrong pid\n");
     d28:	c7 44 24 04 2b 47 00 	movl   $0x472b,0x4(%esp)
     d2f:	00 
     d30:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     d37:	e8 a4 32 00 00       	call   3fe0 <printf>
}
     d3c:	83 c4 10             	add    $0x10,%esp
     d3f:	5b                   	pop    %ebx
     d40:	5e                   	pop    %esi
     d41:	5d                   	pop    %ebp
     d42:	c3                   	ret    
     d43:	90                   	nop
     d44:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  printf(1, "exitwait ok\n");
     d48:	c7 44 24 04 3b 47 00 	movl   $0x473b,0x4(%esp)
     d4f:	00 
     d50:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     d57:	e8 84 32 00 00       	call   3fe0 <printf>
}
     d5c:	83 c4 10             	add    $0x10,%esp
     d5f:	5b                   	pop    %ebx
     d60:	5e                   	pop    %esi
     d61:	5d                   	pop    %ebp
     d62:	c3                   	ret    
      exit(0);
     d63:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     d6a:	e8 23 31 00 00       	call   3e92 <exit>
     d6f:	90                   	nop

00000d70 <mem>:
{
     d70:	55                   	push   %ebp
     d71:	89 e5                	mov    %esp,%ebp
     d73:	57                   	push   %edi
     d74:	56                   	push   %esi
     d75:	53                   	push   %ebx
     d76:	83 ec 1c             	sub    $0x1c,%esp
  printf(1, "mem test\n");
     d79:	c7 44 24 04 48 47 00 	movl   $0x4748,0x4(%esp)
     d80:	00 
     d81:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     d88:	e8 53 32 00 00       	call   3fe0 <printf>
  ppid = getpid();
     d8d:	e8 80 31 00 00       	call   3f12 <getpid>
     d92:	89 c6                	mov    %eax,%esi
  if((pid = fork()) == 0){
     d94:	e8 f1 30 00 00       	call   3e8a <fork>
     d99:	85 c0                	test   %eax,%eax
     d9b:	75 73                	jne    e10 <mem+0xa0>
     d9d:	31 db                	xor    %ebx,%ebx
     d9f:	90                   	nop
     da0:	eb 0a                	jmp    dac <mem+0x3c>
     da2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
      *(char**)m2 = m1;
     da8:	89 18                	mov    %ebx,(%eax)
     daa:	89 c3                	mov    %eax,%ebx
    while((m2 = malloc(10001)) != 0){
     dac:	c7 04 24 11 27 00 00 	movl   $0x2711,(%esp)
     db3:	e8 a8 34 00 00       	call   4260 <malloc>
     db8:	85 c0                	test   %eax,%eax
     dba:	75 ec                	jne    da8 <mem+0x38>
    while(m1){
     dbc:	85 db                	test   %ebx,%ebx
     dbe:	75 0a                	jne    dca <mem+0x5a>
     dc0:	eb 16                	jmp    dd8 <mem+0x68>
     dc2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
      m1 = m2;
     dc8:	89 fb                	mov    %edi,%ebx
      m2 = *(char**)m1;
     dca:	8b 3b                	mov    (%ebx),%edi
      free(m1);
     dcc:	89 1c 24             	mov    %ebx,(%esp)
     dcf:	e8 fc 33 00 00       	call   41d0 <free>
    while(m1){
     dd4:	85 ff                	test   %edi,%edi
     dd6:	75 f0                	jne    dc8 <mem+0x58>
    m1 = malloc(1024*20);
     dd8:	c7 04 24 00 50 00 00 	movl   $0x5000,(%esp)
     ddf:	e8 7c 34 00 00       	call   4260 <malloc>
    if(m1 == 0){
     de4:	85 c0                	test   %eax,%eax
     de6:	74 38                	je     e20 <mem+0xb0>
    free(m1);
     de8:	89 04 24             	mov    %eax,(%esp)
     deb:	e8 e0 33 00 00       	call   41d0 <free>
    printf(1, "mem ok\n");
     df0:	c7 44 24 04 6c 47 00 	movl   $0x476c,0x4(%esp)
     df7:	00 
     df8:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     dff:	e8 dc 31 00 00       	call   3fe0 <printf>
    exit(0);
     e04:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     e0b:	e8 82 30 00 00       	call   3e92 <exit>
}
     e10:	83 c4 1c             	add    $0x1c,%esp
     e13:	5b                   	pop    %ebx
     e14:	5e                   	pop    %esi
     e15:	5f                   	pop    %edi
     e16:	5d                   	pop    %ebp
    wait();
     e17:	e9 7e 30 00 00       	jmp    3e9a <wait>
     e1c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      printf(1, "couldn't allocate mem?!!\n");
     e20:	c7 44 24 04 52 47 00 	movl   $0x4752,0x4(%esp)
     e27:	00 
     e28:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     e2f:	e8 ac 31 00 00       	call   3fe0 <printf>
      kill(ppid);
     e34:	89 34 24             	mov    %esi,(%esp)
     e37:	e8 86 30 00 00       	call   3ec2 <kill>
      exit(0);
     e3c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
     e43:	e8 4a 30 00 00       	call   3e92 <exit>
     e48:	90                   	nop
     e49:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00000e50 <sharedfd>:
{
     e50:	55                   	push   %ebp
     e51:	89 e5                	mov    %esp,%ebp
     e53:	57                   	push   %edi
     e54:	56                   	push   %esi
     e55:	53                   	push   %ebx
     e56:	83 ec 3c             	sub    $0x3c,%esp
  printf(1, "sharedfd test\n");
     e59:	c7 44 24 04 74 47 00 	movl   $0x4774,0x4(%esp)
     e60:	00 
     e61:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     e68:	e8 73 31 00 00       	call   3fe0 <printf>
  unlink("sharedfd");
     e6d:	c7 04 24 83 47 00 00 	movl   $0x4783,(%esp)
     e74:	e8 69 30 00 00       	call   3ee2 <unlink>
  fd = open("sharedfd", O_CREATE|O_RDWR);
     e79:	c7 44 24 04 02 02 00 	movl   $0x202,0x4(%esp)
     e80:	00 
     e81:	c7 04 24 83 47 00 00 	movl   $0x4783,(%esp)
     e88:	e8 45 30 00 00       	call   3ed2 <open>
  if(fd < 0){
     e8d:	85 c0                	test   %eax,%eax
  fd = open("sharedfd", O_CREATE|O_RDWR);
     e8f:	89 c7                	mov    %eax,%edi
  if(fd < 0){
     e91:	0f 88 40 01 00 00    	js     fd7 <sharedfd+0x187>
  pid = fork();
     e97:	e8 ee 2f 00 00       	call   3e8a <fork>
  memset(buf, pid==0?'c':'p', sizeof(buf));
     e9c:	8d 75 de             	lea    -0x22(%ebp),%esi
     e9f:	bb e8 03 00 00       	mov    $0x3e8,%ebx
     ea4:	c7 44 24 08 0a 00 00 	movl   $0xa,0x8(%esp)
     eab:	00 
     eac:	89 34 24             	mov    %esi,(%esp)
     eaf:	83 f8 01             	cmp    $0x1,%eax
  pid = fork();
     eb2:	89 45 d4             	mov    %eax,-0x2c(%ebp)
  memset(buf, pid==0?'c':'p', sizeof(buf));
     eb5:	19 c0                	sbb    %eax,%eax
     eb7:	83 e0 f3             	and    $0xfffffff3,%eax
     eba:	83 c0 70             	add    $0x70,%eax
     ebd:	89 44 24 04          	mov    %eax,0x4(%esp)
     ec1:	e8 5a 2e 00 00       	call   3d20 <memset>
     ec6:	eb 05                	jmp    ecd <sharedfd+0x7d>
  for(i = 0; i < 1000; i++){
     ec8:	83 eb 01             	sub    $0x1,%ebx
     ecb:	74 2d                	je     efa <sharedfd+0xaa>
    if(write(fd, buf, sizeof(buf)) != sizeof(buf)){
     ecd:	c7 44 24 08 0a 00 00 	movl   $0xa,0x8(%esp)
     ed4:	00 
     ed5:	89 74 24 04          	mov    %esi,0x4(%esp)
     ed9:	89 3c 24             	mov    %edi,(%esp)
     edc:	e8 d1 2f 00 00       	call   3eb2 <write>
     ee1:	83 f8 0a             	cmp    $0xa,%eax
     ee4:	74 e2                	je     ec8 <sharedfd+0x78>
      printf(1, "fstests: write sharedfd failed\n");
     ee6:	c7 44 24 04 74 54 00 	movl   $0x5474,0x4(%esp)
     eed:	00 
     eee:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     ef5:	e8 e6 30 00 00       	call   3fe0 <printf>
  if(pid == 0)
     efa:	8b 45 d4             	mov    -0x2c(%ebp),%eax
     efd:	85 c0                	test   %eax,%eax
     eff:	0f 84 26 01 00 00    	je     102b <sharedfd+0x1db>
    wait();
     f05:	e8 90 2f 00 00       	call   3e9a <wait>
  close(fd);
     f0a:	89 3c 24             	mov    %edi,(%esp)
     f0d:	e8 a8 2f 00 00       	call   3eba <close>
  fd = open("sharedfd", 0);
     f12:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
     f19:	00 
     f1a:	c7 04 24 83 47 00 00 	movl   $0x4783,(%esp)
     f21:	e8 ac 2f 00 00       	call   3ed2 <open>
  if(fd < 0){
     f26:	85 c0                	test   %eax,%eax
  fd = open("sharedfd", 0);
     f28:	89 45 d0             	mov    %eax,-0x30(%ebp)
  if(fd < 0){
     f2b:	0f 88 c2 00 00 00    	js     ff3 <sharedfd+0x1a3>
     f31:	31 d2                	xor    %edx,%edx
     f33:	31 db                	xor    %ebx,%ebx
     f35:	8d 7d e8             	lea    -0x18(%ebp),%edi
     f38:	89 55 d4             	mov    %edx,-0x2c(%ebp)
     f3b:	90                   	nop
     f3c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  while((n = read(fd, buf, sizeof(buf))) > 0){
     f40:	8b 45 d0             	mov    -0x30(%ebp),%eax
     f43:	c7 44 24 08 0a 00 00 	movl   $0xa,0x8(%esp)
     f4a:	00 
     f4b:	89 74 24 04          	mov    %esi,0x4(%esp)
     f4f:	89 04 24             	mov    %eax,(%esp)
     f52:	e8 53 2f 00 00       	call   3eaa <read>
     f57:	85 c0                	test   %eax,%eax
     f59:	7e 36                	jle    f91 <sharedfd+0x141>
     f5b:	89 f0                	mov    %esi,%eax
     f5d:	8b 55 d4             	mov    -0x2c(%ebp),%edx
     f60:	eb 18                	jmp    f7a <sharedfd+0x12a>
     f62:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        np++;
     f68:	80 f9 70             	cmp    $0x70,%cl
     f6b:	0f 94 c1             	sete   %cl
     f6e:	83 c0 01             	add    $0x1,%eax
     f71:	0f b6 c9             	movzbl %cl,%ecx
     f74:	01 cb                	add    %ecx,%ebx
    for(i = 0; i < sizeof(buf); i++){
     f76:	39 f8                	cmp    %edi,%eax
     f78:	74 12                	je     f8c <sharedfd+0x13c>
      if(buf[i] == 'c')
     f7a:	0f b6 08             	movzbl (%eax),%ecx
     f7d:	80 f9 63             	cmp    $0x63,%cl
     f80:	75 e6                	jne    f68 <sharedfd+0x118>
     f82:	83 c0 01             	add    $0x1,%eax
        nc++;
     f85:	83 c2 01             	add    $0x1,%edx
    for(i = 0; i < sizeof(buf); i++){
     f88:	39 f8                	cmp    %edi,%eax
     f8a:	75 ee                	jne    f7a <sharedfd+0x12a>
     f8c:	89 55 d4             	mov    %edx,-0x2c(%ebp)
     f8f:	eb af                	jmp    f40 <sharedfd+0xf0>
  close(fd);
     f91:	8b 45 d0             	mov    -0x30(%ebp),%eax
     f94:	89 04 24             	mov    %eax,(%esp)
     f97:	e8 1e 2f 00 00       	call   3eba <close>
  unlink("sharedfd");
     f9c:	c7 04 24 83 47 00 00 	movl   $0x4783,(%esp)
     fa3:	e8 3a 2f 00 00       	call   3ee2 <unlink>
  if(nc == 10000 && np == 10000){
     fa8:	81 fb 10 27 00 00    	cmp    $0x2710,%ebx
     fae:	8b 55 d4             	mov    -0x2c(%ebp),%edx
     fb1:	75 5c                	jne    100f <sharedfd+0x1bf>
     fb3:	81 fa 10 27 00 00    	cmp    $0x2710,%edx
     fb9:	75 54                	jne    100f <sharedfd+0x1bf>
    printf(1, "sharedfd ok\n");
     fbb:	c7 44 24 04 8c 47 00 	movl   $0x478c,0x4(%esp)
     fc2:	00 
     fc3:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     fca:	e8 11 30 00 00       	call   3fe0 <printf>
}
     fcf:	83 c4 3c             	add    $0x3c,%esp
     fd2:	5b                   	pop    %ebx
     fd3:	5e                   	pop    %esi
     fd4:	5f                   	pop    %edi
     fd5:	5d                   	pop    %ebp
     fd6:	c3                   	ret    
    printf(1, "fstests: cannot open sharedfd for writing");
     fd7:	c7 44 24 04 48 54 00 	movl   $0x5448,0x4(%esp)
     fde:	00 
     fdf:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
     fe6:	e8 f5 2f 00 00       	call   3fe0 <printf>
}
     feb:	83 c4 3c             	add    $0x3c,%esp
     fee:	5b                   	pop    %ebx
     fef:	5e                   	pop    %esi
     ff0:	5f                   	pop    %edi
     ff1:	5d                   	pop    %ebp
     ff2:	c3                   	ret    
    printf(1, "fstests: cannot open sharedfd for reading\n");
     ff3:	c7 44 24 04 94 54 00 	movl   $0x5494,0x4(%esp)
     ffa:	00 
     ffb:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1002:	e8 d9 2f 00 00       	call   3fe0 <printf>
}
    1007:	83 c4 3c             	add    $0x3c,%esp
    100a:	5b                   	pop    %ebx
    100b:	5e                   	pop    %esi
    100c:	5f                   	pop    %edi
    100d:	5d                   	pop    %ebp
    100e:	c3                   	ret    
    printf(1, "sharedfd oops %d %d\n", nc, np);
    100f:	89 5c 24 0c          	mov    %ebx,0xc(%esp)
    1013:	89 54 24 08          	mov    %edx,0x8(%esp)
    1017:	c7 44 24 04 99 47 00 	movl   $0x4799,0x4(%esp)
    101e:	00 
    101f:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1026:	e8 b5 2f 00 00       	call   3fe0 <printf>
    exit(0);
    102b:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1032:	e8 5b 2e 00 00       	call   3e92 <exit>
    1037:	89 f6                	mov    %esi,%esi
    1039:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00001040 <fourfiles>:
{
    1040:	55                   	push   %ebp
    1041:	89 e5                	mov    %esp,%ebp
    1043:	57                   	push   %edi
    1044:	56                   	push   %esi
  printf(1, "fourfiles test\n");
    1045:	be ae 47 00 00       	mov    $0x47ae,%esi
{
    104a:	53                   	push   %ebx
  for(pi = 0; pi < 4; pi++){
    104b:	31 db                	xor    %ebx,%ebx
{
    104d:	83 ec 2c             	sub    $0x2c,%esp
  printf(1, "fourfiles test\n");
    1050:	c7 44 24 04 b4 47 00 	movl   $0x47b4,0x4(%esp)
    1057:	00 
    1058:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
  char *names[] = { "f0", "f1", "f2", "f3" };
    105f:	c7 45 d8 ae 47 00 00 	movl   $0x47ae,-0x28(%ebp)
    1066:	c7 45 dc f7 48 00 00 	movl   $0x48f7,-0x24(%ebp)
    106d:	c7 45 e0 fb 48 00 00 	movl   $0x48fb,-0x20(%ebp)
    1074:	c7 45 e4 b1 47 00 00 	movl   $0x47b1,-0x1c(%ebp)
  printf(1, "fourfiles test\n");
    107b:	e8 60 2f 00 00       	call   3fe0 <printf>
    unlink(fname);
    1080:	89 34 24             	mov    %esi,(%esp)
    1083:	e8 5a 2e 00 00       	call   3ee2 <unlink>
    pid = fork();
    1088:	e8 fd 2d 00 00       	call   3e8a <fork>
    if(pid < 0){
    108d:	85 c0                	test   %eax,%eax
    108f:	0f 88 9f 01 00 00    	js     1234 <fourfiles+0x1f4>
    if(pid == 0){
    1095:	0f 84 eb 00 00 00    	je     1186 <fourfiles+0x146>
  for(pi = 0; pi < 4; pi++){
    109b:	83 c3 01             	add    $0x1,%ebx
    109e:	83 fb 04             	cmp    $0x4,%ebx
    10a1:	74 06                	je     10a9 <fourfiles+0x69>
    10a3:	8b 74 9d d8          	mov    -0x28(%ebp,%ebx,4),%esi
    10a7:	eb d7                	jmp    1080 <fourfiles+0x40>
    wait();
    10a9:	e8 ec 2d 00 00       	call   3e9a <wait>
    10ae:	bf 30 00 00 00       	mov    $0x30,%edi
    10b3:	e8 e2 2d 00 00       	call   3e9a <wait>
    10b8:	e8 dd 2d 00 00       	call   3e9a <wait>
    10bd:	e8 d8 2d 00 00       	call   3e9a <wait>
    10c2:	c7 45 d4 ae 47 00 00 	movl   $0x47ae,-0x2c(%ebp)
    fd = open(fname, 0);
    10c9:	8b 45 d4             	mov    -0x2c(%ebp),%eax
    total = 0;
    10cc:	31 db                	xor    %ebx,%ebx
    fd = open(fname, 0);
    10ce:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
    10d5:	00 
    10d6:	89 04 24             	mov    %eax,(%esp)
    10d9:	e8 f4 2d 00 00       	call   3ed2 <open>
    10de:	89 c6                	mov    %eax,%esi
    while((n = read(fd, buf, sizeof(buf))) > 0){
    10e0:	c7 44 24 08 00 20 00 	movl   $0x2000,0x8(%esp)
    10e7:	00 
    10e8:	c7 44 24 04 a0 8b 00 	movl   $0x8ba0,0x4(%esp)
    10ef:	00 
    10f0:	89 34 24             	mov    %esi,(%esp)
    10f3:	e8 b2 2d 00 00       	call   3eaa <read>
    10f8:	85 c0                	test   %eax,%eax
    10fa:	7e 1a                	jle    1116 <fourfiles+0xd6>
    10fc:	31 d2                	xor    %edx,%edx
    10fe:	66 90                	xchg   %ax,%ax
        if(buf[j] != '0'+i){
    1100:	0f be 8a a0 8b 00 00 	movsbl 0x8ba0(%edx),%ecx
    1107:	39 cf                	cmp    %ecx,%edi
    1109:	75 5b                	jne    1166 <fourfiles+0x126>
      for(j = 0; j < n; j++){
    110b:	83 c2 01             	add    $0x1,%edx
    110e:	39 c2                	cmp    %eax,%edx
    1110:	75 ee                	jne    1100 <fourfiles+0xc0>
      total += n;
    1112:	01 d3                	add    %edx,%ebx
    1114:	eb ca                	jmp    10e0 <fourfiles+0xa0>
    close(fd);
    1116:	89 34 24             	mov    %esi,(%esp)
    1119:	e8 9c 2d 00 00       	call   3eba <close>
    if(total != 12*500){
    111e:	81 fb 70 17 00 00    	cmp    $0x1770,%ebx
    1124:	0f 85 e6 00 00 00    	jne    1210 <fourfiles+0x1d0>
    unlink(fname);
    112a:	8b 45 d4             	mov    -0x2c(%ebp),%eax
    112d:	89 04 24             	mov    %eax,(%esp)
    1130:	e8 ad 2d 00 00       	call   3ee2 <unlink>
  for(i = 0; i < 2; i++){
    1135:	83 ff 31             	cmp    $0x31,%edi
    1138:	75 1c                	jne    1156 <fourfiles+0x116>
  printf(1, "fourfiles ok\n");
    113a:	c7 44 24 04 f2 47 00 	movl   $0x47f2,0x4(%esp)
    1141:	00 
    1142:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1149:	e8 92 2e 00 00       	call   3fe0 <printf>
}
    114e:	83 c4 2c             	add    $0x2c,%esp
    1151:	5b                   	pop    %ebx
    1152:	5e                   	pop    %esi
    1153:	5f                   	pop    %edi
    1154:	5d                   	pop    %ebp
    1155:	c3                   	ret    
    1156:	8b 45 dc             	mov    -0x24(%ebp),%eax
    1159:	bf 31 00 00 00       	mov    $0x31,%edi
    115e:	89 45 d4             	mov    %eax,-0x2c(%ebp)
    1161:	e9 63 ff ff ff       	jmp    10c9 <fourfiles+0x89>
          printf(1, "wrong char\n");
    1166:	c7 44 24 04 d5 47 00 	movl   $0x47d5,0x4(%esp)
    116d:	00 
    116e:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1175:	e8 66 2e 00 00       	call   3fe0 <printf>
          exit(0);
    117a:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1181:	e8 0c 2d 00 00       	call   3e92 <exit>
      fd = open(fname, O_CREATE | O_RDWR);
    1186:	89 34 24             	mov    %esi,(%esp)
    1189:	c7 44 24 04 02 02 00 	movl   $0x202,0x4(%esp)
    1190:	00 
    1191:	e8 3c 2d 00 00       	call   3ed2 <open>
      if(fd < 0){
    1196:	85 c0                	test   %eax,%eax
      fd = open(fname, O_CREATE | O_RDWR);
    1198:	89 c6                	mov    %eax,%esi
      if(fd < 0){
    119a:	0f 88 b4 00 00 00    	js     1254 <fourfiles+0x214>
      memset(buf, '0'+pi, 512);
    11a0:	83 c3 30             	add    $0x30,%ebx
    11a3:	89 5c 24 04          	mov    %ebx,0x4(%esp)
    11a7:	bb 0c 00 00 00       	mov    $0xc,%ebx
    11ac:	c7 44 24 08 00 02 00 	movl   $0x200,0x8(%esp)
    11b3:	00 
    11b4:	c7 04 24 a0 8b 00 00 	movl   $0x8ba0,(%esp)
    11bb:	e8 60 2b 00 00       	call   3d20 <memset>
    11c0:	eb 0b                	jmp    11cd <fourfiles+0x18d>
    11c2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
      for(i = 0; i < 12; i++){
    11c8:	83 eb 01             	sub    $0x1,%ebx
    11cb:	74 ad                	je     117a <fourfiles+0x13a>
        if((n = write(fd, buf, 500)) != 500){
    11cd:	c7 44 24 08 f4 01 00 	movl   $0x1f4,0x8(%esp)
    11d4:	00 
    11d5:	c7 44 24 04 a0 8b 00 	movl   $0x8ba0,0x4(%esp)
    11dc:	00 
    11dd:	89 34 24             	mov    %esi,(%esp)
    11e0:	e8 cd 2c 00 00       	call   3eb2 <write>
    11e5:	3d f4 01 00 00       	cmp    $0x1f4,%eax
    11ea:	74 dc                	je     11c8 <fourfiles+0x188>
          printf(1, "write failed %d\n", n);
    11ec:	89 44 24 08          	mov    %eax,0x8(%esp)
    11f0:	c7 44 24 04 c4 47 00 	movl   $0x47c4,0x4(%esp)
    11f7:	00 
    11f8:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    11ff:	e8 dc 2d 00 00       	call   3fe0 <printf>
          exit(0);
    1204:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    120b:	e8 82 2c 00 00       	call   3e92 <exit>
      printf(1, "wrong length %d\n", total);
    1210:	89 5c 24 08          	mov    %ebx,0x8(%esp)
    1214:	c7 44 24 04 e1 47 00 	movl   $0x47e1,0x4(%esp)
    121b:	00 
    121c:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1223:	e8 b8 2d 00 00       	call   3fe0 <printf>
      exit(0);
    1228:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    122f:	e8 5e 2c 00 00       	call   3e92 <exit>
      printf(1, "fork failed\n");
    1234:	c7 44 24 04 89 52 00 	movl   $0x5289,0x4(%esp)
    123b:	00 
    123c:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1243:	e8 98 2d 00 00       	call   3fe0 <printf>
      exit(0);
    1248:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    124f:	e8 3e 2c 00 00       	call   3e92 <exit>
        printf(1, "create failed\n");
    1254:	c7 44 24 04 4f 4a 00 	movl   $0x4a4f,0x4(%esp)
    125b:	00 
    125c:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1263:	e8 78 2d 00 00       	call   3fe0 <printf>
        exit(0);
    1268:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    126f:	e8 1e 2c 00 00       	call   3e92 <exit>
    1274:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    127a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

00001280 <createdelete>:
{
    1280:	55                   	push   %ebp
    1281:	89 e5                	mov    %esp,%ebp
    1283:	57                   	push   %edi
    1284:	56                   	push   %esi
    1285:	53                   	push   %ebx
  for(pi = 0; pi < 4; pi++){
    1286:	31 db                	xor    %ebx,%ebx
{
    1288:	83 ec 4c             	sub    $0x4c,%esp
  printf(1, "createdelete test\n");
    128b:	c7 44 24 04 00 48 00 	movl   $0x4800,0x4(%esp)
    1292:	00 
    1293:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    129a:	e8 41 2d 00 00       	call   3fe0 <printf>
    pid = fork();
    129f:	e8 e6 2b 00 00       	call   3e8a <fork>
    if(pid < 0){
    12a4:	85 c0                	test   %eax,%eax
    12a6:	0f 88 e8 01 00 00    	js     1494 <createdelete+0x214>
    12ac:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    if(pid == 0){
    12b0:	0f 84 14 01 00 00    	je     13ca <createdelete+0x14a>
  for(pi = 0; pi < 4; pi++){
    12b6:	83 c3 01             	add    $0x1,%ebx
    12b9:	83 fb 04             	cmp    $0x4,%ebx
    12bc:	75 e1                	jne    129f <createdelete+0x1f>
    12be:	66 90                	xchg   %ax,%ax
    wait();
    12c0:	e8 d5 2b 00 00       	call   3e9a <wait>
  for(i = 0; i < N; i++){
    12c5:	31 f6                	xor    %esi,%esi
    wait();
    12c7:	e8 ce 2b 00 00       	call   3e9a <wait>
    12cc:	8d 7d c8             	lea    -0x38(%ebp),%edi
    12cf:	e8 c6 2b 00 00       	call   3e9a <wait>
    12d4:	e8 c1 2b 00 00       	call   3e9a <wait>
  name[0] = name[1] = name[2] = 0;
    12d9:	c6 45 ca 00          	movb   $0x0,-0x36(%ebp)
    12dd:	8d 76 00             	lea    0x0(%esi),%esi
    12e0:	85 f6                	test   %esi,%esi
      name[2] = '\0';
    12e2:	bb 70 00 00 00       	mov    $0x70,%ebx
    12e7:	8d 46 30             	lea    0x30(%esi),%eax
    12ea:	0f 94 45 c7          	sete   -0x39(%ebp)
    12ee:	83 fe 09             	cmp    $0x9,%esi
    12f1:	88 45 c6             	mov    %al,-0x3a(%ebp)
    12f4:	0f 9f c0             	setg   %al
    12f7:	08 45 c7             	or     %al,-0x39(%ebp)
    12fa:	8d 46 ff             	lea    -0x1(%esi),%eax
    12fd:	89 45 c0             	mov    %eax,-0x40(%ebp)
      name[1] = '0' + i;
    1300:	0f b6 45 c6          	movzbl -0x3a(%ebp),%eax
      fd = open(name, 0);
    1304:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
    130b:	00 
    130c:	89 3c 24             	mov    %edi,(%esp)
      name[0] = 'p' + pi;
    130f:	88 5d c8             	mov    %bl,-0x38(%ebp)
      name[1] = '0' + i;
    1312:	88 45 c9             	mov    %al,-0x37(%ebp)
      fd = open(name, 0);
    1315:	e8 b8 2b 00 00       	call   3ed2 <open>
      if((i == 0 || i >= N/2) && fd < 0){
    131a:	80 7d c7 00          	cmpb   $0x0,-0x39(%ebp)
    131e:	0f 84 84 00 00 00    	je     13a8 <createdelete+0x128>
    1324:	85 c0                	test   %eax,%eax
    1326:	0f 88 24 01 00 00    	js     1450 <createdelete+0x1d0>
      } else if((i >= 1 && i < N/2) && fd >= 0){
    132c:	83 7d c0 08          	cmpl   $0x8,-0x40(%ebp)
    1330:	0f 86 7e 01 00 00    	jbe    14b4 <createdelete+0x234>
        close(fd);
    1336:	89 04 24             	mov    %eax,(%esp)
    1339:	83 c3 01             	add    $0x1,%ebx
    133c:	e8 79 2b 00 00       	call   3eba <close>
    for(pi = 0; pi < 4; pi++){
    1341:	80 fb 74             	cmp    $0x74,%bl
    1344:	75 ba                	jne    1300 <createdelete+0x80>
  for(i = 0; i < N; i++){
    1346:	83 c6 01             	add    $0x1,%esi
    1349:	83 fe 14             	cmp    $0x14,%esi
    134c:	75 92                	jne    12e0 <createdelete+0x60>
    134e:	be 70 00 00 00       	mov    $0x70,%esi
    1353:	90                   	nop
    1354:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    1358:	8d 46 c0             	lea    -0x40(%esi),%eax
    135b:	bb 04 00 00 00       	mov    $0x4,%ebx
    1360:	88 45 c7             	mov    %al,-0x39(%ebp)
      name[0] = 'p' + i;
    1363:	89 f0                	mov    %esi,%eax
    1365:	88 45 c8             	mov    %al,-0x38(%ebp)
      name[1] = '0' + i;
    1368:	0f b6 45 c7          	movzbl -0x39(%ebp),%eax
      unlink(name);
    136c:	89 3c 24             	mov    %edi,(%esp)
      name[1] = '0' + i;
    136f:	88 45 c9             	mov    %al,-0x37(%ebp)
      unlink(name);
    1372:	e8 6b 2b 00 00       	call   3ee2 <unlink>
    for(pi = 0; pi < 4; pi++){
    1377:	83 eb 01             	sub    $0x1,%ebx
    137a:	75 e7                	jne    1363 <createdelete+0xe3>
    137c:	83 c6 01             	add    $0x1,%esi
  for(i = 0; i < N; i++){
    137f:	89 f0                	mov    %esi,%eax
    1381:	3c 84                	cmp    $0x84,%al
    1383:	75 d3                	jne    1358 <createdelete+0xd8>
  printf(1, "createdelete ok\n");
    1385:	c7 44 24 04 13 48 00 	movl   $0x4813,0x4(%esp)
    138c:	00 
    138d:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1394:	e8 47 2c 00 00       	call   3fe0 <printf>
}
    1399:	83 c4 4c             	add    $0x4c,%esp
    139c:	5b                   	pop    %ebx
    139d:	5e                   	pop    %esi
    139e:	5f                   	pop    %edi
    139f:	5d                   	pop    %ebp
    13a0:	c3                   	ret    
    13a1:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
      } else if((i >= 1 && i < N/2) && fd >= 0){
    13a8:	85 c0                	test   %eax,%eax
    13aa:	0f 89 04 01 00 00    	jns    14b4 <createdelete+0x234>
    13b0:	83 c3 01             	add    $0x1,%ebx
    for(pi = 0; pi < 4; pi++){
    13b3:	80 fb 74             	cmp    $0x74,%bl
    13b6:	0f 85 44 ff ff ff    	jne    1300 <createdelete+0x80>
  for(i = 0; i < N; i++){
    13bc:	83 c6 01             	add    $0x1,%esi
    13bf:	83 fe 14             	cmp    $0x14,%esi
    13c2:	0f 85 18 ff ff ff    	jne    12e0 <createdelete+0x60>
    13c8:	eb 84                	jmp    134e <createdelete+0xce>
      name[0] = 'p' + pi;
    13ca:	83 c3 70             	add    $0x70,%ebx
      name[2] = '\0';
    13cd:	be 01 00 00 00       	mov    $0x1,%esi
      name[0] = 'p' + pi;
    13d2:	88 5d c8             	mov    %bl,-0x38(%ebp)
    13d5:	8d 7d c8             	lea    -0x38(%ebp),%edi
      name[2] = '\0';
    13d8:	31 db                	xor    %ebx,%ebx
    13da:	c6 45 ca 00          	movb   $0x0,-0x36(%ebp)
    13de:	eb 0f                	jmp    13ef <createdelete+0x16f>
      for(i = 0; i < N; i++){
    13e0:	83 fe 14             	cmp    $0x14,%esi
    13e3:	0f 84 7f 00 00 00    	je     1468 <createdelete+0x1e8>
    13e9:	83 c3 01             	add    $0x1,%ebx
    13ec:	83 c6 01             	add    $0x1,%esi
    13ef:	8d 43 30             	lea    0x30(%ebx),%eax
        fd = open(name, O_CREATE | O_RDWR);
    13f2:	c7 44 24 04 02 02 00 	movl   $0x202,0x4(%esp)
    13f9:	00 
    13fa:	89 3c 24             	mov    %edi,(%esp)
    13fd:	88 45 c9             	mov    %al,-0x37(%ebp)
    1400:	e8 cd 2a 00 00       	call   3ed2 <open>
        if(fd < 0){
    1405:	85 c0                	test   %eax,%eax
    1407:	78 6b                	js     1474 <createdelete+0x1f4>
        close(fd);
    1409:	89 04 24             	mov    %eax,(%esp)
    140c:	e8 a9 2a 00 00       	call   3eba <close>
        if(i > 0 && (i % 2 ) == 0){
    1411:	85 db                	test   %ebx,%ebx
    1413:	74 d4                	je     13e9 <createdelete+0x169>
    1415:	f6 c3 01             	test   $0x1,%bl
    1418:	75 c6                	jne    13e0 <createdelete+0x160>
          name[1] = '0' + (i / 2);
    141a:	89 d8                	mov    %ebx,%eax
    141c:	d1 f8                	sar    %eax
    141e:	83 c0 30             	add    $0x30,%eax
          if(unlink(name) < 0){
    1421:	89 3c 24             	mov    %edi,(%esp)
          name[1] = '0' + (i / 2);
    1424:	88 45 c9             	mov    %al,-0x37(%ebp)
          if(unlink(name) < 0){
    1427:	e8 b6 2a 00 00       	call   3ee2 <unlink>
    142c:	85 c0                	test   %eax,%eax
    142e:	79 b0                	jns    13e0 <createdelete+0x160>
            printf(1, "unlink failed\n");
    1430:	c7 44 24 04 01 44 00 	movl   $0x4401,0x4(%esp)
    1437:	00 
    1438:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    143f:	e8 9c 2b 00 00       	call   3fe0 <printf>
            exit(0);
    1444:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    144b:	e8 42 2a 00 00       	call   3e92 <exit>
        printf(1, "oops createdelete %s didn't exist\n", name);
    1450:	89 7c 24 08          	mov    %edi,0x8(%esp)
    1454:	c7 44 24 04 c0 54 00 	movl   $0x54c0,0x4(%esp)
    145b:	00 
    145c:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1463:	e8 78 2b 00 00       	call   3fe0 <printf>
        exit(0);
    1468:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    146f:	e8 1e 2a 00 00       	call   3e92 <exit>
          printf(1, "create failed\n");
    1474:	c7 44 24 04 4f 4a 00 	movl   $0x4a4f,0x4(%esp)
    147b:	00 
    147c:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1483:	e8 58 2b 00 00       	call   3fe0 <printf>
          exit(0);
    1488:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    148f:	e8 fe 29 00 00       	call   3e92 <exit>
      printf(1, "fork failed\n");
    1494:	c7 44 24 04 89 52 00 	movl   $0x5289,0x4(%esp)
    149b:	00 
    149c:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    14a3:	e8 38 2b 00 00       	call   3fe0 <printf>
      exit(0);
    14a8:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    14af:	e8 de 29 00 00       	call   3e92 <exit>
        printf(1, "oops createdelete %s did exist\n", name);
    14b4:	89 7c 24 08          	mov    %edi,0x8(%esp)
    14b8:	c7 44 24 04 e4 54 00 	movl   $0x54e4,0x4(%esp)
    14bf:	00 
    14c0:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    14c7:	e8 14 2b 00 00       	call   3fe0 <printf>
        exit(0);
    14cc:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    14d3:	e8 ba 29 00 00       	call   3e92 <exit>
    14d8:	90                   	nop
    14d9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

000014e0 <unlinkread>:
{
    14e0:	55                   	push   %ebp
    14e1:	89 e5                	mov    %esp,%ebp
    14e3:	56                   	push   %esi
    14e4:	53                   	push   %ebx
    14e5:	83 ec 10             	sub    $0x10,%esp
  printf(1, "unlinkread test\n");
    14e8:	c7 44 24 04 24 48 00 	movl   $0x4824,0x4(%esp)
    14ef:	00 
    14f0:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    14f7:	e8 e4 2a 00 00       	call   3fe0 <printf>
  fd = open("unlinkread", O_CREATE | O_RDWR);
    14fc:	c7 44 24 04 02 02 00 	movl   $0x202,0x4(%esp)
    1503:	00 
    1504:	c7 04 24 35 48 00 00 	movl   $0x4835,(%esp)
    150b:	e8 c2 29 00 00       	call   3ed2 <open>
  if(fd < 0){
    1510:	85 c0                	test   %eax,%eax
  fd = open("unlinkread", O_CREATE | O_RDWR);
    1512:	89 c3                	mov    %eax,%ebx
  if(fd < 0){
    1514:	0f 88 02 01 00 00    	js     161c <unlinkread+0x13c>
  write(fd, "hello", 5);
    151a:	c7 44 24 08 05 00 00 	movl   $0x5,0x8(%esp)
    1521:	00 
    1522:	c7 44 24 04 5a 48 00 	movl   $0x485a,0x4(%esp)
    1529:	00 
    152a:	89 04 24             	mov    %eax,(%esp)
    152d:	e8 80 29 00 00       	call   3eb2 <write>
  close(fd);
    1532:	89 1c 24             	mov    %ebx,(%esp)
    1535:	e8 80 29 00 00       	call   3eba <close>
  fd = open("unlinkread", O_RDWR);
    153a:	c7 44 24 04 02 00 00 	movl   $0x2,0x4(%esp)
    1541:	00 
    1542:	c7 04 24 35 48 00 00 	movl   $0x4835,(%esp)
    1549:	e8 84 29 00 00       	call   3ed2 <open>
  if(fd < 0){
    154e:	85 c0                	test   %eax,%eax
  fd = open("unlinkread", O_RDWR);
    1550:	89 c3                	mov    %eax,%ebx
  if(fd < 0){
    1552:	0f 88 64 01 00 00    	js     16bc <unlinkread+0x1dc>
  if(unlink("unlinkread") != 0){
    1558:	c7 04 24 35 48 00 00 	movl   $0x4835,(%esp)
    155f:	e8 7e 29 00 00       	call   3ee2 <unlink>
    1564:	85 c0                	test   %eax,%eax
    1566:	0f 85 30 01 00 00    	jne    169c <unlinkread+0x1bc>
  fd1 = open("unlinkread", O_CREATE | O_RDWR);
    156c:	c7 44 24 04 02 02 00 	movl   $0x202,0x4(%esp)
    1573:	00 
    1574:	c7 04 24 35 48 00 00 	movl   $0x4835,(%esp)
    157b:	e8 52 29 00 00       	call   3ed2 <open>
  write(fd1, "yyy", 3);
    1580:	c7 44 24 08 03 00 00 	movl   $0x3,0x8(%esp)
    1587:	00 
    1588:	c7 44 24 04 92 48 00 	movl   $0x4892,0x4(%esp)
    158f:	00 
  fd1 = open("unlinkread", O_CREATE | O_RDWR);
    1590:	89 c6                	mov    %eax,%esi
  write(fd1, "yyy", 3);
    1592:	89 04 24             	mov    %eax,(%esp)
    1595:	e8 18 29 00 00       	call   3eb2 <write>
  close(fd1);
    159a:	89 34 24             	mov    %esi,(%esp)
    159d:	e8 18 29 00 00       	call   3eba <close>
  if(read(fd, buf, sizeof(buf)) != 5){
    15a2:	c7 44 24 08 00 20 00 	movl   $0x2000,0x8(%esp)
    15a9:	00 
    15aa:	c7 44 24 04 a0 8b 00 	movl   $0x8ba0,0x4(%esp)
    15b1:	00 
    15b2:	89 1c 24             	mov    %ebx,(%esp)
    15b5:	e8 f0 28 00 00       	call   3eaa <read>
    15ba:	83 f8 05             	cmp    $0x5,%eax
    15bd:	0f 85 b9 00 00 00    	jne    167c <unlinkread+0x19c>
  if(buf[0] != 'h'){
    15c3:	80 3d a0 8b 00 00 68 	cmpb   $0x68,0x8ba0
    15ca:	0f 85 8c 00 00 00    	jne    165c <unlinkread+0x17c>
  if(write(fd, buf, 10) != 10){
    15d0:	c7 44 24 08 0a 00 00 	movl   $0xa,0x8(%esp)
    15d7:	00 
    15d8:	c7 44 24 04 a0 8b 00 	movl   $0x8ba0,0x4(%esp)
    15df:	00 
    15e0:	89 1c 24             	mov    %ebx,(%esp)
    15e3:	e8 ca 28 00 00       	call   3eb2 <write>
    15e8:	83 f8 0a             	cmp    $0xa,%eax
    15eb:	75 4f                	jne    163c <unlinkread+0x15c>
  close(fd);
    15ed:	89 1c 24             	mov    %ebx,(%esp)
    15f0:	e8 c5 28 00 00       	call   3eba <close>
  unlink("unlinkread");
    15f5:	c7 04 24 35 48 00 00 	movl   $0x4835,(%esp)
    15fc:	e8 e1 28 00 00       	call   3ee2 <unlink>
  printf(1, "unlinkread ok\n");
    1601:	c7 44 24 04 dd 48 00 	movl   $0x48dd,0x4(%esp)
    1608:	00 
    1609:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1610:	e8 cb 29 00 00       	call   3fe0 <printf>
}
    1615:	83 c4 10             	add    $0x10,%esp
    1618:	5b                   	pop    %ebx
    1619:	5e                   	pop    %esi
    161a:	5d                   	pop    %ebp
    161b:	c3                   	ret    
    printf(1, "create unlinkread failed\n");
    161c:	c7 44 24 04 40 48 00 	movl   $0x4840,0x4(%esp)
    1623:	00 
    1624:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    162b:	e8 b0 29 00 00       	call   3fe0 <printf>
    exit(0);
    1630:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1637:	e8 56 28 00 00       	call   3e92 <exit>
    printf(1, "unlinkread write failed\n");
    163c:	c7 44 24 04 c4 48 00 	movl   $0x48c4,0x4(%esp)
    1643:	00 
    1644:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    164b:	e8 90 29 00 00       	call   3fe0 <printf>
    exit(0);
    1650:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1657:	e8 36 28 00 00       	call   3e92 <exit>
    printf(1, "unlinkread wrong data\n");
    165c:	c7 44 24 04 ad 48 00 	movl   $0x48ad,0x4(%esp)
    1663:	00 
    1664:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    166b:	e8 70 29 00 00       	call   3fe0 <printf>
    exit(0);
    1670:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1677:	e8 16 28 00 00       	call   3e92 <exit>
    printf(1, "unlinkread read failed");
    167c:	c7 44 24 04 96 48 00 	movl   $0x4896,0x4(%esp)
    1683:	00 
    1684:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    168b:	e8 50 29 00 00       	call   3fe0 <printf>
    exit(0);
    1690:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1697:	e8 f6 27 00 00       	call   3e92 <exit>
    printf(1, "unlink unlinkread failed\n");
    169c:	c7 44 24 04 78 48 00 	movl   $0x4878,0x4(%esp)
    16a3:	00 
    16a4:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    16ab:	e8 30 29 00 00       	call   3fe0 <printf>
    exit(0);
    16b0:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    16b7:	e8 d6 27 00 00       	call   3e92 <exit>
    printf(1, "open unlinkread failed\n");
    16bc:	c7 44 24 04 60 48 00 	movl   $0x4860,0x4(%esp)
    16c3:	00 
    16c4:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    16cb:	e8 10 29 00 00       	call   3fe0 <printf>
    exit(0);
    16d0:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    16d7:	e8 b6 27 00 00       	call   3e92 <exit>
    16dc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

000016e0 <linktest>:
{
    16e0:	55                   	push   %ebp
    16e1:	89 e5                	mov    %esp,%ebp
    16e3:	53                   	push   %ebx
    16e4:	83 ec 14             	sub    $0x14,%esp
  printf(1, "linktest\n");
    16e7:	c7 44 24 04 ec 48 00 	movl   $0x48ec,0x4(%esp)
    16ee:	00 
    16ef:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    16f6:	e8 e5 28 00 00       	call   3fe0 <printf>
  unlink("lf1");
    16fb:	c7 04 24 f6 48 00 00 	movl   $0x48f6,(%esp)
    1702:	e8 db 27 00 00       	call   3ee2 <unlink>
  unlink("lf2");
    1707:	c7 04 24 fa 48 00 00 	movl   $0x48fa,(%esp)
    170e:	e8 cf 27 00 00       	call   3ee2 <unlink>
  fd = open("lf1", O_CREATE|O_RDWR);
    1713:	c7 44 24 04 02 02 00 	movl   $0x202,0x4(%esp)
    171a:	00 
    171b:	c7 04 24 f6 48 00 00 	movl   $0x48f6,(%esp)
    1722:	e8 ab 27 00 00       	call   3ed2 <open>
  if(fd < 0){
    1727:	85 c0                	test   %eax,%eax
  fd = open("lf1", O_CREATE|O_RDWR);
    1729:	89 c3                	mov    %eax,%ebx
  if(fd < 0){
    172b:	0f 88 26 01 00 00    	js     1857 <linktest+0x177>
  if(write(fd, "hello", 5) != 5){
    1731:	c7 44 24 08 05 00 00 	movl   $0x5,0x8(%esp)
    1738:	00 
    1739:	c7 44 24 04 5a 48 00 	movl   $0x485a,0x4(%esp)
    1740:	00 
    1741:	89 04 24             	mov    %eax,(%esp)
    1744:	e8 69 27 00 00       	call   3eb2 <write>
    1749:	83 f8 05             	cmp    $0x5,%eax
    174c:	0f 85 05 02 00 00    	jne    1957 <linktest+0x277>
  close(fd);
    1752:	89 1c 24             	mov    %ebx,(%esp)
    1755:	e8 60 27 00 00       	call   3eba <close>
  if(link("lf1", "lf2") < 0){
    175a:	c7 44 24 04 fa 48 00 	movl   $0x48fa,0x4(%esp)
    1761:	00 
    1762:	c7 04 24 f6 48 00 00 	movl   $0x48f6,(%esp)
    1769:	e8 84 27 00 00       	call   3ef2 <link>
    176e:	85 c0                	test   %eax,%eax
    1770:	0f 88 c1 01 00 00    	js     1937 <linktest+0x257>
  unlink("lf1");
    1776:	c7 04 24 f6 48 00 00 	movl   $0x48f6,(%esp)
    177d:	e8 60 27 00 00       	call   3ee2 <unlink>
  if(open("lf1", 0) >= 0){
    1782:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
    1789:	00 
    178a:	c7 04 24 f6 48 00 00 	movl   $0x48f6,(%esp)
    1791:	e8 3c 27 00 00       	call   3ed2 <open>
    1796:	85 c0                	test   %eax,%eax
    1798:	0f 89 79 01 00 00    	jns    1917 <linktest+0x237>
  fd = open("lf2", 0);
    179e:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
    17a5:	00 
    17a6:	c7 04 24 fa 48 00 00 	movl   $0x48fa,(%esp)
    17ad:	e8 20 27 00 00       	call   3ed2 <open>
  if(fd < 0){
    17b2:	85 c0                	test   %eax,%eax
  fd = open("lf2", 0);
    17b4:	89 c3                	mov    %eax,%ebx
  if(fd < 0){
    17b6:	0f 88 3b 01 00 00    	js     18f7 <linktest+0x217>
  if(read(fd, buf, sizeof(buf)) != 5){
    17bc:	c7 44 24 08 00 20 00 	movl   $0x2000,0x8(%esp)
    17c3:	00 
    17c4:	c7 44 24 04 a0 8b 00 	movl   $0x8ba0,0x4(%esp)
    17cb:	00 
    17cc:	89 04 24             	mov    %eax,(%esp)
    17cf:	e8 d6 26 00 00       	call   3eaa <read>
    17d4:	83 f8 05             	cmp    $0x5,%eax
    17d7:	0f 85 fa 00 00 00    	jne    18d7 <linktest+0x1f7>
  close(fd);
    17dd:	89 1c 24             	mov    %ebx,(%esp)
    17e0:	e8 d5 26 00 00       	call   3eba <close>
  if(link("lf2", "lf2") >= 0){
    17e5:	c7 44 24 04 fa 48 00 	movl   $0x48fa,0x4(%esp)
    17ec:	00 
    17ed:	c7 04 24 fa 48 00 00 	movl   $0x48fa,(%esp)
    17f4:	e8 f9 26 00 00       	call   3ef2 <link>
    17f9:	85 c0                	test   %eax,%eax
    17fb:	0f 89 b6 00 00 00    	jns    18b7 <linktest+0x1d7>
  unlink("lf2");
    1801:	c7 04 24 fa 48 00 00 	movl   $0x48fa,(%esp)
    1808:	e8 d5 26 00 00       	call   3ee2 <unlink>
  if(link("lf2", "lf1") >= 0){
    180d:	c7 44 24 04 f6 48 00 	movl   $0x48f6,0x4(%esp)
    1814:	00 
    1815:	c7 04 24 fa 48 00 00 	movl   $0x48fa,(%esp)
    181c:	e8 d1 26 00 00       	call   3ef2 <link>
    1821:	85 c0                	test   %eax,%eax
    1823:	79 72                	jns    1897 <linktest+0x1b7>
  if(link(".", "lf1") >= 0){
    1825:	c7 44 24 04 f6 48 00 	movl   $0x48f6,0x4(%esp)
    182c:	00 
    182d:	c7 04 24 be 4b 00 00 	movl   $0x4bbe,(%esp)
    1834:	e8 b9 26 00 00       	call   3ef2 <link>
    1839:	85 c0                	test   %eax,%eax
    183b:	79 3a                	jns    1877 <linktest+0x197>
  printf(1, "linktest ok\n");
    183d:	c7 44 24 04 94 49 00 	movl   $0x4994,0x4(%esp)
    1844:	00 
    1845:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    184c:	e8 8f 27 00 00       	call   3fe0 <printf>
}
    1851:	83 c4 14             	add    $0x14,%esp
    1854:	5b                   	pop    %ebx
    1855:	5d                   	pop    %ebp
    1856:	c3                   	ret    
    printf(1, "create lf1 failed\n");
    1857:	c7 44 24 04 fe 48 00 	movl   $0x48fe,0x4(%esp)
    185e:	00 
    185f:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1866:	e8 75 27 00 00       	call   3fe0 <printf>
    exit(0);
    186b:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1872:	e8 1b 26 00 00       	call   3e92 <exit>
    printf(1, "link . lf1 succeeded! oops\n");
    1877:	c7 44 24 04 78 49 00 	movl   $0x4978,0x4(%esp)
    187e:	00 
    187f:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1886:	e8 55 27 00 00       	call   3fe0 <printf>
    exit(0);
    188b:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1892:	e8 fb 25 00 00       	call   3e92 <exit>
    printf(1, "link non-existant succeeded! oops\n");
    1897:	c7 44 24 04 2c 55 00 	movl   $0x552c,0x4(%esp)
    189e:	00 
    189f:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    18a6:	e8 35 27 00 00       	call   3fe0 <printf>
    exit(0);
    18ab:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    18b2:	e8 db 25 00 00       	call   3e92 <exit>
    printf(1, "link lf2 lf2 succeeded! oops\n");
    18b7:	c7 44 24 04 5a 49 00 	movl   $0x495a,0x4(%esp)
    18be:	00 
    18bf:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    18c6:	e8 15 27 00 00       	call   3fe0 <printf>
    exit(0);
    18cb:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    18d2:	e8 bb 25 00 00       	call   3e92 <exit>
    printf(1, "read lf2 failed\n");
    18d7:	c7 44 24 04 49 49 00 	movl   $0x4949,0x4(%esp)
    18de:	00 
    18df:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    18e6:	e8 f5 26 00 00       	call   3fe0 <printf>
    exit(0);
    18eb:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    18f2:	e8 9b 25 00 00       	call   3e92 <exit>
    printf(1, "open lf2 failed\n");
    18f7:	c7 44 24 04 38 49 00 	movl   $0x4938,0x4(%esp)
    18fe:	00 
    18ff:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1906:	e8 d5 26 00 00       	call   3fe0 <printf>
    exit(0);
    190b:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1912:	e8 7b 25 00 00       	call   3e92 <exit>
    printf(1, "unlinked lf1 but it is still there!\n");
    1917:	c7 44 24 04 04 55 00 	movl   $0x5504,0x4(%esp)
    191e:	00 
    191f:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1926:	e8 b5 26 00 00       	call   3fe0 <printf>
    exit(0);
    192b:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1932:	e8 5b 25 00 00       	call   3e92 <exit>
    printf(1, "link lf1 lf2 failed\n");
    1937:	c7 44 24 04 23 49 00 	movl   $0x4923,0x4(%esp)
    193e:	00 
    193f:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1946:	e8 95 26 00 00       	call   3fe0 <printf>
    exit(0);
    194b:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1952:	e8 3b 25 00 00       	call   3e92 <exit>
    printf(1, "write lf1 failed\n");
    1957:	c7 44 24 04 11 49 00 	movl   $0x4911,0x4(%esp)
    195e:	00 
    195f:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1966:	e8 75 26 00 00       	call   3fe0 <printf>
    exit(0);
    196b:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1972:	e8 1b 25 00 00       	call   3e92 <exit>
    1977:	89 f6                	mov    %esi,%esi
    1979:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00001980 <concreate>:
{
    1980:	55                   	push   %ebp
    1981:	89 e5                	mov    %esp,%ebp
    1983:	57                   	push   %edi
    1984:	56                   	push   %esi
  for(i = 0; i < 40; i++){
    1985:	31 f6                	xor    %esi,%esi
{
    1987:	53                   	push   %ebx
    1988:	83 ec 5c             	sub    $0x5c,%esp
  printf(1, "concreate test\n");
    198b:	c7 44 24 04 a1 49 00 	movl   $0x49a1,0x4(%esp)
    1992:	00 
    1993:	8d 5d ad             	lea    -0x53(%ebp),%ebx
    1996:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    199d:	e8 3e 26 00 00       	call   3fe0 <printf>
  file[0] = 'C';
    19a2:	c6 45 ad 43          	movb   $0x43,-0x53(%ebp)
  file[2] = '\0';
    19a6:	c6 45 af 00          	movb   $0x0,-0x51(%ebp)
    19aa:	eb 53                	jmp    19ff <concreate+0x7f>
    19ac:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    if(pid && (i % 3) == 1){
    19b0:	b8 56 55 55 55       	mov    $0x55555556,%eax
    19b5:	89 f1                	mov    %esi,%ecx
    19b7:	f7 ee                	imul   %esi
    19b9:	89 f0                	mov    %esi,%eax
    19bb:	c1 f8 1f             	sar    $0x1f,%eax
    19be:	29 c2                	sub    %eax,%edx
    19c0:	8d 04 52             	lea    (%edx,%edx,2),%eax
    19c3:	29 c1                	sub    %eax,%ecx
    19c5:	83 f9 01             	cmp    $0x1,%ecx
    19c8:	0f 84 82 00 00 00    	je     1a50 <concreate+0xd0>
      fd = open(file, O_CREATE | O_RDWR);
    19ce:	c7 44 24 04 02 02 00 	movl   $0x202,0x4(%esp)
    19d5:	00 
    19d6:	89 1c 24             	mov    %ebx,(%esp)
    19d9:	e8 f4 24 00 00       	call   3ed2 <open>
      if(fd < 0){
    19de:	85 c0                	test   %eax,%eax
    19e0:	0f 88 4e 02 00 00    	js     1c34 <concreate+0x2b4>
      close(fd);
    19e6:	89 04 24             	mov    %eax,(%esp)
    19e9:	e8 cc 24 00 00       	call   3eba <close>
    if(pid == 0)
    19ee:	85 ff                	test   %edi,%edi
    19f0:	74 52                	je     1a44 <concreate+0xc4>
  for(i = 0; i < 40; i++){
    19f2:	83 c6 01             	add    $0x1,%esi
      wait();
    19f5:	e8 a0 24 00 00       	call   3e9a <wait>
  for(i = 0; i < 40; i++){
    19fa:	83 fe 28             	cmp    $0x28,%esi
    19fd:	74 71                	je     1a70 <concreate+0xf0>
    19ff:	8d 46 30             	lea    0x30(%esi),%eax
    unlink(file);
    1a02:	89 1c 24             	mov    %ebx,(%esp)
    1a05:	88 45 ae             	mov    %al,-0x52(%ebp)
    1a08:	e8 d5 24 00 00       	call   3ee2 <unlink>
    pid = fork();
    1a0d:	e8 78 24 00 00       	call   3e8a <fork>
    if(pid && (i % 3) == 1){
    1a12:	85 c0                	test   %eax,%eax
    pid = fork();
    1a14:	89 c7                	mov    %eax,%edi
    if(pid && (i % 3) == 1){
    1a16:	75 98                	jne    19b0 <concreate+0x30>
    } else if(pid == 0 && (i % 5) == 1){
    1a18:	b8 67 66 66 66       	mov    $0x66666667,%eax
    1a1d:	89 f1                	mov    %esi,%ecx
    1a1f:	f7 ee                	imul   %esi
    1a21:	89 f0                	mov    %esi,%eax
    1a23:	c1 f8 1f             	sar    $0x1f,%eax
    1a26:	d1 fa                	sar    %edx
    1a28:	29 c2                	sub    %eax,%edx
    1a2a:	8d 04 92             	lea    (%edx,%edx,4),%eax
    1a2d:	29 c1                	sub    %eax,%ecx
    1a2f:	83 f9 01             	cmp    $0x1,%ecx
    1a32:	75 9a                	jne    19ce <concreate+0x4e>
      link("C0", file);
    1a34:	89 5c 24 04          	mov    %ebx,0x4(%esp)
    1a38:	c7 04 24 b1 49 00 00 	movl   $0x49b1,(%esp)
    1a3f:	e8 ae 24 00 00       	call   3ef2 <link>
        exit(0);
    1a44:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1a4b:	e8 42 24 00 00       	call   3e92 <exit>
      link("C0", file);
    1a50:	89 5c 24 04          	mov    %ebx,0x4(%esp)
  for(i = 0; i < 40; i++){
    1a54:	83 c6 01             	add    $0x1,%esi
      link("C0", file);
    1a57:	c7 04 24 b1 49 00 00 	movl   $0x49b1,(%esp)
    1a5e:	e8 8f 24 00 00       	call   3ef2 <link>
      wait();
    1a63:	e8 32 24 00 00       	call   3e9a <wait>
  for(i = 0; i < 40; i++){
    1a68:	83 fe 28             	cmp    $0x28,%esi
    1a6b:	75 92                	jne    19ff <concreate+0x7f>
    1a6d:	8d 76 00             	lea    0x0(%esi),%esi
  memset(fa, 0, sizeof(fa));
    1a70:	8d 45 c0             	lea    -0x40(%ebp),%eax
    1a73:	c7 44 24 08 28 00 00 	movl   $0x28,0x8(%esp)
    1a7a:	00 
    1a7b:	8d 7d b0             	lea    -0x50(%ebp),%edi
    1a7e:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
    1a85:	00 
    1a86:	89 04 24             	mov    %eax,(%esp)
    1a89:	e8 92 22 00 00       	call   3d20 <memset>
  fd = open(".", 0);
    1a8e:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
    1a95:	00 
    1a96:	c7 04 24 be 4b 00 00 	movl   $0x4bbe,(%esp)
    1a9d:	e8 30 24 00 00       	call   3ed2 <open>
  n = 0;
    1aa2:	c7 45 a4 00 00 00 00 	movl   $0x0,-0x5c(%ebp)
  fd = open(".", 0);
    1aa9:	89 c6                	mov    %eax,%esi
    1aab:	90                   	nop
    1aac:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  while(read(fd, &de, sizeof(de)) > 0){
    1ab0:	c7 44 24 08 10 00 00 	movl   $0x10,0x8(%esp)
    1ab7:	00 
    1ab8:	89 7c 24 04          	mov    %edi,0x4(%esp)
    1abc:	89 34 24             	mov    %esi,(%esp)
    1abf:	e8 e6 23 00 00       	call   3eaa <read>
    1ac4:	85 c0                	test   %eax,%eax
    1ac6:	7e 40                	jle    1b08 <concreate+0x188>
    if(de.inum == 0)
    1ac8:	66 83 7d b0 00       	cmpw   $0x0,-0x50(%ebp)
    1acd:	74 e1                	je     1ab0 <concreate+0x130>
    if(de.name[0] == 'C' && de.name[2] == '\0'){
    1acf:	80 7d b2 43          	cmpb   $0x43,-0x4e(%ebp)
    1ad3:	75 db                	jne    1ab0 <concreate+0x130>
    1ad5:	80 7d b4 00          	cmpb   $0x0,-0x4c(%ebp)
    1ad9:	75 d5                	jne    1ab0 <concreate+0x130>
      i = de.name[1] - '0';
    1adb:	0f be 45 b3          	movsbl -0x4d(%ebp),%eax
    1adf:	83 e8 30             	sub    $0x30,%eax
      if(i < 0 || i >= sizeof(fa)){
    1ae2:	83 f8 27             	cmp    $0x27,%eax
    1ae5:	0f 87 6d 01 00 00    	ja     1c58 <concreate+0x2d8>
      if(fa[i]){
    1aeb:	80 7c 05 c0 00       	cmpb   $0x0,-0x40(%ebp,%eax,1)
    1af0:	0f 85 a2 01 00 00    	jne    1c98 <concreate+0x318>
      fa[i] = 1;
    1af6:	c6 44 05 c0 01       	movb   $0x1,-0x40(%ebp,%eax,1)
      n++;
    1afb:	83 45 a4 01          	addl   $0x1,-0x5c(%ebp)
    1aff:	eb af                	jmp    1ab0 <concreate+0x130>
    1b01:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  close(fd);
    1b08:	89 34 24             	mov    %esi,(%esp)
    1b0b:	e8 aa 23 00 00       	call   3eba <close>
  if(n != 40){
    1b10:	83 7d a4 28          	cmpl   $0x28,-0x5c(%ebp)
    1b14:	0f 85 5e 01 00 00    	jne    1c78 <concreate+0x2f8>
    1b1a:	31 f6                	xor    %esi,%esi
    1b1c:	eb 7f                	jmp    1b9d <concreate+0x21d>
    1b1e:	66 90                	xchg   %ax,%ax
       ((i % 3) == 1 && pid != 0)){
    1b20:	85 ff                	test   %edi,%edi
    1b22:	0f 84 ae 00 00 00    	je     1bd6 <concreate+0x256>
      close(open(file, 0));
    1b28:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
    1b2f:	00 
    1b30:	89 1c 24             	mov    %ebx,(%esp)
    1b33:	e8 9a 23 00 00       	call   3ed2 <open>
    1b38:	89 04 24             	mov    %eax,(%esp)
    1b3b:	e8 7a 23 00 00       	call   3eba <close>
      close(open(file, 0));
    1b40:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
    1b47:	00 
    1b48:	89 1c 24             	mov    %ebx,(%esp)
    1b4b:	e8 82 23 00 00       	call   3ed2 <open>
    1b50:	89 04 24             	mov    %eax,(%esp)
    1b53:	e8 62 23 00 00       	call   3eba <close>
      close(open(file, 0));
    1b58:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
    1b5f:	00 
    1b60:	89 1c 24             	mov    %ebx,(%esp)
    1b63:	e8 6a 23 00 00       	call   3ed2 <open>
    1b68:	89 04 24             	mov    %eax,(%esp)
    1b6b:	e8 4a 23 00 00       	call   3eba <close>
      close(open(file, 0));
    1b70:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
    1b77:	00 
    1b78:	89 1c 24             	mov    %ebx,(%esp)
    1b7b:	e8 52 23 00 00       	call   3ed2 <open>
    1b80:	89 04 24             	mov    %eax,(%esp)
    1b83:	e8 32 23 00 00       	call   3eba <close>
    if(pid == 0)
    1b88:	85 ff                	test   %edi,%edi
    1b8a:	0f 84 b4 fe ff ff    	je     1a44 <concreate+0xc4>
  for(i = 0; i < 40; i++){
    1b90:	83 c6 01             	add    $0x1,%esi
      wait();
    1b93:	e8 02 23 00 00       	call   3e9a <wait>
  for(i = 0; i < 40; i++){
    1b98:	83 fe 28             	cmp    $0x28,%esi
    1b9b:	74 5b                	je     1bf8 <concreate+0x278>
    1b9d:	8d 46 30             	lea    0x30(%esi),%eax
    1ba0:	88 45 ae             	mov    %al,-0x52(%ebp)
    pid = fork();
    1ba3:	e8 e2 22 00 00       	call   3e8a <fork>
    if(pid < 0){
    1ba8:	85 c0                	test   %eax,%eax
    pid = fork();
    1baa:	89 c7                	mov    %eax,%edi
    if(pid < 0){
    1bac:	78 66                	js     1c14 <concreate+0x294>
    if(((i % 3) == 0 && pid == 0) ||
    1bae:	b8 56 55 55 55       	mov    $0x55555556,%eax
    1bb3:	f7 ee                	imul   %esi
    1bb5:	89 f0                	mov    %esi,%eax
    1bb7:	c1 f8 1f             	sar    $0x1f,%eax
    1bba:	29 c2                	sub    %eax,%edx
    1bbc:	8d 04 52             	lea    (%edx,%edx,2),%eax
    1bbf:	89 f2                	mov    %esi,%edx
    1bc1:	29 c2                	sub    %eax,%edx
    1bc3:	89 d0                	mov    %edx,%eax
    1bc5:	09 f8                	or     %edi,%eax
    1bc7:	0f 84 5b ff ff ff    	je     1b28 <concreate+0x1a8>
    1bcd:	83 fa 01             	cmp    $0x1,%edx
    1bd0:	0f 84 4a ff ff ff    	je     1b20 <concreate+0x1a0>
      unlink(file);
    1bd6:	89 1c 24             	mov    %ebx,(%esp)
    1bd9:	e8 04 23 00 00       	call   3ee2 <unlink>
      unlink(file);
    1bde:	89 1c 24             	mov    %ebx,(%esp)
    1be1:	e8 fc 22 00 00       	call   3ee2 <unlink>
      unlink(file);
    1be6:	89 1c 24             	mov    %ebx,(%esp)
    1be9:	e8 f4 22 00 00       	call   3ee2 <unlink>
      unlink(file);
    1bee:	89 1c 24             	mov    %ebx,(%esp)
    1bf1:	e8 ec 22 00 00       	call   3ee2 <unlink>
    1bf6:	eb 90                	jmp    1b88 <concreate+0x208>
  printf(1, "concreate ok\n");
    1bf8:	c7 44 24 04 06 4a 00 	movl   $0x4a06,0x4(%esp)
    1bff:	00 
    1c00:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1c07:	e8 d4 23 00 00       	call   3fe0 <printf>
}
    1c0c:	83 c4 5c             	add    $0x5c,%esp
    1c0f:	5b                   	pop    %ebx
    1c10:	5e                   	pop    %esi
    1c11:	5f                   	pop    %edi
    1c12:	5d                   	pop    %ebp
    1c13:	c3                   	ret    
      printf(1, "fork failed\n");
    1c14:	c7 44 24 04 89 52 00 	movl   $0x5289,0x4(%esp)
    1c1b:	00 
    1c1c:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1c23:	e8 b8 23 00 00       	call   3fe0 <printf>
      exit(0);
    1c28:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1c2f:	e8 5e 22 00 00       	call   3e92 <exit>
        printf(1, "concreate create %s failed\n", file);
    1c34:	89 5c 24 08          	mov    %ebx,0x8(%esp)
    1c38:	c7 44 24 04 b4 49 00 	movl   $0x49b4,0x4(%esp)
    1c3f:	00 
    1c40:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1c47:	e8 94 23 00 00       	call   3fe0 <printf>
        exit(0);
    1c4c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1c53:	e8 3a 22 00 00       	call   3e92 <exit>
        printf(1, "concreate weird file %s\n", de.name);
    1c58:	8d 45 b2             	lea    -0x4e(%ebp),%eax
    1c5b:	89 44 24 08          	mov    %eax,0x8(%esp)
    1c5f:	c7 44 24 04 d0 49 00 	movl   $0x49d0,0x4(%esp)
    1c66:	00 
    1c67:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1c6e:	e8 6d 23 00 00       	call   3fe0 <printf>
    1c73:	e9 cc fd ff ff       	jmp    1a44 <concreate+0xc4>
    printf(1, "concreate not enough files in directory listing\n");
    1c78:	c7 44 24 04 50 55 00 	movl   $0x5550,0x4(%esp)
    1c7f:	00 
    1c80:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1c87:	e8 54 23 00 00       	call   3fe0 <printf>
    exit(0);
    1c8c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1c93:	e8 fa 21 00 00       	call   3e92 <exit>
        printf(1, "concreate duplicate file %s\n", de.name);
    1c98:	8d 45 b2             	lea    -0x4e(%ebp),%eax
    1c9b:	89 44 24 08          	mov    %eax,0x8(%esp)
    1c9f:	c7 44 24 04 e9 49 00 	movl   $0x49e9,0x4(%esp)
    1ca6:	00 
    1ca7:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1cae:	e8 2d 23 00 00       	call   3fe0 <printf>
        exit(0);
    1cb3:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1cba:	e8 d3 21 00 00       	call   3e92 <exit>
    1cbf:	90                   	nop

00001cc0 <linkunlink>:
{
    1cc0:	55                   	push   %ebp
    1cc1:	89 e5                	mov    %esp,%ebp
    1cc3:	57                   	push   %edi
    1cc4:	56                   	push   %esi
    1cc5:	53                   	push   %ebx
    1cc6:	83 ec 1c             	sub    $0x1c,%esp
  printf(1, "linkunlink test\n");
    1cc9:	c7 44 24 04 14 4a 00 	movl   $0x4a14,0x4(%esp)
    1cd0:	00 
    1cd1:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1cd8:	e8 03 23 00 00       	call   3fe0 <printf>
  unlink("x");
    1cdd:	c7 04 24 a1 4c 00 00 	movl   $0x4ca1,(%esp)
    1ce4:	e8 f9 21 00 00       	call   3ee2 <unlink>
  pid = fork();
    1ce9:	e8 9c 21 00 00       	call   3e8a <fork>
  if(pid < 0){
    1cee:	85 c0                	test   %eax,%eax
  pid = fork();
    1cf0:	89 45 e4             	mov    %eax,-0x1c(%ebp)
  if(pid < 0){
    1cf3:	0f 88 b8 00 00 00    	js     1db1 <linkunlink+0xf1>
  unsigned int x = (pid ? 1 : 97);
    1cf9:	83 7d e4 01          	cmpl   $0x1,-0x1c(%ebp)
    1cfd:	bb 64 00 00 00       	mov    $0x64,%ebx
    if((x % 3) == 0){
    1d02:	be ab aa aa aa       	mov    $0xaaaaaaab,%esi
  unsigned int x = (pid ? 1 : 97);
    1d07:	19 ff                	sbb    %edi,%edi
    1d09:	83 e7 60             	and    $0x60,%edi
    1d0c:	83 c7 01             	add    $0x1,%edi
    1d0f:	eb 1d                	jmp    1d2e <linkunlink+0x6e>
    1d11:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
    } else if((x % 3) == 1){
    1d18:	83 fa 01             	cmp    $0x1,%edx
    1d1b:	74 7b                	je     1d98 <linkunlink+0xd8>
      unlink("x");
    1d1d:	c7 04 24 a1 4c 00 00 	movl   $0x4ca1,(%esp)
    1d24:	e8 b9 21 00 00       	call   3ee2 <unlink>
  for(i = 0; i < 100; i++){
    1d29:	83 eb 01             	sub    $0x1,%ebx
    1d2c:	74 3c                	je     1d6a <linkunlink+0xaa>
    x = x * 1103515245 + 12345;
    1d2e:	69 cf 6d 4e c6 41    	imul   $0x41c64e6d,%edi,%ecx
    1d34:	8d b9 39 30 00 00    	lea    0x3039(%ecx),%edi
    if((x % 3) == 0){
    1d3a:	89 f8                	mov    %edi,%eax
    1d3c:	f7 e6                	mul    %esi
    1d3e:	d1 ea                	shr    %edx
    1d40:	8d 04 52             	lea    (%edx,%edx,2),%eax
    1d43:	89 fa                	mov    %edi,%edx
    1d45:	29 c2                	sub    %eax,%edx
    1d47:	75 cf                	jne    1d18 <linkunlink+0x58>
      close(open("x", O_RDWR | O_CREATE));
    1d49:	c7 44 24 04 02 02 00 	movl   $0x202,0x4(%esp)
    1d50:	00 
    1d51:	c7 04 24 a1 4c 00 00 	movl   $0x4ca1,(%esp)
    1d58:	e8 75 21 00 00       	call   3ed2 <open>
    1d5d:	89 04 24             	mov    %eax,(%esp)
    1d60:	e8 55 21 00 00       	call   3eba <close>
  for(i = 0; i < 100; i++){
    1d65:	83 eb 01             	sub    $0x1,%ebx
    1d68:	75 c4                	jne    1d2e <linkunlink+0x6e>
  if(pid)
    1d6a:	8b 45 e4             	mov    -0x1c(%ebp),%eax
    1d6d:	85 c0                	test   %eax,%eax
    1d6f:	74 60                	je     1dd1 <linkunlink+0x111>
    wait();
    1d71:	e8 24 21 00 00       	call   3e9a <wait>
  printf(1, "linkunlink ok\n");
    1d76:	c7 44 24 04 29 4a 00 	movl   $0x4a29,0x4(%esp)
    1d7d:	00 
    1d7e:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1d85:	e8 56 22 00 00       	call   3fe0 <printf>
}
    1d8a:	83 c4 1c             	add    $0x1c,%esp
    1d8d:	5b                   	pop    %ebx
    1d8e:	5e                   	pop    %esi
    1d8f:	5f                   	pop    %edi
    1d90:	5d                   	pop    %ebp
    1d91:	c3                   	ret    
    1d92:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
      link("cat", "x");
    1d98:	c7 44 24 04 a1 4c 00 	movl   $0x4ca1,0x4(%esp)
    1d9f:	00 
    1da0:	c7 04 24 25 4a 00 00 	movl   $0x4a25,(%esp)
    1da7:	e8 46 21 00 00       	call   3ef2 <link>
    1dac:	e9 78 ff ff ff       	jmp    1d29 <linkunlink+0x69>
    printf(1, "fork failed\n");
    1db1:	c7 44 24 04 89 52 00 	movl   $0x5289,0x4(%esp)
    1db8:	00 
    1db9:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1dc0:	e8 1b 22 00 00       	call   3fe0 <printf>
    exit(0);
    1dc5:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1dcc:	e8 c1 20 00 00       	call   3e92 <exit>
    exit(0);
    1dd1:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1dd8:	e8 b5 20 00 00       	call   3e92 <exit>
    1ddd:	8d 76 00             	lea    0x0(%esi),%esi

00001de0 <bigdir>:
{
    1de0:	55                   	push   %ebp
    1de1:	89 e5                	mov    %esp,%ebp
    1de3:	56                   	push   %esi
    1de4:	53                   	push   %ebx
    1de5:	83 ec 20             	sub    $0x20,%esp
  printf(1, "bigdir test\n");
    1de8:	c7 44 24 04 38 4a 00 	movl   $0x4a38,0x4(%esp)
    1def:	00 
    1df0:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1df7:	e8 e4 21 00 00       	call   3fe0 <printf>
  unlink("bd");
    1dfc:	c7 04 24 45 4a 00 00 	movl   $0x4a45,(%esp)
    1e03:	e8 da 20 00 00       	call   3ee2 <unlink>
  fd = open("bd", O_CREATE);
    1e08:	c7 44 24 04 00 02 00 	movl   $0x200,0x4(%esp)
    1e0f:	00 
    1e10:	c7 04 24 45 4a 00 00 	movl   $0x4a45,(%esp)
    1e17:	e8 b6 20 00 00       	call   3ed2 <open>
  if(fd < 0){
    1e1c:	85 c0                	test   %eax,%eax
    1e1e:	0f 88 f4 00 00 00    	js     1f18 <bigdir+0x138>
  close(fd);
    1e24:	89 04 24             	mov    %eax,(%esp)
  for(i = 0; i < 500; i++){
    1e27:	31 db                	xor    %ebx,%ebx
  close(fd);
    1e29:	e8 8c 20 00 00       	call   3eba <close>
    1e2e:	8d 75 ee             	lea    -0x12(%ebp),%esi
    1e31:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
    name[1] = '0' + (i / 64);
    1e38:	89 d8                	mov    %ebx,%eax
    1e3a:	c1 f8 06             	sar    $0x6,%eax
    1e3d:	83 c0 30             	add    $0x30,%eax
    1e40:	88 45 ef             	mov    %al,-0x11(%ebp)
    name[2] = '0' + (i % 64);
    1e43:	89 d8                	mov    %ebx,%eax
    1e45:	83 e0 3f             	and    $0x3f,%eax
    1e48:	83 c0 30             	add    $0x30,%eax
    if(link("bd", name) != 0){
    1e4b:	89 74 24 04          	mov    %esi,0x4(%esp)
    1e4f:	c7 04 24 45 4a 00 00 	movl   $0x4a45,(%esp)
    name[0] = 'x';
    1e56:	c6 45 ee 78          	movb   $0x78,-0x12(%ebp)
    name[2] = '0' + (i % 64);
    1e5a:	88 45 f0             	mov    %al,-0x10(%ebp)
    name[3] = '\0';
    1e5d:	c6 45 f1 00          	movb   $0x0,-0xf(%ebp)
    if(link("bd", name) != 0){
    1e61:	e8 8c 20 00 00       	call   3ef2 <link>
    1e66:	85 c0                	test   %eax,%eax
    1e68:	75 6e                	jne    1ed8 <bigdir+0xf8>
  for(i = 0; i < 500; i++){
    1e6a:	83 c3 01             	add    $0x1,%ebx
    1e6d:	81 fb f4 01 00 00    	cmp    $0x1f4,%ebx
    1e73:	75 c3                	jne    1e38 <bigdir+0x58>
  unlink("bd");
    1e75:	c7 04 24 45 4a 00 00 	movl   $0x4a45,(%esp)
  for(i = 0; i < 500; i++){
    1e7c:	66 31 db             	xor    %bx,%bx
  unlink("bd");
    1e7f:	e8 5e 20 00 00       	call   3ee2 <unlink>
    1e84:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    name[1] = '0' + (i / 64);
    1e88:	89 d8                	mov    %ebx,%eax
    1e8a:	c1 f8 06             	sar    $0x6,%eax
    1e8d:	83 c0 30             	add    $0x30,%eax
    1e90:	88 45 ef             	mov    %al,-0x11(%ebp)
    name[2] = '0' + (i % 64);
    1e93:	89 d8                	mov    %ebx,%eax
    1e95:	83 e0 3f             	and    $0x3f,%eax
    1e98:	83 c0 30             	add    $0x30,%eax
    if(unlink(name) != 0){
    1e9b:	89 34 24             	mov    %esi,(%esp)
    name[0] = 'x';
    1e9e:	c6 45 ee 78          	movb   $0x78,-0x12(%ebp)
    name[2] = '0' + (i % 64);
    1ea2:	88 45 f0             	mov    %al,-0x10(%ebp)
    name[3] = '\0';
    1ea5:	c6 45 f1 00          	movb   $0x0,-0xf(%ebp)
    if(unlink(name) != 0){
    1ea9:	e8 34 20 00 00       	call   3ee2 <unlink>
    1eae:	85 c0                	test   %eax,%eax
    1eb0:	75 46                	jne    1ef8 <bigdir+0x118>
  for(i = 0; i < 500; i++){
    1eb2:	83 c3 01             	add    $0x1,%ebx
    1eb5:	81 fb f4 01 00 00    	cmp    $0x1f4,%ebx
    1ebb:	75 cb                	jne    1e88 <bigdir+0xa8>
  printf(1, "bigdir ok\n");
    1ebd:	c7 44 24 04 87 4a 00 	movl   $0x4a87,0x4(%esp)
    1ec4:	00 
    1ec5:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1ecc:	e8 0f 21 00 00       	call   3fe0 <printf>
}
    1ed1:	83 c4 20             	add    $0x20,%esp
    1ed4:	5b                   	pop    %ebx
    1ed5:	5e                   	pop    %esi
    1ed6:	5d                   	pop    %ebp
    1ed7:	c3                   	ret    
      printf(1, "bigdir link failed\n");
    1ed8:	c7 44 24 04 5e 4a 00 	movl   $0x4a5e,0x4(%esp)
    1edf:	00 
    1ee0:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1ee7:	e8 f4 20 00 00       	call   3fe0 <printf>
      exit(0);
    1eec:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1ef3:	e8 9a 1f 00 00       	call   3e92 <exit>
      printf(1, "bigdir unlink failed");
    1ef8:	c7 44 24 04 72 4a 00 	movl   $0x4a72,0x4(%esp)
    1eff:	00 
    1f00:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1f07:	e8 d4 20 00 00       	call   3fe0 <printf>
      exit(0);
    1f0c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1f13:	e8 7a 1f 00 00       	call   3e92 <exit>
    printf(1, "bigdir create failed\n");
    1f18:	c7 44 24 04 48 4a 00 	movl   $0x4a48,0x4(%esp)
    1f1f:	00 
    1f20:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1f27:	e8 b4 20 00 00       	call   3fe0 <printf>
    exit(0);
    1f2c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    1f33:	e8 5a 1f 00 00       	call   3e92 <exit>
    1f38:	90                   	nop
    1f39:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00001f40 <subdir>:
{
    1f40:	55                   	push   %ebp
    1f41:	89 e5                	mov    %esp,%ebp
    1f43:	53                   	push   %ebx
    1f44:	83 ec 14             	sub    $0x14,%esp
  printf(1, "subdir test\n");
    1f47:	c7 44 24 04 92 4a 00 	movl   $0x4a92,0x4(%esp)
    1f4e:	00 
    1f4f:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    1f56:	e8 85 20 00 00       	call   3fe0 <printf>
  unlink("ff");
    1f5b:	c7 04 24 1b 4b 00 00 	movl   $0x4b1b,(%esp)
    1f62:	e8 7b 1f 00 00       	call   3ee2 <unlink>
  if(mkdir("dd") != 0){
    1f67:	c7 04 24 b8 4b 00 00 	movl   $0x4bb8,(%esp)
    1f6e:	e8 87 1f 00 00       	call   3efa <mkdir>
    1f73:	85 c0                	test   %eax,%eax
    1f75:	0f 85 a1 06 00 00    	jne    261c <subdir+0x6dc>
  fd = open("dd/ff", O_CREATE | O_RDWR);
    1f7b:	c7 44 24 04 02 02 00 	movl   $0x202,0x4(%esp)
    1f82:	00 
    1f83:	c7 04 24 f1 4a 00 00 	movl   $0x4af1,(%esp)
    1f8a:	e8 43 1f 00 00       	call   3ed2 <open>
  if(fd < 0){
    1f8f:	85 c0                	test   %eax,%eax
  fd = open("dd/ff", O_CREATE | O_RDWR);
    1f91:	89 c3                	mov    %eax,%ebx
  if(fd < 0){
    1f93:	0f 88 63 06 00 00    	js     25fc <subdir+0x6bc>
  write(fd, "ff", 2);
    1f99:	c7 44 24 08 02 00 00 	movl   $0x2,0x8(%esp)
    1fa0:	00 
    1fa1:	c7 44 24 04 1b 4b 00 	movl   $0x4b1b,0x4(%esp)
    1fa8:	00 
    1fa9:	89 04 24             	mov    %eax,(%esp)
    1fac:	e8 01 1f 00 00       	call   3eb2 <write>
  close(fd);
    1fb1:	89 1c 24             	mov    %ebx,(%esp)
    1fb4:	e8 01 1f 00 00       	call   3eba <close>
  if(unlink("dd") >= 0){
    1fb9:	c7 04 24 b8 4b 00 00 	movl   $0x4bb8,(%esp)
    1fc0:	e8 1d 1f 00 00       	call   3ee2 <unlink>
    1fc5:	85 c0                	test   %eax,%eax
    1fc7:	0f 89 0f 06 00 00    	jns    25dc <subdir+0x69c>
  if(mkdir("/dd/dd") != 0){
    1fcd:	c7 04 24 cc 4a 00 00 	movl   $0x4acc,(%esp)
    1fd4:	e8 21 1f 00 00       	call   3efa <mkdir>
    1fd9:	85 c0                	test   %eax,%eax
    1fdb:	0f 85 db 05 00 00    	jne    25bc <subdir+0x67c>
  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
    1fe1:	c7 44 24 04 02 02 00 	movl   $0x202,0x4(%esp)
    1fe8:	00 
    1fe9:	c7 04 24 ee 4a 00 00 	movl   $0x4aee,(%esp)
    1ff0:	e8 dd 1e 00 00       	call   3ed2 <open>
  if(fd < 0){
    1ff5:	85 c0                	test   %eax,%eax
  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
    1ff7:	89 c3                	mov    %eax,%ebx
  if(fd < 0){
    1ff9:	0f 88 5d 04 00 00    	js     245c <subdir+0x51c>
  write(fd, "FF", 2);
    1fff:	c7 44 24 08 02 00 00 	movl   $0x2,0x8(%esp)
    2006:	00 
    2007:	c7 44 24 04 0f 4b 00 	movl   $0x4b0f,0x4(%esp)
    200e:	00 
    200f:	89 04 24             	mov    %eax,(%esp)
    2012:	e8 9b 1e 00 00       	call   3eb2 <write>
  close(fd);
    2017:	89 1c 24             	mov    %ebx,(%esp)
    201a:	e8 9b 1e 00 00       	call   3eba <close>
  fd = open("dd/dd/../ff", 0);
    201f:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
    2026:	00 
    2027:	c7 04 24 12 4b 00 00 	movl   $0x4b12,(%esp)
    202e:	e8 9f 1e 00 00       	call   3ed2 <open>
  if(fd < 0){
    2033:	85 c0                	test   %eax,%eax
  fd = open("dd/dd/../ff", 0);
    2035:	89 c3                	mov    %eax,%ebx
  if(fd < 0){
    2037:	0f 88 ff 03 00 00    	js     243c <subdir+0x4fc>
  cc = read(fd, buf, sizeof(buf));
    203d:	c7 44 24 08 00 20 00 	movl   $0x2000,0x8(%esp)
    2044:	00 
    2045:	c7 44 24 04 a0 8b 00 	movl   $0x8ba0,0x4(%esp)
    204c:	00 
    204d:	89 04 24             	mov    %eax,(%esp)
    2050:	e8 55 1e 00 00       	call   3eaa <read>
  if(cc != 2 || buf[0] != 'f'){
    2055:	83 f8 02             	cmp    $0x2,%eax
    2058:	0f 85 fe 02 00 00    	jne    235c <subdir+0x41c>
    205e:	80 3d a0 8b 00 00 66 	cmpb   $0x66,0x8ba0
    2065:	0f 85 f1 02 00 00    	jne    235c <subdir+0x41c>
  close(fd);
    206b:	89 1c 24             	mov    %ebx,(%esp)
    206e:	e8 47 1e 00 00       	call   3eba <close>
  if(link("dd/dd/ff", "dd/dd/ffff") != 0){
    2073:	c7 44 24 04 52 4b 00 	movl   $0x4b52,0x4(%esp)
    207a:	00 
    207b:	c7 04 24 ee 4a 00 00 	movl   $0x4aee,(%esp)
    2082:	e8 6b 1e 00 00       	call   3ef2 <link>
    2087:	85 c0                	test   %eax,%eax
    2089:	0f 85 0d 04 00 00    	jne    249c <subdir+0x55c>
  if(unlink("dd/dd/ff") != 0){
    208f:	c7 04 24 ee 4a 00 00 	movl   $0x4aee,(%esp)
    2096:	e8 47 1e 00 00       	call   3ee2 <unlink>
    209b:	85 c0                	test   %eax,%eax
    209d:	0f 85 f9 02 00 00    	jne    239c <subdir+0x45c>
  if(open("dd/dd/ff", O_RDONLY) >= 0){
    20a3:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
    20aa:	00 
    20ab:	c7 04 24 ee 4a 00 00 	movl   $0x4aee,(%esp)
    20b2:	e8 1b 1e 00 00       	call   3ed2 <open>
    20b7:	85 c0                	test   %eax,%eax
    20b9:	0f 89 dd 04 00 00    	jns    259c <subdir+0x65c>
  if(chdir("dd") != 0){
    20bf:	c7 04 24 b8 4b 00 00 	movl   $0x4bb8,(%esp)
    20c6:	e8 37 1e 00 00       	call   3f02 <chdir>
    20cb:	85 c0                	test   %eax,%eax
    20cd:	0f 85 a9 04 00 00    	jne    257c <subdir+0x63c>
  if(chdir("dd/../../dd") != 0){
    20d3:	c7 04 24 86 4b 00 00 	movl   $0x4b86,(%esp)
    20da:	e8 23 1e 00 00       	call   3f02 <chdir>
    20df:	85 c0                	test   %eax,%eax
    20e1:	0f 85 95 02 00 00    	jne    237c <subdir+0x43c>
  if(chdir("dd/../../../dd") != 0){
    20e7:	c7 04 24 ac 4b 00 00 	movl   $0x4bac,(%esp)
    20ee:	e8 0f 1e 00 00       	call   3f02 <chdir>
    20f3:	85 c0                	test   %eax,%eax
    20f5:	0f 85 81 02 00 00    	jne    237c <subdir+0x43c>
  if(chdir("./..") != 0){
    20fb:	c7 04 24 bb 4b 00 00 	movl   $0x4bbb,(%esp)
    2102:	e8 fb 1d 00 00       	call   3f02 <chdir>
    2107:	85 c0                	test   %eax,%eax
    2109:	0f 85 6d 03 00 00    	jne    247c <subdir+0x53c>
  fd = open("dd/dd/ffff", 0);
    210f:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
    2116:	00 
    2117:	c7 04 24 52 4b 00 00 	movl   $0x4b52,(%esp)
    211e:	e8 af 1d 00 00       	call   3ed2 <open>
  if(fd < 0){
    2123:	85 c0                	test   %eax,%eax
  fd = open("dd/dd/ffff", 0);
    2125:	89 c3                	mov    %eax,%ebx
  if(fd < 0){
    2127:	0f 88 6f 06 00 00    	js     279c <subdir+0x85c>
  if(read(fd, buf, sizeof(buf)) != 2){
    212d:	c7 44 24 08 00 20 00 	movl   $0x2000,0x8(%esp)
    2134:	00 
    2135:	c7 44 24 04 a0 8b 00 	movl   $0x8ba0,0x4(%esp)
    213c:	00 
    213d:	89 04 24             	mov    %eax,(%esp)
    2140:	e8 65 1d 00 00       	call   3eaa <read>
    2145:	83 f8 02             	cmp    $0x2,%eax
    2148:	0f 85 2e 06 00 00    	jne    277c <subdir+0x83c>
  close(fd);
    214e:	89 1c 24             	mov    %ebx,(%esp)
    2151:	e8 64 1d 00 00       	call   3eba <close>
  if(open("dd/dd/ff", O_RDONLY) >= 0){
    2156:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
    215d:	00 
    215e:	c7 04 24 ee 4a 00 00 	movl   $0x4aee,(%esp)
    2165:	e8 68 1d 00 00       	call   3ed2 <open>
    216a:	85 c0                	test   %eax,%eax
    216c:	0f 89 6a 02 00 00    	jns    23dc <subdir+0x49c>
  if(open("dd/ff/ff", O_CREATE|O_RDWR) >= 0){
    2172:	c7 44 24 04 02 02 00 	movl   $0x202,0x4(%esp)
    2179:	00 
    217a:	c7 04 24 06 4c 00 00 	movl   $0x4c06,(%esp)
    2181:	e8 4c 1d 00 00       	call   3ed2 <open>
    2186:	85 c0                	test   %eax,%eax
    2188:	0f 89 2e 02 00 00    	jns    23bc <subdir+0x47c>
  if(open("dd/xx/ff", O_CREATE|O_RDWR) >= 0){
    218e:	c7 44 24 04 02 02 00 	movl   $0x202,0x4(%esp)
    2195:	00 
    2196:	c7 04 24 2b 4c 00 00 	movl   $0x4c2b,(%esp)
    219d:	e8 30 1d 00 00       	call   3ed2 <open>
    21a2:	85 c0                	test   %eax,%eax
    21a4:	0f 89 b2 03 00 00    	jns    255c <subdir+0x61c>
  if(open("dd", O_CREATE) >= 0){
    21aa:	c7 44 24 04 00 02 00 	movl   $0x200,0x4(%esp)
    21b1:	00 
    21b2:	c7 04 24 b8 4b 00 00 	movl   $0x4bb8,(%esp)
    21b9:	e8 14 1d 00 00       	call   3ed2 <open>
    21be:	85 c0                	test   %eax,%eax
    21c0:	0f 89 76 03 00 00    	jns    253c <subdir+0x5fc>
  if(open("dd", O_RDWR) >= 0){
    21c6:	c7 44 24 04 02 00 00 	movl   $0x2,0x4(%esp)
    21cd:	00 
    21ce:	c7 04 24 b8 4b 00 00 	movl   $0x4bb8,(%esp)
    21d5:	e8 f8 1c 00 00       	call   3ed2 <open>
    21da:	85 c0                	test   %eax,%eax
    21dc:	0f 89 3a 03 00 00    	jns    251c <subdir+0x5dc>
  if(open("dd", O_WRONLY) >= 0){
    21e2:	c7 44 24 04 01 00 00 	movl   $0x1,0x4(%esp)
    21e9:	00 
    21ea:	c7 04 24 b8 4b 00 00 	movl   $0x4bb8,(%esp)
    21f1:	e8 dc 1c 00 00       	call   3ed2 <open>
    21f6:	85 c0                	test   %eax,%eax
    21f8:	0f 89 fe 02 00 00    	jns    24fc <subdir+0x5bc>
  if(link("dd/ff/ff", "dd/dd/xx") == 0){
    21fe:	c7 44 24 04 9a 4c 00 	movl   $0x4c9a,0x4(%esp)
    2205:	00 
    2206:	c7 04 24 06 4c 00 00 	movl   $0x4c06,(%esp)
    220d:	e8 e0 1c 00 00       	call   3ef2 <link>
    2212:	85 c0                	test   %eax,%eax
    2214:	0f 84 c2 02 00 00    	je     24dc <subdir+0x59c>
  if(link("dd/xx/ff", "dd/dd/xx") == 0){
    221a:	c7 44 24 04 9a 4c 00 	movl   $0x4c9a,0x4(%esp)
    2221:	00 
    2222:	c7 04 24 2b 4c 00 00 	movl   $0x4c2b,(%esp)
    2229:	e8 c4 1c 00 00       	call   3ef2 <link>
    222e:	85 c0                	test   %eax,%eax
    2230:	0f 84 86 02 00 00    	je     24bc <subdir+0x57c>
  if(link("dd/ff", "dd/dd/ffff") == 0){
    2236:	c7 44 24 04 52 4b 00 	movl   $0x4b52,0x4(%esp)
    223d:	00 
    223e:	c7 04 24 f1 4a 00 00 	movl   $0x4af1,(%esp)
    2245:	e8 a8 1c 00 00       	call   3ef2 <link>
    224a:	85 c0                	test   %eax,%eax
    224c:	0f 84 ca 01 00 00    	je     241c <subdir+0x4dc>
  if(mkdir("dd/ff/ff") == 0){
    2252:	c7 04 24 06 4c 00 00 	movl   $0x4c06,(%esp)
    2259:	e8 9c 1c 00 00       	call   3efa <mkdir>
    225e:	85 c0                	test   %eax,%eax
    2260:	0f 84 96 01 00 00    	je     23fc <subdir+0x4bc>
  if(mkdir("dd/xx/ff") == 0){
    2266:	c7 04 24 2b 4c 00 00 	movl   $0x4c2b,(%esp)
    226d:	e8 88 1c 00 00       	call   3efa <mkdir>
    2272:	85 c0                	test   %eax,%eax
    2274:	0f 84 e2 04 00 00    	je     275c <subdir+0x81c>
  if(mkdir("dd/dd/ffff") == 0){
    227a:	c7 04 24 52 4b 00 00 	movl   $0x4b52,(%esp)
    2281:	e8 74 1c 00 00       	call   3efa <mkdir>
    2286:	85 c0                	test   %eax,%eax
    2288:	0f 84 ae 04 00 00    	je     273c <subdir+0x7fc>
  if(unlink("dd/xx/ff") == 0){
    228e:	c7 04 24 2b 4c 00 00 	movl   $0x4c2b,(%esp)
    2295:	e8 48 1c 00 00       	call   3ee2 <unlink>
    229a:	85 c0                	test   %eax,%eax
    229c:	0f 84 7a 04 00 00    	je     271c <subdir+0x7dc>
  if(unlink("dd/ff/ff") == 0){
    22a2:	c7 04 24 06 4c 00 00 	movl   $0x4c06,(%esp)
    22a9:	e8 34 1c 00 00       	call   3ee2 <unlink>
    22ae:	85 c0                	test   %eax,%eax
    22b0:	0f 84 46 04 00 00    	je     26fc <subdir+0x7bc>
  if(chdir("dd/ff") == 0){
    22b6:	c7 04 24 f1 4a 00 00 	movl   $0x4af1,(%esp)
    22bd:	e8 40 1c 00 00       	call   3f02 <chdir>
    22c2:	85 c0                	test   %eax,%eax
    22c4:	0f 84 12 04 00 00    	je     26dc <subdir+0x79c>
  if(chdir("dd/xx") == 0){
    22ca:	c7 04 24 9d 4c 00 00 	movl   $0x4c9d,(%esp)
    22d1:	e8 2c 1c 00 00       	call   3f02 <chdir>
    22d6:	85 c0                	test   %eax,%eax
    22d8:	0f 84 de 03 00 00    	je     26bc <subdir+0x77c>
  if(unlink("dd/dd/ffff") != 0){
    22de:	c7 04 24 52 4b 00 00 	movl   $0x4b52,(%esp)
    22e5:	e8 f8 1b 00 00       	call   3ee2 <unlink>
    22ea:	85 c0                	test   %eax,%eax
    22ec:	0f 85 aa 00 00 00    	jne    239c <subdir+0x45c>
  if(unlink("dd/ff") != 0){
    22f2:	c7 04 24 f1 4a 00 00 	movl   $0x4af1,(%esp)
    22f9:	e8 e4 1b 00 00       	call   3ee2 <unlink>
    22fe:	85 c0                	test   %eax,%eax
    2300:	0f 85 96 03 00 00    	jne    269c <subdir+0x75c>
  if(unlink("dd") == 0){
    2306:	c7 04 24 b8 4b 00 00 	movl   $0x4bb8,(%esp)
    230d:	e8 d0 1b 00 00       	call   3ee2 <unlink>
    2312:	85 c0                	test   %eax,%eax
    2314:	0f 84 62 03 00 00    	je     267c <subdir+0x73c>
  if(unlink("dd/dd") < 0){
    231a:	c7 04 24 cd 4a 00 00 	movl   $0x4acd,(%esp)
    2321:	e8 bc 1b 00 00       	call   3ee2 <unlink>
    2326:	85 c0                	test   %eax,%eax
    2328:	0f 88 2e 03 00 00    	js     265c <subdir+0x71c>
  if(unlink("dd") < 0){
    232e:	c7 04 24 b8 4b 00 00 	movl   $0x4bb8,(%esp)
    2335:	e8 a8 1b 00 00       	call   3ee2 <unlink>
    233a:	85 c0                	test   %eax,%eax
    233c:	0f 88 fa 02 00 00    	js     263c <subdir+0x6fc>
  printf(1, "subdir ok\n");
    2342:	c7 44 24 04 9a 4d 00 	movl   $0x4d9a,0x4(%esp)
    2349:	00 
    234a:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2351:	e8 8a 1c 00 00       	call   3fe0 <printf>
}
    2356:	83 c4 14             	add    $0x14,%esp
    2359:	5b                   	pop    %ebx
    235a:	5d                   	pop    %ebp
    235b:	c3                   	ret    
    printf(1, "dd/dd/../ff wrong content\n");
    235c:	c7 44 24 04 37 4b 00 	movl   $0x4b37,0x4(%esp)
    2363:	00 
    2364:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    236b:	e8 70 1c 00 00       	call   3fe0 <printf>
    exit(0);
    2370:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2377:	e8 16 1b 00 00       	call   3e92 <exit>
    printf(1, "chdir dd/../../dd failed\n");
    237c:	c7 44 24 04 92 4b 00 	movl   $0x4b92,0x4(%esp)
    2383:	00 
    2384:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    238b:	e8 50 1c 00 00       	call   3fe0 <printf>
    exit(0);
    2390:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2397:	e8 f6 1a 00 00       	call   3e92 <exit>
    printf(1, "unlink dd/dd/ff failed\n");
    239c:	c7 44 24 04 5d 4b 00 	movl   $0x4b5d,0x4(%esp)
    23a3:	00 
    23a4:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    23ab:	e8 30 1c 00 00       	call   3fe0 <printf>
    exit(0);
    23b0:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    23b7:	e8 d6 1a 00 00       	call   3e92 <exit>
    printf(1, "create dd/ff/ff succeeded!\n");
    23bc:	c7 44 24 04 0f 4c 00 	movl   $0x4c0f,0x4(%esp)
    23c3:	00 
    23c4:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    23cb:	e8 10 1c 00 00       	call   3fe0 <printf>
    exit(0);
    23d0:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    23d7:	e8 b6 1a 00 00       	call   3e92 <exit>
    printf(1, "open (unlinked) dd/dd/ff succeeded!\n");
    23dc:	c7 44 24 04 f4 55 00 	movl   $0x55f4,0x4(%esp)
    23e3:	00 
    23e4:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    23eb:	e8 f0 1b 00 00       	call   3fe0 <printf>
    exit(0);
    23f0:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    23f7:	e8 96 1a 00 00       	call   3e92 <exit>
    printf(1, "mkdir dd/ff/ff succeeded!\n");
    23fc:	c7 44 24 04 a3 4c 00 	movl   $0x4ca3,0x4(%esp)
    2403:	00 
    2404:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    240b:	e8 d0 1b 00 00       	call   3fe0 <printf>
    exit(0);
    2410:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2417:	e8 76 1a 00 00       	call   3e92 <exit>
    printf(1, "link dd/ff dd/dd/ffff succeeded!\n");
    241c:	c7 44 24 04 64 56 00 	movl   $0x5664,0x4(%esp)
    2423:	00 
    2424:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    242b:	e8 b0 1b 00 00       	call   3fe0 <printf>
    exit(0);
    2430:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2437:	e8 56 1a 00 00       	call   3e92 <exit>
    printf(1, "open dd/dd/../ff failed\n");
    243c:	c7 44 24 04 1e 4b 00 	movl   $0x4b1e,0x4(%esp)
    2443:	00 
    2444:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    244b:	e8 90 1b 00 00       	call   3fe0 <printf>
    exit(0);
    2450:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2457:	e8 36 1a 00 00       	call   3e92 <exit>
    printf(1, "create dd/dd/ff failed\n");
    245c:	c7 44 24 04 f7 4a 00 	movl   $0x4af7,0x4(%esp)
    2463:	00 
    2464:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    246b:	e8 70 1b 00 00       	call   3fe0 <printf>
    exit(0);
    2470:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2477:	e8 16 1a 00 00       	call   3e92 <exit>
    printf(1, "chdir ./.. failed\n");
    247c:	c7 44 24 04 c0 4b 00 	movl   $0x4bc0,0x4(%esp)
    2483:	00 
    2484:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    248b:	e8 50 1b 00 00       	call   3fe0 <printf>
    exit(0);
    2490:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2497:	e8 f6 19 00 00       	call   3e92 <exit>
    printf(1, "link dd/dd/ff dd/dd/ffff failed\n");
    249c:	c7 44 24 04 ac 55 00 	movl   $0x55ac,0x4(%esp)
    24a3:	00 
    24a4:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    24ab:	e8 30 1b 00 00       	call   3fe0 <printf>
    exit(0);
    24b0:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    24b7:	e8 d6 19 00 00       	call   3e92 <exit>
    printf(1, "link dd/xx/ff dd/dd/xx succeeded!\n");
    24bc:	c7 44 24 04 40 56 00 	movl   $0x5640,0x4(%esp)
    24c3:	00 
    24c4:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    24cb:	e8 10 1b 00 00       	call   3fe0 <printf>
    exit(0);
    24d0:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    24d7:	e8 b6 19 00 00       	call   3e92 <exit>
    printf(1, "link dd/ff/ff dd/dd/xx succeeded!\n");
    24dc:	c7 44 24 04 1c 56 00 	movl   $0x561c,0x4(%esp)
    24e3:	00 
    24e4:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    24eb:	e8 f0 1a 00 00       	call   3fe0 <printf>
    exit(0);
    24f0:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    24f7:	e8 96 19 00 00       	call   3e92 <exit>
    printf(1, "open dd wronly succeeded!\n");
    24fc:	c7 44 24 04 7f 4c 00 	movl   $0x4c7f,0x4(%esp)
    2503:	00 
    2504:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    250b:	e8 d0 1a 00 00       	call   3fe0 <printf>
    exit(0);
    2510:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2517:	e8 76 19 00 00       	call   3e92 <exit>
    printf(1, "open dd rdwr succeeded!\n");
    251c:	c7 44 24 04 66 4c 00 	movl   $0x4c66,0x4(%esp)
    2523:	00 
    2524:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    252b:	e8 b0 1a 00 00       	call   3fe0 <printf>
    exit(0);
    2530:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2537:	e8 56 19 00 00       	call   3e92 <exit>
    printf(1, "create dd succeeded!\n");
    253c:	c7 44 24 04 50 4c 00 	movl   $0x4c50,0x4(%esp)
    2543:	00 
    2544:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    254b:	e8 90 1a 00 00       	call   3fe0 <printf>
    exit(0);
    2550:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2557:	e8 36 19 00 00       	call   3e92 <exit>
    printf(1, "create dd/xx/ff succeeded!\n");
    255c:	c7 44 24 04 34 4c 00 	movl   $0x4c34,0x4(%esp)
    2563:	00 
    2564:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    256b:	e8 70 1a 00 00       	call   3fe0 <printf>
    exit(0);
    2570:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2577:	e8 16 19 00 00       	call   3e92 <exit>
    printf(1, "chdir dd failed\n");
    257c:	c7 44 24 04 75 4b 00 	movl   $0x4b75,0x4(%esp)
    2583:	00 
    2584:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    258b:	e8 50 1a 00 00       	call   3fe0 <printf>
    exit(0);
    2590:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2597:	e8 f6 18 00 00       	call   3e92 <exit>
    printf(1, "open (unlinked) dd/dd/ff succeeded\n");
    259c:	c7 44 24 04 d0 55 00 	movl   $0x55d0,0x4(%esp)
    25a3:	00 
    25a4:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    25ab:	e8 30 1a 00 00       	call   3fe0 <printf>
    exit(0);
    25b0:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    25b7:	e8 d6 18 00 00       	call   3e92 <exit>
    printf(1, "subdir mkdir dd/dd failed\n");
    25bc:	c7 44 24 04 d3 4a 00 	movl   $0x4ad3,0x4(%esp)
    25c3:	00 
    25c4:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    25cb:	e8 10 1a 00 00       	call   3fe0 <printf>
    exit(0);
    25d0:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    25d7:	e8 b6 18 00 00       	call   3e92 <exit>
    printf(1, "unlink dd (non-empty dir) succeeded!\n");
    25dc:	c7 44 24 04 84 55 00 	movl   $0x5584,0x4(%esp)
    25e3:	00 
    25e4:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    25eb:	e8 f0 19 00 00       	call   3fe0 <printf>
    exit(0);
    25f0:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    25f7:	e8 96 18 00 00       	call   3e92 <exit>
    printf(1, "create dd/ff failed\n");
    25fc:	c7 44 24 04 b7 4a 00 	movl   $0x4ab7,0x4(%esp)
    2603:	00 
    2604:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    260b:	e8 d0 19 00 00       	call   3fe0 <printf>
    exit(0);
    2610:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2617:	e8 76 18 00 00       	call   3e92 <exit>
    printf(1, "subdir mkdir dd failed\n");
    261c:	c7 44 24 04 9f 4a 00 	movl   $0x4a9f,0x4(%esp)
    2623:	00 
    2624:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    262b:	e8 b0 19 00 00       	call   3fe0 <printf>
    exit(0);
    2630:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2637:	e8 56 18 00 00       	call   3e92 <exit>
    printf(1, "unlink dd failed\n");
    263c:	c7 44 24 04 88 4d 00 	movl   $0x4d88,0x4(%esp)
    2643:	00 
    2644:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    264b:	e8 90 19 00 00       	call   3fe0 <printf>
    exit(0);
    2650:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2657:	e8 36 18 00 00       	call   3e92 <exit>
    printf(1, "unlink dd/dd failed\n");
    265c:	c7 44 24 04 73 4d 00 	movl   $0x4d73,0x4(%esp)
    2663:	00 
    2664:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    266b:	e8 70 19 00 00       	call   3fe0 <printf>
    exit(0);
    2670:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2677:	e8 16 18 00 00       	call   3e92 <exit>
    printf(1, "unlink non-empty dd succeeded!\n");
    267c:	c7 44 24 04 88 56 00 	movl   $0x5688,0x4(%esp)
    2683:	00 
    2684:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    268b:	e8 50 19 00 00       	call   3fe0 <printf>
    exit(0);
    2690:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2697:	e8 f6 17 00 00       	call   3e92 <exit>
    printf(1, "unlink dd/ff failed\n");
    269c:	c7 44 24 04 5e 4d 00 	movl   $0x4d5e,0x4(%esp)
    26a3:	00 
    26a4:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    26ab:	e8 30 19 00 00       	call   3fe0 <printf>
    exit(0);
    26b0:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    26b7:	e8 d6 17 00 00       	call   3e92 <exit>
    printf(1, "chdir dd/xx succeeded!\n");
    26bc:	c7 44 24 04 46 4d 00 	movl   $0x4d46,0x4(%esp)
    26c3:	00 
    26c4:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    26cb:	e8 10 19 00 00       	call   3fe0 <printf>
    exit(0);
    26d0:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    26d7:	e8 b6 17 00 00       	call   3e92 <exit>
    printf(1, "chdir dd/ff succeeded!\n");
    26dc:	c7 44 24 04 2e 4d 00 	movl   $0x4d2e,0x4(%esp)
    26e3:	00 
    26e4:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    26eb:	e8 f0 18 00 00       	call   3fe0 <printf>
    exit(0);
    26f0:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    26f7:	e8 96 17 00 00       	call   3e92 <exit>
    printf(1, "unlink dd/ff/ff succeeded!\n");
    26fc:	c7 44 24 04 12 4d 00 	movl   $0x4d12,0x4(%esp)
    2703:	00 
    2704:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    270b:	e8 d0 18 00 00       	call   3fe0 <printf>
    exit(0);
    2710:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2717:	e8 76 17 00 00       	call   3e92 <exit>
    printf(1, "unlink dd/xx/ff succeeded!\n");
    271c:	c7 44 24 04 f6 4c 00 	movl   $0x4cf6,0x4(%esp)
    2723:	00 
    2724:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    272b:	e8 b0 18 00 00       	call   3fe0 <printf>
    exit(0);
    2730:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2737:	e8 56 17 00 00       	call   3e92 <exit>
    printf(1, "mkdir dd/dd/ffff succeeded!\n");
    273c:	c7 44 24 04 d9 4c 00 	movl   $0x4cd9,0x4(%esp)
    2743:	00 
    2744:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    274b:	e8 90 18 00 00       	call   3fe0 <printf>
    exit(0);
    2750:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2757:	e8 36 17 00 00       	call   3e92 <exit>
    printf(1, "mkdir dd/xx/ff succeeded!\n");
    275c:	c7 44 24 04 be 4c 00 	movl   $0x4cbe,0x4(%esp)
    2763:	00 
    2764:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    276b:	e8 70 18 00 00       	call   3fe0 <printf>
    exit(0);
    2770:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2777:	e8 16 17 00 00       	call   3e92 <exit>
    printf(1, "read dd/dd/ffff wrong len\n");
    277c:	c7 44 24 04 eb 4b 00 	movl   $0x4beb,0x4(%esp)
    2783:	00 
    2784:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    278b:	e8 50 18 00 00       	call   3fe0 <printf>
    exit(0);
    2790:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2797:	e8 f6 16 00 00       	call   3e92 <exit>
    printf(1, "open dd/dd/ffff failed\n");
    279c:	c7 44 24 04 d3 4b 00 	movl   $0x4bd3,0x4(%esp)
    27a3:	00 
    27a4:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    27ab:	e8 30 18 00 00       	call   3fe0 <printf>
    exit(0);
    27b0:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    27b7:	e8 d6 16 00 00       	call   3e92 <exit>
    27bc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

000027c0 <bigwrite>:
{
    27c0:	55                   	push   %ebp
    27c1:	89 e5                	mov    %esp,%ebp
    27c3:	56                   	push   %esi
    27c4:	53                   	push   %ebx
  for(sz = 499; sz < 12*512; sz += 471){
    27c5:	bb f3 01 00 00       	mov    $0x1f3,%ebx
{
    27ca:	83 ec 10             	sub    $0x10,%esp
  printf(1, "bigwrite test\n");
    27cd:	c7 44 24 04 a5 4d 00 	movl   $0x4da5,0x4(%esp)
    27d4:	00 
    27d5:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    27dc:	e8 ff 17 00 00       	call   3fe0 <printf>
  unlink("bigwrite");
    27e1:	c7 04 24 b4 4d 00 00 	movl   $0x4db4,(%esp)
    27e8:	e8 f5 16 00 00       	call   3ee2 <unlink>
    27ed:	8d 76 00             	lea    0x0(%esi),%esi
    fd = open("bigwrite", O_CREATE | O_RDWR);
    27f0:	c7 44 24 04 02 02 00 	movl   $0x202,0x4(%esp)
    27f7:	00 
    27f8:	c7 04 24 b4 4d 00 00 	movl   $0x4db4,(%esp)
    27ff:	e8 ce 16 00 00       	call   3ed2 <open>
    if(fd < 0){
    2804:	85 c0                	test   %eax,%eax
    fd = open("bigwrite", O_CREATE | O_RDWR);
    2806:	89 c6                	mov    %eax,%esi
    if(fd < 0){
    2808:	0f 88 95 00 00 00    	js     28a3 <bigwrite+0xe3>
      int cc = write(fd, buf, sz);
    280e:	89 5c 24 08          	mov    %ebx,0x8(%esp)
    2812:	c7 44 24 04 a0 8b 00 	movl   $0x8ba0,0x4(%esp)
    2819:	00 
    281a:	89 04 24             	mov    %eax,(%esp)
    281d:	e8 90 16 00 00       	call   3eb2 <write>
      if(cc != sz){
    2822:	39 d8                	cmp    %ebx,%eax
    2824:	75 55                	jne    287b <bigwrite+0xbb>
      int cc = write(fd, buf, sz);
    2826:	89 5c 24 08          	mov    %ebx,0x8(%esp)
    282a:	c7 44 24 04 a0 8b 00 	movl   $0x8ba0,0x4(%esp)
    2831:	00 
    2832:	89 34 24             	mov    %esi,(%esp)
    2835:	e8 78 16 00 00       	call   3eb2 <write>
      if(cc != sz){
    283a:	39 c3                	cmp    %eax,%ebx
    283c:	75 3d                	jne    287b <bigwrite+0xbb>
    close(fd);
    283e:	89 34 24             	mov    %esi,(%esp)
  for(sz = 499; sz < 12*512; sz += 471){
    2841:	81 c3 d7 01 00 00    	add    $0x1d7,%ebx
    close(fd);
    2847:	e8 6e 16 00 00       	call   3eba <close>
    unlink("bigwrite");
    284c:	c7 04 24 b4 4d 00 00 	movl   $0x4db4,(%esp)
    2853:	e8 8a 16 00 00       	call   3ee2 <unlink>
  for(sz = 499; sz < 12*512; sz += 471){
    2858:	81 fb 07 18 00 00    	cmp    $0x1807,%ebx
    285e:	75 90                	jne    27f0 <bigwrite+0x30>
  printf(1, "bigwrite ok\n");
    2860:	c7 44 24 04 e7 4d 00 	movl   $0x4de7,0x4(%esp)
    2867:	00 
    2868:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    286f:	e8 6c 17 00 00       	call   3fe0 <printf>
}
    2874:	83 c4 10             	add    $0x10,%esp
    2877:	5b                   	pop    %ebx
    2878:	5e                   	pop    %esi
    2879:	5d                   	pop    %ebp
    287a:	c3                   	ret    
        printf(1, "write(%d) ret %d\n", sz, cc);
    287b:	89 44 24 0c          	mov    %eax,0xc(%esp)
    287f:	89 5c 24 08          	mov    %ebx,0x8(%esp)
    2883:	c7 44 24 04 d5 4d 00 	movl   $0x4dd5,0x4(%esp)
    288a:	00 
    288b:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2892:	e8 49 17 00 00       	call   3fe0 <printf>
        exit(0);
    2897:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    289e:	e8 ef 15 00 00       	call   3e92 <exit>
      printf(1, "cannot create bigwrite\n");
    28a3:	c7 44 24 04 bd 4d 00 	movl   $0x4dbd,0x4(%esp)
    28aa:	00 
    28ab:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    28b2:	e8 29 17 00 00       	call   3fe0 <printf>
      exit(0);
    28b7:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    28be:	e8 cf 15 00 00       	call   3e92 <exit>
    28c3:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    28c9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

000028d0 <bigfile>:
{
    28d0:	55                   	push   %ebp
    28d1:	89 e5                	mov    %esp,%ebp
    28d3:	57                   	push   %edi
    28d4:	56                   	push   %esi
    28d5:	53                   	push   %ebx
    28d6:	83 ec 1c             	sub    $0x1c,%esp
  printf(1, "bigfile test\n");
    28d9:	c7 44 24 04 f4 4d 00 	movl   $0x4df4,0x4(%esp)
    28e0:	00 
    28e1:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    28e8:	e8 f3 16 00 00       	call   3fe0 <printf>
  unlink("bigfile");
    28ed:	c7 04 24 10 4e 00 00 	movl   $0x4e10,(%esp)
    28f4:	e8 e9 15 00 00       	call   3ee2 <unlink>
  fd = open("bigfile", O_CREATE | O_RDWR);
    28f9:	c7 44 24 04 02 02 00 	movl   $0x202,0x4(%esp)
    2900:	00 
    2901:	c7 04 24 10 4e 00 00 	movl   $0x4e10,(%esp)
    2908:	e8 c5 15 00 00       	call   3ed2 <open>
  if(fd < 0){
    290d:	85 c0                	test   %eax,%eax
  fd = open("bigfile", O_CREATE | O_RDWR);
    290f:	89 c6                	mov    %eax,%esi
  if(fd < 0){
    2911:	0f 88 a2 01 00 00    	js     2ab9 <bigfile+0x1e9>
    2917:	31 db                	xor    %ebx,%ebx
    2919:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
    memset(buf, i, 600);
    2920:	c7 44 24 08 58 02 00 	movl   $0x258,0x8(%esp)
    2927:	00 
    2928:	89 5c 24 04          	mov    %ebx,0x4(%esp)
    292c:	c7 04 24 a0 8b 00 00 	movl   $0x8ba0,(%esp)
    2933:	e8 e8 13 00 00       	call   3d20 <memset>
    if(write(fd, buf, 600) != 600){
    2938:	c7 44 24 08 58 02 00 	movl   $0x258,0x8(%esp)
    293f:	00 
    2940:	c7 44 24 04 a0 8b 00 	movl   $0x8ba0,0x4(%esp)
    2947:	00 
    2948:	89 34 24             	mov    %esi,(%esp)
    294b:	e8 62 15 00 00       	call   3eb2 <write>
    2950:	3d 58 02 00 00       	cmp    $0x258,%eax
    2955:	0f 85 1e 01 00 00    	jne    2a79 <bigfile+0x1a9>
  for(i = 0; i < 20; i++){
    295b:	83 c3 01             	add    $0x1,%ebx
    295e:	83 fb 14             	cmp    $0x14,%ebx
    2961:	75 bd                	jne    2920 <bigfile+0x50>
  close(fd);
    2963:	89 34 24             	mov    %esi,(%esp)
    2966:	e8 4f 15 00 00       	call   3eba <close>
  fd = open("bigfile", 0);
    296b:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
    2972:	00 
    2973:	c7 04 24 10 4e 00 00 	movl   $0x4e10,(%esp)
    297a:	e8 53 15 00 00       	call   3ed2 <open>
  if(fd < 0){
    297f:	85 c0                	test   %eax,%eax
  fd = open("bigfile", 0);
    2981:	89 c6                	mov    %eax,%esi
  if(fd < 0){
    2983:	0f 88 10 01 00 00    	js     2a99 <bigfile+0x1c9>
    2989:	31 db                	xor    %ebx,%ebx
    298b:	31 ff                	xor    %edi,%edi
    298d:	eb 2f                	jmp    29be <bigfile+0xee>
    298f:	90                   	nop
    if(cc != 300){
    2990:	3d 2c 01 00 00       	cmp    $0x12c,%eax
    2995:	0f 85 9e 00 00 00    	jne    2a39 <bigfile+0x169>
    if(buf[0] != i/2 || buf[299] != i/2){
    299b:	0f be 05 a0 8b 00 00 	movsbl 0x8ba0,%eax
    29a2:	89 fa                	mov    %edi,%edx
    29a4:	d1 fa                	sar    %edx
    29a6:	39 d0                	cmp    %edx,%eax
    29a8:	75 6f                	jne    2a19 <bigfile+0x149>
    29aa:	0f be 15 cb 8c 00 00 	movsbl 0x8ccb,%edx
    29b1:	39 d0                	cmp    %edx,%eax
    29b3:	75 64                	jne    2a19 <bigfile+0x149>
    total += cc;
    29b5:	81 c3 2c 01 00 00    	add    $0x12c,%ebx
  for(i = 0; ; i++){
    29bb:	83 c7 01             	add    $0x1,%edi
    cc = read(fd, buf, 300);
    29be:	c7 44 24 08 2c 01 00 	movl   $0x12c,0x8(%esp)
    29c5:	00 
    29c6:	c7 44 24 04 a0 8b 00 	movl   $0x8ba0,0x4(%esp)
    29cd:	00 
    29ce:	89 34 24             	mov    %esi,(%esp)
    29d1:	e8 d4 14 00 00       	call   3eaa <read>
    if(cc < 0){
    29d6:	85 c0                	test   %eax,%eax
    29d8:	78 7f                	js     2a59 <bigfile+0x189>
    if(cc == 0)
    29da:	75 b4                	jne    2990 <bigfile+0xc0>
  close(fd);
    29dc:	89 34 24             	mov    %esi,(%esp)
    29df:	90                   	nop
    29e0:	e8 d5 14 00 00       	call   3eba <close>
  if(total != 20*600){
    29e5:	81 fb e0 2e 00 00    	cmp    $0x2ee0,%ebx
    29eb:	0f 85 e8 00 00 00    	jne    2ad9 <bigfile+0x209>
  unlink("bigfile");
    29f1:	c7 04 24 10 4e 00 00 	movl   $0x4e10,(%esp)
    29f8:	e8 e5 14 00 00       	call   3ee2 <unlink>
  printf(1, "bigfile test ok\n");
    29fd:	c7 44 24 04 9f 4e 00 	movl   $0x4e9f,0x4(%esp)
    2a04:	00 
    2a05:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2a0c:	e8 cf 15 00 00       	call   3fe0 <printf>
}
    2a11:	83 c4 1c             	add    $0x1c,%esp
    2a14:	5b                   	pop    %ebx
    2a15:	5e                   	pop    %esi
    2a16:	5f                   	pop    %edi
    2a17:	5d                   	pop    %ebp
    2a18:	c3                   	ret    
      printf(1, "read bigfile wrong data\n");
    2a19:	c7 44 24 04 6c 4e 00 	movl   $0x4e6c,0x4(%esp)
    2a20:	00 
    2a21:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2a28:	e8 b3 15 00 00       	call   3fe0 <printf>
      exit(0);
    2a2d:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2a34:	e8 59 14 00 00       	call   3e92 <exit>
      printf(1, "short read bigfile\n");
    2a39:	c7 44 24 04 58 4e 00 	movl   $0x4e58,0x4(%esp)
    2a40:	00 
    2a41:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2a48:	e8 93 15 00 00       	call   3fe0 <printf>
      exit(0);
    2a4d:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2a54:	e8 39 14 00 00       	call   3e92 <exit>
      printf(1, "read bigfile failed\n");
    2a59:	c7 44 24 04 43 4e 00 	movl   $0x4e43,0x4(%esp)
    2a60:	00 
    2a61:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2a68:	e8 73 15 00 00       	call   3fe0 <printf>
      exit(0);
    2a6d:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2a74:	e8 19 14 00 00       	call   3e92 <exit>
      printf(1, "write bigfile failed\n");
    2a79:	c7 44 24 04 18 4e 00 	movl   $0x4e18,0x4(%esp)
    2a80:	00 
    2a81:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2a88:	e8 53 15 00 00       	call   3fe0 <printf>
      exit(0);
    2a8d:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2a94:	e8 f9 13 00 00       	call   3e92 <exit>
    printf(1, "cannot open bigfile\n");
    2a99:	c7 44 24 04 2e 4e 00 	movl   $0x4e2e,0x4(%esp)
    2aa0:	00 
    2aa1:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2aa8:	e8 33 15 00 00       	call   3fe0 <printf>
    exit(0);
    2aad:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2ab4:	e8 d9 13 00 00       	call   3e92 <exit>
    printf(1, "cannot create bigfile");
    2ab9:	c7 44 24 04 02 4e 00 	movl   $0x4e02,0x4(%esp)
    2ac0:	00 
    2ac1:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2ac8:	e8 13 15 00 00       	call   3fe0 <printf>
    exit(0);
    2acd:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2ad4:	e8 b9 13 00 00       	call   3e92 <exit>
    printf(1, "read bigfile wrong total\n");
    2ad9:	c7 44 24 04 85 4e 00 	movl   $0x4e85,0x4(%esp)
    2ae0:	00 
    2ae1:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2ae8:	e8 f3 14 00 00       	call   3fe0 <printf>
    exit(0);
    2aed:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2af4:	e8 99 13 00 00       	call   3e92 <exit>
    2af9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00002b00 <fourteen>:
{
    2b00:	55                   	push   %ebp
    2b01:	89 e5                	mov    %esp,%ebp
    2b03:	83 ec 18             	sub    $0x18,%esp
  printf(1, "fourteen test\n");
    2b06:	c7 44 24 04 b0 4e 00 	movl   $0x4eb0,0x4(%esp)
    2b0d:	00 
    2b0e:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2b15:	e8 c6 14 00 00       	call   3fe0 <printf>
  if(mkdir("12345678901234") != 0){
    2b1a:	c7 04 24 eb 4e 00 00 	movl   $0x4eeb,(%esp)
    2b21:	e8 d4 13 00 00       	call   3efa <mkdir>
    2b26:	85 c0                	test   %eax,%eax
    2b28:	0f 85 92 00 00 00    	jne    2bc0 <fourteen+0xc0>
  if(mkdir("12345678901234/123456789012345") != 0){
    2b2e:	c7 04 24 a8 56 00 00 	movl   $0x56a8,(%esp)
    2b35:	e8 c0 13 00 00       	call   3efa <mkdir>
    2b3a:	85 c0                	test   %eax,%eax
    2b3c:	0f 85 1e 01 00 00    	jne    2c60 <fourteen+0x160>
  fd = open("123456789012345/123456789012345/123456789012345", O_CREATE);
    2b42:	c7 44 24 04 00 02 00 	movl   $0x200,0x4(%esp)
    2b49:	00 
    2b4a:	c7 04 24 f8 56 00 00 	movl   $0x56f8,(%esp)
    2b51:	e8 7c 13 00 00       	call   3ed2 <open>
  if(fd < 0){
    2b56:	85 c0                	test   %eax,%eax
    2b58:	0f 88 e2 00 00 00    	js     2c40 <fourteen+0x140>
  close(fd);
    2b5e:	89 04 24             	mov    %eax,(%esp)
    2b61:	e8 54 13 00 00       	call   3eba <close>
  fd = open("12345678901234/12345678901234/12345678901234", 0);
    2b66:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
    2b6d:	00 
    2b6e:	c7 04 24 68 57 00 00 	movl   $0x5768,(%esp)
    2b75:	e8 58 13 00 00       	call   3ed2 <open>
  if(fd < 0){
    2b7a:	85 c0                	test   %eax,%eax
    2b7c:	0f 88 9e 00 00 00    	js     2c20 <fourteen+0x120>
  close(fd);
    2b82:	89 04 24             	mov    %eax,(%esp)
    2b85:	e8 30 13 00 00       	call   3eba <close>
  if(mkdir("12345678901234/12345678901234") == 0){
    2b8a:	c7 04 24 dc 4e 00 00 	movl   $0x4edc,(%esp)
    2b91:	e8 64 13 00 00       	call   3efa <mkdir>
    2b96:	85 c0                	test   %eax,%eax
    2b98:	74 66                	je     2c00 <fourteen+0x100>
  if(mkdir("123456789012345/12345678901234") == 0){
    2b9a:	c7 04 24 04 58 00 00 	movl   $0x5804,(%esp)
    2ba1:	e8 54 13 00 00       	call   3efa <mkdir>
    2ba6:	85 c0                	test   %eax,%eax
    2ba8:	74 36                	je     2be0 <fourteen+0xe0>
  printf(1, "fourteen ok\n");
    2baa:	c7 44 24 04 fa 4e 00 	movl   $0x4efa,0x4(%esp)
    2bb1:	00 
    2bb2:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2bb9:	e8 22 14 00 00       	call   3fe0 <printf>
}
    2bbe:	c9                   	leave  
    2bbf:	c3                   	ret    
    printf(1, "mkdir 12345678901234 failed\n");
    2bc0:	c7 44 24 04 bf 4e 00 	movl   $0x4ebf,0x4(%esp)
    2bc7:	00 
    2bc8:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2bcf:	e8 0c 14 00 00       	call   3fe0 <printf>
    exit(0);
    2bd4:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2bdb:	e8 b2 12 00 00       	call   3e92 <exit>
    printf(1, "mkdir 12345678901234/123456789012345 succeeded!\n");
    2be0:	c7 44 24 04 24 58 00 	movl   $0x5824,0x4(%esp)
    2be7:	00 
    2be8:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2bef:	e8 ec 13 00 00       	call   3fe0 <printf>
    exit(0);
    2bf4:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2bfb:	e8 92 12 00 00       	call   3e92 <exit>
    printf(1, "mkdir 12345678901234/12345678901234 succeeded!\n");
    2c00:	c7 44 24 04 d4 57 00 	movl   $0x57d4,0x4(%esp)
    2c07:	00 
    2c08:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2c0f:	e8 cc 13 00 00       	call   3fe0 <printf>
    exit(0);
    2c14:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2c1b:	e8 72 12 00 00       	call   3e92 <exit>
    printf(1, "open 12345678901234/12345678901234/12345678901234 failed\n");
    2c20:	c7 44 24 04 98 57 00 	movl   $0x5798,0x4(%esp)
    2c27:	00 
    2c28:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2c2f:	e8 ac 13 00 00       	call   3fe0 <printf>
    exit(0);
    2c34:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2c3b:	e8 52 12 00 00       	call   3e92 <exit>
    printf(1, "create 123456789012345/123456789012345/123456789012345 failed\n");
    2c40:	c7 44 24 04 28 57 00 	movl   $0x5728,0x4(%esp)
    2c47:	00 
    2c48:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2c4f:	e8 8c 13 00 00       	call   3fe0 <printf>
    exit(0);
    2c54:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2c5b:	e8 32 12 00 00       	call   3e92 <exit>
    printf(1, "mkdir 12345678901234/123456789012345 failed\n");
    2c60:	c7 44 24 04 c8 56 00 	movl   $0x56c8,0x4(%esp)
    2c67:	00 
    2c68:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2c6f:	e8 6c 13 00 00       	call   3fe0 <printf>
    exit(0);
    2c74:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2c7b:	e8 12 12 00 00       	call   3e92 <exit>

00002c80 <rmdot>:
{
    2c80:	55                   	push   %ebp
    2c81:	89 e5                	mov    %esp,%ebp
    2c83:	83 ec 18             	sub    $0x18,%esp
  printf(1, "rmdot test\n");
    2c86:	c7 44 24 04 07 4f 00 	movl   $0x4f07,0x4(%esp)
    2c8d:	00 
    2c8e:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2c95:	e8 46 13 00 00       	call   3fe0 <printf>
  if(mkdir("dots") != 0){
    2c9a:	c7 04 24 13 4f 00 00 	movl   $0x4f13,(%esp)
    2ca1:	e8 54 12 00 00       	call   3efa <mkdir>
    2ca6:	85 c0                	test   %eax,%eax
    2ca8:	0f 85 9a 00 00 00    	jne    2d48 <rmdot+0xc8>
  if(chdir("dots") != 0){
    2cae:	c7 04 24 13 4f 00 00 	movl   $0x4f13,(%esp)
    2cb5:	e8 48 12 00 00       	call   3f02 <chdir>
    2cba:	85 c0                	test   %eax,%eax
    2cbc:	0f 85 66 01 00 00    	jne    2e28 <rmdot+0x1a8>
  if(unlink(".") == 0){
    2cc2:	c7 04 24 be 4b 00 00 	movl   $0x4bbe,(%esp)
    2cc9:	e8 14 12 00 00       	call   3ee2 <unlink>
    2cce:	85 c0                	test   %eax,%eax
    2cd0:	0f 84 32 01 00 00    	je     2e08 <rmdot+0x188>
  if(unlink("..") == 0){
    2cd6:	c7 04 24 bd 4b 00 00 	movl   $0x4bbd,(%esp)
    2cdd:	e8 00 12 00 00       	call   3ee2 <unlink>
    2ce2:	85 c0                	test   %eax,%eax
    2ce4:	0f 84 fe 00 00 00    	je     2de8 <rmdot+0x168>
  if(chdir("/") != 0){
    2cea:	c7 04 24 91 43 00 00 	movl   $0x4391,(%esp)
    2cf1:	e8 0c 12 00 00       	call   3f02 <chdir>
    2cf6:	85 c0                	test   %eax,%eax
    2cf8:	0f 85 ca 00 00 00    	jne    2dc8 <rmdot+0x148>
  if(unlink("dots/.") == 0){
    2cfe:	c7 04 24 5b 4f 00 00 	movl   $0x4f5b,(%esp)
    2d05:	e8 d8 11 00 00       	call   3ee2 <unlink>
    2d0a:	85 c0                	test   %eax,%eax
    2d0c:	0f 84 96 00 00 00    	je     2da8 <rmdot+0x128>
  if(unlink("dots/..") == 0){
    2d12:	c7 04 24 79 4f 00 00 	movl   $0x4f79,(%esp)
    2d19:	e8 c4 11 00 00       	call   3ee2 <unlink>
    2d1e:	85 c0                	test   %eax,%eax
    2d20:	74 66                	je     2d88 <rmdot+0x108>
  if(unlink("dots") != 0){
    2d22:	c7 04 24 13 4f 00 00 	movl   $0x4f13,(%esp)
    2d29:	e8 b4 11 00 00       	call   3ee2 <unlink>
    2d2e:	85 c0                	test   %eax,%eax
    2d30:	75 36                	jne    2d68 <rmdot+0xe8>
  printf(1, "rmdot ok\n");
    2d32:	c7 44 24 04 ae 4f 00 	movl   $0x4fae,0x4(%esp)
    2d39:	00 
    2d3a:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2d41:	e8 9a 12 00 00       	call   3fe0 <printf>
}
    2d46:	c9                   	leave  
    2d47:	c3                   	ret    
    printf(1, "mkdir dots failed\n");
    2d48:	c7 44 24 04 18 4f 00 	movl   $0x4f18,0x4(%esp)
    2d4f:	00 
    2d50:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2d57:	e8 84 12 00 00       	call   3fe0 <printf>
    exit(0);
    2d5c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2d63:	e8 2a 11 00 00       	call   3e92 <exit>
    printf(1, "unlink dots failed!\n");
    2d68:	c7 44 24 04 99 4f 00 	movl   $0x4f99,0x4(%esp)
    2d6f:	00 
    2d70:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2d77:	e8 64 12 00 00       	call   3fe0 <printf>
    exit(0);
    2d7c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2d83:	e8 0a 11 00 00       	call   3e92 <exit>
    printf(1, "unlink dots/.. worked!\n");
    2d88:	c7 44 24 04 81 4f 00 	movl   $0x4f81,0x4(%esp)
    2d8f:	00 
    2d90:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2d97:	e8 44 12 00 00       	call   3fe0 <printf>
    exit(0);
    2d9c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2da3:	e8 ea 10 00 00       	call   3e92 <exit>
    printf(1, "unlink dots/. worked!\n");
    2da8:	c7 44 24 04 62 4f 00 	movl   $0x4f62,0x4(%esp)
    2daf:	00 
    2db0:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2db7:	e8 24 12 00 00       	call   3fe0 <printf>
    exit(0);
    2dbc:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2dc3:	e8 ca 10 00 00       	call   3e92 <exit>
    printf(1, "chdir / failed\n");
    2dc8:	c7 44 24 04 93 43 00 	movl   $0x4393,0x4(%esp)
    2dcf:	00 
    2dd0:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2dd7:	e8 04 12 00 00       	call   3fe0 <printf>
    exit(0);
    2ddc:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2de3:	e8 aa 10 00 00       	call   3e92 <exit>
    printf(1, "rm .. worked!\n");
    2de8:	c7 44 24 04 4c 4f 00 	movl   $0x4f4c,0x4(%esp)
    2def:	00 
    2df0:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2df7:	e8 e4 11 00 00       	call   3fe0 <printf>
    exit(0);
    2dfc:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2e03:	e8 8a 10 00 00       	call   3e92 <exit>
    printf(1, "rm . worked!\n");
    2e08:	c7 44 24 04 3e 4f 00 	movl   $0x4f3e,0x4(%esp)
    2e0f:	00 
    2e10:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2e17:	e8 c4 11 00 00       	call   3fe0 <printf>
    exit(0);
    2e1c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2e23:	e8 6a 10 00 00       	call   3e92 <exit>
    printf(1, "chdir dots failed\n");
    2e28:	c7 44 24 04 2b 4f 00 	movl   $0x4f2b,0x4(%esp)
    2e2f:	00 
    2e30:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2e37:	e8 a4 11 00 00       	call   3fe0 <printf>
    exit(0);
    2e3c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2e43:	e8 4a 10 00 00       	call   3e92 <exit>
    2e48:	90                   	nop
    2e49:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00002e50 <dirfile>:
{
    2e50:	55                   	push   %ebp
    2e51:	89 e5                	mov    %esp,%ebp
    2e53:	53                   	push   %ebx
    2e54:	83 ec 14             	sub    $0x14,%esp
  printf(1, "dir vs file\n");
    2e57:	c7 44 24 04 b8 4f 00 	movl   $0x4fb8,0x4(%esp)
    2e5e:	00 
    2e5f:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2e66:	e8 75 11 00 00       	call   3fe0 <printf>
  fd = open("dirfile", O_CREATE);
    2e6b:	c7 44 24 04 00 02 00 	movl   $0x200,0x4(%esp)
    2e72:	00 
    2e73:	c7 04 24 c5 4f 00 00 	movl   $0x4fc5,(%esp)
    2e7a:	e8 53 10 00 00       	call   3ed2 <open>
  if(fd < 0){
    2e7f:	85 c0                	test   %eax,%eax
    2e81:	0f 88 60 01 00 00    	js     2fe7 <dirfile+0x197>
  close(fd);
    2e87:	89 04 24             	mov    %eax,(%esp)
    2e8a:	e8 2b 10 00 00       	call   3eba <close>
  if(chdir("dirfile") == 0){
    2e8f:	c7 04 24 c5 4f 00 00 	movl   $0x4fc5,(%esp)
    2e96:	e8 67 10 00 00       	call   3f02 <chdir>
    2e9b:	85 c0                	test   %eax,%eax
    2e9d:	0f 84 24 01 00 00    	je     2fc7 <dirfile+0x177>
  fd = open("dirfile/xx", 0);
    2ea3:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
    2eaa:	00 
    2eab:	c7 04 24 fe 4f 00 00 	movl   $0x4ffe,(%esp)
    2eb2:	e8 1b 10 00 00       	call   3ed2 <open>
  if(fd >= 0){
    2eb7:	85 c0                	test   %eax,%eax
    2eb9:	0f 89 e8 00 00 00    	jns    2fa7 <dirfile+0x157>
  fd = open("dirfile/xx", O_CREATE);
    2ebf:	c7 44 24 04 00 02 00 	movl   $0x200,0x4(%esp)
    2ec6:	00 
    2ec7:	c7 04 24 fe 4f 00 00 	movl   $0x4ffe,(%esp)
    2ece:	e8 ff 0f 00 00       	call   3ed2 <open>
  if(fd >= 0){
    2ed3:	85 c0                	test   %eax,%eax
    2ed5:	0f 89 cc 00 00 00    	jns    2fa7 <dirfile+0x157>
  if(mkdir("dirfile/xx") == 0){
    2edb:	c7 04 24 fe 4f 00 00 	movl   $0x4ffe,(%esp)
    2ee2:	e8 13 10 00 00       	call   3efa <mkdir>
    2ee7:	85 c0                	test   %eax,%eax
    2ee9:	0f 84 b8 01 00 00    	je     30a7 <dirfile+0x257>
  if(unlink("dirfile/xx") == 0){
    2eef:	c7 04 24 fe 4f 00 00 	movl   $0x4ffe,(%esp)
    2ef6:	e8 e7 0f 00 00       	call   3ee2 <unlink>
    2efb:	85 c0                	test   %eax,%eax
    2efd:	0f 84 84 01 00 00    	je     3087 <dirfile+0x237>
  if(link("README", "dirfile/xx") == 0){
    2f03:	c7 44 24 04 fe 4f 00 	movl   $0x4ffe,0x4(%esp)
    2f0a:	00 
    2f0b:	c7 04 24 62 50 00 00 	movl   $0x5062,(%esp)
    2f12:	e8 db 0f 00 00       	call   3ef2 <link>
    2f17:	85 c0                	test   %eax,%eax
    2f19:	0f 84 48 01 00 00    	je     3067 <dirfile+0x217>
  if(unlink("dirfile") != 0){
    2f1f:	c7 04 24 c5 4f 00 00 	movl   $0x4fc5,(%esp)
    2f26:	e8 b7 0f 00 00       	call   3ee2 <unlink>
    2f2b:	85 c0                	test   %eax,%eax
    2f2d:	0f 85 14 01 00 00    	jne    3047 <dirfile+0x1f7>
  fd = open(".", O_RDWR);
    2f33:	c7 44 24 04 02 00 00 	movl   $0x2,0x4(%esp)
    2f3a:	00 
    2f3b:	c7 04 24 be 4b 00 00 	movl   $0x4bbe,(%esp)
    2f42:	e8 8b 0f 00 00       	call   3ed2 <open>
  if(fd >= 0){
    2f47:	85 c0                	test   %eax,%eax
    2f49:	0f 89 d8 00 00 00    	jns    3027 <dirfile+0x1d7>
  fd = open(".", 0);
    2f4f:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
    2f56:	00 
    2f57:	c7 04 24 be 4b 00 00 	movl   $0x4bbe,(%esp)
    2f5e:	e8 6f 0f 00 00       	call   3ed2 <open>
  if(write(fd, "x", 1) > 0){
    2f63:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
    2f6a:	00 
    2f6b:	c7 44 24 04 a1 4c 00 	movl   $0x4ca1,0x4(%esp)
    2f72:	00 
    2f73:	89 04 24             	mov    %eax,(%esp)
  fd = open(".", 0);
    2f76:	89 c3                	mov    %eax,%ebx
  if(write(fd, "x", 1) > 0){
    2f78:	e8 35 0f 00 00       	call   3eb2 <write>
    2f7d:	85 c0                	test   %eax,%eax
    2f7f:	0f 8f 82 00 00 00    	jg     3007 <dirfile+0x1b7>
  close(fd);
    2f85:	89 1c 24             	mov    %ebx,(%esp)
    2f88:	e8 2d 0f 00 00       	call   3eba <close>
  printf(1, "dir vs file OK\n");
    2f8d:	c7 44 24 04 95 50 00 	movl   $0x5095,0x4(%esp)
    2f94:	00 
    2f95:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2f9c:	e8 3f 10 00 00       	call   3fe0 <printf>
}
    2fa1:	83 c4 14             	add    $0x14,%esp
    2fa4:	5b                   	pop    %ebx
    2fa5:	5d                   	pop    %ebp
    2fa6:	c3                   	ret    
    printf(1, "create dirfile/xx succeeded!\n");
    2fa7:	c7 44 24 04 09 50 00 	movl   $0x5009,0x4(%esp)
    2fae:	00 
    2faf:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2fb6:	e8 25 10 00 00       	call   3fe0 <printf>
    exit(0);
    2fbb:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2fc2:	e8 cb 0e 00 00       	call   3e92 <exit>
    printf(1, "chdir dirfile succeeded!\n");
    2fc7:	c7 44 24 04 e4 4f 00 	movl   $0x4fe4,0x4(%esp)
    2fce:	00 
    2fcf:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2fd6:	e8 05 10 00 00       	call   3fe0 <printf>
    exit(0);
    2fdb:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    2fe2:	e8 ab 0e 00 00       	call   3e92 <exit>
    printf(1, "create dirfile failed\n");
    2fe7:	c7 44 24 04 cd 4f 00 	movl   $0x4fcd,0x4(%esp)
    2fee:	00 
    2fef:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    2ff6:	e8 e5 0f 00 00       	call   3fe0 <printf>
    exit(0);
    2ffb:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    3002:	e8 8b 0e 00 00       	call   3e92 <exit>
    printf(1, "write . succeeded!\n");
    3007:	c7 44 24 04 81 50 00 	movl   $0x5081,0x4(%esp)
    300e:	00 
    300f:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    3016:	e8 c5 0f 00 00       	call   3fe0 <printf>
    exit(0);
    301b:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    3022:	e8 6b 0e 00 00       	call   3e92 <exit>
    printf(1, "open . for writing succeeded!\n");
    3027:	c7 44 24 04 78 58 00 	movl   $0x5878,0x4(%esp)
    302e:	00 
    302f:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    3036:	e8 a5 0f 00 00       	call   3fe0 <printf>
    exit(0);
    303b:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    3042:	e8 4b 0e 00 00       	call   3e92 <exit>
    printf(1, "unlink dirfile failed!\n");
    3047:	c7 44 24 04 69 50 00 	movl   $0x5069,0x4(%esp)
    304e:	00 
    304f:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    3056:	e8 85 0f 00 00       	call   3fe0 <printf>
    exit(0);
    305b:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    3062:	e8 2b 0e 00 00       	call   3e92 <exit>
    printf(1, "link to dirfile/xx succeeded!\n");
    3067:	c7 44 24 04 58 58 00 	movl   $0x5858,0x4(%esp)
    306e:	00 
    306f:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    3076:	e8 65 0f 00 00       	call   3fe0 <printf>
    exit(0);
    307b:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    3082:	e8 0b 0e 00 00       	call   3e92 <exit>
    printf(1, "unlink dirfile/xx succeeded!\n");
    3087:	c7 44 24 04 44 50 00 	movl   $0x5044,0x4(%esp)
    308e:	00 
    308f:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    3096:	e8 45 0f 00 00       	call   3fe0 <printf>
    exit(0);
    309b:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    30a2:	e8 eb 0d 00 00       	call   3e92 <exit>
    printf(1, "mkdir dirfile/xx succeeded!\n");
    30a7:	c7 44 24 04 27 50 00 	movl   $0x5027,0x4(%esp)
    30ae:	00 
    30af:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    30b6:	e8 25 0f 00 00       	call   3fe0 <printf>
    exit(0);
    30bb:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    30c2:	e8 cb 0d 00 00       	call   3e92 <exit>
    30c7:	89 f6                	mov    %esi,%esi
    30c9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

000030d0 <iref>:
{
    30d0:	55                   	push   %ebp
    30d1:	89 e5                	mov    %esp,%ebp
    30d3:	53                   	push   %ebx
  printf(1, "empty file name\n");
    30d4:	bb 33 00 00 00       	mov    $0x33,%ebx
{
    30d9:	83 ec 14             	sub    $0x14,%esp
  printf(1, "empty file name\n");
    30dc:	c7 44 24 04 a5 50 00 	movl   $0x50a5,0x4(%esp)
    30e3:	00 
    30e4:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    30eb:	e8 f0 0e 00 00       	call   3fe0 <printf>
    if(mkdir("irefd") != 0){
    30f0:	c7 04 24 b6 50 00 00 	movl   $0x50b6,(%esp)
    30f7:	e8 fe 0d 00 00       	call   3efa <mkdir>
    30fc:	85 c0                	test   %eax,%eax
    30fe:	0f 85 af 00 00 00    	jne    31b3 <iref+0xe3>
    if(chdir("irefd") != 0){
    3104:	c7 04 24 b6 50 00 00 	movl   $0x50b6,(%esp)
    310b:	e8 f2 0d 00 00       	call   3f02 <chdir>
    3110:	85 c0                	test   %eax,%eax
    3112:	0f 85 bb 00 00 00    	jne    31d3 <iref+0x103>
    mkdir("");
    3118:	c7 04 24 6b 47 00 00 	movl   $0x476b,(%esp)
    311f:	e8 d6 0d 00 00       	call   3efa <mkdir>
    link("README", "");
    3124:	c7 44 24 04 6b 47 00 	movl   $0x476b,0x4(%esp)
    312b:	00 
    312c:	c7 04 24 62 50 00 00 	movl   $0x5062,(%esp)
    3133:	e8 ba 0d 00 00       	call   3ef2 <link>
    fd = open("", O_CREATE);
    3138:	c7 44 24 04 00 02 00 	movl   $0x200,0x4(%esp)
    313f:	00 
    3140:	c7 04 24 6b 47 00 00 	movl   $0x476b,(%esp)
    3147:	e8 86 0d 00 00       	call   3ed2 <open>
    if(fd >= 0)
    314c:	85 c0                	test   %eax,%eax
    314e:	78 08                	js     3158 <iref+0x88>
      close(fd);
    3150:	89 04 24             	mov    %eax,(%esp)
    3153:	e8 62 0d 00 00       	call   3eba <close>
    fd = open("xx", O_CREATE);
    3158:	c7 44 24 04 00 02 00 	movl   $0x200,0x4(%esp)
    315f:	00 
    3160:	c7 04 24 a0 4c 00 00 	movl   $0x4ca0,(%esp)
    3167:	e8 66 0d 00 00       	call   3ed2 <open>
    if(fd >= 0)
    316c:	85 c0                	test   %eax,%eax
    316e:	78 08                	js     3178 <iref+0xa8>
      close(fd);
    3170:	89 04 24             	mov    %eax,(%esp)
    3173:	e8 42 0d 00 00       	call   3eba <close>
    unlink("xx");
    3178:	c7 04 24 a0 4c 00 00 	movl   $0x4ca0,(%esp)
    317f:	e8 5e 0d 00 00       	call   3ee2 <unlink>
  for(i = 0; i < 50 + 1; i++){
    3184:	83 eb 01             	sub    $0x1,%ebx
    3187:	0f 85 63 ff ff ff    	jne    30f0 <iref+0x20>
  chdir("/");
    318d:	c7 04 24 91 43 00 00 	movl   $0x4391,(%esp)
    3194:	e8 69 0d 00 00       	call   3f02 <chdir>
  printf(1, "empty file name OK\n");
    3199:	c7 44 24 04 e4 50 00 	movl   $0x50e4,0x4(%esp)
    31a0:	00 
    31a1:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    31a8:	e8 33 0e 00 00       	call   3fe0 <printf>
}
    31ad:	83 c4 14             	add    $0x14,%esp
    31b0:	5b                   	pop    %ebx
    31b1:	5d                   	pop    %ebp
    31b2:	c3                   	ret    
      printf(1, "mkdir irefd failed\n");
    31b3:	c7 44 24 04 bc 50 00 	movl   $0x50bc,0x4(%esp)
    31ba:	00 
    31bb:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    31c2:	e8 19 0e 00 00       	call   3fe0 <printf>
      exit(0);
    31c7:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    31ce:	e8 bf 0c 00 00       	call   3e92 <exit>
      printf(1, "chdir irefd failed\n");
    31d3:	c7 44 24 04 d0 50 00 	movl   $0x50d0,0x4(%esp)
    31da:	00 
    31db:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    31e2:	e8 f9 0d 00 00       	call   3fe0 <printf>
      exit(0);
    31e7:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    31ee:	e8 9f 0c 00 00       	call   3e92 <exit>
    31f3:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    31f9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00003200 <forktest>:
{
    3200:	55                   	push   %ebp
    3201:	89 e5                	mov    %esp,%ebp
    3203:	53                   	push   %ebx
  for(n=0; n<1000; n++){
    3204:	31 db                	xor    %ebx,%ebx
{
    3206:	83 ec 14             	sub    $0x14,%esp
  printf(1, "fork test\n");
    3209:	c7 44 24 04 f8 50 00 	movl   $0x50f8,0x4(%esp)
    3210:	00 
    3211:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    3218:	e8 c3 0d 00 00       	call   3fe0 <printf>
    321d:	eb 13                	jmp    3232 <forktest+0x32>
    321f:	90                   	nop
    if(pid == 0)
    3220:	0f 84 8e 00 00 00    	je     32b4 <forktest+0xb4>
  for(n=0; n<1000; n++){
    3226:	83 c3 01             	add    $0x1,%ebx
    3229:	81 fb e8 03 00 00    	cmp    $0x3e8,%ebx
    322f:	90                   	nop
    3230:	74 4e                	je     3280 <forktest+0x80>
    pid = fork();
    3232:	e8 53 0c 00 00       	call   3e8a <fork>
    if(pid < 0)
    3237:	85 c0                	test   %eax,%eax
    3239:	79 e5                	jns    3220 <forktest+0x20>
  for(; n > 0; n--){
    323b:	85 db                	test   %ebx,%ebx
    323d:	8d 76 00             	lea    0x0(%esi),%esi
    3240:	74 15                	je     3257 <forktest+0x57>
    3242:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    if(wait() < 0){
    3248:	e8 4d 0c 00 00       	call   3e9a <wait>
    324d:	85 c0                	test   %eax,%eax
    324f:	90                   	nop
    3250:	78 4e                	js     32a0 <forktest+0xa0>
  for(; n > 0; n--){
    3252:	83 eb 01             	sub    $0x1,%ebx
    3255:	75 f1                	jne    3248 <forktest+0x48>
  if(wait() != -1){
    3257:	e8 3e 0c 00 00       	call   3e9a <wait>
    325c:	83 f8 ff             	cmp    $0xffffffff,%eax
    325f:	90                   	nop
    3260:	75 5e                	jne    32c0 <forktest+0xc0>
  printf(1, "fork test OK\n");
    3262:	c7 44 24 04 2a 51 00 	movl   $0x512a,0x4(%esp)
    3269:	00 
    326a:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    3271:	e8 6a 0d 00 00       	call   3fe0 <printf>
}
    3276:	83 c4 14             	add    $0x14,%esp
    3279:	5b                   	pop    %ebx
    327a:	5d                   	pop    %ebp
    327b:	c3                   	ret    
    327c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    printf(1, "fork claimed to work 1000 times!\n");
    3280:	c7 44 24 04 98 58 00 	movl   $0x5898,0x4(%esp)
    3287:	00 
    3288:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    328f:	e8 4c 0d 00 00       	call   3fe0 <printf>
    exit(0);
    3294:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    329b:	e8 f2 0b 00 00       	call   3e92 <exit>
      printf(1, "wait stopped early\n");
    32a0:	c7 44 24 04 03 51 00 	movl   $0x5103,0x4(%esp)
    32a7:	00 
    32a8:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    32af:	e8 2c 0d 00 00       	call   3fe0 <printf>
      exit(0);
    32b4:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    32bb:	e8 d2 0b 00 00       	call   3e92 <exit>
    printf(1, "wait got too many\n");
    32c0:	c7 44 24 04 17 51 00 	movl   $0x5117,0x4(%esp)
    32c7:	00 
    32c8:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    32cf:	e8 0c 0d 00 00       	call   3fe0 <printf>
    exit(0);
    32d4:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    32db:	e8 b2 0b 00 00       	call   3e92 <exit>

000032e0 <sbrktest>:
{
    32e0:	55                   	push   %ebp
    32e1:	89 e5                	mov    %esp,%ebp
    32e3:	57                   	push   %edi
    32e4:	56                   	push   %esi
  for(i = 0; i < 5000; i++){
    32e5:	31 f6                	xor    %esi,%esi
{
    32e7:	53                   	push   %ebx
    32e8:	83 ec 6c             	sub    $0x6c,%esp
  printf(stdout, "sbrk test\n");
    32eb:	a1 c4 63 00 00       	mov    0x63c4,%eax
    32f0:	c7 44 24 04 38 51 00 	movl   $0x5138,0x4(%esp)
    32f7:	00 
    32f8:	89 04 24             	mov    %eax,(%esp)
    32fb:	e8 e0 0c 00 00       	call   3fe0 <printf>
  oldbrk = sbrk(0);
    3300:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    3307:	e8 0e 0c 00 00       	call   3f1a <sbrk>
  a = sbrk(0);
    330c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  oldbrk = sbrk(0);
    3313:	89 45 a4             	mov    %eax,-0x5c(%ebp)
  a = sbrk(0);
    3316:	e8 ff 0b 00 00       	call   3f1a <sbrk>
    331b:	89 c3                	mov    %eax,%ebx
    331d:	8d 76 00             	lea    0x0(%esi),%esi
    b = sbrk(1);
    3320:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    3327:	e8 ee 0b 00 00       	call   3f1a <sbrk>
    if(b != a){
    332c:	39 d8                	cmp    %ebx,%eax
    b = sbrk(1);
    332e:	89 c7                	mov    %eax,%edi
    if(b != a){
    3330:	0f 85 78 02 00 00    	jne    35ae <sbrktest+0x2ce>
  for(i = 0; i < 5000; i++){
    3336:	83 c6 01             	add    $0x1,%esi
    a = b + 1;
    3339:	83 c3 01             	add    $0x1,%ebx
    *b = 1;
    333c:	c6 43 ff 01          	movb   $0x1,-0x1(%ebx)
  for(i = 0; i < 5000; i++){
    3340:	81 fe 88 13 00 00    	cmp    $0x1388,%esi
    3346:	75 d8                	jne    3320 <sbrktest+0x40>
  pid = fork();
    3348:	e8 3d 0b 00 00       	call   3e8a <fork>
  if(pid < 0){
    334d:	85 c0                	test   %eax,%eax
  pid = fork();
    334f:	89 c3                	mov    %eax,%ebx
  if(pid < 0){
    3351:	0f 88 7a 03 00 00    	js     36d1 <sbrktest+0x3f1>
  c = sbrk(1);
    3357:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    335e:	e8 b7 0b 00 00       	call   3f1a <sbrk>
  c = sbrk(1);
    3363:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    336a:	e8 ab 0b 00 00       	call   3f1a <sbrk>
  if(c != a + 1){
    336f:	8d 57 02             	lea    0x2(%edi),%edx
    3372:	39 d0                	cmp    %edx,%eax
    3374:	0f 85 4a 03 00 00    	jne    36c4 <sbrktest+0x3e4>
  if(pid == 0)
    337a:	85 db                	test   %ebx,%ebx
    337c:	0f 84 8e 02 00 00    	je     3610 <sbrktest+0x330>
  wait();
    3382:	e8 13 0b 00 00       	call   3e9a <wait>
  a = sbrk(0);
    3387:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    338e:	e8 87 0b 00 00       	call   3f1a <sbrk>
  amt = (BIG) - (uint)a;
    3393:	ba 00 00 40 06       	mov    $0x6400000,%edx
    3398:	29 c2                	sub    %eax,%edx
  a = sbrk(0);
    339a:	89 c3                	mov    %eax,%ebx
  p = sbrk(amt);
    339c:	89 14 24             	mov    %edx,(%esp)
    339f:	e8 76 0b 00 00       	call   3f1a <sbrk>
  if (p != a) {
    33a4:	39 d8                	cmp    %ebx,%eax
    33a6:	0f 85 ec 02 00 00    	jne    3698 <sbrktest+0x3b8>
  *lastaddr = 99;
    33ac:	c6 05 ff ff 3f 06 63 	movb   $0x63,0x63fffff
  a = sbrk(0);
    33b3:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    33ba:	e8 5b 0b 00 00       	call   3f1a <sbrk>
  c = sbrk(-4096);
    33bf:	c7 04 24 00 f0 ff ff 	movl   $0xfffff000,(%esp)
  a = sbrk(0);
    33c6:	89 c3                	mov    %eax,%ebx
  c = sbrk(-4096);
    33c8:	e8 4d 0b 00 00       	call   3f1a <sbrk>
  if(c == (char*)0xffffffff){
    33cd:	83 f8 ff             	cmp    $0xffffffff,%eax
    33d0:	0f 84 e1 02 00 00    	je     36b7 <sbrktest+0x3d7>
  c = sbrk(0);
    33d6:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    33dd:	e8 38 0b 00 00       	call   3f1a <sbrk>
  if(c != a - 4096){
    33e2:	8d 93 00 f0 ff ff    	lea    -0x1000(%ebx),%edx
    33e8:	39 d0                	cmp    %edx,%eax
    33ea:	0f 85 b5 02 00 00    	jne    36a5 <sbrktest+0x3c5>
  a = sbrk(0);
    33f0:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    33f7:	e8 1e 0b 00 00       	call   3f1a <sbrk>
  c = sbrk(4096);
    33fc:	c7 04 24 00 10 00 00 	movl   $0x1000,(%esp)
  a = sbrk(0);
    3403:	89 c6                	mov    %eax,%esi
  c = sbrk(4096);
    3405:	e8 10 0b 00 00       	call   3f1a <sbrk>
  if(c != a || sbrk(0) != a + 4096){
    340a:	39 f0                	cmp    %esi,%eax
  c = sbrk(4096);
    340c:	89 c3                	mov    %eax,%ebx
  if(c != a || sbrk(0) != a + 4096){
    340e:	0f 85 72 02 00 00    	jne    3686 <sbrktest+0x3a6>
    3414:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    341b:	e8 fa 0a 00 00       	call   3f1a <sbrk>
    3420:	8d 93 00 10 00 00    	lea    0x1000(%ebx),%edx
    3426:	39 d0                	cmp    %edx,%eax
    3428:	0f 85 58 02 00 00    	jne    3686 <sbrktest+0x3a6>
  if(*lastaddr == 99){
    342e:	80 3d ff ff 3f 06 63 	cmpb   $0x63,0x63fffff
    3435:	0f 84 3e 02 00 00    	je     3679 <sbrktest+0x399>
  a = sbrk(0);
    343b:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    3442:	e8 d3 0a 00 00       	call   3f1a <sbrk>
  c = sbrk(-(sbrk(0) - oldbrk));
    3447:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  a = sbrk(0);
    344e:	89 c3                	mov    %eax,%ebx
  c = sbrk(-(sbrk(0) - oldbrk));
    3450:	e8 c5 0a 00 00       	call   3f1a <sbrk>
    3455:	8b 4d a4             	mov    -0x5c(%ebp),%ecx
    3458:	29 c1                	sub    %eax,%ecx
    345a:	89 0c 24             	mov    %ecx,(%esp)
    345d:	e8 b8 0a 00 00       	call   3f1a <sbrk>
  if(c != a){
    3462:	39 d8                	cmp    %ebx,%eax
    3464:	0f 85 f0 01 00 00    	jne    365a <sbrktest+0x37a>
    346a:	bb 00 00 00 80       	mov    $0x80000000,%ebx
    346f:	90                   	nop
    ppid = getpid();
    3470:	e8 9d 0a 00 00       	call   3f12 <getpid>
    3475:	89 c6                	mov    %eax,%esi
    pid = fork();
    3477:	e8 0e 0a 00 00       	call   3e8a <fork>
    if(pid < 0){
    347c:	85 c0                	test   %eax,%eax
    347e:	0f 88 cc 01 00 00    	js     3650 <sbrktest+0x370>
    if(pid == 0){
    3484:	0f 84 92 01 00 00    	je     361c <sbrktest+0x33c>
  for(a = (char*)(KERNBASE); a < (char*) (KERNBASE+2000000); a += 50000){
    348a:	81 c3 50 c3 00 00    	add    $0xc350,%ebx
    wait();
    3490:	e8 05 0a 00 00       	call   3e9a <wait>
  for(a = (char*)(KERNBASE); a < (char*) (KERNBASE+2000000); a += 50000){
    3495:	81 fb 80 84 1e 80    	cmp    $0x801e8480,%ebx
    349b:	75 d3                	jne    3470 <sbrktest+0x190>
  if(pipe(fds) != 0){
    349d:	8d 45 b8             	lea    -0x48(%ebp),%eax
    34a0:	89 04 24             	mov    %eax,(%esp)
    34a3:	e8 fa 09 00 00       	call   3ea2 <pipe>
    34a8:	85 c0                	test   %eax,%eax
    34aa:	0f 85 4c 01 00 00    	jne    35fc <sbrktest+0x31c>
    34b0:	8d 5d e8             	lea    -0x18(%ebp),%ebx
    34b3:	8d 75 c0             	lea    -0x40(%ebp),%esi
      read(fds[0], &scratch, 1);
    34b6:	8d 7d b7             	lea    -0x49(%ebp),%edi
    if((pids[i] = fork()) == 0){
    34b9:	e8 cc 09 00 00       	call   3e8a <fork>
    34be:	85 c0                	test   %eax,%eax
    34c0:	89 06                	mov    %eax,(%esi)
    34c2:	0f 84 9f 00 00 00    	je     3567 <sbrktest+0x287>
    if(pids[i] != -1)
    34c8:	83 f8 ff             	cmp    $0xffffffff,%eax
    34cb:	74 17                	je     34e4 <sbrktest+0x204>
      read(fds[0], &scratch, 1);
    34cd:	8b 45 b8             	mov    -0x48(%ebp),%eax
    34d0:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
    34d7:	00 
    34d8:	89 7c 24 04          	mov    %edi,0x4(%esp)
    34dc:	89 04 24             	mov    %eax,(%esp)
    34df:	e8 c6 09 00 00       	call   3eaa <read>
    34e4:	83 c6 04             	add    $0x4,%esi
  for(i = 0; i < sizeof(pids)/sizeof(pids[0]); i++){
    34e7:	39 de                	cmp    %ebx,%esi
    34e9:	75 ce                	jne    34b9 <sbrktest+0x1d9>
  c = sbrk(4096);
    34eb:	c7 04 24 00 10 00 00 	movl   $0x1000,(%esp)
    34f2:	8d 75 c0             	lea    -0x40(%ebp),%esi
    34f5:	e8 20 0a 00 00       	call   3f1a <sbrk>
    34fa:	89 c7                	mov    %eax,%edi
    if(pids[i] == -1)
    34fc:	8b 06                	mov    (%esi),%eax
    34fe:	83 f8 ff             	cmp    $0xffffffff,%eax
    3501:	74 0d                	je     3510 <sbrktest+0x230>
    kill(pids[i]);
    3503:	89 04 24             	mov    %eax,(%esp)
    3506:	e8 b7 09 00 00       	call   3ec2 <kill>
    wait();
    350b:	e8 8a 09 00 00       	call   3e9a <wait>
    3510:	83 c6 04             	add    $0x4,%esi
  for(i = 0; i < sizeof(pids)/sizeof(pids[0]); i++){
    3513:	39 f3                	cmp    %esi,%ebx
    3515:	75 e5                	jne    34fc <sbrktest+0x21c>
  if(c == (char*)0xffffffff){
    3517:	83 ff ff             	cmp    $0xffffffff,%edi
    351a:	0f 84 bb 00 00 00    	je     35db <sbrktest+0x2fb>
  if(sbrk(0) > oldbrk)
    3520:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    3527:	e8 ee 09 00 00       	call   3f1a <sbrk>
    352c:	39 45 a4             	cmp    %eax,-0x5c(%ebp)
    352f:	73 19                	jae    354a <sbrktest+0x26a>
    sbrk(-(sbrk(0) - oldbrk));
    3531:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    3538:	e8 dd 09 00 00       	call   3f1a <sbrk>
    353d:	8b 7d a4             	mov    -0x5c(%ebp),%edi
    3540:	29 c7                	sub    %eax,%edi
    3542:	89 3c 24             	mov    %edi,(%esp)
    3545:	e8 d0 09 00 00       	call   3f1a <sbrk>
  printf(stdout, "sbrk test OK\n");
    354a:	a1 c4 63 00 00       	mov    0x63c4,%eax
    354f:	c7 44 24 04 e0 51 00 	movl   $0x51e0,0x4(%esp)
    3556:	00 
    3557:	89 04 24             	mov    %eax,(%esp)
    355a:	e8 81 0a 00 00       	call   3fe0 <printf>
}
    355f:	83 c4 6c             	add    $0x6c,%esp
    3562:	5b                   	pop    %ebx
    3563:	5e                   	pop    %esi
    3564:	5f                   	pop    %edi
    3565:	5d                   	pop    %ebp
    3566:	c3                   	ret    
      sbrk(BIG - (uint)sbrk(0));
    3567:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    356e:	e8 a7 09 00 00       	call   3f1a <sbrk>
    3573:	ba 00 00 40 06       	mov    $0x6400000,%edx
    3578:	29 c2                	sub    %eax,%edx
    357a:	89 14 24             	mov    %edx,(%esp)
    357d:	e8 98 09 00 00       	call   3f1a <sbrk>
      write(fds[1], "x", 1);
    3582:	8b 45 bc             	mov    -0x44(%ebp),%eax
    3585:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
    358c:	00 
    358d:	c7 44 24 04 a1 4c 00 	movl   $0x4ca1,0x4(%esp)
    3594:	00 
    3595:	89 04 24             	mov    %eax,(%esp)
    3598:	e8 15 09 00 00       	call   3eb2 <write>
    359d:	8d 76 00             	lea    0x0(%esi),%esi
      for(;;) sleep(1000);
    35a0:	c7 04 24 e8 03 00 00 	movl   $0x3e8,(%esp)
    35a7:	e8 76 09 00 00       	call   3f22 <sleep>
    35ac:	eb f2                	jmp    35a0 <sbrktest+0x2c0>
      printf(stdout, "sbrk test failed %d %x %x\n", i, a, b);
    35ae:	89 44 24 10          	mov    %eax,0x10(%esp)
    35b2:	a1 c4 63 00 00       	mov    0x63c4,%eax
    35b7:	89 5c 24 0c          	mov    %ebx,0xc(%esp)
    35bb:	89 74 24 08          	mov    %esi,0x8(%esp)
    35bf:	c7 44 24 04 43 51 00 	movl   $0x5143,0x4(%esp)
    35c6:	00 
    35c7:	89 04 24             	mov    %eax,(%esp)
    35ca:	e8 11 0a 00 00       	call   3fe0 <printf>
      exit(0);
    35cf:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    35d6:	e8 b7 08 00 00       	call   3e92 <exit>
    printf(stdout, "failed sbrk leaked memory\n");
    35db:	c7 44 24 04 c5 51 00 	movl   $0x51c5,0x4(%esp)
    35e2:	00 
    printf(stdout, "sbrk test failed post-fork\n");
    35e3:	a1 c4 63 00 00       	mov    0x63c4,%eax
    35e8:	89 04 24             	mov    %eax,(%esp)
    35eb:	e8 f0 09 00 00       	call   3fe0 <printf>
    exit(0);
    35f0:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    35f7:	e8 96 08 00 00       	call   3e92 <exit>
    printf(1, "pipe() failed\n");
    35fc:	c7 44 24 04 81 46 00 	movl   $0x4681,0x4(%esp)
    3603:	00 
    3604:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    360b:	e8 d0 09 00 00       	call   3fe0 <printf>
    exit(0);
    3610:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    3617:	e8 76 08 00 00       	call   3e92 <exit>
      printf(stdout, "oops could read %x = %x\n", a, *a);
    361c:	0f be 03             	movsbl (%ebx),%eax
    361f:	89 5c 24 08          	mov    %ebx,0x8(%esp)
    3623:	c7 44 24 04 ac 51 00 	movl   $0x51ac,0x4(%esp)
    362a:	00 
    362b:	89 44 24 0c          	mov    %eax,0xc(%esp)
    362f:	a1 c4 63 00 00       	mov    0x63c4,%eax
    3634:	89 04 24             	mov    %eax,(%esp)
    3637:	e8 a4 09 00 00       	call   3fe0 <printf>
      kill(ppid);
    363c:	89 34 24             	mov    %esi,(%esp)
    363f:	e8 7e 08 00 00       	call   3ec2 <kill>
      exit(0);
    3644:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    364b:	e8 42 08 00 00       	call   3e92 <exit>
      printf(stdout, "fork failed\n");
    3650:	c7 44 24 04 89 52 00 	movl   $0x5289,0x4(%esp)
    3657:	00 
    3658:	eb 89                	jmp    35e3 <sbrktest+0x303>
    printf(stdout, "sbrk downsize failed, a %x c %x\n", a, c);
    365a:	89 44 24 0c          	mov    %eax,0xc(%esp)
    365e:	89 5c 24 08          	mov    %ebx,0x8(%esp)
    3662:	c7 44 24 04 8c 59 00 	movl   $0x598c,0x4(%esp)
    3669:	00 
    366a:	a1 c4 63 00 00       	mov    0x63c4,%eax
    366f:	89 04 24             	mov    %eax,(%esp)
    3672:	e8 69 09 00 00       	call   3fe0 <printf>
    3677:	eb 97                	jmp    3610 <sbrktest+0x330>
    printf(stdout, "sbrk de-allocation didn't really deallocate\n");
    3679:	c7 44 24 04 5c 59 00 	movl   $0x595c,0x4(%esp)
    3680:	00 
    3681:	e9 5d ff ff ff       	jmp    35e3 <sbrktest+0x303>
    printf(stdout, "sbrk re-allocation failed, a %x c %x\n", a, c);
    3686:	89 5c 24 0c          	mov    %ebx,0xc(%esp)
    368a:	89 74 24 08          	mov    %esi,0x8(%esp)
    368e:	c7 44 24 04 34 59 00 	movl   $0x5934,0x4(%esp)
    3695:	00 
    3696:	eb d2                	jmp    366a <sbrktest+0x38a>
    printf(stdout, "sbrk test failed to grow big address space; enough phys mem?\n");
    3698:	c7 44 24 04 bc 58 00 	movl   $0x58bc,0x4(%esp)
    369f:	00 
    36a0:	e9 3e ff ff ff       	jmp    35e3 <sbrktest+0x303>
    printf(stdout, "sbrk deallocation produced wrong address, a %x c %x\n", a, c);
    36a5:	89 44 24 0c          	mov    %eax,0xc(%esp)
    36a9:	89 5c 24 08          	mov    %ebx,0x8(%esp)
    36ad:	c7 44 24 04 fc 58 00 	movl   $0x58fc,0x4(%esp)
    36b4:	00 
    36b5:	eb b3                	jmp    366a <sbrktest+0x38a>
    printf(stdout, "sbrk could not deallocate\n");
    36b7:	c7 44 24 04 91 51 00 	movl   $0x5191,0x4(%esp)
    36be:	00 
    36bf:	e9 1f ff ff ff       	jmp    35e3 <sbrktest+0x303>
    printf(stdout, "sbrk test failed post-fork\n");
    36c4:	c7 44 24 04 75 51 00 	movl   $0x5175,0x4(%esp)
    36cb:	00 
    36cc:	e9 12 ff ff ff       	jmp    35e3 <sbrktest+0x303>
    printf(stdout, "sbrk test fork failed\n");
    36d1:	c7 44 24 04 5e 51 00 	movl   $0x515e,0x4(%esp)
    36d8:	00 
    36d9:	e9 05 ff ff ff       	jmp    35e3 <sbrktest+0x303>
    36de:	66 90                	xchg   %ax,%ax

000036e0 <validateint>:
{
    36e0:	55                   	push   %ebp
    36e1:	89 e5                	mov    %esp,%ebp
}
    36e3:	5d                   	pop    %ebp
    36e4:	c3                   	ret    
    36e5:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    36e9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

000036f0 <validatetest>:
{
    36f0:	55                   	push   %ebp
    36f1:	89 e5                	mov    %esp,%ebp
    36f3:	56                   	push   %esi
    36f4:	53                   	push   %ebx
  for(p = 0; p <= (uint)hi; p += 4096){
    36f5:	31 db                	xor    %ebx,%ebx
{
    36f7:	83 ec 10             	sub    $0x10,%esp
  printf(stdout, "validate test\n");
    36fa:	a1 c4 63 00 00       	mov    0x63c4,%eax
    36ff:	c7 44 24 04 ee 51 00 	movl   $0x51ee,0x4(%esp)
    3706:	00 
    3707:	89 04 24             	mov    %eax,(%esp)
    370a:	e8 d1 08 00 00       	call   3fe0 <printf>
    370f:	90                   	nop
    if((pid = fork()) == 0){
    3710:	e8 75 07 00 00       	call   3e8a <fork>
      exit(0);
    3715:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    if((pid = fork()) == 0){
    371c:	85 c0                	test   %eax,%eax
    371e:	89 c6                	mov    %eax,%esi
    3720:	74 79                	je     379b <validatetest+0xab>
    sleep(0);
    3722:	e8 fb 07 00 00       	call   3f22 <sleep>
    sleep(0);
    3727:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    372e:	e8 ef 07 00 00       	call   3f22 <sleep>
    kill(pid);
    3733:	89 34 24             	mov    %esi,(%esp)
    3736:	e8 87 07 00 00       	call   3ec2 <kill>
    wait();
    373b:	e8 5a 07 00 00       	call   3e9a <wait>
    if(link("nosuchfile", (char*)p) != -1){
    3740:	89 5c 24 04          	mov    %ebx,0x4(%esp)
    3744:	c7 04 24 fd 51 00 00 	movl   $0x51fd,(%esp)
    374b:	e8 a2 07 00 00       	call   3ef2 <link>
    3750:	83 f8 ff             	cmp    $0xffffffff,%eax
    3753:	75 2a                	jne    377f <validatetest+0x8f>
  for(p = 0; p <= (uint)hi; p += 4096){
    3755:	81 c3 00 10 00 00    	add    $0x1000,%ebx
    375b:	81 fb 00 40 11 00    	cmp    $0x114000,%ebx
    3761:	75 ad                	jne    3710 <validatetest+0x20>
  printf(stdout, "validate ok\n");
    3763:	a1 c4 63 00 00       	mov    0x63c4,%eax
    3768:	c7 44 24 04 21 52 00 	movl   $0x5221,0x4(%esp)
    376f:	00 
    3770:	89 04 24             	mov    %eax,(%esp)
    3773:	e8 68 08 00 00       	call   3fe0 <printf>
}
    3778:	83 c4 10             	add    $0x10,%esp
    377b:	5b                   	pop    %ebx
    377c:	5e                   	pop    %esi
    377d:	5d                   	pop    %ebp
    377e:	c3                   	ret    
      printf(stdout, "link should not succeed\n");
    377f:	a1 c4 63 00 00       	mov    0x63c4,%eax
    3784:	c7 44 24 04 08 52 00 	movl   $0x5208,0x4(%esp)
    378b:	00 
    378c:	89 04 24             	mov    %eax,(%esp)
    378f:	e8 4c 08 00 00       	call   3fe0 <printf>
      exit(0);
    3794:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    379b:	e8 f2 06 00 00       	call   3e92 <exit>

000037a0 <bsstest>:
{
    37a0:	55                   	push   %ebp
    37a1:	89 e5                	mov    %esp,%ebp
    37a3:	83 ec 18             	sub    $0x18,%esp
  printf(stdout, "bss test\n");
    37a6:	a1 c4 63 00 00       	mov    0x63c4,%eax
    37ab:	c7 44 24 04 2e 52 00 	movl   $0x522e,0x4(%esp)
    37b2:	00 
    37b3:	89 04 24             	mov    %eax,(%esp)
    37b6:	e8 25 08 00 00       	call   3fe0 <printf>
    if(uninit[i] != '\0'){
    37bb:	80 3d 80 64 00 00 00 	cmpb   $0x0,0x6480
    37c2:	75 36                	jne    37fa <bsstest+0x5a>
  for(i = 0; i < sizeof(uninit); i++){
    37c4:	b8 01 00 00 00       	mov    $0x1,%eax
    37c9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
    if(uninit[i] != '\0'){
    37d0:	80 b8 80 64 00 00 00 	cmpb   $0x0,0x6480(%eax)
    37d7:	75 21                	jne    37fa <bsstest+0x5a>
  for(i = 0; i < sizeof(uninit); i++){
    37d9:	83 c0 01             	add    $0x1,%eax
    37dc:	3d 10 27 00 00       	cmp    $0x2710,%eax
    37e1:	75 ed                	jne    37d0 <bsstest+0x30>
  printf(stdout, "bss test ok\n");
    37e3:	a1 c4 63 00 00       	mov    0x63c4,%eax
    37e8:	c7 44 24 04 49 52 00 	movl   $0x5249,0x4(%esp)
    37ef:	00 
    37f0:	89 04 24             	mov    %eax,(%esp)
    37f3:	e8 e8 07 00 00       	call   3fe0 <printf>
}
    37f8:	c9                   	leave  
    37f9:	c3                   	ret    
      printf(stdout, "bss test failed\n");
    37fa:	a1 c4 63 00 00       	mov    0x63c4,%eax
    37ff:	c7 44 24 04 38 52 00 	movl   $0x5238,0x4(%esp)
    3806:	00 
    3807:	89 04 24             	mov    %eax,(%esp)
    380a:	e8 d1 07 00 00       	call   3fe0 <printf>
      exit(0);
    380f:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    3816:	e8 77 06 00 00       	call   3e92 <exit>
    381b:	90                   	nop
    381c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00003820 <bigargtest>:
{
    3820:	55                   	push   %ebp
    3821:	89 e5                	mov    %esp,%ebp
    3823:	83 ec 18             	sub    $0x18,%esp
  unlink("bigarg-ok");
    3826:	c7 04 24 56 52 00 00 	movl   $0x5256,(%esp)
    382d:	e8 b0 06 00 00       	call   3ee2 <unlink>
  pid = fork();
    3832:	e8 53 06 00 00       	call   3e8a <fork>
  if(pid == 0){
    3837:	85 c0                	test   %eax,%eax
    3839:	74 45                	je     3880 <bigargtest+0x60>
    383b:	90                   	nop
    383c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  } else if(pid < 0){
    3840:	0f 88 de 00 00 00    	js     3924 <bigargtest+0x104>
  wait();
    3846:	e8 4f 06 00 00       	call   3e9a <wait>
  fd = open("bigarg-ok", 0);
    384b:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
    3852:	00 
    3853:	c7 04 24 56 52 00 00 	movl   $0x5256,(%esp)
    385a:	e8 73 06 00 00       	call   3ed2 <open>
  if(fd < 0){
    385f:	85 c0                	test   %eax,%eax
    3861:	0f 88 9c 00 00 00    	js     3903 <bigargtest+0xe3>
  close(fd);
    3867:	89 04 24             	mov    %eax,(%esp)
    386a:	e8 4b 06 00 00       	call   3eba <close>
  unlink("bigarg-ok");
    386f:	c7 04 24 56 52 00 00 	movl   $0x5256,(%esp)
    3876:	e8 67 06 00 00       	call   3ee2 <unlink>
}
    387b:	c9                   	leave  
    387c:	c3                   	ret    
    387d:	8d 76 00             	lea    0x0(%esi),%esi
      args[i] = "bigargs test: failed\n                                                                                                                                                                                                       ";
    3880:	c7 04 85 e0 63 00 00 	movl   $0x59b0,0x63e0(,%eax,4)
    3887:	b0 59 00 00 
    for(i = 0; i < MAXARG-1; i++)
    388b:	83 c0 01             	add    $0x1,%eax
    388e:	83 f8 1f             	cmp    $0x1f,%eax
    3891:	75 ed                	jne    3880 <bigargtest+0x60>
    printf(stdout, "bigarg test\n");
    3893:	a1 c4 63 00 00       	mov    0x63c4,%eax
    3898:	c7 44 24 04 60 52 00 	movl   $0x5260,0x4(%esp)
    389f:	00 
    args[MAXARG-1] = 0;
    38a0:	c7 05 5c 64 00 00 00 	movl   $0x0,0x645c
    38a7:	00 00 00 
    printf(stdout, "bigarg test\n");
    38aa:	89 04 24             	mov    %eax,(%esp)
    38ad:	e8 2e 07 00 00       	call   3fe0 <printf>
    exec("echo", args);
    38b2:	c7 44 24 04 e0 63 00 	movl   $0x63e0,0x4(%esp)
    38b9:	00 
    38ba:	c7 04 24 2d 44 00 00 	movl   $0x442d,(%esp)
    38c1:	e8 04 06 00 00       	call   3eca <exec>
    printf(stdout, "bigarg test ok\n");
    38c6:	a1 c4 63 00 00       	mov    0x63c4,%eax
    38cb:	c7 44 24 04 6d 52 00 	movl   $0x526d,0x4(%esp)
    38d2:	00 
    38d3:	89 04 24             	mov    %eax,(%esp)
    38d6:	e8 05 07 00 00       	call   3fe0 <printf>
    fd = open("bigarg-ok", O_CREATE);
    38db:	c7 44 24 04 00 02 00 	movl   $0x200,0x4(%esp)
    38e2:	00 
    38e3:	c7 04 24 56 52 00 00 	movl   $0x5256,(%esp)
    38ea:	e8 e3 05 00 00       	call   3ed2 <open>
    close(fd);
    38ef:	89 04 24             	mov    %eax,(%esp)
    38f2:	e8 c3 05 00 00       	call   3eba <close>
    exit(0);
    38f7:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    38fe:	e8 8f 05 00 00       	call   3e92 <exit>
    printf(stdout, "bigarg test failed!\n");
    3903:	c7 44 24 04 96 52 00 	movl   $0x5296,0x4(%esp)
    390a:	00 
    390b:	a1 c4 63 00 00       	mov    0x63c4,%eax
    3910:	89 04 24             	mov    %eax,(%esp)
    3913:	e8 c8 06 00 00       	call   3fe0 <printf>
    exit(0);
    3918:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    391f:	e8 6e 05 00 00       	call   3e92 <exit>
    printf(stdout, "bigargtest: fork failed\n");
    3924:	c7 44 24 04 7d 52 00 	movl   $0x527d,0x4(%esp)
    392b:	00 
    392c:	eb dd                	jmp    390b <bigargtest+0xeb>
    392e:	66 90                	xchg   %ax,%ax

00003930 <fsfull>:
{
    3930:	55                   	push   %ebp
    3931:	89 e5                	mov    %esp,%ebp
    3933:	57                   	push   %edi
    3934:	56                   	push   %esi
    3935:	53                   	push   %ebx
  for(nfiles = 0; ; nfiles++){
    3936:	31 db                	xor    %ebx,%ebx
{
    3938:	83 ec 5c             	sub    $0x5c,%esp
  printf(1, "fsfull test\n");
    393b:	c7 44 24 04 ab 52 00 	movl   $0x52ab,0x4(%esp)
    3942:	00 
    3943:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    394a:	e8 91 06 00 00       	call   3fe0 <printf>
    394f:	90                   	nop
    name[1] = '0' + nfiles / 1000;
    3950:	b8 d3 4d 62 10       	mov    $0x10624dd3,%eax
    3955:	89 d9                	mov    %ebx,%ecx
    3957:	f7 eb                	imul   %ebx
    name[2] = '0' + (nfiles % 1000) / 100;
    3959:	89 de                	mov    %ebx,%esi
    name[1] = '0' + nfiles / 1000;
    395b:	c1 f9 1f             	sar    $0x1f,%ecx
    name[3] = '0' + (nfiles % 100) / 10;
    395e:	89 df                	mov    %ebx,%edi
    printf(1, "writing %s\n", name);
    3960:	c7 44 24 04 b8 52 00 	movl   $0x52b8,0x4(%esp)
    3967:	00 
    3968:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    name[1] = '0' + nfiles / 1000;
    396f:	c1 fa 06             	sar    $0x6,%edx
    3972:	29 ca                	sub    %ecx,%edx
    3974:	8d 42 30             	lea    0x30(%edx),%eax
    name[2] = '0' + (nfiles % 1000) / 100;
    3977:	69 d2 e8 03 00 00    	imul   $0x3e8,%edx,%edx
    name[1] = '0' + nfiles / 1000;
    397d:	88 45 a9             	mov    %al,-0x57(%ebp)
    name[2] = '0' + (nfiles % 1000) / 100;
    3980:	b8 1f 85 eb 51       	mov    $0x51eb851f,%eax
    name[0] = 'f';
    3985:	c6 45 a8 66          	movb   $0x66,-0x58(%ebp)
    name[5] = '\0';
    3989:	c6 45 ad 00          	movb   $0x0,-0x53(%ebp)
    name[2] = '0' + (nfiles % 1000) / 100;
    398d:	29 d6                	sub    %edx,%esi
    398f:	f7 ee                	imul   %esi
    name[3] = '0' + (nfiles % 100) / 10;
    3991:	b8 1f 85 eb 51       	mov    $0x51eb851f,%eax
    name[2] = '0' + (nfiles % 1000) / 100;
    3996:	c1 fe 1f             	sar    $0x1f,%esi
    3999:	c1 fa 05             	sar    $0x5,%edx
    399c:	29 f2                	sub    %esi,%edx
    name[3] = '0' + (nfiles % 100) / 10;
    399e:	be 67 66 66 66       	mov    $0x66666667,%esi
    name[2] = '0' + (nfiles % 1000) / 100;
    39a3:	83 c2 30             	add    $0x30,%edx
    39a6:	88 55 aa             	mov    %dl,-0x56(%ebp)
    name[3] = '0' + (nfiles % 100) / 10;
    39a9:	f7 eb                	imul   %ebx
    39ab:	c1 fa 05             	sar    $0x5,%edx
    39ae:	29 ca                	sub    %ecx,%edx
    39b0:	6b d2 64             	imul   $0x64,%edx,%edx
    39b3:	29 d7                	sub    %edx,%edi
    39b5:	89 f8                	mov    %edi,%eax
    39b7:	f7 ee                	imul   %esi
    name[4] = '0' + (nfiles % 10);
    39b9:	89 d8                	mov    %ebx,%eax
    name[3] = '0' + (nfiles % 100) / 10;
    39bb:	c1 ff 1f             	sar    $0x1f,%edi
    39be:	c1 fa 02             	sar    $0x2,%edx
    39c1:	29 fa                	sub    %edi,%edx
    39c3:	83 c2 30             	add    $0x30,%edx
    39c6:	88 55 ab             	mov    %dl,-0x55(%ebp)
    name[4] = '0' + (nfiles % 10);
    39c9:	f7 ee                	imul   %esi
    39cb:	c1 fa 02             	sar    $0x2,%edx
    39ce:	29 ca                	sub    %ecx,%edx
    39d0:	89 d9                	mov    %ebx,%ecx
    39d2:	8d 04 92             	lea    (%edx,%edx,4),%eax
    39d5:	01 c0                	add    %eax,%eax
    39d7:	29 c1                	sub    %eax,%ecx
    39d9:	89 c8                	mov    %ecx,%eax
    39db:	83 c0 30             	add    $0x30,%eax
    39de:	88 45 ac             	mov    %al,-0x54(%ebp)
    printf(1, "writing %s\n", name);
    39e1:	8d 45 a8             	lea    -0x58(%ebp),%eax
    39e4:	89 44 24 08          	mov    %eax,0x8(%esp)
    39e8:	e8 f3 05 00 00       	call   3fe0 <printf>
    int fd = open(name, O_CREATE|O_RDWR);
    39ed:	8d 45 a8             	lea    -0x58(%ebp),%eax
    39f0:	c7 44 24 04 02 02 00 	movl   $0x202,0x4(%esp)
    39f7:	00 
    39f8:	89 04 24             	mov    %eax,(%esp)
    39fb:	e8 d2 04 00 00       	call   3ed2 <open>
    if(fd < 0){
    3a00:	85 c0                	test   %eax,%eax
    int fd = open(name, O_CREATE|O_RDWR);
    3a02:	89 c7                	mov    %eax,%edi
    if(fd < 0){
    3a04:	78 57                	js     3a5d <fsfull+0x12d>
    3a06:	31 f6                	xor    %esi,%esi
    3a08:	eb 08                	jmp    3a12 <fsfull+0xe2>
    3a0a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
      total += cc;
    3a10:	01 c6                	add    %eax,%esi
      int cc = write(fd, buf, 512);
    3a12:	c7 44 24 08 00 02 00 	movl   $0x200,0x8(%esp)
    3a19:	00 
    3a1a:	c7 44 24 04 a0 8b 00 	movl   $0x8ba0,0x4(%esp)
    3a21:	00 
    3a22:	89 3c 24             	mov    %edi,(%esp)
    3a25:	e8 88 04 00 00       	call   3eb2 <write>
      if(cc < 512)
    3a2a:	3d ff 01 00 00       	cmp    $0x1ff,%eax
    3a2f:	7f df                	jg     3a10 <fsfull+0xe0>
    printf(1, "wrote %d bytes\n", total);
    3a31:	89 74 24 08          	mov    %esi,0x8(%esp)
    3a35:	c7 44 24 04 d4 52 00 	movl   $0x52d4,0x4(%esp)
    3a3c:	00 
    3a3d:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    3a44:	e8 97 05 00 00       	call   3fe0 <printf>
    close(fd);
    3a49:	89 3c 24             	mov    %edi,(%esp)
    3a4c:	e8 69 04 00 00       	call   3eba <close>
    if(total == 0)
    3a51:	85 f6                	test   %esi,%esi
    3a53:	74 23                	je     3a78 <fsfull+0x148>
  for(nfiles = 0; ; nfiles++){
    3a55:	83 c3 01             	add    $0x1,%ebx
  }
    3a58:	e9 f3 fe ff ff       	jmp    3950 <fsfull+0x20>
      printf(1, "open %s failed\n", name);
    3a5d:	8d 45 a8             	lea    -0x58(%ebp),%eax
    3a60:	89 44 24 08          	mov    %eax,0x8(%esp)
    3a64:	c7 44 24 04 c4 52 00 	movl   $0x52c4,0x4(%esp)
    3a6b:	00 
    3a6c:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    3a73:	e8 68 05 00 00       	call   3fe0 <printf>
    name[1] = '0' + nfiles / 1000;
    3a78:	b8 d3 4d 62 10       	mov    $0x10624dd3,%eax
    3a7d:	89 d9                	mov    %ebx,%ecx
    3a7f:	f7 eb                	imul   %ebx
    name[2] = '0' + (nfiles % 1000) / 100;
    3a81:	89 de                	mov    %ebx,%esi
    name[1] = '0' + nfiles / 1000;
    3a83:	c1 f9 1f             	sar    $0x1f,%ecx
    name[3] = '0' + (nfiles % 100) / 10;
    3a86:	89 df                	mov    %ebx,%edi
    name[0] = 'f';
    3a88:	c6 45 a8 66          	movb   $0x66,-0x58(%ebp)
    name[5] = '\0';
    3a8c:	c6 45 ad 00          	movb   $0x0,-0x53(%ebp)
    name[1] = '0' + nfiles / 1000;
    3a90:	c1 fa 06             	sar    $0x6,%edx
    3a93:	29 ca                	sub    %ecx,%edx
    3a95:	8d 42 30             	lea    0x30(%edx),%eax
    name[2] = '0' + (nfiles % 1000) / 100;
    3a98:	69 d2 e8 03 00 00    	imul   $0x3e8,%edx,%edx
    name[1] = '0' + nfiles / 1000;
    3a9e:	88 45 a9             	mov    %al,-0x57(%ebp)
    name[2] = '0' + (nfiles % 1000) / 100;
    3aa1:	b8 1f 85 eb 51       	mov    $0x51eb851f,%eax
    3aa6:	29 d6                	sub    %edx,%esi
    3aa8:	f7 ee                	imul   %esi
    name[3] = '0' + (nfiles % 100) / 10;
    3aaa:	b8 1f 85 eb 51       	mov    $0x51eb851f,%eax
    name[2] = '0' + (nfiles % 1000) / 100;
    3aaf:	c1 fe 1f             	sar    $0x1f,%esi
    3ab2:	c1 fa 05             	sar    $0x5,%edx
    3ab5:	29 f2                	sub    %esi,%edx
    name[3] = '0' + (nfiles % 100) / 10;
    3ab7:	be 67 66 66 66       	mov    $0x66666667,%esi
    name[2] = '0' + (nfiles % 1000) / 100;
    3abc:	83 c2 30             	add    $0x30,%edx
    3abf:	88 55 aa             	mov    %dl,-0x56(%ebp)
    name[3] = '0' + (nfiles % 100) / 10;
    3ac2:	f7 eb                	imul   %ebx
    3ac4:	c1 fa 05             	sar    $0x5,%edx
    3ac7:	29 ca                	sub    %ecx,%edx
    3ac9:	6b d2 64             	imul   $0x64,%edx,%edx
    3acc:	29 d7                	sub    %edx,%edi
    3ace:	89 f8                	mov    %edi,%eax
    3ad0:	f7 ee                	imul   %esi
    name[4] = '0' + (nfiles % 10);
    3ad2:	89 d8                	mov    %ebx,%eax
    name[3] = '0' + (nfiles % 100) / 10;
    3ad4:	c1 ff 1f             	sar    $0x1f,%edi
    3ad7:	c1 fa 02             	sar    $0x2,%edx
    3ada:	29 fa                	sub    %edi,%edx
    3adc:	83 c2 30             	add    $0x30,%edx
    3adf:	88 55 ab             	mov    %dl,-0x55(%ebp)
    name[4] = '0' + (nfiles % 10);
    3ae2:	f7 ee                	imul   %esi
    3ae4:	c1 fa 02             	sar    $0x2,%edx
    3ae7:	29 ca                	sub    %ecx,%edx
    3ae9:	89 d9                	mov    %ebx,%ecx
    3aeb:	8d 04 92             	lea    (%edx,%edx,4),%eax
    nfiles--;
    3aee:	83 eb 01             	sub    $0x1,%ebx
    name[4] = '0' + (nfiles % 10);
    3af1:	01 c0                	add    %eax,%eax
    3af3:	29 c1                	sub    %eax,%ecx
    3af5:	89 c8                	mov    %ecx,%eax
    3af7:	83 c0 30             	add    $0x30,%eax
    3afa:	88 45 ac             	mov    %al,-0x54(%ebp)
    unlink(name);
    3afd:	8d 45 a8             	lea    -0x58(%ebp),%eax
    3b00:	89 04 24             	mov    %eax,(%esp)
    3b03:	e8 da 03 00 00       	call   3ee2 <unlink>
  while(nfiles >= 0){
    3b08:	83 fb ff             	cmp    $0xffffffff,%ebx
    3b0b:	0f 85 67 ff ff ff    	jne    3a78 <fsfull+0x148>
  printf(1, "fsfull test finished\n");
    3b11:	c7 44 24 04 e4 52 00 	movl   $0x52e4,0x4(%esp)
    3b18:	00 
    3b19:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    3b20:	e8 bb 04 00 00       	call   3fe0 <printf>
}
    3b25:	83 c4 5c             	add    $0x5c,%esp
    3b28:	5b                   	pop    %ebx
    3b29:	5e                   	pop    %esi
    3b2a:	5f                   	pop    %edi
    3b2b:	5d                   	pop    %ebp
    3b2c:	c3                   	ret    
    3b2d:	8d 76 00             	lea    0x0(%esi),%esi

00003b30 <uio>:
{
    3b30:	55                   	push   %ebp
    3b31:	89 e5                	mov    %esp,%ebp
    3b33:	83 ec 18             	sub    $0x18,%esp
  printf(1, "uio test\n");
    3b36:	c7 44 24 04 fa 52 00 	movl   $0x52fa,0x4(%esp)
    3b3d:	00 
    3b3e:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    3b45:	e8 96 04 00 00       	call   3fe0 <printf>
  pid = fork();
    3b4a:	e8 3b 03 00 00       	call   3e8a <fork>
  if(pid == 0){
    3b4f:	85 c0                	test   %eax,%eax
    3b51:	74 1d                	je     3b70 <uio+0x40>
  } else if(pid < 0){
    3b53:	78 49                	js     3b9e <uio+0x6e>
  wait();
    3b55:	e8 40 03 00 00       	call   3e9a <wait>
  printf(1, "uio test done\n");
    3b5a:	c7 44 24 04 04 53 00 	movl   $0x5304,0x4(%esp)
    3b61:	00 
    3b62:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    3b69:	e8 72 04 00 00       	call   3fe0 <printf>
}
    3b6e:	c9                   	leave  
    3b6f:	c3                   	ret    
    asm volatile("outb %0,%1"::"a"(val), "d" (port));
    3b70:	ba 70 00 00 00       	mov    $0x70,%edx
    3b75:	b8 09 00 00 00       	mov    $0x9,%eax
    3b7a:	ee                   	out    %al,(%dx)
    asm volatile("inb %1,%0" : "=a" (val) : "d" (port));
    3b7b:	b2 71                	mov    $0x71,%dl
    3b7d:	ec                   	in     (%dx),%al
    printf(1, "uio: uio succeeded; test FAILED\n");
    3b7e:	c7 44 24 04 90 5a 00 	movl   $0x5a90,0x4(%esp)
    3b85:	00 
    3b86:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    3b8d:	e8 4e 04 00 00       	call   3fe0 <printf>
    exit(0);
    3b92:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    3b99:	e8 f4 02 00 00       	call   3e92 <exit>
    printf (1, "fork failed\n");
    3b9e:	c7 44 24 04 89 52 00 	movl   $0x5289,0x4(%esp)
    3ba5:	00 
    3ba6:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    3bad:	e8 2e 04 00 00       	call   3fe0 <printf>
    exit(0);
    3bb2:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    3bb9:	e8 d4 02 00 00       	call   3e92 <exit>
    3bbe:	66 90                	xchg   %ax,%ax

00003bc0 <argptest>:
{
    3bc0:	55                   	push   %ebp
    3bc1:	89 e5                	mov    %esp,%ebp
    3bc3:	53                   	push   %ebx
    3bc4:	83 ec 14             	sub    $0x14,%esp
  fd = open("init", O_RDONLY);
    3bc7:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
    3bce:	00 
    3bcf:	c7 04 24 13 53 00 00 	movl   $0x5313,(%esp)
    3bd6:	e8 f7 02 00 00       	call   3ed2 <open>
  if (fd < 0) {
    3bdb:	85 c0                	test   %eax,%eax
  fd = open("init", O_RDONLY);
    3bdd:	89 c3                	mov    %eax,%ebx
  if (fd < 0) {
    3bdf:	78 45                	js     3c26 <argptest+0x66>
  read(fd, sbrk(0) - 1, -1);
    3be1:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    3be8:	e8 2d 03 00 00       	call   3f1a <sbrk>
    3bed:	89 1c 24             	mov    %ebx,(%esp)
    3bf0:	c7 44 24 08 ff ff ff 	movl   $0xffffffff,0x8(%esp)
    3bf7:	ff 
    3bf8:	83 e8 01             	sub    $0x1,%eax
    3bfb:	89 44 24 04          	mov    %eax,0x4(%esp)
    3bff:	e8 a6 02 00 00       	call   3eaa <read>
  close(fd);
    3c04:	89 1c 24             	mov    %ebx,(%esp)
    3c07:	e8 ae 02 00 00       	call   3eba <close>
  printf(1, "arg test passed\n");
    3c0c:	c7 44 24 04 25 53 00 	movl   $0x5325,0x4(%esp)
    3c13:	00 
    3c14:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    3c1b:	e8 c0 03 00 00       	call   3fe0 <printf>
}
    3c20:	83 c4 14             	add    $0x14,%esp
    3c23:	5b                   	pop    %ebx
    3c24:	5d                   	pop    %ebp
    3c25:	c3                   	ret    
    printf(2, "open failed\n");
    3c26:	c7 44 24 04 18 53 00 	movl   $0x5318,0x4(%esp)
    3c2d:	00 
    3c2e:	c7 04 24 02 00 00 00 	movl   $0x2,(%esp)
    3c35:	e8 a6 03 00 00       	call   3fe0 <printf>
    exit(0);
    3c3a:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    3c41:	e8 4c 02 00 00       	call   3e92 <exit>
    3c46:	8d 76 00             	lea    0x0(%esi),%esi
    3c49:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00003c50 <rand>:
  randstate = randstate * 1664525 + 1013904223;
    3c50:	69 05 c0 63 00 00 0d 	imul   $0x19660d,0x63c0,%eax
    3c57:	66 19 00 
{
    3c5a:	55                   	push   %ebp
    3c5b:	89 e5                	mov    %esp,%ebp
}
    3c5d:	5d                   	pop    %ebp
  randstate = randstate * 1664525 + 1013904223;
    3c5e:	05 5f f3 6e 3c       	add    $0x3c6ef35f,%eax
    3c63:	a3 c0 63 00 00       	mov    %eax,0x63c0
}
    3c68:	c3                   	ret    
    3c69:	66 90                	xchg   %ax,%ax
    3c6b:	66 90                	xchg   %ax,%ax
    3c6d:	66 90                	xchg   %ax,%ax
    3c6f:	90                   	nop

00003c70 <strcpy>:
#include "user.h"
#include "x86.h"

char*
strcpy(char *s, const char *t)
{
    3c70:	55                   	push   %ebp
    3c71:	89 e5                	mov    %esp,%ebp
    3c73:	8b 45 08             	mov    0x8(%ebp),%eax
    3c76:	8b 4d 0c             	mov    0xc(%ebp),%ecx
    3c79:	53                   	push   %ebx
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    3c7a:	89 c2                	mov    %eax,%edx
    3c7c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    3c80:	83 c1 01             	add    $0x1,%ecx
    3c83:	0f b6 59 ff          	movzbl -0x1(%ecx),%ebx
    3c87:	83 c2 01             	add    $0x1,%edx
    3c8a:	84 db                	test   %bl,%bl
    3c8c:	88 5a ff             	mov    %bl,-0x1(%edx)
    3c8f:	75 ef                	jne    3c80 <strcpy+0x10>
    ;
  return os;
}
    3c91:	5b                   	pop    %ebx
    3c92:	5d                   	pop    %ebp
    3c93:	c3                   	ret    
    3c94:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    3c9a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

00003ca0 <strcmp>:

int
strcmp(const char *p, const char *q)
{
    3ca0:	55                   	push   %ebp
    3ca1:	89 e5                	mov    %esp,%ebp
    3ca3:	8b 55 08             	mov    0x8(%ebp),%edx
    3ca6:	53                   	push   %ebx
    3ca7:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  while(*p && *p == *q)
    3caa:	0f b6 02             	movzbl (%edx),%eax
    3cad:	84 c0                	test   %al,%al
    3caf:	74 2d                	je     3cde <strcmp+0x3e>
    3cb1:	0f b6 19             	movzbl (%ecx),%ebx
    3cb4:	38 d8                	cmp    %bl,%al
    3cb6:	74 0e                	je     3cc6 <strcmp+0x26>
    3cb8:	eb 2b                	jmp    3ce5 <strcmp+0x45>
    3cba:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    3cc0:	38 c8                	cmp    %cl,%al
    3cc2:	75 15                	jne    3cd9 <strcmp+0x39>
    p++, q++;
    3cc4:	89 d9                	mov    %ebx,%ecx
    3cc6:	83 c2 01             	add    $0x1,%edx
  while(*p && *p == *q)
    3cc9:	0f b6 02             	movzbl (%edx),%eax
    p++, q++;
    3ccc:	8d 59 01             	lea    0x1(%ecx),%ebx
  while(*p && *p == *q)
    3ccf:	0f b6 49 01          	movzbl 0x1(%ecx),%ecx
    3cd3:	84 c0                	test   %al,%al
    3cd5:	75 e9                	jne    3cc0 <strcmp+0x20>
    3cd7:	31 c0                	xor    %eax,%eax
  return (uchar)*p - (uchar)*q;
    3cd9:	29 c8                	sub    %ecx,%eax
}
    3cdb:	5b                   	pop    %ebx
    3cdc:	5d                   	pop    %ebp
    3cdd:	c3                   	ret    
    3cde:	0f b6 09             	movzbl (%ecx),%ecx
  while(*p && *p == *q)
    3ce1:	31 c0                	xor    %eax,%eax
    3ce3:	eb f4                	jmp    3cd9 <strcmp+0x39>
    3ce5:	0f b6 cb             	movzbl %bl,%ecx
    3ce8:	eb ef                	jmp    3cd9 <strcmp+0x39>
    3cea:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

00003cf0 <strlen>:

uint
strlen(const char *s)
{
    3cf0:	55                   	push   %ebp
    3cf1:	89 e5                	mov    %esp,%ebp
    3cf3:	8b 4d 08             	mov    0x8(%ebp),%ecx
  int n;

  for(n = 0; s[n]; n++)
    3cf6:	80 39 00             	cmpb   $0x0,(%ecx)
    3cf9:	74 12                	je     3d0d <strlen+0x1d>
    3cfb:	31 d2                	xor    %edx,%edx
    3cfd:	8d 76 00             	lea    0x0(%esi),%esi
    3d00:	83 c2 01             	add    $0x1,%edx
    3d03:	80 3c 11 00          	cmpb   $0x0,(%ecx,%edx,1)
    3d07:	89 d0                	mov    %edx,%eax
    3d09:	75 f5                	jne    3d00 <strlen+0x10>
    ;
  return n;
}
    3d0b:	5d                   	pop    %ebp
    3d0c:	c3                   	ret    
  for(n = 0; s[n]; n++)
    3d0d:	31 c0                	xor    %eax,%eax
}
    3d0f:	5d                   	pop    %ebp
    3d10:	c3                   	ret    
    3d11:	eb 0d                	jmp    3d20 <memset>
    3d13:	90                   	nop
    3d14:	90                   	nop
    3d15:	90                   	nop
    3d16:	90                   	nop
    3d17:	90                   	nop
    3d18:	90                   	nop
    3d19:	90                   	nop
    3d1a:	90                   	nop
    3d1b:	90                   	nop
    3d1c:	90                   	nop
    3d1d:	90                   	nop
    3d1e:	90                   	nop
    3d1f:	90                   	nop

00003d20 <memset>:

void*
memset(void *dst, int c, uint n)
{
    3d20:	55                   	push   %ebp
    3d21:	89 e5                	mov    %esp,%ebp
    3d23:	8b 55 08             	mov    0x8(%ebp),%edx
    3d26:	57                   	push   %edi
}

static inline void
stosb(void *addr, int data, int cnt)
{
  asm volatile("cld; rep stosb" :
    3d27:	8b 4d 10             	mov    0x10(%ebp),%ecx
    3d2a:	8b 45 0c             	mov    0xc(%ebp),%eax
    3d2d:	89 d7                	mov    %edx,%edi
    3d2f:	fc                   	cld    
    3d30:	f3 aa                	rep stos %al,%es:(%edi)
  stosb(dst, c, n);
  return dst;
}
    3d32:	89 d0                	mov    %edx,%eax
    3d34:	5f                   	pop    %edi
    3d35:	5d                   	pop    %ebp
    3d36:	c3                   	ret    
    3d37:	89 f6                	mov    %esi,%esi
    3d39:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00003d40 <strchr>:

char*
strchr(const char *s, char c)
{
    3d40:	55                   	push   %ebp
    3d41:	89 e5                	mov    %esp,%ebp
    3d43:	8b 45 08             	mov    0x8(%ebp),%eax
    3d46:	53                   	push   %ebx
    3d47:	8b 55 0c             	mov    0xc(%ebp),%edx
  for(; *s; s++)
    3d4a:	0f b6 18             	movzbl (%eax),%ebx
    3d4d:	84 db                	test   %bl,%bl
    3d4f:	74 1d                	je     3d6e <strchr+0x2e>
    if(*s == c)
    3d51:	38 d3                	cmp    %dl,%bl
    3d53:	89 d1                	mov    %edx,%ecx
    3d55:	75 0d                	jne    3d64 <strchr+0x24>
    3d57:	eb 17                	jmp    3d70 <strchr+0x30>
    3d59:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
    3d60:	38 ca                	cmp    %cl,%dl
    3d62:	74 0c                	je     3d70 <strchr+0x30>
  for(; *s; s++)
    3d64:	83 c0 01             	add    $0x1,%eax
    3d67:	0f b6 10             	movzbl (%eax),%edx
    3d6a:	84 d2                	test   %dl,%dl
    3d6c:	75 f2                	jne    3d60 <strchr+0x20>
      return (char*)s;
  return 0;
    3d6e:	31 c0                	xor    %eax,%eax
}
    3d70:	5b                   	pop    %ebx
    3d71:	5d                   	pop    %ebp
    3d72:	c3                   	ret    
    3d73:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    3d79:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

00003d80 <gets>:

char*
gets(char *buf, int max)
{
    3d80:	55                   	push   %ebp
    3d81:	89 e5                	mov    %esp,%ebp
    3d83:	57                   	push   %edi
    3d84:	56                   	push   %esi
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    3d85:	31 f6                	xor    %esi,%esi
{
    3d87:	53                   	push   %ebx
    3d88:	83 ec 2c             	sub    $0x2c,%esp
    cc = read(0, &c, 1);
    3d8b:	8d 7d e7             	lea    -0x19(%ebp),%edi
  for(i=0; i+1 < max; ){
    3d8e:	eb 31                	jmp    3dc1 <gets+0x41>
    cc = read(0, &c, 1);
    3d90:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
    3d97:	00 
    3d98:	89 7c 24 04          	mov    %edi,0x4(%esp)
    3d9c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    3da3:	e8 02 01 00 00       	call   3eaa <read>
    if(cc < 1)
    3da8:	85 c0                	test   %eax,%eax
    3daa:	7e 1d                	jle    3dc9 <gets+0x49>
      break;
    buf[i++] = c;
    3dac:	0f b6 45 e7          	movzbl -0x19(%ebp),%eax
  for(i=0; i+1 < max; ){
    3db0:	89 de                	mov    %ebx,%esi
    buf[i++] = c;
    3db2:	8b 55 08             	mov    0x8(%ebp),%edx
    if(c == '\n' || c == '\r')
    3db5:	3c 0d                	cmp    $0xd,%al
    buf[i++] = c;
    3db7:	88 44 1a ff          	mov    %al,-0x1(%edx,%ebx,1)
    if(c == '\n' || c == '\r')
    3dbb:	74 0c                	je     3dc9 <gets+0x49>
    3dbd:	3c 0a                	cmp    $0xa,%al
    3dbf:	74 08                	je     3dc9 <gets+0x49>
  for(i=0; i+1 < max; ){
    3dc1:	8d 5e 01             	lea    0x1(%esi),%ebx
    3dc4:	3b 5d 0c             	cmp    0xc(%ebp),%ebx
    3dc7:	7c c7                	jl     3d90 <gets+0x10>
      break;
  }
  buf[i] = '\0';
    3dc9:	8b 45 08             	mov    0x8(%ebp),%eax
    3dcc:	c6 04 30 00          	movb   $0x0,(%eax,%esi,1)
  return buf;
}
    3dd0:	83 c4 2c             	add    $0x2c,%esp
    3dd3:	5b                   	pop    %ebx
    3dd4:	5e                   	pop    %esi
    3dd5:	5f                   	pop    %edi
    3dd6:	5d                   	pop    %ebp
    3dd7:	c3                   	ret    
    3dd8:	90                   	nop
    3dd9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00003de0 <stat>:

int
stat(const char *n, struct stat *st)
{
    3de0:	55                   	push   %ebp
    3de1:	89 e5                	mov    %esp,%ebp
    3de3:	56                   	push   %esi
    3de4:	53                   	push   %ebx
    3de5:	83 ec 10             	sub    $0x10,%esp
  int fd;
  int r;

  fd = open(n, O_RDONLY);
    3de8:	8b 45 08             	mov    0x8(%ebp),%eax
    3deb:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
    3df2:	00 
    3df3:	89 04 24             	mov    %eax,(%esp)
    3df6:	e8 d7 00 00 00       	call   3ed2 <open>
  if(fd < 0)
    3dfb:	85 c0                	test   %eax,%eax
  fd = open(n, O_RDONLY);
    3dfd:	89 c3                	mov    %eax,%ebx
  if(fd < 0)
    3dff:	78 27                	js     3e28 <stat+0x48>
    return -1;
  r = fstat(fd, st);
    3e01:	8b 45 0c             	mov    0xc(%ebp),%eax
    3e04:	89 1c 24             	mov    %ebx,(%esp)
    3e07:	89 44 24 04          	mov    %eax,0x4(%esp)
    3e0b:	e8 da 00 00 00       	call   3eea <fstat>
  close(fd);
    3e10:	89 1c 24             	mov    %ebx,(%esp)
  r = fstat(fd, st);
    3e13:	89 c6                	mov    %eax,%esi
  close(fd);
    3e15:	e8 a0 00 00 00       	call   3eba <close>
  return r;
    3e1a:	89 f0                	mov    %esi,%eax
}
    3e1c:	83 c4 10             	add    $0x10,%esp
    3e1f:	5b                   	pop    %ebx
    3e20:	5e                   	pop    %esi
    3e21:	5d                   	pop    %ebp
    3e22:	c3                   	ret    
    3e23:	90                   	nop
    3e24:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    return -1;
    3e28:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
    3e2d:	eb ed                	jmp    3e1c <stat+0x3c>
    3e2f:	90                   	nop

00003e30 <atoi>:

int
atoi(const char *s)
{
    3e30:	55                   	push   %ebp
    3e31:	89 e5                	mov    %esp,%ebp
    3e33:	8b 4d 08             	mov    0x8(%ebp),%ecx
    3e36:	53                   	push   %ebx
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    3e37:	0f be 11             	movsbl (%ecx),%edx
    3e3a:	8d 42 d0             	lea    -0x30(%edx),%eax
    3e3d:	3c 09                	cmp    $0x9,%al
  n = 0;
    3e3f:	b8 00 00 00 00       	mov    $0x0,%eax
  while('0' <= *s && *s <= '9')
    3e44:	77 17                	ja     3e5d <atoi+0x2d>
    3e46:	66 90                	xchg   %ax,%ax
    n = n*10 + *s++ - '0';
    3e48:	83 c1 01             	add    $0x1,%ecx
    3e4b:	8d 04 80             	lea    (%eax,%eax,4),%eax
    3e4e:	8d 44 42 d0          	lea    -0x30(%edx,%eax,2),%eax
  while('0' <= *s && *s <= '9')
    3e52:	0f be 11             	movsbl (%ecx),%edx
    3e55:	8d 5a d0             	lea    -0x30(%edx),%ebx
    3e58:	80 fb 09             	cmp    $0x9,%bl
    3e5b:	76 eb                	jbe    3e48 <atoi+0x18>
  return n;
}
    3e5d:	5b                   	pop    %ebx
    3e5e:	5d                   	pop    %ebp
    3e5f:	c3                   	ret    

00003e60 <memmove>:

void*
memmove(void *vdst, const void *vsrc, int n)
{
    3e60:	55                   	push   %ebp
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
    3e61:	31 d2                	xor    %edx,%edx
{
    3e63:	89 e5                	mov    %esp,%ebp
    3e65:	56                   	push   %esi
    3e66:	8b 45 08             	mov    0x8(%ebp),%eax
    3e69:	53                   	push   %ebx
    3e6a:	8b 5d 10             	mov    0x10(%ebp),%ebx
    3e6d:	8b 75 0c             	mov    0xc(%ebp),%esi
  while(n-- > 0)
    3e70:	85 db                	test   %ebx,%ebx
    3e72:	7e 12                	jle    3e86 <memmove+0x26>
    3e74:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    *dst++ = *src++;
    3e78:	0f b6 0c 16          	movzbl (%esi,%edx,1),%ecx
    3e7c:	88 0c 10             	mov    %cl,(%eax,%edx,1)
    3e7f:	83 c2 01             	add    $0x1,%edx
  while(n-- > 0)
    3e82:	39 da                	cmp    %ebx,%edx
    3e84:	75 f2                	jne    3e78 <memmove+0x18>
  return vdst;
}
    3e86:	5b                   	pop    %ebx
    3e87:	5e                   	pop    %esi
    3e88:	5d                   	pop    %ebp
    3e89:	c3                   	ret    

00003e8a <fork>:
  name: \
    movl $SYS_ ## name, %eax; \
    int $T_SYSCALL; \
    ret

SYSCALL(fork)
    3e8a:	b8 01 00 00 00       	mov    $0x1,%eax
    3e8f:	cd 40                	int    $0x40
    3e91:	c3                   	ret    

00003e92 <exit>:
SYSCALL(exit)
    3e92:	b8 02 00 00 00       	mov    $0x2,%eax
    3e97:	cd 40                	int    $0x40
    3e99:	c3                   	ret    

00003e9a <wait>:
SYSCALL(wait)
    3e9a:	b8 03 00 00 00       	mov    $0x3,%eax
    3e9f:	cd 40                	int    $0x40
    3ea1:	c3                   	ret    

00003ea2 <pipe>:
SYSCALL(pipe)
    3ea2:	b8 04 00 00 00       	mov    $0x4,%eax
    3ea7:	cd 40                	int    $0x40
    3ea9:	c3                   	ret    

00003eaa <read>:
SYSCALL(read)
    3eaa:	b8 05 00 00 00       	mov    $0x5,%eax
    3eaf:	cd 40                	int    $0x40
    3eb1:	c3                   	ret    

00003eb2 <write>:
SYSCALL(write)
    3eb2:	b8 10 00 00 00       	mov    $0x10,%eax
    3eb7:	cd 40                	int    $0x40
    3eb9:	c3                   	ret    

00003eba <close>:
SYSCALL(close)
    3eba:	b8 15 00 00 00       	mov    $0x15,%eax
    3ebf:	cd 40                	int    $0x40
    3ec1:	c3                   	ret    

00003ec2 <kill>:
SYSCALL(kill)
    3ec2:	b8 06 00 00 00       	mov    $0x6,%eax
    3ec7:	cd 40                	int    $0x40
    3ec9:	c3                   	ret    

00003eca <exec>:
SYSCALL(exec)
    3eca:	b8 07 00 00 00       	mov    $0x7,%eax
    3ecf:	cd 40                	int    $0x40
    3ed1:	c3                   	ret    

00003ed2 <open>:
SYSCALL(open)
    3ed2:	b8 0f 00 00 00       	mov    $0xf,%eax
    3ed7:	cd 40                	int    $0x40
    3ed9:	c3                   	ret    

00003eda <mknod>:
SYSCALL(mknod)
    3eda:	b8 11 00 00 00       	mov    $0x11,%eax
    3edf:	cd 40                	int    $0x40
    3ee1:	c3                   	ret    

00003ee2 <unlink>:
SYSCALL(unlink)
    3ee2:	b8 12 00 00 00       	mov    $0x12,%eax
    3ee7:	cd 40                	int    $0x40
    3ee9:	c3                   	ret    

00003eea <fstat>:
SYSCALL(fstat)
    3eea:	b8 08 00 00 00       	mov    $0x8,%eax
    3eef:	cd 40                	int    $0x40
    3ef1:	c3                   	ret    

00003ef2 <link>:
SYSCALL(link)
    3ef2:	b8 13 00 00 00       	mov    $0x13,%eax
    3ef7:	cd 40                	int    $0x40
    3ef9:	c3                   	ret    

00003efa <mkdir>:
SYSCALL(mkdir)
    3efa:	b8 14 00 00 00       	mov    $0x14,%eax
    3eff:	cd 40                	int    $0x40
    3f01:	c3                   	ret    

00003f02 <chdir>:
SYSCALL(chdir)
    3f02:	b8 09 00 00 00       	mov    $0x9,%eax
    3f07:	cd 40                	int    $0x40
    3f09:	c3                   	ret    

00003f0a <dup>:
SYSCALL(dup)
    3f0a:	b8 0a 00 00 00       	mov    $0xa,%eax
    3f0f:	cd 40                	int    $0x40
    3f11:	c3                   	ret    

00003f12 <getpid>:
SYSCALL(getpid)
    3f12:	b8 0b 00 00 00       	mov    $0xb,%eax
    3f17:	cd 40                	int    $0x40
    3f19:	c3                   	ret    

00003f1a <sbrk>:
SYSCALL(sbrk)
    3f1a:	b8 0c 00 00 00       	mov    $0xc,%eax
    3f1f:	cd 40                	int    $0x40
    3f21:	c3                   	ret    

00003f22 <sleep>:
SYSCALL(sleep)
    3f22:	b8 0d 00 00 00       	mov    $0xd,%eax
    3f27:	cd 40                	int    $0x40
    3f29:	c3                   	ret    

00003f2a <uptime>:
SYSCALL(uptime)
    3f2a:	b8 0e 00 00 00       	mov    $0xe,%eax
    3f2f:	cd 40                	int    $0x40
    3f31:	c3                   	ret    
    3f32:	66 90                	xchg   %ax,%ax
    3f34:	66 90                	xchg   %ax,%ax
    3f36:	66 90                	xchg   %ax,%ax
    3f38:	66 90                	xchg   %ax,%ax
    3f3a:	66 90                	xchg   %ax,%ax
    3f3c:	66 90                	xchg   %ax,%ax
    3f3e:	66 90                	xchg   %ax,%ax

00003f40 <printint>:
  write(fd, &c, 1);
}

static void
printint(int fd, int xx, int base, int sgn)
{
    3f40:	55                   	push   %ebp
    3f41:	89 e5                	mov    %esp,%ebp
    3f43:	57                   	push   %edi
    3f44:	56                   	push   %esi
    3f45:	89 c6                	mov    %eax,%esi
    3f47:	53                   	push   %ebx
    3f48:	83 ec 4c             	sub    $0x4c,%esp
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
    3f4b:	8b 5d 08             	mov    0x8(%ebp),%ebx
    3f4e:	85 db                	test   %ebx,%ebx
    3f50:	74 09                	je     3f5b <printint+0x1b>
    3f52:	89 d0                	mov    %edx,%eax
    3f54:	c1 e8 1f             	shr    $0x1f,%eax
    3f57:	84 c0                	test   %al,%al
    3f59:	75 75                	jne    3fd0 <printint+0x90>
    neg = 1;
    x = -xx;
  } else {
    x = xx;
    3f5b:	89 d0                	mov    %edx,%eax
  neg = 0;
    3f5d:	c7 45 c4 00 00 00 00 	movl   $0x0,-0x3c(%ebp)
    3f64:	89 75 c0             	mov    %esi,-0x40(%ebp)
  }

  i = 0;
    3f67:	31 ff                	xor    %edi,%edi
    3f69:	89 ce                	mov    %ecx,%esi
    3f6b:	8d 5d d7             	lea    -0x29(%ebp),%ebx
    3f6e:	eb 02                	jmp    3f72 <printint+0x32>
  do{
    buf[i++] = digits[x % base];
    3f70:	89 cf                	mov    %ecx,%edi
    3f72:	31 d2                	xor    %edx,%edx
    3f74:	f7 f6                	div    %esi
    3f76:	8d 4f 01             	lea    0x1(%edi),%ecx
    3f79:	0f b6 92 e7 5a 00 00 	movzbl 0x5ae7(%edx),%edx
  }while((x /= base) != 0);
    3f80:	85 c0                	test   %eax,%eax
    buf[i++] = digits[x % base];
    3f82:	88 14 0b             	mov    %dl,(%ebx,%ecx,1)
  }while((x /= base) != 0);
    3f85:	75 e9                	jne    3f70 <printint+0x30>
  if(neg)
    3f87:	8b 55 c4             	mov    -0x3c(%ebp),%edx
    buf[i++] = digits[x % base];
    3f8a:	89 c8                	mov    %ecx,%eax
    3f8c:	8b 75 c0             	mov    -0x40(%ebp),%esi
  if(neg)
    3f8f:	85 d2                	test   %edx,%edx
    3f91:	74 08                	je     3f9b <printint+0x5b>
    buf[i++] = '-';
    3f93:	8d 4f 02             	lea    0x2(%edi),%ecx
    3f96:	c6 44 05 d8 2d       	movb   $0x2d,-0x28(%ebp,%eax,1)

  while(--i >= 0)
    3f9b:	8d 79 ff             	lea    -0x1(%ecx),%edi
    3f9e:	66 90                	xchg   %ax,%ax
    3fa0:	0f b6 44 3d d8       	movzbl -0x28(%ebp,%edi,1),%eax
    3fa5:	83 ef 01             	sub    $0x1,%edi
  write(fd, &c, 1);
    3fa8:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
    3faf:	00 
    3fb0:	89 5c 24 04          	mov    %ebx,0x4(%esp)
    3fb4:	89 34 24             	mov    %esi,(%esp)
    3fb7:	88 45 d7             	mov    %al,-0x29(%ebp)
    3fba:	e8 f3 fe ff ff       	call   3eb2 <write>
  while(--i >= 0)
    3fbf:	83 ff ff             	cmp    $0xffffffff,%edi
    3fc2:	75 dc                	jne    3fa0 <printint+0x60>
    putc(fd, buf[i]);
}
    3fc4:	83 c4 4c             	add    $0x4c,%esp
    3fc7:	5b                   	pop    %ebx
    3fc8:	5e                   	pop    %esi
    3fc9:	5f                   	pop    %edi
    3fca:	5d                   	pop    %ebp
    3fcb:	c3                   	ret    
    3fcc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    x = -xx;
    3fd0:	89 d0                	mov    %edx,%eax
    3fd2:	f7 d8                	neg    %eax
    neg = 1;
    3fd4:	c7 45 c4 01 00 00 00 	movl   $0x1,-0x3c(%ebp)
    3fdb:	eb 87                	jmp    3f64 <printint+0x24>
    3fdd:	8d 76 00             	lea    0x0(%esi),%esi

00003fe0 <printf>:

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, const char *fmt, ...)
{
    3fe0:	55                   	push   %ebp
    3fe1:	89 e5                	mov    %esp,%ebp
    3fe3:	57                   	push   %edi
  char *s;
  int c, i, state;
  uint *ap;

  state = 0;
    3fe4:	31 ff                	xor    %edi,%edi
{
    3fe6:	56                   	push   %esi
    3fe7:	53                   	push   %ebx
    3fe8:	83 ec 3c             	sub    $0x3c,%esp
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
    3feb:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  ap = (uint*)(void*)&fmt + 1;
    3fee:	8d 45 10             	lea    0x10(%ebp),%eax
{
    3ff1:	8b 75 08             	mov    0x8(%ebp),%esi
  ap = (uint*)(void*)&fmt + 1;
    3ff4:	89 45 d4             	mov    %eax,-0x2c(%ebp)
  for(i = 0; fmt[i]; i++){
    3ff7:	0f b6 13             	movzbl (%ebx),%edx
    3ffa:	83 c3 01             	add    $0x1,%ebx
    3ffd:	84 d2                	test   %dl,%dl
    3fff:	75 39                	jne    403a <printf+0x5a>
    4001:	e9 c2 00 00 00       	jmp    40c8 <printf+0xe8>
    4006:	66 90                	xchg   %ax,%ax
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
    4008:	83 fa 25             	cmp    $0x25,%edx
    400b:	0f 84 bf 00 00 00    	je     40d0 <printf+0xf0>
  write(fd, &c, 1);
    4011:	8d 45 e2             	lea    -0x1e(%ebp),%eax
    4014:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
    401b:	00 
    401c:	89 44 24 04          	mov    %eax,0x4(%esp)
    4020:	89 34 24             	mov    %esi,(%esp)
        state = '%';
      } else {
        putc(fd, c);
    4023:	88 55 e2             	mov    %dl,-0x1e(%ebp)
  write(fd, &c, 1);
    4026:	e8 87 fe ff ff       	call   3eb2 <write>
    402b:	83 c3 01             	add    $0x1,%ebx
  for(i = 0; fmt[i]; i++){
    402e:	0f b6 53 ff          	movzbl -0x1(%ebx),%edx
    4032:	84 d2                	test   %dl,%dl
    4034:	0f 84 8e 00 00 00    	je     40c8 <printf+0xe8>
    if(state == 0){
    403a:	85 ff                	test   %edi,%edi
    c = fmt[i] & 0xff;
    403c:	0f be c2             	movsbl %dl,%eax
    if(state == 0){
    403f:	74 c7                	je     4008 <printf+0x28>
      }
    } else if(state == '%'){
    4041:	83 ff 25             	cmp    $0x25,%edi
    4044:	75 e5                	jne    402b <printf+0x4b>
      if(c == 'd'){
    4046:	83 fa 64             	cmp    $0x64,%edx
    4049:	0f 84 31 01 00 00    	je     4180 <printf+0x1a0>
        printint(fd, *ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
    404f:	25 f7 00 00 00       	and    $0xf7,%eax
    4054:	83 f8 70             	cmp    $0x70,%eax
    4057:	0f 84 83 00 00 00    	je     40e0 <printf+0x100>
        printint(fd, *ap, 16, 0);
        ap++;
      } else if(c == 's'){
    405d:	83 fa 73             	cmp    $0x73,%edx
    4060:	0f 84 a2 00 00 00    	je     4108 <printf+0x128>
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
        }
      } else if(c == 'c'){
    4066:	83 fa 63             	cmp    $0x63,%edx
    4069:	0f 84 35 01 00 00    	je     41a4 <printf+0x1c4>
        putc(fd, *ap);
        ap++;
      } else if(c == '%'){
    406f:	83 fa 25             	cmp    $0x25,%edx
    4072:	0f 84 e0 00 00 00    	je     4158 <printf+0x178>
  write(fd, &c, 1);
    4078:	8d 45 e6             	lea    -0x1a(%ebp),%eax
    407b:	83 c3 01             	add    $0x1,%ebx
    407e:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
    4085:	00 
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
    4086:	31 ff                	xor    %edi,%edi
  write(fd, &c, 1);
    4088:	89 44 24 04          	mov    %eax,0x4(%esp)
    408c:	89 34 24             	mov    %esi,(%esp)
    408f:	89 55 d0             	mov    %edx,-0x30(%ebp)
    4092:	c6 45 e6 25          	movb   $0x25,-0x1a(%ebp)
    4096:	e8 17 fe ff ff       	call   3eb2 <write>
        putc(fd, c);
    409b:	8b 55 d0             	mov    -0x30(%ebp),%edx
  write(fd, &c, 1);
    409e:	8d 45 e7             	lea    -0x19(%ebp),%eax
    40a1:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
    40a8:	00 
    40a9:	89 44 24 04          	mov    %eax,0x4(%esp)
    40ad:	89 34 24             	mov    %esi,(%esp)
        putc(fd, c);
    40b0:	88 55 e7             	mov    %dl,-0x19(%ebp)
  write(fd, &c, 1);
    40b3:	e8 fa fd ff ff       	call   3eb2 <write>
  for(i = 0; fmt[i]; i++){
    40b8:	0f b6 53 ff          	movzbl -0x1(%ebx),%edx
    40bc:	84 d2                	test   %dl,%dl
    40be:	0f 85 76 ff ff ff    	jne    403a <printf+0x5a>
    40c4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    }
  }
}
    40c8:	83 c4 3c             	add    $0x3c,%esp
    40cb:	5b                   	pop    %ebx
    40cc:	5e                   	pop    %esi
    40cd:	5f                   	pop    %edi
    40ce:	5d                   	pop    %ebp
    40cf:	c3                   	ret    
        state = '%';
    40d0:	bf 25 00 00 00       	mov    $0x25,%edi
    40d5:	e9 51 ff ff ff       	jmp    402b <printf+0x4b>
    40da:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        printint(fd, *ap, 16, 0);
    40e0:	8b 45 d4             	mov    -0x2c(%ebp),%eax
    40e3:	b9 10 00 00 00       	mov    $0x10,%ecx
      state = 0;
    40e8:	31 ff                	xor    %edi,%edi
        printint(fd, *ap, 16, 0);
    40ea:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
    40f1:	8b 10                	mov    (%eax),%edx
    40f3:	89 f0                	mov    %esi,%eax
    40f5:	e8 46 fe ff ff       	call   3f40 <printint>
        ap++;
    40fa:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
    40fe:	e9 28 ff ff ff       	jmp    402b <printf+0x4b>
    4103:	90                   	nop
    4104:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
        s = (char*)*ap;
    4108:	8b 45 d4             	mov    -0x2c(%ebp),%eax
        ap++;
    410b:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
        s = (char*)*ap;
    410f:	8b 38                	mov    (%eax),%edi
          s = "(null)";
    4111:	b8 e0 5a 00 00       	mov    $0x5ae0,%eax
    4116:	85 ff                	test   %edi,%edi
    4118:	0f 44 f8             	cmove  %eax,%edi
        while(*s != 0){
    411b:	0f b6 07             	movzbl (%edi),%eax
    411e:	84 c0                	test   %al,%al
    4120:	74 2a                	je     414c <printf+0x16c>
    4122:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    4128:	88 45 e3             	mov    %al,-0x1d(%ebp)
  write(fd, &c, 1);
    412b:	8d 45 e3             	lea    -0x1d(%ebp),%eax
          s++;
    412e:	83 c7 01             	add    $0x1,%edi
  write(fd, &c, 1);
    4131:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
    4138:	00 
    4139:	89 44 24 04          	mov    %eax,0x4(%esp)
    413d:	89 34 24             	mov    %esi,(%esp)
    4140:	e8 6d fd ff ff       	call   3eb2 <write>
        while(*s != 0){
    4145:	0f b6 07             	movzbl (%edi),%eax
    4148:	84 c0                	test   %al,%al
    414a:	75 dc                	jne    4128 <printf+0x148>
      state = 0;
    414c:	31 ff                	xor    %edi,%edi
    414e:	e9 d8 fe ff ff       	jmp    402b <printf+0x4b>
    4153:	90                   	nop
    4154:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  write(fd, &c, 1);
    4158:	8d 45 e5             	lea    -0x1b(%ebp),%eax
      state = 0;
    415b:	31 ff                	xor    %edi,%edi
  write(fd, &c, 1);
    415d:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
    4164:	00 
    4165:	89 44 24 04          	mov    %eax,0x4(%esp)
    4169:	89 34 24             	mov    %esi,(%esp)
    416c:	c6 45 e5 25          	movb   $0x25,-0x1b(%ebp)
    4170:	e8 3d fd ff ff       	call   3eb2 <write>
    4175:	e9 b1 fe ff ff       	jmp    402b <printf+0x4b>
    417a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        printint(fd, *ap, 10, 1);
    4180:	8b 45 d4             	mov    -0x2c(%ebp),%eax
    4183:	b9 0a 00 00 00       	mov    $0xa,%ecx
      state = 0;
    4188:	66 31 ff             	xor    %di,%di
        printint(fd, *ap, 10, 1);
    418b:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    4192:	8b 10                	mov    (%eax),%edx
    4194:	89 f0                	mov    %esi,%eax
    4196:	e8 a5 fd ff ff       	call   3f40 <printint>
        ap++;
    419b:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
    419f:	e9 87 fe ff ff       	jmp    402b <printf+0x4b>
        putc(fd, *ap);
    41a4:	8b 45 d4             	mov    -0x2c(%ebp),%eax
      state = 0;
    41a7:	31 ff                	xor    %edi,%edi
        putc(fd, *ap);
    41a9:	8b 00                	mov    (%eax),%eax
  write(fd, &c, 1);
    41ab:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
    41b2:	00 
    41b3:	89 34 24             	mov    %esi,(%esp)
        putc(fd, *ap);
    41b6:	88 45 e4             	mov    %al,-0x1c(%ebp)
  write(fd, &c, 1);
    41b9:	8d 45 e4             	lea    -0x1c(%ebp),%eax
    41bc:	89 44 24 04          	mov    %eax,0x4(%esp)
    41c0:	e8 ed fc ff ff       	call   3eb2 <write>
        ap++;
    41c5:	83 45 d4 04          	addl   $0x4,-0x2c(%ebp)
    41c9:	e9 5d fe ff ff       	jmp    402b <printf+0x4b>
    41ce:	66 90                	xchg   %ax,%ax

000041d0 <free>:
static Header base;
static Header *freep;

void
free(void *ap)
{
    41d0:	55                   	push   %ebp
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    41d1:	a1 60 64 00 00       	mov    0x6460,%eax
{
    41d6:	89 e5                	mov    %esp,%ebp
    41d8:	57                   	push   %edi
    41d9:	56                   	push   %esi
    41da:	53                   	push   %ebx
    41db:	8b 5d 08             	mov    0x8(%ebp),%ebx
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
    41de:	8b 08                	mov    (%eax),%ecx
  bp = (Header*)ap - 1;
    41e0:	8d 53 f8             	lea    -0x8(%ebx),%edx
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    41e3:	39 d0                	cmp    %edx,%eax
    41e5:	72 11                	jb     41f8 <free+0x28>
    41e7:	90                   	nop
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
    41e8:	39 c8                	cmp    %ecx,%eax
    41ea:	72 04                	jb     41f0 <free+0x20>
    41ec:	39 ca                	cmp    %ecx,%edx
    41ee:	72 10                	jb     4200 <free+0x30>
    41f0:	89 c8                	mov    %ecx,%eax
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    41f2:	39 d0                	cmp    %edx,%eax
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
    41f4:	8b 08                	mov    (%eax),%ecx
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    41f6:	73 f0                	jae    41e8 <free+0x18>
    41f8:	39 ca                	cmp    %ecx,%edx
    41fa:	72 04                	jb     4200 <free+0x30>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
    41fc:	39 c8                	cmp    %ecx,%eax
    41fe:	72 f0                	jb     41f0 <free+0x20>
      break;
  if(bp + bp->s.size == p->s.ptr){
    4200:	8b 73 fc             	mov    -0x4(%ebx),%esi
    4203:	8d 3c f2             	lea    (%edx,%esi,8),%edi
    4206:	39 cf                	cmp    %ecx,%edi
    4208:	74 1e                	je     4228 <free+0x58>
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
    420a:	89 4b f8             	mov    %ecx,-0x8(%ebx)
  if(p + p->s.size == bp){
    420d:	8b 48 04             	mov    0x4(%eax),%ecx
    4210:	8d 34 c8             	lea    (%eax,%ecx,8),%esi
    4213:	39 f2                	cmp    %esi,%edx
    4215:	74 28                	je     423f <free+0x6f>
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
    4217:	89 10                	mov    %edx,(%eax)
  freep = p;
    4219:	a3 60 64 00 00       	mov    %eax,0x6460
}
    421e:	5b                   	pop    %ebx
    421f:	5e                   	pop    %esi
    4220:	5f                   	pop    %edi
    4221:	5d                   	pop    %ebp
    4222:	c3                   	ret    
    4223:	90                   	nop
    4224:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    bp->s.size += p->s.ptr->s.size;
    4228:	03 71 04             	add    0x4(%ecx),%esi
    422b:	89 73 fc             	mov    %esi,-0x4(%ebx)
    bp->s.ptr = p->s.ptr->s.ptr;
    422e:	8b 08                	mov    (%eax),%ecx
    4230:	8b 09                	mov    (%ecx),%ecx
    4232:	89 4b f8             	mov    %ecx,-0x8(%ebx)
  if(p + p->s.size == bp){
    4235:	8b 48 04             	mov    0x4(%eax),%ecx
    4238:	8d 34 c8             	lea    (%eax,%ecx,8),%esi
    423b:	39 f2                	cmp    %esi,%edx
    423d:	75 d8                	jne    4217 <free+0x47>
    p->s.size += bp->s.size;
    423f:	03 4b fc             	add    -0x4(%ebx),%ecx
  freep = p;
    4242:	a3 60 64 00 00       	mov    %eax,0x6460
    p->s.size += bp->s.size;
    4247:	89 48 04             	mov    %ecx,0x4(%eax)
    p->s.ptr = bp->s.ptr;
    424a:	8b 53 f8             	mov    -0x8(%ebx),%edx
    424d:	89 10                	mov    %edx,(%eax)
}
    424f:	5b                   	pop    %ebx
    4250:	5e                   	pop    %esi
    4251:	5f                   	pop    %edi
    4252:	5d                   	pop    %ebp
    4253:	c3                   	ret    
    4254:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    425a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

00004260 <malloc>:
  return freep;
}

void*
malloc(uint nbytes)
{
    4260:	55                   	push   %ebp
    4261:	89 e5                	mov    %esp,%ebp
    4263:	57                   	push   %edi
    4264:	56                   	push   %esi
    4265:	53                   	push   %ebx
    4266:	83 ec 1c             	sub    $0x1c,%esp
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
    4269:	8b 45 08             	mov    0x8(%ebp),%eax
  if((prevp = freep) == 0){
    426c:	8b 1d 60 64 00 00    	mov    0x6460,%ebx
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
    4272:	8d 48 07             	lea    0x7(%eax),%ecx
    4275:	c1 e9 03             	shr    $0x3,%ecx
  if((prevp = freep) == 0){
    4278:	85 db                	test   %ebx,%ebx
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
    427a:	8d 71 01             	lea    0x1(%ecx),%esi
  if((prevp = freep) == 0){
    427d:	0f 84 9b 00 00 00    	je     431e <malloc+0xbe>
    4283:	8b 13                	mov    (%ebx),%edx
    4285:	8b 7a 04             	mov    0x4(%edx),%edi
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
    4288:	39 fe                	cmp    %edi,%esi
    428a:	76 64                	jbe    42f0 <malloc+0x90>
    428c:	8d 04 f5 00 00 00 00 	lea    0x0(,%esi,8),%eax
  if(nu < 4096)
    4293:	bb 00 80 00 00       	mov    $0x8000,%ebx
    4298:	89 45 e4             	mov    %eax,-0x1c(%ebp)
    429b:	eb 0e                	jmp    42ab <malloc+0x4b>
    429d:	8d 76 00             	lea    0x0(%esi),%esi
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    42a0:	8b 02                	mov    (%edx),%eax
    if(p->s.size >= nunits){
    42a2:	8b 78 04             	mov    0x4(%eax),%edi
    42a5:	39 fe                	cmp    %edi,%esi
    42a7:	76 4f                	jbe    42f8 <malloc+0x98>
    42a9:	89 c2                	mov    %eax,%edx
        p->s.size = nunits;
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
    42ab:	3b 15 60 64 00 00    	cmp    0x6460,%edx
    42b1:	75 ed                	jne    42a0 <malloc+0x40>
  if(nu < 4096)
    42b3:	8b 45 e4             	mov    -0x1c(%ebp),%eax
    42b6:	81 fe 00 10 00 00    	cmp    $0x1000,%esi
    42bc:	bf 00 10 00 00       	mov    $0x1000,%edi
    42c1:	0f 43 fe             	cmovae %esi,%edi
    42c4:	0f 42 c3             	cmovb  %ebx,%eax
  p = sbrk(nu * sizeof(Header));
    42c7:	89 04 24             	mov    %eax,(%esp)
    42ca:	e8 4b fc ff ff       	call   3f1a <sbrk>
  if(p == (char*)-1)
    42cf:	83 f8 ff             	cmp    $0xffffffff,%eax
    42d2:	74 18                	je     42ec <malloc+0x8c>
  hp->s.size = nu;
    42d4:	89 78 04             	mov    %edi,0x4(%eax)
  free((void*)(hp + 1));
    42d7:	83 c0 08             	add    $0x8,%eax
    42da:	89 04 24             	mov    %eax,(%esp)
    42dd:	e8 ee fe ff ff       	call   41d0 <free>
  return freep;
    42e2:	8b 15 60 64 00 00    	mov    0x6460,%edx
      if((p = morecore(nunits)) == 0)
    42e8:	85 d2                	test   %edx,%edx
    42ea:	75 b4                	jne    42a0 <malloc+0x40>
        return 0;
    42ec:	31 c0                	xor    %eax,%eax
    42ee:	eb 20                	jmp    4310 <malloc+0xb0>
    if(p->s.size >= nunits){
    42f0:	89 d0                	mov    %edx,%eax
    42f2:	89 da                	mov    %ebx,%edx
    42f4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      if(p->s.size == nunits)
    42f8:	39 fe                	cmp    %edi,%esi
    42fa:	74 1c                	je     4318 <malloc+0xb8>
        p->s.size -= nunits;
    42fc:	29 f7                	sub    %esi,%edi
    42fe:	89 78 04             	mov    %edi,0x4(%eax)
        p += p->s.size;
    4301:	8d 04 f8             	lea    (%eax,%edi,8),%eax
        p->s.size = nunits;
    4304:	89 70 04             	mov    %esi,0x4(%eax)
      freep = prevp;
    4307:	89 15 60 64 00 00    	mov    %edx,0x6460
      return (void*)(p + 1);
    430d:	83 c0 08             	add    $0x8,%eax
  }
}
    4310:	83 c4 1c             	add    $0x1c,%esp
    4313:	5b                   	pop    %ebx
    4314:	5e                   	pop    %esi
    4315:	5f                   	pop    %edi
    4316:	5d                   	pop    %ebp
    4317:	c3                   	ret    
        prevp->s.ptr = p->s.ptr;
    4318:	8b 08                	mov    (%eax),%ecx
    431a:	89 0a                	mov    %ecx,(%edx)
    431c:	eb e9                	jmp    4307 <malloc+0xa7>
    base.s.ptr = freep = prevp = &base;
    431e:	c7 05 60 64 00 00 64 	movl   $0x6464,0x6460
    4325:	64 00 00 
    base.s.size = 0;
    4328:	ba 64 64 00 00       	mov    $0x6464,%edx
    base.s.ptr = freep = prevp = &base;
    432d:	c7 05 64 64 00 00 64 	movl   $0x6464,0x6464
    4334:	64 00 00 
    base.s.size = 0;
    4337:	c7 05 68 64 00 00 00 	movl   $0x0,0x6468
    433e:	00 00 00 
    4341:	e9 46 ff ff ff       	jmp    428c <malloc+0x2c>
