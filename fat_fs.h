#ifndef FAT_FS_H
#define FAT_FS_H

#define SECSIZE    512  // sector size (bytes) 单个扇区字节数
#define SECPERCLUS   8  // number of sector per cluster 每簇扇区数
#define RETAINSEC    2  // number of retain sectors 保留扇区个数

struct FAT32_DBR
{
  uchar jmpBoot[3];    //跳转指令            offset:0
  uchar OEMName[8]; //                   offset: 3
  ushort BytesPerSec;//每扇区字节数        offset:11
  uchar SecPerClus; //每簇扇区数          offset:13
  ushort RsvdSecCnt; //保留扇区数目(从DBR到FAT的扇区数)       offset:14
  uchar NumFATs;  //此卷中FAT表数     offset:16
  ushort RootEntCnt; //FAT32为0           offset:17 根目录最多容纳的目录相数，FAT12/16位512
  ushort TotSec16;   //FAT32为0           offset:19 扇区总数小于32M时使用
  uchar Media;    //存储介质            offset:21 0XF8本地硬盘固定0XF0移动存储介质
  ushort FATSz16;   //FAT32为0         offset:22 每个FAT表的大小扇区数
  ushort SecPerTrk;  //磁道扇区数          offset:24
  ushort NumHeads;   //磁头数            offset:26
  uint HiddSec;    //隐扇区数（从MBR到DBR的扇区数）    offset:28 分区前已使用扇区数
  uint TotSec32;   //该卷总扇区数        offset:32
  uint FATSz32;    //一个FAT表扇区数   offset:36
  ushort ExtFlags;   //FAT32特有           offset:40
  ushort FSVer;      //FAT32特有          offset:42 版本号
  uint RootClus;   //根目录簇号          offset:44
  ushort FSInfo;         //保留扇区FSINFO扇区数offset:48
  ushort BkBootSec;  //通常为6           offset:50
  uchar Reserved[12];  //扩展用             offset:52
  uchar DrvNum;     //                  offset:64
  uchar Reserved1;  //                   offset:65
  uchar BootSig;    //                   offset:66
  uint VolID;      //                   offset:67
  uchar FilSysType[11]; //                offset:71
  uchar FilSysType1[8];//"FAT32    "        offset:82
};

struct FSInfo
{
  uint LeadSig;			//用来标记FSInfo扇区
  uchar Reserved1[480];		//保留用作拓展
  uint StruSig;			//更具体得表明该扇区已被使用
  uint Free_Count;		//保存最新的剩余簇的数量
  uint Nxt_Free;		//启动程序最后分配出的簇号
  uchar Reserved2[12];		//保留用作拓展
  uint TrailSig;		//结束标记表示是一个FSInfo扇区
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
};

#define LAST_FAT_VALUE 0xffffff0f