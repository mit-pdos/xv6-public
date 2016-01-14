struct FAT32_DBR
{
  uchar BS_jmpBoot[3];    //跳转指令            offset:0
  uchar BS_OEMName[8]; //                   offset: 3
  ushort BPB_BytesPerSec;//每扇区字节数        offset:11
  uchar BPB_SecPerClus; //每簇扇区数          offset:13
  ushort BPB_RsvdSecCnt; //保留扇区数目(从DBR到FAT的扇区数)       offset:14
  uchar BPB_NumFATs;  //此卷中FAT表数     offset:16
  ushort BPB_RootEntCnt; //FAT32为0           offset:17 根目录最多容纳的目录相数，FAT12/16位512
  ushort BPB_TotSec16;   //FAT32为0           offset:19 扇区总数小于32M时使用
  uchar BPB_Media;    //存储介质            offset:21 0XF8本地硬盘固定0XF0移动存储介质
  ushort BPB_FATSz16;   //FAT32为0         offset:22 每个FAT表的大小扇区数
  ushort BPB_SecPerTrk;  //磁道扇区数          offset:24
  ushort BPB_NumHeads;   //磁头数            offset:26
  uint BPB_HiddSec;    //隐扇区数（从MBR到DBR的扇区数）    offset:28 分区前已使用扇区数
  uint BPB_TotSec32;   //该卷总扇区数        offset:32
  uint BPB_FATSz32;    //一个FAT表扇区数   offset:36
  ushort BPB_ExtFlags;   //FAT32特有           offset:40
  ushort BPB_FSVer;      //FAT32特有          offset:42 版本号
  uint BPB_RootClus;   //根目录簇号          offset:44
  ushort FSInfo;         //保留扇区FSINFO扇区数offset:48
  ushort BPB_BkBootSec;  //通常为6           offset:50
  uchar BPB_Reserved[12];  //扩展用             offset:52
  uchar BS_DrvNum;     //                  offset:64
  uchar BS_Reserved1;  //                   offset:65
  uchar BS_BootSig;    //                   offset:66
  uint BS_VolID;      //                   offset:67
  uchar BS_FilSysType[11]; //                offset:71
  uchar BS_FilSysType1[8];//"FAT32    "        offset:82
};

struct direntry
{
   uchar deName[8];       // 文件名
   uchar deExtension[3];    // 扩展名
   uchar deAttributes;     // 文件属性
   uchar deLowerCase;         // 系统保留
   uchar deCHundredth;         // 创建时间的10 毫秒位
   ushort deCTime;           // 文件创建时间
   ushort deCDate;    // 文件创建日期
   ushort deADate;    // 文件最后访问日期
   ushort deHighClust;  // 文件起始簇号的高16 位
   ushort deMTime;          // 文件的最近修改时间
   ushort deMDate;           // 文件的最近修改日期
   ushort deLowCluster;//　文件起始簇号的低16 位
   uint deFileSize;   // 表示文件的长度
};

struct finode {
  short type;           // File type
  short major;          // Major device number (T_DEV only)
  short minor;          // Minor device number (T_DEV only)
  short nlink;          // Number of links to inode in file system
  uint size;            // Size of file (bytes)
  uint addrs[NDIRECT+2];   // Data block addresses
};
