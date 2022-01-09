#ifndef PTKDRV_H
#define PTKDRV_H

#define NWORDS_MASK                             0x001F
#define CMD_MASK                                0x041F
#define SUBADDR_MASK                            0x03E0
#define CI_MASK                                 0x03E0
#define HBIT_MASK                               0x0200
#define RT_DIR_MASK                             0x0400
#define ADDRESS_MASK                            0xF800
#define RTFL_MASK                               0x0001
#define DNBA_MASK                               0x0002
#define SSFL_MASK                               0x0004
#define BUSY_MASK                               0x0008
#define BRCST_MASK                              0x0010
#define NULL_MASK                               0x00E0
#define SREQ_MASK                               0x0100
#define ERROR_MASK                              0x0400

#define DATA_BC_RT                              0x0000
#define DATA_BC_RT_BRCST                        0x0008
#define DATA_RT_BC                              0x0001
#define DATA_RT_RT                              0x0002
#define DATA_RT_RT_BRCST                        0x000A
#define CTRL_C_A                                0x0003
#define CTRL_C_BRCST                            0x000B
#define CTRL_CD_A                               0x0004
#define CTRL_CD_BRCST                           0x000C
#define CTRL_C_AD                               0x0005
                          
#define BC_MODE                                 0x0100
#define RT_MODE                                 0x0000
                          
#define	RT_RECEIVE                              0x0000
#define RT_TRANSMIT                             0x0400

#define CMD_DYNAMIC_BUS_CONTROL                 0x0400
#define CMD_SYNCHRONIZE                         0x0401
#define CMD_TRANSMIT_STATUS_WORD                0x0402
#define CMD_INITIATE_SELF_TEST                  0x0403
#define CMD_TRANSMITTER_SHUTDOWN                0x0404
#define CMD_OVERRIDE_TRANSMITTER_SHUTDOWN       0x0405
#define CMD_INHIBIT_TERMINAL_FLAG_BIT           0x0406
#define CMD_OVERRIDE_INHIBIT_TERMINAL_FLAG_BIT  0x0407
#define CMD_RESET_REMOTE_TERMINAL               0x0408
#define CMD_TRANSMIT_VECTOR_WORD                0x0410
#define CMD_SYNCHRONIZE_WITH_DATA_WORD          0x0011
#define CMD_TRANSMIT_LAST_COMMAND_WORD          0x0412
#define CMD_TRANSMIT_BUILT_IN_TEST_WORD         0x0413

#define TMK_PROXY_BUFFERED                      0x0001
#define RT_DATA_BL                              0x2000

#define CW(ADDR,DIR,SUBADDR,NWORDS) ((unsigned short)(((ADDR)<<11)|(DIR)|((SUBADDR)<<5)|((NWORDS)&0x1F)))
#define CWM(ADDR,COMMAND) ((unsigned short)(((ADDR)<<11)|(CI_MASK)|(COMMAND)))

typedef struct
{
  short int nInt;
  unsigned short wMode;
  union
  {
    struct
    {
      unsigned short wResult;
      unsigned short wAW1;
      unsigned short wAW2;
    } bc;
    struct
    {
      unsigned short wBase;
      unsigned short wResultX;
    } bcx;
    struct
    {
      unsigned short wStatus;
      unsigned short wCmd;
    } rt;
    struct
    {
      unsigned short wBase;
      unsigned short wResultX;
    } mt;
  };
} TTmkProxyData;  

extern int tmkconfig(int tmkNumber, char *tmkPath);
extern int tmkdefproxy(pid_t tmkProxy);
extern int tmk_lock();
extern int tmkdefmode(unsigned short tmkMode);
extern unsigned short tmkproxymode(unsigned short tmkProxyMode);
extern int tmkgetpxd(TTmkProxyData *pProxyData);

extern void rtdefsubaddr(unsigned short rtDir, unsigned short rtSubAddr);
extern void rtputcmddata(unsigned short rtBusCommand, unsigned short rtData);
extern unsigned short rtgetw(unsigned short rtAddr);
extern int rtgetblk(unsigned short rtAddr, void *buff, unsigned short length);
extern void rtputw(unsigned short rtAddr, unsigned short rtData);
extern int rtputblk(unsigned short rtAddr, void *buff, unsigned short length);
extern int rtdefaddress(unsigned short rtAddress);
extern int rtreset();
extern int rtansw_lock();
extern int rtdefpage(unsigned short rtPage);
extern int rtdefmode(unsigned short rtMode);
extern int rtdefirqmode(unsigned short rtIrqMode);
extern unsigned short rtgetirqmode();

extern int bcreset();
extern int bcdefirqmode(unsigned short bcIrqMode);
extern int bcdefbase(unsigned short bcBasePC);
extern void bcputw(unsigned short bcAddr, unsigned short bcData);
extern int bcstart(unsigned short bcBaseBus, unsigned short bcCtrlCode);
extern unsigned short bcgetw(unsigned short bcAddr);
extern int bcgetblk(unsigned short bcAddr, void *pcBuffer, unsigned short cwLength);
extern int bcputblk(unsigned short bcAddr, void *pcBuffer, unsigned short cwLength);

extern unsigned short getfifo() ;
extern void rstfifo() ;

#endif
