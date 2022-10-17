
kernel:     file format elf32-i386


Disassembly of section .text:

80100000 <multiboot_header>:
80100000:	02 b0 ad 1b 00 00    	add    0x1bad(%eax),%dh
80100006:	00 00                	add    %al,(%eax)
80100008:	fe 4f 52             	decb   0x52(%edi)
8010000b:	e4                   	.byte 0xe4

8010000c <entry>:
8010000c:	0f 20 e0             	mov    %cr4,%eax
8010000f:	83 c8 10             	or     $0x10,%eax
80100012:	0f 22 e0             	mov    %eax,%cr4
80100015:	b8 00 90 10 00       	mov    $0x109000,%eax
8010001a:	0f 22 d8             	mov    %eax,%cr3
8010001d:	0f 20 c0             	mov    %cr0,%eax
80100020:	0d 00 00 01 80       	or     $0x80010000,%eax
80100025:	0f 22 c0             	mov    %eax,%cr0
80100028:	bc c0 b5 10 80       	mov    $0x8010b5c0,%esp
8010002d:	b8 f0 2d 10 80       	mov    $0x80102df0,%eax
80100032:	ff e0                	jmp    *%eax
80100034:	66 90                	xchg   %ax,%ax
80100036:	66 90                	xchg   %ax,%ax
80100038:	66 90                	xchg   %ax,%ax
8010003a:	66 90                	xchg   %ax,%ax
8010003c:	66 90                	xchg   %ax,%ax
8010003e:	66 90                	xchg   %ax,%ax

80100040 <binit>:
  struct buf head;
} bcache;

void
binit(void)
{
80100040:	55                   	push   %ebp
80100041:	89 e5                	mov    %esp,%ebp
80100043:	53                   	push   %ebx

//PAGEBREAK!
  // Create linked list of buffers
  bcache.head.prev = &bcache.head;
  bcache.head.next = &bcache.head;
  for(b = bcache.buf; b < bcache.buf+NBUF; b++){
80100044:	bb f4 b5 10 80       	mov    $0x8010b5f4,%ebx
{
80100049:	83 ec 14             	sub    $0x14,%esp
  initlock(&bcache.lock, "bcache");
8010004c:	c7 44 24 04 40 6c 10 	movl   $0x80106c40,0x4(%esp)
80100053:	80 
80100054:	c7 04 24 c0 b5 10 80 	movl   $0x8010b5c0,(%esp)
8010005b:	e8 10 40 00 00       	call   80104070 <initlock>
  bcache.head.next = &bcache.head;
80100060:	ba bc fc 10 80       	mov    $0x8010fcbc,%edx
  bcache.head.prev = &bcache.head;
80100065:	c7 05 0c fd 10 80 bc 	movl   $0x8010fcbc,0x8010fd0c
8010006c:	fc 10 80 
  bcache.head.next = &bcache.head;
8010006f:	c7 05 10 fd 10 80 bc 	movl   $0x8010fcbc,0x8010fd10
80100076:	fc 10 80 
80100079:	eb 09                	jmp    80100084 <binit+0x44>
8010007b:	90                   	nop
8010007c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80100080:	89 da                	mov    %ebx,%edx
  for(b = bcache.buf; b < bcache.buf+NBUF; b++){
80100082:	89 c3                	mov    %eax,%ebx
80100084:	8d 43 0c             	lea    0xc(%ebx),%eax
    b->next = bcache.head.next;
80100087:	89 53 54             	mov    %edx,0x54(%ebx)
    b->prev = &bcache.head;
8010008a:	c7 43 50 bc fc 10 80 	movl   $0x8010fcbc,0x50(%ebx)
    initsleeplock(&b->lock, "buffer");
80100091:	89 04 24             	mov    %eax,(%esp)
80100094:	c7 44 24 04 47 6c 10 	movl   $0x80106c47,0x4(%esp)
8010009b:	80 
8010009c:	e8 9f 3e 00 00       	call   80103f40 <initsleeplock>
    bcache.head.next->prev = b;
801000a1:	a1 10 fd 10 80       	mov    0x8010fd10,%eax
801000a6:	89 58 50             	mov    %ebx,0x50(%eax)
  for(b = bcache.buf; b < bcache.buf+NBUF; b++){
801000a9:	8d 83 5c 02 00 00    	lea    0x25c(%ebx),%eax
801000af:	3d bc fc 10 80       	cmp    $0x8010fcbc,%eax
    bcache.head.next = b;
801000b4:	89 1d 10 fd 10 80    	mov    %ebx,0x8010fd10
  for(b = bcache.buf; b < bcache.buf+NBUF; b++){
801000ba:	75 c4                	jne    80100080 <binit+0x40>
  }
}
801000bc:	83 c4 14             	add    $0x14,%esp
801000bf:	5b                   	pop    %ebx
801000c0:	5d                   	pop    %ebp
801000c1:	c3                   	ret    
801000c2:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801000c9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801000d0 <bread>:
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
801000d0:	55                   	push   %ebp
801000d1:	89 e5                	mov    %esp,%ebp
801000d3:	57                   	push   %edi
801000d4:	56                   	push   %esi
801000d5:	53                   	push   %ebx
801000d6:	83 ec 1c             	sub    $0x1c,%esp
801000d9:	8b 75 08             	mov    0x8(%ebp),%esi
  acquire(&bcache.lock);
801000dc:	c7 04 24 c0 b5 10 80 	movl   $0x8010b5c0,(%esp)
{
801000e3:	8b 7d 0c             	mov    0xc(%ebp),%edi
  acquire(&bcache.lock);
801000e6:	e8 f5 40 00 00       	call   801041e0 <acquire>
  for(b = bcache.head.next; b != &bcache.head; b = b->next){
801000eb:	8b 1d 10 fd 10 80    	mov    0x8010fd10,%ebx
801000f1:	81 fb bc fc 10 80    	cmp    $0x8010fcbc,%ebx
801000f7:	75 12                	jne    8010010b <bread+0x3b>
801000f9:	eb 25                	jmp    80100120 <bread+0x50>
801000fb:	90                   	nop
801000fc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80100100:	8b 5b 54             	mov    0x54(%ebx),%ebx
80100103:	81 fb bc fc 10 80    	cmp    $0x8010fcbc,%ebx
80100109:	74 15                	je     80100120 <bread+0x50>
    if(b->dev == dev && b->blockno == blockno){
8010010b:	3b 73 04             	cmp    0x4(%ebx),%esi
8010010e:	75 f0                	jne    80100100 <bread+0x30>
80100110:	3b 7b 08             	cmp    0x8(%ebx),%edi
80100113:	75 eb                	jne    80100100 <bread+0x30>
      b->refcnt++;
80100115:	83 43 4c 01          	addl   $0x1,0x4c(%ebx)
80100119:	eb 3f                	jmp    8010015a <bread+0x8a>
8010011b:	90                   	nop
8010011c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  for(b = bcache.head.prev; b != &bcache.head; b = b->prev){
80100120:	8b 1d 0c fd 10 80    	mov    0x8010fd0c,%ebx
80100126:	81 fb bc fc 10 80    	cmp    $0x8010fcbc,%ebx
8010012c:	75 0d                	jne    8010013b <bread+0x6b>
8010012e:	eb 58                	jmp    80100188 <bread+0xb8>
80100130:	8b 5b 50             	mov    0x50(%ebx),%ebx
80100133:	81 fb bc fc 10 80    	cmp    $0x8010fcbc,%ebx
80100139:	74 4d                	je     80100188 <bread+0xb8>
    if(b->refcnt == 0 && (b->flags & B_DIRTY) == 0) {
8010013b:	8b 43 4c             	mov    0x4c(%ebx),%eax
8010013e:	85 c0                	test   %eax,%eax
80100140:	75 ee                	jne    80100130 <bread+0x60>
80100142:	f6 03 04             	testb  $0x4,(%ebx)
80100145:	75 e9                	jne    80100130 <bread+0x60>
      b->dev = dev;
80100147:	89 73 04             	mov    %esi,0x4(%ebx)
      b->blockno = blockno;
8010014a:	89 7b 08             	mov    %edi,0x8(%ebx)
      b->flags = 0;
8010014d:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)
      b->refcnt = 1;
80100153:	c7 43 4c 01 00 00 00 	movl   $0x1,0x4c(%ebx)
      release(&bcache.lock);
8010015a:	c7 04 24 c0 b5 10 80 	movl   $0x8010b5c0,(%esp)
80100161:	e8 ea 40 00 00       	call   80104250 <release>
      acquiresleep(&b->lock);
80100166:	8d 43 0c             	lea    0xc(%ebx),%eax
80100169:	89 04 24             	mov    %eax,(%esp)
8010016c:	e8 0f 3e 00 00       	call   80103f80 <acquiresleep>
  struct buf *b;

  b = bget(dev, blockno);
  if((b->flags & B_VALID) == 0) {
80100171:	f6 03 02             	testb  $0x2,(%ebx)
80100174:	75 08                	jne    8010017e <bread+0xae>
    iderw(b);
80100176:	89 1c 24             	mov    %ebx,(%esp)
80100179:	e8 a2 1f 00 00       	call   80102120 <iderw>
  }
  return b;
}
8010017e:	83 c4 1c             	add    $0x1c,%esp
80100181:	89 d8                	mov    %ebx,%eax
80100183:	5b                   	pop    %ebx
80100184:	5e                   	pop    %esi
80100185:	5f                   	pop    %edi
80100186:	5d                   	pop    %ebp
80100187:	c3                   	ret    
  panic("bget: no buffers");
80100188:	c7 04 24 4e 6c 10 80 	movl   $0x80106c4e,(%esp)
8010018f:	e8 cc 01 00 00       	call   80100360 <panic>
80100194:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
8010019a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

801001a0 <bwrite>:

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
801001a0:	55                   	push   %ebp
801001a1:	89 e5                	mov    %esp,%ebp
801001a3:	53                   	push   %ebx
801001a4:	83 ec 14             	sub    $0x14,%esp
801001a7:	8b 5d 08             	mov    0x8(%ebp),%ebx
  if(!holdingsleep(&b->lock))
801001aa:	8d 43 0c             	lea    0xc(%ebx),%eax
801001ad:	89 04 24             	mov    %eax,(%esp)
801001b0:	e8 6b 3e 00 00       	call   80104020 <holdingsleep>
801001b5:	85 c0                	test   %eax,%eax
801001b7:	74 10                	je     801001c9 <bwrite+0x29>
    panic("bwrite");
  b->flags |= B_DIRTY;
801001b9:	83 0b 04             	orl    $0x4,(%ebx)
  iderw(b);
801001bc:	89 5d 08             	mov    %ebx,0x8(%ebp)
}
801001bf:	83 c4 14             	add    $0x14,%esp
801001c2:	5b                   	pop    %ebx
801001c3:	5d                   	pop    %ebp
  iderw(b);
801001c4:	e9 57 1f 00 00       	jmp    80102120 <iderw>
    panic("bwrite");
801001c9:	c7 04 24 5f 6c 10 80 	movl   $0x80106c5f,(%esp)
801001d0:	e8 8b 01 00 00       	call   80100360 <panic>
801001d5:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
801001d9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801001e0 <brelse>:

// Release a locked buffer.
// Move to the head of the MRU list.
void
brelse(struct buf *b)
{
801001e0:	55                   	push   %ebp
801001e1:	89 e5                	mov    %esp,%ebp
801001e3:	56                   	push   %esi
801001e4:	53                   	push   %ebx
801001e5:	83 ec 10             	sub    $0x10,%esp
801001e8:	8b 5d 08             	mov    0x8(%ebp),%ebx
  if(!holdingsleep(&b->lock))
801001eb:	8d 73 0c             	lea    0xc(%ebx),%esi
801001ee:	89 34 24             	mov    %esi,(%esp)
801001f1:	e8 2a 3e 00 00       	call   80104020 <holdingsleep>
801001f6:	85 c0                	test   %eax,%eax
801001f8:	74 5b                	je     80100255 <brelse+0x75>
    panic("brelse");

  releasesleep(&b->lock);
801001fa:	89 34 24             	mov    %esi,(%esp)
801001fd:	e8 de 3d 00 00       	call   80103fe0 <releasesleep>

  acquire(&bcache.lock);
80100202:	c7 04 24 c0 b5 10 80 	movl   $0x8010b5c0,(%esp)
80100209:	e8 d2 3f 00 00       	call   801041e0 <acquire>
  b->refcnt--;
  if (b->refcnt == 0) {
8010020e:	83 6b 4c 01          	subl   $0x1,0x4c(%ebx)
80100212:	75 2f                	jne    80100243 <brelse+0x63>
    // no one is waiting for it.
    b->next->prev = b->prev;
80100214:	8b 43 54             	mov    0x54(%ebx),%eax
80100217:	8b 53 50             	mov    0x50(%ebx),%edx
8010021a:	89 50 50             	mov    %edx,0x50(%eax)
    b->prev->next = b->next;
8010021d:	8b 43 50             	mov    0x50(%ebx),%eax
80100220:	8b 53 54             	mov    0x54(%ebx),%edx
80100223:	89 50 54             	mov    %edx,0x54(%eax)
    b->next = bcache.head.next;
80100226:	a1 10 fd 10 80       	mov    0x8010fd10,%eax
    b->prev = &bcache.head;
8010022b:	c7 43 50 bc fc 10 80 	movl   $0x8010fcbc,0x50(%ebx)
    b->next = bcache.head.next;
80100232:	89 43 54             	mov    %eax,0x54(%ebx)
    bcache.head.next->prev = b;
80100235:	a1 10 fd 10 80       	mov    0x8010fd10,%eax
8010023a:	89 58 50             	mov    %ebx,0x50(%eax)
    bcache.head.next = b;
8010023d:	89 1d 10 fd 10 80    	mov    %ebx,0x8010fd10
  }
  
  release(&bcache.lock);
80100243:	c7 45 08 c0 b5 10 80 	movl   $0x8010b5c0,0x8(%ebp)
}
8010024a:	83 c4 10             	add    $0x10,%esp
8010024d:	5b                   	pop    %ebx
8010024e:	5e                   	pop    %esi
8010024f:	5d                   	pop    %ebp
  release(&bcache.lock);
80100250:	e9 fb 3f 00 00       	jmp    80104250 <release>
    panic("brelse");
80100255:	c7 04 24 66 6c 10 80 	movl   $0x80106c66,(%esp)
8010025c:	e8 ff 00 00 00       	call   80100360 <panic>
80100261:	66 90                	xchg   %ax,%ax
80100263:	66 90                	xchg   %ax,%ax
80100265:	66 90                	xchg   %ax,%ax
80100267:	66 90                	xchg   %ax,%ax
80100269:	66 90                	xchg   %ax,%ax
8010026b:	66 90                	xchg   %ax,%ax
8010026d:	66 90                	xchg   %ax,%ax
8010026f:	90                   	nop

80100270 <consoleread>:
  }
}

int
consoleread(struct inode *ip, char *dst, int n)
{
80100270:	55                   	push   %ebp
80100271:	89 e5                	mov    %esp,%ebp
80100273:	57                   	push   %edi
80100274:	56                   	push   %esi
80100275:	53                   	push   %ebx
80100276:	83 ec 1c             	sub    $0x1c,%esp
80100279:	8b 7d 08             	mov    0x8(%ebp),%edi
8010027c:	8b 75 0c             	mov    0xc(%ebp),%esi
  uint target;
  int c;

  iunlock(ip);
8010027f:	89 3c 24             	mov    %edi,(%esp)
80100282:	e8 09 15 00 00       	call   80101790 <iunlock>
  target = n;
  acquire(&cons.lock);
80100287:	c7 04 24 20 a5 10 80 	movl   $0x8010a520,(%esp)
8010028e:	e8 4d 3f 00 00       	call   801041e0 <acquire>
  while(n > 0){
80100293:	8b 55 10             	mov    0x10(%ebp),%edx
80100296:	85 d2                	test   %edx,%edx
80100298:	0f 8e bc 00 00 00    	jle    8010035a <consoleread+0xea>
8010029e:	8b 5d 10             	mov    0x10(%ebp),%ebx
801002a1:	eb 25                	jmp    801002c8 <consoleread+0x58>
801002a3:	90                   	nop
801002a4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    while(input.r == input.w){
      if(myproc()->killed){
801002a8:	e8 f3 33 00 00       	call   801036a0 <myproc>
801002ad:	8b 40 24             	mov    0x24(%eax),%eax
801002b0:	85 c0                	test   %eax,%eax
801002b2:	75 74                	jne    80100328 <consoleread+0xb8>
        release(&cons.lock);
        ilock(ip);
        return -1;
      }
      sleep(&input.r, &cons.lock);
801002b4:	c7 44 24 04 20 a5 10 	movl   $0x8010a520,0x4(%esp)
801002bb:	80 
801002bc:	c7 04 24 a0 ff 10 80 	movl   $0x8010ffa0,(%esp)
801002c3:	e8 38 39 00 00       	call   80103c00 <sleep>
    while(input.r == input.w){
801002c8:	a1 a0 ff 10 80       	mov    0x8010ffa0,%eax
801002cd:	3b 05 a4 ff 10 80    	cmp    0x8010ffa4,%eax
801002d3:	74 d3                	je     801002a8 <consoleread+0x38>
    }
    c = input.buf[input.r++ % INPUT_BUF];
801002d5:	8d 50 01             	lea    0x1(%eax),%edx
801002d8:	89 15 a0 ff 10 80    	mov    %edx,0x8010ffa0
801002de:	89 c2                	mov    %eax,%edx
801002e0:	83 e2 7f             	and    $0x7f,%edx
801002e3:	0f b6 8a 20 ff 10 80 	movzbl -0x7fef00e0(%edx),%ecx
801002ea:	0f be d1             	movsbl %cl,%edx
    if(c == C('D')){  // EOF
801002ed:	83 fa 04             	cmp    $0x4,%edx
801002f0:	74 57                	je     80100349 <consoleread+0xd9>
        // caller gets a 0-byte result.
        input.r--;
      }
      break;
    }
    *dst++ = c;
801002f2:	83 c6 01             	add    $0x1,%esi
    --n;
801002f5:	83 eb 01             	sub    $0x1,%ebx
    if(c == '\n')
801002f8:	83 fa 0a             	cmp    $0xa,%edx
    *dst++ = c;
801002fb:	88 4e ff             	mov    %cl,-0x1(%esi)
    if(c == '\n')
801002fe:	74 53                	je     80100353 <consoleread+0xe3>
  while(n > 0){
80100300:	85 db                	test   %ebx,%ebx
80100302:	75 c4                	jne    801002c8 <consoleread+0x58>
80100304:	8b 45 10             	mov    0x10(%ebp),%eax
      break;
  }
  release(&cons.lock);
80100307:	c7 04 24 20 a5 10 80 	movl   $0x8010a520,(%esp)
8010030e:	89 45 e4             	mov    %eax,-0x1c(%ebp)
80100311:	e8 3a 3f 00 00       	call   80104250 <release>
  ilock(ip);
80100316:	89 3c 24             	mov    %edi,(%esp)
80100319:	e8 92 13 00 00       	call   801016b0 <ilock>
8010031e:	8b 45 e4             	mov    -0x1c(%ebp),%eax

  return target - n;
80100321:	eb 1e                	jmp    80100341 <consoleread+0xd1>
80100323:	90                   	nop
80100324:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
        release(&cons.lock);
80100328:	c7 04 24 20 a5 10 80 	movl   $0x8010a520,(%esp)
8010032f:	e8 1c 3f 00 00       	call   80104250 <release>
        ilock(ip);
80100334:	89 3c 24             	mov    %edi,(%esp)
80100337:	e8 74 13 00 00       	call   801016b0 <ilock>
        return -1;
8010033c:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80100341:	83 c4 1c             	add    $0x1c,%esp
80100344:	5b                   	pop    %ebx
80100345:	5e                   	pop    %esi
80100346:	5f                   	pop    %edi
80100347:	5d                   	pop    %ebp
80100348:	c3                   	ret    
      if(n < target){
80100349:	39 5d 10             	cmp    %ebx,0x10(%ebp)
8010034c:	76 05                	jbe    80100353 <consoleread+0xe3>
        input.r--;
8010034e:	a3 a0 ff 10 80       	mov    %eax,0x8010ffa0
80100353:	8b 45 10             	mov    0x10(%ebp),%eax
80100356:	29 d8                	sub    %ebx,%eax
80100358:	eb ad                	jmp    80100307 <consoleread+0x97>
  while(n > 0){
8010035a:	31 c0                	xor    %eax,%eax
8010035c:	eb a9                	jmp    80100307 <consoleread+0x97>
8010035e:	66 90                	xchg   %ax,%ax

80100360 <panic>:
{
80100360:	55                   	push   %ebp
80100361:	89 e5                	mov    %esp,%ebp
80100363:	56                   	push   %esi
80100364:	53                   	push   %ebx
80100365:	83 ec 40             	sub    $0x40,%esp
}

static inline void
cli(void)
{
  asm volatile("cli");
80100368:	fa                   	cli    
  cons.locking = 0;
80100369:	c7 05 54 a5 10 80 00 	movl   $0x0,0x8010a554
80100370:	00 00 00 
  getcallerpcs(&s, pcs);
80100373:	8d 5d d0             	lea    -0x30(%ebp),%ebx
  cprintf("lapicid %d: panic: ", lapicid());
80100376:	e8 e5 23 00 00       	call   80102760 <lapicid>
8010037b:	8d 75 f8             	lea    -0x8(%ebp),%esi
8010037e:	c7 04 24 6d 6c 10 80 	movl   $0x80106c6d,(%esp)
80100385:	89 44 24 04          	mov    %eax,0x4(%esp)
80100389:	e8 c2 02 00 00       	call   80100650 <cprintf>
  cprintf(s);
8010038e:	8b 45 08             	mov    0x8(%ebp),%eax
80100391:	89 04 24             	mov    %eax,(%esp)
80100394:	e8 b7 02 00 00       	call   80100650 <cprintf>
  cprintf("\n");
80100399:	c7 04 24 b7 75 10 80 	movl   $0x801075b7,(%esp)
801003a0:	e8 ab 02 00 00       	call   80100650 <cprintf>
  getcallerpcs(&s, pcs);
801003a5:	8d 45 08             	lea    0x8(%ebp),%eax
801003a8:	89 5c 24 04          	mov    %ebx,0x4(%esp)
801003ac:	89 04 24             	mov    %eax,(%esp)
801003af:	e8 dc 3c 00 00       	call   80104090 <getcallerpcs>
801003b4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    cprintf(" %p", pcs[i]);
801003b8:	8b 03                	mov    (%ebx),%eax
801003ba:	83 c3 04             	add    $0x4,%ebx
801003bd:	c7 04 24 81 6c 10 80 	movl   $0x80106c81,(%esp)
801003c4:	89 44 24 04          	mov    %eax,0x4(%esp)
801003c8:	e8 83 02 00 00       	call   80100650 <cprintf>
  for(i=0; i<10; i++)
801003cd:	39 f3                	cmp    %esi,%ebx
801003cf:	75 e7                	jne    801003b8 <panic+0x58>
  panicked = 1; // freeze other CPU
801003d1:	c7 05 58 a5 10 80 01 	movl   $0x1,0x8010a558
801003d8:	00 00 00 
801003db:	eb fe                	jmp    801003db <panic+0x7b>
801003dd:	8d 76 00             	lea    0x0(%esi),%esi

801003e0 <consputc>:
  if(panicked){
801003e0:	8b 15 58 a5 10 80    	mov    0x8010a558,%edx
801003e6:	85 d2                	test   %edx,%edx
801003e8:	74 06                	je     801003f0 <consputc+0x10>
801003ea:	fa                   	cli    
801003eb:	eb fe                	jmp    801003eb <consputc+0xb>
801003ed:	8d 76 00             	lea    0x0(%esi),%esi
{
801003f0:	55                   	push   %ebp
801003f1:	89 e5                	mov    %esp,%ebp
801003f3:	57                   	push   %edi
801003f4:	56                   	push   %esi
801003f5:	53                   	push   %ebx
801003f6:	89 c3                	mov    %eax,%ebx
801003f8:	83 ec 1c             	sub    $0x1c,%esp
  if(c == BACKSPACE){
801003fb:	3d 00 01 00 00       	cmp    $0x100,%eax
80100400:	0f 84 ac 00 00 00    	je     801004b2 <consputc+0xd2>
    uartputc(c);
80100406:	89 04 24             	mov    %eax,(%esp)
80100409:	e8 a2 53 00 00       	call   801057b0 <uartputc>
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
8010040e:	bf d4 03 00 00       	mov    $0x3d4,%edi
80100413:	b8 0e 00 00 00       	mov    $0xe,%eax
80100418:	89 fa                	mov    %edi,%edx
8010041a:	ee                   	out    %al,(%dx)
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
8010041b:	be d5 03 00 00       	mov    $0x3d5,%esi
80100420:	89 f2                	mov    %esi,%edx
80100422:	ec                   	in     (%dx),%al
  pos = inb(CRTPORT+1) << 8;
80100423:	0f b6 c8             	movzbl %al,%ecx
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
80100426:	89 fa                	mov    %edi,%edx
80100428:	c1 e1 08             	shl    $0x8,%ecx
8010042b:	b8 0f 00 00 00       	mov    $0xf,%eax
80100430:	ee                   	out    %al,(%dx)
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
80100431:	89 f2                	mov    %esi,%edx
80100433:	ec                   	in     (%dx),%al
  pos |= inb(CRTPORT+1);
80100434:	0f b6 c0             	movzbl %al,%eax
80100437:	09 c1                	or     %eax,%ecx
  if(c == '\n')
80100439:	83 fb 0a             	cmp    $0xa,%ebx
8010043c:	0f 84 0d 01 00 00    	je     8010054f <consputc+0x16f>
  else if(c == BACKSPACE){
80100442:	81 fb 00 01 00 00    	cmp    $0x100,%ebx
80100448:	0f 84 e8 00 00 00    	je     80100536 <consputc+0x156>
    crt[pos++] = (c&0xff) | 0x0700;  // black on white
8010044e:	0f b6 db             	movzbl %bl,%ebx
80100451:	80 cf 07             	or     $0x7,%bh
80100454:	8d 79 01             	lea    0x1(%ecx),%edi
80100457:	66 89 9c 09 00 80 0b 	mov    %bx,-0x7ff48000(%ecx,%ecx,1)
8010045e:	80 
  if(pos < 0 || pos > 25*80)
8010045f:	81 ff d0 07 00 00    	cmp    $0x7d0,%edi
80100465:	0f 87 bf 00 00 00    	ja     8010052a <consputc+0x14a>
  if((pos/80) >= 24){  // Scroll up.
8010046b:	81 ff 7f 07 00 00    	cmp    $0x77f,%edi
80100471:	7f 68                	jg     801004db <consputc+0xfb>
80100473:	89 f8                	mov    %edi,%eax
80100475:	89 fb                	mov    %edi,%ebx
80100477:	c1 e8 08             	shr    $0x8,%eax
8010047a:	89 c6                	mov    %eax,%esi
8010047c:	8d 8c 3f 00 80 0b 80 	lea    -0x7ff48000(%edi,%edi,1),%ecx
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
80100483:	bf d4 03 00 00       	mov    $0x3d4,%edi
80100488:	b8 0e 00 00 00       	mov    $0xe,%eax
8010048d:	89 fa                	mov    %edi,%edx
8010048f:	ee                   	out    %al,(%dx)
80100490:	89 f0                	mov    %esi,%eax
80100492:	b2 d5                	mov    $0xd5,%dl
80100494:	ee                   	out    %al,(%dx)
80100495:	b8 0f 00 00 00       	mov    $0xf,%eax
8010049a:	89 fa                	mov    %edi,%edx
8010049c:	ee                   	out    %al,(%dx)
8010049d:	89 d8                	mov    %ebx,%eax
8010049f:	b2 d5                	mov    $0xd5,%dl
801004a1:	ee                   	out    %al,(%dx)
  crt[pos] = ' ' | 0x0700;
801004a2:	b8 20 07 00 00       	mov    $0x720,%eax
801004a7:	66 89 01             	mov    %ax,(%ecx)
}
801004aa:	83 c4 1c             	add    $0x1c,%esp
801004ad:	5b                   	pop    %ebx
801004ae:	5e                   	pop    %esi
801004af:	5f                   	pop    %edi
801004b0:	5d                   	pop    %ebp
801004b1:	c3                   	ret    
    uartputc('\b'); uartputc(' '); uartputc('\b');
801004b2:	c7 04 24 08 00 00 00 	movl   $0x8,(%esp)
801004b9:	e8 f2 52 00 00       	call   801057b0 <uartputc>
801004be:	c7 04 24 20 00 00 00 	movl   $0x20,(%esp)
801004c5:	e8 e6 52 00 00       	call   801057b0 <uartputc>
801004ca:	c7 04 24 08 00 00 00 	movl   $0x8,(%esp)
801004d1:	e8 da 52 00 00       	call   801057b0 <uartputc>
801004d6:	e9 33 ff ff ff       	jmp    8010040e <consputc+0x2e>
    memmove(crt, crt+80, sizeof(crt[0])*23*80);
801004db:	c7 44 24 08 60 0e 00 	movl   $0xe60,0x8(%esp)
801004e2:	00 
    pos -= 80;
801004e3:	8d 5f b0             	lea    -0x50(%edi),%ebx
    memmove(crt, crt+80, sizeof(crt[0])*23*80);
801004e6:	c7 44 24 04 a0 80 0b 	movl   $0x800b80a0,0x4(%esp)
801004ed:	80 
    memset(crt+pos, 0, sizeof(crt[0])*(24*80 - pos));
801004ee:	8d b4 1b 00 80 0b 80 	lea    -0x7ff48000(%ebx,%ebx,1),%esi
    memmove(crt, crt+80, sizeof(crt[0])*23*80);
801004f5:	c7 04 24 00 80 0b 80 	movl   $0x800b8000,(%esp)
801004fc:	e8 3f 3e 00 00       	call   80104340 <memmove>
    memset(crt+pos, 0, sizeof(crt[0])*(24*80 - pos));
80100501:	b8 d0 07 00 00       	mov    $0x7d0,%eax
80100506:	29 f8                	sub    %edi,%eax
80100508:	01 c0                	add    %eax,%eax
8010050a:	89 34 24             	mov    %esi,(%esp)
8010050d:	89 44 24 08          	mov    %eax,0x8(%esp)
80100511:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
80100518:	00 
80100519:	e8 82 3d 00 00       	call   801042a0 <memset>
8010051e:	89 f1                	mov    %esi,%ecx
80100520:	be 07 00 00 00       	mov    $0x7,%esi
80100525:	e9 59 ff ff ff       	jmp    80100483 <consputc+0xa3>
    panic("pos under/overflow");
8010052a:	c7 04 24 85 6c 10 80 	movl   $0x80106c85,(%esp)
80100531:	e8 2a fe ff ff       	call   80100360 <panic>
    if(pos > 0) --pos;
80100536:	85 c9                	test   %ecx,%ecx
80100538:	8d 79 ff             	lea    -0x1(%ecx),%edi
8010053b:	0f 85 1e ff ff ff    	jne    8010045f <consputc+0x7f>
80100541:	b9 00 80 0b 80       	mov    $0x800b8000,%ecx
80100546:	31 db                	xor    %ebx,%ebx
80100548:	31 f6                	xor    %esi,%esi
8010054a:	e9 34 ff ff ff       	jmp    80100483 <consputc+0xa3>
    pos += 80 - pos%80;
8010054f:	89 c8                	mov    %ecx,%eax
80100551:	ba 67 66 66 66       	mov    $0x66666667,%edx
80100556:	f7 ea                	imul   %edx
80100558:	c1 ea 05             	shr    $0x5,%edx
8010055b:	8d 04 92             	lea    (%edx,%edx,4),%eax
8010055e:	c1 e0 04             	shl    $0x4,%eax
80100561:	8d 78 50             	lea    0x50(%eax),%edi
80100564:	e9 f6 fe ff ff       	jmp    8010045f <consputc+0x7f>
80100569:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

80100570 <printint>:
{
80100570:	55                   	push   %ebp
80100571:	89 e5                	mov    %esp,%ebp
80100573:	57                   	push   %edi
80100574:	56                   	push   %esi
80100575:	89 d6                	mov    %edx,%esi
80100577:	53                   	push   %ebx
80100578:	83 ec 1c             	sub    $0x1c,%esp
  if(sign && (sign = xx < 0))
8010057b:	85 c9                	test   %ecx,%ecx
8010057d:	74 61                	je     801005e0 <printint+0x70>
8010057f:	85 c0                	test   %eax,%eax
80100581:	79 5d                	jns    801005e0 <printint+0x70>
    x = -xx;
80100583:	f7 d8                	neg    %eax
80100585:	bf 01 00 00 00       	mov    $0x1,%edi
  i = 0;
8010058a:	31 c9                	xor    %ecx,%ecx
8010058c:	eb 04                	jmp    80100592 <printint+0x22>
8010058e:	66 90                	xchg   %ax,%ax
    buf[i++] = digits[x % base];
80100590:	89 d9                	mov    %ebx,%ecx
80100592:	31 d2                	xor    %edx,%edx
80100594:	f7 f6                	div    %esi
80100596:	8d 59 01             	lea    0x1(%ecx),%ebx
80100599:	0f b6 92 b0 6c 10 80 	movzbl -0x7fef9350(%edx),%edx
  }while((x /= base) != 0);
801005a0:	85 c0                	test   %eax,%eax
    buf[i++] = digits[x % base];
801005a2:	88 54 1d d7          	mov    %dl,-0x29(%ebp,%ebx,1)
  }while((x /= base) != 0);
801005a6:	75 e8                	jne    80100590 <printint+0x20>
  if(sign)
801005a8:	85 ff                	test   %edi,%edi
    buf[i++] = digits[x % base];
801005aa:	89 d8                	mov    %ebx,%eax
  if(sign)
801005ac:	74 08                	je     801005b6 <printint+0x46>
    buf[i++] = '-';
801005ae:	8d 59 02             	lea    0x2(%ecx),%ebx
801005b1:	c6 44 05 d8 2d       	movb   $0x2d,-0x28(%ebp,%eax,1)
  while(--i >= 0)
801005b6:	83 eb 01             	sub    $0x1,%ebx
801005b9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
    consputc(buf[i]);
801005c0:	0f be 44 1d d8       	movsbl -0x28(%ebp,%ebx,1),%eax
  while(--i >= 0)
801005c5:	83 eb 01             	sub    $0x1,%ebx
    consputc(buf[i]);
801005c8:	e8 13 fe ff ff       	call   801003e0 <consputc>
  while(--i >= 0)
801005cd:	83 fb ff             	cmp    $0xffffffff,%ebx
801005d0:	75 ee                	jne    801005c0 <printint+0x50>
}
801005d2:	83 c4 1c             	add    $0x1c,%esp
801005d5:	5b                   	pop    %ebx
801005d6:	5e                   	pop    %esi
801005d7:	5f                   	pop    %edi
801005d8:	5d                   	pop    %ebp
801005d9:	c3                   	ret    
801005da:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    x = xx;
801005e0:	31 ff                	xor    %edi,%edi
801005e2:	eb a6                	jmp    8010058a <printint+0x1a>
801005e4:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
801005ea:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

801005f0 <consolewrite>:

int
consolewrite(struct inode *ip, char *buf, int n)
{
801005f0:	55                   	push   %ebp
801005f1:	89 e5                	mov    %esp,%ebp
801005f3:	57                   	push   %edi
801005f4:	56                   	push   %esi
801005f5:	53                   	push   %ebx
801005f6:	83 ec 1c             	sub    $0x1c,%esp
  int i;

  iunlock(ip);
801005f9:	8b 45 08             	mov    0x8(%ebp),%eax
{
801005fc:	8b 75 10             	mov    0x10(%ebp),%esi
  iunlock(ip);
801005ff:	89 04 24             	mov    %eax,(%esp)
80100602:	e8 89 11 00 00       	call   80101790 <iunlock>
  acquire(&cons.lock);
80100607:	c7 04 24 20 a5 10 80 	movl   $0x8010a520,(%esp)
8010060e:	e8 cd 3b 00 00       	call   801041e0 <acquire>
80100613:	8b 7d 0c             	mov    0xc(%ebp),%edi
  for(i = 0; i < n; i++)
80100616:	85 f6                	test   %esi,%esi
80100618:	8d 1c 37             	lea    (%edi,%esi,1),%ebx
8010061b:	7e 12                	jle    8010062f <consolewrite+0x3f>
8010061d:	8d 76 00             	lea    0x0(%esi),%esi
    consputc(buf[i] & 0xff);
80100620:	0f b6 07             	movzbl (%edi),%eax
80100623:	83 c7 01             	add    $0x1,%edi
80100626:	e8 b5 fd ff ff       	call   801003e0 <consputc>
  for(i = 0; i < n; i++)
8010062b:	39 df                	cmp    %ebx,%edi
8010062d:	75 f1                	jne    80100620 <consolewrite+0x30>
  release(&cons.lock);
8010062f:	c7 04 24 20 a5 10 80 	movl   $0x8010a520,(%esp)
80100636:	e8 15 3c 00 00       	call   80104250 <release>
  ilock(ip);
8010063b:	8b 45 08             	mov    0x8(%ebp),%eax
8010063e:	89 04 24             	mov    %eax,(%esp)
80100641:	e8 6a 10 00 00       	call   801016b0 <ilock>

  return n;
}
80100646:	83 c4 1c             	add    $0x1c,%esp
80100649:	89 f0                	mov    %esi,%eax
8010064b:	5b                   	pop    %ebx
8010064c:	5e                   	pop    %esi
8010064d:	5f                   	pop    %edi
8010064e:	5d                   	pop    %ebp
8010064f:	c3                   	ret    

80100650 <cprintf>:
{
80100650:	55                   	push   %ebp
80100651:	89 e5                	mov    %esp,%ebp
80100653:	57                   	push   %edi
80100654:	56                   	push   %esi
80100655:	53                   	push   %ebx
80100656:	83 ec 1c             	sub    $0x1c,%esp
  locking = cons.locking;
80100659:	a1 54 a5 10 80       	mov    0x8010a554,%eax
  if(locking)
8010065e:	85 c0                	test   %eax,%eax
  locking = cons.locking;
80100660:	89 45 e0             	mov    %eax,-0x20(%ebp)
  if(locking)
80100663:	0f 85 27 01 00 00    	jne    80100790 <cprintf+0x140>
  if (fmt == 0)
80100669:	8b 45 08             	mov    0x8(%ebp),%eax
8010066c:	85 c0                	test   %eax,%eax
8010066e:	89 c1                	mov    %eax,%ecx
80100670:	0f 84 2b 01 00 00    	je     801007a1 <cprintf+0x151>
  for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
80100676:	0f b6 00             	movzbl (%eax),%eax
80100679:	31 db                	xor    %ebx,%ebx
8010067b:	89 cf                	mov    %ecx,%edi
8010067d:	8d 75 0c             	lea    0xc(%ebp),%esi
80100680:	85 c0                	test   %eax,%eax
80100682:	75 4c                	jne    801006d0 <cprintf+0x80>
80100684:	eb 5f                	jmp    801006e5 <cprintf+0x95>
80100686:	66 90                	xchg   %ax,%ax
    c = fmt[++i] & 0xff;
80100688:	83 c3 01             	add    $0x1,%ebx
8010068b:	0f b6 14 1f          	movzbl (%edi,%ebx,1),%edx
    if(c == 0)
8010068f:	85 d2                	test   %edx,%edx
80100691:	74 52                	je     801006e5 <cprintf+0x95>
    switch(c){
80100693:	83 fa 70             	cmp    $0x70,%edx
80100696:	74 72                	je     8010070a <cprintf+0xba>
80100698:	7f 66                	jg     80100700 <cprintf+0xb0>
8010069a:	83 fa 25             	cmp    $0x25,%edx
8010069d:	8d 76 00             	lea    0x0(%esi),%esi
801006a0:	0f 84 a2 00 00 00    	je     80100748 <cprintf+0xf8>
801006a6:	83 fa 64             	cmp    $0x64,%edx
801006a9:	75 7d                	jne    80100728 <cprintf+0xd8>
      printint(*argp++, 10, 1);
801006ab:	8d 46 04             	lea    0x4(%esi),%eax
801006ae:	b9 01 00 00 00       	mov    $0x1,%ecx
801006b3:	89 45 e4             	mov    %eax,-0x1c(%ebp)
801006b6:	8b 06                	mov    (%esi),%eax
801006b8:	ba 0a 00 00 00       	mov    $0xa,%edx
801006bd:	e8 ae fe ff ff       	call   80100570 <printint>
801006c2:	8b 75 e4             	mov    -0x1c(%ebp),%esi
  for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
801006c5:	83 c3 01             	add    $0x1,%ebx
801006c8:	0f b6 04 1f          	movzbl (%edi,%ebx,1),%eax
801006cc:	85 c0                	test   %eax,%eax
801006ce:	74 15                	je     801006e5 <cprintf+0x95>
    if(c != '%'){
801006d0:	83 f8 25             	cmp    $0x25,%eax
801006d3:	74 b3                	je     80100688 <cprintf+0x38>
      consputc(c);
801006d5:	e8 06 fd ff ff       	call   801003e0 <consputc>
  for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
801006da:	83 c3 01             	add    $0x1,%ebx
801006dd:	0f b6 04 1f          	movzbl (%edi,%ebx,1),%eax
801006e1:	85 c0                	test   %eax,%eax
801006e3:	75 eb                	jne    801006d0 <cprintf+0x80>
  if(locking)
801006e5:	8b 45 e0             	mov    -0x20(%ebp),%eax
801006e8:	85 c0                	test   %eax,%eax
801006ea:	74 0c                	je     801006f8 <cprintf+0xa8>
    release(&cons.lock);
801006ec:	c7 04 24 20 a5 10 80 	movl   $0x8010a520,(%esp)
801006f3:	e8 58 3b 00 00       	call   80104250 <release>
}
801006f8:	83 c4 1c             	add    $0x1c,%esp
801006fb:	5b                   	pop    %ebx
801006fc:	5e                   	pop    %esi
801006fd:	5f                   	pop    %edi
801006fe:	5d                   	pop    %ebp
801006ff:	c3                   	ret    
    switch(c){
80100700:	83 fa 73             	cmp    $0x73,%edx
80100703:	74 53                	je     80100758 <cprintf+0x108>
80100705:	83 fa 78             	cmp    $0x78,%edx
80100708:	75 1e                	jne    80100728 <cprintf+0xd8>
      printint(*argp++, 16, 0);
8010070a:	8d 46 04             	lea    0x4(%esi),%eax
8010070d:	31 c9                	xor    %ecx,%ecx
8010070f:	89 45 e4             	mov    %eax,-0x1c(%ebp)
80100712:	8b 06                	mov    (%esi),%eax
80100714:	ba 10 00 00 00       	mov    $0x10,%edx
80100719:	e8 52 fe ff ff       	call   80100570 <printint>
8010071e:	8b 75 e4             	mov    -0x1c(%ebp),%esi
      break;
80100721:	eb a2                	jmp    801006c5 <cprintf+0x75>
80100723:	90                   	nop
80100724:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      consputc('%');
80100728:	b8 25 00 00 00       	mov    $0x25,%eax
8010072d:	89 55 e4             	mov    %edx,-0x1c(%ebp)
80100730:	e8 ab fc ff ff       	call   801003e0 <consputc>
      consputc(c);
80100735:	8b 55 e4             	mov    -0x1c(%ebp),%edx
80100738:	89 d0                	mov    %edx,%eax
8010073a:	e8 a1 fc ff ff       	call   801003e0 <consputc>
8010073f:	eb 99                	jmp    801006da <cprintf+0x8a>
80100741:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
      consputc('%');
80100748:	b8 25 00 00 00       	mov    $0x25,%eax
8010074d:	e8 8e fc ff ff       	call   801003e0 <consputc>
      break;
80100752:	e9 6e ff ff ff       	jmp    801006c5 <cprintf+0x75>
80100757:	90                   	nop
      if((s = (char*)*argp++) == 0)
80100758:	8d 46 04             	lea    0x4(%esi),%eax
8010075b:	8b 36                	mov    (%esi),%esi
8010075d:	89 45 e4             	mov    %eax,-0x1c(%ebp)
        s = "(null)";
80100760:	b8 98 6c 10 80       	mov    $0x80106c98,%eax
80100765:	85 f6                	test   %esi,%esi
80100767:	0f 44 f0             	cmove  %eax,%esi
      for(; *s; s++)
8010076a:	0f be 06             	movsbl (%esi),%eax
8010076d:	84 c0                	test   %al,%al
8010076f:	74 16                	je     80100787 <cprintf+0x137>
80100771:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80100778:	83 c6 01             	add    $0x1,%esi
        consputc(*s);
8010077b:	e8 60 fc ff ff       	call   801003e0 <consputc>
      for(; *s; s++)
80100780:	0f be 06             	movsbl (%esi),%eax
80100783:	84 c0                	test   %al,%al
80100785:	75 f1                	jne    80100778 <cprintf+0x128>
      if((s = (char*)*argp++) == 0)
80100787:	8b 75 e4             	mov    -0x1c(%ebp),%esi
8010078a:	e9 36 ff ff ff       	jmp    801006c5 <cprintf+0x75>
8010078f:	90                   	nop
    acquire(&cons.lock);
80100790:	c7 04 24 20 a5 10 80 	movl   $0x8010a520,(%esp)
80100797:	e8 44 3a 00 00       	call   801041e0 <acquire>
8010079c:	e9 c8 fe ff ff       	jmp    80100669 <cprintf+0x19>
    panic("null fmt");
801007a1:	c7 04 24 9f 6c 10 80 	movl   $0x80106c9f,(%esp)
801007a8:	e8 b3 fb ff ff       	call   80100360 <panic>
801007ad:	8d 76 00             	lea    0x0(%esi),%esi

801007b0 <consoleintr>:
{
801007b0:	55                   	push   %ebp
801007b1:	89 e5                	mov    %esp,%ebp
801007b3:	57                   	push   %edi
801007b4:	56                   	push   %esi
  int c, doprocdump = 0;
801007b5:	31 f6                	xor    %esi,%esi
{
801007b7:	53                   	push   %ebx
801007b8:	83 ec 1c             	sub    $0x1c,%esp
801007bb:	8b 5d 08             	mov    0x8(%ebp),%ebx
  acquire(&cons.lock);
801007be:	c7 04 24 20 a5 10 80 	movl   $0x8010a520,(%esp)
801007c5:	e8 16 3a 00 00       	call   801041e0 <acquire>
801007ca:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  while((c = getc()) >= 0){
801007d0:	ff d3                	call   *%ebx
801007d2:	85 c0                	test   %eax,%eax
801007d4:	89 c7                	mov    %eax,%edi
801007d6:	78 48                	js     80100820 <consoleintr+0x70>
    switch(c){
801007d8:	83 ff 10             	cmp    $0x10,%edi
801007db:	0f 84 2f 01 00 00    	je     80100910 <consoleintr+0x160>
801007e1:	7e 5d                	jle    80100840 <consoleintr+0x90>
801007e3:	83 ff 15             	cmp    $0x15,%edi
801007e6:	0f 84 d4 00 00 00    	je     801008c0 <consoleintr+0x110>
801007ec:	83 ff 7f             	cmp    $0x7f,%edi
801007ef:	90                   	nop
801007f0:	75 53                	jne    80100845 <consoleintr+0x95>
      if(input.e != input.w){
801007f2:	a1 a8 ff 10 80       	mov    0x8010ffa8,%eax
801007f7:	3b 05 a4 ff 10 80    	cmp    0x8010ffa4,%eax
801007fd:	74 d1                	je     801007d0 <consoleintr+0x20>
        input.e--;
801007ff:	83 e8 01             	sub    $0x1,%eax
80100802:	a3 a8 ff 10 80       	mov    %eax,0x8010ffa8
        consputc(BACKSPACE);
80100807:	b8 00 01 00 00       	mov    $0x100,%eax
8010080c:	e8 cf fb ff ff       	call   801003e0 <consputc>
  while((c = getc()) >= 0){
80100811:	ff d3                	call   *%ebx
80100813:	85 c0                	test   %eax,%eax
80100815:	89 c7                	mov    %eax,%edi
80100817:	79 bf                	jns    801007d8 <consoleintr+0x28>
80100819:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  release(&cons.lock);
80100820:	c7 04 24 20 a5 10 80 	movl   $0x8010a520,(%esp)
80100827:	e8 24 3a 00 00       	call   80104250 <release>
  if(doprocdump) {
8010082c:	85 f6                	test   %esi,%esi
8010082e:	0f 85 ec 00 00 00    	jne    80100920 <consoleintr+0x170>
}
80100834:	83 c4 1c             	add    $0x1c,%esp
80100837:	5b                   	pop    %ebx
80100838:	5e                   	pop    %esi
80100839:	5f                   	pop    %edi
8010083a:	5d                   	pop    %ebp
8010083b:	c3                   	ret    
8010083c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    switch(c){
80100840:	83 ff 08             	cmp    $0x8,%edi
80100843:	74 ad                	je     801007f2 <consoleintr+0x42>
      if(c != 0 && input.e-input.r < INPUT_BUF){
80100845:	85 ff                	test   %edi,%edi
80100847:	74 87                	je     801007d0 <consoleintr+0x20>
80100849:	a1 a8 ff 10 80       	mov    0x8010ffa8,%eax
8010084e:	89 c2                	mov    %eax,%edx
80100850:	2b 15 a0 ff 10 80    	sub    0x8010ffa0,%edx
80100856:	83 fa 7f             	cmp    $0x7f,%edx
80100859:	0f 87 71 ff ff ff    	ja     801007d0 <consoleintr+0x20>
        input.buf[input.e++ % INPUT_BUF] = c;
8010085f:	8d 50 01             	lea    0x1(%eax),%edx
80100862:	83 e0 7f             	and    $0x7f,%eax
        c = (c == '\r') ? '\n' : c;
80100865:	83 ff 0d             	cmp    $0xd,%edi
        input.buf[input.e++ % INPUT_BUF] = c;
80100868:	89 15 a8 ff 10 80    	mov    %edx,0x8010ffa8
        c = (c == '\r') ? '\n' : c;
8010086e:	0f 84 b8 00 00 00    	je     8010092c <consoleintr+0x17c>
        input.buf[input.e++ % INPUT_BUF] = c;
80100874:	89 f9                	mov    %edi,%ecx
80100876:	88 88 20 ff 10 80    	mov    %cl,-0x7fef00e0(%eax)
        consputc(c);
8010087c:	89 f8                	mov    %edi,%eax
8010087e:	e8 5d fb ff ff       	call   801003e0 <consputc>
        if(c == '\n' || c == C('D') || input.e == input.r+INPUT_BUF){
80100883:	83 ff 04             	cmp    $0x4,%edi
80100886:	a1 a8 ff 10 80       	mov    0x8010ffa8,%eax
8010088b:	74 19                	je     801008a6 <consoleintr+0xf6>
8010088d:	83 ff 0a             	cmp    $0xa,%edi
80100890:	74 14                	je     801008a6 <consoleintr+0xf6>
80100892:	8b 0d a0 ff 10 80    	mov    0x8010ffa0,%ecx
80100898:	8d 91 80 00 00 00    	lea    0x80(%ecx),%edx
8010089e:	39 d0                	cmp    %edx,%eax
801008a0:	0f 85 2a ff ff ff    	jne    801007d0 <consoleintr+0x20>
          wakeup(&input.r);
801008a6:	c7 04 24 a0 ff 10 80 	movl   $0x8010ffa0,(%esp)
          input.w = input.e;
801008ad:	a3 a4 ff 10 80       	mov    %eax,0x8010ffa4
          wakeup(&input.r);
801008b2:	e8 d9 34 00 00       	call   80103d90 <wakeup>
801008b7:	e9 14 ff ff ff       	jmp    801007d0 <consoleintr+0x20>
801008bc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      while(input.e != input.w &&
801008c0:	a1 a8 ff 10 80       	mov    0x8010ffa8,%eax
801008c5:	3b 05 a4 ff 10 80    	cmp    0x8010ffa4,%eax
801008cb:	75 2b                	jne    801008f8 <consoleintr+0x148>
801008cd:	e9 fe fe ff ff       	jmp    801007d0 <consoleintr+0x20>
801008d2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        input.e--;
801008d8:	a3 a8 ff 10 80       	mov    %eax,0x8010ffa8
        consputc(BACKSPACE);
801008dd:	b8 00 01 00 00       	mov    $0x100,%eax
801008e2:	e8 f9 fa ff ff       	call   801003e0 <consputc>
      while(input.e != input.w &&
801008e7:	a1 a8 ff 10 80       	mov    0x8010ffa8,%eax
801008ec:	3b 05 a4 ff 10 80    	cmp    0x8010ffa4,%eax
801008f2:	0f 84 d8 fe ff ff    	je     801007d0 <consoleintr+0x20>
            input.buf[(input.e-1) % INPUT_BUF] != '\n'){
801008f8:	83 e8 01             	sub    $0x1,%eax
801008fb:	89 c2                	mov    %eax,%edx
801008fd:	83 e2 7f             	and    $0x7f,%edx
      while(input.e != input.w &&
80100900:	80 ba 20 ff 10 80 0a 	cmpb   $0xa,-0x7fef00e0(%edx)
80100907:	75 cf                	jne    801008d8 <consoleintr+0x128>
80100909:	e9 c2 fe ff ff       	jmp    801007d0 <consoleintr+0x20>
8010090e:	66 90                	xchg   %ax,%ax
      doprocdump = 1;
80100910:	be 01 00 00 00       	mov    $0x1,%esi
80100915:	e9 b6 fe ff ff       	jmp    801007d0 <consoleintr+0x20>
8010091a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
}
80100920:	83 c4 1c             	add    $0x1c,%esp
80100923:	5b                   	pop    %ebx
80100924:	5e                   	pop    %esi
80100925:	5f                   	pop    %edi
80100926:	5d                   	pop    %ebp
    procdump();  // now call procdump() wo. cons.lock held
80100927:	e9 44 35 00 00       	jmp    80103e70 <procdump>
        input.buf[input.e++ % INPUT_BUF] = c;
8010092c:	c6 80 20 ff 10 80 0a 	movb   $0xa,-0x7fef00e0(%eax)
        consputc(c);
80100933:	b8 0a 00 00 00       	mov    $0xa,%eax
80100938:	e8 a3 fa ff ff       	call   801003e0 <consputc>
8010093d:	a1 a8 ff 10 80       	mov    0x8010ffa8,%eax
80100942:	e9 5f ff ff ff       	jmp    801008a6 <consoleintr+0xf6>
80100947:	89 f6                	mov    %esi,%esi
80100949:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80100950 <consoleinit>:

void
consoleinit(void)
{
80100950:	55                   	push   %ebp
80100951:	89 e5                	mov    %esp,%ebp
80100953:	83 ec 18             	sub    $0x18,%esp
  initlock(&cons.lock, "console");
80100956:	c7 44 24 04 a8 6c 10 	movl   $0x80106ca8,0x4(%esp)
8010095d:	80 
8010095e:	c7 04 24 20 a5 10 80 	movl   $0x8010a520,(%esp)
80100965:	e8 06 37 00 00       	call   80104070 <initlock>

  devsw[CONSOLE].write = consolewrite;
  devsw[CONSOLE].read = consoleread;
  cons.locking = 1;

  ioapicenable(IRQ_KBD, 0);
8010096a:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
80100971:	00 
80100972:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
  devsw[CONSOLE].write = consolewrite;
80100979:	c7 05 6c 09 11 80 f0 	movl   $0x801005f0,0x8011096c
80100980:	05 10 80 
  devsw[CONSOLE].read = consoleread;
80100983:	c7 05 68 09 11 80 70 	movl   $0x80100270,0x80110968
8010098a:	02 10 80 
  cons.locking = 1;
8010098d:	c7 05 54 a5 10 80 01 	movl   $0x1,0x8010a554
80100994:	00 00 00 
  ioapicenable(IRQ_KBD, 0);
80100997:	e8 14 19 00 00       	call   801022b0 <ioapicenable>
}
8010099c:	c9                   	leave  
8010099d:	c3                   	ret    
8010099e:	66 90                	xchg   %ax,%ax

801009a0 <exec>:
#include "x86.h"
#include "elf.h"

int
exec(char *path, char **argv)
{
801009a0:	55                   	push   %ebp
801009a1:	89 e5                	mov    %esp,%ebp
801009a3:	57                   	push   %edi
801009a4:	56                   	push   %esi
801009a5:	53                   	push   %ebx
801009a6:	81 ec 2c 01 00 00    	sub    $0x12c,%esp
  uint argc, sz, sp, ustack[3+MAXARG+1];
  struct elfhdr elf;
  struct inode *ip;
  struct proghdr ph;
  pde_t *pgdir, *oldpgdir;
  struct proc *curproc = myproc();
801009ac:	e8 ef 2c 00 00       	call   801036a0 <myproc>
801009b1:	89 85 f4 fe ff ff    	mov    %eax,-0x10c(%ebp)

  begin_op();
801009b7:	e8 54 21 00 00       	call   80102b10 <begin_op>

  if((ip = namei(path)) == 0){
801009bc:	8b 45 08             	mov    0x8(%ebp),%eax
801009bf:	89 04 24             	mov    %eax,(%esp)
801009c2:	e8 39 15 00 00       	call   80101f00 <namei>
801009c7:	85 c0                	test   %eax,%eax
801009c9:	89 c3                	mov    %eax,%ebx
801009cb:	0f 84 c2 01 00 00    	je     80100b93 <exec+0x1f3>
    end_op();
    cprintf("exec: fail\n");
    return -1;
  }
  ilock(ip);
801009d1:	89 04 24             	mov    %eax,(%esp)
801009d4:	e8 d7 0c 00 00       	call   801016b0 <ilock>
  pgdir = 0;

  // Check ELF header
  if(readi(ip, (char*)&elf, 0, sizeof(elf)) != sizeof(elf))
801009d9:	8d 85 24 ff ff ff    	lea    -0xdc(%ebp),%eax
801009df:	c7 44 24 0c 34 00 00 	movl   $0x34,0xc(%esp)
801009e6:	00 
801009e7:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
801009ee:	00 
801009ef:	89 44 24 04          	mov    %eax,0x4(%esp)
801009f3:	89 1c 24             	mov    %ebx,(%esp)
801009f6:	e8 65 0f 00 00       	call   80101960 <readi>
801009fb:	83 f8 34             	cmp    $0x34,%eax
801009fe:	74 20                	je     80100a20 <exec+0x80>

 bad:
  if(pgdir)
    freevm(pgdir);
  if(ip){
    iunlockput(ip);
80100a00:	89 1c 24             	mov    %ebx,(%esp)
80100a03:	e8 08 0f 00 00       	call   80101910 <iunlockput>
    end_op();
80100a08:	e8 73 21 00 00       	call   80102b80 <end_op>
  }
  return -1;
80100a0d:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80100a12:	81 c4 2c 01 00 00    	add    $0x12c,%esp
80100a18:	5b                   	pop    %ebx
80100a19:	5e                   	pop    %esi
80100a1a:	5f                   	pop    %edi
80100a1b:	5d                   	pop    %ebp
80100a1c:	c3                   	ret    
80100a1d:	8d 76 00             	lea    0x0(%esi),%esi
  if(elf.magic != ELF_MAGIC)
80100a20:	81 bd 24 ff ff ff 7f 	cmpl   $0x464c457f,-0xdc(%ebp)
80100a27:	45 4c 46 
80100a2a:	75 d4                	jne    80100a00 <exec+0x60>
  if((pgdir = setupkvm()) == 0)
80100a2c:	e8 6f 5f 00 00       	call   801069a0 <setupkvm>
80100a31:	85 c0                	test   %eax,%eax
80100a33:	89 85 f0 fe ff ff    	mov    %eax,-0x110(%ebp)
80100a39:	74 c5                	je     80100a00 <exec+0x60>
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
80100a3b:	66 83 bd 50 ff ff ff 	cmpw   $0x0,-0xb0(%ebp)
80100a42:	00 
80100a43:	8b b5 40 ff ff ff    	mov    -0xc0(%ebp),%esi
  sz = 0;
80100a49:	c7 85 ec fe ff ff 00 	movl   $0x0,-0x114(%ebp)
80100a50:	00 00 00 
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
80100a53:	0f 84 da 00 00 00    	je     80100b33 <exec+0x193>
80100a59:	31 ff                	xor    %edi,%edi
80100a5b:	eb 18                	jmp    80100a75 <exec+0xd5>
80100a5d:	8d 76 00             	lea    0x0(%esi),%esi
80100a60:	0f b7 85 50 ff ff ff 	movzwl -0xb0(%ebp),%eax
80100a67:	83 c7 01             	add    $0x1,%edi
80100a6a:	83 c6 20             	add    $0x20,%esi
80100a6d:	39 f8                	cmp    %edi,%eax
80100a6f:	0f 8e be 00 00 00    	jle    80100b33 <exec+0x193>
    if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
80100a75:	8d 85 04 ff ff ff    	lea    -0xfc(%ebp),%eax
80100a7b:	c7 44 24 0c 20 00 00 	movl   $0x20,0xc(%esp)
80100a82:	00 
80100a83:	89 74 24 08          	mov    %esi,0x8(%esp)
80100a87:	89 44 24 04          	mov    %eax,0x4(%esp)
80100a8b:	89 1c 24             	mov    %ebx,(%esp)
80100a8e:	e8 cd 0e 00 00       	call   80101960 <readi>
80100a93:	83 f8 20             	cmp    $0x20,%eax
80100a96:	0f 85 84 00 00 00    	jne    80100b20 <exec+0x180>
    if(ph.type != ELF_PROG_LOAD)
80100a9c:	83 bd 04 ff ff ff 01 	cmpl   $0x1,-0xfc(%ebp)
80100aa3:	75 bb                	jne    80100a60 <exec+0xc0>
    if(ph.memsz < ph.filesz)
80100aa5:	8b 85 18 ff ff ff    	mov    -0xe8(%ebp),%eax
80100aab:	3b 85 14 ff ff ff    	cmp    -0xec(%ebp),%eax
80100ab1:	72 6d                	jb     80100b20 <exec+0x180>
    if(ph.vaddr + ph.memsz < ph.vaddr)
80100ab3:	03 85 0c ff ff ff    	add    -0xf4(%ebp),%eax
80100ab9:	72 65                	jb     80100b20 <exec+0x180>
    if((sz = allocuvm(pgdir, sz, ph.vaddr + ph.memsz)) == 0)
80100abb:	89 44 24 08          	mov    %eax,0x8(%esp)
80100abf:	8b 85 ec fe ff ff    	mov    -0x114(%ebp),%eax
80100ac5:	89 44 24 04          	mov    %eax,0x4(%esp)
80100ac9:	8b 85 f0 fe ff ff    	mov    -0x110(%ebp),%eax
80100acf:	89 04 24             	mov    %eax,(%esp)
80100ad2:	e8 39 5d 00 00       	call   80106810 <allocuvm>
80100ad7:	85 c0                	test   %eax,%eax
80100ad9:	89 85 ec fe ff ff    	mov    %eax,-0x114(%ebp)
80100adf:	74 3f                	je     80100b20 <exec+0x180>
    if(ph.vaddr % PGSIZE != 0)
80100ae1:	8b 85 0c ff ff ff    	mov    -0xf4(%ebp),%eax
80100ae7:	a9 ff 0f 00 00       	test   $0xfff,%eax
80100aec:	75 32                	jne    80100b20 <exec+0x180>
    if(loaduvm(pgdir, (char*)ph.vaddr, ip, ph.off, ph.filesz) < 0)
80100aee:	8b 95 14 ff ff ff    	mov    -0xec(%ebp),%edx
80100af4:	89 44 24 04          	mov    %eax,0x4(%esp)
80100af8:	8b 85 f0 fe ff ff    	mov    -0x110(%ebp),%eax
80100afe:	89 5c 24 08          	mov    %ebx,0x8(%esp)
80100b02:	89 54 24 10          	mov    %edx,0x10(%esp)
80100b06:	8b 95 08 ff ff ff    	mov    -0xf8(%ebp),%edx
80100b0c:	89 04 24             	mov    %eax,(%esp)
80100b0f:	89 54 24 0c          	mov    %edx,0xc(%esp)
80100b13:	e8 38 5c 00 00       	call   80106750 <loaduvm>
80100b18:	85 c0                	test   %eax,%eax
80100b1a:	0f 89 40 ff ff ff    	jns    80100a60 <exec+0xc0>
    freevm(pgdir);
80100b20:	8b 85 f0 fe ff ff    	mov    -0x110(%ebp),%eax
80100b26:	89 04 24             	mov    %eax,(%esp)
80100b29:	e8 f2 5d 00 00       	call   80106920 <freevm>
80100b2e:	e9 cd fe ff ff       	jmp    80100a00 <exec+0x60>
  iunlockput(ip);
80100b33:	89 1c 24             	mov    %ebx,(%esp)
80100b36:	e8 d5 0d 00 00       	call   80101910 <iunlockput>
80100b3b:	90                   	nop
80100b3c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  end_op();
80100b40:	e8 3b 20 00 00       	call   80102b80 <end_op>
  sz = PGROUNDUP(sz);
80100b45:	8b 85 ec fe ff ff    	mov    -0x114(%ebp),%eax
80100b4b:	05 ff 0f 00 00       	add    $0xfff,%eax
80100b50:	25 00 f0 ff ff       	and    $0xfffff000,%eax
  if((sz = allocuvm(pgdir, sz, sz + 2*PGSIZE)) == 0)
80100b55:	8d 90 00 20 00 00    	lea    0x2000(%eax),%edx
80100b5b:	89 44 24 04          	mov    %eax,0x4(%esp)
80100b5f:	8b 85 f0 fe ff ff    	mov    -0x110(%ebp),%eax
80100b65:	89 54 24 08          	mov    %edx,0x8(%esp)
80100b69:	89 04 24             	mov    %eax,(%esp)
80100b6c:	e8 9f 5c 00 00       	call   80106810 <allocuvm>
80100b71:	85 c0                	test   %eax,%eax
80100b73:	89 85 e8 fe ff ff    	mov    %eax,-0x118(%ebp)
80100b79:	75 33                	jne    80100bae <exec+0x20e>
    freevm(pgdir);
80100b7b:	8b 85 f0 fe ff ff    	mov    -0x110(%ebp),%eax
80100b81:	89 04 24             	mov    %eax,(%esp)
80100b84:	e8 97 5d 00 00       	call   80106920 <freevm>
  return -1;
80100b89:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80100b8e:	e9 7f fe ff ff       	jmp    80100a12 <exec+0x72>
    end_op();
80100b93:	e8 e8 1f 00 00       	call   80102b80 <end_op>
    cprintf("exec: fail\n");
80100b98:	c7 04 24 c1 6c 10 80 	movl   $0x80106cc1,(%esp)
80100b9f:	e8 ac fa ff ff       	call   80100650 <cprintf>
    return -1;
80100ba4:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80100ba9:	e9 64 fe ff ff       	jmp    80100a12 <exec+0x72>
  clearpteu(pgdir, (char*)(sz - 2*PGSIZE));
80100bae:	8b 9d e8 fe ff ff    	mov    -0x118(%ebp),%ebx
80100bb4:	89 d8                	mov    %ebx,%eax
80100bb6:	2d 00 20 00 00       	sub    $0x2000,%eax
80100bbb:	89 44 24 04          	mov    %eax,0x4(%esp)
80100bbf:	8b 85 f0 fe ff ff    	mov    -0x110(%ebp),%eax
80100bc5:	89 04 24             	mov    %eax,(%esp)
80100bc8:	e8 83 5e 00 00       	call   80106a50 <clearpteu>
  for(argc = 0; argv[argc]; argc++) {
80100bcd:	8b 45 0c             	mov    0xc(%ebp),%eax
80100bd0:	8b 00                	mov    (%eax),%eax
80100bd2:	85 c0                	test   %eax,%eax
80100bd4:	0f 84 59 01 00 00    	je     80100d33 <exec+0x393>
80100bda:	8b 4d 0c             	mov    0xc(%ebp),%ecx
80100bdd:	31 d2                	xor    %edx,%edx
80100bdf:	8d 71 04             	lea    0x4(%ecx),%esi
80100be2:	89 cf                	mov    %ecx,%edi
80100be4:	89 d1                	mov    %edx,%ecx
80100be6:	89 f2                	mov    %esi,%edx
80100be8:	89 fe                	mov    %edi,%esi
80100bea:	89 cf                	mov    %ecx,%edi
80100bec:	eb 0a                	jmp    80100bf8 <exec+0x258>
80100bee:	66 90                	xchg   %ax,%ax
80100bf0:	83 c2 04             	add    $0x4,%edx
    if(argc >= MAXARG)
80100bf3:	83 ff 20             	cmp    $0x20,%edi
80100bf6:	74 83                	je     80100b7b <exec+0x1db>
    sp = (sp - (strlen(argv[argc]) + 1)) & ~3;
80100bf8:	89 04 24             	mov    %eax,(%esp)
80100bfb:	89 95 ec fe ff ff    	mov    %edx,-0x114(%ebp)
80100c01:	e8 ba 38 00 00       	call   801044c0 <strlen>
80100c06:	f7 d0                	not    %eax
80100c08:	01 c3                	add    %eax,%ebx
    if(copyout(pgdir, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
80100c0a:	8b 06                	mov    (%esi),%eax
    sp = (sp - (strlen(argv[argc]) + 1)) & ~3;
80100c0c:	83 e3 fc             	and    $0xfffffffc,%ebx
    if(copyout(pgdir, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
80100c0f:	89 04 24             	mov    %eax,(%esp)
80100c12:	e8 a9 38 00 00       	call   801044c0 <strlen>
80100c17:	83 c0 01             	add    $0x1,%eax
80100c1a:	89 44 24 0c          	mov    %eax,0xc(%esp)
80100c1e:	8b 06                	mov    (%esi),%eax
80100c20:	89 5c 24 04          	mov    %ebx,0x4(%esp)
80100c24:	89 44 24 08          	mov    %eax,0x8(%esp)
80100c28:	8b 85 f0 fe ff ff    	mov    -0x110(%ebp),%eax
80100c2e:	89 04 24             	mov    %eax,(%esp)
80100c31:	e8 7a 5f 00 00       	call   80106bb0 <copyout>
80100c36:	85 c0                	test   %eax,%eax
80100c38:	0f 88 3d ff ff ff    	js     80100b7b <exec+0x1db>
  for(argc = 0; argv[argc]; argc++) {
80100c3e:	8b 95 ec fe ff ff    	mov    -0x114(%ebp),%edx
    ustack[3+argc] = sp;
80100c44:	8d 8d 58 ff ff ff    	lea    -0xa8(%ebp),%ecx
80100c4a:	89 9c bd 64 ff ff ff 	mov    %ebx,-0x9c(%ebp,%edi,4)
  for(argc = 0; argv[argc]; argc++) {
80100c51:	83 c7 01             	add    $0x1,%edi
80100c54:	8b 02                	mov    (%edx),%eax
80100c56:	89 d6                	mov    %edx,%esi
80100c58:	85 c0                	test   %eax,%eax
80100c5a:	75 94                	jne    80100bf0 <exec+0x250>
80100c5c:	89 fa                	mov    %edi,%edx
  ustack[3+argc] = 0;
80100c5e:	c7 84 95 64 ff ff ff 	movl   $0x0,-0x9c(%ebp,%edx,4)
80100c65:	00 00 00 00 
  ustack[2] = sp - (argc+1)*4;  // argv pointer
80100c69:	8d 04 95 04 00 00 00 	lea    0x4(,%edx,4),%eax
  ustack[1] = argc;
80100c70:	89 95 5c ff ff ff    	mov    %edx,-0xa4(%ebp)
  ustack[2] = sp - (argc+1)*4;  // argv pointer
80100c76:	89 da                	mov    %ebx,%edx
80100c78:	29 c2                	sub    %eax,%edx
  sp -= (3+argc+1) * 4;
80100c7a:	83 c0 0c             	add    $0xc,%eax
80100c7d:	29 c3                	sub    %eax,%ebx
  if(copyout(pgdir, sp, ustack, (3+argc+1)*4) < 0)
80100c7f:	89 44 24 0c          	mov    %eax,0xc(%esp)
80100c83:	8b 85 f0 fe ff ff    	mov    -0x110(%ebp),%eax
80100c89:	89 4c 24 08          	mov    %ecx,0x8(%esp)
80100c8d:	89 5c 24 04          	mov    %ebx,0x4(%esp)
  ustack[0] = 0xffffffff;  // fake return PC
80100c91:	c7 85 58 ff ff ff ff 	movl   $0xffffffff,-0xa8(%ebp)
80100c98:	ff ff ff 
  if(copyout(pgdir, sp, ustack, (3+argc+1)*4) < 0)
80100c9b:	89 04 24             	mov    %eax,(%esp)
  ustack[2] = sp - (argc+1)*4;  // argv pointer
80100c9e:	89 95 60 ff ff ff    	mov    %edx,-0xa0(%ebp)
  if(copyout(pgdir, sp, ustack, (3+argc+1)*4) < 0)
80100ca4:	e8 07 5f 00 00       	call   80106bb0 <copyout>
80100ca9:	85 c0                	test   %eax,%eax
80100cab:	0f 88 ca fe ff ff    	js     80100b7b <exec+0x1db>
  for(last=s=path; *s; s++)
80100cb1:	8b 45 08             	mov    0x8(%ebp),%eax
80100cb4:	0f b6 10             	movzbl (%eax),%edx
80100cb7:	84 d2                	test   %dl,%dl
80100cb9:	74 19                	je     80100cd4 <exec+0x334>
80100cbb:	8b 4d 08             	mov    0x8(%ebp),%ecx
80100cbe:	83 c0 01             	add    $0x1,%eax
      last = s+1;
80100cc1:	80 fa 2f             	cmp    $0x2f,%dl
  for(last=s=path; *s; s++)
80100cc4:	0f b6 10             	movzbl (%eax),%edx
      last = s+1;
80100cc7:	0f 44 c8             	cmove  %eax,%ecx
80100cca:	83 c0 01             	add    $0x1,%eax
  for(last=s=path; *s; s++)
80100ccd:	84 d2                	test   %dl,%dl
80100ccf:	75 f0                	jne    80100cc1 <exec+0x321>
80100cd1:	89 4d 08             	mov    %ecx,0x8(%ebp)
  safestrcpy(curproc->name, last, sizeof(curproc->name));
80100cd4:	8b bd f4 fe ff ff    	mov    -0x10c(%ebp),%edi
80100cda:	8b 45 08             	mov    0x8(%ebp),%eax
80100cdd:	c7 44 24 08 10 00 00 	movl   $0x10,0x8(%esp)
80100ce4:	00 
80100ce5:	89 44 24 04          	mov    %eax,0x4(%esp)
80100ce9:	89 f8                	mov    %edi,%eax
80100ceb:	83 c0 6c             	add    $0x6c,%eax
80100cee:	89 04 24             	mov    %eax,(%esp)
80100cf1:	e8 8a 37 00 00       	call   80104480 <safestrcpy>
  curproc->pgdir = pgdir;
80100cf6:	8b 8d f0 fe ff ff    	mov    -0x110(%ebp),%ecx
  oldpgdir = curproc->pgdir;
80100cfc:	8b 77 04             	mov    0x4(%edi),%esi
  curproc->tf->eip = elf.entry;  // main
80100cff:	8b 47 18             	mov    0x18(%edi),%eax
  curproc->pgdir = pgdir;
80100d02:	89 4f 04             	mov    %ecx,0x4(%edi)
  curproc->sz = sz;
80100d05:	8b 8d e8 fe ff ff    	mov    -0x118(%ebp),%ecx
80100d0b:	89 0f                	mov    %ecx,(%edi)
  curproc->tf->eip = elf.entry;  // main
80100d0d:	8b 95 3c ff ff ff    	mov    -0xc4(%ebp),%edx
80100d13:	89 50 38             	mov    %edx,0x38(%eax)
  curproc->tf->esp = sp;
80100d16:	8b 47 18             	mov    0x18(%edi),%eax
80100d19:	89 58 44             	mov    %ebx,0x44(%eax)
  switchuvm(curproc);
80100d1c:	89 3c 24             	mov    %edi,(%esp)
80100d1f:	e8 9c 58 00 00       	call   801065c0 <switchuvm>
  freevm(oldpgdir);
80100d24:	89 34 24             	mov    %esi,(%esp)
80100d27:	e8 f4 5b 00 00       	call   80106920 <freevm>
  return 0;
80100d2c:	31 c0                	xor    %eax,%eax
80100d2e:	e9 df fc ff ff       	jmp    80100a12 <exec+0x72>
  for(argc = 0; argv[argc]; argc++) {
80100d33:	8b 9d e8 fe ff ff    	mov    -0x118(%ebp),%ebx
80100d39:	31 d2                	xor    %edx,%edx
80100d3b:	8d 8d 58 ff ff ff    	lea    -0xa8(%ebp),%ecx
80100d41:	e9 18 ff ff ff       	jmp    80100c5e <exec+0x2be>
80100d46:	66 90                	xchg   %ax,%ax
80100d48:	66 90                	xchg   %ax,%ax
80100d4a:	66 90                	xchg   %ax,%ax
80100d4c:	66 90                	xchg   %ax,%ax
80100d4e:	66 90                	xchg   %ax,%ax

80100d50 <fileinit>:
  struct file file[NFILE];
} ftable;

void
fileinit(void)
{
80100d50:	55                   	push   %ebp
80100d51:	89 e5                	mov    %esp,%ebp
80100d53:	83 ec 18             	sub    $0x18,%esp
  initlock(&ftable.lock, "ftable");
80100d56:	c7 44 24 04 cd 6c 10 	movl   $0x80106ccd,0x4(%esp)
80100d5d:	80 
80100d5e:	c7 04 24 c0 ff 10 80 	movl   $0x8010ffc0,(%esp)
80100d65:	e8 06 33 00 00       	call   80104070 <initlock>
}
80100d6a:	c9                   	leave  
80100d6b:	c3                   	ret    
80100d6c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80100d70 <filealloc>:

// Allocate a file structure.
struct file*
filealloc(void)
{
80100d70:	55                   	push   %ebp
80100d71:	89 e5                	mov    %esp,%ebp
80100d73:	53                   	push   %ebx
  struct file *f;

  acquire(&ftable.lock);
  for(f = ftable.file; f < ftable.file + NFILE; f++){
80100d74:	bb f4 ff 10 80       	mov    $0x8010fff4,%ebx
{
80100d79:	83 ec 14             	sub    $0x14,%esp
  acquire(&ftable.lock);
80100d7c:	c7 04 24 c0 ff 10 80 	movl   $0x8010ffc0,(%esp)
80100d83:	e8 58 34 00 00       	call   801041e0 <acquire>
80100d88:	eb 11                	jmp    80100d9b <filealloc+0x2b>
80100d8a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  for(f = ftable.file; f < ftable.file + NFILE; f++){
80100d90:	83 c3 18             	add    $0x18,%ebx
80100d93:	81 fb 54 09 11 80    	cmp    $0x80110954,%ebx
80100d99:	74 25                	je     80100dc0 <filealloc+0x50>
    if(f->ref == 0){
80100d9b:	8b 43 04             	mov    0x4(%ebx),%eax
80100d9e:	85 c0                	test   %eax,%eax
80100da0:	75 ee                	jne    80100d90 <filealloc+0x20>
      f->ref = 1;
      release(&ftable.lock);
80100da2:	c7 04 24 c0 ff 10 80 	movl   $0x8010ffc0,(%esp)
      f->ref = 1;
80100da9:	c7 43 04 01 00 00 00 	movl   $0x1,0x4(%ebx)
      release(&ftable.lock);
80100db0:	e8 9b 34 00 00       	call   80104250 <release>
      return f;
    }
  }
  release(&ftable.lock);
  return 0;
}
80100db5:	83 c4 14             	add    $0x14,%esp
      return f;
80100db8:	89 d8                	mov    %ebx,%eax
}
80100dba:	5b                   	pop    %ebx
80100dbb:	5d                   	pop    %ebp
80100dbc:	c3                   	ret    
80100dbd:	8d 76 00             	lea    0x0(%esi),%esi
  release(&ftable.lock);
80100dc0:	c7 04 24 c0 ff 10 80 	movl   $0x8010ffc0,(%esp)
80100dc7:	e8 84 34 00 00       	call   80104250 <release>
}
80100dcc:	83 c4 14             	add    $0x14,%esp
  return 0;
80100dcf:	31 c0                	xor    %eax,%eax
}
80100dd1:	5b                   	pop    %ebx
80100dd2:	5d                   	pop    %ebp
80100dd3:	c3                   	ret    
80100dd4:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80100dda:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

80100de0 <filedup>:

// Increment ref count for file f.
struct file*
filedup(struct file *f)
{
80100de0:	55                   	push   %ebp
80100de1:	89 e5                	mov    %esp,%ebp
80100de3:	53                   	push   %ebx
80100de4:	83 ec 14             	sub    $0x14,%esp
80100de7:	8b 5d 08             	mov    0x8(%ebp),%ebx
  acquire(&ftable.lock);
80100dea:	c7 04 24 c0 ff 10 80 	movl   $0x8010ffc0,(%esp)
80100df1:	e8 ea 33 00 00       	call   801041e0 <acquire>
  if(f->ref < 1)
80100df6:	8b 43 04             	mov    0x4(%ebx),%eax
80100df9:	85 c0                	test   %eax,%eax
80100dfb:	7e 1a                	jle    80100e17 <filedup+0x37>
    panic("filedup");
  f->ref++;
80100dfd:	83 c0 01             	add    $0x1,%eax
80100e00:	89 43 04             	mov    %eax,0x4(%ebx)
  release(&ftable.lock);
80100e03:	c7 04 24 c0 ff 10 80 	movl   $0x8010ffc0,(%esp)
80100e0a:	e8 41 34 00 00       	call   80104250 <release>
  return f;
}
80100e0f:	83 c4 14             	add    $0x14,%esp
80100e12:	89 d8                	mov    %ebx,%eax
80100e14:	5b                   	pop    %ebx
80100e15:	5d                   	pop    %ebp
80100e16:	c3                   	ret    
    panic("filedup");
80100e17:	c7 04 24 d4 6c 10 80 	movl   $0x80106cd4,(%esp)
80100e1e:	e8 3d f5 ff ff       	call   80100360 <panic>
80100e23:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80100e29:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80100e30 <fileclose>:

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(struct file *f)
{
80100e30:	55                   	push   %ebp
80100e31:	89 e5                	mov    %esp,%ebp
80100e33:	57                   	push   %edi
80100e34:	56                   	push   %esi
80100e35:	53                   	push   %ebx
80100e36:	83 ec 1c             	sub    $0x1c,%esp
80100e39:	8b 7d 08             	mov    0x8(%ebp),%edi
  struct file ff;

  acquire(&ftable.lock);
80100e3c:	c7 04 24 c0 ff 10 80 	movl   $0x8010ffc0,(%esp)
80100e43:	e8 98 33 00 00       	call   801041e0 <acquire>
  if(f->ref < 1)
80100e48:	8b 57 04             	mov    0x4(%edi),%edx
80100e4b:	85 d2                	test   %edx,%edx
80100e4d:	0f 8e 89 00 00 00    	jle    80100edc <fileclose+0xac>
    panic("fileclose");
  if(--f->ref > 0){
80100e53:	83 ea 01             	sub    $0x1,%edx
80100e56:	85 d2                	test   %edx,%edx
80100e58:	89 57 04             	mov    %edx,0x4(%edi)
80100e5b:	74 13                	je     80100e70 <fileclose+0x40>
    release(&ftable.lock);
80100e5d:	c7 45 08 c0 ff 10 80 	movl   $0x8010ffc0,0x8(%ebp)
  else if(ff.type == FD_INODE){
    begin_op();
    iput(ff.ip);
    end_op();
  }
}
80100e64:	83 c4 1c             	add    $0x1c,%esp
80100e67:	5b                   	pop    %ebx
80100e68:	5e                   	pop    %esi
80100e69:	5f                   	pop    %edi
80100e6a:	5d                   	pop    %ebp
    release(&ftable.lock);
80100e6b:	e9 e0 33 00 00       	jmp    80104250 <release>
  ff = *f;
80100e70:	0f b6 47 09          	movzbl 0x9(%edi),%eax
80100e74:	8b 37                	mov    (%edi),%esi
80100e76:	8b 5f 0c             	mov    0xc(%edi),%ebx
  f->type = FD_NONE;
80100e79:	c7 07 00 00 00 00    	movl   $0x0,(%edi)
  ff = *f;
80100e7f:	88 45 e7             	mov    %al,-0x19(%ebp)
80100e82:	8b 47 10             	mov    0x10(%edi),%eax
  release(&ftable.lock);
80100e85:	c7 04 24 c0 ff 10 80 	movl   $0x8010ffc0,(%esp)
  ff = *f;
80100e8c:	89 45 e0             	mov    %eax,-0x20(%ebp)
  release(&ftable.lock);
80100e8f:	e8 bc 33 00 00       	call   80104250 <release>
  if(ff.type == FD_PIPE)
80100e94:	83 fe 01             	cmp    $0x1,%esi
80100e97:	74 0f                	je     80100ea8 <fileclose+0x78>
  else if(ff.type == FD_INODE){
80100e99:	83 fe 02             	cmp    $0x2,%esi
80100e9c:	74 22                	je     80100ec0 <fileclose+0x90>
}
80100e9e:	83 c4 1c             	add    $0x1c,%esp
80100ea1:	5b                   	pop    %ebx
80100ea2:	5e                   	pop    %esi
80100ea3:	5f                   	pop    %edi
80100ea4:	5d                   	pop    %ebp
80100ea5:	c3                   	ret    
80100ea6:	66 90                	xchg   %ax,%ax
    pipeclose(ff.pipe, ff.writable);
80100ea8:	0f be 75 e7          	movsbl -0x19(%ebp),%esi
80100eac:	89 1c 24             	mov    %ebx,(%esp)
80100eaf:	89 74 24 04          	mov    %esi,0x4(%esp)
80100eb3:	e8 a8 23 00 00       	call   80103260 <pipeclose>
80100eb8:	eb e4                	jmp    80100e9e <fileclose+0x6e>
80100eba:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    begin_op();
80100ec0:	e8 4b 1c 00 00       	call   80102b10 <begin_op>
    iput(ff.ip);
80100ec5:	8b 45 e0             	mov    -0x20(%ebp),%eax
80100ec8:	89 04 24             	mov    %eax,(%esp)
80100ecb:	e8 00 09 00 00       	call   801017d0 <iput>
}
80100ed0:	83 c4 1c             	add    $0x1c,%esp
80100ed3:	5b                   	pop    %ebx
80100ed4:	5e                   	pop    %esi
80100ed5:	5f                   	pop    %edi
80100ed6:	5d                   	pop    %ebp
    end_op();
80100ed7:	e9 a4 1c 00 00       	jmp    80102b80 <end_op>
    panic("fileclose");
80100edc:	c7 04 24 dc 6c 10 80 	movl   $0x80106cdc,(%esp)
80100ee3:	e8 78 f4 ff ff       	call   80100360 <panic>
80100ee8:	90                   	nop
80100ee9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

80100ef0 <filestat>:

// Get metadata about file f.
int
filestat(struct file *f, struct stat *st)
{
80100ef0:	55                   	push   %ebp
80100ef1:	89 e5                	mov    %esp,%ebp
80100ef3:	53                   	push   %ebx
80100ef4:	83 ec 14             	sub    $0x14,%esp
80100ef7:	8b 5d 08             	mov    0x8(%ebp),%ebx
  if(f->type == FD_INODE){
80100efa:	83 3b 02             	cmpl   $0x2,(%ebx)
80100efd:	75 31                	jne    80100f30 <filestat+0x40>
    ilock(f->ip);
80100eff:	8b 43 10             	mov    0x10(%ebx),%eax
80100f02:	89 04 24             	mov    %eax,(%esp)
80100f05:	e8 a6 07 00 00       	call   801016b0 <ilock>
    stati(f->ip, st);
80100f0a:	8b 45 0c             	mov    0xc(%ebp),%eax
80100f0d:	89 44 24 04          	mov    %eax,0x4(%esp)
80100f11:	8b 43 10             	mov    0x10(%ebx),%eax
80100f14:	89 04 24             	mov    %eax,(%esp)
80100f17:	e8 14 0a 00 00       	call   80101930 <stati>
    iunlock(f->ip);
80100f1c:	8b 43 10             	mov    0x10(%ebx),%eax
80100f1f:	89 04 24             	mov    %eax,(%esp)
80100f22:	e8 69 08 00 00       	call   80101790 <iunlock>
    return 0;
  }
  return -1;
}
80100f27:	83 c4 14             	add    $0x14,%esp
    return 0;
80100f2a:	31 c0                	xor    %eax,%eax
}
80100f2c:	5b                   	pop    %ebx
80100f2d:	5d                   	pop    %ebp
80100f2e:	c3                   	ret    
80100f2f:	90                   	nop
80100f30:	83 c4 14             	add    $0x14,%esp
  return -1;
80100f33:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80100f38:	5b                   	pop    %ebx
80100f39:	5d                   	pop    %ebp
80100f3a:	c3                   	ret    
80100f3b:	90                   	nop
80100f3c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80100f40 <fileread>:

// Read from file f.
int
fileread(struct file *f, char *addr, int n)
{
80100f40:	55                   	push   %ebp
80100f41:	89 e5                	mov    %esp,%ebp
80100f43:	57                   	push   %edi
80100f44:	56                   	push   %esi
80100f45:	53                   	push   %ebx
80100f46:	83 ec 1c             	sub    $0x1c,%esp
80100f49:	8b 5d 08             	mov    0x8(%ebp),%ebx
80100f4c:	8b 75 0c             	mov    0xc(%ebp),%esi
80100f4f:	8b 7d 10             	mov    0x10(%ebp),%edi
  int r;

  if(f->readable == 0)
80100f52:	80 7b 08 00          	cmpb   $0x0,0x8(%ebx)
80100f56:	74 68                	je     80100fc0 <fileread+0x80>
    return -1;
  if(f->type == FD_PIPE)
80100f58:	8b 03                	mov    (%ebx),%eax
80100f5a:	83 f8 01             	cmp    $0x1,%eax
80100f5d:	74 49                	je     80100fa8 <fileread+0x68>
    return piperead(f->pipe, addr, n);
  if(f->type == FD_INODE){
80100f5f:	83 f8 02             	cmp    $0x2,%eax
80100f62:	75 63                	jne    80100fc7 <fileread+0x87>
    ilock(f->ip);
80100f64:	8b 43 10             	mov    0x10(%ebx),%eax
80100f67:	89 04 24             	mov    %eax,(%esp)
80100f6a:	e8 41 07 00 00       	call   801016b0 <ilock>
    if((r = readi(f->ip, addr, f->off, n)) > 0)
80100f6f:	89 7c 24 0c          	mov    %edi,0xc(%esp)
80100f73:	8b 43 14             	mov    0x14(%ebx),%eax
80100f76:	89 74 24 04          	mov    %esi,0x4(%esp)
80100f7a:	89 44 24 08          	mov    %eax,0x8(%esp)
80100f7e:	8b 43 10             	mov    0x10(%ebx),%eax
80100f81:	89 04 24             	mov    %eax,(%esp)
80100f84:	e8 d7 09 00 00       	call   80101960 <readi>
80100f89:	85 c0                	test   %eax,%eax
80100f8b:	89 c6                	mov    %eax,%esi
80100f8d:	7e 03                	jle    80100f92 <fileread+0x52>
      f->off += r;
80100f8f:	01 43 14             	add    %eax,0x14(%ebx)
    iunlock(f->ip);
80100f92:	8b 43 10             	mov    0x10(%ebx),%eax
80100f95:	89 04 24             	mov    %eax,(%esp)
80100f98:	e8 f3 07 00 00       	call   80101790 <iunlock>
    if((r = readi(f->ip, addr, f->off, n)) > 0)
80100f9d:	89 f0                	mov    %esi,%eax
    return r;
  }
  panic("fileread");
}
80100f9f:	83 c4 1c             	add    $0x1c,%esp
80100fa2:	5b                   	pop    %ebx
80100fa3:	5e                   	pop    %esi
80100fa4:	5f                   	pop    %edi
80100fa5:	5d                   	pop    %ebp
80100fa6:	c3                   	ret    
80100fa7:	90                   	nop
    return piperead(f->pipe, addr, n);
80100fa8:	8b 43 0c             	mov    0xc(%ebx),%eax
80100fab:	89 45 08             	mov    %eax,0x8(%ebp)
}
80100fae:	83 c4 1c             	add    $0x1c,%esp
80100fb1:	5b                   	pop    %ebx
80100fb2:	5e                   	pop    %esi
80100fb3:	5f                   	pop    %edi
80100fb4:	5d                   	pop    %ebp
    return piperead(f->pipe, addr, n);
80100fb5:	e9 26 24 00 00       	jmp    801033e0 <piperead>
80100fba:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    return -1;
80100fc0:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80100fc5:	eb d8                	jmp    80100f9f <fileread+0x5f>
  panic("fileread");
80100fc7:	c7 04 24 e6 6c 10 80 	movl   $0x80106ce6,(%esp)
80100fce:	e8 8d f3 ff ff       	call   80100360 <panic>
80100fd3:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80100fd9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80100fe0 <filewrite>:

//PAGEBREAK!
// Write to file f.
int
filewrite(struct file *f, char *addr, int n)
{
80100fe0:	55                   	push   %ebp
80100fe1:	89 e5                	mov    %esp,%ebp
80100fe3:	57                   	push   %edi
80100fe4:	56                   	push   %esi
80100fe5:	53                   	push   %ebx
80100fe6:	83 ec 2c             	sub    $0x2c,%esp
80100fe9:	8b 45 0c             	mov    0xc(%ebp),%eax
80100fec:	8b 7d 08             	mov    0x8(%ebp),%edi
80100fef:	89 45 dc             	mov    %eax,-0x24(%ebp)
80100ff2:	8b 45 10             	mov    0x10(%ebp),%eax
  int r;

  if(f->writable == 0)
80100ff5:	80 7f 09 00          	cmpb   $0x0,0x9(%edi)
{
80100ff9:	89 45 e4             	mov    %eax,-0x1c(%ebp)
  if(f->writable == 0)
80100ffc:	0f 84 ae 00 00 00    	je     801010b0 <filewrite+0xd0>
    return -1;
  if(f->type == FD_PIPE)
80101002:	8b 07                	mov    (%edi),%eax
80101004:	83 f8 01             	cmp    $0x1,%eax
80101007:	0f 84 c2 00 00 00    	je     801010cf <filewrite+0xef>
    return pipewrite(f->pipe, addr, n);
  if(f->type == FD_INODE){
8010100d:	83 f8 02             	cmp    $0x2,%eax
80101010:	0f 85 d7 00 00 00    	jne    801010ed <filewrite+0x10d>
    // and 2 blocks of slop for non-aligned writes.
    // this really belongs lower down, since writei()
    // might be writing a device like the console.
    int max = ((MAXOPBLOCKS-1-1-2) / 2) * 512;
    int i = 0;
    while(i < n){
80101016:	8b 45 e4             	mov    -0x1c(%ebp),%eax
80101019:	31 db                	xor    %ebx,%ebx
8010101b:	85 c0                	test   %eax,%eax
8010101d:	7f 31                	jg     80101050 <filewrite+0x70>
8010101f:	e9 9c 00 00 00       	jmp    801010c0 <filewrite+0xe0>
80101024:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

      begin_op();
      ilock(f->ip);
      if ((r = writei(f->ip, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
80101028:	8b 4f 10             	mov    0x10(%edi),%ecx
        f->off += r;
8010102b:	01 47 14             	add    %eax,0x14(%edi)
8010102e:	89 45 e0             	mov    %eax,-0x20(%ebp)
      iunlock(f->ip);
80101031:	89 0c 24             	mov    %ecx,(%esp)
80101034:	e8 57 07 00 00       	call   80101790 <iunlock>
      end_op();
80101039:	e8 42 1b 00 00       	call   80102b80 <end_op>
8010103e:	8b 45 e0             	mov    -0x20(%ebp),%eax

      if(r < 0)
        break;
      if(r != n1)
80101041:	39 f0                	cmp    %esi,%eax
80101043:	0f 85 98 00 00 00    	jne    801010e1 <filewrite+0x101>
        panic("short filewrite");
      i += r;
80101049:	01 c3                	add    %eax,%ebx
    while(i < n){
8010104b:	39 5d e4             	cmp    %ebx,-0x1c(%ebp)
8010104e:	7e 70                	jle    801010c0 <filewrite+0xe0>
      int n1 = n - i;
80101050:	8b 75 e4             	mov    -0x1c(%ebp),%esi
80101053:	b8 00 06 00 00       	mov    $0x600,%eax
80101058:	29 de                	sub    %ebx,%esi
8010105a:	81 fe 00 06 00 00    	cmp    $0x600,%esi
80101060:	0f 4f f0             	cmovg  %eax,%esi
      begin_op();
80101063:	e8 a8 1a 00 00       	call   80102b10 <begin_op>
      ilock(f->ip);
80101068:	8b 47 10             	mov    0x10(%edi),%eax
8010106b:	89 04 24             	mov    %eax,(%esp)
8010106e:	e8 3d 06 00 00       	call   801016b0 <ilock>
      if ((r = writei(f->ip, addr + i, f->off, n1)) > 0)
80101073:	89 74 24 0c          	mov    %esi,0xc(%esp)
80101077:	8b 47 14             	mov    0x14(%edi),%eax
8010107a:	89 44 24 08          	mov    %eax,0x8(%esp)
8010107e:	8b 45 dc             	mov    -0x24(%ebp),%eax
80101081:	01 d8                	add    %ebx,%eax
80101083:	89 44 24 04          	mov    %eax,0x4(%esp)
80101087:	8b 47 10             	mov    0x10(%edi),%eax
8010108a:	89 04 24             	mov    %eax,(%esp)
8010108d:	e8 ce 09 00 00       	call   80101a60 <writei>
80101092:	85 c0                	test   %eax,%eax
80101094:	7f 92                	jg     80101028 <filewrite+0x48>
      iunlock(f->ip);
80101096:	8b 4f 10             	mov    0x10(%edi),%ecx
80101099:	89 45 e0             	mov    %eax,-0x20(%ebp)
8010109c:	89 0c 24             	mov    %ecx,(%esp)
8010109f:	e8 ec 06 00 00       	call   80101790 <iunlock>
      end_op();
801010a4:	e8 d7 1a 00 00       	call   80102b80 <end_op>
      if(r < 0)
801010a9:	8b 45 e0             	mov    -0x20(%ebp),%eax
801010ac:	85 c0                	test   %eax,%eax
801010ae:	74 91                	je     80101041 <filewrite+0x61>
    }
    return i == n ? n : -1;
  }
  panic("filewrite");
}
801010b0:	83 c4 2c             	add    $0x2c,%esp
    return -1;
801010b3:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
801010b8:	5b                   	pop    %ebx
801010b9:	5e                   	pop    %esi
801010ba:	5f                   	pop    %edi
801010bb:	5d                   	pop    %ebp
801010bc:	c3                   	ret    
801010bd:	8d 76 00             	lea    0x0(%esi),%esi
    return i == n ? n : -1;
801010c0:	3b 5d e4             	cmp    -0x1c(%ebp),%ebx
801010c3:	89 d8                	mov    %ebx,%eax
801010c5:	75 e9                	jne    801010b0 <filewrite+0xd0>
}
801010c7:	83 c4 2c             	add    $0x2c,%esp
801010ca:	5b                   	pop    %ebx
801010cb:	5e                   	pop    %esi
801010cc:	5f                   	pop    %edi
801010cd:	5d                   	pop    %ebp
801010ce:	c3                   	ret    
    return pipewrite(f->pipe, addr, n);
801010cf:	8b 47 0c             	mov    0xc(%edi),%eax
801010d2:	89 45 08             	mov    %eax,0x8(%ebp)
}
801010d5:	83 c4 2c             	add    $0x2c,%esp
801010d8:	5b                   	pop    %ebx
801010d9:	5e                   	pop    %esi
801010da:	5f                   	pop    %edi
801010db:	5d                   	pop    %ebp
    return pipewrite(f->pipe, addr, n);
801010dc:	e9 0f 22 00 00       	jmp    801032f0 <pipewrite>
        panic("short filewrite");
801010e1:	c7 04 24 ef 6c 10 80 	movl   $0x80106cef,(%esp)
801010e8:	e8 73 f2 ff ff       	call   80100360 <panic>
  panic("filewrite");
801010ed:	c7 04 24 f5 6c 10 80 	movl   $0x80106cf5,(%esp)
801010f4:	e8 67 f2 ff ff       	call   80100360 <panic>
801010f9:	66 90                	xchg   %ax,%ax
801010fb:	66 90                	xchg   %ax,%ax
801010fd:	66 90                	xchg   %ax,%ax
801010ff:	90                   	nop

80101100 <bfree>:
}

// Free a disk block.
static void
bfree(int dev, uint b)
{
80101100:	55                   	push   %ebp
80101101:	89 e5                	mov    %esp,%ebp
80101103:	57                   	push   %edi
80101104:	89 d7                	mov    %edx,%edi
80101106:	56                   	push   %esi
80101107:	53                   	push   %ebx
  struct buf *bp;
  int bi, m;

  bp = bread(dev, BBLOCK(b, sb));
  bi = b % BPB;
  m = 1 << (bi % 8);
80101108:	bb 01 00 00 00       	mov    $0x1,%ebx
{
8010110d:	83 ec 1c             	sub    $0x1c,%esp
  bp = bread(dev, BBLOCK(b, sb));
80101110:	c1 ea 0c             	shr    $0xc,%edx
80101113:	03 15 d8 09 11 80    	add    0x801109d8,%edx
80101119:	89 04 24             	mov    %eax,(%esp)
8010111c:	89 54 24 04          	mov    %edx,0x4(%esp)
80101120:	e8 ab ef ff ff       	call   801000d0 <bread>
  m = 1 << (bi % 8);
80101125:	89 f9                	mov    %edi,%ecx
  bi = b % BPB;
80101127:	81 e7 ff 0f 00 00    	and    $0xfff,%edi
8010112d:	89 fa                	mov    %edi,%edx
  m = 1 << (bi % 8);
8010112f:	83 e1 07             	and    $0x7,%ecx
  if((bp->data[bi/8] & m) == 0)
80101132:	c1 fa 03             	sar    $0x3,%edx
  m = 1 << (bi % 8);
80101135:	d3 e3                	shl    %cl,%ebx
  bp = bread(dev, BBLOCK(b, sb));
80101137:	89 c6                	mov    %eax,%esi
  if((bp->data[bi/8] & m) == 0)
80101139:	0f b6 44 10 5c       	movzbl 0x5c(%eax,%edx,1),%eax
8010113e:	0f b6 c8             	movzbl %al,%ecx
80101141:	85 d9                	test   %ebx,%ecx
80101143:	74 20                	je     80101165 <bfree+0x65>
    panic("freeing free block");
  bp->data[bi/8] &= ~m;
80101145:	f7 d3                	not    %ebx
80101147:	21 c3                	and    %eax,%ebx
80101149:	88 5c 16 5c          	mov    %bl,0x5c(%esi,%edx,1)
  log_write(bp);
8010114d:	89 34 24             	mov    %esi,(%esp)
80101150:	e8 5b 1b 00 00       	call   80102cb0 <log_write>
  brelse(bp);
80101155:	89 34 24             	mov    %esi,(%esp)
80101158:	e8 83 f0 ff ff       	call   801001e0 <brelse>
}
8010115d:	83 c4 1c             	add    $0x1c,%esp
80101160:	5b                   	pop    %ebx
80101161:	5e                   	pop    %esi
80101162:	5f                   	pop    %edi
80101163:	5d                   	pop    %ebp
80101164:	c3                   	ret    
    panic("freeing free block");
80101165:	c7 04 24 ff 6c 10 80 	movl   $0x80106cff,(%esp)
8010116c:	e8 ef f1 ff ff       	call   80100360 <panic>
80101171:	eb 0d                	jmp    80101180 <balloc>
80101173:	90                   	nop
80101174:	90                   	nop
80101175:	90                   	nop
80101176:	90                   	nop
80101177:	90                   	nop
80101178:	90                   	nop
80101179:	90                   	nop
8010117a:	90                   	nop
8010117b:	90                   	nop
8010117c:	90                   	nop
8010117d:	90                   	nop
8010117e:	90                   	nop
8010117f:	90                   	nop

80101180 <balloc>:
{
80101180:	55                   	push   %ebp
80101181:	89 e5                	mov    %esp,%ebp
80101183:	57                   	push   %edi
80101184:	56                   	push   %esi
80101185:	53                   	push   %ebx
80101186:	83 ec 2c             	sub    $0x2c,%esp
80101189:	89 45 d8             	mov    %eax,-0x28(%ebp)
  for(b = 0; b < sb.size; b += BPB){
8010118c:	a1 c0 09 11 80       	mov    0x801109c0,%eax
80101191:	85 c0                	test   %eax,%eax
80101193:	0f 84 8c 00 00 00    	je     80101225 <balloc+0xa5>
80101199:	c7 45 dc 00 00 00 00 	movl   $0x0,-0x24(%ebp)
    bp = bread(dev, BBLOCK(b, sb));
801011a0:	8b 75 dc             	mov    -0x24(%ebp),%esi
801011a3:	89 f0                	mov    %esi,%eax
801011a5:	c1 f8 0c             	sar    $0xc,%eax
801011a8:	03 05 d8 09 11 80    	add    0x801109d8,%eax
801011ae:	89 44 24 04          	mov    %eax,0x4(%esp)
801011b2:	8b 45 d8             	mov    -0x28(%ebp),%eax
801011b5:	89 04 24             	mov    %eax,(%esp)
801011b8:	e8 13 ef ff ff       	call   801000d0 <bread>
801011bd:	89 45 e4             	mov    %eax,-0x1c(%ebp)
801011c0:	a1 c0 09 11 80       	mov    0x801109c0,%eax
801011c5:	89 45 e0             	mov    %eax,-0x20(%ebp)
    for(bi = 0; bi < BPB && b + bi < sb.size; bi++){
801011c8:	31 c0                	xor    %eax,%eax
801011ca:	eb 33                	jmp    801011ff <balloc+0x7f>
801011cc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      if((bp->data[bi/8] & m) == 0){  // Is block free?
801011d0:	8b 5d e4             	mov    -0x1c(%ebp),%ebx
801011d3:	89 c2                	mov    %eax,%edx
      m = 1 << (bi % 8);
801011d5:	89 c1                	mov    %eax,%ecx
      if((bp->data[bi/8] & m) == 0){  // Is block free?
801011d7:	c1 fa 03             	sar    $0x3,%edx
      m = 1 << (bi % 8);
801011da:	83 e1 07             	and    $0x7,%ecx
801011dd:	bf 01 00 00 00       	mov    $0x1,%edi
801011e2:	d3 e7                	shl    %cl,%edi
      if((bp->data[bi/8] & m) == 0){  // Is block free?
801011e4:	0f b6 5c 13 5c       	movzbl 0x5c(%ebx,%edx,1),%ebx
      m = 1 << (bi % 8);
801011e9:	89 f9                	mov    %edi,%ecx
      if((bp->data[bi/8] & m) == 0){  // Is block free?
801011eb:	0f b6 fb             	movzbl %bl,%edi
801011ee:	85 cf                	test   %ecx,%edi
801011f0:	74 46                	je     80101238 <balloc+0xb8>
    for(bi = 0; bi < BPB && b + bi < sb.size; bi++){
801011f2:	83 c0 01             	add    $0x1,%eax
801011f5:	83 c6 01             	add    $0x1,%esi
801011f8:	3d 00 10 00 00       	cmp    $0x1000,%eax
801011fd:	74 05                	je     80101204 <balloc+0x84>
801011ff:	3b 75 e0             	cmp    -0x20(%ebp),%esi
80101202:	72 cc                	jb     801011d0 <balloc+0x50>
    brelse(bp);
80101204:	8b 45 e4             	mov    -0x1c(%ebp),%eax
80101207:	89 04 24             	mov    %eax,(%esp)
8010120a:	e8 d1 ef ff ff       	call   801001e0 <brelse>
  for(b = 0; b < sb.size; b += BPB){
8010120f:	81 45 dc 00 10 00 00 	addl   $0x1000,-0x24(%ebp)
80101216:	8b 45 dc             	mov    -0x24(%ebp),%eax
80101219:	3b 05 c0 09 11 80    	cmp    0x801109c0,%eax
8010121f:	0f 82 7b ff ff ff    	jb     801011a0 <balloc+0x20>
  panic("balloc: out of blocks");
80101225:	c7 04 24 12 6d 10 80 	movl   $0x80106d12,(%esp)
8010122c:	e8 2f f1 ff ff       	call   80100360 <panic>
80101231:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
        bp->data[bi/8] |= m;  // Mark block in use.
80101238:	09 d9                	or     %ebx,%ecx
8010123a:	8b 5d e4             	mov    -0x1c(%ebp),%ebx
8010123d:	88 4c 13 5c          	mov    %cl,0x5c(%ebx,%edx,1)
        log_write(bp);
80101241:	89 1c 24             	mov    %ebx,(%esp)
80101244:	e8 67 1a 00 00       	call   80102cb0 <log_write>
        brelse(bp);
80101249:	89 1c 24             	mov    %ebx,(%esp)
8010124c:	e8 8f ef ff ff       	call   801001e0 <brelse>
  bp = bread(dev, bno);
80101251:	8b 45 d8             	mov    -0x28(%ebp),%eax
80101254:	89 74 24 04          	mov    %esi,0x4(%esp)
80101258:	89 04 24             	mov    %eax,(%esp)
8010125b:	e8 70 ee ff ff       	call   801000d0 <bread>
  memset(bp->data, 0, BSIZE);
80101260:	c7 44 24 08 00 02 00 	movl   $0x200,0x8(%esp)
80101267:	00 
80101268:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
8010126f:	00 
  bp = bread(dev, bno);
80101270:	89 c3                	mov    %eax,%ebx
  memset(bp->data, 0, BSIZE);
80101272:	8d 40 5c             	lea    0x5c(%eax),%eax
80101275:	89 04 24             	mov    %eax,(%esp)
80101278:	e8 23 30 00 00       	call   801042a0 <memset>
  log_write(bp);
8010127d:	89 1c 24             	mov    %ebx,(%esp)
80101280:	e8 2b 1a 00 00       	call   80102cb0 <log_write>
  brelse(bp);
80101285:	89 1c 24             	mov    %ebx,(%esp)
80101288:	e8 53 ef ff ff       	call   801001e0 <brelse>
}
8010128d:	83 c4 2c             	add    $0x2c,%esp
80101290:	89 f0                	mov    %esi,%eax
80101292:	5b                   	pop    %ebx
80101293:	5e                   	pop    %esi
80101294:	5f                   	pop    %edi
80101295:	5d                   	pop    %ebp
80101296:	c3                   	ret    
80101297:	89 f6                	mov    %esi,%esi
80101299:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801012a0 <iget>:
// Find the inode with number inum on device dev
// and return the in-memory copy. Does not lock
// the inode and does not read it from disk.
static struct inode*
iget(uint dev, uint inum)
{
801012a0:	55                   	push   %ebp
801012a1:	89 e5                	mov    %esp,%ebp
801012a3:	57                   	push   %edi
801012a4:	89 c7                	mov    %eax,%edi
801012a6:	56                   	push   %esi
  struct inode *ip, *empty;

  acquire(&icache.lock);

  // Is the inode already cached?
  empty = 0;
801012a7:	31 f6                	xor    %esi,%esi
{
801012a9:	53                   	push   %ebx
  for(ip = &icache.inode[0]; ip < &icache.inode[NINODE]; ip++){
801012aa:	bb 14 0a 11 80       	mov    $0x80110a14,%ebx
{
801012af:	83 ec 1c             	sub    $0x1c,%esp
  acquire(&icache.lock);
801012b2:	c7 04 24 e0 09 11 80 	movl   $0x801109e0,(%esp)
{
801012b9:	89 55 e4             	mov    %edx,-0x1c(%ebp)
  acquire(&icache.lock);
801012bc:	e8 1f 2f 00 00       	call   801041e0 <acquire>
  for(ip = &icache.inode[0]; ip < &icache.inode[NINODE]; ip++){
801012c1:	8b 55 e4             	mov    -0x1c(%ebp),%edx
801012c4:	eb 14                	jmp    801012da <iget+0x3a>
801012c6:	66 90                	xchg   %ax,%ax
    if(ip->ref > 0 && ip->dev == dev && ip->inum == inum){
      ip->ref++;
      release(&icache.lock);
      return ip;
    }
    if(empty == 0 && ip->ref == 0)    // Remember empty slot.
801012c8:	85 f6                	test   %esi,%esi
801012ca:	74 3c                	je     80101308 <iget+0x68>
  for(ip = &icache.inode[0]; ip < &icache.inode[NINODE]; ip++){
801012cc:	81 c3 90 00 00 00    	add    $0x90,%ebx
801012d2:	81 fb 34 26 11 80    	cmp    $0x80112634,%ebx
801012d8:	74 46                	je     80101320 <iget+0x80>
    if(ip->ref > 0 && ip->dev == dev && ip->inum == inum){
801012da:	8b 4b 08             	mov    0x8(%ebx),%ecx
801012dd:	85 c9                	test   %ecx,%ecx
801012df:	7e e7                	jle    801012c8 <iget+0x28>
801012e1:	39 3b                	cmp    %edi,(%ebx)
801012e3:	75 e3                	jne    801012c8 <iget+0x28>
801012e5:	39 53 04             	cmp    %edx,0x4(%ebx)
801012e8:	75 de                	jne    801012c8 <iget+0x28>
      ip->ref++;
801012ea:	83 c1 01             	add    $0x1,%ecx
      return ip;
801012ed:	89 de                	mov    %ebx,%esi
      release(&icache.lock);
801012ef:	c7 04 24 e0 09 11 80 	movl   $0x801109e0,(%esp)
      ip->ref++;
801012f6:	89 4b 08             	mov    %ecx,0x8(%ebx)
      release(&icache.lock);
801012f9:	e8 52 2f 00 00       	call   80104250 <release>
  ip->ref = 1;
  ip->valid = 0;
  release(&icache.lock);

  return ip;
}
801012fe:	83 c4 1c             	add    $0x1c,%esp
80101301:	89 f0                	mov    %esi,%eax
80101303:	5b                   	pop    %ebx
80101304:	5e                   	pop    %esi
80101305:	5f                   	pop    %edi
80101306:	5d                   	pop    %ebp
80101307:	c3                   	ret    
80101308:	85 c9                	test   %ecx,%ecx
8010130a:	0f 44 f3             	cmove  %ebx,%esi
  for(ip = &icache.inode[0]; ip < &icache.inode[NINODE]; ip++){
8010130d:	81 c3 90 00 00 00    	add    $0x90,%ebx
80101313:	81 fb 34 26 11 80    	cmp    $0x80112634,%ebx
80101319:	75 bf                	jne    801012da <iget+0x3a>
8010131b:	90                   	nop
8010131c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  if(empty == 0)
80101320:	85 f6                	test   %esi,%esi
80101322:	74 29                	je     8010134d <iget+0xad>
  ip->dev = dev;
80101324:	89 3e                	mov    %edi,(%esi)
  ip->inum = inum;
80101326:	89 56 04             	mov    %edx,0x4(%esi)
  ip->ref = 1;
80101329:	c7 46 08 01 00 00 00 	movl   $0x1,0x8(%esi)
  ip->valid = 0;
80101330:	c7 46 4c 00 00 00 00 	movl   $0x0,0x4c(%esi)
  release(&icache.lock);
80101337:	c7 04 24 e0 09 11 80 	movl   $0x801109e0,(%esp)
8010133e:	e8 0d 2f 00 00       	call   80104250 <release>
}
80101343:	83 c4 1c             	add    $0x1c,%esp
80101346:	89 f0                	mov    %esi,%eax
80101348:	5b                   	pop    %ebx
80101349:	5e                   	pop    %esi
8010134a:	5f                   	pop    %edi
8010134b:	5d                   	pop    %ebp
8010134c:	c3                   	ret    
    panic("iget: no inodes");
8010134d:	c7 04 24 28 6d 10 80 	movl   $0x80106d28,(%esp)
80101354:	e8 07 f0 ff ff       	call   80100360 <panic>
80101359:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

80101360 <bmap>:

// Return the disk block address of the nth block in inode ip.
// If there is no such block, bmap allocates one.
static uint
bmap(struct inode *ip, uint bn)
{
80101360:	55                   	push   %ebp
80101361:	89 e5                	mov    %esp,%ebp
80101363:	57                   	push   %edi
80101364:	56                   	push   %esi
80101365:	53                   	push   %ebx
80101366:	89 c3                	mov    %eax,%ebx
80101368:	83 ec 1c             	sub    $0x1c,%esp
  uint addr, *a;
  struct buf *bp;

  if(bn < NDIRECT){
8010136b:	83 fa 0b             	cmp    $0xb,%edx
8010136e:	77 18                	ja     80101388 <bmap+0x28>
80101370:	8d 34 90             	lea    (%eax,%edx,4),%esi
    if((addr = ip->addrs[bn]) == 0)
80101373:	8b 46 5c             	mov    0x5c(%esi),%eax
80101376:	85 c0                	test   %eax,%eax
80101378:	74 66                	je     801013e0 <bmap+0x80>
    brelse(bp);
    return addr;
  }

  panic("bmap: out of range");
}
8010137a:	83 c4 1c             	add    $0x1c,%esp
8010137d:	5b                   	pop    %ebx
8010137e:	5e                   	pop    %esi
8010137f:	5f                   	pop    %edi
80101380:	5d                   	pop    %ebp
80101381:	c3                   	ret    
80101382:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  bn -= NDIRECT;
80101388:	8d 72 f4             	lea    -0xc(%edx),%esi
  if(bn < NINDIRECT){
8010138b:	83 fe 7f             	cmp    $0x7f,%esi
8010138e:	77 77                	ja     80101407 <bmap+0xa7>
    if((addr = ip->addrs[NDIRECT]) == 0)
80101390:	8b 80 8c 00 00 00    	mov    0x8c(%eax),%eax
80101396:	85 c0                	test   %eax,%eax
80101398:	74 5e                	je     801013f8 <bmap+0x98>
    bp = bread(ip->dev, addr);
8010139a:	89 44 24 04          	mov    %eax,0x4(%esp)
8010139e:	8b 03                	mov    (%ebx),%eax
801013a0:	89 04 24             	mov    %eax,(%esp)
801013a3:	e8 28 ed ff ff       	call   801000d0 <bread>
    if((addr = a[bn]) == 0){
801013a8:	8d 54 b0 5c          	lea    0x5c(%eax,%esi,4),%edx
    bp = bread(ip->dev, addr);
801013ac:	89 c7                	mov    %eax,%edi
    if((addr = a[bn]) == 0){
801013ae:	8b 32                	mov    (%edx),%esi
801013b0:	85 f6                	test   %esi,%esi
801013b2:	75 19                	jne    801013cd <bmap+0x6d>
      a[bn] = addr = balloc(ip->dev);
801013b4:	8b 03                	mov    (%ebx),%eax
801013b6:	89 55 e4             	mov    %edx,-0x1c(%ebp)
801013b9:	e8 c2 fd ff ff       	call   80101180 <balloc>
801013be:	8b 55 e4             	mov    -0x1c(%ebp),%edx
801013c1:	89 02                	mov    %eax,(%edx)
801013c3:	89 c6                	mov    %eax,%esi
      log_write(bp);
801013c5:	89 3c 24             	mov    %edi,(%esp)
801013c8:	e8 e3 18 00 00       	call   80102cb0 <log_write>
    brelse(bp);
801013cd:	89 3c 24             	mov    %edi,(%esp)
801013d0:	e8 0b ee ff ff       	call   801001e0 <brelse>
}
801013d5:	83 c4 1c             	add    $0x1c,%esp
    brelse(bp);
801013d8:	89 f0                	mov    %esi,%eax
}
801013da:	5b                   	pop    %ebx
801013db:	5e                   	pop    %esi
801013dc:	5f                   	pop    %edi
801013dd:	5d                   	pop    %ebp
801013de:	c3                   	ret    
801013df:	90                   	nop
      ip->addrs[bn] = addr = balloc(ip->dev);
801013e0:	8b 03                	mov    (%ebx),%eax
801013e2:	e8 99 fd ff ff       	call   80101180 <balloc>
801013e7:	89 46 5c             	mov    %eax,0x5c(%esi)
}
801013ea:	83 c4 1c             	add    $0x1c,%esp
801013ed:	5b                   	pop    %ebx
801013ee:	5e                   	pop    %esi
801013ef:	5f                   	pop    %edi
801013f0:	5d                   	pop    %ebp
801013f1:	c3                   	ret    
801013f2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
      ip->addrs[NDIRECT] = addr = balloc(ip->dev);
801013f8:	8b 03                	mov    (%ebx),%eax
801013fa:	e8 81 fd ff ff       	call   80101180 <balloc>
801013ff:	89 83 8c 00 00 00    	mov    %eax,0x8c(%ebx)
80101405:	eb 93                	jmp    8010139a <bmap+0x3a>
  panic("bmap: out of range");
80101407:	c7 04 24 38 6d 10 80 	movl   $0x80106d38,(%esp)
8010140e:	e8 4d ef ff ff       	call   80100360 <panic>
80101413:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80101419:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80101420 <readsb>:
{
80101420:	55                   	push   %ebp
80101421:	89 e5                	mov    %esp,%ebp
80101423:	56                   	push   %esi
80101424:	53                   	push   %ebx
80101425:	83 ec 10             	sub    $0x10,%esp
  bp = bread(dev, 1);
80101428:	8b 45 08             	mov    0x8(%ebp),%eax
8010142b:	c7 44 24 04 01 00 00 	movl   $0x1,0x4(%esp)
80101432:	00 
{
80101433:	8b 75 0c             	mov    0xc(%ebp),%esi
  bp = bread(dev, 1);
80101436:	89 04 24             	mov    %eax,(%esp)
80101439:	e8 92 ec ff ff       	call   801000d0 <bread>
  memmove(sb, bp->data, sizeof(*sb));
8010143e:	89 34 24             	mov    %esi,(%esp)
80101441:	c7 44 24 08 1c 00 00 	movl   $0x1c,0x8(%esp)
80101448:	00 
  bp = bread(dev, 1);
80101449:	89 c3                	mov    %eax,%ebx
  memmove(sb, bp->data, sizeof(*sb));
8010144b:	8d 40 5c             	lea    0x5c(%eax),%eax
8010144e:	89 44 24 04          	mov    %eax,0x4(%esp)
80101452:	e8 e9 2e 00 00       	call   80104340 <memmove>
  brelse(bp);
80101457:	89 5d 08             	mov    %ebx,0x8(%ebp)
}
8010145a:	83 c4 10             	add    $0x10,%esp
8010145d:	5b                   	pop    %ebx
8010145e:	5e                   	pop    %esi
8010145f:	5d                   	pop    %ebp
  brelse(bp);
80101460:	e9 7b ed ff ff       	jmp    801001e0 <brelse>
80101465:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80101469:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80101470 <iinit>:
{
80101470:	55                   	push   %ebp
80101471:	89 e5                	mov    %esp,%ebp
80101473:	53                   	push   %ebx
80101474:	bb 20 0a 11 80       	mov    $0x80110a20,%ebx
80101479:	83 ec 24             	sub    $0x24,%esp
  initlock(&icache.lock, "icache");
8010147c:	c7 44 24 04 4b 6d 10 	movl   $0x80106d4b,0x4(%esp)
80101483:	80 
80101484:	c7 04 24 e0 09 11 80 	movl   $0x801109e0,(%esp)
8010148b:	e8 e0 2b 00 00       	call   80104070 <initlock>
    initsleeplock(&icache.inode[i].lock, "inode");
80101490:	89 1c 24             	mov    %ebx,(%esp)
80101493:	81 c3 90 00 00 00    	add    $0x90,%ebx
80101499:	c7 44 24 04 52 6d 10 	movl   $0x80106d52,0x4(%esp)
801014a0:	80 
801014a1:	e8 9a 2a 00 00       	call   80103f40 <initsleeplock>
  for(i = 0; i < NINODE; i++) {
801014a6:	81 fb 40 26 11 80    	cmp    $0x80112640,%ebx
801014ac:	75 e2                	jne    80101490 <iinit+0x20>
  readsb(dev, &sb);
801014ae:	8b 45 08             	mov    0x8(%ebp),%eax
801014b1:	c7 44 24 04 c0 09 11 	movl   $0x801109c0,0x4(%esp)
801014b8:	80 
801014b9:	89 04 24             	mov    %eax,(%esp)
801014bc:	e8 5f ff ff ff       	call   80101420 <readsb>
  cprintf("sb: size %d nblocks %d ninodes %d nlog %d logstart %d\
801014c1:	a1 d8 09 11 80       	mov    0x801109d8,%eax
801014c6:	c7 04 24 b8 6d 10 80 	movl   $0x80106db8,(%esp)
801014cd:	89 44 24 1c          	mov    %eax,0x1c(%esp)
801014d1:	a1 d4 09 11 80       	mov    0x801109d4,%eax
801014d6:	89 44 24 18          	mov    %eax,0x18(%esp)
801014da:	a1 d0 09 11 80       	mov    0x801109d0,%eax
801014df:	89 44 24 14          	mov    %eax,0x14(%esp)
801014e3:	a1 cc 09 11 80       	mov    0x801109cc,%eax
801014e8:	89 44 24 10          	mov    %eax,0x10(%esp)
801014ec:	a1 c8 09 11 80       	mov    0x801109c8,%eax
801014f1:	89 44 24 0c          	mov    %eax,0xc(%esp)
801014f5:	a1 c4 09 11 80       	mov    0x801109c4,%eax
801014fa:	89 44 24 08          	mov    %eax,0x8(%esp)
801014fe:	a1 c0 09 11 80       	mov    0x801109c0,%eax
80101503:	89 44 24 04          	mov    %eax,0x4(%esp)
80101507:	e8 44 f1 ff ff       	call   80100650 <cprintf>
}
8010150c:	83 c4 24             	add    $0x24,%esp
8010150f:	5b                   	pop    %ebx
80101510:	5d                   	pop    %ebp
80101511:	c3                   	ret    
80101512:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80101519:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80101520 <ialloc>:
{
80101520:	55                   	push   %ebp
80101521:	89 e5                	mov    %esp,%ebp
80101523:	57                   	push   %edi
80101524:	56                   	push   %esi
80101525:	53                   	push   %ebx
80101526:	83 ec 2c             	sub    $0x2c,%esp
80101529:	8b 45 0c             	mov    0xc(%ebp),%eax
  for(inum = 1; inum < sb.ninodes; inum++){
8010152c:	83 3d c8 09 11 80 01 	cmpl   $0x1,0x801109c8
{
80101533:	8b 7d 08             	mov    0x8(%ebp),%edi
80101536:	89 45 e4             	mov    %eax,-0x1c(%ebp)
  for(inum = 1; inum < sb.ninodes; inum++){
80101539:	0f 86 a2 00 00 00    	jbe    801015e1 <ialloc+0xc1>
8010153f:	be 01 00 00 00       	mov    $0x1,%esi
80101544:	bb 01 00 00 00       	mov    $0x1,%ebx
80101549:	eb 1a                	jmp    80101565 <ialloc+0x45>
8010154b:	90                   	nop
8010154c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    brelse(bp);
80101550:	89 14 24             	mov    %edx,(%esp)
  for(inum = 1; inum < sb.ninodes; inum++){
80101553:	83 c3 01             	add    $0x1,%ebx
    brelse(bp);
80101556:	e8 85 ec ff ff       	call   801001e0 <brelse>
  for(inum = 1; inum < sb.ninodes; inum++){
8010155b:	89 de                	mov    %ebx,%esi
8010155d:	3b 1d c8 09 11 80    	cmp    0x801109c8,%ebx
80101563:	73 7c                	jae    801015e1 <ialloc+0xc1>
    bp = bread(dev, IBLOCK(inum, sb));
80101565:	89 f0                	mov    %esi,%eax
80101567:	c1 e8 03             	shr    $0x3,%eax
8010156a:	03 05 d4 09 11 80    	add    0x801109d4,%eax
80101570:	89 3c 24             	mov    %edi,(%esp)
80101573:	89 44 24 04          	mov    %eax,0x4(%esp)
80101577:	e8 54 eb ff ff       	call   801000d0 <bread>
8010157c:	89 c2                	mov    %eax,%edx
    dip = (struct dinode*)bp->data + inum%IPB;
8010157e:	89 f0                	mov    %esi,%eax
80101580:	83 e0 07             	and    $0x7,%eax
80101583:	c1 e0 06             	shl    $0x6,%eax
80101586:	8d 4c 02 5c          	lea    0x5c(%edx,%eax,1),%ecx
    if(dip->type == 0){  // a free inode
8010158a:	66 83 39 00          	cmpw   $0x0,(%ecx)
8010158e:	75 c0                	jne    80101550 <ialloc+0x30>
      memset(dip, 0, sizeof(*dip));
80101590:	89 0c 24             	mov    %ecx,(%esp)
80101593:	c7 44 24 08 40 00 00 	movl   $0x40,0x8(%esp)
8010159a:	00 
8010159b:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
801015a2:	00 
801015a3:	89 55 dc             	mov    %edx,-0x24(%ebp)
801015a6:	89 4d e0             	mov    %ecx,-0x20(%ebp)
801015a9:	e8 f2 2c 00 00       	call   801042a0 <memset>
      dip->type = type;
801015ae:	0f b7 45 e4          	movzwl -0x1c(%ebp),%eax
      log_write(bp);   // mark it allocated on the disk
801015b2:	8b 55 dc             	mov    -0x24(%ebp),%edx
      dip->type = type;
801015b5:	8b 4d e0             	mov    -0x20(%ebp),%ecx
      log_write(bp);   // mark it allocated on the disk
801015b8:	89 55 e4             	mov    %edx,-0x1c(%ebp)
      dip->type = type;
801015bb:	66 89 01             	mov    %ax,(%ecx)
      log_write(bp);   // mark it allocated on the disk
801015be:	89 14 24             	mov    %edx,(%esp)
801015c1:	e8 ea 16 00 00       	call   80102cb0 <log_write>
      brelse(bp);
801015c6:	8b 55 e4             	mov    -0x1c(%ebp),%edx
801015c9:	89 14 24             	mov    %edx,(%esp)
801015cc:	e8 0f ec ff ff       	call   801001e0 <brelse>
}
801015d1:	83 c4 2c             	add    $0x2c,%esp
      return iget(dev, inum);
801015d4:	89 f2                	mov    %esi,%edx
}
801015d6:	5b                   	pop    %ebx
      return iget(dev, inum);
801015d7:	89 f8                	mov    %edi,%eax
}
801015d9:	5e                   	pop    %esi
801015da:	5f                   	pop    %edi
801015db:	5d                   	pop    %ebp
      return iget(dev, inum);
801015dc:	e9 bf fc ff ff       	jmp    801012a0 <iget>
  panic("ialloc: no inodes");
801015e1:	c7 04 24 58 6d 10 80 	movl   $0x80106d58,(%esp)
801015e8:	e8 73 ed ff ff       	call   80100360 <panic>
801015ed:	8d 76 00             	lea    0x0(%esi),%esi

801015f0 <iupdate>:
{
801015f0:	55                   	push   %ebp
801015f1:	89 e5                	mov    %esp,%ebp
801015f3:	56                   	push   %esi
801015f4:	53                   	push   %ebx
801015f5:	83 ec 10             	sub    $0x10,%esp
801015f8:	8b 5d 08             	mov    0x8(%ebp),%ebx
  bp = bread(ip->dev, IBLOCK(ip->inum, sb));
801015fb:	8b 43 04             	mov    0x4(%ebx),%eax
  memmove(dip->addrs, ip->addrs, sizeof(ip->addrs));
801015fe:	83 c3 5c             	add    $0x5c,%ebx
  bp = bread(ip->dev, IBLOCK(ip->inum, sb));
80101601:	c1 e8 03             	shr    $0x3,%eax
80101604:	03 05 d4 09 11 80    	add    0x801109d4,%eax
8010160a:	89 44 24 04          	mov    %eax,0x4(%esp)
8010160e:	8b 43 a4             	mov    -0x5c(%ebx),%eax
80101611:	89 04 24             	mov    %eax,(%esp)
80101614:	e8 b7 ea ff ff       	call   801000d0 <bread>
  dip = (struct dinode*)bp->data + ip->inum%IPB;
80101619:	8b 53 a8             	mov    -0x58(%ebx),%edx
8010161c:	83 e2 07             	and    $0x7,%edx
8010161f:	c1 e2 06             	shl    $0x6,%edx
80101622:	8d 54 10 5c          	lea    0x5c(%eax,%edx,1),%edx
  bp = bread(ip->dev, IBLOCK(ip->inum, sb));
80101626:	89 c6                	mov    %eax,%esi
  dip->type = ip->type;
80101628:	0f b7 43 f4          	movzwl -0xc(%ebx),%eax
  memmove(dip->addrs, ip->addrs, sizeof(ip->addrs));
8010162c:	83 c2 0c             	add    $0xc,%edx
  dip->type = ip->type;
8010162f:	66 89 42 f4          	mov    %ax,-0xc(%edx)
  dip->major = ip->major;
80101633:	0f b7 43 f6          	movzwl -0xa(%ebx),%eax
80101637:	66 89 42 f6          	mov    %ax,-0xa(%edx)
  dip->minor = ip->minor;
8010163b:	0f b7 43 f8          	movzwl -0x8(%ebx),%eax
8010163f:	66 89 42 f8          	mov    %ax,-0x8(%edx)
  dip->nlink = ip->nlink;
80101643:	0f b7 43 fa          	movzwl -0x6(%ebx),%eax
80101647:	66 89 42 fa          	mov    %ax,-0x6(%edx)
  dip->size = ip->size;
8010164b:	8b 43 fc             	mov    -0x4(%ebx),%eax
8010164e:	89 42 fc             	mov    %eax,-0x4(%edx)
  memmove(dip->addrs, ip->addrs, sizeof(ip->addrs));
80101651:	89 5c 24 04          	mov    %ebx,0x4(%esp)
80101655:	89 14 24             	mov    %edx,(%esp)
80101658:	c7 44 24 08 34 00 00 	movl   $0x34,0x8(%esp)
8010165f:	00 
80101660:	e8 db 2c 00 00       	call   80104340 <memmove>
  log_write(bp);
80101665:	89 34 24             	mov    %esi,(%esp)
80101668:	e8 43 16 00 00       	call   80102cb0 <log_write>
  brelse(bp);
8010166d:	89 75 08             	mov    %esi,0x8(%ebp)
}
80101670:	83 c4 10             	add    $0x10,%esp
80101673:	5b                   	pop    %ebx
80101674:	5e                   	pop    %esi
80101675:	5d                   	pop    %ebp
  brelse(bp);
80101676:	e9 65 eb ff ff       	jmp    801001e0 <brelse>
8010167b:	90                   	nop
8010167c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80101680 <idup>:
{
80101680:	55                   	push   %ebp
80101681:	89 e5                	mov    %esp,%ebp
80101683:	53                   	push   %ebx
80101684:	83 ec 14             	sub    $0x14,%esp
80101687:	8b 5d 08             	mov    0x8(%ebp),%ebx
  acquire(&icache.lock);
8010168a:	c7 04 24 e0 09 11 80 	movl   $0x801109e0,(%esp)
80101691:	e8 4a 2b 00 00       	call   801041e0 <acquire>
  ip->ref++;
80101696:	83 43 08 01          	addl   $0x1,0x8(%ebx)
  release(&icache.lock);
8010169a:	c7 04 24 e0 09 11 80 	movl   $0x801109e0,(%esp)
801016a1:	e8 aa 2b 00 00       	call   80104250 <release>
}
801016a6:	83 c4 14             	add    $0x14,%esp
801016a9:	89 d8                	mov    %ebx,%eax
801016ab:	5b                   	pop    %ebx
801016ac:	5d                   	pop    %ebp
801016ad:	c3                   	ret    
801016ae:	66 90                	xchg   %ax,%ax

801016b0 <ilock>:
{
801016b0:	55                   	push   %ebp
801016b1:	89 e5                	mov    %esp,%ebp
801016b3:	56                   	push   %esi
801016b4:	53                   	push   %ebx
801016b5:	83 ec 10             	sub    $0x10,%esp
801016b8:	8b 5d 08             	mov    0x8(%ebp),%ebx
  if(ip == 0 || ip->ref < 1)
801016bb:	85 db                	test   %ebx,%ebx
801016bd:	0f 84 b3 00 00 00    	je     80101776 <ilock+0xc6>
801016c3:	8b 53 08             	mov    0x8(%ebx),%edx
801016c6:	85 d2                	test   %edx,%edx
801016c8:	0f 8e a8 00 00 00    	jle    80101776 <ilock+0xc6>
  acquiresleep(&ip->lock);
801016ce:	8d 43 0c             	lea    0xc(%ebx),%eax
801016d1:	89 04 24             	mov    %eax,(%esp)
801016d4:	e8 a7 28 00 00       	call   80103f80 <acquiresleep>
  if(ip->valid == 0){
801016d9:	8b 43 4c             	mov    0x4c(%ebx),%eax
801016dc:	85 c0                	test   %eax,%eax
801016de:	74 08                	je     801016e8 <ilock+0x38>
}
801016e0:	83 c4 10             	add    $0x10,%esp
801016e3:	5b                   	pop    %ebx
801016e4:	5e                   	pop    %esi
801016e5:	5d                   	pop    %ebp
801016e6:	c3                   	ret    
801016e7:	90                   	nop
    bp = bread(ip->dev, IBLOCK(ip->inum, sb));
801016e8:	8b 43 04             	mov    0x4(%ebx),%eax
801016eb:	c1 e8 03             	shr    $0x3,%eax
801016ee:	03 05 d4 09 11 80    	add    0x801109d4,%eax
801016f4:	89 44 24 04          	mov    %eax,0x4(%esp)
801016f8:	8b 03                	mov    (%ebx),%eax
801016fa:	89 04 24             	mov    %eax,(%esp)
801016fd:	e8 ce e9 ff ff       	call   801000d0 <bread>
    dip = (struct dinode*)bp->data + ip->inum%IPB;
80101702:	8b 53 04             	mov    0x4(%ebx),%edx
80101705:	83 e2 07             	and    $0x7,%edx
80101708:	c1 e2 06             	shl    $0x6,%edx
8010170b:	8d 54 10 5c          	lea    0x5c(%eax,%edx,1),%edx
    bp = bread(ip->dev, IBLOCK(ip->inum, sb));
8010170f:	89 c6                	mov    %eax,%esi
    ip->type = dip->type;
80101711:	0f b7 02             	movzwl (%edx),%eax
    memmove(ip->addrs, dip->addrs, sizeof(ip->addrs));
80101714:	83 c2 0c             	add    $0xc,%edx
    ip->type = dip->type;
80101717:	66 89 43 50          	mov    %ax,0x50(%ebx)
    ip->major = dip->major;
8010171b:	0f b7 42 f6          	movzwl -0xa(%edx),%eax
8010171f:	66 89 43 52          	mov    %ax,0x52(%ebx)
    ip->minor = dip->minor;
80101723:	0f b7 42 f8          	movzwl -0x8(%edx),%eax
80101727:	66 89 43 54          	mov    %ax,0x54(%ebx)
    ip->nlink = dip->nlink;
8010172b:	0f b7 42 fa          	movzwl -0x6(%edx),%eax
8010172f:	66 89 43 56          	mov    %ax,0x56(%ebx)
    ip->size = dip->size;
80101733:	8b 42 fc             	mov    -0x4(%edx),%eax
80101736:	89 43 58             	mov    %eax,0x58(%ebx)
    memmove(ip->addrs, dip->addrs, sizeof(ip->addrs));
80101739:	8d 43 5c             	lea    0x5c(%ebx),%eax
8010173c:	89 54 24 04          	mov    %edx,0x4(%esp)
80101740:	c7 44 24 08 34 00 00 	movl   $0x34,0x8(%esp)
80101747:	00 
80101748:	89 04 24             	mov    %eax,(%esp)
8010174b:	e8 f0 2b 00 00       	call   80104340 <memmove>
    brelse(bp);
80101750:	89 34 24             	mov    %esi,(%esp)
80101753:	e8 88 ea ff ff       	call   801001e0 <brelse>
    if(ip->type == 0)
80101758:	66 83 7b 50 00       	cmpw   $0x0,0x50(%ebx)
    ip->valid = 1;
8010175d:	c7 43 4c 01 00 00 00 	movl   $0x1,0x4c(%ebx)
    if(ip->type == 0)
80101764:	0f 85 76 ff ff ff    	jne    801016e0 <ilock+0x30>
      panic("ilock: no type");
8010176a:	c7 04 24 70 6d 10 80 	movl   $0x80106d70,(%esp)
80101771:	e8 ea eb ff ff       	call   80100360 <panic>
    panic("ilock");
80101776:	c7 04 24 6a 6d 10 80 	movl   $0x80106d6a,(%esp)
8010177d:	e8 de eb ff ff       	call   80100360 <panic>
80101782:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80101789:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80101790 <iunlock>:
{
80101790:	55                   	push   %ebp
80101791:	89 e5                	mov    %esp,%ebp
80101793:	56                   	push   %esi
80101794:	53                   	push   %ebx
80101795:	83 ec 10             	sub    $0x10,%esp
80101798:	8b 5d 08             	mov    0x8(%ebp),%ebx
  if(ip == 0 || !holdingsleep(&ip->lock) || ip->ref < 1)
8010179b:	85 db                	test   %ebx,%ebx
8010179d:	74 24                	je     801017c3 <iunlock+0x33>
8010179f:	8d 73 0c             	lea    0xc(%ebx),%esi
801017a2:	89 34 24             	mov    %esi,(%esp)
801017a5:	e8 76 28 00 00       	call   80104020 <holdingsleep>
801017aa:	85 c0                	test   %eax,%eax
801017ac:	74 15                	je     801017c3 <iunlock+0x33>
801017ae:	8b 43 08             	mov    0x8(%ebx),%eax
801017b1:	85 c0                	test   %eax,%eax
801017b3:	7e 0e                	jle    801017c3 <iunlock+0x33>
  releasesleep(&ip->lock);
801017b5:	89 75 08             	mov    %esi,0x8(%ebp)
}
801017b8:	83 c4 10             	add    $0x10,%esp
801017bb:	5b                   	pop    %ebx
801017bc:	5e                   	pop    %esi
801017bd:	5d                   	pop    %ebp
  releasesleep(&ip->lock);
801017be:	e9 1d 28 00 00       	jmp    80103fe0 <releasesleep>
    panic("iunlock");
801017c3:	c7 04 24 7f 6d 10 80 	movl   $0x80106d7f,(%esp)
801017ca:	e8 91 eb ff ff       	call   80100360 <panic>
801017cf:	90                   	nop

801017d0 <iput>:
{
801017d0:	55                   	push   %ebp
801017d1:	89 e5                	mov    %esp,%ebp
801017d3:	57                   	push   %edi
801017d4:	56                   	push   %esi
801017d5:	53                   	push   %ebx
801017d6:	83 ec 1c             	sub    $0x1c,%esp
801017d9:	8b 75 08             	mov    0x8(%ebp),%esi
  acquiresleep(&ip->lock);
801017dc:	8d 7e 0c             	lea    0xc(%esi),%edi
801017df:	89 3c 24             	mov    %edi,(%esp)
801017e2:	e8 99 27 00 00       	call   80103f80 <acquiresleep>
  if(ip->valid && ip->nlink == 0){
801017e7:	8b 56 4c             	mov    0x4c(%esi),%edx
801017ea:	85 d2                	test   %edx,%edx
801017ec:	74 07                	je     801017f5 <iput+0x25>
801017ee:	66 83 7e 56 00       	cmpw   $0x0,0x56(%esi)
801017f3:	74 2b                	je     80101820 <iput+0x50>
  releasesleep(&ip->lock);
801017f5:	89 3c 24             	mov    %edi,(%esp)
801017f8:	e8 e3 27 00 00       	call   80103fe0 <releasesleep>
  acquire(&icache.lock);
801017fd:	c7 04 24 e0 09 11 80 	movl   $0x801109e0,(%esp)
80101804:	e8 d7 29 00 00       	call   801041e0 <acquire>
  ip->ref--;
80101809:	83 6e 08 01          	subl   $0x1,0x8(%esi)
  release(&icache.lock);
8010180d:	c7 45 08 e0 09 11 80 	movl   $0x801109e0,0x8(%ebp)
}
80101814:	83 c4 1c             	add    $0x1c,%esp
80101817:	5b                   	pop    %ebx
80101818:	5e                   	pop    %esi
80101819:	5f                   	pop    %edi
8010181a:	5d                   	pop    %ebp
  release(&icache.lock);
8010181b:	e9 30 2a 00 00       	jmp    80104250 <release>
    acquire(&icache.lock);
80101820:	c7 04 24 e0 09 11 80 	movl   $0x801109e0,(%esp)
80101827:	e8 b4 29 00 00       	call   801041e0 <acquire>
    int r = ip->ref;
8010182c:	8b 5e 08             	mov    0x8(%esi),%ebx
    release(&icache.lock);
8010182f:	c7 04 24 e0 09 11 80 	movl   $0x801109e0,(%esp)
80101836:	e8 15 2a 00 00       	call   80104250 <release>
    if(r == 1){
8010183b:	83 fb 01             	cmp    $0x1,%ebx
8010183e:	75 b5                	jne    801017f5 <iput+0x25>
80101840:	8d 4e 30             	lea    0x30(%esi),%ecx
80101843:	89 f3                	mov    %esi,%ebx
80101845:	89 7d e4             	mov    %edi,-0x1c(%ebp)
80101848:	89 cf                	mov    %ecx,%edi
8010184a:	eb 0b                	jmp    80101857 <iput+0x87>
8010184c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80101850:	83 c3 04             	add    $0x4,%ebx
{
  int i, j;
  struct buf *bp;
  uint *a;

  for(i = 0; i < NDIRECT; i++){
80101853:	39 fb                	cmp    %edi,%ebx
80101855:	74 19                	je     80101870 <iput+0xa0>
    if(ip->addrs[i]){
80101857:	8b 53 5c             	mov    0x5c(%ebx),%edx
8010185a:	85 d2                	test   %edx,%edx
8010185c:	74 f2                	je     80101850 <iput+0x80>
      bfree(ip->dev, ip->addrs[i]);
8010185e:	8b 06                	mov    (%esi),%eax
80101860:	e8 9b f8 ff ff       	call   80101100 <bfree>
      ip->addrs[i] = 0;
80101865:	c7 43 5c 00 00 00 00 	movl   $0x0,0x5c(%ebx)
8010186c:	eb e2                	jmp    80101850 <iput+0x80>
8010186e:	66 90                	xchg   %ax,%ax
    }
  }

  if(ip->addrs[NDIRECT]){
80101870:	8b 86 8c 00 00 00    	mov    0x8c(%esi),%eax
80101876:	8b 7d e4             	mov    -0x1c(%ebp),%edi
80101879:	85 c0                	test   %eax,%eax
8010187b:	75 2b                	jne    801018a8 <iput+0xd8>
    brelse(bp);
    bfree(ip->dev, ip->addrs[NDIRECT]);
    ip->addrs[NDIRECT] = 0;
  }

  ip->size = 0;
8010187d:	c7 46 58 00 00 00 00 	movl   $0x0,0x58(%esi)
  iupdate(ip);
80101884:	89 34 24             	mov    %esi,(%esp)
80101887:	e8 64 fd ff ff       	call   801015f0 <iupdate>
      ip->type = 0;
8010188c:	31 c0                	xor    %eax,%eax
8010188e:	66 89 46 50          	mov    %ax,0x50(%esi)
      iupdate(ip);
80101892:	89 34 24             	mov    %esi,(%esp)
80101895:	e8 56 fd ff ff       	call   801015f0 <iupdate>
      ip->valid = 0;
8010189a:	c7 46 4c 00 00 00 00 	movl   $0x0,0x4c(%esi)
801018a1:	e9 4f ff ff ff       	jmp    801017f5 <iput+0x25>
801018a6:	66 90                	xchg   %ax,%ax
    bp = bread(ip->dev, ip->addrs[NDIRECT]);
801018a8:	89 44 24 04          	mov    %eax,0x4(%esp)
801018ac:	8b 06                	mov    (%esi),%eax
    for(j = 0; j < NINDIRECT; j++){
801018ae:	31 db                	xor    %ebx,%ebx
    bp = bread(ip->dev, ip->addrs[NDIRECT]);
801018b0:	89 04 24             	mov    %eax,(%esp)
801018b3:	e8 18 e8 ff ff       	call   801000d0 <bread>
    for(j = 0; j < NINDIRECT; j++){
801018b8:	89 7d e0             	mov    %edi,-0x20(%ebp)
    a = (uint*)bp->data;
801018bb:	8d 48 5c             	lea    0x5c(%eax),%ecx
    bp = bread(ip->dev, ip->addrs[NDIRECT]);
801018be:	89 45 e4             	mov    %eax,-0x1c(%ebp)
    for(j = 0; j < NINDIRECT; j++){
801018c1:	89 cf                	mov    %ecx,%edi
801018c3:	31 c0                	xor    %eax,%eax
801018c5:	eb 0e                	jmp    801018d5 <iput+0x105>
801018c7:	90                   	nop
801018c8:	83 c3 01             	add    $0x1,%ebx
801018cb:	81 fb 80 00 00 00    	cmp    $0x80,%ebx
801018d1:	89 d8                	mov    %ebx,%eax
801018d3:	74 10                	je     801018e5 <iput+0x115>
      if(a[j])
801018d5:	8b 14 87             	mov    (%edi,%eax,4),%edx
801018d8:	85 d2                	test   %edx,%edx
801018da:	74 ec                	je     801018c8 <iput+0xf8>
        bfree(ip->dev, a[j]);
801018dc:	8b 06                	mov    (%esi),%eax
801018de:	e8 1d f8 ff ff       	call   80101100 <bfree>
801018e3:	eb e3                	jmp    801018c8 <iput+0xf8>
    brelse(bp);
801018e5:	8b 45 e4             	mov    -0x1c(%ebp),%eax
801018e8:	8b 7d e0             	mov    -0x20(%ebp),%edi
801018eb:	89 04 24             	mov    %eax,(%esp)
801018ee:	e8 ed e8 ff ff       	call   801001e0 <brelse>
    bfree(ip->dev, ip->addrs[NDIRECT]);
801018f3:	8b 96 8c 00 00 00    	mov    0x8c(%esi),%edx
801018f9:	8b 06                	mov    (%esi),%eax
801018fb:	e8 00 f8 ff ff       	call   80101100 <bfree>
    ip->addrs[NDIRECT] = 0;
80101900:	c7 86 8c 00 00 00 00 	movl   $0x0,0x8c(%esi)
80101907:	00 00 00 
8010190a:	e9 6e ff ff ff       	jmp    8010187d <iput+0xad>
8010190f:	90                   	nop

80101910 <iunlockput>:
{
80101910:	55                   	push   %ebp
80101911:	89 e5                	mov    %esp,%ebp
80101913:	53                   	push   %ebx
80101914:	83 ec 14             	sub    $0x14,%esp
80101917:	8b 5d 08             	mov    0x8(%ebp),%ebx
  iunlock(ip);
8010191a:	89 1c 24             	mov    %ebx,(%esp)
8010191d:	e8 6e fe ff ff       	call   80101790 <iunlock>
  iput(ip);
80101922:	89 5d 08             	mov    %ebx,0x8(%ebp)
}
80101925:	83 c4 14             	add    $0x14,%esp
80101928:	5b                   	pop    %ebx
80101929:	5d                   	pop    %ebp
  iput(ip);
8010192a:	e9 a1 fe ff ff       	jmp    801017d0 <iput>
8010192f:	90                   	nop

80101930 <stati>:

// Copy stat information from inode.
// Caller must hold ip->lock.
void
stati(struct inode *ip, struct stat *st)
{
80101930:	55                   	push   %ebp
80101931:	89 e5                	mov    %esp,%ebp
80101933:	8b 55 08             	mov    0x8(%ebp),%edx
80101936:	8b 45 0c             	mov    0xc(%ebp),%eax
  st->dev = ip->dev;
80101939:	8b 0a                	mov    (%edx),%ecx
8010193b:	89 48 04             	mov    %ecx,0x4(%eax)
  st->ino = ip->inum;
8010193e:	8b 4a 04             	mov    0x4(%edx),%ecx
80101941:	89 48 08             	mov    %ecx,0x8(%eax)
  st->type = ip->type;
80101944:	0f b7 4a 50          	movzwl 0x50(%edx),%ecx
80101948:	66 89 08             	mov    %cx,(%eax)
  st->nlink = ip->nlink;
8010194b:	0f b7 4a 56          	movzwl 0x56(%edx),%ecx
8010194f:	66 89 48 0c          	mov    %cx,0xc(%eax)
  st->size = ip->size;
80101953:	8b 52 58             	mov    0x58(%edx),%edx
80101956:	89 50 10             	mov    %edx,0x10(%eax)
}
80101959:	5d                   	pop    %ebp
8010195a:	c3                   	ret    
8010195b:	90                   	nop
8010195c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80101960 <readi>:
//PAGEBREAK!
// Read data from inode.
// Caller must hold ip->lock.
int
readi(struct inode *ip, char *dst, uint off, uint n)
{
80101960:	55                   	push   %ebp
80101961:	89 e5                	mov    %esp,%ebp
80101963:	57                   	push   %edi
80101964:	56                   	push   %esi
80101965:	53                   	push   %ebx
80101966:	83 ec 2c             	sub    $0x2c,%esp
80101969:	8b 45 0c             	mov    0xc(%ebp),%eax
8010196c:	8b 7d 08             	mov    0x8(%ebp),%edi
8010196f:	8b 75 10             	mov    0x10(%ebp),%esi
80101972:	89 45 e0             	mov    %eax,-0x20(%ebp)
80101975:	8b 45 14             	mov    0x14(%ebp),%eax
  uint tot, m;
  struct buf *bp;

  if(ip->type == T_DEV){
80101978:	66 83 7f 50 03       	cmpw   $0x3,0x50(%edi)
{
8010197d:	89 45 e4             	mov    %eax,-0x1c(%ebp)
  if(ip->type == T_DEV){
80101980:	0f 84 aa 00 00 00    	je     80101a30 <readi+0xd0>
    if(ip->major < 0 || ip->major >= NDEV || !devsw[ip->major].read)
      return -1;
    return devsw[ip->major].read(ip, dst, n);
  }

  if(off > ip->size || off + n < off)
80101986:	8b 47 58             	mov    0x58(%edi),%eax
80101989:	39 f0                	cmp    %esi,%eax
8010198b:	0f 82 c7 00 00 00    	jb     80101a58 <readi+0xf8>
80101991:	8b 5d e4             	mov    -0x1c(%ebp),%ebx
80101994:	89 da                	mov    %ebx,%edx
80101996:	01 f2                	add    %esi,%edx
80101998:	0f 82 ba 00 00 00    	jb     80101a58 <readi+0xf8>
    return -1;
  if(off + n > ip->size)
    n = ip->size - off;
8010199e:	89 c1                	mov    %eax,%ecx
801019a0:	29 f1                	sub    %esi,%ecx
801019a2:	39 d0                	cmp    %edx,%eax
801019a4:	0f 43 cb             	cmovae %ebx,%ecx

  for(tot=0; tot<n; tot+=m, off+=m, dst+=m){
801019a7:	31 c0                	xor    %eax,%eax
801019a9:	85 c9                	test   %ecx,%ecx
    n = ip->size - off;
801019ab:	89 4d e4             	mov    %ecx,-0x1c(%ebp)
  for(tot=0; tot<n; tot+=m, off+=m, dst+=m){
801019ae:	74 70                	je     80101a20 <readi+0xc0>
801019b0:	89 7d d8             	mov    %edi,-0x28(%ebp)
801019b3:	89 c7                	mov    %eax,%edi
801019b5:	8d 76 00             	lea    0x0(%esi),%esi
    bp = bread(ip->dev, bmap(ip, off/BSIZE));
801019b8:	8b 5d d8             	mov    -0x28(%ebp),%ebx
801019bb:	89 f2                	mov    %esi,%edx
801019bd:	c1 ea 09             	shr    $0x9,%edx
801019c0:	89 d8                	mov    %ebx,%eax
801019c2:	e8 99 f9 ff ff       	call   80101360 <bmap>
801019c7:	89 44 24 04          	mov    %eax,0x4(%esp)
801019cb:	8b 03                	mov    (%ebx),%eax
    m = min(n - tot, BSIZE - off%BSIZE);
801019cd:	bb 00 02 00 00       	mov    $0x200,%ebx
    bp = bread(ip->dev, bmap(ip, off/BSIZE));
801019d2:	89 04 24             	mov    %eax,(%esp)
801019d5:	e8 f6 e6 ff ff       	call   801000d0 <bread>
    m = min(n - tot, BSIZE - off%BSIZE);
801019da:	8b 4d e4             	mov    -0x1c(%ebp),%ecx
801019dd:	29 f9                	sub    %edi,%ecx
    bp = bread(ip->dev, bmap(ip, off/BSIZE));
801019df:	89 c2                	mov    %eax,%edx
    m = min(n - tot, BSIZE - off%BSIZE);
801019e1:	89 f0                	mov    %esi,%eax
801019e3:	25 ff 01 00 00       	and    $0x1ff,%eax
801019e8:	29 c3                	sub    %eax,%ebx
    memmove(dst, bp->data + off%BSIZE, m);
801019ea:	8d 44 02 5c          	lea    0x5c(%edx,%eax,1),%eax
    m = min(n - tot, BSIZE - off%BSIZE);
801019ee:	39 cb                	cmp    %ecx,%ebx
    memmove(dst, bp->data + off%BSIZE, m);
801019f0:	89 44 24 04          	mov    %eax,0x4(%esp)
801019f4:	8b 45 e0             	mov    -0x20(%ebp),%eax
    m = min(n - tot, BSIZE - off%BSIZE);
801019f7:	0f 47 d9             	cmova  %ecx,%ebx
    memmove(dst, bp->data + off%BSIZE, m);
801019fa:	89 5c 24 08          	mov    %ebx,0x8(%esp)
  for(tot=0; tot<n; tot+=m, off+=m, dst+=m){
801019fe:	01 df                	add    %ebx,%edi
80101a00:	01 de                	add    %ebx,%esi
    memmove(dst, bp->data + off%BSIZE, m);
80101a02:	89 55 dc             	mov    %edx,-0x24(%ebp)
80101a05:	89 04 24             	mov    %eax,(%esp)
80101a08:	e8 33 29 00 00       	call   80104340 <memmove>
    brelse(bp);
80101a0d:	8b 55 dc             	mov    -0x24(%ebp),%edx
80101a10:	89 14 24             	mov    %edx,(%esp)
80101a13:	e8 c8 e7 ff ff       	call   801001e0 <brelse>
  for(tot=0; tot<n; tot+=m, off+=m, dst+=m){
80101a18:	01 5d e0             	add    %ebx,-0x20(%ebp)
80101a1b:	39 7d e4             	cmp    %edi,-0x1c(%ebp)
80101a1e:	77 98                	ja     801019b8 <readi+0x58>
  }
  return n;
80101a20:	8b 45 e4             	mov    -0x1c(%ebp),%eax
}
80101a23:	83 c4 2c             	add    $0x2c,%esp
80101a26:	5b                   	pop    %ebx
80101a27:	5e                   	pop    %esi
80101a28:	5f                   	pop    %edi
80101a29:	5d                   	pop    %ebp
80101a2a:	c3                   	ret    
80101a2b:	90                   	nop
80101a2c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    if(ip->major < 0 || ip->major >= NDEV || !devsw[ip->major].read)
80101a30:	0f bf 47 52          	movswl 0x52(%edi),%eax
80101a34:	66 83 f8 09          	cmp    $0x9,%ax
80101a38:	77 1e                	ja     80101a58 <readi+0xf8>
80101a3a:	8b 04 c5 60 09 11 80 	mov    -0x7feef6a0(,%eax,8),%eax
80101a41:	85 c0                	test   %eax,%eax
80101a43:	74 13                	je     80101a58 <readi+0xf8>
    return devsw[ip->major].read(ip, dst, n);
80101a45:	8b 75 e4             	mov    -0x1c(%ebp),%esi
80101a48:	89 75 10             	mov    %esi,0x10(%ebp)
}
80101a4b:	83 c4 2c             	add    $0x2c,%esp
80101a4e:	5b                   	pop    %ebx
80101a4f:	5e                   	pop    %esi
80101a50:	5f                   	pop    %edi
80101a51:	5d                   	pop    %ebp
    return devsw[ip->major].read(ip, dst, n);
80101a52:	ff e0                	jmp    *%eax
80101a54:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      return -1;
80101a58:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80101a5d:	eb c4                	jmp    80101a23 <readi+0xc3>
80101a5f:	90                   	nop

80101a60 <writei>:
// PAGEBREAK!
// Write data to inode.
// Caller must hold ip->lock.
int
writei(struct inode *ip, char *src, uint off, uint n)
{
80101a60:	55                   	push   %ebp
80101a61:	89 e5                	mov    %esp,%ebp
80101a63:	57                   	push   %edi
80101a64:	56                   	push   %esi
80101a65:	53                   	push   %ebx
80101a66:	83 ec 2c             	sub    $0x2c,%esp
80101a69:	8b 45 08             	mov    0x8(%ebp),%eax
80101a6c:	8b 75 0c             	mov    0xc(%ebp),%esi
80101a6f:	8b 4d 14             	mov    0x14(%ebp),%ecx
  uint tot, m;
  struct buf *bp;

  if(ip->type == T_DEV){
80101a72:	66 83 78 50 03       	cmpw   $0x3,0x50(%eax)
{
80101a77:	89 75 dc             	mov    %esi,-0x24(%ebp)
80101a7a:	8b 75 10             	mov    0x10(%ebp),%esi
80101a7d:	89 45 d8             	mov    %eax,-0x28(%ebp)
80101a80:	89 4d e0             	mov    %ecx,-0x20(%ebp)
  if(ip->type == T_DEV){
80101a83:	0f 84 b7 00 00 00    	je     80101b40 <writei+0xe0>
    if(ip->major < 0 || ip->major >= NDEV || !devsw[ip->major].write)
      return -1;
    return devsw[ip->major].write(ip, src, n);
  }

  if(off > ip->size || off + n < off)
80101a89:	8b 45 d8             	mov    -0x28(%ebp),%eax
80101a8c:	39 70 58             	cmp    %esi,0x58(%eax)
80101a8f:	0f 82 e3 00 00 00    	jb     80101b78 <writei+0x118>
80101a95:	8b 4d e0             	mov    -0x20(%ebp),%ecx
80101a98:	89 c8                	mov    %ecx,%eax
80101a9a:	01 f0                	add    %esi,%eax
80101a9c:	0f 82 d6 00 00 00    	jb     80101b78 <writei+0x118>
    return -1;
  if(off + n > MAXFILE*BSIZE)
80101aa2:	3d 00 18 01 00       	cmp    $0x11800,%eax
80101aa7:	0f 87 cb 00 00 00    	ja     80101b78 <writei+0x118>
    return -1;

  for(tot=0; tot<n; tot+=m, off+=m, src+=m){
80101aad:	85 c9                	test   %ecx,%ecx
80101aaf:	c7 45 e4 00 00 00 00 	movl   $0x0,-0x1c(%ebp)
80101ab6:	74 77                	je     80101b2f <writei+0xcf>
    bp = bread(ip->dev, bmap(ip, off/BSIZE));
80101ab8:	8b 7d d8             	mov    -0x28(%ebp),%edi
80101abb:	89 f2                	mov    %esi,%edx
    m = min(n - tot, BSIZE - off%BSIZE);
80101abd:	bb 00 02 00 00       	mov    $0x200,%ebx
    bp = bread(ip->dev, bmap(ip, off/BSIZE));
80101ac2:	c1 ea 09             	shr    $0x9,%edx
80101ac5:	89 f8                	mov    %edi,%eax
80101ac7:	e8 94 f8 ff ff       	call   80101360 <bmap>
80101acc:	89 44 24 04          	mov    %eax,0x4(%esp)
80101ad0:	8b 07                	mov    (%edi),%eax
80101ad2:	89 04 24             	mov    %eax,(%esp)
80101ad5:	e8 f6 e5 ff ff       	call   801000d0 <bread>
    m = min(n - tot, BSIZE - off%BSIZE);
80101ada:	8b 4d e0             	mov    -0x20(%ebp),%ecx
80101add:	2b 4d e4             	sub    -0x1c(%ebp),%ecx
    memmove(bp->data + off%BSIZE, src, m);
80101ae0:	8b 55 dc             	mov    -0x24(%ebp),%edx
    bp = bread(ip->dev, bmap(ip, off/BSIZE));
80101ae3:	89 c7                	mov    %eax,%edi
    m = min(n - tot, BSIZE - off%BSIZE);
80101ae5:	89 f0                	mov    %esi,%eax
80101ae7:	25 ff 01 00 00       	and    $0x1ff,%eax
80101aec:	29 c3                	sub    %eax,%ebx
80101aee:	39 cb                	cmp    %ecx,%ebx
80101af0:	0f 47 d9             	cmova  %ecx,%ebx
    memmove(bp->data + off%BSIZE, src, m);
80101af3:	8d 44 07 5c          	lea    0x5c(%edi,%eax,1),%eax
  for(tot=0; tot<n; tot+=m, off+=m, src+=m){
80101af7:	01 de                	add    %ebx,%esi
    memmove(bp->data + off%BSIZE, src, m);
80101af9:	89 54 24 04          	mov    %edx,0x4(%esp)
80101afd:	89 5c 24 08          	mov    %ebx,0x8(%esp)
80101b01:	89 04 24             	mov    %eax,(%esp)
80101b04:	e8 37 28 00 00       	call   80104340 <memmove>
    log_write(bp);
80101b09:	89 3c 24             	mov    %edi,(%esp)
80101b0c:	e8 9f 11 00 00       	call   80102cb0 <log_write>
    brelse(bp);
80101b11:	89 3c 24             	mov    %edi,(%esp)
80101b14:	e8 c7 e6 ff ff       	call   801001e0 <brelse>
  for(tot=0; tot<n; tot+=m, off+=m, src+=m){
80101b19:	01 5d e4             	add    %ebx,-0x1c(%ebp)
80101b1c:	8b 45 e4             	mov    -0x1c(%ebp),%eax
80101b1f:	01 5d dc             	add    %ebx,-0x24(%ebp)
80101b22:	39 45 e0             	cmp    %eax,-0x20(%ebp)
80101b25:	77 91                	ja     80101ab8 <writei+0x58>
  }

  if(n > 0 && off > ip->size){
80101b27:	8b 45 d8             	mov    -0x28(%ebp),%eax
80101b2a:	39 70 58             	cmp    %esi,0x58(%eax)
80101b2d:	72 39                	jb     80101b68 <writei+0x108>
    ip->size = off;
    iupdate(ip);
  }
  return n;
80101b2f:	8b 45 e0             	mov    -0x20(%ebp),%eax
}
80101b32:	83 c4 2c             	add    $0x2c,%esp
80101b35:	5b                   	pop    %ebx
80101b36:	5e                   	pop    %esi
80101b37:	5f                   	pop    %edi
80101b38:	5d                   	pop    %ebp
80101b39:	c3                   	ret    
80101b3a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    if(ip->major < 0 || ip->major >= NDEV || !devsw[ip->major].write)
80101b40:	0f bf 40 52          	movswl 0x52(%eax),%eax
80101b44:	66 83 f8 09          	cmp    $0x9,%ax
80101b48:	77 2e                	ja     80101b78 <writei+0x118>
80101b4a:	8b 04 c5 64 09 11 80 	mov    -0x7feef69c(,%eax,8),%eax
80101b51:	85 c0                	test   %eax,%eax
80101b53:	74 23                	je     80101b78 <writei+0x118>
    return devsw[ip->major].write(ip, src, n);
80101b55:	89 4d 10             	mov    %ecx,0x10(%ebp)
}
80101b58:	83 c4 2c             	add    $0x2c,%esp
80101b5b:	5b                   	pop    %ebx
80101b5c:	5e                   	pop    %esi
80101b5d:	5f                   	pop    %edi
80101b5e:	5d                   	pop    %ebp
    return devsw[ip->major].write(ip, src, n);
80101b5f:	ff e0                	jmp    *%eax
80101b61:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
    ip->size = off;
80101b68:	8b 45 d8             	mov    -0x28(%ebp),%eax
80101b6b:	89 70 58             	mov    %esi,0x58(%eax)
    iupdate(ip);
80101b6e:	89 04 24             	mov    %eax,(%esp)
80101b71:	e8 7a fa ff ff       	call   801015f0 <iupdate>
80101b76:	eb b7                	jmp    80101b2f <writei+0xcf>
}
80101b78:	83 c4 2c             	add    $0x2c,%esp
      return -1;
80101b7b:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80101b80:	5b                   	pop    %ebx
80101b81:	5e                   	pop    %esi
80101b82:	5f                   	pop    %edi
80101b83:	5d                   	pop    %ebp
80101b84:	c3                   	ret    
80101b85:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80101b89:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80101b90 <namecmp>:
//PAGEBREAK!
// Directories

int
namecmp(const char *s, const char *t)
{
80101b90:	55                   	push   %ebp
80101b91:	89 e5                	mov    %esp,%ebp
80101b93:	83 ec 18             	sub    $0x18,%esp
  return strncmp(s, t, DIRSIZ);
80101b96:	8b 45 0c             	mov    0xc(%ebp),%eax
80101b99:	c7 44 24 08 0e 00 00 	movl   $0xe,0x8(%esp)
80101ba0:	00 
80101ba1:	89 44 24 04          	mov    %eax,0x4(%esp)
80101ba5:	8b 45 08             	mov    0x8(%ebp),%eax
80101ba8:	89 04 24             	mov    %eax,(%esp)
80101bab:	e8 10 28 00 00       	call   801043c0 <strncmp>
}
80101bb0:	c9                   	leave  
80101bb1:	c3                   	ret    
80101bb2:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80101bb9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80101bc0 <dirlookup>:

// Look for a directory entry in a directory.
// If found, set *poff to byte offset of entry.
struct inode*
dirlookup(struct inode *dp, char *name, uint *poff)
{
80101bc0:	55                   	push   %ebp
80101bc1:	89 e5                	mov    %esp,%ebp
80101bc3:	57                   	push   %edi
80101bc4:	56                   	push   %esi
80101bc5:	53                   	push   %ebx
80101bc6:	83 ec 2c             	sub    $0x2c,%esp
80101bc9:	8b 5d 08             	mov    0x8(%ebp),%ebx
  uint off, inum;
  struct dirent de;

  if(dp->type != T_DIR)
80101bcc:	66 83 7b 50 01       	cmpw   $0x1,0x50(%ebx)
80101bd1:	0f 85 97 00 00 00    	jne    80101c6e <dirlookup+0xae>
    panic("dirlookup not DIR");

  for(off = 0; off < dp->size; off += sizeof(de)){
80101bd7:	8b 53 58             	mov    0x58(%ebx),%edx
80101bda:	31 ff                	xor    %edi,%edi
80101bdc:	8d 75 d8             	lea    -0x28(%ebp),%esi
80101bdf:	85 d2                	test   %edx,%edx
80101be1:	75 0d                	jne    80101bf0 <dirlookup+0x30>
80101be3:	eb 73                	jmp    80101c58 <dirlookup+0x98>
80101be5:	8d 76 00             	lea    0x0(%esi),%esi
80101be8:	83 c7 10             	add    $0x10,%edi
80101beb:	39 7b 58             	cmp    %edi,0x58(%ebx)
80101bee:	76 68                	jbe    80101c58 <dirlookup+0x98>
    if(readi(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
80101bf0:	c7 44 24 0c 10 00 00 	movl   $0x10,0xc(%esp)
80101bf7:	00 
80101bf8:	89 7c 24 08          	mov    %edi,0x8(%esp)
80101bfc:	89 74 24 04          	mov    %esi,0x4(%esp)
80101c00:	89 1c 24             	mov    %ebx,(%esp)
80101c03:	e8 58 fd ff ff       	call   80101960 <readi>
80101c08:	83 f8 10             	cmp    $0x10,%eax
80101c0b:	75 55                	jne    80101c62 <dirlookup+0xa2>
      panic("dirlookup read");
    if(de.inum == 0)
80101c0d:	66 83 7d d8 00       	cmpw   $0x0,-0x28(%ebp)
80101c12:	74 d4                	je     80101be8 <dirlookup+0x28>
  return strncmp(s, t, DIRSIZ);
80101c14:	8d 45 da             	lea    -0x26(%ebp),%eax
80101c17:	89 44 24 04          	mov    %eax,0x4(%esp)
80101c1b:	8b 45 0c             	mov    0xc(%ebp),%eax
80101c1e:	c7 44 24 08 0e 00 00 	movl   $0xe,0x8(%esp)
80101c25:	00 
80101c26:	89 04 24             	mov    %eax,(%esp)
80101c29:	e8 92 27 00 00       	call   801043c0 <strncmp>
      continue;
    if(namecmp(name, de.name) == 0){
80101c2e:	85 c0                	test   %eax,%eax
80101c30:	75 b6                	jne    80101be8 <dirlookup+0x28>
      // entry matches path element
      if(poff)
80101c32:	8b 45 10             	mov    0x10(%ebp),%eax
80101c35:	85 c0                	test   %eax,%eax
80101c37:	74 05                	je     80101c3e <dirlookup+0x7e>
        *poff = off;
80101c39:	8b 45 10             	mov    0x10(%ebp),%eax
80101c3c:	89 38                	mov    %edi,(%eax)
      inum = de.inum;
80101c3e:	0f b7 55 d8          	movzwl -0x28(%ebp),%edx
      return iget(dp->dev, inum);
80101c42:	8b 03                	mov    (%ebx),%eax
80101c44:	e8 57 f6 ff ff       	call   801012a0 <iget>
    }
  }

  return 0;
}
80101c49:	83 c4 2c             	add    $0x2c,%esp
80101c4c:	5b                   	pop    %ebx
80101c4d:	5e                   	pop    %esi
80101c4e:	5f                   	pop    %edi
80101c4f:	5d                   	pop    %ebp
80101c50:	c3                   	ret    
80101c51:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80101c58:	83 c4 2c             	add    $0x2c,%esp
  return 0;
80101c5b:	31 c0                	xor    %eax,%eax
}
80101c5d:	5b                   	pop    %ebx
80101c5e:	5e                   	pop    %esi
80101c5f:	5f                   	pop    %edi
80101c60:	5d                   	pop    %ebp
80101c61:	c3                   	ret    
      panic("dirlookup read");
80101c62:	c7 04 24 99 6d 10 80 	movl   $0x80106d99,(%esp)
80101c69:	e8 f2 e6 ff ff       	call   80100360 <panic>
    panic("dirlookup not DIR");
80101c6e:	c7 04 24 87 6d 10 80 	movl   $0x80106d87,(%esp)
80101c75:	e8 e6 e6 ff ff       	call   80100360 <panic>
80101c7a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

80101c80 <namex>:
// If parent != 0, return the inode for the parent and copy the final
// path element into name, which must have room for DIRSIZ bytes.
// Must be called inside a transaction since it calls iput().
static struct inode*
namex(char *path, int nameiparent, char *name)
{
80101c80:	55                   	push   %ebp
80101c81:	89 e5                	mov    %esp,%ebp
80101c83:	57                   	push   %edi
80101c84:	89 cf                	mov    %ecx,%edi
80101c86:	56                   	push   %esi
80101c87:	53                   	push   %ebx
80101c88:	89 c3                	mov    %eax,%ebx
80101c8a:	83 ec 2c             	sub    $0x2c,%esp
  struct inode *ip, *next;

  if(*path == '/')
80101c8d:	80 38 2f             	cmpb   $0x2f,(%eax)
{
80101c90:	89 55 e0             	mov    %edx,-0x20(%ebp)
  if(*path == '/')
80101c93:	0f 84 51 01 00 00    	je     80101dea <namex+0x16a>
    ip = iget(ROOTDEV, ROOTINO);
  else
    ip = idup(myproc()->cwd);
80101c99:	e8 02 1a 00 00       	call   801036a0 <myproc>
80101c9e:	8b 70 68             	mov    0x68(%eax),%esi
  acquire(&icache.lock);
80101ca1:	c7 04 24 e0 09 11 80 	movl   $0x801109e0,(%esp)
80101ca8:	e8 33 25 00 00       	call   801041e0 <acquire>
  ip->ref++;
80101cad:	83 46 08 01          	addl   $0x1,0x8(%esi)
  release(&icache.lock);
80101cb1:	c7 04 24 e0 09 11 80 	movl   $0x801109e0,(%esp)
80101cb8:	e8 93 25 00 00       	call   80104250 <release>
80101cbd:	eb 04                	jmp    80101cc3 <namex+0x43>
80101cbf:	90                   	nop
    path++;
80101cc0:	83 c3 01             	add    $0x1,%ebx
  while(*path == '/')
80101cc3:	0f b6 03             	movzbl (%ebx),%eax
80101cc6:	3c 2f                	cmp    $0x2f,%al
80101cc8:	74 f6                	je     80101cc0 <namex+0x40>
  if(*path == 0)
80101cca:	84 c0                	test   %al,%al
80101ccc:	0f 84 ed 00 00 00    	je     80101dbf <namex+0x13f>
  while(*path != '/' && *path != 0)
80101cd2:	0f b6 03             	movzbl (%ebx),%eax
80101cd5:	89 da                	mov    %ebx,%edx
80101cd7:	84 c0                	test   %al,%al
80101cd9:	0f 84 b1 00 00 00    	je     80101d90 <namex+0x110>
80101cdf:	3c 2f                	cmp    $0x2f,%al
80101ce1:	75 0f                	jne    80101cf2 <namex+0x72>
80101ce3:	e9 a8 00 00 00       	jmp    80101d90 <namex+0x110>
80101ce8:	3c 2f                	cmp    $0x2f,%al
80101cea:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80101cf0:	74 0a                	je     80101cfc <namex+0x7c>
    path++;
80101cf2:	83 c2 01             	add    $0x1,%edx
  while(*path != '/' && *path != 0)
80101cf5:	0f b6 02             	movzbl (%edx),%eax
80101cf8:	84 c0                	test   %al,%al
80101cfa:	75 ec                	jne    80101ce8 <namex+0x68>
80101cfc:	89 d1                	mov    %edx,%ecx
80101cfe:	29 d9                	sub    %ebx,%ecx
  if(len >= DIRSIZ)
80101d00:	83 f9 0d             	cmp    $0xd,%ecx
80101d03:	0f 8e 8f 00 00 00    	jle    80101d98 <namex+0x118>
    memmove(name, s, DIRSIZ);
80101d09:	89 5c 24 04          	mov    %ebx,0x4(%esp)
80101d0d:	c7 44 24 08 0e 00 00 	movl   $0xe,0x8(%esp)
80101d14:	00 
80101d15:	89 3c 24             	mov    %edi,(%esp)
80101d18:	89 55 e4             	mov    %edx,-0x1c(%ebp)
80101d1b:	e8 20 26 00 00       	call   80104340 <memmove>
    path++;
80101d20:	8b 55 e4             	mov    -0x1c(%ebp),%edx
80101d23:	89 d3                	mov    %edx,%ebx
  while(*path == '/')
80101d25:	80 3a 2f             	cmpb   $0x2f,(%edx)
80101d28:	75 0e                	jne    80101d38 <namex+0xb8>
80101d2a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    path++;
80101d30:	83 c3 01             	add    $0x1,%ebx
  while(*path == '/')
80101d33:	80 3b 2f             	cmpb   $0x2f,(%ebx)
80101d36:	74 f8                	je     80101d30 <namex+0xb0>

  while((path = skipelem(path, name)) != 0){
    ilock(ip);
80101d38:	89 34 24             	mov    %esi,(%esp)
80101d3b:	e8 70 f9 ff ff       	call   801016b0 <ilock>
    if(ip->type != T_DIR){
80101d40:	66 83 7e 50 01       	cmpw   $0x1,0x50(%esi)
80101d45:	0f 85 85 00 00 00    	jne    80101dd0 <namex+0x150>
      iunlockput(ip);
      return 0;
    }
    if(nameiparent && *path == '\0'){
80101d4b:	8b 55 e0             	mov    -0x20(%ebp),%edx
80101d4e:	85 d2                	test   %edx,%edx
80101d50:	74 09                	je     80101d5b <namex+0xdb>
80101d52:	80 3b 00             	cmpb   $0x0,(%ebx)
80101d55:	0f 84 a5 00 00 00    	je     80101e00 <namex+0x180>
      // Stop one level early.
      iunlock(ip);
      return ip;
    }
    if((next = dirlookup(ip, name, 0)) == 0){
80101d5b:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
80101d62:	00 
80101d63:	89 7c 24 04          	mov    %edi,0x4(%esp)
80101d67:	89 34 24             	mov    %esi,(%esp)
80101d6a:	e8 51 fe ff ff       	call   80101bc0 <dirlookup>
80101d6f:	85 c0                	test   %eax,%eax
80101d71:	74 5d                	je     80101dd0 <namex+0x150>
  iunlock(ip);
80101d73:	89 34 24             	mov    %esi,(%esp)
80101d76:	89 45 e4             	mov    %eax,-0x1c(%ebp)
80101d79:	e8 12 fa ff ff       	call   80101790 <iunlock>
  iput(ip);
80101d7e:	89 34 24             	mov    %esi,(%esp)
80101d81:	e8 4a fa ff ff       	call   801017d0 <iput>
      iunlockput(ip);
      return 0;
    }
    iunlockput(ip);
    ip = next;
80101d86:	8b 45 e4             	mov    -0x1c(%ebp),%eax
80101d89:	89 c6                	mov    %eax,%esi
80101d8b:	e9 33 ff ff ff       	jmp    80101cc3 <namex+0x43>
  while(*path != '/' && *path != 0)
80101d90:	31 c9                	xor    %ecx,%ecx
80101d92:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    memmove(name, s, len);
80101d98:	89 4c 24 08          	mov    %ecx,0x8(%esp)
80101d9c:	89 5c 24 04          	mov    %ebx,0x4(%esp)
80101da0:	89 3c 24             	mov    %edi,(%esp)
80101da3:	89 55 dc             	mov    %edx,-0x24(%ebp)
80101da6:	89 4d e4             	mov    %ecx,-0x1c(%ebp)
80101da9:	e8 92 25 00 00       	call   80104340 <memmove>
    name[len] = 0;
80101dae:	8b 4d e4             	mov    -0x1c(%ebp),%ecx
80101db1:	8b 55 dc             	mov    -0x24(%ebp),%edx
80101db4:	c6 04 0f 00          	movb   $0x0,(%edi,%ecx,1)
80101db8:	89 d3                	mov    %edx,%ebx
80101dba:	e9 66 ff ff ff       	jmp    80101d25 <namex+0xa5>
  }
  if(nameiparent){
80101dbf:	8b 45 e0             	mov    -0x20(%ebp),%eax
80101dc2:	85 c0                	test   %eax,%eax
80101dc4:	75 4c                	jne    80101e12 <namex+0x192>
80101dc6:	89 f0                	mov    %esi,%eax
    iput(ip);
    return 0;
  }
  return ip;
}
80101dc8:	83 c4 2c             	add    $0x2c,%esp
80101dcb:	5b                   	pop    %ebx
80101dcc:	5e                   	pop    %esi
80101dcd:	5f                   	pop    %edi
80101dce:	5d                   	pop    %ebp
80101dcf:	c3                   	ret    
  iunlock(ip);
80101dd0:	89 34 24             	mov    %esi,(%esp)
80101dd3:	e8 b8 f9 ff ff       	call   80101790 <iunlock>
  iput(ip);
80101dd8:	89 34 24             	mov    %esi,(%esp)
80101ddb:	e8 f0 f9 ff ff       	call   801017d0 <iput>
}
80101de0:	83 c4 2c             	add    $0x2c,%esp
      return 0;
80101de3:	31 c0                	xor    %eax,%eax
}
80101de5:	5b                   	pop    %ebx
80101de6:	5e                   	pop    %esi
80101de7:	5f                   	pop    %edi
80101de8:	5d                   	pop    %ebp
80101de9:	c3                   	ret    
    ip = iget(ROOTDEV, ROOTINO);
80101dea:	ba 01 00 00 00       	mov    $0x1,%edx
80101def:	b8 01 00 00 00       	mov    $0x1,%eax
80101df4:	e8 a7 f4 ff ff       	call   801012a0 <iget>
80101df9:	89 c6                	mov    %eax,%esi
80101dfb:	e9 c3 fe ff ff       	jmp    80101cc3 <namex+0x43>
      iunlock(ip);
80101e00:	89 34 24             	mov    %esi,(%esp)
80101e03:	e8 88 f9 ff ff       	call   80101790 <iunlock>
}
80101e08:	83 c4 2c             	add    $0x2c,%esp
      return ip;
80101e0b:	89 f0                	mov    %esi,%eax
}
80101e0d:	5b                   	pop    %ebx
80101e0e:	5e                   	pop    %esi
80101e0f:	5f                   	pop    %edi
80101e10:	5d                   	pop    %ebp
80101e11:	c3                   	ret    
    iput(ip);
80101e12:	89 34 24             	mov    %esi,(%esp)
80101e15:	e8 b6 f9 ff ff       	call   801017d0 <iput>
    return 0;
80101e1a:	31 c0                	xor    %eax,%eax
80101e1c:	eb aa                	jmp    80101dc8 <namex+0x148>
80101e1e:	66 90                	xchg   %ax,%ax

80101e20 <dirlink>:
{
80101e20:	55                   	push   %ebp
80101e21:	89 e5                	mov    %esp,%ebp
80101e23:	57                   	push   %edi
80101e24:	56                   	push   %esi
80101e25:	53                   	push   %ebx
80101e26:	83 ec 2c             	sub    $0x2c,%esp
80101e29:	8b 5d 08             	mov    0x8(%ebp),%ebx
  if((ip = dirlookup(dp, name, 0)) != 0){
80101e2c:	8b 45 0c             	mov    0xc(%ebp),%eax
80101e2f:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
80101e36:	00 
80101e37:	89 1c 24             	mov    %ebx,(%esp)
80101e3a:	89 44 24 04          	mov    %eax,0x4(%esp)
80101e3e:	e8 7d fd ff ff       	call   80101bc0 <dirlookup>
80101e43:	85 c0                	test   %eax,%eax
80101e45:	0f 85 8b 00 00 00    	jne    80101ed6 <dirlink+0xb6>
  for(off = 0; off < dp->size; off += sizeof(de)){
80101e4b:	8b 43 58             	mov    0x58(%ebx),%eax
80101e4e:	31 ff                	xor    %edi,%edi
80101e50:	8d 75 d8             	lea    -0x28(%ebp),%esi
80101e53:	85 c0                	test   %eax,%eax
80101e55:	75 13                	jne    80101e6a <dirlink+0x4a>
80101e57:	eb 35                	jmp    80101e8e <dirlink+0x6e>
80101e59:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80101e60:	8d 57 10             	lea    0x10(%edi),%edx
80101e63:	39 53 58             	cmp    %edx,0x58(%ebx)
80101e66:	89 d7                	mov    %edx,%edi
80101e68:	76 24                	jbe    80101e8e <dirlink+0x6e>
    if(readi(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
80101e6a:	c7 44 24 0c 10 00 00 	movl   $0x10,0xc(%esp)
80101e71:	00 
80101e72:	89 7c 24 08          	mov    %edi,0x8(%esp)
80101e76:	89 74 24 04          	mov    %esi,0x4(%esp)
80101e7a:	89 1c 24             	mov    %ebx,(%esp)
80101e7d:	e8 de fa ff ff       	call   80101960 <readi>
80101e82:	83 f8 10             	cmp    $0x10,%eax
80101e85:	75 5e                	jne    80101ee5 <dirlink+0xc5>
    if(de.inum == 0)
80101e87:	66 83 7d d8 00       	cmpw   $0x0,-0x28(%ebp)
80101e8c:	75 d2                	jne    80101e60 <dirlink+0x40>
  strncpy(de.name, name, DIRSIZ);
80101e8e:	8b 45 0c             	mov    0xc(%ebp),%eax
80101e91:	c7 44 24 08 0e 00 00 	movl   $0xe,0x8(%esp)
80101e98:	00 
80101e99:	89 44 24 04          	mov    %eax,0x4(%esp)
80101e9d:	8d 45 da             	lea    -0x26(%ebp),%eax
80101ea0:	89 04 24             	mov    %eax,(%esp)
80101ea3:	e8 88 25 00 00       	call   80104430 <strncpy>
  de.inum = inum;
80101ea8:	8b 45 10             	mov    0x10(%ebp),%eax
  if(writei(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
80101eab:	c7 44 24 0c 10 00 00 	movl   $0x10,0xc(%esp)
80101eb2:	00 
80101eb3:	89 7c 24 08          	mov    %edi,0x8(%esp)
80101eb7:	89 74 24 04          	mov    %esi,0x4(%esp)
80101ebb:	89 1c 24             	mov    %ebx,(%esp)
  de.inum = inum;
80101ebe:	66 89 45 d8          	mov    %ax,-0x28(%ebp)
  if(writei(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
80101ec2:	e8 99 fb ff ff       	call   80101a60 <writei>
80101ec7:	83 f8 10             	cmp    $0x10,%eax
80101eca:	75 25                	jne    80101ef1 <dirlink+0xd1>
  return 0;
80101ecc:	31 c0                	xor    %eax,%eax
}
80101ece:	83 c4 2c             	add    $0x2c,%esp
80101ed1:	5b                   	pop    %ebx
80101ed2:	5e                   	pop    %esi
80101ed3:	5f                   	pop    %edi
80101ed4:	5d                   	pop    %ebp
80101ed5:	c3                   	ret    
    iput(ip);
80101ed6:	89 04 24             	mov    %eax,(%esp)
80101ed9:	e8 f2 f8 ff ff       	call   801017d0 <iput>
    return -1;
80101ede:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80101ee3:	eb e9                	jmp    80101ece <dirlink+0xae>
      panic("dirlink read");
80101ee5:	c7 04 24 a8 6d 10 80 	movl   $0x80106da8,(%esp)
80101eec:	e8 6f e4 ff ff       	call   80100360 <panic>
    panic("dirlink");
80101ef1:	c7 04 24 9e 73 10 80 	movl   $0x8010739e,(%esp)
80101ef8:	e8 63 e4 ff ff       	call   80100360 <panic>
80101efd:	8d 76 00             	lea    0x0(%esi),%esi

80101f00 <namei>:

struct inode*
namei(char *path)
{
80101f00:	55                   	push   %ebp
  char name[DIRSIZ];
  return namex(path, 0, name);
80101f01:	31 d2                	xor    %edx,%edx
{
80101f03:	89 e5                	mov    %esp,%ebp
80101f05:	83 ec 18             	sub    $0x18,%esp
  return namex(path, 0, name);
80101f08:	8b 45 08             	mov    0x8(%ebp),%eax
80101f0b:	8d 4d ea             	lea    -0x16(%ebp),%ecx
80101f0e:	e8 6d fd ff ff       	call   80101c80 <namex>
}
80101f13:	c9                   	leave  
80101f14:	c3                   	ret    
80101f15:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80101f19:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80101f20 <nameiparent>:

struct inode*
nameiparent(char *path, char *name)
{
80101f20:	55                   	push   %ebp
  return namex(path, 1, name);
80101f21:	ba 01 00 00 00       	mov    $0x1,%edx
{
80101f26:	89 e5                	mov    %esp,%ebp
  return namex(path, 1, name);
80101f28:	8b 4d 0c             	mov    0xc(%ebp),%ecx
80101f2b:	8b 45 08             	mov    0x8(%ebp),%eax
}
80101f2e:	5d                   	pop    %ebp
  return namex(path, 1, name);
80101f2f:	e9 4c fd ff ff       	jmp    80101c80 <namex>
80101f34:	66 90                	xchg   %ax,%ax
80101f36:	66 90                	xchg   %ax,%ax
80101f38:	66 90                	xchg   %ax,%ax
80101f3a:	66 90                	xchg   %ax,%ax
80101f3c:	66 90                	xchg   %ax,%ax
80101f3e:	66 90                	xchg   %ax,%ax

80101f40 <idestart>:
}

// Start the request for b.  Caller must hold idelock.
static void
idestart(struct buf *b)
{
80101f40:	55                   	push   %ebp
80101f41:	89 e5                	mov    %esp,%ebp
80101f43:	56                   	push   %esi
80101f44:	89 c6                	mov    %eax,%esi
80101f46:	53                   	push   %ebx
80101f47:	83 ec 10             	sub    $0x10,%esp
  if(b == 0)
80101f4a:	85 c0                	test   %eax,%eax
80101f4c:	0f 84 99 00 00 00    	je     80101feb <idestart+0xab>
    panic("idestart");
  if(b->blockno >= FSSIZE)
80101f52:	8b 48 08             	mov    0x8(%eax),%ecx
80101f55:	81 f9 e7 03 00 00    	cmp    $0x3e7,%ecx
80101f5b:	0f 87 7e 00 00 00    	ja     80101fdf <idestart+0x9f>
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
80101f61:	ba f7 01 00 00       	mov    $0x1f7,%edx
80101f66:	66 90                	xchg   %ax,%ax
80101f68:	ec                   	in     (%dx),%al
  while(((r = inb(0x1f7)) & (IDE_BSY|IDE_DRDY)) != IDE_DRDY)
80101f69:	83 e0 c0             	and    $0xffffffc0,%eax
80101f6c:	3c 40                	cmp    $0x40,%al
80101f6e:	75 f8                	jne    80101f68 <idestart+0x28>
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
80101f70:	31 db                	xor    %ebx,%ebx
80101f72:	ba f6 03 00 00       	mov    $0x3f6,%edx
80101f77:	89 d8                	mov    %ebx,%eax
80101f79:	ee                   	out    %al,(%dx)
80101f7a:	ba f2 01 00 00       	mov    $0x1f2,%edx
80101f7f:	b8 01 00 00 00       	mov    $0x1,%eax
80101f84:	ee                   	out    %al,(%dx)
80101f85:	0f b6 c1             	movzbl %cl,%eax
80101f88:	b2 f3                	mov    $0xf3,%dl
80101f8a:	ee                   	out    %al,(%dx)

  idewait(0);
  outb(0x3f6, 0);  // generate interrupt
  outb(0x1f2, sector_per_block);  // number of sectors
  outb(0x1f3, sector & 0xff);
  outb(0x1f4, (sector >> 8) & 0xff);
80101f8b:	89 c8                	mov    %ecx,%eax
80101f8d:	b2 f4                	mov    $0xf4,%dl
80101f8f:	c1 f8 08             	sar    $0x8,%eax
80101f92:	ee                   	out    %al,(%dx)
80101f93:	b2 f5                	mov    $0xf5,%dl
80101f95:	89 d8                	mov    %ebx,%eax
80101f97:	ee                   	out    %al,(%dx)
  outb(0x1f5, (sector >> 16) & 0xff);
  outb(0x1f6, 0xe0 | ((b->dev&1)<<4) | ((sector>>24)&0x0f));
80101f98:	0f b6 46 04          	movzbl 0x4(%esi),%eax
80101f9c:	b2 f6                	mov    $0xf6,%dl
80101f9e:	83 e0 01             	and    $0x1,%eax
80101fa1:	c1 e0 04             	shl    $0x4,%eax
80101fa4:	83 c8 e0             	or     $0xffffffe0,%eax
80101fa7:	ee                   	out    %al,(%dx)
  if(b->flags & B_DIRTY){
80101fa8:	f6 06 04             	testb  $0x4,(%esi)
80101fab:	75 13                	jne    80101fc0 <idestart+0x80>
80101fad:	ba f7 01 00 00       	mov    $0x1f7,%edx
80101fb2:	b8 20 00 00 00       	mov    $0x20,%eax
80101fb7:	ee                   	out    %al,(%dx)
    outb(0x1f7, write_cmd);
    outsl(0x1f0, b->data, BSIZE/4);
  } else {
    outb(0x1f7, read_cmd);
  }
}
80101fb8:	83 c4 10             	add    $0x10,%esp
80101fbb:	5b                   	pop    %ebx
80101fbc:	5e                   	pop    %esi
80101fbd:	5d                   	pop    %ebp
80101fbe:	c3                   	ret    
80101fbf:	90                   	nop
80101fc0:	b2 f7                	mov    $0xf7,%dl
80101fc2:	b8 30 00 00 00       	mov    $0x30,%eax
80101fc7:	ee                   	out    %al,(%dx)
  asm volatile("cld; rep outsl" :
80101fc8:	b9 80 00 00 00       	mov    $0x80,%ecx
    outsl(0x1f0, b->data, BSIZE/4);
80101fcd:	83 c6 5c             	add    $0x5c,%esi
80101fd0:	ba f0 01 00 00       	mov    $0x1f0,%edx
80101fd5:	fc                   	cld    
80101fd6:	f3 6f                	rep outsl %ds:(%esi),(%dx)
}
80101fd8:	83 c4 10             	add    $0x10,%esp
80101fdb:	5b                   	pop    %ebx
80101fdc:	5e                   	pop    %esi
80101fdd:	5d                   	pop    %ebp
80101fde:	c3                   	ret    
    panic("incorrect blockno");
80101fdf:	c7 04 24 14 6e 10 80 	movl   $0x80106e14,(%esp)
80101fe6:	e8 75 e3 ff ff       	call   80100360 <panic>
    panic("idestart");
80101feb:	c7 04 24 0b 6e 10 80 	movl   $0x80106e0b,(%esp)
80101ff2:	e8 69 e3 ff ff       	call   80100360 <panic>
80101ff7:	89 f6                	mov    %esi,%esi
80101ff9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80102000 <ideinit>:
{
80102000:	55                   	push   %ebp
80102001:	89 e5                	mov    %esp,%ebp
80102003:	83 ec 18             	sub    $0x18,%esp
  initlock(&idelock, "ide");
80102006:	c7 44 24 04 26 6e 10 	movl   $0x80106e26,0x4(%esp)
8010200d:	80 
8010200e:	c7 04 24 80 a5 10 80 	movl   $0x8010a580,(%esp)
80102015:	e8 56 20 00 00       	call   80104070 <initlock>
  ioapicenable(IRQ_IDE, ncpu - 1);
8010201a:	a1 00 2d 11 80       	mov    0x80112d00,%eax
8010201f:	c7 04 24 0e 00 00 00 	movl   $0xe,(%esp)
80102026:	83 e8 01             	sub    $0x1,%eax
80102029:	89 44 24 04          	mov    %eax,0x4(%esp)
8010202d:	e8 7e 02 00 00       	call   801022b0 <ioapicenable>
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
80102032:	ba f7 01 00 00       	mov    $0x1f7,%edx
80102037:	90                   	nop
80102038:	ec                   	in     (%dx),%al
  while(((r = inb(0x1f7)) & (IDE_BSY|IDE_DRDY)) != IDE_DRDY)
80102039:	83 e0 c0             	and    $0xffffffc0,%eax
8010203c:	3c 40                	cmp    $0x40,%al
8010203e:	75 f8                	jne    80102038 <ideinit+0x38>
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
80102040:	ba f6 01 00 00       	mov    $0x1f6,%edx
80102045:	b8 f0 ff ff ff       	mov    $0xfffffff0,%eax
8010204a:	ee                   	out    %al,(%dx)
8010204b:	b9 e8 03 00 00       	mov    $0x3e8,%ecx
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
80102050:	b2 f7                	mov    $0xf7,%dl
80102052:	eb 09                	jmp    8010205d <ideinit+0x5d>
80102054:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  for(i=0; i<1000; i++){
80102058:	83 e9 01             	sub    $0x1,%ecx
8010205b:	74 0f                	je     8010206c <ideinit+0x6c>
8010205d:	ec                   	in     (%dx),%al
    if(inb(0x1f7) != 0){
8010205e:	84 c0                	test   %al,%al
80102060:	74 f6                	je     80102058 <ideinit+0x58>
      havedisk1 = 1;
80102062:	c7 05 60 a5 10 80 01 	movl   $0x1,0x8010a560
80102069:	00 00 00 
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
8010206c:	ba f6 01 00 00       	mov    $0x1f6,%edx
80102071:	b8 e0 ff ff ff       	mov    $0xffffffe0,%eax
80102076:	ee                   	out    %al,(%dx)
}
80102077:	c9                   	leave  
80102078:	c3                   	ret    
80102079:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

80102080 <ideintr>:

// Interrupt handler.
void
ideintr(void)
{
80102080:	55                   	push   %ebp
80102081:	89 e5                	mov    %esp,%ebp
80102083:	57                   	push   %edi
80102084:	56                   	push   %esi
80102085:	53                   	push   %ebx
80102086:	83 ec 1c             	sub    $0x1c,%esp
  struct buf *b;

  // First queued buffer is the active request.
  acquire(&idelock);
80102089:	c7 04 24 80 a5 10 80 	movl   $0x8010a580,(%esp)
80102090:	e8 4b 21 00 00       	call   801041e0 <acquire>

  if((b = idequeue) == 0){
80102095:	8b 1d 64 a5 10 80    	mov    0x8010a564,%ebx
8010209b:	85 db                	test   %ebx,%ebx
8010209d:	74 30                	je     801020cf <ideintr+0x4f>
    release(&idelock);
    return;
  }
  idequeue = b->qnext;
8010209f:	8b 43 58             	mov    0x58(%ebx),%eax
801020a2:	a3 64 a5 10 80       	mov    %eax,0x8010a564

  // Read data if needed.
  if(!(b->flags & B_DIRTY) && idewait(1) >= 0)
801020a7:	8b 33                	mov    (%ebx),%esi
801020a9:	f7 c6 04 00 00 00    	test   $0x4,%esi
801020af:	74 37                	je     801020e8 <ideintr+0x68>
    insl(0x1f0, b->data, BSIZE/4);

  // Wake process waiting for this buf.
  b->flags |= B_VALID;
  b->flags &= ~B_DIRTY;
801020b1:	83 e6 fb             	and    $0xfffffffb,%esi
801020b4:	83 ce 02             	or     $0x2,%esi
801020b7:	89 33                	mov    %esi,(%ebx)
  wakeup(b);
801020b9:	89 1c 24             	mov    %ebx,(%esp)
801020bc:	e8 cf 1c 00 00       	call   80103d90 <wakeup>

  // Start disk on next buf in queue.
  if(idequeue != 0)
801020c1:	a1 64 a5 10 80       	mov    0x8010a564,%eax
801020c6:	85 c0                	test   %eax,%eax
801020c8:	74 05                	je     801020cf <ideintr+0x4f>
    idestart(idequeue);
801020ca:	e8 71 fe ff ff       	call   80101f40 <idestart>
    release(&idelock);
801020cf:	c7 04 24 80 a5 10 80 	movl   $0x8010a580,(%esp)
801020d6:	e8 75 21 00 00       	call   80104250 <release>

  release(&idelock);
}
801020db:	83 c4 1c             	add    $0x1c,%esp
801020de:	5b                   	pop    %ebx
801020df:	5e                   	pop    %esi
801020e0:	5f                   	pop    %edi
801020e1:	5d                   	pop    %ebp
801020e2:	c3                   	ret    
801020e3:	90                   	nop
801020e4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
801020e8:	ba f7 01 00 00       	mov    $0x1f7,%edx
801020ed:	8d 76 00             	lea    0x0(%esi),%esi
801020f0:	ec                   	in     (%dx),%al
  while(((r = inb(0x1f7)) & (IDE_BSY|IDE_DRDY)) != IDE_DRDY)
801020f1:	89 c1                	mov    %eax,%ecx
801020f3:	83 e1 c0             	and    $0xffffffc0,%ecx
801020f6:	80 f9 40             	cmp    $0x40,%cl
801020f9:	75 f5                	jne    801020f0 <ideintr+0x70>
  if(checkerr && (r & (IDE_DF|IDE_ERR)) != 0)
801020fb:	a8 21                	test   $0x21,%al
801020fd:	75 b2                	jne    801020b1 <ideintr+0x31>
    insl(0x1f0, b->data, BSIZE/4);
801020ff:	8d 7b 5c             	lea    0x5c(%ebx),%edi
  asm volatile("cld; rep insl" :
80102102:	b9 80 00 00 00       	mov    $0x80,%ecx
80102107:	ba f0 01 00 00       	mov    $0x1f0,%edx
8010210c:	fc                   	cld    
8010210d:	f3 6d                	rep insl (%dx),%es:(%edi)
8010210f:	8b 33                	mov    (%ebx),%esi
80102111:	eb 9e                	jmp    801020b1 <ideintr+0x31>
80102113:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80102119:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80102120 <iderw>:
// Sync buf with disk.
// If B_DIRTY is set, write buf to disk, clear B_DIRTY, set B_VALID.
// Else if B_VALID is not set, read buf from disk, set B_VALID.
void
iderw(struct buf *b)
{
80102120:	55                   	push   %ebp
80102121:	89 e5                	mov    %esp,%ebp
80102123:	53                   	push   %ebx
80102124:	83 ec 14             	sub    $0x14,%esp
80102127:	8b 5d 08             	mov    0x8(%ebp),%ebx
  struct buf **pp;

  if(!holdingsleep(&b->lock))
8010212a:	8d 43 0c             	lea    0xc(%ebx),%eax
8010212d:	89 04 24             	mov    %eax,(%esp)
80102130:	e8 eb 1e 00 00       	call   80104020 <holdingsleep>
80102135:	85 c0                	test   %eax,%eax
80102137:	0f 84 9e 00 00 00    	je     801021db <iderw+0xbb>
    panic("iderw: buf not locked");
  if((b->flags & (B_VALID|B_DIRTY)) == B_VALID)
8010213d:	8b 03                	mov    (%ebx),%eax
8010213f:	83 e0 06             	and    $0x6,%eax
80102142:	83 f8 02             	cmp    $0x2,%eax
80102145:	0f 84 a8 00 00 00    	je     801021f3 <iderw+0xd3>
    panic("iderw: nothing to do");
  if(b->dev != 0 && !havedisk1)
8010214b:	8b 53 04             	mov    0x4(%ebx),%edx
8010214e:	85 d2                	test   %edx,%edx
80102150:	74 0d                	je     8010215f <iderw+0x3f>
80102152:	a1 60 a5 10 80       	mov    0x8010a560,%eax
80102157:	85 c0                	test   %eax,%eax
80102159:	0f 84 88 00 00 00    	je     801021e7 <iderw+0xc7>
    panic("iderw: ide disk 1 not present");

  acquire(&idelock);  //DOC:acquire-lock
8010215f:	c7 04 24 80 a5 10 80 	movl   $0x8010a580,(%esp)
80102166:	e8 75 20 00 00       	call   801041e0 <acquire>

  // Append b to idequeue.
  b->qnext = 0;
  for(pp=&idequeue; *pp; pp=&(*pp)->qnext)  //DOC:insert-queue
8010216b:	a1 64 a5 10 80       	mov    0x8010a564,%eax
  b->qnext = 0;
80102170:	c7 43 58 00 00 00 00 	movl   $0x0,0x58(%ebx)
  for(pp=&idequeue; *pp; pp=&(*pp)->qnext)  //DOC:insert-queue
80102177:	85 c0                	test   %eax,%eax
80102179:	75 07                	jne    80102182 <iderw+0x62>
8010217b:	eb 4e                	jmp    801021cb <iderw+0xab>
8010217d:	8d 76 00             	lea    0x0(%esi),%esi
80102180:	89 d0                	mov    %edx,%eax
80102182:	8b 50 58             	mov    0x58(%eax),%edx
80102185:	85 d2                	test   %edx,%edx
80102187:	75 f7                	jne    80102180 <iderw+0x60>
80102189:	83 c0 58             	add    $0x58,%eax
    ;
  *pp = b;
8010218c:	89 18                	mov    %ebx,(%eax)

  // Start disk if necessary.
  if(idequeue == b)
8010218e:	39 1d 64 a5 10 80    	cmp    %ebx,0x8010a564
80102194:	74 3c                	je     801021d2 <iderw+0xb2>
    idestart(b);

  // Wait for request to finish.
  while((b->flags & (B_VALID|B_DIRTY)) != B_VALID){
80102196:	8b 03                	mov    (%ebx),%eax
80102198:	83 e0 06             	and    $0x6,%eax
8010219b:	83 f8 02             	cmp    $0x2,%eax
8010219e:	74 1a                	je     801021ba <iderw+0x9a>
    sleep(b, &idelock);
801021a0:	c7 44 24 04 80 a5 10 	movl   $0x8010a580,0x4(%esp)
801021a7:	80 
801021a8:	89 1c 24             	mov    %ebx,(%esp)
801021ab:	e8 50 1a 00 00       	call   80103c00 <sleep>
  while((b->flags & (B_VALID|B_DIRTY)) != B_VALID){
801021b0:	8b 13                	mov    (%ebx),%edx
801021b2:	83 e2 06             	and    $0x6,%edx
801021b5:	83 fa 02             	cmp    $0x2,%edx
801021b8:	75 e6                	jne    801021a0 <iderw+0x80>
  }


  release(&idelock);
801021ba:	c7 45 08 80 a5 10 80 	movl   $0x8010a580,0x8(%ebp)
}
801021c1:	83 c4 14             	add    $0x14,%esp
801021c4:	5b                   	pop    %ebx
801021c5:	5d                   	pop    %ebp
  release(&idelock);
801021c6:	e9 85 20 00 00       	jmp    80104250 <release>
  for(pp=&idequeue; *pp; pp=&(*pp)->qnext)  //DOC:insert-queue
801021cb:	b8 64 a5 10 80       	mov    $0x8010a564,%eax
801021d0:	eb ba                	jmp    8010218c <iderw+0x6c>
    idestart(b);
801021d2:	89 d8                	mov    %ebx,%eax
801021d4:	e8 67 fd ff ff       	call   80101f40 <idestart>
801021d9:	eb bb                	jmp    80102196 <iderw+0x76>
    panic("iderw: buf not locked");
801021db:	c7 04 24 2a 6e 10 80 	movl   $0x80106e2a,(%esp)
801021e2:	e8 79 e1 ff ff       	call   80100360 <panic>
    panic("iderw: ide disk 1 not present");
801021e7:	c7 04 24 55 6e 10 80 	movl   $0x80106e55,(%esp)
801021ee:	e8 6d e1 ff ff       	call   80100360 <panic>
    panic("iderw: nothing to do");
801021f3:	c7 04 24 40 6e 10 80 	movl   $0x80106e40,(%esp)
801021fa:	e8 61 e1 ff ff       	call   80100360 <panic>
801021ff:	90                   	nop

80102200 <ioapicinit>:
  ioapic->data = data;
}

void
ioapicinit(void)
{
80102200:	55                   	push   %ebp
80102201:	89 e5                	mov    %esp,%ebp
80102203:	56                   	push   %esi
80102204:	53                   	push   %ebx
80102205:	83 ec 10             	sub    $0x10,%esp
  int i, id, maxintr;

  ioapic = (volatile struct ioapic*)IOAPIC;
80102208:	c7 05 34 26 11 80 00 	movl   $0xfec00000,0x80112634
8010220f:	00 c0 fe 
  ioapic->reg = reg;
80102212:	c7 05 00 00 c0 fe 01 	movl   $0x1,0xfec00000
80102219:	00 00 00 
  return ioapic->data;
8010221c:	8b 15 34 26 11 80    	mov    0x80112634,%edx
80102222:	8b 42 10             	mov    0x10(%edx),%eax
  ioapic->reg = reg;
80102225:	c7 02 00 00 00 00    	movl   $0x0,(%edx)
  return ioapic->data;
8010222b:	8b 1d 34 26 11 80    	mov    0x80112634,%ebx
  maxintr = (ioapicread(REG_VER) >> 16) & 0xFF;
  id = ioapicread(REG_ID) >> 24;
  if(id != ioapicid)
80102231:	0f b6 15 60 27 11 80 	movzbl 0x80112760,%edx
  maxintr = (ioapicread(REG_VER) >> 16) & 0xFF;
80102238:	c1 e8 10             	shr    $0x10,%eax
8010223b:	0f b6 f0             	movzbl %al,%esi
  return ioapic->data;
8010223e:	8b 43 10             	mov    0x10(%ebx),%eax
  id = ioapicread(REG_ID) >> 24;
80102241:	c1 e8 18             	shr    $0x18,%eax
  if(id != ioapicid)
80102244:	39 c2                	cmp    %eax,%edx
80102246:	74 12                	je     8010225a <ioapicinit+0x5a>
    cprintf("ioapicinit: id isn't equal to ioapicid; not a MP\n");
80102248:	c7 04 24 74 6e 10 80 	movl   $0x80106e74,(%esp)
8010224f:	e8 fc e3 ff ff       	call   80100650 <cprintf>
80102254:	8b 1d 34 26 11 80    	mov    0x80112634,%ebx
8010225a:	ba 10 00 00 00       	mov    $0x10,%edx
8010225f:	31 c0                	xor    %eax,%eax
80102261:	eb 07                	jmp    8010226a <ioapicinit+0x6a>
80102263:	90                   	nop
80102264:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80102268:	89 cb                	mov    %ecx,%ebx
  ioapic->reg = reg;
8010226a:	89 13                	mov    %edx,(%ebx)
  ioapic->data = data;
8010226c:	8b 1d 34 26 11 80    	mov    0x80112634,%ebx
80102272:	8d 48 20             	lea    0x20(%eax),%ecx

  // Mark all interrupts edge-triggered, active high, disabled,
  // and not routed to any CPUs.
  for(i = 0; i <= maxintr; i++){
    ioapicwrite(REG_TABLE+2*i, INT_DISABLED | (T_IRQ0 + i));
80102275:	81 c9 00 00 01 00    	or     $0x10000,%ecx
  for(i = 0; i <= maxintr; i++){
8010227b:	83 c0 01             	add    $0x1,%eax
  ioapic->data = data;
8010227e:	89 4b 10             	mov    %ecx,0x10(%ebx)
80102281:	8d 4a 01             	lea    0x1(%edx),%ecx
80102284:	83 c2 02             	add    $0x2,%edx
  ioapic->reg = reg;
80102287:	89 0b                	mov    %ecx,(%ebx)
  ioapic->data = data;
80102289:	8b 0d 34 26 11 80    	mov    0x80112634,%ecx
  for(i = 0; i <= maxintr; i++){
8010228f:	39 c6                	cmp    %eax,%esi
  ioapic->data = data;
80102291:	c7 41 10 00 00 00 00 	movl   $0x0,0x10(%ecx)
  for(i = 0; i <= maxintr; i++){
80102298:	7d ce                	jge    80102268 <ioapicinit+0x68>
    ioapicwrite(REG_TABLE+2*i+1, 0);
  }
}
8010229a:	83 c4 10             	add    $0x10,%esp
8010229d:	5b                   	pop    %ebx
8010229e:	5e                   	pop    %esi
8010229f:	5d                   	pop    %ebp
801022a0:	c3                   	ret    
801022a1:	eb 0d                	jmp    801022b0 <ioapicenable>
801022a3:	90                   	nop
801022a4:	90                   	nop
801022a5:	90                   	nop
801022a6:	90                   	nop
801022a7:	90                   	nop
801022a8:	90                   	nop
801022a9:	90                   	nop
801022aa:	90                   	nop
801022ab:	90                   	nop
801022ac:	90                   	nop
801022ad:	90                   	nop
801022ae:	90                   	nop
801022af:	90                   	nop

801022b0 <ioapicenable>:

void
ioapicenable(int irq, int cpunum)
{
801022b0:	55                   	push   %ebp
801022b1:	89 e5                	mov    %esp,%ebp
801022b3:	8b 55 08             	mov    0x8(%ebp),%edx
801022b6:	53                   	push   %ebx
801022b7:	8b 45 0c             	mov    0xc(%ebp),%eax
  // Mark interrupt edge-triggered, active high,
  // enabled, and routed to the given cpunum,
  // which happens to be that cpu's APIC ID.
  ioapicwrite(REG_TABLE+2*irq, T_IRQ0 + irq);
801022ba:	8d 5a 20             	lea    0x20(%edx),%ebx
801022bd:	8d 4c 12 10          	lea    0x10(%edx,%edx,1),%ecx
  ioapic->reg = reg;
801022c1:	8b 15 34 26 11 80    	mov    0x80112634,%edx
  ioapicwrite(REG_TABLE+2*irq+1, cpunum << 24);
801022c7:	c1 e0 18             	shl    $0x18,%eax
  ioapic->reg = reg;
801022ca:	89 0a                	mov    %ecx,(%edx)
  ioapic->data = data;
801022cc:	8b 15 34 26 11 80    	mov    0x80112634,%edx
  ioapicwrite(REG_TABLE+2*irq+1, cpunum << 24);
801022d2:	83 c1 01             	add    $0x1,%ecx
  ioapic->data = data;
801022d5:	89 5a 10             	mov    %ebx,0x10(%edx)
  ioapic->reg = reg;
801022d8:	89 0a                	mov    %ecx,(%edx)
  ioapic->data = data;
801022da:	8b 15 34 26 11 80    	mov    0x80112634,%edx
801022e0:	89 42 10             	mov    %eax,0x10(%edx)
}
801022e3:	5b                   	pop    %ebx
801022e4:	5d                   	pop    %ebp
801022e5:	c3                   	ret    
801022e6:	66 90                	xchg   %ax,%ax
801022e8:	66 90                	xchg   %ax,%ax
801022ea:	66 90                	xchg   %ax,%ax
801022ec:	66 90                	xchg   %ax,%ax
801022ee:	66 90                	xchg   %ax,%ax

801022f0 <kfree>:
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
801022f0:	55                   	push   %ebp
801022f1:	89 e5                	mov    %esp,%ebp
801022f3:	53                   	push   %ebx
801022f4:	83 ec 14             	sub    $0x14,%esp
801022f7:	8b 5d 08             	mov    0x8(%ebp),%ebx
  struct run *r;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
801022fa:	f7 c3 ff 0f 00 00    	test   $0xfff,%ebx
80102300:	75 7c                	jne    8010237e <kfree+0x8e>
80102302:	81 fb a8 55 11 80    	cmp    $0x801155a8,%ebx
80102308:	72 74                	jb     8010237e <kfree+0x8e>
8010230a:	8d 83 00 00 00 80    	lea    -0x80000000(%ebx),%eax
80102310:	3d ff ff ff 0d       	cmp    $0xdffffff,%eax
80102315:	77 67                	ja     8010237e <kfree+0x8e>
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);
80102317:	c7 44 24 08 00 10 00 	movl   $0x1000,0x8(%esp)
8010231e:	00 
8010231f:	c7 44 24 04 01 00 00 	movl   $0x1,0x4(%esp)
80102326:	00 
80102327:	89 1c 24             	mov    %ebx,(%esp)
8010232a:	e8 71 1f 00 00       	call   801042a0 <memset>

  if(kmem.use_lock)
8010232f:	8b 15 74 26 11 80    	mov    0x80112674,%edx
80102335:	85 d2                	test   %edx,%edx
80102337:	75 37                	jne    80102370 <kfree+0x80>
    acquire(&kmem.lock);
  r = (struct run*)v;
  r->next = kmem.freelist;
80102339:	a1 78 26 11 80       	mov    0x80112678,%eax
8010233e:	89 03                	mov    %eax,(%ebx)
  kmem.freelist = r;
  if(kmem.use_lock)
80102340:	a1 74 26 11 80       	mov    0x80112674,%eax
  kmem.freelist = r;
80102345:	89 1d 78 26 11 80    	mov    %ebx,0x80112678
  if(kmem.use_lock)
8010234b:	85 c0                	test   %eax,%eax
8010234d:	75 09                	jne    80102358 <kfree+0x68>
    release(&kmem.lock);
}
8010234f:	83 c4 14             	add    $0x14,%esp
80102352:	5b                   	pop    %ebx
80102353:	5d                   	pop    %ebp
80102354:	c3                   	ret    
80102355:	8d 76 00             	lea    0x0(%esi),%esi
    release(&kmem.lock);
80102358:	c7 45 08 40 26 11 80 	movl   $0x80112640,0x8(%ebp)
}
8010235f:	83 c4 14             	add    $0x14,%esp
80102362:	5b                   	pop    %ebx
80102363:	5d                   	pop    %ebp
    release(&kmem.lock);
80102364:	e9 e7 1e 00 00       	jmp    80104250 <release>
80102369:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
    acquire(&kmem.lock);
80102370:	c7 04 24 40 26 11 80 	movl   $0x80112640,(%esp)
80102377:	e8 64 1e 00 00       	call   801041e0 <acquire>
8010237c:	eb bb                	jmp    80102339 <kfree+0x49>
    panic("kfree");
8010237e:	c7 04 24 a6 6e 10 80 	movl   $0x80106ea6,(%esp)
80102385:	e8 d6 df ff ff       	call   80100360 <panic>
8010238a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

80102390 <freerange>:
{
80102390:	55                   	push   %ebp
80102391:	89 e5                	mov    %esp,%ebp
80102393:	56                   	push   %esi
80102394:	53                   	push   %ebx
80102395:	83 ec 10             	sub    $0x10,%esp
  p = (char*)PGROUNDUP((uint)vstart);
80102398:	8b 45 08             	mov    0x8(%ebp),%eax
{
8010239b:	8b 75 0c             	mov    0xc(%ebp),%esi
  p = (char*)PGROUNDUP((uint)vstart);
8010239e:	8d 90 ff 0f 00 00    	lea    0xfff(%eax),%edx
801023a4:	81 e2 00 f0 ff ff    	and    $0xfffff000,%edx
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
801023aa:	8d 9a 00 10 00 00    	lea    0x1000(%edx),%ebx
801023b0:	39 de                	cmp    %ebx,%esi
801023b2:	73 08                	jae    801023bc <freerange+0x2c>
801023b4:	eb 18                	jmp    801023ce <freerange+0x3e>
801023b6:	66 90                	xchg   %ax,%ax
801023b8:	89 da                	mov    %ebx,%edx
801023ba:	89 c3                	mov    %eax,%ebx
    kfree(p);
801023bc:	89 14 24             	mov    %edx,(%esp)
801023bf:	e8 2c ff ff ff       	call   801022f0 <kfree>
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
801023c4:	8d 83 00 10 00 00    	lea    0x1000(%ebx),%eax
801023ca:	39 f0                	cmp    %esi,%eax
801023cc:	76 ea                	jbe    801023b8 <freerange+0x28>
}
801023ce:	83 c4 10             	add    $0x10,%esp
801023d1:	5b                   	pop    %ebx
801023d2:	5e                   	pop    %esi
801023d3:	5d                   	pop    %ebp
801023d4:	c3                   	ret    
801023d5:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
801023d9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801023e0 <kinit1>:
{
801023e0:	55                   	push   %ebp
801023e1:	89 e5                	mov    %esp,%ebp
801023e3:	56                   	push   %esi
801023e4:	53                   	push   %ebx
801023e5:	83 ec 10             	sub    $0x10,%esp
801023e8:	8b 75 0c             	mov    0xc(%ebp),%esi
  initlock(&kmem.lock, "kmem");
801023eb:	c7 44 24 04 ac 6e 10 	movl   $0x80106eac,0x4(%esp)
801023f2:	80 
801023f3:	c7 04 24 40 26 11 80 	movl   $0x80112640,(%esp)
801023fa:	e8 71 1c 00 00       	call   80104070 <initlock>
  p = (char*)PGROUNDUP((uint)vstart);
801023ff:	8b 45 08             	mov    0x8(%ebp),%eax
  kmem.use_lock = 0;
80102402:	c7 05 74 26 11 80 00 	movl   $0x0,0x80112674
80102409:	00 00 00 
  p = (char*)PGROUNDUP((uint)vstart);
8010240c:	8d 90 ff 0f 00 00    	lea    0xfff(%eax),%edx
80102412:	81 e2 00 f0 ff ff    	and    $0xfffff000,%edx
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
80102418:	8d 9a 00 10 00 00    	lea    0x1000(%edx),%ebx
8010241e:	39 de                	cmp    %ebx,%esi
80102420:	73 0a                	jae    8010242c <kinit1+0x4c>
80102422:	eb 1a                	jmp    8010243e <kinit1+0x5e>
80102424:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80102428:	89 da                	mov    %ebx,%edx
8010242a:	89 c3                	mov    %eax,%ebx
    kfree(p);
8010242c:	89 14 24             	mov    %edx,(%esp)
8010242f:	e8 bc fe ff ff       	call   801022f0 <kfree>
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
80102434:	8d 83 00 10 00 00    	lea    0x1000(%ebx),%eax
8010243a:	39 c6                	cmp    %eax,%esi
8010243c:	73 ea                	jae    80102428 <kinit1+0x48>
}
8010243e:	83 c4 10             	add    $0x10,%esp
80102441:	5b                   	pop    %ebx
80102442:	5e                   	pop    %esi
80102443:	5d                   	pop    %ebp
80102444:	c3                   	ret    
80102445:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80102449:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80102450 <kinit2>:
{
80102450:	55                   	push   %ebp
80102451:	89 e5                	mov    %esp,%ebp
80102453:	56                   	push   %esi
80102454:	53                   	push   %ebx
80102455:	83 ec 10             	sub    $0x10,%esp
  p = (char*)PGROUNDUP((uint)vstart);
80102458:	8b 45 08             	mov    0x8(%ebp),%eax
{
8010245b:	8b 75 0c             	mov    0xc(%ebp),%esi
  p = (char*)PGROUNDUP((uint)vstart);
8010245e:	8d 90 ff 0f 00 00    	lea    0xfff(%eax),%edx
80102464:	81 e2 00 f0 ff ff    	and    $0xfffff000,%edx
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
8010246a:	8d 9a 00 10 00 00    	lea    0x1000(%edx),%ebx
80102470:	39 de                	cmp    %ebx,%esi
80102472:	73 08                	jae    8010247c <kinit2+0x2c>
80102474:	eb 18                	jmp    8010248e <kinit2+0x3e>
80102476:	66 90                	xchg   %ax,%ax
80102478:	89 da                	mov    %ebx,%edx
8010247a:	89 c3                	mov    %eax,%ebx
    kfree(p);
8010247c:	89 14 24             	mov    %edx,(%esp)
8010247f:	e8 6c fe ff ff       	call   801022f0 <kfree>
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
80102484:	8d 83 00 10 00 00    	lea    0x1000(%ebx),%eax
8010248a:	39 c6                	cmp    %eax,%esi
8010248c:	73 ea                	jae    80102478 <kinit2+0x28>
  kmem.use_lock = 1;
8010248e:	c7 05 74 26 11 80 01 	movl   $0x1,0x80112674
80102495:	00 00 00 
}
80102498:	83 c4 10             	add    $0x10,%esp
8010249b:	5b                   	pop    %ebx
8010249c:	5e                   	pop    %esi
8010249d:	5d                   	pop    %ebp
8010249e:	c3                   	ret    
8010249f:	90                   	nop

801024a0 <kalloc>:
// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
801024a0:	55                   	push   %ebp
801024a1:	89 e5                	mov    %esp,%ebp
801024a3:	53                   	push   %ebx
801024a4:	83 ec 14             	sub    $0x14,%esp
  struct run *r;

  if(kmem.use_lock)
801024a7:	a1 74 26 11 80       	mov    0x80112674,%eax
801024ac:	85 c0                	test   %eax,%eax
801024ae:	75 30                	jne    801024e0 <kalloc+0x40>
    acquire(&kmem.lock);
  r = kmem.freelist;
801024b0:	8b 1d 78 26 11 80    	mov    0x80112678,%ebx
  if(r)
801024b6:	85 db                	test   %ebx,%ebx
801024b8:	74 08                	je     801024c2 <kalloc+0x22>
    kmem.freelist = r->next;
801024ba:	8b 13                	mov    (%ebx),%edx
801024bc:	89 15 78 26 11 80    	mov    %edx,0x80112678
  if(kmem.use_lock)
801024c2:	85 c0                	test   %eax,%eax
801024c4:	74 0c                	je     801024d2 <kalloc+0x32>
    release(&kmem.lock);
801024c6:	c7 04 24 40 26 11 80 	movl   $0x80112640,(%esp)
801024cd:	e8 7e 1d 00 00       	call   80104250 <release>
  return (char*)r;
}
801024d2:	83 c4 14             	add    $0x14,%esp
801024d5:	89 d8                	mov    %ebx,%eax
801024d7:	5b                   	pop    %ebx
801024d8:	5d                   	pop    %ebp
801024d9:	c3                   	ret    
801024da:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    acquire(&kmem.lock);
801024e0:	c7 04 24 40 26 11 80 	movl   $0x80112640,(%esp)
801024e7:	e8 f4 1c 00 00       	call   801041e0 <acquire>
801024ec:	a1 74 26 11 80       	mov    0x80112674,%eax
801024f1:	eb bd                	jmp    801024b0 <kalloc+0x10>
801024f3:	66 90                	xchg   %ax,%ax
801024f5:	66 90                	xchg   %ax,%ax
801024f7:	66 90                	xchg   %ax,%ax
801024f9:	66 90                	xchg   %ax,%ax
801024fb:	66 90                	xchg   %ax,%ax
801024fd:	66 90                	xchg   %ax,%ax
801024ff:	90                   	nop

80102500 <kbdgetc>:
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
80102500:	ba 64 00 00 00       	mov    $0x64,%edx
80102505:	ec                   	in     (%dx),%al
    normalmap, shiftmap, ctlmap, ctlmap
  };
  uint st, data, c;

  st = inb(KBSTATP);
  if((st & KBS_DIB) == 0)
80102506:	a8 01                	test   $0x1,%al
80102508:	0f 84 ba 00 00 00    	je     801025c8 <kbdgetc+0xc8>
8010250e:	b2 60                	mov    $0x60,%dl
80102510:	ec                   	in     (%dx),%al
    return -1;
  data = inb(KBDATAP);
80102511:	0f b6 c8             	movzbl %al,%ecx

  if(data == 0xE0){
80102514:	81 f9 e0 00 00 00    	cmp    $0xe0,%ecx
8010251a:	0f 84 88 00 00 00    	je     801025a8 <kbdgetc+0xa8>
    shift |= E0ESC;
    return 0;
  } else if(data & 0x80){
80102520:	84 c0                	test   %al,%al
80102522:	79 2c                	jns    80102550 <kbdgetc+0x50>
    // Key released
    data = (shift & E0ESC ? data : data & 0x7F);
80102524:	8b 15 b4 a5 10 80    	mov    0x8010a5b4,%edx
8010252a:	f6 c2 40             	test   $0x40,%dl
8010252d:	75 05                	jne    80102534 <kbdgetc+0x34>
8010252f:	89 c1                	mov    %eax,%ecx
80102531:	83 e1 7f             	and    $0x7f,%ecx
    shift &= ~(shiftcode[data] | E0ESC);
80102534:	0f b6 81 e0 6f 10 80 	movzbl -0x7fef9020(%ecx),%eax
8010253b:	83 c8 40             	or     $0x40,%eax
8010253e:	0f b6 c0             	movzbl %al,%eax
80102541:	f7 d0                	not    %eax
80102543:	21 d0                	and    %edx,%eax
80102545:	a3 b4 a5 10 80       	mov    %eax,0x8010a5b4
    return 0;
8010254a:	31 c0                	xor    %eax,%eax
8010254c:	c3                   	ret    
8010254d:	8d 76 00             	lea    0x0(%esi),%esi
{
80102550:	55                   	push   %ebp
80102551:	89 e5                	mov    %esp,%ebp
80102553:	53                   	push   %ebx
80102554:	8b 1d b4 a5 10 80    	mov    0x8010a5b4,%ebx
  } else if(shift & E0ESC){
8010255a:	f6 c3 40             	test   $0x40,%bl
8010255d:	74 09                	je     80102568 <kbdgetc+0x68>
    // Last character was an E0 escape; or with 0x80
    data |= 0x80;
8010255f:	83 c8 80             	or     $0xffffff80,%eax
    shift &= ~E0ESC;
80102562:	83 e3 bf             	and    $0xffffffbf,%ebx
    data |= 0x80;
80102565:	0f b6 c8             	movzbl %al,%ecx
  }

  shift |= shiftcode[data];
80102568:	0f b6 91 e0 6f 10 80 	movzbl -0x7fef9020(%ecx),%edx
  shift ^= togglecode[data];
8010256f:	0f b6 81 e0 6e 10 80 	movzbl -0x7fef9120(%ecx),%eax
  shift |= shiftcode[data];
80102576:	09 da                	or     %ebx,%edx
  shift ^= togglecode[data];
80102578:	31 c2                	xor    %eax,%edx
  c = charcode[shift & (CTL | SHIFT)][data];
8010257a:	89 d0                	mov    %edx,%eax
8010257c:	83 e0 03             	and    $0x3,%eax
8010257f:	8b 04 85 c0 6e 10 80 	mov    -0x7fef9140(,%eax,4),%eax
  shift ^= togglecode[data];
80102586:	89 15 b4 a5 10 80    	mov    %edx,0x8010a5b4
  if(shift & CAPSLOCK){
8010258c:	83 e2 08             	and    $0x8,%edx
  c = charcode[shift & (CTL | SHIFT)][data];
8010258f:	0f b6 04 08          	movzbl (%eax,%ecx,1),%eax
  if(shift & CAPSLOCK){
80102593:	74 0b                	je     801025a0 <kbdgetc+0xa0>
    if('a' <= c && c <= 'z')
80102595:	8d 50 9f             	lea    -0x61(%eax),%edx
80102598:	83 fa 19             	cmp    $0x19,%edx
8010259b:	77 1b                	ja     801025b8 <kbdgetc+0xb8>
      c += 'A' - 'a';
8010259d:	83 e8 20             	sub    $0x20,%eax
    else if('A' <= c && c <= 'Z')
      c += 'a' - 'A';
  }
  return c;
}
801025a0:	5b                   	pop    %ebx
801025a1:	5d                   	pop    %ebp
801025a2:	c3                   	ret    
801025a3:	90                   	nop
801025a4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    shift |= E0ESC;
801025a8:	83 0d b4 a5 10 80 40 	orl    $0x40,0x8010a5b4
    return 0;
801025af:	31 c0                	xor    %eax,%eax
801025b1:	c3                   	ret    
801025b2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    else if('A' <= c && c <= 'Z')
801025b8:	8d 48 bf             	lea    -0x41(%eax),%ecx
      c += 'a' - 'A';
801025bb:	8d 50 20             	lea    0x20(%eax),%edx
801025be:	83 f9 19             	cmp    $0x19,%ecx
801025c1:	0f 46 c2             	cmovbe %edx,%eax
  return c;
801025c4:	eb da                	jmp    801025a0 <kbdgetc+0xa0>
801025c6:	66 90                	xchg   %ax,%ax
    return -1;
801025c8:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
801025cd:	c3                   	ret    
801025ce:	66 90                	xchg   %ax,%ax

801025d0 <kbdintr>:

void
kbdintr(void)
{
801025d0:	55                   	push   %ebp
801025d1:	89 e5                	mov    %esp,%ebp
801025d3:	83 ec 18             	sub    $0x18,%esp
  consoleintr(kbdgetc);
801025d6:	c7 04 24 00 25 10 80 	movl   $0x80102500,(%esp)
801025dd:	e8 ce e1 ff ff       	call   801007b0 <consoleintr>
}
801025e2:	c9                   	leave  
801025e3:	c3                   	ret    
801025e4:	66 90                	xchg   %ax,%ax
801025e6:	66 90                	xchg   %ax,%ax
801025e8:	66 90                	xchg   %ax,%ax
801025ea:	66 90                	xchg   %ax,%ax
801025ec:	66 90                	xchg   %ax,%ax
801025ee:	66 90                	xchg   %ax,%ax

801025f0 <fill_rtcdate>:
  return inb(CMOS_RETURN);
}

static void
fill_rtcdate(struct rtcdate *r)
{
801025f0:	55                   	push   %ebp
801025f1:	89 c1                	mov    %eax,%ecx
801025f3:	89 e5                	mov    %esp,%ebp
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
801025f5:	ba 70 00 00 00       	mov    $0x70,%edx
801025fa:	53                   	push   %ebx
801025fb:	31 c0                	xor    %eax,%eax
801025fd:	ee                   	out    %al,(%dx)
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
801025fe:	bb 71 00 00 00       	mov    $0x71,%ebx
80102603:	89 da                	mov    %ebx,%edx
80102605:	ec                   	in     (%dx),%al
  return inb(CMOS_RETURN);
80102606:	0f b6 c0             	movzbl %al,%eax
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
80102609:	b2 70                	mov    $0x70,%dl
8010260b:	89 01                	mov    %eax,(%ecx)
8010260d:	b8 02 00 00 00       	mov    $0x2,%eax
80102612:	ee                   	out    %al,(%dx)
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
80102613:	89 da                	mov    %ebx,%edx
80102615:	ec                   	in     (%dx),%al
80102616:	0f b6 c0             	movzbl %al,%eax
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
80102619:	b2 70                	mov    $0x70,%dl
8010261b:	89 41 04             	mov    %eax,0x4(%ecx)
8010261e:	b8 04 00 00 00       	mov    $0x4,%eax
80102623:	ee                   	out    %al,(%dx)
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
80102624:	89 da                	mov    %ebx,%edx
80102626:	ec                   	in     (%dx),%al
80102627:	0f b6 c0             	movzbl %al,%eax
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
8010262a:	b2 70                	mov    $0x70,%dl
8010262c:	89 41 08             	mov    %eax,0x8(%ecx)
8010262f:	b8 07 00 00 00       	mov    $0x7,%eax
80102634:	ee                   	out    %al,(%dx)
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
80102635:	89 da                	mov    %ebx,%edx
80102637:	ec                   	in     (%dx),%al
80102638:	0f b6 c0             	movzbl %al,%eax
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
8010263b:	b2 70                	mov    $0x70,%dl
8010263d:	89 41 0c             	mov    %eax,0xc(%ecx)
80102640:	b8 08 00 00 00       	mov    $0x8,%eax
80102645:	ee                   	out    %al,(%dx)
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
80102646:	89 da                	mov    %ebx,%edx
80102648:	ec                   	in     (%dx),%al
80102649:	0f b6 c0             	movzbl %al,%eax
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
8010264c:	b2 70                	mov    $0x70,%dl
8010264e:	89 41 10             	mov    %eax,0x10(%ecx)
80102651:	b8 09 00 00 00       	mov    $0x9,%eax
80102656:	ee                   	out    %al,(%dx)
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
80102657:	89 da                	mov    %ebx,%edx
80102659:	ec                   	in     (%dx),%al
8010265a:	0f b6 d8             	movzbl %al,%ebx
8010265d:	89 59 14             	mov    %ebx,0x14(%ecx)
  r->minute = cmos_read(MINS);
  r->hour   = cmos_read(HOURS);
  r->day    = cmos_read(DAY);
  r->month  = cmos_read(MONTH);
  r->year   = cmos_read(YEAR);
}
80102660:	5b                   	pop    %ebx
80102661:	5d                   	pop    %ebp
80102662:	c3                   	ret    
80102663:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80102669:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80102670 <lapicinit>:
  if(!lapic)
80102670:	a1 7c 26 11 80       	mov    0x8011267c,%eax
{
80102675:	55                   	push   %ebp
80102676:	89 e5                	mov    %esp,%ebp
  if(!lapic)
80102678:	85 c0                	test   %eax,%eax
8010267a:	0f 84 c0 00 00 00    	je     80102740 <lapicinit+0xd0>
  lapic[index] = value;
80102680:	c7 80 f0 00 00 00 3f 	movl   $0x13f,0xf0(%eax)
80102687:	01 00 00 
  lapic[ID];  // wait for write to finish, by reading
8010268a:	8b 50 20             	mov    0x20(%eax),%edx
  lapic[index] = value;
8010268d:	c7 80 e0 03 00 00 0b 	movl   $0xb,0x3e0(%eax)
80102694:	00 00 00 
  lapic[ID];  // wait for write to finish, by reading
80102697:	8b 50 20             	mov    0x20(%eax),%edx
  lapic[index] = value;
8010269a:	c7 80 20 03 00 00 20 	movl   $0x20020,0x320(%eax)
801026a1:	00 02 00 
  lapic[ID];  // wait for write to finish, by reading
801026a4:	8b 50 20             	mov    0x20(%eax),%edx
  lapic[index] = value;
801026a7:	c7 80 80 03 00 00 80 	movl   $0x989680,0x380(%eax)
801026ae:	96 98 00 
  lapic[ID];  // wait for write to finish, by reading
801026b1:	8b 50 20             	mov    0x20(%eax),%edx
  lapic[index] = value;
801026b4:	c7 80 50 03 00 00 00 	movl   $0x10000,0x350(%eax)
801026bb:	00 01 00 
  lapic[ID];  // wait for write to finish, by reading
801026be:	8b 50 20             	mov    0x20(%eax),%edx
  lapic[index] = value;
801026c1:	c7 80 60 03 00 00 00 	movl   $0x10000,0x360(%eax)
801026c8:	00 01 00 
  lapic[ID];  // wait for write to finish, by reading
801026cb:	8b 50 20             	mov    0x20(%eax),%edx
  if(((lapic[VER]>>16) & 0xFF) >= 4)
801026ce:	8b 50 30             	mov    0x30(%eax),%edx
801026d1:	c1 ea 10             	shr    $0x10,%edx
801026d4:	80 fa 03             	cmp    $0x3,%dl
801026d7:	77 6f                	ja     80102748 <lapicinit+0xd8>
  lapic[index] = value;
801026d9:	c7 80 70 03 00 00 33 	movl   $0x33,0x370(%eax)
801026e0:	00 00 00 
  lapic[ID];  // wait for write to finish, by reading
801026e3:	8b 50 20             	mov    0x20(%eax),%edx
  lapic[index] = value;
801026e6:	c7 80 80 02 00 00 00 	movl   $0x0,0x280(%eax)
801026ed:	00 00 00 
  lapic[ID];  // wait for write to finish, by reading
801026f0:	8b 50 20             	mov    0x20(%eax),%edx
  lapic[index] = value;
801026f3:	c7 80 80 02 00 00 00 	movl   $0x0,0x280(%eax)
801026fa:	00 00 00 
  lapic[ID];  // wait for write to finish, by reading
801026fd:	8b 50 20             	mov    0x20(%eax),%edx
  lapic[index] = value;
80102700:	c7 80 b0 00 00 00 00 	movl   $0x0,0xb0(%eax)
80102707:	00 00 00 
  lapic[ID];  // wait for write to finish, by reading
8010270a:	8b 50 20             	mov    0x20(%eax),%edx
  lapic[index] = value;
8010270d:	c7 80 10 03 00 00 00 	movl   $0x0,0x310(%eax)
80102714:	00 00 00 
  lapic[ID];  // wait for write to finish, by reading
80102717:	8b 50 20             	mov    0x20(%eax),%edx
  lapic[index] = value;
8010271a:	c7 80 00 03 00 00 00 	movl   $0x88500,0x300(%eax)
80102721:	85 08 00 
  lapic[ID];  // wait for write to finish, by reading
80102724:	8b 50 20             	mov    0x20(%eax),%edx
80102727:	90                   	nop
  while(lapic[ICRLO] & DELIVS)
80102728:	8b 90 00 03 00 00    	mov    0x300(%eax),%edx
8010272e:	80 e6 10             	and    $0x10,%dh
80102731:	75 f5                	jne    80102728 <lapicinit+0xb8>
  lapic[index] = value;
80102733:	c7 80 80 00 00 00 00 	movl   $0x0,0x80(%eax)
8010273a:	00 00 00 
  lapic[ID];  // wait for write to finish, by reading
8010273d:	8b 40 20             	mov    0x20(%eax),%eax
}
80102740:	5d                   	pop    %ebp
80102741:	c3                   	ret    
80102742:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  lapic[index] = value;
80102748:	c7 80 40 03 00 00 00 	movl   $0x10000,0x340(%eax)
8010274f:	00 01 00 
  lapic[ID];  // wait for write to finish, by reading
80102752:	8b 50 20             	mov    0x20(%eax),%edx
80102755:	eb 82                	jmp    801026d9 <lapicinit+0x69>
80102757:	89 f6                	mov    %esi,%esi
80102759:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80102760 <lapicid>:
  if (!lapic)
80102760:	a1 7c 26 11 80       	mov    0x8011267c,%eax
{
80102765:	55                   	push   %ebp
80102766:	89 e5                	mov    %esp,%ebp
  if (!lapic)
80102768:	85 c0                	test   %eax,%eax
8010276a:	74 0c                	je     80102778 <lapicid+0x18>
  return lapic[ID] >> 24;
8010276c:	8b 40 20             	mov    0x20(%eax),%eax
}
8010276f:	5d                   	pop    %ebp
  return lapic[ID] >> 24;
80102770:	c1 e8 18             	shr    $0x18,%eax
}
80102773:	c3                   	ret    
80102774:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    return 0;
80102778:	31 c0                	xor    %eax,%eax
}
8010277a:	5d                   	pop    %ebp
8010277b:	c3                   	ret    
8010277c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80102780 <lapiceoi>:
  if(lapic)
80102780:	a1 7c 26 11 80       	mov    0x8011267c,%eax
{
80102785:	55                   	push   %ebp
80102786:	89 e5                	mov    %esp,%ebp
  if(lapic)
80102788:	85 c0                	test   %eax,%eax
8010278a:	74 0d                	je     80102799 <lapiceoi+0x19>
  lapic[index] = value;
8010278c:	c7 80 b0 00 00 00 00 	movl   $0x0,0xb0(%eax)
80102793:	00 00 00 
  lapic[ID];  // wait for write to finish, by reading
80102796:	8b 40 20             	mov    0x20(%eax),%eax
}
80102799:	5d                   	pop    %ebp
8010279a:	c3                   	ret    
8010279b:	90                   	nop
8010279c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

801027a0 <microdelay>:
{
801027a0:	55                   	push   %ebp
801027a1:	89 e5                	mov    %esp,%ebp
}
801027a3:	5d                   	pop    %ebp
801027a4:	c3                   	ret    
801027a5:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
801027a9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801027b0 <lapicstartap>:
{
801027b0:	55                   	push   %ebp
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
801027b1:	ba 70 00 00 00       	mov    $0x70,%edx
801027b6:	89 e5                	mov    %esp,%ebp
801027b8:	b8 0f 00 00 00       	mov    $0xf,%eax
801027bd:	53                   	push   %ebx
801027be:	8b 4d 08             	mov    0x8(%ebp),%ecx
801027c1:	8b 5d 0c             	mov    0xc(%ebp),%ebx
801027c4:	ee                   	out    %al,(%dx)
801027c5:	b8 0a 00 00 00       	mov    $0xa,%eax
801027ca:	b2 71                	mov    $0x71,%dl
801027cc:	ee                   	out    %al,(%dx)
  wrv[0] = 0;
801027cd:	31 c0                	xor    %eax,%eax
801027cf:	66 a3 67 04 00 80    	mov    %ax,0x80000467
  wrv[1] = addr >> 4;
801027d5:	89 d8                	mov    %ebx,%eax
801027d7:	c1 e8 04             	shr    $0x4,%eax
801027da:	66 a3 69 04 00 80    	mov    %ax,0x80000469
  lapic[index] = value;
801027e0:	a1 7c 26 11 80       	mov    0x8011267c,%eax
  lapicw(ICRHI, apicid<<24);
801027e5:	c1 e1 18             	shl    $0x18,%ecx
    lapicw(ICRLO, STARTUP | (addr>>12));
801027e8:	c1 eb 0c             	shr    $0xc,%ebx
  lapic[index] = value;
801027eb:	89 88 10 03 00 00    	mov    %ecx,0x310(%eax)
  lapic[ID];  // wait for write to finish, by reading
801027f1:	8b 50 20             	mov    0x20(%eax),%edx
  lapic[index] = value;
801027f4:	c7 80 00 03 00 00 00 	movl   $0xc500,0x300(%eax)
801027fb:	c5 00 00 
  lapic[ID];  // wait for write to finish, by reading
801027fe:	8b 50 20             	mov    0x20(%eax),%edx
  lapic[index] = value;
80102801:	c7 80 00 03 00 00 00 	movl   $0x8500,0x300(%eax)
80102808:	85 00 00 
  lapic[ID];  // wait for write to finish, by reading
8010280b:	8b 50 20             	mov    0x20(%eax),%edx
  lapic[index] = value;
8010280e:	89 88 10 03 00 00    	mov    %ecx,0x310(%eax)
  lapic[ID];  // wait for write to finish, by reading
80102814:	8b 50 20             	mov    0x20(%eax),%edx
    lapicw(ICRLO, STARTUP | (addr>>12));
80102817:	89 da                	mov    %ebx,%edx
80102819:	80 ce 06             	or     $0x6,%dh
  lapic[index] = value;
8010281c:	89 90 00 03 00 00    	mov    %edx,0x300(%eax)
  lapic[ID];  // wait for write to finish, by reading
80102822:	8b 58 20             	mov    0x20(%eax),%ebx
  lapic[index] = value;
80102825:	89 88 10 03 00 00    	mov    %ecx,0x310(%eax)
  lapic[ID];  // wait for write to finish, by reading
8010282b:	8b 48 20             	mov    0x20(%eax),%ecx
  lapic[index] = value;
8010282e:	89 90 00 03 00 00    	mov    %edx,0x300(%eax)
  lapic[ID];  // wait for write to finish, by reading
80102834:	8b 40 20             	mov    0x20(%eax),%eax
}
80102837:	5b                   	pop    %ebx
80102838:	5d                   	pop    %ebp
80102839:	c3                   	ret    
8010283a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

80102840 <cmostime>:

// qemu seems to use 24-hour GWT and the values are BCD encoded
void
cmostime(struct rtcdate *r)
{
80102840:	55                   	push   %ebp
80102841:	ba 70 00 00 00       	mov    $0x70,%edx
80102846:	89 e5                	mov    %esp,%ebp
80102848:	b8 0b 00 00 00       	mov    $0xb,%eax
8010284d:	57                   	push   %edi
8010284e:	56                   	push   %esi
8010284f:	53                   	push   %ebx
80102850:	83 ec 4c             	sub    $0x4c,%esp
80102853:	ee                   	out    %al,(%dx)
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
80102854:	b2 71                	mov    $0x71,%dl
80102856:	ec                   	in     (%dx),%al
80102857:	88 45 b7             	mov    %al,-0x49(%ebp)
8010285a:	8d 5d b8             	lea    -0x48(%ebp),%ebx
  struct rtcdate t1, t2;
  int sb, bcd;

  sb = cmos_read(CMOS_STATB);

  bcd = (sb & (1 << 2)) == 0;
8010285d:	80 65 b7 04          	andb   $0x4,-0x49(%ebp)
80102861:	8d 7d d0             	lea    -0x30(%ebp),%edi
80102864:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
80102868:	be 70 00 00 00       	mov    $0x70,%esi

  // make sure CMOS doesn't modify time while we read it
  for(;;) {
    fill_rtcdate(&t1);
8010286d:	89 d8                	mov    %ebx,%eax
8010286f:	e8 7c fd ff ff       	call   801025f0 <fill_rtcdate>
80102874:	b8 0a 00 00 00       	mov    $0xa,%eax
80102879:	89 f2                	mov    %esi,%edx
8010287b:	ee                   	out    %al,(%dx)
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
8010287c:	ba 71 00 00 00       	mov    $0x71,%edx
80102881:	ec                   	in     (%dx),%al
    if(cmos_read(CMOS_STATA) & CMOS_UIP)
80102882:	84 c0                	test   %al,%al
80102884:	78 e7                	js     8010286d <cmostime+0x2d>
        continue;
    fill_rtcdate(&t2);
80102886:	89 f8                	mov    %edi,%eax
80102888:	e8 63 fd ff ff       	call   801025f0 <fill_rtcdate>
    if(memcmp(&t1, &t2, sizeof(t1)) == 0)
8010288d:	c7 44 24 08 18 00 00 	movl   $0x18,0x8(%esp)
80102894:	00 
80102895:	89 7c 24 04          	mov    %edi,0x4(%esp)
80102899:	89 1c 24             	mov    %ebx,(%esp)
8010289c:	e8 4f 1a 00 00       	call   801042f0 <memcmp>
801028a1:	85 c0                	test   %eax,%eax
801028a3:	75 c3                	jne    80102868 <cmostime+0x28>
      break;
  }

  // convert
  if(bcd) {
801028a5:	80 7d b7 00          	cmpb   $0x0,-0x49(%ebp)
801028a9:	75 78                	jne    80102923 <cmostime+0xe3>
#define    CONV(x)     (t1.x = ((t1.x >> 4) * 10) + (t1.x & 0xf))
    CONV(second);
801028ab:	8b 45 b8             	mov    -0x48(%ebp),%eax
801028ae:	89 c2                	mov    %eax,%edx
801028b0:	83 e0 0f             	and    $0xf,%eax
801028b3:	c1 ea 04             	shr    $0x4,%edx
801028b6:	8d 14 92             	lea    (%edx,%edx,4),%edx
801028b9:	8d 04 50             	lea    (%eax,%edx,2),%eax
801028bc:	89 45 b8             	mov    %eax,-0x48(%ebp)
    CONV(minute);
801028bf:	8b 45 bc             	mov    -0x44(%ebp),%eax
801028c2:	89 c2                	mov    %eax,%edx
801028c4:	83 e0 0f             	and    $0xf,%eax
801028c7:	c1 ea 04             	shr    $0x4,%edx
801028ca:	8d 14 92             	lea    (%edx,%edx,4),%edx
801028cd:	8d 04 50             	lea    (%eax,%edx,2),%eax
801028d0:	89 45 bc             	mov    %eax,-0x44(%ebp)
    CONV(hour  );
801028d3:	8b 45 c0             	mov    -0x40(%ebp),%eax
801028d6:	89 c2                	mov    %eax,%edx
801028d8:	83 e0 0f             	and    $0xf,%eax
801028db:	c1 ea 04             	shr    $0x4,%edx
801028de:	8d 14 92             	lea    (%edx,%edx,4),%edx
801028e1:	8d 04 50             	lea    (%eax,%edx,2),%eax
801028e4:	89 45 c0             	mov    %eax,-0x40(%ebp)
    CONV(day   );
801028e7:	8b 45 c4             	mov    -0x3c(%ebp),%eax
801028ea:	89 c2                	mov    %eax,%edx
801028ec:	83 e0 0f             	and    $0xf,%eax
801028ef:	c1 ea 04             	shr    $0x4,%edx
801028f2:	8d 14 92             	lea    (%edx,%edx,4),%edx
801028f5:	8d 04 50             	lea    (%eax,%edx,2),%eax
801028f8:	89 45 c4             	mov    %eax,-0x3c(%ebp)
    CONV(month );
801028fb:	8b 45 c8             	mov    -0x38(%ebp),%eax
801028fe:	89 c2                	mov    %eax,%edx
80102900:	83 e0 0f             	and    $0xf,%eax
80102903:	c1 ea 04             	shr    $0x4,%edx
80102906:	8d 14 92             	lea    (%edx,%edx,4),%edx
80102909:	8d 04 50             	lea    (%eax,%edx,2),%eax
8010290c:	89 45 c8             	mov    %eax,-0x38(%ebp)
    CONV(year  );
8010290f:	8b 45 cc             	mov    -0x34(%ebp),%eax
80102912:	89 c2                	mov    %eax,%edx
80102914:	83 e0 0f             	and    $0xf,%eax
80102917:	c1 ea 04             	shr    $0x4,%edx
8010291a:	8d 14 92             	lea    (%edx,%edx,4),%edx
8010291d:	8d 04 50             	lea    (%eax,%edx,2),%eax
80102920:	89 45 cc             	mov    %eax,-0x34(%ebp)
#undef     CONV
  }

  *r = t1;
80102923:	8b 4d 08             	mov    0x8(%ebp),%ecx
80102926:	8b 45 b8             	mov    -0x48(%ebp),%eax
80102929:	89 01                	mov    %eax,(%ecx)
8010292b:	8b 45 bc             	mov    -0x44(%ebp),%eax
8010292e:	89 41 04             	mov    %eax,0x4(%ecx)
80102931:	8b 45 c0             	mov    -0x40(%ebp),%eax
80102934:	89 41 08             	mov    %eax,0x8(%ecx)
80102937:	8b 45 c4             	mov    -0x3c(%ebp),%eax
8010293a:	89 41 0c             	mov    %eax,0xc(%ecx)
8010293d:	8b 45 c8             	mov    -0x38(%ebp),%eax
80102940:	89 41 10             	mov    %eax,0x10(%ecx)
80102943:	8b 45 cc             	mov    -0x34(%ebp),%eax
80102946:	89 41 14             	mov    %eax,0x14(%ecx)
  r->year += 2000;
80102949:	81 41 14 d0 07 00 00 	addl   $0x7d0,0x14(%ecx)
}
80102950:	83 c4 4c             	add    $0x4c,%esp
80102953:	5b                   	pop    %ebx
80102954:	5e                   	pop    %esi
80102955:	5f                   	pop    %edi
80102956:	5d                   	pop    %ebp
80102957:	c3                   	ret    
80102958:	66 90                	xchg   %ax,%ax
8010295a:	66 90                	xchg   %ax,%ax
8010295c:	66 90                	xchg   %ax,%ax
8010295e:	66 90                	xchg   %ax,%ax

80102960 <install_trans>:
}

// Copy committed blocks from log to their home location
static void
install_trans(void)
{
80102960:	55                   	push   %ebp
80102961:	89 e5                	mov    %esp,%ebp
80102963:	57                   	push   %edi
80102964:	56                   	push   %esi
80102965:	53                   	push   %ebx
  int tail;

  for (tail = 0; tail < log.lh.n; tail++) {
80102966:	31 db                	xor    %ebx,%ebx
{
80102968:	83 ec 1c             	sub    $0x1c,%esp
  for (tail = 0; tail < log.lh.n; tail++) {
8010296b:	a1 c8 26 11 80       	mov    0x801126c8,%eax
80102970:	85 c0                	test   %eax,%eax
80102972:	7e 78                	jle    801029ec <install_trans+0x8c>
80102974:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    struct buf *lbuf = bread(log.dev, log.start+tail+1); // read log block
80102978:	a1 b4 26 11 80       	mov    0x801126b4,%eax
8010297d:	01 d8                	add    %ebx,%eax
8010297f:	83 c0 01             	add    $0x1,%eax
80102982:	89 44 24 04          	mov    %eax,0x4(%esp)
80102986:	a1 c4 26 11 80       	mov    0x801126c4,%eax
8010298b:	89 04 24             	mov    %eax,(%esp)
8010298e:	e8 3d d7 ff ff       	call   801000d0 <bread>
80102993:	89 c7                	mov    %eax,%edi
    struct buf *dbuf = bread(log.dev, log.lh.block[tail]); // read dst
80102995:	8b 04 9d cc 26 11 80 	mov    -0x7feed934(,%ebx,4),%eax
  for (tail = 0; tail < log.lh.n; tail++) {
8010299c:	83 c3 01             	add    $0x1,%ebx
    struct buf *dbuf = bread(log.dev, log.lh.block[tail]); // read dst
8010299f:	89 44 24 04          	mov    %eax,0x4(%esp)
801029a3:	a1 c4 26 11 80       	mov    0x801126c4,%eax
801029a8:	89 04 24             	mov    %eax,(%esp)
801029ab:	e8 20 d7 ff ff       	call   801000d0 <bread>
    memmove(dbuf->data, lbuf->data, BSIZE);  // copy block to dst
801029b0:	c7 44 24 08 00 02 00 	movl   $0x200,0x8(%esp)
801029b7:	00 
    struct buf *dbuf = bread(log.dev, log.lh.block[tail]); // read dst
801029b8:	89 c6                	mov    %eax,%esi
    memmove(dbuf->data, lbuf->data, BSIZE);  // copy block to dst
801029ba:	8d 47 5c             	lea    0x5c(%edi),%eax
801029bd:	89 44 24 04          	mov    %eax,0x4(%esp)
801029c1:	8d 46 5c             	lea    0x5c(%esi),%eax
801029c4:	89 04 24             	mov    %eax,(%esp)
801029c7:	e8 74 19 00 00       	call   80104340 <memmove>
    bwrite(dbuf);  // write dst to disk
801029cc:	89 34 24             	mov    %esi,(%esp)
801029cf:	e8 cc d7 ff ff       	call   801001a0 <bwrite>
    brelse(lbuf);
801029d4:	89 3c 24             	mov    %edi,(%esp)
801029d7:	e8 04 d8 ff ff       	call   801001e0 <brelse>
    brelse(dbuf);
801029dc:	89 34 24             	mov    %esi,(%esp)
801029df:	e8 fc d7 ff ff       	call   801001e0 <brelse>
  for (tail = 0; tail < log.lh.n; tail++) {
801029e4:	39 1d c8 26 11 80    	cmp    %ebx,0x801126c8
801029ea:	7f 8c                	jg     80102978 <install_trans+0x18>
  }
}
801029ec:	83 c4 1c             	add    $0x1c,%esp
801029ef:	5b                   	pop    %ebx
801029f0:	5e                   	pop    %esi
801029f1:	5f                   	pop    %edi
801029f2:	5d                   	pop    %ebp
801029f3:	c3                   	ret    
801029f4:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
801029fa:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

80102a00 <write_head>:
// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
static void
write_head(void)
{
80102a00:	55                   	push   %ebp
80102a01:	89 e5                	mov    %esp,%ebp
80102a03:	57                   	push   %edi
80102a04:	56                   	push   %esi
80102a05:	53                   	push   %ebx
80102a06:	83 ec 1c             	sub    $0x1c,%esp
  struct buf *buf = bread(log.dev, log.start);
80102a09:	a1 b4 26 11 80       	mov    0x801126b4,%eax
80102a0e:	89 44 24 04          	mov    %eax,0x4(%esp)
80102a12:	a1 c4 26 11 80       	mov    0x801126c4,%eax
80102a17:	89 04 24             	mov    %eax,(%esp)
80102a1a:	e8 b1 d6 ff ff       	call   801000d0 <bread>
  struct logheader *hb = (struct logheader *) (buf->data);
  int i;
  hb->n = log.lh.n;
80102a1f:	8b 1d c8 26 11 80    	mov    0x801126c8,%ebx
  for (i = 0; i < log.lh.n; i++) {
80102a25:	31 d2                	xor    %edx,%edx
80102a27:	85 db                	test   %ebx,%ebx
  struct buf *buf = bread(log.dev, log.start);
80102a29:	89 c7                	mov    %eax,%edi
  hb->n = log.lh.n;
80102a2b:	89 58 5c             	mov    %ebx,0x5c(%eax)
80102a2e:	8d 70 5c             	lea    0x5c(%eax),%esi
  for (i = 0; i < log.lh.n; i++) {
80102a31:	7e 17                	jle    80102a4a <write_head+0x4a>
80102a33:	90                   	nop
80102a34:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    hb->block[i] = log.lh.block[i];
80102a38:	8b 0c 95 cc 26 11 80 	mov    -0x7feed934(,%edx,4),%ecx
80102a3f:	89 4c 96 04          	mov    %ecx,0x4(%esi,%edx,4)
  for (i = 0; i < log.lh.n; i++) {
80102a43:	83 c2 01             	add    $0x1,%edx
80102a46:	39 da                	cmp    %ebx,%edx
80102a48:	75 ee                	jne    80102a38 <write_head+0x38>
  }
  bwrite(buf);
80102a4a:	89 3c 24             	mov    %edi,(%esp)
80102a4d:	e8 4e d7 ff ff       	call   801001a0 <bwrite>
  brelse(buf);
80102a52:	89 3c 24             	mov    %edi,(%esp)
80102a55:	e8 86 d7 ff ff       	call   801001e0 <brelse>
}
80102a5a:	83 c4 1c             	add    $0x1c,%esp
80102a5d:	5b                   	pop    %ebx
80102a5e:	5e                   	pop    %esi
80102a5f:	5f                   	pop    %edi
80102a60:	5d                   	pop    %ebp
80102a61:	c3                   	ret    
80102a62:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80102a69:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80102a70 <initlog>:
{
80102a70:	55                   	push   %ebp
80102a71:	89 e5                	mov    %esp,%ebp
80102a73:	56                   	push   %esi
80102a74:	53                   	push   %ebx
80102a75:	83 ec 30             	sub    $0x30,%esp
80102a78:	8b 5d 08             	mov    0x8(%ebp),%ebx
  initlock(&log.lock, "log");
80102a7b:	c7 44 24 04 e0 70 10 	movl   $0x801070e0,0x4(%esp)
80102a82:	80 
80102a83:	c7 04 24 80 26 11 80 	movl   $0x80112680,(%esp)
80102a8a:	e8 e1 15 00 00       	call   80104070 <initlock>
  readsb(dev, &sb);
80102a8f:	8d 45 dc             	lea    -0x24(%ebp),%eax
80102a92:	89 44 24 04          	mov    %eax,0x4(%esp)
80102a96:	89 1c 24             	mov    %ebx,(%esp)
80102a99:	e8 82 e9 ff ff       	call   80101420 <readsb>
  log.start = sb.logstart;
80102a9e:	8b 45 ec             	mov    -0x14(%ebp),%eax
  log.size = sb.nlog;
80102aa1:	8b 55 e8             	mov    -0x18(%ebp),%edx
  struct buf *buf = bread(log.dev, log.start);
80102aa4:	89 1c 24             	mov    %ebx,(%esp)
  log.dev = dev;
80102aa7:	89 1d c4 26 11 80    	mov    %ebx,0x801126c4
  struct buf *buf = bread(log.dev, log.start);
80102aad:	89 44 24 04          	mov    %eax,0x4(%esp)
  log.size = sb.nlog;
80102ab1:	89 15 b8 26 11 80    	mov    %edx,0x801126b8
  log.start = sb.logstart;
80102ab7:	a3 b4 26 11 80       	mov    %eax,0x801126b4
  struct buf *buf = bread(log.dev, log.start);
80102abc:	e8 0f d6 ff ff       	call   801000d0 <bread>
  for (i = 0; i < log.lh.n; i++) {
80102ac1:	31 d2                	xor    %edx,%edx
  log.lh.n = lh->n;
80102ac3:	8b 58 5c             	mov    0x5c(%eax),%ebx
80102ac6:	8d 70 5c             	lea    0x5c(%eax),%esi
  for (i = 0; i < log.lh.n; i++) {
80102ac9:	85 db                	test   %ebx,%ebx
  log.lh.n = lh->n;
80102acb:	89 1d c8 26 11 80    	mov    %ebx,0x801126c8
  for (i = 0; i < log.lh.n; i++) {
80102ad1:	7e 17                	jle    80102aea <initlog+0x7a>
80102ad3:	90                   	nop
80102ad4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    log.lh.block[i] = lh->block[i];
80102ad8:	8b 4c 96 04          	mov    0x4(%esi,%edx,4),%ecx
80102adc:	89 0c 95 cc 26 11 80 	mov    %ecx,-0x7feed934(,%edx,4)
  for (i = 0; i < log.lh.n; i++) {
80102ae3:	83 c2 01             	add    $0x1,%edx
80102ae6:	39 da                	cmp    %ebx,%edx
80102ae8:	75 ee                	jne    80102ad8 <initlog+0x68>
  brelse(buf);
80102aea:	89 04 24             	mov    %eax,(%esp)
80102aed:	e8 ee d6 ff ff       	call   801001e0 <brelse>

static void
recover_from_log(void)
{
  read_head();
  install_trans(); // if committed, copy from log to disk
80102af2:	e8 69 fe ff ff       	call   80102960 <install_trans>
  log.lh.n = 0;
80102af7:	c7 05 c8 26 11 80 00 	movl   $0x0,0x801126c8
80102afe:	00 00 00 
  write_head(); // clear the log
80102b01:	e8 fa fe ff ff       	call   80102a00 <write_head>
}
80102b06:	83 c4 30             	add    $0x30,%esp
80102b09:	5b                   	pop    %ebx
80102b0a:	5e                   	pop    %esi
80102b0b:	5d                   	pop    %ebp
80102b0c:	c3                   	ret    
80102b0d:	8d 76 00             	lea    0x0(%esi),%esi

80102b10 <begin_op>:
}

// called at the start of each FS system call.
void
begin_op(void)
{
80102b10:	55                   	push   %ebp
80102b11:	89 e5                	mov    %esp,%ebp
80102b13:	83 ec 18             	sub    $0x18,%esp
  acquire(&log.lock);
80102b16:	c7 04 24 80 26 11 80 	movl   $0x80112680,(%esp)
80102b1d:	e8 be 16 00 00       	call   801041e0 <acquire>
80102b22:	eb 18                	jmp    80102b3c <begin_op+0x2c>
80102b24:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  while(1){
    if(log.committing){
      sleep(&log, &log.lock);
80102b28:	c7 44 24 04 80 26 11 	movl   $0x80112680,0x4(%esp)
80102b2f:	80 
80102b30:	c7 04 24 80 26 11 80 	movl   $0x80112680,(%esp)
80102b37:	e8 c4 10 00 00       	call   80103c00 <sleep>
    if(log.committing){
80102b3c:	a1 c0 26 11 80       	mov    0x801126c0,%eax
80102b41:	85 c0                	test   %eax,%eax
80102b43:	75 e3                	jne    80102b28 <begin_op+0x18>
    } else if(log.lh.n + (log.outstanding+1)*MAXOPBLOCKS > LOGSIZE){
80102b45:	a1 bc 26 11 80       	mov    0x801126bc,%eax
80102b4a:	8b 15 c8 26 11 80    	mov    0x801126c8,%edx
80102b50:	83 c0 01             	add    $0x1,%eax
80102b53:	8d 0c 80             	lea    (%eax,%eax,4),%ecx
80102b56:	8d 14 4a             	lea    (%edx,%ecx,2),%edx
80102b59:	83 fa 1e             	cmp    $0x1e,%edx
80102b5c:	7f ca                	jg     80102b28 <begin_op+0x18>
      // this op might exhaust log space; wait for commit.
      sleep(&log, &log.lock);
    } else {
      log.outstanding += 1;
      release(&log.lock);
80102b5e:	c7 04 24 80 26 11 80 	movl   $0x80112680,(%esp)
      log.outstanding += 1;
80102b65:	a3 bc 26 11 80       	mov    %eax,0x801126bc
      release(&log.lock);
80102b6a:	e8 e1 16 00 00       	call   80104250 <release>
      break;
    }
  }
}
80102b6f:	c9                   	leave  
80102b70:	c3                   	ret    
80102b71:	eb 0d                	jmp    80102b80 <end_op>
80102b73:	90                   	nop
80102b74:	90                   	nop
80102b75:	90                   	nop
80102b76:	90                   	nop
80102b77:	90                   	nop
80102b78:	90                   	nop
80102b79:	90                   	nop
80102b7a:	90                   	nop
80102b7b:	90                   	nop
80102b7c:	90                   	nop
80102b7d:	90                   	nop
80102b7e:	90                   	nop
80102b7f:	90                   	nop

80102b80 <end_op>:

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
void
end_op(void)
{
80102b80:	55                   	push   %ebp
80102b81:	89 e5                	mov    %esp,%ebp
80102b83:	57                   	push   %edi
80102b84:	56                   	push   %esi
80102b85:	53                   	push   %ebx
80102b86:	83 ec 1c             	sub    $0x1c,%esp
  int do_commit = 0;

  acquire(&log.lock);
80102b89:	c7 04 24 80 26 11 80 	movl   $0x80112680,(%esp)
80102b90:	e8 4b 16 00 00       	call   801041e0 <acquire>
  log.outstanding -= 1;
80102b95:	a1 bc 26 11 80       	mov    0x801126bc,%eax
  if(log.committing)
80102b9a:	8b 15 c0 26 11 80    	mov    0x801126c0,%edx
  log.outstanding -= 1;
80102ba0:	83 e8 01             	sub    $0x1,%eax
  if(log.committing)
80102ba3:	85 d2                	test   %edx,%edx
  log.outstanding -= 1;
80102ba5:	a3 bc 26 11 80       	mov    %eax,0x801126bc
  if(log.committing)
80102baa:	0f 85 f3 00 00 00    	jne    80102ca3 <end_op+0x123>
    panic("log.committing");
  if(log.outstanding == 0){
80102bb0:	85 c0                	test   %eax,%eax
80102bb2:	0f 85 cb 00 00 00    	jne    80102c83 <end_op+0x103>
    // begin_op() may be waiting for log space,
    // and decrementing log.outstanding has decreased
    // the amount of reserved space.
    wakeup(&log);
  }
  release(&log.lock);
80102bb8:	c7 04 24 80 26 11 80 	movl   $0x80112680,(%esp)
}

static void
commit()
{
  if (log.lh.n > 0) {
80102bbf:	31 db                	xor    %ebx,%ebx
    log.committing = 1;
80102bc1:	c7 05 c0 26 11 80 01 	movl   $0x1,0x801126c0
80102bc8:	00 00 00 
  release(&log.lock);
80102bcb:	e8 80 16 00 00       	call   80104250 <release>
  if (log.lh.n > 0) {
80102bd0:	a1 c8 26 11 80       	mov    0x801126c8,%eax
80102bd5:	85 c0                	test   %eax,%eax
80102bd7:	0f 8e 90 00 00 00    	jle    80102c6d <end_op+0xed>
80102bdd:	8d 76 00             	lea    0x0(%esi),%esi
    struct buf *to = bread(log.dev, log.start+tail+1); // log block
80102be0:	a1 b4 26 11 80       	mov    0x801126b4,%eax
80102be5:	01 d8                	add    %ebx,%eax
80102be7:	83 c0 01             	add    $0x1,%eax
80102bea:	89 44 24 04          	mov    %eax,0x4(%esp)
80102bee:	a1 c4 26 11 80       	mov    0x801126c4,%eax
80102bf3:	89 04 24             	mov    %eax,(%esp)
80102bf6:	e8 d5 d4 ff ff       	call   801000d0 <bread>
80102bfb:	89 c6                	mov    %eax,%esi
    struct buf *from = bread(log.dev, log.lh.block[tail]); // cache block
80102bfd:	8b 04 9d cc 26 11 80 	mov    -0x7feed934(,%ebx,4),%eax
  for (tail = 0; tail < log.lh.n; tail++) {
80102c04:	83 c3 01             	add    $0x1,%ebx
    struct buf *from = bread(log.dev, log.lh.block[tail]); // cache block
80102c07:	89 44 24 04          	mov    %eax,0x4(%esp)
80102c0b:	a1 c4 26 11 80       	mov    0x801126c4,%eax
80102c10:	89 04 24             	mov    %eax,(%esp)
80102c13:	e8 b8 d4 ff ff       	call   801000d0 <bread>
    memmove(to->data, from->data, BSIZE);
80102c18:	c7 44 24 08 00 02 00 	movl   $0x200,0x8(%esp)
80102c1f:	00 
    struct buf *from = bread(log.dev, log.lh.block[tail]); // cache block
80102c20:	89 c7                	mov    %eax,%edi
    memmove(to->data, from->data, BSIZE);
80102c22:	8d 40 5c             	lea    0x5c(%eax),%eax
80102c25:	89 44 24 04          	mov    %eax,0x4(%esp)
80102c29:	8d 46 5c             	lea    0x5c(%esi),%eax
80102c2c:	89 04 24             	mov    %eax,(%esp)
80102c2f:	e8 0c 17 00 00       	call   80104340 <memmove>
    bwrite(to);  // write the log
80102c34:	89 34 24             	mov    %esi,(%esp)
80102c37:	e8 64 d5 ff ff       	call   801001a0 <bwrite>
    brelse(from);
80102c3c:	89 3c 24             	mov    %edi,(%esp)
80102c3f:	e8 9c d5 ff ff       	call   801001e0 <brelse>
    brelse(to);
80102c44:	89 34 24             	mov    %esi,(%esp)
80102c47:	e8 94 d5 ff ff       	call   801001e0 <brelse>
  for (tail = 0; tail < log.lh.n; tail++) {
80102c4c:	3b 1d c8 26 11 80    	cmp    0x801126c8,%ebx
80102c52:	7c 8c                	jl     80102be0 <end_op+0x60>
    write_log();     // Write modified blocks from cache to log
    write_head();    // Write header to disk -- the real commit
80102c54:	e8 a7 fd ff ff       	call   80102a00 <write_head>
    install_trans(); // Now install writes to home locations
80102c59:	e8 02 fd ff ff       	call   80102960 <install_trans>
    log.lh.n = 0;
80102c5e:	c7 05 c8 26 11 80 00 	movl   $0x0,0x801126c8
80102c65:	00 00 00 
    write_head();    // Erase the transaction from the log
80102c68:	e8 93 fd ff ff       	call   80102a00 <write_head>
    acquire(&log.lock);
80102c6d:	c7 04 24 80 26 11 80 	movl   $0x80112680,(%esp)
80102c74:	e8 67 15 00 00       	call   801041e0 <acquire>
    log.committing = 0;
80102c79:	c7 05 c0 26 11 80 00 	movl   $0x0,0x801126c0
80102c80:	00 00 00 
    wakeup(&log);
80102c83:	c7 04 24 80 26 11 80 	movl   $0x80112680,(%esp)
80102c8a:	e8 01 11 00 00       	call   80103d90 <wakeup>
    release(&log.lock);
80102c8f:	c7 04 24 80 26 11 80 	movl   $0x80112680,(%esp)
80102c96:	e8 b5 15 00 00       	call   80104250 <release>
}
80102c9b:	83 c4 1c             	add    $0x1c,%esp
80102c9e:	5b                   	pop    %ebx
80102c9f:	5e                   	pop    %esi
80102ca0:	5f                   	pop    %edi
80102ca1:	5d                   	pop    %ebp
80102ca2:	c3                   	ret    
    panic("log.committing");
80102ca3:	c7 04 24 e4 70 10 80 	movl   $0x801070e4,(%esp)
80102caa:	e8 b1 d6 ff ff       	call   80100360 <panic>
80102caf:	90                   	nop

80102cb0 <log_write>:
//   modify bp->data[]
//   log_write(bp)
//   brelse(bp)
void
log_write(struct buf *b)
{
80102cb0:	55                   	push   %ebp
80102cb1:	89 e5                	mov    %esp,%ebp
80102cb3:	53                   	push   %ebx
80102cb4:	83 ec 14             	sub    $0x14,%esp
  int i;

  if (log.lh.n >= LOGSIZE || log.lh.n >= log.size - 1)
80102cb7:	a1 c8 26 11 80       	mov    0x801126c8,%eax
{
80102cbc:	8b 5d 08             	mov    0x8(%ebp),%ebx
  if (log.lh.n >= LOGSIZE || log.lh.n >= log.size - 1)
80102cbf:	83 f8 1d             	cmp    $0x1d,%eax
80102cc2:	0f 8f 98 00 00 00    	jg     80102d60 <log_write+0xb0>
80102cc8:	8b 0d b8 26 11 80    	mov    0x801126b8,%ecx
80102cce:	8d 51 ff             	lea    -0x1(%ecx),%edx
80102cd1:	39 d0                	cmp    %edx,%eax
80102cd3:	0f 8d 87 00 00 00    	jge    80102d60 <log_write+0xb0>
    panic("too big a transaction");
  if (log.outstanding < 1)
80102cd9:	a1 bc 26 11 80       	mov    0x801126bc,%eax
80102cde:	85 c0                	test   %eax,%eax
80102ce0:	0f 8e 86 00 00 00    	jle    80102d6c <log_write+0xbc>
    panic("log_write outside of trans");

  acquire(&log.lock);
80102ce6:	c7 04 24 80 26 11 80 	movl   $0x80112680,(%esp)
80102ced:	e8 ee 14 00 00       	call   801041e0 <acquire>
  for (i = 0; i < log.lh.n; i++) {
80102cf2:	8b 15 c8 26 11 80    	mov    0x801126c8,%edx
80102cf8:	83 fa 00             	cmp    $0x0,%edx
80102cfb:	7e 54                	jle    80102d51 <log_write+0xa1>
    if (log.lh.block[i] == b->blockno)   // log absorbtion
80102cfd:	8b 4b 08             	mov    0x8(%ebx),%ecx
  for (i = 0; i < log.lh.n; i++) {
80102d00:	31 c0                	xor    %eax,%eax
    if (log.lh.block[i] == b->blockno)   // log absorbtion
80102d02:	39 0d cc 26 11 80    	cmp    %ecx,0x801126cc
80102d08:	75 0f                	jne    80102d19 <log_write+0x69>
80102d0a:	eb 3c                	jmp    80102d48 <log_write+0x98>
80102d0c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80102d10:	39 0c 85 cc 26 11 80 	cmp    %ecx,-0x7feed934(,%eax,4)
80102d17:	74 2f                	je     80102d48 <log_write+0x98>
  for (i = 0; i < log.lh.n; i++) {
80102d19:	83 c0 01             	add    $0x1,%eax
80102d1c:	39 d0                	cmp    %edx,%eax
80102d1e:	75 f0                	jne    80102d10 <log_write+0x60>
      break;
  }
  log.lh.block[i] = b->blockno;
80102d20:	89 0c 95 cc 26 11 80 	mov    %ecx,-0x7feed934(,%edx,4)
  if (i == log.lh.n)
    log.lh.n++;
80102d27:	83 c2 01             	add    $0x1,%edx
80102d2a:	89 15 c8 26 11 80    	mov    %edx,0x801126c8
  b->flags |= B_DIRTY; // prevent eviction
80102d30:	83 0b 04             	orl    $0x4,(%ebx)
  release(&log.lock);
80102d33:	c7 45 08 80 26 11 80 	movl   $0x80112680,0x8(%ebp)
}
80102d3a:	83 c4 14             	add    $0x14,%esp
80102d3d:	5b                   	pop    %ebx
80102d3e:	5d                   	pop    %ebp
  release(&log.lock);
80102d3f:	e9 0c 15 00 00       	jmp    80104250 <release>
80102d44:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  log.lh.block[i] = b->blockno;
80102d48:	89 0c 85 cc 26 11 80 	mov    %ecx,-0x7feed934(,%eax,4)
80102d4f:	eb df                	jmp    80102d30 <log_write+0x80>
80102d51:	8b 43 08             	mov    0x8(%ebx),%eax
80102d54:	a3 cc 26 11 80       	mov    %eax,0x801126cc
  if (i == log.lh.n)
80102d59:	75 d5                	jne    80102d30 <log_write+0x80>
80102d5b:	eb ca                	jmp    80102d27 <log_write+0x77>
80102d5d:	8d 76 00             	lea    0x0(%esi),%esi
    panic("too big a transaction");
80102d60:	c7 04 24 f3 70 10 80 	movl   $0x801070f3,(%esp)
80102d67:	e8 f4 d5 ff ff       	call   80100360 <panic>
    panic("log_write outside of trans");
80102d6c:	c7 04 24 09 71 10 80 	movl   $0x80107109,(%esp)
80102d73:	e8 e8 d5 ff ff       	call   80100360 <panic>
80102d78:	66 90                	xchg   %ax,%ax
80102d7a:	66 90                	xchg   %ax,%ax
80102d7c:	66 90                	xchg   %ax,%ax
80102d7e:	66 90                	xchg   %ax,%ax

80102d80 <mpmain>:
}

// Common CPU setup code.
static void
mpmain(void)
{
80102d80:	55                   	push   %ebp
80102d81:	89 e5                	mov    %esp,%ebp
80102d83:	53                   	push   %ebx
80102d84:	83 ec 14             	sub    $0x14,%esp
  cprintf("cpu%d: starting %d\n", cpuid(), cpuid());
80102d87:	e8 f4 08 00 00       	call   80103680 <cpuid>
80102d8c:	89 c3                	mov    %eax,%ebx
80102d8e:	e8 ed 08 00 00       	call   80103680 <cpuid>
80102d93:	89 5c 24 08          	mov    %ebx,0x8(%esp)
80102d97:	c7 04 24 24 71 10 80 	movl   $0x80107124,(%esp)
80102d9e:	89 44 24 04          	mov    %eax,0x4(%esp)
80102da2:	e8 a9 d8 ff ff       	call   80100650 <cprintf>
  idtinit();       // load idt register
80102da7:	e8 24 27 00 00       	call   801054d0 <idtinit>
  xchg(&(mycpu()->started), 1); // tell startothers() we're up
80102dac:	e8 4f 08 00 00       	call   80103600 <mycpu>
80102db1:	89 c2                	mov    %eax,%edx
xchg(volatile uint *addr, uint newval)
{
  uint result;

  // The + in "+m" denotes a read-modify-write operand.
  asm volatile("lock; xchgl %0, %1" :
80102db3:	b8 01 00 00 00       	mov    $0x1,%eax
80102db8:	f0 87 82 a0 00 00 00 	lock xchg %eax,0xa0(%edx)
  scheduler();     // start running processes
80102dbf:	e8 9c 0b 00 00       	call   80103960 <scheduler>
80102dc4:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80102dca:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

80102dd0 <mpenter>:
{
80102dd0:	55                   	push   %ebp
80102dd1:	89 e5                	mov    %esp,%ebp
80102dd3:	83 ec 08             	sub    $0x8,%esp
  switchkvm();
80102dd6:	e8 c5 37 00 00       	call   801065a0 <switchkvm>
  seginit();
80102ddb:	e8 00 37 00 00       	call   801064e0 <seginit>
  lapicinit();
80102de0:	e8 8b f8 ff ff       	call   80102670 <lapicinit>
  mpmain();
80102de5:	e8 96 ff ff ff       	call   80102d80 <mpmain>
80102dea:	66 90                	xchg   %ax,%ax
80102dec:	66 90                	xchg   %ax,%ax
80102dee:	66 90                	xchg   %ax,%ax

80102df0 <main>:
{
80102df0:	55                   	push   %ebp
80102df1:	89 e5                	mov    %esp,%ebp
80102df3:	53                   	push   %ebx
  // The linker has placed the image of entryother.S in
  // _binary_entryother_start.
  code = P2V(0x7000);
  memmove(code, _binary_entryother_start, (uint)_binary_entryother_size);

  for(c = cpus; c < cpus+ncpu; c++){
80102df4:	bb 80 27 11 80       	mov    $0x80112780,%ebx
{
80102df9:	83 e4 f0             	and    $0xfffffff0,%esp
80102dfc:	83 ec 10             	sub    $0x10,%esp
  kinit1(end, P2V(4*1024*1024)); // phys page allocator
80102dff:	c7 44 24 04 00 00 40 	movl   $0x80400000,0x4(%esp)
80102e06:	80 
80102e07:	c7 04 24 a8 55 11 80 	movl   $0x801155a8,(%esp)
80102e0e:	e8 cd f5 ff ff       	call   801023e0 <kinit1>
  kvmalloc();      // kernel page table
80102e13:	e8 18 3c 00 00       	call   80106a30 <kvmalloc>
  mpinit();        // detect other processors
80102e18:	e8 73 01 00 00       	call   80102f90 <mpinit>
80102e1d:	8d 76 00             	lea    0x0(%esi),%esi
  lapicinit();     // interrupt controller
80102e20:	e8 4b f8 ff ff       	call   80102670 <lapicinit>
  seginit();       // segment descriptors
80102e25:	e8 b6 36 00 00       	call   801064e0 <seginit>
  picinit();       // disable pic
80102e2a:	e8 21 03 00 00       	call   80103150 <picinit>
80102e2f:	90                   	nop
  ioapicinit();    // another interrupt controller
80102e30:	e8 cb f3 ff ff       	call   80102200 <ioapicinit>
  consoleinit();   // console hardware
80102e35:	e8 16 db ff ff       	call   80100950 <consoleinit>
  uartinit();      // serial port
80102e3a:	e8 c1 29 00 00       	call   80105800 <uartinit>
80102e3f:	90                   	nop
  pinit();         // process table
80102e40:	e8 9b 07 00 00       	call   801035e0 <pinit>
  tvinit();        // trap vectors
80102e45:	e8 e6 25 00 00       	call   80105430 <tvinit>
  binit();         // buffer cache
80102e4a:	e8 f1 d1 ff ff       	call   80100040 <binit>
80102e4f:	90                   	nop
  fileinit();      // file table
80102e50:	e8 fb de ff ff       	call   80100d50 <fileinit>
  ideinit();       // disk 
80102e55:	e8 a6 f1 ff ff       	call   80102000 <ideinit>
  memmove(code, _binary_entryother_start, (uint)_binary_entryother_size);
80102e5a:	c7 44 24 08 8a 00 00 	movl   $0x8a,0x8(%esp)
80102e61:	00 
80102e62:	c7 44 24 04 8c a4 10 	movl   $0x8010a48c,0x4(%esp)
80102e69:	80 
80102e6a:	c7 04 24 00 70 00 80 	movl   $0x80007000,(%esp)
80102e71:	e8 ca 14 00 00       	call   80104340 <memmove>
  for(c = cpus; c < cpus+ncpu; c++){
80102e76:	69 05 00 2d 11 80 b0 	imul   $0xb0,0x80112d00,%eax
80102e7d:	00 00 00 
80102e80:	05 80 27 11 80       	add    $0x80112780,%eax
80102e85:	39 d8                	cmp    %ebx,%eax
80102e87:	76 6a                	jbe    80102ef3 <main+0x103>
80102e89:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
    if(c == mycpu())  // We've started already.
80102e90:	e8 6b 07 00 00       	call   80103600 <mycpu>
80102e95:	39 d8                	cmp    %ebx,%eax
80102e97:	74 41                	je     80102eda <main+0xea>
      continue;

    // Tell entryother.S what stack to use, where to enter, and what
    // pgdir to use. We cannot use kpgdir yet, because the AP processor
    // is running in low  memory, so we use entrypgdir for the APs too.
    stack = kalloc();
80102e99:	e8 02 f6 ff ff       	call   801024a0 <kalloc>
    *(void**)(code-4) = stack + KSTACKSIZE;
    *(void(**)(void))(code-8) = mpenter;
80102e9e:	c7 05 f8 6f 00 80 d0 	movl   $0x80102dd0,0x80006ff8
80102ea5:	2d 10 80 
    *(int**)(code-12) = (void *) V2P(entrypgdir);
80102ea8:	c7 05 f4 6f 00 80 00 	movl   $0x109000,0x80006ff4
80102eaf:	90 10 00 
    *(void**)(code-4) = stack + KSTACKSIZE;
80102eb2:	05 00 10 00 00       	add    $0x1000,%eax
80102eb7:	a3 fc 6f 00 80       	mov    %eax,0x80006ffc

    lapicstartap(c->apicid, V2P(code));
80102ebc:	0f b6 03             	movzbl (%ebx),%eax
80102ebf:	c7 44 24 04 00 70 00 	movl   $0x7000,0x4(%esp)
80102ec6:	00 
80102ec7:	89 04 24             	mov    %eax,(%esp)
80102eca:	e8 e1 f8 ff ff       	call   801027b0 <lapicstartap>
80102ecf:	90                   	nop

    // wait for cpu to finish mpmain()
    while(c->started == 0)
80102ed0:	8b 83 a0 00 00 00    	mov    0xa0(%ebx),%eax
80102ed6:	85 c0                	test   %eax,%eax
80102ed8:	74 f6                	je     80102ed0 <main+0xe0>
  for(c = cpus; c < cpus+ncpu; c++){
80102eda:	69 05 00 2d 11 80 b0 	imul   $0xb0,0x80112d00,%eax
80102ee1:	00 00 00 
80102ee4:	81 c3 b0 00 00 00    	add    $0xb0,%ebx
80102eea:	05 80 27 11 80       	add    $0x80112780,%eax
80102eef:	39 c3                	cmp    %eax,%ebx
80102ef1:	72 9d                	jb     80102e90 <main+0xa0>
  kinit2(P2V(4*1024*1024), P2V(PHYSTOP)); // must come after startothers()
80102ef3:	c7 44 24 04 00 00 00 	movl   $0x8e000000,0x4(%esp)
80102efa:	8e 
80102efb:	c7 04 24 00 00 40 80 	movl   $0x80400000,(%esp)
80102f02:	e8 49 f5 ff ff       	call   80102450 <kinit2>
  userinit();      // first user process
80102f07:	e8 c4 07 00 00       	call   801036d0 <userinit>
  mpmain();        // finish this processor's setup
80102f0c:	e8 6f fe ff ff       	call   80102d80 <mpmain>
80102f11:	66 90                	xchg   %ax,%ax
80102f13:	66 90                	xchg   %ax,%ax
80102f15:	66 90                	xchg   %ax,%ax
80102f17:	66 90                	xchg   %ax,%ax
80102f19:	66 90                	xchg   %ax,%ax
80102f1b:	66 90                	xchg   %ax,%ax
80102f1d:	66 90                	xchg   %ax,%ax
80102f1f:	90                   	nop

80102f20 <mpsearch1>:
}

// Look for an MP structure in the len bytes at addr.
static struct mp*
mpsearch1(uint a, int len)
{
80102f20:	55                   	push   %ebp
80102f21:	89 e5                	mov    %esp,%ebp
80102f23:	56                   	push   %esi
  uchar *e, *p, *addr;

  addr = P2V(a);
80102f24:	8d b0 00 00 00 80    	lea    -0x80000000(%eax),%esi
{
80102f2a:	53                   	push   %ebx
  e = addr+len;
80102f2b:	8d 1c 16             	lea    (%esi,%edx,1),%ebx
{
80102f2e:	83 ec 10             	sub    $0x10,%esp
  for(p = addr; p < e; p += sizeof(struct mp))
80102f31:	39 de                	cmp    %ebx,%esi
80102f33:	73 3c                	jae    80102f71 <mpsearch1+0x51>
80102f35:	8d 76 00             	lea    0x0(%esi),%esi
    if(memcmp(p, "_MP_", 4) == 0 && sum(p, sizeof(struct mp)) == 0)
80102f38:	c7 44 24 08 04 00 00 	movl   $0x4,0x8(%esp)
80102f3f:	00 
80102f40:	c7 44 24 04 38 71 10 	movl   $0x80107138,0x4(%esp)
80102f47:	80 
80102f48:	89 34 24             	mov    %esi,(%esp)
80102f4b:	e8 a0 13 00 00       	call   801042f0 <memcmp>
80102f50:	85 c0                	test   %eax,%eax
80102f52:	75 16                	jne    80102f6a <mpsearch1+0x4a>
80102f54:	31 c9                	xor    %ecx,%ecx
80102f56:	31 d2                	xor    %edx,%edx
    sum += addr[i];
80102f58:	0f b6 04 16          	movzbl (%esi,%edx,1),%eax
  for(i=0; i<len; i++)
80102f5c:	83 c2 01             	add    $0x1,%edx
    sum += addr[i];
80102f5f:	01 c1                	add    %eax,%ecx
  for(i=0; i<len; i++)
80102f61:	83 fa 10             	cmp    $0x10,%edx
80102f64:	75 f2                	jne    80102f58 <mpsearch1+0x38>
    if(memcmp(p, "_MP_", 4) == 0 && sum(p, sizeof(struct mp)) == 0)
80102f66:	84 c9                	test   %cl,%cl
80102f68:	74 10                	je     80102f7a <mpsearch1+0x5a>
  for(p = addr; p < e; p += sizeof(struct mp))
80102f6a:	83 c6 10             	add    $0x10,%esi
80102f6d:	39 f3                	cmp    %esi,%ebx
80102f6f:	77 c7                	ja     80102f38 <mpsearch1+0x18>
      return (struct mp*)p;
  return 0;
}
80102f71:	83 c4 10             	add    $0x10,%esp
  return 0;
80102f74:	31 c0                	xor    %eax,%eax
}
80102f76:	5b                   	pop    %ebx
80102f77:	5e                   	pop    %esi
80102f78:	5d                   	pop    %ebp
80102f79:	c3                   	ret    
80102f7a:	83 c4 10             	add    $0x10,%esp
80102f7d:	89 f0                	mov    %esi,%eax
80102f7f:	5b                   	pop    %ebx
80102f80:	5e                   	pop    %esi
80102f81:	5d                   	pop    %ebp
80102f82:	c3                   	ret    
80102f83:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80102f89:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80102f90 <mpinit>:
  return conf;
}

void
mpinit(void)
{
80102f90:	55                   	push   %ebp
80102f91:	89 e5                	mov    %esp,%ebp
80102f93:	57                   	push   %edi
80102f94:	56                   	push   %esi
80102f95:	53                   	push   %ebx
80102f96:	83 ec 1c             	sub    $0x1c,%esp
  if((p = ((bda[0x0F]<<8)| bda[0x0E]) << 4)){
80102f99:	0f b6 05 0f 04 00 80 	movzbl 0x8000040f,%eax
80102fa0:	0f b6 15 0e 04 00 80 	movzbl 0x8000040e,%edx
80102fa7:	c1 e0 08             	shl    $0x8,%eax
80102faa:	09 d0                	or     %edx,%eax
80102fac:	c1 e0 04             	shl    $0x4,%eax
80102faf:	85 c0                	test   %eax,%eax
80102fb1:	75 1b                	jne    80102fce <mpinit+0x3e>
    p = ((bda[0x14]<<8)|bda[0x13])*1024;
80102fb3:	0f b6 05 14 04 00 80 	movzbl 0x80000414,%eax
80102fba:	0f b6 15 13 04 00 80 	movzbl 0x80000413,%edx
80102fc1:	c1 e0 08             	shl    $0x8,%eax
80102fc4:	09 d0                	or     %edx,%eax
80102fc6:	c1 e0 0a             	shl    $0xa,%eax
    if((mp = mpsearch1(p-1024, 1024)))
80102fc9:	2d 00 04 00 00       	sub    $0x400,%eax
    if((mp = mpsearch1(p, 1024)))
80102fce:	ba 00 04 00 00       	mov    $0x400,%edx
80102fd3:	e8 48 ff ff ff       	call   80102f20 <mpsearch1>
80102fd8:	85 c0                	test   %eax,%eax
80102fda:	89 c7                	mov    %eax,%edi
80102fdc:	0f 84 22 01 00 00    	je     80103104 <mpinit+0x174>
  if((mp = mpsearch()) == 0 || mp->physaddr == 0)
80102fe2:	8b 77 04             	mov    0x4(%edi),%esi
80102fe5:	85 f6                	test   %esi,%esi
80102fe7:	0f 84 30 01 00 00    	je     8010311d <mpinit+0x18d>
  conf = (struct mpconf*) P2V((uint) mp->physaddr);
80102fed:	8d 86 00 00 00 80    	lea    -0x80000000(%esi),%eax
  if(memcmp(conf, "PCMP", 4) != 0)
80102ff3:	c7 44 24 08 04 00 00 	movl   $0x4,0x8(%esp)
80102ffa:	00 
80102ffb:	c7 44 24 04 3d 71 10 	movl   $0x8010713d,0x4(%esp)
80103002:	80 
80103003:	89 04 24             	mov    %eax,(%esp)
  conf = (struct mpconf*) P2V((uint) mp->physaddr);
80103006:	89 45 e4             	mov    %eax,-0x1c(%ebp)
  if(memcmp(conf, "PCMP", 4) != 0)
80103009:	e8 e2 12 00 00       	call   801042f0 <memcmp>
8010300e:	85 c0                	test   %eax,%eax
80103010:	0f 85 07 01 00 00    	jne    8010311d <mpinit+0x18d>
  if(conf->version != 1 && conf->version != 4)
80103016:	0f b6 86 06 00 00 80 	movzbl -0x7ffffffa(%esi),%eax
8010301d:	3c 04                	cmp    $0x4,%al
8010301f:	0f 85 0b 01 00 00    	jne    80103130 <mpinit+0x1a0>
  if(sum((uchar*)conf, conf->length) != 0)
80103025:	0f b7 86 04 00 00 80 	movzwl -0x7ffffffc(%esi),%eax
  for(i=0; i<len; i++)
8010302c:	85 c0                	test   %eax,%eax
8010302e:	74 21                	je     80103051 <mpinit+0xc1>
  sum = 0;
80103030:	31 c9                	xor    %ecx,%ecx
  for(i=0; i<len; i++)
80103032:	31 d2                	xor    %edx,%edx
80103034:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    sum += addr[i];
80103038:	0f b6 9c 16 00 00 00 	movzbl -0x80000000(%esi,%edx,1),%ebx
8010303f:	80 
  for(i=0; i<len; i++)
80103040:	83 c2 01             	add    $0x1,%edx
    sum += addr[i];
80103043:	01 d9                	add    %ebx,%ecx
  for(i=0; i<len; i++)
80103045:	39 d0                	cmp    %edx,%eax
80103047:	7f ef                	jg     80103038 <mpinit+0xa8>
  if(sum((uchar*)conf, conf->length) != 0)
80103049:	84 c9                	test   %cl,%cl
8010304b:	0f 85 cc 00 00 00    	jne    8010311d <mpinit+0x18d>
  struct mp *mp;
  struct mpconf *conf;
  struct mpproc *proc;
  struct mpioapic *ioapic;

  if((conf = mpconfig(&mp)) == 0)
80103051:	8b 45 e4             	mov    -0x1c(%ebp),%eax
80103054:	85 c0                	test   %eax,%eax
80103056:	0f 84 c1 00 00 00    	je     8010311d <mpinit+0x18d>
    panic("Expect to run on an SMP");
  ismp = 1;
  lapic = (uint*)conf->lapicaddr;
8010305c:	8b 86 24 00 00 80    	mov    -0x7fffffdc(%esi),%eax
  ismp = 1;
80103062:	bb 01 00 00 00       	mov    $0x1,%ebx
  lapic = (uint*)conf->lapicaddr;
80103067:	a3 7c 26 11 80       	mov    %eax,0x8011267c
  for(p=(uchar*)(conf+1), e=(uchar*)conf+conf->length; p<e; ){
8010306c:	0f b7 96 04 00 00 80 	movzwl -0x7ffffffc(%esi),%edx
80103073:	8d 86 2c 00 00 80    	lea    -0x7fffffd4(%esi),%eax
80103079:	03 55 e4             	add    -0x1c(%ebp),%edx
8010307c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80103080:	39 c2                	cmp    %eax,%edx
80103082:	76 1b                	jbe    8010309f <mpinit+0x10f>
80103084:	0f b6 08             	movzbl (%eax),%ecx
    switch(*p){
80103087:	80 f9 04             	cmp    $0x4,%cl
8010308a:	77 74                	ja     80103100 <mpinit+0x170>
8010308c:	ff 24 8d 7c 71 10 80 	jmp    *-0x7fef8e84(,%ecx,4)
80103093:	90                   	nop
80103094:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      p += sizeof(struct mpioapic);
      continue;
    case MPBUS:
    case MPIOINTR:
    case MPLINTR:
      p += 8;
80103098:	83 c0 08             	add    $0x8,%eax
  for(p=(uchar*)(conf+1), e=(uchar*)conf+conf->length; p<e; ){
8010309b:	39 c2                	cmp    %eax,%edx
8010309d:	77 e5                	ja     80103084 <mpinit+0xf4>
    default:
      ismp = 0;
      break;
    }
  }
  if(!ismp)
8010309f:	85 db                	test   %ebx,%ebx
801030a1:	0f 84 93 00 00 00    	je     8010313a <mpinit+0x1aa>
    panic("Didn't find a suitable machine");

  if(mp->imcrp){
801030a7:	80 7f 0c 00          	cmpb   $0x0,0xc(%edi)
801030ab:	74 12                	je     801030bf <mpinit+0x12f>
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
801030ad:	ba 22 00 00 00       	mov    $0x22,%edx
801030b2:	b8 70 00 00 00       	mov    $0x70,%eax
801030b7:	ee                   	out    %al,(%dx)
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
801030b8:	b2 23                	mov    $0x23,%dl
801030ba:	ec                   	in     (%dx),%al
    // Bochs doesn't support IMCR, so this doesn't run on Bochs.
    // But it would on real hardware.
    outb(0x22, 0x70);   // Select IMCR
    outb(0x23, inb(0x23) | 1);  // Mask external interrupts.
801030bb:	83 c8 01             	or     $0x1,%eax
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
801030be:	ee                   	out    %al,(%dx)
  }
}
801030bf:	83 c4 1c             	add    $0x1c,%esp
801030c2:	5b                   	pop    %ebx
801030c3:	5e                   	pop    %esi
801030c4:	5f                   	pop    %edi
801030c5:	5d                   	pop    %ebp
801030c6:	c3                   	ret    
801030c7:	90                   	nop
      if(ncpu < NCPU) {
801030c8:	8b 35 00 2d 11 80    	mov    0x80112d00,%esi
801030ce:	83 fe 07             	cmp    $0x7,%esi
801030d1:	7f 17                	jg     801030ea <mpinit+0x15a>
        cpus[ncpu].apicid = proc->apicid;  // apicid may differ from ncpu
801030d3:	0f b6 48 01          	movzbl 0x1(%eax),%ecx
801030d7:	69 f6 b0 00 00 00    	imul   $0xb0,%esi,%esi
        ncpu++;
801030dd:	83 05 00 2d 11 80 01 	addl   $0x1,0x80112d00
        cpus[ncpu].apicid = proc->apicid;  // apicid may differ from ncpu
801030e4:	88 8e 80 27 11 80    	mov    %cl,-0x7feed880(%esi)
      p += sizeof(struct mpproc);
801030ea:	83 c0 14             	add    $0x14,%eax
      continue;
801030ed:	eb 91                	jmp    80103080 <mpinit+0xf0>
801030ef:	90                   	nop
      ioapicid = ioapic->apicno;
801030f0:	0f b6 48 01          	movzbl 0x1(%eax),%ecx
      p += sizeof(struct mpioapic);
801030f4:	83 c0 08             	add    $0x8,%eax
      ioapicid = ioapic->apicno;
801030f7:	88 0d 60 27 11 80    	mov    %cl,0x80112760
      continue;
801030fd:	eb 81                	jmp    80103080 <mpinit+0xf0>
801030ff:	90                   	nop
      ismp = 0;
80103100:	31 db                	xor    %ebx,%ebx
80103102:	eb 83                	jmp    80103087 <mpinit+0xf7>
  return mpsearch1(0xF0000, 0x10000);
80103104:	ba 00 00 01 00       	mov    $0x10000,%edx
80103109:	b8 00 00 0f 00       	mov    $0xf0000,%eax
8010310e:	e8 0d fe ff ff       	call   80102f20 <mpsearch1>
  if((mp = mpsearch()) == 0 || mp->physaddr == 0)
80103113:	85 c0                	test   %eax,%eax
  return mpsearch1(0xF0000, 0x10000);
80103115:	89 c7                	mov    %eax,%edi
  if((mp = mpsearch()) == 0 || mp->physaddr == 0)
80103117:	0f 85 c5 fe ff ff    	jne    80102fe2 <mpinit+0x52>
    panic("Expect to run on an SMP");
8010311d:	c7 04 24 42 71 10 80 	movl   $0x80107142,(%esp)
80103124:	e8 37 d2 ff ff       	call   80100360 <panic>
80103129:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  if(conf->version != 1 && conf->version != 4)
80103130:	3c 01                	cmp    $0x1,%al
80103132:	0f 84 ed fe ff ff    	je     80103025 <mpinit+0x95>
80103138:	eb e3                	jmp    8010311d <mpinit+0x18d>
    panic("Didn't find a suitable machine");
8010313a:	c7 04 24 5c 71 10 80 	movl   $0x8010715c,(%esp)
80103141:	e8 1a d2 ff ff       	call   80100360 <panic>
80103146:	66 90                	xchg   %ax,%ax
80103148:	66 90                	xchg   %ax,%ax
8010314a:	66 90                	xchg   %ax,%ax
8010314c:	66 90                	xchg   %ax,%ax
8010314e:	66 90                	xchg   %ax,%ax

80103150 <picinit>:
80103150:	55                   	push   %ebp
80103151:	ba 21 00 00 00       	mov    $0x21,%edx
80103156:	89 e5                	mov    %esp,%ebp
80103158:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
8010315d:	ee                   	out    %al,(%dx)
8010315e:	b2 a1                	mov    $0xa1,%dl
80103160:	ee                   	out    %al,(%dx)
80103161:	5d                   	pop    %ebp
80103162:	c3                   	ret    
80103163:	66 90                	xchg   %ax,%ax
80103165:	66 90                	xchg   %ax,%ax
80103167:	66 90                	xchg   %ax,%ax
80103169:	66 90                	xchg   %ax,%ax
8010316b:	66 90                	xchg   %ax,%ax
8010316d:	66 90                	xchg   %ax,%ax
8010316f:	90                   	nop

80103170 <pipealloc>:
  int writeopen;  // write fd is still open
};

int
pipealloc(struct file **f0, struct file **f1)
{
80103170:	55                   	push   %ebp
80103171:	89 e5                	mov    %esp,%ebp
80103173:	57                   	push   %edi
80103174:	56                   	push   %esi
80103175:	53                   	push   %ebx
80103176:	83 ec 1c             	sub    $0x1c,%esp
80103179:	8b 75 08             	mov    0x8(%ebp),%esi
8010317c:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  struct pipe *p;

  p = 0;
  *f0 = *f1 = 0;
8010317f:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)
80103185:	c7 06 00 00 00 00    	movl   $0x0,(%esi)
  if((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
8010318b:	e8 e0 db ff ff       	call   80100d70 <filealloc>
80103190:	85 c0                	test   %eax,%eax
80103192:	89 06                	mov    %eax,(%esi)
80103194:	0f 84 a4 00 00 00    	je     8010323e <pipealloc+0xce>
8010319a:	e8 d1 db ff ff       	call   80100d70 <filealloc>
8010319f:	85 c0                	test   %eax,%eax
801031a1:	89 03                	mov    %eax,(%ebx)
801031a3:	0f 84 87 00 00 00    	je     80103230 <pipealloc+0xc0>
    goto bad;
  if((p = (struct pipe*)kalloc()) == 0)
801031a9:	e8 f2 f2 ff ff       	call   801024a0 <kalloc>
801031ae:	85 c0                	test   %eax,%eax
801031b0:	89 c7                	mov    %eax,%edi
801031b2:	74 7c                	je     80103230 <pipealloc+0xc0>
    goto bad;
  p->readopen = 1;
801031b4:	c7 80 3c 02 00 00 01 	movl   $0x1,0x23c(%eax)
801031bb:	00 00 00 
  p->writeopen = 1;
801031be:	c7 80 40 02 00 00 01 	movl   $0x1,0x240(%eax)
801031c5:	00 00 00 
  p->nwrite = 0;
801031c8:	c7 80 38 02 00 00 00 	movl   $0x0,0x238(%eax)
801031cf:	00 00 00 
  p->nread = 0;
801031d2:	c7 80 34 02 00 00 00 	movl   $0x0,0x234(%eax)
801031d9:	00 00 00 
  initlock(&p->lock, "pipe");
801031dc:	89 04 24             	mov    %eax,(%esp)
801031df:	c7 44 24 04 90 71 10 	movl   $0x80107190,0x4(%esp)
801031e6:	80 
801031e7:	e8 84 0e 00 00       	call   80104070 <initlock>
  (*f0)->type = FD_PIPE;
801031ec:	8b 06                	mov    (%esi),%eax
801031ee:	c7 00 01 00 00 00    	movl   $0x1,(%eax)
  (*f0)->readable = 1;
801031f4:	8b 06                	mov    (%esi),%eax
801031f6:	c6 40 08 01          	movb   $0x1,0x8(%eax)
  (*f0)->writable = 0;
801031fa:	8b 06                	mov    (%esi),%eax
801031fc:	c6 40 09 00          	movb   $0x0,0x9(%eax)
  (*f0)->pipe = p;
80103200:	8b 06                	mov    (%esi),%eax
80103202:	89 78 0c             	mov    %edi,0xc(%eax)
  (*f1)->type = FD_PIPE;
80103205:	8b 03                	mov    (%ebx),%eax
80103207:	c7 00 01 00 00 00    	movl   $0x1,(%eax)
  (*f1)->readable = 0;
8010320d:	8b 03                	mov    (%ebx),%eax
8010320f:	c6 40 08 00          	movb   $0x0,0x8(%eax)
  (*f1)->writable = 1;
80103213:	8b 03                	mov    (%ebx),%eax
80103215:	c6 40 09 01          	movb   $0x1,0x9(%eax)
  (*f1)->pipe = p;
80103219:	8b 03                	mov    (%ebx),%eax
  return 0;
8010321b:	31 db                	xor    %ebx,%ebx
  (*f1)->pipe = p;
8010321d:	89 78 0c             	mov    %edi,0xc(%eax)
  if(*f0)
    fileclose(*f0);
  if(*f1)
    fileclose(*f1);
  return -1;
}
80103220:	83 c4 1c             	add    $0x1c,%esp
80103223:	89 d8                	mov    %ebx,%eax
80103225:	5b                   	pop    %ebx
80103226:	5e                   	pop    %esi
80103227:	5f                   	pop    %edi
80103228:	5d                   	pop    %ebp
80103229:	c3                   	ret    
8010322a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  if(*f0)
80103230:	8b 06                	mov    (%esi),%eax
80103232:	85 c0                	test   %eax,%eax
80103234:	74 08                	je     8010323e <pipealloc+0xce>
    fileclose(*f0);
80103236:	89 04 24             	mov    %eax,(%esp)
80103239:	e8 f2 db ff ff       	call   80100e30 <fileclose>
  if(*f1)
8010323e:	8b 03                	mov    (%ebx),%eax
  return -1;
80103240:	bb ff ff ff ff       	mov    $0xffffffff,%ebx
  if(*f1)
80103245:	85 c0                	test   %eax,%eax
80103247:	74 d7                	je     80103220 <pipealloc+0xb0>
    fileclose(*f1);
80103249:	89 04 24             	mov    %eax,(%esp)
8010324c:	e8 df db ff ff       	call   80100e30 <fileclose>
}
80103251:	83 c4 1c             	add    $0x1c,%esp
80103254:	89 d8                	mov    %ebx,%eax
80103256:	5b                   	pop    %ebx
80103257:	5e                   	pop    %esi
80103258:	5f                   	pop    %edi
80103259:	5d                   	pop    %ebp
8010325a:	c3                   	ret    
8010325b:	90                   	nop
8010325c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80103260 <pipeclose>:

void
pipeclose(struct pipe *p, int writable)
{
80103260:	55                   	push   %ebp
80103261:	89 e5                	mov    %esp,%ebp
80103263:	56                   	push   %esi
80103264:	53                   	push   %ebx
80103265:	83 ec 10             	sub    $0x10,%esp
80103268:	8b 5d 08             	mov    0x8(%ebp),%ebx
8010326b:	8b 75 0c             	mov    0xc(%ebp),%esi
  acquire(&p->lock);
8010326e:	89 1c 24             	mov    %ebx,(%esp)
80103271:	e8 6a 0f 00 00       	call   801041e0 <acquire>
  if(writable){
80103276:	85 f6                	test   %esi,%esi
80103278:	74 3e                	je     801032b8 <pipeclose+0x58>
    p->writeopen = 0;
    wakeup(&p->nread);
8010327a:	8d 83 34 02 00 00    	lea    0x234(%ebx),%eax
    p->writeopen = 0;
80103280:	c7 83 40 02 00 00 00 	movl   $0x0,0x240(%ebx)
80103287:	00 00 00 
    wakeup(&p->nread);
8010328a:	89 04 24             	mov    %eax,(%esp)
8010328d:	e8 fe 0a 00 00       	call   80103d90 <wakeup>
  } else {
    p->readopen = 0;
    wakeup(&p->nwrite);
  }
  if(p->readopen == 0 && p->writeopen == 0){
80103292:	8b 93 3c 02 00 00    	mov    0x23c(%ebx),%edx
80103298:	85 d2                	test   %edx,%edx
8010329a:	75 0a                	jne    801032a6 <pipeclose+0x46>
8010329c:	8b 83 40 02 00 00    	mov    0x240(%ebx),%eax
801032a2:	85 c0                	test   %eax,%eax
801032a4:	74 32                	je     801032d8 <pipeclose+0x78>
    release(&p->lock);
    kfree((char*)p);
  } else
    release(&p->lock);
801032a6:	89 5d 08             	mov    %ebx,0x8(%ebp)
}
801032a9:	83 c4 10             	add    $0x10,%esp
801032ac:	5b                   	pop    %ebx
801032ad:	5e                   	pop    %esi
801032ae:	5d                   	pop    %ebp
    release(&p->lock);
801032af:	e9 9c 0f 00 00       	jmp    80104250 <release>
801032b4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    wakeup(&p->nwrite);
801032b8:	8d 83 38 02 00 00    	lea    0x238(%ebx),%eax
    p->readopen = 0;
801032be:	c7 83 3c 02 00 00 00 	movl   $0x0,0x23c(%ebx)
801032c5:	00 00 00 
    wakeup(&p->nwrite);
801032c8:	89 04 24             	mov    %eax,(%esp)
801032cb:	e8 c0 0a 00 00       	call   80103d90 <wakeup>
801032d0:	eb c0                	jmp    80103292 <pipeclose+0x32>
801032d2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    release(&p->lock);
801032d8:	89 1c 24             	mov    %ebx,(%esp)
801032db:	e8 70 0f 00 00       	call   80104250 <release>
    kfree((char*)p);
801032e0:	89 5d 08             	mov    %ebx,0x8(%ebp)
}
801032e3:	83 c4 10             	add    $0x10,%esp
801032e6:	5b                   	pop    %ebx
801032e7:	5e                   	pop    %esi
801032e8:	5d                   	pop    %ebp
    kfree((char*)p);
801032e9:	e9 02 f0 ff ff       	jmp    801022f0 <kfree>
801032ee:	66 90                	xchg   %ax,%ax

801032f0 <pipewrite>:

//PAGEBREAK: 40
int
pipewrite(struct pipe *p, char *addr, int n)
{
801032f0:	55                   	push   %ebp
801032f1:	89 e5                	mov    %esp,%ebp
801032f3:	57                   	push   %edi
801032f4:	56                   	push   %esi
801032f5:	53                   	push   %ebx
801032f6:	83 ec 1c             	sub    $0x1c,%esp
801032f9:	8b 5d 08             	mov    0x8(%ebp),%ebx
  int i;

  acquire(&p->lock);
801032fc:	89 1c 24             	mov    %ebx,(%esp)
801032ff:	e8 dc 0e 00 00       	call   801041e0 <acquire>
  for(i = 0; i < n; i++){
80103304:	8b 4d 10             	mov    0x10(%ebp),%ecx
80103307:	85 c9                	test   %ecx,%ecx
80103309:	0f 8e b2 00 00 00    	jle    801033c1 <pipewrite+0xd1>
8010330f:	8b 4d 0c             	mov    0xc(%ebp),%ecx
    while(p->nwrite == p->nread + PIPESIZE){  //DOC: pipewrite-full
      if(p->readopen == 0 || myproc()->killed){
        release(&p->lock);
        return -1;
      }
      wakeup(&p->nread);
80103312:	8d bb 34 02 00 00    	lea    0x234(%ebx),%edi
80103318:	8b 83 38 02 00 00    	mov    0x238(%ebx),%eax
      sleep(&p->nwrite, &p->lock);  //DOC: pipewrite-sleep
8010331e:	8d b3 38 02 00 00    	lea    0x238(%ebx),%esi
80103324:	89 4d e4             	mov    %ecx,-0x1c(%ebp)
80103327:	03 4d 10             	add    0x10(%ebp),%ecx
8010332a:	89 4d e0             	mov    %ecx,-0x20(%ebp)
    while(p->nwrite == p->nread + PIPESIZE){  //DOC: pipewrite-full
8010332d:	8b 8b 34 02 00 00    	mov    0x234(%ebx),%ecx
80103333:	81 c1 00 02 00 00    	add    $0x200,%ecx
80103339:	39 c8                	cmp    %ecx,%eax
8010333b:	74 38                	je     80103375 <pipewrite+0x85>
8010333d:	eb 55                	jmp    80103394 <pipewrite+0xa4>
8010333f:	90                   	nop
      if(p->readopen == 0 || myproc()->killed){
80103340:	e8 5b 03 00 00       	call   801036a0 <myproc>
80103345:	8b 40 24             	mov    0x24(%eax),%eax
80103348:	85 c0                	test   %eax,%eax
8010334a:	75 33                	jne    8010337f <pipewrite+0x8f>
      wakeup(&p->nread);
8010334c:	89 3c 24             	mov    %edi,(%esp)
8010334f:	e8 3c 0a 00 00       	call   80103d90 <wakeup>
      sleep(&p->nwrite, &p->lock);  //DOC: pipewrite-sleep
80103354:	89 5c 24 04          	mov    %ebx,0x4(%esp)
80103358:	89 34 24             	mov    %esi,(%esp)
8010335b:	e8 a0 08 00 00       	call   80103c00 <sleep>
    while(p->nwrite == p->nread + PIPESIZE){  //DOC: pipewrite-full
80103360:	8b 83 34 02 00 00    	mov    0x234(%ebx),%eax
80103366:	8b 93 38 02 00 00    	mov    0x238(%ebx),%edx
8010336c:	05 00 02 00 00       	add    $0x200,%eax
80103371:	39 c2                	cmp    %eax,%edx
80103373:	75 23                	jne    80103398 <pipewrite+0xa8>
      if(p->readopen == 0 || myproc()->killed){
80103375:	8b 93 3c 02 00 00    	mov    0x23c(%ebx),%edx
8010337b:	85 d2                	test   %edx,%edx
8010337d:	75 c1                	jne    80103340 <pipewrite+0x50>
        release(&p->lock);
8010337f:	89 1c 24             	mov    %ebx,(%esp)
80103382:	e8 c9 0e 00 00       	call   80104250 <release>
        return -1;
80103387:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
    p->data[p->nwrite++ % PIPESIZE] = addr[i];
  }
  wakeup(&p->nread);  //DOC: pipewrite-wakeup1
  release(&p->lock);
  return n;
}
8010338c:	83 c4 1c             	add    $0x1c,%esp
8010338f:	5b                   	pop    %ebx
80103390:	5e                   	pop    %esi
80103391:	5f                   	pop    %edi
80103392:	5d                   	pop    %ebp
80103393:	c3                   	ret    
    while(p->nwrite == p->nread + PIPESIZE){  //DOC: pipewrite-full
80103394:	89 c2                	mov    %eax,%edx
80103396:	66 90                	xchg   %ax,%ax
    p->data[p->nwrite++ % PIPESIZE] = addr[i];
80103398:	8b 4d e4             	mov    -0x1c(%ebp),%ecx
8010339b:	8d 42 01             	lea    0x1(%edx),%eax
8010339e:	81 e2 ff 01 00 00    	and    $0x1ff,%edx
801033a4:	89 83 38 02 00 00    	mov    %eax,0x238(%ebx)
801033aa:	83 45 e4 01          	addl   $0x1,-0x1c(%ebp)
801033ae:	0f b6 09             	movzbl (%ecx),%ecx
801033b1:	88 4c 13 34          	mov    %cl,0x34(%ebx,%edx,1)
  for(i = 0; i < n; i++){
801033b5:	8b 4d e4             	mov    -0x1c(%ebp),%ecx
801033b8:	3b 4d e0             	cmp    -0x20(%ebp),%ecx
801033bb:	0f 85 6c ff ff ff    	jne    8010332d <pipewrite+0x3d>
  wakeup(&p->nread);  //DOC: pipewrite-wakeup1
801033c1:	8d 83 34 02 00 00    	lea    0x234(%ebx),%eax
801033c7:	89 04 24             	mov    %eax,(%esp)
801033ca:	e8 c1 09 00 00       	call   80103d90 <wakeup>
  release(&p->lock);
801033cf:	89 1c 24             	mov    %ebx,(%esp)
801033d2:	e8 79 0e 00 00       	call   80104250 <release>
  return n;
801033d7:	8b 45 10             	mov    0x10(%ebp),%eax
801033da:	eb b0                	jmp    8010338c <pipewrite+0x9c>
801033dc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

801033e0 <piperead>:

int
piperead(struct pipe *p, char *addr, int n)
{
801033e0:	55                   	push   %ebp
801033e1:	89 e5                	mov    %esp,%ebp
801033e3:	57                   	push   %edi
801033e4:	56                   	push   %esi
801033e5:	53                   	push   %ebx
801033e6:	83 ec 1c             	sub    $0x1c,%esp
801033e9:	8b 75 08             	mov    0x8(%ebp),%esi
801033ec:	8b 7d 0c             	mov    0xc(%ebp),%edi
  int i;

  acquire(&p->lock);
801033ef:	89 34 24             	mov    %esi,(%esp)
801033f2:	e8 e9 0d 00 00       	call   801041e0 <acquire>
  while(p->nread == p->nwrite && p->writeopen){  //DOC: pipe-empty
801033f7:	8b 86 34 02 00 00    	mov    0x234(%esi),%eax
801033fd:	3b 86 38 02 00 00    	cmp    0x238(%esi),%eax
80103403:	75 5b                	jne    80103460 <piperead+0x80>
80103405:	8b 9e 40 02 00 00    	mov    0x240(%esi),%ebx
8010340b:	85 db                	test   %ebx,%ebx
8010340d:	74 51                	je     80103460 <piperead+0x80>
    if(myproc()->killed){
      release(&p->lock);
      return -1;
    }
    sleep(&p->nread, &p->lock); //DOC: piperead-sleep
8010340f:	8d 9e 34 02 00 00    	lea    0x234(%esi),%ebx
80103415:	eb 25                	jmp    8010343c <piperead+0x5c>
80103417:	90                   	nop
80103418:	89 74 24 04          	mov    %esi,0x4(%esp)
8010341c:	89 1c 24             	mov    %ebx,(%esp)
8010341f:	e8 dc 07 00 00       	call   80103c00 <sleep>
  while(p->nread == p->nwrite && p->writeopen){  //DOC: pipe-empty
80103424:	8b 86 34 02 00 00    	mov    0x234(%esi),%eax
8010342a:	3b 86 38 02 00 00    	cmp    0x238(%esi),%eax
80103430:	75 2e                	jne    80103460 <piperead+0x80>
80103432:	8b 96 40 02 00 00    	mov    0x240(%esi),%edx
80103438:	85 d2                	test   %edx,%edx
8010343a:	74 24                	je     80103460 <piperead+0x80>
    if(myproc()->killed){
8010343c:	e8 5f 02 00 00       	call   801036a0 <myproc>
80103441:	8b 48 24             	mov    0x24(%eax),%ecx
80103444:	85 c9                	test   %ecx,%ecx
80103446:	74 d0                	je     80103418 <piperead+0x38>
      release(&p->lock);
80103448:	89 34 24             	mov    %esi,(%esp)
8010344b:	e8 00 0e 00 00       	call   80104250 <release>
    addr[i] = p->data[p->nread++ % PIPESIZE];
  }
  wakeup(&p->nwrite);  //DOC: piperead-wakeup
  release(&p->lock);
  return i;
}
80103450:	83 c4 1c             	add    $0x1c,%esp
      return -1;
80103453:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80103458:	5b                   	pop    %ebx
80103459:	5e                   	pop    %esi
8010345a:	5f                   	pop    %edi
8010345b:	5d                   	pop    %ebp
8010345c:	c3                   	ret    
8010345d:	8d 76 00             	lea    0x0(%esi),%esi
  for(i = 0; i < n; i++){  //DOC: piperead-copy
80103460:	8b 55 10             	mov    0x10(%ebp),%edx
    if(p->nread == p->nwrite)
80103463:	31 db                	xor    %ebx,%ebx
  for(i = 0; i < n; i++){  //DOC: piperead-copy
80103465:	85 d2                	test   %edx,%edx
80103467:	7f 2b                	jg     80103494 <piperead+0xb4>
80103469:	eb 31                	jmp    8010349c <piperead+0xbc>
8010346b:	90                   	nop
8010346c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    addr[i] = p->data[p->nread++ % PIPESIZE];
80103470:	8d 48 01             	lea    0x1(%eax),%ecx
80103473:	25 ff 01 00 00       	and    $0x1ff,%eax
80103478:	89 8e 34 02 00 00    	mov    %ecx,0x234(%esi)
8010347e:	0f b6 44 06 34       	movzbl 0x34(%esi,%eax,1),%eax
80103483:	88 04 1f             	mov    %al,(%edi,%ebx,1)
  for(i = 0; i < n; i++){  //DOC: piperead-copy
80103486:	83 c3 01             	add    $0x1,%ebx
80103489:	3b 5d 10             	cmp    0x10(%ebp),%ebx
8010348c:	74 0e                	je     8010349c <piperead+0xbc>
    if(p->nread == p->nwrite)
8010348e:	8b 86 34 02 00 00    	mov    0x234(%esi),%eax
80103494:	3b 86 38 02 00 00    	cmp    0x238(%esi),%eax
8010349a:	75 d4                	jne    80103470 <piperead+0x90>
  wakeup(&p->nwrite);  //DOC: piperead-wakeup
8010349c:	8d 86 38 02 00 00    	lea    0x238(%esi),%eax
801034a2:	89 04 24             	mov    %eax,(%esp)
801034a5:	e8 e6 08 00 00       	call   80103d90 <wakeup>
  release(&p->lock);
801034aa:	89 34 24             	mov    %esi,(%esp)
801034ad:	e8 9e 0d 00 00       	call   80104250 <release>
}
801034b2:	83 c4 1c             	add    $0x1c,%esp
  return i;
801034b5:	89 d8                	mov    %ebx,%eax
}
801034b7:	5b                   	pop    %ebx
801034b8:	5e                   	pop    %esi
801034b9:	5f                   	pop    %edi
801034ba:	5d                   	pop    %ebp
801034bb:	c3                   	ret    
801034bc:	66 90                	xchg   %ax,%ax
801034be:	66 90                	xchg   %ax,%ax

801034c0 <allocproc>:
//  If found, change state to EMBRYO and initialize
//  state required to run in the kernel.
//  Otherwise return 0.
static struct proc *
allocproc(void)
{
801034c0:	55                   	push   %ebp
801034c1:	89 e5                	mov    %esp,%ebp
801034c3:	53                   	push   %ebx
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);

  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
801034c4:	bb 54 2d 11 80       	mov    $0x80112d54,%ebx
{
801034c9:	83 ec 14             	sub    $0x14,%esp
  acquire(&ptable.lock);
801034cc:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
801034d3:	e8 08 0d 00 00       	call   801041e0 <acquire>
801034d8:	eb 11                	jmp    801034eb <allocproc+0x2b>
801034da:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
801034e0:	83 eb 80             	sub    $0xffffff80,%ebx
801034e3:	81 fb 54 4d 11 80    	cmp    $0x80114d54,%ebx
801034e9:	74 7d                	je     80103568 <allocproc+0xa8>
    if (p->state == UNUSED)
801034eb:	8b 43 0c             	mov    0xc(%ebx),%eax
801034ee:	85 c0                	test   %eax,%eax
801034f0:	75 ee                	jne    801034e0 <allocproc+0x20>
  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;
801034f2:	a1 04 a0 10 80       	mov    0x8010a004,%eax

  release(&ptable.lock);
801034f7:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
  p->state = EMBRYO;
801034fe:	c7 43 0c 01 00 00 00 	movl   $0x1,0xc(%ebx)
  p->pid = nextpid++;
80103505:	8d 50 01             	lea    0x1(%eax),%edx
80103508:	89 15 04 a0 10 80    	mov    %edx,0x8010a004
8010350e:	89 43 10             	mov    %eax,0x10(%ebx)
  release(&ptable.lock);
80103511:	e8 3a 0d 00 00       	call   80104250 <release>

  // Allocate kernel stack.
  if ((p->kstack = kalloc()) == 0)
80103516:	e8 85 ef ff ff       	call   801024a0 <kalloc>
8010351b:	85 c0                	test   %eax,%eax
8010351d:	89 43 08             	mov    %eax,0x8(%ebx)
80103520:	74 5a                	je     8010357c <allocproc+0xbc>
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;

  // Leave room for trap frame.
  sp -= sizeof *p->tf;
80103522:	8d 90 b4 0f 00 00    	lea    0xfb4(%eax),%edx
  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint *)sp = (uint)trapret;

  sp -= sizeof *p->context;
80103528:	05 9c 0f 00 00       	add    $0xf9c,%eax
  sp -= sizeof *p->tf;
8010352d:	89 53 18             	mov    %edx,0x18(%ebx)
  *(uint *)sp = (uint)trapret;
80103530:	c7 40 14 25 54 10 80 	movl   $0x80105425,0x14(%eax)
  p->context = (struct context *)sp;
  memset(p->context, 0, sizeof *p->context);
80103537:	c7 44 24 08 14 00 00 	movl   $0x14,0x8(%esp)
8010353e:	00 
8010353f:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
80103546:	00 
80103547:	89 04 24             	mov    %eax,(%esp)
  p->context = (struct context *)sp;
8010354a:	89 43 1c             	mov    %eax,0x1c(%ebx)
  memset(p->context, 0, sizeof *p->context);
8010354d:	e8 4e 0d 00 00       	call   801042a0 <memset>
  p->context->eip = (uint)forkret;
80103552:	8b 43 1c             	mov    0x1c(%ebx),%eax
80103555:	c7 40 10 90 35 10 80 	movl   $0x80103590,0x10(%eax)

  return p;
8010355c:	89 d8                	mov    %ebx,%eax
}
8010355e:	83 c4 14             	add    $0x14,%esp
80103561:	5b                   	pop    %ebx
80103562:	5d                   	pop    %ebp
80103563:	c3                   	ret    
80103564:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  release(&ptable.lock);
80103568:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
8010356f:	e8 dc 0c 00 00       	call   80104250 <release>
}
80103574:	83 c4 14             	add    $0x14,%esp
  return 0;
80103577:	31 c0                	xor    %eax,%eax
}
80103579:	5b                   	pop    %ebx
8010357a:	5d                   	pop    %ebp
8010357b:	c3                   	ret    
    p->state = UNUSED;
8010357c:	c7 43 0c 00 00 00 00 	movl   $0x0,0xc(%ebx)
    return 0;
80103583:	eb d9                	jmp    8010355e <allocproc+0x9e>
80103585:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80103589:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80103590 <forkret>:
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void forkret(void)
{
80103590:	55                   	push   %ebp
80103591:	89 e5                	mov    %esp,%ebp
80103593:	83 ec 18             	sub    $0x18,%esp
  static int first = 1;
  // Still holding ptable.lock from scheduler.
  release(&ptable.lock);
80103596:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
8010359d:	e8 ae 0c 00 00       	call   80104250 <release>

  if (first)
801035a2:	a1 00 a0 10 80       	mov    0x8010a000,%eax
801035a7:	85 c0                	test   %eax,%eax
801035a9:	75 05                	jne    801035b0 <forkret+0x20>
    iinit(ROOTDEV);
    initlog(ROOTDEV);
  }

  // Return to "caller", actually trapret (see allocproc).
}
801035ab:	c9                   	leave  
801035ac:	c3                   	ret    
801035ad:	8d 76 00             	lea    0x0(%esi),%esi
    iinit(ROOTDEV);
801035b0:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
    first = 0;
801035b7:	c7 05 00 a0 10 80 00 	movl   $0x0,0x8010a000
801035be:	00 00 00 
    iinit(ROOTDEV);
801035c1:	e8 aa de ff ff       	call   80101470 <iinit>
    initlog(ROOTDEV);
801035c6:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
801035cd:	e8 9e f4 ff ff       	call   80102a70 <initlog>
}
801035d2:	c9                   	leave  
801035d3:	c3                   	ret    
801035d4:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
801035da:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

801035e0 <pinit>:
{
801035e0:	55                   	push   %ebp
801035e1:	89 e5                	mov    %esp,%ebp
801035e3:	83 ec 18             	sub    $0x18,%esp
  initlock(&ptable.lock, "ptable");
801035e6:	c7 44 24 04 95 71 10 	movl   $0x80107195,0x4(%esp)
801035ed:	80 
801035ee:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
801035f5:	e8 76 0a 00 00       	call   80104070 <initlock>
}
801035fa:	c9                   	leave  
801035fb:	c3                   	ret    
801035fc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80103600 <mycpu>:
{
80103600:	55                   	push   %ebp
80103601:	89 e5                	mov    %esp,%ebp
80103603:	56                   	push   %esi
80103604:	53                   	push   %ebx
80103605:	83 ec 10             	sub    $0x10,%esp
  asm volatile("pushfl; popl %0" : "=r" (eflags));
80103608:	9c                   	pushf  
80103609:	58                   	pop    %eax
  if (readeflags() & FL_IF)
8010360a:	f6 c4 02             	test   $0x2,%ah
8010360d:	75 57                	jne    80103666 <mycpu+0x66>
  apicid = lapicid();
8010360f:	e8 4c f1 ff ff       	call   80102760 <lapicid>
  for (i = 0; i < ncpu; ++i)
80103614:	8b 35 00 2d 11 80    	mov    0x80112d00,%esi
8010361a:	85 f6                	test   %esi,%esi
8010361c:	7e 3c                	jle    8010365a <mycpu+0x5a>
    if (cpus[i].apicid == apicid)
8010361e:	0f b6 15 80 27 11 80 	movzbl 0x80112780,%edx
80103625:	39 c2                	cmp    %eax,%edx
80103627:	74 2d                	je     80103656 <mycpu+0x56>
80103629:	b9 30 28 11 80       	mov    $0x80112830,%ecx
  for (i = 0; i < ncpu; ++i)
8010362e:	31 d2                	xor    %edx,%edx
80103630:	83 c2 01             	add    $0x1,%edx
80103633:	39 f2                	cmp    %esi,%edx
80103635:	74 23                	je     8010365a <mycpu+0x5a>
    if (cpus[i].apicid == apicid)
80103637:	0f b6 19             	movzbl (%ecx),%ebx
8010363a:	81 c1 b0 00 00 00    	add    $0xb0,%ecx
80103640:	39 c3                	cmp    %eax,%ebx
80103642:	75 ec                	jne    80103630 <mycpu+0x30>
      return &cpus[i];
80103644:	69 c2 b0 00 00 00    	imul   $0xb0,%edx,%eax
}
8010364a:	83 c4 10             	add    $0x10,%esp
8010364d:	5b                   	pop    %ebx
8010364e:	5e                   	pop    %esi
8010364f:	5d                   	pop    %ebp
      return &cpus[i];
80103650:	05 80 27 11 80       	add    $0x80112780,%eax
}
80103655:	c3                   	ret    
  for (i = 0; i < ncpu; ++i)
80103656:	31 d2                	xor    %edx,%edx
80103658:	eb ea                	jmp    80103644 <mycpu+0x44>
  panic("unknown apicid\n");
8010365a:	c7 04 24 9c 71 10 80 	movl   $0x8010719c,(%esp)
80103661:	e8 fa cc ff ff       	call   80100360 <panic>
    panic("mycpu called with interrupts enabled\n");
80103666:	c7 04 24 78 72 10 80 	movl   $0x80107278,(%esp)
8010366d:	e8 ee cc ff ff       	call   80100360 <panic>
80103672:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80103679:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80103680 <cpuid>:
{
80103680:	55                   	push   %ebp
80103681:	89 e5                	mov    %esp,%ebp
80103683:	83 ec 08             	sub    $0x8,%esp
  return mycpu() - cpus;
80103686:	e8 75 ff ff ff       	call   80103600 <mycpu>
}
8010368b:	c9                   	leave  
  return mycpu() - cpus;
8010368c:	2d 80 27 11 80       	sub    $0x80112780,%eax
80103691:	c1 f8 04             	sar    $0x4,%eax
80103694:	69 c0 a3 8b 2e ba    	imul   $0xba2e8ba3,%eax,%eax
}
8010369a:	c3                   	ret    
8010369b:	90                   	nop
8010369c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

801036a0 <myproc>:
{
801036a0:	55                   	push   %ebp
801036a1:	89 e5                	mov    %esp,%ebp
801036a3:	53                   	push   %ebx
801036a4:	83 ec 04             	sub    $0x4,%esp
  pushcli();
801036a7:	e8 44 0a 00 00       	call   801040f0 <pushcli>
  c = mycpu();
801036ac:	e8 4f ff ff ff       	call   80103600 <mycpu>
  p = c->proc;
801036b1:	8b 98 ac 00 00 00    	mov    0xac(%eax),%ebx
  popcli();
801036b7:	e8 74 0a 00 00       	call   80104130 <popcli>
}
801036bc:	83 c4 04             	add    $0x4,%esp
801036bf:	89 d8                	mov    %ebx,%eax
801036c1:	5b                   	pop    %ebx
801036c2:	5d                   	pop    %ebp
801036c3:	c3                   	ret    
801036c4:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
801036ca:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

801036d0 <userinit>:
{
801036d0:	55                   	push   %ebp
801036d1:	89 e5                	mov    %esp,%ebp
801036d3:	53                   	push   %ebx
801036d4:	83 ec 14             	sub    $0x14,%esp
  p = allocproc();
801036d7:	e8 e4 fd ff ff       	call   801034c0 <allocproc>
801036dc:	89 c3                	mov    %eax,%ebx
  initproc = p;
801036de:	a3 b8 a5 10 80       	mov    %eax,0x8010a5b8
  if ((p->pgdir = setupkvm()) == 0)
801036e3:	e8 b8 32 00 00       	call   801069a0 <setupkvm>
801036e8:	85 c0                	test   %eax,%eax
801036ea:	89 43 04             	mov    %eax,0x4(%ebx)
801036ed:	0f 84 d4 00 00 00    	je     801037c7 <userinit+0xf7>
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
801036f3:	89 04 24             	mov    %eax,(%esp)
801036f6:	c7 44 24 08 2c 00 00 	movl   $0x2c,0x8(%esp)
801036fd:	00 
801036fe:	c7 44 24 04 60 a4 10 	movl   $0x8010a460,0x4(%esp)
80103705:	80 
80103706:	e8 c5 2f 00 00       	call   801066d0 <inituvm>
  p->sz = PGSIZE;
8010370b:	c7 03 00 10 00 00    	movl   $0x1000,(%ebx)
  memset(p->tf, 0, sizeof(*p->tf));
80103711:	c7 44 24 08 4c 00 00 	movl   $0x4c,0x8(%esp)
80103718:	00 
80103719:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
80103720:	00 
80103721:	8b 43 18             	mov    0x18(%ebx),%eax
80103724:	89 04 24             	mov    %eax,(%esp)
80103727:	e8 74 0b 00 00       	call   801042a0 <memset>
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
8010372c:	8b 43 18             	mov    0x18(%ebx),%eax
8010372f:	ba 1b 00 00 00       	mov    $0x1b,%edx
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
80103734:	b9 23 00 00 00       	mov    $0x23,%ecx
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
80103739:	66 89 50 3c          	mov    %dx,0x3c(%eax)
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
8010373d:	8b 43 18             	mov    0x18(%ebx),%eax
80103740:	66 89 48 2c          	mov    %cx,0x2c(%eax)
  p->tf->es = p->tf->ds;
80103744:	8b 43 18             	mov    0x18(%ebx),%eax
80103747:	0f b7 50 2c          	movzwl 0x2c(%eax),%edx
8010374b:	66 89 50 28          	mov    %dx,0x28(%eax)
  p->tf->ss = p->tf->ds;
8010374f:	8b 43 18             	mov    0x18(%ebx),%eax
80103752:	0f b7 50 2c          	movzwl 0x2c(%eax),%edx
80103756:	66 89 50 48          	mov    %dx,0x48(%eax)
  p->tf->eflags = FL_IF;
8010375a:	8b 43 18             	mov    0x18(%ebx),%eax
8010375d:	c7 40 40 00 02 00 00 	movl   $0x200,0x40(%eax)
  p->tf->esp = PGSIZE;
80103764:	8b 43 18             	mov    0x18(%ebx),%eax
80103767:	c7 40 44 00 10 00 00 	movl   $0x1000,0x44(%eax)
  p->tf->eip = 0; // beginning of initcode.S
8010376e:	8b 43 18             	mov    0x18(%ebx),%eax
80103771:	c7 40 38 00 00 00 00 	movl   $0x0,0x38(%eax)
  safestrcpy(p->name, "initcode", sizeof(p->name));
80103778:	8d 43 6c             	lea    0x6c(%ebx),%eax
8010377b:	c7 44 24 08 10 00 00 	movl   $0x10,0x8(%esp)
80103782:	00 
80103783:	c7 44 24 04 c5 71 10 	movl   $0x801071c5,0x4(%esp)
8010378a:	80 
8010378b:	89 04 24             	mov    %eax,(%esp)
8010378e:	e8 ed 0c 00 00       	call   80104480 <safestrcpy>
  p->cwd = namei("/");
80103793:	c7 04 24 ce 71 10 80 	movl   $0x801071ce,(%esp)
8010379a:	e8 61 e7 ff ff       	call   80101f00 <namei>
8010379f:	89 43 68             	mov    %eax,0x68(%ebx)
  acquire(&ptable.lock);
801037a2:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
801037a9:	e8 32 0a 00 00       	call   801041e0 <acquire>
  p->state = RUNNABLE;
801037ae:	c7 43 0c 03 00 00 00 	movl   $0x3,0xc(%ebx)
  release(&ptable.lock);
801037b5:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
801037bc:	e8 8f 0a 00 00       	call   80104250 <release>
}
801037c1:	83 c4 14             	add    $0x14,%esp
801037c4:	5b                   	pop    %ebx
801037c5:	5d                   	pop    %ebp
801037c6:	c3                   	ret    
    panic("userinit: out of memory?");
801037c7:	c7 04 24 ac 71 10 80 	movl   $0x801071ac,(%esp)
801037ce:	e8 8d cb ff ff       	call   80100360 <panic>
801037d3:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
801037d9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801037e0 <growproc>:
{
801037e0:	55                   	push   %ebp
801037e1:	89 e5                	mov    %esp,%ebp
801037e3:	56                   	push   %esi
801037e4:	53                   	push   %ebx
801037e5:	83 ec 10             	sub    $0x10,%esp
801037e8:	8b 75 08             	mov    0x8(%ebp),%esi
  struct proc *curproc = myproc();
801037eb:	e8 b0 fe ff ff       	call   801036a0 <myproc>
  if (n > 0)
801037f0:	83 fe 00             	cmp    $0x0,%esi
  struct proc *curproc = myproc();
801037f3:	89 c3                	mov    %eax,%ebx
  sz = curproc->sz;
801037f5:	8b 00                	mov    (%eax),%eax
  if (n > 0)
801037f7:	7e 2f                	jle    80103828 <growproc+0x48>
    if ((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
801037f9:	01 c6                	add    %eax,%esi
801037fb:	89 74 24 08          	mov    %esi,0x8(%esp)
801037ff:	89 44 24 04          	mov    %eax,0x4(%esp)
80103803:	8b 43 04             	mov    0x4(%ebx),%eax
80103806:	89 04 24             	mov    %eax,(%esp)
80103809:	e8 02 30 00 00       	call   80106810 <allocuvm>
8010380e:	85 c0                	test   %eax,%eax
80103810:	74 36                	je     80103848 <growproc+0x68>
  curproc->sz = sz;
80103812:	89 03                	mov    %eax,(%ebx)
  switchuvm(curproc);
80103814:	89 1c 24             	mov    %ebx,(%esp)
80103817:	e8 a4 2d 00 00       	call   801065c0 <switchuvm>
  return 0;
8010381c:	31 c0                	xor    %eax,%eax
}
8010381e:	83 c4 10             	add    $0x10,%esp
80103821:	5b                   	pop    %ebx
80103822:	5e                   	pop    %esi
80103823:	5d                   	pop    %ebp
80103824:	c3                   	ret    
80103825:	8d 76 00             	lea    0x0(%esi),%esi
  else if (n < 0)
80103828:	74 e8                	je     80103812 <growproc+0x32>
    if ((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
8010382a:	01 c6                	add    %eax,%esi
8010382c:	89 74 24 08          	mov    %esi,0x8(%esp)
80103830:	89 44 24 04          	mov    %eax,0x4(%esp)
80103834:	8b 43 04             	mov    0x4(%ebx),%eax
80103837:	89 04 24             	mov    %eax,(%esp)
8010383a:	e8 c1 30 00 00       	call   80106900 <deallocuvm>
8010383f:	85 c0                	test   %eax,%eax
80103841:	75 cf                	jne    80103812 <growproc+0x32>
80103843:	90                   	nop
80103844:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      return -1;
80103848:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
8010384d:	eb cf                	jmp    8010381e <growproc+0x3e>
8010384f:	90                   	nop

80103850 <fork>:
{
80103850:	55                   	push   %ebp
80103851:	89 e5                	mov    %esp,%ebp
80103853:	57                   	push   %edi
80103854:	56                   	push   %esi
80103855:	53                   	push   %ebx
80103856:	83 ec 1c             	sub    $0x1c,%esp
  struct proc *curproc = myproc();
80103859:	e8 42 fe ff ff       	call   801036a0 <myproc>
8010385e:	89 c3                	mov    %eax,%ebx
  if ((np = allocproc()) == 0)
80103860:	e8 5b fc ff ff       	call   801034c0 <allocproc>
80103865:	85 c0                	test   %eax,%eax
80103867:	89 c7                	mov    %eax,%edi
80103869:	89 45 e4             	mov    %eax,-0x1c(%ebp)
8010386c:	0f 84 bc 00 00 00    	je     8010392e <fork+0xde>
  if ((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0)
80103872:	8b 03                	mov    (%ebx),%eax
80103874:	89 44 24 04          	mov    %eax,0x4(%esp)
80103878:	8b 43 04             	mov    0x4(%ebx),%eax
8010387b:	89 04 24             	mov    %eax,(%esp)
8010387e:	e8 fd 31 00 00       	call   80106a80 <copyuvm>
80103883:	85 c0                	test   %eax,%eax
80103885:	89 47 04             	mov    %eax,0x4(%edi)
80103888:	0f 84 a7 00 00 00    	je     80103935 <fork+0xe5>
  np->sz = curproc->sz;
8010388e:	8b 03                	mov    (%ebx),%eax
80103890:	8b 4d e4             	mov    -0x1c(%ebp),%ecx
80103893:	89 01                	mov    %eax,(%ecx)
  *np->tf = *curproc->tf;
80103895:	8b 79 18             	mov    0x18(%ecx),%edi
80103898:	89 c8                	mov    %ecx,%eax
  np->parent = curproc;
8010389a:	89 59 14             	mov    %ebx,0x14(%ecx)
  *np->tf = *curproc->tf;
8010389d:	8b 73 18             	mov    0x18(%ebx),%esi
801038a0:	b9 13 00 00 00       	mov    $0x13,%ecx
801038a5:	f3 a5                	rep movsl %ds:(%esi),%es:(%edi)
  for (i = 0; i < NOFILE; i++)
801038a7:	31 f6                	xor    %esi,%esi
  np->tf->eax = 0;
801038a9:	8b 40 18             	mov    0x18(%eax),%eax
801038ac:	c7 40 1c 00 00 00 00 	movl   $0x0,0x1c(%eax)
801038b3:	90                   	nop
801038b4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    if (curproc->ofile[i])
801038b8:	8b 44 b3 28          	mov    0x28(%ebx,%esi,4),%eax
801038bc:	85 c0                	test   %eax,%eax
801038be:	74 0f                	je     801038cf <fork+0x7f>
      np->ofile[i] = filedup(curproc->ofile[i]);
801038c0:	89 04 24             	mov    %eax,(%esp)
801038c3:	e8 18 d5 ff ff       	call   80100de0 <filedup>
801038c8:	8b 55 e4             	mov    -0x1c(%ebp),%edx
801038cb:	89 44 b2 28          	mov    %eax,0x28(%edx,%esi,4)
  for (i = 0; i < NOFILE; i++)
801038cf:	83 c6 01             	add    $0x1,%esi
801038d2:	83 fe 10             	cmp    $0x10,%esi
801038d5:	75 e1                	jne    801038b8 <fork+0x68>
  np->cwd = idup(curproc->cwd);
801038d7:	8b 43 68             	mov    0x68(%ebx),%eax
  safestrcpy(np->name, curproc->name, sizeof(curproc->name));
801038da:	83 c3 6c             	add    $0x6c,%ebx
  np->cwd = idup(curproc->cwd);
801038dd:	89 04 24             	mov    %eax,(%esp)
801038e0:	e8 9b dd ff ff       	call   80101680 <idup>
801038e5:	8b 7d e4             	mov    -0x1c(%ebp),%edi
801038e8:	89 47 68             	mov    %eax,0x68(%edi)
  safestrcpy(np->name, curproc->name, sizeof(curproc->name));
801038eb:	8d 47 6c             	lea    0x6c(%edi),%eax
801038ee:	89 5c 24 04          	mov    %ebx,0x4(%esp)
801038f2:	c7 44 24 08 10 00 00 	movl   $0x10,0x8(%esp)
801038f9:	00 
801038fa:	89 04 24             	mov    %eax,(%esp)
801038fd:	e8 7e 0b 00 00       	call   80104480 <safestrcpy>
  pid = np->pid;
80103902:	8b 5f 10             	mov    0x10(%edi),%ebx
  acquire(&ptable.lock);
80103905:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
8010390c:	e8 cf 08 00 00       	call   801041e0 <acquire>
  np->state = RUNNABLE;
80103911:	c7 47 0c 03 00 00 00 	movl   $0x3,0xc(%edi)
  release(&ptable.lock);
80103918:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
8010391f:	e8 2c 09 00 00       	call   80104250 <release>
  return pid;
80103924:	89 d8                	mov    %ebx,%eax
}
80103926:	83 c4 1c             	add    $0x1c,%esp
80103929:	5b                   	pop    %ebx
8010392a:	5e                   	pop    %esi
8010392b:	5f                   	pop    %edi
8010392c:	5d                   	pop    %ebp
8010392d:	c3                   	ret    
    return -1;
8010392e:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80103933:	eb f1                	jmp    80103926 <fork+0xd6>
    kfree(np->kstack);
80103935:	8b 7d e4             	mov    -0x1c(%ebp),%edi
80103938:	8b 47 08             	mov    0x8(%edi),%eax
8010393b:	89 04 24             	mov    %eax,(%esp)
8010393e:	e8 ad e9 ff ff       	call   801022f0 <kfree>
    return -1;
80103943:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
    np->kstack = 0;
80103948:	c7 47 08 00 00 00 00 	movl   $0x0,0x8(%edi)
    np->state = UNUSED;
8010394f:	c7 47 0c 00 00 00 00 	movl   $0x0,0xc(%edi)
    return -1;
80103956:	eb ce                	jmp    80103926 <fork+0xd6>
80103958:	90                   	nop
80103959:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

80103960 <scheduler>:
{
80103960:	55                   	push   %ebp
80103961:	89 e5                	mov    %esp,%ebp
80103963:	57                   	push   %edi
80103964:	56                   	push   %esi
80103965:	53                   	push   %ebx
80103966:	83 ec 1c             	sub    $0x1c,%esp
  struct cpu *c = mycpu();
80103969:	e8 92 fc ff ff       	call   80103600 <mycpu>
8010396e:	89 c6                	mov    %eax,%esi
  c->proc = 0;
80103970:	c7 80 ac 00 00 00 00 	movl   $0x0,0xac(%eax)
80103977:	00 00 00 
8010397a:	8d 78 04             	lea    0x4(%eax),%edi
8010397d:	8d 76 00             	lea    0x0(%esi),%esi
  asm volatile("sti");
80103980:	fb                   	sti    
    acquire(&ptable.lock);
80103981:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
80103988:	bb 54 2d 11 80       	mov    $0x80112d54,%ebx
    acquire(&ptable.lock);
8010398d:	e8 4e 08 00 00       	call   801041e0 <acquire>
80103992:	eb 0f                	jmp    801039a3 <scheduler+0x43>
80103994:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
80103998:	83 eb 80             	sub    $0xffffff80,%ebx
8010399b:	81 fb 54 4d 11 80    	cmp    $0x80114d54,%ebx
801039a1:	74 45                	je     801039e8 <scheduler+0x88>
      if (p->state != RUNNABLE)
801039a3:	83 7b 0c 03          	cmpl   $0x3,0xc(%ebx)
801039a7:	75 ef                	jne    80103998 <scheduler+0x38>
      c->proc = p;
801039a9:	89 9e ac 00 00 00    	mov    %ebx,0xac(%esi)
      switchuvm(p);
801039af:	89 1c 24             	mov    %ebx,(%esp)
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
801039b2:	83 eb 80             	sub    $0xffffff80,%ebx
      switchuvm(p);
801039b5:	e8 06 2c 00 00       	call   801065c0 <switchuvm>
      swtch(&(c->scheduler), p->context);
801039ba:	8b 43 9c             	mov    -0x64(%ebx),%eax
      p->state = RUNNING;
801039bd:	c7 43 8c 04 00 00 00 	movl   $0x4,-0x74(%ebx)
      swtch(&(c->scheduler), p->context);
801039c4:	89 3c 24             	mov    %edi,(%esp)
801039c7:	89 44 24 04          	mov    %eax,0x4(%esp)
801039cb:	e8 0b 0b 00 00       	call   801044db <swtch>
      switchkvm();
801039d0:	e8 cb 2b 00 00       	call   801065a0 <switchkvm>
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
801039d5:	81 fb 54 4d 11 80    	cmp    $0x80114d54,%ebx
      c->proc = 0;
801039db:	c7 86 ac 00 00 00 00 	movl   $0x0,0xac(%esi)
801039e2:	00 00 00 
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
801039e5:	75 bc                	jne    801039a3 <scheduler+0x43>
801039e7:	90                   	nop
    release(&ptable.lock);
801039e8:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
801039ef:	e8 5c 08 00 00       	call   80104250 <release>
  }
801039f4:	eb 8a                	jmp    80103980 <scheduler+0x20>
801039f6:	8d 76 00             	lea    0x0(%esi),%esi
801039f9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80103a00 <sched>:
{
80103a00:	55                   	push   %ebp
80103a01:	89 e5                	mov    %esp,%ebp
80103a03:	56                   	push   %esi
80103a04:	53                   	push   %ebx
80103a05:	83 ec 10             	sub    $0x10,%esp
  struct proc *p = myproc();
80103a08:	e8 93 fc ff ff       	call   801036a0 <myproc>
  if (!holding(&ptable.lock))
80103a0d:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
  struct proc *p = myproc();
80103a14:	89 c3                	mov    %eax,%ebx
  if (!holding(&ptable.lock))
80103a16:	e8 85 07 00 00       	call   801041a0 <holding>
80103a1b:	85 c0                	test   %eax,%eax
80103a1d:	74 4f                	je     80103a6e <sched+0x6e>
  if (mycpu()->ncli != 1)
80103a1f:	e8 dc fb ff ff       	call   80103600 <mycpu>
80103a24:	83 b8 a4 00 00 00 01 	cmpl   $0x1,0xa4(%eax)
80103a2b:	75 65                	jne    80103a92 <sched+0x92>
  if (p->state == RUNNING)
80103a2d:	83 7b 0c 04          	cmpl   $0x4,0xc(%ebx)
80103a31:	74 53                	je     80103a86 <sched+0x86>
  asm volatile("pushfl; popl %0" : "=r" (eflags));
80103a33:	9c                   	pushf  
80103a34:	58                   	pop    %eax
  if (readeflags() & FL_IF)
80103a35:	f6 c4 02             	test   $0x2,%ah
80103a38:	75 40                	jne    80103a7a <sched+0x7a>
  intena = mycpu()->intena;
80103a3a:	e8 c1 fb ff ff       	call   80103600 <mycpu>
  swtch(&p->context, mycpu()->scheduler);
80103a3f:	83 c3 1c             	add    $0x1c,%ebx
  intena = mycpu()->intena;
80103a42:	8b b0 a8 00 00 00    	mov    0xa8(%eax),%esi
  swtch(&p->context, mycpu()->scheduler);
80103a48:	e8 b3 fb ff ff       	call   80103600 <mycpu>
80103a4d:	8b 40 04             	mov    0x4(%eax),%eax
80103a50:	89 1c 24             	mov    %ebx,(%esp)
80103a53:	89 44 24 04          	mov    %eax,0x4(%esp)
80103a57:	e8 7f 0a 00 00       	call   801044db <swtch>
  mycpu()->intena = intena;
80103a5c:	e8 9f fb ff ff       	call   80103600 <mycpu>
80103a61:	89 b0 a8 00 00 00    	mov    %esi,0xa8(%eax)
}
80103a67:	83 c4 10             	add    $0x10,%esp
80103a6a:	5b                   	pop    %ebx
80103a6b:	5e                   	pop    %esi
80103a6c:	5d                   	pop    %ebp
80103a6d:	c3                   	ret    
    panic("sched ptable.lock");
80103a6e:	c7 04 24 d0 71 10 80 	movl   $0x801071d0,(%esp)
80103a75:	e8 e6 c8 ff ff       	call   80100360 <panic>
    panic("sched interruptible");
80103a7a:	c7 04 24 fc 71 10 80 	movl   $0x801071fc,(%esp)
80103a81:	e8 da c8 ff ff       	call   80100360 <panic>
    panic("sched running");
80103a86:	c7 04 24 ee 71 10 80 	movl   $0x801071ee,(%esp)
80103a8d:	e8 ce c8 ff ff       	call   80100360 <panic>
    panic("sched locks");
80103a92:	c7 04 24 e2 71 10 80 	movl   $0x801071e2,(%esp)
80103a99:	e8 c2 c8 ff ff       	call   80100360 <panic>
80103a9e:	66 90                	xchg   %ax,%ax

80103aa0 <exit>:
{
80103aa0:	55                   	push   %ebp
80103aa1:	89 e5                	mov    %esp,%ebp
80103aa3:	56                   	push   %esi
  if (curproc == initproc)
80103aa4:	31 f6                	xor    %esi,%esi
{
80103aa6:	53                   	push   %ebx
80103aa7:	83 ec 10             	sub    $0x10,%esp
  struct proc *curproc = myproc();
80103aaa:	e8 f1 fb ff ff       	call   801036a0 <myproc>
  if (curproc == initproc)
80103aaf:	3b 05 b8 a5 10 80    	cmp    0x8010a5b8,%eax
  struct proc *curproc = myproc();
80103ab5:	89 c3                	mov    %eax,%ebx
  if (curproc == initproc)
80103ab7:	0f 84 ea 00 00 00    	je     80103ba7 <exit+0x107>
80103abd:	8d 76 00             	lea    0x0(%esi),%esi
    if (curproc->ofile[fd])
80103ac0:	8b 44 b3 28          	mov    0x28(%ebx,%esi,4),%eax
80103ac4:	85 c0                	test   %eax,%eax
80103ac6:	74 10                	je     80103ad8 <exit+0x38>
      fileclose(curproc->ofile[fd]);
80103ac8:	89 04 24             	mov    %eax,(%esp)
80103acb:	e8 60 d3 ff ff       	call   80100e30 <fileclose>
      curproc->ofile[fd] = 0;
80103ad0:	c7 44 b3 28 00 00 00 	movl   $0x0,0x28(%ebx,%esi,4)
80103ad7:	00 
  for (fd = 0; fd < NOFILE; fd++)
80103ad8:	83 c6 01             	add    $0x1,%esi
80103adb:	83 fe 10             	cmp    $0x10,%esi
80103ade:	75 e0                	jne    80103ac0 <exit+0x20>
  begin_op();
80103ae0:	e8 2b f0 ff ff       	call   80102b10 <begin_op>
  iput(curproc->cwd);
80103ae5:	8b 43 68             	mov    0x68(%ebx),%eax
80103ae8:	89 04 24             	mov    %eax,(%esp)
80103aeb:	e8 e0 dc ff ff       	call   801017d0 <iput>
  end_op();
80103af0:	e8 8b f0 ff ff       	call   80102b80 <end_op>
  curproc->cwd = 0;
80103af5:	c7 43 68 00 00 00 00 	movl   $0x0,0x68(%ebx)
  acquire(&ptable.lock);
80103afc:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
80103b03:	e8 d8 06 00 00       	call   801041e0 <acquire>
  wakeup1(curproc->parent);
80103b08:	8b 43 14             	mov    0x14(%ebx),%eax
static void
wakeup1(void *chan)
{
  struct proc *p;

  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
80103b0b:	ba 54 2d 11 80       	mov    $0x80112d54,%edx
80103b10:	eb 11                	jmp    80103b23 <exit+0x83>
80103b12:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80103b18:	83 ea 80             	sub    $0xffffff80,%edx
80103b1b:	81 fa 54 4d 11 80    	cmp    $0x80114d54,%edx
80103b21:	74 1d                	je     80103b40 <exit+0xa0>
    if (p->state == SLEEPING && p->chan == chan)
80103b23:	83 7a 0c 02          	cmpl   $0x2,0xc(%edx)
80103b27:	75 ef                	jne    80103b18 <exit+0x78>
80103b29:	3b 42 20             	cmp    0x20(%edx),%eax
80103b2c:	75 ea                	jne    80103b18 <exit+0x78>
      p->state = RUNNABLE;
80103b2e:	c7 42 0c 03 00 00 00 	movl   $0x3,0xc(%edx)
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
80103b35:	83 ea 80             	sub    $0xffffff80,%edx
80103b38:	81 fa 54 4d 11 80    	cmp    $0x80114d54,%edx
80103b3e:	75 e3                	jne    80103b23 <exit+0x83>
      p->parent = initproc;
80103b40:	a1 b8 a5 10 80       	mov    0x8010a5b8,%eax
80103b45:	b9 54 2d 11 80       	mov    $0x80112d54,%ecx
80103b4a:	eb 0f                	jmp    80103b5b <exit+0xbb>
80103b4c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
80103b50:	83 e9 80             	sub    $0xffffff80,%ecx
80103b53:	81 f9 54 4d 11 80    	cmp    $0x80114d54,%ecx
80103b59:	74 34                	je     80103b8f <exit+0xef>
    if (p->parent == curproc)
80103b5b:	39 59 14             	cmp    %ebx,0x14(%ecx)
80103b5e:	75 f0                	jne    80103b50 <exit+0xb0>
      if (p->state == ZOMBIE)
80103b60:	83 79 0c 05          	cmpl   $0x5,0xc(%ecx)
      p->parent = initproc;
80103b64:	89 41 14             	mov    %eax,0x14(%ecx)
      if (p->state == ZOMBIE)
80103b67:	75 e7                	jne    80103b50 <exit+0xb0>
80103b69:	ba 54 2d 11 80       	mov    $0x80112d54,%edx
80103b6e:	eb 0b                	jmp    80103b7b <exit+0xdb>
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
80103b70:	83 ea 80             	sub    $0xffffff80,%edx
80103b73:	81 fa 54 4d 11 80    	cmp    $0x80114d54,%edx
80103b79:	74 d5                	je     80103b50 <exit+0xb0>
    if (p->state == SLEEPING && p->chan == chan)
80103b7b:	83 7a 0c 02          	cmpl   $0x2,0xc(%edx)
80103b7f:	75 ef                	jne    80103b70 <exit+0xd0>
80103b81:	3b 42 20             	cmp    0x20(%edx),%eax
80103b84:	75 ea                	jne    80103b70 <exit+0xd0>
      p->state = RUNNABLE;
80103b86:	c7 42 0c 03 00 00 00 	movl   $0x3,0xc(%edx)
80103b8d:	eb e1                	jmp    80103b70 <exit+0xd0>
  curproc->state = ZOMBIE;
80103b8f:	c7 43 0c 05 00 00 00 	movl   $0x5,0xc(%ebx)
  sched();
80103b96:	e8 65 fe ff ff       	call   80103a00 <sched>
  panic("zombie exit");
80103b9b:	c7 04 24 1d 72 10 80 	movl   $0x8010721d,(%esp)
80103ba2:	e8 b9 c7 ff ff       	call   80100360 <panic>
    panic("init exiting");
80103ba7:	c7 04 24 10 72 10 80 	movl   $0x80107210,(%esp)
80103bae:	e8 ad c7 ff ff       	call   80100360 <panic>
80103bb3:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80103bb9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80103bc0 <yield>:
{
80103bc0:	55                   	push   %ebp
80103bc1:	89 e5                	mov    %esp,%ebp
80103bc3:	83 ec 18             	sub    $0x18,%esp
  acquire(&ptable.lock); // DOC: yieldlock
80103bc6:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
80103bcd:	e8 0e 06 00 00       	call   801041e0 <acquire>
  myproc()->state = RUNNABLE;
80103bd2:	e8 c9 fa ff ff       	call   801036a0 <myproc>
80103bd7:	c7 40 0c 03 00 00 00 	movl   $0x3,0xc(%eax)
  sched();
80103bde:	e8 1d fe ff ff       	call   80103a00 <sched>
  release(&ptable.lock);
80103be3:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
80103bea:	e8 61 06 00 00       	call   80104250 <release>
}
80103bef:	c9                   	leave  
80103bf0:	c3                   	ret    
80103bf1:	eb 0d                	jmp    80103c00 <sleep>
80103bf3:	90                   	nop
80103bf4:	90                   	nop
80103bf5:	90                   	nop
80103bf6:	90                   	nop
80103bf7:	90                   	nop
80103bf8:	90                   	nop
80103bf9:	90                   	nop
80103bfa:	90                   	nop
80103bfb:	90                   	nop
80103bfc:	90                   	nop
80103bfd:	90                   	nop
80103bfe:	90                   	nop
80103bff:	90                   	nop

80103c00 <sleep>:
{
80103c00:	55                   	push   %ebp
80103c01:	89 e5                	mov    %esp,%ebp
80103c03:	57                   	push   %edi
80103c04:	56                   	push   %esi
80103c05:	53                   	push   %ebx
80103c06:	83 ec 1c             	sub    $0x1c,%esp
80103c09:	8b 7d 08             	mov    0x8(%ebp),%edi
80103c0c:	8b 75 0c             	mov    0xc(%ebp),%esi
  struct proc *p = myproc();
80103c0f:	e8 8c fa ff ff       	call   801036a0 <myproc>
  if (p == 0)
80103c14:	85 c0                	test   %eax,%eax
  struct proc *p = myproc();
80103c16:	89 c3                	mov    %eax,%ebx
  if (p == 0)
80103c18:	0f 84 7c 00 00 00    	je     80103c9a <sleep+0x9a>
  if (lk == 0)
80103c1e:	85 f6                	test   %esi,%esi
80103c20:	74 6c                	je     80103c8e <sleep+0x8e>
  if (lk != &ptable.lock)
80103c22:	81 fe 20 2d 11 80    	cmp    $0x80112d20,%esi
80103c28:	74 46                	je     80103c70 <sleep+0x70>
    acquire(&ptable.lock); // DOC: sleeplock1
80103c2a:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
80103c31:	e8 aa 05 00 00       	call   801041e0 <acquire>
    release(lk);
80103c36:	89 34 24             	mov    %esi,(%esp)
80103c39:	e8 12 06 00 00       	call   80104250 <release>
  p->chan = chan;
80103c3e:	89 7b 20             	mov    %edi,0x20(%ebx)
  p->state = SLEEPING;
80103c41:	c7 43 0c 02 00 00 00 	movl   $0x2,0xc(%ebx)
  sched();
80103c48:	e8 b3 fd ff ff       	call   80103a00 <sched>
  p->chan = 0;
80103c4d:	c7 43 20 00 00 00 00 	movl   $0x0,0x20(%ebx)
    release(&ptable.lock);
80103c54:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
80103c5b:	e8 f0 05 00 00       	call   80104250 <release>
    acquire(lk);
80103c60:	89 75 08             	mov    %esi,0x8(%ebp)
}
80103c63:	83 c4 1c             	add    $0x1c,%esp
80103c66:	5b                   	pop    %ebx
80103c67:	5e                   	pop    %esi
80103c68:	5f                   	pop    %edi
80103c69:	5d                   	pop    %ebp
    acquire(lk);
80103c6a:	e9 71 05 00 00       	jmp    801041e0 <acquire>
80103c6f:	90                   	nop
  p->chan = chan;
80103c70:	89 78 20             	mov    %edi,0x20(%eax)
  p->state = SLEEPING;
80103c73:	c7 40 0c 02 00 00 00 	movl   $0x2,0xc(%eax)
  sched();
80103c7a:	e8 81 fd ff ff       	call   80103a00 <sched>
  p->chan = 0;
80103c7f:	c7 43 20 00 00 00 00 	movl   $0x0,0x20(%ebx)
}
80103c86:	83 c4 1c             	add    $0x1c,%esp
80103c89:	5b                   	pop    %ebx
80103c8a:	5e                   	pop    %esi
80103c8b:	5f                   	pop    %edi
80103c8c:	5d                   	pop    %ebp
80103c8d:	c3                   	ret    
    panic("sleep without lk");
80103c8e:	c7 04 24 2f 72 10 80 	movl   $0x8010722f,(%esp)
80103c95:	e8 c6 c6 ff ff       	call   80100360 <panic>
    panic("sleep");
80103c9a:	c7 04 24 29 72 10 80 	movl   $0x80107229,(%esp)
80103ca1:	e8 ba c6 ff ff       	call   80100360 <panic>
80103ca6:	8d 76 00             	lea    0x0(%esi),%esi
80103ca9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80103cb0 <wait>:
{
80103cb0:	55                   	push   %ebp
80103cb1:	89 e5                	mov    %esp,%ebp
80103cb3:	56                   	push   %esi
80103cb4:	53                   	push   %ebx
80103cb5:	83 ec 10             	sub    $0x10,%esp
  struct proc *curproc = myproc();
80103cb8:	e8 e3 f9 ff ff       	call   801036a0 <myproc>
  acquire(&ptable.lock);
80103cbd:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
  struct proc *curproc = myproc();
80103cc4:	89 c6                	mov    %eax,%esi
  acquire(&ptable.lock);
80103cc6:	e8 15 05 00 00       	call   801041e0 <acquire>
    havekids = 0;
80103ccb:	31 c0                	xor    %eax,%eax
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
80103ccd:	bb 54 2d 11 80       	mov    $0x80112d54,%ebx
80103cd2:	eb 0f                	jmp    80103ce3 <wait+0x33>
80103cd4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80103cd8:	83 eb 80             	sub    $0xffffff80,%ebx
80103cdb:	81 fb 54 4d 11 80    	cmp    $0x80114d54,%ebx
80103ce1:	74 1d                	je     80103d00 <wait+0x50>
      if (p->parent != curproc)
80103ce3:	39 73 14             	cmp    %esi,0x14(%ebx)
80103ce6:	75 f0                	jne    80103cd8 <wait+0x28>
      if (p->state == ZOMBIE)
80103ce8:	83 7b 0c 05          	cmpl   $0x5,0xc(%ebx)
80103cec:	74 2f                	je     80103d1d <wait+0x6d>
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
80103cee:	83 eb 80             	sub    $0xffffff80,%ebx
      havekids = 1;
80103cf1:	b8 01 00 00 00       	mov    $0x1,%eax
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
80103cf6:	81 fb 54 4d 11 80    	cmp    $0x80114d54,%ebx
80103cfc:	75 e5                	jne    80103ce3 <wait+0x33>
80103cfe:	66 90                	xchg   %ax,%ax
    if (!havekids || curproc->killed)
80103d00:	85 c0                	test   %eax,%eax
80103d02:	74 6e                	je     80103d72 <wait+0xc2>
80103d04:	8b 46 24             	mov    0x24(%esi),%eax
80103d07:	85 c0                	test   %eax,%eax
80103d09:	75 67                	jne    80103d72 <wait+0xc2>
    sleep(curproc, &ptable.lock); // DOC: wait-sleep
80103d0b:	c7 44 24 04 20 2d 11 	movl   $0x80112d20,0x4(%esp)
80103d12:	80 
80103d13:	89 34 24             	mov    %esi,(%esp)
80103d16:	e8 e5 fe ff ff       	call   80103c00 <sleep>
  }
80103d1b:	eb ae                	jmp    80103ccb <wait+0x1b>
        kfree(p->kstack);
80103d1d:	8b 43 08             	mov    0x8(%ebx),%eax
        pid = p->pid;
80103d20:	8b 73 10             	mov    0x10(%ebx),%esi
        kfree(p->kstack);
80103d23:	89 04 24             	mov    %eax,(%esp)
80103d26:	e8 c5 e5 ff ff       	call   801022f0 <kfree>
        freevm(p->pgdir);
80103d2b:	8b 43 04             	mov    0x4(%ebx),%eax
        p->kstack = 0;
80103d2e:	c7 43 08 00 00 00 00 	movl   $0x0,0x8(%ebx)
        freevm(p->pgdir);
80103d35:	89 04 24             	mov    %eax,(%esp)
80103d38:	e8 e3 2b 00 00       	call   80106920 <freevm>
        release(&ptable.lock);
80103d3d:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
        p->pid = 0;
80103d44:	c7 43 10 00 00 00 00 	movl   $0x0,0x10(%ebx)
        p->parent = 0;
80103d4b:	c7 43 14 00 00 00 00 	movl   $0x0,0x14(%ebx)
        p->name[0] = 0;
80103d52:	c6 43 6c 00          	movb   $0x0,0x6c(%ebx)
        p->killed = 0;
80103d56:	c7 43 24 00 00 00 00 	movl   $0x0,0x24(%ebx)
        p->state = UNUSED;
80103d5d:	c7 43 0c 00 00 00 00 	movl   $0x0,0xc(%ebx)
        release(&ptable.lock);
80103d64:	e8 e7 04 00 00       	call   80104250 <release>
}
80103d69:	83 c4 10             	add    $0x10,%esp
        return pid;
80103d6c:	89 f0                	mov    %esi,%eax
}
80103d6e:	5b                   	pop    %ebx
80103d6f:	5e                   	pop    %esi
80103d70:	5d                   	pop    %ebp
80103d71:	c3                   	ret    
      release(&ptable.lock);
80103d72:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
80103d79:	e8 d2 04 00 00       	call   80104250 <release>
}
80103d7e:	83 c4 10             	add    $0x10,%esp
      return -1;
80103d81:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80103d86:	5b                   	pop    %ebx
80103d87:	5e                   	pop    %esi
80103d88:	5d                   	pop    %ebp
80103d89:	c3                   	ret    
80103d8a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

80103d90 <wakeup>:
}

// Wake up all processes sleeping on chan.
void wakeup(void *chan)
{
80103d90:	55                   	push   %ebp
80103d91:	89 e5                	mov    %esp,%ebp
80103d93:	53                   	push   %ebx
80103d94:	83 ec 14             	sub    $0x14,%esp
80103d97:	8b 5d 08             	mov    0x8(%ebp),%ebx
  acquire(&ptable.lock);
80103d9a:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
80103da1:	e8 3a 04 00 00       	call   801041e0 <acquire>
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
80103da6:	b8 54 2d 11 80       	mov    $0x80112d54,%eax
80103dab:	eb 0d                	jmp    80103dba <wakeup+0x2a>
80103dad:	8d 76 00             	lea    0x0(%esi),%esi
80103db0:	83 e8 80             	sub    $0xffffff80,%eax
80103db3:	3d 54 4d 11 80       	cmp    $0x80114d54,%eax
80103db8:	74 1e                	je     80103dd8 <wakeup+0x48>
    if (p->state == SLEEPING && p->chan == chan)
80103dba:	83 78 0c 02          	cmpl   $0x2,0xc(%eax)
80103dbe:	75 f0                	jne    80103db0 <wakeup+0x20>
80103dc0:	3b 58 20             	cmp    0x20(%eax),%ebx
80103dc3:	75 eb                	jne    80103db0 <wakeup+0x20>
      p->state = RUNNABLE;
80103dc5:	c7 40 0c 03 00 00 00 	movl   $0x3,0xc(%eax)
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
80103dcc:	83 e8 80             	sub    $0xffffff80,%eax
80103dcf:	3d 54 4d 11 80       	cmp    $0x80114d54,%eax
80103dd4:	75 e4                	jne    80103dba <wakeup+0x2a>
80103dd6:	66 90                	xchg   %ax,%ax
  wakeup1(chan);
  release(&ptable.lock);
80103dd8:	c7 45 08 20 2d 11 80 	movl   $0x80112d20,0x8(%ebp)
}
80103ddf:	83 c4 14             	add    $0x14,%esp
80103de2:	5b                   	pop    %ebx
80103de3:	5d                   	pop    %ebp
  release(&ptable.lock);
80103de4:	e9 67 04 00 00       	jmp    80104250 <release>
80103de9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

80103df0 <kill>:

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int kill(int pid)
{
80103df0:	55                   	push   %ebp
80103df1:	89 e5                	mov    %esp,%ebp
80103df3:	53                   	push   %ebx
80103df4:	83 ec 14             	sub    $0x14,%esp
80103df7:	8b 5d 08             	mov    0x8(%ebp),%ebx
  struct proc *p;

  acquire(&ptable.lock);
80103dfa:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
80103e01:	e8 da 03 00 00       	call   801041e0 <acquire>
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
80103e06:	b8 54 2d 11 80       	mov    $0x80112d54,%eax
80103e0b:	eb 0d                	jmp    80103e1a <kill+0x2a>
80103e0d:	8d 76 00             	lea    0x0(%esi),%esi
80103e10:	83 e8 80             	sub    $0xffffff80,%eax
80103e13:	3d 54 4d 11 80       	cmp    $0x80114d54,%eax
80103e18:	74 36                	je     80103e50 <kill+0x60>
  {
    if (p->pid == pid)
80103e1a:	39 58 10             	cmp    %ebx,0x10(%eax)
80103e1d:	75 f1                	jne    80103e10 <kill+0x20>
    {
      p->killed = 1;
      // Wake process from sleep if necessary.
      if (p->state == SLEEPING)
80103e1f:	83 78 0c 02          	cmpl   $0x2,0xc(%eax)
      p->killed = 1;
80103e23:	c7 40 24 01 00 00 00 	movl   $0x1,0x24(%eax)
      if (p->state == SLEEPING)
80103e2a:	74 14                	je     80103e40 <kill+0x50>
        p->state = RUNNABLE;
      release(&ptable.lock);
80103e2c:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
80103e33:	e8 18 04 00 00       	call   80104250 <release>
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}
80103e38:	83 c4 14             	add    $0x14,%esp
      return 0;
80103e3b:	31 c0                	xor    %eax,%eax
}
80103e3d:	5b                   	pop    %ebx
80103e3e:	5d                   	pop    %ebp
80103e3f:	c3                   	ret    
        p->state = RUNNABLE;
80103e40:	c7 40 0c 03 00 00 00 	movl   $0x3,0xc(%eax)
80103e47:	eb e3                	jmp    80103e2c <kill+0x3c>
80103e49:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  release(&ptable.lock);
80103e50:	c7 04 24 20 2d 11 80 	movl   $0x80112d20,(%esp)
80103e57:	e8 f4 03 00 00       	call   80104250 <release>
}
80103e5c:	83 c4 14             	add    $0x14,%esp
  return -1;
80103e5f:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80103e64:	5b                   	pop    %ebx
80103e65:	5d                   	pop    %ebp
80103e66:	c3                   	ret    
80103e67:	89 f6                	mov    %esi,%esi
80103e69:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80103e70 <procdump>:
// PAGEBREAK: 36
//  Print a process listing to console.  For debugging.
//  Runs when user types ^P on console.
//  No lock to avoid wedging a stuck machine further.
void procdump(void)
{
80103e70:	55                   	push   %ebp
80103e71:	89 e5                	mov    %esp,%ebp
80103e73:	57                   	push   %edi
80103e74:	56                   	push   %esi
80103e75:	53                   	push   %ebx
80103e76:	bb c0 2d 11 80       	mov    $0x80112dc0,%ebx
80103e7b:	83 ec 4c             	sub    $0x4c,%esp
80103e7e:	8d 75 e8             	lea    -0x18(%ebp),%esi
80103e81:	eb 20                	jmp    80103ea3 <procdump+0x33>
80103e83:	90                   	nop
80103e84:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    {
      getcallerpcs((uint *)p->context->ebp + 2, pc);
      for (i = 0; i < 10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
80103e88:	c7 04 24 b7 75 10 80 	movl   $0x801075b7,(%esp)
80103e8f:	e8 bc c7 ff ff       	call   80100650 <cprintf>
80103e94:	83 eb 80             	sub    $0xffffff80,%ebx
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
80103e97:	81 fb c0 4d 11 80    	cmp    $0x80114dc0,%ebx
80103e9d:	0f 84 8d 00 00 00    	je     80103f30 <procdump+0xc0>
    if (p->state == UNUSED)
80103ea3:	8b 43 a0             	mov    -0x60(%ebx),%eax
80103ea6:	85 c0                	test   %eax,%eax
80103ea8:	74 ea                	je     80103e94 <procdump+0x24>
    if (p->state >= 0 && p->state < NELEM(states) && states[p->state])
80103eaa:	83 f8 05             	cmp    $0x5,%eax
      state = "???";
80103ead:	ba 40 72 10 80       	mov    $0x80107240,%edx
    if (p->state >= 0 && p->state < NELEM(states) && states[p->state])
80103eb2:	77 11                	ja     80103ec5 <procdump+0x55>
80103eb4:	8b 14 85 a0 72 10 80 	mov    -0x7fef8d60(,%eax,4),%edx
      state = "???";
80103ebb:	b8 40 72 10 80       	mov    $0x80107240,%eax
80103ec0:	85 d2                	test   %edx,%edx
80103ec2:	0f 44 d0             	cmove  %eax,%edx
    cprintf("%d %s %s", p->pid, state, p->name);
80103ec5:	8b 43 a4             	mov    -0x5c(%ebx),%eax
80103ec8:	89 5c 24 0c          	mov    %ebx,0xc(%esp)
80103ecc:	89 54 24 08          	mov    %edx,0x8(%esp)
80103ed0:	c7 04 24 44 72 10 80 	movl   $0x80107244,(%esp)
80103ed7:	89 44 24 04          	mov    %eax,0x4(%esp)
80103edb:	e8 70 c7 ff ff       	call   80100650 <cprintf>
    if (p->state == SLEEPING)
80103ee0:	83 7b a0 02          	cmpl   $0x2,-0x60(%ebx)
80103ee4:	75 a2                	jne    80103e88 <procdump+0x18>
      getcallerpcs((uint *)p->context->ebp + 2, pc);
80103ee6:	8d 45 c0             	lea    -0x40(%ebp),%eax
80103ee9:	89 44 24 04          	mov    %eax,0x4(%esp)
80103eed:	8b 43 b0             	mov    -0x50(%ebx),%eax
80103ef0:	8d 7d c0             	lea    -0x40(%ebp),%edi
80103ef3:	8b 40 0c             	mov    0xc(%eax),%eax
80103ef6:	83 c0 08             	add    $0x8,%eax
80103ef9:	89 04 24             	mov    %eax,(%esp)
80103efc:	e8 8f 01 00 00       	call   80104090 <getcallerpcs>
80103f01:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
      for (i = 0; i < 10 && pc[i] != 0; i++)
80103f08:	8b 17                	mov    (%edi),%edx
80103f0a:	85 d2                	test   %edx,%edx
80103f0c:	0f 84 76 ff ff ff    	je     80103e88 <procdump+0x18>
        cprintf(" %p", pc[i]);
80103f12:	89 54 24 04          	mov    %edx,0x4(%esp)
80103f16:	83 c7 04             	add    $0x4,%edi
80103f19:	c7 04 24 81 6c 10 80 	movl   $0x80106c81,(%esp)
80103f20:	e8 2b c7 ff ff       	call   80100650 <cprintf>
      for (i = 0; i < 10 && pc[i] != 0; i++)
80103f25:	39 f7                	cmp    %esi,%edi
80103f27:	75 df                	jne    80103f08 <procdump+0x98>
80103f29:	e9 5a ff ff ff       	jmp    80103e88 <procdump+0x18>
80103f2e:	66 90                	xchg   %ax,%ax
  }
}
80103f30:	83 c4 4c             	add    $0x4c,%esp
80103f33:	5b                   	pop    %ebx
80103f34:	5e                   	pop    %esi
80103f35:	5f                   	pop    %edi
80103f36:	5d                   	pop    %ebp
80103f37:	c3                   	ret    
80103f38:	66 90                	xchg   %ax,%ax
80103f3a:	66 90                	xchg   %ax,%ax
80103f3c:	66 90                	xchg   %ax,%ax
80103f3e:	66 90                	xchg   %ax,%ax

80103f40 <initsleeplock>:
#include "spinlock.h"
#include "sleeplock.h"

void
initsleeplock(struct sleeplock *lk, char *name)
{
80103f40:	55                   	push   %ebp
80103f41:	89 e5                	mov    %esp,%ebp
80103f43:	53                   	push   %ebx
80103f44:	83 ec 14             	sub    $0x14,%esp
80103f47:	8b 5d 08             	mov    0x8(%ebp),%ebx
  initlock(&lk->lk, "sleep lock");
80103f4a:	c7 44 24 04 b8 72 10 	movl   $0x801072b8,0x4(%esp)
80103f51:	80 
80103f52:	8d 43 04             	lea    0x4(%ebx),%eax
80103f55:	89 04 24             	mov    %eax,(%esp)
80103f58:	e8 13 01 00 00       	call   80104070 <initlock>
  lk->name = name;
80103f5d:	8b 45 0c             	mov    0xc(%ebp),%eax
  lk->locked = 0;
80103f60:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)
  lk->pid = 0;
80103f66:	c7 43 3c 00 00 00 00 	movl   $0x0,0x3c(%ebx)
  lk->name = name;
80103f6d:	89 43 38             	mov    %eax,0x38(%ebx)
}
80103f70:	83 c4 14             	add    $0x14,%esp
80103f73:	5b                   	pop    %ebx
80103f74:	5d                   	pop    %ebp
80103f75:	c3                   	ret    
80103f76:	8d 76 00             	lea    0x0(%esi),%esi
80103f79:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80103f80 <acquiresleep>:

void
acquiresleep(struct sleeplock *lk)
{
80103f80:	55                   	push   %ebp
80103f81:	89 e5                	mov    %esp,%ebp
80103f83:	56                   	push   %esi
80103f84:	53                   	push   %ebx
80103f85:	83 ec 10             	sub    $0x10,%esp
80103f88:	8b 5d 08             	mov    0x8(%ebp),%ebx
  acquire(&lk->lk);
80103f8b:	8d 73 04             	lea    0x4(%ebx),%esi
80103f8e:	89 34 24             	mov    %esi,(%esp)
80103f91:	e8 4a 02 00 00       	call   801041e0 <acquire>
  while (lk->locked) {
80103f96:	8b 13                	mov    (%ebx),%edx
80103f98:	85 d2                	test   %edx,%edx
80103f9a:	74 16                	je     80103fb2 <acquiresleep+0x32>
80103f9c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    sleep(lk, &lk->lk);
80103fa0:	89 74 24 04          	mov    %esi,0x4(%esp)
80103fa4:	89 1c 24             	mov    %ebx,(%esp)
80103fa7:	e8 54 fc ff ff       	call   80103c00 <sleep>
  while (lk->locked) {
80103fac:	8b 03                	mov    (%ebx),%eax
80103fae:	85 c0                	test   %eax,%eax
80103fb0:	75 ee                	jne    80103fa0 <acquiresleep+0x20>
  }
  lk->locked = 1;
80103fb2:	c7 03 01 00 00 00    	movl   $0x1,(%ebx)
  lk->pid = myproc()->pid;
80103fb8:	e8 e3 f6 ff ff       	call   801036a0 <myproc>
80103fbd:	8b 40 10             	mov    0x10(%eax),%eax
80103fc0:	89 43 3c             	mov    %eax,0x3c(%ebx)
  release(&lk->lk);
80103fc3:	89 75 08             	mov    %esi,0x8(%ebp)
}
80103fc6:	83 c4 10             	add    $0x10,%esp
80103fc9:	5b                   	pop    %ebx
80103fca:	5e                   	pop    %esi
80103fcb:	5d                   	pop    %ebp
  release(&lk->lk);
80103fcc:	e9 7f 02 00 00       	jmp    80104250 <release>
80103fd1:	eb 0d                	jmp    80103fe0 <releasesleep>
80103fd3:	90                   	nop
80103fd4:	90                   	nop
80103fd5:	90                   	nop
80103fd6:	90                   	nop
80103fd7:	90                   	nop
80103fd8:	90                   	nop
80103fd9:	90                   	nop
80103fda:	90                   	nop
80103fdb:	90                   	nop
80103fdc:	90                   	nop
80103fdd:	90                   	nop
80103fde:	90                   	nop
80103fdf:	90                   	nop

80103fe0 <releasesleep>:

void
releasesleep(struct sleeplock *lk)
{
80103fe0:	55                   	push   %ebp
80103fe1:	89 e5                	mov    %esp,%ebp
80103fe3:	56                   	push   %esi
80103fe4:	53                   	push   %ebx
80103fe5:	83 ec 10             	sub    $0x10,%esp
80103fe8:	8b 5d 08             	mov    0x8(%ebp),%ebx
  acquire(&lk->lk);
80103feb:	8d 73 04             	lea    0x4(%ebx),%esi
80103fee:	89 34 24             	mov    %esi,(%esp)
80103ff1:	e8 ea 01 00 00       	call   801041e0 <acquire>
  lk->locked = 0;
80103ff6:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)
  lk->pid = 0;
80103ffc:	c7 43 3c 00 00 00 00 	movl   $0x0,0x3c(%ebx)
  wakeup(lk);
80104003:	89 1c 24             	mov    %ebx,(%esp)
80104006:	e8 85 fd ff ff       	call   80103d90 <wakeup>
  release(&lk->lk);
8010400b:	89 75 08             	mov    %esi,0x8(%ebp)
}
8010400e:	83 c4 10             	add    $0x10,%esp
80104011:	5b                   	pop    %ebx
80104012:	5e                   	pop    %esi
80104013:	5d                   	pop    %ebp
  release(&lk->lk);
80104014:	e9 37 02 00 00       	jmp    80104250 <release>
80104019:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

80104020 <holdingsleep>:

int
holdingsleep(struct sleeplock *lk)
{
80104020:	55                   	push   %ebp
80104021:	89 e5                	mov    %esp,%ebp
80104023:	57                   	push   %edi
  int r;
  
  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
80104024:	31 ff                	xor    %edi,%edi
{
80104026:	56                   	push   %esi
80104027:	53                   	push   %ebx
80104028:	83 ec 1c             	sub    $0x1c,%esp
8010402b:	8b 5d 08             	mov    0x8(%ebp),%ebx
  acquire(&lk->lk);
8010402e:	8d 73 04             	lea    0x4(%ebx),%esi
80104031:	89 34 24             	mov    %esi,(%esp)
80104034:	e8 a7 01 00 00       	call   801041e0 <acquire>
  r = lk->locked && (lk->pid == myproc()->pid);
80104039:	8b 03                	mov    (%ebx),%eax
8010403b:	85 c0                	test   %eax,%eax
8010403d:	74 13                	je     80104052 <holdingsleep+0x32>
8010403f:	8b 5b 3c             	mov    0x3c(%ebx),%ebx
80104042:	e8 59 f6 ff ff       	call   801036a0 <myproc>
80104047:	3b 58 10             	cmp    0x10(%eax),%ebx
8010404a:	0f 94 c0             	sete   %al
8010404d:	0f b6 c0             	movzbl %al,%eax
80104050:	89 c7                	mov    %eax,%edi
  release(&lk->lk);
80104052:	89 34 24             	mov    %esi,(%esp)
80104055:	e8 f6 01 00 00       	call   80104250 <release>
  return r;
}
8010405a:	83 c4 1c             	add    $0x1c,%esp
8010405d:	89 f8                	mov    %edi,%eax
8010405f:	5b                   	pop    %ebx
80104060:	5e                   	pop    %esi
80104061:	5f                   	pop    %edi
80104062:	5d                   	pop    %ebp
80104063:	c3                   	ret    
80104064:	66 90                	xchg   %ax,%ax
80104066:	66 90                	xchg   %ax,%ax
80104068:	66 90                	xchg   %ax,%ax
8010406a:	66 90                	xchg   %ax,%ax
8010406c:	66 90                	xchg   %ax,%ax
8010406e:	66 90                	xchg   %ax,%ax

80104070 <initlock>:
#include "proc.h"
#include "spinlock.h"

void
initlock(struct spinlock *lk, char *name)
{
80104070:	55                   	push   %ebp
80104071:	89 e5                	mov    %esp,%ebp
80104073:	8b 45 08             	mov    0x8(%ebp),%eax
  lk->name = name;
80104076:	8b 55 0c             	mov    0xc(%ebp),%edx
  lk->locked = 0;
80104079:	c7 00 00 00 00 00    	movl   $0x0,(%eax)
  lk->name = name;
8010407f:	89 50 04             	mov    %edx,0x4(%eax)
  lk->cpu = 0;
80104082:	c7 40 08 00 00 00 00 	movl   $0x0,0x8(%eax)
}
80104089:	5d                   	pop    %ebp
8010408a:	c3                   	ret    
8010408b:	90                   	nop
8010408c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80104090 <getcallerpcs>:
}

// Record the current call stack in pcs[] by following the %ebp chain.
void
getcallerpcs(void *v, uint pcs[])
{
80104090:	55                   	push   %ebp
80104091:	89 e5                	mov    %esp,%ebp
  uint *ebp;
  int i;

  ebp = (uint*)v - 2;
80104093:	8b 45 08             	mov    0x8(%ebp),%eax
{
80104096:	8b 4d 0c             	mov    0xc(%ebp),%ecx
80104099:	53                   	push   %ebx
  ebp = (uint*)v - 2;
8010409a:	8d 50 f8             	lea    -0x8(%eax),%edx
  for(i = 0; i < 10; i++){
8010409d:	31 c0                	xor    %eax,%eax
8010409f:	90                   	nop
    if(ebp == 0 || ebp < (uint*)KERNBASE || ebp == (uint*)0xffffffff)
801040a0:	8d 9a 00 00 00 80    	lea    -0x80000000(%edx),%ebx
801040a6:	81 fb fe ff ff 7f    	cmp    $0x7ffffffe,%ebx
801040ac:	77 1a                	ja     801040c8 <getcallerpcs+0x38>
      break;
    pcs[i] = ebp[1];     // saved %eip
801040ae:	8b 5a 04             	mov    0x4(%edx),%ebx
801040b1:	89 1c 81             	mov    %ebx,(%ecx,%eax,4)
  for(i = 0; i < 10; i++){
801040b4:	83 c0 01             	add    $0x1,%eax
    ebp = (uint*)ebp[0]; // saved %ebp
801040b7:	8b 12                	mov    (%edx),%edx
  for(i = 0; i < 10; i++){
801040b9:	83 f8 0a             	cmp    $0xa,%eax
801040bc:	75 e2                	jne    801040a0 <getcallerpcs+0x10>
  }
  for(; i < 10; i++)
    pcs[i] = 0;
}
801040be:	5b                   	pop    %ebx
801040bf:	5d                   	pop    %ebp
801040c0:	c3                   	ret    
801040c1:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
    pcs[i] = 0;
801040c8:	c7 04 81 00 00 00 00 	movl   $0x0,(%ecx,%eax,4)
  for(; i < 10; i++)
801040cf:	83 c0 01             	add    $0x1,%eax
801040d2:	83 f8 0a             	cmp    $0xa,%eax
801040d5:	74 e7                	je     801040be <getcallerpcs+0x2e>
    pcs[i] = 0;
801040d7:	c7 04 81 00 00 00 00 	movl   $0x0,(%ecx,%eax,4)
  for(; i < 10; i++)
801040de:	83 c0 01             	add    $0x1,%eax
801040e1:	83 f8 0a             	cmp    $0xa,%eax
801040e4:	75 e2                	jne    801040c8 <getcallerpcs+0x38>
801040e6:	eb d6                	jmp    801040be <getcallerpcs+0x2e>
801040e8:	90                   	nop
801040e9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

801040f0 <pushcli>:
// it takes two popcli to undo two pushcli.  Also, if interrupts
// are off, then pushcli, popcli leaves them off.

void
pushcli(void)
{
801040f0:	55                   	push   %ebp
801040f1:	89 e5                	mov    %esp,%ebp
801040f3:	53                   	push   %ebx
801040f4:	83 ec 04             	sub    $0x4,%esp
801040f7:	9c                   	pushf  
801040f8:	5b                   	pop    %ebx
  asm volatile("cli");
801040f9:	fa                   	cli    
  int eflags;

  eflags = readeflags();
  cli();
  if(mycpu()->ncli == 0)
801040fa:	e8 01 f5 ff ff       	call   80103600 <mycpu>
801040ff:	8b 80 a4 00 00 00    	mov    0xa4(%eax),%eax
80104105:	85 c0                	test   %eax,%eax
80104107:	75 11                	jne    8010411a <pushcli+0x2a>
    mycpu()->intena = eflags & FL_IF;
80104109:	e8 f2 f4 ff ff       	call   80103600 <mycpu>
8010410e:	81 e3 00 02 00 00    	and    $0x200,%ebx
80104114:	89 98 a8 00 00 00    	mov    %ebx,0xa8(%eax)
  mycpu()->ncli += 1;
8010411a:	e8 e1 f4 ff ff       	call   80103600 <mycpu>
8010411f:	83 80 a4 00 00 00 01 	addl   $0x1,0xa4(%eax)
}
80104126:	83 c4 04             	add    $0x4,%esp
80104129:	5b                   	pop    %ebx
8010412a:	5d                   	pop    %ebp
8010412b:	c3                   	ret    
8010412c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80104130 <popcli>:

void
popcli(void)
{
80104130:	55                   	push   %ebp
80104131:	89 e5                	mov    %esp,%ebp
80104133:	83 ec 18             	sub    $0x18,%esp
  asm volatile("pushfl; popl %0" : "=r" (eflags));
80104136:	9c                   	pushf  
80104137:	58                   	pop    %eax
  if(readeflags()&FL_IF)
80104138:	f6 c4 02             	test   $0x2,%ah
8010413b:	75 49                	jne    80104186 <popcli+0x56>
    panic("popcli - interruptible");
  if(--mycpu()->ncli < 0)
8010413d:	e8 be f4 ff ff       	call   80103600 <mycpu>
80104142:	8b 88 a4 00 00 00    	mov    0xa4(%eax),%ecx
80104148:	8d 51 ff             	lea    -0x1(%ecx),%edx
8010414b:	85 d2                	test   %edx,%edx
8010414d:	89 90 a4 00 00 00    	mov    %edx,0xa4(%eax)
80104153:	78 25                	js     8010417a <popcli+0x4a>
    panic("popcli");
  if(mycpu()->ncli == 0 && mycpu()->intena)
80104155:	e8 a6 f4 ff ff       	call   80103600 <mycpu>
8010415a:	8b 90 a4 00 00 00    	mov    0xa4(%eax),%edx
80104160:	85 d2                	test   %edx,%edx
80104162:	74 04                	je     80104168 <popcli+0x38>
    sti();
}
80104164:	c9                   	leave  
80104165:	c3                   	ret    
80104166:	66 90                	xchg   %ax,%ax
  if(mycpu()->ncli == 0 && mycpu()->intena)
80104168:	e8 93 f4 ff ff       	call   80103600 <mycpu>
8010416d:	8b 80 a8 00 00 00    	mov    0xa8(%eax),%eax
80104173:	85 c0                	test   %eax,%eax
80104175:	74 ed                	je     80104164 <popcli+0x34>
  asm volatile("sti");
80104177:	fb                   	sti    
}
80104178:	c9                   	leave  
80104179:	c3                   	ret    
    panic("popcli");
8010417a:	c7 04 24 da 72 10 80 	movl   $0x801072da,(%esp)
80104181:	e8 da c1 ff ff       	call   80100360 <panic>
    panic("popcli - interruptible");
80104186:	c7 04 24 c3 72 10 80 	movl   $0x801072c3,(%esp)
8010418d:	e8 ce c1 ff ff       	call   80100360 <panic>
80104192:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80104199:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801041a0 <holding>:
{
801041a0:	55                   	push   %ebp
801041a1:	89 e5                	mov    %esp,%ebp
801041a3:	56                   	push   %esi
  r = lock->locked && lock->cpu == mycpu();
801041a4:	31 f6                	xor    %esi,%esi
{
801041a6:	53                   	push   %ebx
801041a7:	8b 5d 08             	mov    0x8(%ebp),%ebx
  pushcli();
801041aa:	e8 41 ff ff ff       	call   801040f0 <pushcli>
  r = lock->locked && lock->cpu == mycpu();
801041af:	8b 03                	mov    (%ebx),%eax
801041b1:	85 c0                	test   %eax,%eax
801041b3:	74 12                	je     801041c7 <holding+0x27>
801041b5:	8b 5b 08             	mov    0x8(%ebx),%ebx
801041b8:	e8 43 f4 ff ff       	call   80103600 <mycpu>
801041bd:	39 c3                	cmp    %eax,%ebx
801041bf:	0f 94 c0             	sete   %al
801041c2:	0f b6 c0             	movzbl %al,%eax
801041c5:	89 c6                	mov    %eax,%esi
  popcli();
801041c7:	e8 64 ff ff ff       	call   80104130 <popcli>
}
801041cc:	89 f0                	mov    %esi,%eax
801041ce:	5b                   	pop    %ebx
801041cf:	5e                   	pop    %esi
801041d0:	5d                   	pop    %ebp
801041d1:	c3                   	ret    
801041d2:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801041d9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801041e0 <acquire>:
{
801041e0:	55                   	push   %ebp
801041e1:	89 e5                	mov    %esp,%ebp
801041e3:	53                   	push   %ebx
801041e4:	83 ec 14             	sub    $0x14,%esp
  pushcli(); // disable interrupts to avoid deadlock.
801041e7:	e8 04 ff ff ff       	call   801040f0 <pushcli>
  if(holding(lk))
801041ec:	8b 45 08             	mov    0x8(%ebp),%eax
801041ef:	89 04 24             	mov    %eax,(%esp)
801041f2:	e8 a9 ff ff ff       	call   801041a0 <holding>
801041f7:	85 c0                	test   %eax,%eax
801041f9:	75 3a                	jne    80104235 <acquire+0x55>
  asm volatile("lock; xchgl %0, %1" :
801041fb:	b9 01 00 00 00       	mov    $0x1,%ecx
  while(xchg(&lk->locked, 1) != 0)
80104200:	8b 55 08             	mov    0x8(%ebp),%edx
80104203:	89 c8                	mov    %ecx,%eax
80104205:	f0 87 02             	lock xchg %eax,(%edx)
80104208:	85 c0                	test   %eax,%eax
8010420a:	75 f4                	jne    80104200 <acquire+0x20>
  __sync_synchronize();
8010420c:	0f ae f0             	mfence 
  lk->cpu = mycpu();
8010420f:	8b 5d 08             	mov    0x8(%ebp),%ebx
80104212:	e8 e9 f3 ff ff       	call   80103600 <mycpu>
80104217:	89 43 08             	mov    %eax,0x8(%ebx)
  getcallerpcs(&lk, lk->pcs);
8010421a:	8b 45 08             	mov    0x8(%ebp),%eax
8010421d:	83 c0 0c             	add    $0xc,%eax
80104220:	89 44 24 04          	mov    %eax,0x4(%esp)
80104224:	8d 45 08             	lea    0x8(%ebp),%eax
80104227:	89 04 24             	mov    %eax,(%esp)
8010422a:	e8 61 fe ff ff       	call   80104090 <getcallerpcs>
}
8010422f:	83 c4 14             	add    $0x14,%esp
80104232:	5b                   	pop    %ebx
80104233:	5d                   	pop    %ebp
80104234:	c3                   	ret    
    panic("acquire");
80104235:	c7 04 24 e1 72 10 80 	movl   $0x801072e1,(%esp)
8010423c:	e8 1f c1 ff ff       	call   80100360 <panic>
80104241:	eb 0d                	jmp    80104250 <release>
80104243:	90                   	nop
80104244:	90                   	nop
80104245:	90                   	nop
80104246:	90                   	nop
80104247:	90                   	nop
80104248:	90                   	nop
80104249:	90                   	nop
8010424a:	90                   	nop
8010424b:	90                   	nop
8010424c:	90                   	nop
8010424d:	90                   	nop
8010424e:	90                   	nop
8010424f:	90                   	nop

80104250 <release>:
{
80104250:	55                   	push   %ebp
80104251:	89 e5                	mov    %esp,%ebp
80104253:	53                   	push   %ebx
80104254:	83 ec 14             	sub    $0x14,%esp
80104257:	8b 5d 08             	mov    0x8(%ebp),%ebx
  if(!holding(lk))
8010425a:	89 1c 24             	mov    %ebx,(%esp)
8010425d:	e8 3e ff ff ff       	call   801041a0 <holding>
80104262:	85 c0                	test   %eax,%eax
80104264:	74 21                	je     80104287 <release+0x37>
  lk->pcs[0] = 0;
80104266:	c7 43 0c 00 00 00 00 	movl   $0x0,0xc(%ebx)
  lk->cpu = 0;
8010426d:	c7 43 08 00 00 00 00 	movl   $0x0,0x8(%ebx)
  __sync_synchronize();
80104274:	0f ae f0             	mfence 
  asm volatile("movl $0, %0" : "+m" (lk->locked) : );
80104277:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)
}
8010427d:	83 c4 14             	add    $0x14,%esp
80104280:	5b                   	pop    %ebx
80104281:	5d                   	pop    %ebp
  popcli();
80104282:	e9 a9 fe ff ff       	jmp    80104130 <popcli>
    panic("release");
80104287:	c7 04 24 e9 72 10 80 	movl   $0x801072e9,(%esp)
8010428e:	e8 cd c0 ff ff       	call   80100360 <panic>
80104293:	66 90                	xchg   %ax,%ax
80104295:	66 90                	xchg   %ax,%ax
80104297:	66 90                	xchg   %ax,%ax
80104299:	66 90                	xchg   %ax,%ax
8010429b:	66 90                	xchg   %ax,%ax
8010429d:	66 90                	xchg   %ax,%ax
8010429f:	90                   	nop

801042a0 <memset>:
801042a0:	55                   	push   %ebp
801042a1:	89 e5                	mov    %esp,%ebp
801042a3:	8b 55 08             	mov    0x8(%ebp),%edx
801042a6:	57                   	push   %edi
801042a7:	8b 4d 10             	mov    0x10(%ebp),%ecx
801042aa:	53                   	push   %ebx
801042ab:	f6 c2 03             	test   $0x3,%dl
801042ae:	75 05                	jne    801042b5 <memset+0x15>
801042b0:	f6 c1 03             	test   $0x3,%cl
801042b3:	74 13                	je     801042c8 <memset+0x28>
801042b5:	89 d7                	mov    %edx,%edi
801042b7:	8b 45 0c             	mov    0xc(%ebp),%eax
801042ba:	fc                   	cld    
801042bb:	f3 aa                	rep stos %al,%es:(%edi)
801042bd:	5b                   	pop    %ebx
801042be:	89 d0                	mov    %edx,%eax
801042c0:	5f                   	pop    %edi
801042c1:	5d                   	pop    %ebp
801042c2:	c3                   	ret    
801042c3:	90                   	nop
801042c4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
801042c8:	0f b6 7d 0c          	movzbl 0xc(%ebp),%edi
801042cc:	c1 e9 02             	shr    $0x2,%ecx
801042cf:	89 f8                	mov    %edi,%eax
801042d1:	89 fb                	mov    %edi,%ebx
801042d3:	c1 e0 18             	shl    $0x18,%eax
801042d6:	c1 e3 10             	shl    $0x10,%ebx
801042d9:	09 d8                	or     %ebx,%eax
801042db:	09 f8                	or     %edi,%eax
801042dd:	c1 e7 08             	shl    $0x8,%edi
801042e0:	09 f8                	or     %edi,%eax
801042e2:	89 d7                	mov    %edx,%edi
801042e4:	fc                   	cld    
801042e5:	f3 ab                	rep stos %eax,%es:(%edi)
801042e7:	5b                   	pop    %ebx
801042e8:	89 d0                	mov    %edx,%eax
801042ea:	5f                   	pop    %edi
801042eb:	5d                   	pop    %ebp
801042ec:	c3                   	ret    
801042ed:	8d 76 00             	lea    0x0(%esi),%esi

801042f0 <memcmp>:
801042f0:	55                   	push   %ebp
801042f1:	89 e5                	mov    %esp,%ebp
801042f3:	8b 45 10             	mov    0x10(%ebp),%eax
801042f6:	57                   	push   %edi
801042f7:	56                   	push   %esi
801042f8:	8b 75 0c             	mov    0xc(%ebp),%esi
801042fb:	53                   	push   %ebx
801042fc:	8b 5d 08             	mov    0x8(%ebp),%ebx
801042ff:	85 c0                	test   %eax,%eax
80104301:	8d 78 ff             	lea    -0x1(%eax),%edi
80104304:	74 26                	je     8010432c <memcmp+0x3c>
80104306:	0f b6 03             	movzbl (%ebx),%eax
80104309:	31 d2                	xor    %edx,%edx
8010430b:	0f b6 0e             	movzbl (%esi),%ecx
8010430e:	38 c8                	cmp    %cl,%al
80104310:	74 16                	je     80104328 <memcmp+0x38>
80104312:	eb 24                	jmp    80104338 <memcmp+0x48>
80104314:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80104318:	0f b6 44 13 01       	movzbl 0x1(%ebx,%edx,1),%eax
8010431d:	83 c2 01             	add    $0x1,%edx
80104320:	0f b6 0c 16          	movzbl (%esi,%edx,1),%ecx
80104324:	38 c8                	cmp    %cl,%al
80104326:	75 10                	jne    80104338 <memcmp+0x48>
80104328:	39 fa                	cmp    %edi,%edx
8010432a:	75 ec                	jne    80104318 <memcmp+0x28>
8010432c:	5b                   	pop    %ebx
8010432d:	31 c0                	xor    %eax,%eax
8010432f:	5e                   	pop    %esi
80104330:	5f                   	pop    %edi
80104331:	5d                   	pop    %ebp
80104332:	c3                   	ret    
80104333:	90                   	nop
80104334:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80104338:	5b                   	pop    %ebx
80104339:	29 c8                	sub    %ecx,%eax
8010433b:	5e                   	pop    %esi
8010433c:	5f                   	pop    %edi
8010433d:	5d                   	pop    %ebp
8010433e:	c3                   	ret    
8010433f:	90                   	nop

80104340 <memmove>:
80104340:	55                   	push   %ebp
80104341:	89 e5                	mov    %esp,%ebp
80104343:	57                   	push   %edi
80104344:	8b 45 08             	mov    0x8(%ebp),%eax
80104347:	56                   	push   %esi
80104348:	8b 75 0c             	mov    0xc(%ebp),%esi
8010434b:	53                   	push   %ebx
8010434c:	8b 5d 10             	mov    0x10(%ebp),%ebx
8010434f:	39 c6                	cmp    %eax,%esi
80104351:	73 35                	jae    80104388 <memmove+0x48>
80104353:	8d 0c 1e             	lea    (%esi,%ebx,1),%ecx
80104356:	39 c8                	cmp    %ecx,%eax
80104358:	73 2e                	jae    80104388 <memmove+0x48>
8010435a:	85 db                	test   %ebx,%ebx
8010435c:	8d 3c 18             	lea    (%eax,%ebx,1),%edi
8010435f:	8d 53 ff             	lea    -0x1(%ebx),%edx
80104362:	74 1b                	je     8010437f <memmove+0x3f>
80104364:	f7 db                	neg    %ebx
80104366:	8d 34 19             	lea    (%ecx,%ebx,1),%esi
80104369:	01 fb                	add    %edi,%ebx
8010436b:	90                   	nop
8010436c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80104370:	0f b6 0c 16          	movzbl (%esi,%edx,1),%ecx
80104374:	88 0c 13             	mov    %cl,(%ebx,%edx,1)
80104377:	83 ea 01             	sub    $0x1,%edx
8010437a:	83 fa ff             	cmp    $0xffffffff,%edx
8010437d:	75 f1                	jne    80104370 <memmove+0x30>
8010437f:	5b                   	pop    %ebx
80104380:	5e                   	pop    %esi
80104381:	5f                   	pop    %edi
80104382:	5d                   	pop    %ebp
80104383:	c3                   	ret    
80104384:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80104388:	31 d2                	xor    %edx,%edx
8010438a:	85 db                	test   %ebx,%ebx
8010438c:	74 f1                	je     8010437f <memmove+0x3f>
8010438e:	66 90                	xchg   %ax,%ax
80104390:	0f b6 0c 16          	movzbl (%esi,%edx,1),%ecx
80104394:	88 0c 10             	mov    %cl,(%eax,%edx,1)
80104397:	83 c2 01             	add    $0x1,%edx
8010439a:	39 da                	cmp    %ebx,%edx
8010439c:	75 f2                	jne    80104390 <memmove+0x50>
8010439e:	5b                   	pop    %ebx
8010439f:	5e                   	pop    %esi
801043a0:	5f                   	pop    %edi
801043a1:	5d                   	pop    %ebp
801043a2:	c3                   	ret    
801043a3:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
801043a9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801043b0 <memcpy>:
801043b0:	55                   	push   %ebp
801043b1:	89 e5                	mov    %esp,%ebp
801043b3:	5d                   	pop    %ebp
801043b4:	eb 8a                	jmp    80104340 <memmove>
801043b6:	8d 76 00             	lea    0x0(%esi),%esi
801043b9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801043c0 <strncmp>:
801043c0:	55                   	push   %ebp
801043c1:	89 e5                	mov    %esp,%ebp
801043c3:	56                   	push   %esi
801043c4:	8b 75 10             	mov    0x10(%ebp),%esi
801043c7:	53                   	push   %ebx
801043c8:	8b 4d 08             	mov    0x8(%ebp),%ecx
801043cb:	8b 5d 0c             	mov    0xc(%ebp),%ebx
801043ce:	85 f6                	test   %esi,%esi
801043d0:	74 30                	je     80104402 <strncmp+0x42>
801043d2:	0f b6 01             	movzbl (%ecx),%eax
801043d5:	84 c0                	test   %al,%al
801043d7:	74 2f                	je     80104408 <strncmp+0x48>
801043d9:	0f b6 13             	movzbl (%ebx),%edx
801043dc:	38 d0                	cmp    %dl,%al
801043de:	75 46                	jne    80104426 <strncmp+0x66>
801043e0:	8d 51 01             	lea    0x1(%ecx),%edx
801043e3:	01 ce                	add    %ecx,%esi
801043e5:	eb 14                	jmp    801043fb <strncmp+0x3b>
801043e7:	90                   	nop
801043e8:	0f b6 02             	movzbl (%edx),%eax
801043eb:	84 c0                	test   %al,%al
801043ed:	74 31                	je     80104420 <strncmp+0x60>
801043ef:	0f b6 19             	movzbl (%ecx),%ebx
801043f2:	83 c2 01             	add    $0x1,%edx
801043f5:	38 d8                	cmp    %bl,%al
801043f7:	75 17                	jne    80104410 <strncmp+0x50>
801043f9:	89 cb                	mov    %ecx,%ebx
801043fb:	39 f2                	cmp    %esi,%edx
801043fd:	8d 4b 01             	lea    0x1(%ebx),%ecx
80104400:	75 e6                	jne    801043e8 <strncmp+0x28>
80104402:	5b                   	pop    %ebx
80104403:	31 c0                	xor    %eax,%eax
80104405:	5e                   	pop    %esi
80104406:	5d                   	pop    %ebp
80104407:	c3                   	ret    
80104408:	0f b6 1b             	movzbl (%ebx),%ebx
8010440b:	31 c0                	xor    %eax,%eax
8010440d:	8d 76 00             	lea    0x0(%esi),%esi
80104410:	0f b6 d3             	movzbl %bl,%edx
80104413:	29 d0                	sub    %edx,%eax
80104415:	5b                   	pop    %ebx
80104416:	5e                   	pop    %esi
80104417:	5d                   	pop    %ebp
80104418:	c3                   	ret    
80104419:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80104420:	0f b6 5b 01          	movzbl 0x1(%ebx),%ebx
80104424:	eb ea                	jmp    80104410 <strncmp+0x50>
80104426:	89 d3                	mov    %edx,%ebx
80104428:	eb e6                	jmp    80104410 <strncmp+0x50>
8010442a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

80104430 <strncpy>:
80104430:	55                   	push   %ebp
80104431:	89 e5                	mov    %esp,%ebp
80104433:	8b 45 08             	mov    0x8(%ebp),%eax
80104436:	56                   	push   %esi
80104437:	8b 4d 10             	mov    0x10(%ebp),%ecx
8010443a:	53                   	push   %ebx
8010443b:	8b 5d 0c             	mov    0xc(%ebp),%ebx
8010443e:	89 c2                	mov    %eax,%edx
80104440:	eb 19                	jmp    8010445b <strncpy+0x2b>
80104442:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80104448:	83 c3 01             	add    $0x1,%ebx
8010444b:	0f b6 4b ff          	movzbl -0x1(%ebx),%ecx
8010444f:	83 c2 01             	add    $0x1,%edx
80104452:	84 c9                	test   %cl,%cl
80104454:	88 4a ff             	mov    %cl,-0x1(%edx)
80104457:	74 09                	je     80104462 <strncpy+0x32>
80104459:	89 f1                	mov    %esi,%ecx
8010445b:	85 c9                	test   %ecx,%ecx
8010445d:	8d 71 ff             	lea    -0x1(%ecx),%esi
80104460:	7f e6                	jg     80104448 <strncpy+0x18>
80104462:	31 c9                	xor    %ecx,%ecx
80104464:	85 f6                	test   %esi,%esi
80104466:	7e 0f                	jle    80104477 <strncpy+0x47>
80104468:	c6 04 0a 00          	movb   $0x0,(%edx,%ecx,1)
8010446c:	89 f3                	mov    %esi,%ebx
8010446e:	83 c1 01             	add    $0x1,%ecx
80104471:	29 cb                	sub    %ecx,%ebx
80104473:	85 db                	test   %ebx,%ebx
80104475:	7f f1                	jg     80104468 <strncpy+0x38>
80104477:	5b                   	pop    %ebx
80104478:	5e                   	pop    %esi
80104479:	5d                   	pop    %ebp
8010447a:	c3                   	ret    
8010447b:	90                   	nop
8010447c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80104480 <safestrcpy>:
80104480:	55                   	push   %ebp
80104481:	89 e5                	mov    %esp,%ebp
80104483:	8b 4d 10             	mov    0x10(%ebp),%ecx
80104486:	56                   	push   %esi
80104487:	8b 45 08             	mov    0x8(%ebp),%eax
8010448a:	53                   	push   %ebx
8010448b:	8b 55 0c             	mov    0xc(%ebp),%edx
8010448e:	85 c9                	test   %ecx,%ecx
80104490:	7e 26                	jle    801044b8 <safestrcpy+0x38>
80104492:	8d 74 0a ff          	lea    -0x1(%edx,%ecx,1),%esi
80104496:	89 c1                	mov    %eax,%ecx
80104498:	eb 17                	jmp    801044b1 <safestrcpy+0x31>
8010449a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
801044a0:	83 c2 01             	add    $0x1,%edx
801044a3:	0f b6 5a ff          	movzbl -0x1(%edx),%ebx
801044a7:	83 c1 01             	add    $0x1,%ecx
801044aa:	84 db                	test   %bl,%bl
801044ac:	88 59 ff             	mov    %bl,-0x1(%ecx)
801044af:	74 04                	je     801044b5 <safestrcpy+0x35>
801044b1:	39 f2                	cmp    %esi,%edx
801044b3:	75 eb                	jne    801044a0 <safestrcpy+0x20>
801044b5:	c6 01 00             	movb   $0x0,(%ecx)
801044b8:	5b                   	pop    %ebx
801044b9:	5e                   	pop    %esi
801044ba:	5d                   	pop    %ebp
801044bb:	c3                   	ret    
801044bc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

801044c0 <strlen>:
801044c0:	55                   	push   %ebp
801044c1:	31 c0                	xor    %eax,%eax
801044c3:	89 e5                	mov    %esp,%ebp
801044c5:	8b 55 08             	mov    0x8(%ebp),%edx
801044c8:	80 3a 00             	cmpb   $0x0,(%edx)
801044cb:	74 0c                	je     801044d9 <strlen+0x19>
801044cd:	8d 76 00             	lea    0x0(%esi),%esi
801044d0:	83 c0 01             	add    $0x1,%eax
801044d3:	80 3c 02 00          	cmpb   $0x0,(%edx,%eax,1)
801044d7:	75 f7                	jne    801044d0 <strlen+0x10>
801044d9:	5d                   	pop    %ebp
801044da:	c3                   	ret    

801044db <swtch>:
801044db:	8b 44 24 04          	mov    0x4(%esp),%eax
801044df:	8b 54 24 08          	mov    0x8(%esp),%edx
801044e3:	55                   	push   %ebp
801044e4:	53                   	push   %ebx
801044e5:	56                   	push   %esi
801044e6:	57                   	push   %edi
801044e7:	89 20                	mov    %esp,(%eax)
801044e9:	89 d4                	mov    %edx,%esp
801044eb:	5f                   	pop    %edi
801044ec:	5e                   	pop    %esi
801044ed:	5b                   	pop    %ebx
801044ee:	5d                   	pop    %ebp
801044ef:	c3                   	ret    

801044f0 <fetchint>:
// to a saved program counter, and then the first argument.

// Fetch the int at addr from the current process.
int
fetchint(uint addr, int *ip)
{
801044f0:	55                   	push   %ebp
801044f1:	89 e5                	mov    %esp,%ebp
801044f3:	53                   	push   %ebx
801044f4:	83 ec 04             	sub    $0x4,%esp
801044f7:	8b 5d 08             	mov    0x8(%ebp),%ebx
  struct proc *curproc = myproc();
801044fa:	e8 a1 f1 ff ff       	call   801036a0 <myproc>

  if(addr >= curproc->sz || addr+4 > curproc->sz)
801044ff:	8b 00                	mov    (%eax),%eax
80104501:	39 d8                	cmp    %ebx,%eax
80104503:	76 1b                	jbe    80104520 <fetchint+0x30>
80104505:	8d 53 04             	lea    0x4(%ebx),%edx
80104508:	39 d0                	cmp    %edx,%eax
8010450a:	72 14                	jb     80104520 <fetchint+0x30>
    return -1;
  *ip = *(int*)(addr);
8010450c:	8b 45 0c             	mov    0xc(%ebp),%eax
8010450f:	8b 13                	mov    (%ebx),%edx
80104511:	89 10                	mov    %edx,(%eax)
  return 0;
80104513:	31 c0                	xor    %eax,%eax
}
80104515:	83 c4 04             	add    $0x4,%esp
80104518:	5b                   	pop    %ebx
80104519:	5d                   	pop    %ebp
8010451a:	c3                   	ret    
8010451b:	90                   	nop
8010451c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    return -1;
80104520:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80104525:	eb ee                	jmp    80104515 <fetchint+0x25>
80104527:	89 f6                	mov    %esi,%esi
80104529:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80104530 <fetchstr>:
// Fetch the nul-terminated string at addr from the current process.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int
fetchstr(uint addr, char **pp)
{
80104530:	55                   	push   %ebp
80104531:	89 e5                	mov    %esp,%ebp
80104533:	53                   	push   %ebx
80104534:	83 ec 04             	sub    $0x4,%esp
80104537:	8b 5d 08             	mov    0x8(%ebp),%ebx
  char *s, *ep;
  struct proc *curproc = myproc();
8010453a:	e8 61 f1 ff ff       	call   801036a0 <myproc>

  if(addr >= curproc->sz)
8010453f:	39 18                	cmp    %ebx,(%eax)
80104541:	76 26                	jbe    80104569 <fetchstr+0x39>
    return -1;
  *pp = (char*)addr;
80104543:	8b 4d 0c             	mov    0xc(%ebp),%ecx
80104546:	89 da                	mov    %ebx,%edx
80104548:	89 19                	mov    %ebx,(%ecx)
  ep = (char*)curproc->sz;
8010454a:	8b 00                	mov    (%eax),%eax
  for(s = *pp; s < ep; s++){
8010454c:	39 c3                	cmp    %eax,%ebx
8010454e:	73 19                	jae    80104569 <fetchstr+0x39>
    if(*s == 0)
80104550:	80 3b 00             	cmpb   $0x0,(%ebx)
80104553:	75 0d                	jne    80104562 <fetchstr+0x32>
80104555:	eb 21                	jmp    80104578 <fetchstr+0x48>
80104557:	90                   	nop
80104558:	80 3a 00             	cmpb   $0x0,(%edx)
8010455b:	90                   	nop
8010455c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80104560:	74 16                	je     80104578 <fetchstr+0x48>
  for(s = *pp; s < ep; s++){
80104562:	83 c2 01             	add    $0x1,%edx
80104565:	39 d0                	cmp    %edx,%eax
80104567:	77 ef                	ja     80104558 <fetchstr+0x28>
      return s - *pp;
  }
  return -1;
}
80104569:	83 c4 04             	add    $0x4,%esp
    return -1;
8010456c:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80104571:	5b                   	pop    %ebx
80104572:	5d                   	pop    %ebp
80104573:	c3                   	ret    
80104574:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80104578:	83 c4 04             	add    $0x4,%esp
      return s - *pp;
8010457b:	89 d0                	mov    %edx,%eax
8010457d:	29 d8                	sub    %ebx,%eax
}
8010457f:	5b                   	pop    %ebx
80104580:	5d                   	pop    %ebp
80104581:	c3                   	ret    
80104582:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80104589:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80104590 <argint>:

// Fetch the nth 32-bit system call argument.
int
argint(int n, int *ip)
{
80104590:	55                   	push   %ebp
80104591:	89 e5                	mov    %esp,%ebp
80104593:	56                   	push   %esi
80104594:	8b 75 0c             	mov    0xc(%ebp),%esi
80104597:	53                   	push   %ebx
80104598:	8b 5d 08             	mov    0x8(%ebp),%ebx
  return fetchint((myproc()->tf->esp) + 4 + 4*n, ip);
8010459b:	e8 00 f1 ff ff       	call   801036a0 <myproc>
801045a0:	89 75 0c             	mov    %esi,0xc(%ebp)
801045a3:	8b 40 18             	mov    0x18(%eax),%eax
801045a6:	8b 40 44             	mov    0x44(%eax),%eax
801045a9:	8d 44 98 04          	lea    0x4(%eax,%ebx,4),%eax
801045ad:	89 45 08             	mov    %eax,0x8(%ebp)
}
801045b0:	5b                   	pop    %ebx
801045b1:	5e                   	pop    %esi
801045b2:	5d                   	pop    %ebp
  return fetchint((myproc()->tf->esp) + 4 + 4*n, ip);
801045b3:	e9 38 ff ff ff       	jmp    801044f0 <fetchint>
801045b8:	90                   	nop
801045b9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

801045c0 <argptr>:
// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size bytes.  Check that the pointer
// lies within the process address space.
int
argptr(int n, char **pp, int size)
{
801045c0:	55                   	push   %ebp
801045c1:	89 e5                	mov    %esp,%ebp
801045c3:	56                   	push   %esi
801045c4:	53                   	push   %ebx
801045c5:	83 ec 20             	sub    $0x20,%esp
801045c8:	8b 5d 10             	mov    0x10(%ebp),%ebx
  int i;
  struct proc *curproc = myproc();
801045cb:	e8 d0 f0 ff ff       	call   801036a0 <myproc>
801045d0:	89 c6                	mov    %eax,%esi
 
  if(argint(n, &i) < 0)
801045d2:	8d 45 f4             	lea    -0xc(%ebp),%eax
801045d5:	89 44 24 04          	mov    %eax,0x4(%esp)
801045d9:	8b 45 08             	mov    0x8(%ebp),%eax
801045dc:	89 04 24             	mov    %eax,(%esp)
801045df:	e8 ac ff ff ff       	call   80104590 <argint>
801045e4:	85 c0                	test   %eax,%eax
801045e6:	78 28                	js     80104610 <argptr+0x50>
    return -1;
  if(size < 0 || (uint)i >= curproc->sz || (uint)i+size > curproc->sz)
801045e8:	85 db                	test   %ebx,%ebx
801045ea:	78 24                	js     80104610 <argptr+0x50>
801045ec:	8b 55 f4             	mov    -0xc(%ebp),%edx
801045ef:	8b 06                	mov    (%esi),%eax
801045f1:	39 c2                	cmp    %eax,%edx
801045f3:	73 1b                	jae    80104610 <argptr+0x50>
801045f5:	01 d3                	add    %edx,%ebx
801045f7:	39 d8                	cmp    %ebx,%eax
801045f9:	72 15                	jb     80104610 <argptr+0x50>
    return -1;
  *pp = (char*)i;
801045fb:	8b 45 0c             	mov    0xc(%ebp),%eax
801045fe:	89 10                	mov    %edx,(%eax)
  return 0;
}
80104600:	83 c4 20             	add    $0x20,%esp
  return 0;
80104603:	31 c0                	xor    %eax,%eax
}
80104605:	5b                   	pop    %ebx
80104606:	5e                   	pop    %esi
80104607:	5d                   	pop    %ebp
80104608:	c3                   	ret    
80104609:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80104610:	83 c4 20             	add    $0x20,%esp
    return -1;
80104613:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80104618:	5b                   	pop    %ebx
80104619:	5e                   	pop    %esi
8010461a:	5d                   	pop    %ebp
8010461b:	c3                   	ret    
8010461c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80104620 <argstr>:
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int
argstr(int n, char **pp)
{
80104620:	55                   	push   %ebp
80104621:	89 e5                	mov    %esp,%ebp
80104623:	83 ec 28             	sub    $0x28,%esp
  int addr;
  if(argint(n, &addr) < 0)
80104626:	8d 45 f4             	lea    -0xc(%ebp),%eax
80104629:	89 44 24 04          	mov    %eax,0x4(%esp)
8010462d:	8b 45 08             	mov    0x8(%ebp),%eax
80104630:	89 04 24             	mov    %eax,(%esp)
80104633:	e8 58 ff ff ff       	call   80104590 <argint>
80104638:	85 c0                	test   %eax,%eax
8010463a:	78 14                	js     80104650 <argstr+0x30>
    return -1;
  return fetchstr(addr, pp);
8010463c:	8b 45 0c             	mov    0xc(%ebp),%eax
8010463f:	89 44 24 04          	mov    %eax,0x4(%esp)
80104643:	8b 45 f4             	mov    -0xc(%ebp),%eax
80104646:	89 04 24             	mov    %eax,(%esp)
80104649:	e8 e2 fe ff ff       	call   80104530 <fetchstr>
}
8010464e:	c9                   	leave  
8010464f:	c3                   	ret    
    return -1;
80104650:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80104655:	c9                   	leave  
80104656:	c3                   	ret    
80104657:	89 f6                	mov    %esi,%esi
80104659:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80104660 <syscall>:
[SYS_close]   sys_close,
};

void
syscall(void)
{
80104660:	55                   	push   %ebp
80104661:	89 e5                	mov    %esp,%ebp
80104663:	56                   	push   %esi
80104664:	53                   	push   %ebx
80104665:	83 ec 10             	sub    $0x10,%esp
  int num;
  struct proc *curproc = myproc();
80104668:	e8 33 f0 ff ff       	call   801036a0 <myproc>

  num = curproc->tf->eax;
8010466d:	8b 70 18             	mov    0x18(%eax),%esi
  struct proc *curproc = myproc();
80104670:	89 c3                	mov    %eax,%ebx
  num = curproc->tf->eax;
80104672:	8b 46 1c             	mov    0x1c(%esi),%eax
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
80104675:	8d 50 ff             	lea    -0x1(%eax),%edx
80104678:	83 fa 14             	cmp    $0x14,%edx
8010467b:	77 1b                	ja     80104698 <syscall+0x38>
8010467d:	8b 14 85 20 73 10 80 	mov    -0x7fef8ce0(,%eax,4),%edx
80104684:	85 d2                	test   %edx,%edx
80104686:	74 10                	je     80104698 <syscall+0x38>
    curproc->tf->eax = syscalls[num]();
80104688:	ff d2                	call   *%edx
8010468a:	89 46 1c             	mov    %eax,0x1c(%esi)
  } else {
    cprintf("%d %s: unknown sys call %d\n",
            curproc->pid, curproc->name, num);
    curproc->tf->eax = -1;
  }
}
8010468d:	83 c4 10             	add    $0x10,%esp
80104690:	5b                   	pop    %ebx
80104691:	5e                   	pop    %esi
80104692:	5d                   	pop    %ebp
80104693:	c3                   	ret    
80104694:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    cprintf("%d %s: unknown sys call %d\n",
80104698:	89 44 24 0c          	mov    %eax,0xc(%esp)
            curproc->pid, curproc->name, num);
8010469c:	8d 43 6c             	lea    0x6c(%ebx),%eax
8010469f:	89 44 24 08          	mov    %eax,0x8(%esp)
    cprintf("%d %s: unknown sys call %d\n",
801046a3:	8b 43 10             	mov    0x10(%ebx),%eax
801046a6:	c7 04 24 f1 72 10 80 	movl   $0x801072f1,(%esp)
801046ad:	89 44 24 04          	mov    %eax,0x4(%esp)
801046b1:	e8 9a bf ff ff       	call   80100650 <cprintf>
    curproc->tf->eax = -1;
801046b6:	8b 43 18             	mov    0x18(%ebx),%eax
801046b9:	c7 40 1c ff ff ff ff 	movl   $0xffffffff,0x1c(%eax)
}
801046c0:	83 c4 10             	add    $0x10,%esp
801046c3:	5b                   	pop    %ebx
801046c4:	5e                   	pop    %esi
801046c5:	5d                   	pop    %ebp
801046c6:	c3                   	ret    
801046c7:	66 90                	xchg   %ax,%ax
801046c9:	66 90                	xchg   %ax,%ax
801046cb:	66 90                	xchg   %ax,%ax
801046cd:	66 90                	xchg   %ax,%ax
801046cf:	90                   	nop

801046d0 <fdalloc>:

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static int
fdalloc(struct file *f)
{
801046d0:	55                   	push   %ebp
801046d1:	89 e5                	mov    %esp,%ebp
801046d3:	53                   	push   %ebx
801046d4:	89 c3                	mov    %eax,%ebx
801046d6:	83 ec 04             	sub    $0x4,%esp
  int fd;
  struct proc *curproc = myproc();
801046d9:	e8 c2 ef ff ff       	call   801036a0 <myproc>

  for(fd = 0; fd < NOFILE; fd++){
801046de:	31 d2                	xor    %edx,%edx
    if(curproc->ofile[fd] == 0){
801046e0:	8b 4c 90 28          	mov    0x28(%eax,%edx,4),%ecx
801046e4:	85 c9                	test   %ecx,%ecx
801046e6:	74 18                	je     80104700 <fdalloc+0x30>
  for(fd = 0; fd < NOFILE; fd++){
801046e8:	83 c2 01             	add    $0x1,%edx
801046eb:	83 fa 10             	cmp    $0x10,%edx
801046ee:	75 f0                	jne    801046e0 <fdalloc+0x10>
      curproc->ofile[fd] = f;
      return fd;
    }
  }
  return -1;
}
801046f0:	83 c4 04             	add    $0x4,%esp
  return -1;
801046f3:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
801046f8:	5b                   	pop    %ebx
801046f9:	5d                   	pop    %ebp
801046fa:	c3                   	ret    
801046fb:	90                   	nop
801046fc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      curproc->ofile[fd] = f;
80104700:	89 5c 90 28          	mov    %ebx,0x28(%eax,%edx,4)
}
80104704:	83 c4 04             	add    $0x4,%esp
      return fd;
80104707:	89 d0                	mov    %edx,%eax
}
80104709:	5b                   	pop    %ebx
8010470a:	5d                   	pop    %ebp
8010470b:	c3                   	ret    
8010470c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80104710 <create>:
  return -1;
}

static struct inode*
create(char *path, short type, short major, short minor)
{
80104710:	55                   	push   %ebp
80104711:	89 e5                	mov    %esp,%ebp
80104713:	57                   	push   %edi
80104714:	56                   	push   %esi
80104715:	53                   	push   %ebx
80104716:	83 ec 3c             	sub    $0x3c,%esp
80104719:	89 4d d0             	mov    %ecx,-0x30(%ebp)
8010471c:	8b 4d 08             	mov    0x8(%ebp),%ecx
  struct inode *ip, *dp;
  char name[DIRSIZ];

  if((dp = nameiparent(path, name)) == 0)
8010471f:	8d 5d da             	lea    -0x26(%ebp),%ebx
80104722:	89 5c 24 04          	mov    %ebx,0x4(%esp)
80104726:	89 04 24             	mov    %eax,(%esp)
{
80104729:	89 55 d4             	mov    %edx,-0x2c(%ebp)
8010472c:	89 4d cc             	mov    %ecx,-0x34(%ebp)
  if((dp = nameiparent(path, name)) == 0)
8010472f:	e8 ec d7 ff ff       	call   80101f20 <nameiparent>
80104734:	85 c0                	test   %eax,%eax
80104736:	89 c7                	mov    %eax,%edi
80104738:	0f 84 da 00 00 00    	je     80104818 <create+0x108>
    return 0;
  ilock(dp);
8010473e:	89 04 24             	mov    %eax,(%esp)
80104741:	e8 6a cf ff ff       	call   801016b0 <ilock>

  if((ip = dirlookup(dp, name, 0)) != 0){
80104746:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
8010474d:	00 
8010474e:	89 5c 24 04          	mov    %ebx,0x4(%esp)
80104752:	89 3c 24             	mov    %edi,(%esp)
80104755:	e8 66 d4 ff ff       	call   80101bc0 <dirlookup>
8010475a:	85 c0                	test   %eax,%eax
8010475c:	89 c6                	mov    %eax,%esi
8010475e:	74 40                	je     801047a0 <create+0x90>
    iunlockput(dp);
80104760:	89 3c 24             	mov    %edi,(%esp)
80104763:	e8 a8 d1 ff ff       	call   80101910 <iunlockput>
    ilock(ip);
80104768:	89 34 24             	mov    %esi,(%esp)
8010476b:	e8 40 cf ff ff       	call   801016b0 <ilock>
    if(type == T_FILE && ip->type == T_FILE)
80104770:	66 83 7d d4 02       	cmpw   $0x2,-0x2c(%ebp)
80104775:	75 11                	jne    80104788 <create+0x78>
80104777:	66 83 7e 50 02       	cmpw   $0x2,0x50(%esi)
8010477c:	89 f0                	mov    %esi,%eax
8010477e:	75 08                	jne    80104788 <create+0x78>
    panic("create: dirlink");

  iunlockput(dp);

  return ip;
}
80104780:	83 c4 3c             	add    $0x3c,%esp
80104783:	5b                   	pop    %ebx
80104784:	5e                   	pop    %esi
80104785:	5f                   	pop    %edi
80104786:	5d                   	pop    %ebp
80104787:	c3                   	ret    
    iunlockput(ip);
80104788:	89 34 24             	mov    %esi,(%esp)
8010478b:	e8 80 d1 ff ff       	call   80101910 <iunlockput>
}
80104790:	83 c4 3c             	add    $0x3c,%esp
    return 0;
80104793:	31 c0                	xor    %eax,%eax
}
80104795:	5b                   	pop    %ebx
80104796:	5e                   	pop    %esi
80104797:	5f                   	pop    %edi
80104798:	5d                   	pop    %ebp
80104799:	c3                   	ret    
8010479a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  if((ip = ialloc(dp->dev, type)) == 0)
801047a0:	0f bf 45 d4          	movswl -0x2c(%ebp),%eax
801047a4:	89 44 24 04          	mov    %eax,0x4(%esp)
801047a8:	8b 07                	mov    (%edi),%eax
801047aa:	89 04 24             	mov    %eax,(%esp)
801047ad:	e8 6e cd ff ff       	call   80101520 <ialloc>
801047b2:	85 c0                	test   %eax,%eax
801047b4:	89 c6                	mov    %eax,%esi
801047b6:	0f 84 bf 00 00 00    	je     8010487b <create+0x16b>
  ilock(ip);
801047bc:	89 04 24             	mov    %eax,(%esp)
801047bf:	e8 ec ce ff ff       	call   801016b0 <ilock>
  ip->major = major;
801047c4:	0f b7 45 d0          	movzwl -0x30(%ebp),%eax
801047c8:	66 89 46 52          	mov    %ax,0x52(%esi)
  ip->minor = minor;
801047cc:	0f b7 45 cc          	movzwl -0x34(%ebp),%eax
801047d0:	66 89 46 54          	mov    %ax,0x54(%esi)
  ip->nlink = 1;
801047d4:	b8 01 00 00 00       	mov    $0x1,%eax
801047d9:	66 89 46 56          	mov    %ax,0x56(%esi)
  iupdate(ip);
801047dd:	89 34 24             	mov    %esi,(%esp)
801047e0:	e8 0b ce ff ff       	call   801015f0 <iupdate>
  if(type == T_DIR){  // Create . and .. entries.
801047e5:	66 83 7d d4 01       	cmpw   $0x1,-0x2c(%ebp)
801047ea:	74 34                	je     80104820 <create+0x110>
  if(dirlink(dp, name, ip->inum) < 0)
801047ec:	8b 46 04             	mov    0x4(%esi),%eax
801047ef:	89 5c 24 04          	mov    %ebx,0x4(%esp)
801047f3:	89 3c 24             	mov    %edi,(%esp)
801047f6:	89 44 24 08          	mov    %eax,0x8(%esp)
801047fa:	e8 21 d6 ff ff       	call   80101e20 <dirlink>
801047ff:	85 c0                	test   %eax,%eax
80104801:	78 6c                	js     8010486f <create+0x15f>
  iunlockput(dp);
80104803:	89 3c 24             	mov    %edi,(%esp)
80104806:	e8 05 d1 ff ff       	call   80101910 <iunlockput>
}
8010480b:	83 c4 3c             	add    $0x3c,%esp
  return ip;
8010480e:	89 f0                	mov    %esi,%eax
}
80104810:	5b                   	pop    %ebx
80104811:	5e                   	pop    %esi
80104812:	5f                   	pop    %edi
80104813:	5d                   	pop    %ebp
80104814:	c3                   	ret    
80104815:	8d 76 00             	lea    0x0(%esi),%esi
    return 0;
80104818:	31 c0                	xor    %eax,%eax
8010481a:	e9 61 ff ff ff       	jmp    80104780 <create+0x70>
8010481f:	90                   	nop
    dp->nlink++;  // for ".."
80104820:	66 83 47 56 01       	addw   $0x1,0x56(%edi)
    iupdate(dp);
80104825:	89 3c 24             	mov    %edi,(%esp)
80104828:	e8 c3 cd ff ff       	call   801015f0 <iupdate>
    if(dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
8010482d:	8b 46 04             	mov    0x4(%esi),%eax
80104830:	c7 44 24 04 94 73 10 	movl   $0x80107394,0x4(%esp)
80104837:	80 
80104838:	89 34 24             	mov    %esi,(%esp)
8010483b:	89 44 24 08          	mov    %eax,0x8(%esp)
8010483f:	e8 dc d5 ff ff       	call   80101e20 <dirlink>
80104844:	85 c0                	test   %eax,%eax
80104846:	78 1b                	js     80104863 <create+0x153>
80104848:	8b 47 04             	mov    0x4(%edi),%eax
8010484b:	c7 44 24 04 93 73 10 	movl   $0x80107393,0x4(%esp)
80104852:	80 
80104853:	89 34 24             	mov    %esi,(%esp)
80104856:	89 44 24 08          	mov    %eax,0x8(%esp)
8010485a:	e8 c1 d5 ff ff       	call   80101e20 <dirlink>
8010485f:	85 c0                	test   %eax,%eax
80104861:	79 89                	jns    801047ec <create+0xdc>
      panic("create dots");
80104863:	c7 04 24 87 73 10 80 	movl   $0x80107387,(%esp)
8010486a:	e8 f1 ba ff ff       	call   80100360 <panic>
    panic("create: dirlink");
8010486f:	c7 04 24 96 73 10 80 	movl   $0x80107396,(%esp)
80104876:	e8 e5 ba ff ff       	call   80100360 <panic>
    panic("create: ialloc");
8010487b:	c7 04 24 78 73 10 80 	movl   $0x80107378,(%esp)
80104882:	e8 d9 ba ff ff       	call   80100360 <panic>
80104887:	89 f6                	mov    %esi,%esi
80104889:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80104890 <argfd.constprop.0>:
argfd(int n, int *pfd, struct file **pf)
80104890:	55                   	push   %ebp
80104891:	89 e5                	mov    %esp,%ebp
80104893:	56                   	push   %esi
80104894:	89 c6                	mov    %eax,%esi
80104896:	53                   	push   %ebx
80104897:	89 d3                	mov    %edx,%ebx
80104899:	83 ec 20             	sub    $0x20,%esp
  if(argint(n, &fd) < 0)
8010489c:	8d 45 f4             	lea    -0xc(%ebp),%eax
8010489f:	89 44 24 04          	mov    %eax,0x4(%esp)
801048a3:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
801048aa:	e8 e1 fc ff ff       	call   80104590 <argint>
801048af:	85 c0                	test   %eax,%eax
801048b1:	78 2d                	js     801048e0 <argfd.constprop.0+0x50>
  if(fd < 0 || fd >= NOFILE || (f=myproc()->ofile[fd]) == 0)
801048b3:	83 7d f4 0f          	cmpl   $0xf,-0xc(%ebp)
801048b7:	77 27                	ja     801048e0 <argfd.constprop.0+0x50>
801048b9:	e8 e2 ed ff ff       	call   801036a0 <myproc>
801048be:	8b 55 f4             	mov    -0xc(%ebp),%edx
801048c1:	8b 44 90 28          	mov    0x28(%eax,%edx,4),%eax
801048c5:	85 c0                	test   %eax,%eax
801048c7:	74 17                	je     801048e0 <argfd.constprop.0+0x50>
  if(pfd)
801048c9:	85 f6                	test   %esi,%esi
801048cb:	74 02                	je     801048cf <argfd.constprop.0+0x3f>
    *pfd = fd;
801048cd:	89 16                	mov    %edx,(%esi)
  if(pf)
801048cf:	85 db                	test   %ebx,%ebx
801048d1:	74 1d                	je     801048f0 <argfd.constprop.0+0x60>
    *pf = f;
801048d3:	89 03                	mov    %eax,(%ebx)
  return 0;
801048d5:	31 c0                	xor    %eax,%eax
}
801048d7:	83 c4 20             	add    $0x20,%esp
801048da:	5b                   	pop    %ebx
801048db:	5e                   	pop    %esi
801048dc:	5d                   	pop    %ebp
801048dd:	c3                   	ret    
801048de:	66 90                	xchg   %ax,%ax
801048e0:	83 c4 20             	add    $0x20,%esp
    return -1;
801048e3:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
801048e8:	5b                   	pop    %ebx
801048e9:	5e                   	pop    %esi
801048ea:	5d                   	pop    %ebp
801048eb:	c3                   	ret    
801048ec:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  return 0;
801048f0:	31 c0                	xor    %eax,%eax
801048f2:	eb e3                	jmp    801048d7 <argfd.constprop.0+0x47>
801048f4:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
801048fa:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

80104900 <sys_dup>:
{
80104900:	55                   	push   %ebp
  if(argfd(0, 0, &f) < 0)
80104901:	31 c0                	xor    %eax,%eax
{
80104903:	89 e5                	mov    %esp,%ebp
80104905:	53                   	push   %ebx
80104906:	83 ec 24             	sub    $0x24,%esp
  if(argfd(0, 0, &f) < 0)
80104909:	8d 55 f4             	lea    -0xc(%ebp),%edx
8010490c:	e8 7f ff ff ff       	call   80104890 <argfd.constprop.0>
80104911:	85 c0                	test   %eax,%eax
80104913:	78 23                	js     80104938 <sys_dup+0x38>
  if((fd=fdalloc(f)) < 0)
80104915:	8b 45 f4             	mov    -0xc(%ebp),%eax
80104918:	e8 b3 fd ff ff       	call   801046d0 <fdalloc>
8010491d:	85 c0                	test   %eax,%eax
8010491f:	89 c3                	mov    %eax,%ebx
80104921:	78 15                	js     80104938 <sys_dup+0x38>
  filedup(f);
80104923:	8b 45 f4             	mov    -0xc(%ebp),%eax
80104926:	89 04 24             	mov    %eax,(%esp)
80104929:	e8 b2 c4 ff ff       	call   80100de0 <filedup>
  return fd;
8010492e:	89 d8                	mov    %ebx,%eax
}
80104930:	83 c4 24             	add    $0x24,%esp
80104933:	5b                   	pop    %ebx
80104934:	5d                   	pop    %ebp
80104935:	c3                   	ret    
80104936:	66 90                	xchg   %ax,%ax
    return -1;
80104938:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
8010493d:	eb f1                	jmp    80104930 <sys_dup+0x30>
8010493f:	90                   	nop

80104940 <sys_read>:
{
80104940:	55                   	push   %ebp
  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
80104941:	31 c0                	xor    %eax,%eax
{
80104943:	89 e5                	mov    %esp,%ebp
80104945:	83 ec 28             	sub    $0x28,%esp
  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
80104948:	8d 55 ec             	lea    -0x14(%ebp),%edx
8010494b:	e8 40 ff ff ff       	call   80104890 <argfd.constprop.0>
80104950:	85 c0                	test   %eax,%eax
80104952:	78 54                	js     801049a8 <sys_read+0x68>
80104954:	8d 45 f0             	lea    -0x10(%ebp),%eax
80104957:	89 44 24 04          	mov    %eax,0x4(%esp)
8010495b:	c7 04 24 02 00 00 00 	movl   $0x2,(%esp)
80104962:	e8 29 fc ff ff       	call   80104590 <argint>
80104967:	85 c0                	test   %eax,%eax
80104969:	78 3d                	js     801049a8 <sys_read+0x68>
8010496b:	8b 45 f0             	mov    -0x10(%ebp),%eax
8010496e:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
80104975:	89 44 24 08          	mov    %eax,0x8(%esp)
80104979:	8d 45 f4             	lea    -0xc(%ebp),%eax
8010497c:	89 44 24 04          	mov    %eax,0x4(%esp)
80104980:	e8 3b fc ff ff       	call   801045c0 <argptr>
80104985:	85 c0                	test   %eax,%eax
80104987:	78 1f                	js     801049a8 <sys_read+0x68>
  return fileread(f, p, n);
80104989:	8b 45 f0             	mov    -0x10(%ebp),%eax
8010498c:	89 44 24 08          	mov    %eax,0x8(%esp)
80104990:	8b 45 f4             	mov    -0xc(%ebp),%eax
80104993:	89 44 24 04          	mov    %eax,0x4(%esp)
80104997:	8b 45 ec             	mov    -0x14(%ebp),%eax
8010499a:	89 04 24             	mov    %eax,(%esp)
8010499d:	e8 9e c5 ff ff       	call   80100f40 <fileread>
}
801049a2:	c9                   	leave  
801049a3:	c3                   	ret    
801049a4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    return -1;
801049a8:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
801049ad:	c9                   	leave  
801049ae:	c3                   	ret    
801049af:	90                   	nop

801049b0 <sys_write>:
{
801049b0:	55                   	push   %ebp
  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
801049b1:	31 c0                	xor    %eax,%eax
{
801049b3:	89 e5                	mov    %esp,%ebp
801049b5:	83 ec 28             	sub    $0x28,%esp
  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
801049b8:	8d 55 ec             	lea    -0x14(%ebp),%edx
801049bb:	e8 d0 fe ff ff       	call   80104890 <argfd.constprop.0>
801049c0:	85 c0                	test   %eax,%eax
801049c2:	78 54                	js     80104a18 <sys_write+0x68>
801049c4:	8d 45 f0             	lea    -0x10(%ebp),%eax
801049c7:	89 44 24 04          	mov    %eax,0x4(%esp)
801049cb:	c7 04 24 02 00 00 00 	movl   $0x2,(%esp)
801049d2:	e8 b9 fb ff ff       	call   80104590 <argint>
801049d7:	85 c0                	test   %eax,%eax
801049d9:	78 3d                	js     80104a18 <sys_write+0x68>
801049db:	8b 45 f0             	mov    -0x10(%ebp),%eax
801049de:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
801049e5:	89 44 24 08          	mov    %eax,0x8(%esp)
801049e9:	8d 45 f4             	lea    -0xc(%ebp),%eax
801049ec:	89 44 24 04          	mov    %eax,0x4(%esp)
801049f0:	e8 cb fb ff ff       	call   801045c0 <argptr>
801049f5:	85 c0                	test   %eax,%eax
801049f7:	78 1f                	js     80104a18 <sys_write+0x68>
  return filewrite(f, p, n);
801049f9:	8b 45 f0             	mov    -0x10(%ebp),%eax
801049fc:	89 44 24 08          	mov    %eax,0x8(%esp)
80104a00:	8b 45 f4             	mov    -0xc(%ebp),%eax
80104a03:	89 44 24 04          	mov    %eax,0x4(%esp)
80104a07:	8b 45 ec             	mov    -0x14(%ebp),%eax
80104a0a:	89 04 24             	mov    %eax,(%esp)
80104a0d:	e8 ce c5 ff ff       	call   80100fe0 <filewrite>
}
80104a12:	c9                   	leave  
80104a13:	c3                   	ret    
80104a14:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    return -1;
80104a18:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80104a1d:	c9                   	leave  
80104a1e:	c3                   	ret    
80104a1f:	90                   	nop

80104a20 <sys_close>:
{
80104a20:	55                   	push   %ebp
80104a21:	89 e5                	mov    %esp,%ebp
80104a23:	83 ec 28             	sub    $0x28,%esp
  if(argfd(0, &fd, &f) < 0)
80104a26:	8d 55 f4             	lea    -0xc(%ebp),%edx
80104a29:	8d 45 f0             	lea    -0x10(%ebp),%eax
80104a2c:	e8 5f fe ff ff       	call   80104890 <argfd.constprop.0>
80104a31:	85 c0                	test   %eax,%eax
80104a33:	78 23                	js     80104a58 <sys_close+0x38>
  myproc()->ofile[fd] = 0;
80104a35:	e8 66 ec ff ff       	call   801036a0 <myproc>
80104a3a:	8b 55 f0             	mov    -0x10(%ebp),%edx
80104a3d:	c7 44 90 28 00 00 00 	movl   $0x0,0x28(%eax,%edx,4)
80104a44:	00 
  fileclose(f);
80104a45:	8b 45 f4             	mov    -0xc(%ebp),%eax
80104a48:	89 04 24             	mov    %eax,(%esp)
80104a4b:	e8 e0 c3 ff ff       	call   80100e30 <fileclose>
  return 0;
80104a50:	31 c0                	xor    %eax,%eax
}
80104a52:	c9                   	leave  
80104a53:	c3                   	ret    
80104a54:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    return -1;
80104a58:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80104a5d:	c9                   	leave  
80104a5e:	c3                   	ret    
80104a5f:	90                   	nop

80104a60 <sys_fstat>:
{
80104a60:	55                   	push   %ebp
  if(argfd(0, 0, &f) < 0 || argptr(1, (void*)&st, sizeof(*st)) < 0)
80104a61:	31 c0                	xor    %eax,%eax
{
80104a63:	89 e5                	mov    %esp,%ebp
80104a65:	83 ec 28             	sub    $0x28,%esp
  if(argfd(0, 0, &f) < 0 || argptr(1, (void*)&st, sizeof(*st)) < 0)
80104a68:	8d 55 f0             	lea    -0x10(%ebp),%edx
80104a6b:	e8 20 fe ff ff       	call   80104890 <argfd.constprop.0>
80104a70:	85 c0                	test   %eax,%eax
80104a72:	78 34                	js     80104aa8 <sys_fstat+0x48>
80104a74:	8d 45 f4             	lea    -0xc(%ebp),%eax
80104a77:	c7 44 24 08 14 00 00 	movl   $0x14,0x8(%esp)
80104a7e:	00 
80104a7f:	89 44 24 04          	mov    %eax,0x4(%esp)
80104a83:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
80104a8a:	e8 31 fb ff ff       	call   801045c0 <argptr>
80104a8f:	85 c0                	test   %eax,%eax
80104a91:	78 15                	js     80104aa8 <sys_fstat+0x48>
  return filestat(f, st);
80104a93:	8b 45 f4             	mov    -0xc(%ebp),%eax
80104a96:	89 44 24 04          	mov    %eax,0x4(%esp)
80104a9a:	8b 45 f0             	mov    -0x10(%ebp),%eax
80104a9d:	89 04 24             	mov    %eax,(%esp)
80104aa0:	e8 4b c4 ff ff       	call   80100ef0 <filestat>
}
80104aa5:	c9                   	leave  
80104aa6:	c3                   	ret    
80104aa7:	90                   	nop
    return -1;
80104aa8:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80104aad:	c9                   	leave  
80104aae:	c3                   	ret    
80104aaf:	90                   	nop

80104ab0 <sys_link>:
{
80104ab0:	55                   	push   %ebp
80104ab1:	89 e5                	mov    %esp,%ebp
80104ab3:	57                   	push   %edi
80104ab4:	56                   	push   %esi
80104ab5:	53                   	push   %ebx
80104ab6:	83 ec 3c             	sub    $0x3c,%esp
  if(argstr(0, &old) < 0 || argstr(1, &new) < 0)
80104ab9:	8d 45 d4             	lea    -0x2c(%ebp),%eax
80104abc:	89 44 24 04          	mov    %eax,0x4(%esp)
80104ac0:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
80104ac7:	e8 54 fb ff ff       	call   80104620 <argstr>
80104acc:	85 c0                	test   %eax,%eax
80104ace:	0f 88 e6 00 00 00    	js     80104bba <sys_link+0x10a>
80104ad4:	8d 45 d0             	lea    -0x30(%ebp),%eax
80104ad7:	89 44 24 04          	mov    %eax,0x4(%esp)
80104adb:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
80104ae2:	e8 39 fb ff ff       	call   80104620 <argstr>
80104ae7:	85 c0                	test   %eax,%eax
80104ae9:	0f 88 cb 00 00 00    	js     80104bba <sys_link+0x10a>
  begin_op();
80104aef:	e8 1c e0 ff ff       	call   80102b10 <begin_op>
  if((ip = namei(old)) == 0){
80104af4:	8b 45 d4             	mov    -0x2c(%ebp),%eax
80104af7:	89 04 24             	mov    %eax,(%esp)
80104afa:	e8 01 d4 ff ff       	call   80101f00 <namei>
80104aff:	85 c0                	test   %eax,%eax
80104b01:	89 c3                	mov    %eax,%ebx
80104b03:	0f 84 ac 00 00 00    	je     80104bb5 <sys_link+0x105>
  ilock(ip);
80104b09:	89 04 24             	mov    %eax,(%esp)
80104b0c:	e8 9f cb ff ff       	call   801016b0 <ilock>
  if(ip->type == T_DIR){
80104b11:	66 83 7b 50 01       	cmpw   $0x1,0x50(%ebx)
80104b16:	0f 84 91 00 00 00    	je     80104bad <sys_link+0xfd>
  ip->nlink++;
80104b1c:	66 83 43 56 01       	addw   $0x1,0x56(%ebx)
  if((dp = nameiparent(new, name)) == 0)
80104b21:	8d 7d da             	lea    -0x26(%ebp),%edi
  iupdate(ip);
80104b24:	89 1c 24             	mov    %ebx,(%esp)
80104b27:	e8 c4 ca ff ff       	call   801015f0 <iupdate>
  iunlock(ip);
80104b2c:	89 1c 24             	mov    %ebx,(%esp)
80104b2f:	e8 5c cc ff ff       	call   80101790 <iunlock>
  if((dp = nameiparent(new, name)) == 0)
80104b34:	8b 45 d0             	mov    -0x30(%ebp),%eax
80104b37:	89 7c 24 04          	mov    %edi,0x4(%esp)
80104b3b:	89 04 24             	mov    %eax,(%esp)
80104b3e:	e8 dd d3 ff ff       	call   80101f20 <nameiparent>
80104b43:	85 c0                	test   %eax,%eax
80104b45:	89 c6                	mov    %eax,%esi
80104b47:	74 4f                	je     80104b98 <sys_link+0xe8>
  ilock(dp);
80104b49:	89 04 24             	mov    %eax,(%esp)
80104b4c:	e8 5f cb ff ff       	call   801016b0 <ilock>
  if(dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0){
80104b51:	8b 03                	mov    (%ebx),%eax
80104b53:	39 06                	cmp    %eax,(%esi)
80104b55:	75 39                	jne    80104b90 <sys_link+0xe0>
80104b57:	8b 43 04             	mov    0x4(%ebx),%eax
80104b5a:	89 7c 24 04          	mov    %edi,0x4(%esp)
80104b5e:	89 34 24             	mov    %esi,(%esp)
80104b61:	89 44 24 08          	mov    %eax,0x8(%esp)
80104b65:	e8 b6 d2 ff ff       	call   80101e20 <dirlink>
80104b6a:	85 c0                	test   %eax,%eax
80104b6c:	78 22                	js     80104b90 <sys_link+0xe0>
  iunlockput(dp);
80104b6e:	89 34 24             	mov    %esi,(%esp)
80104b71:	e8 9a cd ff ff       	call   80101910 <iunlockput>
  iput(ip);
80104b76:	89 1c 24             	mov    %ebx,(%esp)
80104b79:	e8 52 cc ff ff       	call   801017d0 <iput>
  end_op();
80104b7e:	e8 fd df ff ff       	call   80102b80 <end_op>
}
80104b83:	83 c4 3c             	add    $0x3c,%esp
  return 0;
80104b86:	31 c0                	xor    %eax,%eax
}
80104b88:	5b                   	pop    %ebx
80104b89:	5e                   	pop    %esi
80104b8a:	5f                   	pop    %edi
80104b8b:	5d                   	pop    %ebp
80104b8c:	c3                   	ret    
80104b8d:	8d 76 00             	lea    0x0(%esi),%esi
    iunlockput(dp);
80104b90:	89 34 24             	mov    %esi,(%esp)
80104b93:	e8 78 cd ff ff       	call   80101910 <iunlockput>
  ilock(ip);
80104b98:	89 1c 24             	mov    %ebx,(%esp)
80104b9b:	e8 10 cb ff ff       	call   801016b0 <ilock>
  ip->nlink--;
80104ba0:	66 83 6b 56 01       	subw   $0x1,0x56(%ebx)
  iupdate(ip);
80104ba5:	89 1c 24             	mov    %ebx,(%esp)
80104ba8:	e8 43 ca ff ff       	call   801015f0 <iupdate>
  iunlockput(ip);
80104bad:	89 1c 24             	mov    %ebx,(%esp)
80104bb0:	e8 5b cd ff ff       	call   80101910 <iunlockput>
  end_op();
80104bb5:	e8 c6 df ff ff       	call   80102b80 <end_op>
}
80104bba:	83 c4 3c             	add    $0x3c,%esp
  return -1;
80104bbd:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80104bc2:	5b                   	pop    %ebx
80104bc3:	5e                   	pop    %esi
80104bc4:	5f                   	pop    %edi
80104bc5:	5d                   	pop    %ebp
80104bc6:	c3                   	ret    
80104bc7:	89 f6                	mov    %esi,%esi
80104bc9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80104bd0 <sys_unlink>:
{
80104bd0:	55                   	push   %ebp
80104bd1:	89 e5                	mov    %esp,%ebp
80104bd3:	57                   	push   %edi
80104bd4:	56                   	push   %esi
80104bd5:	53                   	push   %ebx
80104bd6:	83 ec 5c             	sub    $0x5c,%esp
  if(argstr(0, &path) < 0)
80104bd9:	8d 45 c0             	lea    -0x40(%ebp),%eax
80104bdc:	89 44 24 04          	mov    %eax,0x4(%esp)
80104be0:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
80104be7:	e8 34 fa ff ff       	call   80104620 <argstr>
80104bec:	85 c0                	test   %eax,%eax
80104bee:	0f 88 76 01 00 00    	js     80104d6a <sys_unlink+0x19a>
  begin_op();
80104bf4:	e8 17 df ff ff       	call   80102b10 <begin_op>
  if((dp = nameiparent(path, name)) == 0){
80104bf9:	8b 45 c0             	mov    -0x40(%ebp),%eax
80104bfc:	8d 5d ca             	lea    -0x36(%ebp),%ebx
80104bff:	89 5c 24 04          	mov    %ebx,0x4(%esp)
80104c03:	89 04 24             	mov    %eax,(%esp)
80104c06:	e8 15 d3 ff ff       	call   80101f20 <nameiparent>
80104c0b:	85 c0                	test   %eax,%eax
80104c0d:	89 45 b4             	mov    %eax,-0x4c(%ebp)
80104c10:	0f 84 4f 01 00 00    	je     80104d65 <sys_unlink+0x195>
  ilock(dp);
80104c16:	8b 75 b4             	mov    -0x4c(%ebp),%esi
80104c19:	89 34 24             	mov    %esi,(%esp)
80104c1c:	e8 8f ca ff ff       	call   801016b0 <ilock>
  if(namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
80104c21:	c7 44 24 04 94 73 10 	movl   $0x80107394,0x4(%esp)
80104c28:	80 
80104c29:	89 1c 24             	mov    %ebx,(%esp)
80104c2c:	e8 5f cf ff ff       	call   80101b90 <namecmp>
80104c31:	85 c0                	test   %eax,%eax
80104c33:	0f 84 21 01 00 00    	je     80104d5a <sys_unlink+0x18a>
80104c39:	c7 44 24 04 93 73 10 	movl   $0x80107393,0x4(%esp)
80104c40:	80 
80104c41:	89 1c 24             	mov    %ebx,(%esp)
80104c44:	e8 47 cf ff ff       	call   80101b90 <namecmp>
80104c49:	85 c0                	test   %eax,%eax
80104c4b:	0f 84 09 01 00 00    	je     80104d5a <sys_unlink+0x18a>
  if((ip = dirlookup(dp, name, &off)) == 0)
80104c51:	8d 45 c4             	lea    -0x3c(%ebp),%eax
80104c54:	89 5c 24 04          	mov    %ebx,0x4(%esp)
80104c58:	89 44 24 08          	mov    %eax,0x8(%esp)
80104c5c:	89 34 24             	mov    %esi,(%esp)
80104c5f:	e8 5c cf ff ff       	call   80101bc0 <dirlookup>
80104c64:	85 c0                	test   %eax,%eax
80104c66:	89 c3                	mov    %eax,%ebx
80104c68:	0f 84 ec 00 00 00    	je     80104d5a <sys_unlink+0x18a>
  ilock(ip);
80104c6e:	89 04 24             	mov    %eax,(%esp)
80104c71:	e8 3a ca ff ff       	call   801016b0 <ilock>
  if(ip->nlink < 1)
80104c76:	66 83 7b 56 00       	cmpw   $0x0,0x56(%ebx)
80104c7b:	0f 8e 24 01 00 00    	jle    80104da5 <sys_unlink+0x1d5>
  if(ip->type == T_DIR && !isdirempty(ip)){
80104c81:	66 83 7b 50 01       	cmpw   $0x1,0x50(%ebx)
80104c86:	8d 75 d8             	lea    -0x28(%ebp),%esi
80104c89:	74 7d                	je     80104d08 <sys_unlink+0x138>
  memset(&de, 0, sizeof(de));
80104c8b:	c7 44 24 08 10 00 00 	movl   $0x10,0x8(%esp)
80104c92:	00 
80104c93:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
80104c9a:	00 
80104c9b:	89 34 24             	mov    %esi,(%esp)
80104c9e:	e8 fd f5 ff ff       	call   801042a0 <memset>
  if(writei(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
80104ca3:	8b 45 c4             	mov    -0x3c(%ebp),%eax
80104ca6:	c7 44 24 0c 10 00 00 	movl   $0x10,0xc(%esp)
80104cad:	00 
80104cae:	89 74 24 04          	mov    %esi,0x4(%esp)
80104cb2:	89 44 24 08          	mov    %eax,0x8(%esp)
80104cb6:	8b 45 b4             	mov    -0x4c(%ebp),%eax
80104cb9:	89 04 24             	mov    %eax,(%esp)
80104cbc:	e8 9f cd ff ff       	call   80101a60 <writei>
80104cc1:	83 f8 10             	cmp    $0x10,%eax
80104cc4:	0f 85 cf 00 00 00    	jne    80104d99 <sys_unlink+0x1c9>
  if(ip->type == T_DIR){
80104cca:	66 83 7b 50 01       	cmpw   $0x1,0x50(%ebx)
80104ccf:	0f 84 a3 00 00 00    	je     80104d78 <sys_unlink+0x1a8>
  iunlockput(dp);
80104cd5:	8b 45 b4             	mov    -0x4c(%ebp),%eax
80104cd8:	89 04 24             	mov    %eax,(%esp)
80104cdb:	e8 30 cc ff ff       	call   80101910 <iunlockput>
  ip->nlink--;
80104ce0:	66 83 6b 56 01       	subw   $0x1,0x56(%ebx)
  iupdate(ip);
80104ce5:	89 1c 24             	mov    %ebx,(%esp)
80104ce8:	e8 03 c9 ff ff       	call   801015f0 <iupdate>
  iunlockput(ip);
80104ced:	89 1c 24             	mov    %ebx,(%esp)
80104cf0:	e8 1b cc ff ff       	call   80101910 <iunlockput>
  end_op();
80104cf5:	e8 86 de ff ff       	call   80102b80 <end_op>
}
80104cfa:	83 c4 5c             	add    $0x5c,%esp
  return 0;
80104cfd:	31 c0                	xor    %eax,%eax
}
80104cff:	5b                   	pop    %ebx
80104d00:	5e                   	pop    %esi
80104d01:	5f                   	pop    %edi
80104d02:	5d                   	pop    %ebp
80104d03:	c3                   	ret    
80104d04:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  for(off=2*sizeof(de); off<dp->size; off+=sizeof(de)){
80104d08:	83 7b 58 20          	cmpl   $0x20,0x58(%ebx)
80104d0c:	0f 86 79 ff ff ff    	jbe    80104c8b <sys_unlink+0xbb>
80104d12:	bf 20 00 00 00       	mov    $0x20,%edi
80104d17:	eb 15                	jmp    80104d2e <sys_unlink+0x15e>
80104d19:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80104d20:	8d 57 10             	lea    0x10(%edi),%edx
80104d23:	3b 53 58             	cmp    0x58(%ebx),%edx
80104d26:	0f 83 5f ff ff ff    	jae    80104c8b <sys_unlink+0xbb>
80104d2c:	89 d7                	mov    %edx,%edi
    if(readi(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
80104d2e:	c7 44 24 0c 10 00 00 	movl   $0x10,0xc(%esp)
80104d35:	00 
80104d36:	89 7c 24 08          	mov    %edi,0x8(%esp)
80104d3a:	89 74 24 04          	mov    %esi,0x4(%esp)
80104d3e:	89 1c 24             	mov    %ebx,(%esp)
80104d41:	e8 1a cc ff ff       	call   80101960 <readi>
80104d46:	83 f8 10             	cmp    $0x10,%eax
80104d49:	75 42                	jne    80104d8d <sys_unlink+0x1bd>
    if(de.inum != 0)
80104d4b:	66 83 7d d8 00       	cmpw   $0x0,-0x28(%ebp)
80104d50:	74 ce                	je     80104d20 <sys_unlink+0x150>
    iunlockput(ip);
80104d52:	89 1c 24             	mov    %ebx,(%esp)
80104d55:	e8 b6 cb ff ff       	call   80101910 <iunlockput>
  iunlockput(dp);
80104d5a:	8b 45 b4             	mov    -0x4c(%ebp),%eax
80104d5d:	89 04 24             	mov    %eax,(%esp)
80104d60:	e8 ab cb ff ff       	call   80101910 <iunlockput>
  end_op();
80104d65:	e8 16 de ff ff       	call   80102b80 <end_op>
}
80104d6a:	83 c4 5c             	add    $0x5c,%esp
  return -1;
80104d6d:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80104d72:	5b                   	pop    %ebx
80104d73:	5e                   	pop    %esi
80104d74:	5f                   	pop    %edi
80104d75:	5d                   	pop    %ebp
80104d76:	c3                   	ret    
80104d77:	90                   	nop
    dp->nlink--;
80104d78:	8b 45 b4             	mov    -0x4c(%ebp),%eax
80104d7b:	66 83 68 56 01       	subw   $0x1,0x56(%eax)
    iupdate(dp);
80104d80:	89 04 24             	mov    %eax,(%esp)
80104d83:	e8 68 c8 ff ff       	call   801015f0 <iupdate>
80104d88:	e9 48 ff ff ff       	jmp    80104cd5 <sys_unlink+0x105>
      panic("isdirempty: readi");
80104d8d:	c7 04 24 b8 73 10 80 	movl   $0x801073b8,(%esp)
80104d94:	e8 c7 b5 ff ff       	call   80100360 <panic>
    panic("unlink: writei");
80104d99:	c7 04 24 ca 73 10 80 	movl   $0x801073ca,(%esp)
80104da0:	e8 bb b5 ff ff       	call   80100360 <panic>
    panic("unlink: nlink < 1");
80104da5:	c7 04 24 a6 73 10 80 	movl   $0x801073a6,(%esp)
80104dac:	e8 af b5 ff ff       	call   80100360 <panic>
80104db1:	eb 0d                	jmp    80104dc0 <sys_open>
80104db3:	90                   	nop
80104db4:	90                   	nop
80104db5:	90                   	nop
80104db6:	90                   	nop
80104db7:	90                   	nop
80104db8:	90                   	nop
80104db9:	90                   	nop
80104dba:	90                   	nop
80104dbb:	90                   	nop
80104dbc:	90                   	nop
80104dbd:	90                   	nop
80104dbe:	90                   	nop
80104dbf:	90                   	nop

80104dc0 <sys_open>:

int
sys_open(void)
{
80104dc0:	55                   	push   %ebp
80104dc1:	89 e5                	mov    %esp,%ebp
80104dc3:	57                   	push   %edi
80104dc4:	56                   	push   %esi
80104dc5:	53                   	push   %ebx
80104dc6:	83 ec 2c             	sub    $0x2c,%esp
  char *path;
  int fd, omode;
  struct file *f;
  struct inode *ip;

  if(argstr(0, &path) < 0 || argint(1, &omode) < 0)
80104dc9:	8d 45 e0             	lea    -0x20(%ebp),%eax
80104dcc:	89 44 24 04          	mov    %eax,0x4(%esp)
80104dd0:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
80104dd7:	e8 44 f8 ff ff       	call   80104620 <argstr>
80104ddc:	85 c0                	test   %eax,%eax
80104dde:	0f 88 d1 00 00 00    	js     80104eb5 <sys_open+0xf5>
80104de4:	8d 45 e4             	lea    -0x1c(%ebp),%eax
80104de7:	89 44 24 04          	mov    %eax,0x4(%esp)
80104deb:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
80104df2:	e8 99 f7 ff ff       	call   80104590 <argint>
80104df7:	85 c0                	test   %eax,%eax
80104df9:	0f 88 b6 00 00 00    	js     80104eb5 <sys_open+0xf5>
    return -1;

  begin_op();
80104dff:	e8 0c dd ff ff       	call   80102b10 <begin_op>

  if(omode & O_CREATE){
80104e04:	f6 45 e5 02          	testb  $0x2,-0x1b(%ebp)
80104e08:	0f 85 82 00 00 00    	jne    80104e90 <sys_open+0xd0>
    if(ip == 0){
      end_op();
      return -1;
    }
  } else {
    if((ip = namei(path)) == 0){
80104e0e:	8b 45 e0             	mov    -0x20(%ebp),%eax
80104e11:	89 04 24             	mov    %eax,(%esp)
80104e14:	e8 e7 d0 ff ff       	call   80101f00 <namei>
80104e19:	85 c0                	test   %eax,%eax
80104e1b:	89 c6                	mov    %eax,%esi
80104e1d:	0f 84 8d 00 00 00    	je     80104eb0 <sys_open+0xf0>
      end_op();
      return -1;
    }
    ilock(ip);
80104e23:	89 04 24             	mov    %eax,(%esp)
80104e26:	e8 85 c8 ff ff       	call   801016b0 <ilock>
    if(ip->type == T_DIR && omode != O_RDONLY){
80104e2b:	66 83 7e 50 01       	cmpw   $0x1,0x50(%esi)
80104e30:	0f 84 92 00 00 00    	je     80104ec8 <sys_open+0x108>
      end_op();
      return -1;
    }
  }

  if((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0){
80104e36:	e8 35 bf ff ff       	call   80100d70 <filealloc>
80104e3b:	85 c0                	test   %eax,%eax
80104e3d:	89 c3                	mov    %eax,%ebx
80104e3f:	0f 84 93 00 00 00    	je     80104ed8 <sys_open+0x118>
80104e45:	e8 86 f8 ff ff       	call   801046d0 <fdalloc>
80104e4a:	85 c0                	test   %eax,%eax
80104e4c:	89 c7                	mov    %eax,%edi
80104e4e:	0f 88 94 00 00 00    	js     80104ee8 <sys_open+0x128>
      fileclose(f);
    iunlockput(ip);
    end_op();
    return -1;
  }
  iunlock(ip);
80104e54:	89 34 24             	mov    %esi,(%esp)
80104e57:	e8 34 c9 ff ff       	call   80101790 <iunlock>
  end_op();
80104e5c:	e8 1f dd ff ff       	call   80102b80 <end_op>

  f->type = FD_INODE;
80104e61:	c7 03 02 00 00 00    	movl   $0x2,(%ebx)
  f->ip = ip;
  f->off = 0;
  f->readable = !(omode & O_WRONLY);
80104e67:	8b 45 e4             	mov    -0x1c(%ebp),%eax
  f->ip = ip;
80104e6a:	89 73 10             	mov    %esi,0x10(%ebx)
  f->off = 0;
80104e6d:	c7 43 14 00 00 00 00 	movl   $0x0,0x14(%ebx)
  f->readable = !(omode & O_WRONLY);
80104e74:	89 c2                	mov    %eax,%edx
80104e76:	83 e2 01             	and    $0x1,%edx
80104e79:	83 f2 01             	xor    $0x1,%edx
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
80104e7c:	a8 03                	test   $0x3,%al
  f->readable = !(omode & O_WRONLY);
80104e7e:	88 53 08             	mov    %dl,0x8(%ebx)
  return fd;
80104e81:	89 f8                	mov    %edi,%eax
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
80104e83:	0f 95 43 09          	setne  0x9(%ebx)
}
80104e87:	83 c4 2c             	add    $0x2c,%esp
80104e8a:	5b                   	pop    %ebx
80104e8b:	5e                   	pop    %esi
80104e8c:	5f                   	pop    %edi
80104e8d:	5d                   	pop    %ebp
80104e8e:	c3                   	ret    
80104e8f:	90                   	nop
    ip = create(path, T_FILE, 0, 0);
80104e90:	8b 45 e0             	mov    -0x20(%ebp),%eax
80104e93:	31 c9                	xor    %ecx,%ecx
80104e95:	ba 02 00 00 00       	mov    $0x2,%edx
80104e9a:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
80104ea1:	e8 6a f8 ff ff       	call   80104710 <create>
    if(ip == 0){
80104ea6:	85 c0                	test   %eax,%eax
    ip = create(path, T_FILE, 0, 0);
80104ea8:	89 c6                	mov    %eax,%esi
    if(ip == 0){
80104eaa:	75 8a                	jne    80104e36 <sys_open+0x76>
80104eac:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    end_op();
80104eb0:	e8 cb dc ff ff       	call   80102b80 <end_op>
}
80104eb5:	83 c4 2c             	add    $0x2c,%esp
    return -1;
80104eb8:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80104ebd:	5b                   	pop    %ebx
80104ebe:	5e                   	pop    %esi
80104ebf:	5f                   	pop    %edi
80104ec0:	5d                   	pop    %ebp
80104ec1:	c3                   	ret    
80104ec2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    if(ip->type == T_DIR && omode != O_RDONLY){
80104ec8:	8b 45 e4             	mov    -0x1c(%ebp),%eax
80104ecb:	85 c0                	test   %eax,%eax
80104ecd:	0f 84 63 ff ff ff    	je     80104e36 <sys_open+0x76>
80104ed3:	90                   	nop
80104ed4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    iunlockput(ip);
80104ed8:	89 34 24             	mov    %esi,(%esp)
80104edb:	e8 30 ca ff ff       	call   80101910 <iunlockput>
80104ee0:	eb ce                	jmp    80104eb0 <sys_open+0xf0>
80104ee2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
      fileclose(f);
80104ee8:	89 1c 24             	mov    %ebx,(%esp)
80104eeb:	e8 40 bf ff ff       	call   80100e30 <fileclose>
80104ef0:	eb e6                	jmp    80104ed8 <sys_open+0x118>
80104ef2:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80104ef9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80104f00 <sys_mkdir>:

int
sys_mkdir(void)
{
80104f00:	55                   	push   %ebp
80104f01:	89 e5                	mov    %esp,%ebp
80104f03:	83 ec 28             	sub    $0x28,%esp
  char *path;
  struct inode *ip;

  begin_op();
80104f06:	e8 05 dc ff ff       	call   80102b10 <begin_op>
  if(argstr(0, &path) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0){
80104f0b:	8d 45 f4             	lea    -0xc(%ebp),%eax
80104f0e:	89 44 24 04          	mov    %eax,0x4(%esp)
80104f12:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
80104f19:	e8 02 f7 ff ff       	call   80104620 <argstr>
80104f1e:	85 c0                	test   %eax,%eax
80104f20:	78 2e                	js     80104f50 <sys_mkdir+0x50>
80104f22:	8b 45 f4             	mov    -0xc(%ebp),%eax
80104f25:	31 c9                	xor    %ecx,%ecx
80104f27:	ba 01 00 00 00       	mov    $0x1,%edx
80104f2c:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
80104f33:	e8 d8 f7 ff ff       	call   80104710 <create>
80104f38:	85 c0                	test   %eax,%eax
80104f3a:	74 14                	je     80104f50 <sys_mkdir+0x50>
    end_op();
    return -1;
  }
  iunlockput(ip);
80104f3c:	89 04 24             	mov    %eax,(%esp)
80104f3f:	e8 cc c9 ff ff       	call   80101910 <iunlockput>
  end_op();
80104f44:	e8 37 dc ff ff       	call   80102b80 <end_op>
  return 0;
80104f49:	31 c0                	xor    %eax,%eax
}
80104f4b:	c9                   	leave  
80104f4c:	c3                   	ret    
80104f4d:	8d 76 00             	lea    0x0(%esi),%esi
    end_op();
80104f50:	e8 2b dc ff ff       	call   80102b80 <end_op>
    return -1;
80104f55:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80104f5a:	c9                   	leave  
80104f5b:	c3                   	ret    
80104f5c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80104f60 <sys_mknod>:

int
sys_mknod(void)
{
80104f60:	55                   	push   %ebp
80104f61:	89 e5                	mov    %esp,%ebp
80104f63:	83 ec 28             	sub    $0x28,%esp
  struct inode *ip;
  char *path;
  int major, minor;

  begin_op();
80104f66:	e8 a5 db ff ff       	call   80102b10 <begin_op>
  if((argstr(0, &path)) < 0 ||
80104f6b:	8d 45 ec             	lea    -0x14(%ebp),%eax
80104f6e:	89 44 24 04          	mov    %eax,0x4(%esp)
80104f72:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
80104f79:	e8 a2 f6 ff ff       	call   80104620 <argstr>
80104f7e:	85 c0                	test   %eax,%eax
80104f80:	78 5e                	js     80104fe0 <sys_mknod+0x80>
     argint(1, &major) < 0 ||
80104f82:	8d 45 f0             	lea    -0x10(%ebp),%eax
80104f85:	89 44 24 04          	mov    %eax,0x4(%esp)
80104f89:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
80104f90:	e8 fb f5 ff ff       	call   80104590 <argint>
  if((argstr(0, &path)) < 0 ||
80104f95:	85 c0                	test   %eax,%eax
80104f97:	78 47                	js     80104fe0 <sys_mknod+0x80>
     argint(2, &minor) < 0 ||
80104f99:	8d 45 f4             	lea    -0xc(%ebp),%eax
80104f9c:	89 44 24 04          	mov    %eax,0x4(%esp)
80104fa0:	c7 04 24 02 00 00 00 	movl   $0x2,(%esp)
80104fa7:	e8 e4 f5 ff ff       	call   80104590 <argint>
     argint(1, &major) < 0 ||
80104fac:	85 c0                	test   %eax,%eax
80104fae:	78 30                	js     80104fe0 <sys_mknod+0x80>
     (ip = create(path, T_DEV, major, minor)) == 0){
80104fb0:	0f bf 45 f4          	movswl -0xc(%ebp),%eax
     argint(2, &minor) < 0 ||
80104fb4:	ba 03 00 00 00       	mov    $0x3,%edx
     (ip = create(path, T_DEV, major, minor)) == 0){
80104fb9:	0f bf 4d f0          	movswl -0x10(%ebp),%ecx
80104fbd:	89 04 24             	mov    %eax,(%esp)
     argint(2, &minor) < 0 ||
80104fc0:	8b 45 ec             	mov    -0x14(%ebp),%eax
80104fc3:	e8 48 f7 ff ff       	call   80104710 <create>
80104fc8:	85 c0                	test   %eax,%eax
80104fca:	74 14                	je     80104fe0 <sys_mknod+0x80>
    end_op();
    return -1;
  }
  iunlockput(ip);
80104fcc:	89 04 24             	mov    %eax,(%esp)
80104fcf:	e8 3c c9 ff ff       	call   80101910 <iunlockput>
  end_op();
80104fd4:	e8 a7 db ff ff       	call   80102b80 <end_op>
  return 0;
80104fd9:	31 c0                	xor    %eax,%eax
}
80104fdb:	c9                   	leave  
80104fdc:	c3                   	ret    
80104fdd:	8d 76 00             	lea    0x0(%esi),%esi
    end_op();
80104fe0:	e8 9b db ff ff       	call   80102b80 <end_op>
    return -1;
80104fe5:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80104fea:	c9                   	leave  
80104feb:	c3                   	ret    
80104fec:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80104ff0 <sys_chdir>:

int
sys_chdir(void)
{
80104ff0:	55                   	push   %ebp
80104ff1:	89 e5                	mov    %esp,%ebp
80104ff3:	56                   	push   %esi
80104ff4:	53                   	push   %ebx
80104ff5:	83 ec 20             	sub    $0x20,%esp
  char *path;
  struct inode *ip;
  struct proc *curproc = myproc();
80104ff8:	e8 a3 e6 ff ff       	call   801036a0 <myproc>
80104ffd:	89 c6                	mov    %eax,%esi
  
  begin_op();
80104fff:	e8 0c db ff ff       	call   80102b10 <begin_op>
  if(argstr(0, &path) < 0 || (ip = namei(path)) == 0){
80105004:	8d 45 f4             	lea    -0xc(%ebp),%eax
80105007:	89 44 24 04          	mov    %eax,0x4(%esp)
8010500b:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
80105012:	e8 09 f6 ff ff       	call   80104620 <argstr>
80105017:	85 c0                	test   %eax,%eax
80105019:	78 4a                	js     80105065 <sys_chdir+0x75>
8010501b:	8b 45 f4             	mov    -0xc(%ebp),%eax
8010501e:	89 04 24             	mov    %eax,(%esp)
80105021:	e8 da ce ff ff       	call   80101f00 <namei>
80105026:	85 c0                	test   %eax,%eax
80105028:	89 c3                	mov    %eax,%ebx
8010502a:	74 39                	je     80105065 <sys_chdir+0x75>
    end_op();
    return -1;
  }
  ilock(ip);
8010502c:	89 04 24             	mov    %eax,(%esp)
8010502f:	e8 7c c6 ff ff       	call   801016b0 <ilock>
  if(ip->type != T_DIR){
80105034:	66 83 7b 50 01       	cmpw   $0x1,0x50(%ebx)
    iunlockput(ip);
80105039:	89 1c 24             	mov    %ebx,(%esp)
  if(ip->type != T_DIR){
8010503c:	75 22                	jne    80105060 <sys_chdir+0x70>
    end_op();
    return -1;
  }
  iunlock(ip);
8010503e:	e8 4d c7 ff ff       	call   80101790 <iunlock>
  iput(curproc->cwd);
80105043:	8b 46 68             	mov    0x68(%esi),%eax
80105046:	89 04 24             	mov    %eax,(%esp)
80105049:	e8 82 c7 ff ff       	call   801017d0 <iput>
  end_op();
8010504e:	e8 2d db ff ff       	call   80102b80 <end_op>
  curproc->cwd = ip;
  return 0;
80105053:	31 c0                	xor    %eax,%eax
  curproc->cwd = ip;
80105055:	89 5e 68             	mov    %ebx,0x68(%esi)
}
80105058:	83 c4 20             	add    $0x20,%esp
8010505b:	5b                   	pop    %ebx
8010505c:	5e                   	pop    %esi
8010505d:	5d                   	pop    %ebp
8010505e:	c3                   	ret    
8010505f:	90                   	nop
    iunlockput(ip);
80105060:	e8 ab c8 ff ff       	call   80101910 <iunlockput>
    end_op();
80105065:	e8 16 db ff ff       	call   80102b80 <end_op>
}
8010506a:	83 c4 20             	add    $0x20,%esp
    return -1;
8010506d:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80105072:	5b                   	pop    %ebx
80105073:	5e                   	pop    %esi
80105074:	5d                   	pop    %ebp
80105075:	c3                   	ret    
80105076:	8d 76 00             	lea    0x0(%esi),%esi
80105079:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80105080 <sys_exec>:

int
sys_exec(void)
{
80105080:	55                   	push   %ebp
80105081:	89 e5                	mov    %esp,%ebp
80105083:	57                   	push   %edi
80105084:	56                   	push   %esi
80105085:	53                   	push   %ebx
80105086:	81 ec ac 00 00 00    	sub    $0xac,%esp
  char *path, *argv[MAXARG];
  int i;
  uint uargv, uarg;

  if(argstr(0, &path) < 0 || argint(1, (int*)&uargv) < 0){
8010508c:	8d 85 5c ff ff ff    	lea    -0xa4(%ebp),%eax
80105092:	89 44 24 04          	mov    %eax,0x4(%esp)
80105096:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
8010509d:	e8 7e f5 ff ff       	call   80104620 <argstr>
801050a2:	85 c0                	test   %eax,%eax
801050a4:	0f 88 84 00 00 00    	js     8010512e <sys_exec+0xae>
801050aa:	8d 85 60 ff ff ff    	lea    -0xa0(%ebp),%eax
801050b0:	89 44 24 04          	mov    %eax,0x4(%esp)
801050b4:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
801050bb:	e8 d0 f4 ff ff       	call   80104590 <argint>
801050c0:	85 c0                	test   %eax,%eax
801050c2:	78 6a                	js     8010512e <sys_exec+0xae>
    return -1;
  }
  memset(argv, 0, sizeof(argv));
801050c4:	8d 85 68 ff ff ff    	lea    -0x98(%ebp),%eax
  for(i=0;; i++){
801050ca:	31 db                	xor    %ebx,%ebx
  memset(argv, 0, sizeof(argv));
801050cc:	c7 44 24 08 80 00 00 	movl   $0x80,0x8(%esp)
801050d3:	00 
801050d4:	8d b5 68 ff ff ff    	lea    -0x98(%ebp),%esi
801050da:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
801050e1:	00 
801050e2:	8d bd 64 ff ff ff    	lea    -0x9c(%ebp),%edi
801050e8:	89 04 24             	mov    %eax,(%esp)
801050eb:	e8 b0 f1 ff ff       	call   801042a0 <memset>
    if(i >= NELEM(argv))
      return -1;
    if(fetchint(uargv+4*i, (int*)&uarg) < 0)
801050f0:	8b 85 60 ff ff ff    	mov    -0xa0(%ebp),%eax
801050f6:	89 7c 24 04          	mov    %edi,0x4(%esp)
801050fa:	8d 04 98             	lea    (%eax,%ebx,4),%eax
801050fd:	89 04 24             	mov    %eax,(%esp)
80105100:	e8 eb f3 ff ff       	call   801044f0 <fetchint>
80105105:	85 c0                	test   %eax,%eax
80105107:	78 25                	js     8010512e <sys_exec+0xae>
      return -1;
    if(uarg == 0){
80105109:	8b 85 64 ff ff ff    	mov    -0x9c(%ebp),%eax
8010510f:	85 c0                	test   %eax,%eax
80105111:	74 2d                	je     80105140 <sys_exec+0xc0>
      argv[i] = 0;
      break;
    }
    if(fetchstr(uarg, &argv[i]) < 0)
80105113:	89 74 24 04          	mov    %esi,0x4(%esp)
80105117:	89 04 24             	mov    %eax,(%esp)
8010511a:	e8 11 f4 ff ff       	call   80104530 <fetchstr>
8010511f:	85 c0                	test   %eax,%eax
80105121:	78 0b                	js     8010512e <sys_exec+0xae>
  for(i=0;; i++){
80105123:	83 c3 01             	add    $0x1,%ebx
80105126:	83 c6 04             	add    $0x4,%esi
    if(i >= NELEM(argv))
80105129:	83 fb 20             	cmp    $0x20,%ebx
8010512c:	75 c2                	jne    801050f0 <sys_exec+0x70>
      return -1;
  }
  return exec(path, argv);
}
8010512e:	81 c4 ac 00 00 00    	add    $0xac,%esp
    return -1;
80105134:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80105139:	5b                   	pop    %ebx
8010513a:	5e                   	pop    %esi
8010513b:	5f                   	pop    %edi
8010513c:	5d                   	pop    %ebp
8010513d:	c3                   	ret    
8010513e:	66 90                	xchg   %ax,%ax
  return exec(path, argv);
80105140:	8d 85 68 ff ff ff    	lea    -0x98(%ebp),%eax
80105146:	89 44 24 04          	mov    %eax,0x4(%esp)
8010514a:	8b 85 5c ff ff ff    	mov    -0xa4(%ebp),%eax
      argv[i] = 0;
80105150:	c7 84 9d 68 ff ff ff 	movl   $0x0,-0x98(%ebp,%ebx,4)
80105157:	00 00 00 00 
  return exec(path, argv);
8010515b:	89 04 24             	mov    %eax,(%esp)
8010515e:	e8 3d b8 ff ff       	call   801009a0 <exec>
}
80105163:	81 c4 ac 00 00 00    	add    $0xac,%esp
80105169:	5b                   	pop    %ebx
8010516a:	5e                   	pop    %esi
8010516b:	5f                   	pop    %edi
8010516c:	5d                   	pop    %ebp
8010516d:	c3                   	ret    
8010516e:	66 90                	xchg   %ax,%ax

80105170 <sys_pipe>:

int
sys_pipe(void)
{
80105170:	55                   	push   %ebp
80105171:	89 e5                	mov    %esp,%ebp
80105173:	53                   	push   %ebx
80105174:	83 ec 24             	sub    $0x24,%esp
  int *fd;
  struct file *rf, *wf;
  int fd0, fd1;

  if(argptr(0, (void*)&fd, 2*sizeof(fd[0])) < 0)
80105177:	8d 45 ec             	lea    -0x14(%ebp),%eax
8010517a:	c7 44 24 08 08 00 00 	movl   $0x8,0x8(%esp)
80105181:	00 
80105182:	89 44 24 04          	mov    %eax,0x4(%esp)
80105186:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
8010518d:	e8 2e f4 ff ff       	call   801045c0 <argptr>
80105192:	85 c0                	test   %eax,%eax
80105194:	78 6d                	js     80105203 <sys_pipe+0x93>
    return -1;
  if(pipealloc(&rf, &wf) < 0)
80105196:	8d 45 f4             	lea    -0xc(%ebp),%eax
80105199:	89 44 24 04          	mov    %eax,0x4(%esp)
8010519d:	8d 45 f0             	lea    -0x10(%ebp),%eax
801051a0:	89 04 24             	mov    %eax,(%esp)
801051a3:	e8 c8 df ff ff       	call   80103170 <pipealloc>
801051a8:	85 c0                	test   %eax,%eax
801051aa:	78 57                	js     80105203 <sys_pipe+0x93>
    return -1;
  fd0 = -1;
  if((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0){
801051ac:	8b 45 f0             	mov    -0x10(%ebp),%eax
801051af:	e8 1c f5 ff ff       	call   801046d0 <fdalloc>
801051b4:	85 c0                	test   %eax,%eax
801051b6:	89 c3                	mov    %eax,%ebx
801051b8:	78 33                	js     801051ed <sys_pipe+0x7d>
801051ba:	8b 45 f4             	mov    -0xc(%ebp),%eax
801051bd:	e8 0e f5 ff ff       	call   801046d0 <fdalloc>
801051c2:	85 c0                	test   %eax,%eax
801051c4:	78 1a                	js     801051e0 <sys_pipe+0x70>
      myproc()->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    return -1;
  }
  fd[0] = fd0;
801051c6:	8b 55 ec             	mov    -0x14(%ebp),%edx
801051c9:	89 1a                	mov    %ebx,(%edx)
  fd[1] = fd1;
801051cb:	8b 55 ec             	mov    -0x14(%ebp),%edx
801051ce:	89 42 04             	mov    %eax,0x4(%edx)
  return 0;
}
801051d1:	83 c4 24             	add    $0x24,%esp
  return 0;
801051d4:	31 c0                	xor    %eax,%eax
}
801051d6:	5b                   	pop    %ebx
801051d7:	5d                   	pop    %ebp
801051d8:	c3                   	ret    
801051d9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
      myproc()->ofile[fd0] = 0;
801051e0:	e8 bb e4 ff ff       	call   801036a0 <myproc>
801051e5:	c7 44 98 28 00 00 00 	movl   $0x0,0x28(%eax,%ebx,4)
801051ec:	00 
    fileclose(rf);
801051ed:	8b 45 f0             	mov    -0x10(%ebp),%eax
801051f0:	89 04 24             	mov    %eax,(%esp)
801051f3:	e8 38 bc ff ff       	call   80100e30 <fileclose>
    fileclose(wf);
801051f8:	8b 45 f4             	mov    -0xc(%ebp),%eax
801051fb:	89 04 24             	mov    %eax,(%esp)
801051fe:	e8 2d bc ff ff       	call   80100e30 <fileclose>
}
80105203:	83 c4 24             	add    $0x24,%esp
    return -1;
80105206:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
8010520b:	5b                   	pop    %ebx
8010520c:	5d                   	pop    %ebp
8010520d:	c3                   	ret    
8010520e:	66 90                	xchg   %ax,%ax

80105210 <sys_fork>:
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int sys_fork(void)
{
80105210:	55                   	push   %ebp
80105211:	89 e5                	mov    %esp,%ebp
  return fork();
}
80105213:	5d                   	pop    %ebp
  return fork();
80105214:	e9 37 e6 ff ff       	jmp    80103850 <fork>
80105219:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

80105220 <sys_exit>:

int sys_exit(void)
{
80105220:	55                   	push   %ebp
80105221:	89 e5                	mov    %esp,%ebp
80105223:	83 ec 28             	sub    $0x28,%esp
  int status;
  argint(0, &status);
80105226:	8d 45 f4             	lea    -0xc(%ebp),%eax
80105229:	89 44 24 04          	mov    %eax,0x4(%esp)
8010522d:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
80105234:	e8 57 f3 ff ff       	call   80104590 <argint>
  exit(status);
80105239:	8b 45 f4             	mov    -0xc(%ebp),%eax
8010523c:	89 04 24             	mov    %eax,(%esp)
8010523f:	e8 5c e8 ff ff       	call   80103aa0 <exit>
  return 0; // not reached
}
80105244:	31 c0                	xor    %eax,%eax
80105246:	c9                   	leave  
80105247:	c3                   	ret    
80105248:	90                   	nop
80105249:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

80105250 <sys_wait>:

int sys_wait(void)
{
80105250:	55                   	push   %ebp
80105251:	89 e5                	mov    %esp,%ebp
80105253:	83 ec 28             	sub    $0x28,%esp
  int *status;
  if (argptr(0, (void *)&status, sizeof(status)) < 0)
80105256:	8d 45 f4             	lea    -0xc(%ebp),%eax
80105259:	c7 44 24 08 04 00 00 	movl   $0x4,0x8(%esp)
80105260:	00 
80105261:	89 44 24 04          	mov    %eax,0x4(%esp)
80105265:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
8010526c:	e8 4f f3 ff ff       	call   801045c0 <argptr>
80105271:	85 c0                	test   %eax,%eax
80105273:	78 13                	js     80105288 <sys_wait+0x38>
  {
    return -1;
  }

  return wait(status);
80105275:	8b 45 f4             	mov    -0xc(%ebp),%eax
80105278:	89 04 24             	mov    %eax,(%esp)
8010527b:	e8 30 ea ff ff       	call   80103cb0 <wait>
}
80105280:	c9                   	leave  
80105281:	c3                   	ret    
80105282:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    return -1;
80105288:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
8010528d:	c9                   	leave  
8010528e:	c3                   	ret    
8010528f:	90                   	nop

80105290 <sys_kill>:

int sys_kill(void)
{
80105290:	55                   	push   %ebp
80105291:	89 e5                	mov    %esp,%ebp
80105293:	83 ec 28             	sub    $0x28,%esp
  int pid;

  if (argint(0, &pid) < 0)
80105296:	8d 45 f4             	lea    -0xc(%ebp),%eax
80105299:	89 44 24 04          	mov    %eax,0x4(%esp)
8010529d:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
801052a4:	e8 e7 f2 ff ff       	call   80104590 <argint>
801052a9:	85 c0                	test   %eax,%eax
801052ab:	78 13                	js     801052c0 <sys_kill+0x30>
    return -1;
  return kill(pid);
801052ad:	8b 45 f4             	mov    -0xc(%ebp),%eax
801052b0:	89 04 24             	mov    %eax,(%esp)
801052b3:	e8 38 eb ff ff       	call   80103df0 <kill>
}
801052b8:	c9                   	leave  
801052b9:	c3                   	ret    
801052ba:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    return -1;
801052c0:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
801052c5:	c9                   	leave  
801052c6:	c3                   	ret    
801052c7:	89 f6                	mov    %esi,%esi
801052c9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801052d0 <sys_getpid>:

int sys_getpid(void)
{
801052d0:	55                   	push   %ebp
801052d1:	89 e5                	mov    %esp,%ebp
801052d3:	83 ec 08             	sub    $0x8,%esp
  return myproc()->pid;
801052d6:	e8 c5 e3 ff ff       	call   801036a0 <myproc>
801052db:	8b 40 10             	mov    0x10(%eax),%eax
}
801052de:	c9                   	leave  
801052df:	c3                   	ret    

801052e0 <sys_sbrk>:

int sys_sbrk(void)
{
801052e0:	55                   	push   %ebp
801052e1:	89 e5                	mov    %esp,%ebp
801052e3:	53                   	push   %ebx
801052e4:	83 ec 24             	sub    $0x24,%esp
  int addr;
  int n;

  if (argint(0, &n) < 0)
801052e7:	8d 45 f4             	lea    -0xc(%ebp),%eax
801052ea:	89 44 24 04          	mov    %eax,0x4(%esp)
801052ee:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
801052f5:	e8 96 f2 ff ff       	call   80104590 <argint>
801052fa:	85 c0                	test   %eax,%eax
801052fc:	78 22                	js     80105320 <sys_sbrk+0x40>
    return -1;
  addr = myproc()->sz;
801052fe:	e8 9d e3 ff ff       	call   801036a0 <myproc>
  if (growproc(n) < 0)
80105303:	8b 55 f4             	mov    -0xc(%ebp),%edx
  addr = myproc()->sz;
80105306:	8b 18                	mov    (%eax),%ebx
  if (growproc(n) < 0)
80105308:	89 14 24             	mov    %edx,(%esp)
8010530b:	e8 d0 e4 ff ff       	call   801037e0 <growproc>
80105310:	85 c0                	test   %eax,%eax
80105312:	78 0c                	js     80105320 <sys_sbrk+0x40>
    return -1;
  return addr;
80105314:	89 d8                	mov    %ebx,%eax
}
80105316:	83 c4 24             	add    $0x24,%esp
80105319:	5b                   	pop    %ebx
8010531a:	5d                   	pop    %ebp
8010531b:	c3                   	ret    
8010531c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    return -1;
80105320:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
80105325:	eb ef                	jmp    80105316 <sys_sbrk+0x36>
80105327:	89 f6                	mov    %esi,%esi
80105329:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80105330 <sys_sleep>:

int sys_sleep(void)
{
80105330:	55                   	push   %ebp
80105331:	89 e5                	mov    %esp,%ebp
80105333:	53                   	push   %ebx
80105334:	83 ec 24             	sub    $0x24,%esp
  int n;
  uint ticks0;

  if (argint(0, &n) < 0)
80105337:	8d 45 f4             	lea    -0xc(%ebp),%eax
8010533a:	89 44 24 04          	mov    %eax,0x4(%esp)
8010533e:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
80105345:	e8 46 f2 ff ff       	call   80104590 <argint>
8010534a:	85 c0                	test   %eax,%eax
8010534c:	78 7e                	js     801053cc <sys_sleep+0x9c>
    return -1;
  acquire(&tickslock);
8010534e:	c7 04 24 60 4d 11 80 	movl   $0x80114d60,(%esp)
80105355:	e8 86 ee ff ff       	call   801041e0 <acquire>
  ticks0 = ticks;
  while (ticks - ticks0 < n)
8010535a:	8b 55 f4             	mov    -0xc(%ebp),%edx
  ticks0 = ticks;
8010535d:	8b 1d a0 55 11 80    	mov    0x801155a0,%ebx
  while (ticks - ticks0 < n)
80105363:	85 d2                	test   %edx,%edx
80105365:	75 29                	jne    80105390 <sys_sleep+0x60>
80105367:	eb 4f                	jmp    801053b8 <sys_sleep+0x88>
80105369:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
    if (myproc()->killed)
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
80105370:	c7 44 24 04 60 4d 11 	movl   $0x80114d60,0x4(%esp)
80105377:	80 
80105378:	c7 04 24 a0 55 11 80 	movl   $0x801155a0,(%esp)
8010537f:	e8 7c e8 ff ff       	call   80103c00 <sleep>
  while (ticks - ticks0 < n)
80105384:	a1 a0 55 11 80       	mov    0x801155a0,%eax
80105389:	29 d8                	sub    %ebx,%eax
8010538b:	3b 45 f4             	cmp    -0xc(%ebp),%eax
8010538e:	73 28                	jae    801053b8 <sys_sleep+0x88>
    if (myproc()->killed)
80105390:	e8 0b e3 ff ff       	call   801036a0 <myproc>
80105395:	8b 40 24             	mov    0x24(%eax),%eax
80105398:	85 c0                	test   %eax,%eax
8010539a:	74 d4                	je     80105370 <sys_sleep+0x40>
      release(&tickslock);
8010539c:	c7 04 24 60 4d 11 80 	movl   $0x80114d60,(%esp)
801053a3:	e8 a8 ee ff ff       	call   80104250 <release>
      return -1;
801053a8:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  }
  release(&tickslock);
  return 0;
}
801053ad:	83 c4 24             	add    $0x24,%esp
801053b0:	5b                   	pop    %ebx
801053b1:	5d                   	pop    %ebp
801053b2:	c3                   	ret    
801053b3:	90                   	nop
801053b4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  release(&tickslock);
801053b8:	c7 04 24 60 4d 11 80 	movl   $0x80114d60,(%esp)
801053bf:	e8 8c ee ff ff       	call   80104250 <release>
}
801053c4:	83 c4 24             	add    $0x24,%esp
  return 0;
801053c7:	31 c0                	xor    %eax,%eax
}
801053c9:	5b                   	pop    %ebx
801053ca:	5d                   	pop    %ebp
801053cb:	c3                   	ret    
    return -1;
801053cc:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
801053d1:	eb da                	jmp    801053ad <sys_sleep+0x7d>
801053d3:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
801053d9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801053e0 <sys_uptime>:

// return how many clock tick interrupts have occurred
// since start.
int sys_uptime(void)
{
801053e0:	55                   	push   %ebp
801053e1:	89 e5                	mov    %esp,%ebp
801053e3:	53                   	push   %ebx
801053e4:	83 ec 14             	sub    $0x14,%esp
  uint xticks;

  acquire(&tickslock);
801053e7:	c7 04 24 60 4d 11 80 	movl   $0x80114d60,(%esp)
801053ee:	e8 ed ed ff ff       	call   801041e0 <acquire>
  xticks = ticks;
801053f3:	8b 1d a0 55 11 80    	mov    0x801155a0,%ebx
  release(&tickslock);
801053f9:	c7 04 24 60 4d 11 80 	movl   $0x80114d60,(%esp)
80105400:	e8 4b ee ff ff       	call   80104250 <release>
  return xticks;
}
80105405:	83 c4 14             	add    $0x14,%esp
80105408:	89 d8                	mov    %ebx,%eax
8010540a:	5b                   	pop    %ebx
8010540b:	5d                   	pop    %ebp
8010540c:	c3                   	ret    

8010540d <alltraps>:
8010540d:	1e                   	push   %ds
8010540e:	06                   	push   %es
8010540f:	0f a0                	push   %fs
80105411:	0f a8                	push   %gs
80105413:	60                   	pusha  
80105414:	66 b8 10 00          	mov    $0x10,%ax
80105418:	8e d8                	mov    %eax,%ds
8010541a:	8e c0                	mov    %eax,%es
8010541c:	54                   	push   %esp
8010541d:	e8 de 00 00 00       	call   80105500 <trap>
80105422:	83 c4 04             	add    $0x4,%esp

80105425 <trapret>:
80105425:	61                   	popa   
80105426:	0f a9                	pop    %gs
80105428:	0f a1                	pop    %fs
8010542a:	07                   	pop    %es
8010542b:	1f                   	pop    %ds
8010542c:	83 c4 08             	add    $0x8,%esp
8010542f:	cf                   	iret   

80105430 <tvinit>:
void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
80105430:	31 c0                	xor    %eax,%eax
80105432:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
80105438:	8b 14 85 08 a0 10 80 	mov    -0x7fef5ff8(,%eax,4),%edx
8010543f:	b9 08 00 00 00       	mov    $0x8,%ecx
80105444:	66 89 0c c5 a2 4d 11 	mov    %cx,-0x7feeb25e(,%eax,8)
8010544b:	80 
8010544c:	c6 04 c5 a4 4d 11 80 	movb   $0x0,-0x7feeb25c(,%eax,8)
80105453:	00 
80105454:	c6 04 c5 a5 4d 11 80 	movb   $0x8e,-0x7feeb25b(,%eax,8)
8010545b:	8e 
8010545c:	66 89 14 c5 a0 4d 11 	mov    %dx,-0x7feeb260(,%eax,8)
80105463:	80 
80105464:	c1 ea 10             	shr    $0x10,%edx
80105467:	66 89 14 c5 a6 4d 11 	mov    %dx,-0x7feeb25a(,%eax,8)
8010546e:	80 
  for(i = 0; i < 256; i++)
8010546f:	83 c0 01             	add    $0x1,%eax
80105472:	3d 00 01 00 00       	cmp    $0x100,%eax
80105477:	75 bf                	jne    80105438 <tvinit+0x8>
{
80105479:	55                   	push   %ebp
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);
8010547a:	ba 08 00 00 00       	mov    $0x8,%edx
{
8010547f:	89 e5                	mov    %esp,%ebp
80105481:	83 ec 18             	sub    $0x18,%esp
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);
80105484:	a1 08 a1 10 80       	mov    0x8010a108,%eax

  initlock(&tickslock, "time");
80105489:	c7 44 24 04 d9 73 10 	movl   $0x801073d9,0x4(%esp)
80105490:	80 
80105491:	c7 04 24 60 4d 11 80 	movl   $0x80114d60,(%esp)
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);
80105498:	66 89 15 a2 4f 11 80 	mov    %dx,0x80114fa2
8010549f:	66 a3 a0 4f 11 80    	mov    %ax,0x80114fa0
801054a5:	c1 e8 10             	shr    $0x10,%eax
801054a8:	c6 05 a4 4f 11 80 00 	movb   $0x0,0x80114fa4
801054af:	c6 05 a5 4f 11 80 ef 	movb   $0xef,0x80114fa5
801054b6:	66 a3 a6 4f 11 80    	mov    %ax,0x80114fa6
  initlock(&tickslock, "time");
801054bc:	e8 af eb ff ff       	call   80104070 <initlock>
}
801054c1:	c9                   	leave  
801054c2:	c3                   	ret    
801054c3:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
801054c9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801054d0 <idtinit>:

void
idtinit(void)
{
801054d0:	55                   	push   %ebp
  pd[0] = size-1;
801054d1:	b8 ff 07 00 00       	mov    $0x7ff,%eax
801054d6:	89 e5                	mov    %esp,%ebp
801054d8:	83 ec 10             	sub    $0x10,%esp
801054db:	66 89 45 fa          	mov    %ax,-0x6(%ebp)
  pd[1] = (uint)p;
801054df:	b8 a0 4d 11 80       	mov    $0x80114da0,%eax
801054e4:	66 89 45 fc          	mov    %ax,-0x4(%ebp)
  pd[2] = (uint)p >> 16;
801054e8:	c1 e8 10             	shr    $0x10,%eax
801054eb:	66 89 45 fe          	mov    %ax,-0x2(%ebp)
  asm volatile("lidt (%0)" : : "r" (pd));
801054ef:	8d 45 fa             	lea    -0x6(%ebp),%eax
801054f2:	0f 01 18             	lidtl  (%eax)
  lidt(idt, sizeof(idt));
}
801054f5:	c9                   	leave  
801054f6:	c3                   	ret    
801054f7:	89 f6                	mov    %esi,%esi
801054f9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

80105500 <trap>:

//PAGEBREAK: 41
void
trap(struct trapframe *tf)
{
80105500:	55                   	push   %ebp
80105501:	89 e5                	mov    %esp,%ebp
80105503:	57                   	push   %edi
80105504:	56                   	push   %esi
80105505:	53                   	push   %ebx
80105506:	83 ec 3c             	sub    $0x3c,%esp
80105509:	8b 5d 08             	mov    0x8(%ebp),%ebx
  if(tf->trapno == T_SYSCALL){
8010550c:	8b 43 30             	mov    0x30(%ebx),%eax
8010550f:	83 f8 40             	cmp    $0x40,%eax
80105512:	0f 84 a0 01 00 00    	je     801056b8 <trap+0x1b8>
    if(myproc()->killed)
      exit(0);
    return;
  }

  switch(tf->trapno){
80105518:	83 e8 20             	sub    $0x20,%eax
8010551b:	83 f8 1f             	cmp    $0x1f,%eax
8010551e:	77 08                	ja     80105528 <trap+0x28>
80105520:	ff 24 85 80 74 10 80 	jmp    *-0x7fef8b80(,%eax,4)
80105527:	90                   	nop
    lapiceoi();
    break;

  //PAGEBREAK: 13
  default:
    if(myproc() == 0 || (tf->cs&3) == 0){
80105528:	e8 73 e1 ff ff       	call   801036a0 <myproc>
8010552d:	85 c0                	test   %eax,%eax
8010552f:	90                   	nop
80105530:	0f 84 0a 02 00 00    	je     80105740 <trap+0x240>
80105536:	f6 43 3c 03          	testb  $0x3,0x3c(%ebx)
8010553a:	0f 84 00 02 00 00    	je     80105740 <trap+0x240>

static inline uint
rcr2(void)
{
  uint val;
  asm volatile("movl %%cr2,%0" : "=r" (val));
80105540:	0f 20 d1             	mov    %cr2,%ecx
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, cpuid(), tf->eip, rcr2());
      panic("trap");
    }
    // In user space, assume process misbehaved.
    cprintf("pid %d %s: trap %d err %d on cpu %d "
80105543:	8b 53 38             	mov    0x38(%ebx),%edx
80105546:	89 4d d8             	mov    %ecx,-0x28(%ebp)
80105549:	89 55 dc             	mov    %edx,-0x24(%ebp)
8010554c:	e8 2f e1 ff ff       	call   80103680 <cpuid>
80105551:	8b 73 30             	mov    0x30(%ebx),%esi
80105554:	89 c7                	mov    %eax,%edi
80105556:	8b 43 34             	mov    0x34(%ebx),%eax
80105559:	89 45 e4             	mov    %eax,-0x1c(%ebp)
            "eip 0x%x addr 0x%x--kill proc\n",
            myproc()->pid, myproc()->name, tf->trapno,
8010555c:	e8 3f e1 ff ff       	call   801036a0 <myproc>
80105561:	89 45 e0             	mov    %eax,-0x20(%ebp)
80105564:	e8 37 e1 ff ff       	call   801036a0 <myproc>
    cprintf("pid %d %s: trap %d err %d on cpu %d "
80105569:	8b 55 dc             	mov    -0x24(%ebp),%edx
8010556c:	89 74 24 0c          	mov    %esi,0xc(%esp)
            myproc()->pid, myproc()->name, tf->trapno,
80105570:	8b 75 e0             	mov    -0x20(%ebp),%esi
    cprintf("pid %d %s: trap %d err %d on cpu %d "
80105573:	8b 4d d8             	mov    -0x28(%ebp),%ecx
80105576:	89 7c 24 14          	mov    %edi,0x14(%esp)
8010557a:	89 54 24 18          	mov    %edx,0x18(%esp)
8010557e:	8b 55 e4             	mov    -0x1c(%ebp),%edx
            myproc()->pid, myproc()->name, tf->trapno,
80105581:	83 c6 6c             	add    $0x6c,%esi
    cprintf("pid %d %s: trap %d err %d on cpu %d "
80105584:	89 4c 24 1c          	mov    %ecx,0x1c(%esp)
            myproc()->pid, myproc()->name, tf->trapno,
80105588:	89 74 24 08          	mov    %esi,0x8(%esp)
    cprintf("pid %d %s: trap %d err %d on cpu %d "
8010558c:	89 54 24 10          	mov    %edx,0x10(%esp)
80105590:	8b 40 10             	mov    0x10(%eax),%eax
80105593:	c7 04 24 3c 74 10 80 	movl   $0x8010743c,(%esp)
8010559a:	89 44 24 04          	mov    %eax,0x4(%esp)
8010559e:	e8 ad b0 ff ff       	call   80100650 <cprintf>
            tf->err, cpuid(), tf->eip, rcr2());
    myproc()->killed = 1;
801055a3:	e8 f8 e0 ff ff       	call   801036a0 <myproc>
801055a8:	c7 40 24 01 00 00 00 	movl   $0x1,0x24(%eax)
801055af:	90                   	nop
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running
  // until it gets to the regular system call return.)
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
801055b0:	e8 eb e0 ff ff       	call   801036a0 <myproc>
801055b5:	85 c0                	test   %eax,%eax
801055b7:	74 0c                	je     801055c5 <trap+0xc5>
801055b9:	e8 e2 e0 ff ff       	call   801036a0 <myproc>
801055be:	8b 50 24             	mov    0x24(%eax),%edx
801055c1:	85 d2                	test   %edx,%edx
801055c3:	75 4b                	jne    80105610 <trap+0x110>
    exit(0);

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(myproc() && myproc()->state == RUNNING &&
801055c5:	e8 d6 e0 ff ff       	call   801036a0 <myproc>
801055ca:	85 c0                	test   %eax,%eax
801055cc:	74 0d                	je     801055db <trap+0xdb>
801055ce:	66 90                	xchg   %ax,%ax
801055d0:	e8 cb e0 ff ff       	call   801036a0 <myproc>
801055d5:	83 78 0c 04          	cmpl   $0x4,0xc(%eax)
801055d9:	74 55                	je     80105630 <trap+0x130>
     tf->trapno == T_IRQ0+IRQ_TIMER)
    yield();

  // Check if the process has been killed since we yielded
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
801055db:	e8 c0 e0 ff ff       	call   801036a0 <myproc>
801055e0:	85 c0                	test   %eax,%eax
801055e2:	74 1d                	je     80105601 <trap+0x101>
801055e4:	e8 b7 e0 ff ff       	call   801036a0 <myproc>
801055e9:	8b 40 24             	mov    0x24(%eax),%eax
801055ec:	85 c0                	test   %eax,%eax
801055ee:	74 11                	je     80105601 <trap+0x101>
801055f0:	0f b7 43 3c          	movzwl 0x3c(%ebx),%eax
801055f4:	83 e0 03             	and    $0x3,%eax
801055f7:	66 83 f8 03          	cmp    $0x3,%ax
801055fb:	0f 84 e8 00 00 00    	je     801056e9 <trap+0x1e9>
    exit(0);
}
80105601:	83 c4 3c             	add    $0x3c,%esp
80105604:	5b                   	pop    %ebx
80105605:	5e                   	pop    %esi
80105606:	5f                   	pop    %edi
80105607:	5d                   	pop    %ebp
80105608:	c3                   	ret    
80105609:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
80105610:	0f b7 43 3c          	movzwl 0x3c(%ebx),%eax
80105614:	83 e0 03             	and    $0x3,%eax
80105617:	66 83 f8 03          	cmp    $0x3,%ax
8010561b:	75 a8                	jne    801055c5 <trap+0xc5>
    exit(0);
8010561d:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
80105624:	e8 77 e4 ff ff       	call   80103aa0 <exit>
80105629:	eb 9a                	jmp    801055c5 <trap+0xc5>
8010562b:	90                   	nop
8010562c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  if(myproc() && myproc()->state == RUNNING &&
80105630:	83 7b 30 20          	cmpl   $0x20,0x30(%ebx)
80105634:	75 a5                	jne    801055db <trap+0xdb>
    yield();
80105636:	e8 85 e5 ff ff       	call   80103bc0 <yield>
8010563b:	eb 9e                	jmp    801055db <trap+0xdb>
8010563d:	8d 76 00             	lea    0x0(%esi),%esi
    if(cpuid() == 0){
80105640:	e8 3b e0 ff ff       	call   80103680 <cpuid>
80105645:	85 c0                	test   %eax,%eax
80105647:	0f 84 c3 00 00 00    	je     80105710 <trap+0x210>
8010564d:	8d 76 00             	lea    0x0(%esi),%esi
    lapiceoi();
80105650:	e8 2b d1 ff ff       	call   80102780 <lapiceoi>
    break;
80105655:	e9 56 ff ff ff       	jmp    801055b0 <trap+0xb0>
8010565a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    kbdintr();
80105660:	e8 6b cf ff ff       	call   801025d0 <kbdintr>
    lapiceoi();
80105665:	e8 16 d1 ff ff       	call   80102780 <lapiceoi>
    break;
8010566a:	e9 41 ff ff ff       	jmp    801055b0 <trap+0xb0>
8010566f:	90                   	nop
    uartintr();
80105670:	e8 2b 02 00 00       	call   801058a0 <uartintr>
    lapiceoi();
80105675:	e8 06 d1 ff ff       	call   80102780 <lapiceoi>
    break;
8010567a:	e9 31 ff ff ff       	jmp    801055b0 <trap+0xb0>
8010567f:	90                   	nop
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
80105680:	8b 7b 38             	mov    0x38(%ebx),%edi
80105683:	0f b7 73 3c          	movzwl 0x3c(%ebx),%esi
80105687:	e8 f4 df ff ff       	call   80103680 <cpuid>
8010568c:	c7 04 24 e4 73 10 80 	movl   $0x801073e4,(%esp)
80105693:	89 7c 24 0c          	mov    %edi,0xc(%esp)
80105697:	89 74 24 08          	mov    %esi,0x8(%esp)
8010569b:	89 44 24 04          	mov    %eax,0x4(%esp)
8010569f:	e8 ac af ff ff       	call   80100650 <cprintf>
    lapiceoi();
801056a4:	e8 d7 d0 ff ff       	call   80102780 <lapiceoi>
    break;
801056a9:	e9 02 ff ff ff       	jmp    801055b0 <trap+0xb0>
801056ae:	66 90                	xchg   %ax,%ax
    ideintr();
801056b0:	e8 cb c9 ff ff       	call   80102080 <ideintr>
801056b5:	eb 96                	jmp    8010564d <trap+0x14d>
801056b7:	90                   	nop
801056b8:	90                   	nop
801056b9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
    if(myproc()->killed)
801056c0:	e8 db df ff ff       	call   801036a0 <myproc>
801056c5:	8b 70 24             	mov    0x24(%eax),%esi
801056c8:	85 f6                	test   %esi,%esi
801056ca:	75 34                	jne    80105700 <trap+0x200>
    myproc()->tf = tf;
801056cc:	e8 cf df ff ff       	call   801036a0 <myproc>
801056d1:	89 58 18             	mov    %ebx,0x18(%eax)
    syscall();
801056d4:	e8 87 ef ff ff       	call   80104660 <syscall>
    if(myproc()->killed)
801056d9:	e8 c2 df ff ff       	call   801036a0 <myproc>
801056de:	8b 48 24             	mov    0x24(%eax),%ecx
801056e1:	85 c9                	test   %ecx,%ecx
801056e3:	0f 84 18 ff ff ff    	je     80105601 <trap+0x101>
      exit(0);
801056e9:	c7 45 08 00 00 00 00 	movl   $0x0,0x8(%ebp)
}
801056f0:	83 c4 3c             	add    $0x3c,%esp
801056f3:	5b                   	pop    %ebx
801056f4:	5e                   	pop    %esi
801056f5:	5f                   	pop    %edi
801056f6:	5d                   	pop    %ebp
      exit(0);
801056f7:	e9 a4 e3 ff ff       	jmp    80103aa0 <exit>
801056fc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      exit(0);
80105700:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
80105707:	e8 94 e3 ff ff       	call   80103aa0 <exit>
8010570c:	eb be                	jmp    801056cc <trap+0x1cc>
8010570e:	66 90                	xchg   %ax,%ax
      acquire(&tickslock);
80105710:	c7 04 24 60 4d 11 80 	movl   $0x80114d60,(%esp)
80105717:	e8 c4 ea ff ff       	call   801041e0 <acquire>
      wakeup(&ticks);
8010571c:	c7 04 24 a0 55 11 80 	movl   $0x801155a0,(%esp)
      ticks++;
80105723:	83 05 a0 55 11 80 01 	addl   $0x1,0x801155a0
      wakeup(&ticks);
8010572a:	e8 61 e6 ff ff       	call   80103d90 <wakeup>
      release(&tickslock);
8010572f:	c7 04 24 60 4d 11 80 	movl   $0x80114d60,(%esp)
80105736:	e8 15 eb ff ff       	call   80104250 <release>
8010573b:	e9 0d ff ff ff       	jmp    8010564d <trap+0x14d>
80105740:	0f 20 d7             	mov    %cr2,%edi
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
80105743:	8b 73 38             	mov    0x38(%ebx),%esi
80105746:	e8 35 df ff ff       	call   80103680 <cpuid>
8010574b:	89 7c 24 10          	mov    %edi,0x10(%esp)
8010574f:	89 74 24 0c          	mov    %esi,0xc(%esp)
80105753:	89 44 24 08          	mov    %eax,0x8(%esp)
80105757:	8b 43 30             	mov    0x30(%ebx),%eax
8010575a:	c7 04 24 08 74 10 80 	movl   $0x80107408,(%esp)
80105761:	89 44 24 04          	mov    %eax,0x4(%esp)
80105765:	e8 e6 ae ff ff       	call   80100650 <cprintf>
      panic("trap");
8010576a:	c7 04 24 de 73 10 80 	movl   $0x801073de,(%esp)
80105771:	e8 ea ab ff ff       	call   80100360 <panic>
80105776:	66 90                	xchg   %ax,%ax
80105778:	66 90                	xchg   %ax,%ax
8010577a:	66 90                	xchg   %ax,%ax
8010577c:	66 90                	xchg   %ax,%ax
8010577e:	66 90                	xchg   %ax,%ax

80105780 <uartgetc>:
}

static int
uartgetc(void)
{
  if (!uart)
80105780:	a1 bc a5 10 80       	mov    0x8010a5bc,%eax
{
80105785:	55                   	push   %ebp
80105786:	89 e5                	mov    %esp,%ebp
  if (!uart)
80105788:	85 c0                	test   %eax,%eax
8010578a:	74 14                	je     801057a0 <uartgetc+0x20>
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
8010578c:	ba fd 03 00 00       	mov    $0x3fd,%edx
80105791:	ec                   	in     (%dx),%al
    return -1;
  if (!(inb(COM1 + 5) & 0x01))
80105792:	a8 01                	test   $0x1,%al
80105794:	74 0a                	je     801057a0 <uartgetc+0x20>
80105796:	b2 f8                	mov    $0xf8,%dl
80105798:	ec                   	in     (%dx),%al
    return -1;
  return inb(COM1 + 0);
80105799:	0f b6 c0             	movzbl %al,%eax
}
8010579c:	5d                   	pop    %ebp
8010579d:	c3                   	ret    
8010579e:	66 90                	xchg   %ax,%ax
    return -1;
801057a0:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
801057a5:	5d                   	pop    %ebp
801057a6:	c3                   	ret    
801057a7:	89 f6                	mov    %esi,%esi
801057a9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801057b0 <uartputc>:
  if (!uart)
801057b0:	a1 bc a5 10 80       	mov    0x8010a5bc,%eax
801057b5:	85 c0                	test   %eax,%eax
801057b7:	74 3f                	je     801057f8 <uartputc+0x48>
{
801057b9:	55                   	push   %ebp
801057ba:	89 e5                	mov    %esp,%ebp
801057bc:	56                   	push   %esi
801057bd:	be fd 03 00 00       	mov    $0x3fd,%esi
801057c2:	53                   	push   %ebx
  if (!uart)
801057c3:	bb 80 00 00 00       	mov    $0x80,%ebx
{
801057c8:	83 ec 10             	sub    $0x10,%esp
801057cb:	eb 14                	jmp    801057e1 <uartputc+0x31>
801057cd:	8d 76 00             	lea    0x0(%esi),%esi
    microdelay(10);
801057d0:	c7 04 24 0a 00 00 00 	movl   $0xa,(%esp)
801057d7:	e8 c4 cf ff ff       	call   801027a0 <microdelay>
  for (i = 0; i < 128 && !(inb(COM1 + 5) & 0x20); i++)
801057dc:	83 eb 01             	sub    $0x1,%ebx
801057df:	74 07                	je     801057e8 <uartputc+0x38>
801057e1:	89 f2                	mov    %esi,%edx
801057e3:	ec                   	in     (%dx),%al
801057e4:	a8 20                	test   $0x20,%al
801057e6:	74 e8                	je     801057d0 <uartputc+0x20>
  outb(COM1 + 0, c);
801057e8:	0f b6 45 08          	movzbl 0x8(%ebp),%eax
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
801057ec:	ba f8 03 00 00       	mov    $0x3f8,%edx
801057f1:	ee                   	out    %al,(%dx)
}
801057f2:	83 c4 10             	add    $0x10,%esp
801057f5:	5b                   	pop    %ebx
801057f6:	5e                   	pop    %esi
801057f7:	5d                   	pop    %ebp
801057f8:	f3 c3                	repz ret 
801057fa:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

80105800 <uartinit>:
{
80105800:	55                   	push   %ebp
80105801:	31 c9                	xor    %ecx,%ecx
80105803:	89 e5                	mov    %esp,%ebp
80105805:	89 c8                	mov    %ecx,%eax
80105807:	57                   	push   %edi
80105808:	bf fa 03 00 00       	mov    $0x3fa,%edi
8010580d:	56                   	push   %esi
8010580e:	89 fa                	mov    %edi,%edx
80105810:	53                   	push   %ebx
80105811:	83 ec 1c             	sub    $0x1c,%esp
80105814:	ee                   	out    %al,(%dx)
80105815:	be fb 03 00 00       	mov    $0x3fb,%esi
8010581a:	b8 80 ff ff ff       	mov    $0xffffff80,%eax
8010581f:	89 f2                	mov    %esi,%edx
80105821:	ee                   	out    %al,(%dx)
80105822:	b8 0c 00 00 00       	mov    $0xc,%eax
80105827:	b2 f8                	mov    $0xf8,%dl
80105829:	ee                   	out    %al,(%dx)
8010582a:	bb f9 03 00 00       	mov    $0x3f9,%ebx
8010582f:	89 c8                	mov    %ecx,%eax
80105831:	89 da                	mov    %ebx,%edx
80105833:	ee                   	out    %al,(%dx)
80105834:	b8 03 00 00 00       	mov    $0x3,%eax
80105839:	89 f2                	mov    %esi,%edx
8010583b:	ee                   	out    %al,(%dx)
8010583c:	b2 fc                	mov    $0xfc,%dl
8010583e:	89 c8                	mov    %ecx,%eax
80105840:	ee                   	out    %al,(%dx)
80105841:	b8 01 00 00 00       	mov    $0x1,%eax
80105846:	89 da                	mov    %ebx,%edx
80105848:	ee                   	out    %al,(%dx)
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
80105849:	b2 fd                	mov    $0xfd,%dl
8010584b:	ec                   	in     (%dx),%al
  if (inb(COM1 + 5) == 0xFF)
8010584c:	3c ff                	cmp    $0xff,%al
8010584e:	74 42                	je     80105892 <uartinit+0x92>
  uart = 1;
80105850:	c7 05 bc a5 10 80 01 	movl   $0x1,0x8010a5bc
80105857:	00 00 00 
8010585a:	89 fa                	mov    %edi,%edx
8010585c:	ec                   	in     (%dx),%al
8010585d:	b2 f8                	mov    $0xf8,%dl
8010585f:	ec                   	in     (%dx),%al
  ioapicenable(IRQ_COM1, 0);
80105860:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
80105867:	00 
  for (p = "xv6...\n"; *p; p++)
80105868:	bb 00 75 10 80       	mov    $0x80107500,%ebx
  ioapicenable(IRQ_COM1, 0);
8010586d:	c7 04 24 04 00 00 00 	movl   $0x4,(%esp)
80105874:	e8 37 ca ff ff       	call   801022b0 <ioapicenable>
  for (p = "xv6...\n"; *p; p++)
80105879:	b8 78 00 00 00       	mov    $0x78,%eax
8010587e:	66 90                	xchg   %ax,%ax
    uartputc(*p);
80105880:	89 04 24             	mov    %eax,(%esp)
  for (p = "xv6...\n"; *p; p++)
80105883:	83 c3 01             	add    $0x1,%ebx
    uartputc(*p);
80105886:	e8 25 ff ff ff       	call   801057b0 <uartputc>
  for (p = "xv6...\n"; *p; p++)
8010588b:	0f be 03             	movsbl (%ebx),%eax
8010588e:	84 c0                	test   %al,%al
80105890:	75 ee                	jne    80105880 <uartinit+0x80>
}
80105892:	83 c4 1c             	add    $0x1c,%esp
80105895:	5b                   	pop    %ebx
80105896:	5e                   	pop    %esi
80105897:	5f                   	pop    %edi
80105898:	5d                   	pop    %ebp
80105899:	c3                   	ret    
8010589a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

801058a0 <uartintr>:

void uartintr(void)
{
801058a0:	55                   	push   %ebp
801058a1:	89 e5                	mov    %esp,%ebp
801058a3:	83 ec 18             	sub    $0x18,%esp
  consoleintr(uartgetc);
801058a6:	c7 04 24 80 57 10 80 	movl   $0x80105780,(%esp)
801058ad:	e8 fe ae ff ff       	call   801007b0 <consoleintr>
}
801058b2:	c9                   	leave  
801058b3:	c3                   	ret    

801058b4 <vector0>:
801058b4:	6a 00                	push   $0x0
801058b6:	6a 00                	push   $0x0
801058b8:	e9 50 fb ff ff       	jmp    8010540d <alltraps>

801058bd <vector1>:
801058bd:	6a 00                	push   $0x0
801058bf:	6a 01                	push   $0x1
801058c1:	e9 47 fb ff ff       	jmp    8010540d <alltraps>

801058c6 <vector2>:
801058c6:	6a 00                	push   $0x0
801058c8:	6a 02                	push   $0x2
801058ca:	e9 3e fb ff ff       	jmp    8010540d <alltraps>

801058cf <vector3>:
801058cf:	6a 00                	push   $0x0
801058d1:	6a 03                	push   $0x3
801058d3:	e9 35 fb ff ff       	jmp    8010540d <alltraps>

801058d8 <vector4>:
801058d8:	6a 00                	push   $0x0
801058da:	6a 04                	push   $0x4
801058dc:	e9 2c fb ff ff       	jmp    8010540d <alltraps>

801058e1 <vector5>:
801058e1:	6a 00                	push   $0x0
801058e3:	6a 05                	push   $0x5
801058e5:	e9 23 fb ff ff       	jmp    8010540d <alltraps>

801058ea <vector6>:
801058ea:	6a 00                	push   $0x0
801058ec:	6a 06                	push   $0x6
801058ee:	e9 1a fb ff ff       	jmp    8010540d <alltraps>

801058f3 <vector7>:
801058f3:	6a 00                	push   $0x0
801058f5:	6a 07                	push   $0x7
801058f7:	e9 11 fb ff ff       	jmp    8010540d <alltraps>

801058fc <vector8>:
801058fc:	6a 08                	push   $0x8
801058fe:	e9 0a fb ff ff       	jmp    8010540d <alltraps>

80105903 <vector9>:
80105903:	6a 00                	push   $0x0
80105905:	6a 09                	push   $0x9
80105907:	e9 01 fb ff ff       	jmp    8010540d <alltraps>

8010590c <vector10>:
8010590c:	6a 0a                	push   $0xa
8010590e:	e9 fa fa ff ff       	jmp    8010540d <alltraps>

80105913 <vector11>:
80105913:	6a 0b                	push   $0xb
80105915:	e9 f3 fa ff ff       	jmp    8010540d <alltraps>

8010591a <vector12>:
8010591a:	6a 0c                	push   $0xc
8010591c:	e9 ec fa ff ff       	jmp    8010540d <alltraps>

80105921 <vector13>:
80105921:	6a 0d                	push   $0xd
80105923:	e9 e5 fa ff ff       	jmp    8010540d <alltraps>

80105928 <vector14>:
80105928:	6a 0e                	push   $0xe
8010592a:	e9 de fa ff ff       	jmp    8010540d <alltraps>

8010592f <vector15>:
8010592f:	6a 00                	push   $0x0
80105931:	6a 0f                	push   $0xf
80105933:	e9 d5 fa ff ff       	jmp    8010540d <alltraps>

80105938 <vector16>:
80105938:	6a 00                	push   $0x0
8010593a:	6a 10                	push   $0x10
8010593c:	e9 cc fa ff ff       	jmp    8010540d <alltraps>

80105941 <vector17>:
80105941:	6a 11                	push   $0x11
80105943:	e9 c5 fa ff ff       	jmp    8010540d <alltraps>

80105948 <vector18>:
80105948:	6a 00                	push   $0x0
8010594a:	6a 12                	push   $0x12
8010594c:	e9 bc fa ff ff       	jmp    8010540d <alltraps>

80105951 <vector19>:
80105951:	6a 00                	push   $0x0
80105953:	6a 13                	push   $0x13
80105955:	e9 b3 fa ff ff       	jmp    8010540d <alltraps>

8010595a <vector20>:
8010595a:	6a 00                	push   $0x0
8010595c:	6a 14                	push   $0x14
8010595e:	e9 aa fa ff ff       	jmp    8010540d <alltraps>

80105963 <vector21>:
80105963:	6a 00                	push   $0x0
80105965:	6a 15                	push   $0x15
80105967:	e9 a1 fa ff ff       	jmp    8010540d <alltraps>

8010596c <vector22>:
8010596c:	6a 00                	push   $0x0
8010596e:	6a 16                	push   $0x16
80105970:	e9 98 fa ff ff       	jmp    8010540d <alltraps>

80105975 <vector23>:
80105975:	6a 00                	push   $0x0
80105977:	6a 17                	push   $0x17
80105979:	e9 8f fa ff ff       	jmp    8010540d <alltraps>

8010597e <vector24>:
8010597e:	6a 00                	push   $0x0
80105980:	6a 18                	push   $0x18
80105982:	e9 86 fa ff ff       	jmp    8010540d <alltraps>

80105987 <vector25>:
80105987:	6a 00                	push   $0x0
80105989:	6a 19                	push   $0x19
8010598b:	e9 7d fa ff ff       	jmp    8010540d <alltraps>

80105990 <vector26>:
80105990:	6a 00                	push   $0x0
80105992:	6a 1a                	push   $0x1a
80105994:	e9 74 fa ff ff       	jmp    8010540d <alltraps>

80105999 <vector27>:
80105999:	6a 00                	push   $0x0
8010599b:	6a 1b                	push   $0x1b
8010599d:	e9 6b fa ff ff       	jmp    8010540d <alltraps>

801059a2 <vector28>:
801059a2:	6a 00                	push   $0x0
801059a4:	6a 1c                	push   $0x1c
801059a6:	e9 62 fa ff ff       	jmp    8010540d <alltraps>

801059ab <vector29>:
801059ab:	6a 00                	push   $0x0
801059ad:	6a 1d                	push   $0x1d
801059af:	e9 59 fa ff ff       	jmp    8010540d <alltraps>

801059b4 <vector30>:
801059b4:	6a 00                	push   $0x0
801059b6:	6a 1e                	push   $0x1e
801059b8:	e9 50 fa ff ff       	jmp    8010540d <alltraps>

801059bd <vector31>:
801059bd:	6a 00                	push   $0x0
801059bf:	6a 1f                	push   $0x1f
801059c1:	e9 47 fa ff ff       	jmp    8010540d <alltraps>

801059c6 <vector32>:
801059c6:	6a 00                	push   $0x0
801059c8:	6a 20                	push   $0x20
801059ca:	e9 3e fa ff ff       	jmp    8010540d <alltraps>

801059cf <vector33>:
801059cf:	6a 00                	push   $0x0
801059d1:	6a 21                	push   $0x21
801059d3:	e9 35 fa ff ff       	jmp    8010540d <alltraps>

801059d8 <vector34>:
801059d8:	6a 00                	push   $0x0
801059da:	6a 22                	push   $0x22
801059dc:	e9 2c fa ff ff       	jmp    8010540d <alltraps>

801059e1 <vector35>:
801059e1:	6a 00                	push   $0x0
801059e3:	6a 23                	push   $0x23
801059e5:	e9 23 fa ff ff       	jmp    8010540d <alltraps>

801059ea <vector36>:
801059ea:	6a 00                	push   $0x0
801059ec:	6a 24                	push   $0x24
801059ee:	e9 1a fa ff ff       	jmp    8010540d <alltraps>

801059f3 <vector37>:
801059f3:	6a 00                	push   $0x0
801059f5:	6a 25                	push   $0x25
801059f7:	e9 11 fa ff ff       	jmp    8010540d <alltraps>

801059fc <vector38>:
801059fc:	6a 00                	push   $0x0
801059fe:	6a 26                	push   $0x26
80105a00:	e9 08 fa ff ff       	jmp    8010540d <alltraps>

80105a05 <vector39>:
80105a05:	6a 00                	push   $0x0
80105a07:	6a 27                	push   $0x27
80105a09:	e9 ff f9 ff ff       	jmp    8010540d <alltraps>

80105a0e <vector40>:
80105a0e:	6a 00                	push   $0x0
80105a10:	6a 28                	push   $0x28
80105a12:	e9 f6 f9 ff ff       	jmp    8010540d <alltraps>

80105a17 <vector41>:
80105a17:	6a 00                	push   $0x0
80105a19:	6a 29                	push   $0x29
80105a1b:	e9 ed f9 ff ff       	jmp    8010540d <alltraps>

80105a20 <vector42>:
80105a20:	6a 00                	push   $0x0
80105a22:	6a 2a                	push   $0x2a
80105a24:	e9 e4 f9 ff ff       	jmp    8010540d <alltraps>

80105a29 <vector43>:
80105a29:	6a 00                	push   $0x0
80105a2b:	6a 2b                	push   $0x2b
80105a2d:	e9 db f9 ff ff       	jmp    8010540d <alltraps>

80105a32 <vector44>:
80105a32:	6a 00                	push   $0x0
80105a34:	6a 2c                	push   $0x2c
80105a36:	e9 d2 f9 ff ff       	jmp    8010540d <alltraps>

80105a3b <vector45>:
80105a3b:	6a 00                	push   $0x0
80105a3d:	6a 2d                	push   $0x2d
80105a3f:	e9 c9 f9 ff ff       	jmp    8010540d <alltraps>

80105a44 <vector46>:
80105a44:	6a 00                	push   $0x0
80105a46:	6a 2e                	push   $0x2e
80105a48:	e9 c0 f9 ff ff       	jmp    8010540d <alltraps>

80105a4d <vector47>:
80105a4d:	6a 00                	push   $0x0
80105a4f:	6a 2f                	push   $0x2f
80105a51:	e9 b7 f9 ff ff       	jmp    8010540d <alltraps>

80105a56 <vector48>:
80105a56:	6a 00                	push   $0x0
80105a58:	6a 30                	push   $0x30
80105a5a:	e9 ae f9 ff ff       	jmp    8010540d <alltraps>

80105a5f <vector49>:
80105a5f:	6a 00                	push   $0x0
80105a61:	6a 31                	push   $0x31
80105a63:	e9 a5 f9 ff ff       	jmp    8010540d <alltraps>

80105a68 <vector50>:
80105a68:	6a 00                	push   $0x0
80105a6a:	6a 32                	push   $0x32
80105a6c:	e9 9c f9 ff ff       	jmp    8010540d <alltraps>

80105a71 <vector51>:
80105a71:	6a 00                	push   $0x0
80105a73:	6a 33                	push   $0x33
80105a75:	e9 93 f9 ff ff       	jmp    8010540d <alltraps>

80105a7a <vector52>:
80105a7a:	6a 00                	push   $0x0
80105a7c:	6a 34                	push   $0x34
80105a7e:	e9 8a f9 ff ff       	jmp    8010540d <alltraps>

80105a83 <vector53>:
80105a83:	6a 00                	push   $0x0
80105a85:	6a 35                	push   $0x35
80105a87:	e9 81 f9 ff ff       	jmp    8010540d <alltraps>

80105a8c <vector54>:
80105a8c:	6a 00                	push   $0x0
80105a8e:	6a 36                	push   $0x36
80105a90:	e9 78 f9 ff ff       	jmp    8010540d <alltraps>

80105a95 <vector55>:
80105a95:	6a 00                	push   $0x0
80105a97:	6a 37                	push   $0x37
80105a99:	e9 6f f9 ff ff       	jmp    8010540d <alltraps>

80105a9e <vector56>:
80105a9e:	6a 00                	push   $0x0
80105aa0:	6a 38                	push   $0x38
80105aa2:	e9 66 f9 ff ff       	jmp    8010540d <alltraps>

80105aa7 <vector57>:
80105aa7:	6a 00                	push   $0x0
80105aa9:	6a 39                	push   $0x39
80105aab:	e9 5d f9 ff ff       	jmp    8010540d <alltraps>

80105ab0 <vector58>:
80105ab0:	6a 00                	push   $0x0
80105ab2:	6a 3a                	push   $0x3a
80105ab4:	e9 54 f9 ff ff       	jmp    8010540d <alltraps>

80105ab9 <vector59>:
80105ab9:	6a 00                	push   $0x0
80105abb:	6a 3b                	push   $0x3b
80105abd:	e9 4b f9 ff ff       	jmp    8010540d <alltraps>

80105ac2 <vector60>:
80105ac2:	6a 00                	push   $0x0
80105ac4:	6a 3c                	push   $0x3c
80105ac6:	e9 42 f9 ff ff       	jmp    8010540d <alltraps>

80105acb <vector61>:
80105acb:	6a 00                	push   $0x0
80105acd:	6a 3d                	push   $0x3d
80105acf:	e9 39 f9 ff ff       	jmp    8010540d <alltraps>

80105ad4 <vector62>:
80105ad4:	6a 00                	push   $0x0
80105ad6:	6a 3e                	push   $0x3e
80105ad8:	e9 30 f9 ff ff       	jmp    8010540d <alltraps>

80105add <vector63>:
80105add:	6a 00                	push   $0x0
80105adf:	6a 3f                	push   $0x3f
80105ae1:	e9 27 f9 ff ff       	jmp    8010540d <alltraps>

80105ae6 <vector64>:
80105ae6:	6a 00                	push   $0x0
80105ae8:	6a 40                	push   $0x40
80105aea:	e9 1e f9 ff ff       	jmp    8010540d <alltraps>

80105aef <vector65>:
80105aef:	6a 00                	push   $0x0
80105af1:	6a 41                	push   $0x41
80105af3:	e9 15 f9 ff ff       	jmp    8010540d <alltraps>

80105af8 <vector66>:
80105af8:	6a 00                	push   $0x0
80105afa:	6a 42                	push   $0x42
80105afc:	e9 0c f9 ff ff       	jmp    8010540d <alltraps>

80105b01 <vector67>:
80105b01:	6a 00                	push   $0x0
80105b03:	6a 43                	push   $0x43
80105b05:	e9 03 f9 ff ff       	jmp    8010540d <alltraps>

80105b0a <vector68>:
80105b0a:	6a 00                	push   $0x0
80105b0c:	6a 44                	push   $0x44
80105b0e:	e9 fa f8 ff ff       	jmp    8010540d <alltraps>

80105b13 <vector69>:
80105b13:	6a 00                	push   $0x0
80105b15:	6a 45                	push   $0x45
80105b17:	e9 f1 f8 ff ff       	jmp    8010540d <alltraps>

80105b1c <vector70>:
80105b1c:	6a 00                	push   $0x0
80105b1e:	6a 46                	push   $0x46
80105b20:	e9 e8 f8 ff ff       	jmp    8010540d <alltraps>

80105b25 <vector71>:
80105b25:	6a 00                	push   $0x0
80105b27:	6a 47                	push   $0x47
80105b29:	e9 df f8 ff ff       	jmp    8010540d <alltraps>

80105b2e <vector72>:
80105b2e:	6a 00                	push   $0x0
80105b30:	6a 48                	push   $0x48
80105b32:	e9 d6 f8 ff ff       	jmp    8010540d <alltraps>

80105b37 <vector73>:
80105b37:	6a 00                	push   $0x0
80105b39:	6a 49                	push   $0x49
80105b3b:	e9 cd f8 ff ff       	jmp    8010540d <alltraps>

80105b40 <vector74>:
80105b40:	6a 00                	push   $0x0
80105b42:	6a 4a                	push   $0x4a
80105b44:	e9 c4 f8 ff ff       	jmp    8010540d <alltraps>

80105b49 <vector75>:
80105b49:	6a 00                	push   $0x0
80105b4b:	6a 4b                	push   $0x4b
80105b4d:	e9 bb f8 ff ff       	jmp    8010540d <alltraps>

80105b52 <vector76>:
80105b52:	6a 00                	push   $0x0
80105b54:	6a 4c                	push   $0x4c
80105b56:	e9 b2 f8 ff ff       	jmp    8010540d <alltraps>

80105b5b <vector77>:
80105b5b:	6a 00                	push   $0x0
80105b5d:	6a 4d                	push   $0x4d
80105b5f:	e9 a9 f8 ff ff       	jmp    8010540d <alltraps>

80105b64 <vector78>:
80105b64:	6a 00                	push   $0x0
80105b66:	6a 4e                	push   $0x4e
80105b68:	e9 a0 f8 ff ff       	jmp    8010540d <alltraps>

80105b6d <vector79>:
80105b6d:	6a 00                	push   $0x0
80105b6f:	6a 4f                	push   $0x4f
80105b71:	e9 97 f8 ff ff       	jmp    8010540d <alltraps>

80105b76 <vector80>:
80105b76:	6a 00                	push   $0x0
80105b78:	6a 50                	push   $0x50
80105b7a:	e9 8e f8 ff ff       	jmp    8010540d <alltraps>

80105b7f <vector81>:
80105b7f:	6a 00                	push   $0x0
80105b81:	6a 51                	push   $0x51
80105b83:	e9 85 f8 ff ff       	jmp    8010540d <alltraps>

80105b88 <vector82>:
80105b88:	6a 00                	push   $0x0
80105b8a:	6a 52                	push   $0x52
80105b8c:	e9 7c f8 ff ff       	jmp    8010540d <alltraps>

80105b91 <vector83>:
80105b91:	6a 00                	push   $0x0
80105b93:	6a 53                	push   $0x53
80105b95:	e9 73 f8 ff ff       	jmp    8010540d <alltraps>

80105b9a <vector84>:
80105b9a:	6a 00                	push   $0x0
80105b9c:	6a 54                	push   $0x54
80105b9e:	e9 6a f8 ff ff       	jmp    8010540d <alltraps>

80105ba3 <vector85>:
80105ba3:	6a 00                	push   $0x0
80105ba5:	6a 55                	push   $0x55
80105ba7:	e9 61 f8 ff ff       	jmp    8010540d <alltraps>

80105bac <vector86>:
80105bac:	6a 00                	push   $0x0
80105bae:	6a 56                	push   $0x56
80105bb0:	e9 58 f8 ff ff       	jmp    8010540d <alltraps>

80105bb5 <vector87>:
80105bb5:	6a 00                	push   $0x0
80105bb7:	6a 57                	push   $0x57
80105bb9:	e9 4f f8 ff ff       	jmp    8010540d <alltraps>

80105bbe <vector88>:
80105bbe:	6a 00                	push   $0x0
80105bc0:	6a 58                	push   $0x58
80105bc2:	e9 46 f8 ff ff       	jmp    8010540d <alltraps>

80105bc7 <vector89>:
80105bc7:	6a 00                	push   $0x0
80105bc9:	6a 59                	push   $0x59
80105bcb:	e9 3d f8 ff ff       	jmp    8010540d <alltraps>

80105bd0 <vector90>:
80105bd0:	6a 00                	push   $0x0
80105bd2:	6a 5a                	push   $0x5a
80105bd4:	e9 34 f8 ff ff       	jmp    8010540d <alltraps>

80105bd9 <vector91>:
80105bd9:	6a 00                	push   $0x0
80105bdb:	6a 5b                	push   $0x5b
80105bdd:	e9 2b f8 ff ff       	jmp    8010540d <alltraps>

80105be2 <vector92>:
80105be2:	6a 00                	push   $0x0
80105be4:	6a 5c                	push   $0x5c
80105be6:	e9 22 f8 ff ff       	jmp    8010540d <alltraps>

80105beb <vector93>:
80105beb:	6a 00                	push   $0x0
80105bed:	6a 5d                	push   $0x5d
80105bef:	e9 19 f8 ff ff       	jmp    8010540d <alltraps>

80105bf4 <vector94>:
80105bf4:	6a 00                	push   $0x0
80105bf6:	6a 5e                	push   $0x5e
80105bf8:	e9 10 f8 ff ff       	jmp    8010540d <alltraps>

80105bfd <vector95>:
80105bfd:	6a 00                	push   $0x0
80105bff:	6a 5f                	push   $0x5f
80105c01:	e9 07 f8 ff ff       	jmp    8010540d <alltraps>

80105c06 <vector96>:
80105c06:	6a 00                	push   $0x0
80105c08:	6a 60                	push   $0x60
80105c0a:	e9 fe f7 ff ff       	jmp    8010540d <alltraps>

80105c0f <vector97>:
80105c0f:	6a 00                	push   $0x0
80105c11:	6a 61                	push   $0x61
80105c13:	e9 f5 f7 ff ff       	jmp    8010540d <alltraps>

80105c18 <vector98>:
80105c18:	6a 00                	push   $0x0
80105c1a:	6a 62                	push   $0x62
80105c1c:	e9 ec f7 ff ff       	jmp    8010540d <alltraps>

80105c21 <vector99>:
80105c21:	6a 00                	push   $0x0
80105c23:	6a 63                	push   $0x63
80105c25:	e9 e3 f7 ff ff       	jmp    8010540d <alltraps>

80105c2a <vector100>:
80105c2a:	6a 00                	push   $0x0
80105c2c:	6a 64                	push   $0x64
80105c2e:	e9 da f7 ff ff       	jmp    8010540d <alltraps>

80105c33 <vector101>:
80105c33:	6a 00                	push   $0x0
80105c35:	6a 65                	push   $0x65
80105c37:	e9 d1 f7 ff ff       	jmp    8010540d <alltraps>

80105c3c <vector102>:
80105c3c:	6a 00                	push   $0x0
80105c3e:	6a 66                	push   $0x66
80105c40:	e9 c8 f7 ff ff       	jmp    8010540d <alltraps>

80105c45 <vector103>:
80105c45:	6a 00                	push   $0x0
80105c47:	6a 67                	push   $0x67
80105c49:	e9 bf f7 ff ff       	jmp    8010540d <alltraps>

80105c4e <vector104>:
80105c4e:	6a 00                	push   $0x0
80105c50:	6a 68                	push   $0x68
80105c52:	e9 b6 f7 ff ff       	jmp    8010540d <alltraps>

80105c57 <vector105>:
80105c57:	6a 00                	push   $0x0
80105c59:	6a 69                	push   $0x69
80105c5b:	e9 ad f7 ff ff       	jmp    8010540d <alltraps>

80105c60 <vector106>:
80105c60:	6a 00                	push   $0x0
80105c62:	6a 6a                	push   $0x6a
80105c64:	e9 a4 f7 ff ff       	jmp    8010540d <alltraps>

80105c69 <vector107>:
80105c69:	6a 00                	push   $0x0
80105c6b:	6a 6b                	push   $0x6b
80105c6d:	e9 9b f7 ff ff       	jmp    8010540d <alltraps>

80105c72 <vector108>:
80105c72:	6a 00                	push   $0x0
80105c74:	6a 6c                	push   $0x6c
80105c76:	e9 92 f7 ff ff       	jmp    8010540d <alltraps>

80105c7b <vector109>:
80105c7b:	6a 00                	push   $0x0
80105c7d:	6a 6d                	push   $0x6d
80105c7f:	e9 89 f7 ff ff       	jmp    8010540d <alltraps>

80105c84 <vector110>:
80105c84:	6a 00                	push   $0x0
80105c86:	6a 6e                	push   $0x6e
80105c88:	e9 80 f7 ff ff       	jmp    8010540d <alltraps>

80105c8d <vector111>:
80105c8d:	6a 00                	push   $0x0
80105c8f:	6a 6f                	push   $0x6f
80105c91:	e9 77 f7 ff ff       	jmp    8010540d <alltraps>

80105c96 <vector112>:
80105c96:	6a 00                	push   $0x0
80105c98:	6a 70                	push   $0x70
80105c9a:	e9 6e f7 ff ff       	jmp    8010540d <alltraps>

80105c9f <vector113>:
80105c9f:	6a 00                	push   $0x0
80105ca1:	6a 71                	push   $0x71
80105ca3:	e9 65 f7 ff ff       	jmp    8010540d <alltraps>

80105ca8 <vector114>:
80105ca8:	6a 00                	push   $0x0
80105caa:	6a 72                	push   $0x72
80105cac:	e9 5c f7 ff ff       	jmp    8010540d <alltraps>

80105cb1 <vector115>:
80105cb1:	6a 00                	push   $0x0
80105cb3:	6a 73                	push   $0x73
80105cb5:	e9 53 f7 ff ff       	jmp    8010540d <alltraps>

80105cba <vector116>:
80105cba:	6a 00                	push   $0x0
80105cbc:	6a 74                	push   $0x74
80105cbe:	e9 4a f7 ff ff       	jmp    8010540d <alltraps>

80105cc3 <vector117>:
80105cc3:	6a 00                	push   $0x0
80105cc5:	6a 75                	push   $0x75
80105cc7:	e9 41 f7 ff ff       	jmp    8010540d <alltraps>

80105ccc <vector118>:
80105ccc:	6a 00                	push   $0x0
80105cce:	6a 76                	push   $0x76
80105cd0:	e9 38 f7 ff ff       	jmp    8010540d <alltraps>

80105cd5 <vector119>:
80105cd5:	6a 00                	push   $0x0
80105cd7:	6a 77                	push   $0x77
80105cd9:	e9 2f f7 ff ff       	jmp    8010540d <alltraps>

80105cde <vector120>:
80105cde:	6a 00                	push   $0x0
80105ce0:	6a 78                	push   $0x78
80105ce2:	e9 26 f7 ff ff       	jmp    8010540d <alltraps>

80105ce7 <vector121>:
80105ce7:	6a 00                	push   $0x0
80105ce9:	6a 79                	push   $0x79
80105ceb:	e9 1d f7 ff ff       	jmp    8010540d <alltraps>

80105cf0 <vector122>:
80105cf0:	6a 00                	push   $0x0
80105cf2:	6a 7a                	push   $0x7a
80105cf4:	e9 14 f7 ff ff       	jmp    8010540d <alltraps>

80105cf9 <vector123>:
80105cf9:	6a 00                	push   $0x0
80105cfb:	6a 7b                	push   $0x7b
80105cfd:	e9 0b f7 ff ff       	jmp    8010540d <alltraps>

80105d02 <vector124>:
80105d02:	6a 00                	push   $0x0
80105d04:	6a 7c                	push   $0x7c
80105d06:	e9 02 f7 ff ff       	jmp    8010540d <alltraps>

80105d0b <vector125>:
80105d0b:	6a 00                	push   $0x0
80105d0d:	6a 7d                	push   $0x7d
80105d0f:	e9 f9 f6 ff ff       	jmp    8010540d <alltraps>

80105d14 <vector126>:
80105d14:	6a 00                	push   $0x0
80105d16:	6a 7e                	push   $0x7e
80105d18:	e9 f0 f6 ff ff       	jmp    8010540d <alltraps>

80105d1d <vector127>:
80105d1d:	6a 00                	push   $0x0
80105d1f:	6a 7f                	push   $0x7f
80105d21:	e9 e7 f6 ff ff       	jmp    8010540d <alltraps>

80105d26 <vector128>:
80105d26:	6a 00                	push   $0x0
80105d28:	68 80 00 00 00       	push   $0x80
80105d2d:	e9 db f6 ff ff       	jmp    8010540d <alltraps>

80105d32 <vector129>:
80105d32:	6a 00                	push   $0x0
80105d34:	68 81 00 00 00       	push   $0x81
80105d39:	e9 cf f6 ff ff       	jmp    8010540d <alltraps>

80105d3e <vector130>:
80105d3e:	6a 00                	push   $0x0
80105d40:	68 82 00 00 00       	push   $0x82
80105d45:	e9 c3 f6 ff ff       	jmp    8010540d <alltraps>

80105d4a <vector131>:
80105d4a:	6a 00                	push   $0x0
80105d4c:	68 83 00 00 00       	push   $0x83
80105d51:	e9 b7 f6 ff ff       	jmp    8010540d <alltraps>

80105d56 <vector132>:
80105d56:	6a 00                	push   $0x0
80105d58:	68 84 00 00 00       	push   $0x84
80105d5d:	e9 ab f6 ff ff       	jmp    8010540d <alltraps>

80105d62 <vector133>:
80105d62:	6a 00                	push   $0x0
80105d64:	68 85 00 00 00       	push   $0x85
80105d69:	e9 9f f6 ff ff       	jmp    8010540d <alltraps>

80105d6e <vector134>:
80105d6e:	6a 00                	push   $0x0
80105d70:	68 86 00 00 00       	push   $0x86
80105d75:	e9 93 f6 ff ff       	jmp    8010540d <alltraps>

80105d7a <vector135>:
80105d7a:	6a 00                	push   $0x0
80105d7c:	68 87 00 00 00       	push   $0x87
80105d81:	e9 87 f6 ff ff       	jmp    8010540d <alltraps>

80105d86 <vector136>:
80105d86:	6a 00                	push   $0x0
80105d88:	68 88 00 00 00       	push   $0x88
80105d8d:	e9 7b f6 ff ff       	jmp    8010540d <alltraps>

80105d92 <vector137>:
80105d92:	6a 00                	push   $0x0
80105d94:	68 89 00 00 00       	push   $0x89
80105d99:	e9 6f f6 ff ff       	jmp    8010540d <alltraps>

80105d9e <vector138>:
80105d9e:	6a 00                	push   $0x0
80105da0:	68 8a 00 00 00       	push   $0x8a
80105da5:	e9 63 f6 ff ff       	jmp    8010540d <alltraps>

80105daa <vector139>:
80105daa:	6a 00                	push   $0x0
80105dac:	68 8b 00 00 00       	push   $0x8b
80105db1:	e9 57 f6 ff ff       	jmp    8010540d <alltraps>

80105db6 <vector140>:
80105db6:	6a 00                	push   $0x0
80105db8:	68 8c 00 00 00       	push   $0x8c
80105dbd:	e9 4b f6 ff ff       	jmp    8010540d <alltraps>

80105dc2 <vector141>:
80105dc2:	6a 00                	push   $0x0
80105dc4:	68 8d 00 00 00       	push   $0x8d
80105dc9:	e9 3f f6 ff ff       	jmp    8010540d <alltraps>

80105dce <vector142>:
80105dce:	6a 00                	push   $0x0
80105dd0:	68 8e 00 00 00       	push   $0x8e
80105dd5:	e9 33 f6 ff ff       	jmp    8010540d <alltraps>

80105dda <vector143>:
80105dda:	6a 00                	push   $0x0
80105ddc:	68 8f 00 00 00       	push   $0x8f
80105de1:	e9 27 f6 ff ff       	jmp    8010540d <alltraps>

80105de6 <vector144>:
80105de6:	6a 00                	push   $0x0
80105de8:	68 90 00 00 00       	push   $0x90
80105ded:	e9 1b f6 ff ff       	jmp    8010540d <alltraps>

80105df2 <vector145>:
80105df2:	6a 00                	push   $0x0
80105df4:	68 91 00 00 00       	push   $0x91
80105df9:	e9 0f f6 ff ff       	jmp    8010540d <alltraps>

80105dfe <vector146>:
80105dfe:	6a 00                	push   $0x0
80105e00:	68 92 00 00 00       	push   $0x92
80105e05:	e9 03 f6 ff ff       	jmp    8010540d <alltraps>

80105e0a <vector147>:
80105e0a:	6a 00                	push   $0x0
80105e0c:	68 93 00 00 00       	push   $0x93
80105e11:	e9 f7 f5 ff ff       	jmp    8010540d <alltraps>

80105e16 <vector148>:
80105e16:	6a 00                	push   $0x0
80105e18:	68 94 00 00 00       	push   $0x94
80105e1d:	e9 eb f5 ff ff       	jmp    8010540d <alltraps>

80105e22 <vector149>:
80105e22:	6a 00                	push   $0x0
80105e24:	68 95 00 00 00       	push   $0x95
80105e29:	e9 df f5 ff ff       	jmp    8010540d <alltraps>

80105e2e <vector150>:
80105e2e:	6a 00                	push   $0x0
80105e30:	68 96 00 00 00       	push   $0x96
80105e35:	e9 d3 f5 ff ff       	jmp    8010540d <alltraps>

80105e3a <vector151>:
80105e3a:	6a 00                	push   $0x0
80105e3c:	68 97 00 00 00       	push   $0x97
80105e41:	e9 c7 f5 ff ff       	jmp    8010540d <alltraps>

80105e46 <vector152>:
80105e46:	6a 00                	push   $0x0
80105e48:	68 98 00 00 00       	push   $0x98
80105e4d:	e9 bb f5 ff ff       	jmp    8010540d <alltraps>

80105e52 <vector153>:
80105e52:	6a 00                	push   $0x0
80105e54:	68 99 00 00 00       	push   $0x99
80105e59:	e9 af f5 ff ff       	jmp    8010540d <alltraps>

80105e5e <vector154>:
80105e5e:	6a 00                	push   $0x0
80105e60:	68 9a 00 00 00       	push   $0x9a
80105e65:	e9 a3 f5 ff ff       	jmp    8010540d <alltraps>

80105e6a <vector155>:
80105e6a:	6a 00                	push   $0x0
80105e6c:	68 9b 00 00 00       	push   $0x9b
80105e71:	e9 97 f5 ff ff       	jmp    8010540d <alltraps>

80105e76 <vector156>:
80105e76:	6a 00                	push   $0x0
80105e78:	68 9c 00 00 00       	push   $0x9c
80105e7d:	e9 8b f5 ff ff       	jmp    8010540d <alltraps>

80105e82 <vector157>:
80105e82:	6a 00                	push   $0x0
80105e84:	68 9d 00 00 00       	push   $0x9d
80105e89:	e9 7f f5 ff ff       	jmp    8010540d <alltraps>

80105e8e <vector158>:
80105e8e:	6a 00                	push   $0x0
80105e90:	68 9e 00 00 00       	push   $0x9e
80105e95:	e9 73 f5 ff ff       	jmp    8010540d <alltraps>

80105e9a <vector159>:
80105e9a:	6a 00                	push   $0x0
80105e9c:	68 9f 00 00 00       	push   $0x9f
80105ea1:	e9 67 f5 ff ff       	jmp    8010540d <alltraps>

80105ea6 <vector160>:
80105ea6:	6a 00                	push   $0x0
80105ea8:	68 a0 00 00 00       	push   $0xa0
80105ead:	e9 5b f5 ff ff       	jmp    8010540d <alltraps>

80105eb2 <vector161>:
80105eb2:	6a 00                	push   $0x0
80105eb4:	68 a1 00 00 00       	push   $0xa1
80105eb9:	e9 4f f5 ff ff       	jmp    8010540d <alltraps>

80105ebe <vector162>:
80105ebe:	6a 00                	push   $0x0
80105ec0:	68 a2 00 00 00       	push   $0xa2
80105ec5:	e9 43 f5 ff ff       	jmp    8010540d <alltraps>

80105eca <vector163>:
80105eca:	6a 00                	push   $0x0
80105ecc:	68 a3 00 00 00       	push   $0xa3
80105ed1:	e9 37 f5 ff ff       	jmp    8010540d <alltraps>

80105ed6 <vector164>:
80105ed6:	6a 00                	push   $0x0
80105ed8:	68 a4 00 00 00       	push   $0xa4
80105edd:	e9 2b f5 ff ff       	jmp    8010540d <alltraps>

80105ee2 <vector165>:
80105ee2:	6a 00                	push   $0x0
80105ee4:	68 a5 00 00 00       	push   $0xa5
80105ee9:	e9 1f f5 ff ff       	jmp    8010540d <alltraps>

80105eee <vector166>:
80105eee:	6a 00                	push   $0x0
80105ef0:	68 a6 00 00 00       	push   $0xa6
80105ef5:	e9 13 f5 ff ff       	jmp    8010540d <alltraps>

80105efa <vector167>:
80105efa:	6a 00                	push   $0x0
80105efc:	68 a7 00 00 00       	push   $0xa7
80105f01:	e9 07 f5 ff ff       	jmp    8010540d <alltraps>

80105f06 <vector168>:
80105f06:	6a 00                	push   $0x0
80105f08:	68 a8 00 00 00       	push   $0xa8
80105f0d:	e9 fb f4 ff ff       	jmp    8010540d <alltraps>

80105f12 <vector169>:
80105f12:	6a 00                	push   $0x0
80105f14:	68 a9 00 00 00       	push   $0xa9
80105f19:	e9 ef f4 ff ff       	jmp    8010540d <alltraps>

80105f1e <vector170>:
80105f1e:	6a 00                	push   $0x0
80105f20:	68 aa 00 00 00       	push   $0xaa
80105f25:	e9 e3 f4 ff ff       	jmp    8010540d <alltraps>

80105f2a <vector171>:
80105f2a:	6a 00                	push   $0x0
80105f2c:	68 ab 00 00 00       	push   $0xab
80105f31:	e9 d7 f4 ff ff       	jmp    8010540d <alltraps>

80105f36 <vector172>:
80105f36:	6a 00                	push   $0x0
80105f38:	68 ac 00 00 00       	push   $0xac
80105f3d:	e9 cb f4 ff ff       	jmp    8010540d <alltraps>

80105f42 <vector173>:
80105f42:	6a 00                	push   $0x0
80105f44:	68 ad 00 00 00       	push   $0xad
80105f49:	e9 bf f4 ff ff       	jmp    8010540d <alltraps>

80105f4e <vector174>:
80105f4e:	6a 00                	push   $0x0
80105f50:	68 ae 00 00 00       	push   $0xae
80105f55:	e9 b3 f4 ff ff       	jmp    8010540d <alltraps>

80105f5a <vector175>:
80105f5a:	6a 00                	push   $0x0
80105f5c:	68 af 00 00 00       	push   $0xaf
80105f61:	e9 a7 f4 ff ff       	jmp    8010540d <alltraps>

80105f66 <vector176>:
80105f66:	6a 00                	push   $0x0
80105f68:	68 b0 00 00 00       	push   $0xb0
80105f6d:	e9 9b f4 ff ff       	jmp    8010540d <alltraps>

80105f72 <vector177>:
80105f72:	6a 00                	push   $0x0
80105f74:	68 b1 00 00 00       	push   $0xb1
80105f79:	e9 8f f4 ff ff       	jmp    8010540d <alltraps>

80105f7e <vector178>:
80105f7e:	6a 00                	push   $0x0
80105f80:	68 b2 00 00 00       	push   $0xb2
80105f85:	e9 83 f4 ff ff       	jmp    8010540d <alltraps>

80105f8a <vector179>:
80105f8a:	6a 00                	push   $0x0
80105f8c:	68 b3 00 00 00       	push   $0xb3
80105f91:	e9 77 f4 ff ff       	jmp    8010540d <alltraps>

80105f96 <vector180>:
80105f96:	6a 00                	push   $0x0
80105f98:	68 b4 00 00 00       	push   $0xb4
80105f9d:	e9 6b f4 ff ff       	jmp    8010540d <alltraps>

80105fa2 <vector181>:
80105fa2:	6a 00                	push   $0x0
80105fa4:	68 b5 00 00 00       	push   $0xb5
80105fa9:	e9 5f f4 ff ff       	jmp    8010540d <alltraps>

80105fae <vector182>:
80105fae:	6a 00                	push   $0x0
80105fb0:	68 b6 00 00 00       	push   $0xb6
80105fb5:	e9 53 f4 ff ff       	jmp    8010540d <alltraps>

80105fba <vector183>:
80105fba:	6a 00                	push   $0x0
80105fbc:	68 b7 00 00 00       	push   $0xb7
80105fc1:	e9 47 f4 ff ff       	jmp    8010540d <alltraps>

80105fc6 <vector184>:
80105fc6:	6a 00                	push   $0x0
80105fc8:	68 b8 00 00 00       	push   $0xb8
80105fcd:	e9 3b f4 ff ff       	jmp    8010540d <alltraps>

80105fd2 <vector185>:
80105fd2:	6a 00                	push   $0x0
80105fd4:	68 b9 00 00 00       	push   $0xb9
80105fd9:	e9 2f f4 ff ff       	jmp    8010540d <alltraps>

80105fde <vector186>:
80105fde:	6a 00                	push   $0x0
80105fe0:	68 ba 00 00 00       	push   $0xba
80105fe5:	e9 23 f4 ff ff       	jmp    8010540d <alltraps>

80105fea <vector187>:
80105fea:	6a 00                	push   $0x0
80105fec:	68 bb 00 00 00       	push   $0xbb
80105ff1:	e9 17 f4 ff ff       	jmp    8010540d <alltraps>

80105ff6 <vector188>:
80105ff6:	6a 00                	push   $0x0
80105ff8:	68 bc 00 00 00       	push   $0xbc
80105ffd:	e9 0b f4 ff ff       	jmp    8010540d <alltraps>

80106002 <vector189>:
80106002:	6a 00                	push   $0x0
80106004:	68 bd 00 00 00       	push   $0xbd
80106009:	e9 ff f3 ff ff       	jmp    8010540d <alltraps>

8010600e <vector190>:
8010600e:	6a 00                	push   $0x0
80106010:	68 be 00 00 00       	push   $0xbe
80106015:	e9 f3 f3 ff ff       	jmp    8010540d <alltraps>

8010601a <vector191>:
8010601a:	6a 00                	push   $0x0
8010601c:	68 bf 00 00 00       	push   $0xbf
80106021:	e9 e7 f3 ff ff       	jmp    8010540d <alltraps>

80106026 <vector192>:
80106026:	6a 00                	push   $0x0
80106028:	68 c0 00 00 00       	push   $0xc0
8010602d:	e9 db f3 ff ff       	jmp    8010540d <alltraps>

80106032 <vector193>:
80106032:	6a 00                	push   $0x0
80106034:	68 c1 00 00 00       	push   $0xc1
80106039:	e9 cf f3 ff ff       	jmp    8010540d <alltraps>

8010603e <vector194>:
8010603e:	6a 00                	push   $0x0
80106040:	68 c2 00 00 00       	push   $0xc2
80106045:	e9 c3 f3 ff ff       	jmp    8010540d <alltraps>

8010604a <vector195>:
8010604a:	6a 00                	push   $0x0
8010604c:	68 c3 00 00 00       	push   $0xc3
80106051:	e9 b7 f3 ff ff       	jmp    8010540d <alltraps>

80106056 <vector196>:
80106056:	6a 00                	push   $0x0
80106058:	68 c4 00 00 00       	push   $0xc4
8010605d:	e9 ab f3 ff ff       	jmp    8010540d <alltraps>

80106062 <vector197>:
80106062:	6a 00                	push   $0x0
80106064:	68 c5 00 00 00       	push   $0xc5
80106069:	e9 9f f3 ff ff       	jmp    8010540d <alltraps>

8010606e <vector198>:
8010606e:	6a 00                	push   $0x0
80106070:	68 c6 00 00 00       	push   $0xc6
80106075:	e9 93 f3 ff ff       	jmp    8010540d <alltraps>

8010607a <vector199>:
8010607a:	6a 00                	push   $0x0
8010607c:	68 c7 00 00 00       	push   $0xc7
80106081:	e9 87 f3 ff ff       	jmp    8010540d <alltraps>

80106086 <vector200>:
80106086:	6a 00                	push   $0x0
80106088:	68 c8 00 00 00       	push   $0xc8
8010608d:	e9 7b f3 ff ff       	jmp    8010540d <alltraps>

80106092 <vector201>:
80106092:	6a 00                	push   $0x0
80106094:	68 c9 00 00 00       	push   $0xc9
80106099:	e9 6f f3 ff ff       	jmp    8010540d <alltraps>

8010609e <vector202>:
8010609e:	6a 00                	push   $0x0
801060a0:	68 ca 00 00 00       	push   $0xca
801060a5:	e9 63 f3 ff ff       	jmp    8010540d <alltraps>

801060aa <vector203>:
801060aa:	6a 00                	push   $0x0
801060ac:	68 cb 00 00 00       	push   $0xcb
801060b1:	e9 57 f3 ff ff       	jmp    8010540d <alltraps>

801060b6 <vector204>:
801060b6:	6a 00                	push   $0x0
801060b8:	68 cc 00 00 00       	push   $0xcc
801060bd:	e9 4b f3 ff ff       	jmp    8010540d <alltraps>

801060c2 <vector205>:
801060c2:	6a 00                	push   $0x0
801060c4:	68 cd 00 00 00       	push   $0xcd
801060c9:	e9 3f f3 ff ff       	jmp    8010540d <alltraps>

801060ce <vector206>:
801060ce:	6a 00                	push   $0x0
801060d0:	68 ce 00 00 00       	push   $0xce
801060d5:	e9 33 f3 ff ff       	jmp    8010540d <alltraps>

801060da <vector207>:
801060da:	6a 00                	push   $0x0
801060dc:	68 cf 00 00 00       	push   $0xcf
801060e1:	e9 27 f3 ff ff       	jmp    8010540d <alltraps>

801060e6 <vector208>:
801060e6:	6a 00                	push   $0x0
801060e8:	68 d0 00 00 00       	push   $0xd0
801060ed:	e9 1b f3 ff ff       	jmp    8010540d <alltraps>

801060f2 <vector209>:
801060f2:	6a 00                	push   $0x0
801060f4:	68 d1 00 00 00       	push   $0xd1
801060f9:	e9 0f f3 ff ff       	jmp    8010540d <alltraps>

801060fe <vector210>:
801060fe:	6a 00                	push   $0x0
80106100:	68 d2 00 00 00       	push   $0xd2
80106105:	e9 03 f3 ff ff       	jmp    8010540d <alltraps>

8010610a <vector211>:
8010610a:	6a 00                	push   $0x0
8010610c:	68 d3 00 00 00       	push   $0xd3
80106111:	e9 f7 f2 ff ff       	jmp    8010540d <alltraps>

80106116 <vector212>:
80106116:	6a 00                	push   $0x0
80106118:	68 d4 00 00 00       	push   $0xd4
8010611d:	e9 eb f2 ff ff       	jmp    8010540d <alltraps>

80106122 <vector213>:
80106122:	6a 00                	push   $0x0
80106124:	68 d5 00 00 00       	push   $0xd5
80106129:	e9 df f2 ff ff       	jmp    8010540d <alltraps>

8010612e <vector214>:
8010612e:	6a 00                	push   $0x0
80106130:	68 d6 00 00 00       	push   $0xd6
80106135:	e9 d3 f2 ff ff       	jmp    8010540d <alltraps>

8010613a <vector215>:
8010613a:	6a 00                	push   $0x0
8010613c:	68 d7 00 00 00       	push   $0xd7
80106141:	e9 c7 f2 ff ff       	jmp    8010540d <alltraps>

80106146 <vector216>:
80106146:	6a 00                	push   $0x0
80106148:	68 d8 00 00 00       	push   $0xd8
8010614d:	e9 bb f2 ff ff       	jmp    8010540d <alltraps>

80106152 <vector217>:
80106152:	6a 00                	push   $0x0
80106154:	68 d9 00 00 00       	push   $0xd9
80106159:	e9 af f2 ff ff       	jmp    8010540d <alltraps>

8010615e <vector218>:
8010615e:	6a 00                	push   $0x0
80106160:	68 da 00 00 00       	push   $0xda
80106165:	e9 a3 f2 ff ff       	jmp    8010540d <alltraps>

8010616a <vector219>:
8010616a:	6a 00                	push   $0x0
8010616c:	68 db 00 00 00       	push   $0xdb
80106171:	e9 97 f2 ff ff       	jmp    8010540d <alltraps>

80106176 <vector220>:
80106176:	6a 00                	push   $0x0
80106178:	68 dc 00 00 00       	push   $0xdc
8010617d:	e9 8b f2 ff ff       	jmp    8010540d <alltraps>

80106182 <vector221>:
80106182:	6a 00                	push   $0x0
80106184:	68 dd 00 00 00       	push   $0xdd
80106189:	e9 7f f2 ff ff       	jmp    8010540d <alltraps>

8010618e <vector222>:
8010618e:	6a 00                	push   $0x0
80106190:	68 de 00 00 00       	push   $0xde
80106195:	e9 73 f2 ff ff       	jmp    8010540d <alltraps>

8010619a <vector223>:
8010619a:	6a 00                	push   $0x0
8010619c:	68 df 00 00 00       	push   $0xdf
801061a1:	e9 67 f2 ff ff       	jmp    8010540d <alltraps>

801061a6 <vector224>:
801061a6:	6a 00                	push   $0x0
801061a8:	68 e0 00 00 00       	push   $0xe0
801061ad:	e9 5b f2 ff ff       	jmp    8010540d <alltraps>

801061b2 <vector225>:
801061b2:	6a 00                	push   $0x0
801061b4:	68 e1 00 00 00       	push   $0xe1
801061b9:	e9 4f f2 ff ff       	jmp    8010540d <alltraps>

801061be <vector226>:
801061be:	6a 00                	push   $0x0
801061c0:	68 e2 00 00 00       	push   $0xe2
801061c5:	e9 43 f2 ff ff       	jmp    8010540d <alltraps>

801061ca <vector227>:
801061ca:	6a 00                	push   $0x0
801061cc:	68 e3 00 00 00       	push   $0xe3
801061d1:	e9 37 f2 ff ff       	jmp    8010540d <alltraps>

801061d6 <vector228>:
801061d6:	6a 00                	push   $0x0
801061d8:	68 e4 00 00 00       	push   $0xe4
801061dd:	e9 2b f2 ff ff       	jmp    8010540d <alltraps>

801061e2 <vector229>:
801061e2:	6a 00                	push   $0x0
801061e4:	68 e5 00 00 00       	push   $0xe5
801061e9:	e9 1f f2 ff ff       	jmp    8010540d <alltraps>

801061ee <vector230>:
801061ee:	6a 00                	push   $0x0
801061f0:	68 e6 00 00 00       	push   $0xe6
801061f5:	e9 13 f2 ff ff       	jmp    8010540d <alltraps>

801061fa <vector231>:
801061fa:	6a 00                	push   $0x0
801061fc:	68 e7 00 00 00       	push   $0xe7
80106201:	e9 07 f2 ff ff       	jmp    8010540d <alltraps>

80106206 <vector232>:
80106206:	6a 00                	push   $0x0
80106208:	68 e8 00 00 00       	push   $0xe8
8010620d:	e9 fb f1 ff ff       	jmp    8010540d <alltraps>

80106212 <vector233>:
80106212:	6a 00                	push   $0x0
80106214:	68 e9 00 00 00       	push   $0xe9
80106219:	e9 ef f1 ff ff       	jmp    8010540d <alltraps>

8010621e <vector234>:
8010621e:	6a 00                	push   $0x0
80106220:	68 ea 00 00 00       	push   $0xea
80106225:	e9 e3 f1 ff ff       	jmp    8010540d <alltraps>

8010622a <vector235>:
8010622a:	6a 00                	push   $0x0
8010622c:	68 eb 00 00 00       	push   $0xeb
80106231:	e9 d7 f1 ff ff       	jmp    8010540d <alltraps>

80106236 <vector236>:
80106236:	6a 00                	push   $0x0
80106238:	68 ec 00 00 00       	push   $0xec
8010623d:	e9 cb f1 ff ff       	jmp    8010540d <alltraps>

80106242 <vector237>:
80106242:	6a 00                	push   $0x0
80106244:	68 ed 00 00 00       	push   $0xed
80106249:	e9 bf f1 ff ff       	jmp    8010540d <alltraps>

8010624e <vector238>:
8010624e:	6a 00                	push   $0x0
80106250:	68 ee 00 00 00       	push   $0xee
80106255:	e9 b3 f1 ff ff       	jmp    8010540d <alltraps>

8010625a <vector239>:
8010625a:	6a 00                	push   $0x0
8010625c:	68 ef 00 00 00       	push   $0xef
80106261:	e9 a7 f1 ff ff       	jmp    8010540d <alltraps>

80106266 <vector240>:
80106266:	6a 00                	push   $0x0
80106268:	68 f0 00 00 00       	push   $0xf0
8010626d:	e9 9b f1 ff ff       	jmp    8010540d <alltraps>

80106272 <vector241>:
80106272:	6a 00                	push   $0x0
80106274:	68 f1 00 00 00       	push   $0xf1
80106279:	e9 8f f1 ff ff       	jmp    8010540d <alltraps>

8010627e <vector242>:
8010627e:	6a 00                	push   $0x0
80106280:	68 f2 00 00 00       	push   $0xf2
80106285:	e9 83 f1 ff ff       	jmp    8010540d <alltraps>

8010628a <vector243>:
8010628a:	6a 00                	push   $0x0
8010628c:	68 f3 00 00 00       	push   $0xf3
80106291:	e9 77 f1 ff ff       	jmp    8010540d <alltraps>

80106296 <vector244>:
80106296:	6a 00                	push   $0x0
80106298:	68 f4 00 00 00       	push   $0xf4
8010629d:	e9 6b f1 ff ff       	jmp    8010540d <alltraps>

801062a2 <vector245>:
801062a2:	6a 00                	push   $0x0
801062a4:	68 f5 00 00 00       	push   $0xf5
801062a9:	e9 5f f1 ff ff       	jmp    8010540d <alltraps>

801062ae <vector246>:
801062ae:	6a 00                	push   $0x0
801062b0:	68 f6 00 00 00       	push   $0xf6
801062b5:	e9 53 f1 ff ff       	jmp    8010540d <alltraps>

801062ba <vector247>:
801062ba:	6a 00                	push   $0x0
801062bc:	68 f7 00 00 00       	push   $0xf7
801062c1:	e9 47 f1 ff ff       	jmp    8010540d <alltraps>

801062c6 <vector248>:
801062c6:	6a 00                	push   $0x0
801062c8:	68 f8 00 00 00       	push   $0xf8
801062cd:	e9 3b f1 ff ff       	jmp    8010540d <alltraps>

801062d2 <vector249>:
801062d2:	6a 00                	push   $0x0
801062d4:	68 f9 00 00 00       	push   $0xf9
801062d9:	e9 2f f1 ff ff       	jmp    8010540d <alltraps>

801062de <vector250>:
801062de:	6a 00                	push   $0x0
801062e0:	68 fa 00 00 00       	push   $0xfa
801062e5:	e9 23 f1 ff ff       	jmp    8010540d <alltraps>

801062ea <vector251>:
801062ea:	6a 00                	push   $0x0
801062ec:	68 fb 00 00 00       	push   $0xfb
801062f1:	e9 17 f1 ff ff       	jmp    8010540d <alltraps>

801062f6 <vector252>:
801062f6:	6a 00                	push   $0x0
801062f8:	68 fc 00 00 00       	push   $0xfc
801062fd:	e9 0b f1 ff ff       	jmp    8010540d <alltraps>

80106302 <vector253>:
80106302:	6a 00                	push   $0x0
80106304:	68 fd 00 00 00       	push   $0xfd
80106309:	e9 ff f0 ff ff       	jmp    8010540d <alltraps>

8010630e <vector254>:
8010630e:	6a 00                	push   $0x0
80106310:	68 fe 00 00 00       	push   $0xfe
80106315:	e9 f3 f0 ff ff       	jmp    8010540d <alltraps>

8010631a <vector255>:
8010631a:	6a 00                	push   $0x0
8010631c:	68 ff 00 00 00       	push   $0xff
80106321:	e9 e7 f0 ff ff       	jmp    8010540d <alltraps>
80106326:	66 90                	xchg   %ax,%ax
80106328:	66 90                	xchg   %ax,%ax
8010632a:	66 90                	xchg   %ax,%ax
8010632c:	66 90                	xchg   %ax,%ax
8010632e:	66 90                	xchg   %ax,%ax

80106330 <walkpgdir>:
// Return the address of the PTE in page table pgdir
// that corresponds to virtual address va.  If alloc!=0,
// create any required page table pages.
static pte_t *
walkpgdir(pde_t *pgdir, const void *va, int alloc)
{
80106330:	55                   	push   %ebp
80106331:	89 e5                	mov    %esp,%ebp
80106333:	57                   	push   %edi
80106334:	56                   	push   %esi
80106335:	89 d6                	mov    %edx,%esi
  pde_t *pde;
  pte_t *pgtab;

  pde = &pgdir[PDX(va)];
80106337:	c1 ea 16             	shr    $0x16,%edx
{
8010633a:	53                   	push   %ebx
  pde = &pgdir[PDX(va)];
8010633b:	8d 3c 90             	lea    (%eax,%edx,4),%edi
{
8010633e:	83 ec 1c             	sub    $0x1c,%esp
  if(*pde & PTE_P){
80106341:	8b 1f                	mov    (%edi),%ebx
80106343:	f6 c3 01             	test   $0x1,%bl
80106346:	74 28                	je     80106370 <walkpgdir+0x40>
    pgtab = (pte_t*)P2V(PTE_ADDR(*pde));
80106348:	81 e3 00 f0 ff ff    	and    $0xfffff000,%ebx
8010634e:	81 c3 00 00 00 80    	add    $0x80000000,%ebx
    // The permissions here are overly generous, but they can
    // be further restricted by the permissions in the page table
    // entries, if necessary.
    *pde = V2P(pgtab) | PTE_P | PTE_W | PTE_U;
  }
  return &pgtab[PTX(va)];
80106354:	c1 ee 0a             	shr    $0xa,%esi
}
80106357:	83 c4 1c             	add    $0x1c,%esp
  return &pgtab[PTX(va)];
8010635a:	89 f2                	mov    %esi,%edx
8010635c:	81 e2 fc 0f 00 00    	and    $0xffc,%edx
80106362:	8d 04 13             	lea    (%ebx,%edx,1),%eax
}
80106365:	5b                   	pop    %ebx
80106366:	5e                   	pop    %esi
80106367:	5f                   	pop    %edi
80106368:	5d                   	pop    %ebp
80106369:	c3                   	ret    
8010636a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    if(!alloc || (pgtab = (pte_t*)kalloc()) == 0)
80106370:	85 c9                	test   %ecx,%ecx
80106372:	74 34                	je     801063a8 <walkpgdir+0x78>
80106374:	e8 27 c1 ff ff       	call   801024a0 <kalloc>
80106379:	85 c0                	test   %eax,%eax
8010637b:	89 c3                	mov    %eax,%ebx
8010637d:	74 29                	je     801063a8 <walkpgdir+0x78>
    memset(pgtab, 0, PGSIZE);
8010637f:	c7 44 24 08 00 10 00 	movl   $0x1000,0x8(%esp)
80106386:	00 
80106387:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
8010638e:	00 
8010638f:	89 04 24             	mov    %eax,(%esp)
80106392:	e8 09 df ff ff       	call   801042a0 <memset>
    *pde = V2P(pgtab) | PTE_P | PTE_W | PTE_U;
80106397:	8d 83 00 00 00 80    	lea    -0x80000000(%ebx),%eax
8010639d:	83 c8 07             	or     $0x7,%eax
801063a0:	89 07                	mov    %eax,(%edi)
801063a2:	eb b0                	jmp    80106354 <walkpgdir+0x24>
801063a4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
}
801063a8:	83 c4 1c             	add    $0x1c,%esp
      return 0;
801063ab:	31 c0                	xor    %eax,%eax
}
801063ad:	5b                   	pop    %ebx
801063ae:	5e                   	pop    %esi
801063af:	5f                   	pop    %edi
801063b0:	5d                   	pop    %ebp
801063b1:	c3                   	ret    
801063b2:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801063b9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801063c0 <mappages>:
// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa. va and size might not
// be page-aligned.
static int
mappages(pde_t *pgdir, void *va, uint size, uint pa, int perm)
{
801063c0:	55                   	push   %ebp
801063c1:	89 e5                	mov    %esp,%ebp
801063c3:	57                   	push   %edi
801063c4:	56                   	push   %esi
801063c5:	53                   	push   %ebx
  char *a, *last;
  pte_t *pte;

  a = (char*)PGROUNDDOWN((uint)va);
801063c6:	89 d3                	mov    %edx,%ebx
{
801063c8:	83 ec 1c             	sub    $0x1c,%esp
801063cb:	8b 7d 08             	mov    0x8(%ebp),%edi
  a = (char*)PGROUNDDOWN((uint)va);
801063ce:	81 e3 00 f0 ff ff    	and    $0xfffff000,%ebx
{
801063d4:	89 45 e0             	mov    %eax,-0x20(%ebp)
  last = (char*)PGROUNDDOWN(((uint)va) + size - 1);
801063d7:	8d 44 0a ff          	lea    -0x1(%edx,%ecx,1),%eax
801063db:	89 45 e4             	mov    %eax,-0x1c(%ebp)
  for(;;){
    if((pte = walkpgdir(pgdir, a, 1)) == 0)
      return -1;
    if(*pte & PTE_P)
      panic("remap");
    *pte = pa | perm | PTE_P;
801063de:	83 4d 0c 01          	orl    $0x1,0xc(%ebp)
  last = (char*)PGROUNDDOWN(((uint)va) + size - 1);
801063e2:	81 65 e4 00 f0 ff ff 	andl   $0xfffff000,-0x1c(%ebp)
801063e9:	29 df                	sub    %ebx,%edi
801063eb:	eb 18                	jmp    80106405 <mappages+0x45>
801063ed:	8d 76 00             	lea    0x0(%esi),%esi
    if(*pte & PTE_P)
801063f0:	f6 00 01             	testb  $0x1,(%eax)
801063f3:	75 3d                	jne    80106432 <mappages+0x72>
    *pte = pa | perm | PTE_P;
801063f5:	0b 75 0c             	or     0xc(%ebp),%esi
    if(a == last)
801063f8:	3b 5d e4             	cmp    -0x1c(%ebp),%ebx
    *pte = pa | perm | PTE_P;
801063fb:	89 30                	mov    %esi,(%eax)
    if(a == last)
801063fd:	74 29                	je     80106428 <mappages+0x68>
      break;
    a += PGSIZE;
801063ff:	81 c3 00 10 00 00    	add    $0x1000,%ebx
    if((pte = walkpgdir(pgdir, a, 1)) == 0)
80106405:	8b 45 e0             	mov    -0x20(%ebp),%eax
80106408:	b9 01 00 00 00       	mov    $0x1,%ecx
8010640d:	89 da                	mov    %ebx,%edx
8010640f:	8d 34 3b             	lea    (%ebx,%edi,1),%esi
80106412:	e8 19 ff ff ff       	call   80106330 <walkpgdir>
80106417:	85 c0                	test   %eax,%eax
80106419:	75 d5                	jne    801063f0 <mappages+0x30>
    pa += PGSIZE;
  }
  return 0;
}
8010641b:	83 c4 1c             	add    $0x1c,%esp
      return -1;
8010641e:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80106423:	5b                   	pop    %ebx
80106424:	5e                   	pop    %esi
80106425:	5f                   	pop    %edi
80106426:	5d                   	pop    %ebp
80106427:	c3                   	ret    
80106428:	83 c4 1c             	add    $0x1c,%esp
  return 0;
8010642b:	31 c0                	xor    %eax,%eax
}
8010642d:	5b                   	pop    %ebx
8010642e:	5e                   	pop    %esi
8010642f:	5f                   	pop    %edi
80106430:	5d                   	pop    %ebp
80106431:	c3                   	ret    
      panic("remap");
80106432:	c7 04 24 08 75 10 80 	movl   $0x80107508,(%esp)
80106439:	e8 22 9f ff ff       	call   80100360 <panic>
8010643e:	66 90                	xchg   %ax,%ax

80106440 <deallocuvm.part.0>:
// Deallocate user pages to bring the process size from oldsz to
// newsz.  oldsz and newsz need not be page-aligned, nor does newsz
// need to be less than oldsz.  oldsz can be larger than the actual
// process size.  Returns the new process size.
int
deallocuvm(pde_t *pgdir, uint oldsz, uint newsz)
80106440:	55                   	push   %ebp
80106441:	89 e5                	mov    %esp,%ebp
80106443:	57                   	push   %edi
80106444:	89 c7                	mov    %eax,%edi
80106446:	56                   	push   %esi
80106447:	89 d6                	mov    %edx,%esi
80106449:	53                   	push   %ebx
  uint a, pa;

  if(newsz >= oldsz)
    return oldsz;

  a = PGROUNDUP(newsz);
8010644a:	8d 99 ff 0f 00 00    	lea    0xfff(%ecx),%ebx
deallocuvm(pde_t *pgdir, uint oldsz, uint newsz)
80106450:	83 ec 1c             	sub    $0x1c,%esp
  a = PGROUNDUP(newsz);
80106453:	81 e3 00 f0 ff ff    	and    $0xfffff000,%ebx
  for(; a  < oldsz; a += PGSIZE){
80106459:	39 d3                	cmp    %edx,%ebx
deallocuvm(pde_t *pgdir, uint oldsz, uint newsz)
8010645b:	89 4d e0             	mov    %ecx,-0x20(%ebp)
  for(; a  < oldsz; a += PGSIZE){
8010645e:	72 3b                	jb     8010649b <deallocuvm.part.0+0x5b>
80106460:	eb 5e                	jmp    801064c0 <deallocuvm.part.0+0x80>
80106462:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
    pte = walkpgdir(pgdir, (char*)a, 0);
    if(!pte)
      a = PGADDR(PDX(a) + 1, 0, 0) - PGSIZE;
    else if((*pte & PTE_P) != 0){
80106468:	8b 10                	mov    (%eax),%edx
8010646a:	f6 c2 01             	test   $0x1,%dl
8010646d:	74 22                	je     80106491 <deallocuvm.part.0+0x51>
      pa = PTE_ADDR(*pte);
      if(pa == 0)
8010646f:	81 e2 00 f0 ff ff    	and    $0xfffff000,%edx
80106475:	74 54                	je     801064cb <deallocuvm.part.0+0x8b>
        panic("kfree");
      char *v = P2V(pa);
80106477:	81 c2 00 00 00 80    	add    $0x80000000,%edx
      kfree(v);
8010647d:	89 14 24             	mov    %edx,(%esp)
80106480:	89 45 e4             	mov    %eax,-0x1c(%ebp)
80106483:	e8 68 be ff ff       	call   801022f0 <kfree>
      *pte = 0;
80106488:	8b 45 e4             	mov    -0x1c(%ebp),%eax
8010648b:	c7 00 00 00 00 00    	movl   $0x0,(%eax)
  for(; a  < oldsz; a += PGSIZE){
80106491:	81 c3 00 10 00 00    	add    $0x1000,%ebx
80106497:	39 f3                	cmp    %esi,%ebx
80106499:	73 25                	jae    801064c0 <deallocuvm.part.0+0x80>
    pte = walkpgdir(pgdir, (char*)a, 0);
8010649b:	31 c9                	xor    %ecx,%ecx
8010649d:	89 da                	mov    %ebx,%edx
8010649f:	89 f8                	mov    %edi,%eax
801064a1:	e8 8a fe ff ff       	call   80106330 <walkpgdir>
    if(!pte)
801064a6:	85 c0                	test   %eax,%eax
801064a8:	75 be                	jne    80106468 <deallocuvm.part.0+0x28>
      a = PGADDR(PDX(a) + 1, 0, 0) - PGSIZE;
801064aa:	81 e3 00 00 c0 ff    	and    $0xffc00000,%ebx
801064b0:	81 c3 00 f0 3f 00    	add    $0x3ff000,%ebx
  for(; a  < oldsz; a += PGSIZE){
801064b6:	81 c3 00 10 00 00    	add    $0x1000,%ebx
801064bc:	39 f3                	cmp    %esi,%ebx
801064be:	72 db                	jb     8010649b <deallocuvm.part.0+0x5b>
    }
  }
  return newsz;
}
801064c0:	8b 45 e0             	mov    -0x20(%ebp),%eax
801064c3:	83 c4 1c             	add    $0x1c,%esp
801064c6:	5b                   	pop    %ebx
801064c7:	5e                   	pop    %esi
801064c8:	5f                   	pop    %edi
801064c9:	5d                   	pop    %ebp
801064ca:	c3                   	ret    
        panic("kfree");
801064cb:	c7 04 24 a6 6e 10 80 	movl   $0x80106ea6,(%esp)
801064d2:	e8 89 9e ff ff       	call   80100360 <panic>
801064d7:	89 f6                	mov    %esi,%esi
801064d9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801064e0 <seginit>:
{
801064e0:	55                   	push   %ebp
801064e1:	89 e5                	mov    %esp,%ebp
801064e3:	83 ec 18             	sub    $0x18,%esp
  c = &cpus[cpuid()];
801064e6:	e8 95 d1 ff ff       	call   80103680 <cpuid>
  c->gdt[SEG_KCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, 0);
801064eb:	31 c9                	xor    %ecx,%ecx
801064ed:	ba ff ff ff ff       	mov    $0xffffffff,%edx
  c = &cpus[cpuid()];
801064f2:	69 c0 b0 00 00 00    	imul   $0xb0,%eax,%eax
801064f8:	05 80 27 11 80       	add    $0x80112780,%eax
  c->gdt[SEG_KCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, 0);
801064fd:	66 89 50 78          	mov    %dx,0x78(%eax)
  c->gdt[SEG_KDATA] = SEG(STA_W, 0, 0xffffffff, 0);
80106501:	ba ff ff ff ff       	mov    $0xffffffff,%edx
  lgdt(c->gdt, sizeof(c->gdt));
80106506:	83 c0 70             	add    $0x70,%eax
  c->gdt[SEG_KCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, 0);
80106509:	66 89 48 0a          	mov    %cx,0xa(%eax)
  c->gdt[SEG_KDATA] = SEG(STA_W, 0, 0xffffffff, 0);
8010650d:	31 c9                	xor    %ecx,%ecx
8010650f:	66 89 50 10          	mov    %dx,0x10(%eax)
  c->gdt[SEG_UCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, DPL_USER);
80106513:	ba ff ff ff ff       	mov    $0xffffffff,%edx
  c->gdt[SEG_KDATA] = SEG(STA_W, 0, 0xffffffff, 0);
80106518:	66 89 48 12          	mov    %cx,0x12(%eax)
  c->gdt[SEG_UCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, DPL_USER);
8010651c:	31 c9                	xor    %ecx,%ecx
8010651e:	66 89 50 18          	mov    %dx,0x18(%eax)
  c->gdt[SEG_UDATA] = SEG(STA_W, 0, 0xffffffff, DPL_USER);
80106522:	ba ff ff ff ff       	mov    $0xffffffff,%edx
  c->gdt[SEG_UCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, DPL_USER);
80106527:	66 89 48 1a          	mov    %cx,0x1a(%eax)
  c->gdt[SEG_UDATA] = SEG(STA_W, 0, 0xffffffff, DPL_USER);
8010652b:	31 c9                	xor    %ecx,%ecx
  c->gdt[SEG_KCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, 0);
8010652d:	c6 40 0d 9a          	movb   $0x9a,0xd(%eax)
80106531:	c6 40 0e cf          	movb   $0xcf,0xe(%eax)
  c->gdt[SEG_KDATA] = SEG(STA_W, 0, 0xffffffff, 0);
80106535:	c6 40 15 92          	movb   $0x92,0x15(%eax)
80106539:	c6 40 16 cf          	movb   $0xcf,0x16(%eax)
  c->gdt[SEG_UCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, DPL_USER);
8010653d:	c6 40 1d fa          	movb   $0xfa,0x1d(%eax)
80106541:	c6 40 1e cf          	movb   $0xcf,0x1e(%eax)
  c->gdt[SEG_UDATA] = SEG(STA_W, 0, 0xffffffff, DPL_USER);
80106545:	c6 40 25 f2          	movb   $0xf2,0x25(%eax)
80106549:	c6 40 26 cf          	movb   $0xcf,0x26(%eax)
8010654d:	66 89 50 20          	mov    %dx,0x20(%eax)
  pd[0] = size-1;
80106551:	ba 2f 00 00 00       	mov    $0x2f,%edx
  c->gdt[SEG_KCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, 0);
80106556:	c6 40 0c 00          	movb   $0x0,0xc(%eax)
8010655a:	c6 40 0f 00          	movb   $0x0,0xf(%eax)
  c->gdt[SEG_KDATA] = SEG(STA_W, 0, 0xffffffff, 0);
8010655e:	c6 40 14 00          	movb   $0x0,0x14(%eax)
80106562:	c6 40 17 00          	movb   $0x0,0x17(%eax)
  c->gdt[SEG_UCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, DPL_USER);
80106566:	c6 40 1c 00          	movb   $0x0,0x1c(%eax)
8010656a:	c6 40 1f 00          	movb   $0x0,0x1f(%eax)
  c->gdt[SEG_UDATA] = SEG(STA_W, 0, 0xffffffff, DPL_USER);
8010656e:	66 89 48 22          	mov    %cx,0x22(%eax)
80106572:	c6 40 24 00          	movb   $0x0,0x24(%eax)
80106576:	c6 40 27 00          	movb   $0x0,0x27(%eax)
8010657a:	66 89 55 f2          	mov    %dx,-0xe(%ebp)
  pd[1] = (uint)p;
8010657e:	66 89 45 f4          	mov    %ax,-0xc(%ebp)
  pd[2] = (uint)p >> 16;
80106582:	c1 e8 10             	shr    $0x10,%eax
80106585:	66 89 45 f6          	mov    %ax,-0xa(%ebp)
  asm volatile("lgdt (%0)" : : "r" (pd));
80106589:	8d 45 f2             	lea    -0xe(%ebp),%eax
8010658c:	0f 01 10             	lgdtl  (%eax)
}
8010658f:	c9                   	leave  
80106590:	c3                   	ret    
80106591:	eb 0d                	jmp    801065a0 <switchkvm>
80106593:	90                   	nop
80106594:	90                   	nop
80106595:	90                   	nop
80106596:	90                   	nop
80106597:	90                   	nop
80106598:	90                   	nop
80106599:	90                   	nop
8010659a:	90                   	nop
8010659b:	90                   	nop
8010659c:	90                   	nop
8010659d:	90                   	nop
8010659e:	90                   	nop
8010659f:	90                   	nop

801065a0 <switchkvm>:
  lcr3(V2P(kpgdir));   // switch to the kernel page table
801065a0:	a1 a4 55 11 80       	mov    0x801155a4,%eax
{
801065a5:	55                   	push   %ebp
801065a6:	89 e5                	mov    %esp,%ebp
  lcr3(V2P(kpgdir));   // switch to the kernel page table
801065a8:	05 00 00 00 80       	add    $0x80000000,%eax
}

static inline void
lcr3(uint val)
{
  asm volatile("movl %0,%%cr3" : : "r" (val));
801065ad:	0f 22 d8             	mov    %eax,%cr3
}
801065b0:	5d                   	pop    %ebp
801065b1:	c3                   	ret    
801065b2:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
801065b9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801065c0 <switchuvm>:
{
801065c0:	55                   	push   %ebp
801065c1:	89 e5                	mov    %esp,%ebp
801065c3:	57                   	push   %edi
801065c4:	56                   	push   %esi
801065c5:	53                   	push   %ebx
801065c6:	83 ec 1c             	sub    $0x1c,%esp
801065c9:	8b 75 08             	mov    0x8(%ebp),%esi
  if(p == 0)
801065cc:	85 f6                	test   %esi,%esi
801065ce:	0f 84 cd 00 00 00    	je     801066a1 <switchuvm+0xe1>
  if(p->kstack == 0)
801065d4:	8b 46 08             	mov    0x8(%esi),%eax
801065d7:	85 c0                	test   %eax,%eax
801065d9:	0f 84 da 00 00 00    	je     801066b9 <switchuvm+0xf9>
  if(p->pgdir == 0)
801065df:	8b 7e 04             	mov    0x4(%esi),%edi
801065e2:	85 ff                	test   %edi,%edi
801065e4:	0f 84 c3 00 00 00    	je     801066ad <switchuvm+0xed>
  pushcli();
801065ea:	e8 01 db ff ff       	call   801040f0 <pushcli>
  mycpu()->gdt[SEG_TSS] = SEG16(STS_T32A, &mycpu()->ts,
801065ef:	e8 0c d0 ff ff       	call   80103600 <mycpu>
801065f4:	89 c3                	mov    %eax,%ebx
801065f6:	e8 05 d0 ff ff       	call   80103600 <mycpu>
801065fb:	89 c7                	mov    %eax,%edi
801065fd:	e8 fe cf ff ff       	call   80103600 <mycpu>
80106602:	83 c7 08             	add    $0x8,%edi
80106605:	89 45 e4             	mov    %eax,-0x1c(%ebp)
80106608:	e8 f3 cf ff ff       	call   80103600 <mycpu>
8010660d:	8b 4d e4             	mov    -0x1c(%ebp),%ecx
80106610:	ba 67 00 00 00       	mov    $0x67,%edx
80106615:	66 89 93 98 00 00 00 	mov    %dx,0x98(%ebx)
8010661c:	66 89 bb 9a 00 00 00 	mov    %di,0x9a(%ebx)
80106623:	c6 83 9d 00 00 00 99 	movb   $0x99,0x9d(%ebx)
8010662a:	83 c1 08             	add    $0x8,%ecx
8010662d:	c1 e9 10             	shr    $0x10,%ecx
80106630:	83 c0 08             	add    $0x8,%eax
80106633:	c1 e8 18             	shr    $0x18,%eax
80106636:	88 8b 9c 00 00 00    	mov    %cl,0x9c(%ebx)
8010663c:	c6 83 9e 00 00 00 40 	movb   $0x40,0x9e(%ebx)
80106643:	88 83 9f 00 00 00    	mov    %al,0x9f(%ebx)
  mycpu()->ts.iomb = (ushort) 0xFFFF;
80106649:	bb ff ff ff ff       	mov    $0xffffffff,%ebx
  mycpu()->gdt[SEG_TSS].s = 0;
8010664e:	e8 ad cf ff ff       	call   80103600 <mycpu>
80106653:	80 a0 9d 00 00 00 ef 	andb   $0xef,0x9d(%eax)
  mycpu()->ts.ss0 = SEG_KDATA << 3;
8010665a:	e8 a1 cf ff ff       	call   80103600 <mycpu>
8010665f:	b9 10 00 00 00       	mov    $0x10,%ecx
80106664:	66 89 48 10          	mov    %cx,0x10(%eax)
  mycpu()->ts.esp0 = (uint)p->kstack + KSTACKSIZE;
80106668:	e8 93 cf ff ff       	call   80103600 <mycpu>
8010666d:	8b 56 08             	mov    0x8(%esi),%edx
80106670:	8d 8a 00 10 00 00    	lea    0x1000(%edx),%ecx
80106676:	89 48 0c             	mov    %ecx,0xc(%eax)
  mycpu()->ts.iomb = (ushort) 0xFFFF;
80106679:	e8 82 cf ff ff       	call   80103600 <mycpu>
8010667e:	66 89 58 6e          	mov    %bx,0x6e(%eax)
  asm volatile("ltr %0" : : "r" (sel));
80106682:	b8 28 00 00 00       	mov    $0x28,%eax
80106687:	0f 00 d8             	ltr    %ax
  lcr3(V2P(p->pgdir));  // switch to process's address space
8010668a:	8b 46 04             	mov    0x4(%esi),%eax
8010668d:	05 00 00 00 80       	add    $0x80000000,%eax
  asm volatile("movl %0,%%cr3" : : "r" (val));
80106692:	0f 22 d8             	mov    %eax,%cr3
}
80106695:	83 c4 1c             	add    $0x1c,%esp
80106698:	5b                   	pop    %ebx
80106699:	5e                   	pop    %esi
8010669a:	5f                   	pop    %edi
8010669b:	5d                   	pop    %ebp
  popcli();
8010669c:	e9 8f da ff ff       	jmp    80104130 <popcli>
    panic("switchuvm: no process");
801066a1:	c7 04 24 0e 75 10 80 	movl   $0x8010750e,(%esp)
801066a8:	e8 b3 9c ff ff       	call   80100360 <panic>
    panic("switchuvm: no pgdir");
801066ad:	c7 04 24 39 75 10 80 	movl   $0x80107539,(%esp)
801066b4:	e8 a7 9c ff ff       	call   80100360 <panic>
    panic("switchuvm: no kstack");
801066b9:	c7 04 24 24 75 10 80 	movl   $0x80107524,(%esp)
801066c0:	e8 9b 9c ff ff       	call   80100360 <panic>
801066c5:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
801066c9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

801066d0 <inituvm>:
{
801066d0:	55                   	push   %ebp
801066d1:	89 e5                	mov    %esp,%ebp
801066d3:	57                   	push   %edi
801066d4:	56                   	push   %esi
801066d5:	53                   	push   %ebx
801066d6:	83 ec 1c             	sub    $0x1c,%esp
801066d9:	8b 75 10             	mov    0x10(%ebp),%esi
801066dc:	8b 45 08             	mov    0x8(%ebp),%eax
801066df:	8b 7d 0c             	mov    0xc(%ebp),%edi
  if(sz >= PGSIZE)
801066e2:	81 fe ff 0f 00 00    	cmp    $0xfff,%esi
{
801066e8:	89 45 e4             	mov    %eax,-0x1c(%ebp)
  if(sz >= PGSIZE)
801066eb:	77 54                	ja     80106741 <inituvm+0x71>
  mem = kalloc();
801066ed:	e8 ae bd ff ff       	call   801024a0 <kalloc>
  memset(mem, 0, PGSIZE);
801066f2:	c7 44 24 08 00 10 00 	movl   $0x1000,0x8(%esp)
801066f9:	00 
801066fa:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
80106701:	00 
  mem = kalloc();
80106702:	89 c3                	mov    %eax,%ebx
  memset(mem, 0, PGSIZE);
80106704:	89 04 24             	mov    %eax,(%esp)
80106707:	e8 94 db ff ff       	call   801042a0 <memset>
  mappages(pgdir, 0, PGSIZE, V2P(mem), PTE_W|PTE_U);
8010670c:	8d 83 00 00 00 80    	lea    -0x80000000(%ebx),%eax
80106712:	b9 00 10 00 00       	mov    $0x1000,%ecx
80106717:	89 04 24             	mov    %eax,(%esp)
8010671a:	8b 45 e4             	mov    -0x1c(%ebp),%eax
8010671d:	31 d2                	xor    %edx,%edx
8010671f:	c7 44 24 04 06 00 00 	movl   $0x6,0x4(%esp)
80106726:	00 
80106727:	e8 94 fc ff ff       	call   801063c0 <mappages>
  memmove(mem, init, sz);
8010672c:	89 75 10             	mov    %esi,0x10(%ebp)
8010672f:	89 7d 0c             	mov    %edi,0xc(%ebp)
80106732:	89 5d 08             	mov    %ebx,0x8(%ebp)
}
80106735:	83 c4 1c             	add    $0x1c,%esp
80106738:	5b                   	pop    %ebx
80106739:	5e                   	pop    %esi
8010673a:	5f                   	pop    %edi
8010673b:	5d                   	pop    %ebp
  memmove(mem, init, sz);
8010673c:	e9 ff db ff ff       	jmp    80104340 <memmove>
    panic("inituvm: more than a page");
80106741:	c7 04 24 4d 75 10 80 	movl   $0x8010754d,(%esp)
80106748:	e8 13 9c ff ff       	call   80100360 <panic>
8010674d:	8d 76 00             	lea    0x0(%esi),%esi

80106750 <loaduvm>:
{
80106750:	55                   	push   %ebp
80106751:	89 e5                	mov    %esp,%ebp
80106753:	57                   	push   %edi
80106754:	56                   	push   %esi
80106755:	53                   	push   %ebx
80106756:	83 ec 1c             	sub    $0x1c,%esp
  if((uint) addr % PGSIZE != 0)
80106759:	f7 45 0c ff 0f 00 00 	testl  $0xfff,0xc(%ebp)
80106760:	0f 85 98 00 00 00    	jne    801067fe <loaduvm+0xae>
  for(i = 0; i < sz; i += PGSIZE){
80106766:	8b 75 18             	mov    0x18(%ebp),%esi
80106769:	31 db                	xor    %ebx,%ebx
8010676b:	85 f6                	test   %esi,%esi
8010676d:	75 1a                	jne    80106789 <loaduvm+0x39>
8010676f:	eb 77                	jmp    801067e8 <loaduvm+0x98>
80106771:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80106778:	81 c3 00 10 00 00    	add    $0x1000,%ebx
8010677e:	81 ee 00 10 00 00    	sub    $0x1000,%esi
80106784:	39 5d 18             	cmp    %ebx,0x18(%ebp)
80106787:	76 5f                	jbe    801067e8 <loaduvm+0x98>
80106789:	8b 55 0c             	mov    0xc(%ebp),%edx
    if((pte = walkpgdir(pgdir, addr+i, 0)) == 0)
8010678c:	31 c9                	xor    %ecx,%ecx
8010678e:	8b 45 08             	mov    0x8(%ebp),%eax
80106791:	01 da                	add    %ebx,%edx
80106793:	e8 98 fb ff ff       	call   80106330 <walkpgdir>
80106798:	85 c0                	test   %eax,%eax
8010679a:	74 56                	je     801067f2 <loaduvm+0xa2>
    pa = PTE_ADDR(*pte);
8010679c:	8b 00                	mov    (%eax),%eax
      n = PGSIZE;
8010679e:	bf 00 10 00 00       	mov    $0x1000,%edi
801067a3:	8b 4d 14             	mov    0x14(%ebp),%ecx
    pa = PTE_ADDR(*pte);
801067a6:	25 00 f0 ff ff       	and    $0xfffff000,%eax
      n = PGSIZE;
801067ab:	81 fe 00 10 00 00    	cmp    $0x1000,%esi
801067b1:	0f 42 fe             	cmovb  %esi,%edi
    if(readi(ip, P2V(pa), offset+i, n) != n)
801067b4:	05 00 00 00 80       	add    $0x80000000,%eax
801067b9:	89 44 24 04          	mov    %eax,0x4(%esp)
801067bd:	8b 45 10             	mov    0x10(%ebp),%eax
801067c0:	01 d9                	add    %ebx,%ecx
801067c2:	89 7c 24 0c          	mov    %edi,0xc(%esp)
801067c6:	89 4c 24 08          	mov    %ecx,0x8(%esp)
801067ca:	89 04 24             	mov    %eax,(%esp)
801067cd:	e8 8e b1 ff ff       	call   80101960 <readi>
801067d2:	39 f8                	cmp    %edi,%eax
801067d4:	74 a2                	je     80106778 <loaduvm+0x28>
}
801067d6:	83 c4 1c             	add    $0x1c,%esp
      return -1;
801067d9:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
801067de:	5b                   	pop    %ebx
801067df:	5e                   	pop    %esi
801067e0:	5f                   	pop    %edi
801067e1:	5d                   	pop    %ebp
801067e2:	c3                   	ret    
801067e3:	90                   	nop
801067e4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
801067e8:	83 c4 1c             	add    $0x1c,%esp
  return 0;
801067eb:	31 c0                	xor    %eax,%eax
}
801067ed:	5b                   	pop    %ebx
801067ee:	5e                   	pop    %esi
801067ef:	5f                   	pop    %edi
801067f0:	5d                   	pop    %ebp
801067f1:	c3                   	ret    
      panic("loaduvm: address should exist");
801067f2:	c7 04 24 67 75 10 80 	movl   $0x80107567,(%esp)
801067f9:	e8 62 9b ff ff       	call   80100360 <panic>
    panic("loaduvm: addr must be page aligned");
801067fe:	c7 04 24 08 76 10 80 	movl   $0x80107608,(%esp)
80106805:	e8 56 9b ff ff       	call   80100360 <panic>
8010680a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

80106810 <allocuvm>:
{
80106810:	55                   	push   %ebp
80106811:	89 e5                	mov    %esp,%ebp
80106813:	57                   	push   %edi
80106814:	56                   	push   %esi
80106815:	53                   	push   %ebx
80106816:	83 ec 1c             	sub    $0x1c,%esp
80106819:	8b 7d 10             	mov    0x10(%ebp),%edi
  if(newsz >= KERNBASE)
8010681c:	85 ff                	test   %edi,%edi
8010681e:	0f 88 7e 00 00 00    	js     801068a2 <allocuvm+0x92>
  if(newsz < oldsz)
80106824:	3b 7d 0c             	cmp    0xc(%ebp),%edi
    return oldsz;
80106827:	8b 45 0c             	mov    0xc(%ebp),%eax
  if(newsz < oldsz)
8010682a:	72 78                	jb     801068a4 <allocuvm+0x94>
  a = PGROUNDUP(oldsz);
8010682c:	8d 98 ff 0f 00 00    	lea    0xfff(%eax),%ebx
80106832:	81 e3 00 f0 ff ff    	and    $0xfffff000,%ebx
  for(; a < newsz; a += PGSIZE){
80106838:	39 df                	cmp    %ebx,%edi
8010683a:	77 4a                	ja     80106886 <allocuvm+0x76>
8010683c:	eb 72                	jmp    801068b0 <allocuvm+0xa0>
8010683e:	66 90                	xchg   %ax,%ax
    memset(mem, 0, PGSIZE);
80106840:	c7 44 24 08 00 10 00 	movl   $0x1000,0x8(%esp)
80106847:	00 
80106848:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
8010684f:	00 
80106850:	89 04 24             	mov    %eax,(%esp)
80106853:	e8 48 da ff ff       	call   801042a0 <memset>
    if(mappages(pgdir, (char*)a, PGSIZE, V2P(mem), PTE_W|PTE_U) < 0){
80106858:	8d 86 00 00 00 80    	lea    -0x80000000(%esi),%eax
8010685e:	b9 00 10 00 00       	mov    $0x1000,%ecx
80106863:	89 04 24             	mov    %eax,(%esp)
80106866:	8b 45 08             	mov    0x8(%ebp),%eax
80106869:	89 da                	mov    %ebx,%edx
8010686b:	c7 44 24 04 06 00 00 	movl   $0x6,0x4(%esp)
80106872:	00 
80106873:	e8 48 fb ff ff       	call   801063c0 <mappages>
80106878:	85 c0                	test   %eax,%eax
8010687a:	78 44                	js     801068c0 <allocuvm+0xb0>
  for(; a < newsz; a += PGSIZE){
8010687c:	81 c3 00 10 00 00    	add    $0x1000,%ebx
80106882:	39 df                	cmp    %ebx,%edi
80106884:	76 2a                	jbe    801068b0 <allocuvm+0xa0>
    mem = kalloc();
80106886:	e8 15 bc ff ff       	call   801024a0 <kalloc>
    if(mem == 0){
8010688b:	85 c0                	test   %eax,%eax
    mem = kalloc();
8010688d:	89 c6                	mov    %eax,%esi
    if(mem == 0){
8010688f:	75 af                	jne    80106840 <allocuvm+0x30>
      cprintf("allocuvm out of memory\n");
80106891:	c7 04 24 85 75 10 80 	movl   $0x80107585,(%esp)
80106898:	e8 b3 9d ff ff       	call   80100650 <cprintf>
  if(newsz >= oldsz)
8010689d:	3b 7d 0c             	cmp    0xc(%ebp),%edi
801068a0:	77 48                	ja     801068ea <allocuvm+0xda>
      return 0;
801068a2:	31 c0                	xor    %eax,%eax
}
801068a4:	83 c4 1c             	add    $0x1c,%esp
801068a7:	5b                   	pop    %ebx
801068a8:	5e                   	pop    %esi
801068a9:	5f                   	pop    %edi
801068aa:	5d                   	pop    %ebp
801068ab:	c3                   	ret    
801068ac:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
801068b0:	83 c4 1c             	add    $0x1c,%esp
801068b3:	89 f8                	mov    %edi,%eax
801068b5:	5b                   	pop    %ebx
801068b6:	5e                   	pop    %esi
801068b7:	5f                   	pop    %edi
801068b8:	5d                   	pop    %ebp
801068b9:	c3                   	ret    
801068ba:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
      cprintf("allocuvm out of memory (2)\n");
801068c0:	c7 04 24 9d 75 10 80 	movl   $0x8010759d,(%esp)
801068c7:	e8 84 9d ff ff       	call   80100650 <cprintf>
  if(newsz >= oldsz)
801068cc:	3b 7d 0c             	cmp    0xc(%ebp),%edi
801068cf:	76 0d                	jbe    801068de <allocuvm+0xce>
801068d1:	8b 4d 0c             	mov    0xc(%ebp),%ecx
801068d4:	89 fa                	mov    %edi,%edx
801068d6:	8b 45 08             	mov    0x8(%ebp),%eax
801068d9:	e8 62 fb ff ff       	call   80106440 <deallocuvm.part.0>
      kfree(mem);
801068de:	89 34 24             	mov    %esi,(%esp)
801068e1:	e8 0a ba ff ff       	call   801022f0 <kfree>
      return 0;
801068e6:	31 c0                	xor    %eax,%eax
801068e8:	eb ba                	jmp    801068a4 <allocuvm+0x94>
801068ea:	8b 4d 0c             	mov    0xc(%ebp),%ecx
801068ed:	89 fa                	mov    %edi,%edx
801068ef:	8b 45 08             	mov    0x8(%ebp),%eax
801068f2:	e8 49 fb ff ff       	call   80106440 <deallocuvm.part.0>
      return 0;
801068f7:	31 c0                	xor    %eax,%eax
801068f9:	eb a9                	jmp    801068a4 <allocuvm+0x94>
801068fb:	90                   	nop
801068fc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80106900 <deallocuvm>:
{
80106900:	55                   	push   %ebp
80106901:	89 e5                	mov    %esp,%ebp
80106903:	8b 55 0c             	mov    0xc(%ebp),%edx
80106906:	8b 4d 10             	mov    0x10(%ebp),%ecx
80106909:	8b 45 08             	mov    0x8(%ebp),%eax
  if(newsz >= oldsz)
8010690c:	39 d1                	cmp    %edx,%ecx
8010690e:	73 08                	jae    80106918 <deallocuvm+0x18>
}
80106910:	5d                   	pop    %ebp
80106911:	e9 2a fb ff ff       	jmp    80106440 <deallocuvm.part.0>
80106916:	66 90                	xchg   %ax,%ax
80106918:	89 d0                	mov    %edx,%eax
8010691a:	5d                   	pop    %ebp
8010691b:	c3                   	ret    
8010691c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

80106920 <freevm>:

// Free a page table and all the physical memory pages
// in the user part.
void
freevm(pde_t *pgdir)
{
80106920:	55                   	push   %ebp
80106921:	89 e5                	mov    %esp,%ebp
80106923:	56                   	push   %esi
80106924:	53                   	push   %ebx
80106925:	83 ec 10             	sub    $0x10,%esp
80106928:	8b 75 08             	mov    0x8(%ebp),%esi
  uint i;

  if(pgdir == 0)
8010692b:	85 f6                	test   %esi,%esi
8010692d:	74 59                	je     80106988 <freevm+0x68>
8010692f:	31 c9                	xor    %ecx,%ecx
80106931:	ba 00 00 00 80       	mov    $0x80000000,%edx
80106936:	89 f0                	mov    %esi,%eax
    panic("freevm: no pgdir");
  deallocuvm(pgdir, KERNBASE, 0);
  for(i = 0; i < NPDENTRIES; i++){
80106938:	31 db                	xor    %ebx,%ebx
8010693a:	e8 01 fb ff ff       	call   80106440 <deallocuvm.part.0>
8010693f:	eb 12                	jmp    80106953 <freevm+0x33>
80106941:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
80106948:	83 c3 01             	add    $0x1,%ebx
8010694b:	81 fb 00 04 00 00    	cmp    $0x400,%ebx
80106951:	74 27                	je     8010697a <freevm+0x5a>
    if(pgdir[i] & PTE_P){
80106953:	8b 14 9e             	mov    (%esi,%ebx,4),%edx
80106956:	f6 c2 01             	test   $0x1,%dl
80106959:	74 ed                	je     80106948 <freevm+0x28>
      char * v = P2V(PTE_ADDR(pgdir[i]));
8010695b:	81 e2 00 f0 ff ff    	and    $0xfffff000,%edx
  for(i = 0; i < NPDENTRIES; i++){
80106961:	83 c3 01             	add    $0x1,%ebx
      char * v = P2V(PTE_ADDR(pgdir[i]));
80106964:	81 c2 00 00 00 80    	add    $0x80000000,%edx
      kfree(v);
8010696a:	89 14 24             	mov    %edx,(%esp)
8010696d:	e8 7e b9 ff ff       	call   801022f0 <kfree>
  for(i = 0; i < NPDENTRIES; i++){
80106972:	81 fb 00 04 00 00    	cmp    $0x400,%ebx
80106978:	75 d9                	jne    80106953 <freevm+0x33>
    }
  }
  kfree((char*)pgdir);
8010697a:	89 75 08             	mov    %esi,0x8(%ebp)
}
8010697d:	83 c4 10             	add    $0x10,%esp
80106980:	5b                   	pop    %ebx
80106981:	5e                   	pop    %esi
80106982:	5d                   	pop    %ebp
  kfree((char*)pgdir);
80106983:	e9 68 b9 ff ff       	jmp    801022f0 <kfree>
    panic("freevm: no pgdir");
80106988:	c7 04 24 b9 75 10 80 	movl   $0x801075b9,(%esp)
8010698f:	e8 cc 99 ff ff       	call   80100360 <panic>
80106994:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
8010699a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

801069a0 <setupkvm>:
{
801069a0:	55                   	push   %ebp
801069a1:	89 e5                	mov    %esp,%ebp
801069a3:	56                   	push   %esi
801069a4:	53                   	push   %ebx
801069a5:	83 ec 10             	sub    $0x10,%esp
  if((pgdir = (pde_t*)kalloc()) == 0)
801069a8:	e8 f3 ba ff ff       	call   801024a0 <kalloc>
801069ad:	85 c0                	test   %eax,%eax
801069af:	89 c6                	mov    %eax,%esi
801069b1:	74 6d                	je     80106a20 <setupkvm+0x80>
  memset(pgdir, 0, PGSIZE);
801069b3:	c7 44 24 08 00 10 00 	movl   $0x1000,0x8(%esp)
801069ba:	00 
  for(k = kmap; k < &kmap[NELEM(kmap)]; k++)
801069bb:	bb 20 a4 10 80       	mov    $0x8010a420,%ebx
  memset(pgdir, 0, PGSIZE);
801069c0:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
801069c7:	00 
801069c8:	89 04 24             	mov    %eax,(%esp)
801069cb:	e8 d0 d8 ff ff       	call   801042a0 <memset>
    if(mappages(pgdir, k->virt, k->phys_end - k->phys_start,
801069d0:	8b 53 0c             	mov    0xc(%ebx),%edx
801069d3:	8b 43 04             	mov    0x4(%ebx),%eax
801069d6:	8b 4b 08             	mov    0x8(%ebx),%ecx
801069d9:	89 54 24 04          	mov    %edx,0x4(%esp)
801069dd:	8b 13                	mov    (%ebx),%edx
801069df:	89 04 24             	mov    %eax,(%esp)
801069e2:	29 c1                	sub    %eax,%ecx
801069e4:	89 f0                	mov    %esi,%eax
801069e6:	e8 d5 f9 ff ff       	call   801063c0 <mappages>
801069eb:	85 c0                	test   %eax,%eax
801069ed:	78 19                	js     80106a08 <setupkvm+0x68>
  for(k = kmap; k < &kmap[NELEM(kmap)]; k++)
801069ef:	83 c3 10             	add    $0x10,%ebx
801069f2:	81 fb 60 a4 10 80    	cmp    $0x8010a460,%ebx
801069f8:	72 d6                	jb     801069d0 <setupkvm+0x30>
801069fa:	89 f0                	mov    %esi,%eax
}
801069fc:	83 c4 10             	add    $0x10,%esp
801069ff:	5b                   	pop    %ebx
80106a00:	5e                   	pop    %esi
80106a01:	5d                   	pop    %ebp
80106a02:	c3                   	ret    
80106a03:	90                   	nop
80106a04:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      freevm(pgdir);
80106a08:	89 34 24             	mov    %esi,(%esp)
80106a0b:	e8 10 ff ff ff       	call   80106920 <freevm>
}
80106a10:	83 c4 10             	add    $0x10,%esp
      return 0;
80106a13:	31 c0                	xor    %eax,%eax
}
80106a15:	5b                   	pop    %ebx
80106a16:	5e                   	pop    %esi
80106a17:	5d                   	pop    %ebp
80106a18:	c3                   	ret    
80106a19:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
    return 0;
80106a20:	31 c0                	xor    %eax,%eax
80106a22:	eb d8                	jmp    801069fc <setupkvm+0x5c>
80106a24:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80106a2a:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

80106a30 <kvmalloc>:
{
80106a30:	55                   	push   %ebp
80106a31:	89 e5                	mov    %esp,%ebp
80106a33:	83 ec 08             	sub    $0x8,%esp
  kpgdir = setupkvm();
80106a36:	e8 65 ff ff ff       	call   801069a0 <setupkvm>
80106a3b:	a3 a4 55 11 80       	mov    %eax,0x801155a4
  lcr3(V2P(kpgdir));   // switch to the kernel page table
80106a40:	05 00 00 00 80       	add    $0x80000000,%eax
80106a45:	0f 22 d8             	mov    %eax,%cr3
}
80106a48:	c9                   	leave  
80106a49:	c3                   	ret    
80106a4a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

80106a50 <clearpteu>:

// Clear PTE_U on a page. Used to create an inaccessible
// page beneath the user stack.
void
clearpteu(pde_t *pgdir, char *uva)
{
80106a50:	55                   	push   %ebp
  pte_t *pte;

  pte = walkpgdir(pgdir, uva, 0);
80106a51:	31 c9                	xor    %ecx,%ecx
{
80106a53:	89 e5                	mov    %esp,%ebp
80106a55:	83 ec 18             	sub    $0x18,%esp
  pte = walkpgdir(pgdir, uva, 0);
80106a58:	8b 55 0c             	mov    0xc(%ebp),%edx
80106a5b:	8b 45 08             	mov    0x8(%ebp),%eax
80106a5e:	e8 cd f8 ff ff       	call   80106330 <walkpgdir>
  if(pte == 0)
80106a63:	85 c0                	test   %eax,%eax
80106a65:	74 05                	je     80106a6c <clearpteu+0x1c>
    panic("clearpteu");
  *pte &= ~PTE_U;
80106a67:	83 20 fb             	andl   $0xfffffffb,(%eax)
}
80106a6a:	c9                   	leave  
80106a6b:	c3                   	ret    
    panic("clearpteu");
80106a6c:	c7 04 24 ca 75 10 80 	movl   $0x801075ca,(%esp)
80106a73:	e8 e8 98 ff ff       	call   80100360 <panic>
80106a78:	90                   	nop
80106a79:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

80106a80 <copyuvm>:

// Given a parent process's page table, create a copy
// of it for a child.
pde_t*
copyuvm(pde_t *pgdir, uint sz)
{
80106a80:	55                   	push   %ebp
80106a81:	89 e5                	mov    %esp,%ebp
80106a83:	57                   	push   %edi
80106a84:	56                   	push   %esi
80106a85:	53                   	push   %ebx
80106a86:	83 ec 2c             	sub    $0x2c,%esp
  pde_t *d;
  pte_t *pte;
  uint pa, i, flags;
  char *mem;

  if((d = setupkvm()) == 0)
80106a89:	e8 12 ff ff ff       	call   801069a0 <setupkvm>
80106a8e:	85 c0                	test   %eax,%eax
80106a90:	89 45 e0             	mov    %eax,-0x20(%ebp)
80106a93:	0f 84 b9 00 00 00    	je     80106b52 <copyuvm+0xd2>
    return 0;
  for(i = 0; i < sz; i += PGSIZE){
80106a99:	8b 45 0c             	mov    0xc(%ebp),%eax
80106a9c:	85 c0                	test   %eax,%eax
80106a9e:	0f 84 94 00 00 00    	je     80106b38 <copyuvm+0xb8>
80106aa4:	31 ff                	xor    %edi,%edi
80106aa6:	eb 48                	jmp    80106af0 <copyuvm+0x70>
      panic("copyuvm: page not present");
    pa = PTE_ADDR(*pte);
    flags = PTE_FLAGS(*pte);
    if((mem = kalloc()) == 0)
      goto bad;
    memmove(mem, (char*)P2V(pa), PGSIZE);
80106aa8:	81 c6 00 00 00 80    	add    $0x80000000,%esi
80106aae:	c7 44 24 08 00 10 00 	movl   $0x1000,0x8(%esp)
80106ab5:	00 
80106ab6:	89 74 24 04          	mov    %esi,0x4(%esp)
80106aba:	89 04 24             	mov    %eax,(%esp)
80106abd:	e8 7e d8 ff ff       	call   80104340 <memmove>
    if(mappages(d, (void*)i, PGSIZE, V2P(mem), flags) < 0) {
80106ac2:	8b 45 e4             	mov    -0x1c(%ebp),%eax
80106ac5:	b9 00 10 00 00       	mov    $0x1000,%ecx
80106aca:	89 fa                	mov    %edi,%edx
80106acc:	89 44 24 04          	mov    %eax,0x4(%esp)
80106ad0:	8d 83 00 00 00 80    	lea    -0x80000000(%ebx),%eax
80106ad6:	89 04 24             	mov    %eax,(%esp)
80106ad9:	8b 45 e0             	mov    -0x20(%ebp),%eax
80106adc:	e8 df f8 ff ff       	call   801063c0 <mappages>
80106ae1:	85 c0                	test   %eax,%eax
80106ae3:	78 63                	js     80106b48 <copyuvm+0xc8>
  for(i = 0; i < sz; i += PGSIZE){
80106ae5:	81 c7 00 10 00 00    	add    $0x1000,%edi
80106aeb:	39 7d 0c             	cmp    %edi,0xc(%ebp)
80106aee:	76 48                	jbe    80106b38 <copyuvm+0xb8>
    if((pte = walkpgdir(pgdir, (void *) i, 0)) == 0)
80106af0:	8b 45 08             	mov    0x8(%ebp),%eax
80106af3:	31 c9                	xor    %ecx,%ecx
80106af5:	89 fa                	mov    %edi,%edx
80106af7:	e8 34 f8 ff ff       	call   80106330 <walkpgdir>
80106afc:	85 c0                	test   %eax,%eax
80106afe:	74 62                	je     80106b62 <copyuvm+0xe2>
    if(!(*pte & PTE_P))
80106b00:	8b 00                	mov    (%eax),%eax
80106b02:	a8 01                	test   $0x1,%al
80106b04:	74 50                	je     80106b56 <copyuvm+0xd6>
    pa = PTE_ADDR(*pte);
80106b06:	89 c6                	mov    %eax,%esi
    flags = PTE_FLAGS(*pte);
80106b08:	25 ff 0f 00 00       	and    $0xfff,%eax
80106b0d:	89 45 e4             	mov    %eax,-0x1c(%ebp)
    pa = PTE_ADDR(*pte);
80106b10:	81 e6 00 f0 ff ff    	and    $0xfffff000,%esi
    if((mem = kalloc()) == 0)
80106b16:	e8 85 b9 ff ff       	call   801024a0 <kalloc>
80106b1b:	85 c0                	test   %eax,%eax
80106b1d:	89 c3                	mov    %eax,%ebx
80106b1f:	75 87                	jne    80106aa8 <copyuvm+0x28>
    }
  }
  return d;

bad:
  freevm(d);
80106b21:	8b 45 e0             	mov    -0x20(%ebp),%eax
80106b24:	89 04 24             	mov    %eax,(%esp)
80106b27:	e8 f4 fd ff ff       	call   80106920 <freevm>
  return 0;
80106b2c:	31 c0                	xor    %eax,%eax
}
80106b2e:	83 c4 2c             	add    $0x2c,%esp
80106b31:	5b                   	pop    %ebx
80106b32:	5e                   	pop    %esi
80106b33:	5f                   	pop    %edi
80106b34:	5d                   	pop    %ebp
80106b35:	c3                   	ret    
80106b36:	66 90                	xchg   %ax,%ax
80106b38:	8b 45 e0             	mov    -0x20(%ebp),%eax
80106b3b:	83 c4 2c             	add    $0x2c,%esp
80106b3e:	5b                   	pop    %ebx
80106b3f:	5e                   	pop    %esi
80106b40:	5f                   	pop    %edi
80106b41:	5d                   	pop    %ebp
80106b42:	c3                   	ret    
80106b43:	90                   	nop
80106b44:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      kfree(mem);
80106b48:	89 1c 24             	mov    %ebx,(%esp)
80106b4b:	e8 a0 b7 ff ff       	call   801022f0 <kfree>
      goto bad;
80106b50:	eb cf                	jmp    80106b21 <copyuvm+0xa1>
    return 0;
80106b52:	31 c0                	xor    %eax,%eax
80106b54:	eb d8                	jmp    80106b2e <copyuvm+0xae>
      panic("copyuvm: page not present");
80106b56:	c7 04 24 ee 75 10 80 	movl   $0x801075ee,(%esp)
80106b5d:	e8 fe 97 ff ff       	call   80100360 <panic>
      panic("copyuvm: pte should exist");
80106b62:	c7 04 24 d4 75 10 80 	movl   $0x801075d4,(%esp)
80106b69:	e8 f2 97 ff ff       	call   80100360 <panic>
80106b6e:	66 90                	xchg   %ax,%ax

80106b70 <uva2ka>:

//PAGEBREAK!
// Map user virtual address to kernel address.
char*
uva2ka(pde_t *pgdir, char *uva)
{
80106b70:	55                   	push   %ebp
  pte_t *pte;

  pte = walkpgdir(pgdir, uva, 0);
80106b71:	31 c9                	xor    %ecx,%ecx
{
80106b73:	89 e5                	mov    %esp,%ebp
80106b75:	83 ec 08             	sub    $0x8,%esp
  pte = walkpgdir(pgdir, uva, 0);
80106b78:	8b 55 0c             	mov    0xc(%ebp),%edx
80106b7b:	8b 45 08             	mov    0x8(%ebp),%eax
80106b7e:	e8 ad f7 ff ff       	call   80106330 <walkpgdir>
  if((*pte & PTE_P) == 0)
80106b83:	8b 00                	mov    (%eax),%eax
80106b85:	89 c2                	mov    %eax,%edx
80106b87:	83 e2 05             	and    $0x5,%edx
    return 0;
  if((*pte & PTE_U) == 0)
80106b8a:	83 fa 05             	cmp    $0x5,%edx
80106b8d:	75 11                	jne    80106ba0 <uva2ka+0x30>
    return 0;
  return (char*)P2V(PTE_ADDR(*pte));
80106b8f:	25 00 f0 ff ff       	and    $0xfffff000,%eax
80106b94:	05 00 00 00 80       	add    $0x80000000,%eax
}
80106b99:	c9                   	leave  
80106b9a:	c3                   	ret    
80106b9b:	90                   	nop
80106b9c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    return 0;
80106ba0:	31 c0                	xor    %eax,%eax
}
80106ba2:	c9                   	leave  
80106ba3:	c3                   	ret    
80106ba4:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
80106baa:	8d bf 00 00 00 00    	lea    0x0(%edi),%edi

80106bb0 <copyout>:
// Copy len bytes from p to user address va in page table pgdir.
// Most useful when pgdir is not the current page table.
// uva2ka ensures this only works for PTE_U pages.
int
copyout(pde_t *pgdir, uint va, void *p, uint len)
{
80106bb0:	55                   	push   %ebp
80106bb1:	89 e5                	mov    %esp,%ebp
80106bb3:	57                   	push   %edi
80106bb4:	56                   	push   %esi
80106bb5:	53                   	push   %ebx
80106bb6:	83 ec 1c             	sub    $0x1c,%esp
80106bb9:	8b 5d 14             	mov    0x14(%ebp),%ebx
80106bbc:	8b 4d 0c             	mov    0xc(%ebp),%ecx
80106bbf:	8b 7d 10             	mov    0x10(%ebp),%edi
  char *buf, *pa0;
  uint n, va0;

  buf = (char*)p;
  while(len > 0){
80106bc2:	85 db                	test   %ebx,%ebx
80106bc4:	75 3a                	jne    80106c00 <copyout+0x50>
80106bc6:	eb 68                	jmp    80106c30 <copyout+0x80>
    va0 = (uint)PGROUNDDOWN(va);
    pa0 = uva2ka(pgdir, (char*)va0);
    if(pa0 == 0)
      return -1;
    n = PGSIZE - (va - va0);
80106bc8:	8b 4d e4             	mov    -0x1c(%ebp),%ecx
80106bcb:	89 f2                	mov    %esi,%edx
    if(n > len)
      n = len;
    memmove(pa0 + (va - va0), buf, n);
80106bcd:	89 7c 24 04          	mov    %edi,0x4(%esp)
    n = PGSIZE - (va - va0);
80106bd1:	29 ca                	sub    %ecx,%edx
80106bd3:	81 c2 00 10 00 00    	add    $0x1000,%edx
80106bd9:	39 da                	cmp    %ebx,%edx
80106bdb:	0f 47 d3             	cmova  %ebx,%edx
    memmove(pa0 + (va - va0), buf, n);
80106bde:	29 f1                	sub    %esi,%ecx
80106be0:	01 c8                	add    %ecx,%eax
80106be2:	89 54 24 08          	mov    %edx,0x8(%esp)
80106be6:	89 04 24             	mov    %eax,(%esp)
80106be9:	89 55 e4             	mov    %edx,-0x1c(%ebp)
80106bec:	e8 4f d7 ff ff       	call   80104340 <memmove>
    len -= n;
    buf += n;
80106bf1:	8b 55 e4             	mov    -0x1c(%ebp),%edx
    va = va0 + PGSIZE;
80106bf4:	8d 8e 00 10 00 00    	lea    0x1000(%esi),%ecx
    buf += n;
80106bfa:	01 d7                	add    %edx,%edi
  while(len > 0){
80106bfc:	29 d3                	sub    %edx,%ebx
80106bfe:	74 30                	je     80106c30 <copyout+0x80>
    pa0 = uva2ka(pgdir, (char*)va0);
80106c00:	8b 45 08             	mov    0x8(%ebp),%eax
    va0 = (uint)PGROUNDDOWN(va);
80106c03:	89 ce                	mov    %ecx,%esi
80106c05:	81 e6 00 f0 ff ff    	and    $0xfffff000,%esi
    pa0 = uva2ka(pgdir, (char*)va0);
80106c0b:	89 74 24 04          	mov    %esi,0x4(%esp)
    va0 = (uint)PGROUNDDOWN(va);
80106c0f:	89 4d e4             	mov    %ecx,-0x1c(%ebp)
    pa0 = uva2ka(pgdir, (char*)va0);
80106c12:	89 04 24             	mov    %eax,(%esp)
80106c15:	e8 56 ff ff ff       	call   80106b70 <uva2ka>
    if(pa0 == 0)
80106c1a:	85 c0                	test   %eax,%eax
80106c1c:	75 aa                	jne    80106bc8 <copyout+0x18>
  }
  return 0;
}
80106c1e:	83 c4 1c             	add    $0x1c,%esp
      return -1;
80106c21:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
80106c26:	5b                   	pop    %ebx
80106c27:	5e                   	pop    %esi
80106c28:	5f                   	pop    %edi
80106c29:	5d                   	pop    %ebp
80106c2a:	c3                   	ret    
80106c2b:	90                   	nop
80106c2c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
80106c30:	83 c4 1c             	add    $0x1c,%esp
  return 0;
80106c33:	31 c0                	xor    %eax,%eax
}
80106c35:	5b                   	pop    %ebx
80106c36:	5e                   	pop    %esi
80106c37:	5f                   	pop    %edi
80106c38:	5d                   	pop    %ebp
80106c39:	c3                   	ret    
