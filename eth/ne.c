#include "../types.h"
#include "../x86.h"
#include "../defs.h"
#include "../net/net.h"
#include "ne.h"

// ※コメントの[数値(キーワード)]はDP8390仕様書のページ番号
// DP8390 Documents: http://www.national.com/ds/DP/DP8390D.pdf

// NICの接続チェックとMACアドレスの取得。
int
ne_probe(ne_t* ne)
{
  uchar eprom[32];
  int reg0;
  int i;
  
  reg0 = inb(ne->base);
  if (reg0 == 0xFF)
    return FALSE;

  // DP8390が存在するかのチェック
  {
    int regd;
    // ページ1にしてMAR5を保存してから0xFFを書き込む。
    outb(ne->base + DP_CR, CR_STP | CR_NO_DMA | CR_PS_P1);
    regd = inb(ne->base + DP_MAR5);
    outb(ne->base + DP_MAR5, 0xFF);
    // [17] ページ1のMAR5はページ0のCNTR0に相当する。
    outb(ne->base + DP_CR, CR_NO_DMA | CR_PS_P0);
    // [29] CNTR0レジスタはCRCエラーの時にインクリメントされるカウンタ。
    // プロセッサに読み出された後にクリアされる仕様。
    inb(ne->base + DP_CNTR0);
    if (inb(ne->base + DP_CNTR0) != 0) {
      // 違うようなので値を戻しておく。
      outb(ne->base, reg0);
      outb(ne->base + DP_TCR, regd);
      cprintf("%s: This is not NEx000.\n", ne->name);
      return FALSE;
    }
  }

  // ボードのリセット
  {
    int i = 0;
    // よく分からないがこれでリセットできるようだ
    outb(ne->base + NE_RESET, inb(ne->base + NE_RESET));
    // リセットのISR(割り込み)をpollingする。
    while (inb(ne->base + DP_ISR) == 0) {
      // 一応終わるようにしておく。
      // できれば20msなど正確なタイムアウト処理ができるとよいのだが、
      // xv6では現時点では正確にsleepする処理が作成されていないので適当。
      if (i++ > 10000) {
        cprintf("%s: NIC reset failure\n", ne->name);
        return FALSE;
      }
    }
    // [20] ISRは各ビットが1の時、割り込み無しと言う意味。
    // 0のビットがあった場合、CPUに割り込みが発生し、CPUは処理したらフラグを
    // 立て直すが、まだCPUへの割り込みは無効なので、手動で直しておく。
    outb(ne->base + DP_ISR, 0xFF);
  }

  // PROMから16バイト(正確には32バイトであるが)読み出す。
  // そのためには、事前に基本に沿った初期化処理を行っておく必要があるようだ。
  // (経験の賜なようなので詳細は無視！)
  {
    int i;
    // [27] DMA関連のレジスタには16ビットのものもあるが、
    // 8ビット幅でアクセスしなければならない(ex: RBCR0, RBCR1)。
    // [29] seqは概ねリファレンス通りの初期化処理
    struct {
      uchar offset, value;
    } seq[] = {
      // 1. ページ0の書き込みモード。DMAはOFF。NICはオフラインに。
      { DP_CR, CR_NO_DMA | CR_PS_P0 | CR_STP },
      // 2. バイト幅でのアクセスで、バーストモード。
      { DP_DCR, (DCR_BMS | DCR_8BYTES) },
      // 3. カウントレジスタをクリア
      { DP_RBCR0, 0x00 }, { DP_RBCR1, 0x00 },
      // 4. モニターモード(メモリに書かれない？)
      { DP_RCR, RCR_MON },
      // 5. ループバックモード
      { DP_TCR, TCR_INTERNAL },
      // (6はいらない？)
      // 7. ISRの初期化
      { DP_ISR, 0xFF },
      // 8. とりあえず割り込み全無効化
      { DP_IMR, 0x00 },
      // (9はいらない？)

      // この後PROMの読み出しするので、その旨を設定しておく
      // 32バイト読み込むぞ
      { DP_RBCR0, 32 }, { DP_RBCR1, 0 },
      // メモリの0x00から
      { DP_RSAR0, 0x00 }, { DP_RSAR1, 0x00 },

      // 10. ページ0の読み込みモード。NICはオンラインになるが、
      // ループバックモードなのでlocal receive DMAはまだ動いてない。
      { DP_CR, (CR_PS_P0 | CR_DM_RR | CR_STA) },
    };
    for (i = 0; i < NELEM(seq); ++i)
      outb(ne->base + seq[i].offset, seq[i].value);
    // 8ビットのNIC(NE1000?)かどうかもチェック
    ne->is16bit = TRUE;
    for (i = 0; i < 32; i += 2) {
      eprom[i+0] = inb(ne->base + NE_DATA);
      eprom[i+1] = inb(ne->base + NE_DATA);
      // NE2000やそのクローンは奇数偶数同じなようだ。違ったら多分NE1000。
      if (eprom[i+0] != eprom[i+1])
        ne->is16bit = FALSE;
    }
    // ノーマライズ
    if (ne->is16bit)
      for (i = 0; i < 16; ++i)
        eprom[i] = eprom[i*2];
    // NE2000(NE1000)は14, 15番目が0x57らしい。
    if (eprom[14] != 0x57 || eprom[15] != 0x57)
      return FALSE;
  }
  
  // MACアドレスの取得
  for (i = 0; i < 6; ++i)
    ne->address[i] = eprom[i];
  
  return TRUE;
}

// NICの初期化処理
void
ne_init(ne_t* ne)
{
  int i;

  // ページ関連の値の算出
  if (ne->is16bit) {
    ne->ramsize = NE2000_SIZE;
    ne->startaddr = NE2000_START;
    ne->send_startpage = NE2000_START / DP_PAGESIZE;
  } else {
    ne->startaddr = NE1000_START;
    ne->ramsize = NE1000_SIZE;
    ne->send_startpage = NE1000_START / DP_PAGESIZE;
  }
  ne->pages = ne->ramsize / DP_PAGESIZE;
  ne->send_stoppage = ne->send_startpage + SENDQ_PAGES * SENDQ_LEN - 1;
  ne->recv_startpage = ne->send_stoppage + 1;
  ne->recv_stoppage = ne->send_startpage + ne->pages;
  // send queueのための下準備
  for (i = 0; i < SENDQ_LEN; ++i) {
    ne->sendq[i].sendpage = ne->send_startpage + i * SENDQ_PAGES;
    ne->sendq[i].filled = 0;
  }
  ne->sendq_head = 0;
  ne->sendq_tail = SENDQ_LEN-1;

  // ステータスを表示しておく
  cprintf("%s: NE%d000 (%dkB RAM) at 0x%x:%d - ",
         ne->name,
         ne->is16bit ? 2 : 1,
         ne->ramsize/1024,
         ne->base,
         ne->irq);
  for (i = 0; i < 6; ++i)
    cprintf("%x%s", ne->address[i], i < 5 ? ":" : "\n");

  // [29] 基本的な初期化処理
  {
    struct {
      uchar offset, value;
    } seq[] = {
      // 1. CR
      { DP_CR, CR_PS_P0 | CR_STP | CR_NO_DMA },
      // 2. DCR. [5(PRQ)] LASが指定されていないので16bitモード。
      { DP_DCR, ((ne->is16bit ? DCR_WORDWIDE : DCR_BYTEWIDE) |
                 DCR_LTLENDIAN | DCR_8BYTES | DCR_BMS) },
      // 3. RCR
      { DP_RCR, RCR_MON },
      // 4. RBCR
      { DP_RBCR0, 0 }, { DP_RBCR1, 0 },
      // 5. TCR
      { DP_TCR, TCR_INTERNAL },
      // 6. リングバッファ初期化 [10]参照
      { DP_PSTART, ne->recv_startpage },
      { DP_PSTOP, ne->recv_stoppage },
      { DP_BNRY, ne->recv_startpage }, // CURRの1つ前であれば良い
      // 7. ISR初期化
      { DP_ISR, 0xFF },
      // 8. IMR初期化(全割り込みを許容する)
      { DP_IMR, (IMR_PRXE | IMR_PTXE | IMR_RXEE |
                 IMR_TXEE | IMR_OVWE | IMR_CNTE) },
      // 9. ページ1に移動し、
      { DP_CR, CR_PS_P1 | CR_NO_DMA },
      // 9. i. PAR0-5を初期化
      { DP_PAR0, ne->address[0] }, { DP_PAR1, ne->address[1] },
      { DP_PAR2, ne->address[2] }, { DP_PAR3, ne->address[3] },
      { DP_PAR4, ne->address[4] }, { DP_PAR5, ne->address[5] },
      // 9. ii. MAR0-7を初期化
      { DP_MAR0, 0xFF }, { DP_MAR1, 0xFF }, { DP_MAR2, 0xFF },
      { DP_MAR3, 0xFF }, { DP_MAR4, 0xFF }, { DP_MAR5, 0xFF },
      { DP_MAR6, 0xFF }, { DP_MAR7, 0xFF },
      // 9. iii. CURRent pointerを初期化
      { DP_CURR, ne->recv_startpage + 1 },
      // 10. NICをスタートモードに(0x22)。まだDMAは動いていない。
      { DP_CR, CR_STA | CR_NO_DMA },
      // 11. TCRを変更(NIC動作開始？)
      { DP_TCR, TCR_NORMAL },
      // RCRはとりあえずPromiscuous(何でも受け入れ)モードに
      // (設定できるようにしたほうが良いのか？)
      { DP_RCR, RCR_PRO },
    };
    for (i = 0; i < NELEM(seq); ++i)
      outb(ne->base + seq[i].offset, seq[i].value);
  }

  return;
}

// 読み書きする際のリモートDMAのセットアップ
// mode: CR_DM_R*, addr: ローカルバッファの実アドレス、size: バイト数
void
ne_rdma_setup(ne_t* ne, int mode, ushort addr, int size)
{
  // [13-14] Writeの時はPRQ(Port ReQest)を出さなければならない
  if (mode == CR_DM_RW) {
    // このコードは謎
    //outb(ne->base + DP_ISR, ISR_RDC);
    // Dummy DMA Readを行う
    uchar dummy[4];
    ushort safeloc = ne->startaddr - sizeof(dummy);
    int oldcrda, newcrda;
    oldcrda = inb(ne->base + DP_CRDA0);
    oldcrda |= ((inb(ne->base + DP_CRDA1) << 8) & 0xFF00);
    ne_getblock(ne, safeloc, sizeof(dummy), dummy);
    // required delay(polling)
    do {
      newcrda = inb(ne->base + DP_CRDA0);
      newcrda |= ((inb(ne->base + DP_CRDA1) << 8) & 0xFF00);
    } while (oldcrda == newcrda);
  }
  // リモートDMAでデータポートを介して双方向の転送ができる。
  // 転送されるごとに、RBCRは減少、RSARは増加する
  // RBCRが0になったら転送終了。
  outb(ne->base + DP_RSAR0, addr & 0xFF);
  outb(ne->base + DP_RSAR1, (addr >> 8) & 0xFF);
  outb(ne->base + DP_RBCR0, size & 0xFF);
  outb(ne->base + DP_RBCR1, (size >> 8) & 0xFF);
  // RemoteDMA処理開始
  outb(ne->base + DP_CR, mode | CR_PS_P0 | CR_STA);
  return;
}

// RAMのaddrからsizeバイトdstに読み込み。
void
ne_getblock(ne_t* ne, ushort addr, int size, void* dst)
{
  ne_rdma_setup(ne, CR_DM_RR, addr, size);
  if (ne->is16bit)
    insw(ne->base + NE_DATA, dst, size);
  else
    insb(ne->base + NE_DATA, dst, size);
  return;
}

// データの転送開始処理
// ローカルバッファにあるpageページからsizeバイト送信開始
void
ne_start_xmit(ne_t* ne, int page, int size)
{
  outb(ne->base + DP_TPSR, page);
  outb(ne->base + DP_TBCR0, size & 0xFF);
  outb(ne->base + DP_TBCR1, (size >> 8) & 0xFF);
  // [12,19] TXPで転送開始となる。
  // 転送終了または失敗したら内部的にフラグはリセットされる。
  // 事前にTBCRとTPSRをセットしておくこと。
  outb(ne->base + DP_CR, CR_PS_P0 | CR_NO_DMA | CR_STA | CR_TXP); // 転送開始！
  return;
}

// sizeバイトのpacketをローカルバッファに書き込み、その後送信する
// sizeは適正([46, 1514])であること
int
ne_pio_write(ne_t* ne, uchar* packet, int size)
{
  // sendqの先頭がちゃんと空いてる？
  int q = ne->sendq_head % SENDQ_LEN;
  if (ne->sendq[q].filled || ne->sendq_head > ne->sendq_tail) {
    cprintf("%s: all transmitting buffers in NIC are busy.\n", ne->name);
    return 0;
  }

  // 書き込み処理
  ne_rdma_setup(ne, CR_DM_RW, ne->sendq[q].sendpage * DP_PAGESIZE, size);
  if (ne->is16bit)
    outsw(ne->base + NE_DATA, packet, size);
  else
    outsb(ne->base + NE_DATA, packet, size);
  // ToDo: ここでISR_RDCのチェックは必要なのか？

  ne->sendq[q].filled = TRUE;
  ne_start_xmit(ne, ne->sendq[q].sendpage, size);

  // ToDo: 多分算術オーバーフロー対策しないといけない。
  ne->sendq_head++;
  
  return size;
}

// [11] Strage Format
typedef struct {
  uchar status;           // Receive Status
  uchar next;             // Next Packet Pointer
  uchar rbc0, rbc1;       // Receive Byte Count 0 (low)
} ne_recv_hdr;            // Receive Byte Count 1 (high)

// ローカルバッファのpageからの一連のデータを読み込む。
// bufsizeはbufサイズ。足りない場合パケットサイズを返す。
// また、bufがNULLならパケットサイズ。
// bufがNULLでないなら読み込んだサイズを返す。
// 0ならパケットなし、-1ならエラー。
int
ne_pio_read(ne_t* ne, uchar* buf, int bufsize)
{
  uint pktsize;
  ne_recv_hdr header;
  uint curr, bnry, page;
  // CURR: 次にNICが書きこむべきページが格納
  // BNRY: 次に読み込むべきページの一つ前のページが格納
  // よって、CURRがBNRYに追いついたらOverflow
  outb(ne->base + DP_CR, CR_PS_P1); // CURRはページ2なので切り替えが必要
  curr = inb(ne->base + DP_CURR);
  outb(ne->base + DP_CR, CR_PS_P0 | CR_NO_DMA | CR_STA);
  bnry = inb(ne->base + DP_BNRY);
  page = bnry + 1;
  
  // 末尾だったら先頭に。
  if (page == ne->recv_stoppage)
    page = ne->recv_startpage;

  // CURRに追いついた＝パケットがない（CURRはstoppageになることがあるのか？）
  //if (bnry + 1 == curr || next == curr) {
  if (page == curr) {
    cprintf("%s: no packet to read\n", ne->name);
    return 0;
  }

  // バッファの先頭にあるヘッダ(StrageFormat)を読み出す
  ne_getblock(ne, page * DP_PAGESIZE, sizeof(header), &header);
  // パケットサイズを算出
  pktsize = (header.rbc0 | (header.rbc1 << 8)) - sizeof(header);
  
  // パケット長がイーサネットのパケットサイズ範囲内か
  if (pktsize < ETH_MIN_SIZE || pktsize > ETH_MAX_SIZE) {
    cprintf("%s: Packet with strange length arrived: %d\n",
            ne->name, pktsize);
    return -1;
  }
  // statusが問題ないか
  if ((header.status & RSR_PRX) == 0) {
    cprintf("%s: Bad status: %d\n", ne->name, header.status);
    return -1;
  }

  if (buf == 0 || pktsize > bufsize) {
    return pktsize;
  } else {
    // データ読み込み。末尾から先頭に折り返しもチェックする。
    int remain = (ne->recv_stoppage - page) * DP_PAGESIZE;
    if (remain < pktsize) {
      ne_getblock(ne, page * DP_PAGESIZE + sizeof(header), remain, buf);
      ne_getblock(ne, ne->recv_startpage * DP_PAGESIZE, pktsize - remain, buf);
    } else {
      ne_getblock(ne, page * DP_PAGESIZE + sizeof(header), pktsize, buf);
    }
  }

  // 次のパケットの1ページ前にBNRYを進める
  bnry = header.next - 1;
  outb(ne->base + DP_BNRY,
       bnry < ne->recv_startpage ? ne->recv_stoppage-1 : bnry);

  return pktsize;
}

void
ne_interrupt(ne_t* ne)
{
  int isr;

  while ((isr = inb(ne->base + DP_ISR)) != 0) {
    outb(ne->base + DP_ISR, isr);
    if (isr & ISR_PTX) {
      ne->sendq_tail++;
      ne->sendq[ne->sendq_tail%SENDQ_LEN].filled = FALSE;
      cprintf("%s: packet transmitted with no error.\n", ne->name);
    }
    if (isr & ISR_PRX) {
      cprintf("%s: packet received with no error.\n", ne->name);
    }
#if 0
    if (~(isr & (ISR_PTX | ISR_PRX)) != 0) {
      cprintf("%s: this interrupt event is not implemented now.\n", ne->name);
      panic("panic end\n");
    }
#endif
  }
}






