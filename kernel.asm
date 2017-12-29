
kernel:     file format elf32-i386


Disassembly of section .text:

80100000 <multiboot_header>:
80100000:	02 b0 ad 1b 00 00    	add    0x1bad(%eax),%dh
80100006:	00 00                	add    %al,(%eax)
80100008:	fe 4f 52             	decb   0x52(%edi)
8010000b:	e4                   	.byte 0xe4

8010000c <entry>:

# Entering xv6 on boot processor, with paging off.
.globl entry
entry:
  # Turn on page size extension for 4Mbyte pages
  movl    %cr4, %eax
8010000c:	0f 20 e0             	mov    %cr4,%eax
  orl     $(CR4_PSE), %eax
8010000f:	83 c8 10             	or     $0x10,%eax
  movl    %eax, %cr4
80100012:	0f 22 e0             	mov    %eax,%cr4
  # Set page directory
  movl    $(V2P_WO(entrypgdir)), %eax
80100015:	b8 00 90 10 00       	mov    $0x109000,%eax
  movl    %eax, %cr3
8010001a:	0f 22 d8             	mov    %eax,%cr3
  # Turn on paging.
  movl    %cr0, %eax
8010001d:	0f 20 c0             	mov    %cr0,%eax
  orl     $(CR0_PG|CR0_WP), %eax
80100020:	0d 00 00 01 80       	or     $0x80010000,%eax
  movl    %eax, %cr0
80100025:	0f 22 c0             	mov    %eax,%cr0

  # Set up the stack pointer.
  movl $(stack + KSTACKSIZE), %esp
80100028:	bc c0 b5 10 80       	mov    $0x8010b5c0,%esp

  # Jump to main(), and switch to executing at
  # high addresses. The indirect call is needed because
  # the assembler produces a PC-relative instruction
  # for a direct jump.
  mov $main, %eax
8010002d:	b8 60 2e 10 80       	mov    $0x80102e60,%eax
  jmp *%eax
80100032:	ff e0                	jmp    *%eax
80100034:	66 90                	xchg   %ax,%ax
80100036:	66 90                	xchg   %ax,%ax
80100038:	66 90                	xchg   %ax,%ax
8010003a:	66 90                	xchg   %ax,%ax
8010003c:	66 90                	xchg   %ax,%ax
8010003e:	66 90                	xchg   %ax,%ax

80100040 <binit>:
80100040:	55                   	push   %ebp
80100041:	89 e5                	mov    %esp,%ebp
80100043:	53                   	push   %ebx
80100044:	bb f4 b5 10 80       	mov    $0x8010b5f4,%ebx
80100049:	83 ec 0c             	sub    $0xc,%esp
8010004c:	68 80 74 10 80       	push   $0x80107480
80100051:	68 c0 b5 10 80       	push   $0x8010b5c0
80100056:	e8 15 46 00 00       	call   80104670 <initlock>
8010005b:	c7 05 0c fd 10 80 bc 	movl   $0x8010fcbc,0x8010fd0c
80100062:	fc 10 80 
80100065:	c7 05 10 fd 10 80 bc 	movl   $0x8010fcbc,0x8010fd10
8010006c:	fc 10 80 
8010006f:	83 c4 10             	add    $0x10,%esp
80100072:	ba bc fc 10 80       	mov    $0x8010fcbc,%edx
80100077:	eb 09                	jmp    80100082 <binit+0x42>
80100079:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80100080:	89 c3                	mov    %eax,%ebx
80100082:	8d 43 0c             	lea    0xc(%ebx),%eax
80100085:	83 ec 08             	sub    $0x8,%esp
80100088:	89 53 54             	mov    %edx,0x54(%ebx)
8010008b:	c7 43 50 bc fc 10 80 	movl   $0x8010fcbc,0x50(%ebx)
80100092:	68 87 74 10 80       	push   $0x80107487
80100097:	50                   	push   %eax
80100098:	e8 c3 44 00 00       	call   80104560 <initsleeplock>
8010009d:	a1 10 fd 10 80       	mov    0x8010fd10,%eax
801000a2:	83 c4 10             	add    $0x10,%esp
801000a5:	89 da                	mov    %ebx,%edx
801000a7:	89 58 50             	mov    %ebx,0x50(%eax)
801000aa:	8d 83 5c 02 00 00    	lea    0x25c(%ebx),%eax
801000b0:	89 1d 10 fd 10 80    	mov    %ebx,0x8010fd10
801000b6:	3d bc fc 10 80       	cmp    $0x8010fcbc,%eax
801000bb:	75 c3                	jne    80100080 <binit+0x40>
801000bd:	8b 5d fc             	mov    -0x4(%ebp),%ebx
801000c0:	c9                   	leave  
801000c1:	c3                   	ret    
801000c2:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801000c9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801000d0 <bread>:
801000d0:	55                   	push   %ebp
801000d1:	89 e5                	mov    %esp,%ebp
801000d3:	57                   	push   %edi
801000d4:	56                   	push   %esi
801000d5:	53                   	push   %ebx
801000d6:	83 ec 18             	sub    $0x18,%esp
801000d9:	8b 75 08             	mov    0x8(%ebp),%esi
801000dc:	8b 7d 0c             	mov    0xc(%ebp),%edi
801000df:	68 c0 b5 10 80       	push   $0x8010b5c0
801000e4:	e8 87 46 00 00       	call   80104770 <acquire>
801000e9:	8b 1d 10 fd 10 80    	mov    0x8010fd10,%ebx
801000ef:	83 c4 10             	add    $0x10,%esp
801000f2:	81 fb bc fc 10 80    	cmp    $0x8010fcbc,%ebx
801000f8:	75 11                	jne    8010010b <bread+0x3b>
801000fa:	eb 24                	jmp    80100120 <bread+0x50>
801000fc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80100100:	8b 5b 54             	mov    0x54(%ebx),%ebx
80100103:	81 fb bc fc 10 80    	cmp    $0x8010fcbc,%ebx
80100109:	74 15                	je     80100120 <bread+0x50>
8010010b:	3b 73 04             	cmp    0x4(%ebx),%esi
8010010e:	75 f0                	jne    80100100 <bread+0x30>
80100110:	3b 7b 08             	cmp    0x8(%ebx),%edi
80100113:	75 eb                	jne    80100100 <bread+0x30>
80100115:	83 43 4c 01          	addl   $0x1,0x4c(%ebx)
80100119:	eb 3f                	jmp    8010015a <bread+0x8a>
8010011b:	90                   	nop
8010011c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80100120:	8b 1d 0c fd 10 80    	mov    0x8010fd0c,%ebx
80100126:	81 fb bc fc 10 80    	cmp    $0x8010fcbc,%ebx
8010012c:	75 0d                	jne    8010013b <bread+0x6b>
8010012e:	eb 60                	jmp    80100190 <bread+0xc0>
80100130:	8b 5b 50             	mov    0x50(%ebx),%ebx
80100133:	81 fb bc fc 10 80    	cmp    $0x8010fcbc,%ebx
80100139:	74 55                	je     80100190 <bread+0xc0>
8010013b:	8b 43 4c             	mov    0x4c(%ebx),%eax
8010013e:	85 c0                	test   %eax,%eax
80100140:	75 ee                	jne    80100130 <bread+0x60>
80100142:	f6 03 04             	testb  $0x4,(%ebx)
80100145:	75 e9                	jne    80100130 <bread+0x60>
80100147:	89 73 04             	mov    %esi,0x4(%ebx)
8010014a:	89 7b 08             	mov    %edi,0x8(%ebx)
8010014d:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)
80100153:	c7 43 4c 01 00 00 00 	movl   $0x1,0x4c(%ebx)
8010015a:	83 ec 0c             	sub    $0xc,%esp
8010015d:	68 c0 b5 10 80       	push   $0x8010b5c0
80100162:	e8 29 47 00 00       	call   80104890 <release>
80100167:	8d 43 0c             	lea    0xc(%ebx),%eax
8010016a:	89 04 24             	mov    %eax,(%esp)
8010016d:	e8 2e 44 00 00       	call   801045a0 <acquiresleep>
80100172:	83 c4 10             	add    $0x10,%esp
80100175:	f6 03 02             	testb  $0x2,(%ebx)
80100178:	75 0c                	jne    80100186 <bread+0xb6>
8010017a:	83 ec 0c             	sub    $0xc,%esp
8010017d:	53                   	push   %ebx
8010017e:	e8 6d 1f 00 00       	call   801020f0 <iderw>
80100183:	83 c4 10             	add    $0x10,%esp
80100186:	8d 65 f4             	lea    -0xc(%ebp),%esp
80100189:	89 d8                	mov    %ebx,%eax
8010018b:	5b                   	pop    %ebx
8010018c:	5e                   	pop    %esi
8010018d:	5f                   	pop    %edi
8010018e:	5d                   	pop    %ebp
8010018f:	c3                   	ret    
80100190:	83 ec 0c             	sub    $0xc,%esp
80100193:	68 8e 74 10 80       	push   $0x8010748e
80100198:	e8 d3 01 00 00       	call   80100370 <panic>
8010019d:	8d 76 00             	lea    0x0(%esi),%esi

801001a0 <bwrite>:
801001a0:	55                   	push   %ebp
801001a1:	89 e5                	mov    %esp,%ebp
801001a3:	53                   	push   %ebx
801001a4:	83 ec 10             	sub    $0x10,%esp
801001a7:	8b 5d 08             	mov    0x8(%ebp),%ebx
801001aa:	8d 43 0c             	lea    0xc(%ebx),%eax
801001ad:	50                   	push   %eax
801001ae:	e8 8d 44 00 00       	call   80104640 <holdingsleep>
801001b3:	83 c4 10             	add    $0x10,%esp
801001b6:	85 c0                	test   %eax,%eax
801001b8:	74 0f                	je     801001c9 <bwrite+0x29>
801001ba:	83 0b 04             	orl    $0x4,(%ebx)
801001bd:	89 5d 08             	mov    %ebx,0x8(%ebp)
801001c0:	8b 5d fc             	mov    -0x4(%ebp),%ebx
801001c3:	c9                   	leave  
801001c4:	e9 27 1f 00 00       	jmp    801020f0 <iderw>
801001c9:	83 ec 0c             	sub    $0xc,%esp
801001cc:	68 9f 74 10 80       	push   $0x8010749f
801001d1:	e8 9a 01 00 00       	call   80100370 <panic>
801001d6:	8d 76 00             	lea    0x0(%esi),%esi
801001d9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801001e0 <brelse>:
801001e0:	55                   	push   %ebp
801001e1:	89 e5                	mov    %esp,%ebp
801001e3:	56                   	push   %esi
801001e4:	53                   	push   %ebx
801001e5:	8b 5d 08             	mov    0x8(%ebp),%ebx
801001e8:	83 ec 0c             	sub    $0xc,%esp
801001eb:	8d 73 0c             	lea    0xc(%ebx),%esi
801001ee:	56                   	push   %esi
801001ef:	e8 4c 44 00 00       	call   80104640 <holdingsleep>
801001f4:	83 c4 10             	add    $0x10,%esp
801001f7:	85 c0                	test   %eax,%eax
801001f9:	74 66                	je     80100261 <brelse+0x81>
801001fb:	83 ec 0c             	sub    $0xc,%esp
801001fe:	56                   	push   %esi
801001ff:	e8 fc 43 00 00       	call   80104600 <releasesleep>
80100204:	c7 04 24 c0 b5 10 80 	movl   $0x8010b5c0,(%esp)
8010020b:	e8 60 45 00 00       	call   80104770 <acquire>
80100210:	8b 43 4c             	mov    0x4c(%ebx),%eax
80100213:	83 c4 10             	add    $0x10,%esp
80100216:	83 e8 01             	sub    $0x1,%eax
80100219:	85 c0                	test   %eax,%eax
8010021b:	89 43 4c             	mov    %eax,0x4c(%ebx)
8010021e:	75 2f                	jne    8010024f <brelse+0x6f>
80100220:	8b 43 54             	mov    0x54(%ebx),%eax
80100223:	8b 53 50             	mov    0x50(%ebx),%edx
80100226:	89 50 50             	mov    %edx,0x50(%eax)
80100229:	8b 43 50             	mov    0x50(%ebx),%eax
8010022c:	8b 53 54             	mov    0x54(%ebx),%edx
8010022f:	89 50 54             	mov    %edx,0x54(%eax)
80100232:	a1 10 fd 10 80       	mov    0x8010fd10,%eax
80100237:	c7 43 50 bc fc 10 80 	movl   $0x8010fcbc,0x50(%ebx)
8010023e:	89 43 54             	mov    %eax,0x54(%ebx)
80100241:	a1 10 fd 10 80       	mov    0x8010fd10,%eax
80100246:	89 58 50             	mov    %ebx,0x50(%eax)
80100249:	89 1d 10 fd 10 80    	mov    %ebx,0x8010fd10
8010024f:	c7 45 08 c0 b5 10 80 	movl   $0x8010b5c0,0x8(%ebp)
80100256:	8d 65 f8             	lea    -0x8(%ebp),%esp
80100259:	5b                   	pop    %ebx
8010025a:	5e                   	pop    %esi
8010025b:	5d                   	pop    %ebp
8010025c:	e9 2f 46 00 00       	jmp    80104890 <release>
80100261:	83 ec 0c             	sub    $0xc,%esp
80100264:	68 a6 74 10 80       	push   $0x801074a6
80100269:	e8 02 01 00 00       	call   80100370 <panic>
8010026e:	66 90                	xchg   %ax,%ax

80100270 <consoleread>:
80100270:	55                   	push   %ebp
80100271:	89 e5                	mov    %esp,%ebp
80100273:	57                   	push   %edi
80100274:	56                   	push   %esi
80100275:	53                   	push   %ebx
80100276:	83 ec 28             	sub    $0x28,%esp
80100279:	8b 7d 08             	mov    0x8(%ebp),%edi
8010027c:	8b 75 0c             	mov    0xc(%ebp),%esi
8010027f:	57                   	push   %edi
80100280:	e8 cb 14 00 00       	call   80101750 <iunlock>
80100285:	c7 04 24 20 a5 10 80 	movl   $0x8010a520,(%esp)
8010028c:	e8 df 44 00 00       	call   80104770 <acquire>
80100291:	8b 5d 10             	mov    0x10(%ebp),%ebx
80100294:	83 c4 10             	add    $0x10,%esp
80100297:	31 c0                	xor    %eax,%eax
80100299:	85 db                	test   %ebx,%ebx
8010029b:	0f 8e 9a 00 00 00    	jle    8010033b <consoleread+0xcb>
801002a1:	a1 a0 ff 10 80       	mov    0x8010ffa0,%eax
801002a6:	3b 05 a4 ff 10 80    	cmp    0x8010ffa4,%eax
801002ac:	74 24                	je     801002d2 <consoleread+0x62>
801002ae:	eb 58                	jmp    80100308 <consoleread+0x98>
801002b0:	83 ec 08             	sub    $0x8,%esp
801002b3:	68 20 a5 10 80       	push   $0x8010a520
801002b8:	68 a0 ff 10 80       	push   $0x8010ffa0
801002bd:	e8 de 3c 00 00       	call   80103fa0 <sleep>
801002c2:	a1 a0 ff 10 80       	mov    0x8010ffa0,%eax
801002c7:	83 c4 10             	add    $0x10,%esp
801002ca:	3b 05 a4 ff 10 80    	cmp    0x8010ffa4,%eax
801002d0:	75 36                	jne    80100308 <consoleread+0x98>
801002d2:	e8 99 35 00 00       	call   80103870 <myproc>
801002d7:	8b 40 24             	mov    0x24(%eax),%eax
801002da:	85 c0                	test   %eax,%eax
801002dc:	74 d2                	je     801002b0 <consoleread+0x40>
801002de:	83 ec 0c             	sub    $0xc,%esp
801002e1:	68 20 a5 10 80       	push   $0x8010a520
801002e6:	e8 a5 45 00 00       	call   80104890 <release>
801002eb:	89 3c 24             	mov    %edi,(%esp)
801002ee:	e8 7d 13 00 00       	call   80101670 <ilock>
801002f3:	83 c4 10             	add    $0x10,%esp
801002f6:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
801002fb:	8d 65 f4             	lea    -0xc(%ebp),%esp
801002fe:	5b                   	pop    %ebx
801002ff:	5e                   	pop    %esi
80100300:	5f                   	pop    %edi
80100301:	5d                   	pop    %ebp
80100302:	c3                   	ret    
80100303:	90                   	nop
80100304:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80100308:	8d 50 01             	lea    0x1(%eax),%edx
8010030b:	89 15 a0 ff 10 80    	mov    %edx,0x8010ffa0
80100311:	89 c2                	mov    %eax,%edx
80100313:	83 e2 7f             	and    $0x7f,%edx
80100316:	0f be 92 20 ff 10 80 	movsbl -0x7fef00e0(%edx),%edx
8010031d:	83 fa 04             	cmp    $0x4,%edx
80100320:	74 39                	je     8010035b <consoleread+0xeb>
80100322:	83 c6 01             	add    $0x1,%esi
80100325:	83 eb 01             	sub    $0x1,%ebx
80100328:	83 fa 0a             	cmp    $0xa,%edx
8010032b:	88 56 ff             	mov    %dl,-0x1(%esi)
8010032e:	74 35                	je     80100365 <consoleread+0xf5>
80100330:	85 db                	test   %ebx,%ebx
80100332:	0f 85 69 ff ff ff    	jne    801002a1 <consoleread+0x31>
80100338:	8b 45 10             	mov    0x10(%ebp),%eax
8010033b:	83 ec 0c             	sub    $0xc,%esp
8010033e:	89 45 e4             	mov    %eax,-0x1c(%ebp)
80100341:	68 20 a5 10 80       	push   $0x8010a520
80100346:	e8 45 45 00 00       	call   80104890 <release>
8010034b:	89 3c 24             	mov    %edi,(%esp)
8010034e:	e8 1d 13 00 00       	call   80101670 <ilock>
80100353:	83 c4 10             	add    $0x10,%esp
80100356:	8b 45 e4             	mov    -0x1c(%ebp),%eax
80100359:	eb a0                	jmp    801002fb <consoleread+0x8b>
8010035b:	39 5d 10             	cmp    %ebx,0x10(%ebp)
8010035e:	76 05                	jbe    80100365 <consoleread+0xf5>
80100360:	a3 a0 ff 10 80       	mov    %eax,0x8010ffa0
80100365:	8b 45 10             	mov    0x10(%ebp),%eax
80100368:	29 d8                	sub    %ebx,%eax
8010036a:	eb cf                	jmp    8010033b <consoleread+0xcb>
8010036c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80100370 <panic>:
80100370:	55                   	push   %ebp
80100371:	89 e5                	mov    %esp,%ebp
80100373:	56                   	push   %esi
80100374:	53                   	push   %ebx
80100375:	83 ec 30             	sub    $0x30,%esp
80100378:	fa                   	cli    
80100379:	c7 05 54 a5 10 80 00 	movl   $0x0,0x8010a554
80100380:	00 00 00 
80100383:	8d 5d d0             	lea    -0x30(%ebp),%ebx
80100386:	8d 75 f8             	lea    -0x8(%ebp),%esi
80100389:	e8 62 23 00 00       	call   801026f0 <lapicid>
8010038e:	83 ec 08             	sub    $0x8,%esp
80100391:	50                   	push   %eax
80100392:	68 ad 74 10 80       	push   $0x801074ad
80100397:	e8 c4 02 00 00       	call   80100660 <cprintf>
8010039c:	58                   	pop    %eax
8010039d:	ff 75 08             	pushl  0x8(%ebp)
801003a0:	e8 bb 02 00 00       	call   80100660 <cprintf>
801003a5:	c7 04 24 c3 7e 10 80 	movl   $0x80107ec3,(%esp)
801003ac:	e8 af 02 00 00       	call   80100660 <cprintf>
801003b1:	5a                   	pop    %edx
801003b2:	8d 45 08             	lea    0x8(%ebp),%eax
801003b5:	59                   	pop    %ecx
801003b6:	53                   	push   %ebx
801003b7:	50                   	push   %eax
801003b8:	e8 d3 42 00 00       	call   80104690 <getcallerpcs>
801003bd:	83 c4 10             	add    $0x10,%esp
801003c0:	83 ec 08             	sub    $0x8,%esp
801003c3:	ff 33                	pushl  (%ebx)
801003c5:	83 c3 04             	add    $0x4,%ebx
801003c8:	68 c1 74 10 80       	push   $0x801074c1
801003cd:	e8 8e 02 00 00       	call   80100660 <cprintf>
801003d2:	83 c4 10             	add    $0x10,%esp
801003d5:	39 f3                	cmp    %esi,%ebx
801003d7:	75 e7                	jne    801003c0 <panic+0x50>
801003d9:	c7 05 58 a5 10 80 01 	movl   $0x1,0x8010a558
801003e0:	00 00 00 
801003e3:	eb fe                	jmp    801003e3 <panic+0x73>
801003e5:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
801003e9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801003f0 <consputc>:
801003f0:	8b 15 58 a5 10 80    	mov    0x8010a558,%edx
801003f6:	85 d2                	test   %edx,%edx
801003f8:	74 06                	je     80100400 <consputc+0x10>
801003fa:	fa                   	cli    
801003fb:	eb fe                	jmp    801003fb <consputc+0xb>
801003fd:	8d 76 00             	lea    0x0(%esi),%esi
80100400:	55                   	push   %ebp
80100401:	89 e5                	mov    %esp,%ebp
80100403:	57                   	push   %edi
80100404:	56                   	push   %esi
80100405:	53                   	push   %ebx
80100406:	89 c3                	mov    %eax,%ebx
80100408:	83 ec 0c             	sub    $0xc,%esp
8010040b:	3d 00 01 00 00       	cmp    $0x100,%eax
80100410:	0f 84 b8 00 00 00    	je     801004ce <consputc+0xde>
80100416:	83 ec 0c             	sub    $0xc,%esp
80100419:	50                   	push   %eax
8010041a:	e8 21 5c 00 00       	call   80106040 <uartputc>
8010041f:	83 c4 10             	add    $0x10,%esp
80100422:	bf d4 03 00 00       	mov    $0x3d4,%edi
80100427:	b8 0e 00 00 00       	mov    $0xe,%eax
8010042c:	89 fa                	mov    %edi,%edx
8010042e:	ee                   	out    %al,(%dx)
8010042f:	be d5 03 00 00       	mov    $0x3d5,%esi
80100434:	89 f2                	mov    %esi,%edx
80100436:	ec                   	in     (%dx),%al
80100437:	0f b6 c0             	movzbl %al,%eax
8010043a:	89 fa                	mov    %edi,%edx
8010043c:	c1 e0 08             	shl    $0x8,%eax
8010043f:	89 c1                	mov    %eax,%ecx
80100441:	b8 0f 00 00 00       	mov    $0xf,%eax
80100446:	ee                   	out    %al,(%dx)
80100447:	89 f2                	mov    %esi,%edx
80100449:	ec                   	in     (%dx),%al
8010044a:	0f b6 c0             	movzbl %al,%eax
8010044d:	09 c8                	or     %ecx,%eax
8010044f:	83 fb 0a             	cmp    $0xa,%ebx
80100452:	0f 84 0b 01 00 00    	je     80100563 <consputc+0x173>
80100458:	81 fb 00 01 00 00    	cmp    $0x100,%ebx
8010045e:	0f 84 e6 00 00 00    	je     8010054a <consputc+0x15a>
80100464:	0f b6 d3             	movzbl %bl,%edx
80100467:	8d 78 01             	lea    0x1(%eax),%edi
8010046a:	80 ce 07             	or     $0x7,%dh
8010046d:	66 89 94 00 00 80 0b 	mov    %dx,-0x7ff48000(%eax,%eax,1)
80100474:	80 
80100475:	81 ff d0 07 00 00    	cmp    $0x7d0,%edi
8010047b:	0f 8f bc 00 00 00    	jg     8010053d <consputc+0x14d>
80100481:	81 ff 7f 07 00 00    	cmp    $0x77f,%edi
80100487:	7f 6f                	jg     801004f8 <consputc+0x108>
80100489:	89 f8                	mov    %edi,%eax
8010048b:	8d 8c 3f 00 80 0b 80 	lea    -0x7ff48000(%edi,%edi,1),%ecx
80100492:	89 fb                	mov    %edi,%ebx
80100494:	c1 e8 08             	shr    $0x8,%eax
80100497:	89 c6                	mov    %eax,%esi
80100499:	bf d4 03 00 00       	mov    $0x3d4,%edi
8010049e:	b8 0e 00 00 00       	mov    $0xe,%eax
801004a3:	89 fa                	mov    %edi,%edx
801004a5:	ee                   	out    %al,(%dx)
801004a6:	ba d5 03 00 00       	mov    $0x3d5,%edx
801004ab:	89 f0                	mov    %esi,%eax
801004ad:	ee                   	out    %al,(%dx)
801004ae:	b8 0f 00 00 00       	mov    $0xf,%eax
801004b3:	89 fa                	mov    %edi,%edx
801004b5:	ee                   	out    %al,(%dx)
801004b6:	ba d5 03 00 00       	mov    $0x3d5,%edx
801004bb:	89 d8                	mov    %ebx,%eax
801004bd:	ee                   	out    %al,(%dx)
801004be:	b8 20 07 00 00       	mov    $0x720,%eax
801004c3:	66 89 01             	mov    %ax,(%ecx)
801004c6:	8d 65 f4             	lea    -0xc(%ebp),%esp
801004c9:	5b                   	pop    %ebx
801004ca:	5e                   	pop    %esi
801004cb:	5f                   	pop    %edi
801004cc:	5d                   	pop    %ebp
801004cd:	c3                   	ret    
801004ce:	83 ec 0c             	sub    $0xc,%esp
801004d1:	6a 08                	push   $0x8
801004d3:	e8 68 5b 00 00       	call   80106040 <uartputc>
801004d8:	c7 04 24 20 00 00 00 	movl   $0x20,(%esp)
801004df:	e8 5c 5b 00 00       	call   80106040 <uartputc>
801004e4:	c7 04 24 08 00 00 00 	movl   $0x8,(%esp)
801004eb:	e8 50 5b 00 00       	call   80106040 <uartputc>
801004f0:	83 c4 10             	add    $0x10,%esp
801004f3:	e9 2a ff ff ff       	jmp    80100422 <consputc+0x32>
801004f8:	83 ec 04             	sub    $0x4,%esp
801004fb:	8d 5f b0             	lea    -0x50(%edi),%ebx
801004fe:	68 60 0e 00 00       	push   $0xe60
80100503:	68 a0 80 0b 80       	push   $0x800b80a0
80100508:	68 00 80 0b 80       	push   $0x800b8000
8010050d:	8d b4 1b 00 80 0b 80 	lea    -0x7ff48000(%ebx,%ebx,1),%esi
80100514:	e8 77 44 00 00       	call   80104990 <memmove>
80100519:	b8 80 07 00 00       	mov    $0x780,%eax
8010051e:	83 c4 0c             	add    $0xc,%esp
80100521:	29 d8                	sub    %ebx,%eax
80100523:	01 c0                	add    %eax,%eax
80100525:	50                   	push   %eax
80100526:	6a 00                	push   $0x0
80100528:	56                   	push   %esi
80100529:	e8 b2 43 00 00       	call   801048e0 <memset>
8010052e:	89 f1                	mov    %esi,%ecx
80100530:	83 c4 10             	add    $0x10,%esp
80100533:	be 07 00 00 00       	mov    $0x7,%esi
80100538:	e9 5c ff ff ff       	jmp    80100499 <consputc+0xa9>
8010053d:	83 ec 0c             	sub    $0xc,%esp
80100540:	68 c5 74 10 80       	push   $0x801074c5
80100545:	e8 26 fe ff ff       	call   80100370 <panic>
8010054a:	85 c0                	test   %eax,%eax
8010054c:	8d 78 ff             	lea    -0x1(%eax),%edi
8010054f:	0f 85 20 ff ff ff    	jne    80100475 <consputc+0x85>
80100555:	b9 00 80 0b 80       	mov    $0x800b8000,%ecx
8010055a:	31 db                	xor    %ebx,%ebx
8010055c:	31 f6                	xor    %esi,%esi
8010055e:	e9 36 ff ff ff       	jmp    80100499 <consputc+0xa9>
80100563:	ba 67 66 66 66       	mov    $0x66666667,%edx
80100568:	f7 ea                	imul   %edx
8010056a:	89 d0                	mov    %edx,%eax
8010056c:	c1 e8 05             	shr    $0x5,%eax
8010056f:	8d 04 80             	lea    (%eax,%eax,4),%eax
80100572:	c1 e0 04             	shl    $0x4,%eax
80100575:	8d 78 50             	lea    0x50(%eax),%edi
80100578:	e9 f8 fe ff ff       	jmp    80100475 <consputc+0x85>
8010057d:	8d 76 00             	lea    0x0(%esi),%esi

80100580 <printint>:
80100580:	55                   	push   %ebp
80100581:	89 e5                	mov    %esp,%ebp
80100583:	57                   	push   %edi
80100584:	56                   	push   %esi
80100585:	53                   	push   %ebx
80100586:	89 d6                	mov    %edx,%esi
80100588:	83 ec 2c             	sub    $0x2c,%esp
8010058b:	85 c9                	test   %ecx,%ecx
8010058d:	89 4d d4             	mov    %ecx,-0x2c(%ebp)
80100590:	74 0c                	je     8010059e <printint+0x1e>
80100592:	89 c7                	mov    %eax,%edi
80100594:	c1 ef 1f             	shr    $0x1f,%edi
80100597:	85 c0                	test   %eax,%eax
80100599:	89 7d d4             	mov    %edi,-0x2c(%ebp)
8010059c:	78 51                	js     801005ef <printint+0x6f>
8010059e:	31 ff                	xor    %edi,%edi
801005a0:	8d 5d d7             	lea    -0x29(%ebp),%ebx
801005a3:	eb 05                	jmp    801005aa <printint+0x2a>
801005a5:	8d 76 00             	lea    0x0(%esi),%esi
801005a8:	89 cf                	mov    %ecx,%edi
801005aa:	31 d2                	xor    %edx,%edx
801005ac:	8d 4f 01             	lea    0x1(%edi),%ecx
801005af:	f7 f6                	div    %esi
801005b1:	0f b6 92 f0 74 10 80 	movzbl -0x7fef8b10(%edx),%edx
801005b8:	85 c0                	test   %eax,%eax
801005ba:	88 14 0b             	mov    %dl,(%ebx,%ecx,1)
801005bd:	75 e9                	jne    801005a8 <printint+0x28>
801005bf:	8b 45 d4             	mov    -0x2c(%ebp),%eax
801005c2:	85 c0                	test   %eax,%eax
801005c4:	74 08                	je     801005ce <printint+0x4e>
801005c6:	c6 44 0d d8 2d       	movb   $0x2d,-0x28(%ebp,%ecx,1)
801005cb:	8d 4f 02             	lea    0x2(%edi),%ecx
801005ce:	8d 74 0d d7          	lea    -0x29(%ebp,%ecx,1),%esi
801005d2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
801005d8:	0f be 06             	movsbl (%esi),%eax
801005db:	83 ee 01             	sub    $0x1,%esi
801005de:	e8 0d fe ff ff       	call   801003f0 <consputc>
801005e3:	39 de                	cmp    %ebx,%esi
801005e5:	75 f1                	jne    801005d8 <printint+0x58>
801005e7:	83 c4 2c             	add    $0x2c,%esp
801005ea:	5b                   	pop    %ebx
801005eb:	5e                   	pop    %esi
801005ec:	5f                   	pop    %edi
801005ed:	5d                   	pop    %ebp
801005ee:	c3                   	ret    
801005ef:	f7 d8                	neg    %eax
801005f1:	eb ab                	jmp    8010059e <printint+0x1e>
801005f3:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
801005f9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80100600 <consolewrite>:
80100600:	55                   	push   %ebp
80100601:	89 e5                	mov    %esp,%ebp
80100603:	57                   	push   %edi
80100604:	56                   	push   %esi
80100605:	53                   	push   %ebx
80100606:	83 ec 18             	sub    $0x18,%esp
80100609:	ff 75 08             	pushl  0x8(%ebp)
8010060c:	8b 75 10             	mov    0x10(%ebp),%esi
8010060f:	e8 3c 11 00 00       	call   80101750 <iunlock>
80100614:	c7 04 24 20 a5 10 80 	movl   $0x8010a520,(%esp)
8010061b:	e8 50 41 00 00       	call   80104770 <acquire>
80100620:	8b 7d 0c             	mov    0xc(%ebp),%edi
80100623:	83 c4 10             	add    $0x10,%esp
80100626:	85 f6                	test   %esi,%esi
80100628:	8d 1c 37             	lea    (%edi,%esi,1),%ebx
8010062b:	7e 12                	jle    8010063f <consolewrite+0x3f>
8010062d:	8d 76 00             	lea    0x0(%esi),%esi
80100630:	0f b6 07             	movzbl (%edi),%eax
80100633:	83 c7 01             	add    $0x1,%edi
80100636:	e8 b5 fd ff ff       	call   801003f0 <consputc>
8010063b:	39 df                	cmp    %ebx,%edi
8010063d:	75 f1                	jne    80100630 <consolewrite+0x30>
8010063f:	83 ec 0c             	sub    $0xc,%esp
80100642:	68 20 a5 10 80       	push   $0x8010a520
80100647:	e8 44 42 00 00       	call   80104890 <release>
8010064c:	58                   	pop    %eax
8010064d:	ff 75 08             	pushl  0x8(%ebp)
80100650:	e8 1b 10 00 00       	call   80101670 <ilock>
80100655:	8d 65 f4             	lea    -0xc(%ebp),%esp
80100658:	89 f0                	mov    %esi,%eax
8010065a:	5b                   	pop    %ebx
8010065b:	5e                   	pop    %esi
8010065c:	5f                   	pop    %edi
8010065d:	5d                   	pop    %ebp
8010065e:	c3                   	ret    
8010065f:	90                   	nop

80100660 <cprintf>:
80100660:	55                   	push   %ebp
80100661:	89 e5                	mov    %esp,%ebp
80100663:	57                   	push   %edi
80100664:	56                   	push   %esi
80100665:	53                   	push   %ebx
80100666:	83 ec 1c             	sub    $0x1c,%esp
80100669:	a1 54 a5 10 80       	mov    0x8010a554,%eax
8010066e:	85 c0                	test   %eax,%eax
80100670:	89 45 e0             	mov    %eax,-0x20(%ebp)
80100673:	0f 85 47 01 00 00    	jne    801007c0 <cprintf+0x160>
80100679:	8b 45 08             	mov    0x8(%ebp),%eax
8010067c:	85 c0                	test   %eax,%eax
8010067e:	89 c1                	mov    %eax,%ecx
80100680:	0f 84 4f 01 00 00    	je     801007d5 <cprintf+0x175>
80100686:	0f b6 00             	movzbl (%eax),%eax
80100689:	31 db                	xor    %ebx,%ebx
8010068b:	8d 75 0c             	lea    0xc(%ebp),%esi
8010068e:	89 cf                	mov    %ecx,%edi
80100690:	85 c0                	test   %eax,%eax
80100692:	75 55                	jne    801006e9 <cprintf+0x89>
80100694:	eb 68                	jmp    801006fe <cprintf+0x9e>
80100696:	8d 76 00             	lea    0x0(%esi),%esi
80100699:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
801006a0:	83 c3 01             	add    $0x1,%ebx
801006a3:	0f b6 14 1f          	movzbl (%edi,%ebx,1),%edx
801006a7:	85 d2                	test   %edx,%edx
801006a9:	74 53                	je     801006fe <cprintf+0x9e>
801006ab:	83 fa 70             	cmp    $0x70,%edx
801006ae:	74 7a                	je     8010072a <cprintf+0xca>
801006b0:	7f 6e                	jg     80100720 <cprintf+0xc0>
801006b2:	83 fa 25             	cmp    $0x25,%edx
801006b5:	0f 84 ad 00 00 00    	je     80100768 <cprintf+0x108>
801006bb:	83 fa 64             	cmp    $0x64,%edx
801006be:	0f 85 84 00 00 00    	jne    80100748 <cprintf+0xe8>
801006c4:	8d 46 04             	lea    0x4(%esi),%eax
801006c7:	b9 01 00 00 00       	mov    $0x1,%ecx
801006cc:	ba 0a 00 00 00       	mov    $0xa,%edx
801006d1:	89 45 e4             	mov    %eax,-0x1c(%ebp)
801006d4:	8b 06                	mov    (%esi),%eax
801006d6:	e8 a5 fe ff ff       	call   80100580 <printint>
801006db:	8b 75 e4             	mov    -0x1c(%ebp),%esi
801006de:	83 c3 01             	add    $0x1,%ebx
801006e1:	0f b6 04 1f          	movzbl (%edi,%ebx,1),%eax
801006e5:	85 c0                	test   %eax,%eax
801006e7:	74 15                	je     801006fe <cprintf+0x9e>
801006e9:	83 f8 25             	cmp    $0x25,%eax
801006ec:	74 b2                	je     801006a0 <cprintf+0x40>
801006ee:	e8 fd fc ff ff       	call   801003f0 <consputc>
801006f3:	83 c3 01             	add    $0x1,%ebx
801006f6:	0f b6 04 1f          	movzbl (%edi,%ebx,1),%eax
801006fa:	85 c0                	test   %eax,%eax
801006fc:	75 eb                	jne    801006e9 <cprintf+0x89>
801006fe:	8b 45 e0             	mov    -0x20(%ebp),%eax
80100701:	85 c0                	test   %eax,%eax
80100703:	74 10                	je     80100715 <cprintf+0xb5>
80100705:	83 ec 0c             	sub    $0xc,%esp
80100708:	68 20 a5 10 80       	push   $0x8010a520
8010070d:	e8 7e 41 00 00       	call   80104890 <release>
80100712:	83 c4 10             	add    $0x10,%esp
80100715:	8d 65 f4             	lea    -0xc(%ebp),%esp
80100718:	5b                   	pop    %ebx
80100719:	5e                   	pop    %esi
8010071a:	5f                   	pop    %edi
8010071b:	5d                   	pop    %ebp
8010071c:	c3                   	ret    
8010071d:	8d 76 00             	lea    0x0(%esi),%esi
80100720:	83 fa 73             	cmp    $0x73,%edx
80100723:	74 5b                	je     80100780 <cprintf+0x120>
80100725:	83 fa 78             	cmp    $0x78,%edx
80100728:	75 1e                	jne    80100748 <cprintf+0xe8>
8010072a:	8d 46 04             	lea    0x4(%esi),%eax
8010072d:	31 c9                	xor    %ecx,%ecx
8010072f:	ba 10 00 00 00       	mov    $0x10,%edx
80100734:	89 45 e4             	mov    %eax,-0x1c(%ebp)
80100737:	8b 06                	mov    (%esi),%eax
80100739:	e8 42 fe ff ff       	call   80100580 <printint>
8010073e:	8b 75 e4             	mov    -0x1c(%ebp),%esi
80100741:	eb 9b                	jmp    801006de <cprintf+0x7e>
80100743:	90                   	nop
80100744:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80100748:	b8 25 00 00 00       	mov    $0x25,%eax
8010074d:	89 55 e4             	mov    %edx,-0x1c(%ebp)
80100750:	e8 9b fc ff ff       	call   801003f0 <consputc>
80100755:	8b 55 e4             	mov    -0x1c(%ebp),%edx
80100758:	89 d0                	mov    %edx,%eax
8010075a:	e8 91 fc ff ff       	call   801003f0 <consputc>
8010075f:	e9 7a ff ff ff       	jmp    801006de <cprintf+0x7e>
80100764:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80100768:	b8 25 00 00 00       	mov    $0x25,%eax
8010076d:	e8 7e fc ff ff       	call   801003f0 <consputc>
80100772:	e9 7c ff ff ff       	jmp    801006f3 <cprintf+0x93>
80100777:	89 f6                	mov    %esi,%esi
80100779:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80100780:	8d 46 04             	lea    0x4(%esi),%eax
80100783:	8b 36                	mov    (%esi),%esi
80100785:	89 45 e4             	mov    %eax,-0x1c(%ebp)
80100788:	b8 d8 74 10 80       	mov    $0x801074d8,%eax
8010078d:	85 f6                	test   %esi,%esi
8010078f:	0f 44 f0             	cmove  %eax,%esi
80100792:	0f be 06             	movsbl (%esi),%eax
80100795:	84 c0                	test   %al,%al
80100797:	74 16                	je     801007af <cprintf+0x14f>
80100799:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801007a0:	83 c6 01             	add    $0x1,%esi
801007a3:	e8 48 fc ff ff       	call   801003f0 <consputc>
801007a8:	0f be 06             	movsbl (%esi),%eax
801007ab:	84 c0                	test   %al,%al
801007ad:	75 f1                	jne    801007a0 <cprintf+0x140>
801007af:	8b 75 e4             	mov    -0x1c(%ebp),%esi
801007b2:	e9 27 ff ff ff       	jmp    801006de <cprintf+0x7e>
801007b7:	89 f6                	mov    %esi,%esi
801007b9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
801007c0:	83 ec 0c             	sub    $0xc,%esp
801007c3:	68 20 a5 10 80       	push   $0x8010a520
801007c8:	e8 a3 3f 00 00       	call   80104770 <acquire>
801007cd:	83 c4 10             	add    $0x10,%esp
801007d0:	e9 a4 fe ff ff       	jmp    80100679 <cprintf+0x19>
801007d5:	83 ec 0c             	sub    $0xc,%esp
801007d8:	68 df 74 10 80       	push   $0x801074df
801007dd:	e8 8e fb ff ff       	call   80100370 <panic>
801007e2:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801007e9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801007f0 <consoleintr>:
801007f0:	55                   	push   %ebp
801007f1:	89 e5                	mov    %esp,%ebp
801007f3:	57                   	push   %edi
801007f4:	56                   	push   %esi
801007f5:	53                   	push   %ebx
801007f6:	31 f6                	xor    %esi,%esi
801007f8:	83 ec 18             	sub    $0x18,%esp
801007fb:	8b 5d 08             	mov    0x8(%ebp),%ebx
801007fe:	68 20 a5 10 80       	push   $0x8010a520
80100803:	e8 68 3f 00 00       	call   80104770 <acquire>
80100808:	83 c4 10             	add    $0x10,%esp
8010080b:	90                   	nop
8010080c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80100810:	ff d3                	call   *%ebx
80100812:	85 c0                	test   %eax,%eax
80100814:	89 c7                	mov    %eax,%edi
80100816:	78 48                	js     80100860 <consoleintr+0x70>
80100818:	83 ff 10             	cmp    $0x10,%edi
8010081b:	0f 84 3f 01 00 00    	je     80100960 <consoleintr+0x170>
80100821:	7e 5d                	jle    80100880 <consoleintr+0x90>
80100823:	83 ff 15             	cmp    $0x15,%edi
80100826:	0f 84 dc 00 00 00    	je     80100908 <consoleintr+0x118>
8010082c:	83 ff 7f             	cmp    $0x7f,%edi
8010082f:	75 54                	jne    80100885 <consoleintr+0x95>
80100831:	a1 a8 ff 10 80       	mov    0x8010ffa8,%eax
80100836:	3b 05 a4 ff 10 80    	cmp    0x8010ffa4,%eax
8010083c:	74 d2                	je     80100810 <consoleintr+0x20>
8010083e:	83 e8 01             	sub    $0x1,%eax
80100841:	a3 a8 ff 10 80       	mov    %eax,0x8010ffa8
80100846:	b8 00 01 00 00       	mov    $0x100,%eax
8010084b:	e8 a0 fb ff ff       	call   801003f0 <consputc>
80100850:	ff d3                	call   *%ebx
80100852:	85 c0                	test   %eax,%eax
80100854:	89 c7                	mov    %eax,%edi
80100856:	79 c0                	jns    80100818 <consoleintr+0x28>
80100858:	90                   	nop
80100859:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80100860:	83 ec 0c             	sub    $0xc,%esp
80100863:	68 20 a5 10 80       	push   $0x8010a520
80100868:	e8 23 40 00 00       	call   80104890 <release>
8010086d:	83 c4 10             	add    $0x10,%esp
80100870:	85 f6                	test   %esi,%esi
80100872:	0f 85 f8 00 00 00    	jne    80100970 <consoleintr+0x180>
80100878:	8d 65 f4             	lea    -0xc(%ebp),%esp
8010087b:	5b                   	pop    %ebx
8010087c:	5e                   	pop    %esi
8010087d:	5f                   	pop    %edi
8010087e:	5d                   	pop    %ebp
8010087f:	c3                   	ret    
80100880:	83 ff 08             	cmp    $0x8,%edi
80100883:	74 ac                	je     80100831 <consoleintr+0x41>
80100885:	85 ff                	test   %edi,%edi
80100887:	74 87                	je     80100810 <consoleintr+0x20>
80100889:	a1 a8 ff 10 80       	mov    0x8010ffa8,%eax
8010088e:	89 c2                	mov    %eax,%edx
80100890:	2b 15 a0 ff 10 80    	sub    0x8010ffa0,%edx
80100896:	83 fa 7f             	cmp    $0x7f,%edx
80100899:	0f 87 71 ff ff ff    	ja     80100810 <consoleintr+0x20>
8010089f:	8d 50 01             	lea    0x1(%eax),%edx
801008a2:	83 e0 7f             	and    $0x7f,%eax
801008a5:	83 ff 0d             	cmp    $0xd,%edi
801008a8:	89 15 a8 ff 10 80    	mov    %edx,0x8010ffa8
801008ae:	0f 84 c8 00 00 00    	je     8010097c <consoleintr+0x18c>
801008b4:	89 f9                	mov    %edi,%ecx
801008b6:	88 88 20 ff 10 80    	mov    %cl,-0x7fef00e0(%eax)
801008bc:	89 f8                	mov    %edi,%eax
801008be:	e8 2d fb ff ff       	call   801003f0 <consputc>
801008c3:	83 ff 0a             	cmp    $0xa,%edi
801008c6:	0f 84 c1 00 00 00    	je     8010098d <consoleintr+0x19d>
801008cc:	83 ff 04             	cmp    $0x4,%edi
801008cf:	0f 84 b8 00 00 00    	je     8010098d <consoleintr+0x19d>
801008d5:	a1 a0 ff 10 80       	mov    0x8010ffa0,%eax
801008da:	83 e8 80             	sub    $0xffffff80,%eax
801008dd:	39 05 a8 ff 10 80    	cmp    %eax,0x8010ffa8
801008e3:	0f 85 27 ff ff ff    	jne    80100810 <consoleintr+0x20>
801008e9:	83 ec 0c             	sub    $0xc,%esp
801008ec:	a3 a4 ff 10 80       	mov    %eax,0x8010ffa4
801008f1:	68 a0 ff 10 80       	push   $0x8010ffa0
801008f6:	e8 65 38 00 00       	call   80104160 <wakeup>
801008fb:	83 c4 10             	add    $0x10,%esp
801008fe:	e9 0d ff ff ff       	jmp    80100810 <consoleintr+0x20>
80100903:	90                   	nop
80100904:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80100908:	a1 a8 ff 10 80       	mov    0x8010ffa8,%eax
8010090d:	39 05 a4 ff 10 80    	cmp    %eax,0x8010ffa4
80100913:	75 2b                	jne    80100940 <consoleintr+0x150>
80100915:	e9 f6 fe ff ff       	jmp    80100810 <consoleintr+0x20>
8010091a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80100920:	a3 a8 ff 10 80       	mov    %eax,0x8010ffa8
80100925:	b8 00 01 00 00       	mov    $0x100,%eax
8010092a:	e8 c1 fa ff ff       	call   801003f0 <consputc>
8010092f:	a1 a8 ff 10 80       	mov    0x8010ffa8,%eax
80100934:	3b 05 a4 ff 10 80    	cmp    0x8010ffa4,%eax
8010093a:	0f 84 d0 fe ff ff    	je     80100810 <consoleintr+0x20>
80100940:	83 e8 01             	sub    $0x1,%eax
80100943:	89 c2                	mov    %eax,%edx
80100945:	83 e2 7f             	and    $0x7f,%edx
80100948:	80 ba 20 ff 10 80 0a 	cmpb   $0xa,-0x7fef00e0(%edx)
8010094f:	75 cf                	jne    80100920 <consoleintr+0x130>
80100951:	e9 ba fe ff ff       	jmp    80100810 <consoleintr+0x20>
80100956:	8d 76 00             	lea    0x0(%esi),%esi
80100959:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80100960:	be 01 00 00 00       	mov    $0x1,%esi
80100965:	e9 a6 fe ff ff       	jmp    80100810 <consoleintr+0x20>
8010096a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80100970:	8d 65 f4             	lea    -0xc(%ebp),%esp
80100973:	5b                   	pop    %ebx
80100974:	5e                   	pop    %esi
80100975:	5f                   	pop    %edi
80100976:	5d                   	pop    %ebp
80100977:	e9 d4 38 00 00       	jmp    80104250 <procdump>
8010097c:	c6 80 20 ff 10 80 0a 	movb   $0xa,-0x7fef00e0(%eax)
80100983:	b8 0a 00 00 00       	mov    $0xa,%eax
80100988:	e8 63 fa ff ff       	call   801003f0 <consputc>
8010098d:	a1 a8 ff 10 80       	mov    0x8010ffa8,%eax
80100992:	e9 52 ff ff ff       	jmp    801008e9 <consoleintr+0xf9>
80100997:	89 f6                	mov    %esi,%esi
80100999:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801009a0 <consoleinit>:
801009a0:	55                   	push   %ebp
801009a1:	89 e5                	mov    %esp,%ebp
801009a3:	83 ec 10             	sub    $0x10,%esp
801009a6:	68 e8 74 10 80       	push   $0x801074e8
801009ab:	68 20 a5 10 80       	push   $0x8010a520
801009b0:	e8 bb 3c 00 00       	call   80104670 <initlock>
801009b5:	58                   	pop    %eax
801009b6:	5a                   	pop    %edx
801009b7:	6a 00                	push   $0x0
801009b9:	6a 01                	push   $0x1
801009bb:	c7 05 6c 09 11 80 00 	movl   $0x80100600,0x8011096c
801009c2:	06 10 80 
801009c5:	c7 05 68 09 11 80 70 	movl   $0x80100270,0x80110968
801009cc:	02 10 80 
801009cf:	c7 05 54 a5 10 80 01 	movl   $0x1,0x8010a554
801009d6:	00 00 00 
801009d9:	e8 c2 18 00 00       	call   801022a0 <ioapicenable>
801009de:	83 c4 10             	add    $0x10,%esp
801009e1:	c9                   	leave  
801009e2:	c3                   	ret    
801009e3:	66 90                	xchg   %ax,%ax
801009e5:	66 90                	xchg   %ax,%ax
801009e7:	66 90                	xchg   %ax,%ax
801009e9:	66 90                	xchg   %ax,%ax
801009eb:	66 90                	xchg   %ax,%ax
801009ed:	66 90                	xchg   %ax,%ax
801009ef:	90                   	nop

801009f0 <exec>:
801009f0:	55                   	push   %ebp
801009f1:	89 e5                	mov    %esp,%ebp
801009f3:	57                   	push   %edi
801009f4:	56                   	push   %esi
801009f5:	53                   	push   %ebx
801009f6:	81 ec 0c 01 00 00    	sub    $0x10c,%esp
801009fc:	e8 6f 2e 00 00       	call   80103870 <myproc>
80100a01:	89 85 f4 fe ff ff    	mov    %eax,-0x10c(%ebp)
80100a07:	e8 44 21 00 00       	call   80102b50 <begin_op>
80100a0c:	83 ec 0c             	sub    $0xc,%esp
80100a0f:	ff 75 08             	pushl  0x8(%ebp)
80100a12:	e8 a9 14 00 00       	call   80101ec0 <namei>
80100a17:	83 c4 10             	add    $0x10,%esp
80100a1a:	85 c0                	test   %eax,%eax
80100a1c:	0f 84 9c 01 00 00    	je     80100bbe <exec+0x1ce>
80100a22:	83 ec 0c             	sub    $0xc,%esp
80100a25:	89 c3                	mov    %eax,%ebx
80100a27:	50                   	push   %eax
80100a28:	e8 43 0c 00 00       	call   80101670 <ilock>
80100a2d:	8d 85 24 ff ff ff    	lea    -0xdc(%ebp),%eax
80100a33:	6a 34                	push   $0x34
80100a35:	6a 00                	push   $0x0
80100a37:	50                   	push   %eax
80100a38:	53                   	push   %ebx
80100a39:	e8 12 0f 00 00       	call   80101950 <readi>
80100a3e:	83 c4 20             	add    $0x20,%esp
80100a41:	83 f8 34             	cmp    $0x34,%eax
80100a44:	74 22                	je     80100a68 <exec+0x78>
80100a46:	83 ec 0c             	sub    $0xc,%esp
80100a49:	53                   	push   %ebx
80100a4a:	e8 b1 0e 00 00       	call   80101900 <iunlockput>
80100a4f:	e8 6c 21 00 00       	call   80102bc0 <end_op>
80100a54:	83 c4 10             	add    $0x10,%esp
80100a57:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80100a5c:	8d 65 f4             	lea    -0xc(%ebp),%esp
80100a5f:	5b                   	pop    %ebx
80100a60:	5e                   	pop    %esi
80100a61:	5f                   	pop    %edi
80100a62:	5d                   	pop    %ebp
80100a63:	c3                   	ret    
80100a64:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80100a68:	81 bd 24 ff ff ff 7f 	cmpl   $0x464c457f,-0xdc(%ebp)
80100a6f:	45 4c 46 
80100a72:	75 d2                	jne    80100a46 <exec+0x56>
80100a74:	e8 57 67 00 00       	call   801071d0 <setupkvm>
80100a79:	85 c0                	test   %eax,%eax
80100a7b:	89 85 f0 fe ff ff    	mov    %eax,-0x110(%ebp)
80100a81:	74 c3                	je     80100a46 <exec+0x56>
80100a83:	66 83 bd 50 ff ff ff 	cmpw   $0x0,-0xb0(%ebp)
80100a8a:	00 
80100a8b:	8b b5 40 ff ff ff    	mov    -0xc0(%ebp),%esi
80100a91:	c7 85 ec fe ff ff 00 	movl   $0x0,-0x114(%ebp)
80100a98:	00 00 00 
80100a9b:	0f 84 c5 00 00 00    	je     80100b66 <exec+0x176>
80100aa1:	31 ff                	xor    %edi,%edi
80100aa3:	eb 18                	jmp    80100abd <exec+0xcd>
80100aa5:	8d 76 00             	lea    0x0(%esi),%esi
80100aa8:	0f b7 85 50 ff ff ff 	movzwl -0xb0(%ebp),%eax
80100aaf:	83 c7 01             	add    $0x1,%edi
80100ab2:	83 c6 20             	add    $0x20,%esi
80100ab5:	39 f8                	cmp    %edi,%eax
80100ab7:	0f 8e a9 00 00 00    	jle    80100b66 <exec+0x176>
80100abd:	8d 85 04 ff ff ff    	lea    -0xfc(%ebp),%eax
80100ac3:	6a 20                	push   $0x20
80100ac5:	56                   	push   %esi
80100ac6:	50                   	push   %eax
80100ac7:	53                   	push   %ebx
80100ac8:	e8 83 0e 00 00       	call   80101950 <readi>
80100acd:	83 c4 10             	add    $0x10,%esp
80100ad0:	83 f8 20             	cmp    $0x20,%eax
80100ad3:	75 7b                	jne    80100b50 <exec+0x160>
80100ad5:	83 bd 04 ff ff ff 01 	cmpl   $0x1,-0xfc(%ebp)
80100adc:	75 ca                	jne    80100aa8 <exec+0xb8>
80100ade:	8b 85 18 ff ff ff    	mov    -0xe8(%ebp),%eax
80100ae4:	3b 85 14 ff ff ff    	cmp    -0xec(%ebp),%eax
80100aea:	72 64                	jb     80100b50 <exec+0x160>
80100aec:	03 85 0c ff ff ff    	add    -0xf4(%ebp),%eax
80100af2:	72 5c                	jb     80100b50 <exec+0x160>
80100af4:	83 ec 04             	sub    $0x4,%esp
80100af7:	50                   	push   %eax
80100af8:	ff b5 ec fe ff ff    	pushl  -0x114(%ebp)
80100afe:	ff b5 f0 fe ff ff    	pushl  -0x110(%ebp)
80100b04:	e8 17 65 00 00       	call   80107020 <allocuvm>
80100b09:	83 c4 10             	add    $0x10,%esp
80100b0c:	85 c0                	test   %eax,%eax
80100b0e:	89 85 ec fe ff ff    	mov    %eax,-0x114(%ebp)
80100b14:	74 3a                	je     80100b50 <exec+0x160>
80100b16:	8b 85 0c ff ff ff    	mov    -0xf4(%ebp),%eax
80100b1c:	a9 ff 0f 00 00       	test   $0xfff,%eax
80100b21:	75 2d                	jne    80100b50 <exec+0x160>
80100b23:	83 ec 0c             	sub    $0xc,%esp
80100b26:	ff b5 14 ff ff ff    	pushl  -0xec(%ebp)
80100b2c:	ff b5 08 ff ff ff    	pushl  -0xf8(%ebp)
80100b32:	53                   	push   %ebx
80100b33:	50                   	push   %eax
80100b34:	ff b5 f0 fe ff ff    	pushl  -0x110(%ebp)
80100b3a:	e8 21 64 00 00       	call   80106f60 <loaduvm>
80100b3f:	83 c4 20             	add    $0x20,%esp
80100b42:	85 c0                	test   %eax,%eax
80100b44:	0f 89 5e ff ff ff    	jns    80100aa8 <exec+0xb8>
80100b4a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80100b50:	83 ec 0c             	sub    $0xc,%esp
80100b53:	ff b5 f0 fe ff ff    	pushl  -0x110(%ebp)
80100b59:	e8 f2 65 00 00       	call   80107150 <freevm>
80100b5e:	83 c4 10             	add    $0x10,%esp
80100b61:	e9 e0 fe ff ff       	jmp    80100a46 <exec+0x56>
80100b66:	83 ec 0c             	sub    $0xc,%esp
80100b69:	53                   	push   %ebx
80100b6a:	e8 91 0d 00 00       	call   80101900 <iunlockput>
80100b6f:	e8 4c 20 00 00       	call   80102bc0 <end_op>
80100b74:	8b 85 ec fe ff ff    	mov    -0x114(%ebp),%eax
80100b7a:	83 c4 0c             	add    $0xc,%esp
80100b7d:	05 ff 0f 00 00       	add    $0xfff,%eax
80100b82:	25 00 f0 ff ff       	and    $0xfffff000,%eax
80100b87:	8d 90 00 20 00 00    	lea    0x2000(%eax),%edx
80100b8d:	52                   	push   %edx
80100b8e:	50                   	push   %eax
80100b8f:	ff b5 f0 fe ff ff    	pushl  -0x110(%ebp)
80100b95:	e8 86 64 00 00       	call   80107020 <allocuvm>
80100b9a:	83 c4 10             	add    $0x10,%esp
80100b9d:	85 c0                	test   %eax,%eax
80100b9f:	89 c6                	mov    %eax,%esi
80100ba1:	75 3a                	jne    80100bdd <exec+0x1ed>
80100ba3:	83 ec 0c             	sub    $0xc,%esp
80100ba6:	ff b5 f0 fe ff ff    	pushl  -0x110(%ebp)
80100bac:	e8 9f 65 00 00       	call   80107150 <freevm>
80100bb1:	83 c4 10             	add    $0x10,%esp
80100bb4:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80100bb9:	e9 9e fe ff ff       	jmp    80100a5c <exec+0x6c>
80100bbe:	e8 fd 1f 00 00       	call   80102bc0 <end_op>
80100bc3:	83 ec 0c             	sub    $0xc,%esp
80100bc6:	68 01 75 10 80       	push   $0x80107501
80100bcb:	e8 90 fa ff ff       	call   80100660 <cprintf>
80100bd0:	83 c4 10             	add    $0x10,%esp
80100bd3:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80100bd8:	e9 7f fe ff ff       	jmp    80100a5c <exec+0x6c>
80100bdd:	8d 80 00 e0 ff ff    	lea    -0x2000(%eax),%eax
80100be3:	83 ec 08             	sub    $0x8,%esp
80100be6:	31 ff                	xor    %edi,%edi
80100be8:	89 f3                	mov    %esi,%ebx
80100bea:	50                   	push   %eax
80100beb:	ff b5 f0 fe ff ff    	pushl  -0x110(%ebp)
80100bf1:	e8 7a 66 00 00       	call   80107270 <clearpteu>
80100bf6:	8b 45 0c             	mov    0xc(%ebp),%eax
80100bf9:	83 c4 10             	add    $0x10,%esp
80100bfc:	8d 95 58 ff ff ff    	lea    -0xa8(%ebp),%edx
80100c02:	8b 00                	mov    (%eax),%eax
80100c04:	85 c0                	test   %eax,%eax
80100c06:	74 79                	je     80100c81 <exec+0x291>
80100c08:	89 b5 ec fe ff ff    	mov    %esi,-0x114(%ebp)
80100c0e:	8b b5 f0 fe ff ff    	mov    -0x110(%ebp),%esi
80100c14:	eb 13                	jmp    80100c29 <exec+0x239>
80100c16:	8d 76 00             	lea    0x0(%esi),%esi
80100c19:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80100c20:	83 ff 20             	cmp    $0x20,%edi
80100c23:	0f 84 7a ff ff ff    	je     80100ba3 <exec+0x1b3>
80100c29:	83 ec 0c             	sub    $0xc,%esp
80100c2c:	50                   	push   %eax
80100c2d:	e8 ee 3e 00 00       	call   80104b20 <strlen>
80100c32:	f7 d0                	not    %eax
80100c34:	01 c3                	add    %eax,%ebx
80100c36:	8b 45 0c             	mov    0xc(%ebp),%eax
80100c39:	5a                   	pop    %edx
80100c3a:	83 e3 fc             	and    $0xfffffffc,%ebx
80100c3d:	ff 34 b8             	pushl  (%eax,%edi,4)
80100c40:	e8 db 3e 00 00       	call   80104b20 <strlen>
80100c45:	83 c0 01             	add    $0x1,%eax
80100c48:	50                   	push   %eax
80100c49:	8b 45 0c             	mov    0xc(%ebp),%eax
80100c4c:	ff 34 b8             	pushl  (%eax,%edi,4)
80100c4f:	53                   	push   %ebx
80100c50:	56                   	push   %esi
80100c51:	e8 7a 67 00 00       	call   801073d0 <copyout>
80100c56:	83 c4 20             	add    $0x20,%esp
80100c59:	85 c0                	test   %eax,%eax
80100c5b:	0f 88 42 ff ff ff    	js     80100ba3 <exec+0x1b3>
80100c61:	8b 45 0c             	mov    0xc(%ebp),%eax
80100c64:	89 9c bd 64 ff ff ff 	mov    %ebx,-0x9c(%ebp,%edi,4)
80100c6b:	83 c7 01             	add    $0x1,%edi
80100c6e:	8d 95 58 ff ff ff    	lea    -0xa8(%ebp),%edx
80100c74:	8b 04 b8             	mov    (%eax,%edi,4),%eax
80100c77:	85 c0                	test   %eax,%eax
80100c79:	75 a5                	jne    80100c20 <exec+0x230>
80100c7b:	8b b5 ec fe ff ff    	mov    -0x114(%ebp),%esi
80100c81:	8d 04 bd 04 00 00 00 	lea    0x4(,%edi,4),%eax
80100c88:	89 d9                	mov    %ebx,%ecx
80100c8a:	c7 84 bd 64 ff ff ff 	movl   $0x0,-0x9c(%ebp,%edi,4)
80100c91:	00 00 00 00 
80100c95:	c7 85 58 ff ff ff ff 	movl   $0xffffffff,-0xa8(%ebp)
80100c9c:	ff ff ff 
80100c9f:	89 bd 5c ff ff ff    	mov    %edi,-0xa4(%ebp)
80100ca5:	29 c1                	sub    %eax,%ecx
80100ca7:	83 c0 0c             	add    $0xc,%eax
80100caa:	29 c3                	sub    %eax,%ebx
80100cac:	50                   	push   %eax
80100cad:	52                   	push   %edx
80100cae:	53                   	push   %ebx
80100caf:	ff b5 f0 fe ff ff    	pushl  -0x110(%ebp)
80100cb5:	89 8d 60 ff ff ff    	mov    %ecx,-0xa0(%ebp)
80100cbb:	e8 10 67 00 00       	call   801073d0 <copyout>
80100cc0:	83 c4 10             	add    $0x10,%esp
80100cc3:	85 c0                	test   %eax,%eax
80100cc5:	0f 88 d8 fe ff ff    	js     80100ba3 <exec+0x1b3>
80100ccb:	8b 45 08             	mov    0x8(%ebp),%eax
80100cce:	0f b6 10             	movzbl (%eax),%edx
80100cd1:	84 d2                	test   %dl,%dl
80100cd3:	74 19                	je     80100cee <exec+0x2fe>
80100cd5:	8b 4d 08             	mov    0x8(%ebp),%ecx
80100cd8:	83 c0 01             	add    $0x1,%eax
80100cdb:	80 fa 2f             	cmp    $0x2f,%dl
80100cde:	0f b6 10             	movzbl (%eax),%edx
80100ce1:	0f 44 c8             	cmove  %eax,%ecx
80100ce4:	83 c0 01             	add    $0x1,%eax
80100ce7:	84 d2                	test   %dl,%dl
80100ce9:	75 f0                	jne    80100cdb <exec+0x2eb>
80100ceb:	89 4d 08             	mov    %ecx,0x8(%ebp)
80100cee:	8b bd f4 fe ff ff    	mov    -0x10c(%ebp),%edi
80100cf4:	50                   	push   %eax
80100cf5:	6a 10                	push   $0x10
80100cf7:	ff 75 08             	pushl  0x8(%ebp)
80100cfa:	89 f8                	mov    %edi,%eax
80100cfc:	83 c0 6c             	add    $0x6c,%eax
80100cff:	50                   	push   %eax
80100d00:	e8 db 3d 00 00       	call   80104ae0 <safestrcpy>
80100d05:	8b 8d f0 fe ff ff    	mov    -0x110(%ebp),%ecx
80100d0b:	89 f8                	mov    %edi,%eax
80100d0d:	8b 7f 04             	mov    0x4(%edi),%edi
80100d10:	89 30                	mov    %esi,(%eax)
80100d12:	89 48 04             	mov    %ecx,0x4(%eax)
80100d15:	89 c1                	mov    %eax,%ecx
80100d17:	8b 95 3c ff ff ff    	mov    -0xc4(%ebp),%edx
80100d1d:	8b 40 18             	mov    0x18(%eax),%eax
80100d20:	89 50 38             	mov    %edx,0x38(%eax)
80100d23:	8b 41 18             	mov    0x18(%ecx),%eax
80100d26:	89 58 44             	mov    %ebx,0x44(%eax)
80100d29:	89 0c 24             	mov    %ecx,(%esp)
80100d2c:	e8 9f 60 00 00       	call   80106dd0 <switchuvm>
80100d31:	89 3c 24             	mov    %edi,(%esp)
80100d34:	e8 17 64 00 00       	call   80107150 <freevm>
80100d39:	83 c4 10             	add    $0x10,%esp
80100d3c:	31 c0                	xor    %eax,%eax
80100d3e:	e9 19 fd ff ff       	jmp    80100a5c <exec+0x6c>
80100d43:	66 90                	xchg   %ax,%ax
80100d45:	66 90                	xchg   %ax,%ax
80100d47:	66 90                	xchg   %ax,%ax
80100d49:	66 90                	xchg   %ax,%ax
80100d4b:	66 90                	xchg   %ax,%ax
80100d4d:	66 90                	xchg   %ax,%ax
80100d4f:	90                   	nop

80100d50 <fileinit>:
80100d50:	55                   	push   %ebp
80100d51:	89 e5                	mov    %esp,%ebp
80100d53:	83 ec 10             	sub    $0x10,%esp
80100d56:	68 0d 75 10 80       	push   $0x8010750d
80100d5b:	68 c0 ff 10 80       	push   $0x8010ffc0
80100d60:	e8 0b 39 00 00       	call   80104670 <initlock>
80100d65:	83 c4 10             	add    $0x10,%esp
80100d68:	c9                   	leave  
80100d69:	c3                   	ret    
80100d6a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

80100d70 <filealloc>:
80100d70:	55                   	push   %ebp
80100d71:	89 e5                	mov    %esp,%ebp
80100d73:	53                   	push   %ebx
80100d74:	bb f4 ff 10 80       	mov    $0x8010fff4,%ebx
80100d79:	83 ec 10             	sub    $0x10,%esp
80100d7c:	68 c0 ff 10 80       	push   $0x8010ffc0
80100d81:	e8 ea 39 00 00       	call   80104770 <acquire>
80100d86:	83 c4 10             	add    $0x10,%esp
80100d89:	eb 10                	jmp    80100d9b <filealloc+0x2b>
80100d8b:	90                   	nop
80100d8c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80100d90:	83 c3 18             	add    $0x18,%ebx
80100d93:	81 fb 54 09 11 80    	cmp    $0x80110954,%ebx
80100d99:	74 25                	je     80100dc0 <filealloc+0x50>
80100d9b:	8b 43 04             	mov    0x4(%ebx),%eax
80100d9e:	85 c0                	test   %eax,%eax
80100da0:	75 ee                	jne    80100d90 <filealloc+0x20>
80100da2:	83 ec 0c             	sub    $0xc,%esp
80100da5:	c7 43 04 01 00 00 00 	movl   $0x1,0x4(%ebx)
80100dac:	68 c0 ff 10 80       	push   $0x8010ffc0
80100db1:	e8 da 3a 00 00       	call   80104890 <release>
80100db6:	89 d8                	mov    %ebx,%eax
80100db8:	83 c4 10             	add    $0x10,%esp
80100dbb:	8b 5d fc             	mov    -0x4(%ebp),%ebx
80100dbe:	c9                   	leave  
80100dbf:	c3                   	ret    
80100dc0:	83 ec 0c             	sub    $0xc,%esp
80100dc3:	68 c0 ff 10 80       	push   $0x8010ffc0
80100dc8:	e8 c3 3a 00 00       	call   80104890 <release>
80100dcd:	83 c4 10             	add    $0x10,%esp
80100dd0:	31 c0                	xor    %eax,%eax
80100dd2:	8b 5d fc             	mov    -0x4(%ebp),%ebx
80100dd5:	c9                   	leave  
80100dd6:	c3                   	ret    
80100dd7:	89 f6                	mov    %esi,%esi
80100dd9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80100de0 <filedup>:
80100de0:	55                   	push   %ebp
80100de1:	89 e5                	mov    %esp,%ebp
80100de3:	53                   	push   %ebx
80100de4:	83 ec 10             	sub    $0x10,%esp
80100de7:	8b 5d 08             	mov    0x8(%ebp),%ebx
80100dea:	68 c0 ff 10 80       	push   $0x8010ffc0
80100def:	e8 7c 39 00 00       	call   80104770 <acquire>
80100df4:	8b 43 04             	mov    0x4(%ebx),%eax
80100df7:	83 c4 10             	add    $0x10,%esp
80100dfa:	85 c0                	test   %eax,%eax
80100dfc:	7e 1a                	jle    80100e18 <filedup+0x38>
80100dfe:	83 c0 01             	add    $0x1,%eax
80100e01:	83 ec 0c             	sub    $0xc,%esp
80100e04:	89 43 04             	mov    %eax,0x4(%ebx)
80100e07:	68 c0 ff 10 80       	push   $0x8010ffc0
80100e0c:	e8 7f 3a 00 00       	call   80104890 <release>
80100e11:	89 d8                	mov    %ebx,%eax
80100e13:	8b 5d fc             	mov    -0x4(%ebp),%ebx
80100e16:	c9                   	leave  
80100e17:	c3                   	ret    
80100e18:	83 ec 0c             	sub    $0xc,%esp
80100e1b:	68 14 75 10 80       	push   $0x80107514
80100e20:	e8 4b f5 ff ff       	call   80100370 <panic>
80100e25:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80100e29:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80100e30 <fileclose>:
80100e30:	55                   	push   %ebp
80100e31:	89 e5                	mov    %esp,%ebp
80100e33:	57                   	push   %edi
80100e34:	56                   	push   %esi
80100e35:	53                   	push   %ebx
80100e36:	83 ec 28             	sub    $0x28,%esp
80100e39:	8b 7d 08             	mov    0x8(%ebp),%edi
80100e3c:	68 c0 ff 10 80       	push   $0x8010ffc0
80100e41:	e8 2a 39 00 00       	call   80104770 <acquire>
80100e46:	8b 47 04             	mov    0x4(%edi),%eax
80100e49:	83 c4 10             	add    $0x10,%esp
80100e4c:	85 c0                	test   %eax,%eax
80100e4e:	0f 8e 9b 00 00 00    	jle    80100eef <fileclose+0xbf>
80100e54:	83 e8 01             	sub    $0x1,%eax
80100e57:	85 c0                	test   %eax,%eax
80100e59:	89 47 04             	mov    %eax,0x4(%edi)
80100e5c:	74 1a                	je     80100e78 <fileclose+0x48>
80100e5e:	c7 45 08 c0 ff 10 80 	movl   $0x8010ffc0,0x8(%ebp)
80100e65:	8d 65 f4             	lea    -0xc(%ebp),%esp
80100e68:	5b                   	pop    %ebx
80100e69:	5e                   	pop    %esi
80100e6a:	5f                   	pop    %edi
80100e6b:	5d                   	pop    %ebp
80100e6c:	e9 1f 3a 00 00       	jmp    80104890 <release>
80100e71:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80100e78:	0f b6 47 09          	movzbl 0x9(%edi),%eax
80100e7c:	8b 1f                	mov    (%edi),%ebx
80100e7e:	83 ec 0c             	sub    $0xc,%esp
80100e81:	8b 77 0c             	mov    0xc(%edi),%esi
80100e84:	c7 07 00 00 00 00    	movl   $0x0,(%edi)
80100e8a:	88 45 e7             	mov    %al,-0x19(%ebp)
80100e8d:	8b 47 10             	mov    0x10(%edi),%eax
80100e90:	68 c0 ff 10 80       	push   $0x8010ffc0
80100e95:	89 45 e0             	mov    %eax,-0x20(%ebp)
80100e98:	e8 f3 39 00 00       	call   80104890 <release>
80100e9d:	83 c4 10             	add    $0x10,%esp
80100ea0:	83 fb 01             	cmp    $0x1,%ebx
80100ea3:	74 13                	je     80100eb8 <fileclose+0x88>
80100ea5:	83 fb 02             	cmp    $0x2,%ebx
80100ea8:	74 26                	je     80100ed0 <fileclose+0xa0>
80100eaa:	8d 65 f4             	lea    -0xc(%ebp),%esp
80100ead:	5b                   	pop    %ebx
80100eae:	5e                   	pop    %esi
80100eaf:	5f                   	pop    %edi
80100eb0:	5d                   	pop    %ebp
80100eb1:	c3                   	ret    
80100eb2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80100eb8:	0f be 5d e7          	movsbl -0x19(%ebp),%ebx
80100ebc:	83 ec 08             	sub    $0x8,%esp
80100ebf:	53                   	push   %ebx
80100ec0:	56                   	push   %esi
80100ec1:	e8 2a 24 00 00       	call   801032f0 <pipeclose>
80100ec6:	83 c4 10             	add    $0x10,%esp
80100ec9:	eb df                	jmp    80100eaa <fileclose+0x7a>
80100ecb:	90                   	nop
80100ecc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80100ed0:	e8 7b 1c 00 00       	call   80102b50 <begin_op>
80100ed5:	83 ec 0c             	sub    $0xc,%esp
80100ed8:	ff 75 e0             	pushl  -0x20(%ebp)
80100edb:	e8 c0 08 00 00       	call   801017a0 <iput>
80100ee0:	83 c4 10             	add    $0x10,%esp
80100ee3:	8d 65 f4             	lea    -0xc(%ebp),%esp
80100ee6:	5b                   	pop    %ebx
80100ee7:	5e                   	pop    %esi
80100ee8:	5f                   	pop    %edi
80100ee9:	5d                   	pop    %ebp
80100eea:	e9 d1 1c 00 00       	jmp    80102bc0 <end_op>
80100eef:	83 ec 0c             	sub    $0xc,%esp
80100ef2:	68 1c 75 10 80       	push   $0x8010751c
80100ef7:	e8 74 f4 ff ff       	call   80100370 <panic>
80100efc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80100f00 <filestat>:
80100f00:	55                   	push   %ebp
80100f01:	89 e5                	mov    %esp,%ebp
80100f03:	53                   	push   %ebx
80100f04:	83 ec 04             	sub    $0x4,%esp
80100f07:	8b 5d 08             	mov    0x8(%ebp),%ebx
80100f0a:	83 3b 02             	cmpl   $0x2,(%ebx)
80100f0d:	75 31                	jne    80100f40 <filestat+0x40>
80100f0f:	83 ec 0c             	sub    $0xc,%esp
80100f12:	ff 73 10             	pushl  0x10(%ebx)
80100f15:	e8 56 07 00 00       	call   80101670 <ilock>
80100f1a:	58                   	pop    %eax
80100f1b:	5a                   	pop    %edx
80100f1c:	ff 75 0c             	pushl  0xc(%ebp)
80100f1f:	ff 73 10             	pushl  0x10(%ebx)
80100f22:	e8 f9 09 00 00       	call   80101920 <stati>
80100f27:	59                   	pop    %ecx
80100f28:	ff 73 10             	pushl  0x10(%ebx)
80100f2b:	e8 20 08 00 00       	call   80101750 <iunlock>
80100f30:	83 c4 10             	add    $0x10,%esp
80100f33:	31 c0                	xor    %eax,%eax
80100f35:	8b 5d fc             	mov    -0x4(%ebp),%ebx
80100f38:	c9                   	leave  
80100f39:	c3                   	ret    
80100f3a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80100f40:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80100f45:	8b 5d fc             	mov    -0x4(%ebp),%ebx
80100f48:	c9                   	leave  
80100f49:	c3                   	ret    
80100f4a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

80100f50 <fileread>:
80100f50:	55                   	push   %ebp
80100f51:	89 e5                	mov    %esp,%ebp
80100f53:	57                   	push   %edi
80100f54:	56                   	push   %esi
80100f55:	53                   	push   %ebx
80100f56:	83 ec 0c             	sub    $0xc,%esp
80100f59:	8b 5d 08             	mov    0x8(%ebp),%ebx
80100f5c:	8b 75 0c             	mov    0xc(%ebp),%esi
80100f5f:	8b 7d 10             	mov    0x10(%ebp),%edi
80100f62:	80 7b 08 00          	cmpb   $0x0,0x8(%ebx)
80100f66:	74 60                	je     80100fc8 <fileread+0x78>
80100f68:	8b 03                	mov    (%ebx),%eax
80100f6a:	83 f8 01             	cmp    $0x1,%eax
80100f6d:	74 41                	je     80100fb0 <fileread+0x60>
80100f6f:	83 f8 02             	cmp    $0x2,%eax
80100f72:	75 5b                	jne    80100fcf <fileread+0x7f>
80100f74:	83 ec 0c             	sub    $0xc,%esp
80100f77:	ff 73 10             	pushl  0x10(%ebx)
80100f7a:	e8 f1 06 00 00       	call   80101670 <ilock>
80100f7f:	57                   	push   %edi
80100f80:	ff 73 14             	pushl  0x14(%ebx)
80100f83:	56                   	push   %esi
80100f84:	ff 73 10             	pushl  0x10(%ebx)
80100f87:	e8 c4 09 00 00       	call   80101950 <readi>
80100f8c:	83 c4 20             	add    $0x20,%esp
80100f8f:	85 c0                	test   %eax,%eax
80100f91:	89 c6                	mov    %eax,%esi
80100f93:	7e 03                	jle    80100f98 <fileread+0x48>
80100f95:	01 43 14             	add    %eax,0x14(%ebx)
80100f98:	83 ec 0c             	sub    $0xc,%esp
80100f9b:	ff 73 10             	pushl  0x10(%ebx)
80100f9e:	e8 ad 07 00 00       	call   80101750 <iunlock>
80100fa3:	83 c4 10             	add    $0x10,%esp
80100fa6:	89 f0                	mov    %esi,%eax
80100fa8:	8d 65 f4             	lea    -0xc(%ebp),%esp
80100fab:	5b                   	pop    %ebx
80100fac:	5e                   	pop    %esi
80100fad:	5f                   	pop    %edi
80100fae:	5d                   	pop    %ebp
80100faf:	c3                   	ret    
80100fb0:	8b 43 0c             	mov    0xc(%ebx),%eax
80100fb3:	89 45 08             	mov    %eax,0x8(%ebp)
80100fb6:	8d 65 f4             	lea    -0xc(%ebp),%esp
80100fb9:	5b                   	pop    %ebx
80100fba:	5e                   	pop    %esi
80100fbb:	5f                   	pop    %edi
80100fbc:	5d                   	pop    %ebp
80100fbd:	e9 ce 24 00 00       	jmp    80103490 <piperead>
80100fc2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80100fc8:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80100fcd:	eb d9                	jmp    80100fa8 <fileread+0x58>
80100fcf:	83 ec 0c             	sub    $0xc,%esp
80100fd2:	68 26 75 10 80       	push   $0x80107526
80100fd7:	e8 94 f3 ff ff       	call   80100370 <panic>
80100fdc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80100fe0 <filewrite>:
80100fe0:	55                   	push   %ebp
80100fe1:	89 e5                	mov    %esp,%ebp
80100fe3:	57                   	push   %edi
80100fe4:	56                   	push   %esi
80100fe5:	53                   	push   %ebx
80100fe6:	83 ec 1c             	sub    $0x1c,%esp
80100fe9:	8b 75 08             	mov    0x8(%ebp),%esi
80100fec:	8b 45 0c             	mov    0xc(%ebp),%eax
80100fef:	80 7e 09 00          	cmpb   $0x0,0x9(%esi)
80100ff3:	89 45 dc             	mov    %eax,-0x24(%ebp)
80100ff6:	8b 45 10             	mov    0x10(%ebp),%eax
80100ff9:	89 45 e4             	mov    %eax,-0x1c(%ebp)
80100ffc:	0f 84 aa 00 00 00    	je     801010ac <filewrite+0xcc>
80101002:	8b 06                	mov    (%esi),%eax
80101004:	83 f8 01             	cmp    $0x1,%eax
80101007:	0f 84 c2 00 00 00    	je     801010cf <filewrite+0xef>
8010100d:	83 f8 02             	cmp    $0x2,%eax
80101010:	0f 85 d8 00 00 00    	jne    801010ee <filewrite+0x10e>
80101016:	8b 45 e4             	mov    -0x1c(%ebp),%eax
80101019:	31 ff                	xor    %edi,%edi
8010101b:	85 c0                	test   %eax,%eax
8010101d:	7f 34                	jg     80101053 <filewrite+0x73>
8010101f:	e9 9c 00 00 00       	jmp    801010c0 <filewrite+0xe0>
80101024:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80101028:	01 46 14             	add    %eax,0x14(%esi)
8010102b:	83 ec 0c             	sub    $0xc,%esp
8010102e:	ff 76 10             	pushl  0x10(%esi)
80101031:	89 45 e0             	mov    %eax,-0x20(%ebp)
80101034:	e8 17 07 00 00       	call   80101750 <iunlock>
80101039:	e8 82 1b 00 00       	call   80102bc0 <end_op>
8010103e:	8b 45 e0             	mov    -0x20(%ebp),%eax
80101041:	83 c4 10             	add    $0x10,%esp
80101044:	39 d8                	cmp    %ebx,%eax
80101046:	0f 85 95 00 00 00    	jne    801010e1 <filewrite+0x101>
8010104c:	01 c7                	add    %eax,%edi
8010104e:	39 7d e4             	cmp    %edi,-0x1c(%ebp)
80101051:	7e 6d                	jle    801010c0 <filewrite+0xe0>
80101053:	8b 5d e4             	mov    -0x1c(%ebp),%ebx
80101056:	b8 00 1a 00 00       	mov    $0x1a00,%eax
8010105b:	29 fb                	sub    %edi,%ebx
8010105d:	81 fb 00 1a 00 00    	cmp    $0x1a00,%ebx
80101063:	0f 4f d8             	cmovg  %eax,%ebx
80101066:	e8 e5 1a 00 00       	call   80102b50 <begin_op>
8010106b:	83 ec 0c             	sub    $0xc,%esp
8010106e:	ff 76 10             	pushl  0x10(%esi)
80101071:	e8 fa 05 00 00       	call   80101670 <ilock>
80101076:	8b 45 dc             	mov    -0x24(%ebp),%eax
80101079:	53                   	push   %ebx
8010107a:	ff 76 14             	pushl  0x14(%esi)
8010107d:	01 f8                	add    %edi,%eax
8010107f:	50                   	push   %eax
80101080:	ff 76 10             	pushl  0x10(%esi)
80101083:	e8 c8 09 00 00       	call   80101a50 <writei>
80101088:	83 c4 20             	add    $0x20,%esp
8010108b:	85 c0                	test   %eax,%eax
8010108d:	7f 99                	jg     80101028 <filewrite+0x48>
8010108f:	83 ec 0c             	sub    $0xc,%esp
80101092:	ff 76 10             	pushl  0x10(%esi)
80101095:	89 45 e0             	mov    %eax,-0x20(%ebp)
80101098:	e8 b3 06 00 00       	call   80101750 <iunlock>
8010109d:	e8 1e 1b 00 00       	call   80102bc0 <end_op>
801010a2:	8b 45 e0             	mov    -0x20(%ebp),%eax
801010a5:	83 c4 10             	add    $0x10,%esp
801010a8:	85 c0                	test   %eax,%eax
801010aa:	74 98                	je     80101044 <filewrite+0x64>
801010ac:	8d 65 f4             	lea    -0xc(%ebp),%esp
801010af:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
801010b4:	5b                   	pop    %ebx
801010b5:	5e                   	pop    %esi
801010b6:	5f                   	pop    %edi
801010b7:	5d                   	pop    %ebp
801010b8:	c3                   	ret    
801010b9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801010c0:	3b 7d e4             	cmp    -0x1c(%ebp),%edi
801010c3:	75 e7                	jne    801010ac <filewrite+0xcc>
801010c5:	8d 65 f4             	lea    -0xc(%ebp),%esp
801010c8:	89 f8                	mov    %edi,%eax
801010ca:	5b                   	pop    %ebx
801010cb:	5e                   	pop    %esi
801010cc:	5f                   	pop    %edi
801010cd:	5d                   	pop    %ebp
801010ce:	c3                   	ret    
801010cf:	8b 46 0c             	mov    0xc(%esi),%eax
801010d2:	89 45 08             	mov    %eax,0x8(%ebp)
801010d5:	8d 65 f4             	lea    -0xc(%ebp),%esp
801010d8:	5b                   	pop    %ebx
801010d9:	5e                   	pop    %esi
801010da:	5f                   	pop    %edi
801010db:	5d                   	pop    %ebp
801010dc:	e9 af 22 00 00       	jmp    80103390 <pipewrite>
801010e1:	83 ec 0c             	sub    $0xc,%esp
801010e4:	68 2f 75 10 80       	push   $0x8010752f
801010e9:	e8 82 f2 ff ff       	call   80100370 <panic>
801010ee:	83 ec 0c             	sub    $0xc,%esp
801010f1:	68 35 75 10 80       	push   $0x80107535
801010f6:	e8 75 f2 ff ff       	call   80100370 <panic>
801010fb:	66 90                	xchg   %ax,%ax
801010fd:	66 90                	xchg   %ax,%ax
801010ff:	90                   	nop

80101100 <balloc>:
80101100:	55                   	push   %ebp
80101101:	89 e5                	mov    %esp,%ebp
80101103:	57                   	push   %edi
80101104:	56                   	push   %esi
80101105:	53                   	push   %ebx
80101106:	83 ec 1c             	sub    $0x1c,%esp
80101109:	8b 0d c0 09 11 80    	mov    0x801109c0,%ecx
8010110f:	89 45 d8             	mov    %eax,-0x28(%ebp)
80101112:	85 c9                	test   %ecx,%ecx
80101114:	0f 84 85 00 00 00    	je     8010119f <balloc+0x9f>
8010111a:	c7 45 dc 00 00 00 00 	movl   $0x0,-0x24(%ebp)
80101121:	8b 75 dc             	mov    -0x24(%ebp),%esi
80101124:	83 ec 08             	sub    $0x8,%esp
80101127:	89 f0                	mov    %esi,%eax
80101129:	c1 f8 0c             	sar    $0xc,%eax
8010112c:	03 05 d8 09 11 80    	add    0x801109d8,%eax
80101132:	50                   	push   %eax
80101133:	ff 75 d8             	pushl  -0x28(%ebp)
80101136:	e8 95 ef ff ff       	call   801000d0 <bread>
8010113b:	89 45 e4             	mov    %eax,-0x1c(%ebp)
8010113e:	a1 c0 09 11 80       	mov    0x801109c0,%eax
80101143:	83 c4 10             	add    $0x10,%esp
80101146:	89 45 e0             	mov    %eax,-0x20(%ebp)
80101149:	31 c0                	xor    %eax,%eax
8010114b:	eb 2d                	jmp    8010117a <balloc+0x7a>
8010114d:	8d 76 00             	lea    0x0(%esi),%esi
80101150:	89 c1                	mov    %eax,%ecx
80101152:	ba 01 00 00 00       	mov    $0x1,%edx
80101157:	8b 5d e4             	mov    -0x1c(%ebp),%ebx
8010115a:	83 e1 07             	and    $0x7,%ecx
8010115d:	d3 e2                	shl    %cl,%edx
8010115f:	89 c1                	mov    %eax,%ecx
80101161:	c1 f9 03             	sar    $0x3,%ecx
80101164:	0f b6 7c 0b 5c       	movzbl 0x5c(%ebx,%ecx,1),%edi
80101169:	85 d7                	test   %edx,%edi
8010116b:	74 43                	je     801011b0 <balloc+0xb0>
8010116d:	83 c0 01             	add    $0x1,%eax
80101170:	83 c6 01             	add    $0x1,%esi
80101173:	3d 00 10 00 00       	cmp    $0x1000,%eax
80101178:	74 05                	je     8010117f <balloc+0x7f>
8010117a:	3b 75 e0             	cmp    -0x20(%ebp),%esi
8010117d:	72 d1                	jb     80101150 <balloc+0x50>
8010117f:	83 ec 0c             	sub    $0xc,%esp
80101182:	ff 75 e4             	pushl  -0x1c(%ebp)
80101185:	e8 56 f0 ff ff       	call   801001e0 <brelse>
8010118a:	81 45 dc 00 10 00 00 	addl   $0x1000,-0x24(%ebp)
80101191:	83 c4 10             	add    $0x10,%esp
80101194:	8b 45 dc             	mov    -0x24(%ebp),%eax
80101197:	39 05 c0 09 11 80    	cmp    %eax,0x801109c0
8010119d:	77 82                	ja     80101121 <balloc+0x21>
8010119f:	83 ec 0c             	sub    $0xc,%esp
801011a2:	68 3f 75 10 80       	push   $0x8010753f
801011a7:	e8 c4 f1 ff ff       	call   80100370 <panic>
801011ac:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
801011b0:	09 fa                	or     %edi,%edx
801011b2:	8b 7d e4             	mov    -0x1c(%ebp),%edi
801011b5:	83 ec 0c             	sub    $0xc,%esp
801011b8:	88 54 0f 5c          	mov    %dl,0x5c(%edi,%ecx,1)
801011bc:	57                   	push   %edi
801011bd:	e8 6e 1b 00 00       	call   80102d30 <log_write>
801011c2:	89 3c 24             	mov    %edi,(%esp)
801011c5:	e8 16 f0 ff ff       	call   801001e0 <brelse>
801011ca:	58                   	pop    %eax
801011cb:	5a                   	pop    %edx
801011cc:	56                   	push   %esi
801011cd:	ff 75 d8             	pushl  -0x28(%ebp)
801011d0:	e8 fb ee ff ff       	call   801000d0 <bread>
801011d5:	89 c3                	mov    %eax,%ebx
801011d7:	8d 40 5c             	lea    0x5c(%eax),%eax
801011da:	83 c4 0c             	add    $0xc,%esp
801011dd:	68 00 02 00 00       	push   $0x200
801011e2:	6a 00                	push   $0x0
801011e4:	50                   	push   %eax
801011e5:	e8 f6 36 00 00       	call   801048e0 <memset>
801011ea:	89 1c 24             	mov    %ebx,(%esp)
801011ed:	e8 3e 1b 00 00       	call   80102d30 <log_write>
801011f2:	89 1c 24             	mov    %ebx,(%esp)
801011f5:	e8 e6 ef ff ff       	call   801001e0 <brelse>
801011fa:	8d 65 f4             	lea    -0xc(%ebp),%esp
801011fd:	89 f0                	mov    %esi,%eax
801011ff:	5b                   	pop    %ebx
80101200:	5e                   	pop    %esi
80101201:	5f                   	pop    %edi
80101202:	5d                   	pop    %ebp
80101203:	c3                   	ret    
80101204:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
8010120a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

80101210 <iget>:
80101210:	55                   	push   %ebp
80101211:	89 e5                	mov    %esp,%ebp
80101213:	57                   	push   %edi
80101214:	56                   	push   %esi
80101215:	53                   	push   %ebx
80101216:	89 c7                	mov    %eax,%edi
80101218:	31 f6                	xor    %esi,%esi
8010121a:	bb 14 0a 11 80       	mov    $0x80110a14,%ebx
8010121f:	83 ec 28             	sub    $0x28,%esp
80101222:	89 55 e4             	mov    %edx,-0x1c(%ebp)
80101225:	68 e0 09 11 80       	push   $0x801109e0
8010122a:	e8 41 35 00 00       	call   80104770 <acquire>
8010122f:	83 c4 10             	add    $0x10,%esp
80101232:	8b 55 e4             	mov    -0x1c(%ebp),%edx
80101235:	eb 1b                	jmp    80101252 <iget+0x42>
80101237:	89 f6                	mov    %esi,%esi
80101239:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80101240:	85 f6                	test   %esi,%esi
80101242:	74 44                	je     80101288 <iget+0x78>
80101244:	81 c3 90 00 00 00    	add    $0x90,%ebx
8010124a:	81 fb 34 26 11 80    	cmp    $0x80112634,%ebx
80101250:	74 4e                	je     801012a0 <iget+0x90>
80101252:	8b 4b 08             	mov    0x8(%ebx),%ecx
80101255:	85 c9                	test   %ecx,%ecx
80101257:	7e e7                	jle    80101240 <iget+0x30>
80101259:	39 3b                	cmp    %edi,(%ebx)
8010125b:	75 e3                	jne    80101240 <iget+0x30>
8010125d:	39 53 04             	cmp    %edx,0x4(%ebx)
80101260:	75 de                	jne    80101240 <iget+0x30>
80101262:	83 ec 0c             	sub    $0xc,%esp
80101265:	83 c1 01             	add    $0x1,%ecx
80101268:	89 de                	mov    %ebx,%esi
8010126a:	68 e0 09 11 80       	push   $0x801109e0
8010126f:	89 4b 08             	mov    %ecx,0x8(%ebx)
80101272:	e8 19 36 00 00       	call   80104890 <release>
80101277:	83 c4 10             	add    $0x10,%esp
8010127a:	8d 65 f4             	lea    -0xc(%ebp),%esp
8010127d:	89 f0                	mov    %esi,%eax
8010127f:	5b                   	pop    %ebx
80101280:	5e                   	pop    %esi
80101281:	5f                   	pop    %edi
80101282:	5d                   	pop    %ebp
80101283:	c3                   	ret    
80101284:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80101288:	85 c9                	test   %ecx,%ecx
8010128a:	0f 44 f3             	cmove  %ebx,%esi
8010128d:	81 c3 90 00 00 00    	add    $0x90,%ebx
80101293:	81 fb 34 26 11 80    	cmp    $0x80112634,%ebx
80101299:	75 b7                	jne    80101252 <iget+0x42>
8010129b:	90                   	nop
8010129c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
801012a0:	85 f6                	test   %esi,%esi
801012a2:	74 2d                	je     801012d1 <iget+0xc1>
801012a4:	83 ec 0c             	sub    $0xc,%esp
801012a7:	89 3e                	mov    %edi,(%esi)
801012a9:	89 56 04             	mov    %edx,0x4(%esi)
801012ac:	c7 46 08 01 00 00 00 	movl   $0x1,0x8(%esi)
801012b3:	c7 46 4c 00 00 00 00 	movl   $0x0,0x4c(%esi)
801012ba:	68 e0 09 11 80       	push   $0x801109e0
801012bf:	e8 cc 35 00 00       	call   80104890 <release>
801012c4:	83 c4 10             	add    $0x10,%esp
801012c7:	8d 65 f4             	lea    -0xc(%ebp),%esp
801012ca:	89 f0                	mov    %esi,%eax
801012cc:	5b                   	pop    %ebx
801012cd:	5e                   	pop    %esi
801012ce:	5f                   	pop    %edi
801012cf:	5d                   	pop    %ebp
801012d0:	c3                   	ret    
801012d1:	83 ec 0c             	sub    $0xc,%esp
801012d4:	68 55 75 10 80       	push   $0x80107555
801012d9:	e8 92 f0 ff ff       	call   80100370 <panic>
801012de:	66 90                	xchg   %ax,%ax

801012e0 <bmap>:
801012e0:	55                   	push   %ebp
801012e1:	89 e5                	mov    %esp,%ebp
801012e3:	57                   	push   %edi
801012e4:	56                   	push   %esi
801012e5:	53                   	push   %ebx
801012e6:	89 c6                	mov    %eax,%esi
801012e8:	83 ec 1c             	sub    $0x1c,%esp
801012eb:	83 fa 0b             	cmp    $0xb,%edx
801012ee:	77 18                	ja     80101308 <bmap+0x28>
801012f0:	8d 1c 90             	lea    (%eax,%edx,4),%ebx
801012f3:	8b 43 5c             	mov    0x5c(%ebx),%eax
801012f6:	85 c0                	test   %eax,%eax
801012f8:	74 76                	je     80101370 <bmap+0x90>
801012fa:	8d 65 f4             	lea    -0xc(%ebp),%esp
801012fd:	5b                   	pop    %ebx
801012fe:	5e                   	pop    %esi
801012ff:	5f                   	pop    %edi
80101300:	5d                   	pop    %ebp
80101301:	c3                   	ret    
80101302:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80101308:	8d 5a f4             	lea    -0xc(%edx),%ebx
8010130b:	83 fb 7f             	cmp    $0x7f,%ebx
8010130e:	0f 87 83 00 00 00    	ja     80101397 <bmap+0xb7>
80101314:	8b 80 8c 00 00 00    	mov    0x8c(%eax),%eax
8010131a:	85 c0                	test   %eax,%eax
8010131c:	74 6a                	je     80101388 <bmap+0xa8>
8010131e:	83 ec 08             	sub    $0x8,%esp
80101321:	50                   	push   %eax
80101322:	ff 36                	pushl  (%esi)
80101324:	e8 a7 ed ff ff       	call   801000d0 <bread>
80101329:	8d 54 98 5c          	lea    0x5c(%eax,%ebx,4),%edx
8010132d:	83 c4 10             	add    $0x10,%esp
80101330:	89 c7                	mov    %eax,%edi
80101332:	8b 1a                	mov    (%edx),%ebx
80101334:	85 db                	test   %ebx,%ebx
80101336:	75 1d                	jne    80101355 <bmap+0x75>
80101338:	8b 06                	mov    (%esi),%eax
8010133a:	89 55 e4             	mov    %edx,-0x1c(%ebp)
8010133d:	e8 be fd ff ff       	call   80101100 <balloc>
80101342:	8b 55 e4             	mov    -0x1c(%ebp),%edx
80101345:	83 ec 0c             	sub    $0xc,%esp
80101348:	89 c3                	mov    %eax,%ebx
8010134a:	89 02                	mov    %eax,(%edx)
8010134c:	57                   	push   %edi
8010134d:	e8 de 19 00 00       	call   80102d30 <log_write>
80101352:	83 c4 10             	add    $0x10,%esp
80101355:	83 ec 0c             	sub    $0xc,%esp
80101358:	57                   	push   %edi
80101359:	e8 82 ee ff ff       	call   801001e0 <brelse>
8010135e:	83 c4 10             	add    $0x10,%esp
80101361:	8d 65 f4             	lea    -0xc(%ebp),%esp
80101364:	89 d8                	mov    %ebx,%eax
80101366:	5b                   	pop    %ebx
80101367:	5e                   	pop    %esi
80101368:	5f                   	pop    %edi
80101369:	5d                   	pop    %ebp
8010136a:	c3                   	ret    
8010136b:	90                   	nop
8010136c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80101370:	8b 06                	mov    (%esi),%eax
80101372:	e8 89 fd ff ff       	call   80101100 <balloc>
80101377:	89 43 5c             	mov    %eax,0x5c(%ebx)
8010137a:	8d 65 f4             	lea    -0xc(%ebp),%esp
8010137d:	5b                   	pop    %ebx
8010137e:	5e                   	pop    %esi
8010137f:	5f                   	pop    %edi
80101380:	5d                   	pop    %ebp
80101381:	c3                   	ret    
80101382:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80101388:	8b 06                	mov    (%esi),%eax
8010138a:	e8 71 fd ff ff       	call   80101100 <balloc>
8010138f:	89 86 8c 00 00 00    	mov    %eax,0x8c(%esi)
80101395:	eb 87                	jmp    8010131e <bmap+0x3e>
80101397:	83 ec 0c             	sub    $0xc,%esp
8010139a:	68 65 75 10 80       	push   $0x80107565
8010139f:	e8 cc ef ff ff       	call   80100370 <panic>
801013a4:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
801013aa:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

801013b0 <readsb>:
801013b0:	55                   	push   %ebp
801013b1:	89 e5                	mov    %esp,%ebp
801013b3:	56                   	push   %esi
801013b4:	53                   	push   %ebx
801013b5:	8b 75 0c             	mov    0xc(%ebp),%esi
801013b8:	83 ec 08             	sub    $0x8,%esp
801013bb:	6a 01                	push   $0x1
801013bd:	ff 75 08             	pushl  0x8(%ebp)
801013c0:	e8 0b ed ff ff       	call   801000d0 <bread>
801013c5:	89 c3                	mov    %eax,%ebx
801013c7:	8d 40 5c             	lea    0x5c(%eax),%eax
801013ca:	83 c4 0c             	add    $0xc,%esp
801013cd:	6a 1c                	push   $0x1c
801013cf:	50                   	push   %eax
801013d0:	56                   	push   %esi
801013d1:	e8 ba 35 00 00       	call   80104990 <memmove>
801013d6:	89 5d 08             	mov    %ebx,0x8(%ebp)
801013d9:	83 c4 10             	add    $0x10,%esp
801013dc:	8d 65 f8             	lea    -0x8(%ebp),%esp
801013df:	5b                   	pop    %ebx
801013e0:	5e                   	pop    %esi
801013e1:	5d                   	pop    %ebp
801013e2:	e9 f9 ed ff ff       	jmp    801001e0 <brelse>
801013e7:	89 f6                	mov    %esi,%esi
801013e9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801013f0 <bfree>:
801013f0:	55                   	push   %ebp
801013f1:	89 e5                	mov    %esp,%ebp
801013f3:	56                   	push   %esi
801013f4:	53                   	push   %ebx
801013f5:	89 d3                	mov    %edx,%ebx
801013f7:	89 c6                	mov    %eax,%esi
801013f9:	83 ec 08             	sub    $0x8,%esp
801013fc:	68 c0 09 11 80       	push   $0x801109c0
80101401:	50                   	push   %eax
80101402:	e8 a9 ff ff ff       	call   801013b0 <readsb>
80101407:	58                   	pop    %eax
80101408:	5a                   	pop    %edx
80101409:	89 da                	mov    %ebx,%edx
8010140b:	c1 ea 0c             	shr    $0xc,%edx
8010140e:	03 15 d8 09 11 80    	add    0x801109d8,%edx
80101414:	52                   	push   %edx
80101415:	56                   	push   %esi
80101416:	e8 b5 ec ff ff       	call   801000d0 <bread>
8010141b:	89 d9                	mov    %ebx,%ecx
8010141d:	81 e3 ff 0f 00 00    	and    $0xfff,%ebx
80101423:	ba 01 00 00 00       	mov    $0x1,%edx
80101428:	83 e1 07             	and    $0x7,%ecx
8010142b:	c1 fb 03             	sar    $0x3,%ebx
8010142e:	83 c4 10             	add    $0x10,%esp
80101431:	d3 e2                	shl    %cl,%edx
80101433:	0f b6 4c 18 5c       	movzbl 0x5c(%eax,%ebx,1),%ecx
80101438:	85 d1                	test   %edx,%ecx
8010143a:	74 27                	je     80101463 <bfree+0x73>
8010143c:	89 c6                	mov    %eax,%esi
8010143e:	f7 d2                	not    %edx
80101440:	89 c8                	mov    %ecx,%eax
80101442:	83 ec 0c             	sub    $0xc,%esp
80101445:	21 d0                	and    %edx,%eax
80101447:	88 44 1e 5c          	mov    %al,0x5c(%esi,%ebx,1)
8010144b:	56                   	push   %esi
8010144c:	e8 df 18 00 00       	call   80102d30 <log_write>
80101451:	89 34 24             	mov    %esi,(%esp)
80101454:	e8 87 ed ff ff       	call   801001e0 <brelse>
80101459:	83 c4 10             	add    $0x10,%esp
8010145c:	8d 65 f8             	lea    -0x8(%ebp),%esp
8010145f:	5b                   	pop    %ebx
80101460:	5e                   	pop    %esi
80101461:	5d                   	pop    %ebp
80101462:	c3                   	ret    
80101463:	83 ec 0c             	sub    $0xc,%esp
80101466:	68 78 75 10 80       	push   $0x80107578
8010146b:	e8 00 ef ff ff       	call   80100370 <panic>

80101470 <iinit>:
80101470:	55                   	push   %ebp
80101471:	89 e5                	mov    %esp,%ebp
80101473:	53                   	push   %ebx
80101474:	bb 20 0a 11 80       	mov    $0x80110a20,%ebx
80101479:	83 ec 0c             	sub    $0xc,%esp
8010147c:	68 8b 75 10 80       	push   $0x8010758b
80101481:	68 e0 09 11 80       	push   $0x801109e0
80101486:	e8 e5 31 00 00       	call   80104670 <initlock>
8010148b:	83 c4 10             	add    $0x10,%esp
8010148e:	66 90                	xchg   %ax,%ax
80101490:	83 ec 08             	sub    $0x8,%esp
80101493:	68 92 75 10 80       	push   $0x80107592
80101498:	53                   	push   %ebx
80101499:	81 c3 90 00 00 00    	add    $0x90,%ebx
8010149f:	e8 bc 30 00 00       	call   80104560 <initsleeplock>
801014a4:	83 c4 10             	add    $0x10,%esp
801014a7:	81 fb 40 26 11 80    	cmp    $0x80112640,%ebx
801014ad:	75 e1                	jne    80101490 <iinit+0x20>
801014af:	83 ec 08             	sub    $0x8,%esp
801014b2:	68 c0 09 11 80       	push   $0x801109c0
801014b7:	ff 75 08             	pushl  0x8(%ebp)
801014ba:	e8 f1 fe ff ff       	call   801013b0 <readsb>
801014bf:	ff 35 d8 09 11 80    	pushl  0x801109d8
801014c5:	ff 35 d4 09 11 80    	pushl  0x801109d4
801014cb:	ff 35 d0 09 11 80    	pushl  0x801109d0
801014d1:	ff 35 cc 09 11 80    	pushl  0x801109cc
801014d7:	ff 35 c8 09 11 80    	pushl  0x801109c8
801014dd:	ff 35 c4 09 11 80    	pushl  0x801109c4
801014e3:	ff 35 c0 09 11 80    	pushl  0x801109c0
801014e9:	68 f8 75 10 80       	push   $0x801075f8
801014ee:	e8 6d f1 ff ff       	call   80100660 <cprintf>
801014f3:	83 c4 30             	add    $0x30,%esp
801014f6:	8b 5d fc             	mov    -0x4(%ebp),%ebx
801014f9:	c9                   	leave  
801014fa:	c3                   	ret    
801014fb:	90                   	nop
801014fc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80101500 <ialloc>:
80101500:	55                   	push   %ebp
80101501:	89 e5                	mov    %esp,%ebp
80101503:	57                   	push   %edi
80101504:	56                   	push   %esi
80101505:	53                   	push   %ebx
80101506:	83 ec 1c             	sub    $0x1c,%esp
80101509:	83 3d c8 09 11 80 01 	cmpl   $0x1,0x801109c8
80101510:	8b 45 0c             	mov    0xc(%ebp),%eax
80101513:	8b 75 08             	mov    0x8(%ebp),%esi
80101516:	89 45 e4             	mov    %eax,-0x1c(%ebp)
80101519:	0f 86 91 00 00 00    	jbe    801015b0 <ialloc+0xb0>
8010151f:	bb 01 00 00 00       	mov    $0x1,%ebx
80101524:	eb 21                	jmp    80101547 <ialloc+0x47>
80101526:	8d 76 00             	lea    0x0(%esi),%esi
80101529:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80101530:	83 ec 0c             	sub    $0xc,%esp
80101533:	83 c3 01             	add    $0x1,%ebx
80101536:	57                   	push   %edi
80101537:	e8 a4 ec ff ff       	call   801001e0 <brelse>
8010153c:	83 c4 10             	add    $0x10,%esp
8010153f:	39 1d c8 09 11 80    	cmp    %ebx,0x801109c8
80101545:	76 69                	jbe    801015b0 <ialloc+0xb0>
80101547:	89 d8                	mov    %ebx,%eax
80101549:	83 ec 08             	sub    $0x8,%esp
8010154c:	c1 e8 03             	shr    $0x3,%eax
8010154f:	03 05 d4 09 11 80    	add    0x801109d4,%eax
80101555:	50                   	push   %eax
80101556:	56                   	push   %esi
80101557:	e8 74 eb ff ff       	call   801000d0 <bread>
8010155c:	89 c7                	mov    %eax,%edi
8010155e:	89 d8                	mov    %ebx,%eax
80101560:	83 c4 10             	add    $0x10,%esp
80101563:	83 e0 07             	and    $0x7,%eax
80101566:	c1 e0 06             	shl    $0x6,%eax
80101569:	8d 4c 07 5c          	lea    0x5c(%edi,%eax,1),%ecx
8010156d:	66 83 39 00          	cmpw   $0x0,(%ecx)
80101571:	75 bd                	jne    80101530 <ialloc+0x30>
80101573:	83 ec 04             	sub    $0x4,%esp
80101576:	89 4d e0             	mov    %ecx,-0x20(%ebp)
80101579:	6a 40                	push   $0x40
8010157b:	6a 00                	push   $0x0
8010157d:	51                   	push   %ecx
8010157e:	e8 5d 33 00 00       	call   801048e0 <memset>
80101583:	0f b7 45 e4          	movzwl -0x1c(%ebp),%eax
80101587:	8b 4d e0             	mov    -0x20(%ebp),%ecx
8010158a:	66 89 01             	mov    %ax,(%ecx)
8010158d:	89 3c 24             	mov    %edi,(%esp)
80101590:	e8 9b 17 00 00       	call   80102d30 <log_write>
80101595:	89 3c 24             	mov    %edi,(%esp)
80101598:	e8 43 ec ff ff       	call   801001e0 <brelse>
8010159d:	83 c4 10             	add    $0x10,%esp
801015a0:	8d 65 f4             	lea    -0xc(%ebp),%esp
801015a3:	89 da                	mov    %ebx,%edx
801015a5:	89 f0                	mov    %esi,%eax
801015a7:	5b                   	pop    %ebx
801015a8:	5e                   	pop    %esi
801015a9:	5f                   	pop    %edi
801015aa:	5d                   	pop    %ebp
801015ab:	e9 60 fc ff ff       	jmp    80101210 <iget>
801015b0:	83 ec 0c             	sub    $0xc,%esp
801015b3:	68 98 75 10 80       	push   $0x80107598
801015b8:	e8 b3 ed ff ff       	call   80100370 <panic>
801015bd:	8d 76 00             	lea    0x0(%esi),%esi

801015c0 <iupdate>:
801015c0:	55                   	push   %ebp
801015c1:	89 e5                	mov    %esp,%ebp
801015c3:	56                   	push   %esi
801015c4:	53                   	push   %ebx
801015c5:	8b 5d 08             	mov    0x8(%ebp),%ebx
801015c8:	83 ec 08             	sub    $0x8,%esp
801015cb:	8b 43 04             	mov    0x4(%ebx),%eax
801015ce:	83 c3 5c             	add    $0x5c,%ebx
801015d1:	c1 e8 03             	shr    $0x3,%eax
801015d4:	03 05 d4 09 11 80    	add    0x801109d4,%eax
801015da:	50                   	push   %eax
801015db:	ff 73 a4             	pushl  -0x5c(%ebx)
801015de:	e8 ed ea ff ff       	call   801000d0 <bread>
801015e3:	89 c6                	mov    %eax,%esi
801015e5:	8b 43 a8             	mov    -0x58(%ebx),%eax
801015e8:	0f b7 53 f4          	movzwl -0xc(%ebx),%edx
801015ec:	83 c4 0c             	add    $0xc,%esp
801015ef:	83 e0 07             	and    $0x7,%eax
801015f2:	c1 e0 06             	shl    $0x6,%eax
801015f5:	8d 44 06 5c          	lea    0x5c(%esi,%eax,1),%eax
801015f9:	66 89 10             	mov    %dx,(%eax)
801015fc:	0f b7 53 f6          	movzwl -0xa(%ebx),%edx
80101600:	83 c0 0c             	add    $0xc,%eax
80101603:	66 89 50 f6          	mov    %dx,-0xa(%eax)
80101607:	0f b7 53 f8          	movzwl -0x8(%ebx),%edx
8010160b:	66 89 50 f8          	mov    %dx,-0x8(%eax)
8010160f:	0f b7 53 fa          	movzwl -0x6(%ebx),%edx
80101613:	66 89 50 fa          	mov    %dx,-0x6(%eax)
80101617:	8b 53 fc             	mov    -0x4(%ebx),%edx
8010161a:	89 50 fc             	mov    %edx,-0x4(%eax)
8010161d:	6a 34                	push   $0x34
8010161f:	53                   	push   %ebx
80101620:	50                   	push   %eax
80101621:	e8 6a 33 00 00       	call   80104990 <memmove>
80101626:	89 34 24             	mov    %esi,(%esp)
80101629:	e8 02 17 00 00       	call   80102d30 <log_write>
8010162e:	89 75 08             	mov    %esi,0x8(%ebp)
80101631:	83 c4 10             	add    $0x10,%esp
80101634:	8d 65 f8             	lea    -0x8(%ebp),%esp
80101637:	5b                   	pop    %ebx
80101638:	5e                   	pop    %esi
80101639:	5d                   	pop    %ebp
8010163a:	e9 a1 eb ff ff       	jmp    801001e0 <brelse>
8010163f:	90                   	nop

80101640 <idup>:
80101640:	55                   	push   %ebp
80101641:	89 e5                	mov    %esp,%ebp
80101643:	53                   	push   %ebx
80101644:	83 ec 10             	sub    $0x10,%esp
80101647:	8b 5d 08             	mov    0x8(%ebp),%ebx
8010164a:	68 e0 09 11 80       	push   $0x801109e0
8010164f:	e8 1c 31 00 00       	call   80104770 <acquire>
80101654:	83 43 08 01          	addl   $0x1,0x8(%ebx)
80101658:	c7 04 24 e0 09 11 80 	movl   $0x801109e0,(%esp)
8010165f:	e8 2c 32 00 00       	call   80104890 <release>
80101664:	89 d8                	mov    %ebx,%eax
80101666:	8b 5d fc             	mov    -0x4(%ebp),%ebx
80101669:	c9                   	leave  
8010166a:	c3                   	ret    
8010166b:	90                   	nop
8010166c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80101670 <ilock>:
80101670:	55                   	push   %ebp
80101671:	89 e5                	mov    %esp,%ebp
80101673:	56                   	push   %esi
80101674:	53                   	push   %ebx
80101675:	8b 5d 08             	mov    0x8(%ebp),%ebx
80101678:	85 db                	test   %ebx,%ebx
8010167a:	0f 84 b7 00 00 00    	je     80101737 <ilock+0xc7>
80101680:	8b 53 08             	mov    0x8(%ebx),%edx
80101683:	85 d2                	test   %edx,%edx
80101685:	0f 8e ac 00 00 00    	jle    80101737 <ilock+0xc7>
8010168b:	8d 43 0c             	lea    0xc(%ebx),%eax
8010168e:	83 ec 0c             	sub    $0xc,%esp
80101691:	50                   	push   %eax
80101692:	e8 09 2f 00 00       	call   801045a0 <acquiresleep>
80101697:	8b 43 4c             	mov    0x4c(%ebx),%eax
8010169a:	83 c4 10             	add    $0x10,%esp
8010169d:	85 c0                	test   %eax,%eax
8010169f:	74 0f                	je     801016b0 <ilock+0x40>
801016a1:	8d 65 f8             	lea    -0x8(%ebp),%esp
801016a4:	5b                   	pop    %ebx
801016a5:	5e                   	pop    %esi
801016a6:	5d                   	pop    %ebp
801016a7:	c3                   	ret    
801016a8:	90                   	nop
801016a9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801016b0:	8b 43 04             	mov    0x4(%ebx),%eax
801016b3:	83 ec 08             	sub    $0x8,%esp
801016b6:	c1 e8 03             	shr    $0x3,%eax
801016b9:	03 05 d4 09 11 80    	add    0x801109d4,%eax
801016bf:	50                   	push   %eax
801016c0:	ff 33                	pushl  (%ebx)
801016c2:	e8 09 ea ff ff       	call   801000d0 <bread>
801016c7:	89 c6                	mov    %eax,%esi
801016c9:	8b 43 04             	mov    0x4(%ebx),%eax
801016cc:	83 c4 0c             	add    $0xc,%esp
801016cf:	83 e0 07             	and    $0x7,%eax
801016d2:	c1 e0 06             	shl    $0x6,%eax
801016d5:	8d 44 06 5c          	lea    0x5c(%esi,%eax,1),%eax
801016d9:	0f b7 10             	movzwl (%eax),%edx
801016dc:	83 c0 0c             	add    $0xc,%eax
801016df:	66 89 53 50          	mov    %dx,0x50(%ebx)
801016e3:	0f b7 50 f6          	movzwl -0xa(%eax),%edx
801016e7:	66 89 53 52          	mov    %dx,0x52(%ebx)
801016eb:	0f b7 50 f8          	movzwl -0x8(%eax),%edx
801016ef:	66 89 53 54          	mov    %dx,0x54(%ebx)
801016f3:	0f b7 50 fa          	movzwl -0x6(%eax),%edx
801016f7:	66 89 53 56          	mov    %dx,0x56(%ebx)
801016fb:	8b 50 fc             	mov    -0x4(%eax),%edx
801016fe:	89 53 58             	mov    %edx,0x58(%ebx)
80101701:	6a 34                	push   $0x34
80101703:	50                   	push   %eax
80101704:	8d 43 5c             	lea    0x5c(%ebx),%eax
80101707:	50                   	push   %eax
80101708:	e8 83 32 00 00       	call   80104990 <memmove>
8010170d:	89 34 24             	mov    %esi,(%esp)
80101710:	e8 cb ea ff ff       	call   801001e0 <brelse>
80101715:	83 c4 10             	add    $0x10,%esp
80101718:	66 83 7b 50 00       	cmpw   $0x0,0x50(%ebx)
8010171d:	c7 43 4c 01 00 00 00 	movl   $0x1,0x4c(%ebx)
80101724:	0f 85 77 ff ff ff    	jne    801016a1 <ilock+0x31>
8010172a:	83 ec 0c             	sub    $0xc,%esp
8010172d:	68 b0 75 10 80       	push   $0x801075b0
80101732:	e8 39 ec ff ff       	call   80100370 <panic>
80101737:	83 ec 0c             	sub    $0xc,%esp
8010173a:	68 aa 75 10 80       	push   $0x801075aa
8010173f:	e8 2c ec ff ff       	call   80100370 <panic>
80101744:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
8010174a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

80101750 <iunlock>:
80101750:	55                   	push   %ebp
80101751:	89 e5                	mov    %esp,%ebp
80101753:	56                   	push   %esi
80101754:	53                   	push   %ebx
80101755:	8b 5d 08             	mov    0x8(%ebp),%ebx
80101758:	85 db                	test   %ebx,%ebx
8010175a:	74 28                	je     80101784 <iunlock+0x34>
8010175c:	8d 73 0c             	lea    0xc(%ebx),%esi
8010175f:	83 ec 0c             	sub    $0xc,%esp
80101762:	56                   	push   %esi
80101763:	e8 d8 2e 00 00       	call   80104640 <holdingsleep>
80101768:	83 c4 10             	add    $0x10,%esp
8010176b:	85 c0                	test   %eax,%eax
8010176d:	74 15                	je     80101784 <iunlock+0x34>
8010176f:	8b 43 08             	mov    0x8(%ebx),%eax
80101772:	85 c0                	test   %eax,%eax
80101774:	7e 0e                	jle    80101784 <iunlock+0x34>
80101776:	89 75 08             	mov    %esi,0x8(%ebp)
80101779:	8d 65 f8             	lea    -0x8(%ebp),%esp
8010177c:	5b                   	pop    %ebx
8010177d:	5e                   	pop    %esi
8010177e:	5d                   	pop    %ebp
8010177f:	e9 7c 2e 00 00       	jmp    80104600 <releasesleep>
80101784:	83 ec 0c             	sub    $0xc,%esp
80101787:	68 bf 75 10 80       	push   $0x801075bf
8010178c:	e8 df eb ff ff       	call   80100370 <panic>
80101791:	eb 0d                	jmp    801017a0 <iput>
80101793:	90                   	nop
80101794:	90                   	nop
80101795:	90                   	nop
80101796:	90                   	nop
80101797:	90                   	nop
80101798:	90                   	nop
80101799:	90                   	nop
8010179a:	90                   	nop
8010179b:	90                   	nop
8010179c:	90                   	nop
8010179d:	90                   	nop
8010179e:	90                   	nop
8010179f:	90                   	nop

801017a0 <iput>:
801017a0:	55                   	push   %ebp
801017a1:	89 e5                	mov    %esp,%ebp
801017a3:	57                   	push   %edi
801017a4:	56                   	push   %esi
801017a5:	53                   	push   %ebx
801017a6:	83 ec 28             	sub    $0x28,%esp
801017a9:	8b 75 08             	mov    0x8(%ebp),%esi
801017ac:	8d 7e 0c             	lea    0xc(%esi),%edi
801017af:	57                   	push   %edi
801017b0:	e8 eb 2d 00 00       	call   801045a0 <acquiresleep>
801017b5:	8b 56 4c             	mov    0x4c(%esi),%edx
801017b8:	83 c4 10             	add    $0x10,%esp
801017bb:	85 d2                	test   %edx,%edx
801017bd:	74 07                	je     801017c6 <iput+0x26>
801017bf:	66 83 7e 56 00       	cmpw   $0x0,0x56(%esi)
801017c4:	74 32                	je     801017f8 <iput+0x58>
801017c6:	83 ec 0c             	sub    $0xc,%esp
801017c9:	57                   	push   %edi
801017ca:	e8 31 2e 00 00       	call   80104600 <releasesleep>
801017cf:	c7 04 24 e0 09 11 80 	movl   $0x801109e0,(%esp)
801017d6:	e8 95 2f 00 00       	call   80104770 <acquire>
801017db:	83 6e 08 01          	subl   $0x1,0x8(%esi)
801017df:	83 c4 10             	add    $0x10,%esp
801017e2:	c7 45 08 e0 09 11 80 	movl   $0x801109e0,0x8(%ebp)
801017e9:	8d 65 f4             	lea    -0xc(%ebp),%esp
801017ec:	5b                   	pop    %ebx
801017ed:	5e                   	pop    %esi
801017ee:	5f                   	pop    %edi
801017ef:	5d                   	pop    %ebp
801017f0:	e9 9b 30 00 00       	jmp    80104890 <release>
801017f5:	8d 76 00             	lea    0x0(%esi),%esi
801017f8:	83 ec 0c             	sub    $0xc,%esp
801017fb:	68 e0 09 11 80       	push   $0x801109e0
80101800:	e8 6b 2f 00 00       	call   80104770 <acquire>
80101805:	8b 5e 08             	mov    0x8(%esi),%ebx
80101808:	c7 04 24 e0 09 11 80 	movl   $0x801109e0,(%esp)
8010180f:	e8 7c 30 00 00       	call   80104890 <release>
80101814:	83 c4 10             	add    $0x10,%esp
80101817:	83 fb 01             	cmp    $0x1,%ebx
8010181a:	75 aa                	jne    801017c6 <iput+0x26>
8010181c:	8d 8e 8c 00 00 00    	lea    0x8c(%esi),%ecx
80101822:	89 7d e4             	mov    %edi,-0x1c(%ebp)
80101825:	8d 5e 5c             	lea    0x5c(%esi),%ebx
80101828:	89 cf                	mov    %ecx,%edi
8010182a:	eb 0b                	jmp    80101837 <iput+0x97>
8010182c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80101830:	83 c3 04             	add    $0x4,%ebx
80101833:	39 fb                	cmp    %edi,%ebx
80101835:	74 19                	je     80101850 <iput+0xb0>
80101837:	8b 13                	mov    (%ebx),%edx
80101839:	85 d2                	test   %edx,%edx
8010183b:	74 f3                	je     80101830 <iput+0x90>
8010183d:	8b 06                	mov    (%esi),%eax
8010183f:	e8 ac fb ff ff       	call   801013f0 <bfree>
80101844:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)
8010184a:	eb e4                	jmp    80101830 <iput+0x90>
8010184c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80101850:	8b 86 8c 00 00 00    	mov    0x8c(%esi),%eax
80101856:	8b 7d e4             	mov    -0x1c(%ebp),%edi
80101859:	85 c0                	test   %eax,%eax
8010185b:	75 33                	jne    80101890 <iput+0xf0>
8010185d:	83 ec 0c             	sub    $0xc,%esp
80101860:	c7 46 58 00 00 00 00 	movl   $0x0,0x58(%esi)
80101867:	56                   	push   %esi
80101868:	e8 53 fd ff ff       	call   801015c0 <iupdate>
8010186d:	31 c0                	xor    %eax,%eax
8010186f:	66 89 46 50          	mov    %ax,0x50(%esi)
80101873:	89 34 24             	mov    %esi,(%esp)
80101876:	e8 45 fd ff ff       	call   801015c0 <iupdate>
8010187b:	c7 46 4c 00 00 00 00 	movl   $0x0,0x4c(%esi)
80101882:	83 c4 10             	add    $0x10,%esp
80101885:	e9 3c ff ff ff       	jmp    801017c6 <iput+0x26>
8010188a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80101890:	83 ec 08             	sub    $0x8,%esp
80101893:	50                   	push   %eax
80101894:	ff 36                	pushl  (%esi)
80101896:	e8 35 e8 ff ff       	call   801000d0 <bread>
8010189b:	8d 88 5c 02 00 00    	lea    0x25c(%eax),%ecx
801018a1:	89 7d e0             	mov    %edi,-0x20(%ebp)
801018a4:	89 45 e4             	mov    %eax,-0x1c(%ebp)
801018a7:	8d 58 5c             	lea    0x5c(%eax),%ebx
801018aa:	83 c4 10             	add    $0x10,%esp
801018ad:	89 cf                	mov    %ecx,%edi
801018af:	eb 0e                	jmp    801018bf <iput+0x11f>
801018b1:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801018b8:	83 c3 04             	add    $0x4,%ebx
801018bb:	39 fb                	cmp    %edi,%ebx
801018bd:	74 0f                	je     801018ce <iput+0x12e>
801018bf:	8b 13                	mov    (%ebx),%edx
801018c1:	85 d2                	test   %edx,%edx
801018c3:	74 f3                	je     801018b8 <iput+0x118>
801018c5:	8b 06                	mov    (%esi),%eax
801018c7:	e8 24 fb ff ff       	call   801013f0 <bfree>
801018cc:	eb ea                	jmp    801018b8 <iput+0x118>
801018ce:	83 ec 0c             	sub    $0xc,%esp
801018d1:	ff 75 e4             	pushl  -0x1c(%ebp)
801018d4:	8b 7d e0             	mov    -0x20(%ebp),%edi
801018d7:	e8 04 e9 ff ff       	call   801001e0 <brelse>
801018dc:	8b 96 8c 00 00 00    	mov    0x8c(%esi),%edx
801018e2:	8b 06                	mov    (%esi),%eax
801018e4:	e8 07 fb ff ff       	call   801013f0 <bfree>
801018e9:	c7 86 8c 00 00 00 00 	movl   $0x0,0x8c(%esi)
801018f0:	00 00 00 
801018f3:	83 c4 10             	add    $0x10,%esp
801018f6:	e9 62 ff ff ff       	jmp    8010185d <iput+0xbd>
801018fb:	90                   	nop
801018fc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80101900 <iunlockput>:
80101900:	55                   	push   %ebp
80101901:	89 e5                	mov    %esp,%ebp
80101903:	53                   	push   %ebx
80101904:	83 ec 10             	sub    $0x10,%esp
80101907:	8b 5d 08             	mov    0x8(%ebp),%ebx
8010190a:	53                   	push   %ebx
8010190b:	e8 40 fe ff ff       	call   80101750 <iunlock>
80101910:	89 5d 08             	mov    %ebx,0x8(%ebp)
80101913:	83 c4 10             	add    $0x10,%esp
80101916:	8b 5d fc             	mov    -0x4(%ebp),%ebx
80101919:	c9                   	leave  
8010191a:	e9 81 fe ff ff       	jmp    801017a0 <iput>
8010191f:	90                   	nop

80101920 <stati>:
80101920:	55                   	push   %ebp
80101921:	89 e5                	mov    %esp,%ebp
80101923:	8b 55 08             	mov    0x8(%ebp),%edx
80101926:	8b 45 0c             	mov    0xc(%ebp),%eax
80101929:	8b 0a                	mov    (%edx),%ecx
8010192b:	89 48 04             	mov    %ecx,0x4(%eax)
8010192e:	8b 4a 04             	mov    0x4(%edx),%ecx
80101931:	89 48 08             	mov    %ecx,0x8(%eax)
80101934:	0f b7 4a 50          	movzwl 0x50(%edx),%ecx
80101938:	66 89 08             	mov    %cx,(%eax)
8010193b:	0f b7 4a 56          	movzwl 0x56(%edx),%ecx
8010193f:	66 89 48 0c          	mov    %cx,0xc(%eax)
80101943:	8b 52 58             	mov    0x58(%edx),%edx
80101946:	89 50 10             	mov    %edx,0x10(%eax)
80101949:	5d                   	pop    %ebp
8010194a:	c3                   	ret    
8010194b:	90                   	nop
8010194c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80101950 <readi>:
80101950:	55                   	push   %ebp
80101951:	89 e5                	mov    %esp,%ebp
80101953:	57                   	push   %edi
80101954:	56                   	push   %esi
80101955:	53                   	push   %ebx
80101956:	83 ec 1c             	sub    $0x1c,%esp
80101959:	8b 45 08             	mov    0x8(%ebp),%eax
8010195c:	8b 7d 0c             	mov    0xc(%ebp),%edi
8010195f:	8b 75 10             	mov    0x10(%ebp),%esi
80101962:	66 83 78 50 03       	cmpw   $0x3,0x50(%eax)
80101967:	89 7d e0             	mov    %edi,-0x20(%ebp)
8010196a:	8b 7d 14             	mov    0x14(%ebp),%edi
8010196d:	89 45 d8             	mov    %eax,-0x28(%ebp)
80101970:	89 7d e4             	mov    %edi,-0x1c(%ebp)
80101973:	0f 84 a7 00 00 00    	je     80101a20 <readi+0xd0>
80101979:	8b 45 d8             	mov    -0x28(%ebp),%eax
8010197c:	8b 40 58             	mov    0x58(%eax),%eax
8010197f:	39 f0                	cmp    %esi,%eax
80101981:	0f 82 c1 00 00 00    	jb     80101a48 <readi+0xf8>
80101987:	8b 7d e4             	mov    -0x1c(%ebp),%edi
8010198a:	89 fa                	mov    %edi,%edx
8010198c:	01 f2                	add    %esi,%edx
8010198e:	0f 82 b4 00 00 00    	jb     80101a48 <readi+0xf8>
80101994:	89 c1                	mov    %eax,%ecx
80101996:	29 f1                	sub    %esi,%ecx
80101998:	39 d0                	cmp    %edx,%eax
8010199a:	0f 43 cf             	cmovae %edi,%ecx
8010199d:	31 ff                	xor    %edi,%edi
8010199f:	85 c9                	test   %ecx,%ecx
801019a1:	89 4d e4             	mov    %ecx,-0x1c(%ebp)
801019a4:	74 6d                	je     80101a13 <readi+0xc3>
801019a6:	8d 76 00             	lea    0x0(%esi),%esi
801019a9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
801019b0:	8b 5d d8             	mov    -0x28(%ebp),%ebx
801019b3:	89 f2                	mov    %esi,%edx
801019b5:	c1 ea 09             	shr    $0x9,%edx
801019b8:	89 d8                	mov    %ebx,%eax
801019ba:	e8 21 f9 ff ff       	call   801012e0 <bmap>
801019bf:	83 ec 08             	sub    $0x8,%esp
801019c2:	50                   	push   %eax
801019c3:	ff 33                	pushl  (%ebx)
801019c5:	bb 00 02 00 00       	mov    $0x200,%ebx
801019ca:	e8 01 e7 ff ff       	call   801000d0 <bread>
801019cf:	89 c2                	mov    %eax,%edx
801019d1:	8b 45 e4             	mov    -0x1c(%ebp),%eax
801019d4:	89 f1                	mov    %esi,%ecx
801019d6:	81 e1 ff 01 00 00    	and    $0x1ff,%ecx
801019dc:	83 c4 0c             	add    $0xc,%esp
801019df:	89 55 dc             	mov    %edx,-0x24(%ebp)
801019e2:	29 cb                	sub    %ecx,%ebx
801019e4:	29 f8                	sub    %edi,%eax
801019e6:	39 c3                	cmp    %eax,%ebx
801019e8:	0f 47 d8             	cmova  %eax,%ebx
801019eb:	8d 44 0a 5c          	lea    0x5c(%edx,%ecx,1),%eax
801019ef:	53                   	push   %ebx
801019f0:	01 df                	add    %ebx,%edi
801019f2:	01 de                	add    %ebx,%esi
801019f4:	50                   	push   %eax
801019f5:	ff 75 e0             	pushl  -0x20(%ebp)
801019f8:	e8 93 2f 00 00       	call   80104990 <memmove>
801019fd:	8b 55 dc             	mov    -0x24(%ebp),%edx
80101a00:	89 14 24             	mov    %edx,(%esp)
80101a03:	e8 d8 e7 ff ff       	call   801001e0 <brelse>
80101a08:	01 5d e0             	add    %ebx,-0x20(%ebp)
80101a0b:	83 c4 10             	add    $0x10,%esp
80101a0e:	39 7d e4             	cmp    %edi,-0x1c(%ebp)
80101a11:	77 9d                	ja     801019b0 <readi+0x60>
80101a13:	8b 45 e4             	mov    -0x1c(%ebp),%eax
80101a16:	8d 65 f4             	lea    -0xc(%ebp),%esp
80101a19:	5b                   	pop    %ebx
80101a1a:	5e                   	pop    %esi
80101a1b:	5f                   	pop    %edi
80101a1c:	5d                   	pop    %ebp
80101a1d:	c3                   	ret    
80101a1e:	66 90                	xchg   %ax,%ax
80101a20:	0f bf 40 52          	movswl 0x52(%eax),%eax
80101a24:	66 83 f8 09          	cmp    $0x9,%ax
80101a28:	77 1e                	ja     80101a48 <readi+0xf8>
80101a2a:	8b 04 c5 60 09 11 80 	mov    -0x7feef6a0(,%eax,8),%eax
80101a31:	85 c0                	test   %eax,%eax
80101a33:	74 13                	je     80101a48 <readi+0xf8>
80101a35:	89 7d 10             	mov    %edi,0x10(%ebp)
80101a38:	8d 65 f4             	lea    -0xc(%ebp),%esp
80101a3b:	5b                   	pop    %ebx
80101a3c:	5e                   	pop    %esi
80101a3d:	5f                   	pop    %edi
80101a3e:	5d                   	pop    %ebp
80101a3f:	ff e0                	jmp    *%eax
80101a41:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80101a48:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80101a4d:	eb c7                	jmp    80101a16 <readi+0xc6>
80101a4f:	90                   	nop

80101a50 <writei>:
80101a50:	55                   	push   %ebp
80101a51:	89 e5                	mov    %esp,%ebp
80101a53:	57                   	push   %edi
80101a54:	56                   	push   %esi
80101a55:	53                   	push   %ebx
80101a56:	83 ec 1c             	sub    $0x1c,%esp
80101a59:	8b 45 08             	mov    0x8(%ebp),%eax
80101a5c:	8b 75 0c             	mov    0xc(%ebp),%esi
80101a5f:	8b 7d 14             	mov    0x14(%ebp),%edi
80101a62:	66 83 78 50 03       	cmpw   $0x3,0x50(%eax)
80101a67:	89 75 dc             	mov    %esi,-0x24(%ebp)
80101a6a:	89 45 d8             	mov    %eax,-0x28(%ebp)
80101a6d:	8b 75 10             	mov    0x10(%ebp),%esi
80101a70:	89 7d e0             	mov    %edi,-0x20(%ebp)
80101a73:	0f 84 b7 00 00 00    	je     80101b30 <writei+0xe0>
80101a79:	8b 45 d8             	mov    -0x28(%ebp),%eax
80101a7c:	39 70 58             	cmp    %esi,0x58(%eax)
80101a7f:	0f 82 eb 00 00 00    	jb     80101b70 <writei+0x120>
80101a85:	8b 7d e0             	mov    -0x20(%ebp),%edi
80101a88:	89 f8                	mov    %edi,%eax
80101a8a:	01 f0                	add    %esi,%eax
80101a8c:	3d 00 18 01 00       	cmp    $0x11800,%eax
80101a91:	0f 87 d9 00 00 00    	ja     80101b70 <writei+0x120>
80101a97:	39 c6                	cmp    %eax,%esi
80101a99:	0f 87 d1 00 00 00    	ja     80101b70 <writei+0x120>
80101a9f:	85 ff                	test   %edi,%edi
80101aa1:	c7 45 e4 00 00 00 00 	movl   $0x0,-0x1c(%ebp)
80101aa8:	74 78                	je     80101b22 <writei+0xd2>
80101aaa:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80101ab0:	8b 7d d8             	mov    -0x28(%ebp),%edi
80101ab3:	89 f2                	mov    %esi,%edx
80101ab5:	bb 00 02 00 00       	mov    $0x200,%ebx
80101aba:	c1 ea 09             	shr    $0x9,%edx
80101abd:	89 f8                	mov    %edi,%eax
80101abf:	e8 1c f8 ff ff       	call   801012e0 <bmap>
80101ac4:	83 ec 08             	sub    $0x8,%esp
80101ac7:	50                   	push   %eax
80101ac8:	ff 37                	pushl  (%edi)
80101aca:	e8 01 e6 ff ff       	call   801000d0 <bread>
80101acf:	89 c7                	mov    %eax,%edi
80101ad1:	8b 45 e0             	mov    -0x20(%ebp),%eax
80101ad4:	2b 45 e4             	sub    -0x1c(%ebp),%eax
80101ad7:	89 f1                	mov    %esi,%ecx
80101ad9:	83 c4 0c             	add    $0xc,%esp
80101adc:	81 e1 ff 01 00 00    	and    $0x1ff,%ecx
80101ae2:	29 cb                	sub    %ecx,%ebx
80101ae4:	39 c3                	cmp    %eax,%ebx
80101ae6:	0f 47 d8             	cmova  %eax,%ebx
80101ae9:	8d 44 0f 5c          	lea    0x5c(%edi,%ecx,1),%eax
80101aed:	53                   	push   %ebx
80101aee:	ff 75 dc             	pushl  -0x24(%ebp)
80101af1:	01 de                	add    %ebx,%esi
80101af3:	50                   	push   %eax
80101af4:	e8 97 2e 00 00       	call   80104990 <memmove>
80101af9:	89 3c 24             	mov    %edi,(%esp)
80101afc:	e8 2f 12 00 00       	call   80102d30 <log_write>
80101b01:	89 3c 24             	mov    %edi,(%esp)
80101b04:	e8 d7 e6 ff ff       	call   801001e0 <brelse>
80101b09:	01 5d e4             	add    %ebx,-0x1c(%ebp)
80101b0c:	01 5d dc             	add    %ebx,-0x24(%ebp)
80101b0f:	83 c4 10             	add    $0x10,%esp
80101b12:	8b 55 e4             	mov    -0x1c(%ebp),%edx
80101b15:	39 55 e0             	cmp    %edx,-0x20(%ebp)
80101b18:	77 96                	ja     80101ab0 <writei+0x60>
80101b1a:	8b 45 d8             	mov    -0x28(%ebp),%eax
80101b1d:	3b 70 58             	cmp    0x58(%eax),%esi
80101b20:	77 36                	ja     80101b58 <writei+0x108>
80101b22:	8b 45 e0             	mov    -0x20(%ebp),%eax
80101b25:	8d 65 f4             	lea    -0xc(%ebp),%esp
80101b28:	5b                   	pop    %ebx
80101b29:	5e                   	pop    %esi
80101b2a:	5f                   	pop    %edi
80101b2b:	5d                   	pop    %ebp
80101b2c:	c3                   	ret    
80101b2d:	8d 76 00             	lea    0x0(%esi),%esi
80101b30:	0f bf 40 52          	movswl 0x52(%eax),%eax
80101b34:	66 83 f8 09          	cmp    $0x9,%ax
80101b38:	77 36                	ja     80101b70 <writei+0x120>
80101b3a:	8b 04 c5 64 09 11 80 	mov    -0x7feef69c(,%eax,8),%eax
80101b41:	85 c0                	test   %eax,%eax
80101b43:	74 2b                	je     80101b70 <writei+0x120>
80101b45:	89 7d 10             	mov    %edi,0x10(%ebp)
80101b48:	8d 65 f4             	lea    -0xc(%ebp),%esp
80101b4b:	5b                   	pop    %ebx
80101b4c:	5e                   	pop    %esi
80101b4d:	5f                   	pop    %edi
80101b4e:	5d                   	pop    %ebp
80101b4f:	ff e0                	jmp    *%eax
80101b51:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80101b58:	8b 45 d8             	mov    -0x28(%ebp),%eax
80101b5b:	83 ec 0c             	sub    $0xc,%esp
80101b5e:	89 70 58             	mov    %esi,0x58(%eax)
80101b61:	50                   	push   %eax
80101b62:	e8 59 fa ff ff       	call   801015c0 <iupdate>
80101b67:	83 c4 10             	add    $0x10,%esp
80101b6a:	eb b6                	jmp    80101b22 <writei+0xd2>
80101b6c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80101b70:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80101b75:	eb ae                	jmp    80101b25 <writei+0xd5>
80101b77:	89 f6                	mov    %esi,%esi
80101b79:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80101b80 <namecmp>:
80101b80:	55                   	push   %ebp
80101b81:	89 e5                	mov    %esp,%ebp
80101b83:	83 ec 0c             	sub    $0xc,%esp
80101b86:	6a 0e                	push   $0xe
80101b88:	ff 75 0c             	pushl  0xc(%ebp)
80101b8b:	ff 75 08             	pushl  0x8(%ebp)
80101b8e:	e8 7d 2e 00 00       	call   80104a10 <strncmp>
80101b93:	c9                   	leave  
80101b94:	c3                   	ret    
80101b95:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80101b99:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80101ba0 <dirlookup>:
80101ba0:	55                   	push   %ebp
80101ba1:	89 e5                	mov    %esp,%ebp
80101ba3:	57                   	push   %edi
80101ba4:	56                   	push   %esi
80101ba5:	53                   	push   %ebx
80101ba6:	83 ec 1c             	sub    $0x1c,%esp
80101ba9:	8b 5d 08             	mov    0x8(%ebp),%ebx
80101bac:	66 83 7b 50 01       	cmpw   $0x1,0x50(%ebx)
80101bb1:	0f 85 80 00 00 00    	jne    80101c37 <dirlookup+0x97>
80101bb7:	8b 53 58             	mov    0x58(%ebx),%edx
80101bba:	31 ff                	xor    %edi,%edi
80101bbc:	8d 75 d8             	lea    -0x28(%ebp),%esi
80101bbf:	85 d2                	test   %edx,%edx
80101bc1:	75 0d                	jne    80101bd0 <dirlookup+0x30>
80101bc3:	eb 5b                	jmp    80101c20 <dirlookup+0x80>
80101bc5:	8d 76 00             	lea    0x0(%esi),%esi
80101bc8:	83 c7 10             	add    $0x10,%edi
80101bcb:	39 7b 58             	cmp    %edi,0x58(%ebx)
80101bce:	76 50                	jbe    80101c20 <dirlookup+0x80>
80101bd0:	6a 10                	push   $0x10
80101bd2:	57                   	push   %edi
80101bd3:	56                   	push   %esi
80101bd4:	53                   	push   %ebx
80101bd5:	e8 76 fd ff ff       	call   80101950 <readi>
80101bda:	83 c4 10             	add    $0x10,%esp
80101bdd:	83 f8 10             	cmp    $0x10,%eax
80101be0:	75 48                	jne    80101c2a <dirlookup+0x8a>
80101be2:	66 83 7d d8 00       	cmpw   $0x0,-0x28(%ebp)
80101be7:	74 df                	je     80101bc8 <dirlookup+0x28>
80101be9:	8d 45 da             	lea    -0x26(%ebp),%eax
80101bec:	83 ec 04             	sub    $0x4,%esp
80101bef:	6a 0e                	push   $0xe
80101bf1:	50                   	push   %eax
80101bf2:	ff 75 0c             	pushl  0xc(%ebp)
80101bf5:	e8 16 2e 00 00       	call   80104a10 <strncmp>
80101bfa:	83 c4 10             	add    $0x10,%esp
80101bfd:	85 c0                	test   %eax,%eax
80101bff:	75 c7                	jne    80101bc8 <dirlookup+0x28>
80101c01:	8b 45 10             	mov    0x10(%ebp),%eax
80101c04:	85 c0                	test   %eax,%eax
80101c06:	74 05                	je     80101c0d <dirlookup+0x6d>
80101c08:	8b 45 10             	mov    0x10(%ebp),%eax
80101c0b:	89 38                	mov    %edi,(%eax)
80101c0d:	0f b7 55 d8          	movzwl -0x28(%ebp),%edx
80101c11:	8b 03                	mov    (%ebx),%eax
80101c13:	e8 f8 f5 ff ff       	call   80101210 <iget>
80101c18:	8d 65 f4             	lea    -0xc(%ebp),%esp
80101c1b:	5b                   	pop    %ebx
80101c1c:	5e                   	pop    %esi
80101c1d:	5f                   	pop    %edi
80101c1e:	5d                   	pop    %ebp
80101c1f:	c3                   	ret    
80101c20:	8d 65 f4             	lea    -0xc(%ebp),%esp
80101c23:	31 c0                	xor    %eax,%eax
80101c25:	5b                   	pop    %ebx
80101c26:	5e                   	pop    %esi
80101c27:	5f                   	pop    %edi
80101c28:	5d                   	pop    %ebp
80101c29:	c3                   	ret    
80101c2a:	83 ec 0c             	sub    $0xc,%esp
80101c2d:	68 d9 75 10 80       	push   $0x801075d9
80101c32:	e8 39 e7 ff ff       	call   80100370 <panic>
80101c37:	83 ec 0c             	sub    $0xc,%esp
80101c3a:	68 c7 75 10 80       	push   $0x801075c7
80101c3f:	e8 2c e7 ff ff       	call   80100370 <panic>
80101c44:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80101c4a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

80101c50 <namex>:
80101c50:	55                   	push   %ebp
80101c51:	89 e5                	mov    %esp,%ebp
80101c53:	57                   	push   %edi
80101c54:	56                   	push   %esi
80101c55:	53                   	push   %ebx
80101c56:	89 cf                	mov    %ecx,%edi
80101c58:	89 c3                	mov    %eax,%ebx
80101c5a:	83 ec 1c             	sub    $0x1c,%esp
80101c5d:	80 38 2f             	cmpb   $0x2f,(%eax)
80101c60:	89 55 e0             	mov    %edx,-0x20(%ebp)
80101c63:	0f 84 53 01 00 00    	je     80101dbc <namex+0x16c>
80101c69:	e8 02 1c 00 00       	call   80103870 <myproc>
80101c6e:	83 ec 0c             	sub    $0xc,%esp
80101c71:	8b 70 68             	mov    0x68(%eax),%esi
80101c74:	68 e0 09 11 80       	push   $0x801109e0
80101c79:	e8 f2 2a 00 00       	call   80104770 <acquire>
80101c7e:	83 46 08 01          	addl   $0x1,0x8(%esi)
80101c82:	c7 04 24 e0 09 11 80 	movl   $0x801109e0,(%esp)
80101c89:	e8 02 2c 00 00       	call   80104890 <release>
80101c8e:	83 c4 10             	add    $0x10,%esp
80101c91:	eb 08                	jmp    80101c9b <namex+0x4b>
80101c93:	90                   	nop
80101c94:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80101c98:	83 c3 01             	add    $0x1,%ebx
80101c9b:	0f b6 03             	movzbl (%ebx),%eax
80101c9e:	3c 2f                	cmp    $0x2f,%al
80101ca0:	74 f6                	je     80101c98 <namex+0x48>
80101ca2:	84 c0                	test   %al,%al
80101ca4:	0f 84 e3 00 00 00    	je     80101d8d <namex+0x13d>
80101caa:	0f b6 03             	movzbl (%ebx),%eax
80101cad:	89 da                	mov    %ebx,%edx
80101caf:	84 c0                	test   %al,%al
80101cb1:	0f 84 ac 00 00 00    	je     80101d63 <namex+0x113>
80101cb7:	3c 2f                	cmp    $0x2f,%al
80101cb9:	75 09                	jne    80101cc4 <namex+0x74>
80101cbb:	e9 a3 00 00 00       	jmp    80101d63 <namex+0x113>
80101cc0:	84 c0                	test   %al,%al
80101cc2:	74 0a                	je     80101cce <namex+0x7e>
80101cc4:	83 c2 01             	add    $0x1,%edx
80101cc7:	0f b6 02             	movzbl (%edx),%eax
80101cca:	3c 2f                	cmp    $0x2f,%al
80101ccc:	75 f2                	jne    80101cc0 <namex+0x70>
80101cce:	89 d1                	mov    %edx,%ecx
80101cd0:	29 d9                	sub    %ebx,%ecx
80101cd2:	83 f9 0d             	cmp    $0xd,%ecx
80101cd5:	0f 8e 8d 00 00 00    	jle    80101d68 <namex+0x118>
80101cdb:	83 ec 04             	sub    $0x4,%esp
80101cde:	89 55 e4             	mov    %edx,-0x1c(%ebp)
80101ce1:	6a 0e                	push   $0xe
80101ce3:	53                   	push   %ebx
80101ce4:	57                   	push   %edi
80101ce5:	e8 a6 2c 00 00       	call   80104990 <memmove>
80101cea:	8b 55 e4             	mov    -0x1c(%ebp),%edx
80101ced:	83 c4 10             	add    $0x10,%esp
80101cf0:	89 d3                	mov    %edx,%ebx
80101cf2:	80 3a 2f             	cmpb   $0x2f,(%edx)
80101cf5:	75 11                	jne    80101d08 <namex+0xb8>
80101cf7:	89 f6                	mov    %esi,%esi
80101cf9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80101d00:	83 c3 01             	add    $0x1,%ebx
80101d03:	80 3b 2f             	cmpb   $0x2f,(%ebx)
80101d06:	74 f8                	je     80101d00 <namex+0xb0>
80101d08:	83 ec 0c             	sub    $0xc,%esp
80101d0b:	56                   	push   %esi
80101d0c:	e8 5f f9 ff ff       	call   80101670 <ilock>
80101d11:	83 c4 10             	add    $0x10,%esp
80101d14:	66 83 7e 50 01       	cmpw   $0x1,0x50(%esi)
80101d19:	0f 85 7f 00 00 00    	jne    80101d9e <namex+0x14e>
80101d1f:	8b 55 e0             	mov    -0x20(%ebp),%edx
80101d22:	85 d2                	test   %edx,%edx
80101d24:	74 09                	je     80101d2f <namex+0xdf>
80101d26:	80 3b 00             	cmpb   $0x0,(%ebx)
80101d29:	0f 84 a3 00 00 00    	je     80101dd2 <namex+0x182>
80101d2f:	83 ec 04             	sub    $0x4,%esp
80101d32:	6a 00                	push   $0x0
80101d34:	57                   	push   %edi
80101d35:	56                   	push   %esi
80101d36:	e8 65 fe ff ff       	call   80101ba0 <dirlookup>
80101d3b:	83 c4 10             	add    $0x10,%esp
80101d3e:	85 c0                	test   %eax,%eax
80101d40:	74 5c                	je     80101d9e <namex+0x14e>
80101d42:	83 ec 0c             	sub    $0xc,%esp
80101d45:	89 45 e4             	mov    %eax,-0x1c(%ebp)
80101d48:	56                   	push   %esi
80101d49:	e8 02 fa ff ff       	call   80101750 <iunlock>
80101d4e:	89 34 24             	mov    %esi,(%esp)
80101d51:	e8 4a fa ff ff       	call   801017a0 <iput>
80101d56:	8b 45 e4             	mov    -0x1c(%ebp),%eax
80101d59:	83 c4 10             	add    $0x10,%esp
80101d5c:	89 c6                	mov    %eax,%esi
80101d5e:	e9 38 ff ff ff       	jmp    80101c9b <namex+0x4b>
80101d63:	31 c9                	xor    %ecx,%ecx
80101d65:	8d 76 00             	lea    0x0(%esi),%esi
80101d68:	83 ec 04             	sub    $0x4,%esp
80101d6b:	89 55 dc             	mov    %edx,-0x24(%ebp)
80101d6e:	89 4d e4             	mov    %ecx,-0x1c(%ebp)
80101d71:	51                   	push   %ecx
80101d72:	53                   	push   %ebx
80101d73:	57                   	push   %edi
80101d74:	e8 17 2c 00 00       	call   80104990 <memmove>
80101d79:	8b 4d e4             	mov    -0x1c(%ebp),%ecx
80101d7c:	8b 55 dc             	mov    -0x24(%ebp),%edx
80101d7f:	83 c4 10             	add    $0x10,%esp
80101d82:	c6 04 0f 00          	movb   $0x0,(%edi,%ecx,1)
80101d86:	89 d3                	mov    %edx,%ebx
80101d88:	e9 65 ff ff ff       	jmp    80101cf2 <namex+0xa2>
80101d8d:	8b 45 e0             	mov    -0x20(%ebp),%eax
80101d90:	85 c0                	test   %eax,%eax
80101d92:	75 54                	jne    80101de8 <namex+0x198>
80101d94:	89 f0                	mov    %esi,%eax
80101d96:	8d 65 f4             	lea    -0xc(%ebp),%esp
80101d99:	5b                   	pop    %ebx
80101d9a:	5e                   	pop    %esi
80101d9b:	5f                   	pop    %edi
80101d9c:	5d                   	pop    %ebp
80101d9d:	c3                   	ret    
80101d9e:	83 ec 0c             	sub    $0xc,%esp
80101da1:	56                   	push   %esi
80101da2:	e8 a9 f9 ff ff       	call   80101750 <iunlock>
80101da7:	89 34 24             	mov    %esi,(%esp)
80101daa:	e8 f1 f9 ff ff       	call   801017a0 <iput>
80101daf:	83 c4 10             	add    $0x10,%esp
80101db2:	8d 65 f4             	lea    -0xc(%ebp),%esp
80101db5:	31 c0                	xor    %eax,%eax
80101db7:	5b                   	pop    %ebx
80101db8:	5e                   	pop    %esi
80101db9:	5f                   	pop    %edi
80101dba:	5d                   	pop    %ebp
80101dbb:	c3                   	ret    
80101dbc:	ba 01 00 00 00       	mov    $0x1,%edx
80101dc1:	b8 01 00 00 00       	mov    $0x1,%eax
80101dc6:	e8 45 f4 ff ff       	call   80101210 <iget>
80101dcb:	89 c6                	mov    %eax,%esi
80101dcd:	e9 c9 fe ff ff       	jmp    80101c9b <namex+0x4b>
80101dd2:	83 ec 0c             	sub    $0xc,%esp
80101dd5:	56                   	push   %esi
80101dd6:	e8 75 f9 ff ff       	call   80101750 <iunlock>
80101ddb:	83 c4 10             	add    $0x10,%esp
80101dde:	8d 65 f4             	lea    -0xc(%ebp),%esp
80101de1:	89 f0                	mov    %esi,%eax
80101de3:	5b                   	pop    %ebx
80101de4:	5e                   	pop    %esi
80101de5:	5f                   	pop    %edi
80101de6:	5d                   	pop    %ebp
80101de7:	c3                   	ret    
80101de8:	83 ec 0c             	sub    $0xc,%esp
80101deb:	56                   	push   %esi
80101dec:	e8 af f9 ff ff       	call   801017a0 <iput>
80101df1:	83 c4 10             	add    $0x10,%esp
80101df4:	31 c0                	xor    %eax,%eax
80101df6:	eb 9e                	jmp    80101d96 <namex+0x146>
80101df8:	90                   	nop
80101df9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

80101e00 <dirlink>:
80101e00:	55                   	push   %ebp
80101e01:	89 e5                	mov    %esp,%ebp
80101e03:	57                   	push   %edi
80101e04:	56                   	push   %esi
80101e05:	53                   	push   %ebx
80101e06:	83 ec 20             	sub    $0x20,%esp
80101e09:	8b 5d 08             	mov    0x8(%ebp),%ebx
80101e0c:	6a 00                	push   $0x0
80101e0e:	ff 75 0c             	pushl  0xc(%ebp)
80101e11:	53                   	push   %ebx
80101e12:	e8 89 fd ff ff       	call   80101ba0 <dirlookup>
80101e17:	83 c4 10             	add    $0x10,%esp
80101e1a:	85 c0                	test   %eax,%eax
80101e1c:	75 67                	jne    80101e85 <dirlink+0x85>
80101e1e:	8b 7b 58             	mov    0x58(%ebx),%edi
80101e21:	8d 75 d8             	lea    -0x28(%ebp),%esi
80101e24:	85 ff                	test   %edi,%edi
80101e26:	74 29                	je     80101e51 <dirlink+0x51>
80101e28:	31 ff                	xor    %edi,%edi
80101e2a:	8d 75 d8             	lea    -0x28(%ebp),%esi
80101e2d:	eb 09                	jmp    80101e38 <dirlink+0x38>
80101e2f:	90                   	nop
80101e30:	83 c7 10             	add    $0x10,%edi
80101e33:	39 7b 58             	cmp    %edi,0x58(%ebx)
80101e36:	76 19                	jbe    80101e51 <dirlink+0x51>
80101e38:	6a 10                	push   $0x10
80101e3a:	57                   	push   %edi
80101e3b:	56                   	push   %esi
80101e3c:	53                   	push   %ebx
80101e3d:	e8 0e fb ff ff       	call   80101950 <readi>
80101e42:	83 c4 10             	add    $0x10,%esp
80101e45:	83 f8 10             	cmp    $0x10,%eax
80101e48:	75 4e                	jne    80101e98 <dirlink+0x98>
80101e4a:	66 83 7d d8 00       	cmpw   $0x0,-0x28(%ebp)
80101e4f:	75 df                	jne    80101e30 <dirlink+0x30>
80101e51:	8d 45 da             	lea    -0x26(%ebp),%eax
80101e54:	83 ec 04             	sub    $0x4,%esp
80101e57:	6a 0e                	push   $0xe
80101e59:	ff 75 0c             	pushl  0xc(%ebp)
80101e5c:	50                   	push   %eax
80101e5d:	e8 1e 2c 00 00       	call   80104a80 <strncpy>
80101e62:	8b 45 10             	mov    0x10(%ebp),%eax
80101e65:	6a 10                	push   $0x10
80101e67:	57                   	push   %edi
80101e68:	56                   	push   %esi
80101e69:	53                   	push   %ebx
80101e6a:	66 89 45 d8          	mov    %ax,-0x28(%ebp)
80101e6e:	e8 dd fb ff ff       	call   80101a50 <writei>
80101e73:	83 c4 20             	add    $0x20,%esp
80101e76:	83 f8 10             	cmp    $0x10,%eax
80101e79:	75 2a                	jne    80101ea5 <dirlink+0xa5>
80101e7b:	31 c0                	xor    %eax,%eax
80101e7d:	8d 65 f4             	lea    -0xc(%ebp),%esp
80101e80:	5b                   	pop    %ebx
80101e81:	5e                   	pop    %esi
80101e82:	5f                   	pop    %edi
80101e83:	5d                   	pop    %ebp
80101e84:	c3                   	ret    
80101e85:	83 ec 0c             	sub    $0xc,%esp
80101e88:	50                   	push   %eax
80101e89:	e8 12 f9 ff ff       	call   801017a0 <iput>
80101e8e:	83 c4 10             	add    $0x10,%esp
80101e91:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80101e96:	eb e5                	jmp    80101e7d <dirlink+0x7d>
80101e98:	83 ec 0c             	sub    $0xc,%esp
80101e9b:	68 e8 75 10 80       	push   $0x801075e8
80101ea0:	e8 cb e4 ff ff       	call   80100370 <panic>
80101ea5:	83 ec 0c             	sub    $0xc,%esp
80101ea8:	68 aa 7c 10 80       	push   $0x80107caa
80101ead:	e8 be e4 ff ff       	call   80100370 <panic>
80101eb2:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80101eb9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80101ec0 <namei>:
80101ec0:	55                   	push   %ebp
80101ec1:	31 d2                	xor    %edx,%edx
80101ec3:	89 e5                	mov    %esp,%ebp
80101ec5:	83 ec 18             	sub    $0x18,%esp
80101ec8:	8b 45 08             	mov    0x8(%ebp),%eax
80101ecb:	8d 4d ea             	lea    -0x16(%ebp),%ecx
80101ece:	e8 7d fd ff ff       	call   80101c50 <namex>
80101ed3:	c9                   	leave  
80101ed4:	c3                   	ret    
80101ed5:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80101ed9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80101ee0 <nameiparent>:
80101ee0:	55                   	push   %ebp
80101ee1:	ba 01 00 00 00       	mov    $0x1,%edx
80101ee6:	89 e5                	mov    %esp,%ebp
80101ee8:	8b 4d 0c             	mov    0xc(%ebp),%ecx
80101eeb:	8b 45 08             	mov    0x8(%ebp),%eax
80101eee:	5d                   	pop    %ebp
80101eef:	e9 5c fd ff ff       	jmp    80101c50 <namex>
80101ef4:	66 90                	xchg   %ax,%ax
80101ef6:	66 90                	xchg   %ax,%ax
80101ef8:	66 90                	xchg   %ax,%ax
80101efa:	66 90                	xchg   %ax,%ax
80101efc:	66 90                	xchg   %ax,%ax
80101efe:	66 90                	xchg   %ax,%ax

80101f00 <idestart>:
80101f00:	55                   	push   %ebp
80101f01:	85 c0                	test   %eax,%eax
80101f03:	89 e5                	mov    %esp,%ebp
80101f05:	56                   	push   %esi
80101f06:	53                   	push   %ebx
80101f07:	0f 84 ad 00 00 00    	je     80101fba <idestart+0xba>
80101f0d:	8b 58 08             	mov    0x8(%eax),%ebx
80101f10:	89 c1                	mov    %eax,%ecx
80101f12:	81 fb e7 03 00 00    	cmp    $0x3e7,%ebx
80101f18:	0f 87 8f 00 00 00    	ja     80101fad <idestart+0xad>
80101f1e:	ba f7 01 00 00       	mov    $0x1f7,%edx
80101f23:	90                   	nop
80101f24:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80101f28:	ec                   	in     (%dx),%al
80101f29:	83 e0 c0             	and    $0xffffffc0,%eax
80101f2c:	3c 40                	cmp    $0x40,%al
80101f2e:	75 f8                	jne    80101f28 <idestart+0x28>
80101f30:	31 f6                	xor    %esi,%esi
80101f32:	ba f6 03 00 00       	mov    $0x3f6,%edx
80101f37:	89 f0                	mov    %esi,%eax
80101f39:	ee                   	out    %al,(%dx)
80101f3a:	ba f2 01 00 00       	mov    $0x1f2,%edx
80101f3f:	b8 01 00 00 00       	mov    $0x1,%eax
80101f44:	ee                   	out    %al,(%dx)
80101f45:	ba f3 01 00 00       	mov    $0x1f3,%edx
80101f4a:	89 d8                	mov    %ebx,%eax
80101f4c:	ee                   	out    %al,(%dx)
80101f4d:	89 d8                	mov    %ebx,%eax
80101f4f:	ba f4 01 00 00       	mov    $0x1f4,%edx
80101f54:	c1 f8 08             	sar    $0x8,%eax
80101f57:	ee                   	out    %al,(%dx)
80101f58:	ba f5 01 00 00       	mov    $0x1f5,%edx
80101f5d:	89 f0                	mov    %esi,%eax
80101f5f:	ee                   	out    %al,(%dx)
80101f60:	0f b6 41 04          	movzbl 0x4(%ecx),%eax
80101f64:	ba f6 01 00 00       	mov    $0x1f6,%edx
80101f69:	83 e0 01             	and    $0x1,%eax
80101f6c:	c1 e0 04             	shl    $0x4,%eax
80101f6f:	83 c8 e0             	or     $0xffffffe0,%eax
80101f72:	ee                   	out    %al,(%dx)
80101f73:	f6 01 04             	testb  $0x4,(%ecx)
80101f76:	ba f7 01 00 00       	mov    $0x1f7,%edx
80101f7b:	75 13                	jne    80101f90 <idestart+0x90>
80101f7d:	b8 20 00 00 00       	mov    $0x20,%eax
80101f82:	ee                   	out    %al,(%dx)
80101f83:	8d 65 f8             	lea    -0x8(%ebp),%esp
80101f86:	5b                   	pop    %ebx
80101f87:	5e                   	pop    %esi
80101f88:	5d                   	pop    %ebp
80101f89:	c3                   	ret    
80101f8a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80101f90:	b8 30 00 00 00       	mov    $0x30,%eax
80101f95:	ee                   	out    %al,(%dx)
80101f96:	ba f0 01 00 00       	mov    $0x1f0,%edx
80101f9b:	8d 71 5c             	lea    0x5c(%ecx),%esi
80101f9e:	b9 80 00 00 00       	mov    $0x80,%ecx
80101fa3:	fc                   	cld    
80101fa4:	f3 6f                	rep outsl %ds:(%esi),(%dx)
80101fa6:	8d 65 f8             	lea    -0x8(%ebp),%esp
80101fa9:	5b                   	pop    %ebx
80101faa:	5e                   	pop    %esi
80101fab:	5d                   	pop    %ebp
80101fac:	c3                   	ret    
80101fad:	83 ec 0c             	sub    $0xc,%esp
80101fb0:	68 54 76 10 80       	push   $0x80107654
80101fb5:	e8 b6 e3 ff ff       	call   80100370 <panic>
80101fba:	83 ec 0c             	sub    $0xc,%esp
80101fbd:	68 4b 76 10 80       	push   $0x8010764b
80101fc2:	e8 a9 e3 ff ff       	call   80100370 <panic>
80101fc7:	89 f6                	mov    %esi,%esi
80101fc9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80101fd0 <ideinit>:
80101fd0:	55                   	push   %ebp
80101fd1:	89 e5                	mov    %esp,%ebp
80101fd3:	83 ec 10             	sub    $0x10,%esp
80101fd6:	68 66 76 10 80       	push   $0x80107666
80101fdb:	68 80 a5 10 80       	push   $0x8010a580
80101fe0:	e8 8b 26 00 00       	call   80104670 <initlock>
80101fe5:	58                   	pop    %eax
80101fe6:	a1 00 2d 11 80       	mov    0x80112d00,%eax
80101feb:	5a                   	pop    %edx
80101fec:	83 e8 01             	sub    $0x1,%eax
80101fef:	50                   	push   %eax
80101ff0:	6a 0e                	push   $0xe
80101ff2:	e8 a9 02 00 00       	call   801022a0 <ioapicenable>
80101ff7:	83 c4 10             	add    $0x10,%esp
80101ffa:	ba f7 01 00 00       	mov    $0x1f7,%edx
80101fff:	90                   	nop
80102000:	ec                   	in     (%dx),%al
80102001:	83 e0 c0             	and    $0xffffffc0,%eax
80102004:	3c 40                	cmp    $0x40,%al
80102006:	75 f8                	jne    80102000 <ideinit+0x30>
80102008:	ba f6 01 00 00       	mov    $0x1f6,%edx
8010200d:	b8 f0 ff ff ff       	mov    $0xfffffff0,%eax
80102012:	ee                   	out    %al,(%dx)
80102013:	b9 e8 03 00 00       	mov    $0x3e8,%ecx
80102018:	ba f7 01 00 00       	mov    $0x1f7,%edx
8010201d:	eb 06                	jmp    80102025 <ideinit+0x55>
8010201f:	90                   	nop
80102020:	83 e9 01             	sub    $0x1,%ecx
80102023:	74 0f                	je     80102034 <ideinit+0x64>
80102025:	ec                   	in     (%dx),%al
80102026:	84 c0                	test   %al,%al
80102028:	74 f6                	je     80102020 <ideinit+0x50>
8010202a:	c7 05 60 a5 10 80 01 	movl   $0x1,0x8010a560
80102031:	00 00 00 
80102034:	ba f6 01 00 00       	mov    $0x1f6,%edx
80102039:	b8 e0 ff ff ff       	mov    $0xffffffe0,%eax
8010203e:	ee                   	out    %al,(%dx)
8010203f:	c9                   	leave  
80102040:	c3                   	ret    
80102041:	eb 0d                	jmp    80102050 <ideintr>
80102043:	90                   	nop
80102044:	90                   	nop
80102045:	90                   	nop
80102046:	90                   	nop
80102047:	90                   	nop
80102048:	90                   	nop
80102049:	90                   	nop
8010204a:	90                   	nop
8010204b:	90                   	nop
8010204c:	90                   	nop
8010204d:	90                   	nop
8010204e:	90                   	nop
8010204f:	90                   	nop

80102050 <ideintr>:
80102050:	55                   	push   %ebp
80102051:	89 e5                	mov    %esp,%ebp
80102053:	57                   	push   %edi
80102054:	56                   	push   %esi
80102055:	53                   	push   %ebx
80102056:	83 ec 18             	sub    $0x18,%esp
80102059:	68 80 a5 10 80       	push   $0x8010a580
8010205e:	e8 0d 27 00 00       	call   80104770 <acquire>
80102063:	8b 1d 64 a5 10 80    	mov    0x8010a564,%ebx
80102069:	83 c4 10             	add    $0x10,%esp
8010206c:	85 db                	test   %ebx,%ebx
8010206e:	74 34                	je     801020a4 <ideintr+0x54>
80102070:	8b 43 58             	mov    0x58(%ebx),%eax
80102073:	a3 64 a5 10 80       	mov    %eax,0x8010a564
80102078:	8b 33                	mov    (%ebx),%esi
8010207a:	f7 c6 04 00 00 00    	test   $0x4,%esi
80102080:	74 3e                	je     801020c0 <ideintr+0x70>
80102082:	83 e6 fb             	and    $0xfffffffb,%esi
80102085:	83 ec 0c             	sub    $0xc,%esp
80102088:	83 ce 02             	or     $0x2,%esi
8010208b:	89 33                	mov    %esi,(%ebx)
8010208d:	53                   	push   %ebx
8010208e:	e8 cd 20 00 00       	call   80104160 <wakeup>
80102093:	a1 64 a5 10 80       	mov    0x8010a564,%eax
80102098:	83 c4 10             	add    $0x10,%esp
8010209b:	85 c0                	test   %eax,%eax
8010209d:	74 05                	je     801020a4 <ideintr+0x54>
8010209f:	e8 5c fe ff ff       	call   80101f00 <idestart>
801020a4:	83 ec 0c             	sub    $0xc,%esp
801020a7:	68 80 a5 10 80       	push   $0x8010a580
801020ac:	e8 df 27 00 00       	call   80104890 <release>
801020b1:	8d 65 f4             	lea    -0xc(%ebp),%esp
801020b4:	5b                   	pop    %ebx
801020b5:	5e                   	pop    %esi
801020b6:	5f                   	pop    %edi
801020b7:	5d                   	pop    %ebp
801020b8:	c3                   	ret    
801020b9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801020c0:	ba f7 01 00 00       	mov    $0x1f7,%edx
801020c5:	8d 76 00             	lea    0x0(%esi),%esi
801020c8:	ec                   	in     (%dx),%al
801020c9:	89 c1                	mov    %eax,%ecx
801020cb:	83 e1 c0             	and    $0xffffffc0,%ecx
801020ce:	80 f9 40             	cmp    $0x40,%cl
801020d1:	75 f5                	jne    801020c8 <ideintr+0x78>
801020d3:	a8 21                	test   $0x21,%al
801020d5:	75 ab                	jne    80102082 <ideintr+0x32>
801020d7:	8d 7b 5c             	lea    0x5c(%ebx),%edi
801020da:	b9 80 00 00 00       	mov    $0x80,%ecx
801020df:	ba f0 01 00 00       	mov    $0x1f0,%edx
801020e4:	fc                   	cld    
801020e5:	f3 6d                	rep insl (%dx),%es:(%edi)
801020e7:	8b 33                	mov    (%ebx),%esi
801020e9:	eb 97                	jmp    80102082 <ideintr+0x32>
801020eb:	90                   	nop
801020ec:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

801020f0 <iderw>:
801020f0:	55                   	push   %ebp
801020f1:	89 e5                	mov    %esp,%ebp
801020f3:	53                   	push   %ebx
801020f4:	83 ec 10             	sub    $0x10,%esp
801020f7:	8b 5d 08             	mov    0x8(%ebp),%ebx
801020fa:	8d 43 0c             	lea    0xc(%ebx),%eax
801020fd:	50                   	push   %eax
801020fe:	e8 3d 25 00 00       	call   80104640 <holdingsleep>
80102103:	83 c4 10             	add    $0x10,%esp
80102106:	85 c0                	test   %eax,%eax
80102108:	0f 84 ad 00 00 00    	je     801021bb <iderw+0xcb>
8010210e:	8b 03                	mov    (%ebx),%eax
80102110:	83 e0 06             	and    $0x6,%eax
80102113:	83 f8 02             	cmp    $0x2,%eax
80102116:	0f 84 b9 00 00 00    	je     801021d5 <iderw+0xe5>
8010211c:	8b 53 04             	mov    0x4(%ebx),%edx
8010211f:	85 d2                	test   %edx,%edx
80102121:	74 0d                	je     80102130 <iderw+0x40>
80102123:	a1 60 a5 10 80       	mov    0x8010a560,%eax
80102128:	85 c0                	test   %eax,%eax
8010212a:	0f 84 98 00 00 00    	je     801021c8 <iderw+0xd8>
80102130:	83 ec 0c             	sub    $0xc,%esp
80102133:	68 80 a5 10 80       	push   $0x8010a580
80102138:	e8 33 26 00 00       	call   80104770 <acquire>
8010213d:	8b 15 64 a5 10 80    	mov    0x8010a564,%edx
80102143:	83 c4 10             	add    $0x10,%esp
80102146:	c7 43 58 00 00 00 00 	movl   $0x0,0x58(%ebx)
8010214d:	85 d2                	test   %edx,%edx
8010214f:	75 09                	jne    8010215a <iderw+0x6a>
80102151:	eb 58                	jmp    801021ab <iderw+0xbb>
80102153:	90                   	nop
80102154:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80102158:	89 c2                	mov    %eax,%edx
8010215a:	8b 42 58             	mov    0x58(%edx),%eax
8010215d:	85 c0                	test   %eax,%eax
8010215f:	75 f7                	jne    80102158 <iderw+0x68>
80102161:	83 c2 58             	add    $0x58,%edx
80102164:	89 1a                	mov    %ebx,(%edx)
80102166:	3b 1d 64 a5 10 80    	cmp    0x8010a564,%ebx
8010216c:	74 44                	je     801021b2 <iderw+0xc2>
8010216e:	8b 03                	mov    (%ebx),%eax
80102170:	83 e0 06             	and    $0x6,%eax
80102173:	83 f8 02             	cmp    $0x2,%eax
80102176:	74 23                	je     8010219b <iderw+0xab>
80102178:	90                   	nop
80102179:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80102180:	83 ec 08             	sub    $0x8,%esp
80102183:	68 80 a5 10 80       	push   $0x8010a580
80102188:	53                   	push   %ebx
80102189:	e8 12 1e 00 00       	call   80103fa0 <sleep>
8010218e:	8b 03                	mov    (%ebx),%eax
80102190:	83 c4 10             	add    $0x10,%esp
80102193:	83 e0 06             	and    $0x6,%eax
80102196:	83 f8 02             	cmp    $0x2,%eax
80102199:	75 e5                	jne    80102180 <iderw+0x90>
8010219b:	c7 45 08 80 a5 10 80 	movl   $0x8010a580,0x8(%ebp)
801021a2:	8b 5d fc             	mov    -0x4(%ebp),%ebx
801021a5:	c9                   	leave  
801021a6:	e9 e5 26 00 00       	jmp    80104890 <release>
801021ab:	ba 64 a5 10 80       	mov    $0x8010a564,%edx
801021b0:	eb b2                	jmp    80102164 <iderw+0x74>
801021b2:	89 d8                	mov    %ebx,%eax
801021b4:	e8 47 fd ff ff       	call   80101f00 <idestart>
801021b9:	eb b3                	jmp    8010216e <iderw+0x7e>
801021bb:	83 ec 0c             	sub    $0xc,%esp
801021be:	68 6a 76 10 80       	push   $0x8010766a
801021c3:	e8 a8 e1 ff ff       	call   80100370 <panic>
801021c8:	83 ec 0c             	sub    $0xc,%esp
801021cb:	68 95 76 10 80       	push   $0x80107695
801021d0:	e8 9b e1 ff ff       	call   80100370 <panic>
801021d5:	83 ec 0c             	sub    $0xc,%esp
801021d8:	68 80 76 10 80       	push   $0x80107680
801021dd:	e8 8e e1 ff ff       	call   80100370 <panic>
801021e2:	66 90                	xchg   %ax,%ax
801021e4:	66 90                	xchg   %ax,%ax
801021e6:	66 90                	xchg   %ax,%ax
801021e8:	66 90                	xchg   %ax,%ax
801021ea:	66 90                	xchg   %ax,%ax
801021ec:	66 90                	xchg   %ax,%ax
801021ee:	66 90                	xchg   %ax,%ax

801021f0 <ioapicinit>:
801021f0:	55                   	push   %ebp
801021f1:	c7 05 34 26 11 80 00 	movl   $0xfec00000,0x80112634
801021f8:	00 c0 fe 
801021fb:	89 e5                	mov    %esp,%ebp
801021fd:	56                   	push   %esi
801021fe:	53                   	push   %ebx
801021ff:	c7 05 00 00 c0 fe 01 	movl   $0x1,0xfec00000
80102206:	00 00 00 
80102209:	8b 15 34 26 11 80    	mov    0x80112634,%edx
8010220f:	8b 72 10             	mov    0x10(%edx),%esi
80102212:	c7 02 00 00 00 00    	movl   $0x0,(%edx)
80102218:	8b 0d 34 26 11 80    	mov    0x80112634,%ecx
8010221e:	0f b6 15 60 27 11 80 	movzbl 0x80112760,%edx
80102225:	89 f0                	mov    %esi,%eax
80102227:	c1 e8 10             	shr    $0x10,%eax
8010222a:	0f b6 f0             	movzbl %al,%esi
8010222d:	8b 41 10             	mov    0x10(%ecx),%eax
80102230:	c1 e8 18             	shr    $0x18,%eax
80102233:	39 d0                	cmp    %edx,%eax
80102235:	74 16                	je     8010224d <ioapicinit+0x5d>
80102237:	83 ec 0c             	sub    $0xc,%esp
8010223a:	68 b4 76 10 80       	push   $0x801076b4
8010223f:	e8 1c e4 ff ff       	call   80100660 <cprintf>
80102244:	8b 0d 34 26 11 80    	mov    0x80112634,%ecx
8010224a:	83 c4 10             	add    $0x10,%esp
8010224d:	83 c6 21             	add    $0x21,%esi
80102250:	ba 10 00 00 00       	mov    $0x10,%edx
80102255:	b8 20 00 00 00       	mov    $0x20,%eax
8010225a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80102260:	89 11                	mov    %edx,(%ecx)
80102262:	8b 0d 34 26 11 80    	mov    0x80112634,%ecx
80102268:	89 c3                	mov    %eax,%ebx
8010226a:	81 cb 00 00 01 00    	or     $0x10000,%ebx
80102270:	83 c0 01             	add    $0x1,%eax
80102273:	89 59 10             	mov    %ebx,0x10(%ecx)
80102276:	8d 5a 01             	lea    0x1(%edx),%ebx
80102279:	83 c2 02             	add    $0x2,%edx
8010227c:	39 f0                	cmp    %esi,%eax
8010227e:	89 19                	mov    %ebx,(%ecx)
80102280:	8b 0d 34 26 11 80    	mov    0x80112634,%ecx
80102286:	c7 41 10 00 00 00 00 	movl   $0x0,0x10(%ecx)
8010228d:	75 d1                	jne    80102260 <ioapicinit+0x70>
8010228f:	8d 65 f8             	lea    -0x8(%ebp),%esp
80102292:	5b                   	pop    %ebx
80102293:	5e                   	pop    %esi
80102294:	5d                   	pop    %ebp
80102295:	c3                   	ret    
80102296:	8d 76 00             	lea    0x0(%esi),%esi
80102299:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801022a0 <ioapicenable>:
801022a0:	55                   	push   %ebp
801022a1:	8b 0d 34 26 11 80    	mov    0x80112634,%ecx
801022a7:	89 e5                	mov    %esp,%ebp
801022a9:	8b 45 08             	mov    0x8(%ebp),%eax
801022ac:	8d 50 20             	lea    0x20(%eax),%edx
801022af:	8d 44 00 10          	lea    0x10(%eax,%eax,1),%eax
801022b3:	89 01                	mov    %eax,(%ecx)
801022b5:	8b 0d 34 26 11 80    	mov    0x80112634,%ecx
801022bb:	83 c0 01             	add    $0x1,%eax
801022be:	89 51 10             	mov    %edx,0x10(%ecx)
801022c1:	8b 55 0c             	mov    0xc(%ebp),%edx
801022c4:	89 01                	mov    %eax,(%ecx)
801022c6:	a1 34 26 11 80       	mov    0x80112634,%eax
801022cb:	c1 e2 18             	shl    $0x18,%edx
801022ce:	89 50 10             	mov    %edx,0x10(%eax)
801022d1:	5d                   	pop    %ebp
801022d2:	c3                   	ret    
801022d3:	66 90                	xchg   %ax,%ax
801022d5:	66 90                	xchg   %ax,%ax
801022d7:	66 90                	xchg   %ax,%ax
801022d9:	66 90                	xchg   %ax,%ax
801022db:	66 90                	xchg   %ax,%ax
801022dd:	66 90                	xchg   %ax,%ax
801022df:	90                   	nop

801022e0 <kfree>:
801022e0:	55                   	push   %ebp
801022e1:	89 e5                	mov    %esp,%ebp
801022e3:	53                   	push   %ebx
801022e4:	83 ec 04             	sub    $0x4,%esp
801022e7:	8b 5d 08             	mov    0x8(%ebp),%ebx
801022ea:	f7 c3 ff 0f 00 00    	test   $0xfff,%ebx
801022f0:	75 70                	jne    80102362 <kfree+0x82>
801022f2:	81 fb a8 5b 11 80    	cmp    $0x80115ba8,%ebx
801022f8:	72 68                	jb     80102362 <kfree+0x82>
801022fa:	8d 83 00 00 00 80    	lea    -0x80000000(%ebx),%eax
80102300:	3d ff ff ff 0d       	cmp    $0xdffffff,%eax
80102305:	77 5b                	ja     80102362 <kfree+0x82>
80102307:	83 ec 04             	sub    $0x4,%esp
8010230a:	68 00 10 00 00       	push   $0x1000
8010230f:	6a 01                	push   $0x1
80102311:	53                   	push   %ebx
80102312:	e8 c9 25 00 00       	call   801048e0 <memset>
80102317:	8b 15 74 26 11 80    	mov    0x80112674,%edx
8010231d:	83 c4 10             	add    $0x10,%esp
80102320:	85 d2                	test   %edx,%edx
80102322:	75 2c                	jne    80102350 <kfree+0x70>
80102324:	a1 78 26 11 80       	mov    0x80112678,%eax
80102329:	89 03                	mov    %eax,(%ebx)
8010232b:	a1 74 26 11 80       	mov    0x80112674,%eax
80102330:	89 1d 78 26 11 80    	mov    %ebx,0x80112678
80102336:	85 c0                	test   %eax,%eax
80102338:	75 06                	jne    80102340 <kfree+0x60>
8010233a:	8b 5d fc             	mov    -0x4(%ebp),%ebx
8010233d:	c9                   	leave  
8010233e:	c3                   	ret    
8010233f:	90                   	nop
80102340:	c7 45 08 40 26 11 80 	movl   $0x80112640,0x8(%ebp)
80102347:	8b 5d fc             	mov    -0x4(%ebp),%ebx
8010234a:	c9                   	leave  
8010234b:	e9 40 25 00 00       	jmp    80104890 <release>
80102350:	83 ec 0c             	sub    $0xc,%esp
80102353:	68 40 26 11 80       	push   $0x80112640
80102358:	e8 13 24 00 00       	call   80104770 <acquire>
8010235d:	83 c4 10             	add    $0x10,%esp
80102360:	eb c2                	jmp    80102324 <kfree+0x44>
80102362:	83 ec 0c             	sub    $0xc,%esp
80102365:	68 e6 76 10 80       	push   $0x801076e6
8010236a:	e8 01 e0 ff ff       	call   80100370 <panic>
8010236f:	90                   	nop

80102370 <freerange>:
80102370:	55                   	push   %ebp
80102371:	89 e5                	mov    %esp,%ebp
80102373:	56                   	push   %esi
80102374:	53                   	push   %ebx
80102375:	8b 45 08             	mov    0x8(%ebp),%eax
80102378:	8b 75 0c             	mov    0xc(%ebp),%esi
8010237b:	8d 98 ff 0f 00 00    	lea    0xfff(%eax),%ebx
80102381:	81 e3 00 f0 ff ff    	and    $0xfffff000,%ebx
80102387:	81 c3 00 10 00 00    	add    $0x1000,%ebx
8010238d:	39 de                	cmp    %ebx,%esi
8010238f:	72 23                	jb     801023b4 <freerange+0x44>
80102391:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80102398:	8d 83 00 f0 ff ff    	lea    -0x1000(%ebx),%eax
8010239e:	83 ec 0c             	sub    $0xc,%esp
801023a1:	81 c3 00 10 00 00    	add    $0x1000,%ebx
801023a7:	50                   	push   %eax
801023a8:	e8 33 ff ff ff       	call   801022e0 <kfree>
801023ad:	83 c4 10             	add    $0x10,%esp
801023b0:	39 f3                	cmp    %esi,%ebx
801023b2:	76 e4                	jbe    80102398 <freerange+0x28>
801023b4:	8d 65 f8             	lea    -0x8(%ebp),%esp
801023b7:	5b                   	pop    %ebx
801023b8:	5e                   	pop    %esi
801023b9:	5d                   	pop    %ebp
801023ba:	c3                   	ret    
801023bb:	90                   	nop
801023bc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

801023c0 <kinit1>:
801023c0:	55                   	push   %ebp
801023c1:	89 e5                	mov    %esp,%ebp
801023c3:	56                   	push   %esi
801023c4:	53                   	push   %ebx
801023c5:	8b 75 0c             	mov    0xc(%ebp),%esi
801023c8:	83 ec 08             	sub    $0x8,%esp
801023cb:	68 ec 76 10 80       	push   $0x801076ec
801023d0:	68 40 26 11 80       	push   $0x80112640
801023d5:	e8 96 22 00 00       	call   80104670 <initlock>
801023da:	8b 45 08             	mov    0x8(%ebp),%eax
801023dd:	83 c4 10             	add    $0x10,%esp
801023e0:	c7 05 74 26 11 80 00 	movl   $0x0,0x80112674
801023e7:	00 00 00 
801023ea:	8d 98 ff 0f 00 00    	lea    0xfff(%eax),%ebx
801023f0:	81 e3 00 f0 ff ff    	and    $0xfffff000,%ebx
801023f6:	81 c3 00 10 00 00    	add    $0x1000,%ebx
801023fc:	39 de                	cmp    %ebx,%esi
801023fe:	72 1c                	jb     8010241c <kinit1+0x5c>
80102400:	8d 83 00 f0 ff ff    	lea    -0x1000(%ebx),%eax
80102406:	83 ec 0c             	sub    $0xc,%esp
80102409:	81 c3 00 10 00 00    	add    $0x1000,%ebx
8010240f:	50                   	push   %eax
80102410:	e8 cb fe ff ff       	call   801022e0 <kfree>
80102415:	83 c4 10             	add    $0x10,%esp
80102418:	39 de                	cmp    %ebx,%esi
8010241a:	73 e4                	jae    80102400 <kinit1+0x40>
8010241c:	8d 65 f8             	lea    -0x8(%ebp),%esp
8010241f:	5b                   	pop    %ebx
80102420:	5e                   	pop    %esi
80102421:	5d                   	pop    %ebp
80102422:	c3                   	ret    
80102423:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80102429:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80102430 <kinit2>:
80102430:	55                   	push   %ebp
80102431:	89 e5                	mov    %esp,%ebp
80102433:	56                   	push   %esi
80102434:	53                   	push   %ebx
80102435:	8b 45 08             	mov    0x8(%ebp),%eax
80102438:	8b 75 0c             	mov    0xc(%ebp),%esi
8010243b:	8d 98 ff 0f 00 00    	lea    0xfff(%eax),%ebx
80102441:	81 e3 00 f0 ff ff    	and    $0xfffff000,%ebx
80102447:	81 c3 00 10 00 00    	add    $0x1000,%ebx
8010244d:	39 de                	cmp    %ebx,%esi
8010244f:	72 23                	jb     80102474 <kinit2+0x44>
80102451:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80102458:	8d 83 00 f0 ff ff    	lea    -0x1000(%ebx),%eax
8010245e:	83 ec 0c             	sub    $0xc,%esp
80102461:	81 c3 00 10 00 00    	add    $0x1000,%ebx
80102467:	50                   	push   %eax
80102468:	e8 73 fe ff ff       	call   801022e0 <kfree>
8010246d:	83 c4 10             	add    $0x10,%esp
80102470:	39 de                	cmp    %ebx,%esi
80102472:	73 e4                	jae    80102458 <kinit2+0x28>
80102474:	c7 05 74 26 11 80 01 	movl   $0x1,0x80112674
8010247b:	00 00 00 
8010247e:	8d 65 f8             	lea    -0x8(%ebp),%esp
80102481:	5b                   	pop    %ebx
80102482:	5e                   	pop    %esi
80102483:	5d                   	pop    %ebp
80102484:	c3                   	ret    
80102485:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80102489:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80102490 <kalloc>:
80102490:	55                   	push   %ebp
80102491:	89 e5                	mov    %esp,%ebp
80102493:	53                   	push   %ebx
80102494:	83 ec 04             	sub    $0x4,%esp
80102497:	a1 74 26 11 80       	mov    0x80112674,%eax
8010249c:	85 c0                	test   %eax,%eax
8010249e:	75 30                	jne    801024d0 <kalloc+0x40>
801024a0:	8b 1d 78 26 11 80    	mov    0x80112678,%ebx
801024a6:	85 db                	test   %ebx,%ebx
801024a8:	74 1c                	je     801024c6 <kalloc+0x36>
801024aa:	8b 13                	mov    (%ebx),%edx
801024ac:	89 15 78 26 11 80    	mov    %edx,0x80112678
801024b2:	85 c0                	test   %eax,%eax
801024b4:	74 10                	je     801024c6 <kalloc+0x36>
801024b6:	83 ec 0c             	sub    $0xc,%esp
801024b9:	68 40 26 11 80       	push   $0x80112640
801024be:	e8 cd 23 00 00       	call   80104890 <release>
801024c3:	83 c4 10             	add    $0x10,%esp
801024c6:	89 d8                	mov    %ebx,%eax
801024c8:	8b 5d fc             	mov    -0x4(%ebp),%ebx
801024cb:	c9                   	leave  
801024cc:	c3                   	ret    
801024cd:	8d 76 00             	lea    0x0(%esi),%esi
801024d0:	83 ec 0c             	sub    $0xc,%esp
801024d3:	68 40 26 11 80       	push   $0x80112640
801024d8:	e8 93 22 00 00       	call   80104770 <acquire>
801024dd:	8b 1d 78 26 11 80    	mov    0x80112678,%ebx
801024e3:	83 c4 10             	add    $0x10,%esp
801024e6:	a1 74 26 11 80       	mov    0x80112674,%eax
801024eb:	85 db                	test   %ebx,%ebx
801024ed:	75 bb                	jne    801024aa <kalloc+0x1a>
801024ef:	eb c1                	jmp    801024b2 <kalloc+0x22>
801024f1:	66 90                	xchg   %ax,%ax
801024f3:	66 90                	xchg   %ax,%ax
801024f5:	66 90                	xchg   %ax,%ax
801024f7:	66 90                	xchg   %ax,%ax
801024f9:	66 90                	xchg   %ax,%ax
801024fb:	66 90                	xchg   %ax,%ax
801024fd:	66 90                	xchg   %ax,%ax
801024ff:	90                   	nop

80102500 <kbdgetc>:
80102500:	55                   	push   %ebp
80102501:	ba 64 00 00 00       	mov    $0x64,%edx
80102506:	89 e5                	mov    %esp,%ebp
80102508:	ec                   	in     (%dx),%al
80102509:	a8 01                	test   $0x1,%al
8010250b:	0f 84 af 00 00 00    	je     801025c0 <kbdgetc+0xc0>
80102511:	ba 60 00 00 00       	mov    $0x60,%edx
80102516:	ec                   	in     (%dx),%al
80102517:	0f b6 d0             	movzbl %al,%edx
8010251a:	81 fa e0 00 00 00    	cmp    $0xe0,%edx
80102520:	74 7e                	je     801025a0 <kbdgetc+0xa0>
80102522:	84 c0                	test   %al,%al
80102524:	8b 0d b4 a5 10 80    	mov    0x8010a5b4,%ecx
8010252a:	79 24                	jns    80102550 <kbdgetc+0x50>
8010252c:	f6 c1 40             	test   $0x40,%cl
8010252f:	75 05                	jne    80102536 <kbdgetc+0x36>
80102531:	89 c2                	mov    %eax,%edx
80102533:	83 e2 7f             	and    $0x7f,%edx
80102536:	0f b6 82 20 78 10 80 	movzbl -0x7fef87e0(%edx),%eax
8010253d:	83 c8 40             	or     $0x40,%eax
80102540:	0f b6 c0             	movzbl %al,%eax
80102543:	f7 d0                	not    %eax
80102545:	21 c8                	and    %ecx,%eax
80102547:	a3 b4 a5 10 80       	mov    %eax,0x8010a5b4
8010254c:	31 c0                	xor    %eax,%eax
8010254e:	5d                   	pop    %ebp
8010254f:	c3                   	ret    
80102550:	f6 c1 40             	test   $0x40,%cl
80102553:	74 09                	je     8010255e <kbdgetc+0x5e>
80102555:	83 c8 80             	or     $0xffffff80,%eax
80102558:	83 e1 bf             	and    $0xffffffbf,%ecx
8010255b:	0f b6 d0             	movzbl %al,%edx
8010255e:	0f b6 82 20 78 10 80 	movzbl -0x7fef87e0(%edx),%eax
80102565:	09 c1                	or     %eax,%ecx
80102567:	0f b6 82 20 77 10 80 	movzbl -0x7fef88e0(%edx),%eax
8010256e:	31 c1                	xor    %eax,%ecx
80102570:	89 c8                	mov    %ecx,%eax
80102572:	89 0d b4 a5 10 80    	mov    %ecx,0x8010a5b4
80102578:	83 e0 03             	and    $0x3,%eax
8010257b:	83 e1 08             	and    $0x8,%ecx
8010257e:	8b 04 85 00 77 10 80 	mov    -0x7fef8900(,%eax,4),%eax
80102585:	0f b6 04 10          	movzbl (%eax,%edx,1),%eax
80102589:	74 c3                	je     8010254e <kbdgetc+0x4e>
8010258b:	8d 50 9f             	lea    -0x61(%eax),%edx
8010258e:	83 fa 19             	cmp    $0x19,%edx
80102591:	77 1d                	ja     801025b0 <kbdgetc+0xb0>
80102593:	83 e8 20             	sub    $0x20,%eax
80102596:	5d                   	pop    %ebp
80102597:	c3                   	ret    
80102598:	90                   	nop
80102599:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801025a0:	31 c0                	xor    %eax,%eax
801025a2:	83 0d b4 a5 10 80 40 	orl    $0x40,0x8010a5b4
801025a9:	5d                   	pop    %ebp
801025aa:	c3                   	ret    
801025ab:	90                   	nop
801025ac:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
801025b0:	8d 48 bf             	lea    -0x41(%eax),%ecx
801025b3:	8d 50 20             	lea    0x20(%eax),%edx
801025b6:	5d                   	pop    %ebp
801025b7:	83 f9 19             	cmp    $0x19,%ecx
801025ba:	0f 46 c2             	cmovbe %edx,%eax
801025bd:	c3                   	ret    
801025be:	66 90                	xchg   %ax,%ax
801025c0:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
801025c5:	5d                   	pop    %ebp
801025c6:	c3                   	ret    
801025c7:	89 f6                	mov    %esi,%esi
801025c9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801025d0 <kbdintr>:
801025d0:	55                   	push   %ebp
801025d1:	89 e5                	mov    %esp,%ebp
801025d3:	83 ec 14             	sub    $0x14,%esp
801025d6:	68 00 25 10 80       	push   $0x80102500
801025db:	e8 10 e2 ff ff       	call   801007f0 <consoleintr>
801025e0:	83 c4 10             	add    $0x10,%esp
801025e3:	c9                   	leave  
801025e4:	c3                   	ret    
801025e5:	66 90                	xchg   %ax,%ax
801025e7:	66 90                	xchg   %ax,%ax
801025e9:	66 90                	xchg   %ax,%ax
801025eb:	66 90                	xchg   %ax,%ax
801025ed:	66 90                	xchg   %ax,%ax
801025ef:	90                   	nop

801025f0 <lapicinit>:
801025f0:	a1 7c 26 11 80       	mov    0x8011267c,%eax
801025f5:	55                   	push   %ebp
801025f6:	89 e5                	mov    %esp,%ebp
801025f8:	85 c0                	test   %eax,%eax
801025fa:	0f 84 c8 00 00 00    	je     801026c8 <lapicinit+0xd8>
80102600:	c7 80 f0 00 00 00 3f 	movl   $0x13f,0xf0(%eax)
80102607:	01 00 00 
8010260a:	8b 50 20             	mov    0x20(%eax),%edx
8010260d:	c7 80 e0 03 00 00 0b 	movl   $0xb,0x3e0(%eax)
80102614:	00 00 00 
80102617:	8b 50 20             	mov    0x20(%eax),%edx
8010261a:	c7 80 20 03 00 00 20 	movl   $0x20020,0x320(%eax)
80102621:	00 02 00 
80102624:	8b 50 20             	mov    0x20(%eax),%edx
80102627:	c7 80 80 03 00 00 80 	movl   $0x989680,0x380(%eax)
8010262e:	96 98 00 
80102631:	8b 50 20             	mov    0x20(%eax),%edx
80102634:	c7 80 50 03 00 00 00 	movl   $0x10000,0x350(%eax)
8010263b:	00 01 00 
8010263e:	8b 50 20             	mov    0x20(%eax),%edx
80102641:	c7 80 60 03 00 00 00 	movl   $0x10000,0x360(%eax)
80102648:	00 01 00 
8010264b:	8b 50 20             	mov    0x20(%eax),%edx
8010264e:	8b 50 30             	mov    0x30(%eax),%edx
80102651:	c1 ea 10             	shr    $0x10,%edx
80102654:	80 fa 03             	cmp    $0x3,%dl
80102657:	77 77                	ja     801026d0 <lapicinit+0xe0>
80102659:	c7 80 70 03 00 00 33 	movl   $0x33,0x370(%eax)
80102660:	00 00 00 
80102663:	8b 50 20             	mov    0x20(%eax),%edx
80102666:	c7 80 80 02 00 00 00 	movl   $0x0,0x280(%eax)
8010266d:	00 00 00 
80102670:	8b 50 20             	mov    0x20(%eax),%edx
80102673:	c7 80 80 02 00 00 00 	movl   $0x0,0x280(%eax)
8010267a:	00 00 00 
8010267d:	8b 50 20             	mov    0x20(%eax),%edx
80102680:	c7 80 b0 00 00 00 00 	movl   $0x0,0xb0(%eax)
80102687:	00 00 00 
8010268a:	8b 50 20             	mov    0x20(%eax),%edx
8010268d:	c7 80 10 03 00 00 00 	movl   $0x0,0x310(%eax)
80102694:	00 00 00 
80102697:	8b 50 20             	mov    0x20(%eax),%edx
8010269a:	c7 80 00 03 00 00 00 	movl   $0x88500,0x300(%eax)
801026a1:	85 08 00 
801026a4:	8b 50 20             	mov    0x20(%eax),%edx
801026a7:	89 f6                	mov    %esi,%esi
801026a9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
801026b0:	8b 90 00 03 00 00    	mov    0x300(%eax),%edx
801026b6:	80 e6 10             	and    $0x10,%dh
801026b9:	75 f5                	jne    801026b0 <lapicinit+0xc0>
801026bb:	c7 80 80 00 00 00 00 	movl   $0x0,0x80(%eax)
801026c2:	00 00 00 
801026c5:	8b 40 20             	mov    0x20(%eax),%eax
801026c8:	5d                   	pop    %ebp
801026c9:	c3                   	ret    
801026ca:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
801026d0:	c7 80 40 03 00 00 00 	movl   $0x10000,0x340(%eax)
801026d7:	00 01 00 
801026da:	8b 50 20             	mov    0x20(%eax),%edx
801026dd:	e9 77 ff ff ff       	jmp    80102659 <lapicinit+0x69>
801026e2:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801026e9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801026f0 <lapicid>:
801026f0:	a1 7c 26 11 80       	mov    0x8011267c,%eax
801026f5:	55                   	push   %ebp
801026f6:	89 e5                	mov    %esp,%ebp
801026f8:	85 c0                	test   %eax,%eax
801026fa:	74 0c                	je     80102708 <lapicid+0x18>
801026fc:	8b 40 20             	mov    0x20(%eax),%eax
801026ff:	5d                   	pop    %ebp
80102700:	c1 e8 18             	shr    $0x18,%eax
80102703:	c3                   	ret    
80102704:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80102708:	31 c0                	xor    %eax,%eax
8010270a:	5d                   	pop    %ebp
8010270b:	c3                   	ret    
8010270c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80102710 <lapiceoi>:
80102710:	a1 7c 26 11 80       	mov    0x8011267c,%eax
80102715:	55                   	push   %ebp
80102716:	89 e5                	mov    %esp,%ebp
80102718:	85 c0                	test   %eax,%eax
8010271a:	74 0d                	je     80102729 <lapiceoi+0x19>
8010271c:	c7 80 b0 00 00 00 00 	movl   $0x0,0xb0(%eax)
80102723:	00 00 00 
80102726:	8b 40 20             	mov    0x20(%eax),%eax
80102729:	5d                   	pop    %ebp
8010272a:	c3                   	ret    
8010272b:	90                   	nop
8010272c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80102730 <microdelay>:
80102730:	55                   	push   %ebp
80102731:	89 e5                	mov    %esp,%ebp
80102733:	5d                   	pop    %ebp
80102734:	c3                   	ret    
80102735:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80102739:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80102740 <lapicstartap>:
80102740:	55                   	push   %ebp
80102741:	ba 70 00 00 00       	mov    $0x70,%edx
80102746:	b8 0f 00 00 00       	mov    $0xf,%eax
8010274b:	89 e5                	mov    %esp,%ebp
8010274d:	53                   	push   %ebx
8010274e:	8b 4d 0c             	mov    0xc(%ebp),%ecx
80102751:	8b 5d 08             	mov    0x8(%ebp),%ebx
80102754:	ee                   	out    %al,(%dx)
80102755:	ba 71 00 00 00       	mov    $0x71,%edx
8010275a:	b8 0a 00 00 00       	mov    $0xa,%eax
8010275f:	ee                   	out    %al,(%dx)
80102760:	31 c0                	xor    %eax,%eax
80102762:	c1 e3 18             	shl    $0x18,%ebx
80102765:	66 a3 67 04 00 80    	mov    %ax,0x80000467
8010276b:	89 c8                	mov    %ecx,%eax
8010276d:	c1 e9 0c             	shr    $0xc,%ecx
80102770:	c1 e8 04             	shr    $0x4,%eax
80102773:	89 da                	mov    %ebx,%edx
80102775:	80 cd 06             	or     $0x6,%ch
80102778:	66 a3 69 04 00 80    	mov    %ax,0x80000469
8010277e:	a1 7c 26 11 80       	mov    0x8011267c,%eax
80102783:	89 98 10 03 00 00    	mov    %ebx,0x310(%eax)
80102789:	8b 58 20             	mov    0x20(%eax),%ebx
8010278c:	c7 80 00 03 00 00 00 	movl   $0xc500,0x300(%eax)
80102793:	c5 00 00 
80102796:	8b 58 20             	mov    0x20(%eax),%ebx
80102799:	c7 80 00 03 00 00 00 	movl   $0x8500,0x300(%eax)
801027a0:	85 00 00 
801027a3:	8b 58 20             	mov    0x20(%eax),%ebx
801027a6:	89 90 10 03 00 00    	mov    %edx,0x310(%eax)
801027ac:	8b 58 20             	mov    0x20(%eax),%ebx
801027af:	89 88 00 03 00 00    	mov    %ecx,0x300(%eax)
801027b5:	8b 58 20             	mov    0x20(%eax),%ebx
801027b8:	89 90 10 03 00 00    	mov    %edx,0x310(%eax)
801027be:	8b 50 20             	mov    0x20(%eax),%edx
801027c1:	89 88 00 03 00 00    	mov    %ecx,0x300(%eax)
801027c7:	8b 40 20             	mov    0x20(%eax),%eax
801027ca:	5b                   	pop    %ebx
801027cb:	5d                   	pop    %ebp
801027cc:	c3                   	ret    
801027cd:	8d 76 00             	lea    0x0(%esi),%esi

801027d0 <cmostime>:
801027d0:	55                   	push   %ebp
801027d1:	ba 70 00 00 00       	mov    $0x70,%edx
801027d6:	b8 0b 00 00 00       	mov    $0xb,%eax
801027db:	89 e5                	mov    %esp,%ebp
801027dd:	57                   	push   %edi
801027de:	56                   	push   %esi
801027df:	53                   	push   %ebx
801027e0:	83 ec 4c             	sub    $0x4c,%esp
801027e3:	ee                   	out    %al,(%dx)
801027e4:	ba 71 00 00 00       	mov    $0x71,%edx
801027e9:	ec                   	in     (%dx),%al
801027ea:	83 e0 04             	and    $0x4,%eax
801027ed:	8d 75 d0             	lea    -0x30(%ebp),%esi
801027f0:	31 db                	xor    %ebx,%ebx
801027f2:	88 45 b7             	mov    %al,-0x49(%ebp)
801027f5:	bf 70 00 00 00       	mov    $0x70,%edi
801027fa:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80102800:	89 d8                	mov    %ebx,%eax
80102802:	89 fa                	mov    %edi,%edx
80102804:	ee                   	out    %al,(%dx)
80102805:	b9 71 00 00 00       	mov    $0x71,%ecx
8010280a:	89 ca                	mov    %ecx,%edx
8010280c:	ec                   	in     (%dx),%al
8010280d:	0f b6 c0             	movzbl %al,%eax
80102810:	89 fa                	mov    %edi,%edx
80102812:	89 45 b8             	mov    %eax,-0x48(%ebp)
80102815:	b8 02 00 00 00       	mov    $0x2,%eax
8010281a:	ee                   	out    %al,(%dx)
8010281b:	89 ca                	mov    %ecx,%edx
8010281d:	ec                   	in     (%dx),%al
8010281e:	0f b6 c0             	movzbl %al,%eax
80102821:	89 fa                	mov    %edi,%edx
80102823:	89 45 bc             	mov    %eax,-0x44(%ebp)
80102826:	b8 04 00 00 00       	mov    $0x4,%eax
8010282b:	ee                   	out    %al,(%dx)
8010282c:	89 ca                	mov    %ecx,%edx
8010282e:	ec                   	in     (%dx),%al
8010282f:	0f b6 c0             	movzbl %al,%eax
80102832:	89 fa                	mov    %edi,%edx
80102834:	89 45 c0             	mov    %eax,-0x40(%ebp)
80102837:	b8 07 00 00 00       	mov    $0x7,%eax
8010283c:	ee                   	out    %al,(%dx)
8010283d:	89 ca                	mov    %ecx,%edx
8010283f:	ec                   	in     (%dx),%al
80102840:	0f b6 c0             	movzbl %al,%eax
80102843:	89 fa                	mov    %edi,%edx
80102845:	89 45 c4             	mov    %eax,-0x3c(%ebp)
80102848:	b8 08 00 00 00       	mov    $0x8,%eax
8010284d:	ee                   	out    %al,(%dx)
8010284e:	89 ca                	mov    %ecx,%edx
80102850:	ec                   	in     (%dx),%al
80102851:	0f b6 c0             	movzbl %al,%eax
80102854:	89 fa                	mov    %edi,%edx
80102856:	89 45 c8             	mov    %eax,-0x38(%ebp)
80102859:	b8 09 00 00 00       	mov    $0x9,%eax
8010285e:	ee                   	out    %al,(%dx)
8010285f:	89 ca                	mov    %ecx,%edx
80102861:	ec                   	in     (%dx),%al
80102862:	0f b6 c0             	movzbl %al,%eax
80102865:	89 fa                	mov    %edi,%edx
80102867:	89 45 cc             	mov    %eax,-0x34(%ebp)
8010286a:	b8 0a 00 00 00       	mov    $0xa,%eax
8010286f:	ee                   	out    %al,(%dx)
80102870:	89 ca                	mov    %ecx,%edx
80102872:	ec                   	in     (%dx),%al
80102873:	84 c0                	test   %al,%al
80102875:	78 89                	js     80102800 <cmostime+0x30>
80102877:	89 d8                	mov    %ebx,%eax
80102879:	89 fa                	mov    %edi,%edx
8010287b:	ee                   	out    %al,(%dx)
8010287c:	89 ca                	mov    %ecx,%edx
8010287e:	ec                   	in     (%dx),%al
8010287f:	0f b6 c0             	movzbl %al,%eax
80102882:	89 fa                	mov    %edi,%edx
80102884:	89 45 d0             	mov    %eax,-0x30(%ebp)
80102887:	b8 02 00 00 00       	mov    $0x2,%eax
8010288c:	ee                   	out    %al,(%dx)
8010288d:	89 ca                	mov    %ecx,%edx
8010288f:	ec                   	in     (%dx),%al
80102890:	0f b6 c0             	movzbl %al,%eax
80102893:	89 fa                	mov    %edi,%edx
80102895:	89 45 d4             	mov    %eax,-0x2c(%ebp)
80102898:	b8 04 00 00 00       	mov    $0x4,%eax
8010289d:	ee                   	out    %al,(%dx)
8010289e:	89 ca                	mov    %ecx,%edx
801028a0:	ec                   	in     (%dx),%al
801028a1:	0f b6 c0             	movzbl %al,%eax
801028a4:	89 fa                	mov    %edi,%edx
801028a6:	89 45 d8             	mov    %eax,-0x28(%ebp)
801028a9:	b8 07 00 00 00       	mov    $0x7,%eax
801028ae:	ee                   	out    %al,(%dx)
801028af:	89 ca                	mov    %ecx,%edx
801028b1:	ec                   	in     (%dx),%al
801028b2:	0f b6 c0             	movzbl %al,%eax
801028b5:	89 fa                	mov    %edi,%edx
801028b7:	89 45 dc             	mov    %eax,-0x24(%ebp)
801028ba:	b8 08 00 00 00       	mov    $0x8,%eax
801028bf:	ee                   	out    %al,(%dx)
801028c0:	89 ca                	mov    %ecx,%edx
801028c2:	ec                   	in     (%dx),%al
801028c3:	0f b6 c0             	movzbl %al,%eax
801028c6:	89 fa                	mov    %edi,%edx
801028c8:	89 45 e0             	mov    %eax,-0x20(%ebp)
801028cb:	b8 09 00 00 00       	mov    $0x9,%eax
801028d0:	ee                   	out    %al,(%dx)
801028d1:	89 ca                	mov    %ecx,%edx
801028d3:	ec                   	in     (%dx),%al
801028d4:	0f b6 c0             	movzbl %al,%eax
801028d7:	83 ec 04             	sub    $0x4,%esp
801028da:	89 45 e4             	mov    %eax,-0x1c(%ebp)
801028dd:	8d 45 b8             	lea    -0x48(%ebp),%eax
801028e0:	6a 18                	push   $0x18
801028e2:	56                   	push   %esi
801028e3:	50                   	push   %eax
801028e4:	e8 47 20 00 00       	call   80104930 <memcmp>
801028e9:	83 c4 10             	add    $0x10,%esp
801028ec:	85 c0                	test   %eax,%eax
801028ee:	0f 85 0c ff ff ff    	jne    80102800 <cmostime+0x30>
801028f4:	80 7d b7 00          	cmpb   $0x0,-0x49(%ebp)
801028f8:	75 78                	jne    80102972 <cmostime+0x1a2>
801028fa:	8b 45 b8             	mov    -0x48(%ebp),%eax
801028fd:	89 c2                	mov    %eax,%edx
801028ff:	83 e0 0f             	and    $0xf,%eax
80102902:	c1 ea 04             	shr    $0x4,%edx
80102905:	8d 14 92             	lea    (%edx,%edx,4),%edx
80102908:	8d 04 50             	lea    (%eax,%edx,2),%eax
8010290b:	89 45 b8             	mov    %eax,-0x48(%ebp)
8010290e:	8b 45 bc             	mov    -0x44(%ebp),%eax
80102911:	89 c2                	mov    %eax,%edx
80102913:	83 e0 0f             	and    $0xf,%eax
80102916:	c1 ea 04             	shr    $0x4,%edx
80102919:	8d 14 92             	lea    (%edx,%edx,4),%edx
8010291c:	8d 04 50             	lea    (%eax,%edx,2),%eax
8010291f:	89 45 bc             	mov    %eax,-0x44(%ebp)
80102922:	8b 45 c0             	mov    -0x40(%ebp),%eax
80102925:	89 c2                	mov    %eax,%edx
80102927:	83 e0 0f             	and    $0xf,%eax
8010292a:	c1 ea 04             	shr    $0x4,%edx
8010292d:	8d 14 92             	lea    (%edx,%edx,4),%edx
80102930:	8d 04 50             	lea    (%eax,%edx,2),%eax
80102933:	89 45 c0             	mov    %eax,-0x40(%ebp)
80102936:	8b 45 c4             	mov    -0x3c(%ebp),%eax
80102939:	89 c2                	mov    %eax,%edx
8010293b:	83 e0 0f             	and    $0xf,%eax
8010293e:	c1 ea 04             	shr    $0x4,%edx
80102941:	8d 14 92             	lea    (%edx,%edx,4),%edx
80102944:	8d 04 50             	lea    (%eax,%edx,2),%eax
80102947:	89 45 c4             	mov    %eax,-0x3c(%ebp)
8010294a:	8b 45 c8             	mov    -0x38(%ebp),%eax
8010294d:	89 c2                	mov    %eax,%edx
8010294f:	83 e0 0f             	and    $0xf,%eax
80102952:	c1 ea 04             	shr    $0x4,%edx
80102955:	8d 14 92             	lea    (%edx,%edx,4),%edx
80102958:	8d 04 50             	lea    (%eax,%edx,2),%eax
8010295b:	89 45 c8             	mov    %eax,-0x38(%ebp)
8010295e:	8b 45 cc             	mov    -0x34(%ebp),%eax
80102961:	89 c2                	mov    %eax,%edx
80102963:	83 e0 0f             	and    $0xf,%eax
80102966:	c1 ea 04             	shr    $0x4,%edx
80102969:	8d 14 92             	lea    (%edx,%edx,4),%edx
8010296c:	8d 04 50             	lea    (%eax,%edx,2),%eax
8010296f:	89 45 cc             	mov    %eax,-0x34(%ebp)
80102972:	8b 75 08             	mov    0x8(%ebp),%esi
80102975:	8b 45 b8             	mov    -0x48(%ebp),%eax
80102978:	89 06                	mov    %eax,(%esi)
8010297a:	8b 45 bc             	mov    -0x44(%ebp),%eax
8010297d:	89 46 04             	mov    %eax,0x4(%esi)
80102980:	8b 45 c0             	mov    -0x40(%ebp),%eax
80102983:	89 46 08             	mov    %eax,0x8(%esi)
80102986:	8b 45 c4             	mov    -0x3c(%ebp),%eax
80102989:	89 46 0c             	mov    %eax,0xc(%esi)
8010298c:	8b 45 c8             	mov    -0x38(%ebp),%eax
8010298f:	89 46 10             	mov    %eax,0x10(%esi)
80102992:	8b 45 cc             	mov    -0x34(%ebp),%eax
80102995:	89 46 14             	mov    %eax,0x14(%esi)
80102998:	81 46 14 d0 07 00 00 	addl   $0x7d0,0x14(%esi)
8010299f:	8d 65 f4             	lea    -0xc(%ebp),%esp
801029a2:	5b                   	pop    %ebx
801029a3:	5e                   	pop    %esi
801029a4:	5f                   	pop    %edi
801029a5:	5d                   	pop    %ebp
801029a6:	c3                   	ret    
801029a7:	66 90                	xchg   %ax,%ax
801029a9:	66 90                	xchg   %ax,%ax
801029ab:	66 90                	xchg   %ax,%ax
801029ad:	66 90                	xchg   %ax,%ax
801029af:	90                   	nop

801029b0 <install_trans>:
801029b0:	8b 0d c8 26 11 80    	mov    0x801126c8,%ecx
801029b6:	85 c9                	test   %ecx,%ecx
801029b8:	0f 8e 85 00 00 00    	jle    80102a43 <install_trans+0x93>
801029be:	55                   	push   %ebp
801029bf:	89 e5                	mov    %esp,%ebp
801029c1:	57                   	push   %edi
801029c2:	56                   	push   %esi
801029c3:	53                   	push   %ebx
801029c4:	31 db                	xor    %ebx,%ebx
801029c6:	83 ec 0c             	sub    $0xc,%esp
801029c9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801029d0:	a1 b4 26 11 80       	mov    0x801126b4,%eax
801029d5:	83 ec 08             	sub    $0x8,%esp
801029d8:	01 d8                	add    %ebx,%eax
801029da:	83 c0 01             	add    $0x1,%eax
801029dd:	50                   	push   %eax
801029de:	ff 35 c4 26 11 80    	pushl  0x801126c4
801029e4:	e8 e7 d6 ff ff       	call   801000d0 <bread>
801029e9:	89 c7                	mov    %eax,%edi
801029eb:	58                   	pop    %eax
801029ec:	5a                   	pop    %edx
801029ed:	ff 34 9d cc 26 11 80 	pushl  -0x7feed934(,%ebx,4)
801029f4:	ff 35 c4 26 11 80    	pushl  0x801126c4
801029fa:	83 c3 01             	add    $0x1,%ebx
801029fd:	e8 ce d6 ff ff       	call   801000d0 <bread>
80102a02:	89 c6                	mov    %eax,%esi
80102a04:	8d 47 5c             	lea    0x5c(%edi),%eax
80102a07:	83 c4 0c             	add    $0xc,%esp
80102a0a:	68 00 02 00 00       	push   $0x200
80102a0f:	50                   	push   %eax
80102a10:	8d 46 5c             	lea    0x5c(%esi),%eax
80102a13:	50                   	push   %eax
80102a14:	e8 77 1f 00 00       	call   80104990 <memmove>
80102a19:	89 34 24             	mov    %esi,(%esp)
80102a1c:	e8 7f d7 ff ff       	call   801001a0 <bwrite>
80102a21:	89 3c 24             	mov    %edi,(%esp)
80102a24:	e8 b7 d7 ff ff       	call   801001e0 <brelse>
80102a29:	89 34 24             	mov    %esi,(%esp)
80102a2c:	e8 af d7 ff ff       	call   801001e0 <brelse>
80102a31:	83 c4 10             	add    $0x10,%esp
80102a34:	39 1d c8 26 11 80    	cmp    %ebx,0x801126c8
80102a3a:	7f 94                	jg     801029d0 <install_trans+0x20>
80102a3c:	8d 65 f4             	lea    -0xc(%ebp),%esp
80102a3f:	5b                   	pop    %ebx
80102a40:	5e                   	pop    %esi
80102a41:	5f                   	pop    %edi
80102a42:	5d                   	pop    %ebp
80102a43:	f3 c3                	repz ret 
80102a45:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80102a49:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80102a50 <write_head>:
80102a50:	55                   	push   %ebp
80102a51:	89 e5                	mov    %esp,%ebp
80102a53:	53                   	push   %ebx
80102a54:	83 ec 0c             	sub    $0xc,%esp
80102a57:	ff 35 b4 26 11 80    	pushl  0x801126b4
80102a5d:	ff 35 c4 26 11 80    	pushl  0x801126c4
80102a63:	e8 68 d6 ff ff       	call   801000d0 <bread>
80102a68:	8b 0d c8 26 11 80    	mov    0x801126c8,%ecx
80102a6e:	83 c4 10             	add    $0x10,%esp
80102a71:	89 c3                	mov    %eax,%ebx
80102a73:	85 c9                	test   %ecx,%ecx
80102a75:	89 48 5c             	mov    %ecx,0x5c(%eax)
80102a78:	7e 1f                	jle    80102a99 <write_head+0x49>
80102a7a:	8d 04 8d 00 00 00 00 	lea    0x0(,%ecx,4),%eax
80102a81:	31 d2                	xor    %edx,%edx
80102a83:	90                   	nop
80102a84:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80102a88:	8b 8a cc 26 11 80    	mov    -0x7feed934(%edx),%ecx
80102a8e:	89 4c 13 60          	mov    %ecx,0x60(%ebx,%edx,1)
80102a92:	83 c2 04             	add    $0x4,%edx
80102a95:	39 c2                	cmp    %eax,%edx
80102a97:	75 ef                	jne    80102a88 <write_head+0x38>
80102a99:	83 ec 0c             	sub    $0xc,%esp
80102a9c:	53                   	push   %ebx
80102a9d:	e8 fe d6 ff ff       	call   801001a0 <bwrite>
80102aa2:	89 1c 24             	mov    %ebx,(%esp)
80102aa5:	e8 36 d7 ff ff       	call   801001e0 <brelse>
80102aaa:	8b 5d fc             	mov    -0x4(%ebp),%ebx
80102aad:	c9                   	leave  
80102aae:	c3                   	ret    
80102aaf:	90                   	nop

80102ab0 <initlog>:
80102ab0:	55                   	push   %ebp
80102ab1:	89 e5                	mov    %esp,%ebp
80102ab3:	53                   	push   %ebx
80102ab4:	83 ec 2c             	sub    $0x2c,%esp
80102ab7:	8b 5d 08             	mov    0x8(%ebp),%ebx
80102aba:	68 20 79 10 80       	push   $0x80107920
80102abf:	68 80 26 11 80       	push   $0x80112680
80102ac4:	e8 a7 1b 00 00       	call   80104670 <initlock>
80102ac9:	58                   	pop    %eax
80102aca:	8d 45 dc             	lea    -0x24(%ebp),%eax
80102acd:	5a                   	pop    %edx
80102ace:	50                   	push   %eax
80102acf:	53                   	push   %ebx
80102ad0:	e8 db e8 ff ff       	call   801013b0 <readsb>
80102ad5:	8b 55 e8             	mov    -0x18(%ebp),%edx
80102ad8:	8b 45 ec             	mov    -0x14(%ebp),%eax
80102adb:	59                   	pop    %ecx
80102adc:	89 1d c4 26 11 80    	mov    %ebx,0x801126c4
80102ae2:	89 15 b8 26 11 80    	mov    %edx,0x801126b8
80102ae8:	a3 b4 26 11 80       	mov    %eax,0x801126b4
80102aed:	5a                   	pop    %edx
80102aee:	50                   	push   %eax
80102aef:	53                   	push   %ebx
80102af0:	e8 db d5 ff ff       	call   801000d0 <bread>
80102af5:	8b 48 5c             	mov    0x5c(%eax),%ecx
80102af8:	83 c4 10             	add    $0x10,%esp
80102afb:	85 c9                	test   %ecx,%ecx
80102afd:	89 0d c8 26 11 80    	mov    %ecx,0x801126c8
80102b03:	7e 1c                	jle    80102b21 <initlog+0x71>
80102b05:	8d 1c 8d 00 00 00 00 	lea    0x0(,%ecx,4),%ebx
80102b0c:	31 d2                	xor    %edx,%edx
80102b0e:	66 90                	xchg   %ax,%ax
80102b10:	8b 4c 10 60          	mov    0x60(%eax,%edx,1),%ecx
80102b14:	83 c2 04             	add    $0x4,%edx
80102b17:	89 8a c8 26 11 80    	mov    %ecx,-0x7feed938(%edx)
80102b1d:	39 da                	cmp    %ebx,%edx
80102b1f:	75 ef                	jne    80102b10 <initlog+0x60>
80102b21:	83 ec 0c             	sub    $0xc,%esp
80102b24:	50                   	push   %eax
80102b25:	e8 b6 d6 ff ff       	call   801001e0 <brelse>
80102b2a:	e8 81 fe ff ff       	call   801029b0 <install_trans>
80102b2f:	c7 05 c8 26 11 80 00 	movl   $0x0,0x801126c8
80102b36:	00 00 00 
80102b39:	e8 12 ff ff ff       	call   80102a50 <write_head>
80102b3e:	8b 5d fc             	mov    -0x4(%ebp),%ebx
80102b41:	c9                   	leave  
80102b42:	c3                   	ret    
80102b43:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80102b49:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80102b50 <begin_op>:
80102b50:	55                   	push   %ebp
80102b51:	89 e5                	mov    %esp,%ebp
80102b53:	83 ec 14             	sub    $0x14,%esp
80102b56:	68 80 26 11 80       	push   $0x80112680
80102b5b:	e8 10 1c 00 00       	call   80104770 <acquire>
80102b60:	83 c4 10             	add    $0x10,%esp
80102b63:	eb 18                	jmp    80102b7d <begin_op+0x2d>
80102b65:	8d 76 00             	lea    0x0(%esi),%esi
80102b68:	83 ec 08             	sub    $0x8,%esp
80102b6b:	68 80 26 11 80       	push   $0x80112680
80102b70:	68 80 26 11 80       	push   $0x80112680
80102b75:	e8 26 14 00 00       	call   80103fa0 <sleep>
80102b7a:	83 c4 10             	add    $0x10,%esp
80102b7d:	a1 c0 26 11 80       	mov    0x801126c0,%eax
80102b82:	85 c0                	test   %eax,%eax
80102b84:	75 e2                	jne    80102b68 <begin_op+0x18>
80102b86:	a1 bc 26 11 80       	mov    0x801126bc,%eax
80102b8b:	8b 15 c8 26 11 80    	mov    0x801126c8,%edx
80102b91:	83 c0 01             	add    $0x1,%eax
80102b94:	8d 0c 80             	lea    (%eax,%eax,4),%ecx
80102b97:	8d 14 4a             	lea    (%edx,%ecx,2),%edx
80102b9a:	83 fa 1e             	cmp    $0x1e,%edx
80102b9d:	7f c9                	jg     80102b68 <begin_op+0x18>
80102b9f:	83 ec 0c             	sub    $0xc,%esp
80102ba2:	a3 bc 26 11 80       	mov    %eax,0x801126bc
80102ba7:	68 80 26 11 80       	push   $0x80112680
80102bac:	e8 df 1c 00 00       	call   80104890 <release>
80102bb1:	83 c4 10             	add    $0x10,%esp
80102bb4:	c9                   	leave  
80102bb5:	c3                   	ret    
80102bb6:	8d 76 00             	lea    0x0(%esi),%esi
80102bb9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80102bc0 <end_op>:
80102bc0:	55                   	push   %ebp
80102bc1:	89 e5                	mov    %esp,%ebp
80102bc3:	57                   	push   %edi
80102bc4:	56                   	push   %esi
80102bc5:	53                   	push   %ebx
80102bc6:	83 ec 18             	sub    $0x18,%esp
80102bc9:	68 80 26 11 80       	push   $0x80112680
80102bce:	e8 9d 1b 00 00       	call   80104770 <acquire>
80102bd3:	a1 bc 26 11 80       	mov    0x801126bc,%eax
80102bd8:	8b 1d c0 26 11 80    	mov    0x801126c0,%ebx
80102bde:	83 c4 10             	add    $0x10,%esp
80102be1:	83 e8 01             	sub    $0x1,%eax
80102be4:	85 db                	test   %ebx,%ebx
80102be6:	a3 bc 26 11 80       	mov    %eax,0x801126bc
80102beb:	0f 85 23 01 00 00    	jne    80102d14 <end_op+0x154>
80102bf1:	85 c0                	test   %eax,%eax
80102bf3:	0f 85 f7 00 00 00    	jne    80102cf0 <end_op+0x130>
80102bf9:	83 ec 0c             	sub    $0xc,%esp
80102bfc:	c7 05 c0 26 11 80 01 	movl   $0x1,0x801126c0
80102c03:	00 00 00 
80102c06:	31 db                	xor    %ebx,%ebx
80102c08:	68 80 26 11 80       	push   $0x80112680
80102c0d:	e8 7e 1c 00 00       	call   80104890 <release>
80102c12:	8b 0d c8 26 11 80    	mov    0x801126c8,%ecx
80102c18:	83 c4 10             	add    $0x10,%esp
80102c1b:	85 c9                	test   %ecx,%ecx
80102c1d:	0f 8e 8a 00 00 00    	jle    80102cad <end_op+0xed>
80102c23:	90                   	nop
80102c24:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80102c28:	a1 b4 26 11 80       	mov    0x801126b4,%eax
80102c2d:	83 ec 08             	sub    $0x8,%esp
80102c30:	01 d8                	add    %ebx,%eax
80102c32:	83 c0 01             	add    $0x1,%eax
80102c35:	50                   	push   %eax
80102c36:	ff 35 c4 26 11 80    	pushl  0x801126c4
80102c3c:	e8 8f d4 ff ff       	call   801000d0 <bread>
80102c41:	89 c6                	mov    %eax,%esi
80102c43:	58                   	pop    %eax
80102c44:	5a                   	pop    %edx
80102c45:	ff 34 9d cc 26 11 80 	pushl  -0x7feed934(,%ebx,4)
80102c4c:	ff 35 c4 26 11 80    	pushl  0x801126c4
80102c52:	83 c3 01             	add    $0x1,%ebx
80102c55:	e8 76 d4 ff ff       	call   801000d0 <bread>
80102c5a:	89 c7                	mov    %eax,%edi
80102c5c:	8d 40 5c             	lea    0x5c(%eax),%eax
80102c5f:	83 c4 0c             	add    $0xc,%esp
80102c62:	68 00 02 00 00       	push   $0x200
80102c67:	50                   	push   %eax
80102c68:	8d 46 5c             	lea    0x5c(%esi),%eax
80102c6b:	50                   	push   %eax
80102c6c:	e8 1f 1d 00 00       	call   80104990 <memmove>
80102c71:	89 34 24             	mov    %esi,(%esp)
80102c74:	e8 27 d5 ff ff       	call   801001a0 <bwrite>
80102c79:	89 3c 24             	mov    %edi,(%esp)
80102c7c:	e8 5f d5 ff ff       	call   801001e0 <brelse>
80102c81:	89 34 24             	mov    %esi,(%esp)
80102c84:	e8 57 d5 ff ff       	call   801001e0 <brelse>
80102c89:	83 c4 10             	add    $0x10,%esp
80102c8c:	3b 1d c8 26 11 80    	cmp    0x801126c8,%ebx
80102c92:	7c 94                	jl     80102c28 <end_op+0x68>
80102c94:	e8 b7 fd ff ff       	call   80102a50 <write_head>
80102c99:	e8 12 fd ff ff       	call   801029b0 <install_trans>
80102c9e:	c7 05 c8 26 11 80 00 	movl   $0x0,0x801126c8
80102ca5:	00 00 00 
80102ca8:	e8 a3 fd ff ff       	call   80102a50 <write_head>
80102cad:	83 ec 0c             	sub    $0xc,%esp
80102cb0:	68 80 26 11 80       	push   $0x80112680
80102cb5:	e8 b6 1a 00 00       	call   80104770 <acquire>
80102cba:	c7 04 24 80 26 11 80 	movl   $0x80112680,(%esp)
80102cc1:	c7 05 c0 26 11 80 00 	movl   $0x0,0x801126c0
80102cc8:	00 00 00 
80102ccb:	e8 90 14 00 00       	call   80104160 <wakeup>
80102cd0:	c7 04 24 80 26 11 80 	movl   $0x80112680,(%esp)
80102cd7:	e8 b4 1b 00 00       	call   80104890 <release>
80102cdc:	83 c4 10             	add    $0x10,%esp
80102cdf:	8d 65 f4             	lea    -0xc(%ebp),%esp
80102ce2:	5b                   	pop    %ebx
80102ce3:	5e                   	pop    %esi
80102ce4:	5f                   	pop    %edi
80102ce5:	5d                   	pop    %ebp
80102ce6:	c3                   	ret    
80102ce7:	89 f6                	mov    %esi,%esi
80102ce9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80102cf0:	83 ec 0c             	sub    $0xc,%esp
80102cf3:	68 80 26 11 80       	push   $0x80112680
80102cf8:	e8 63 14 00 00       	call   80104160 <wakeup>
80102cfd:	c7 04 24 80 26 11 80 	movl   $0x80112680,(%esp)
80102d04:	e8 87 1b 00 00       	call   80104890 <release>
80102d09:	83 c4 10             	add    $0x10,%esp
80102d0c:	8d 65 f4             	lea    -0xc(%ebp),%esp
80102d0f:	5b                   	pop    %ebx
80102d10:	5e                   	pop    %esi
80102d11:	5f                   	pop    %edi
80102d12:	5d                   	pop    %ebp
80102d13:	c3                   	ret    
80102d14:	83 ec 0c             	sub    $0xc,%esp
80102d17:	68 24 79 10 80       	push   $0x80107924
80102d1c:	e8 4f d6 ff ff       	call   80100370 <panic>
80102d21:	eb 0d                	jmp    80102d30 <log_write>
80102d23:	90                   	nop
80102d24:	90                   	nop
80102d25:	90                   	nop
80102d26:	90                   	nop
80102d27:	90                   	nop
80102d28:	90                   	nop
80102d29:	90                   	nop
80102d2a:	90                   	nop
80102d2b:	90                   	nop
80102d2c:	90                   	nop
80102d2d:	90                   	nop
80102d2e:	90                   	nop
80102d2f:	90                   	nop

80102d30 <log_write>:
80102d30:	55                   	push   %ebp
80102d31:	89 e5                	mov    %esp,%ebp
80102d33:	53                   	push   %ebx
80102d34:	83 ec 04             	sub    $0x4,%esp
80102d37:	8b 15 c8 26 11 80    	mov    0x801126c8,%edx
80102d3d:	8b 5d 08             	mov    0x8(%ebp),%ebx
80102d40:	83 fa 1d             	cmp    $0x1d,%edx
80102d43:	0f 8f 97 00 00 00    	jg     80102de0 <log_write+0xb0>
80102d49:	a1 b8 26 11 80       	mov    0x801126b8,%eax
80102d4e:	83 e8 01             	sub    $0x1,%eax
80102d51:	39 c2                	cmp    %eax,%edx
80102d53:	0f 8d 87 00 00 00    	jge    80102de0 <log_write+0xb0>
80102d59:	a1 bc 26 11 80       	mov    0x801126bc,%eax
80102d5e:	85 c0                	test   %eax,%eax
80102d60:	0f 8e 87 00 00 00    	jle    80102ded <log_write+0xbd>
80102d66:	83 ec 0c             	sub    $0xc,%esp
80102d69:	68 80 26 11 80       	push   $0x80112680
80102d6e:	e8 fd 19 00 00       	call   80104770 <acquire>
80102d73:	8b 15 c8 26 11 80    	mov    0x801126c8,%edx
80102d79:	83 c4 10             	add    $0x10,%esp
80102d7c:	83 fa 00             	cmp    $0x0,%edx
80102d7f:	7e 50                	jle    80102dd1 <log_write+0xa1>
80102d81:	8b 4b 08             	mov    0x8(%ebx),%ecx
80102d84:	31 c0                	xor    %eax,%eax
80102d86:	3b 0d cc 26 11 80    	cmp    0x801126cc,%ecx
80102d8c:	75 0b                	jne    80102d99 <log_write+0x69>
80102d8e:	eb 38                	jmp    80102dc8 <log_write+0x98>
80102d90:	39 0c 85 cc 26 11 80 	cmp    %ecx,-0x7feed934(,%eax,4)
80102d97:	74 2f                	je     80102dc8 <log_write+0x98>
80102d99:	83 c0 01             	add    $0x1,%eax
80102d9c:	39 d0                	cmp    %edx,%eax
80102d9e:	75 f0                	jne    80102d90 <log_write+0x60>
80102da0:	89 0c 95 cc 26 11 80 	mov    %ecx,-0x7feed934(,%edx,4)
80102da7:	83 c2 01             	add    $0x1,%edx
80102daa:	89 15 c8 26 11 80    	mov    %edx,0x801126c8
80102db0:	83 0b 04             	orl    $0x4,(%ebx)
80102db3:	c7 45 08 80 26 11 80 	movl   $0x80112680,0x8(%ebp)
80102dba:	8b 5d fc             	mov    -0x4(%ebp),%ebx
80102dbd:	c9                   	leave  
80102dbe:	e9 cd 1a 00 00       	jmp    80104890 <release>
80102dc3:	90                   	nop
80102dc4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80102dc8:	89 0c 85 cc 26 11 80 	mov    %ecx,-0x7feed934(,%eax,4)
80102dcf:	eb df                	jmp    80102db0 <log_write+0x80>
80102dd1:	8b 43 08             	mov    0x8(%ebx),%eax
80102dd4:	a3 cc 26 11 80       	mov    %eax,0x801126cc
80102dd9:	75 d5                	jne    80102db0 <log_write+0x80>
80102ddb:	eb ca                	jmp    80102da7 <log_write+0x77>
80102ddd:	8d 76 00             	lea    0x0(%esi),%esi
80102de0:	83 ec 0c             	sub    $0xc,%esp
80102de3:	68 33 79 10 80       	push   $0x80107933
80102de8:	e8 83 d5 ff ff       	call   80100370 <panic>
80102ded:	83 ec 0c             	sub    $0xc,%esp
80102df0:	68 49 79 10 80       	push   $0x80107949
80102df5:	e8 76 d5 ff ff       	call   80100370 <panic>
80102dfa:	66 90                	xchg   %ax,%ax
80102dfc:	66 90                	xchg   %ax,%ax
80102dfe:	66 90                	xchg   %ax,%ax

80102e00 <mpmain>:
80102e00:	55                   	push   %ebp
80102e01:	89 e5                	mov    %esp,%ebp
80102e03:	53                   	push   %ebx
80102e04:	83 ec 04             	sub    $0x4,%esp
80102e07:	e8 44 0a 00 00       	call   80103850 <cpuid>
80102e0c:	89 c3                	mov    %eax,%ebx
80102e0e:	e8 3d 0a 00 00       	call   80103850 <cpuid>
80102e13:	83 ec 04             	sub    $0x4,%esp
80102e16:	53                   	push   %ebx
80102e17:	50                   	push   %eax
80102e18:	68 64 79 10 80       	push   $0x80107964
80102e1d:	e8 3e d8 ff ff       	call   80100660 <cprintf>
80102e22:	e8 29 2e 00 00       	call   80105c50 <idtinit>
80102e27:	e8 a4 09 00 00       	call   801037d0 <mycpu>
80102e2c:	89 c2                	mov    %eax,%edx
80102e2e:	b8 01 00 00 00       	mov    $0x1,%eax
80102e33:	f0 87 82 a0 00 00 00 	lock xchg %eax,0xa0(%edx)
80102e3a:	e8 f1 0c 00 00       	call   80103b30 <scheduler>
80102e3f:	90                   	nop

80102e40 <mpenter>:
80102e40:	55                   	push   %ebp
80102e41:	89 e5                	mov    %esp,%ebp
80102e43:	83 ec 08             	sub    $0x8,%esp
80102e46:	e8 65 3f 00 00       	call   80106db0 <switchkvm>
80102e4b:	e8 60 3e 00 00       	call   80106cb0 <seginit>
80102e50:	e8 9b f7 ff ff       	call   801025f0 <lapicinit>
80102e55:	e8 a6 ff ff ff       	call   80102e00 <mpmain>
80102e5a:	66 90                	xchg   %ax,%ax
80102e5c:	66 90                	xchg   %ax,%ax
80102e5e:	66 90                	xchg   %ax,%ax

80102e60 <main>:
80102e60:	8d 4c 24 04          	lea    0x4(%esp),%ecx
80102e64:	83 e4 f0             	and    $0xfffffff0,%esp
80102e67:	ff 71 fc             	pushl  -0x4(%ecx)
80102e6a:	55                   	push   %ebp
80102e6b:	89 e5                	mov    %esp,%ebp
80102e6d:	53                   	push   %ebx
80102e6e:	51                   	push   %ecx
80102e6f:	bb 80 27 11 80       	mov    $0x80112780,%ebx
80102e74:	83 ec 08             	sub    $0x8,%esp
80102e77:	68 00 00 40 80       	push   $0x80400000
80102e7c:	68 a8 5b 11 80       	push   $0x80115ba8
80102e81:	e8 3a f5 ff ff       	call   801023c0 <kinit1>
80102e86:	e8 c5 43 00 00       	call   80107250 <kvmalloc>
80102e8b:	e8 70 01 00 00       	call   80103000 <mpinit>
80102e90:	e8 5b f7 ff ff       	call   801025f0 <lapicinit>
80102e95:	e8 16 3e 00 00       	call   80106cb0 <seginit>
80102e9a:	e8 31 03 00 00       	call   801031d0 <picinit>
80102e9f:	e8 4c f3 ff ff       	call   801021f0 <ioapicinit>
80102ea4:	e8 f7 da ff ff       	call   801009a0 <consoleinit>
80102ea9:	e8 d2 30 00 00       	call   80105f80 <uartinit>
80102eae:	e8 fd 08 00 00       	call   801037b0 <pinit>
80102eb3:	e8 f8 2c 00 00       	call   80105bb0 <tvinit>
80102eb8:	e8 83 d1 ff ff       	call   80100040 <binit>
80102ebd:	e8 8e de ff ff       	call   80100d50 <fileinit>
80102ec2:	e8 09 f1 ff ff       	call   80101fd0 <ideinit>
80102ec7:	83 c4 0c             	add    $0xc,%esp
80102eca:	68 8a 00 00 00       	push   $0x8a
80102ecf:	68 8c a4 10 80       	push   $0x8010a48c
80102ed4:	68 00 70 00 80       	push   $0x80007000
80102ed9:	e8 b2 1a 00 00       	call   80104990 <memmove>
80102ede:	69 05 00 2d 11 80 b0 	imul   $0xb0,0x80112d00,%eax
80102ee5:	00 00 00 
80102ee8:	83 c4 10             	add    $0x10,%esp
80102eeb:	05 80 27 11 80       	add    $0x80112780,%eax
80102ef0:	39 d8                	cmp    %ebx,%eax
80102ef2:	76 6f                	jbe    80102f63 <main+0x103>
80102ef4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80102ef8:	e8 d3 08 00 00       	call   801037d0 <mycpu>
80102efd:	39 d8                	cmp    %ebx,%eax
80102eff:	74 49                	je     80102f4a <main+0xea>
80102f01:	e8 8a f5 ff ff       	call   80102490 <kalloc>
80102f06:	05 00 10 00 00       	add    $0x1000,%eax
80102f0b:	c7 05 f8 6f 00 80 40 	movl   $0x80102e40,0x80006ff8
80102f12:	2e 10 80 
80102f15:	c7 05 f4 6f 00 80 00 	movl   $0x109000,0x80006ff4
80102f1c:	90 10 00 
80102f1f:	a3 fc 6f 00 80       	mov    %eax,0x80006ffc
80102f24:	0f b6 03             	movzbl (%ebx),%eax
80102f27:	83 ec 08             	sub    $0x8,%esp
80102f2a:	68 00 70 00 00       	push   $0x7000
80102f2f:	50                   	push   %eax
80102f30:	e8 0b f8 ff ff       	call   80102740 <lapicstartap>
80102f35:	83 c4 10             	add    $0x10,%esp
80102f38:	90                   	nop
80102f39:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80102f40:	8b 83 a0 00 00 00    	mov    0xa0(%ebx),%eax
80102f46:	85 c0                	test   %eax,%eax
80102f48:	74 f6                	je     80102f40 <main+0xe0>
80102f4a:	69 05 00 2d 11 80 b0 	imul   $0xb0,0x80112d00,%eax
80102f51:	00 00 00 
80102f54:	81 c3 b0 00 00 00    	add    $0xb0,%ebx
80102f5a:	05 80 27 11 80       	add    $0x80112780,%eax
80102f5f:	39 c3                	cmp    %eax,%ebx
80102f61:	72 95                	jb     80102ef8 <main+0x98>
80102f63:	83 ec 08             	sub    $0x8,%esp
80102f66:	68 00 00 00 8e       	push   $0x8e000000
80102f6b:	68 00 00 40 80       	push   $0x80400000
80102f70:	e8 bb f4 ff ff       	call   80102430 <kinit2>
80102f75:	e8 26 09 00 00       	call   801038a0 <userinit>
80102f7a:	e8 81 fe ff ff       	call   80102e00 <mpmain>
80102f7f:	90                   	nop

80102f80 <mpsearch1>:
80102f80:	55                   	push   %ebp
80102f81:	89 e5                	mov    %esp,%ebp
80102f83:	57                   	push   %edi
80102f84:	56                   	push   %esi
80102f85:	8d b0 00 00 00 80    	lea    -0x80000000(%eax),%esi
80102f8b:	53                   	push   %ebx
80102f8c:	8d 1c 16             	lea    (%esi,%edx,1),%ebx
80102f8f:	83 ec 0c             	sub    $0xc,%esp
80102f92:	39 de                	cmp    %ebx,%esi
80102f94:	73 48                	jae    80102fde <mpsearch1+0x5e>
80102f96:	8d 76 00             	lea    0x0(%esi),%esi
80102f99:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80102fa0:	83 ec 04             	sub    $0x4,%esp
80102fa3:	8d 7e 10             	lea    0x10(%esi),%edi
80102fa6:	6a 04                	push   $0x4
80102fa8:	68 78 79 10 80       	push   $0x80107978
80102fad:	56                   	push   %esi
80102fae:	e8 7d 19 00 00       	call   80104930 <memcmp>
80102fb3:	83 c4 10             	add    $0x10,%esp
80102fb6:	85 c0                	test   %eax,%eax
80102fb8:	75 1e                	jne    80102fd8 <mpsearch1+0x58>
80102fba:	8d 7e 10             	lea    0x10(%esi),%edi
80102fbd:	89 f2                	mov    %esi,%edx
80102fbf:	31 c9                	xor    %ecx,%ecx
80102fc1:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80102fc8:	0f b6 02             	movzbl (%edx),%eax
80102fcb:	83 c2 01             	add    $0x1,%edx
80102fce:	01 c1                	add    %eax,%ecx
80102fd0:	39 fa                	cmp    %edi,%edx
80102fd2:	75 f4                	jne    80102fc8 <mpsearch1+0x48>
80102fd4:	84 c9                	test   %cl,%cl
80102fd6:	74 10                	je     80102fe8 <mpsearch1+0x68>
80102fd8:	39 fb                	cmp    %edi,%ebx
80102fda:	89 fe                	mov    %edi,%esi
80102fdc:	77 c2                	ja     80102fa0 <mpsearch1+0x20>
80102fde:	8d 65 f4             	lea    -0xc(%ebp),%esp
80102fe1:	31 c0                	xor    %eax,%eax
80102fe3:	5b                   	pop    %ebx
80102fe4:	5e                   	pop    %esi
80102fe5:	5f                   	pop    %edi
80102fe6:	5d                   	pop    %ebp
80102fe7:	c3                   	ret    
80102fe8:	8d 65 f4             	lea    -0xc(%ebp),%esp
80102feb:	89 f0                	mov    %esi,%eax
80102fed:	5b                   	pop    %ebx
80102fee:	5e                   	pop    %esi
80102fef:	5f                   	pop    %edi
80102ff0:	5d                   	pop    %ebp
80102ff1:	c3                   	ret    
80102ff2:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80102ff9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80103000 <mpinit>:
80103000:	55                   	push   %ebp
80103001:	89 e5                	mov    %esp,%ebp
80103003:	57                   	push   %edi
80103004:	56                   	push   %esi
80103005:	53                   	push   %ebx
80103006:	83 ec 1c             	sub    $0x1c,%esp
80103009:	0f b6 05 0f 04 00 80 	movzbl 0x8000040f,%eax
80103010:	0f b6 15 0e 04 00 80 	movzbl 0x8000040e,%edx
80103017:	c1 e0 08             	shl    $0x8,%eax
8010301a:	09 d0                	or     %edx,%eax
8010301c:	c1 e0 04             	shl    $0x4,%eax
8010301f:	85 c0                	test   %eax,%eax
80103021:	75 1b                	jne    8010303e <mpinit+0x3e>
80103023:	0f b6 05 14 04 00 80 	movzbl 0x80000414,%eax
8010302a:	0f b6 15 13 04 00 80 	movzbl 0x80000413,%edx
80103031:	c1 e0 08             	shl    $0x8,%eax
80103034:	09 d0                	or     %edx,%eax
80103036:	c1 e0 0a             	shl    $0xa,%eax
80103039:	2d 00 04 00 00       	sub    $0x400,%eax
8010303e:	ba 00 04 00 00       	mov    $0x400,%edx
80103043:	e8 38 ff ff ff       	call   80102f80 <mpsearch1>
80103048:	85 c0                	test   %eax,%eax
8010304a:	89 45 e4             	mov    %eax,-0x1c(%ebp)
8010304d:	0f 84 37 01 00 00    	je     8010318a <mpinit+0x18a>
80103053:	8b 45 e4             	mov    -0x1c(%ebp),%eax
80103056:	8b 58 04             	mov    0x4(%eax),%ebx
80103059:	85 db                	test   %ebx,%ebx
8010305b:	0f 84 43 01 00 00    	je     801031a4 <mpinit+0x1a4>
80103061:	8d b3 00 00 00 80    	lea    -0x80000000(%ebx),%esi
80103067:	83 ec 04             	sub    $0x4,%esp
8010306a:	6a 04                	push   $0x4
8010306c:	68 7d 79 10 80       	push   $0x8010797d
80103071:	56                   	push   %esi
80103072:	e8 b9 18 00 00       	call   80104930 <memcmp>
80103077:	83 c4 10             	add    $0x10,%esp
8010307a:	85 c0                	test   %eax,%eax
8010307c:	0f 85 22 01 00 00    	jne    801031a4 <mpinit+0x1a4>
80103082:	0f b6 83 06 00 00 80 	movzbl -0x7ffffffa(%ebx),%eax
80103089:	3c 01                	cmp    $0x1,%al
8010308b:	74 08                	je     80103095 <mpinit+0x95>
8010308d:	3c 04                	cmp    $0x4,%al
8010308f:	0f 85 0f 01 00 00    	jne    801031a4 <mpinit+0x1a4>
80103095:	0f b7 bb 04 00 00 80 	movzwl -0x7ffffffc(%ebx),%edi
8010309c:	85 ff                	test   %edi,%edi
8010309e:	74 21                	je     801030c1 <mpinit+0xc1>
801030a0:	31 d2                	xor    %edx,%edx
801030a2:	31 c0                	xor    %eax,%eax
801030a4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
801030a8:	0f b6 8c 03 00 00 00 	movzbl -0x80000000(%ebx,%eax,1),%ecx
801030af:	80 
801030b0:	83 c0 01             	add    $0x1,%eax
801030b3:	01 ca                	add    %ecx,%edx
801030b5:	39 c7                	cmp    %eax,%edi
801030b7:	75 ef                	jne    801030a8 <mpinit+0xa8>
801030b9:	84 d2                	test   %dl,%dl
801030bb:	0f 85 e3 00 00 00    	jne    801031a4 <mpinit+0x1a4>
801030c1:	85 f6                	test   %esi,%esi
801030c3:	0f 84 db 00 00 00    	je     801031a4 <mpinit+0x1a4>
801030c9:	8b 83 24 00 00 80    	mov    -0x7fffffdc(%ebx),%eax
801030cf:	a3 7c 26 11 80       	mov    %eax,0x8011267c
801030d4:	0f b7 93 04 00 00 80 	movzwl -0x7ffffffc(%ebx),%edx
801030db:	8d 83 2c 00 00 80    	lea    -0x7fffffd4(%ebx),%eax
801030e1:	bb 01 00 00 00       	mov    $0x1,%ebx
801030e6:	01 d6                	add    %edx,%esi
801030e8:	90                   	nop
801030e9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801030f0:	39 c6                	cmp    %eax,%esi
801030f2:	76 23                	jbe    80103117 <mpinit+0x117>
801030f4:	0f b6 10             	movzbl (%eax),%edx
801030f7:	80 fa 04             	cmp    $0x4,%dl
801030fa:	0f 87 c0 00 00 00    	ja     801031c0 <mpinit+0x1c0>
80103100:	ff 24 95 bc 79 10 80 	jmp    *-0x7fef8644(,%edx,4)
80103107:	89 f6                	mov    %esi,%esi
80103109:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80103110:	83 c0 08             	add    $0x8,%eax
80103113:	39 c6                	cmp    %eax,%esi
80103115:	77 dd                	ja     801030f4 <mpinit+0xf4>
80103117:	85 db                	test   %ebx,%ebx
80103119:	0f 84 92 00 00 00    	je     801031b1 <mpinit+0x1b1>
8010311f:	8b 45 e4             	mov    -0x1c(%ebp),%eax
80103122:	80 78 0c 00          	cmpb   $0x0,0xc(%eax)
80103126:	74 15                	je     8010313d <mpinit+0x13d>
80103128:	ba 22 00 00 00       	mov    $0x22,%edx
8010312d:	b8 70 00 00 00       	mov    $0x70,%eax
80103132:	ee                   	out    %al,(%dx)
80103133:	ba 23 00 00 00       	mov    $0x23,%edx
80103138:	ec                   	in     (%dx),%al
80103139:	83 c8 01             	or     $0x1,%eax
8010313c:	ee                   	out    %al,(%dx)
8010313d:	8d 65 f4             	lea    -0xc(%ebp),%esp
80103140:	5b                   	pop    %ebx
80103141:	5e                   	pop    %esi
80103142:	5f                   	pop    %edi
80103143:	5d                   	pop    %ebp
80103144:	c3                   	ret    
80103145:	8d 76 00             	lea    0x0(%esi),%esi
80103148:	8b 0d 00 2d 11 80    	mov    0x80112d00,%ecx
8010314e:	83 f9 07             	cmp    $0x7,%ecx
80103151:	7f 19                	jg     8010316c <mpinit+0x16c>
80103153:	0f b6 50 01          	movzbl 0x1(%eax),%edx
80103157:	69 f9 b0 00 00 00    	imul   $0xb0,%ecx,%edi
8010315d:	83 c1 01             	add    $0x1,%ecx
80103160:	89 0d 00 2d 11 80    	mov    %ecx,0x80112d00
80103166:	88 97 80 27 11 80    	mov    %dl,-0x7feed880(%edi)
8010316c:	83 c0 14             	add    $0x14,%eax
8010316f:	e9 7c ff ff ff       	jmp    801030f0 <mpinit+0xf0>
80103174:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80103178:	0f b6 50 01          	movzbl 0x1(%eax),%edx
8010317c:	83 c0 08             	add    $0x8,%eax
8010317f:	88 15 60 27 11 80    	mov    %dl,0x80112760
80103185:	e9 66 ff ff ff       	jmp    801030f0 <mpinit+0xf0>
8010318a:	ba 00 00 01 00       	mov    $0x10000,%edx
8010318f:	b8 00 00 0f 00       	mov    $0xf0000,%eax
80103194:	e8 e7 fd ff ff       	call   80102f80 <mpsearch1>
80103199:	85 c0                	test   %eax,%eax
8010319b:	89 45 e4             	mov    %eax,-0x1c(%ebp)
8010319e:	0f 85 af fe ff ff    	jne    80103053 <mpinit+0x53>
801031a4:	83 ec 0c             	sub    $0xc,%esp
801031a7:	68 82 79 10 80       	push   $0x80107982
801031ac:	e8 bf d1 ff ff       	call   80100370 <panic>
801031b1:	83 ec 0c             	sub    $0xc,%esp
801031b4:	68 9c 79 10 80       	push   $0x8010799c
801031b9:	e8 b2 d1 ff ff       	call   80100370 <panic>
801031be:	66 90                	xchg   %ax,%ax
801031c0:	31 db                	xor    %ebx,%ebx
801031c2:	e9 30 ff ff ff       	jmp    801030f7 <mpinit+0xf7>
801031c7:	66 90                	xchg   %ax,%ax
801031c9:	66 90                	xchg   %ax,%ax
801031cb:	66 90                	xchg   %ax,%ax
801031cd:	66 90                	xchg   %ax,%ax
801031cf:	90                   	nop

801031d0 <picinit>:
#define IO_PIC2         0xA0    // Slave (IRQs 8-15)

// Don't use the 8259A interrupt controllers.  Xv6 assumes SMP hardware.
void
picinit(void)
{
801031d0:	55                   	push   %ebp
}

static inline void
outb(ushort port, uchar data)
{
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
801031d1:	ba 21 00 00 00       	mov    $0x21,%edx
801031d6:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
801031db:	89 e5                	mov    %esp,%ebp
801031dd:	ee                   	out    %al,(%dx)
801031de:	ba a1 00 00 00       	mov    $0xa1,%edx
801031e3:	ee                   	out    %al,(%dx)
  // mask all interrupts
  outb(IO_PIC1+1, 0xFF);
  outb(IO_PIC2+1, 0xFF);
}
801031e4:	5d                   	pop    %ebp
801031e5:	c3                   	ret    
801031e6:	66 90                	xchg   %ax,%ax
801031e8:	66 90                	xchg   %ax,%ax
801031ea:	66 90                	xchg   %ax,%ax
801031ec:	66 90                	xchg   %ax,%ax
801031ee:	66 90                	xchg   %ax,%ax

801031f0 <pipealloc>:
801031f0:	55                   	push   %ebp
801031f1:	89 e5                	mov    %esp,%ebp
801031f3:	57                   	push   %edi
801031f4:	56                   	push   %esi
801031f5:	53                   	push   %ebx
801031f6:	83 ec 0c             	sub    $0xc,%esp
801031f9:	8b 75 08             	mov    0x8(%ebp),%esi
801031fc:	8b 5d 0c             	mov    0xc(%ebp),%ebx
801031ff:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)
80103205:	c7 06 00 00 00 00    	movl   $0x0,(%esi)
8010320b:	e8 60 db ff ff       	call   80100d70 <filealloc>
80103210:	85 c0                	test   %eax,%eax
80103212:	89 06                	mov    %eax,(%esi)
80103214:	0f 84 a8 00 00 00    	je     801032c2 <pipealloc+0xd2>
8010321a:	e8 51 db ff ff       	call   80100d70 <filealloc>
8010321f:	85 c0                	test   %eax,%eax
80103221:	89 03                	mov    %eax,(%ebx)
80103223:	0f 84 87 00 00 00    	je     801032b0 <pipealloc+0xc0>
80103229:	e8 62 f2 ff ff       	call   80102490 <kalloc>
8010322e:	85 c0                	test   %eax,%eax
80103230:	89 c7                	mov    %eax,%edi
80103232:	0f 84 b0 00 00 00    	je     801032e8 <pipealloc+0xf8>
80103238:	83 ec 08             	sub    $0x8,%esp
8010323b:	c7 80 3c 02 00 00 01 	movl   $0x1,0x23c(%eax)
80103242:	00 00 00 
80103245:	c7 80 40 02 00 00 01 	movl   $0x1,0x240(%eax)
8010324c:	00 00 00 
8010324f:	c7 80 38 02 00 00 00 	movl   $0x0,0x238(%eax)
80103256:	00 00 00 
80103259:	c7 80 34 02 00 00 00 	movl   $0x0,0x234(%eax)
80103260:	00 00 00 
80103263:	68 d0 79 10 80       	push   $0x801079d0
80103268:	50                   	push   %eax
80103269:	e8 02 14 00 00       	call   80104670 <initlock>
8010326e:	8b 06                	mov    (%esi),%eax
80103270:	83 c4 10             	add    $0x10,%esp
80103273:	c7 00 01 00 00 00    	movl   $0x1,(%eax)
80103279:	8b 06                	mov    (%esi),%eax
8010327b:	c6 40 08 01          	movb   $0x1,0x8(%eax)
8010327f:	8b 06                	mov    (%esi),%eax
80103281:	c6 40 09 00          	movb   $0x0,0x9(%eax)
80103285:	8b 06                	mov    (%esi),%eax
80103287:	89 78 0c             	mov    %edi,0xc(%eax)
8010328a:	8b 03                	mov    (%ebx),%eax
8010328c:	c7 00 01 00 00 00    	movl   $0x1,(%eax)
80103292:	8b 03                	mov    (%ebx),%eax
80103294:	c6 40 08 00          	movb   $0x0,0x8(%eax)
80103298:	8b 03                	mov    (%ebx),%eax
8010329a:	c6 40 09 01          	movb   $0x1,0x9(%eax)
8010329e:	8b 03                	mov    (%ebx),%eax
801032a0:	89 78 0c             	mov    %edi,0xc(%eax)
801032a3:	8d 65 f4             	lea    -0xc(%ebp),%esp
801032a6:	31 c0                	xor    %eax,%eax
801032a8:	5b                   	pop    %ebx
801032a9:	5e                   	pop    %esi
801032aa:	5f                   	pop    %edi
801032ab:	5d                   	pop    %ebp
801032ac:	c3                   	ret    
801032ad:	8d 76 00             	lea    0x0(%esi),%esi
801032b0:	8b 06                	mov    (%esi),%eax
801032b2:	85 c0                	test   %eax,%eax
801032b4:	74 1e                	je     801032d4 <pipealloc+0xe4>
801032b6:	83 ec 0c             	sub    $0xc,%esp
801032b9:	50                   	push   %eax
801032ba:	e8 71 db ff ff       	call   80100e30 <fileclose>
801032bf:	83 c4 10             	add    $0x10,%esp
801032c2:	8b 03                	mov    (%ebx),%eax
801032c4:	85 c0                	test   %eax,%eax
801032c6:	74 0c                	je     801032d4 <pipealloc+0xe4>
801032c8:	83 ec 0c             	sub    $0xc,%esp
801032cb:	50                   	push   %eax
801032cc:	e8 5f db ff ff       	call   80100e30 <fileclose>
801032d1:	83 c4 10             	add    $0x10,%esp
801032d4:	8d 65 f4             	lea    -0xc(%ebp),%esp
801032d7:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
801032dc:	5b                   	pop    %ebx
801032dd:	5e                   	pop    %esi
801032de:	5f                   	pop    %edi
801032df:	5d                   	pop    %ebp
801032e0:	c3                   	ret    
801032e1:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801032e8:	8b 06                	mov    (%esi),%eax
801032ea:	85 c0                	test   %eax,%eax
801032ec:	75 c8                	jne    801032b6 <pipealloc+0xc6>
801032ee:	eb d2                	jmp    801032c2 <pipealloc+0xd2>

801032f0 <pipeclose>:
801032f0:	55                   	push   %ebp
801032f1:	89 e5                	mov    %esp,%ebp
801032f3:	56                   	push   %esi
801032f4:	53                   	push   %ebx
801032f5:	8b 5d 08             	mov    0x8(%ebp),%ebx
801032f8:	8b 75 0c             	mov    0xc(%ebp),%esi
801032fb:	83 ec 0c             	sub    $0xc,%esp
801032fe:	53                   	push   %ebx
801032ff:	e8 6c 14 00 00       	call   80104770 <acquire>
80103304:	83 c4 10             	add    $0x10,%esp
80103307:	85 f6                	test   %esi,%esi
80103309:	74 45                	je     80103350 <pipeclose+0x60>
8010330b:	8d 83 34 02 00 00    	lea    0x234(%ebx),%eax
80103311:	83 ec 0c             	sub    $0xc,%esp
80103314:	c7 83 40 02 00 00 00 	movl   $0x0,0x240(%ebx)
8010331b:	00 00 00 
8010331e:	50                   	push   %eax
8010331f:	e8 3c 0e 00 00       	call   80104160 <wakeup>
80103324:	83 c4 10             	add    $0x10,%esp
80103327:	8b 93 3c 02 00 00    	mov    0x23c(%ebx),%edx
8010332d:	85 d2                	test   %edx,%edx
8010332f:	75 0a                	jne    8010333b <pipeclose+0x4b>
80103331:	8b 83 40 02 00 00    	mov    0x240(%ebx),%eax
80103337:	85 c0                	test   %eax,%eax
80103339:	74 35                	je     80103370 <pipeclose+0x80>
8010333b:	89 5d 08             	mov    %ebx,0x8(%ebp)
8010333e:	8d 65 f8             	lea    -0x8(%ebp),%esp
80103341:	5b                   	pop    %ebx
80103342:	5e                   	pop    %esi
80103343:	5d                   	pop    %ebp
80103344:	e9 47 15 00 00       	jmp    80104890 <release>
80103349:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80103350:	8d 83 38 02 00 00    	lea    0x238(%ebx),%eax
80103356:	83 ec 0c             	sub    $0xc,%esp
80103359:	c7 83 3c 02 00 00 00 	movl   $0x0,0x23c(%ebx)
80103360:	00 00 00 
80103363:	50                   	push   %eax
80103364:	e8 f7 0d 00 00       	call   80104160 <wakeup>
80103369:	83 c4 10             	add    $0x10,%esp
8010336c:	eb b9                	jmp    80103327 <pipeclose+0x37>
8010336e:	66 90                	xchg   %ax,%ax
80103370:	83 ec 0c             	sub    $0xc,%esp
80103373:	53                   	push   %ebx
80103374:	e8 17 15 00 00       	call   80104890 <release>
80103379:	89 5d 08             	mov    %ebx,0x8(%ebp)
8010337c:	83 c4 10             	add    $0x10,%esp
8010337f:	8d 65 f8             	lea    -0x8(%ebp),%esp
80103382:	5b                   	pop    %ebx
80103383:	5e                   	pop    %esi
80103384:	5d                   	pop    %ebp
80103385:	e9 56 ef ff ff       	jmp    801022e0 <kfree>
8010338a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

80103390 <pipewrite>:
80103390:	55                   	push   %ebp
80103391:	89 e5                	mov    %esp,%ebp
80103393:	57                   	push   %edi
80103394:	56                   	push   %esi
80103395:	53                   	push   %ebx
80103396:	83 ec 28             	sub    $0x28,%esp
80103399:	8b 5d 08             	mov    0x8(%ebp),%ebx
8010339c:	53                   	push   %ebx
8010339d:	e8 ce 13 00 00       	call   80104770 <acquire>
801033a2:	8b 45 10             	mov    0x10(%ebp),%eax
801033a5:	83 c4 10             	add    $0x10,%esp
801033a8:	85 c0                	test   %eax,%eax
801033aa:	0f 8e b9 00 00 00    	jle    80103469 <pipewrite+0xd9>
801033b0:	8b 4d 0c             	mov    0xc(%ebp),%ecx
801033b3:	8b 83 38 02 00 00    	mov    0x238(%ebx),%eax
801033b9:	8d bb 34 02 00 00    	lea    0x234(%ebx),%edi
801033bf:	8d b3 38 02 00 00    	lea    0x238(%ebx),%esi
801033c5:	89 4d e4             	mov    %ecx,-0x1c(%ebp)
801033c8:	03 4d 10             	add    0x10(%ebp),%ecx
801033cb:	89 4d e0             	mov    %ecx,-0x20(%ebp)
801033ce:	8b 8b 34 02 00 00    	mov    0x234(%ebx),%ecx
801033d4:	8d 91 00 02 00 00    	lea    0x200(%ecx),%edx
801033da:	39 d0                	cmp    %edx,%eax
801033dc:	74 38                	je     80103416 <pipewrite+0x86>
801033de:	eb 59                	jmp    80103439 <pipewrite+0xa9>
801033e0:	e8 8b 04 00 00       	call   80103870 <myproc>
801033e5:	8b 48 24             	mov    0x24(%eax),%ecx
801033e8:	85 c9                	test   %ecx,%ecx
801033ea:	75 34                	jne    80103420 <pipewrite+0x90>
801033ec:	83 ec 0c             	sub    $0xc,%esp
801033ef:	57                   	push   %edi
801033f0:	e8 6b 0d 00 00       	call   80104160 <wakeup>
801033f5:	58                   	pop    %eax
801033f6:	5a                   	pop    %edx
801033f7:	53                   	push   %ebx
801033f8:	56                   	push   %esi
801033f9:	e8 a2 0b 00 00       	call   80103fa0 <sleep>
801033fe:	8b 83 34 02 00 00    	mov    0x234(%ebx),%eax
80103404:	8b 93 38 02 00 00    	mov    0x238(%ebx),%edx
8010340a:	83 c4 10             	add    $0x10,%esp
8010340d:	05 00 02 00 00       	add    $0x200,%eax
80103412:	39 c2                	cmp    %eax,%edx
80103414:	75 2a                	jne    80103440 <pipewrite+0xb0>
80103416:	8b 83 3c 02 00 00    	mov    0x23c(%ebx),%eax
8010341c:	85 c0                	test   %eax,%eax
8010341e:	75 c0                	jne    801033e0 <pipewrite+0x50>
80103420:	83 ec 0c             	sub    $0xc,%esp
80103423:	53                   	push   %ebx
80103424:	e8 67 14 00 00       	call   80104890 <release>
80103429:	83 c4 10             	add    $0x10,%esp
8010342c:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80103431:	8d 65 f4             	lea    -0xc(%ebp),%esp
80103434:	5b                   	pop    %ebx
80103435:	5e                   	pop    %esi
80103436:	5f                   	pop    %edi
80103437:	5d                   	pop    %ebp
80103438:	c3                   	ret    
80103439:	89 c2                	mov    %eax,%edx
8010343b:	90                   	nop
8010343c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80103440:	8b 4d e4             	mov    -0x1c(%ebp),%ecx
80103443:	8d 42 01             	lea    0x1(%edx),%eax
80103446:	83 45 e4 01          	addl   $0x1,-0x1c(%ebp)
8010344a:	81 e2 ff 01 00 00    	and    $0x1ff,%edx
80103450:	89 83 38 02 00 00    	mov    %eax,0x238(%ebx)
80103456:	0f b6 09             	movzbl (%ecx),%ecx
80103459:	88 4c 13 34          	mov    %cl,0x34(%ebx,%edx,1)
8010345d:	8b 4d e4             	mov    -0x1c(%ebp),%ecx
80103460:	3b 4d e0             	cmp    -0x20(%ebp),%ecx
80103463:	0f 85 65 ff ff ff    	jne    801033ce <pipewrite+0x3e>
80103469:	8d 83 34 02 00 00    	lea    0x234(%ebx),%eax
8010346f:	83 ec 0c             	sub    $0xc,%esp
80103472:	50                   	push   %eax
80103473:	e8 e8 0c 00 00       	call   80104160 <wakeup>
80103478:	89 1c 24             	mov    %ebx,(%esp)
8010347b:	e8 10 14 00 00       	call   80104890 <release>
80103480:	83 c4 10             	add    $0x10,%esp
80103483:	8b 45 10             	mov    0x10(%ebp),%eax
80103486:	eb a9                	jmp    80103431 <pipewrite+0xa1>
80103488:	90                   	nop
80103489:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

80103490 <piperead>:
80103490:	55                   	push   %ebp
80103491:	89 e5                	mov    %esp,%ebp
80103493:	57                   	push   %edi
80103494:	56                   	push   %esi
80103495:	53                   	push   %ebx
80103496:	83 ec 18             	sub    $0x18,%esp
80103499:	8b 5d 08             	mov    0x8(%ebp),%ebx
8010349c:	8b 7d 0c             	mov    0xc(%ebp),%edi
8010349f:	53                   	push   %ebx
801034a0:	e8 cb 12 00 00       	call   80104770 <acquire>
801034a5:	83 c4 10             	add    $0x10,%esp
801034a8:	8b 83 34 02 00 00    	mov    0x234(%ebx),%eax
801034ae:	39 83 38 02 00 00    	cmp    %eax,0x238(%ebx)
801034b4:	75 6a                	jne    80103520 <piperead+0x90>
801034b6:	8b b3 40 02 00 00    	mov    0x240(%ebx),%esi
801034bc:	85 f6                	test   %esi,%esi
801034be:	0f 84 cc 00 00 00    	je     80103590 <piperead+0x100>
801034c4:	8d b3 34 02 00 00    	lea    0x234(%ebx),%esi
801034ca:	eb 2d                	jmp    801034f9 <piperead+0x69>
801034cc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
801034d0:	83 ec 08             	sub    $0x8,%esp
801034d3:	53                   	push   %ebx
801034d4:	56                   	push   %esi
801034d5:	e8 c6 0a 00 00       	call   80103fa0 <sleep>
801034da:	83 c4 10             	add    $0x10,%esp
801034dd:	8b 83 38 02 00 00    	mov    0x238(%ebx),%eax
801034e3:	39 83 34 02 00 00    	cmp    %eax,0x234(%ebx)
801034e9:	75 35                	jne    80103520 <piperead+0x90>
801034eb:	8b 93 40 02 00 00    	mov    0x240(%ebx),%edx
801034f1:	85 d2                	test   %edx,%edx
801034f3:	0f 84 97 00 00 00    	je     80103590 <piperead+0x100>
801034f9:	e8 72 03 00 00       	call   80103870 <myproc>
801034fe:	8b 48 24             	mov    0x24(%eax),%ecx
80103501:	85 c9                	test   %ecx,%ecx
80103503:	74 cb                	je     801034d0 <piperead+0x40>
80103505:	83 ec 0c             	sub    $0xc,%esp
80103508:	53                   	push   %ebx
80103509:	e8 82 13 00 00       	call   80104890 <release>
8010350e:	83 c4 10             	add    $0x10,%esp
80103511:	8d 65 f4             	lea    -0xc(%ebp),%esp
80103514:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80103519:	5b                   	pop    %ebx
8010351a:	5e                   	pop    %esi
8010351b:	5f                   	pop    %edi
8010351c:	5d                   	pop    %ebp
8010351d:	c3                   	ret    
8010351e:	66 90                	xchg   %ax,%ax
80103520:	8b 45 10             	mov    0x10(%ebp),%eax
80103523:	85 c0                	test   %eax,%eax
80103525:	7e 69                	jle    80103590 <piperead+0x100>
80103527:	8b 83 34 02 00 00    	mov    0x234(%ebx),%eax
8010352d:	31 c9                	xor    %ecx,%ecx
8010352f:	eb 15                	jmp    80103546 <piperead+0xb6>
80103531:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80103538:	8b 83 34 02 00 00    	mov    0x234(%ebx),%eax
8010353e:	3b 83 38 02 00 00    	cmp    0x238(%ebx),%eax
80103544:	74 5a                	je     801035a0 <piperead+0x110>
80103546:	8d 70 01             	lea    0x1(%eax),%esi
80103549:	25 ff 01 00 00       	and    $0x1ff,%eax
8010354e:	89 b3 34 02 00 00    	mov    %esi,0x234(%ebx)
80103554:	0f b6 44 03 34       	movzbl 0x34(%ebx,%eax,1),%eax
80103559:	88 04 0f             	mov    %al,(%edi,%ecx,1)
8010355c:	83 c1 01             	add    $0x1,%ecx
8010355f:	39 4d 10             	cmp    %ecx,0x10(%ebp)
80103562:	75 d4                	jne    80103538 <piperead+0xa8>
80103564:	8d 83 38 02 00 00    	lea    0x238(%ebx),%eax
8010356a:	83 ec 0c             	sub    $0xc,%esp
8010356d:	50                   	push   %eax
8010356e:	e8 ed 0b 00 00       	call   80104160 <wakeup>
80103573:	89 1c 24             	mov    %ebx,(%esp)
80103576:	e8 15 13 00 00       	call   80104890 <release>
8010357b:	8b 45 10             	mov    0x10(%ebp),%eax
8010357e:	83 c4 10             	add    $0x10,%esp
80103581:	8d 65 f4             	lea    -0xc(%ebp),%esp
80103584:	5b                   	pop    %ebx
80103585:	5e                   	pop    %esi
80103586:	5f                   	pop    %edi
80103587:	5d                   	pop    %ebp
80103588:	c3                   	ret    
80103589:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80103590:	c7 45 10 00 00 00 00 	movl   $0x0,0x10(%ebp)
80103597:	eb cb                	jmp    80103564 <piperead+0xd4>
80103599:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801035a0:	89 4d 10             	mov    %ecx,0x10(%ebp)
801035a3:	eb bf                	jmp    80103564 <piperead+0xd4>
801035a5:	66 90                	xchg   %ax,%ax
801035a7:	66 90                	xchg   %ax,%ax
801035a9:	66 90                	xchg   %ax,%ax
801035ab:	66 90                	xchg   %ax,%ax
801035ad:	66 90                	xchg   %ax,%ax
801035af:	90                   	nop

801035b0 <allocproc>:
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void)
{
801035b0:	55                   	push   %ebp
801035b1:	89 e5                	mov    %esp,%ebp
801035b3:	57                   	push   %edi
801035b4:	56                   	push   %esi
801035b5:	53                   	push   %ebx
801035b6:	83 ec 28             	sub    $0x28,%esp
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);
801035b9:	68 20 2d 11 80       	push   $0x80112d20
801035be:	e8 ad 11 00 00       	call   80104770 <acquire>
801035c3:	83 c4 10             	add    $0x10,%esp

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
801035c6:	b8 54 2d 11 80       	mov    $0x80112d54,%eax
801035cb:	eb 10                	jmp    801035dd <allocproc+0x2d>
801035cd:	05 98 00 00 00       	add    $0x98,%eax
801035d2:	3d 54 53 11 80       	cmp    $0x80115354,%eax
801035d7:	0f 84 57 01 00 00    	je     80103734 <allocproc+0x184>
    if(p->state == UNUSED)
801035dd:	8b 50 0c             	mov    0xc(%eax),%edx
801035e0:	85 d2                	test   %edx,%edx
801035e2:	75 e9                	jne    801035cd <allocproc+0x1d>
801035e4:	89 c7                	mov    %eax,%edi

  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
801035e6:	c7 40 0c 01 00 00 00 	movl   $0x1,0xc(%eax)
801035ed:	89 45 e4             	mov    %eax,-0x1c(%ebp)
  p->pid = nextpid++;
801035f0:	a1 08 a0 10 80       	mov    0x8010a008,%eax
  p->priority = priorityDefault; //default priority
801035f5:	8b 35 04 a0 10 80    	mov    0x8010a004,%esi
  p->runtimeDeadline=50;
  p->tickcounts=0;

  int priorityNow = priorityDefault;//priority sort
  int maxDeadline=0;
  int tempDeadline=-1;
801035fb:	b9 ff ff ff ff       	mov    $0xffffffff,%ecx
found:
  p->state = EMBRYO;
  p->pid = nextpid++;
  p->priority = priorityDefault; //default priority
  p->lowestPriority=priorityDefault;
  p->runTimes=0;
80103600:	c7 87 84 00 00 00 00 	movl   $0x0,0x84(%edi)
80103607:	00 00 00 
  p->highestPriority=0;
8010360a:	c7 87 88 00 00 00 00 	movl   $0x0,0x88(%edi)
80103611:	00 00 00 
  p->deadline=50;
80103614:	c7 87 8c 00 00 00 32 	movl   $0x32,0x8c(%edi)
8010361b:	00 00 00 
  p->runtimeDeadline=50;
8010361e:	c7 87 90 00 00 00 32 	movl   $0x32,0x90(%edi)
80103625:	00 00 00 
  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;
80103628:	8d 50 01             	lea    0x1(%eax),%edx
8010362b:	89 47 10             	mov    %eax,0x10(%edi)
  p->priority = priorityDefault; //default priority
8010362e:	89 77 7c             	mov    %esi,0x7c(%edi)
  p->lowestPriority=priorityDefault;
80103631:	89 b7 80 00 00 00    	mov    %esi,0x80(%edi)
  p->runTimes=0;
  p->highestPriority=0;
  p->deadline=50;
  p->runtimeDeadline=50;
  p->tickcounts=0;
80103637:	c7 87 94 00 00 00 00 	movl   $0x0,0x94(%edi)
8010363e:	00 00 00 
  int priorityNow = priorityDefault;//priority sort
  int maxDeadline=0;
  int tempDeadline=-1;
  struct proc *a;
  struct proc *b;
  for(a =ptable.proc; a< &ptable.proc[NPROC];a++){
80103641:	bf 54 2d 11 80       	mov    $0x80112d54,%edi
  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;
80103646:	89 15 08 a0 10 80    	mov    %edx,0x8010a008
8010364c:	eb 10                	jmp    8010365e <allocproc+0xae>
8010364e:	66 90                	xchg   %ax,%ax
  int priorityNow = priorityDefault;//priority sort
  int maxDeadline=0;
  int tempDeadline=-1;
  struct proc *a;
  struct proc *b;
  for(a =ptable.proc; a< &ptable.proc[NPROC];a++){
80103650:	81 c7 98 00 00 00    	add    $0x98,%edi
80103656:	81 ff 54 53 11 80    	cmp    $0x80115354,%edi
8010365c:	74 78                	je     801036d6 <allocproc+0x126>
	if(a->state != SLEEPING&&a->state != RUNNING&&a->state != RUNNABLE)
8010365e:	8b 47 0c             	mov    0xc(%edi),%eax
80103661:	83 e8 02             	sub    $0x2,%eax
80103664:	83 f8 02             	cmp    $0x2,%eax
80103667:	77 e7                	ja     80103650 <allocproc+0xa0>
80103669:	b8 54 2d 11 80       	mov    $0x80112d54,%eax
8010366e:	31 db                	xor    %ebx,%ebx
80103670:	eb 12                	jmp    80103684 <allocproc+0xd4>
80103672:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
		continue;
	for(b =ptable.proc; b< &ptable.proc[NPROC];b++){
80103678:	05 98 00 00 00       	add    $0x98,%eax
8010367d:	3d 54 53 11 80       	cmp    $0x80115354,%eax
80103682:	74 3f                	je     801036c3 <allocproc+0x113>
		if(b->state != SLEEPING&&b->state != RUNNING&&b->state != RUNNABLE)
80103684:	8b 50 0c             	mov    0xc(%eax),%edx
80103687:	83 ea 02             	sub    $0x2,%edx
8010368a:	83 fa 02             	cmp    $0x2,%edx
8010368d:	77 e9                	ja     80103678 <allocproc+0xc8>
			continue;
	if(tempDeadline==-1||tempDeadline > b->runtimeDeadline)
8010368f:	83 f9 ff             	cmp    $0xffffffff,%ecx
80103692:	74 08                	je     8010369c <allocproc+0xec>
80103694:	39 88 90 00 00 00    	cmp    %ecx,0x90(%eax)
8010369a:	7d dc                	jge    80103678 <allocproc+0xc8>
	{
	      b->lowestPriority = priorityNow;
	      b->priority = priorityNow;
	      if(maxDeadline==0||maxDeadline < b->runtimeDeadline)
8010369c:	85 db                	test   %ebx,%ebx
	for(b =ptable.proc; b< &ptable.proc[NPROC];b++){
		if(b->state != SLEEPING&&b->state != RUNNING&&b->state != RUNNABLE)
			continue;
	if(tempDeadline==-1||tempDeadline > b->runtimeDeadline)
	{
	      b->lowestPriority = priorityNow;
8010369e:	89 b0 80 00 00 00    	mov    %esi,0x80(%eax)
	      b->priority = priorityNow;
801036a4:	89 70 7c             	mov    %esi,0x7c(%eax)
	      if(maxDeadline==0||maxDeadline < b->runtimeDeadline)
801036a7:	74 08                	je     801036b1 <allocproc+0x101>
801036a9:	39 98 90 00 00 00    	cmp    %ebx,0x90(%eax)
801036af:	7e c7                	jle    80103678 <allocproc+0xc8>
	         maxDeadline = b->deadline;
801036b1:	8b 98 8c 00 00 00    	mov    0x8c(%eax),%ebx
  struct proc *a;
  struct proc *b;
  for(a =ptable.proc; a< &ptable.proc[NPROC];a++){
	if(a->state != SLEEPING&&a->state != RUNNING&&a->state != RUNNABLE)
		continue;
	for(b =ptable.proc; b< &ptable.proc[NPROC];b++){
801036b7:	05 98 00 00 00       	add    $0x98,%eax
801036bc:	3d 54 53 11 80       	cmp    $0x80115354,%eax
801036c1:	75 c1                	jne    80103684 <allocproc+0xd4>
  int priorityNow = priorityDefault;//priority sort
  int maxDeadline=0;
  int tempDeadline=-1;
  struct proc *a;
  struct proc *b;
  for(a =ptable.proc; a< &ptable.proc[NPROC];a++){
801036c3:	81 c7 98 00 00 00    	add    $0x98,%edi
	         maxDeadline = b->deadline;
	}
      }
	tempDeadline=maxDeadline;
	maxDeadline=0;
	priorityNow = priorityNow+1;
801036c9:	83 c6 01             	add    $0x1,%esi
801036cc:	89 d9                	mov    %ebx,%ecx
  int priorityNow = priorityDefault;//priority sort
  int maxDeadline=0;
  int tempDeadline=-1;
  struct proc *a;
  struct proc *b;
  for(a =ptable.proc; a< &ptable.proc[NPROC];a++){
801036ce:	81 ff 54 53 11 80    	cmp    $0x80115354,%edi
801036d4:	75 88                	jne    8010365e <allocproc+0xae>
      }
	tempDeadline=maxDeadline;
	maxDeadline=0;
	priorityNow = priorityNow+1;
  }
  release(&ptable.lock);
801036d6:	83 ec 0c             	sub    $0xc,%esp
801036d9:	68 20 2d 11 80       	push   $0x80112d20
801036de:	e8 ad 11 00 00       	call   80104890 <release>

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){
801036e3:	e8 a8 ed ff ff       	call   80102490 <kalloc>
801036e8:	8b 7d e4             	mov    -0x1c(%ebp),%edi
801036eb:	83 c4 10             	add    $0x10,%esp
801036ee:	85 c0                	test   %eax,%eax
801036f0:	89 47 08             	mov    %eax,0x8(%edi)
801036f3:	74 59                	je     8010374e <allocproc+0x19e>
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;

  // Leave room for trap frame.
  sp -= sizeof *p->tf;
801036f5:	8b 7d e4             	mov    -0x1c(%ebp),%edi
801036f8:	8d 90 b4 0f 00 00    	lea    0xfb4(%eax),%edx
  sp -= 4;
  *(uint*)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
801036fe:	83 ec 04             	sub    $0x4,%esp
  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret;

  sp -= sizeof *p->context;
80103701:	05 9c 0f 00 00       	add    $0xf9c,%eax
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;

  // Leave room for trap frame.
  sp -= sizeof *p->tf;
80103706:	89 57 18             	mov    %edx,0x18(%edi)
  p->tf = (struct trapframe*)sp;

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret;
80103709:	c7 40 14 9f 5b 10 80 	movl   $0x80105b9f,0x14(%eax)

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
80103710:	89 47 1c             	mov    %eax,0x1c(%edi)
  memset(p->context, 0, sizeof *p->context);
80103713:	6a 14                	push   $0x14
80103715:	6a 00                	push   $0x0
80103717:	50                   	push   %eax
80103718:	e8 c3 11 00 00       	call   801048e0 <memset>
  p->context->eip = (uint)forkret;
8010371d:	8b 47 1c             	mov    0x1c(%edi),%eax

  return p;
80103720:	83 c4 10             	add    $0x10,%esp
  *(uint*)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;
80103723:	c7 40 10 60 37 10 80 	movl   $0x80103760,0x10(%eax)

  return p;
}
8010372a:	8d 65 f4             	lea    -0xc(%ebp),%esp
  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  return p;
8010372d:	89 f8                	mov    %edi,%eax
}
8010372f:	5b                   	pop    %ebx
80103730:	5e                   	pop    %esi
80103731:	5f                   	pop    %edi
80103732:	5d                   	pop    %ebp
80103733:	c3                   	ret    

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
80103734:	83 ec 0c             	sub    $0xc,%esp
80103737:	68 20 2d 11 80       	push   $0x80112d20
8010373c:	e8 4f 11 00 00       	call   80104890 <release>
  return 0;
80103741:	83 c4 10             	add    $0x10,%esp
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  return p;
}
80103744:	8d 65 f4             	lea    -0xc(%ebp),%esp
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;
80103747:	31 c0                	xor    %eax,%eax
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  return p;
}
80103749:	5b                   	pop    %ebx
8010374a:	5e                   	pop    %esi
8010374b:	5f                   	pop    %edi
8010374c:	5d                   	pop    %ebp
8010374d:	c3                   	ret    
  }
  release(&ptable.lock);

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
8010374e:	c7 47 0c 00 00 00 00 	movl   $0x0,0xc(%edi)
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  return p;
}
80103755:	8d 65 f4             	lea    -0xc(%ebp),%esp
  release(&ptable.lock);

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
80103758:	31 c0                	xor    %eax,%eax
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  return p;
}
8010375a:	5b                   	pop    %ebx
8010375b:	5e                   	pop    %esi
8010375c:	5f                   	pop    %edi
8010375d:	5d                   	pop    %ebp
8010375e:	c3                   	ret    
8010375f:	90                   	nop

80103760 <forkret>:

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
80103760:	55                   	push   %ebp
80103761:	89 e5                	mov    %esp,%ebp
80103763:	83 ec 14             	sub    $0x14,%esp
  static int first = 1;
  // Still holding ptable.lock from scheduler.
  release(&ptable.lock);
80103766:	68 20 2d 11 80       	push   $0x80112d20
8010376b:	e8 20 11 00 00       	call   80104890 <release>

  if (first) {
80103770:	a1 00 a0 10 80       	mov    0x8010a000,%eax
80103775:	83 c4 10             	add    $0x10,%esp
80103778:	85 c0                	test   %eax,%eax
8010377a:	75 04                	jne    80103780 <forkret+0x20>
    iinit(ROOTDEV);
    initlog(ROOTDEV);
  }

  // Return to "caller", actually trapret (see allocproc).
}
8010377c:	c9                   	leave  
8010377d:	c3                   	ret    
8010377e:	66 90                	xchg   %ax,%ax
  if (first) {
    // Some initialization functions must be run in the context
    // of a regular process (e.g., they call sleep), and thus cannot
    // be run from main().
    first = 0;
    iinit(ROOTDEV);
80103780:	83 ec 0c             	sub    $0xc,%esp

  if (first) {
    // Some initialization functions must be run in the context
    // of a regular process (e.g., they call sleep), and thus cannot
    // be run from main().
    first = 0;
80103783:	c7 05 00 a0 10 80 00 	movl   $0x0,0x8010a000
8010378a:	00 00 00 
    iinit(ROOTDEV);
8010378d:	6a 01                	push   $0x1
8010378f:	e8 dc dc ff ff       	call   80101470 <iinit>
    initlog(ROOTDEV);
80103794:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
8010379b:	e8 10 f3 ff ff       	call   80102ab0 <initlog>
801037a0:	83 c4 10             	add    $0x10,%esp
  }

  // Return to "caller", actually trapret (see allocproc).
}
801037a3:	c9                   	leave  
801037a4:	c3                   	ret    
801037a5:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
801037a9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801037b0 <pinit>:

int priorityDefault=10;

void
pinit(void)
{
801037b0:	55                   	push   %ebp
801037b1:	89 e5                	mov    %esp,%ebp
801037b3:	83 ec 10             	sub    $0x10,%esp
  initlock(&ptable.lock, "ptable");
801037b6:	68 d5 79 10 80       	push   $0x801079d5
801037bb:	68 20 2d 11 80       	push   $0x80112d20
801037c0:	e8 ab 0e 00 00       	call   80104670 <initlock>
}
801037c5:	83 c4 10             	add    $0x10,%esp
801037c8:	c9                   	leave  
801037c9:	c3                   	ret    
801037ca:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

801037d0 <mycpu>:

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu*
mycpu(void)
{
801037d0:	55                   	push   %ebp
801037d1:	89 e5                	mov    %esp,%ebp
801037d3:	56                   	push   %esi
801037d4:	53                   	push   %ebx

static inline uint
readeflags(void)
{
  uint eflags;
  asm volatile("pushfl; popl %0" : "=r" (eflags));
801037d5:	9c                   	pushf  
801037d6:	58                   	pop    %eax
  int apicid, i;
  
  if(readeflags()&FL_IF)
801037d7:	f6 c4 02             	test   $0x2,%ah
801037da:	75 5b                	jne    80103837 <mycpu+0x67>
    panic("mycpu called with interrupts enabled\n");
  
  apicid = lapicid();
801037dc:	e8 0f ef ff ff       	call   801026f0 <lapicid>
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i) {
801037e1:	8b 35 00 2d 11 80    	mov    0x80112d00,%esi
801037e7:	85 f6                	test   %esi,%esi
801037e9:	7e 3f                	jle    8010382a <mycpu+0x5a>
    if (cpus[i].apicid == apicid)
801037eb:	0f b6 15 80 27 11 80 	movzbl 0x80112780,%edx
801037f2:	39 d0                	cmp    %edx,%eax
801037f4:	74 30                	je     80103826 <mycpu+0x56>
801037f6:	b9 30 28 11 80       	mov    $0x80112830,%ecx
801037fb:	31 d2                	xor    %edx,%edx
801037fd:	8d 76 00             	lea    0x0(%esi),%esi
    panic("mycpu called with interrupts enabled\n");
  
  apicid = lapicid();
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i) {
80103800:	83 c2 01             	add    $0x1,%edx
80103803:	39 f2                	cmp    %esi,%edx
80103805:	74 23                	je     8010382a <mycpu+0x5a>
    if (cpus[i].apicid == apicid)
80103807:	0f b6 19             	movzbl (%ecx),%ebx
8010380a:	81 c1 b0 00 00 00    	add    $0xb0,%ecx
80103810:	39 d8                	cmp    %ebx,%eax
80103812:	75 ec                	jne    80103800 <mycpu+0x30>
      return &cpus[i];
80103814:	69 c2 b0 00 00 00    	imul   $0xb0,%edx,%eax
  }
  panic("unknown apicid\n");
}
8010381a:	8d 65 f8             	lea    -0x8(%ebp),%esp
8010381d:	5b                   	pop    %ebx
  apicid = lapicid();
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i) {
    if (cpus[i].apicid == apicid)
      return &cpus[i];
8010381e:	05 80 27 11 80       	add    $0x80112780,%eax
  }
  panic("unknown apicid\n");
}
80103823:	5e                   	pop    %esi
80103824:	5d                   	pop    %ebp
80103825:	c3                   	ret    
    panic("mycpu called with interrupts enabled\n");
  
  apicid = lapicid();
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i) {
80103826:	31 d2                	xor    %edx,%edx
80103828:	eb ea                	jmp    80103814 <mycpu+0x44>
    if (cpus[i].apicid == apicid)
      return &cpus[i];
  }
  panic("unknown apicid\n");
8010382a:	83 ec 0c             	sub    $0xc,%esp
8010382d:	68 dc 79 10 80       	push   $0x801079dc
80103832:	e8 39 cb ff ff       	call   80100370 <panic>
mycpu(void)
{
  int apicid, i;
  
  if(readeflags()&FL_IF)
    panic("mycpu called with interrupts enabled\n");
80103837:	83 ec 0c             	sub    $0xc,%esp
8010383a:	68 b8 7a 10 80       	push   $0x80107ab8
8010383f:	e8 2c cb ff ff       	call   80100370 <panic>
80103844:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
8010384a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

80103850 <cpuid>:
  initlock(&ptable.lock, "ptable");
}

// Must be called with interrupts disabled
int
cpuid() {
80103850:	55                   	push   %ebp
80103851:	89 e5                	mov    %esp,%ebp
80103853:	83 ec 08             	sub    $0x8,%esp
  return mycpu()-cpus;
80103856:	e8 75 ff ff ff       	call   801037d0 <mycpu>
8010385b:	2d 80 27 11 80       	sub    $0x80112780,%eax
}
80103860:	c9                   	leave  
}

// Must be called with interrupts disabled
int
cpuid() {
  return mycpu()-cpus;
80103861:	c1 f8 04             	sar    $0x4,%eax
80103864:	69 c0 a3 8b 2e ba    	imul   $0xba2e8ba3,%eax,%eax
}
8010386a:	c3                   	ret    
8010386b:	90                   	nop
8010386c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80103870 <myproc>:
}

// Disable interrupts so that we are not rescheduled
// while reading proc from the cpu structure
struct proc*
myproc(void) {
80103870:	55                   	push   %ebp
80103871:	89 e5                	mov    %esp,%ebp
80103873:	53                   	push   %ebx
80103874:	83 ec 04             	sub    $0x4,%esp
  struct cpu *c;
  struct proc *p;
  pushcli();
80103877:	e8 b4 0e 00 00       	call   80104730 <pushcli>
  c = mycpu();
8010387c:	e8 4f ff ff ff       	call   801037d0 <mycpu>
  p = c->proc;
80103881:	8b 98 ac 00 00 00    	mov    0xac(%eax),%ebx
  popcli();
80103887:	e8 94 0f 00 00       	call   80104820 <popcli>
  return p;
}
8010388c:	83 c4 04             	add    $0x4,%esp
8010388f:	89 d8                	mov    %ebx,%eax
80103891:	5b                   	pop    %ebx
80103892:	5d                   	pop    %ebp
80103893:	c3                   	ret    
80103894:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
8010389a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

801038a0 <userinit>:

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
801038a0:	55                   	push   %ebp
801038a1:	89 e5                	mov    %esp,%ebp
801038a3:	53                   	push   %ebx
801038a4:	83 ec 04             	sub    $0x4,%esp
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];

  p = allocproc();
801038a7:	e8 04 fd ff ff       	call   801035b0 <allocproc>
801038ac:	89 c3                	mov    %eax,%ebx
  
  initproc = p;
801038ae:	a3 b8 a5 10 80       	mov    %eax,0x8010a5b8
  if((p->pgdir = setupkvm()) == 0)
801038b3:	e8 18 39 00 00       	call   801071d0 <setupkvm>
801038b8:	85 c0                	test   %eax,%eax
801038ba:	89 43 04             	mov    %eax,0x4(%ebx)
801038bd:	0f 84 bd 00 00 00    	je     80103980 <userinit+0xe0>
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
801038c3:	83 ec 04             	sub    $0x4,%esp
801038c6:	68 2c 00 00 00       	push   $0x2c
801038cb:	68 60 a4 10 80       	push   $0x8010a460
801038d0:	50                   	push   %eax
801038d1:	e8 0a 36 00 00       	call   80106ee0 <inituvm>
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
801038d6:	83 c4 0c             	add    $0xc,%esp
  
  initproc = p;
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
801038d9:	c7 03 00 10 00 00    	movl   $0x1000,(%ebx)
  memset(p->tf, 0, sizeof(*p->tf));
801038df:	6a 4c                	push   $0x4c
801038e1:	6a 00                	push   $0x0
801038e3:	ff 73 18             	pushl  0x18(%ebx)
801038e6:	e8 f5 0f 00 00       	call   801048e0 <memset>
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
801038eb:	8b 43 18             	mov    0x18(%ebx),%eax
801038ee:	ba 1b 00 00 00       	mov    $0x1b,%edx
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
801038f3:	b9 23 00 00 00       	mov    $0x23,%ecx
  p->tf->ss = p->tf->ds;
  p->tf->eflags = FL_IF;
  p->tf->esp = PGSIZE;
  p->tf->eip = 0;  // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
801038f8:	83 c4 0c             	add    $0xc,%esp
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
801038fb:	66 89 50 3c          	mov    %dx,0x3c(%eax)
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
801038ff:	8b 43 18             	mov    0x18(%ebx),%eax
80103902:	66 89 48 2c          	mov    %cx,0x2c(%eax)
  p->tf->es = p->tf->ds;
80103906:	8b 43 18             	mov    0x18(%ebx),%eax
80103909:	0f b7 50 2c          	movzwl 0x2c(%eax),%edx
8010390d:	66 89 50 28          	mov    %dx,0x28(%eax)
  p->tf->ss = p->tf->ds;
80103911:	8b 43 18             	mov    0x18(%ebx),%eax
80103914:	0f b7 50 2c          	movzwl 0x2c(%eax),%edx
80103918:	66 89 50 48          	mov    %dx,0x48(%eax)
  p->tf->eflags = FL_IF;
8010391c:	8b 43 18             	mov    0x18(%ebx),%eax
8010391f:	c7 40 40 00 02 00 00 	movl   $0x200,0x40(%eax)
  p->tf->esp = PGSIZE;
80103926:	8b 43 18             	mov    0x18(%ebx),%eax
80103929:	c7 40 44 00 10 00 00 	movl   $0x1000,0x44(%eax)
  p->tf->eip = 0;  // beginning of initcode.S
80103930:	8b 43 18             	mov    0x18(%ebx),%eax
80103933:	c7 40 38 00 00 00 00 	movl   $0x0,0x38(%eax)

  safestrcpy(p->name, "initcode", sizeof(p->name));
8010393a:	8d 43 6c             	lea    0x6c(%ebx),%eax
8010393d:	6a 10                	push   $0x10
8010393f:	68 05 7a 10 80       	push   $0x80107a05
80103944:	50                   	push   %eax
80103945:	e8 96 11 00 00       	call   80104ae0 <safestrcpy>
  p->cwd = namei("/");
8010394a:	c7 04 24 0e 7a 10 80 	movl   $0x80107a0e,(%esp)
80103951:	e8 6a e5 ff ff       	call   80101ec0 <namei>
80103956:	89 43 68             	mov    %eax,0x68(%ebx)

  // this assignment to p->state lets other cores
  // run this process. the acquire forces the above
  // writes to be visible, and the lock is also needed
  // because the assignment might not be atomic.
  acquire(&ptable.lock);
80103959:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
80103960:	e8 0b 0e 00 00       	call   80104770 <acquire>

  p->state = RUNNABLE;
80103965:	c7 43 0c 03 00 00 00 	movl   $0x3,0xc(%ebx)

  release(&ptable.lock);
8010396c:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
80103973:	e8 18 0f 00 00       	call   80104890 <release>
}
80103978:	83 c4 10             	add    $0x10,%esp
8010397b:	8b 5d fc             	mov    -0x4(%ebp),%ebx
8010397e:	c9                   	leave  
8010397f:	c3                   	ret    

  p = allocproc();
  
  initproc = p;
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
80103980:	83 ec 0c             	sub    $0xc,%esp
80103983:	68 ec 79 10 80       	push   $0x801079ec
80103988:	e8 e3 c9 ff ff       	call   80100370 <panic>
8010398d:	8d 76 00             	lea    0x0(%esi),%esi

80103990 <growproc>:

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
80103990:	55                   	push   %ebp
80103991:	89 e5                	mov    %esp,%ebp
80103993:	56                   	push   %esi
80103994:	53                   	push   %ebx
80103995:	8b 75 08             	mov    0x8(%ebp),%esi
// while reading proc from the cpu structure
struct proc*
myproc(void) {
  struct cpu *c;
  struct proc *p;
  pushcli();
80103998:	e8 93 0d 00 00       	call   80104730 <pushcli>
  c = mycpu();
8010399d:	e8 2e fe ff ff       	call   801037d0 <mycpu>
  p = c->proc;
801039a2:	8b 98 ac 00 00 00    	mov    0xac(%eax),%ebx
  popcli();
801039a8:	e8 73 0e 00 00       	call   80104820 <popcli>
{
  uint sz;
  struct proc *curproc = myproc();

  sz = curproc->sz;
  if(n > 0){
801039ad:	83 fe 00             	cmp    $0x0,%esi
growproc(int n)
{
  uint sz;
  struct proc *curproc = myproc();

  sz = curproc->sz;
801039b0:	8b 03                	mov    (%ebx),%eax
  if(n > 0){
801039b2:	7e 34                	jle    801039e8 <growproc+0x58>
    if((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
801039b4:	83 ec 04             	sub    $0x4,%esp
801039b7:	01 c6                	add    %eax,%esi
801039b9:	56                   	push   %esi
801039ba:	50                   	push   %eax
801039bb:	ff 73 04             	pushl  0x4(%ebx)
801039be:	e8 5d 36 00 00       	call   80107020 <allocuvm>
801039c3:	83 c4 10             	add    $0x10,%esp
801039c6:	85 c0                	test   %eax,%eax
801039c8:	74 36                	je     80103a00 <growproc+0x70>
  } else if(n < 0){
    if((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  curproc->sz = sz;
  switchuvm(curproc);
801039ca:	83 ec 0c             	sub    $0xc,%esp
      return -1;
  } else if(n < 0){
    if((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  curproc->sz = sz;
801039cd:	89 03                	mov    %eax,(%ebx)
  switchuvm(curproc);
801039cf:	53                   	push   %ebx
801039d0:	e8 fb 33 00 00       	call   80106dd0 <switchuvm>
  return 0;
801039d5:	83 c4 10             	add    $0x10,%esp
801039d8:	31 c0                	xor    %eax,%eax
}
801039da:	8d 65 f8             	lea    -0x8(%ebp),%esp
801039dd:	5b                   	pop    %ebx
801039de:	5e                   	pop    %esi
801039df:	5d                   	pop    %ebp
801039e0:	c3                   	ret    
801039e1:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

  sz = curproc->sz;
  if(n > 0){
    if((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if(n < 0){
801039e8:	74 e0                	je     801039ca <growproc+0x3a>
    if((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
801039ea:	83 ec 04             	sub    $0x4,%esp
801039ed:	01 c6                	add    %eax,%esi
801039ef:	56                   	push   %esi
801039f0:	50                   	push   %eax
801039f1:	ff 73 04             	pushl  0x4(%ebx)
801039f4:	e8 27 37 00 00       	call   80107120 <deallocuvm>
801039f9:	83 c4 10             	add    $0x10,%esp
801039fc:	85 c0                	test   %eax,%eax
801039fe:	75 ca                	jne    801039ca <growproc+0x3a>
  struct proc *curproc = myproc();

  sz = curproc->sz;
  if(n > 0){
    if((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
80103a00:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80103a05:	eb d3                	jmp    801039da <growproc+0x4a>
80103a07:	89 f6                	mov    %esi,%esi
80103a09:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80103a10 <fork>:
// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void)
{
80103a10:	55                   	push   %ebp
80103a11:	89 e5                	mov    %esp,%ebp
80103a13:	57                   	push   %edi
80103a14:	56                   	push   %esi
80103a15:	53                   	push   %ebx
80103a16:	83 ec 1c             	sub    $0x1c,%esp
// while reading proc from the cpu structure
struct proc*
myproc(void) {
  struct cpu *c;
  struct proc *p;
  pushcli();
80103a19:	e8 12 0d 00 00       	call   80104730 <pushcli>
  c = mycpu();
80103a1e:	e8 ad fd ff ff       	call   801037d0 <mycpu>
  p = c->proc;
80103a23:	8b 98 ac 00 00 00    	mov    0xac(%eax),%ebx
  popcli();
80103a29:	e8 f2 0d 00 00       	call   80104820 <popcli>
  int i, pid;
  struct proc *np;
  struct proc *curproc = myproc();

  // Allocate process.
  if((np = allocproc()) == 0){
80103a2e:	e8 7d fb ff ff       	call   801035b0 <allocproc>
80103a33:	85 c0                	test   %eax,%eax
80103a35:	89 c7                	mov    %eax,%edi
80103a37:	89 45 e4             	mov    %eax,-0x1c(%ebp)
80103a3a:	0f 84 b5 00 00 00    	je     80103af5 <fork+0xe5>
    return -1;
  }

  // Copy process state from proc.
  if((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0){
80103a40:	83 ec 08             	sub    $0x8,%esp
80103a43:	ff 33                	pushl  (%ebx)
80103a45:	ff 73 04             	pushl  0x4(%ebx)
80103a48:	e8 53 38 00 00       	call   801072a0 <copyuvm>
80103a4d:	83 c4 10             	add    $0x10,%esp
80103a50:	85 c0                	test   %eax,%eax
80103a52:	89 47 04             	mov    %eax,0x4(%edi)
80103a55:	0f 84 a1 00 00 00    	je     80103afc <fork+0xec>
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }
  np->sz = curproc->sz;
80103a5b:	8b 03                	mov    (%ebx),%eax
80103a5d:	8b 4d e4             	mov    -0x1c(%ebp),%ecx
80103a60:	89 01                	mov    %eax,(%ecx)
  np->parent = curproc;
80103a62:	89 59 14             	mov    %ebx,0x14(%ecx)
  *np->tf = *curproc->tf;
80103a65:	89 c8                	mov    %ecx,%eax
80103a67:	8b 79 18             	mov    0x18(%ecx),%edi
80103a6a:	8b 73 18             	mov    0x18(%ebx),%esi
80103a6d:	b9 13 00 00 00       	mov    $0x13,%ecx
80103a72:	f3 a5                	rep movsl %ds:(%esi),%es:(%edi)

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
80103a74:	31 f6                	xor    %esi,%esi
  np->sz = curproc->sz;
  np->parent = curproc;
  *np->tf = *curproc->tf;

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;
80103a76:	8b 40 18             	mov    0x18(%eax),%eax
80103a79:	c7 40 1c 00 00 00 00 	movl   $0x0,0x1c(%eax)

  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
80103a80:	8b 44 b3 28          	mov    0x28(%ebx,%esi,4),%eax
80103a84:	85 c0                	test   %eax,%eax
80103a86:	74 13                	je     80103a9b <fork+0x8b>
      np->ofile[i] = filedup(curproc->ofile[i]);
80103a88:	83 ec 0c             	sub    $0xc,%esp
80103a8b:	50                   	push   %eax
80103a8c:	e8 4f d3 ff ff       	call   80100de0 <filedup>
80103a91:	8b 55 e4             	mov    -0x1c(%ebp),%edx
80103a94:	83 c4 10             	add    $0x10,%esp
80103a97:	89 44 b2 28          	mov    %eax,0x28(%edx,%esi,4)
  *np->tf = *curproc->tf;

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
80103a9b:	83 c6 01             	add    $0x1,%esi
80103a9e:	83 fe 10             	cmp    $0x10,%esi
80103aa1:	75 dd                	jne    80103a80 <fork+0x70>
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  np->cwd = idup(curproc->cwd);
80103aa3:	83 ec 0c             	sub    $0xc,%esp
80103aa6:	ff 73 68             	pushl  0x68(%ebx)

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));
80103aa9:	83 c3 6c             	add    $0x6c,%ebx
  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  np->cwd = idup(curproc->cwd);
80103aac:	e8 8f db ff ff       	call   80101640 <idup>
80103ab1:	8b 7d e4             	mov    -0x1c(%ebp),%edi

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));
80103ab4:	83 c4 0c             	add    $0xc,%esp
  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  np->cwd = idup(curproc->cwd);
80103ab7:	89 47 68             	mov    %eax,0x68(%edi)

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));
80103aba:	8d 47 6c             	lea    0x6c(%edi),%eax
80103abd:	6a 10                	push   $0x10
80103abf:	53                   	push   %ebx
80103ac0:	50                   	push   %eax
80103ac1:	e8 1a 10 00 00       	call   80104ae0 <safestrcpy>

  pid = np->pid;
80103ac6:	8b 5f 10             	mov    0x10(%edi),%ebx

  acquire(&ptable.lock);
80103ac9:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
80103ad0:	e8 9b 0c 00 00       	call   80104770 <acquire>

  np->state = RUNNABLE;
80103ad5:	c7 47 0c 03 00 00 00 	movl   $0x3,0xc(%edi)

  release(&ptable.lock);
80103adc:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
80103ae3:	e8 a8 0d 00 00       	call   80104890 <release>

  return pid;
80103ae8:	83 c4 10             	add    $0x10,%esp
80103aeb:	89 d8                	mov    %ebx,%eax
}
80103aed:	8d 65 f4             	lea    -0xc(%ebp),%esp
80103af0:	5b                   	pop    %ebx
80103af1:	5e                   	pop    %esi
80103af2:	5f                   	pop    %edi
80103af3:	5d                   	pop    %ebp
80103af4:	c3                   	ret    
  struct proc *np;
  struct proc *curproc = myproc();

  // Allocate process.
  if((np = allocproc()) == 0){
    return -1;
80103af5:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80103afa:	eb f1                	jmp    80103aed <fork+0xdd>
  }

  // Copy process state from proc.
  if((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0){
    kfree(np->kstack);
80103afc:	8b 7d e4             	mov    -0x1c(%ebp),%edi
80103aff:	83 ec 0c             	sub    $0xc,%esp
80103b02:	ff 77 08             	pushl  0x8(%edi)
80103b05:	e8 d6 e7 ff ff       	call   801022e0 <kfree>
    np->kstack = 0;
80103b0a:	c7 47 08 00 00 00 00 	movl   $0x0,0x8(%edi)
    np->state = UNUSED;
80103b11:	c7 47 0c 00 00 00 00 	movl   $0x0,0xc(%edi)
    return -1;
80103b18:	83 c4 10             	add    $0x10,%esp
80103b1b:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80103b20:	eb cb                	jmp    80103aed <fork+0xdd>
80103b22:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80103b29:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80103b30 <scheduler>:
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void
scheduler(void)
{
80103b30:	55                   	push   %ebp
80103b31:	89 e5                	mov    %esp,%ebp
80103b33:	57                   	push   %edi
80103b34:	56                   	push   %esi
80103b35:	53                   	push   %ebx
80103b36:	83 ec 1c             	sub    $0x1c,%esp
  struct proc *p;
  struct cpu *c = mycpu();
80103b39:	e8 92 fc ff ff       	call   801037d0 <mycpu>
  c->proc = 0;
80103b3e:	c7 80 ac 00 00 00 00 	movl   $0x0,0xac(%eax)
80103b45:	00 00 00 
//      via swtch back to the scheduler.
void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
80103b48:	89 45 dc             	mov    %eax,-0x24(%ebp)
80103b4b:	83 c0 04             	add    $0x4,%eax
80103b4e:	89 45 d8             	mov    %eax,-0x28(%ebp)
}

static inline void
sti(void)
{
  asm volatile("sti");
80103b51:	fb                   	sti    
  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
80103b52:	83 ec 0c             	sub    $0xc,%esp
80103b55:	68 20 2d 11 80       	push   $0x80112d20
80103b5a:	e8 11 0c 00 00       	call   80104770 <acquire>
80103b5f:	83 c4 10             	add    $0x10,%esp


    int highestPriority=0;
    int nextRunningProcessPid=0;
	nextRunningProcessPid=nextRunningProcessPid;
80103b62:	c7 45 e0 00 00 00 00 	movl   $0x0,-0x20(%ebp)

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);


    int highestPriority=0;
80103b69:	31 d2                	xor    %edx,%edx
    int nextRunningProcessPid=0;
	nextRunningProcessPid=nextRunningProcessPid;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
80103b6b:	b8 54 2d 11 80       	mov    $0x80112d54,%eax
80103b70:	eb 0c                	jmp    80103b7e <scheduler+0x4e>
80103b72:	05 98 00 00 00       	add    $0x98,%eax
80103b77:	3d 54 53 11 80       	cmp    $0x80115354,%eax
80103b7c:	74 21                	je     80103b9f <scheduler+0x6f>
	if(p->state == RUNNABLE){	
80103b7e:	83 78 0c 03          	cmpl   $0x3,0xc(%eax)
80103b82:	75 ee                	jne    80103b72 <scheduler+0x42>
		if(p->priority>highestPriority)
80103b84:	8b 48 7c             	mov    0x7c(%eax),%ecx
80103b87:	39 d1                	cmp    %edx,%ecx
80103b89:	7e e7                	jle    80103b72 <scheduler+0x42>
		{
		highestPriority=p->priority;
		nextRunningProcessPid=p->pid;
80103b8b:	8b 78 10             	mov    0x10(%eax),%edi


    int highestPriority=0;
    int nextRunningProcessPid=0;
	nextRunningProcessPid=nextRunningProcessPid;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
80103b8e:	05 98 00 00 00       	add    $0x98,%eax
	if(p->state == RUNNABLE){	
		if(p->priority>highestPriority)
		{
		highestPriority=p->priority;
		nextRunningProcessPid=p->pid;
80103b93:	89 ca                	mov    %ecx,%edx


    int highestPriority=0;
    int nextRunningProcessPid=0;
	nextRunningProcessPid=nextRunningProcessPid;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
80103b95:	3d 54 53 11 80       	cmp    $0x80115354,%eax
	if(p->state == RUNNABLE){	
		if(p->priority>highestPriority)
		{
		highestPriority=p->priority;
		nextRunningProcessPid=p->pid;
80103b9a:	89 7d e0             	mov    %edi,-0x20(%ebp)


    int highestPriority=0;
    int nextRunningProcessPid=0;
	nextRunningProcessPid=nextRunningProcessPid;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
80103b9d:	75 df                	jne    80103b7e <scheduler+0x4e>
80103b9f:	c7 45 e4 54 2d 11 80 	movl   $0x80112d54,-0x1c(%ebp)
80103ba6:	eb 15                	jmp    80103bbd <scheduler+0x8d>
		//p->priority=p->priority+1;
	}
    }
	

    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
80103ba8:	81 45 e4 98 00 00 00 	addl   $0x98,-0x1c(%ebp)
80103baf:	8b 45 e4             	mov    -0x1c(%ebp),%eax
80103bb2:	3d 54 53 11 80       	cmp    $0x80115354,%eax
80103bb7:	0f 84 82 01 00 00    	je     80103d3f <scheduler+0x20f>
      if(p->state != RUNNABLE)
80103bbd:	8b 45 e4             	mov    -0x1c(%ebp),%eax
80103bc0:	83 78 0c 03          	cmpl   $0x3,0xc(%eax)
80103bc4:	75 e2                	jne    80103ba8 <scheduler+0x78>
        continue;


      if(p->pid!=nextRunningProcessPid)
80103bc6:	8b 7d e0             	mov    -0x20(%ebp),%edi
80103bc9:	3b 78 10             	cmp    0x10(%eax),%edi
80103bcc:	75 da                	jne    80103ba8 <scheduler+0x78>
	continue;

	if(p->priority > p->highestPriority){
80103bce:	89 c7                	mov    %eax,%edi
80103bd0:	8b 40 7c             	mov    0x7c(%eax),%eax
80103bd3:	3b 87 88 00 00 00    	cmp    0x88(%edi),%eax
80103bd9:	7e 06                	jle    80103be1 <scheduler+0xb1>
	p->highestPriority=p->priority;}
80103bdb:	89 87 88 00 00 00    	mov    %eax,0x88(%edi)

	p->priority=p->lowestPriority;//The process be excute,so reset their priority
80103be1:	8b 7d e4             	mov    -0x1c(%ebp),%edi
80103be4:	8b 87 80 00 00 00    	mov    0x80(%edi),%eax
	p->runTimes++;

  struct proc *a;
int tempp=p->runtimeDeadline-p->deadline;
80103bea:	8b 8f 90 00 00 00    	mov    0x90(%edi),%ecx

	if(p->priority > p->highestPriority){
	p->highestPriority=p->priority;}

	p->priority=p->lowestPriority;//The process be excute,so reset their priority
	p->runTimes++;
80103bf0:	83 87 84 00 00 00 01 	addl   $0x1,0x84(%edi)

  struct proc *a;
int tempp=p->runtimeDeadline-p->deadline;
80103bf7:	2b 8f 8c 00 00 00    	sub    0x8c(%edi),%ecx
	continue;

	if(p->priority > p->highestPriority){
	p->highestPriority=p->priority;}

	p->priority=p->lowestPriority;//The process be excute,so reset their priority
80103bfd:	89 47 7c             	mov    %eax,0x7c(%edi)
	p->runTimes++;

  struct proc *a;
int tempp=p->runtimeDeadline-p->deadline;
  for(a =ptable.proc; a< &ptable.proc[NPROC];a++){
80103c00:	b8 54 2d 11 80       	mov    $0x80112d54,%eax
80103c05:	eb 15                	jmp    80103c1c <scheduler+0xec>
80103c07:	89 f6                	mov    %esi,%esi
80103c09:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80103c10:	05 98 00 00 00       	add    $0x98,%eax
80103c15:	3d 54 53 11 80       	cmp    $0x80115354,%eax
80103c1a:	74 32                	je     80103c4e <scheduler+0x11e>
	if(a->state != SLEEPING&&a->state != RUNNING&&a->state != RUNNABLE)
80103c1c:	8b 78 0c             	mov    0xc(%eax),%edi
80103c1f:	8d 57 fe             	lea    -0x2(%edi),%edx
80103c22:	83 fa 02             	cmp    $0x2,%edx
80103c25:	77 e9                	ja     80103c10 <scheduler+0xe0>
		continue;
	a->runtimeDeadline = a->runtimeDeadline - tempp;
80103c27:	8b 90 90 00 00 00    	mov    0x90(%eax),%edx
80103c2d:	29 ca                	sub    %ecx,%edx
	if(a->runtimeDeadline <= 0)
80103c2f:	85 d2                	test   %edx,%edx
  struct proc *a;
int tempp=p->runtimeDeadline-p->deadline;
  for(a =ptable.proc; a< &ptable.proc[NPROC];a++){
	if(a->state != SLEEPING&&a->state != RUNNING&&a->state != RUNNABLE)
		continue;
	a->runtimeDeadline = a->runtimeDeadline - tempp;
80103c31:	89 90 90 00 00 00    	mov    %edx,0x90(%eax)
	if(a->runtimeDeadline <= 0)
80103c37:	7f d7                	jg     80103c10 <scheduler+0xe0>
a->runtimeDeadline= a->deadline;
80103c39:	8b 90 8c 00 00 00    	mov    0x8c(%eax),%edx
	p->priority=p->lowestPriority;//The process be excute,so reset their priority
	p->runTimes++;

  struct proc *a;
int tempp=p->runtimeDeadline-p->deadline;
  for(a =ptable.proc; a< &ptable.proc[NPROC];a++){
80103c3f:	05 98 00 00 00       	add    $0x98,%eax
	if(a->state != SLEEPING&&a->state != RUNNING&&a->state != RUNNABLE)
		continue;
	a->runtimeDeadline = a->runtimeDeadline - tempp;
	if(a->runtimeDeadline <= 0)
a->runtimeDeadline= a->deadline;
80103c44:	89 50 f8             	mov    %edx,-0x8(%eax)
	p->priority=p->lowestPriority;//The process be excute,so reset their priority
	p->runTimes++;

  struct proc *a;
int tempp=p->runtimeDeadline-p->deadline;
  for(a =ptable.proc; a< &ptable.proc[NPROC];a++){
80103c47:	3d 54 53 11 80       	cmp    $0x80115354,%eax
80103c4c:	75 ce                	jne    80103c1c <scheduler+0xec>
		continue;
	a->runtimeDeadline = a->runtimeDeadline - tempp;
	if(a->runtimeDeadline <= 0)
a->runtimeDeadline= a->deadline;
	}
		p->runtimeDeadline = p->runtimeDeadline + p->deadline;
80103c4e:	8b 7d e4             	mov    -0x1c(%ebp),%edi


  int priorityNow = priorityDefault;//priority sort
80103c51:	8b 35 04 a0 10 80    	mov    0x8010a004,%esi
  int maxDeadline=0;
  int tempDeadline=-1;
80103c57:	bb ff ff ff ff       	mov    $0xffffffff,%ebx
		continue;
	a->runtimeDeadline = a->runtimeDeadline - tempp;
	if(a->runtimeDeadline <= 0)
a->runtimeDeadline= a->deadline;
	}
		p->runtimeDeadline = p->runtimeDeadline + p->deadline;
80103c5c:	8b 87 8c 00 00 00    	mov    0x8c(%edi),%eax
80103c62:	01 87 90 00 00 00    	add    %eax,0x90(%edi)
  int priorityNow = priorityDefault;//priority sort
  int maxDeadline=0;
  int tempDeadline=-1;

  struct proc *b;
  for(a =ptable.proc; a< &ptable.proc[NPROC];a++){
80103c68:	bf 54 2d 11 80       	mov    $0x80112d54,%edi
80103c6d:	eb 0f                	jmp    80103c7e <scheduler+0x14e>
80103c6f:	90                   	nop
80103c70:	81 c7 98 00 00 00    	add    $0x98,%edi
80103c76:	81 ff 54 53 11 80    	cmp    $0x80115354,%edi
80103c7c:	74 71                	je     80103cef <scheduler+0x1bf>
	if(a->state != SLEEPING&&a->state != RUNNING&&a->state != RUNNABLE)
80103c7e:	8b 47 0c             	mov    0xc(%edi),%eax
80103c81:	83 e8 02             	sub    $0x2,%eax
80103c84:	83 f8 02             	cmp    $0x2,%eax
80103c87:	77 e7                	ja     80103c70 <scheduler+0x140>
80103c89:	b8 54 2d 11 80       	mov    $0x80112d54,%eax
80103c8e:	31 c9                	xor    %ecx,%ecx
80103c90:	eb 12                	jmp    80103ca4 <scheduler+0x174>
80103c92:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
		continue;
	for(b =ptable.proc; b< &ptable.proc[NPROC];b++){
80103c98:	05 98 00 00 00       	add    $0x98,%eax
80103c9d:	3d 54 53 11 80       	cmp    $0x80115354,%eax
80103ca2:	74 38                	je     80103cdc <scheduler+0x1ac>
		if(b->state != SLEEPING&&b->state != RUNNING&&b->state != RUNNABLE)
80103ca4:	8b 50 0c             	mov    0xc(%eax),%edx
80103ca7:	83 ea 02             	sub    $0x2,%edx
80103caa:	83 fa 02             	cmp    $0x2,%edx
80103cad:	77 e9                	ja     80103c98 <scheduler+0x168>
			continue;
	if(tempDeadline==-1||tempDeadline > b->runtimeDeadline)
80103caf:	83 fb ff             	cmp    $0xffffffff,%ebx
80103cb2:	8b 90 90 00 00 00    	mov    0x90(%eax),%edx
80103cb8:	74 04                	je     80103cbe <scheduler+0x18e>
80103cba:	39 da                	cmp    %ebx,%edx
80103cbc:	7d da                	jge    80103c98 <scheduler+0x168>
	{
	      b->lowestPriority = priorityNow;
	      b->priority = priorityNow;
	      if(maxDeadline==0||maxDeadline < b->runtimeDeadline)
80103cbe:	85 c9                	test   %ecx,%ecx
	for(b =ptable.proc; b< &ptable.proc[NPROC];b++){
		if(b->state != SLEEPING&&b->state != RUNNING&&b->state != RUNNABLE)
			continue;
	if(tempDeadline==-1||tempDeadline > b->runtimeDeadline)
	{
	      b->lowestPriority = priorityNow;
80103cc0:	89 b0 80 00 00 00    	mov    %esi,0x80(%eax)
	      b->priority = priorityNow;
80103cc6:	89 70 7c             	mov    %esi,0x7c(%eax)
	      if(maxDeadline==0||maxDeadline < b->runtimeDeadline)
80103cc9:	74 6d                	je     80103d38 <scheduler+0x208>
80103ccb:	39 d1                	cmp    %edx,%ecx
80103ccd:	0f 4c ca             	cmovl  %edx,%ecx

  struct proc *b;
  for(a =ptable.proc; a< &ptable.proc[NPROC];a++){
	if(a->state != SLEEPING&&a->state != RUNNING&&a->state != RUNNABLE)
		continue;
	for(b =ptable.proc; b< &ptable.proc[NPROC];b++){
80103cd0:	05 98 00 00 00       	add    $0x98,%eax
80103cd5:	3d 54 53 11 80       	cmp    $0x80115354,%eax
80103cda:	75 c8                	jne    80103ca4 <scheduler+0x174>
  int priorityNow = priorityDefault;//priority sort
  int maxDeadline=0;
  int tempDeadline=-1;

  struct proc *b;
  for(a =ptable.proc; a< &ptable.proc[NPROC];a++){
80103cdc:	81 c7 98 00 00 00    	add    $0x98,%edi
	         maxDeadline = b->runtimeDeadline;
	}
      }
	tempDeadline=maxDeadline;
	maxDeadline=0;
	priorityNow = priorityNow+1;
80103ce2:	83 c6 01             	add    $0x1,%esi
80103ce5:	89 cb                	mov    %ecx,%ebx
  int priorityNow = priorityDefault;//priority sort
  int maxDeadline=0;
  int tempDeadline=-1;

  struct proc *b;
  for(a =ptable.proc; a< &ptable.proc[NPROC];a++){
80103ce7:	81 ff 54 53 11 80    	cmp    $0x80115354,%edi
80103ced:	75 8f                	jne    80103c7e <scheduler+0x14e>
  }

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      c->proc = p;
80103cef:	8b 45 e4             	mov    -0x1c(%ebp),%eax
80103cf2:	8b 75 dc             	mov    -0x24(%ebp),%esi
      switchuvm(p);
80103cf5:	83 ec 0c             	sub    $0xc,%esp
  }

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      c->proc = p;
80103cf8:	89 86 ac 00 00 00    	mov    %eax,0xac(%esi)
      switchuvm(p);
80103cfe:	50                   	push   %eax
80103cff:	89 c7                	mov    %eax,%edi
80103d01:	e8 ca 30 00 00       	call   80106dd0 <switchuvm>
      p->state = RUNNING;
80103d06:	c7 47 0c 04 00 00 00 	movl   $0x4,0xc(%edi)
//      p->lowestPriority=p->lowestPriority+1;
      swtch(&(c->scheduler), p->context);
80103d0d:	5a                   	pop    %edx
80103d0e:	59                   	pop    %ecx
80103d0f:	ff 77 1c             	pushl  0x1c(%edi)
80103d12:	ff 75 d8             	pushl  -0x28(%ebp)
80103d15:	e8 21 0e 00 00       	call   80104b3b <swtch>
      switchkvm();
80103d1a:	e8 91 30 00 00       	call   80106db0 <switchkvm>

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
80103d1f:	c7 86 ac 00 00 00 00 	movl   $0x0,0xac(%esi)
80103d26:	00 00 00 
80103d29:	83 c4 10             	add    $0x10,%esp
80103d2c:	e9 77 fe ff ff       	jmp    80103ba8 <scheduler+0x78>
80103d31:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80103d38:	89 d1                	mov    %edx,%ecx
80103d3a:	e9 59 ff ff ff       	jmp    80103c98 <scheduler+0x168>
    }
    release(&ptable.lock);
80103d3f:	83 ec 0c             	sub    $0xc,%esp
80103d42:	68 20 2d 11 80       	push   $0x80112d20
80103d47:	e8 44 0b 00 00       	call   80104890 <release>

  }
80103d4c:	83 c4 10             	add    $0x10,%esp
80103d4f:	e9 fd fd ff ff       	jmp    80103b51 <scheduler+0x21>
80103d54:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80103d5a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

80103d60 <sched>:
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void)
{
80103d60:	55                   	push   %ebp
80103d61:	89 e5                	mov    %esp,%ebp
80103d63:	56                   	push   %esi
80103d64:	53                   	push   %ebx
// while reading proc from the cpu structure
struct proc*
myproc(void) {
  struct cpu *c;
  struct proc *p;
  pushcli();
80103d65:	e8 c6 09 00 00       	call   80104730 <pushcli>
  c = mycpu();
80103d6a:	e8 61 fa ff ff       	call   801037d0 <mycpu>
  p = c->proc;
80103d6f:	8b 98 ac 00 00 00    	mov    0xac(%eax),%ebx
  popcli();
80103d75:	e8 a6 0a 00 00       	call   80104820 <popcli>
sched(void)
{
  int intena;
  struct proc *p = myproc();

  if(!holding(&ptable.lock))
80103d7a:	83 ec 0c             	sub    $0xc,%esp
80103d7d:	68 20 2d 11 80       	push   $0x80112d20
80103d82:	e8 69 09 00 00       	call   801046f0 <holding>
80103d87:	83 c4 10             	add    $0x10,%esp
80103d8a:	85 c0                	test   %eax,%eax
80103d8c:	74 4f                	je     80103ddd <sched+0x7d>
    panic("sched ptable.lock");
  if(mycpu()->ncli != 1)
80103d8e:	e8 3d fa ff ff       	call   801037d0 <mycpu>
80103d93:	83 b8 a4 00 00 00 01 	cmpl   $0x1,0xa4(%eax)
80103d9a:	75 68                	jne    80103e04 <sched+0xa4>
    panic("sched locks");
  if(p->state == RUNNING)
80103d9c:	83 7b 0c 04          	cmpl   $0x4,0xc(%ebx)
80103da0:	74 55                	je     80103df7 <sched+0x97>

static inline uint
readeflags(void)
{
  uint eflags;
  asm volatile("pushfl; popl %0" : "=r" (eflags));
80103da2:	9c                   	pushf  
80103da3:	58                   	pop    %eax
    panic("sched running");
  if(readeflags()&FL_IF)
80103da4:	f6 c4 02             	test   $0x2,%ah
80103da7:	75 41                	jne    80103dea <sched+0x8a>
    panic("sched interruptible");
  intena = mycpu()->intena;
80103da9:	e8 22 fa ff ff       	call   801037d0 <mycpu>
  swtch(&p->context, mycpu()->scheduler);
80103dae:	83 c3 1c             	add    $0x1c,%ebx
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
80103db1:	8b b0 a8 00 00 00    	mov    0xa8(%eax),%esi
  swtch(&p->context, mycpu()->scheduler);
80103db7:	e8 14 fa ff ff       	call   801037d0 <mycpu>
80103dbc:	83 ec 08             	sub    $0x8,%esp
80103dbf:	ff 70 04             	pushl  0x4(%eax)
80103dc2:	53                   	push   %ebx
80103dc3:	e8 73 0d 00 00       	call   80104b3b <swtch>
  mycpu()->intena = intena;
80103dc8:	e8 03 fa ff ff       	call   801037d0 <mycpu>
}
80103dcd:	83 c4 10             	add    $0x10,%esp
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  swtch(&p->context, mycpu()->scheduler);
  mycpu()->intena = intena;
80103dd0:	89 b0 a8 00 00 00    	mov    %esi,0xa8(%eax)
}
80103dd6:	8d 65 f8             	lea    -0x8(%ebp),%esp
80103dd9:	5b                   	pop    %ebx
80103dda:	5e                   	pop    %esi
80103ddb:	5d                   	pop    %ebp
80103ddc:	c3                   	ret    
{
  int intena;
  struct proc *p = myproc();

  if(!holding(&ptable.lock))
    panic("sched ptable.lock");
80103ddd:	83 ec 0c             	sub    $0xc,%esp
80103de0:	68 10 7a 10 80       	push   $0x80107a10
80103de5:	e8 86 c5 ff ff       	call   80100370 <panic>
  if(mycpu()->ncli != 1)
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
80103dea:	83 ec 0c             	sub    $0xc,%esp
80103ded:	68 3c 7a 10 80       	push   $0x80107a3c
80103df2:	e8 79 c5 ff ff       	call   80100370 <panic>
  if(!holding(&ptable.lock))
    panic("sched ptable.lock");
  if(mycpu()->ncli != 1)
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
80103df7:	83 ec 0c             	sub    $0xc,%esp
80103dfa:	68 2e 7a 10 80       	push   $0x80107a2e
80103dff:	e8 6c c5 ff ff       	call   80100370 <panic>
  struct proc *p = myproc();

  if(!holding(&ptable.lock))
    panic("sched ptable.lock");
  if(mycpu()->ncli != 1)
    panic("sched locks");
80103e04:	83 ec 0c             	sub    $0xc,%esp
80103e07:	68 22 7a 10 80       	push   $0x80107a22
80103e0c:	e8 5f c5 ff ff       	call   80100370 <panic>
80103e11:	eb 0d                	jmp    80103e20 <exit>
80103e13:	90                   	nop
80103e14:	90                   	nop
80103e15:	90                   	nop
80103e16:	90                   	nop
80103e17:	90                   	nop
80103e18:	90                   	nop
80103e19:	90                   	nop
80103e1a:	90                   	nop
80103e1b:	90                   	nop
80103e1c:	90                   	nop
80103e1d:	90                   	nop
80103e1e:	90                   	nop
80103e1f:	90                   	nop

80103e20 <exit>:
// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
80103e20:	55                   	push   %ebp
80103e21:	89 e5                	mov    %esp,%ebp
80103e23:	57                   	push   %edi
80103e24:	56                   	push   %esi
80103e25:	53                   	push   %ebx
80103e26:	83 ec 0c             	sub    $0xc,%esp
// while reading proc from the cpu structure
struct proc*
myproc(void) {
  struct cpu *c;
  struct proc *p;
  pushcli();
80103e29:	e8 02 09 00 00       	call   80104730 <pushcli>
  c = mycpu();
80103e2e:	e8 9d f9 ff ff       	call   801037d0 <mycpu>
  p = c->proc;
80103e33:	8b b0 ac 00 00 00    	mov    0xac(%eax),%esi
  popcli();
80103e39:	e8 e2 09 00 00       	call   80104820 <popcli>
{
  struct proc *curproc = myproc();
  struct proc *p;
  int fd;

  if(curproc == initproc)
80103e3e:	39 35 b8 a5 10 80    	cmp    %esi,0x8010a5b8
80103e44:	8d 5e 28             	lea    0x28(%esi),%ebx
80103e47:	8d 7e 68             	lea    0x68(%esi),%edi
80103e4a:	0f 84 f1 00 00 00    	je     80103f41 <exit+0x121>
    panic("init exiting");

  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd]){
80103e50:	8b 03                	mov    (%ebx),%eax
80103e52:	85 c0                	test   %eax,%eax
80103e54:	74 12                	je     80103e68 <exit+0x48>
      fileclose(curproc->ofile[fd]);
80103e56:	83 ec 0c             	sub    $0xc,%esp
80103e59:	50                   	push   %eax
80103e5a:	e8 d1 cf ff ff       	call   80100e30 <fileclose>
      curproc->ofile[fd] = 0;
80103e5f:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)
80103e65:	83 c4 10             	add    $0x10,%esp
80103e68:	83 c3 04             	add    $0x4,%ebx

  if(curproc == initproc)
    panic("init exiting");

  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
80103e6b:	39 df                	cmp    %ebx,%edi
80103e6d:	75 e1                	jne    80103e50 <exit+0x30>
      fileclose(curproc->ofile[fd]);
      curproc->ofile[fd] = 0;
    }
  }

  begin_op();
80103e6f:	e8 dc ec ff ff       	call   80102b50 <begin_op>
  iput(curproc->cwd);
80103e74:	83 ec 0c             	sub    $0xc,%esp
80103e77:	ff 76 68             	pushl  0x68(%esi)
80103e7a:	e8 21 d9 ff ff       	call   801017a0 <iput>
  end_op();
80103e7f:	e8 3c ed ff ff       	call   80102bc0 <end_op>
  curproc->cwd = 0;
80103e84:	c7 46 68 00 00 00 00 	movl   $0x0,0x68(%esi)

  acquire(&ptable.lock);
80103e8b:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
80103e92:	e8 d9 08 00 00       	call   80104770 <acquire>

  // Parent might be sleeping in wait().
  wakeup1(curproc->parent);
80103e97:	8b 56 14             	mov    0x14(%esi),%edx
80103e9a:	83 c4 10             	add    $0x10,%esp
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
80103e9d:	b8 54 2d 11 80       	mov    $0x80112d54,%eax
80103ea2:	eb 10                	jmp    80103eb4 <exit+0x94>
80103ea4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80103ea8:	05 98 00 00 00       	add    $0x98,%eax
80103ead:	3d 54 53 11 80       	cmp    $0x80115354,%eax
80103eb2:	74 1e                	je     80103ed2 <exit+0xb2>
    if(p->state == SLEEPING && p->chan == chan)
80103eb4:	83 78 0c 02          	cmpl   $0x2,0xc(%eax)
80103eb8:	75 ee                	jne    80103ea8 <exit+0x88>
80103eba:	3b 50 20             	cmp    0x20(%eax),%edx
80103ebd:	75 e9                	jne    80103ea8 <exit+0x88>
      p->state = RUNNABLE;
80103ebf:	c7 40 0c 03 00 00 00 	movl   $0x3,0xc(%eax)
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
80103ec6:	05 98 00 00 00       	add    $0x98,%eax
80103ecb:	3d 54 53 11 80       	cmp    $0x80115354,%eax
80103ed0:	75 e2                	jne    80103eb4 <exit+0x94>
  wakeup1(curproc->parent);

  // Pass abandoned children to init.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == curproc){
      p->parent = initproc;
80103ed2:	8b 0d b8 a5 10 80    	mov    0x8010a5b8,%ecx
80103ed8:	ba 54 2d 11 80       	mov    $0x80112d54,%edx
80103edd:	eb 0f                	jmp    80103eee <exit+0xce>
80103edf:	90                   	nop

  // Parent might be sleeping in wait().
  wakeup1(curproc->parent);

  // Pass abandoned children to init.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
80103ee0:	81 c2 98 00 00 00    	add    $0x98,%edx
80103ee6:	81 fa 54 53 11 80    	cmp    $0x80115354,%edx
80103eec:	74 3a                	je     80103f28 <exit+0x108>
    if(p->parent == curproc){
80103eee:	39 72 14             	cmp    %esi,0x14(%edx)
80103ef1:	75 ed                	jne    80103ee0 <exit+0xc0>
      p->parent = initproc;
      if(p->state == ZOMBIE)
80103ef3:	83 7a 0c 05          	cmpl   $0x5,0xc(%edx)
  wakeup1(curproc->parent);

  // Pass abandoned children to init.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == curproc){
      p->parent = initproc;
80103ef7:	89 4a 14             	mov    %ecx,0x14(%edx)
      if(p->state == ZOMBIE)
80103efa:	75 e4                	jne    80103ee0 <exit+0xc0>
80103efc:	b8 54 2d 11 80       	mov    $0x80112d54,%eax
80103f01:	eb 11                	jmp    80103f14 <exit+0xf4>
80103f03:	90                   	nop
80103f04:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
80103f08:	05 98 00 00 00       	add    $0x98,%eax
80103f0d:	3d 54 53 11 80       	cmp    $0x80115354,%eax
80103f12:	74 cc                	je     80103ee0 <exit+0xc0>
    if(p->state == SLEEPING && p->chan == chan)
80103f14:	83 78 0c 02          	cmpl   $0x2,0xc(%eax)
80103f18:	75 ee                	jne    80103f08 <exit+0xe8>
80103f1a:	3b 48 20             	cmp    0x20(%eax),%ecx
80103f1d:	75 e9                	jne    80103f08 <exit+0xe8>
      p->state = RUNNABLE;
80103f1f:	c7 40 0c 03 00 00 00 	movl   $0x3,0xc(%eax)
80103f26:	eb e0                	jmp    80103f08 <exit+0xe8>
        wakeup1(initproc);
    }
  }

  // Jump into the scheduler, never to return.
  curproc->state = ZOMBIE;
80103f28:	c7 46 0c 05 00 00 00 	movl   $0x5,0xc(%esi)
  sched();
80103f2f:	e8 2c fe ff ff       	call   80103d60 <sched>
  panic("zombie exit");
80103f34:	83 ec 0c             	sub    $0xc,%esp
80103f37:	68 5d 7a 10 80       	push   $0x80107a5d
80103f3c:	e8 2f c4 ff ff       	call   80100370 <panic>
  struct proc *curproc = myproc();
  struct proc *p;
  int fd;

  if(curproc == initproc)
    panic("init exiting");
80103f41:	83 ec 0c             	sub    $0xc,%esp
80103f44:	68 50 7a 10 80       	push   $0x80107a50
80103f49:	e8 22 c4 ff ff       	call   80100370 <panic>
80103f4e:	66 90                	xchg   %ax,%ax

80103f50 <yield>:
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
80103f50:	55                   	push   %ebp
80103f51:	89 e5                	mov    %esp,%ebp
80103f53:	53                   	push   %ebx
80103f54:	83 ec 10             	sub    $0x10,%esp
  acquire(&ptable.lock);  //DOC: yieldlock
80103f57:	68 20 2d 11 80       	push   $0x80112d20
80103f5c:	e8 0f 08 00 00       	call   80104770 <acquire>
// while reading proc from the cpu structure
struct proc*
myproc(void) {
  struct cpu *c;
  struct proc *p;
  pushcli();
80103f61:	e8 ca 07 00 00       	call   80104730 <pushcli>
  c = mycpu();
80103f66:	e8 65 f8 ff ff       	call   801037d0 <mycpu>
  p = c->proc;
80103f6b:	8b 98 ac 00 00 00    	mov    0xac(%eax),%ebx
  popcli();
80103f71:	e8 aa 08 00 00       	call   80104820 <popcli>
// Give up the CPU for one scheduling round.
void
yield(void)
{
  acquire(&ptable.lock);  //DOC: yieldlock
  myproc()->state = RUNNABLE;
80103f76:	c7 43 0c 03 00 00 00 	movl   $0x3,0xc(%ebx)
  sched();
80103f7d:	e8 de fd ff ff       	call   80103d60 <sched>
  release(&ptable.lock);
80103f82:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
80103f89:	e8 02 09 00 00       	call   80104890 <release>
}
80103f8e:	83 c4 10             	add    $0x10,%esp
80103f91:	8b 5d fc             	mov    -0x4(%ebp),%ebx
80103f94:	c9                   	leave  
80103f95:	c3                   	ret    
80103f96:	8d 76 00             	lea    0x0(%esi),%esi
80103f99:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80103fa0 <sleep>:

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
80103fa0:	55                   	push   %ebp
80103fa1:	89 e5                	mov    %esp,%ebp
80103fa3:	57                   	push   %edi
80103fa4:	56                   	push   %esi
80103fa5:	53                   	push   %ebx
80103fa6:	83 ec 0c             	sub    $0xc,%esp
80103fa9:	8b 7d 08             	mov    0x8(%ebp),%edi
80103fac:	8b 75 0c             	mov    0xc(%ebp),%esi
// while reading proc from the cpu structure
struct proc*
myproc(void) {
  struct cpu *c;
  struct proc *p;
  pushcli();
80103faf:	e8 7c 07 00 00       	call   80104730 <pushcli>
  c = mycpu();
80103fb4:	e8 17 f8 ff ff       	call   801037d0 <mycpu>
  p = c->proc;
80103fb9:	8b 98 ac 00 00 00    	mov    0xac(%eax),%ebx
  popcli();
80103fbf:	e8 5c 08 00 00       	call   80104820 <popcli>
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  
  if(p == 0)
80103fc4:	85 db                	test   %ebx,%ebx
80103fc6:	0f 84 87 00 00 00    	je     80104053 <sleep+0xb3>
    panic("sleep");

  if(lk == 0)
80103fcc:	85 f6                	test   %esi,%esi
80103fce:	74 76                	je     80104046 <sleep+0xa6>
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with ptable.lock locked),
  // so it's okay to release lk.
  if(lk != &ptable.lock){  //DOC: sleeplock0
80103fd0:	81 fe 20 2d 11 80    	cmp    $0x80112d20,%esi
80103fd6:	74 50                	je     80104028 <sleep+0x88>
    acquire(&ptable.lock);  //DOC: sleeplock1
80103fd8:	83 ec 0c             	sub    $0xc,%esp
80103fdb:	68 20 2d 11 80       	push   $0x80112d20
80103fe0:	e8 8b 07 00 00       	call   80104770 <acquire>
    release(lk);
80103fe5:	89 34 24             	mov    %esi,(%esp)
80103fe8:	e8 a3 08 00 00       	call   80104890 <release>
  }
  // Go to sleep.
  p->chan = chan;
80103fed:	89 7b 20             	mov    %edi,0x20(%ebx)
  p->state = SLEEPING;
80103ff0:	c7 43 0c 02 00 00 00 	movl   $0x2,0xc(%ebx)

  sched();
80103ff7:	e8 64 fd ff ff       	call   80103d60 <sched>

  // Tidy up.
  p->chan = 0;
80103ffc:	c7 43 20 00 00 00 00 	movl   $0x0,0x20(%ebx)

  // Reacquire original lock.
  if(lk != &ptable.lock){  //DOC: sleeplock2
    release(&ptable.lock);
80104003:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
8010400a:	e8 81 08 00 00       	call   80104890 <release>
    acquire(lk);
8010400f:	89 75 08             	mov    %esi,0x8(%ebp)
80104012:	83 c4 10             	add    $0x10,%esp
  }
}
80104015:	8d 65 f4             	lea    -0xc(%ebp),%esp
80104018:	5b                   	pop    %ebx
80104019:	5e                   	pop    %esi
8010401a:	5f                   	pop    %edi
8010401b:	5d                   	pop    %ebp
  p->chan = 0;

  // Reacquire original lock.
  if(lk != &ptable.lock){  //DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
8010401c:	e9 4f 07 00 00       	jmp    80104770 <acquire>
80104021:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  if(lk != &ptable.lock){  //DOC: sleeplock0
    acquire(&ptable.lock);  //DOC: sleeplock1
    release(lk);
  }
  // Go to sleep.
  p->chan = chan;
80104028:	89 7b 20             	mov    %edi,0x20(%ebx)
  p->state = SLEEPING;
8010402b:	c7 43 0c 02 00 00 00 	movl   $0x2,0xc(%ebx)

  sched();
80104032:	e8 29 fd ff ff       	call   80103d60 <sched>

  // Tidy up.
  p->chan = 0;
80104037:	c7 43 20 00 00 00 00 	movl   $0x0,0x20(%ebx)
  // Reacquire original lock.
  if(lk != &ptable.lock){  //DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}
8010403e:	8d 65 f4             	lea    -0xc(%ebp),%esp
80104041:	5b                   	pop    %ebx
80104042:	5e                   	pop    %esi
80104043:	5f                   	pop    %edi
80104044:	5d                   	pop    %ebp
80104045:	c3                   	ret    
  
  if(p == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");
80104046:	83 ec 0c             	sub    $0xc,%esp
80104049:	68 6f 7a 10 80       	push   $0x80107a6f
8010404e:	e8 1d c3 ff ff       	call   80100370 <panic>
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  
  if(p == 0)
    panic("sleep");
80104053:	83 ec 0c             	sub    $0xc,%esp
80104056:	68 69 7a 10 80       	push   $0x80107a69
8010405b:	e8 10 c3 ff ff       	call   80100370 <panic>

80104060 <wait>:

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(void)
{
80104060:	55                   	push   %ebp
80104061:	89 e5                	mov    %esp,%ebp
80104063:	56                   	push   %esi
80104064:	53                   	push   %ebx
// while reading proc from the cpu structure
struct proc*
myproc(void) {
  struct cpu *c;
  struct proc *p;
  pushcli();
80104065:	e8 c6 06 00 00       	call   80104730 <pushcli>
  c = mycpu();
8010406a:	e8 61 f7 ff ff       	call   801037d0 <mycpu>
  p = c->proc;
8010406f:	8b b0 ac 00 00 00    	mov    0xac(%eax),%esi
  popcli();
80104075:	e8 a6 07 00 00       	call   80104820 <popcli>
{
  struct proc *p;
  int havekids, pid;
  struct proc *curproc = myproc();
  
  acquire(&ptable.lock);
8010407a:	83 ec 0c             	sub    $0xc,%esp
8010407d:	68 20 2d 11 80       	push   $0x80112d20
80104082:	e8 e9 06 00 00       	call   80104770 <acquire>
80104087:	83 c4 10             	add    $0x10,%esp
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
8010408a:	31 c0                	xor    %eax,%eax
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
8010408c:	bb 54 2d 11 80       	mov    $0x80112d54,%ebx
80104091:	eb 13                	jmp    801040a6 <wait+0x46>
80104093:	90                   	nop
80104094:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80104098:	81 c3 98 00 00 00    	add    $0x98,%ebx
8010409e:	81 fb 54 53 11 80    	cmp    $0x80115354,%ebx
801040a4:	74 22                	je     801040c8 <wait+0x68>
      if(p->parent != curproc)
801040a6:	39 73 14             	cmp    %esi,0x14(%ebx)
801040a9:	75 ed                	jne    80104098 <wait+0x38>
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
801040ab:	83 7b 0c 05          	cmpl   $0x5,0xc(%ebx)
801040af:	74 35                	je     801040e6 <wait+0x86>
  
  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
801040b1:	81 c3 98 00 00 00    	add    $0x98,%ebx
      if(p->parent != curproc)
        continue;
      havekids = 1;
801040b7:	b8 01 00 00 00       	mov    $0x1,%eax
  
  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
801040bc:	81 fb 54 53 11 80    	cmp    $0x80115354,%ebx
801040c2:	75 e2                	jne    801040a6 <wait+0x46>
801040c4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || curproc->killed){
801040c8:	85 c0                	test   %eax,%eax
801040ca:	74 70                	je     8010413c <wait+0xdc>
801040cc:	8b 46 24             	mov    0x24(%esi),%eax
801040cf:	85 c0                	test   %eax,%eax
801040d1:	75 69                	jne    8010413c <wait+0xdc>
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock);  //DOC: wait-sleep
801040d3:	83 ec 08             	sub    $0x8,%esp
801040d6:	68 20 2d 11 80       	push   $0x80112d20
801040db:	56                   	push   %esi
801040dc:	e8 bf fe ff ff       	call   80103fa0 <sleep>
  }
801040e1:	83 c4 10             	add    $0x10,%esp
801040e4:	eb a4                	jmp    8010408a <wait+0x2a>
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
        kfree(p->kstack);
801040e6:	83 ec 0c             	sub    $0xc,%esp
801040e9:	ff 73 08             	pushl  0x8(%ebx)
      if(p->parent != curproc)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
801040ec:	8b 73 10             	mov    0x10(%ebx),%esi
        kfree(p->kstack);
801040ef:	e8 ec e1 ff ff       	call   801022e0 <kfree>
        p->kstack = 0;
        freevm(p->pgdir);
801040f4:	5a                   	pop    %edx
801040f5:	ff 73 04             	pushl  0x4(%ebx)
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
801040f8:	c7 43 08 00 00 00 00 	movl   $0x0,0x8(%ebx)
        freevm(p->pgdir);
801040ff:	e8 4c 30 00 00       	call   80107150 <freevm>
        p->pid = 0;
80104104:	c7 43 10 00 00 00 00 	movl   $0x0,0x10(%ebx)
        p->parent = 0;
8010410b:	c7 43 14 00 00 00 00 	movl   $0x0,0x14(%ebx)
        p->name[0] = 0;
80104112:	c6 43 6c 00          	movb   $0x0,0x6c(%ebx)
        p->killed = 0;
80104116:	c7 43 24 00 00 00 00 	movl   $0x0,0x24(%ebx)
        p->state = UNUSED;
8010411d:	c7 43 0c 00 00 00 00 	movl   $0x0,0xc(%ebx)
        release(&ptable.lock);
80104124:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
8010412b:	e8 60 07 00 00       	call   80104890 <release>
        return pid;
80104130:	83 c4 10             	add    $0x10,%esp
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock);  //DOC: wait-sleep
  }
}
80104133:	8d 65 f8             	lea    -0x8(%ebp),%esp
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        release(&ptable.lock);
        return pid;
80104136:	89 f0                	mov    %esi,%eax
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock);  //DOC: wait-sleep
  }
}
80104138:	5b                   	pop    %ebx
80104139:	5e                   	pop    %esi
8010413a:	5d                   	pop    %ebp
8010413b:	c3                   	ret    
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || curproc->killed){
      release(&ptable.lock);
8010413c:	83 ec 0c             	sub    $0xc,%esp
8010413f:	68 20 2d 11 80       	push   $0x80112d20
80104144:	e8 47 07 00 00       	call   80104890 <release>
      return -1;
80104149:	83 c4 10             	add    $0x10,%esp
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock);  //DOC: wait-sleep
  }
}
8010414c:	8d 65 f8             	lea    -0x8(%ebp),%esp
    }

    // No point waiting if we don't have any children.
    if(!havekids || curproc->killed){
      release(&ptable.lock);
      return -1;
8010414f:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock);  //DOC: wait-sleep
  }
}
80104154:	5b                   	pop    %ebx
80104155:	5e                   	pop    %esi
80104156:	5d                   	pop    %ebp
80104157:	c3                   	ret    
80104158:	90                   	nop
80104159:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

80104160 <wakeup>:
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
80104160:	55                   	push   %ebp
80104161:	89 e5                	mov    %esp,%ebp
80104163:	53                   	push   %ebx
80104164:	83 ec 10             	sub    $0x10,%esp
80104167:	8b 5d 08             	mov    0x8(%ebp),%ebx
  acquire(&ptable.lock);
8010416a:	68 20 2d 11 80       	push   $0x80112d20
8010416f:	e8 fc 05 00 00       	call   80104770 <acquire>
80104174:	83 c4 10             	add    $0x10,%esp
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
80104177:	b8 54 2d 11 80       	mov    $0x80112d54,%eax
8010417c:	eb 0e                	jmp    8010418c <wakeup+0x2c>
8010417e:	66 90                	xchg   %ax,%ax
80104180:	05 98 00 00 00       	add    $0x98,%eax
80104185:	3d 54 53 11 80       	cmp    $0x80115354,%eax
8010418a:	74 1e                	je     801041aa <wakeup+0x4a>
    if(p->state == SLEEPING && p->chan == chan)
8010418c:	83 78 0c 02          	cmpl   $0x2,0xc(%eax)
80104190:	75 ee                	jne    80104180 <wakeup+0x20>
80104192:	3b 58 20             	cmp    0x20(%eax),%ebx
80104195:	75 e9                	jne    80104180 <wakeup+0x20>
      p->state = RUNNABLE;
80104197:	c7 40 0c 03 00 00 00 	movl   $0x3,0xc(%eax)
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
8010419e:	05 98 00 00 00       	add    $0x98,%eax
801041a3:	3d 54 53 11 80       	cmp    $0x80115354,%eax
801041a8:	75 e2                	jne    8010418c <wakeup+0x2c>
void
wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
801041aa:	c7 45 08 20 2d 11 80 	movl   $0x80112d20,0x8(%ebp)
}
801041b1:	8b 5d fc             	mov    -0x4(%ebp),%ebx
801041b4:	c9                   	leave  
void
wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
801041b5:	e9 d6 06 00 00       	jmp    80104890 <release>
801041ba:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

801041c0 <kill>:
// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
801041c0:	55                   	push   %ebp
801041c1:	89 e5                	mov    %esp,%ebp
801041c3:	53                   	push   %ebx
801041c4:	83 ec 10             	sub    $0x10,%esp
801041c7:	8b 5d 08             	mov    0x8(%ebp),%ebx
  struct proc *p;

  acquire(&ptable.lock);
801041ca:	68 20 2d 11 80       	push   $0x80112d20
801041cf:	e8 9c 05 00 00       	call   80104770 <acquire>
801041d4:	83 c4 10             	add    $0x10,%esp
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
801041d7:	b8 54 2d 11 80       	mov    $0x80112d54,%eax
801041dc:	eb 0e                	jmp    801041ec <kill+0x2c>
801041de:	66 90                	xchg   %ax,%ax
801041e0:	05 98 00 00 00       	add    $0x98,%eax
801041e5:	3d 54 53 11 80       	cmp    $0x80115354,%eax
801041ea:	74 3c                	je     80104228 <kill+0x68>
    if(p->pid == pid){
801041ec:	39 58 10             	cmp    %ebx,0x10(%eax)
801041ef:	75 ef                	jne    801041e0 <kill+0x20>
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
801041f1:	83 78 0c 02          	cmpl   $0x2,0xc(%eax)
  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
801041f5:	c7 40 24 01 00 00 00 	movl   $0x1,0x24(%eax)
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
801041fc:	74 1a                	je     80104218 <kill+0x58>
        p->state = RUNNABLE;
      release(&ptable.lock);
801041fe:	83 ec 0c             	sub    $0xc,%esp
80104201:	68 20 2d 11 80       	push   $0x80112d20
80104206:	e8 85 06 00 00       	call   80104890 <release>
      return 0;
8010420b:	83 c4 10             	add    $0x10,%esp
8010420e:	31 c0                	xor    %eax,%eax
    }
  }
  release(&ptable.lock);
  return -1;
}
80104210:	8b 5d fc             	mov    -0x4(%ebp),%ebx
80104213:	c9                   	leave  
80104214:	c3                   	ret    
80104215:	8d 76 00             	lea    0x0(%esi),%esi
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
80104218:	c7 40 0c 03 00 00 00 	movl   $0x3,0xc(%eax)
8010421f:	eb dd                	jmp    801041fe <kill+0x3e>
80104221:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
80104228:	83 ec 0c             	sub    $0xc,%esp
8010422b:	68 20 2d 11 80       	push   $0x80112d20
80104230:	e8 5b 06 00 00       	call   80104890 <release>
  return -1;
80104235:	83 c4 10             	add    $0x10,%esp
80104238:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
8010423d:	8b 5d fc             	mov    -0x4(%ebp),%ebx
80104240:	c9                   	leave  
80104241:	c3                   	ret    
80104242:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80104249:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80104250 <procdump>:
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
80104250:	55                   	push   %ebp
80104251:	89 e5                	mov    %esp,%ebp
80104253:	57                   	push   %edi
80104254:	56                   	push   %esi
80104255:	53                   	push   %ebx
80104256:	8d 75 e8             	lea    -0x18(%ebp),%esi
80104259:	bb c0 2d 11 80       	mov    $0x80112dc0,%ebx
8010425e:	83 ec 3c             	sub    $0x3c,%esp
80104261:	eb 27                	jmp    8010428a <procdump+0x3a>
80104263:	90                   	nop
80104264:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    if(p->state == SLEEPING){
      getcallerpcs((uint*)p->context->ebp+2, pc);
      for(i=0; i<10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
80104268:	83 ec 0c             	sub    $0xc,%esp
8010426b:	68 c3 7e 10 80       	push   $0x80107ec3
80104270:	e8 eb c3 ff ff       	call   80100660 <cprintf>
80104275:	83 c4 10             	add    $0x10,%esp
80104278:	81 c3 98 00 00 00    	add    $0x98,%ebx
  int i;
  struct proc *p;
  char *state;
  uint pc[10];

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
8010427e:	81 fb c0 53 11 80    	cmp    $0x801153c0,%ebx
80104284:	0f 84 7e 00 00 00    	je     80104308 <procdump+0xb8>
    if(p->state == UNUSED)
8010428a:	8b 43 a0             	mov    -0x60(%ebx),%eax
8010428d:	85 c0                	test   %eax,%eax
8010428f:	74 e7                	je     80104278 <procdump+0x28>
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
80104291:	83 f8 05             	cmp    $0x5,%eax
      state = states[p->state];
    else
      state = "???";
80104294:	ba 80 7a 10 80       	mov    $0x80107a80,%edx
  uint pc[10];

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
80104299:	77 11                	ja     801042ac <procdump+0x5c>
8010429b:	8b 14 85 a8 7b 10 80 	mov    -0x7fef8458(,%eax,4),%edx
      state = states[p->state];
    else
      state = "???";
801042a2:	b8 80 7a 10 80       	mov    $0x80107a80,%eax
801042a7:	85 d2                	test   %edx,%edx
801042a9:	0f 44 d0             	cmove  %eax,%edx
    cprintf("%d %s %s", p->pid, state, p->name);
801042ac:	53                   	push   %ebx
801042ad:	52                   	push   %edx
801042ae:	ff 73 a4             	pushl  -0x5c(%ebx)
801042b1:	68 84 7a 10 80       	push   $0x80107a84
801042b6:	e8 a5 c3 ff ff       	call   80100660 <cprintf>
    if(p->state == SLEEPING){
801042bb:	83 c4 10             	add    $0x10,%esp
801042be:	83 7b a0 02          	cmpl   $0x2,-0x60(%ebx)
801042c2:	75 a4                	jne    80104268 <procdump+0x18>
      getcallerpcs((uint*)p->context->ebp+2, pc);
801042c4:	8d 45 c0             	lea    -0x40(%ebp),%eax
801042c7:	83 ec 08             	sub    $0x8,%esp
801042ca:	8d 7d c0             	lea    -0x40(%ebp),%edi
801042cd:	50                   	push   %eax
801042ce:	8b 43 b0             	mov    -0x50(%ebx),%eax
801042d1:	8b 40 0c             	mov    0xc(%eax),%eax
801042d4:	83 c0 08             	add    $0x8,%eax
801042d7:	50                   	push   %eax
801042d8:	e8 b3 03 00 00       	call   80104690 <getcallerpcs>
801042dd:	83 c4 10             	add    $0x10,%esp
      for(i=0; i<10 && pc[i] != 0; i++)
801042e0:	8b 17                	mov    (%edi),%edx
801042e2:	85 d2                	test   %edx,%edx
801042e4:	74 82                	je     80104268 <procdump+0x18>
        cprintf(" %p", pc[i]);
801042e6:	83 ec 08             	sub    $0x8,%esp
801042e9:	83 c7 04             	add    $0x4,%edi
801042ec:	52                   	push   %edx
801042ed:	68 c1 74 10 80       	push   $0x801074c1
801042f2:	e8 69 c3 ff ff       	call   80100660 <cprintf>
    else
      state = "???";
    cprintf("%d %s %s", p->pid, state, p->name);
    if(p->state == SLEEPING){
      getcallerpcs((uint*)p->context->ebp+2, pc);
      for(i=0; i<10 && pc[i] != 0; i++)
801042f7:	83 c4 10             	add    $0x10,%esp
801042fa:	39 f7                	cmp    %esi,%edi
801042fc:	75 e2                	jne    801042e0 <procdump+0x90>
801042fe:	e9 65 ff ff ff       	jmp    80104268 <procdump+0x18>
80104303:	90                   	nop
80104304:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
}
80104308:	8d 65 f4             	lea    -0xc(%ebp),%esp
8010430b:	5b                   	pop    %ebx
8010430c:	5e                   	pop    %esi
8010430d:	5f                   	pop    %edi
8010430e:	5d                   	pop    %ebp
8010430f:	c3                   	ret    

80104310 <cps>:

int
cps(void)
{
80104310:	55                   	push   %ebp
80104311:	89 e5                	mov    %esp,%ebp
80104313:	53                   	push   %ebx
80104314:	83 ec 10             	sub    $0x10,%esp
}

static inline void
sti(void)
{
  asm volatile("sti");
80104317:	fb                   	sti    
  struct proc *p;

  sti();
  acquire(&ptable.lock);
80104318:	68 20 2d 11 80       	push   $0x80112d20
8010431d:	bb c0 2d 11 80       	mov    $0x80112dc0,%ebx
80104322:	e8 49 04 00 00       	call   80104770 <acquire>
  cprintf("*********************************\n");
80104327:	c7 04 24 e0 7a 10 80 	movl   $0x80107ae0,(%esp)
8010432e:	e8 2d c3 ff ff       	call   80100660 <cprintf>
  cprintf("name\tpid\tstate\t  priority\tDeadline  Memory   ProcessTime\n");
80104333:	c7 04 24 04 7b 10 80 	movl   $0x80107b04,(%esp)
8010433a:	e8 21 c3 ff ff       	call   80100660 <cprintf>
8010433f:	83 c4 10             	add    $0x10,%esp
80104342:	eb 20                	jmp    80104364 <cps+0x54>
80104344:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == SLEEPING)
      cprintf("%s\t%d\tSLEEP\t  %d\t\t%d\t  %d\t   %d\n", p->name, p->pid, p->priority,p->deadline,p->sz,p->tickcounts);
    else if ( p->state == RUNNING )
80104348:	83 f8 04             	cmp    $0x4,%eax
8010434b:	74 73                	je     801043c0 <cps+0xb0>
      cprintf("%s\t%d\tRUN\t  %d\t\t%d\t  %d\t   %d\n", p->name, p->pid, p->priority,p->deadline,p->sz,p->tickcounts);
    else if ( p->state == RUNNABLE)
8010434d:	83 f8 03             	cmp    $0x3,%eax
80104350:	0f 84 92 00 00 00    	je     801043e8 <cps+0xd8>
80104356:	81 c3 98 00 00 00    	add    $0x98,%ebx

  sti();
  acquire(&ptable.lock);
  cprintf("*********************************\n");
  cprintf("name\tpid\tstate\t  priority\tDeadline  Memory   ProcessTime\n");
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
8010435c:	81 fb c0 53 11 80    	cmp    $0x801153c0,%ebx
80104362:	74 36                	je     8010439a <cps+0x8a>
    if(p->state == SLEEPING)
80104364:	8b 43 a0             	mov    -0x60(%ebx),%eax
80104367:	83 f8 02             	cmp    $0x2,%eax
8010436a:	75 dc                	jne    80104348 <cps+0x38>
      cprintf("%s\t%d\tSLEEP\t  %d\t\t%d\t  %d\t   %d\n", p->name, p->pid, p->priority,p->deadline,p->sz,p->tickcounts);
8010436c:	83 ec 04             	sub    $0x4,%esp
8010436f:	ff 73 28             	pushl  0x28(%ebx)
80104372:	ff 73 94             	pushl  -0x6c(%ebx)
80104375:	ff 73 20             	pushl  0x20(%ebx)
80104378:	ff 73 10             	pushl  0x10(%ebx)
8010437b:	ff 73 a4             	pushl  -0x5c(%ebx)
8010437e:	53                   	push   %ebx
8010437f:	81 c3 98 00 00 00    	add    $0x98,%ebx
80104385:	68 40 7b 10 80       	push   $0x80107b40
8010438a:	e8 d1 c2 ff ff       	call   80100660 <cprintf>
8010438f:	83 c4 20             	add    $0x20,%esp

  sti();
  acquire(&ptable.lock);
  cprintf("*********************************\n");
  cprintf("name\tpid\tstate\t  priority\tDeadline  Memory   ProcessTime\n");
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
80104392:	81 fb c0 53 11 80    	cmp    $0x801153c0,%ebx
80104398:	75 ca                	jne    80104364 <cps+0x54>
    else if ( p->state == RUNNING )
      cprintf("%s\t%d\tRUN\t  %d\t\t%d\t  %d\t   %d\n", p->name, p->pid, p->priority,p->deadline,p->sz,p->tickcounts);
    else if ( p->state == RUNNABLE)
      cprintf("%s\t%d\tRUNNABLE  %d\t\t%d\t  %d\t   %d\n", p->name, p->pid, p->priority,p->deadline,p->sz,p->tickcounts);
  }
  release(&ptable.lock);
8010439a:	83 ec 0c             	sub    $0xc,%esp
8010439d:	68 20 2d 11 80       	push   $0x80112d20
801043a2:	e8 e9 04 00 00       	call   80104890 <release>
  cprintf("*********************************\n");
801043a7:	c7 04 24 e0 7a 10 80 	movl   $0x80107ae0,(%esp)
801043ae:	e8 ad c2 ff ff       	call   80100660 <cprintf>
  return 22;
}
801043b3:	b8 16 00 00 00       	mov    $0x16,%eax
801043b8:	8b 5d fc             	mov    -0x4(%ebp),%ebx
801043bb:	c9                   	leave  
801043bc:	c3                   	ret    
801043bd:	8d 76 00             	lea    0x0(%esi),%esi
  cprintf("name\tpid\tstate\t  priority\tDeadline  Memory   ProcessTime\n");
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == SLEEPING)
      cprintf("%s\t%d\tSLEEP\t  %d\t\t%d\t  %d\t   %d\n", p->name, p->pid, p->priority,p->deadline,p->sz,p->tickcounts);
    else if ( p->state == RUNNING )
      cprintf("%s\t%d\tRUN\t  %d\t\t%d\t  %d\t   %d\n", p->name, p->pid, p->priority,p->deadline,p->sz,p->tickcounts);
801043c0:	83 ec 04             	sub    $0x4,%esp
801043c3:	ff 73 28             	pushl  0x28(%ebx)
801043c6:	ff 73 94             	pushl  -0x6c(%ebx)
801043c9:	ff 73 20             	pushl  0x20(%ebx)
801043cc:	ff 73 10             	pushl  0x10(%ebx)
801043cf:	ff 73 a4             	pushl  -0x5c(%ebx)
801043d2:	53                   	push   %ebx
801043d3:	68 64 7b 10 80       	push   $0x80107b64
801043d8:	e8 83 c2 ff ff       	call   80100660 <cprintf>
801043dd:	83 c4 20             	add    $0x20,%esp
801043e0:	e9 71 ff ff ff       	jmp    80104356 <cps+0x46>
801043e5:	8d 76 00             	lea    0x0(%esi),%esi
    else if ( p->state == RUNNABLE)
      cprintf("%s\t%d\tRUNNABLE  %d\t\t%d\t  %d\t   %d\n", p->name, p->pid, p->priority,p->deadline,p->sz,p->tickcounts);
801043e8:	83 ec 04             	sub    $0x4,%esp
801043eb:	ff 73 28             	pushl  0x28(%ebx)
801043ee:	ff 73 94             	pushl  -0x6c(%ebx)
801043f1:	ff 73 20             	pushl  0x20(%ebx)
801043f4:	ff 73 10             	pushl  0x10(%ebx)
801043f7:	ff 73 a4             	pushl  -0x5c(%ebx)
801043fa:	53                   	push   %ebx
801043fb:	68 84 7b 10 80       	push   $0x80107b84
80104400:	e8 5b c2 ff ff       	call   80100660 <cprintf>
80104405:	83 c4 20             	add    $0x20,%esp
80104408:	e9 49 ff ff ff       	jmp    80104356 <cps+0x46>
8010440d:	8d 76 00             	lea    0x0(%esi),%esi

80104410 <chpr>:
  return 22;
}

int
chpr (int pid, int priority)
{
80104410:	55                   	push   %ebp
80104411:	89 e5                	mov    %esp,%ebp
80104413:	53                   	push   %ebx
80104414:	83 ec 10             	sub    $0x10,%esp
80104417:	8b 5d 08             	mov    0x8(%ebp),%ebx
  struct proc *p;

  acquire(&ptable.lock);
8010441a:	68 20 2d 11 80       	push   $0x80112d20
8010441f:	e8 4c 03 00 00       	call   80104770 <acquire>
80104424:	83 c4 10             	add    $0x10,%esp
  for(p =ptable.proc; p< &ptable.proc[NPROC];p++){
80104427:	ba 54 2d 11 80       	mov    $0x80112d54,%edx
8010442c:	eb 10                	jmp    8010443e <chpr+0x2e>
8010442e:	66 90                	xchg   %ax,%ax
80104430:	81 c2 98 00 00 00    	add    $0x98,%edx
80104436:	81 fa 54 53 11 80    	cmp    $0x80115354,%edx
8010443c:	74 0e                	je     8010444c <chpr+0x3c>
    if(p->pid == pid){
8010443e:	39 5a 10             	cmp    %ebx,0x10(%edx)
80104441:	75 ed                	jne    80104430 <chpr+0x20>
      p->lowestPriority = priority;
80104443:	8b 45 0c             	mov    0xc(%ebp),%eax
80104446:	89 82 80 00 00 00    	mov    %eax,0x80(%edx)
      break;
    }
  }
  release(&ptable.lock);
8010444c:	83 ec 0c             	sub    $0xc,%esp
8010444f:	68 20 2d 11 80       	push   $0x80112d20
80104454:	e8 37 04 00 00       	call   80104890 <release>

  return pid;
}
80104459:	89 d8                	mov    %ebx,%eax
8010445b:	8b 5d fc             	mov    -0x4(%ebp),%ebx
8010445e:	c9                   	leave  
8010445f:	c3                   	ret    

80104460 <chdl>:

int
chdl (int pid, int deadline)
{
80104460:	55                   	push   %ebp
80104461:	89 e5                	mov    %esp,%ebp
80104463:	57                   	push   %edi
80104464:	56                   	push   %esi
80104465:	53                   	push   %ebx
80104466:	83 ec 18             	sub    $0x18,%esp
80104469:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  struct proc *p;

  acquire(&ptable.lock);
8010446c:	68 20 2d 11 80       	push   $0x80112d20
80104471:	e8 fa 02 00 00       	call   80104770 <acquire>
80104476:	83 c4 10             	add    $0x10,%esp
  for(p =ptable.proc; p< &ptable.proc[NPROC];p++){
80104479:	b8 54 2d 11 80       	mov    $0x80112d54,%eax
8010447e:	8b 55 08             	mov    0x8(%ebp),%edx
80104481:	eb 0c                	jmp    8010448f <chdl+0x2f>
80104483:	05 98 00 00 00       	add    $0x98,%eax
80104488:	3d 54 53 11 80       	cmp    $0x80115354,%eax
8010448d:	74 11                	je     801044a0 <chdl+0x40>
    if(p->pid == pid){
8010448f:	39 50 10             	cmp    %edx,0x10(%eax)
80104492:	75 ef                	jne    80104483 <chdl+0x23>
	p->deadline = deadline;
80104494:	89 98 8c 00 00 00    	mov    %ebx,0x8c(%eax)
	p->runtimeDeadline = deadline;
8010449a:	89 98 90 00 00 00    	mov    %ebx,0x90(%eax)
      break;
    }
  }
  
  int priorityNow = priorityDefault;
801044a0:	8b 35 04 a0 10 80    	mov    0x8010a004,%esi
  int maxDeadline=0;
  int tempDeadline=-1;
  struct proc *a;
  for(a =ptable.proc; a< &ptable.proc[NPROC];a++){
801044a6:	bf 54 2d 11 80       	mov    $0x80112d54,%edi
    }
  }
  
  int priorityNow = priorityDefault;
  int maxDeadline=0;
  int tempDeadline=-1;
801044ab:	b9 ff ff ff ff       	mov    $0xffffffff,%ecx
801044b0:	eb 14                	jmp    801044c6 <chdl+0x66>
801044b2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  struct proc *a;
  for(a =ptable.proc; a< &ptable.proc[NPROC];a++){
801044b8:	81 c7 98 00 00 00    	add    $0x98,%edi
801044be:	81 ff 54 53 11 80    	cmp    $0x80115354,%edi
801044c4:	74 78                	je     8010453e <chdl+0xde>
	if(a->state != SLEEPING&&a->state != RUNNING&&a->state != RUNNABLE)
801044c6:	8b 47 0c             	mov    0xc(%edi),%eax
801044c9:	83 e8 02             	sub    $0x2,%eax
801044cc:	83 f8 02             	cmp    $0x2,%eax
801044cf:	77 e7                	ja     801044b8 <chdl+0x58>
801044d1:	31 db                	xor    %ebx,%ebx
801044d3:	b8 54 2d 11 80       	mov    $0x80112d54,%eax
801044d8:	eb 12                	jmp    801044ec <chdl+0x8c>
801044da:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
		continue;
	for(p =ptable.proc; p< &ptable.proc[NPROC];p++){
801044e0:	05 98 00 00 00       	add    $0x98,%eax
801044e5:	3d 54 53 11 80       	cmp    $0x80115354,%eax
801044ea:	74 3f                	je     8010452b <chdl+0xcb>
		if(p->state != SLEEPING&&p->state != RUNNING&&p->state != RUNNABLE)
801044ec:	8b 50 0c             	mov    0xc(%eax),%edx
801044ef:	83 ea 02             	sub    $0x2,%edx
801044f2:	83 fa 02             	cmp    $0x2,%edx
801044f5:	77 e9                	ja     801044e0 <chdl+0x80>
			continue;
	if(tempDeadline==-1||tempDeadline > p->runtimeDeadline)
801044f7:	83 f9 ff             	cmp    $0xffffffff,%ecx
801044fa:	74 08                	je     80104504 <chdl+0xa4>
801044fc:	39 88 90 00 00 00    	cmp    %ecx,0x90(%eax)
80104502:	7d dc                	jge    801044e0 <chdl+0x80>
	{
	      p->lowestPriority = priorityNow;
	      p->priority = priorityNow;
	      if(maxDeadline==0||maxDeadline < p->runtimeDeadline)
80104504:	85 db                	test   %ebx,%ebx
	for(p =ptable.proc; p< &ptable.proc[NPROC];p++){
		if(p->state != SLEEPING&&p->state != RUNNING&&p->state != RUNNABLE)
			continue;
	if(tempDeadline==-1||tempDeadline > p->runtimeDeadline)
	{
	      p->lowestPriority = priorityNow;
80104506:	89 b0 80 00 00 00    	mov    %esi,0x80(%eax)
	      p->priority = priorityNow;
8010450c:	89 70 7c             	mov    %esi,0x7c(%eax)
	      if(maxDeadline==0||maxDeadline < p->runtimeDeadline)
8010450f:	74 08                	je     80104519 <chdl+0xb9>
80104511:	39 98 90 00 00 00    	cmp    %ebx,0x90(%eax)
80104517:	7e c7                	jle    801044e0 <chdl+0x80>
	         maxDeadline = p->deadline;
80104519:	8b 98 8c 00 00 00    	mov    0x8c(%eax),%ebx
  int tempDeadline=-1;
  struct proc *a;
  for(a =ptable.proc; a< &ptable.proc[NPROC];a++){
	if(a->state != SLEEPING&&a->state != RUNNING&&a->state != RUNNABLE)
		continue;
	for(p =ptable.proc; p< &ptable.proc[NPROC];p++){
8010451f:	05 98 00 00 00       	add    $0x98,%eax
80104524:	3d 54 53 11 80       	cmp    $0x80115354,%eax
80104529:	75 c1                	jne    801044ec <chdl+0x8c>
  
  int priorityNow = priorityDefault;
  int maxDeadline=0;
  int tempDeadline=-1;
  struct proc *a;
  for(a =ptable.proc; a< &ptable.proc[NPROC];a++){
8010452b:	81 c7 98 00 00 00    	add    $0x98,%edi
	         maxDeadline = p->deadline;
	}
      }
	tempDeadline=maxDeadline;
	maxDeadline=0;
	priorityNow = priorityNow+1;
80104531:	83 c6 01             	add    $0x1,%esi
80104534:	89 d9                	mov    %ebx,%ecx
  
  int priorityNow = priorityDefault;
  int maxDeadline=0;
  int tempDeadline=-1;
  struct proc *a;
  for(a =ptable.proc; a< &ptable.proc[NPROC];a++){
80104536:	81 ff 54 53 11 80    	cmp    $0x80115354,%edi
8010453c:	75 88                	jne    801044c6 <chdl+0x66>
	tempDeadline=maxDeadline;
	maxDeadline=0;
	priorityNow = priorityNow+1;
  }

  release(&ptable.lock);
8010453e:	83 ec 0c             	sub    $0xc,%esp
80104541:	68 20 2d 11 80       	push   $0x80112d20
80104546:	e8 45 03 00 00       	call   80104890 <release>

  return pid;
}
8010454b:	8b 45 08             	mov    0x8(%ebp),%eax
8010454e:	8d 65 f4             	lea    -0xc(%ebp),%esp
80104551:	5b                   	pop    %ebx
80104552:	5e                   	pop    %esi
80104553:	5f                   	pop    %edi
80104554:	5d                   	pop    %ebp
80104555:	c3                   	ret    
80104556:	66 90                	xchg   %ax,%ax
80104558:	66 90                	xchg   %ax,%ax
8010455a:	66 90                	xchg   %ax,%ax
8010455c:	66 90                	xchg   %ax,%ax
8010455e:	66 90                	xchg   %ax,%ax

80104560 <initsleeplock>:
80104560:	55                   	push   %ebp
80104561:	89 e5                	mov    %esp,%ebp
80104563:	53                   	push   %ebx
80104564:	83 ec 0c             	sub    $0xc,%esp
80104567:	8b 5d 08             	mov    0x8(%ebp),%ebx
8010456a:	68 c0 7b 10 80       	push   $0x80107bc0
8010456f:	8d 43 04             	lea    0x4(%ebx),%eax
80104572:	50                   	push   %eax
80104573:	e8 f8 00 00 00       	call   80104670 <initlock>
80104578:	8b 45 0c             	mov    0xc(%ebp),%eax
8010457b:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)
80104581:	83 c4 10             	add    $0x10,%esp
80104584:	c7 43 3c 00 00 00 00 	movl   $0x0,0x3c(%ebx)
8010458b:	89 43 38             	mov    %eax,0x38(%ebx)
8010458e:	8b 5d fc             	mov    -0x4(%ebp),%ebx
80104591:	c9                   	leave  
80104592:	c3                   	ret    
80104593:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80104599:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801045a0 <acquiresleep>:
801045a0:	55                   	push   %ebp
801045a1:	89 e5                	mov    %esp,%ebp
801045a3:	56                   	push   %esi
801045a4:	53                   	push   %ebx
801045a5:	8b 5d 08             	mov    0x8(%ebp),%ebx
801045a8:	83 ec 0c             	sub    $0xc,%esp
801045ab:	8d 73 04             	lea    0x4(%ebx),%esi
801045ae:	56                   	push   %esi
801045af:	e8 bc 01 00 00       	call   80104770 <acquire>
801045b4:	8b 13                	mov    (%ebx),%edx
801045b6:	83 c4 10             	add    $0x10,%esp
801045b9:	85 d2                	test   %edx,%edx
801045bb:	74 16                	je     801045d3 <acquiresleep+0x33>
801045bd:	8d 76 00             	lea    0x0(%esi),%esi
801045c0:	83 ec 08             	sub    $0x8,%esp
801045c3:	56                   	push   %esi
801045c4:	53                   	push   %ebx
801045c5:	e8 d6 f9 ff ff       	call   80103fa0 <sleep>
801045ca:	8b 03                	mov    (%ebx),%eax
801045cc:	83 c4 10             	add    $0x10,%esp
801045cf:	85 c0                	test   %eax,%eax
801045d1:	75 ed                	jne    801045c0 <acquiresleep+0x20>
801045d3:	c7 03 01 00 00 00    	movl   $0x1,(%ebx)
801045d9:	e8 92 f2 ff ff       	call   80103870 <myproc>
801045de:	8b 40 10             	mov    0x10(%eax),%eax
801045e1:	89 43 3c             	mov    %eax,0x3c(%ebx)
801045e4:	89 75 08             	mov    %esi,0x8(%ebp)
801045e7:	8d 65 f8             	lea    -0x8(%ebp),%esp
801045ea:	5b                   	pop    %ebx
801045eb:	5e                   	pop    %esi
801045ec:	5d                   	pop    %ebp
801045ed:	e9 9e 02 00 00       	jmp    80104890 <release>
801045f2:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801045f9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80104600 <releasesleep>:
80104600:	55                   	push   %ebp
80104601:	89 e5                	mov    %esp,%ebp
80104603:	56                   	push   %esi
80104604:	53                   	push   %ebx
80104605:	8b 5d 08             	mov    0x8(%ebp),%ebx
80104608:	83 ec 0c             	sub    $0xc,%esp
8010460b:	8d 73 04             	lea    0x4(%ebx),%esi
8010460e:	56                   	push   %esi
8010460f:	e8 5c 01 00 00       	call   80104770 <acquire>
80104614:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)
8010461a:	c7 43 3c 00 00 00 00 	movl   $0x0,0x3c(%ebx)
80104621:	89 1c 24             	mov    %ebx,(%esp)
80104624:	e8 37 fb ff ff       	call   80104160 <wakeup>
80104629:	89 75 08             	mov    %esi,0x8(%ebp)
8010462c:	83 c4 10             	add    $0x10,%esp
8010462f:	8d 65 f8             	lea    -0x8(%ebp),%esp
80104632:	5b                   	pop    %ebx
80104633:	5e                   	pop    %esi
80104634:	5d                   	pop    %ebp
80104635:	e9 56 02 00 00       	jmp    80104890 <release>
8010463a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

80104640 <holdingsleep>:
80104640:	55                   	push   %ebp
80104641:	89 e5                	mov    %esp,%ebp
80104643:	56                   	push   %esi
80104644:	53                   	push   %ebx
80104645:	8b 75 08             	mov    0x8(%ebp),%esi
80104648:	83 ec 0c             	sub    $0xc,%esp
8010464b:	8d 5e 04             	lea    0x4(%esi),%ebx
8010464e:	53                   	push   %ebx
8010464f:	e8 1c 01 00 00       	call   80104770 <acquire>
80104654:	8b 36                	mov    (%esi),%esi
80104656:	89 1c 24             	mov    %ebx,(%esp)
80104659:	e8 32 02 00 00       	call   80104890 <release>
8010465e:	8d 65 f8             	lea    -0x8(%ebp),%esp
80104661:	89 f0                	mov    %esi,%eax
80104663:	5b                   	pop    %ebx
80104664:	5e                   	pop    %esi
80104665:	5d                   	pop    %ebp
80104666:	c3                   	ret    
80104667:	66 90                	xchg   %ax,%ax
80104669:	66 90                	xchg   %ax,%ax
8010466b:	66 90                	xchg   %ax,%ax
8010466d:	66 90                	xchg   %ax,%ax
8010466f:	90                   	nop

80104670 <initlock>:
80104670:	55                   	push   %ebp
80104671:	89 e5                	mov    %esp,%ebp
80104673:	8b 45 08             	mov    0x8(%ebp),%eax
80104676:	8b 55 0c             	mov    0xc(%ebp),%edx
80104679:	c7 00 00 00 00 00    	movl   $0x0,(%eax)
8010467f:	89 50 04             	mov    %edx,0x4(%eax)
80104682:	c7 40 08 00 00 00 00 	movl   $0x0,0x8(%eax)
80104689:	5d                   	pop    %ebp
8010468a:	c3                   	ret    
8010468b:	90                   	nop
8010468c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80104690 <getcallerpcs>:
80104690:	55                   	push   %ebp
80104691:	89 e5                	mov    %esp,%ebp
80104693:	53                   	push   %ebx
80104694:	8b 45 08             	mov    0x8(%ebp),%eax
80104697:	8b 4d 0c             	mov    0xc(%ebp),%ecx
8010469a:	8d 50 f8             	lea    -0x8(%eax),%edx
8010469d:	31 c0                	xor    %eax,%eax
8010469f:	90                   	nop
801046a0:	8d 9a 00 00 00 80    	lea    -0x80000000(%edx),%ebx
801046a6:	81 fb fe ff ff 7f    	cmp    $0x7ffffffe,%ebx
801046ac:	77 1a                	ja     801046c8 <getcallerpcs+0x38>
801046ae:	8b 5a 04             	mov    0x4(%edx),%ebx
801046b1:	89 1c 81             	mov    %ebx,(%ecx,%eax,4)
801046b4:	83 c0 01             	add    $0x1,%eax
801046b7:	8b 12                	mov    (%edx),%edx
801046b9:	83 f8 0a             	cmp    $0xa,%eax
801046bc:	75 e2                	jne    801046a0 <getcallerpcs+0x10>
801046be:	5b                   	pop    %ebx
801046bf:	5d                   	pop    %ebp
801046c0:	c3                   	ret    
801046c1:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801046c8:	c7 04 81 00 00 00 00 	movl   $0x0,(%ecx,%eax,4)
801046cf:	83 c0 01             	add    $0x1,%eax
801046d2:	83 f8 0a             	cmp    $0xa,%eax
801046d5:	74 e7                	je     801046be <getcallerpcs+0x2e>
801046d7:	c7 04 81 00 00 00 00 	movl   $0x0,(%ecx,%eax,4)
801046de:	83 c0 01             	add    $0x1,%eax
801046e1:	83 f8 0a             	cmp    $0xa,%eax
801046e4:	75 e2                	jne    801046c8 <getcallerpcs+0x38>
801046e6:	eb d6                	jmp    801046be <getcallerpcs+0x2e>
801046e8:	90                   	nop
801046e9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

801046f0 <holding>:
801046f0:	55                   	push   %ebp
801046f1:	89 e5                	mov    %esp,%ebp
801046f3:	53                   	push   %ebx
801046f4:	83 ec 04             	sub    $0x4,%esp
801046f7:	8b 55 08             	mov    0x8(%ebp),%edx
801046fa:	8b 02                	mov    (%edx),%eax
801046fc:	85 c0                	test   %eax,%eax
801046fe:	75 10                	jne    80104710 <holding+0x20>
80104700:	83 c4 04             	add    $0x4,%esp
80104703:	31 c0                	xor    %eax,%eax
80104705:	5b                   	pop    %ebx
80104706:	5d                   	pop    %ebp
80104707:	c3                   	ret    
80104708:	90                   	nop
80104709:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80104710:	8b 5a 08             	mov    0x8(%edx),%ebx
80104713:	e8 b8 f0 ff ff       	call   801037d0 <mycpu>
80104718:	39 c3                	cmp    %eax,%ebx
8010471a:	0f 94 c0             	sete   %al
8010471d:	83 c4 04             	add    $0x4,%esp
80104720:	0f b6 c0             	movzbl %al,%eax
80104723:	5b                   	pop    %ebx
80104724:	5d                   	pop    %ebp
80104725:	c3                   	ret    
80104726:	8d 76 00             	lea    0x0(%esi),%esi
80104729:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80104730 <pushcli>:
80104730:	55                   	push   %ebp
80104731:	89 e5                	mov    %esp,%ebp
80104733:	53                   	push   %ebx
80104734:	83 ec 04             	sub    $0x4,%esp
80104737:	9c                   	pushf  
80104738:	5b                   	pop    %ebx
80104739:	fa                   	cli    
8010473a:	e8 91 f0 ff ff       	call   801037d0 <mycpu>
8010473f:	8b 80 a4 00 00 00    	mov    0xa4(%eax),%eax
80104745:	85 c0                	test   %eax,%eax
80104747:	75 11                	jne    8010475a <pushcli+0x2a>
80104749:	81 e3 00 02 00 00    	and    $0x200,%ebx
8010474f:	e8 7c f0 ff ff       	call   801037d0 <mycpu>
80104754:	89 98 a8 00 00 00    	mov    %ebx,0xa8(%eax)
8010475a:	e8 71 f0 ff ff       	call   801037d0 <mycpu>
8010475f:	83 80 a4 00 00 00 01 	addl   $0x1,0xa4(%eax)
80104766:	83 c4 04             	add    $0x4,%esp
80104769:	5b                   	pop    %ebx
8010476a:	5d                   	pop    %ebp
8010476b:	c3                   	ret    
8010476c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80104770 <acquire>:
80104770:	55                   	push   %ebp
80104771:	89 e5                	mov    %esp,%ebp
80104773:	56                   	push   %esi
80104774:	53                   	push   %ebx
80104775:	e8 b6 ff ff ff       	call   80104730 <pushcli>
8010477a:	8b 5d 08             	mov    0x8(%ebp),%ebx
8010477d:	8b 03                	mov    (%ebx),%eax
8010477f:	85 c0                	test   %eax,%eax
80104781:	75 7d                	jne    80104800 <acquire+0x90>
80104783:	ba 01 00 00 00       	mov    $0x1,%edx
80104788:	eb 09                	jmp    80104793 <acquire+0x23>
8010478a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80104790:	8b 5d 08             	mov    0x8(%ebp),%ebx
80104793:	89 d0                	mov    %edx,%eax
80104795:	f0 87 03             	lock xchg %eax,(%ebx)
80104798:	85 c0                	test   %eax,%eax
8010479a:	75 f4                	jne    80104790 <acquire+0x20>
8010479c:	f0 83 0c 24 00       	lock orl $0x0,(%esp)
801047a1:	8b 5d 08             	mov    0x8(%ebp),%ebx
801047a4:	e8 27 f0 ff ff       	call   801037d0 <mycpu>
801047a9:	89 ea                	mov    %ebp,%edx
801047ab:	8d 4b 0c             	lea    0xc(%ebx),%ecx
801047ae:	89 43 08             	mov    %eax,0x8(%ebx)
801047b1:	31 c0                	xor    %eax,%eax
801047b3:	90                   	nop
801047b4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
801047b8:	8d 9a 00 00 00 80    	lea    -0x80000000(%edx),%ebx
801047be:	81 fb fe ff ff 7f    	cmp    $0x7ffffffe,%ebx
801047c4:	77 1a                	ja     801047e0 <acquire+0x70>
801047c6:	8b 5a 04             	mov    0x4(%edx),%ebx
801047c9:	89 1c 81             	mov    %ebx,(%ecx,%eax,4)
801047cc:	83 c0 01             	add    $0x1,%eax
801047cf:	8b 12                	mov    (%edx),%edx
801047d1:	83 f8 0a             	cmp    $0xa,%eax
801047d4:	75 e2                	jne    801047b8 <acquire+0x48>
801047d6:	8d 65 f8             	lea    -0x8(%ebp),%esp
801047d9:	5b                   	pop    %ebx
801047da:	5e                   	pop    %esi
801047db:	5d                   	pop    %ebp
801047dc:	c3                   	ret    
801047dd:	8d 76 00             	lea    0x0(%esi),%esi
801047e0:	c7 04 81 00 00 00 00 	movl   $0x0,(%ecx,%eax,4)
801047e7:	83 c0 01             	add    $0x1,%eax
801047ea:	83 f8 0a             	cmp    $0xa,%eax
801047ed:	74 e7                	je     801047d6 <acquire+0x66>
801047ef:	c7 04 81 00 00 00 00 	movl   $0x0,(%ecx,%eax,4)
801047f6:	83 c0 01             	add    $0x1,%eax
801047f9:	83 f8 0a             	cmp    $0xa,%eax
801047fc:	75 e2                	jne    801047e0 <acquire+0x70>
801047fe:	eb d6                	jmp    801047d6 <acquire+0x66>
80104800:	8b 73 08             	mov    0x8(%ebx),%esi
80104803:	e8 c8 ef ff ff       	call   801037d0 <mycpu>
80104808:	39 c6                	cmp    %eax,%esi
8010480a:	0f 85 73 ff ff ff    	jne    80104783 <acquire+0x13>
80104810:	83 ec 0c             	sub    $0xc,%esp
80104813:	68 cb 7b 10 80       	push   $0x80107bcb
80104818:	e8 53 bb ff ff       	call   80100370 <panic>
8010481d:	8d 76 00             	lea    0x0(%esi),%esi

80104820 <popcli>:
80104820:	55                   	push   %ebp
80104821:	89 e5                	mov    %esp,%ebp
80104823:	83 ec 08             	sub    $0x8,%esp
80104826:	9c                   	pushf  
80104827:	58                   	pop    %eax
80104828:	f6 c4 02             	test   $0x2,%ah
8010482b:	75 52                	jne    8010487f <popcli+0x5f>
8010482d:	e8 9e ef ff ff       	call   801037d0 <mycpu>
80104832:	8b 88 a4 00 00 00    	mov    0xa4(%eax),%ecx
80104838:	8d 51 ff             	lea    -0x1(%ecx),%edx
8010483b:	85 d2                	test   %edx,%edx
8010483d:	89 90 a4 00 00 00    	mov    %edx,0xa4(%eax)
80104843:	78 2d                	js     80104872 <popcli+0x52>
80104845:	e8 86 ef ff ff       	call   801037d0 <mycpu>
8010484a:	8b 90 a4 00 00 00    	mov    0xa4(%eax),%edx
80104850:	85 d2                	test   %edx,%edx
80104852:	74 0c                	je     80104860 <popcli+0x40>
80104854:	c9                   	leave  
80104855:	c3                   	ret    
80104856:	8d 76 00             	lea    0x0(%esi),%esi
80104859:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80104860:	e8 6b ef ff ff       	call   801037d0 <mycpu>
80104865:	8b 80 a8 00 00 00    	mov    0xa8(%eax),%eax
8010486b:	85 c0                	test   %eax,%eax
8010486d:	74 e5                	je     80104854 <popcli+0x34>
8010486f:	fb                   	sti    
80104870:	c9                   	leave  
80104871:	c3                   	ret    
80104872:	83 ec 0c             	sub    $0xc,%esp
80104875:	68 ea 7b 10 80       	push   $0x80107bea
8010487a:	e8 f1 ba ff ff       	call   80100370 <panic>
8010487f:	83 ec 0c             	sub    $0xc,%esp
80104882:	68 d3 7b 10 80       	push   $0x80107bd3
80104887:	e8 e4 ba ff ff       	call   80100370 <panic>
8010488c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80104890 <release>:
80104890:	55                   	push   %ebp
80104891:	89 e5                	mov    %esp,%ebp
80104893:	56                   	push   %esi
80104894:	53                   	push   %ebx
80104895:	8b 5d 08             	mov    0x8(%ebp),%ebx
80104898:	8b 03                	mov    (%ebx),%eax
8010489a:	85 c0                	test   %eax,%eax
8010489c:	75 12                	jne    801048b0 <release+0x20>
8010489e:	83 ec 0c             	sub    $0xc,%esp
801048a1:	68 f1 7b 10 80       	push   $0x80107bf1
801048a6:	e8 c5 ba ff ff       	call   80100370 <panic>
801048ab:	90                   	nop
801048ac:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
801048b0:	8b 73 08             	mov    0x8(%ebx),%esi
801048b3:	e8 18 ef ff ff       	call   801037d0 <mycpu>
801048b8:	39 c6                	cmp    %eax,%esi
801048ba:	75 e2                	jne    8010489e <release+0xe>
801048bc:	c7 43 0c 00 00 00 00 	movl   $0x0,0xc(%ebx)
801048c3:	c7 43 08 00 00 00 00 	movl   $0x0,0x8(%ebx)
801048ca:	f0 83 0c 24 00       	lock orl $0x0,(%esp)
801048cf:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)
801048d5:	8d 65 f8             	lea    -0x8(%ebp),%esp
801048d8:	5b                   	pop    %ebx
801048d9:	5e                   	pop    %esi
801048da:	5d                   	pop    %ebp
801048db:	e9 40 ff ff ff       	jmp    80104820 <popcli>

801048e0 <memset>:
#include "types.h"
#include "x86.h"

void*
memset(void *dst, int c, uint n)
{
801048e0:	55                   	push   %ebp
801048e1:	89 e5                	mov    %esp,%ebp
801048e3:	57                   	push   %edi
801048e4:	53                   	push   %ebx
801048e5:	8b 55 08             	mov    0x8(%ebp),%edx
801048e8:	8b 4d 10             	mov    0x10(%ebp),%ecx
  if ((int)dst%4 == 0 && n%4 == 0){
801048eb:	f6 c2 03             	test   $0x3,%dl
801048ee:	75 05                	jne    801048f5 <memset+0x15>
801048f0:	f6 c1 03             	test   $0x3,%cl
801048f3:	74 13                	je     80104908 <memset+0x28>
}

static inline void
stosb(void *addr, int data, int cnt)
{
  asm volatile("cld; rep stosb" :
801048f5:	89 d7                	mov    %edx,%edi
801048f7:	8b 45 0c             	mov    0xc(%ebp),%eax
801048fa:	fc                   	cld    
801048fb:	f3 aa                	rep stos %al,%es:(%edi)
    c &= 0xFF;
    stosl(dst, (c<<24)|(c<<16)|(c<<8)|c, n/4);
  } else
    stosb(dst, c, n);
  return dst;
}
801048fd:	5b                   	pop    %ebx
801048fe:	89 d0                	mov    %edx,%eax
80104900:	5f                   	pop    %edi
80104901:	5d                   	pop    %ebp
80104902:	c3                   	ret    
80104903:	90                   	nop
80104904:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

void*
memset(void *dst, int c, uint n)
{
  if ((int)dst%4 == 0 && n%4 == 0){
    c &= 0xFF;
80104908:	0f b6 7d 0c          	movzbl 0xc(%ebp),%edi
}

static inline void
stosl(void *addr, int data, int cnt)
{
  asm volatile("cld; rep stosl" :
8010490c:	c1 e9 02             	shr    $0x2,%ecx
8010490f:	89 fb                	mov    %edi,%ebx
80104911:	89 f8                	mov    %edi,%eax
80104913:	c1 e3 18             	shl    $0x18,%ebx
80104916:	c1 e0 10             	shl    $0x10,%eax
80104919:	09 d8                	or     %ebx,%eax
8010491b:	09 f8                	or     %edi,%eax
8010491d:	c1 e7 08             	shl    $0x8,%edi
80104920:	09 f8                	or     %edi,%eax
80104922:	89 d7                	mov    %edx,%edi
80104924:	fc                   	cld    
80104925:	f3 ab                	rep stos %eax,%es:(%edi)
    stosl(dst, (c<<24)|(c<<16)|(c<<8)|c, n/4);
  } else
    stosb(dst, c, n);
  return dst;
}
80104927:	5b                   	pop    %ebx
80104928:	89 d0                	mov    %edx,%eax
8010492a:	5f                   	pop    %edi
8010492b:	5d                   	pop    %ebp
8010492c:	c3                   	ret    
8010492d:	8d 76 00             	lea    0x0(%esi),%esi

80104930 <memcmp>:

int
memcmp(const void *v1, const void *v2, uint n)
{
80104930:	55                   	push   %ebp
80104931:	89 e5                	mov    %esp,%ebp
80104933:	57                   	push   %edi
80104934:	56                   	push   %esi
80104935:	8b 45 10             	mov    0x10(%ebp),%eax
80104938:	53                   	push   %ebx
80104939:	8b 75 0c             	mov    0xc(%ebp),%esi
8010493c:	8b 5d 08             	mov    0x8(%ebp),%ebx
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  while(n-- > 0){
8010493f:	85 c0                	test   %eax,%eax
80104941:	74 29                	je     8010496c <memcmp+0x3c>
    if(*s1 != *s2)
80104943:	0f b6 13             	movzbl (%ebx),%edx
80104946:	0f b6 0e             	movzbl (%esi),%ecx
80104949:	38 d1                	cmp    %dl,%cl
8010494b:	75 2b                	jne    80104978 <memcmp+0x48>
8010494d:	8d 78 ff             	lea    -0x1(%eax),%edi
80104950:	31 c0                	xor    %eax,%eax
80104952:	eb 14                	jmp    80104968 <memcmp+0x38>
80104954:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80104958:	0f b6 54 03 01       	movzbl 0x1(%ebx,%eax,1),%edx
8010495d:	83 c0 01             	add    $0x1,%eax
80104960:	0f b6 0c 06          	movzbl (%esi,%eax,1),%ecx
80104964:	38 ca                	cmp    %cl,%dl
80104966:	75 10                	jne    80104978 <memcmp+0x48>
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  while(n-- > 0){
80104968:	39 f8                	cmp    %edi,%eax
8010496a:	75 ec                	jne    80104958 <memcmp+0x28>
      return *s1 - *s2;
    s1++, s2++;
  }

  return 0;
}
8010496c:	5b                   	pop    %ebx
    if(*s1 != *s2)
      return *s1 - *s2;
    s1++, s2++;
  }

  return 0;
8010496d:	31 c0                	xor    %eax,%eax
}
8010496f:	5e                   	pop    %esi
80104970:	5f                   	pop    %edi
80104971:	5d                   	pop    %ebp
80104972:	c3                   	ret    
80104973:	90                   	nop
80104974:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

  s1 = v1;
  s2 = v2;
  while(n-- > 0){
    if(*s1 != *s2)
      return *s1 - *s2;
80104978:	0f b6 c2             	movzbl %dl,%eax
    s1++, s2++;
  }

  return 0;
}
8010497b:	5b                   	pop    %ebx

  s1 = v1;
  s2 = v2;
  while(n-- > 0){
    if(*s1 != *s2)
      return *s1 - *s2;
8010497c:	29 c8                	sub    %ecx,%eax
    s1++, s2++;
  }

  return 0;
}
8010497e:	5e                   	pop    %esi
8010497f:	5f                   	pop    %edi
80104980:	5d                   	pop    %ebp
80104981:	c3                   	ret    
80104982:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80104989:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80104990 <memmove>:

void*
memmove(void *dst, const void *src, uint n)
{
80104990:	55                   	push   %ebp
80104991:	89 e5                	mov    %esp,%ebp
80104993:	56                   	push   %esi
80104994:	53                   	push   %ebx
80104995:	8b 45 08             	mov    0x8(%ebp),%eax
80104998:	8b 75 0c             	mov    0xc(%ebp),%esi
8010499b:	8b 5d 10             	mov    0x10(%ebp),%ebx
  const char *s;
  char *d;

  s = src;
  d = dst;
  if(s < d && s + n > d){
8010499e:	39 c6                	cmp    %eax,%esi
801049a0:	73 2e                	jae    801049d0 <memmove+0x40>
801049a2:	8d 0c 1e             	lea    (%esi,%ebx,1),%ecx
801049a5:	39 c8                	cmp    %ecx,%eax
801049a7:	73 27                	jae    801049d0 <memmove+0x40>
    s += n;
    d += n;
    while(n-- > 0)
801049a9:	85 db                	test   %ebx,%ebx
801049ab:	8d 53 ff             	lea    -0x1(%ebx),%edx
801049ae:	74 17                	je     801049c7 <memmove+0x37>
      *--d = *--s;
801049b0:	29 d9                	sub    %ebx,%ecx
801049b2:	89 cb                	mov    %ecx,%ebx
801049b4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
801049b8:	0f b6 0c 13          	movzbl (%ebx,%edx,1),%ecx
801049bc:	88 0c 10             	mov    %cl,(%eax,%edx,1)
  s = src;
  d = dst;
  if(s < d && s + n > d){
    s += n;
    d += n;
    while(n-- > 0)
801049bf:	83 ea 01             	sub    $0x1,%edx
801049c2:	83 fa ff             	cmp    $0xffffffff,%edx
801049c5:	75 f1                	jne    801049b8 <memmove+0x28>
  } else
    while(n-- > 0)
      *d++ = *s++;

  return dst;
}
801049c7:	5b                   	pop    %ebx
801049c8:	5e                   	pop    %esi
801049c9:	5d                   	pop    %ebp
801049ca:	c3                   	ret    
801049cb:	90                   	nop
801049cc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    s += n;
    d += n;
    while(n-- > 0)
      *--d = *--s;
  } else
    while(n-- > 0)
801049d0:	31 d2                	xor    %edx,%edx
801049d2:	85 db                	test   %ebx,%ebx
801049d4:	74 f1                	je     801049c7 <memmove+0x37>
801049d6:	8d 76 00             	lea    0x0(%esi),%esi
801049d9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
      *d++ = *s++;
801049e0:	0f b6 0c 16          	movzbl (%esi,%edx,1),%ecx
801049e4:	88 0c 10             	mov    %cl,(%eax,%edx,1)
801049e7:	83 c2 01             	add    $0x1,%edx
    s += n;
    d += n;
    while(n-- > 0)
      *--d = *--s;
  } else
    while(n-- > 0)
801049ea:	39 d3                	cmp    %edx,%ebx
801049ec:	75 f2                	jne    801049e0 <memmove+0x50>
      *d++ = *s++;

  return dst;
}
801049ee:	5b                   	pop    %ebx
801049ef:	5e                   	pop    %esi
801049f0:	5d                   	pop    %ebp
801049f1:	c3                   	ret    
801049f2:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801049f9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80104a00 <memcpy>:

// memcpy exists to placate GCC.  Use memmove.
void*
memcpy(void *dst, const void *src, uint n)
{
80104a00:	55                   	push   %ebp
80104a01:	89 e5                	mov    %esp,%ebp
  return memmove(dst, src, n);
}
80104a03:	5d                   	pop    %ebp

// memcpy exists to placate GCC.  Use memmove.
void*
memcpy(void *dst, const void *src, uint n)
{
  return memmove(dst, src, n);
80104a04:	eb 8a                	jmp    80104990 <memmove>
80104a06:	8d 76 00             	lea    0x0(%esi),%esi
80104a09:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80104a10 <strncmp>:
}

int
strncmp(const char *p, const char *q, uint n)
{
80104a10:	55                   	push   %ebp
80104a11:	89 e5                	mov    %esp,%ebp
80104a13:	57                   	push   %edi
80104a14:	56                   	push   %esi
80104a15:	8b 4d 10             	mov    0x10(%ebp),%ecx
80104a18:	53                   	push   %ebx
80104a19:	8b 7d 08             	mov    0x8(%ebp),%edi
80104a1c:	8b 75 0c             	mov    0xc(%ebp),%esi
  while(n > 0 && *p && *p == *q)
80104a1f:	85 c9                	test   %ecx,%ecx
80104a21:	74 37                	je     80104a5a <strncmp+0x4a>
80104a23:	0f b6 17             	movzbl (%edi),%edx
80104a26:	0f b6 1e             	movzbl (%esi),%ebx
80104a29:	84 d2                	test   %dl,%dl
80104a2b:	74 3f                	je     80104a6c <strncmp+0x5c>
80104a2d:	38 d3                	cmp    %dl,%bl
80104a2f:	75 3b                	jne    80104a6c <strncmp+0x5c>
80104a31:	8d 47 01             	lea    0x1(%edi),%eax
80104a34:	01 cf                	add    %ecx,%edi
80104a36:	eb 1b                	jmp    80104a53 <strncmp+0x43>
80104a38:	90                   	nop
80104a39:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80104a40:	0f b6 10             	movzbl (%eax),%edx
80104a43:	84 d2                	test   %dl,%dl
80104a45:	74 21                	je     80104a68 <strncmp+0x58>
80104a47:	0f b6 19             	movzbl (%ecx),%ebx
80104a4a:	83 c0 01             	add    $0x1,%eax
80104a4d:	89 ce                	mov    %ecx,%esi
80104a4f:	38 da                	cmp    %bl,%dl
80104a51:	75 19                	jne    80104a6c <strncmp+0x5c>
80104a53:	39 c7                	cmp    %eax,%edi
    n--, p++, q++;
80104a55:	8d 4e 01             	lea    0x1(%esi),%ecx
}

int
strncmp(const char *p, const char *q, uint n)
{
  while(n > 0 && *p && *p == *q)
80104a58:	75 e6                	jne    80104a40 <strncmp+0x30>
    n--, p++, q++;
  if(n == 0)
    return 0;
  return (uchar)*p - (uchar)*q;
}
80104a5a:	5b                   	pop    %ebx
strncmp(const char *p, const char *q, uint n)
{
  while(n > 0 && *p && *p == *q)
    n--, p++, q++;
  if(n == 0)
    return 0;
80104a5b:	31 c0                	xor    %eax,%eax
  return (uchar)*p - (uchar)*q;
}
80104a5d:	5e                   	pop    %esi
80104a5e:	5f                   	pop    %edi
80104a5f:	5d                   	pop    %ebp
80104a60:	c3                   	ret    
80104a61:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80104a68:	0f b6 5e 01          	movzbl 0x1(%esi),%ebx
{
  while(n > 0 && *p && *p == *q)
    n--, p++, q++;
  if(n == 0)
    return 0;
  return (uchar)*p - (uchar)*q;
80104a6c:	0f b6 c2             	movzbl %dl,%eax
80104a6f:	29 d8                	sub    %ebx,%eax
}
80104a71:	5b                   	pop    %ebx
80104a72:	5e                   	pop    %esi
80104a73:	5f                   	pop    %edi
80104a74:	5d                   	pop    %ebp
80104a75:	c3                   	ret    
80104a76:	8d 76 00             	lea    0x0(%esi),%esi
80104a79:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80104a80 <strncpy>:

char*
strncpy(char *s, const char *t, int n)
{
80104a80:	55                   	push   %ebp
80104a81:	89 e5                	mov    %esp,%ebp
80104a83:	56                   	push   %esi
80104a84:	53                   	push   %ebx
80104a85:	8b 45 08             	mov    0x8(%ebp),%eax
80104a88:	8b 5d 0c             	mov    0xc(%ebp),%ebx
80104a8b:	8b 4d 10             	mov    0x10(%ebp),%ecx
  char *os;

  os = s;
  while(n-- > 0 && (*s++ = *t++) != 0)
80104a8e:	89 c2                	mov    %eax,%edx
80104a90:	eb 19                	jmp    80104aab <strncpy+0x2b>
80104a92:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80104a98:	83 c3 01             	add    $0x1,%ebx
80104a9b:	0f b6 4b ff          	movzbl -0x1(%ebx),%ecx
80104a9f:	83 c2 01             	add    $0x1,%edx
80104aa2:	84 c9                	test   %cl,%cl
80104aa4:	88 4a ff             	mov    %cl,-0x1(%edx)
80104aa7:	74 09                	je     80104ab2 <strncpy+0x32>
80104aa9:	89 f1                	mov    %esi,%ecx
80104aab:	85 c9                	test   %ecx,%ecx
80104aad:	8d 71 ff             	lea    -0x1(%ecx),%esi
80104ab0:	7f e6                	jg     80104a98 <strncpy+0x18>
    ;
  while(n-- > 0)
80104ab2:	31 c9                	xor    %ecx,%ecx
80104ab4:	85 f6                	test   %esi,%esi
80104ab6:	7e 17                	jle    80104acf <strncpy+0x4f>
80104ab8:	90                   	nop
80104ab9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
    *s++ = 0;
80104ac0:	c6 04 0a 00          	movb   $0x0,(%edx,%ecx,1)
80104ac4:	89 f3                	mov    %esi,%ebx
80104ac6:	83 c1 01             	add    $0x1,%ecx
80104ac9:	29 cb                	sub    %ecx,%ebx
  char *os;

  os = s;
  while(n-- > 0 && (*s++ = *t++) != 0)
    ;
  while(n-- > 0)
80104acb:	85 db                	test   %ebx,%ebx
80104acd:	7f f1                	jg     80104ac0 <strncpy+0x40>
    *s++ = 0;
  return os;
}
80104acf:	5b                   	pop    %ebx
80104ad0:	5e                   	pop    %esi
80104ad1:	5d                   	pop    %ebp
80104ad2:	c3                   	ret    
80104ad3:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80104ad9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80104ae0 <safestrcpy>:

// Like strncpy but guaranteed to NUL-terminate.
char*
safestrcpy(char *s, const char *t, int n)
{
80104ae0:	55                   	push   %ebp
80104ae1:	89 e5                	mov    %esp,%ebp
80104ae3:	56                   	push   %esi
80104ae4:	53                   	push   %ebx
80104ae5:	8b 4d 10             	mov    0x10(%ebp),%ecx
80104ae8:	8b 45 08             	mov    0x8(%ebp),%eax
80104aeb:	8b 55 0c             	mov    0xc(%ebp),%edx
  char *os;

  os = s;
  if(n <= 0)
80104aee:	85 c9                	test   %ecx,%ecx
80104af0:	7e 26                	jle    80104b18 <safestrcpy+0x38>
80104af2:	8d 74 0a ff          	lea    -0x1(%edx,%ecx,1),%esi
80104af6:	89 c1                	mov    %eax,%ecx
80104af8:	eb 17                	jmp    80104b11 <safestrcpy+0x31>
80104afa:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    return os;
  while(--n > 0 && (*s++ = *t++) != 0)
80104b00:	83 c2 01             	add    $0x1,%edx
80104b03:	0f b6 5a ff          	movzbl -0x1(%edx),%ebx
80104b07:	83 c1 01             	add    $0x1,%ecx
80104b0a:	84 db                	test   %bl,%bl
80104b0c:	88 59 ff             	mov    %bl,-0x1(%ecx)
80104b0f:	74 04                	je     80104b15 <safestrcpy+0x35>
80104b11:	39 f2                	cmp    %esi,%edx
80104b13:	75 eb                	jne    80104b00 <safestrcpy+0x20>
    ;
  *s = 0;
80104b15:	c6 01 00             	movb   $0x0,(%ecx)
  return os;
}
80104b18:	5b                   	pop    %ebx
80104b19:	5e                   	pop    %esi
80104b1a:	5d                   	pop    %ebp
80104b1b:	c3                   	ret    
80104b1c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80104b20 <strlen>:

int
strlen(const char *s)
{
80104b20:	55                   	push   %ebp
  int n;

  for(n = 0; s[n]; n++)
80104b21:	31 c0                	xor    %eax,%eax
  return os;
}

int
strlen(const char *s)
{
80104b23:	89 e5                	mov    %esp,%ebp
80104b25:	8b 55 08             	mov    0x8(%ebp),%edx
  int n;

  for(n = 0; s[n]; n++)
80104b28:	80 3a 00             	cmpb   $0x0,(%edx)
80104b2b:	74 0c                	je     80104b39 <strlen+0x19>
80104b2d:	8d 76 00             	lea    0x0(%esi),%esi
80104b30:	83 c0 01             	add    $0x1,%eax
80104b33:	80 3c 02 00          	cmpb   $0x0,(%edx,%eax,1)
80104b37:	75 f7                	jne    80104b30 <strlen+0x10>
    ;
  return n;
}
80104b39:	5d                   	pop    %ebp
80104b3a:	c3                   	ret    

80104b3b <swtch>:
# Save current register context in old
# and then load register context from new.

.globl swtch
swtch:
  movl 4(%esp), %eax
80104b3b:	8b 44 24 04          	mov    0x4(%esp),%eax
  movl 8(%esp), %edx
80104b3f:	8b 54 24 08          	mov    0x8(%esp),%edx

  # Save old callee-save registers
  pushl %ebp
80104b43:	55                   	push   %ebp
  pushl %ebx
80104b44:	53                   	push   %ebx
  pushl %esi
80104b45:	56                   	push   %esi
  pushl %edi
80104b46:	57                   	push   %edi

  # Switch stacks
  movl %esp, (%eax)
80104b47:	89 20                	mov    %esp,(%eax)
  movl %edx, %esp
80104b49:	89 d4                	mov    %edx,%esp

  # Load new callee-save registers
  popl %edi
80104b4b:	5f                   	pop    %edi
  popl %esi
80104b4c:	5e                   	pop    %esi
  popl %ebx
80104b4d:	5b                   	pop    %ebx
  popl %ebp
80104b4e:	5d                   	pop    %ebp
  ret
80104b4f:	c3                   	ret    

80104b50 <fetchint>:
80104b50:	55                   	push   %ebp
80104b51:	89 e5                	mov    %esp,%ebp
80104b53:	53                   	push   %ebx
80104b54:	83 ec 04             	sub    $0x4,%esp
80104b57:	8b 5d 08             	mov    0x8(%ebp),%ebx
80104b5a:	e8 11 ed ff ff       	call   80103870 <myproc>
80104b5f:	8b 00                	mov    (%eax),%eax
80104b61:	39 d8                	cmp    %ebx,%eax
80104b63:	76 1b                	jbe    80104b80 <fetchint+0x30>
80104b65:	8d 53 04             	lea    0x4(%ebx),%edx
80104b68:	39 d0                	cmp    %edx,%eax
80104b6a:	72 14                	jb     80104b80 <fetchint+0x30>
80104b6c:	8b 45 0c             	mov    0xc(%ebp),%eax
80104b6f:	8b 13                	mov    (%ebx),%edx
80104b71:	89 10                	mov    %edx,(%eax)
80104b73:	31 c0                	xor    %eax,%eax
80104b75:	83 c4 04             	add    $0x4,%esp
80104b78:	5b                   	pop    %ebx
80104b79:	5d                   	pop    %ebp
80104b7a:	c3                   	ret    
80104b7b:	90                   	nop
80104b7c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80104b80:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80104b85:	eb ee                	jmp    80104b75 <fetchint+0x25>
80104b87:	89 f6                	mov    %esi,%esi
80104b89:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80104b90 <fetchstr>:
80104b90:	55                   	push   %ebp
80104b91:	89 e5                	mov    %esp,%ebp
80104b93:	53                   	push   %ebx
80104b94:	83 ec 04             	sub    $0x4,%esp
80104b97:	8b 5d 08             	mov    0x8(%ebp),%ebx
80104b9a:	e8 d1 ec ff ff       	call   80103870 <myproc>
80104b9f:	39 18                	cmp    %ebx,(%eax)
80104ba1:	76 29                	jbe    80104bcc <fetchstr+0x3c>
80104ba3:	8b 4d 0c             	mov    0xc(%ebp),%ecx
80104ba6:	89 da                	mov    %ebx,%edx
80104ba8:	89 19                	mov    %ebx,(%ecx)
80104baa:	8b 00                	mov    (%eax),%eax
80104bac:	39 c3                	cmp    %eax,%ebx
80104bae:	73 1c                	jae    80104bcc <fetchstr+0x3c>
80104bb0:	80 3b 00             	cmpb   $0x0,(%ebx)
80104bb3:	75 10                	jne    80104bc5 <fetchstr+0x35>
80104bb5:	eb 29                	jmp    80104be0 <fetchstr+0x50>
80104bb7:	89 f6                	mov    %esi,%esi
80104bb9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80104bc0:	80 3a 00             	cmpb   $0x0,(%edx)
80104bc3:	74 1b                	je     80104be0 <fetchstr+0x50>
80104bc5:	83 c2 01             	add    $0x1,%edx
80104bc8:	39 d0                	cmp    %edx,%eax
80104bca:	77 f4                	ja     80104bc0 <fetchstr+0x30>
80104bcc:	83 c4 04             	add    $0x4,%esp
80104bcf:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80104bd4:	5b                   	pop    %ebx
80104bd5:	5d                   	pop    %ebp
80104bd6:	c3                   	ret    
80104bd7:	89 f6                	mov    %esi,%esi
80104bd9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80104be0:	83 c4 04             	add    $0x4,%esp
80104be3:	89 d0                	mov    %edx,%eax
80104be5:	29 d8                	sub    %ebx,%eax
80104be7:	5b                   	pop    %ebx
80104be8:	5d                   	pop    %ebp
80104be9:	c3                   	ret    
80104bea:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

80104bf0 <argint>:
80104bf0:	55                   	push   %ebp
80104bf1:	89 e5                	mov    %esp,%ebp
80104bf3:	56                   	push   %esi
80104bf4:	53                   	push   %ebx
80104bf5:	e8 76 ec ff ff       	call   80103870 <myproc>
80104bfa:	8b 40 18             	mov    0x18(%eax),%eax
80104bfd:	8b 55 08             	mov    0x8(%ebp),%edx
80104c00:	8b 40 44             	mov    0x44(%eax),%eax
80104c03:	8d 1c 90             	lea    (%eax,%edx,4),%ebx
80104c06:	e8 65 ec ff ff       	call   80103870 <myproc>
80104c0b:	8b 00                	mov    (%eax),%eax
80104c0d:	8d 73 04             	lea    0x4(%ebx),%esi
80104c10:	39 c6                	cmp    %eax,%esi
80104c12:	73 1c                	jae    80104c30 <argint+0x40>
80104c14:	8d 53 08             	lea    0x8(%ebx),%edx
80104c17:	39 d0                	cmp    %edx,%eax
80104c19:	72 15                	jb     80104c30 <argint+0x40>
80104c1b:	8b 45 0c             	mov    0xc(%ebp),%eax
80104c1e:	8b 53 04             	mov    0x4(%ebx),%edx
80104c21:	89 10                	mov    %edx,(%eax)
80104c23:	31 c0                	xor    %eax,%eax
80104c25:	5b                   	pop    %ebx
80104c26:	5e                   	pop    %esi
80104c27:	5d                   	pop    %ebp
80104c28:	c3                   	ret    
80104c29:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80104c30:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80104c35:	eb ee                	jmp    80104c25 <argint+0x35>
80104c37:	89 f6                	mov    %esi,%esi
80104c39:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80104c40 <argptr>:
80104c40:	55                   	push   %ebp
80104c41:	89 e5                	mov    %esp,%ebp
80104c43:	56                   	push   %esi
80104c44:	53                   	push   %ebx
80104c45:	83 ec 10             	sub    $0x10,%esp
80104c48:	8b 5d 10             	mov    0x10(%ebp),%ebx
80104c4b:	e8 20 ec ff ff       	call   80103870 <myproc>
80104c50:	89 c6                	mov    %eax,%esi
80104c52:	8d 45 f4             	lea    -0xc(%ebp),%eax
80104c55:	83 ec 08             	sub    $0x8,%esp
80104c58:	50                   	push   %eax
80104c59:	ff 75 08             	pushl  0x8(%ebp)
80104c5c:	e8 8f ff ff ff       	call   80104bf0 <argint>
80104c61:	c1 e8 1f             	shr    $0x1f,%eax
80104c64:	83 c4 10             	add    $0x10,%esp
80104c67:	84 c0                	test   %al,%al
80104c69:	75 2d                	jne    80104c98 <argptr+0x58>
80104c6b:	89 d8                	mov    %ebx,%eax
80104c6d:	c1 e8 1f             	shr    $0x1f,%eax
80104c70:	84 c0                	test   %al,%al
80104c72:	75 24                	jne    80104c98 <argptr+0x58>
80104c74:	8b 16                	mov    (%esi),%edx
80104c76:	8b 45 f4             	mov    -0xc(%ebp),%eax
80104c79:	39 c2                	cmp    %eax,%edx
80104c7b:	76 1b                	jbe    80104c98 <argptr+0x58>
80104c7d:	01 c3                	add    %eax,%ebx
80104c7f:	39 da                	cmp    %ebx,%edx
80104c81:	72 15                	jb     80104c98 <argptr+0x58>
80104c83:	8b 55 0c             	mov    0xc(%ebp),%edx
80104c86:	89 02                	mov    %eax,(%edx)
80104c88:	31 c0                	xor    %eax,%eax
80104c8a:	8d 65 f8             	lea    -0x8(%ebp),%esp
80104c8d:	5b                   	pop    %ebx
80104c8e:	5e                   	pop    %esi
80104c8f:	5d                   	pop    %ebp
80104c90:	c3                   	ret    
80104c91:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80104c98:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80104c9d:	eb eb                	jmp    80104c8a <argptr+0x4a>
80104c9f:	90                   	nop

80104ca0 <argstr>:
80104ca0:	55                   	push   %ebp
80104ca1:	89 e5                	mov    %esp,%ebp
80104ca3:	83 ec 20             	sub    $0x20,%esp
80104ca6:	8d 45 f4             	lea    -0xc(%ebp),%eax
80104ca9:	50                   	push   %eax
80104caa:	ff 75 08             	pushl  0x8(%ebp)
80104cad:	e8 3e ff ff ff       	call   80104bf0 <argint>
80104cb2:	83 c4 10             	add    $0x10,%esp
80104cb5:	85 c0                	test   %eax,%eax
80104cb7:	78 17                	js     80104cd0 <argstr+0x30>
80104cb9:	83 ec 08             	sub    $0x8,%esp
80104cbc:	ff 75 0c             	pushl  0xc(%ebp)
80104cbf:	ff 75 f4             	pushl  -0xc(%ebp)
80104cc2:	e8 c9 fe ff ff       	call   80104b90 <fetchstr>
80104cc7:	83 c4 10             	add    $0x10,%esp
80104cca:	c9                   	leave  
80104ccb:	c3                   	ret    
80104ccc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80104cd0:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80104cd5:	c9                   	leave  
80104cd6:	c3                   	ret    
80104cd7:	89 f6                	mov    %esi,%esi
80104cd9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80104ce0 <syscall>:
80104ce0:	55                   	push   %ebp
80104ce1:	89 e5                	mov    %esp,%ebp
80104ce3:	56                   	push   %esi
80104ce4:	53                   	push   %ebx
80104ce5:	e8 86 eb ff ff       	call   80103870 <myproc>
80104cea:	8b 70 18             	mov    0x18(%eax),%esi
80104ced:	89 c3                	mov    %eax,%ebx
80104cef:	8b 46 1c             	mov    0x1c(%esi),%eax
80104cf2:	8d 50 ff             	lea    -0x1(%eax),%edx
80104cf5:	83 fa 17             	cmp    $0x17,%edx
80104cf8:	77 1e                	ja     80104d18 <syscall+0x38>
80104cfa:	8b 14 85 20 7c 10 80 	mov    -0x7fef83e0(,%eax,4),%edx
80104d01:	85 d2                	test   %edx,%edx
80104d03:	74 13                	je     80104d18 <syscall+0x38>
80104d05:	ff d2                	call   *%edx
80104d07:	89 46 1c             	mov    %eax,0x1c(%esi)
80104d0a:	8d 65 f8             	lea    -0x8(%ebp),%esp
80104d0d:	5b                   	pop    %ebx
80104d0e:	5e                   	pop    %esi
80104d0f:	5d                   	pop    %ebp
80104d10:	c3                   	ret    
80104d11:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80104d18:	50                   	push   %eax
80104d19:	8d 43 6c             	lea    0x6c(%ebx),%eax
80104d1c:	50                   	push   %eax
80104d1d:	ff 73 10             	pushl  0x10(%ebx)
80104d20:	68 f9 7b 10 80       	push   $0x80107bf9
80104d25:	e8 36 b9 ff ff       	call   80100660 <cprintf>
80104d2a:	8b 43 18             	mov    0x18(%ebx),%eax
80104d2d:	83 c4 10             	add    $0x10,%esp
80104d30:	c7 40 1c ff ff ff ff 	movl   $0xffffffff,0x1c(%eax)
80104d37:	8d 65 f8             	lea    -0x8(%ebp),%esp
80104d3a:	5b                   	pop    %ebx
80104d3b:	5e                   	pop    %esi
80104d3c:	5d                   	pop    %ebp
80104d3d:	c3                   	ret    
80104d3e:	66 90                	xchg   %ax,%ax

80104d40 <create>:
80104d40:	55                   	push   %ebp
80104d41:	89 e5                	mov    %esp,%ebp
80104d43:	57                   	push   %edi
80104d44:	56                   	push   %esi
80104d45:	53                   	push   %ebx
80104d46:	8d 75 da             	lea    -0x26(%ebp),%esi
80104d49:	83 ec 44             	sub    $0x44,%esp
80104d4c:	89 4d c0             	mov    %ecx,-0x40(%ebp)
80104d4f:	8b 4d 08             	mov    0x8(%ebp),%ecx
80104d52:	56                   	push   %esi
80104d53:	50                   	push   %eax
80104d54:	89 55 c4             	mov    %edx,-0x3c(%ebp)
80104d57:	89 4d bc             	mov    %ecx,-0x44(%ebp)
80104d5a:	e8 81 d1 ff ff       	call   80101ee0 <nameiparent>
80104d5f:	83 c4 10             	add    $0x10,%esp
80104d62:	85 c0                	test   %eax,%eax
80104d64:	0f 84 f6 00 00 00    	je     80104e60 <create+0x120>
80104d6a:	83 ec 0c             	sub    $0xc,%esp
80104d6d:	89 c7                	mov    %eax,%edi
80104d6f:	50                   	push   %eax
80104d70:	e8 fb c8 ff ff       	call   80101670 <ilock>
80104d75:	8d 45 d4             	lea    -0x2c(%ebp),%eax
80104d78:	83 c4 0c             	add    $0xc,%esp
80104d7b:	50                   	push   %eax
80104d7c:	56                   	push   %esi
80104d7d:	57                   	push   %edi
80104d7e:	e8 1d ce ff ff       	call   80101ba0 <dirlookup>
80104d83:	83 c4 10             	add    $0x10,%esp
80104d86:	85 c0                	test   %eax,%eax
80104d88:	89 c3                	mov    %eax,%ebx
80104d8a:	74 54                	je     80104de0 <create+0xa0>
80104d8c:	83 ec 0c             	sub    $0xc,%esp
80104d8f:	57                   	push   %edi
80104d90:	e8 6b cb ff ff       	call   80101900 <iunlockput>
80104d95:	89 1c 24             	mov    %ebx,(%esp)
80104d98:	e8 d3 c8 ff ff       	call   80101670 <ilock>
80104d9d:	83 c4 10             	add    $0x10,%esp
80104da0:	66 83 7d c4 02       	cmpw   $0x2,-0x3c(%ebp)
80104da5:	75 19                	jne    80104dc0 <create+0x80>
80104da7:	66 83 7b 50 02       	cmpw   $0x2,0x50(%ebx)
80104dac:	89 d8                	mov    %ebx,%eax
80104dae:	75 10                	jne    80104dc0 <create+0x80>
80104db0:	8d 65 f4             	lea    -0xc(%ebp),%esp
80104db3:	5b                   	pop    %ebx
80104db4:	5e                   	pop    %esi
80104db5:	5f                   	pop    %edi
80104db6:	5d                   	pop    %ebp
80104db7:	c3                   	ret    
80104db8:	90                   	nop
80104db9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80104dc0:	83 ec 0c             	sub    $0xc,%esp
80104dc3:	53                   	push   %ebx
80104dc4:	e8 37 cb ff ff       	call   80101900 <iunlockput>
80104dc9:	83 c4 10             	add    $0x10,%esp
80104dcc:	8d 65 f4             	lea    -0xc(%ebp),%esp
80104dcf:	31 c0                	xor    %eax,%eax
80104dd1:	5b                   	pop    %ebx
80104dd2:	5e                   	pop    %esi
80104dd3:	5f                   	pop    %edi
80104dd4:	5d                   	pop    %ebp
80104dd5:	c3                   	ret    
80104dd6:	8d 76 00             	lea    0x0(%esi),%esi
80104dd9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80104de0:	0f bf 45 c4          	movswl -0x3c(%ebp),%eax
80104de4:	83 ec 08             	sub    $0x8,%esp
80104de7:	50                   	push   %eax
80104de8:	ff 37                	pushl  (%edi)
80104dea:	e8 11 c7 ff ff       	call   80101500 <ialloc>
80104def:	83 c4 10             	add    $0x10,%esp
80104df2:	85 c0                	test   %eax,%eax
80104df4:	89 c3                	mov    %eax,%ebx
80104df6:	0f 84 cc 00 00 00    	je     80104ec8 <create+0x188>
80104dfc:	83 ec 0c             	sub    $0xc,%esp
80104dff:	50                   	push   %eax
80104e00:	e8 6b c8 ff ff       	call   80101670 <ilock>
80104e05:	0f b7 45 c0          	movzwl -0x40(%ebp),%eax
80104e09:	66 89 43 52          	mov    %ax,0x52(%ebx)
80104e0d:	0f b7 45 bc          	movzwl -0x44(%ebp),%eax
80104e11:	66 89 43 54          	mov    %ax,0x54(%ebx)
80104e15:	b8 01 00 00 00       	mov    $0x1,%eax
80104e1a:	66 89 43 56          	mov    %ax,0x56(%ebx)
80104e1e:	89 1c 24             	mov    %ebx,(%esp)
80104e21:	e8 9a c7 ff ff       	call   801015c0 <iupdate>
80104e26:	83 c4 10             	add    $0x10,%esp
80104e29:	66 83 7d c4 01       	cmpw   $0x1,-0x3c(%ebp)
80104e2e:	74 40                	je     80104e70 <create+0x130>
80104e30:	83 ec 04             	sub    $0x4,%esp
80104e33:	ff 73 04             	pushl  0x4(%ebx)
80104e36:	56                   	push   %esi
80104e37:	57                   	push   %edi
80104e38:	e8 c3 cf ff ff       	call   80101e00 <dirlink>
80104e3d:	83 c4 10             	add    $0x10,%esp
80104e40:	85 c0                	test   %eax,%eax
80104e42:	78 77                	js     80104ebb <create+0x17b>
80104e44:	83 ec 0c             	sub    $0xc,%esp
80104e47:	57                   	push   %edi
80104e48:	e8 b3 ca ff ff       	call   80101900 <iunlockput>
80104e4d:	83 c4 10             	add    $0x10,%esp
80104e50:	8d 65 f4             	lea    -0xc(%ebp),%esp
80104e53:	89 d8                	mov    %ebx,%eax
80104e55:	5b                   	pop    %ebx
80104e56:	5e                   	pop    %esi
80104e57:	5f                   	pop    %edi
80104e58:	5d                   	pop    %ebp
80104e59:	c3                   	ret    
80104e5a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80104e60:	31 c0                	xor    %eax,%eax
80104e62:	e9 49 ff ff ff       	jmp    80104db0 <create+0x70>
80104e67:	89 f6                	mov    %esi,%esi
80104e69:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80104e70:	66 83 47 56 01       	addw   $0x1,0x56(%edi)
80104e75:	83 ec 0c             	sub    $0xc,%esp
80104e78:	57                   	push   %edi
80104e79:	e8 42 c7 ff ff       	call   801015c0 <iupdate>
80104e7e:	83 c4 0c             	add    $0xc,%esp
80104e81:	ff 73 04             	pushl  0x4(%ebx)
80104e84:	68 a0 7c 10 80       	push   $0x80107ca0
80104e89:	53                   	push   %ebx
80104e8a:	e8 71 cf ff ff       	call   80101e00 <dirlink>
80104e8f:	83 c4 10             	add    $0x10,%esp
80104e92:	85 c0                	test   %eax,%eax
80104e94:	78 18                	js     80104eae <create+0x16e>
80104e96:	83 ec 04             	sub    $0x4,%esp
80104e99:	ff 77 04             	pushl  0x4(%edi)
80104e9c:	68 9f 7c 10 80       	push   $0x80107c9f
80104ea1:	53                   	push   %ebx
80104ea2:	e8 59 cf ff ff       	call   80101e00 <dirlink>
80104ea7:	83 c4 10             	add    $0x10,%esp
80104eaa:	85 c0                	test   %eax,%eax
80104eac:	79 82                	jns    80104e30 <create+0xf0>
80104eae:	83 ec 0c             	sub    $0xc,%esp
80104eb1:	68 93 7c 10 80       	push   $0x80107c93
80104eb6:	e8 b5 b4 ff ff       	call   80100370 <panic>
80104ebb:	83 ec 0c             	sub    $0xc,%esp
80104ebe:	68 a2 7c 10 80       	push   $0x80107ca2
80104ec3:	e8 a8 b4 ff ff       	call   80100370 <panic>
80104ec8:	83 ec 0c             	sub    $0xc,%esp
80104ecb:	68 84 7c 10 80       	push   $0x80107c84
80104ed0:	e8 9b b4 ff ff       	call   80100370 <panic>
80104ed5:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80104ed9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80104ee0 <argfd.constprop.0>:
80104ee0:	55                   	push   %ebp
80104ee1:	89 e5                	mov    %esp,%ebp
80104ee3:	56                   	push   %esi
80104ee4:	53                   	push   %ebx
80104ee5:	89 c6                	mov    %eax,%esi
80104ee7:	8d 45 f4             	lea    -0xc(%ebp),%eax
80104eea:	89 d3                	mov    %edx,%ebx
80104eec:	83 ec 18             	sub    $0x18,%esp
80104eef:	50                   	push   %eax
80104ef0:	6a 00                	push   $0x0
80104ef2:	e8 f9 fc ff ff       	call   80104bf0 <argint>
80104ef7:	83 c4 10             	add    $0x10,%esp
80104efa:	85 c0                	test   %eax,%eax
80104efc:	78 32                	js     80104f30 <argfd.constprop.0+0x50>
80104efe:	83 7d f4 0f          	cmpl   $0xf,-0xc(%ebp)
80104f02:	77 2c                	ja     80104f30 <argfd.constprop.0+0x50>
80104f04:	e8 67 e9 ff ff       	call   80103870 <myproc>
80104f09:	8b 55 f4             	mov    -0xc(%ebp),%edx
80104f0c:	8b 44 90 28          	mov    0x28(%eax,%edx,4),%eax
80104f10:	85 c0                	test   %eax,%eax
80104f12:	74 1c                	je     80104f30 <argfd.constprop.0+0x50>
80104f14:	85 f6                	test   %esi,%esi
80104f16:	74 02                	je     80104f1a <argfd.constprop.0+0x3a>
80104f18:	89 16                	mov    %edx,(%esi)
80104f1a:	85 db                	test   %ebx,%ebx
80104f1c:	74 22                	je     80104f40 <argfd.constprop.0+0x60>
80104f1e:	89 03                	mov    %eax,(%ebx)
80104f20:	31 c0                	xor    %eax,%eax
80104f22:	8d 65 f8             	lea    -0x8(%ebp),%esp
80104f25:	5b                   	pop    %ebx
80104f26:	5e                   	pop    %esi
80104f27:	5d                   	pop    %ebp
80104f28:	c3                   	ret    
80104f29:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80104f30:	8d 65 f8             	lea    -0x8(%ebp),%esp
80104f33:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80104f38:	5b                   	pop    %ebx
80104f39:	5e                   	pop    %esi
80104f3a:	5d                   	pop    %ebp
80104f3b:	c3                   	ret    
80104f3c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80104f40:	31 c0                	xor    %eax,%eax
80104f42:	eb de                	jmp    80104f22 <argfd.constprop.0+0x42>
80104f44:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80104f4a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

80104f50 <sys_dup>:
80104f50:	55                   	push   %ebp
80104f51:	31 c0                	xor    %eax,%eax
80104f53:	89 e5                	mov    %esp,%ebp
80104f55:	56                   	push   %esi
80104f56:	53                   	push   %ebx
80104f57:	8d 55 f4             	lea    -0xc(%ebp),%edx
80104f5a:	83 ec 10             	sub    $0x10,%esp
80104f5d:	e8 7e ff ff ff       	call   80104ee0 <argfd.constprop.0>
80104f62:	85 c0                	test   %eax,%eax
80104f64:	78 1a                	js     80104f80 <sys_dup+0x30>
80104f66:	31 db                	xor    %ebx,%ebx
80104f68:	8b 75 f4             	mov    -0xc(%ebp),%esi
80104f6b:	e8 00 e9 ff ff       	call   80103870 <myproc>
80104f70:	8b 54 98 28          	mov    0x28(%eax,%ebx,4),%edx
80104f74:	85 d2                	test   %edx,%edx
80104f76:	74 18                	je     80104f90 <sys_dup+0x40>
80104f78:	83 c3 01             	add    $0x1,%ebx
80104f7b:	83 fb 10             	cmp    $0x10,%ebx
80104f7e:	75 f0                	jne    80104f70 <sys_dup+0x20>
80104f80:	8d 65 f8             	lea    -0x8(%ebp),%esp
80104f83:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80104f88:	5b                   	pop    %ebx
80104f89:	5e                   	pop    %esi
80104f8a:	5d                   	pop    %ebp
80104f8b:	c3                   	ret    
80104f8c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80104f90:	89 74 98 28          	mov    %esi,0x28(%eax,%ebx,4)
80104f94:	83 ec 0c             	sub    $0xc,%esp
80104f97:	ff 75 f4             	pushl  -0xc(%ebp)
80104f9a:	e8 41 be ff ff       	call   80100de0 <filedup>
80104f9f:	83 c4 10             	add    $0x10,%esp
80104fa2:	8d 65 f8             	lea    -0x8(%ebp),%esp
80104fa5:	89 d8                	mov    %ebx,%eax
80104fa7:	5b                   	pop    %ebx
80104fa8:	5e                   	pop    %esi
80104fa9:	5d                   	pop    %ebp
80104faa:	c3                   	ret    
80104fab:	90                   	nop
80104fac:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80104fb0 <sys_read>:
80104fb0:	55                   	push   %ebp
80104fb1:	31 c0                	xor    %eax,%eax
80104fb3:	89 e5                	mov    %esp,%ebp
80104fb5:	83 ec 18             	sub    $0x18,%esp
80104fb8:	8d 55 ec             	lea    -0x14(%ebp),%edx
80104fbb:	e8 20 ff ff ff       	call   80104ee0 <argfd.constprop.0>
80104fc0:	85 c0                	test   %eax,%eax
80104fc2:	78 4c                	js     80105010 <sys_read+0x60>
80104fc4:	8d 45 f0             	lea    -0x10(%ebp),%eax
80104fc7:	83 ec 08             	sub    $0x8,%esp
80104fca:	50                   	push   %eax
80104fcb:	6a 02                	push   $0x2
80104fcd:	e8 1e fc ff ff       	call   80104bf0 <argint>
80104fd2:	83 c4 10             	add    $0x10,%esp
80104fd5:	85 c0                	test   %eax,%eax
80104fd7:	78 37                	js     80105010 <sys_read+0x60>
80104fd9:	8d 45 f4             	lea    -0xc(%ebp),%eax
80104fdc:	83 ec 04             	sub    $0x4,%esp
80104fdf:	ff 75 f0             	pushl  -0x10(%ebp)
80104fe2:	50                   	push   %eax
80104fe3:	6a 01                	push   $0x1
80104fe5:	e8 56 fc ff ff       	call   80104c40 <argptr>
80104fea:	83 c4 10             	add    $0x10,%esp
80104fed:	85 c0                	test   %eax,%eax
80104fef:	78 1f                	js     80105010 <sys_read+0x60>
80104ff1:	83 ec 04             	sub    $0x4,%esp
80104ff4:	ff 75 f0             	pushl  -0x10(%ebp)
80104ff7:	ff 75 f4             	pushl  -0xc(%ebp)
80104ffa:	ff 75 ec             	pushl  -0x14(%ebp)
80104ffd:	e8 4e bf ff ff       	call   80100f50 <fileread>
80105002:	83 c4 10             	add    $0x10,%esp
80105005:	c9                   	leave  
80105006:	c3                   	ret    
80105007:	89 f6                	mov    %esi,%esi
80105009:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80105010:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80105015:	c9                   	leave  
80105016:	c3                   	ret    
80105017:	89 f6                	mov    %esi,%esi
80105019:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80105020 <sys_write>:
80105020:	55                   	push   %ebp
80105021:	31 c0                	xor    %eax,%eax
80105023:	89 e5                	mov    %esp,%ebp
80105025:	83 ec 18             	sub    $0x18,%esp
80105028:	8d 55 ec             	lea    -0x14(%ebp),%edx
8010502b:	e8 b0 fe ff ff       	call   80104ee0 <argfd.constprop.0>
80105030:	85 c0                	test   %eax,%eax
80105032:	78 4c                	js     80105080 <sys_write+0x60>
80105034:	8d 45 f0             	lea    -0x10(%ebp),%eax
80105037:	83 ec 08             	sub    $0x8,%esp
8010503a:	50                   	push   %eax
8010503b:	6a 02                	push   $0x2
8010503d:	e8 ae fb ff ff       	call   80104bf0 <argint>
80105042:	83 c4 10             	add    $0x10,%esp
80105045:	85 c0                	test   %eax,%eax
80105047:	78 37                	js     80105080 <sys_write+0x60>
80105049:	8d 45 f4             	lea    -0xc(%ebp),%eax
8010504c:	83 ec 04             	sub    $0x4,%esp
8010504f:	ff 75 f0             	pushl  -0x10(%ebp)
80105052:	50                   	push   %eax
80105053:	6a 01                	push   $0x1
80105055:	e8 e6 fb ff ff       	call   80104c40 <argptr>
8010505a:	83 c4 10             	add    $0x10,%esp
8010505d:	85 c0                	test   %eax,%eax
8010505f:	78 1f                	js     80105080 <sys_write+0x60>
80105061:	83 ec 04             	sub    $0x4,%esp
80105064:	ff 75 f0             	pushl  -0x10(%ebp)
80105067:	ff 75 f4             	pushl  -0xc(%ebp)
8010506a:	ff 75 ec             	pushl  -0x14(%ebp)
8010506d:	e8 6e bf ff ff       	call   80100fe0 <filewrite>
80105072:	83 c4 10             	add    $0x10,%esp
80105075:	c9                   	leave  
80105076:	c3                   	ret    
80105077:	89 f6                	mov    %esi,%esi
80105079:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80105080:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80105085:	c9                   	leave  
80105086:	c3                   	ret    
80105087:	89 f6                	mov    %esi,%esi
80105089:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80105090 <sys_close>:
80105090:	55                   	push   %ebp
80105091:	89 e5                	mov    %esp,%ebp
80105093:	83 ec 18             	sub    $0x18,%esp
80105096:	8d 55 f4             	lea    -0xc(%ebp),%edx
80105099:	8d 45 f0             	lea    -0x10(%ebp),%eax
8010509c:	e8 3f fe ff ff       	call   80104ee0 <argfd.constprop.0>
801050a1:	85 c0                	test   %eax,%eax
801050a3:	78 2b                	js     801050d0 <sys_close+0x40>
801050a5:	e8 c6 e7 ff ff       	call   80103870 <myproc>
801050aa:	8b 55 f0             	mov    -0x10(%ebp),%edx
801050ad:	83 ec 0c             	sub    $0xc,%esp
801050b0:	c7 44 90 28 00 00 00 	movl   $0x0,0x28(%eax,%edx,4)
801050b7:	00 
801050b8:	ff 75 f4             	pushl  -0xc(%ebp)
801050bb:	e8 70 bd ff ff       	call   80100e30 <fileclose>
801050c0:	83 c4 10             	add    $0x10,%esp
801050c3:	31 c0                	xor    %eax,%eax
801050c5:	c9                   	leave  
801050c6:	c3                   	ret    
801050c7:	89 f6                	mov    %esi,%esi
801050c9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
801050d0:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
801050d5:	c9                   	leave  
801050d6:	c3                   	ret    
801050d7:	89 f6                	mov    %esi,%esi
801050d9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801050e0 <sys_fstat>:
801050e0:	55                   	push   %ebp
801050e1:	31 c0                	xor    %eax,%eax
801050e3:	89 e5                	mov    %esp,%ebp
801050e5:	83 ec 18             	sub    $0x18,%esp
801050e8:	8d 55 f0             	lea    -0x10(%ebp),%edx
801050eb:	e8 f0 fd ff ff       	call   80104ee0 <argfd.constprop.0>
801050f0:	85 c0                	test   %eax,%eax
801050f2:	78 2c                	js     80105120 <sys_fstat+0x40>
801050f4:	8d 45 f4             	lea    -0xc(%ebp),%eax
801050f7:	83 ec 04             	sub    $0x4,%esp
801050fa:	6a 14                	push   $0x14
801050fc:	50                   	push   %eax
801050fd:	6a 01                	push   $0x1
801050ff:	e8 3c fb ff ff       	call   80104c40 <argptr>
80105104:	83 c4 10             	add    $0x10,%esp
80105107:	85 c0                	test   %eax,%eax
80105109:	78 15                	js     80105120 <sys_fstat+0x40>
8010510b:	83 ec 08             	sub    $0x8,%esp
8010510e:	ff 75 f4             	pushl  -0xc(%ebp)
80105111:	ff 75 f0             	pushl  -0x10(%ebp)
80105114:	e8 e7 bd ff ff       	call   80100f00 <filestat>
80105119:	83 c4 10             	add    $0x10,%esp
8010511c:	c9                   	leave  
8010511d:	c3                   	ret    
8010511e:	66 90                	xchg   %ax,%ax
80105120:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80105125:	c9                   	leave  
80105126:	c3                   	ret    
80105127:	89 f6                	mov    %esi,%esi
80105129:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80105130 <sys_link>:
80105130:	55                   	push   %ebp
80105131:	89 e5                	mov    %esp,%ebp
80105133:	57                   	push   %edi
80105134:	56                   	push   %esi
80105135:	53                   	push   %ebx
80105136:	8d 45 d4             	lea    -0x2c(%ebp),%eax
80105139:	83 ec 34             	sub    $0x34,%esp
8010513c:	50                   	push   %eax
8010513d:	6a 00                	push   $0x0
8010513f:	e8 5c fb ff ff       	call   80104ca0 <argstr>
80105144:	83 c4 10             	add    $0x10,%esp
80105147:	85 c0                	test   %eax,%eax
80105149:	0f 88 fb 00 00 00    	js     8010524a <sys_link+0x11a>
8010514f:	8d 45 d0             	lea    -0x30(%ebp),%eax
80105152:	83 ec 08             	sub    $0x8,%esp
80105155:	50                   	push   %eax
80105156:	6a 01                	push   $0x1
80105158:	e8 43 fb ff ff       	call   80104ca0 <argstr>
8010515d:	83 c4 10             	add    $0x10,%esp
80105160:	85 c0                	test   %eax,%eax
80105162:	0f 88 e2 00 00 00    	js     8010524a <sys_link+0x11a>
80105168:	e8 e3 d9 ff ff       	call   80102b50 <begin_op>
8010516d:	83 ec 0c             	sub    $0xc,%esp
80105170:	ff 75 d4             	pushl  -0x2c(%ebp)
80105173:	e8 48 cd ff ff       	call   80101ec0 <namei>
80105178:	83 c4 10             	add    $0x10,%esp
8010517b:	85 c0                	test   %eax,%eax
8010517d:	89 c3                	mov    %eax,%ebx
8010517f:	0f 84 f3 00 00 00    	je     80105278 <sys_link+0x148>
80105185:	83 ec 0c             	sub    $0xc,%esp
80105188:	50                   	push   %eax
80105189:	e8 e2 c4 ff ff       	call   80101670 <ilock>
8010518e:	83 c4 10             	add    $0x10,%esp
80105191:	66 83 7b 50 01       	cmpw   $0x1,0x50(%ebx)
80105196:	0f 84 c4 00 00 00    	je     80105260 <sys_link+0x130>
8010519c:	66 83 43 56 01       	addw   $0x1,0x56(%ebx)
801051a1:	83 ec 0c             	sub    $0xc,%esp
801051a4:	8d 7d da             	lea    -0x26(%ebp),%edi
801051a7:	53                   	push   %ebx
801051a8:	e8 13 c4 ff ff       	call   801015c0 <iupdate>
801051ad:	89 1c 24             	mov    %ebx,(%esp)
801051b0:	e8 9b c5 ff ff       	call   80101750 <iunlock>
801051b5:	58                   	pop    %eax
801051b6:	5a                   	pop    %edx
801051b7:	57                   	push   %edi
801051b8:	ff 75 d0             	pushl  -0x30(%ebp)
801051bb:	e8 20 cd ff ff       	call   80101ee0 <nameiparent>
801051c0:	83 c4 10             	add    $0x10,%esp
801051c3:	85 c0                	test   %eax,%eax
801051c5:	89 c6                	mov    %eax,%esi
801051c7:	74 5b                	je     80105224 <sys_link+0xf4>
801051c9:	83 ec 0c             	sub    $0xc,%esp
801051cc:	50                   	push   %eax
801051cd:	e8 9e c4 ff ff       	call   80101670 <ilock>
801051d2:	83 c4 10             	add    $0x10,%esp
801051d5:	8b 03                	mov    (%ebx),%eax
801051d7:	39 06                	cmp    %eax,(%esi)
801051d9:	75 3d                	jne    80105218 <sys_link+0xe8>
801051db:	83 ec 04             	sub    $0x4,%esp
801051de:	ff 73 04             	pushl  0x4(%ebx)
801051e1:	57                   	push   %edi
801051e2:	56                   	push   %esi
801051e3:	e8 18 cc ff ff       	call   80101e00 <dirlink>
801051e8:	83 c4 10             	add    $0x10,%esp
801051eb:	85 c0                	test   %eax,%eax
801051ed:	78 29                	js     80105218 <sys_link+0xe8>
801051ef:	83 ec 0c             	sub    $0xc,%esp
801051f2:	56                   	push   %esi
801051f3:	e8 08 c7 ff ff       	call   80101900 <iunlockput>
801051f8:	89 1c 24             	mov    %ebx,(%esp)
801051fb:	e8 a0 c5 ff ff       	call   801017a0 <iput>
80105200:	e8 bb d9 ff ff       	call   80102bc0 <end_op>
80105205:	83 c4 10             	add    $0x10,%esp
80105208:	31 c0                	xor    %eax,%eax
8010520a:	8d 65 f4             	lea    -0xc(%ebp),%esp
8010520d:	5b                   	pop    %ebx
8010520e:	5e                   	pop    %esi
8010520f:	5f                   	pop    %edi
80105210:	5d                   	pop    %ebp
80105211:	c3                   	ret    
80105212:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80105218:	83 ec 0c             	sub    $0xc,%esp
8010521b:	56                   	push   %esi
8010521c:	e8 df c6 ff ff       	call   80101900 <iunlockput>
80105221:	83 c4 10             	add    $0x10,%esp
80105224:	83 ec 0c             	sub    $0xc,%esp
80105227:	53                   	push   %ebx
80105228:	e8 43 c4 ff ff       	call   80101670 <ilock>
8010522d:	66 83 6b 56 01       	subw   $0x1,0x56(%ebx)
80105232:	89 1c 24             	mov    %ebx,(%esp)
80105235:	e8 86 c3 ff ff       	call   801015c0 <iupdate>
8010523a:	89 1c 24             	mov    %ebx,(%esp)
8010523d:	e8 be c6 ff ff       	call   80101900 <iunlockput>
80105242:	e8 79 d9 ff ff       	call   80102bc0 <end_op>
80105247:	83 c4 10             	add    $0x10,%esp
8010524a:	8d 65 f4             	lea    -0xc(%ebp),%esp
8010524d:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80105252:	5b                   	pop    %ebx
80105253:	5e                   	pop    %esi
80105254:	5f                   	pop    %edi
80105255:	5d                   	pop    %ebp
80105256:	c3                   	ret    
80105257:	89 f6                	mov    %esi,%esi
80105259:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80105260:	83 ec 0c             	sub    $0xc,%esp
80105263:	53                   	push   %ebx
80105264:	e8 97 c6 ff ff       	call   80101900 <iunlockput>
80105269:	e8 52 d9 ff ff       	call   80102bc0 <end_op>
8010526e:	83 c4 10             	add    $0x10,%esp
80105271:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80105276:	eb 92                	jmp    8010520a <sys_link+0xda>
80105278:	e8 43 d9 ff ff       	call   80102bc0 <end_op>
8010527d:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80105282:	eb 86                	jmp    8010520a <sys_link+0xda>
80105284:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
8010528a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

80105290 <sys_unlink>:
80105290:	55                   	push   %ebp
80105291:	89 e5                	mov    %esp,%ebp
80105293:	57                   	push   %edi
80105294:	56                   	push   %esi
80105295:	53                   	push   %ebx
80105296:	8d 45 c0             	lea    -0x40(%ebp),%eax
80105299:	83 ec 54             	sub    $0x54,%esp
8010529c:	50                   	push   %eax
8010529d:	6a 00                	push   $0x0
8010529f:	e8 fc f9 ff ff       	call   80104ca0 <argstr>
801052a4:	83 c4 10             	add    $0x10,%esp
801052a7:	85 c0                	test   %eax,%eax
801052a9:	0f 88 82 01 00 00    	js     80105431 <sys_unlink+0x1a1>
801052af:	8d 5d ca             	lea    -0x36(%ebp),%ebx
801052b2:	e8 99 d8 ff ff       	call   80102b50 <begin_op>
801052b7:	83 ec 08             	sub    $0x8,%esp
801052ba:	53                   	push   %ebx
801052bb:	ff 75 c0             	pushl  -0x40(%ebp)
801052be:	e8 1d cc ff ff       	call   80101ee0 <nameiparent>
801052c3:	83 c4 10             	add    $0x10,%esp
801052c6:	85 c0                	test   %eax,%eax
801052c8:	89 45 b4             	mov    %eax,-0x4c(%ebp)
801052cb:	0f 84 6a 01 00 00    	je     8010543b <sys_unlink+0x1ab>
801052d1:	8b 75 b4             	mov    -0x4c(%ebp),%esi
801052d4:	83 ec 0c             	sub    $0xc,%esp
801052d7:	56                   	push   %esi
801052d8:	e8 93 c3 ff ff       	call   80101670 <ilock>
801052dd:	58                   	pop    %eax
801052de:	5a                   	pop    %edx
801052df:	68 a0 7c 10 80       	push   $0x80107ca0
801052e4:	53                   	push   %ebx
801052e5:	e8 96 c8 ff ff       	call   80101b80 <namecmp>
801052ea:	83 c4 10             	add    $0x10,%esp
801052ed:	85 c0                	test   %eax,%eax
801052ef:	0f 84 fc 00 00 00    	je     801053f1 <sys_unlink+0x161>
801052f5:	83 ec 08             	sub    $0x8,%esp
801052f8:	68 9f 7c 10 80       	push   $0x80107c9f
801052fd:	53                   	push   %ebx
801052fe:	e8 7d c8 ff ff       	call   80101b80 <namecmp>
80105303:	83 c4 10             	add    $0x10,%esp
80105306:	85 c0                	test   %eax,%eax
80105308:	0f 84 e3 00 00 00    	je     801053f1 <sys_unlink+0x161>
8010530e:	8d 45 c4             	lea    -0x3c(%ebp),%eax
80105311:	83 ec 04             	sub    $0x4,%esp
80105314:	50                   	push   %eax
80105315:	53                   	push   %ebx
80105316:	56                   	push   %esi
80105317:	e8 84 c8 ff ff       	call   80101ba0 <dirlookup>
8010531c:	83 c4 10             	add    $0x10,%esp
8010531f:	85 c0                	test   %eax,%eax
80105321:	89 c3                	mov    %eax,%ebx
80105323:	0f 84 c8 00 00 00    	je     801053f1 <sys_unlink+0x161>
80105329:	83 ec 0c             	sub    $0xc,%esp
8010532c:	50                   	push   %eax
8010532d:	e8 3e c3 ff ff       	call   80101670 <ilock>
80105332:	83 c4 10             	add    $0x10,%esp
80105335:	66 83 7b 56 00       	cmpw   $0x0,0x56(%ebx)
8010533a:	0f 8e 24 01 00 00    	jle    80105464 <sys_unlink+0x1d4>
80105340:	66 83 7b 50 01       	cmpw   $0x1,0x50(%ebx)
80105345:	8d 75 d8             	lea    -0x28(%ebp),%esi
80105348:	74 66                	je     801053b0 <sys_unlink+0x120>
8010534a:	83 ec 04             	sub    $0x4,%esp
8010534d:	6a 10                	push   $0x10
8010534f:	6a 00                	push   $0x0
80105351:	56                   	push   %esi
80105352:	e8 89 f5 ff ff       	call   801048e0 <memset>
80105357:	6a 10                	push   $0x10
80105359:	ff 75 c4             	pushl  -0x3c(%ebp)
8010535c:	56                   	push   %esi
8010535d:	ff 75 b4             	pushl  -0x4c(%ebp)
80105360:	e8 eb c6 ff ff       	call   80101a50 <writei>
80105365:	83 c4 20             	add    $0x20,%esp
80105368:	83 f8 10             	cmp    $0x10,%eax
8010536b:	0f 85 e6 00 00 00    	jne    80105457 <sys_unlink+0x1c7>
80105371:	66 83 7b 50 01       	cmpw   $0x1,0x50(%ebx)
80105376:	0f 84 9c 00 00 00    	je     80105418 <sys_unlink+0x188>
8010537c:	83 ec 0c             	sub    $0xc,%esp
8010537f:	ff 75 b4             	pushl  -0x4c(%ebp)
80105382:	e8 79 c5 ff ff       	call   80101900 <iunlockput>
80105387:	66 83 6b 56 01       	subw   $0x1,0x56(%ebx)
8010538c:	89 1c 24             	mov    %ebx,(%esp)
8010538f:	e8 2c c2 ff ff       	call   801015c0 <iupdate>
80105394:	89 1c 24             	mov    %ebx,(%esp)
80105397:	e8 64 c5 ff ff       	call   80101900 <iunlockput>
8010539c:	e8 1f d8 ff ff       	call   80102bc0 <end_op>
801053a1:	83 c4 10             	add    $0x10,%esp
801053a4:	31 c0                	xor    %eax,%eax
801053a6:	8d 65 f4             	lea    -0xc(%ebp),%esp
801053a9:	5b                   	pop    %ebx
801053aa:	5e                   	pop    %esi
801053ab:	5f                   	pop    %edi
801053ac:	5d                   	pop    %ebp
801053ad:	c3                   	ret    
801053ae:	66 90                	xchg   %ax,%ax
801053b0:	83 7b 58 20          	cmpl   $0x20,0x58(%ebx)
801053b4:	76 94                	jbe    8010534a <sys_unlink+0xba>
801053b6:	bf 20 00 00 00       	mov    $0x20,%edi
801053bb:	eb 0f                	jmp    801053cc <sys_unlink+0x13c>
801053bd:	8d 76 00             	lea    0x0(%esi),%esi
801053c0:	83 c7 10             	add    $0x10,%edi
801053c3:	3b 7b 58             	cmp    0x58(%ebx),%edi
801053c6:	0f 83 7e ff ff ff    	jae    8010534a <sys_unlink+0xba>
801053cc:	6a 10                	push   $0x10
801053ce:	57                   	push   %edi
801053cf:	56                   	push   %esi
801053d0:	53                   	push   %ebx
801053d1:	e8 7a c5 ff ff       	call   80101950 <readi>
801053d6:	83 c4 10             	add    $0x10,%esp
801053d9:	83 f8 10             	cmp    $0x10,%eax
801053dc:	75 6c                	jne    8010544a <sys_unlink+0x1ba>
801053de:	66 83 7d d8 00       	cmpw   $0x0,-0x28(%ebp)
801053e3:	74 db                	je     801053c0 <sys_unlink+0x130>
801053e5:	83 ec 0c             	sub    $0xc,%esp
801053e8:	53                   	push   %ebx
801053e9:	e8 12 c5 ff ff       	call   80101900 <iunlockput>
801053ee:	83 c4 10             	add    $0x10,%esp
801053f1:	83 ec 0c             	sub    $0xc,%esp
801053f4:	ff 75 b4             	pushl  -0x4c(%ebp)
801053f7:	e8 04 c5 ff ff       	call   80101900 <iunlockput>
801053fc:	e8 bf d7 ff ff       	call   80102bc0 <end_op>
80105401:	83 c4 10             	add    $0x10,%esp
80105404:	8d 65 f4             	lea    -0xc(%ebp),%esp
80105407:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
8010540c:	5b                   	pop    %ebx
8010540d:	5e                   	pop    %esi
8010540e:	5f                   	pop    %edi
8010540f:	5d                   	pop    %ebp
80105410:	c3                   	ret    
80105411:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80105418:	8b 45 b4             	mov    -0x4c(%ebp),%eax
8010541b:	83 ec 0c             	sub    $0xc,%esp
8010541e:	66 83 68 56 01       	subw   $0x1,0x56(%eax)
80105423:	50                   	push   %eax
80105424:	e8 97 c1 ff ff       	call   801015c0 <iupdate>
80105429:	83 c4 10             	add    $0x10,%esp
8010542c:	e9 4b ff ff ff       	jmp    8010537c <sys_unlink+0xec>
80105431:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80105436:	e9 6b ff ff ff       	jmp    801053a6 <sys_unlink+0x116>
8010543b:	e8 80 d7 ff ff       	call   80102bc0 <end_op>
80105440:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80105445:	e9 5c ff ff ff       	jmp    801053a6 <sys_unlink+0x116>
8010544a:	83 ec 0c             	sub    $0xc,%esp
8010544d:	68 c4 7c 10 80       	push   $0x80107cc4
80105452:	e8 19 af ff ff       	call   80100370 <panic>
80105457:	83 ec 0c             	sub    $0xc,%esp
8010545a:	68 d6 7c 10 80       	push   $0x80107cd6
8010545f:	e8 0c af ff ff       	call   80100370 <panic>
80105464:	83 ec 0c             	sub    $0xc,%esp
80105467:	68 b2 7c 10 80       	push   $0x80107cb2
8010546c:	e8 ff ae ff ff       	call   80100370 <panic>
80105471:	eb 0d                	jmp    80105480 <sys_open>
80105473:	90                   	nop
80105474:	90                   	nop
80105475:	90                   	nop
80105476:	90                   	nop
80105477:	90                   	nop
80105478:	90                   	nop
80105479:	90                   	nop
8010547a:	90                   	nop
8010547b:	90                   	nop
8010547c:	90                   	nop
8010547d:	90                   	nop
8010547e:	90                   	nop
8010547f:	90                   	nop

80105480 <sys_open>:
80105480:	55                   	push   %ebp
80105481:	89 e5                	mov    %esp,%ebp
80105483:	57                   	push   %edi
80105484:	56                   	push   %esi
80105485:	53                   	push   %ebx
80105486:	8d 45 e0             	lea    -0x20(%ebp),%eax
80105489:	83 ec 24             	sub    $0x24,%esp
8010548c:	50                   	push   %eax
8010548d:	6a 00                	push   $0x0
8010548f:	e8 0c f8 ff ff       	call   80104ca0 <argstr>
80105494:	83 c4 10             	add    $0x10,%esp
80105497:	85 c0                	test   %eax,%eax
80105499:	0f 88 9e 00 00 00    	js     8010553d <sys_open+0xbd>
8010549f:	8d 45 e4             	lea    -0x1c(%ebp),%eax
801054a2:	83 ec 08             	sub    $0x8,%esp
801054a5:	50                   	push   %eax
801054a6:	6a 01                	push   $0x1
801054a8:	e8 43 f7 ff ff       	call   80104bf0 <argint>
801054ad:	83 c4 10             	add    $0x10,%esp
801054b0:	85 c0                	test   %eax,%eax
801054b2:	0f 88 85 00 00 00    	js     8010553d <sys_open+0xbd>
801054b8:	e8 93 d6 ff ff       	call   80102b50 <begin_op>
801054bd:	f6 45 e5 02          	testb  $0x2,-0x1b(%ebp)
801054c1:	0f 85 89 00 00 00    	jne    80105550 <sys_open+0xd0>
801054c7:	83 ec 0c             	sub    $0xc,%esp
801054ca:	ff 75 e0             	pushl  -0x20(%ebp)
801054cd:	e8 ee c9 ff ff       	call   80101ec0 <namei>
801054d2:	83 c4 10             	add    $0x10,%esp
801054d5:	85 c0                	test   %eax,%eax
801054d7:	89 c6                	mov    %eax,%esi
801054d9:	0f 84 8e 00 00 00    	je     8010556d <sys_open+0xed>
801054df:	83 ec 0c             	sub    $0xc,%esp
801054e2:	50                   	push   %eax
801054e3:	e8 88 c1 ff ff       	call   80101670 <ilock>
801054e8:	83 c4 10             	add    $0x10,%esp
801054eb:	66 83 7e 50 01       	cmpw   $0x1,0x50(%esi)
801054f0:	0f 84 d2 00 00 00    	je     801055c8 <sys_open+0x148>
801054f6:	e8 75 b8 ff ff       	call   80100d70 <filealloc>
801054fb:	85 c0                	test   %eax,%eax
801054fd:	89 c7                	mov    %eax,%edi
801054ff:	74 2b                	je     8010552c <sys_open+0xac>
80105501:	31 db                	xor    %ebx,%ebx
80105503:	e8 68 e3 ff ff       	call   80103870 <myproc>
80105508:	90                   	nop
80105509:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80105510:	8b 54 98 28          	mov    0x28(%eax,%ebx,4),%edx
80105514:	85 d2                	test   %edx,%edx
80105516:	74 68                	je     80105580 <sys_open+0x100>
80105518:	83 c3 01             	add    $0x1,%ebx
8010551b:	83 fb 10             	cmp    $0x10,%ebx
8010551e:	75 f0                	jne    80105510 <sys_open+0x90>
80105520:	83 ec 0c             	sub    $0xc,%esp
80105523:	57                   	push   %edi
80105524:	e8 07 b9 ff ff       	call   80100e30 <fileclose>
80105529:	83 c4 10             	add    $0x10,%esp
8010552c:	83 ec 0c             	sub    $0xc,%esp
8010552f:	56                   	push   %esi
80105530:	e8 cb c3 ff ff       	call   80101900 <iunlockput>
80105535:	e8 86 d6 ff ff       	call   80102bc0 <end_op>
8010553a:	83 c4 10             	add    $0x10,%esp
8010553d:	8d 65 f4             	lea    -0xc(%ebp),%esp
80105540:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80105545:	5b                   	pop    %ebx
80105546:	5e                   	pop    %esi
80105547:	5f                   	pop    %edi
80105548:	5d                   	pop    %ebp
80105549:	c3                   	ret    
8010554a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80105550:	83 ec 0c             	sub    $0xc,%esp
80105553:	8b 45 e0             	mov    -0x20(%ebp),%eax
80105556:	31 c9                	xor    %ecx,%ecx
80105558:	6a 00                	push   $0x0
8010555a:	ba 02 00 00 00       	mov    $0x2,%edx
8010555f:	e8 dc f7 ff ff       	call   80104d40 <create>
80105564:	83 c4 10             	add    $0x10,%esp
80105567:	85 c0                	test   %eax,%eax
80105569:	89 c6                	mov    %eax,%esi
8010556b:	75 89                	jne    801054f6 <sys_open+0x76>
8010556d:	e8 4e d6 ff ff       	call   80102bc0 <end_op>
80105572:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80105577:	eb 43                	jmp    801055bc <sys_open+0x13c>
80105579:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80105580:	83 ec 0c             	sub    $0xc,%esp
80105583:	89 7c 98 28          	mov    %edi,0x28(%eax,%ebx,4)
80105587:	56                   	push   %esi
80105588:	e8 c3 c1 ff ff       	call   80101750 <iunlock>
8010558d:	e8 2e d6 ff ff       	call   80102bc0 <end_op>
80105592:	c7 07 02 00 00 00    	movl   $0x2,(%edi)
80105598:	8b 55 e4             	mov    -0x1c(%ebp),%edx
8010559b:	83 c4 10             	add    $0x10,%esp
8010559e:	89 77 10             	mov    %esi,0x10(%edi)
801055a1:	c7 47 14 00 00 00 00 	movl   $0x0,0x14(%edi)
801055a8:	89 d0                	mov    %edx,%eax
801055aa:	83 e0 01             	and    $0x1,%eax
801055ad:	83 f0 01             	xor    $0x1,%eax
801055b0:	83 e2 03             	and    $0x3,%edx
801055b3:	88 47 08             	mov    %al,0x8(%edi)
801055b6:	0f 95 47 09          	setne  0x9(%edi)
801055ba:	89 d8                	mov    %ebx,%eax
801055bc:	8d 65 f4             	lea    -0xc(%ebp),%esp
801055bf:	5b                   	pop    %ebx
801055c0:	5e                   	pop    %esi
801055c1:	5f                   	pop    %edi
801055c2:	5d                   	pop    %ebp
801055c3:	c3                   	ret    
801055c4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
801055c8:	8b 4d e4             	mov    -0x1c(%ebp),%ecx
801055cb:	85 c9                	test   %ecx,%ecx
801055cd:	0f 84 23 ff ff ff    	je     801054f6 <sys_open+0x76>
801055d3:	e9 54 ff ff ff       	jmp    8010552c <sys_open+0xac>
801055d8:	90                   	nop
801055d9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

801055e0 <sys_mkdir>:
801055e0:	55                   	push   %ebp
801055e1:	89 e5                	mov    %esp,%ebp
801055e3:	83 ec 18             	sub    $0x18,%esp
801055e6:	e8 65 d5 ff ff       	call   80102b50 <begin_op>
801055eb:	8d 45 f4             	lea    -0xc(%ebp),%eax
801055ee:	83 ec 08             	sub    $0x8,%esp
801055f1:	50                   	push   %eax
801055f2:	6a 00                	push   $0x0
801055f4:	e8 a7 f6 ff ff       	call   80104ca0 <argstr>
801055f9:	83 c4 10             	add    $0x10,%esp
801055fc:	85 c0                	test   %eax,%eax
801055fe:	78 30                	js     80105630 <sys_mkdir+0x50>
80105600:	83 ec 0c             	sub    $0xc,%esp
80105603:	8b 45 f4             	mov    -0xc(%ebp),%eax
80105606:	31 c9                	xor    %ecx,%ecx
80105608:	6a 00                	push   $0x0
8010560a:	ba 01 00 00 00       	mov    $0x1,%edx
8010560f:	e8 2c f7 ff ff       	call   80104d40 <create>
80105614:	83 c4 10             	add    $0x10,%esp
80105617:	85 c0                	test   %eax,%eax
80105619:	74 15                	je     80105630 <sys_mkdir+0x50>
8010561b:	83 ec 0c             	sub    $0xc,%esp
8010561e:	50                   	push   %eax
8010561f:	e8 dc c2 ff ff       	call   80101900 <iunlockput>
80105624:	e8 97 d5 ff ff       	call   80102bc0 <end_op>
80105629:	83 c4 10             	add    $0x10,%esp
8010562c:	31 c0                	xor    %eax,%eax
8010562e:	c9                   	leave  
8010562f:	c3                   	ret    
80105630:	e8 8b d5 ff ff       	call   80102bc0 <end_op>
80105635:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
8010563a:	c9                   	leave  
8010563b:	c3                   	ret    
8010563c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80105640 <sys_mknod>:
80105640:	55                   	push   %ebp
80105641:	89 e5                	mov    %esp,%ebp
80105643:	83 ec 18             	sub    $0x18,%esp
80105646:	e8 05 d5 ff ff       	call   80102b50 <begin_op>
8010564b:	8d 45 ec             	lea    -0x14(%ebp),%eax
8010564e:	83 ec 08             	sub    $0x8,%esp
80105651:	50                   	push   %eax
80105652:	6a 00                	push   $0x0
80105654:	e8 47 f6 ff ff       	call   80104ca0 <argstr>
80105659:	83 c4 10             	add    $0x10,%esp
8010565c:	85 c0                	test   %eax,%eax
8010565e:	78 60                	js     801056c0 <sys_mknod+0x80>
80105660:	8d 45 f0             	lea    -0x10(%ebp),%eax
80105663:	83 ec 08             	sub    $0x8,%esp
80105666:	50                   	push   %eax
80105667:	6a 01                	push   $0x1
80105669:	e8 82 f5 ff ff       	call   80104bf0 <argint>
8010566e:	83 c4 10             	add    $0x10,%esp
80105671:	85 c0                	test   %eax,%eax
80105673:	78 4b                	js     801056c0 <sys_mknod+0x80>
80105675:	8d 45 f4             	lea    -0xc(%ebp),%eax
80105678:	83 ec 08             	sub    $0x8,%esp
8010567b:	50                   	push   %eax
8010567c:	6a 02                	push   $0x2
8010567e:	e8 6d f5 ff ff       	call   80104bf0 <argint>
80105683:	83 c4 10             	add    $0x10,%esp
80105686:	85 c0                	test   %eax,%eax
80105688:	78 36                	js     801056c0 <sys_mknod+0x80>
8010568a:	0f bf 45 f4          	movswl -0xc(%ebp),%eax
8010568e:	83 ec 0c             	sub    $0xc,%esp
80105691:	0f bf 4d f0          	movswl -0x10(%ebp),%ecx
80105695:	ba 03 00 00 00       	mov    $0x3,%edx
8010569a:	50                   	push   %eax
8010569b:	8b 45 ec             	mov    -0x14(%ebp),%eax
8010569e:	e8 9d f6 ff ff       	call   80104d40 <create>
801056a3:	83 c4 10             	add    $0x10,%esp
801056a6:	85 c0                	test   %eax,%eax
801056a8:	74 16                	je     801056c0 <sys_mknod+0x80>
801056aa:	83 ec 0c             	sub    $0xc,%esp
801056ad:	50                   	push   %eax
801056ae:	e8 4d c2 ff ff       	call   80101900 <iunlockput>
801056b3:	e8 08 d5 ff ff       	call   80102bc0 <end_op>
801056b8:	83 c4 10             	add    $0x10,%esp
801056bb:	31 c0                	xor    %eax,%eax
801056bd:	c9                   	leave  
801056be:	c3                   	ret    
801056bf:	90                   	nop
801056c0:	e8 fb d4 ff ff       	call   80102bc0 <end_op>
801056c5:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
801056ca:	c9                   	leave  
801056cb:	c3                   	ret    
801056cc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

801056d0 <sys_chdir>:
801056d0:	55                   	push   %ebp
801056d1:	89 e5                	mov    %esp,%ebp
801056d3:	56                   	push   %esi
801056d4:	53                   	push   %ebx
801056d5:	83 ec 10             	sub    $0x10,%esp
801056d8:	e8 93 e1 ff ff       	call   80103870 <myproc>
801056dd:	89 c6                	mov    %eax,%esi
801056df:	e8 6c d4 ff ff       	call   80102b50 <begin_op>
801056e4:	8d 45 f4             	lea    -0xc(%ebp),%eax
801056e7:	83 ec 08             	sub    $0x8,%esp
801056ea:	50                   	push   %eax
801056eb:	6a 00                	push   $0x0
801056ed:	e8 ae f5 ff ff       	call   80104ca0 <argstr>
801056f2:	83 c4 10             	add    $0x10,%esp
801056f5:	85 c0                	test   %eax,%eax
801056f7:	78 77                	js     80105770 <sys_chdir+0xa0>
801056f9:	83 ec 0c             	sub    $0xc,%esp
801056fc:	ff 75 f4             	pushl  -0xc(%ebp)
801056ff:	e8 bc c7 ff ff       	call   80101ec0 <namei>
80105704:	83 c4 10             	add    $0x10,%esp
80105707:	85 c0                	test   %eax,%eax
80105709:	89 c3                	mov    %eax,%ebx
8010570b:	74 63                	je     80105770 <sys_chdir+0xa0>
8010570d:	83 ec 0c             	sub    $0xc,%esp
80105710:	50                   	push   %eax
80105711:	e8 5a bf ff ff       	call   80101670 <ilock>
80105716:	83 c4 10             	add    $0x10,%esp
80105719:	66 83 7b 50 01       	cmpw   $0x1,0x50(%ebx)
8010571e:	75 30                	jne    80105750 <sys_chdir+0x80>
80105720:	83 ec 0c             	sub    $0xc,%esp
80105723:	53                   	push   %ebx
80105724:	e8 27 c0 ff ff       	call   80101750 <iunlock>
80105729:	58                   	pop    %eax
8010572a:	ff 76 68             	pushl  0x68(%esi)
8010572d:	e8 6e c0 ff ff       	call   801017a0 <iput>
80105732:	e8 89 d4 ff ff       	call   80102bc0 <end_op>
80105737:	89 5e 68             	mov    %ebx,0x68(%esi)
8010573a:	83 c4 10             	add    $0x10,%esp
8010573d:	31 c0                	xor    %eax,%eax
8010573f:	8d 65 f8             	lea    -0x8(%ebp),%esp
80105742:	5b                   	pop    %ebx
80105743:	5e                   	pop    %esi
80105744:	5d                   	pop    %ebp
80105745:	c3                   	ret    
80105746:	8d 76 00             	lea    0x0(%esi),%esi
80105749:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80105750:	83 ec 0c             	sub    $0xc,%esp
80105753:	53                   	push   %ebx
80105754:	e8 a7 c1 ff ff       	call   80101900 <iunlockput>
80105759:	e8 62 d4 ff ff       	call   80102bc0 <end_op>
8010575e:	83 c4 10             	add    $0x10,%esp
80105761:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80105766:	eb d7                	jmp    8010573f <sys_chdir+0x6f>
80105768:	90                   	nop
80105769:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80105770:	e8 4b d4 ff ff       	call   80102bc0 <end_op>
80105775:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
8010577a:	eb c3                	jmp    8010573f <sys_chdir+0x6f>
8010577c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80105780 <sys_exec>:
80105780:	55                   	push   %ebp
80105781:	89 e5                	mov    %esp,%ebp
80105783:	57                   	push   %edi
80105784:	56                   	push   %esi
80105785:	53                   	push   %ebx
80105786:	8d 85 5c ff ff ff    	lea    -0xa4(%ebp),%eax
8010578c:	81 ec a4 00 00 00    	sub    $0xa4,%esp
80105792:	50                   	push   %eax
80105793:	6a 00                	push   $0x0
80105795:	e8 06 f5 ff ff       	call   80104ca0 <argstr>
8010579a:	83 c4 10             	add    $0x10,%esp
8010579d:	85 c0                	test   %eax,%eax
8010579f:	78 7f                	js     80105820 <sys_exec+0xa0>
801057a1:	8d 85 60 ff ff ff    	lea    -0xa0(%ebp),%eax
801057a7:	83 ec 08             	sub    $0x8,%esp
801057aa:	50                   	push   %eax
801057ab:	6a 01                	push   $0x1
801057ad:	e8 3e f4 ff ff       	call   80104bf0 <argint>
801057b2:	83 c4 10             	add    $0x10,%esp
801057b5:	85 c0                	test   %eax,%eax
801057b7:	78 67                	js     80105820 <sys_exec+0xa0>
801057b9:	8d 85 68 ff ff ff    	lea    -0x98(%ebp),%eax
801057bf:	83 ec 04             	sub    $0x4,%esp
801057c2:	8d b5 68 ff ff ff    	lea    -0x98(%ebp),%esi
801057c8:	68 80 00 00 00       	push   $0x80
801057cd:	6a 00                	push   $0x0
801057cf:	8d bd 64 ff ff ff    	lea    -0x9c(%ebp),%edi
801057d5:	50                   	push   %eax
801057d6:	31 db                	xor    %ebx,%ebx
801057d8:	e8 03 f1 ff ff       	call   801048e0 <memset>
801057dd:	83 c4 10             	add    $0x10,%esp
801057e0:	8b 85 60 ff ff ff    	mov    -0xa0(%ebp),%eax
801057e6:	83 ec 08             	sub    $0x8,%esp
801057e9:	57                   	push   %edi
801057ea:	8d 04 98             	lea    (%eax,%ebx,4),%eax
801057ed:	50                   	push   %eax
801057ee:	e8 5d f3 ff ff       	call   80104b50 <fetchint>
801057f3:	83 c4 10             	add    $0x10,%esp
801057f6:	85 c0                	test   %eax,%eax
801057f8:	78 26                	js     80105820 <sys_exec+0xa0>
801057fa:	8b 85 64 ff ff ff    	mov    -0x9c(%ebp),%eax
80105800:	85 c0                	test   %eax,%eax
80105802:	74 2c                	je     80105830 <sys_exec+0xb0>
80105804:	83 ec 08             	sub    $0x8,%esp
80105807:	56                   	push   %esi
80105808:	50                   	push   %eax
80105809:	e8 82 f3 ff ff       	call   80104b90 <fetchstr>
8010580e:	83 c4 10             	add    $0x10,%esp
80105811:	85 c0                	test   %eax,%eax
80105813:	78 0b                	js     80105820 <sys_exec+0xa0>
80105815:	83 c3 01             	add    $0x1,%ebx
80105818:	83 c6 04             	add    $0x4,%esi
8010581b:	83 fb 20             	cmp    $0x20,%ebx
8010581e:	75 c0                	jne    801057e0 <sys_exec+0x60>
80105820:	8d 65 f4             	lea    -0xc(%ebp),%esp
80105823:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80105828:	5b                   	pop    %ebx
80105829:	5e                   	pop    %esi
8010582a:	5f                   	pop    %edi
8010582b:	5d                   	pop    %ebp
8010582c:	c3                   	ret    
8010582d:	8d 76 00             	lea    0x0(%esi),%esi
80105830:	8d 85 68 ff ff ff    	lea    -0x98(%ebp),%eax
80105836:	83 ec 08             	sub    $0x8,%esp
80105839:	c7 84 9d 68 ff ff ff 	movl   $0x0,-0x98(%ebp,%ebx,4)
80105840:	00 00 00 00 
80105844:	50                   	push   %eax
80105845:	ff b5 5c ff ff ff    	pushl  -0xa4(%ebp)
8010584b:	e8 a0 b1 ff ff       	call   801009f0 <exec>
80105850:	83 c4 10             	add    $0x10,%esp
80105853:	8d 65 f4             	lea    -0xc(%ebp),%esp
80105856:	5b                   	pop    %ebx
80105857:	5e                   	pop    %esi
80105858:	5f                   	pop    %edi
80105859:	5d                   	pop    %ebp
8010585a:	c3                   	ret    
8010585b:	90                   	nop
8010585c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80105860 <sys_pipe>:
80105860:	55                   	push   %ebp
80105861:	89 e5                	mov    %esp,%ebp
80105863:	57                   	push   %edi
80105864:	56                   	push   %esi
80105865:	53                   	push   %ebx
80105866:	8d 45 dc             	lea    -0x24(%ebp),%eax
80105869:	83 ec 20             	sub    $0x20,%esp
8010586c:	6a 08                	push   $0x8
8010586e:	50                   	push   %eax
8010586f:	6a 00                	push   $0x0
80105871:	e8 ca f3 ff ff       	call   80104c40 <argptr>
80105876:	83 c4 10             	add    $0x10,%esp
80105879:	85 c0                	test   %eax,%eax
8010587b:	78 4a                	js     801058c7 <sys_pipe+0x67>
8010587d:	8d 45 e4             	lea    -0x1c(%ebp),%eax
80105880:	83 ec 08             	sub    $0x8,%esp
80105883:	50                   	push   %eax
80105884:	8d 45 e0             	lea    -0x20(%ebp),%eax
80105887:	50                   	push   %eax
80105888:	e8 63 d9 ff ff       	call   801031f0 <pipealloc>
8010588d:	83 c4 10             	add    $0x10,%esp
80105890:	85 c0                	test   %eax,%eax
80105892:	78 33                	js     801058c7 <sys_pipe+0x67>
80105894:	31 db                	xor    %ebx,%ebx
80105896:	8b 7d e0             	mov    -0x20(%ebp),%edi
80105899:	e8 d2 df ff ff       	call   80103870 <myproc>
8010589e:	66 90                	xchg   %ax,%ax
801058a0:	8b 74 98 28          	mov    0x28(%eax,%ebx,4),%esi
801058a4:	85 f6                	test   %esi,%esi
801058a6:	74 30                	je     801058d8 <sys_pipe+0x78>
801058a8:	83 c3 01             	add    $0x1,%ebx
801058ab:	83 fb 10             	cmp    $0x10,%ebx
801058ae:	75 f0                	jne    801058a0 <sys_pipe+0x40>
801058b0:	83 ec 0c             	sub    $0xc,%esp
801058b3:	ff 75 e0             	pushl  -0x20(%ebp)
801058b6:	e8 75 b5 ff ff       	call   80100e30 <fileclose>
801058bb:	58                   	pop    %eax
801058bc:	ff 75 e4             	pushl  -0x1c(%ebp)
801058bf:	e8 6c b5 ff ff       	call   80100e30 <fileclose>
801058c4:	83 c4 10             	add    $0x10,%esp
801058c7:	8d 65 f4             	lea    -0xc(%ebp),%esp
801058ca:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
801058cf:	5b                   	pop    %ebx
801058d0:	5e                   	pop    %esi
801058d1:	5f                   	pop    %edi
801058d2:	5d                   	pop    %ebp
801058d3:	c3                   	ret    
801058d4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
801058d8:	8d 73 08             	lea    0x8(%ebx),%esi
801058db:	89 7c b0 08          	mov    %edi,0x8(%eax,%esi,4)
801058df:	8b 7d e4             	mov    -0x1c(%ebp),%edi
801058e2:	e8 89 df ff ff       	call   80103870 <myproc>
801058e7:	31 d2                	xor    %edx,%edx
801058e9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801058f0:	8b 4c 90 28          	mov    0x28(%eax,%edx,4),%ecx
801058f4:	85 c9                	test   %ecx,%ecx
801058f6:	74 18                	je     80105910 <sys_pipe+0xb0>
801058f8:	83 c2 01             	add    $0x1,%edx
801058fb:	83 fa 10             	cmp    $0x10,%edx
801058fe:	75 f0                	jne    801058f0 <sys_pipe+0x90>
80105900:	e8 6b df ff ff       	call   80103870 <myproc>
80105905:	c7 44 b0 08 00 00 00 	movl   $0x0,0x8(%eax,%esi,4)
8010590c:	00 
8010590d:	eb a1                	jmp    801058b0 <sys_pipe+0x50>
8010590f:	90                   	nop
80105910:	89 7c 90 28          	mov    %edi,0x28(%eax,%edx,4)
80105914:	8b 45 dc             	mov    -0x24(%ebp),%eax
80105917:	89 18                	mov    %ebx,(%eax)
80105919:	8b 45 dc             	mov    -0x24(%ebp),%eax
8010591c:	89 50 04             	mov    %edx,0x4(%eax)
8010591f:	8d 65 f4             	lea    -0xc(%ebp),%esp
80105922:	31 c0                	xor    %eax,%eax
80105924:	5b                   	pop    %ebx
80105925:	5e                   	pop    %esi
80105926:	5f                   	pop    %edi
80105927:	5d                   	pop    %ebp
80105928:	c3                   	ret    
80105929:	66 90                	xchg   %ax,%ax
8010592b:	66 90                	xchg   %ax,%ax
8010592d:	66 90                	xchg   %ax,%ax
8010592f:	90                   	nop

80105930 <sys_fork>:
80105930:	55                   	push   %ebp
80105931:	89 e5                	mov    %esp,%ebp
80105933:	5d                   	pop    %ebp
80105934:	e9 d7 e0 ff ff       	jmp    80103a10 <fork>
80105939:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

80105940 <sys_exit>:
80105940:	55                   	push   %ebp
80105941:	89 e5                	mov    %esp,%ebp
80105943:	83 ec 08             	sub    $0x8,%esp
80105946:	e8 d5 e4 ff ff       	call   80103e20 <exit>
8010594b:	31 c0                	xor    %eax,%eax
8010594d:	c9                   	leave  
8010594e:	c3                   	ret    
8010594f:	90                   	nop

80105950 <sys_wait>:
80105950:	55                   	push   %ebp
80105951:	89 e5                	mov    %esp,%ebp
80105953:	5d                   	pop    %ebp
80105954:	e9 07 e7 ff ff       	jmp    80104060 <wait>
80105959:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

80105960 <sys_kill>:
80105960:	55                   	push   %ebp
80105961:	89 e5                	mov    %esp,%ebp
80105963:	83 ec 20             	sub    $0x20,%esp
80105966:	8d 45 f4             	lea    -0xc(%ebp),%eax
80105969:	50                   	push   %eax
8010596a:	6a 00                	push   $0x0
8010596c:	e8 7f f2 ff ff       	call   80104bf0 <argint>
80105971:	83 c4 10             	add    $0x10,%esp
80105974:	85 c0                	test   %eax,%eax
80105976:	78 18                	js     80105990 <sys_kill+0x30>
80105978:	83 ec 0c             	sub    $0xc,%esp
8010597b:	ff 75 f4             	pushl  -0xc(%ebp)
8010597e:	e8 3d e8 ff ff       	call   801041c0 <kill>
80105983:	83 c4 10             	add    $0x10,%esp
80105986:	c9                   	leave  
80105987:	c3                   	ret    
80105988:	90                   	nop
80105989:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80105990:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80105995:	c9                   	leave  
80105996:	c3                   	ret    
80105997:	89 f6                	mov    %esi,%esi
80105999:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801059a0 <sys_getpid>:
801059a0:	55                   	push   %ebp
801059a1:	89 e5                	mov    %esp,%ebp
801059a3:	83 ec 08             	sub    $0x8,%esp
801059a6:	e8 c5 de ff ff       	call   80103870 <myproc>
801059ab:	8b 40 10             	mov    0x10(%eax),%eax
801059ae:	c9                   	leave  
801059af:	c3                   	ret    

801059b0 <sys_sbrk>:
801059b0:	55                   	push   %ebp
801059b1:	89 e5                	mov    %esp,%ebp
801059b3:	53                   	push   %ebx
801059b4:	8d 45 f4             	lea    -0xc(%ebp),%eax
801059b7:	83 ec 1c             	sub    $0x1c,%esp
801059ba:	50                   	push   %eax
801059bb:	6a 00                	push   $0x0
801059bd:	e8 2e f2 ff ff       	call   80104bf0 <argint>
801059c2:	83 c4 10             	add    $0x10,%esp
801059c5:	85 c0                	test   %eax,%eax
801059c7:	78 27                	js     801059f0 <sys_sbrk+0x40>
801059c9:	e8 a2 de ff ff       	call   80103870 <myproc>
801059ce:	83 ec 0c             	sub    $0xc,%esp
801059d1:	8b 18                	mov    (%eax),%ebx
801059d3:	ff 75 f4             	pushl  -0xc(%ebp)
801059d6:	e8 b5 df ff ff       	call   80103990 <growproc>
801059db:	83 c4 10             	add    $0x10,%esp
801059de:	85 c0                	test   %eax,%eax
801059e0:	78 0e                	js     801059f0 <sys_sbrk+0x40>
801059e2:	89 d8                	mov    %ebx,%eax
801059e4:	8b 5d fc             	mov    -0x4(%ebp),%ebx
801059e7:	c9                   	leave  
801059e8:	c3                   	ret    
801059e9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801059f0:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
801059f5:	eb ed                	jmp    801059e4 <sys_sbrk+0x34>
801059f7:	89 f6                	mov    %esi,%esi
801059f9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80105a00 <sys_sleep>:
80105a00:	55                   	push   %ebp
80105a01:	89 e5                	mov    %esp,%ebp
80105a03:	53                   	push   %ebx
80105a04:	8d 45 f4             	lea    -0xc(%ebp),%eax
80105a07:	83 ec 1c             	sub    $0x1c,%esp
80105a0a:	50                   	push   %eax
80105a0b:	6a 00                	push   $0x0
80105a0d:	e8 de f1 ff ff       	call   80104bf0 <argint>
80105a12:	83 c4 10             	add    $0x10,%esp
80105a15:	85 c0                	test   %eax,%eax
80105a17:	0f 88 8a 00 00 00    	js     80105aa7 <sys_sleep+0xa7>
80105a1d:	83 ec 0c             	sub    $0xc,%esp
80105a20:	68 60 53 11 80       	push   $0x80115360
80105a25:	e8 46 ed ff ff       	call   80104770 <acquire>
80105a2a:	8b 55 f4             	mov    -0xc(%ebp),%edx
80105a2d:	83 c4 10             	add    $0x10,%esp
80105a30:	8b 1d a0 5b 11 80    	mov    0x80115ba0,%ebx
80105a36:	85 d2                	test   %edx,%edx
80105a38:	75 27                	jne    80105a61 <sys_sleep+0x61>
80105a3a:	eb 54                	jmp    80105a90 <sys_sleep+0x90>
80105a3c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80105a40:	83 ec 08             	sub    $0x8,%esp
80105a43:	68 60 53 11 80       	push   $0x80115360
80105a48:	68 a0 5b 11 80       	push   $0x80115ba0
80105a4d:	e8 4e e5 ff ff       	call   80103fa0 <sleep>
80105a52:	a1 a0 5b 11 80       	mov    0x80115ba0,%eax
80105a57:	83 c4 10             	add    $0x10,%esp
80105a5a:	29 d8                	sub    %ebx,%eax
80105a5c:	3b 45 f4             	cmp    -0xc(%ebp),%eax
80105a5f:	73 2f                	jae    80105a90 <sys_sleep+0x90>
80105a61:	e8 0a de ff ff       	call   80103870 <myproc>
80105a66:	8b 40 24             	mov    0x24(%eax),%eax
80105a69:	85 c0                	test   %eax,%eax
80105a6b:	74 d3                	je     80105a40 <sys_sleep+0x40>
80105a6d:	83 ec 0c             	sub    $0xc,%esp
80105a70:	68 60 53 11 80       	push   $0x80115360
80105a75:	e8 16 ee ff ff       	call   80104890 <release>
80105a7a:	83 c4 10             	add    $0x10,%esp
80105a7d:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80105a82:	8b 5d fc             	mov    -0x4(%ebp),%ebx
80105a85:	c9                   	leave  
80105a86:	c3                   	ret    
80105a87:	89 f6                	mov    %esi,%esi
80105a89:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80105a90:	83 ec 0c             	sub    $0xc,%esp
80105a93:	68 60 53 11 80       	push   $0x80115360
80105a98:	e8 f3 ed ff ff       	call   80104890 <release>
80105a9d:	83 c4 10             	add    $0x10,%esp
80105aa0:	31 c0                	xor    %eax,%eax
80105aa2:	8b 5d fc             	mov    -0x4(%ebp),%ebx
80105aa5:	c9                   	leave  
80105aa6:	c3                   	ret    
80105aa7:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80105aac:	eb d4                	jmp    80105a82 <sys_sleep+0x82>
80105aae:	66 90                	xchg   %ax,%ax

80105ab0 <sys_uptime>:
80105ab0:	55                   	push   %ebp
80105ab1:	89 e5                	mov    %esp,%ebp
80105ab3:	53                   	push   %ebx
80105ab4:	83 ec 10             	sub    $0x10,%esp
80105ab7:	68 60 53 11 80       	push   $0x80115360
80105abc:	e8 af ec ff ff       	call   80104770 <acquire>
80105ac1:	8b 1d a0 5b 11 80    	mov    0x80115ba0,%ebx
80105ac7:	c7 04 24 60 53 11 80 	movl   $0x80115360,(%esp)
80105ace:	e8 bd ed ff ff       	call   80104890 <release>
80105ad3:	89 d8                	mov    %ebx,%eax
80105ad5:	8b 5d fc             	mov    -0x4(%ebp),%ebx
80105ad8:	c9                   	leave  
80105ad9:	c3                   	ret    
80105ada:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

80105ae0 <sys_cps>:
80105ae0:	55                   	push   %ebp
80105ae1:	89 e5                	mov    %esp,%ebp
80105ae3:	5d                   	pop    %ebp
80105ae4:	e9 27 e8 ff ff       	jmp    80104310 <cps>
80105ae9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

80105af0 <sys_chpr>:
80105af0:	55                   	push   %ebp
80105af1:	89 e5                	mov    %esp,%ebp
80105af3:	83 ec 20             	sub    $0x20,%esp
80105af6:	8d 45 f0             	lea    -0x10(%ebp),%eax
80105af9:	50                   	push   %eax
80105afa:	6a 00                	push   $0x0
80105afc:	e8 ef f0 ff ff       	call   80104bf0 <argint>
80105b01:	83 c4 10             	add    $0x10,%esp
80105b04:	85 c0                	test   %eax,%eax
80105b06:	78 28                	js     80105b30 <sys_chpr+0x40>
80105b08:	8d 45 f4             	lea    -0xc(%ebp),%eax
80105b0b:	83 ec 08             	sub    $0x8,%esp
80105b0e:	50                   	push   %eax
80105b0f:	6a 01                	push   $0x1
80105b11:	e8 da f0 ff ff       	call   80104bf0 <argint>
80105b16:	83 c4 10             	add    $0x10,%esp
80105b19:	85 c0                	test   %eax,%eax
80105b1b:	78 13                	js     80105b30 <sys_chpr+0x40>
80105b1d:	83 ec 08             	sub    $0x8,%esp
80105b20:	ff 75 f4             	pushl  -0xc(%ebp)
80105b23:	ff 75 f0             	pushl  -0x10(%ebp)
80105b26:	e8 e5 e8 ff ff       	call   80104410 <chpr>
80105b2b:	83 c4 10             	add    $0x10,%esp
80105b2e:	c9                   	leave  
80105b2f:	c3                   	ret    
80105b30:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80105b35:	c9                   	leave  
80105b36:	c3                   	ret    
80105b37:	89 f6                	mov    %esi,%esi
80105b39:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80105b40 <sys_chdl>:
80105b40:	55                   	push   %ebp
80105b41:	89 e5                	mov    %esp,%ebp
80105b43:	83 ec 20             	sub    $0x20,%esp
80105b46:	8d 45 f4             	lea    -0xc(%ebp),%eax
80105b49:	50                   	push   %eax
80105b4a:	6a 00                	push   $0x0
80105b4c:	e8 9f f0 ff ff       	call   80104bf0 <argint>
80105b51:	83 c4 10             	add    $0x10,%esp
80105b54:	85 c0                	test   %eax,%eax
80105b56:	78 28                	js     80105b80 <sys_chdl+0x40>
80105b58:	8d 45 f0             	lea    -0x10(%ebp),%eax
80105b5b:	83 ec 08             	sub    $0x8,%esp
80105b5e:	50                   	push   %eax
80105b5f:	6a 01                	push   $0x1
80105b61:	e8 8a f0 ff ff       	call   80104bf0 <argint>
80105b66:	83 c4 10             	add    $0x10,%esp
80105b69:	85 c0                	test   %eax,%eax
80105b6b:	78 13                	js     80105b80 <sys_chdl+0x40>
80105b6d:	83 ec 08             	sub    $0x8,%esp
80105b70:	ff 75 f0             	pushl  -0x10(%ebp)
80105b73:	ff 75 f4             	pushl  -0xc(%ebp)
80105b76:	e8 e5 e8 ff ff       	call   80104460 <chdl>
80105b7b:	83 c4 10             	add    $0x10,%esp
80105b7e:	c9                   	leave  
80105b7f:	c3                   	ret    
80105b80:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80105b85:	c9                   	leave  
80105b86:	c3                   	ret    

80105b87 <alltraps>:

  # vectors.S sends all traps here.
.globl alltraps
alltraps:
  # Build trap frame.
  pushl %ds
80105b87:	1e                   	push   %ds
  pushl %es
80105b88:	06                   	push   %es
  pushl %fs
80105b89:	0f a0                	push   %fs
  pushl %gs
80105b8b:	0f a8                	push   %gs
  pushal
80105b8d:	60                   	pusha  
  
  # Set up data segments.
  movw $(SEG_KDATA<<3), %ax
80105b8e:	66 b8 10 00          	mov    $0x10,%ax
  movw %ax, %ds
80105b92:	8e d8                	mov    %eax,%ds
  movw %ax, %es
80105b94:	8e c0                	mov    %eax,%es

  # Call trap(tf), where tf=%esp
  pushl %esp
80105b96:	54                   	push   %esp
  call trap
80105b97:	e8 e4 00 00 00       	call   80105c80 <trap>
  addl $4, %esp
80105b9c:	83 c4 04             	add    $0x4,%esp

80105b9f <trapret>:

  # Return falls through to trapret...
.globl trapret
trapret:
  popal
80105b9f:	61                   	popa   
  popl %gs
80105ba0:	0f a9                	pop    %gs
  popl %fs
80105ba2:	0f a1                	pop    %fs
  popl %es
80105ba4:	07                   	pop    %es
  popl %ds
80105ba5:	1f                   	pop    %ds
  addl $0x8, %esp  # trapno and errcode
80105ba6:	83 c4 08             	add    $0x8,%esp
  iret
80105ba9:	cf                   	iret   
80105baa:	66 90                	xchg   %ax,%ax
80105bac:	66 90                	xchg   %ax,%ax
80105bae:	66 90                	xchg   %ax,%ax

80105bb0 <tvinit>:
80105bb0:	31 c0                	xor    %eax,%eax
80105bb2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80105bb8:	8b 14 85 0c a0 10 80 	mov    -0x7fef5ff4(,%eax,4),%edx
80105bbf:	b9 08 00 00 00       	mov    $0x8,%ecx
80105bc4:	c6 04 c5 a4 53 11 80 	movb   $0x0,-0x7feeac5c(,%eax,8)
80105bcb:	00 
80105bcc:	66 89 0c c5 a2 53 11 	mov    %cx,-0x7feeac5e(,%eax,8)
80105bd3:	80 
80105bd4:	c6 04 c5 a5 53 11 80 	movb   $0x8e,-0x7feeac5b(,%eax,8)
80105bdb:	8e 
80105bdc:	66 89 14 c5 a0 53 11 	mov    %dx,-0x7feeac60(,%eax,8)
80105be3:	80 
80105be4:	c1 ea 10             	shr    $0x10,%edx
80105be7:	66 89 14 c5 a6 53 11 	mov    %dx,-0x7feeac5a(,%eax,8)
80105bee:	80 
80105bef:	83 c0 01             	add    $0x1,%eax
80105bf2:	3d 00 01 00 00       	cmp    $0x100,%eax
80105bf7:	75 bf                	jne    80105bb8 <tvinit+0x8>
80105bf9:	55                   	push   %ebp
80105bfa:	ba 08 00 00 00       	mov    $0x8,%edx
80105bff:	89 e5                	mov    %esp,%ebp
80105c01:	83 ec 10             	sub    $0x10,%esp
80105c04:	a1 0c a1 10 80       	mov    0x8010a10c,%eax
80105c09:	68 e5 7c 10 80       	push   $0x80107ce5
80105c0e:	68 60 53 11 80       	push   $0x80115360
80105c13:	66 89 15 a2 55 11 80 	mov    %dx,0x801155a2
80105c1a:	c6 05 a4 55 11 80 00 	movb   $0x0,0x801155a4
80105c21:	66 a3 a0 55 11 80    	mov    %ax,0x801155a0
80105c27:	c1 e8 10             	shr    $0x10,%eax
80105c2a:	c6 05 a5 55 11 80 ef 	movb   $0xef,0x801155a5
80105c31:	66 a3 a6 55 11 80    	mov    %ax,0x801155a6
80105c37:	e8 34 ea ff ff       	call   80104670 <initlock>
80105c3c:	83 c4 10             	add    $0x10,%esp
80105c3f:	c9                   	leave  
80105c40:	c3                   	ret    
80105c41:	eb 0d                	jmp    80105c50 <idtinit>
80105c43:	90                   	nop
80105c44:	90                   	nop
80105c45:	90                   	nop
80105c46:	90                   	nop
80105c47:	90                   	nop
80105c48:	90                   	nop
80105c49:	90                   	nop
80105c4a:	90                   	nop
80105c4b:	90                   	nop
80105c4c:	90                   	nop
80105c4d:	90                   	nop
80105c4e:	90                   	nop
80105c4f:	90                   	nop

80105c50 <idtinit>:
80105c50:	55                   	push   %ebp
80105c51:	b8 ff 07 00 00       	mov    $0x7ff,%eax
80105c56:	89 e5                	mov    %esp,%ebp
80105c58:	83 ec 10             	sub    $0x10,%esp
80105c5b:	66 89 45 fa          	mov    %ax,-0x6(%ebp)
80105c5f:	b8 a0 53 11 80       	mov    $0x801153a0,%eax
80105c64:	66 89 45 fc          	mov    %ax,-0x4(%ebp)
80105c68:	c1 e8 10             	shr    $0x10,%eax
80105c6b:	66 89 45 fe          	mov    %ax,-0x2(%ebp)
80105c6f:	8d 45 fa             	lea    -0x6(%ebp),%eax
80105c72:	0f 01 18             	lidtl  (%eax)
80105c75:	c9                   	leave  
80105c76:	c3                   	ret    
80105c77:	89 f6                	mov    %esi,%esi
80105c79:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80105c80 <trap>:
80105c80:	55                   	push   %ebp
80105c81:	89 e5                	mov    %esp,%ebp
80105c83:	57                   	push   %edi
80105c84:	56                   	push   %esi
80105c85:	53                   	push   %ebx
80105c86:	83 ec 1c             	sub    $0x1c,%esp
80105c89:	8b 7d 08             	mov    0x8(%ebp),%edi
80105c8c:	8b 47 30             	mov    0x30(%edi),%eax
80105c8f:	83 f8 40             	cmp    $0x40,%eax
80105c92:	0f 84 28 01 00 00    	je     80105dc0 <trap+0x140>
80105c98:	83 e8 20             	sub    $0x20,%eax
80105c9b:	83 f8 1f             	cmp    $0x1f,%eax
80105c9e:	0f 87 5c 01 00 00    	ja     80105e00 <trap+0x180>
80105ca4:	ff 24 85 8c 7d 10 80 	jmp    *-0x7fef8274(,%eax,4)
80105cab:	90                   	nop
80105cac:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80105cb0:	e8 9b db ff ff       	call   80103850 <cpuid>
80105cb5:	85 c0                	test   %eax,%eax
80105cb7:	0f 84 cb 01 00 00    	je     80105e88 <trap+0x208>
80105cbd:	e8 ae db ff ff       	call   80103870 <myproc>
80105cc2:	85 c0                	test   %eax,%eax
80105cc4:	74 11                	je     80105cd7 <trap+0x57>
80105cc6:	0f b7 47 3c          	movzwl 0x3c(%edi),%eax
80105cca:	83 e0 03             	and    $0x3,%eax
80105ccd:	66 83 f8 03          	cmp    $0x3,%ax
80105cd1:	0f 84 e9 01 00 00    	je     80105ec0 <trap+0x240>
80105cd7:	e8 34 ca ff ff       	call   80102710 <lapiceoi>
80105cdc:	e8 8f db ff ff       	call   80103870 <myproc>
80105ce1:	85 c0                	test   %eax,%eax
80105ce3:	74 0c                	je     80105cf1 <trap+0x71>
80105ce5:	e8 86 db ff ff       	call   80103870 <myproc>
80105cea:	8b 50 24             	mov    0x24(%eax),%edx
80105ced:	85 d2                	test   %edx,%edx
80105cef:	75 47                	jne    80105d38 <trap+0xb8>
80105cf1:	e8 7a db ff ff       	call   80103870 <myproc>
80105cf6:	85 c0                	test   %eax,%eax
80105cf8:	74 0b                	je     80105d05 <trap+0x85>
80105cfa:	e8 71 db ff ff       	call   80103870 <myproc>
80105cff:	83 78 0c 04          	cmpl   $0x4,0xc(%eax)
80105d03:	74 4b                	je     80105d50 <trap+0xd0>
80105d05:	e8 66 db ff ff       	call   80103870 <myproc>
80105d0a:	85 c0                	test   %eax,%eax
80105d0c:	74 1d                	je     80105d2b <trap+0xab>
80105d0e:	e8 5d db ff ff       	call   80103870 <myproc>
80105d13:	8b 40 24             	mov    0x24(%eax),%eax
80105d16:	85 c0                	test   %eax,%eax
80105d18:	74 11                	je     80105d2b <trap+0xab>
80105d1a:	0f b7 47 3c          	movzwl 0x3c(%edi),%eax
80105d1e:	83 e0 03             	and    $0x3,%eax
80105d21:	66 83 f8 03          	cmp    $0x3,%ax
80105d25:	0f 84 c2 00 00 00    	je     80105ded <trap+0x16d>
80105d2b:	8d 65 f4             	lea    -0xc(%ebp),%esp
80105d2e:	5b                   	pop    %ebx
80105d2f:	5e                   	pop    %esi
80105d30:	5f                   	pop    %edi
80105d31:	5d                   	pop    %ebp
80105d32:	c3                   	ret    
80105d33:	90                   	nop
80105d34:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80105d38:	0f b7 47 3c          	movzwl 0x3c(%edi),%eax
80105d3c:	83 e0 03             	and    $0x3,%eax
80105d3f:	66 83 f8 03          	cmp    $0x3,%ax
80105d43:	75 ac                	jne    80105cf1 <trap+0x71>
80105d45:	e8 d6 e0 ff ff       	call   80103e20 <exit>
80105d4a:	eb a5                	jmp    80105cf1 <trap+0x71>
80105d4c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80105d50:	83 7f 30 20          	cmpl   $0x20,0x30(%edi)
80105d54:	75 af                	jne    80105d05 <trap+0x85>
80105d56:	e8 f5 e1 ff ff       	call   80103f50 <yield>
80105d5b:	eb a8                	jmp    80105d05 <trap+0x85>
80105d5d:	8d 76 00             	lea    0x0(%esi),%esi
80105d60:	e8 6b c8 ff ff       	call   801025d0 <kbdintr>
80105d65:	e8 a6 c9 ff ff       	call   80102710 <lapiceoi>
80105d6a:	e9 6d ff ff ff       	jmp    80105cdc <trap+0x5c>
80105d6f:	90                   	nop
80105d70:	e8 fb 02 00 00       	call   80106070 <uartintr>
80105d75:	e8 96 c9 ff ff       	call   80102710 <lapiceoi>
80105d7a:	e9 5d ff ff ff       	jmp    80105cdc <trap+0x5c>
80105d7f:	90                   	nop
80105d80:	0f b7 5f 3c          	movzwl 0x3c(%edi),%ebx
80105d84:	8b 77 38             	mov    0x38(%edi),%esi
80105d87:	e8 c4 da ff ff       	call   80103850 <cpuid>
80105d8c:	56                   	push   %esi
80105d8d:	53                   	push   %ebx
80105d8e:	50                   	push   %eax
80105d8f:	68 f0 7c 10 80       	push   $0x80107cf0
80105d94:	e8 c7 a8 ff ff       	call   80100660 <cprintf>
80105d99:	e8 72 c9 ff ff       	call   80102710 <lapiceoi>
80105d9e:	83 c4 10             	add    $0x10,%esp
80105da1:	e9 36 ff ff ff       	jmp    80105cdc <trap+0x5c>
80105da6:	8d 76 00             	lea    0x0(%esi),%esi
80105da9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80105db0:	e8 9b c2 ff ff       	call   80102050 <ideintr>
80105db5:	e9 1d ff ff ff       	jmp    80105cd7 <trap+0x57>
80105dba:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80105dc0:	e8 ab da ff ff       	call   80103870 <myproc>
80105dc5:	8b 58 24             	mov    0x24(%eax),%ebx
80105dc8:	85 db                	test   %ebx,%ebx
80105dca:	0f 85 a8 00 00 00    	jne    80105e78 <trap+0x1f8>
80105dd0:	e8 9b da ff ff       	call   80103870 <myproc>
80105dd5:	89 78 18             	mov    %edi,0x18(%eax)
80105dd8:	e8 03 ef ff ff       	call   80104ce0 <syscall>
80105ddd:	e8 8e da ff ff       	call   80103870 <myproc>
80105de2:	8b 48 24             	mov    0x24(%eax),%ecx
80105de5:	85 c9                	test   %ecx,%ecx
80105de7:	0f 84 3e ff ff ff    	je     80105d2b <trap+0xab>
80105ded:	8d 65 f4             	lea    -0xc(%ebp),%esp
80105df0:	5b                   	pop    %ebx
80105df1:	5e                   	pop    %esi
80105df2:	5f                   	pop    %edi
80105df3:	5d                   	pop    %ebp
80105df4:	e9 27 e0 ff ff       	jmp    80103e20 <exit>
80105df9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80105e00:	e8 6b da ff ff       	call   80103870 <myproc>
80105e05:	85 c0                	test   %eax,%eax
80105e07:	0f 84 c4 00 00 00    	je     80105ed1 <trap+0x251>
80105e0d:	f6 47 3c 03          	testb  $0x3,0x3c(%edi)
80105e11:	0f 84 ba 00 00 00    	je     80105ed1 <trap+0x251>
80105e17:	0f 20 d1             	mov    %cr2,%ecx
80105e1a:	8b 57 38             	mov    0x38(%edi),%edx
80105e1d:	89 4d d8             	mov    %ecx,-0x28(%ebp)
80105e20:	89 55 dc             	mov    %edx,-0x24(%ebp)
80105e23:	e8 28 da ff ff       	call   80103850 <cpuid>
80105e28:	8b 77 34             	mov    0x34(%edi),%esi
80105e2b:	8b 5f 30             	mov    0x30(%edi),%ebx
80105e2e:	89 45 e4             	mov    %eax,-0x1c(%ebp)
80105e31:	e8 3a da ff ff       	call   80103870 <myproc>
80105e36:	89 45 e0             	mov    %eax,-0x20(%ebp)
80105e39:	e8 32 da ff ff       	call   80103870 <myproc>
80105e3e:	8b 4d d8             	mov    -0x28(%ebp),%ecx
80105e41:	8b 55 dc             	mov    -0x24(%ebp),%edx
80105e44:	51                   	push   %ecx
80105e45:	52                   	push   %edx
80105e46:	8b 55 e0             	mov    -0x20(%ebp),%edx
80105e49:	ff 75 e4             	pushl  -0x1c(%ebp)
80105e4c:	56                   	push   %esi
80105e4d:	53                   	push   %ebx
80105e4e:	83 c2 6c             	add    $0x6c,%edx
80105e51:	52                   	push   %edx
80105e52:	ff 70 10             	pushl  0x10(%eax)
80105e55:	68 48 7d 10 80       	push   $0x80107d48
80105e5a:	e8 01 a8 ff ff       	call   80100660 <cprintf>
80105e5f:	83 c4 20             	add    $0x20,%esp
80105e62:	e8 09 da ff ff       	call   80103870 <myproc>
80105e67:	c7 40 24 01 00 00 00 	movl   $0x1,0x24(%eax)
80105e6e:	e9 69 fe ff ff       	jmp    80105cdc <trap+0x5c>
80105e73:	90                   	nop
80105e74:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80105e78:	e8 a3 df ff ff       	call   80103e20 <exit>
80105e7d:	e9 4e ff ff ff       	jmp    80105dd0 <trap+0x150>
80105e82:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80105e88:	83 ec 0c             	sub    $0xc,%esp
80105e8b:	68 60 53 11 80       	push   $0x80115360
80105e90:	e8 db e8 ff ff       	call   80104770 <acquire>
80105e95:	c7 04 24 a0 5b 11 80 	movl   $0x80115ba0,(%esp)
80105e9c:	83 05 a0 5b 11 80 01 	addl   $0x1,0x80115ba0
80105ea3:	e8 b8 e2 ff ff       	call   80104160 <wakeup>
80105ea8:	c7 04 24 60 53 11 80 	movl   $0x80115360,(%esp)
80105eaf:	e8 dc e9 ff ff       	call   80104890 <release>
80105eb4:	83 c4 10             	add    $0x10,%esp
80105eb7:	e9 01 fe ff ff       	jmp    80105cbd <trap+0x3d>
80105ebc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80105ec0:	e8 ab d9 ff ff       	call   80103870 <myproc>
80105ec5:	83 80 94 00 00 00 01 	addl   $0x1,0x94(%eax)
80105ecc:	e9 06 fe ff ff       	jmp    80105cd7 <trap+0x57>
80105ed1:	0f 20 d6             	mov    %cr2,%esi
80105ed4:	8b 5f 38             	mov    0x38(%edi),%ebx
80105ed7:	e8 74 d9 ff ff       	call   80103850 <cpuid>
80105edc:	83 ec 0c             	sub    $0xc,%esp
80105edf:	56                   	push   %esi
80105ee0:	53                   	push   %ebx
80105ee1:	50                   	push   %eax
80105ee2:	ff 77 30             	pushl  0x30(%edi)
80105ee5:	68 14 7d 10 80       	push   $0x80107d14
80105eea:	e8 71 a7 ff ff       	call   80100660 <cprintf>
80105eef:	83 c4 14             	add    $0x14,%esp
80105ef2:	68 ea 7c 10 80       	push   $0x80107cea
80105ef7:	e8 74 a4 ff ff       	call   80100370 <panic>
80105efc:	66 90                	xchg   %ax,%ax
80105efe:	66 90                	xchg   %ax,%ax

80105f00 <uartgetc>:
80105f00:	a1 bc a5 10 80       	mov    0x8010a5bc,%eax
80105f05:	55                   	push   %ebp
80105f06:	89 e5                	mov    %esp,%ebp
80105f08:	85 c0                	test   %eax,%eax
80105f0a:	74 1c                	je     80105f28 <uartgetc+0x28>
80105f0c:	ba fd 03 00 00       	mov    $0x3fd,%edx
80105f11:	ec                   	in     (%dx),%al
80105f12:	a8 01                	test   $0x1,%al
80105f14:	74 12                	je     80105f28 <uartgetc+0x28>
80105f16:	ba f8 03 00 00       	mov    $0x3f8,%edx
80105f1b:	ec                   	in     (%dx),%al
80105f1c:	0f b6 c0             	movzbl %al,%eax
80105f1f:	5d                   	pop    %ebp
80105f20:	c3                   	ret    
80105f21:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80105f28:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80105f2d:	5d                   	pop    %ebp
80105f2e:	c3                   	ret    
80105f2f:	90                   	nop

80105f30 <uartputc.part.0>:
80105f30:	55                   	push   %ebp
80105f31:	89 e5                	mov    %esp,%ebp
80105f33:	57                   	push   %edi
80105f34:	56                   	push   %esi
80105f35:	53                   	push   %ebx
80105f36:	89 c7                	mov    %eax,%edi
80105f38:	bb 80 00 00 00       	mov    $0x80,%ebx
80105f3d:	be fd 03 00 00       	mov    $0x3fd,%esi
80105f42:	83 ec 0c             	sub    $0xc,%esp
80105f45:	eb 1b                	jmp    80105f62 <uartputc.part.0+0x32>
80105f47:	89 f6                	mov    %esi,%esi
80105f49:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80105f50:	83 ec 0c             	sub    $0xc,%esp
80105f53:	6a 0a                	push   $0xa
80105f55:	e8 d6 c7 ff ff       	call   80102730 <microdelay>
80105f5a:	83 c4 10             	add    $0x10,%esp
80105f5d:	83 eb 01             	sub    $0x1,%ebx
80105f60:	74 07                	je     80105f69 <uartputc.part.0+0x39>
80105f62:	89 f2                	mov    %esi,%edx
80105f64:	ec                   	in     (%dx),%al
80105f65:	a8 20                	test   $0x20,%al
80105f67:	74 e7                	je     80105f50 <uartputc.part.0+0x20>
80105f69:	ba f8 03 00 00       	mov    $0x3f8,%edx
80105f6e:	89 f8                	mov    %edi,%eax
80105f70:	ee                   	out    %al,(%dx)
80105f71:	8d 65 f4             	lea    -0xc(%ebp),%esp
80105f74:	5b                   	pop    %ebx
80105f75:	5e                   	pop    %esi
80105f76:	5f                   	pop    %edi
80105f77:	5d                   	pop    %ebp
80105f78:	c3                   	ret    
80105f79:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

80105f80 <uartinit>:
80105f80:	55                   	push   %ebp
80105f81:	31 c9                	xor    %ecx,%ecx
80105f83:	89 c8                	mov    %ecx,%eax
80105f85:	89 e5                	mov    %esp,%ebp
80105f87:	57                   	push   %edi
80105f88:	56                   	push   %esi
80105f89:	53                   	push   %ebx
80105f8a:	bb fa 03 00 00       	mov    $0x3fa,%ebx
80105f8f:	89 da                	mov    %ebx,%edx
80105f91:	83 ec 0c             	sub    $0xc,%esp
80105f94:	ee                   	out    %al,(%dx)
80105f95:	bf fb 03 00 00       	mov    $0x3fb,%edi
80105f9a:	b8 80 ff ff ff       	mov    $0xffffff80,%eax
80105f9f:	89 fa                	mov    %edi,%edx
80105fa1:	ee                   	out    %al,(%dx)
80105fa2:	b8 0c 00 00 00       	mov    $0xc,%eax
80105fa7:	ba f8 03 00 00       	mov    $0x3f8,%edx
80105fac:	ee                   	out    %al,(%dx)
80105fad:	be f9 03 00 00       	mov    $0x3f9,%esi
80105fb2:	89 c8                	mov    %ecx,%eax
80105fb4:	89 f2                	mov    %esi,%edx
80105fb6:	ee                   	out    %al,(%dx)
80105fb7:	b8 03 00 00 00       	mov    $0x3,%eax
80105fbc:	89 fa                	mov    %edi,%edx
80105fbe:	ee                   	out    %al,(%dx)
80105fbf:	ba fc 03 00 00       	mov    $0x3fc,%edx
80105fc4:	89 c8                	mov    %ecx,%eax
80105fc6:	ee                   	out    %al,(%dx)
80105fc7:	b8 01 00 00 00       	mov    $0x1,%eax
80105fcc:	89 f2                	mov    %esi,%edx
80105fce:	ee                   	out    %al,(%dx)
80105fcf:	ba fd 03 00 00       	mov    $0x3fd,%edx
80105fd4:	ec                   	in     (%dx),%al
80105fd5:	3c ff                	cmp    $0xff,%al
80105fd7:	74 5a                	je     80106033 <uartinit+0xb3>
80105fd9:	c7 05 bc a5 10 80 01 	movl   $0x1,0x8010a5bc
80105fe0:	00 00 00 
80105fe3:	89 da                	mov    %ebx,%edx
80105fe5:	ec                   	in     (%dx),%al
80105fe6:	ba f8 03 00 00       	mov    $0x3f8,%edx
80105feb:	ec                   	in     (%dx),%al
80105fec:	83 ec 08             	sub    $0x8,%esp
80105fef:	bb 0c 7e 10 80       	mov    $0x80107e0c,%ebx
80105ff4:	6a 00                	push   $0x0
80105ff6:	6a 04                	push   $0x4
80105ff8:	e8 a3 c2 ff ff       	call   801022a0 <ioapicenable>
80105ffd:	83 c4 10             	add    $0x10,%esp
80106000:	b8 78 00 00 00       	mov    $0x78,%eax
80106005:	eb 13                	jmp    8010601a <uartinit+0x9a>
80106007:	89 f6                	mov    %esi,%esi
80106009:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80106010:	83 c3 01             	add    $0x1,%ebx
80106013:	0f be 03             	movsbl (%ebx),%eax
80106016:	84 c0                	test   %al,%al
80106018:	74 19                	je     80106033 <uartinit+0xb3>
8010601a:	8b 15 bc a5 10 80    	mov    0x8010a5bc,%edx
80106020:	85 d2                	test   %edx,%edx
80106022:	74 ec                	je     80106010 <uartinit+0x90>
80106024:	83 c3 01             	add    $0x1,%ebx
80106027:	e8 04 ff ff ff       	call   80105f30 <uartputc.part.0>
8010602c:	0f be 03             	movsbl (%ebx),%eax
8010602f:	84 c0                	test   %al,%al
80106031:	75 e7                	jne    8010601a <uartinit+0x9a>
80106033:	8d 65 f4             	lea    -0xc(%ebp),%esp
80106036:	5b                   	pop    %ebx
80106037:	5e                   	pop    %esi
80106038:	5f                   	pop    %edi
80106039:	5d                   	pop    %ebp
8010603a:	c3                   	ret    
8010603b:	90                   	nop
8010603c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80106040 <uartputc>:
80106040:	8b 15 bc a5 10 80    	mov    0x8010a5bc,%edx
80106046:	55                   	push   %ebp
80106047:	89 e5                	mov    %esp,%ebp
80106049:	85 d2                	test   %edx,%edx
8010604b:	8b 45 08             	mov    0x8(%ebp),%eax
8010604e:	74 10                	je     80106060 <uartputc+0x20>
80106050:	5d                   	pop    %ebp
80106051:	e9 da fe ff ff       	jmp    80105f30 <uartputc.part.0>
80106056:	8d 76 00             	lea    0x0(%esi),%esi
80106059:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80106060:	5d                   	pop    %ebp
80106061:	c3                   	ret    
80106062:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80106069:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80106070 <uartintr>:
80106070:	55                   	push   %ebp
80106071:	89 e5                	mov    %esp,%ebp
80106073:	83 ec 14             	sub    $0x14,%esp
80106076:	68 00 5f 10 80       	push   $0x80105f00
8010607b:	e8 70 a7 ff ff       	call   801007f0 <consoleintr>
80106080:	83 c4 10             	add    $0x10,%esp
80106083:	c9                   	leave  
80106084:	c3                   	ret    

80106085 <vector0>:
# generated by vectors.pl - do not edit
# handlers
.globl alltraps
.globl vector0
vector0:
  pushl $0
80106085:	6a 00                	push   $0x0
  pushl $0
80106087:	6a 00                	push   $0x0
  jmp alltraps
80106089:	e9 f9 fa ff ff       	jmp    80105b87 <alltraps>

8010608e <vector1>:
.globl vector1
vector1:
  pushl $0
8010608e:	6a 00                	push   $0x0
  pushl $1
80106090:	6a 01                	push   $0x1
  jmp alltraps
80106092:	e9 f0 fa ff ff       	jmp    80105b87 <alltraps>

80106097 <vector2>:
.globl vector2
vector2:
  pushl $0
80106097:	6a 00                	push   $0x0
  pushl $2
80106099:	6a 02                	push   $0x2
  jmp alltraps
8010609b:	e9 e7 fa ff ff       	jmp    80105b87 <alltraps>

801060a0 <vector3>:
.globl vector3
vector3:
  pushl $0
801060a0:	6a 00                	push   $0x0
  pushl $3
801060a2:	6a 03                	push   $0x3
  jmp alltraps
801060a4:	e9 de fa ff ff       	jmp    80105b87 <alltraps>

801060a9 <vector4>:
.globl vector4
vector4:
  pushl $0
801060a9:	6a 00                	push   $0x0
  pushl $4
801060ab:	6a 04                	push   $0x4
  jmp alltraps
801060ad:	e9 d5 fa ff ff       	jmp    80105b87 <alltraps>

801060b2 <vector5>:
.globl vector5
vector5:
  pushl $0
801060b2:	6a 00                	push   $0x0
  pushl $5
801060b4:	6a 05                	push   $0x5
  jmp alltraps
801060b6:	e9 cc fa ff ff       	jmp    80105b87 <alltraps>

801060bb <vector6>:
.globl vector6
vector6:
  pushl $0
801060bb:	6a 00                	push   $0x0
  pushl $6
801060bd:	6a 06                	push   $0x6
  jmp alltraps
801060bf:	e9 c3 fa ff ff       	jmp    80105b87 <alltraps>

801060c4 <vector7>:
.globl vector7
vector7:
  pushl $0
801060c4:	6a 00                	push   $0x0
  pushl $7
801060c6:	6a 07                	push   $0x7
  jmp alltraps
801060c8:	e9 ba fa ff ff       	jmp    80105b87 <alltraps>

801060cd <vector8>:
.globl vector8
vector8:
  pushl $8
801060cd:	6a 08                	push   $0x8
  jmp alltraps
801060cf:	e9 b3 fa ff ff       	jmp    80105b87 <alltraps>

801060d4 <vector9>:
.globl vector9
vector9:
  pushl $0
801060d4:	6a 00                	push   $0x0
  pushl $9
801060d6:	6a 09                	push   $0x9
  jmp alltraps
801060d8:	e9 aa fa ff ff       	jmp    80105b87 <alltraps>

801060dd <vector10>:
.globl vector10
vector10:
  pushl $10
801060dd:	6a 0a                	push   $0xa
  jmp alltraps
801060df:	e9 a3 fa ff ff       	jmp    80105b87 <alltraps>

801060e4 <vector11>:
.globl vector11
vector11:
  pushl $11
801060e4:	6a 0b                	push   $0xb
  jmp alltraps
801060e6:	e9 9c fa ff ff       	jmp    80105b87 <alltraps>

801060eb <vector12>:
.globl vector12
vector12:
  pushl $12
801060eb:	6a 0c                	push   $0xc
  jmp alltraps
801060ed:	e9 95 fa ff ff       	jmp    80105b87 <alltraps>

801060f2 <vector13>:
.globl vector13
vector13:
  pushl $13
801060f2:	6a 0d                	push   $0xd
  jmp alltraps
801060f4:	e9 8e fa ff ff       	jmp    80105b87 <alltraps>

801060f9 <vector14>:
.globl vector14
vector14:
  pushl $14
801060f9:	6a 0e                	push   $0xe
  jmp alltraps
801060fb:	e9 87 fa ff ff       	jmp    80105b87 <alltraps>

80106100 <vector15>:
.globl vector15
vector15:
  pushl $0
80106100:	6a 00                	push   $0x0
  pushl $15
80106102:	6a 0f                	push   $0xf
  jmp alltraps
80106104:	e9 7e fa ff ff       	jmp    80105b87 <alltraps>

80106109 <vector16>:
.globl vector16
vector16:
  pushl $0
80106109:	6a 00                	push   $0x0
  pushl $16
8010610b:	6a 10                	push   $0x10
  jmp alltraps
8010610d:	e9 75 fa ff ff       	jmp    80105b87 <alltraps>

80106112 <vector17>:
.globl vector17
vector17:
  pushl $17
80106112:	6a 11                	push   $0x11
  jmp alltraps
80106114:	e9 6e fa ff ff       	jmp    80105b87 <alltraps>

80106119 <vector18>:
.globl vector18
vector18:
  pushl $0
80106119:	6a 00                	push   $0x0
  pushl $18
8010611b:	6a 12                	push   $0x12
  jmp alltraps
8010611d:	e9 65 fa ff ff       	jmp    80105b87 <alltraps>

80106122 <vector19>:
.globl vector19
vector19:
  pushl $0
80106122:	6a 00                	push   $0x0
  pushl $19
80106124:	6a 13                	push   $0x13
  jmp alltraps
80106126:	e9 5c fa ff ff       	jmp    80105b87 <alltraps>

8010612b <vector20>:
.globl vector20
vector20:
  pushl $0
8010612b:	6a 00                	push   $0x0
  pushl $20
8010612d:	6a 14                	push   $0x14
  jmp alltraps
8010612f:	e9 53 fa ff ff       	jmp    80105b87 <alltraps>

80106134 <vector21>:
.globl vector21
vector21:
  pushl $0
80106134:	6a 00                	push   $0x0
  pushl $21
80106136:	6a 15                	push   $0x15
  jmp alltraps
80106138:	e9 4a fa ff ff       	jmp    80105b87 <alltraps>

8010613d <vector22>:
.globl vector22
vector22:
  pushl $0
8010613d:	6a 00                	push   $0x0
  pushl $22
8010613f:	6a 16                	push   $0x16
  jmp alltraps
80106141:	e9 41 fa ff ff       	jmp    80105b87 <alltraps>

80106146 <vector23>:
.globl vector23
vector23:
  pushl $0
80106146:	6a 00                	push   $0x0
  pushl $23
80106148:	6a 17                	push   $0x17
  jmp alltraps
8010614a:	e9 38 fa ff ff       	jmp    80105b87 <alltraps>

8010614f <vector24>:
.globl vector24
vector24:
  pushl $0
8010614f:	6a 00                	push   $0x0
  pushl $24
80106151:	6a 18                	push   $0x18
  jmp alltraps
80106153:	e9 2f fa ff ff       	jmp    80105b87 <alltraps>

80106158 <vector25>:
.globl vector25
vector25:
  pushl $0
80106158:	6a 00                	push   $0x0
  pushl $25
8010615a:	6a 19                	push   $0x19
  jmp alltraps
8010615c:	e9 26 fa ff ff       	jmp    80105b87 <alltraps>

80106161 <vector26>:
.globl vector26
vector26:
  pushl $0
80106161:	6a 00                	push   $0x0
  pushl $26
80106163:	6a 1a                	push   $0x1a
  jmp alltraps
80106165:	e9 1d fa ff ff       	jmp    80105b87 <alltraps>

8010616a <vector27>:
.globl vector27
vector27:
  pushl $0
8010616a:	6a 00                	push   $0x0
  pushl $27
8010616c:	6a 1b                	push   $0x1b
  jmp alltraps
8010616e:	e9 14 fa ff ff       	jmp    80105b87 <alltraps>

80106173 <vector28>:
.globl vector28
vector28:
  pushl $0
80106173:	6a 00                	push   $0x0
  pushl $28
80106175:	6a 1c                	push   $0x1c
  jmp alltraps
80106177:	e9 0b fa ff ff       	jmp    80105b87 <alltraps>

8010617c <vector29>:
.globl vector29
vector29:
  pushl $0
8010617c:	6a 00                	push   $0x0
  pushl $29
8010617e:	6a 1d                	push   $0x1d
  jmp alltraps
80106180:	e9 02 fa ff ff       	jmp    80105b87 <alltraps>

80106185 <vector30>:
.globl vector30
vector30:
  pushl $0
80106185:	6a 00                	push   $0x0
  pushl $30
80106187:	6a 1e                	push   $0x1e
  jmp alltraps
80106189:	e9 f9 f9 ff ff       	jmp    80105b87 <alltraps>

8010618e <vector31>:
.globl vector31
vector31:
  pushl $0
8010618e:	6a 00                	push   $0x0
  pushl $31
80106190:	6a 1f                	push   $0x1f
  jmp alltraps
80106192:	e9 f0 f9 ff ff       	jmp    80105b87 <alltraps>

80106197 <vector32>:
.globl vector32
vector32:
  pushl $0
80106197:	6a 00                	push   $0x0
  pushl $32
80106199:	6a 20                	push   $0x20
  jmp alltraps
8010619b:	e9 e7 f9 ff ff       	jmp    80105b87 <alltraps>

801061a0 <vector33>:
.globl vector33
vector33:
  pushl $0
801061a0:	6a 00                	push   $0x0
  pushl $33
801061a2:	6a 21                	push   $0x21
  jmp alltraps
801061a4:	e9 de f9 ff ff       	jmp    80105b87 <alltraps>

801061a9 <vector34>:
.globl vector34
vector34:
  pushl $0
801061a9:	6a 00                	push   $0x0
  pushl $34
801061ab:	6a 22                	push   $0x22
  jmp alltraps
801061ad:	e9 d5 f9 ff ff       	jmp    80105b87 <alltraps>

801061b2 <vector35>:
.globl vector35
vector35:
  pushl $0
801061b2:	6a 00                	push   $0x0
  pushl $35
801061b4:	6a 23                	push   $0x23
  jmp alltraps
801061b6:	e9 cc f9 ff ff       	jmp    80105b87 <alltraps>

801061bb <vector36>:
.globl vector36
vector36:
  pushl $0
801061bb:	6a 00                	push   $0x0
  pushl $36
801061bd:	6a 24                	push   $0x24
  jmp alltraps
801061bf:	e9 c3 f9 ff ff       	jmp    80105b87 <alltraps>

801061c4 <vector37>:
.globl vector37
vector37:
  pushl $0
801061c4:	6a 00                	push   $0x0
  pushl $37
801061c6:	6a 25                	push   $0x25
  jmp alltraps
801061c8:	e9 ba f9 ff ff       	jmp    80105b87 <alltraps>

801061cd <vector38>:
.globl vector38
vector38:
  pushl $0
801061cd:	6a 00                	push   $0x0
  pushl $38
801061cf:	6a 26                	push   $0x26
  jmp alltraps
801061d1:	e9 b1 f9 ff ff       	jmp    80105b87 <alltraps>

801061d6 <vector39>:
.globl vector39
vector39:
  pushl $0
801061d6:	6a 00                	push   $0x0
  pushl $39
801061d8:	6a 27                	push   $0x27
  jmp alltraps
801061da:	e9 a8 f9 ff ff       	jmp    80105b87 <alltraps>

801061df <vector40>:
.globl vector40
vector40:
  pushl $0
801061df:	6a 00                	push   $0x0
  pushl $40
801061e1:	6a 28                	push   $0x28
  jmp alltraps
801061e3:	e9 9f f9 ff ff       	jmp    80105b87 <alltraps>

801061e8 <vector41>:
.globl vector41
vector41:
  pushl $0
801061e8:	6a 00                	push   $0x0
  pushl $41
801061ea:	6a 29                	push   $0x29
  jmp alltraps
801061ec:	e9 96 f9 ff ff       	jmp    80105b87 <alltraps>

801061f1 <vector42>:
.globl vector42
vector42:
  pushl $0
801061f1:	6a 00                	push   $0x0
  pushl $42
801061f3:	6a 2a                	push   $0x2a
  jmp alltraps
801061f5:	e9 8d f9 ff ff       	jmp    80105b87 <alltraps>

801061fa <vector43>:
.globl vector43
vector43:
  pushl $0
801061fa:	6a 00                	push   $0x0
  pushl $43
801061fc:	6a 2b                	push   $0x2b
  jmp alltraps
801061fe:	e9 84 f9 ff ff       	jmp    80105b87 <alltraps>

80106203 <vector44>:
.globl vector44
vector44:
  pushl $0
80106203:	6a 00                	push   $0x0
  pushl $44
80106205:	6a 2c                	push   $0x2c
  jmp alltraps
80106207:	e9 7b f9 ff ff       	jmp    80105b87 <alltraps>

8010620c <vector45>:
.globl vector45
vector45:
  pushl $0
8010620c:	6a 00                	push   $0x0
  pushl $45
8010620e:	6a 2d                	push   $0x2d
  jmp alltraps
80106210:	e9 72 f9 ff ff       	jmp    80105b87 <alltraps>

80106215 <vector46>:
.globl vector46
vector46:
  pushl $0
80106215:	6a 00                	push   $0x0
  pushl $46
80106217:	6a 2e                	push   $0x2e
  jmp alltraps
80106219:	e9 69 f9 ff ff       	jmp    80105b87 <alltraps>

8010621e <vector47>:
.globl vector47
vector47:
  pushl $0
8010621e:	6a 00                	push   $0x0
  pushl $47
80106220:	6a 2f                	push   $0x2f
  jmp alltraps
80106222:	e9 60 f9 ff ff       	jmp    80105b87 <alltraps>

80106227 <vector48>:
.globl vector48
vector48:
  pushl $0
80106227:	6a 00                	push   $0x0
  pushl $48
80106229:	6a 30                	push   $0x30
  jmp alltraps
8010622b:	e9 57 f9 ff ff       	jmp    80105b87 <alltraps>

80106230 <vector49>:
.globl vector49
vector49:
  pushl $0
80106230:	6a 00                	push   $0x0
  pushl $49
80106232:	6a 31                	push   $0x31
  jmp alltraps
80106234:	e9 4e f9 ff ff       	jmp    80105b87 <alltraps>

80106239 <vector50>:
.globl vector50
vector50:
  pushl $0
80106239:	6a 00                	push   $0x0
  pushl $50
8010623b:	6a 32                	push   $0x32
  jmp alltraps
8010623d:	e9 45 f9 ff ff       	jmp    80105b87 <alltraps>

80106242 <vector51>:
.globl vector51
vector51:
  pushl $0
80106242:	6a 00                	push   $0x0
  pushl $51
80106244:	6a 33                	push   $0x33
  jmp alltraps
80106246:	e9 3c f9 ff ff       	jmp    80105b87 <alltraps>

8010624b <vector52>:
.globl vector52
vector52:
  pushl $0
8010624b:	6a 00                	push   $0x0
  pushl $52
8010624d:	6a 34                	push   $0x34
  jmp alltraps
8010624f:	e9 33 f9 ff ff       	jmp    80105b87 <alltraps>

80106254 <vector53>:
.globl vector53
vector53:
  pushl $0
80106254:	6a 00                	push   $0x0
  pushl $53
80106256:	6a 35                	push   $0x35
  jmp alltraps
80106258:	e9 2a f9 ff ff       	jmp    80105b87 <alltraps>

8010625d <vector54>:
.globl vector54
vector54:
  pushl $0
8010625d:	6a 00                	push   $0x0
  pushl $54
8010625f:	6a 36                	push   $0x36
  jmp alltraps
80106261:	e9 21 f9 ff ff       	jmp    80105b87 <alltraps>

80106266 <vector55>:
.globl vector55
vector55:
  pushl $0
80106266:	6a 00                	push   $0x0
  pushl $55
80106268:	6a 37                	push   $0x37
  jmp alltraps
8010626a:	e9 18 f9 ff ff       	jmp    80105b87 <alltraps>

8010626f <vector56>:
.globl vector56
vector56:
  pushl $0
8010626f:	6a 00                	push   $0x0
  pushl $56
80106271:	6a 38                	push   $0x38
  jmp alltraps
80106273:	e9 0f f9 ff ff       	jmp    80105b87 <alltraps>

80106278 <vector57>:
.globl vector57
vector57:
  pushl $0
80106278:	6a 00                	push   $0x0
  pushl $57
8010627a:	6a 39                	push   $0x39
  jmp alltraps
8010627c:	e9 06 f9 ff ff       	jmp    80105b87 <alltraps>

80106281 <vector58>:
.globl vector58
vector58:
  pushl $0
80106281:	6a 00                	push   $0x0
  pushl $58
80106283:	6a 3a                	push   $0x3a
  jmp alltraps
80106285:	e9 fd f8 ff ff       	jmp    80105b87 <alltraps>

8010628a <vector59>:
.globl vector59
vector59:
  pushl $0
8010628a:	6a 00                	push   $0x0
  pushl $59
8010628c:	6a 3b                	push   $0x3b
  jmp alltraps
8010628e:	e9 f4 f8 ff ff       	jmp    80105b87 <alltraps>

80106293 <vector60>:
.globl vector60
vector60:
  pushl $0
80106293:	6a 00                	push   $0x0
  pushl $60
80106295:	6a 3c                	push   $0x3c
  jmp alltraps
80106297:	e9 eb f8 ff ff       	jmp    80105b87 <alltraps>

8010629c <vector61>:
.globl vector61
vector61:
  pushl $0
8010629c:	6a 00                	push   $0x0
  pushl $61
8010629e:	6a 3d                	push   $0x3d
  jmp alltraps
801062a0:	e9 e2 f8 ff ff       	jmp    80105b87 <alltraps>

801062a5 <vector62>:
.globl vector62
vector62:
  pushl $0
801062a5:	6a 00                	push   $0x0
  pushl $62
801062a7:	6a 3e                	push   $0x3e
  jmp alltraps
801062a9:	e9 d9 f8 ff ff       	jmp    80105b87 <alltraps>

801062ae <vector63>:
.globl vector63
vector63:
  pushl $0
801062ae:	6a 00                	push   $0x0
  pushl $63
801062b0:	6a 3f                	push   $0x3f
  jmp alltraps
801062b2:	e9 d0 f8 ff ff       	jmp    80105b87 <alltraps>

801062b7 <vector64>:
.globl vector64
vector64:
  pushl $0
801062b7:	6a 00                	push   $0x0
  pushl $64
801062b9:	6a 40                	push   $0x40
  jmp alltraps
801062bb:	e9 c7 f8 ff ff       	jmp    80105b87 <alltraps>

801062c0 <vector65>:
.globl vector65
vector65:
  pushl $0
801062c0:	6a 00                	push   $0x0
  pushl $65
801062c2:	6a 41                	push   $0x41
  jmp alltraps
801062c4:	e9 be f8 ff ff       	jmp    80105b87 <alltraps>

801062c9 <vector66>:
.globl vector66
vector66:
  pushl $0
801062c9:	6a 00                	push   $0x0
  pushl $66
801062cb:	6a 42                	push   $0x42
  jmp alltraps
801062cd:	e9 b5 f8 ff ff       	jmp    80105b87 <alltraps>

801062d2 <vector67>:
.globl vector67
vector67:
  pushl $0
801062d2:	6a 00                	push   $0x0
  pushl $67
801062d4:	6a 43                	push   $0x43
  jmp alltraps
801062d6:	e9 ac f8 ff ff       	jmp    80105b87 <alltraps>

801062db <vector68>:
.globl vector68
vector68:
  pushl $0
801062db:	6a 00                	push   $0x0
  pushl $68
801062dd:	6a 44                	push   $0x44
  jmp alltraps
801062df:	e9 a3 f8 ff ff       	jmp    80105b87 <alltraps>

801062e4 <vector69>:
.globl vector69
vector69:
  pushl $0
801062e4:	6a 00                	push   $0x0
  pushl $69
801062e6:	6a 45                	push   $0x45
  jmp alltraps
801062e8:	e9 9a f8 ff ff       	jmp    80105b87 <alltraps>

801062ed <vector70>:
.globl vector70
vector70:
  pushl $0
801062ed:	6a 00                	push   $0x0
  pushl $70
801062ef:	6a 46                	push   $0x46
  jmp alltraps
801062f1:	e9 91 f8 ff ff       	jmp    80105b87 <alltraps>

801062f6 <vector71>:
.globl vector71
vector71:
  pushl $0
801062f6:	6a 00                	push   $0x0
  pushl $71
801062f8:	6a 47                	push   $0x47
  jmp alltraps
801062fa:	e9 88 f8 ff ff       	jmp    80105b87 <alltraps>

801062ff <vector72>:
.globl vector72
vector72:
  pushl $0
801062ff:	6a 00                	push   $0x0
  pushl $72
80106301:	6a 48                	push   $0x48
  jmp alltraps
80106303:	e9 7f f8 ff ff       	jmp    80105b87 <alltraps>

80106308 <vector73>:
.globl vector73
vector73:
  pushl $0
80106308:	6a 00                	push   $0x0
  pushl $73
8010630a:	6a 49                	push   $0x49
  jmp alltraps
8010630c:	e9 76 f8 ff ff       	jmp    80105b87 <alltraps>

80106311 <vector74>:
.globl vector74
vector74:
  pushl $0
80106311:	6a 00                	push   $0x0
  pushl $74
80106313:	6a 4a                	push   $0x4a
  jmp alltraps
80106315:	e9 6d f8 ff ff       	jmp    80105b87 <alltraps>

8010631a <vector75>:
.globl vector75
vector75:
  pushl $0
8010631a:	6a 00                	push   $0x0
  pushl $75
8010631c:	6a 4b                	push   $0x4b
  jmp alltraps
8010631e:	e9 64 f8 ff ff       	jmp    80105b87 <alltraps>

80106323 <vector76>:
.globl vector76
vector76:
  pushl $0
80106323:	6a 00                	push   $0x0
  pushl $76
80106325:	6a 4c                	push   $0x4c
  jmp alltraps
80106327:	e9 5b f8 ff ff       	jmp    80105b87 <alltraps>

8010632c <vector77>:
.globl vector77
vector77:
  pushl $0
8010632c:	6a 00                	push   $0x0
  pushl $77
8010632e:	6a 4d                	push   $0x4d
  jmp alltraps
80106330:	e9 52 f8 ff ff       	jmp    80105b87 <alltraps>

80106335 <vector78>:
.globl vector78
vector78:
  pushl $0
80106335:	6a 00                	push   $0x0
  pushl $78
80106337:	6a 4e                	push   $0x4e
  jmp alltraps
80106339:	e9 49 f8 ff ff       	jmp    80105b87 <alltraps>

8010633e <vector79>:
.globl vector79
vector79:
  pushl $0
8010633e:	6a 00                	push   $0x0
  pushl $79
80106340:	6a 4f                	push   $0x4f
  jmp alltraps
80106342:	e9 40 f8 ff ff       	jmp    80105b87 <alltraps>

80106347 <vector80>:
.globl vector80
vector80:
  pushl $0
80106347:	6a 00                	push   $0x0
  pushl $80
80106349:	6a 50                	push   $0x50
  jmp alltraps
8010634b:	e9 37 f8 ff ff       	jmp    80105b87 <alltraps>

80106350 <vector81>:
.globl vector81
vector81:
  pushl $0
80106350:	6a 00                	push   $0x0
  pushl $81
80106352:	6a 51                	push   $0x51
  jmp alltraps
80106354:	e9 2e f8 ff ff       	jmp    80105b87 <alltraps>

80106359 <vector82>:
.globl vector82
vector82:
  pushl $0
80106359:	6a 00                	push   $0x0
  pushl $82
8010635b:	6a 52                	push   $0x52
  jmp alltraps
8010635d:	e9 25 f8 ff ff       	jmp    80105b87 <alltraps>

80106362 <vector83>:
.globl vector83
vector83:
  pushl $0
80106362:	6a 00                	push   $0x0
  pushl $83
80106364:	6a 53                	push   $0x53
  jmp alltraps
80106366:	e9 1c f8 ff ff       	jmp    80105b87 <alltraps>

8010636b <vector84>:
.globl vector84
vector84:
  pushl $0
8010636b:	6a 00                	push   $0x0
  pushl $84
8010636d:	6a 54                	push   $0x54
  jmp alltraps
8010636f:	e9 13 f8 ff ff       	jmp    80105b87 <alltraps>

80106374 <vector85>:
.globl vector85
vector85:
  pushl $0
80106374:	6a 00                	push   $0x0
  pushl $85
80106376:	6a 55                	push   $0x55
  jmp alltraps
80106378:	e9 0a f8 ff ff       	jmp    80105b87 <alltraps>

8010637d <vector86>:
.globl vector86
vector86:
  pushl $0
8010637d:	6a 00                	push   $0x0
  pushl $86
8010637f:	6a 56                	push   $0x56
  jmp alltraps
80106381:	e9 01 f8 ff ff       	jmp    80105b87 <alltraps>

80106386 <vector87>:
.globl vector87
vector87:
  pushl $0
80106386:	6a 00                	push   $0x0
  pushl $87
80106388:	6a 57                	push   $0x57
  jmp alltraps
8010638a:	e9 f8 f7 ff ff       	jmp    80105b87 <alltraps>

8010638f <vector88>:
.globl vector88
vector88:
  pushl $0
8010638f:	6a 00                	push   $0x0
  pushl $88
80106391:	6a 58                	push   $0x58
  jmp alltraps
80106393:	e9 ef f7 ff ff       	jmp    80105b87 <alltraps>

80106398 <vector89>:
.globl vector89
vector89:
  pushl $0
80106398:	6a 00                	push   $0x0
  pushl $89
8010639a:	6a 59                	push   $0x59
  jmp alltraps
8010639c:	e9 e6 f7 ff ff       	jmp    80105b87 <alltraps>

801063a1 <vector90>:
.globl vector90
vector90:
  pushl $0
801063a1:	6a 00                	push   $0x0
  pushl $90
801063a3:	6a 5a                	push   $0x5a
  jmp alltraps
801063a5:	e9 dd f7 ff ff       	jmp    80105b87 <alltraps>

801063aa <vector91>:
.globl vector91
vector91:
  pushl $0
801063aa:	6a 00                	push   $0x0
  pushl $91
801063ac:	6a 5b                	push   $0x5b
  jmp alltraps
801063ae:	e9 d4 f7 ff ff       	jmp    80105b87 <alltraps>

801063b3 <vector92>:
.globl vector92
vector92:
  pushl $0
801063b3:	6a 00                	push   $0x0
  pushl $92
801063b5:	6a 5c                	push   $0x5c
  jmp alltraps
801063b7:	e9 cb f7 ff ff       	jmp    80105b87 <alltraps>

801063bc <vector93>:
.globl vector93
vector93:
  pushl $0
801063bc:	6a 00                	push   $0x0
  pushl $93
801063be:	6a 5d                	push   $0x5d
  jmp alltraps
801063c0:	e9 c2 f7 ff ff       	jmp    80105b87 <alltraps>

801063c5 <vector94>:
.globl vector94
vector94:
  pushl $0
801063c5:	6a 00                	push   $0x0
  pushl $94
801063c7:	6a 5e                	push   $0x5e
  jmp alltraps
801063c9:	e9 b9 f7 ff ff       	jmp    80105b87 <alltraps>

801063ce <vector95>:
.globl vector95
vector95:
  pushl $0
801063ce:	6a 00                	push   $0x0
  pushl $95
801063d0:	6a 5f                	push   $0x5f
  jmp alltraps
801063d2:	e9 b0 f7 ff ff       	jmp    80105b87 <alltraps>

801063d7 <vector96>:
.globl vector96
vector96:
  pushl $0
801063d7:	6a 00                	push   $0x0
  pushl $96
801063d9:	6a 60                	push   $0x60
  jmp alltraps
801063db:	e9 a7 f7 ff ff       	jmp    80105b87 <alltraps>

801063e0 <vector97>:
.globl vector97
vector97:
  pushl $0
801063e0:	6a 00                	push   $0x0
  pushl $97
801063e2:	6a 61                	push   $0x61
  jmp alltraps
801063e4:	e9 9e f7 ff ff       	jmp    80105b87 <alltraps>

801063e9 <vector98>:
.globl vector98
vector98:
  pushl $0
801063e9:	6a 00                	push   $0x0
  pushl $98
801063eb:	6a 62                	push   $0x62
  jmp alltraps
801063ed:	e9 95 f7 ff ff       	jmp    80105b87 <alltraps>

801063f2 <vector99>:
.globl vector99
vector99:
  pushl $0
801063f2:	6a 00                	push   $0x0
  pushl $99
801063f4:	6a 63                	push   $0x63
  jmp alltraps
801063f6:	e9 8c f7 ff ff       	jmp    80105b87 <alltraps>

801063fb <vector100>:
.globl vector100
vector100:
  pushl $0
801063fb:	6a 00                	push   $0x0
  pushl $100
801063fd:	6a 64                	push   $0x64
  jmp alltraps
801063ff:	e9 83 f7 ff ff       	jmp    80105b87 <alltraps>

80106404 <vector101>:
.globl vector101
vector101:
  pushl $0
80106404:	6a 00                	push   $0x0
  pushl $101
80106406:	6a 65                	push   $0x65
  jmp alltraps
80106408:	e9 7a f7 ff ff       	jmp    80105b87 <alltraps>

8010640d <vector102>:
.globl vector102
vector102:
  pushl $0
8010640d:	6a 00                	push   $0x0
  pushl $102
8010640f:	6a 66                	push   $0x66
  jmp alltraps
80106411:	e9 71 f7 ff ff       	jmp    80105b87 <alltraps>

80106416 <vector103>:
.globl vector103
vector103:
  pushl $0
80106416:	6a 00                	push   $0x0
  pushl $103
80106418:	6a 67                	push   $0x67
  jmp alltraps
8010641a:	e9 68 f7 ff ff       	jmp    80105b87 <alltraps>

8010641f <vector104>:
.globl vector104
vector104:
  pushl $0
8010641f:	6a 00                	push   $0x0
  pushl $104
80106421:	6a 68                	push   $0x68
  jmp alltraps
80106423:	e9 5f f7 ff ff       	jmp    80105b87 <alltraps>

80106428 <vector105>:
.globl vector105
vector105:
  pushl $0
80106428:	6a 00                	push   $0x0
  pushl $105
8010642a:	6a 69                	push   $0x69
  jmp alltraps
8010642c:	e9 56 f7 ff ff       	jmp    80105b87 <alltraps>

80106431 <vector106>:
.globl vector106
vector106:
  pushl $0
80106431:	6a 00                	push   $0x0
  pushl $106
80106433:	6a 6a                	push   $0x6a
  jmp alltraps
80106435:	e9 4d f7 ff ff       	jmp    80105b87 <alltraps>

8010643a <vector107>:
.globl vector107
vector107:
  pushl $0
8010643a:	6a 00                	push   $0x0
  pushl $107
8010643c:	6a 6b                	push   $0x6b
  jmp alltraps
8010643e:	e9 44 f7 ff ff       	jmp    80105b87 <alltraps>

80106443 <vector108>:
.globl vector108
vector108:
  pushl $0
80106443:	6a 00                	push   $0x0
  pushl $108
80106445:	6a 6c                	push   $0x6c
  jmp alltraps
80106447:	e9 3b f7 ff ff       	jmp    80105b87 <alltraps>

8010644c <vector109>:
.globl vector109
vector109:
  pushl $0
8010644c:	6a 00                	push   $0x0
  pushl $109
8010644e:	6a 6d                	push   $0x6d
  jmp alltraps
80106450:	e9 32 f7 ff ff       	jmp    80105b87 <alltraps>

80106455 <vector110>:
.globl vector110
vector110:
  pushl $0
80106455:	6a 00                	push   $0x0
  pushl $110
80106457:	6a 6e                	push   $0x6e
  jmp alltraps
80106459:	e9 29 f7 ff ff       	jmp    80105b87 <alltraps>

8010645e <vector111>:
.globl vector111
vector111:
  pushl $0
8010645e:	6a 00                	push   $0x0
  pushl $111
80106460:	6a 6f                	push   $0x6f
  jmp alltraps
80106462:	e9 20 f7 ff ff       	jmp    80105b87 <alltraps>

80106467 <vector112>:
.globl vector112
vector112:
  pushl $0
80106467:	6a 00                	push   $0x0
  pushl $112
80106469:	6a 70                	push   $0x70
  jmp alltraps
8010646b:	e9 17 f7 ff ff       	jmp    80105b87 <alltraps>

80106470 <vector113>:
.globl vector113
vector113:
  pushl $0
80106470:	6a 00                	push   $0x0
  pushl $113
80106472:	6a 71                	push   $0x71
  jmp alltraps
80106474:	e9 0e f7 ff ff       	jmp    80105b87 <alltraps>

80106479 <vector114>:
.globl vector114
vector114:
  pushl $0
80106479:	6a 00                	push   $0x0
  pushl $114
8010647b:	6a 72                	push   $0x72
  jmp alltraps
8010647d:	e9 05 f7 ff ff       	jmp    80105b87 <alltraps>

80106482 <vector115>:
.globl vector115
vector115:
  pushl $0
80106482:	6a 00                	push   $0x0
  pushl $115
80106484:	6a 73                	push   $0x73
  jmp alltraps
80106486:	e9 fc f6 ff ff       	jmp    80105b87 <alltraps>

8010648b <vector116>:
.globl vector116
vector116:
  pushl $0
8010648b:	6a 00                	push   $0x0
  pushl $116
8010648d:	6a 74                	push   $0x74
  jmp alltraps
8010648f:	e9 f3 f6 ff ff       	jmp    80105b87 <alltraps>

80106494 <vector117>:
.globl vector117
vector117:
  pushl $0
80106494:	6a 00                	push   $0x0
  pushl $117
80106496:	6a 75                	push   $0x75
  jmp alltraps
80106498:	e9 ea f6 ff ff       	jmp    80105b87 <alltraps>

8010649d <vector118>:
.globl vector118
vector118:
  pushl $0
8010649d:	6a 00                	push   $0x0
  pushl $118
8010649f:	6a 76                	push   $0x76
  jmp alltraps
801064a1:	e9 e1 f6 ff ff       	jmp    80105b87 <alltraps>

801064a6 <vector119>:
.globl vector119
vector119:
  pushl $0
801064a6:	6a 00                	push   $0x0
  pushl $119
801064a8:	6a 77                	push   $0x77
  jmp alltraps
801064aa:	e9 d8 f6 ff ff       	jmp    80105b87 <alltraps>

801064af <vector120>:
.globl vector120
vector120:
  pushl $0
801064af:	6a 00                	push   $0x0
  pushl $120
801064b1:	6a 78                	push   $0x78
  jmp alltraps
801064b3:	e9 cf f6 ff ff       	jmp    80105b87 <alltraps>

801064b8 <vector121>:
.globl vector121
vector121:
  pushl $0
801064b8:	6a 00                	push   $0x0
  pushl $121
801064ba:	6a 79                	push   $0x79
  jmp alltraps
801064bc:	e9 c6 f6 ff ff       	jmp    80105b87 <alltraps>

801064c1 <vector122>:
.globl vector122
vector122:
  pushl $0
801064c1:	6a 00                	push   $0x0
  pushl $122
801064c3:	6a 7a                	push   $0x7a
  jmp alltraps
801064c5:	e9 bd f6 ff ff       	jmp    80105b87 <alltraps>

801064ca <vector123>:
.globl vector123
vector123:
  pushl $0
801064ca:	6a 00                	push   $0x0
  pushl $123
801064cc:	6a 7b                	push   $0x7b
  jmp alltraps
801064ce:	e9 b4 f6 ff ff       	jmp    80105b87 <alltraps>

801064d3 <vector124>:
.globl vector124
vector124:
  pushl $0
801064d3:	6a 00                	push   $0x0
  pushl $124
801064d5:	6a 7c                	push   $0x7c
  jmp alltraps
801064d7:	e9 ab f6 ff ff       	jmp    80105b87 <alltraps>

801064dc <vector125>:
.globl vector125
vector125:
  pushl $0
801064dc:	6a 00                	push   $0x0
  pushl $125
801064de:	6a 7d                	push   $0x7d
  jmp alltraps
801064e0:	e9 a2 f6 ff ff       	jmp    80105b87 <alltraps>

801064e5 <vector126>:
.globl vector126
vector126:
  pushl $0
801064e5:	6a 00                	push   $0x0
  pushl $126
801064e7:	6a 7e                	push   $0x7e
  jmp alltraps
801064e9:	e9 99 f6 ff ff       	jmp    80105b87 <alltraps>

801064ee <vector127>:
.globl vector127
vector127:
  pushl $0
801064ee:	6a 00                	push   $0x0
  pushl $127
801064f0:	6a 7f                	push   $0x7f
  jmp alltraps
801064f2:	e9 90 f6 ff ff       	jmp    80105b87 <alltraps>

801064f7 <vector128>:
.globl vector128
vector128:
  pushl $0
801064f7:	6a 00                	push   $0x0
  pushl $128
801064f9:	68 80 00 00 00       	push   $0x80
  jmp alltraps
801064fe:	e9 84 f6 ff ff       	jmp    80105b87 <alltraps>

80106503 <vector129>:
.globl vector129
vector129:
  pushl $0
80106503:	6a 00                	push   $0x0
  pushl $129
80106505:	68 81 00 00 00       	push   $0x81
  jmp alltraps
8010650a:	e9 78 f6 ff ff       	jmp    80105b87 <alltraps>

8010650f <vector130>:
.globl vector130
vector130:
  pushl $0
8010650f:	6a 00                	push   $0x0
  pushl $130
80106511:	68 82 00 00 00       	push   $0x82
  jmp alltraps
80106516:	e9 6c f6 ff ff       	jmp    80105b87 <alltraps>

8010651b <vector131>:
.globl vector131
vector131:
  pushl $0
8010651b:	6a 00                	push   $0x0
  pushl $131
8010651d:	68 83 00 00 00       	push   $0x83
  jmp alltraps
80106522:	e9 60 f6 ff ff       	jmp    80105b87 <alltraps>

80106527 <vector132>:
.globl vector132
vector132:
  pushl $0
80106527:	6a 00                	push   $0x0
  pushl $132
80106529:	68 84 00 00 00       	push   $0x84
  jmp alltraps
8010652e:	e9 54 f6 ff ff       	jmp    80105b87 <alltraps>

80106533 <vector133>:
.globl vector133
vector133:
  pushl $0
80106533:	6a 00                	push   $0x0
  pushl $133
80106535:	68 85 00 00 00       	push   $0x85
  jmp alltraps
8010653a:	e9 48 f6 ff ff       	jmp    80105b87 <alltraps>

8010653f <vector134>:
.globl vector134
vector134:
  pushl $0
8010653f:	6a 00                	push   $0x0
  pushl $134
80106541:	68 86 00 00 00       	push   $0x86
  jmp alltraps
80106546:	e9 3c f6 ff ff       	jmp    80105b87 <alltraps>

8010654b <vector135>:
.globl vector135
vector135:
  pushl $0
8010654b:	6a 00                	push   $0x0
  pushl $135
8010654d:	68 87 00 00 00       	push   $0x87
  jmp alltraps
80106552:	e9 30 f6 ff ff       	jmp    80105b87 <alltraps>

80106557 <vector136>:
.globl vector136
vector136:
  pushl $0
80106557:	6a 00                	push   $0x0
  pushl $136
80106559:	68 88 00 00 00       	push   $0x88
  jmp alltraps
8010655e:	e9 24 f6 ff ff       	jmp    80105b87 <alltraps>

80106563 <vector137>:
.globl vector137
vector137:
  pushl $0
80106563:	6a 00                	push   $0x0
  pushl $137
80106565:	68 89 00 00 00       	push   $0x89
  jmp alltraps
8010656a:	e9 18 f6 ff ff       	jmp    80105b87 <alltraps>

8010656f <vector138>:
.globl vector138
vector138:
  pushl $0
8010656f:	6a 00                	push   $0x0
  pushl $138
80106571:	68 8a 00 00 00       	push   $0x8a
  jmp alltraps
80106576:	e9 0c f6 ff ff       	jmp    80105b87 <alltraps>

8010657b <vector139>:
.globl vector139
vector139:
  pushl $0
8010657b:	6a 00                	push   $0x0
  pushl $139
8010657d:	68 8b 00 00 00       	push   $0x8b
  jmp alltraps
80106582:	e9 00 f6 ff ff       	jmp    80105b87 <alltraps>

80106587 <vector140>:
.globl vector140
vector140:
  pushl $0
80106587:	6a 00                	push   $0x0
  pushl $140
80106589:	68 8c 00 00 00       	push   $0x8c
  jmp alltraps
8010658e:	e9 f4 f5 ff ff       	jmp    80105b87 <alltraps>

80106593 <vector141>:
.globl vector141
vector141:
  pushl $0
80106593:	6a 00                	push   $0x0
  pushl $141
80106595:	68 8d 00 00 00       	push   $0x8d
  jmp alltraps
8010659a:	e9 e8 f5 ff ff       	jmp    80105b87 <alltraps>

8010659f <vector142>:
.globl vector142
vector142:
  pushl $0
8010659f:	6a 00                	push   $0x0
  pushl $142
801065a1:	68 8e 00 00 00       	push   $0x8e
  jmp alltraps
801065a6:	e9 dc f5 ff ff       	jmp    80105b87 <alltraps>

801065ab <vector143>:
.globl vector143
vector143:
  pushl $0
801065ab:	6a 00                	push   $0x0
  pushl $143
801065ad:	68 8f 00 00 00       	push   $0x8f
  jmp alltraps
801065b2:	e9 d0 f5 ff ff       	jmp    80105b87 <alltraps>

801065b7 <vector144>:
.globl vector144
vector144:
  pushl $0
801065b7:	6a 00                	push   $0x0
  pushl $144
801065b9:	68 90 00 00 00       	push   $0x90
  jmp alltraps
801065be:	e9 c4 f5 ff ff       	jmp    80105b87 <alltraps>

801065c3 <vector145>:
.globl vector145
vector145:
  pushl $0
801065c3:	6a 00                	push   $0x0
  pushl $145
801065c5:	68 91 00 00 00       	push   $0x91
  jmp alltraps
801065ca:	e9 b8 f5 ff ff       	jmp    80105b87 <alltraps>

801065cf <vector146>:
.globl vector146
vector146:
  pushl $0
801065cf:	6a 00                	push   $0x0
  pushl $146
801065d1:	68 92 00 00 00       	push   $0x92
  jmp alltraps
801065d6:	e9 ac f5 ff ff       	jmp    80105b87 <alltraps>

801065db <vector147>:
.globl vector147
vector147:
  pushl $0
801065db:	6a 00                	push   $0x0
  pushl $147
801065dd:	68 93 00 00 00       	push   $0x93
  jmp alltraps
801065e2:	e9 a0 f5 ff ff       	jmp    80105b87 <alltraps>

801065e7 <vector148>:
.globl vector148
vector148:
  pushl $0
801065e7:	6a 00                	push   $0x0
  pushl $148
801065e9:	68 94 00 00 00       	push   $0x94
  jmp alltraps
801065ee:	e9 94 f5 ff ff       	jmp    80105b87 <alltraps>

801065f3 <vector149>:
.globl vector149
vector149:
  pushl $0
801065f3:	6a 00                	push   $0x0
  pushl $149
801065f5:	68 95 00 00 00       	push   $0x95
  jmp alltraps
801065fa:	e9 88 f5 ff ff       	jmp    80105b87 <alltraps>

801065ff <vector150>:
.globl vector150
vector150:
  pushl $0
801065ff:	6a 00                	push   $0x0
  pushl $150
80106601:	68 96 00 00 00       	push   $0x96
  jmp alltraps
80106606:	e9 7c f5 ff ff       	jmp    80105b87 <alltraps>

8010660b <vector151>:
.globl vector151
vector151:
  pushl $0
8010660b:	6a 00                	push   $0x0
  pushl $151
8010660d:	68 97 00 00 00       	push   $0x97
  jmp alltraps
80106612:	e9 70 f5 ff ff       	jmp    80105b87 <alltraps>

80106617 <vector152>:
.globl vector152
vector152:
  pushl $0
80106617:	6a 00                	push   $0x0
  pushl $152
80106619:	68 98 00 00 00       	push   $0x98
  jmp alltraps
8010661e:	e9 64 f5 ff ff       	jmp    80105b87 <alltraps>

80106623 <vector153>:
.globl vector153
vector153:
  pushl $0
80106623:	6a 00                	push   $0x0
  pushl $153
80106625:	68 99 00 00 00       	push   $0x99
  jmp alltraps
8010662a:	e9 58 f5 ff ff       	jmp    80105b87 <alltraps>

8010662f <vector154>:
.globl vector154
vector154:
  pushl $0
8010662f:	6a 00                	push   $0x0
  pushl $154
80106631:	68 9a 00 00 00       	push   $0x9a
  jmp alltraps
80106636:	e9 4c f5 ff ff       	jmp    80105b87 <alltraps>

8010663b <vector155>:
.globl vector155
vector155:
  pushl $0
8010663b:	6a 00                	push   $0x0
  pushl $155
8010663d:	68 9b 00 00 00       	push   $0x9b
  jmp alltraps
80106642:	e9 40 f5 ff ff       	jmp    80105b87 <alltraps>

80106647 <vector156>:
.globl vector156
vector156:
  pushl $0
80106647:	6a 00                	push   $0x0
  pushl $156
80106649:	68 9c 00 00 00       	push   $0x9c
  jmp alltraps
8010664e:	e9 34 f5 ff ff       	jmp    80105b87 <alltraps>

80106653 <vector157>:
.globl vector157
vector157:
  pushl $0
80106653:	6a 00                	push   $0x0
  pushl $157
80106655:	68 9d 00 00 00       	push   $0x9d
  jmp alltraps
8010665a:	e9 28 f5 ff ff       	jmp    80105b87 <alltraps>

8010665f <vector158>:
.globl vector158
vector158:
  pushl $0
8010665f:	6a 00                	push   $0x0
  pushl $158
80106661:	68 9e 00 00 00       	push   $0x9e
  jmp alltraps
80106666:	e9 1c f5 ff ff       	jmp    80105b87 <alltraps>

8010666b <vector159>:
.globl vector159
vector159:
  pushl $0
8010666b:	6a 00                	push   $0x0
  pushl $159
8010666d:	68 9f 00 00 00       	push   $0x9f
  jmp alltraps
80106672:	e9 10 f5 ff ff       	jmp    80105b87 <alltraps>

80106677 <vector160>:
.globl vector160
vector160:
  pushl $0
80106677:	6a 00                	push   $0x0
  pushl $160
80106679:	68 a0 00 00 00       	push   $0xa0
  jmp alltraps
8010667e:	e9 04 f5 ff ff       	jmp    80105b87 <alltraps>

80106683 <vector161>:
.globl vector161
vector161:
  pushl $0
80106683:	6a 00                	push   $0x0
  pushl $161
80106685:	68 a1 00 00 00       	push   $0xa1
  jmp alltraps
8010668a:	e9 f8 f4 ff ff       	jmp    80105b87 <alltraps>

8010668f <vector162>:
.globl vector162
vector162:
  pushl $0
8010668f:	6a 00                	push   $0x0
  pushl $162
80106691:	68 a2 00 00 00       	push   $0xa2
  jmp alltraps
80106696:	e9 ec f4 ff ff       	jmp    80105b87 <alltraps>

8010669b <vector163>:
.globl vector163
vector163:
  pushl $0
8010669b:	6a 00                	push   $0x0
  pushl $163
8010669d:	68 a3 00 00 00       	push   $0xa3
  jmp alltraps
801066a2:	e9 e0 f4 ff ff       	jmp    80105b87 <alltraps>

801066a7 <vector164>:
.globl vector164
vector164:
  pushl $0
801066a7:	6a 00                	push   $0x0
  pushl $164
801066a9:	68 a4 00 00 00       	push   $0xa4
  jmp alltraps
801066ae:	e9 d4 f4 ff ff       	jmp    80105b87 <alltraps>

801066b3 <vector165>:
.globl vector165
vector165:
  pushl $0
801066b3:	6a 00                	push   $0x0
  pushl $165
801066b5:	68 a5 00 00 00       	push   $0xa5
  jmp alltraps
801066ba:	e9 c8 f4 ff ff       	jmp    80105b87 <alltraps>

801066bf <vector166>:
.globl vector166
vector166:
  pushl $0
801066bf:	6a 00                	push   $0x0
  pushl $166
801066c1:	68 a6 00 00 00       	push   $0xa6
  jmp alltraps
801066c6:	e9 bc f4 ff ff       	jmp    80105b87 <alltraps>

801066cb <vector167>:
.globl vector167
vector167:
  pushl $0
801066cb:	6a 00                	push   $0x0
  pushl $167
801066cd:	68 a7 00 00 00       	push   $0xa7
  jmp alltraps
801066d2:	e9 b0 f4 ff ff       	jmp    80105b87 <alltraps>

801066d7 <vector168>:
.globl vector168
vector168:
  pushl $0
801066d7:	6a 00                	push   $0x0
  pushl $168
801066d9:	68 a8 00 00 00       	push   $0xa8
  jmp alltraps
801066de:	e9 a4 f4 ff ff       	jmp    80105b87 <alltraps>

801066e3 <vector169>:
.globl vector169
vector169:
  pushl $0
801066e3:	6a 00                	push   $0x0
  pushl $169
801066e5:	68 a9 00 00 00       	push   $0xa9
  jmp alltraps
801066ea:	e9 98 f4 ff ff       	jmp    80105b87 <alltraps>

801066ef <vector170>:
.globl vector170
vector170:
  pushl $0
801066ef:	6a 00                	push   $0x0
  pushl $170
801066f1:	68 aa 00 00 00       	push   $0xaa
  jmp alltraps
801066f6:	e9 8c f4 ff ff       	jmp    80105b87 <alltraps>

801066fb <vector171>:
.globl vector171
vector171:
  pushl $0
801066fb:	6a 00                	push   $0x0
  pushl $171
801066fd:	68 ab 00 00 00       	push   $0xab
  jmp alltraps
80106702:	e9 80 f4 ff ff       	jmp    80105b87 <alltraps>

80106707 <vector172>:
.globl vector172
vector172:
  pushl $0
80106707:	6a 00                	push   $0x0
  pushl $172
80106709:	68 ac 00 00 00       	push   $0xac
  jmp alltraps
8010670e:	e9 74 f4 ff ff       	jmp    80105b87 <alltraps>

80106713 <vector173>:
.globl vector173
vector173:
  pushl $0
80106713:	6a 00                	push   $0x0
  pushl $173
80106715:	68 ad 00 00 00       	push   $0xad
  jmp alltraps
8010671a:	e9 68 f4 ff ff       	jmp    80105b87 <alltraps>

8010671f <vector174>:
.globl vector174
vector174:
  pushl $0
8010671f:	6a 00                	push   $0x0
  pushl $174
80106721:	68 ae 00 00 00       	push   $0xae
  jmp alltraps
80106726:	e9 5c f4 ff ff       	jmp    80105b87 <alltraps>

8010672b <vector175>:
.globl vector175
vector175:
  pushl $0
8010672b:	6a 00                	push   $0x0
  pushl $175
8010672d:	68 af 00 00 00       	push   $0xaf
  jmp alltraps
80106732:	e9 50 f4 ff ff       	jmp    80105b87 <alltraps>

80106737 <vector176>:
.globl vector176
vector176:
  pushl $0
80106737:	6a 00                	push   $0x0
  pushl $176
80106739:	68 b0 00 00 00       	push   $0xb0
  jmp alltraps
8010673e:	e9 44 f4 ff ff       	jmp    80105b87 <alltraps>

80106743 <vector177>:
.globl vector177
vector177:
  pushl $0
80106743:	6a 00                	push   $0x0
  pushl $177
80106745:	68 b1 00 00 00       	push   $0xb1
  jmp alltraps
8010674a:	e9 38 f4 ff ff       	jmp    80105b87 <alltraps>

8010674f <vector178>:
.globl vector178
vector178:
  pushl $0
8010674f:	6a 00                	push   $0x0
  pushl $178
80106751:	68 b2 00 00 00       	push   $0xb2
  jmp alltraps
80106756:	e9 2c f4 ff ff       	jmp    80105b87 <alltraps>

8010675b <vector179>:
.globl vector179
vector179:
  pushl $0
8010675b:	6a 00                	push   $0x0
  pushl $179
8010675d:	68 b3 00 00 00       	push   $0xb3
  jmp alltraps
80106762:	e9 20 f4 ff ff       	jmp    80105b87 <alltraps>

80106767 <vector180>:
.globl vector180
vector180:
  pushl $0
80106767:	6a 00                	push   $0x0
  pushl $180
80106769:	68 b4 00 00 00       	push   $0xb4
  jmp alltraps
8010676e:	e9 14 f4 ff ff       	jmp    80105b87 <alltraps>

80106773 <vector181>:
.globl vector181
vector181:
  pushl $0
80106773:	6a 00                	push   $0x0
  pushl $181
80106775:	68 b5 00 00 00       	push   $0xb5
  jmp alltraps
8010677a:	e9 08 f4 ff ff       	jmp    80105b87 <alltraps>

8010677f <vector182>:
.globl vector182
vector182:
  pushl $0
8010677f:	6a 00                	push   $0x0
  pushl $182
80106781:	68 b6 00 00 00       	push   $0xb6
  jmp alltraps
80106786:	e9 fc f3 ff ff       	jmp    80105b87 <alltraps>

8010678b <vector183>:
.globl vector183
vector183:
  pushl $0
8010678b:	6a 00                	push   $0x0
  pushl $183
8010678d:	68 b7 00 00 00       	push   $0xb7
  jmp alltraps
80106792:	e9 f0 f3 ff ff       	jmp    80105b87 <alltraps>

80106797 <vector184>:
.globl vector184
vector184:
  pushl $0
80106797:	6a 00                	push   $0x0
  pushl $184
80106799:	68 b8 00 00 00       	push   $0xb8
  jmp alltraps
8010679e:	e9 e4 f3 ff ff       	jmp    80105b87 <alltraps>

801067a3 <vector185>:
.globl vector185
vector185:
  pushl $0
801067a3:	6a 00                	push   $0x0
  pushl $185
801067a5:	68 b9 00 00 00       	push   $0xb9
  jmp alltraps
801067aa:	e9 d8 f3 ff ff       	jmp    80105b87 <alltraps>

801067af <vector186>:
.globl vector186
vector186:
  pushl $0
801067af:	6a 00                	push   $0x0
  pushl $186
801067b1:	68 ba 00 00 00       	push   $0xba
  jmp alltraps
801067b6:	e9 cc f3 ff ff       	jmp    80105b87 <alltraps>

801067bb <vector187>:
.globl vector187
vector187:
  pushl $0
801067bb:	6a 00                	push   $0x0
  pushl $187
801067bd:	68 bb 00 00 00       	push   $0xbb
  jmp alltraps
801067c2:	e9 c0 f3 ff ff       	jmp    80105b87 <alltraps>

801067c7 <vector188>:
.globl vector188
vector188:
  pushl $0
801067c7:	6a 00                	push   $0x0
  pushl $188
801067c9:	68 bc 00 00 00       	push   $0xbc
  jmp alltraps
801067ce:	e9 b4 f3 ff ff       	jmp    80105b87 <alltraps>

801067d3 <vector189>:
.globl vector189
vector189:
  pushl $0
801067d3:	6a 00                	push   $0x0
  pushl $189
801067d5:	68 bd 00 00 00       	push   $0xbd
  jmp alltraps
801067da:	e9 a8 f3 ff ff       	jmp    80105b87 <alltraps>

801067df <vector190>:
.globl vector190
vector190:
  pushl $0
801067df:	6a 00                	push   $0x0
  pushl $190
801067e1:	68 be 00 00 00       	push   $0xbe
  jmp alltraps
801067e6:	e9 9c f3 ff ff       	jmp    80105b87 <alltraps>

801067eb <vector191>:
.globl vector191
vector191:
  pushl $0
801067eb:	6a 00                	push   $0x0
  pushl $191
801067ed:	68 bf 00 00 00       	push   $0xbf
  jmp alltraps
801067f2:	e9 90 f3 ff ff       	jmp    80105b87 <alltraps>

801067f7 <vector192>:
.globl vector192
vector192:
  pushl $0
801067f7:	6a 00                	push   $0x0
  pushl $192
801067f9:	68 c0 00 00 00       	push   $0xc0
  jmp alltraps
801067fe:	e9 84 f3 ff ff       	jmp    80105b87 <alltraps>

80106803 <vector193>:
.globl vector193
vector193:
  pushl $0
80106803:	6a 00                	push   $0x0
  pushl $193
80106805:	68 c1 00 00 00       	push   $0xc1
  jmp alltraps
8010680a:	e9 78 f3 ff ff       	jmp    80105b87 <alltraps>

8010680f <vector194>:
.globl vector194
vector194:
  pushl $0
8010680f:	6a 00                	push   $0x0
  pushl $194
80106811:	68 c2 00 00 00       	push   $0xc2
  jmp alltraps
80106816:	e9 6c f3 ff ff       	jmp    80105b87 <alltraps>

8010681b <vector195>:
.globl vector195
vector195:
  pushl $0
8010681b:	6a 00                	push   $0x0
  pushl $195
8010681d:	68 c3 00 00 00       	push   $0xc3
  jmp alltraps
80106822:	e9 60 f3 ff ff       	jmp    80105b87 <alltraps>

80106827 <vector196>:
.globl vector196
vector196:
  pushl $0
80106827:	6a 00                	push   $0x0
  pushl $196
80106829:	68 c4 00 00 00       	push   $0xc4
  jmp alltraps
8010682e:	e9 54 f3 ff ff       	jmp    80105b87 <alltraps>

80106833 <vector197>:
.globl vector197
vector197:
  pushl $0
80106833:	6a 00                	push   $0x0
  pushl $197
80106835:	68 c5 00 00 00       	push   $0xc5
  jmp alltraps
8010683a:	e9 48 f3 ff ff       	jmp    80105b87 <alltraps>

8010683f <vector198>:
.globl vector198
vector198:
  pushl $0
8010683f:	6a 00                	push   $0x0
  pushl $198
80106841:	68 c6 00 00 00       	push   $0xc6
  jmp alltraps
80106846:	e9 3c f3 ff ff       	jmp    80105b87 <alltraps>

8010684b <vector199>:
.globl vector199
vector199:
  pushl $0
8010684b:	6a 00                	push   $0x0
  pushl $199
8010684d:	68 c7 00 00 00       	push   $0xc7
  jmp alltraps
80106852:	e9 30 f3 ff ff       	jmp    80105b87 <alltraps>

80106857 <vector200>:
.globl vector200
vector200:
  pushl $0
80106857:	6a 00                	push   $0x0
  pushl $200
80106859:	68 c8 00 00 00       	push   $0xc8
  jmp alltraps
8010685e:	e9 24 f3 ff ff       	jmp    80105b87 <alltraps>

80106863 <vector201>:
.globl vector201
vector201:
  pushl $0
80106863:	6a 00                	push   $0x0
  pushl $201
80106865:	68 c9 00 00 00       	push   $0xc9
  jmp alltraps
8010686a:	e9 18 f3 ff ff       	jmp    80105b87 <alltraps>

8010686f <vector202>:
.globl vector202
vector202:
  pushl $0
8010686f:	6a 00                	push   $0x0
  pushl $202
80106871:	68 ca 00 00 00       	push   $0xca
  jmp alltraps
80106876:	e9 0c f3 ff ff       	jmp    80105b87 <alltraps>

8010687b <vector203>:
.globl vector203
vector203:
  pushl $0
8010687b:	6a 00                	push   $0x0
  pushl $203
8010687d:	68 cb 00 00 00       	push   $0xcb
  jmp alltraps
80106882:	e9 00 f3 ff ff       	jmp    80105b87 <alltraps>

80106887 <vector204>:
.globl vector204
vector204:
  pushl $0
80106887:	6a 00                	push   $0x0
  pushl $204
80106889:	68 cc 00 00 00       	push   $0xcc
  jmp alltraps
8010688e:	e9 f4 f2 ff ff       	jmp    80105b87 <alltraps>

80106893 <vector205>:
.globl vector205
vector205:
  pushl $0
80106893:	6a 00                	push   $0x0
  pushl $205
80106895:	68 cd 00 00 00       	push   $0xcd
  jmp alltraps
8010689a:	e9 e8 f2 ff ff       	jmp    80105b87 <alltraps>

8010689f <vector206>:
.globl vector206
vector206:
  pushl $0
8010689f:	6a 00                	push   $0x0
  pushl $206
801068a1:	68 ce 00 00 00       	push   $0xce
  jmp alltraps
801068a6:	e9 dc f2 ff ff       	jmp    80105b87 <alltraps>

801068ab <vector207>:
.globl vector207
vector207:
  pushl $0
801068ab:	6a 00                	push   $0x0
  pushl $207
801068ad:	68 cf 00 00 00       	push   $0xcf
  jmp alltraps
801068b2:	e9 d0 f2 ff ff       	jmp    80105b87 <alltraps>

801068b7 <vector208>:
.globl vector208
vector208:
  pushl $0
801068b7:	6a 00                	push   $0x0
  pushl $208
801068b9:	68 d0 00 00 00       	push   $0xd0
  jmp alltraps
801068be:	e9 c4 f2 ff ff       	jmp    80105b87 <alltraps>

801068c3 <vector209>:
.globl vector209
vector209:
  pushl $0
801068c3:	6a 00                	push   $0x0
  pushl $209
801068c5:	68 d1 00 00 00       	push   $0xd1
  jmp alltraps
801068ca:	e9 b8 f2 ff ff       	jmp    80105b87 <alltraps>

801068cf <vector210>:
.globl vector210
vector210:
  pushl $0
801068cf:	6a 00                	push   $0x0
  pushl $210
801068d1:	68 d2 00 00 00       	push   $0xd2
  jmp alltraps
801068d6:	e9 ac f2 ff ff       	jmp    80105b87 <alltraps>

801068db <vector211>:
.globl vector211
vector211:
  pushl $0
801068db:	6a 00                	push   $0x0
  pushl $211
801068dd:	68 d3 00 00 00       	push   $0xd3
  jmp alltraps
801068e2:	e9 a0 f2 ff ff       	jmp    80105b87 <alltraps>

801068e7 <vector212>:
.globl vector212
vector212:
  pushl $0
801068e7:	6a 00                	push   $0x0
  pushl $212
801068e9:	68 d4 00 00 00       	push   $0xd4
  jmp alltraps
801068ee:	e9 94 f2 ff ff       	jmp    80105b87 <alltraps>

801068f3 <vector213>:
.globl vector213
vector213:
  pushl $0
801068f3:	6a 00                	push   $0x0
  pushl $213
801068f5:	68 d5 00 00 00       	push   $0xd5
  jmp alltraps
801068fa:	e9 88 f2 ff ff       	jmp    80105b87 <alltraps>

801068ff <vector214>:
.globl vector214
vector214:
  pushl $0
801068ff:	6a 00                	push   $0x0
  pushl $214
80106901:	68 d6 00 00 00       	push   $0xd6
  jmp alltraps
80106906:	e9 7c f2 ff ff       	jmp    80105b87 <alltraps>

8010690b <vector215>:
.globl vector215
vector215:
  pushl $0
8010690b:	6a 00                	push   $0x0
  pushl $215
8010690d:	68 d7 00 00 00       	push   $0xd7
  jmp alltraps
80106912:	e9 70 f2 ff ff       	jmp    80105b87 <alltraps>

80106917 <vector216>:
.globl vector216
vector216:
  pushl $0
80106917:	6a 00                	push   $0x0
  pushl $216
80106919:	68 d8 00 00 00       	push   $0xd8
  jmp alltraps
8010691e:	e9 64 f2 ff ff       	jmp    80105b87 <alltraps>

80106923 <vector217>:
.globl vector217
vector217:
  pushl $0
80106923:	6a 00                	push   $0x0
  pushl $217
80106925:	68 d9 00 00 00       	push   $0xd9
  jmp alltraps
8010692a:	e9 58 f2 ff ff       	jmp    80105b87 <alltraps>

8010692f <vector218>:
.globl vector218
vector218:
  pushl $0
8010692f:	6a 00                	push   $0x0
  pushl $218
80106931:	68 da 00 00 00       	push   $0xda
  jmp alltraps
80106936:	e9 4c f2 ff ff       	jmp    80105b87 <alltraps>

8010693b <vector219>:
.globl vector219
vector219:
  pushl $0
8010693b:	6a 00                	push   $0x0
  pushl $219
8010693d:	68 db 00 00 00       	push   $0xdb
  jmp alltraps
80106942:	e9 40 f2 ff ff       	jmp    80105b87 <alltraps>

80106947 <vector220>:
.globl vector220
vector220:
  pushl $0
80106947:	6a 00                	push   $0x0
  pushl $220
80106949:	68 dc 00 00 00       	push   $0xdc
  jmp alltraps
8010694e:	e9 34 f2 ff ff       	jmp    80105b87 <alltraps>

80106953 <vector221>:
.globl vector221
vector221:
  pushl $0
80106953:	6a 00                	push   $0x0
  pushl $221
80106955:	68 dd 00 00 00       	push   $0xdd
  jmp alltraps
8010695a:	e9 28 f2 ff ff       	jmp    80105b87 <alltraps>

8010695f <vector222>:
.globl vector222
vector222:
  pushl $0
8010695f:	6a 00                	push   $0x0
  pushl $222
80106961:	68 de 00 00 00       	push   $0xde
  jmp alltraps
80106966:	e9 1c f2 ff ff       	jmp    80105b87 <alltraps>

8010696b <vector223>:
.globl vector223
vector223:
  pushl $0
8010696b:	6a 00                	push   $0x0
  pushl $223
8010696d:	68 df 00 00 00       	push   $0xdf
  jmp alltraps
80106972:	e9 10 f2 ff ff       	jmp    80105b87 <alltraps>

80106977 <vector224>:
.globl vector224
vector224:
  pushl $0
80106977:	6a 00                	push   $0x0
  pushl $224
80106979:	68 e0 00 00 00       	push   $0xe0
  jmp alltraps
8010697e:	e9 04 f2 ff ff       	jmp    80105b87 <alltraps>

80106983 <vector225>:
.globl vector225
vector225:
  pushl $0
80106983:	6a 00                	push   $0x0
  pushl $225
80106985:	68 e1 00 00 00       	push   $0xe1
  jmp alltraps
8010698a:	e9 f8 f1 ff ff       	jmp    80105b87 <alltraps>

8010698f <vector226>:
.globl vector226
vector226:
  pushl $0
8010698f:	6a 00                	push   $0x0
  pushl $226
80106991:	68 e2 00 00 00       	push   $0xe2
  jmp alltraps
80106996:	e9 ec f1 ff ff       	jmp    80105b87 <alltraps>

8010699b <vector227>:
.globl vector227
vector227:
  pushl $0
8010699b:	6a 00                	push   $0x0
  pushl $227
8010699d:	68 e3 00 00 00       	push   $0xe3
  jmp alltraps
801069a2:	e9 e0 f1 ff ff       	jmp    80105b87 <alltraps>

801069a7 <vector228>:
.globl vector228
vector228:
  pushl $0
801069a7:	6a 00                	push   $0x0
  pushl $228
801069a9:	68 e4 00 00 00       	push   $0xe4
  jmp alltraps
801069ae:	e9 d4 f1 ff ff       	jmp    80105b87 <alltraps>

801069b3 <vector229>:
.globl vector229
vector229:
  pushl $0
801069b3:	6a 00                	push   $0x0
  pushl $229
801069b5:	68 e5 00 00 00       	push   $0xe5
  jmp alltraps
801069ba:	e9 c8 f1 ff ff       	jmp    80105b87 <alltraps>

801069bf <vector230>:
.globl vector230
vector230:
  pushl $0
801069bf:	6a 00                	push   $0x0
  pushl $230
801069c1:	68 e6 00 00 00       	push   $0xe6
  jmp alltraps
801069c6:	e9 bc f1 ff ff       	jmp    80105b87 <alltraps>

801069cb <vector231>:
.globl vector231
vector231:
  pushl $0
801069cb:	6a 00                	push   $0x0
  pushl $231
801069cd:	68 e7 00 00 00       	push   $0xe7
  jmp alltraps
801069d2:	e9 b0 f1 ff ff       	jmp    80105b87 <alltraps>

801069d7 <vector232>:
.globl vector232
vector232:
  pushl $0
801069d7:	6a 00                	push   $0x0
  pushl $232
801069d9:	68 e8 00 00 00       	push   $0xe8
  jmp alltraps
801069de:	e9 a4 f1 ff ff       	jmp    80105b87 <alltraps>

801069e3 <vector233>:
.globl vector233
vector233:
  pushl $0
801069e3:	6a 00                	push   $0x0
  pushl $233
801069e5:	68 e9 00 00 00       	push   $0xe9
  jmp alltraps
801069ea:	e9 98 f1 ff ff       	jmp    80105b87 <alltraps>

801069ef <vector234>:
.globl vector234
vector234:
  pushl $0
801069ef:	6a 00                	push   $0x0
  pushl $234
801069f1:	68 ea 00 00 00       	push   $0xea
  jmp alltraps
801069f6:	e9 8c f1 ff ff       	jmp    80105b87 <alltraps>

801069fb <vector235>:
.globl vector235
vector235:
  pushl $0
801069fb:	6a 00                	push   $0x0
  pushl $235
801069fd:	68 eb 00 00 00       	push   $0xeb
  jmp alltraps
80106a02:	e9 80 f1 ff ff       	jmp    80105b87 <alltraps>

80106a07 <vector236>:
.globl vector236
vector236:
  pushl $0
80106a07:	6a 00                	push   $0x0
  pushl $236
80106a09:	68 ec 00 00 00       	push   $0xec
  jmp alltraps
80106a0e:	e9 74 f1 ff ff       	jmp    80105b87 <alltraps>

80106a13 <vector237>:
.globl vector237
vector237:
  pushl $0
80106a13:	6a 00                	push   $0x0
  pushl $237
80106a15:	68 ed 00 00 00       	push   $0xed
  jmp alltraps
80106a1a:	e9 68 f1 ff ff       	jmp    80105b87 <alltraps>

80106a1f <vector238>:
.globl vector238
vector238:
  pushl $0
80106a1f:	6a 00                	push   $0x0
  pushl $238
80106a21:	68 ee 00 00 00       	push   $0xee
  jmp alltraps
80106a26:	e9 5c f1 ff ff       	jmp    80105b87 <alltraps>

80106a2b <vector239>:
.globl vector239
vector239:
  pushl $0
80106a2b:	6a 00                	push   $0x0
  pushl $239
80106a2d:	68 ef 00 00 00       	push   $0xef
  jmp alltraps
80106a32:	e9 50 f1 ff ff       	jmp    80105b87 <alltraps>

80106a37 <vector240>:
.globl vector240
vector240:
  pushl $0
80106a37:	6a 00                	push   $0x0
  pushl $240
80106a39:	68 f0 00 00 00       	push   $0xf0
  jmp alltraps
80106a3e:	e9 44 f1 ff ff       	jmp    80105b87 <alltraps>

80106a43 <vector241>:
.globl vector241
vector241:
  pushl $0
80106a43:	6a 00                	push   $0x0
  pushl $241
80106a45:	68 f1 00 00 00       	push   $0xf1
  jmp alltraps
80106a4a:	e9 38 f1 ff ff       	jmp    80105b87 <alltraps>

80106a4f <vector242>:
.globl vector242
vector242:
  pushl $0
80106a4f:	6a 00                	push   $0x0
  pushl $242
80106a51:	68 f2 00 00 00       	push   $0xf2
  jmp alltraps
80106a56:	e9 2c f1 ff ff       	jmp    80105b87 <alltraps>

80106a5b <vector243>:
.globl vector243
vector243:
  pushl $0
80106a5b:	6a 00                	push   $0x0
  pushl $243
80106a5d:	68 f3 00 00 00       	push   $0xf3
  jmp alltraps
80106a62:	e9 20 f1 ff ff       	jmp    80105b87 <alltraps>

80106a67 <vector244>:
.globl vector244
vector244:
  pushl $0
80106a67:	6a 00                	push   $0x0
  pushl $244
80106a69:	68 f4 00 00 00       	push   $0xf4
  jmp alltraps
80106a6e:	e9 14 f1 ff ff       	jmp    80105b87 <alltraps>

80106a73 <vector245>:
.globl vector245
vector245:
  pushl $0
80106a73:	6a 00                	push   $0x0
  pushl $245
80106a75:	68 f5 00 00 00       	push   $0xf5
  jmp alltraps
80106a7a:	e9 08 f1 ff ff       	jmp    80105b87 <alltraps>

80106a7f <vector246>:
.globl vector246
vector246:
  pushl $0
80106a7f:	6a 00                	push   $0x0
  pushl $246
80106a81:	68 f6 00 00 00       	push   $0xf6
  jmp alltraps
80106a86:	e9 fc f0 ff ff       	jmp    80105b87 <alltraps>

80106a8b <vector247>:
.globl vector247
vector247:
  pushl $0
80106a8b:	6a 00                	push   $0x0
  pushl $247
80106a8d:	68 f7 00 00 00       	push   $0xf7
  jmp alltraps
80106a92:	e9 f0 f0 ff ff       	jmp    80105b87 <alltraps>

80106a97 <vector248>:
.globl vector248
vector248:
  pushl $0
80106a97:	6a 00                	push   $0x0
  pushl $248
80106a99:	68 f8 00 00 00       	push   $0xf8
  jmp alltraps
80106a9e:	e9 e4 f0 ff ff       	jmp    80105b87 <alltraps>

80106aa3 <vector249>:
.globl vector249
vector249:
  pushl $0
80106aa3:	6a 00                	push   $0x0
  pushl $249
80106aa5:	68 f9 00 00 00       	push   $0xf9
  jmp alltraps
80106aaa:	e9 d8 f0 ff ff       	jmp    80105b87 <alltraps>

80106aaf <vector250>:
.globl vector250
vector250:
  pushl $0
80106aaf:	6a 00                	push   $0x0
  pushl $250
80106ab1:	68 fa 00 00 00       	push   $0xfa
  jmp alltraps
80106ab6:	e9 cc f0 ff ff       	jmp    80105b87 <alltraps>

80106abb <vector251>:
.globl vector251
vector251:
  pushl $0
80106abb:	6a 00                	push   $0x0
  pushl $251
80106abd:	68 fb 00 00 00       	push   $0xfb
  jmp alltraps
80106ac2:	e9 c0 f0 ff ff       	jmp    80105b87 <alltraps>

80106ac7 <vector252>:
.globl vector252
vector252:
  pushl $0
80106ac7:	6a 00                	push   $0x0
  pushl $252
80106ac9:	68 fc 00 00 00       	push   $0xfc
  jmp alltraps
80106ace:	e9 b4 f0 ff ff       	jmp    80105b87 <alltraps>

80106ad3 <vector253>:
.globl vector253
vector253:
  pushl $0
80106ad3:	6a 00                	push   $0x0
  pushl $253
80106ad5:	68 fd 00 00 00       	push   $0xfd
  jmp alltraps
80106ada:	e9 a8 f0 ff ff       	jmp    80105b87 <alltraps>

80106adf <vector254>:
.globl vector254
vector254:
  pushl $0
80106adf:	6a 00                	push   $0x0
  pushl $254
80106ae1:	68 fe 00 00 00       	push   $0xfe
  jmp alltraps
80106ae6:	e9 9c f0 ff ff       	jmp    80105b87 <alltraps>

80106aeb <vector255>:
.globl vector255
vector255:
  pushl $0
80106aeb:	6a 00                	push   $0x0
  pushl $255
80106aed:	68 ff 00 00 00       	push   $0xff
  jmp alltraps
80106af2:	e9 90 f0 ff ff       	jmp    80105b87 <alltraps>
80106af7:	66 90                	xchg   %ax,%ax
80106af9:	66 90                	xchg   %ax,%ax
80106afb:	66 90                	xchg   %ax,%ax
80106afd:	66 90                	xchg   %ax,%ax
80106aff:	90                   	nop

80106b00 <walkpgdir>:
80106b00:	55                   	push   %ebp
80106b01:	89 e5                	mov    %esp,%ebp
80106b03:	57                   	push   %edi
80106b04:	56                   	push   %esi
80106b05:	53                   	push   %ebx
80106b06:	89 d3                	mov    %edx,%ebx
80106b08:	c1 ea 16             	shr    $0x16,%edx
80106b0b:	8d 3c 90             	lea    (%eax,%edx,4),%edi
80106b0e:	83 ec 0c             	sub    $0xc,%esp
80106b11:	8b 07                	mov    (%edi),%eax
80106b13:	a8 01                	test   $0x1,%al
80106b15:	74 29                	je     80106b40 <walkpgdir+0x40>
80106b17:	25 00 f0 ff ff       	and    $0xfffff000,%eax
80106b1c:	8d b0 00 00 00 80    	lea    -0x80000000(%eax),%esi
80106b22:	8d 65 f4             	lea    -0xc(%ebp),%esp
80106b25:	c1 eb 0a             	shr    $0xa,%ebx
80106b28:	81 e3 fc 0f 00 00    	and    $0xffc,%ebx
80106b2e:	8d 04 1e             	lea    (%esi,%ebx,1),%eax
80106b31:	5b                   	pop    %ebx
80106b32:	5e                   	pop    %esi
80106b33:	5f                   	pop    %edi
80106b34:	5d                   	pop    %ebp
80106b35:	c3                   	ret    
80106b36:	8d 76 00             	lea    0x0(%esi),%esi
80106b39:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80106b40:	85 c9                	test   %ecx,%ecx
80106b42:	74 2c                	je     80106b70 <walkpgdir+0x70>
80106b44:	e8 47 b9 ff ff       	call   80102490 <kalloc>
80106b49:	85 c0                	test   %eax,%eax
80106b4b:	89 c6                	mov    %eax,%esi
80106b4d:	74 21                	je     80106b70 <walkpgdir+0x70>
80106b4f:	83 ec 04             	sub    $0x4,%esp
80106b52:	68 00 10 00 00       	push   $0x1000
80106b57:	6a 00                	push   $0x0
80106b59:	50                   	push   %eax
80106b5a:	e8 81 dd ff ff       	call   801048e0 <memset>
80106b5f:	8d 86 00 00 00 80    	lea    -0x80000000(%esi),%eax
80106b65:	83 c4 10             	add    $0x10,%esp
80106b68:	83 c8 07             	or     $0x7,%eax
80106b6b:	89 07                	mov    %eax,(%edi)
80106b6d:	eb b3                	jmp    80106b22 <walkpgdir+0x22>
80106b6f:	90                   	nop
80106b70:	8d 65 f4             	lea    -0xc(%ebp),%esp
80106b73:	31 c0                	xor    %eax,%eax
80106b75:	5b                   	pop    %ebx
80106b76:	5e                   	pop    %esi
80106b77:	5f                   	pop    %edi
80106b78:	5d                   	pop    %ebp
80106b79:	c3                   	ret    
80106b7a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

80106b80 <mappages>:
80106b80:	55                   	push   %ebp
80106b81:	89 e5                	mov    %esp,%ebp
80106b83:	57                   	push   %edi
80106b84:	56                   	push   %esi
80106b85:	53                   	push   %ebx
80106b86:	89 d3                	mov    %edx,%ebx
80106b88:	81 e3 00 f0 ff ff    	and    $0xfffff000,%ebx
80106b8e:	83 ec 1c             	sub    $0x1c,%esp
80106b91:	89 45 e4             	mov    %eax,-0x1c(%ebp)
80106b94:	8d 44 0a ff          	lea    -0x1(%edx,%ecx,1),%eax
80106b98:	8b 7d 08             	mov    0x8(%ebp),%edi
80106b9b:	25 00 f0 ff ff       	and    $0xfffff000,%eax
80106ba0:	89 45 e0             	mov    %eax,-0x20(%ebp)
80106ba3:	8b 45 0c             	mov    0xc(%ebp),%eax
80106ba6:	29 df                	sub    %ebx,%edi
80106ba8:	83 c8 01             	or     $0x1,%eax
80106bab:	89 45 dc             	mov    %eax,-0x24(%ebp)
80106bae:	eb 15                	jmp    80106bc5 <mappages+0x45>
80106bb0:	f6 00 01             	testb  $0x1,(%eax)
80106bb3:	75 45                	jne    80106bfa <mappages+0x7a>
80106bb5:	0b 75 dc             	or     -0x24(%ebp),%esi
80106bb8:	3b 5d e0             	cmp    -0x20(%ebp),%ebx
80106bbb:	89 30                	mov    %esi,(%eax)
80106bbd:	74 31                	je     80106bf0 <mappages+0x70>
80106bbf:	81 c3 00 10 00 00    	add    $0x1000,%ebx
80106bc5:	8b 45 e4             	mov    -0x1c(%ebp),%eax
80106bc8:	b9 01 00 00 00       	mov    $0x1,%ecx
80106bcd:	89 da                	mov    %ebx,%edx
80106bcf:	8d 34 3b             	lea    (%ebx,%edi,1),%esi
80106bd2:	e8 29 ff ff ff       	call   80106b00 <walkpgdir>
80106bd7:	85 c0                	test   %eax,%eax
80106bd9:	75 d5                	jne    80106bb0 <mappages+0x30>
80106bdb:	8d 65 f4             	lea    -0xc(%ebp),%esp
80106bde:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80106be3:	5b                   	pop    %ebx
80106be4:	5e                   	pop    %esi
80106be5:	5f                   	pop    %edi
80106be6:	5d                   	pop    %ebp
80106be7:	c3                   	ret    
80106be8:	90                   	nop
80106be9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80106bf0:	8d 65 f4             	lea    -0xc(%ebp),%esp
80106bf3:	31 c0                	xor    %eax,%eax
80106bf5:	5b                   	pop    %ebx
80106bf6:	5e                   	pop    %esi
80106bf7:	5f                   	pop    %edi
80106bf8:	5d                   	pop    %ebp
80106bf9:	c3                   	ret    
80106bfa:	83 ec 0c             	sub    $0xc,%esp
80106bfd:	68 14 7e 10 80       	push   $0x80107e14
80106c02:	e8 69 97 ff ff       	call   80100370 <panic>
80106c07:	89 f6                	mov    %esi,%esi
80106c09:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80106c10 <deallocuvm.part.0>:
80106c10:	55                   	push   %ebp
80106c11:	89 e5                	mov    %esp,%ebp
80106c13:	57                   	push   %edi
80106c14:	56                   	push   %esi
80106c15:	53                   	push   %ebx
80106c16:	8d 99 ff 0f 00 00    	lea    0xfff(%ecx),%ebx
80106c1c:	89 c7                	mov    %eax,%edi
80106c1e:	81 e3 00 f0 ff ff    	and    $0xfffff000,%ebx
80106c24:	83 ec 1c             	sub    $0x1c,%esp
80106c27:	89 4d e0             	mov    %ecx,-0x20(%ebp)
80106c2a:	39 d3                	cmp    %edx,%ebx
80106c2c:	73 66                	jae    80106c94 <deallocuvm.part.0+0x84>
80106c2e:	89 d6                	mov    %edx,%esi
80106c30:	eb 3d                	jmp    80106c6f <deallocuvm.part.0+0x5f>
80106c32:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80106c38:	8b 10                	mov    (%eax),%edx
80106c3a:	f6 c2 01             	test   $0x1,%dl
80106c3d:	74 26                	je     80106c65 <deallocuvm.part.0+0x55>
80106c3f:	81 e2 00 f0 ff ff    	and    $0xfffff000,%edx
80106c45:	74 58                	je     80106c9f <deallocuvm.part.0+0x8f>
80106c47:	83 ec 0c             	sub    $0xc,%esp
80106c4a:	81 c2 00 00 00 80    	add    $0x80000000,%edx
80106c50:	89 45 e4             	mov    %eax,-0x1c(%ebp)
80106c53:	52                   	push   %edx
80106c54:	e8 87 b6 ff ff       	call   801022e0 <kfree>
80106c59:	8b 45 e4             	mov    -0x1c(%ebp),%eax
80106c5c:	83 c4 10             	add    $0x10,%esp
80106c5f:	c7 00 00 00 00 00    	movl   $0x0,(%eax)
80106c65:	81 c3 00 10 00 00    	add    $0x1000,%ebx
80106c6b:	39 f3                	cmp    %esi,%ebx
80106c6d:	73 25                	jae    80106c94 <deallocuvm.part.0+0x84>
80106c6f:	31 c9                	xor    %ecx,%ecx
80106c71:	89 da                	mov    %ebx,%edx
80106c73:	89 f8                	mov    %edi,%eax
80106c75:	e8 86 fe ff ff       	call   80106b00 <walkpgdir>
80106c7a:	85 c0                	test   %eax,%eax
80106c7c:	75 ba                	jne    80106c38 <deallocuvm.part.0+0x28>
80106c7e:	81 e3 00 00 c0 ff    	and    $0xffc00000,%ebx
80106c84:	81 c3 00 f0 3f 00    	add    $0x3ff000,%ebx
80106c8a:	81 c3 00 10 00 00    	add    $0x1000,%ebx
80106c90:	39 f3                	cmp    %esi,%ebx
80106c92:	72 db                	jb     80106c6f <deallocuvm.part.0+0x5f>
80106c94:	8b 45 e0             	mov    -0x20(%ebp),%eax
80106c97:	8d 65 f4             	lea    -0xc(%ebp),%esp
80106c9a:	5b                   	pop    %ebx
80106c9b:	5e                   	pop    %esi
80106c9c:	5f                   	pop    %edi
80106c9d:	5d                   	pop    %ebp
80106c9e:	c3                   	ret    
80106c9f:	83 ec 0c             	sub    $0xc,%esp
80106ca2:	68 e6 76 10 80       	push   $0x801076e6
80106ca7:	e8 c4 96 ff ff       	call   80100370 <panic>
80106cac:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80106cb0 <seginit>:
80106cb0:	55                   	push   %ebp
80106cb1:	89 e5                	mov    %esp,%ebp
80106cb3:	83 ec 18             	sub    $0x18,%esp
80106cb6:	e8 95 cb ff ff       	call   80103850 <cpuid>
80106cbb:	69 c0 b0 00 00 00    	imul   $0xb0,%eax,%eax
80106cc1:	31 c9                	xor    %ecx,%ecx
80106cc3:	ba ff ff ff ff       	mov    $0xffffffff,%edx
80106cc8:	66 89 90 f8 27 11 80 	mov    %dx,-0x7feed808(%eax)
80106ccf:	66 89 88 fa 27 11 80 	mov    %cx,-0x7feed806(%eax)
80106cd6:	ba ff ff ff ff       	mov    $0xffffffff,%edx
80106cdb:	31 c9                	xor    %ecx,%ecx
80106cdd:	66 89 90 00 28 11 80 	mov    %dx,-0x7feed800(%eax)
80106ce4:	ba ff ff ff ff       	mov    $0xffffffff,%edx
80106ce9:	66 89 88 02 28 11 80 	mov    %cx,-0x7feed7fe(%eax)
80106cf0:	31 c9                	xor    %ecx,%ecx
80106cf2:	66 89 90 08 28 11 80 	mov    %dx,-0x7feed7f8(%eax)
80106cf9:	66 89 88 0a 28 11 80 	mov    %cx,-0x7feed7f6(%eax)
80106d00:	ba ff ff ff ff       	mov    $0xffffffff,%edx
80106d05:	31 c9                	xor    %ecx,%ecx
80106d07:	66 89 90 10 28 11 80 	mov    %dx,-0x7feed7f0(%eax)
80106d0e:	c6 80 fc 27 11 80 00 	movb   $0x0,-0x7feed804(%eax)
80106d15:	ba 2f 00 00 00       	mov    $0x2f,%edx
80106d1a:	c6 80 fd 27 11 80 9a 	movb   $0x9a,-0x7feed803(%eax)
80106d21:	c6 80 fe 27 11 80 cf 	movb   $0xcf,-0x7feed802(%eax)
80106d28:	c6 80 ff 27 11 80 00 	movb   $0x0,-0x7feed801(%eax)
80106d2f:	c6 80 04 28 11 80 00 	movb   $0x0,-0x7feed7fc(%eax)
80106d36:	c6 80 05 28 11 80 92 	movb   $0x92,-0x7feed7fb(%eax)
80106d3d:	c6 80 06 28 11 80 cf 	movb   $0xcf,-0x7feed7fa(%eax)
80106d44:	c6 80 07 28 11 80 00 	movb   $0x0,-0x7feed7f9(%eax)
80106d4b:	c6 80 0c 28 11 80 00 	movb   $0x0,-0x7feed7f4(%eax)
80106d52:	c6 80 0d 28 11 80 fa 	movb   $0xfa,-0x7feed7f3(%eax)
80106d59:	c6 80 0e 28 11 80 cf 	movb   $0xcf,-0x7feed7f2(%eax)
80106d60:	c6 80 0f 28 11 80 00 	movb   $0x0,-0x7feed7f1(%eax)
80106d67:	66 89 88 12 28 11 80 	mov    %cx,-0x7feed7ee(%eax)
80106d6e:	c6 80 14 28 11 80 00 	movb   $0x0,-0x7feed7ec(%eax)
80106d75:	c6 80 15 28 11 80 f2 	movb   $0xf2,-0x7feed7eb(%eax)
80106d7c:	c6 80 16 28 11 80 cf 	movb   $0xcf,-0x7feed7ea(%eax)
80106d83:	c6 80 17 28 11 80 00 	movb   $0x0,-0x7feed7e9(%eax)
80106d8a:	05 f0 27 11 80       	add    $0x801127f0,%eax
80106d8f:	66 89 55 f2          	mov    %dx,-0xe(%ebp)
80106d93:	66 89 45 f4          	mov    %ax,-0xc(%ebp)
80106d97:	c1 e8 10             	shr    $0x10,%eax
80106d9a:	66 89 45 f6          	mov    %ax,-0xa(%ebp)
80106d9e:	8d 45 f2             	lea    -0xe(%ebp),%eax
80106da1:	0f 01 10             	lgdtl  (%eax)
80106da4:	c9                   	leave  
80106da5:	c3                   	ret    
80106da6:	8d 76 00             	lea    0x0(%esi),%esi
80106da9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80106db0 <switchkvm>:
80106db0:	a1 a4 5b 11 80       	mov    0x80115ba4,%eax
80106db5:	55                   	push   %ebp
80106db6:	89 e5                	mov    %esp,%ebp
80106db8:	05 00 00 00 80       	add    $0x80000000,%eax
80106dbd:	0f 22 d8             	mov    %eax,%cr3
80106dc0:	5d                   	pop    %ebp
80106dc1:	c3                   	ret    
80106dc2:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80106dc9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80106dd0 <switchuvm>:
80106dd0:	55                   	push   %ebp
80106dd1:	89 e5                	mov    %esp,%ebp
80106dd3:	57                   	push   %edi
80106dd4:	56                   	push   %esi
80106dd5:	53                   	push   %ebx
80106dd6:	83 ec 1c             	sub    $0x1c,%esp
80106dd9:	8b 75 08             	mov    0x8(%ebp),%esi
80106ddc:	85 f6                	test   %esi,%esi
80106dde:	0f 84 cd 00 00 00    	je     80106eb1 <switchuvm+0xe1>
80106de4:	8b 46 08             	mov    0x8(%esi),%eax
80106de7:	85 c0                	test   %eax,%eax
80106de9:	0f 84 dc 00 00 00    	je     80106ecb <switchuvm+0xfb>
80106def:	8b 7e 04             	mov    0x4(%esi),%edi
80106df2:	85 ff                	test   %edi,%edi
80106df4:	0f 84 c4 00 00 00    	je     80106ebe <switchuvm+0xee>
80106dfa:	e8 31 d9 ff ff       	call   80104730 <pushcli>
80106dff:	e8 cc c9 ff ff       	call   801037d0 <mycpu>
80106e04:	89 c3                	mov    %eax,%ebx
80106e06:	e8 c5 c9 ff ff       	call   801037d0 <mycpu>
80106e0b:	89 c7                	mov    %eax,%edi
80106e0d:	e8 be c9 ff ff       	call   801037d0 <mycpu>
80106e12:	89 45 e4             	mov    %eax,-0x1c(%ebp)
80106e15:	83 c7 08             	add    $0x8,%edi
80106e18:	e8 b3 c9 ff ff       	call   801037d0 <mycpu>
80106e1d:	8b 4d e4             	mov    -0x1c(%ebp),%ecx
80106e20:	83 c0 08             	add    $0x8,%eax
80106e23:	ba 67 00 00 00       	mov    $0x67,%edx
80106e28:	c1 e8 18             	shr    $0x18,%eax
80106e2b:	66 89 93 98 00 00 00 	mov    %dx,0x98(%ebx)
80106e32:	66 89 bb 9a 00 00 00 	mov    %di,0x9a(%ebx)
80106e39:	c6 83 9d 00 00 00 99 	movb   $0x99,0x9d(%ebx)
80106e40:	c6 83 9e 00 00 00 40 	movb   $0x40,0x9e(%ebx)
80106e47:	83 c1 08             	add    $0x8,%ecx
80106e4a:	88 83 9f 00 00 00    	mov    %al,0x9f(%ebx)
80106e50:	c1 e9 10             	shr    $0x10,%ecx
80106e53:	88 8b 9c 00 00 00    	mov    %cl,0x9c(%ebx)
80106e59:	bb ff ff ff ff       	mov    $0xffffffff,%ebx
80106e5e:	e8 6d c9 ff ff       	call   801037d0 <mycpu>
80106e63:	80 a0 9d 00 00 00 ef 	andb   $0xef,0x9d(%eax)
80106e6a:	e8 61 c9 ff ff       	call   801037d0 <mycpu>
80106e6f:	b9 10 00 00 00       	mov    $0x10,%ecx
80106e74:	66 89 48 10          	mov    %cx,0x10(%eax)
80106e78:	e8 53 c9 ff ff       	call   801037d0 <mycpu>
80106e7d:	8b 56 08             	mov    0x8(%esi),%edx
80106e80:	8d 8a 00 10 00 00    	lea    0x1000(%edx),%ecx
80106e86:	89 48 0c             	mov    %ecx,0xc(%eax)
80106e89:	e8 42 c9 ff ff       	call   801037d0 <mycpu>
80106e8e:	66 89 58 6e          	mov    %bx,0x6e(%eax)
80106e92:	b8 28 00 00 00       	mov    $0x28,%eax
80106e97:	0f 00 d8             	ltr    %ax
80106e9a:	8b 46 04             	mov    0x4(%esi),%eax
80106e9d:	05 00 00 00 80       	add    $0x80000000,%eax
80106ea2:	0f 22 d8             	mov    %eax,%cr3
80106ea5:	8d 65 f4             	lea    -0xc(%ebp),%esp
80106ea8:	5b                   	pop    %ebx
80106ea9:	5e                   	pop    %esi
80106eaa:	5f                   	pop    %edi
80106eab:	5d                   	pop    %ebp
80106eac:	e9 6f d9 ff ff       	jmp    80104820 <popcli>
80106eb1:	83 ec 0c             	sub    $0xc,%esp
80106eb4:	68 1a 7e 10 80       	push   $0x80107e1a
80106eb9:	e8 b2 94 ff ff       	call   80100370 <panic>
80106ebe:	83 ec 0c             	sub    $0xc,%esp
80106ec1:	68 45 7e 10 80       	push   $0x80107e45
80106ec6:	e8 a5 94 ff ff       	call   80100370 <panic>
80106ecb:	83 ec 0c             	sub    $0xc,%esp
80106ece:	68 30 7e 10 80       	push   $0x80107e30
80106ed3:	e8 98 94 ff ff       	call   80100370 <panic>
80106ed8:	90                   	nop
80106ed9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

80106ee0 <inituvm>:
80106ee0:	55                   	push   %ebp
80106ee1:	89 e5                	mov    %esp,%ebp
80106ee3:	57                   	push   %edi
80106ee4:	56                   	push   %esi
80106ee5:	53                   	push   %ebx
80106ee6:	83 ec 1c             	sub    $0x1c,%esp
80106ee9:	8b 75 10             	mov    0x10(%ebp),%esi
80106eec:	8b 45 08             	mov    0x8(%ebp),%eax
80106eef:	8b 7d 0c             	mov    0xc(%ebp),%edi
80106ef2:	81 fe ff 0f 00 00    	cmp    $0xfff,%esi
80106ef8:	89 45 e4             	mov    %eax,-0x1c(%ebp)
80106efb:	77 49                	ja     80106f46 <inituvm+0x66>
80106efd:	e8 8e b5 ff ff       	call   80102490 <kalloc>
80106f02:	83 ec 04             	sub    $0x4,%esp
80106f05:	89 c3                	mov    %eax,%ebx
80106f07:	68 00 10 00 00       	push   $0x1000
80106f0c:	6a 00                	push   $0x0
80106f0e:	50                   	push   %eax
80106f0f:	e8 cc d9 ff ff       	call   801048e0 <memset>
80106f14:	58                   	pop    %eax
80106f15:	8d 83 00 00 00 80    	lea    -0x80000000(%ebx),%eax
80106f1b:	b9 00 10 00 00       	mov    $0x1000,%ecx
80106f20:	5a                   	pop    %edx
80106f21:	6a 06                	push   $0x6
80106f23:	50                   	push   %eax
80106f24:	31 d2                	xor    %edx,%edx
80106f26:	8b 45 e4             	mov    -0x1c(%ebp),%eax
80106f29:	e8 52 fc ff ff       	call   80106b80 <mappages>
80106f2e:	89 75 10             	mov    %esi,0x10(%ebp)
80106f31:	89 7d 0c             	mov    %edi,0xc(%ebp)
80106f34:	83 c4 10             	add    $0x10,%esp
80106f37:	89 5d 08             	mov    %ebx,0x8(%ebp)
80106f3a:	8d 65 f4             	lea    -0xc(%ebp),%esp
80106f3d:	5b                   	pop    %ebx
80106f3e:	5e                   	pop    %esi
80106f3f:	5f                   	pop    %edi
80106f40:	5d                   	pop    %ebp
80106f41:	e9 4a da ff ff       	jmp    80104990 <memmove>
80106f46:	83 ec 0c             	sub    $0xc,%esp
80106f49:	68 59 7e 10 80       	push   $0x80107e59
80106f4e:	e8 1d 94 ff ff       	call   80100370 <panic>
80106f53:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80106f59:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80106f60 <loaduvm>:
80106f60:	55                   	push   %ebp
80106f61:	89 e5                	mov    %esp,%ebp
80106f63:	57                   	push   %edi
80106f64:	56                   	push   %esi
80106f65:	53                   	push   %ebx
80106f66:	83 ec 0c             	sub    $0xc,%esp
80106f69:	f7 45 0c ff 0f 00 00 	testl  $0xfff,0xc(%ebp)
80106f70:	0f 85 91 00 00 00    	jne    80107007 <loaduvm+0xa7>
80106f76:	8b 75 18             	mov    0x18(%ebp),%esi
80106f79:	31 db                	xor    %ebx,%ebx
80106f7b:	85 f6                	test   %esi,%esi
80106f7d:	75 1a                	jne    80106f99 <loaduvm+0x39>
80106f7f:	eb 6f                	jmp    80106ff0 <loaduvm+0x90>
80106f81:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80106f88:	81 c3 00 10 00 00    	add    $0x1000,%ebx
80106f8e:	81 ee 00 10 00 00    	sub    $0x1000,%esi
80106f94:	39 5d 18             	cmp    %ebx,0x18(%ebp)
80106f97:	76 57                	jbe    80106ff0 <loaduvm+0x90>
80106f99:	8b 55 0c             	mov    0xc(%ebp),%edx
80106f9c:	8b 45 08             	mov    0x8(%ebp),%eax
80106f9f:	31 c9                	xor    %ecx,%ecx
80106fa1:	01 da                	add    %ebx,%edx
80106fa3:	e8 58 fb ff ff       	call   80106b00 <walkpgdir>
80106fa8:	85 c0                	test   %eax,%eax
80106faa:	74 4e                	je     80106ffa <loaduvm+0x9a>
80106fac:	8b 00                	mov    (%eax),%eax
80106fae:	8b 4d 14             	mov    0x14(%ebp),%ecx
80106fb1:	bf 00 10 00 00       	mov    $0x1000,%edi
80106fb6:	25 00 f0 ff ff       	and    $0xfffff000,%eax
80106fbb:	81 fe ff 0f 00 00    	cmp    $0xfff,%esi
80106fc1:	0f 46 fe             	cmovbe %esi,%edi
80106fc4:	01 d9                	add    %ebx,%ecx
80106fc6:	05 00 00 00 80       	add    $0x80000000,%eax
80106fcb:	57                   	push   %edi
80106fcc:	51                   	push   %ecx
80106fcd:	50                   	push   %eax
80106fce:	ff 75 10             	pushl  0x10(%ebp)
80106fd1:	e8 7a a9 ff ff       	call   80101950 <readi>
80106fd6:	83 c4 10             	add    $0x10,%esp
80106fd9:	39 c7                	cmp    %eax,%edi
80106fdb:	74 ab                	je     80106f88 <loaduvm+0x28>
80106fdd:	8d 65 f4             	lea    -0xc(%ebp),%esp
80106fe0:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80106fe5:	5b                   	pop    %ebx
80106fe6:	5e                   	pop    %esi
80106fe7:	5f                   	pop    %edi
80106fe8:	5d                   	pop    %ebp
80106fe9:	c3                   	ret    
80106fea:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80106ff0:	8d 65 f4             	lea    -0xc(%ebp),%esp
80106ff3:	31 c0                	xor    %eax,%eax
80106ff5:	5b                   	pop    %ebx
80106ff6:	5e                   	pop    %esi
80106ff7:	5f                   	pop    %edi
80106ff8:	5d                   	pop    %ebp
80106ff9:	c3                   	ret    
80106ffa:	83 ec 0c             	sub    $0xc,%esp
80106ffd:	68 73 7e 10 80       	push   $0x80107e73
80107002:	e8 69 93 ff ff       	call   80100370 <panic>
80107007:	83 ec 0c             	sub    $0xc,%esp
8010700a:	68 14 7f 10 80       	push   $0x80107f14
8010700f:	e8 5c 93 ff ff       	call   80100370 <panic>
80107014:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
8010701a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

80107020 <allocuvm>:
80107020:	55                   	push   %ebp
80107021:	89 e5                	mov    %esp,%ebp
80107023:	57                   	push   %edi
80107024:	56                   	push   %esi
80107025:	53                   	push   %ebx
80107026:	83 ec 0c             	sub    $0xc,%esp
80107029:	8b 7d 10             	mov    0x10(%ebp),%edi
8010702c:	85 ff                	test   %edi,%edi
8010702e:	0f 88 ca 00 00 00    	js     801070fe <allocuvm+0xde>
80107034:	3b 7d 0c             	cmp    0xc(%ebp),%edi
80107037:	8b 45 0c             	mov    0xc(%ebp),%eax
8010703a:	0f 82 82 00 00 00    	jb     801070c2 <allocuvm+0xa2>
80107040:	8d 98 ff 0f 00 00    	lea    0xfff(%eax),%ebx
80107046:	81 e3 00 f0 ff ff    	and    $0xfffff000,%ebx
8010704c:	39 df                	cmp    %ebx,%edi
8010704e:	77 43                	ja     80107093 <allocuvm+0x73>
80107050:	e9 bb 00 00 00       	jmp    80107110 <allocuvm+0xf0>
80107055:	8d 76 00             	lea    0x0(%esi),%esi
80107058:	83 ec 04             	sub    $0x4,%esp
8010705b:	68 00 10 00 00       	push   $0x1000
80107060:	6a 00                	push   $0x0
80107062:	50                   	push   %eax
80107063:	e8 78 d8 ff ff       	call   801048e0 <memset>
80107068:	58                   	pop    %eax
80107069:	8d 86 00 00 00 80    	lea    -0x80000000(%esi),%eax
8010706f:	b9 00 10 00 00       	mov    $0x1000,%ecx
80107074:	5a                   	pop    %edx
80107075:	6a 06                	push   $0x6
80107077:	50                   	push   %eax
80107078:	89 da                	mov    %ebx,%edx
8010707a:	8b 45 08             	mov    0x8(%ebp),%eax
8010707d:	e8 fe fa ff ff       	call   80106b80 <mappages>
80107082:	83 c4 10             	add    $0x10,%esp
80107085:	85 c0                	test   %eax,%eax
80107087:	78 47                	js     801070d0 <allocuvm+0xb0>
80107089:	81 c3 00 10 00 00    	add    $0x1000,%ebx
8010708f:	39 df                	cmp    %ebx,%edi
80107091:	76 7d                	jbe    80107110 <allocuvm+0xf0>
80107093:	e8 f8 b3 ff ff       	call   80102490 <kalloc>
80107098:	85 c0                	test   %eax,%eax
8010709a:	89 c6                	mov    %eax,%esi
8010709c:	75 ba                	jne    80107058 <allocuvm+0x38>
8010709e:	83 ec 0c             	sub    $0xc,%esp
801070a1:	68 91 7e 10 80       	push   $0x80107e91
801070a6:	e8 b5 95 ff ff       	call   80100660 <cprintf>
801070ab:	83 c4 10             	add    $0x10,%esp
801070ae:	3b 7d 0c             	cmp    0xc(%ebp),%edi
801070b1:	76 4b                	jbe    801070fe <allocuvm+0xde>
801070b3:	8b 4d 0c             	mov    0xc(%ebp),%ecx
801070b6:	8b 45 08             	mov    0x8(%ebp),%eax
801070b9:	89 fa                	mov    %edi,%edx
801070bb:	e8 50 fb ff ff       	call   80106c10 <deallocuvm.part.0>
801070c0:	31 c0                	xor    %eax,%eax
801070c2:	8d 65 f4             	lea    -0xc(%ebp),%esp
801070c5:	5b                   	pop    %ebx
801070c6:	5e                   	pop    %esi
801070c7:	5f                   	pop    %edi
801070c8:	5d                   	pop    %ebp
801070c9:	c3                   	ret    
801070ca:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
801070d0:	83 ec 0c             	sub    $0xc,%esp
801070d3:	68 a9 7e 10 80       	push   $0x80107ea9
801070d8:	e8 83 95 ff ff       	call   80100660 <cprintf>
801070dd:	83 c4 10             	add    $0x10,%esp
801070e0:	3b 7d 0c             	cmp    0xc(%ebp),%edi
801070e3:	76 0d                	jbe    801070f2 <allocuvm+0xd2>
801070e5:	8b 4d 0c             	mov    0xc(%ebp),%ecx
801070e8:	8b 45 08             	mov    0x8(%ebp),%eax
801070eb:	89 fa                	mov    %edi,%edx
801070ed:	e8 1e fb ff ff       	call   80106c10 <deallocuvm.part.0>
801070f2:	83 ec 0c             	sub    $0xc,%esp
801070f5:	56                   	push   %esi
801070f6:	e8 e5 b1 ff ff       	call   801022e0 <kfree>
801070fb:	83 c4 10             	add    $0x10,%esp
801070fe:	8d 65 f4             	lea    -0xc(%ebp),%esp
80107101:	31 c0                	xor    %eax,%eax
80107103:	5b                   	pop    %ebx
80107104:	5e                   	pop    %esi
80107105:	5f                   	pop    %edi
80107106:	5d                   	pop    %ebp
80107107:	c3                   	ret    
80107108:	90                   	nop
80107109:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80107110:	8d 65 f4             	lea    -0xc(%ebp),%esp
80107113:	89 f8                	mov    %edi,%eax
80107115:	5b                   	pop    %ebx
80107116:	5e                   	pop    %esi
80107117:	5f                   	pop    %edi
80107118:	5d                   	pop    %ebp
80107119:	c3                   	ret    
8010711a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

80107120 <deallocuvm>:
80107120:	55                   	push   %ebp
80107121:	89 e5                	mov    %esp,%ebp
80107123:	8b 55 0c             	mov    0xc(%ebp),%edx
80107126:	8b 4d 10             	mov    0x10(%ebp),%ecx
80107129:	8b 45 08             	mov    0x8(%ebp),%eax
8010712c:	39 d1                	cmp    %edx,%ecx
8010712e:	73 10                	jae    80107140 <deallocuvm+0x20>
80107130:	5d                   	pop    %ebp
80107131:	e9 da fa ff ff       	jmp    80106c10 <deallocuvm.part.0>
80107136:	8d 76 00             	lea    0x0(%esi),%esi
80107139:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
80107140:	89 d0                	mov    %edx,%eax
80107142:	5d                   	pop    %ebp
80107143:	c3                   	ret    
80107144:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
8010714a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

80107150 <freevm>:
80107150:	55                   	push   %ebp
80107151:	89 e5                	mov    %esp,%ebp
80107153:	57                   	push   %edi
80107154:	56                   	push   %esi
80107155:	53                   	push   %ebx
80107156:	83 ec 0c             	sub    $0xc,%esp
80107159:	8b 75 08             	mov    0x8(%ebp),%esi
8010715c:	85 f6                	test   %esi,%esi
8010715e:	74 59                	je     801071b9 <freevm+0x69>
80107160:	31 c9                	xor    %ecx,%ecx
80107162:	ba 00 00 00 80       	mov    $0x80000000,%edx
80107167:	89 f0                	mov    %esi,%eax
80107169:	e8 a2 fa ff ff       	call   80106c10 <deallocuvm.part.0>
8010716e:	89 f3                	mov    %esi,%ebx
80107170:	8d be 00 10 00 00    	lea    0x1000(%esi),%edi
80107176:	eb 0f                	jmp    80107187 <freevm+0x37>
80107178:	90                   	nop
80107179:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80107180:	83 c3 04             	add    $0x4,%ebx
80107183:	39 fb                	cmp    %edi,%ebx
80107185:	74 23                	je     801071aa <freevm+0x5a>
80107187:	8b 03                	mov    (%ebx),%eax
80107189:	a8 01                	test   $0x1,%al
8010718b:	74 f3                	je     80107180 <freevm+0x30>
8010718d:	25 00 f0 ff ff       	and    $0xfffff000,%eax
80107192:	83 ec 0c             	sub    $0xc,%esp
80107195:	83 c3 04             	add    $0x4,%ebx
80107198:	05 00 00 00 80       	add    $0x80000000,%eax
8010719d:	50                   	push   %eax
8010719e:	e8 3d b1 ff ff       	call   801022e0 <kfree>
801071a3:	83 c4 10             	add    $0x10,%esp
801071a6:	39 fb                	cmp    %edi,%ebx
801071a8:	75 dd                	jne    80107187 <freevm+0x37>
801071aa:	89 75 08             	mov    %esi,0x8(%ebp)
801071ad:	8d 65 f4             	lea    -0xc(%ebp),%esp
801071b0:	5b                   	pop    %ebx
801071b1:	5e                   	pop    %esi
801071b2:	5f                   	pop    %edi
801071b3:	5d                   	pop    %ebp
801071b4:	e9 27 b1 ff ff       	jmp    801022e0 <kfree>
801071b9:	83 ec 0c             	sub    $0xc,%esp
801071bc:	68 c5 7e 10 80       	push   $0x80107ec5
801071c1:	e8 aa 91 ff ff       	call   80100370 <panic>
801071c6:	8d 76 00             	lea    0x0(%esi),%esi
801071c9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801071d0 <setupkvm>:
801071d0:	55                   	push   %ebp
801071d1:	89 e5                	mov    %esp,%ebp
801071d3:	56                   	push   %esi
801071d4:	53                   	push   %ebx
801071d5:	e8 b6 b2 ff ff       	call   80102490 <kalloc>
801071da:	85 c0                	test   %eax,%eax
801071dc:	74 6a                	je     80107248 <setupkvm+0x78>
801071de:	83 ec 04             	sub    $0x4,%esp
801071e1:	89 c6                	mov    %eax,%esi
801071e3:	bb 20 a4 10 80       	mov    $0x8010a420,%ebx
801071e8:	68 00 10 00 00       	push   $0x1000
801071ed:	6a 00                	push   $0x0
801071ef:	50                   	push   %eax
801071f0:	e8 eb d6 ff ff       	call   801048e0 <memset>
801071f5:	83 c4 10             	add    $0x10,%esp
801071f8:	8b 43 04             	mov    0x4(%ebx),%eax
801071fb:	8b 4b 08             	mov    0x8(%ebx),%ecx
801071fe:	83 ec 08             	sub    $0x8,%esp
80107201:	8b 13                	mov    (%ebx),%edx
80107203:	ff 73 0c             	pushl  0xc(%ebx)
80107206:	50                   	push   %eax
80107207:	29 c1                	sub    %eax,%ecx
80107209:	89 f0                	mov    %esi,%eax
8010720b:	e8 70 f9 ff ff       	call   80106b80 <mappages>
80107210:	83 c4 10             	add    $0x10,%esp
80107213:	85 c0                	test   %eax,%eax
80107215:	78 19                	js     80107230 <setupkvm+0x60>
80107217:	83 c3 10             	add    $0x10,%ebx
8010721a:	81 fb 60 a4 10 80    	cmp    $0x8010a460,%ebx
80107220:	75 d6                	jne    801071f8 <setupkvm+0x28>
80107222:	89 f0                	mov    %esi,%eax
80107224:	8d 65 f8             	lea    -0x8(%ebp),%esp
80107227:	5b                   	pop    %ebx
80107228:	5e                   	pop    %esi
80107229:	5d                   	pop    %ebp
8010722a:	c3                   	ret    
8010722b:	90                   	nop
8010722c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80107230:	83 ec 0c             	sub    $0xc,%esp
80107233:	56                   	push   %esi
80107234:	e8 17 ff ff ff       	call   80107150 <freevm>
80107239:	83 c4 10             	add    $0x10,%esp
8010723c:	8d 65 f8             	lea    -0x8(%ebp),%esp
8010723f:	31 c0                	xor    %eax,%eax
80107241:	5b                   	pop    %ebx
80107242:	5e                   	pop    %esi
80107243:	5d                   	pop    %ebp
80107244:	c3                   	ret    
80107245:	8d 76 00             	lea    0x0(%esi),%esi
80107248:	31 c0                	xor    %eax,%eax
8010724a:	eb d8                	jmp    80107224 <setupkvm+0x54>
8010724c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80107250 <kvmalloc>:
80107250:	55                   	push   %ebp
80107251:	89 e5                	mov    %esp,%ebp
80107253:	83 ec 08             	sub    $0x8,%esp
80107256:	e8 75 ff ff ff       	call   801071d0 <setupkvm>
8010725b:	a3 a4 5b 11 80       	mov    %eax,0x80115ba4
80107260:	05 00 00 00 80       	add    $0x80000000,%eax
80107265:	0f 22 d8             	mov    %eax,%cr3
80107268:	c9                   	leave  
80107269:	c3                   	ret    
8010726a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

80107270 <clearpteu>:
80107270:	55                   	push   %ebp
80107271:	31 c9                	xor    %ecx,%ecx
80107273:	89 e5                	mov    %esp,%ebp
80107275:	83 ec 08             	sub    $0x8,%esp
80107278:	8b 55 0c             	mov    0xc(%ebp),%edx
8010727b:	8b 45 08             	mov    0x8(%ebp),%eax
8010727e:	e8 7d f8 ff ff       	call   80106b00 <walkpgdir>
80107283:	85 c0                	test   %eax,%eax
80107285:	74 05                	je     8010728c <clearpteu+0x1c>
80107287:	83 20 fb             	andl   $0xfffffffb,(%eax)
8010728a:	c9                   	leave  
8010728b:	c3                   	ret    
8010728c:	83 ec 0c             	sub    $0xc,%esp
8010728f:	68 d6 7e 10 80       	push   $0x80107ed6
80107294:	e8 d7 90 ff ff       	call   80100370 <panic>
80107299:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

801072a0 <copyuvm>:
801072a0:	55                   	push   %ebp
801072a1:	89 e5                	mov    %esp,%ebp
801072a3:	57                   	push   %edi
801072a4:	56                   	push   %esi
801072a5:	53                   	push   %ebx
801072a6:	83 ec 1c             	sub    $0x1c,%esp
801072a9:	e8 22 ff ff ff       	call   801071d0 <setupkvm>
801072ae:	85 c0                	test   %eax,%eax
801072b0:	89 45 e0             	mov    %eax,-0x20(%ebp)
801072b3:	0f 84 b2 00 00 00    	je     8010736b <copyuvm+0xcb>
801072b9:	8b 4d 0c             	mov    0xc(%ebp),%ecx
801072bc:	85 c9                	test   %ecx,%ecx
801072be:	0f 84 9c 00 00 00    	je     80107360 <copyuvm+0xc0>
801072c4:	31 f6                	xor    %esi,%esi
801072c6:	eb 4a                	jmp    80107312 <copyuvm+0x72>
801072c8:	90                   	nop
801072c9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801072d0:	83 ec 04             	sub    $0x4,%esp
801072d3:	81 c7 00 00 00 80    	add    $0x80000000,%edi
801072d9:	68 00 10 00 00       	push   $0x1000
801072de:	57                   	push   %edi
801072df:	50                   	push   %eax
801072e0:	e8 ab d6 ff ff       	call   80104990 <memmove>
801072e5:	58                   	pop    %eax
801072e6:	5a                   	pop    %edx
801072e7:	8d 93 00 00 00 80    	lea    -0x80000000(%ebx),%edx
801072ed:	8b 45 e0             	mov    -0x20(%ebp),%eax
801072f0:	ff 75 e4             	pushl  -0x1c(%ebp)
801072f3:	b9 00 10 00 00       	mov    $0x1000,%ecx
801072f8:	52                   	push   %edx
801072f9:	89 f2                	mov    %esi,%edx
801072fb:	e8 80 f8 ff ff       	call   80106b80 <mappages>
80107300:	83 c4 10             	add    $0x10,%esp
80107303:	85 c0                	test   %eax,%eax
80107305:	78 3e                	js     80107345 <copyuvm+0xa5>
80107307:	81 c6 00 10 00 00    	add    $0x1000,%esi
8010730d:	39 75 0c             	cmp    %esi,0xc(%ebp)
80107310:	76 4e                	jbe    80107360 <copyuvm+0xc0>
80107312:	8b 45 08             	mov    0x8(%ebp),%eax
80107315:	31 c9                	xor    %ecx,%ecx
80107317:	89 f2                	mov    %esi,%edx
80107319:	e8 e2 f7 ff ff       	call   80106b00 <walkpgdir>
8010731e:	85 c0                	test   %eax,%eax
80107320:	74 5a                	je     8010737c <copyuvm+0xdc>
80107322:	8b 18                	mov    (%eax),%ebx
80107324:	f6 c3 01             	test   $0x1,%bl
80107327:	74 46                	je     8010736f <copyuvm+0xcf>
80107329:	89 df                	mov    %ebx,%edi
8010732b:	81 e3 ff 0f 00 00    	and    $0xfff,%ebx
80107331:	89 5d e4             	mov    %ebx,-0x1c(%ebp)
80107334:	81 e7 00 f0 ff ff    	and    $0xfffff000,%edi
8010733a:	e8 51 b1 ff ff       	call   80102490 <kalloc>
8010733f:	85 c0                	test   %eax,%eax
80107341:	89 c3                	mov    %eax,%ebx
80107343:	75 8b                	jne    801072d0 <copyuvm+0x30>
80107345:	83 ec 0c             	sub    $0xc,%esp
80107348:	ff 75 e0             	pushl  -0x20(%ebp)
8010734b:	e8 00 fe ff ff       	call   80107150 <freevm>
80107350:	83 c4 10             	add    $0x10,%esp
80107353:	31 c0                	xor    %eax,%eax
80107355:	8d 65 f4             	lea    -0xc(%ebp),%esp
80107358:	5b                   	pop    %ebx
80107359:	5e                   	pop    %esi
8010735a:	5f                   	pop    %edi
8010735b:	5d                   	pop    %ebp
8010735c:	c3                   	ret    
8010735d:	8d 76 00             	lea    0x0(%esi),%esi
80107360:	8b 45 e0             	mov    -0x20(%ebp),%eax
80107363:	8d 65 f4             	lea    -0xc(%ebp),%esp
80107366:	5b                   	pop    %ebx
80107367:	5e                   	pop    %esi
80107368:	5f                   	pop    %edi
80107369:	5d                   	pop    %ebp
8010736a:	c3                   	ret    
8010736b:	31 c0                	xor    %eax,%eax
8010736d:	eb e6                	jmp    80107355 <copyuvm+0xb5>
8010736f:	83 ec 0c             	sub    $0xc,%esp
80107372:	68 fa 7e 10 80       	push   $0x80107efa
80107377:	e8 f4 8f ff ff       	call   80100370 <panic>
8010737c:	83 ec 0c             	sub    $0xc,%esp
8010737f:	68 e0 7e 10 80       	push   $0x80107ee0
80107384:	e8 e7 8f ff ff       	call   80100370 <panic>
80107389:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

80107390 <uva2ka>:
80107390:	55                   	push   %ebp
80107391:	31 c9                	xor    %ecx,%ecx
80107393:	89 e5                	mov    %esp,%ebp
80107395:	83 ec 08             	sub    $0x8,%esp
80107398:	8b 55 0c             	mov    0xc(%ebp),%edx
8010739b:	8b 45 08             	mov    0x8(%ebp),%eax
8010739e:	e8 5d f7 ff ff       	call   80106b00 <walkpgdir>
801073a3:	8b 00                	mov    (%eax),%eax
801073a5:	89 c2                	mov    %eax,%edx
801073a7:	83 e2 05             	and    $0x5,%edx
801073aa:	83 fa 05             	cmp    $0x5,%edx
801073ad:	75 11                	jne    801073c0 <uva2ka+0x30>
801073af:	25 00 f0 ff ff       	and    $0xfffff000,%eax
801073b4:	c9                   	leave  
801073b5:	05 00 00 00 80       	add    $0x80000000,%eax
801073ba:	c3                   	ret    
801073bb:	90                   	nop
801073bc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
801073c0:	31 c0                	xor    %eax,%eax
801073c2:	c9                   	leave  
801073c3:	c3                   	ret    
801073c4:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
801073ca:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

801073d0 <copyout>:
801073d0:	55                   	push   %ebp
801073d1:	89 e5                	mov    %esp,%ebp
801073d3:	57                   	push   %edi
801073d4:	56                   	push   %esi
801073d5:	53                   	push   %ebx
801073d6:	83 ec 1c             	sub    $0x1c,%esp
801073d9:	8b 5d 14             	mov    0x14(%ebp),%ebx
801073dc:	8b 55 0c             	mov    0xc(%ebp),%edx
801073df:	8b 7d 10             	mov    0x10(%ebp),%edi
801073e2:	85 db                	test   %ebx,%ebx
801073e4:	75 40                	jne    80107426 <copyout+0x56>
801073e6:	eb 70                	jmp    80107458 <copyout+0x88>
801073e8:	90                   	nop
801073e9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801073f0:	8b 55 e4             	mov    -0x1c(%ebp),%edx
801073f3:	89 f1                	mov    %esi,%ecx
801073f5:	29 d1                	sub    %edx,%ecx
801073f7:	81 c1 00 10 00 00    	add    $0x1000,%ecx
801073fd:	39 d9                	cmp    %ebx,%ecx
801073ff:	0f 47 cb             	cmova  %ebx,%ecx
80107402:	29 f2                	sub    %esi,%edx
80107404:	83 ec 04             	sub    $0x4,%esp
80107407:	01 d0                	add    %edx,%eax
80107409:	51                   	push   %ecx
8010740a:	57                   	push   %edi
8010740b:	50                   	push   %eax
8010740c:	89 4d e4             	mov    %ecx,-0x1c(%ebp)
8010740f:	e8 7c d5 ff ff       	call   80104990 <memmove>
80107414:	8b 4d e4             	mov    -0x1c(%ebp),%ecx
80107417:	83 c4 10             	add    $0x10,%esp
8010741a:	8d 96 00 10 00 00    	lea    0x1000(%esi),%edx
80107420:	01 cf                	add    %ecx,%edi
80107422:	29 cb                	sub    %ecx,%ebx
80107424:	74 32                	je     80107458 <copyout+0x88>
80107426:	89 d6                	mov    %edx,%esi
80107428:	83 ec 08             	sub    $0x8,%esp
8010742b:	89 55 e4             	mov    %edx,-0x1c(%ebp)
8010742e:	81 e6 00 f0 ff ff    	and    $0xfffff000,%esi
80107434:	56                   	push   %esi
80107435:	ff 75 08             	pushl  0x8(%ebp)
80107438:	e8 53 ff ff ff       	call   80107390 <uva2ka>
8010743d:	83 c4 10             	add    $0x10,%esp
80107440:	85 c0                	test   %eax,%eax
80107442:	75 ac                	jne    801073f0 <copyout+0x20>
80107444:	8d 65 f4             	lea    -0xc(%ebp),%esp
80107447:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
8010744c:	5b                   	pop    %ebx
8010744d:	5e                   	pop    %esi
8010744e:	5f                   	pop    %edi
8010744f:	5d                   	pop    %ebp
80107450:	c3                   	ret    
80107451:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80107458:	8d 65 f4             	lea    -0xc(%ebp),%esp
8010745b:	31 c0                	xor    %eax,%eax
8010745d:	5b                   	pop    %ebx
8010745e:	5e                   	pop    %esi
8010745f:	5f                   	pop    %edi
80107460:	5d                   	pop    %ebp
80107461:	c3                   	ret    
