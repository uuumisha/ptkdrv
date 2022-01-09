//******************************************************************************
//*         Драйвер модуля "POTOK-3V". ОС QNX 4.25.                            *
//*  Обеспечивает сопряжение по мультиплексному каналу ГОСТ 26765.52-87        *
//******************************************************************************

//******************************************************************************
// INCLUDE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "inc/ptkdrv.h"
#include "inc/ptkrg.h"
//******************************************************************************
//******************************************************************************
// DEFINE
#define VMEBUS2408      "/dev/vmebus/A24/D08"
#define VMEBUS2416      "/dev/vmebus/A24/D16"
#define VMEINT_MILSTD0  "/dev/vmebus/int/milstd1"
#define VMEINT_MILSTD1  "/dev/vmebus/int/milstd2"

#define RT_INT_CMD      0x0001
#define RT_INT_ERR      0x0002
#define RT_INT_DATA     0x0003
#define BC_RES_GOOD     0x0000

#define PATH_LENGTH     0x0030
//******************************************************************************
//******************************************************************************
// TYPEDEF
//******************************************************************************
//******************************************************************************
// GLOBAL VARS
//******************************************************************************
//******************************************************************************
static int fd2408 = -1, fd2416 = -1;
static int mode;

static unsigned short base_subaddr_recv[IND_TBL_RECV_SIZE];
static unsigned short base_subaddr_tr[IND_TBL_TR_SIZE];
static unsigned short base_cmd[IND_TBL_CMD_SIZE];

static unsigned short bc_bases[BC_BASES_NUM];

static unsigned short amkAddr;        // адрес выбранного адаптера
static unsigned short base_subaddr;   // адрес выбранного подадреса
static unsigned short base_bc_addr;   // адрес выбранного базы
static unsigned short dataw;          // переменная для записи слова
static unsigned char  datab;          // переменная для записи байта

//******************************************************************************
// FUNCTIONS
//******************************************************************************

//******************************************************************************
// COMMON FUNCTIONS
//******************************************************************************

//******************************************************************************
// выбор номера ТMK 
int tmkconfig(int tmkNumber, char *tmkPath) {
  char vmePath[PATH_LENGTH];
  if(tmkNumber)
    amkAddr = AMK1_BASE_ADDR; // определение начального адреса для 1-й микросхемы
  else
    amkAddr = AMK0_BASE_ADDR; // определение начального адреса для 0-й микросхемы
  
  sprintf(vmePath, "//%d%s", getnid(), VMEBUS2408);
  if((fd2408 = open(vmePath, O_RDWR)) == -1 ){ 
    printf("Unable to open VMEBUS\n");
    return EXIT_FAILURE; 
  }
  
  sprintf(vmePath, "//%d%s", getnid(), VMEBUS2416);
  if((fd2416 = open(vmePath, O_RDWR)) == -1 ){ 
    printf("Unable to open VMEBUS\n");
    return EXIT_FAILURE; 
  }

  lseek(fd2416, amkAddr + RESET_FIFO, SEEK_SET); // сброс FIFO (запись любого слова)
  write(fd2416, &dataw, WORD);
  return EXIT_SUCCESS;
}
//******************************************************************************
// сообщение proxy устройству обработки прерываний
int tmkdefproxy(pid_t tmkProxy) {
  char vmeIntPath[PATH_LENGTH];
  int fdmilstd;
  
  if(amkAddr == AMK0_BASE_ADDR) {
    sprintf(vmeIntPath, "//%d%s", getnid(), VMEINT_MILSTD0);
    if((fdmilstd = open(vmeIntPath, O_RDWR)) == -1 ) {
      printf("Unable to open VMEINT\n");      
      return EXIT_FAILURE; 
    }
    
    if(dev_arm(fdmilstd, tmkProxy, 0)) {
      printf("VMEINT is full. Can't start program.\n");
      close(fdmilstd);
      return EXIT_FAILURE;
    }
  }
  else {
    sprintf(vmeIntPath, "//%d%s", getnid(), VMEINT_MILSTD1);
    if((fdmilstd = open(vmeIntPath, O_RDWR)) == -1 ) {
      printf("Unable to open VMEINT\n");      
      return EXIT_FAILURE; 
    }
    
    if(dev_arm(fdmilstd, tmkProxy, 0)) {
      printf("VMEINT is full. Can't start program.\n");
      close(fdmilstd);
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}
//******************************************************************************
// установка режима КК/ОУ микросхемы
int tmkdefmode(unsigned short tmkMode) {
  lseek(fd2416, amkAddr + CONFIGURATION_1, SEEK_SET);
  write(fd2416, &tmkMode, WORD);
  mode = tmkMode;
  return EXIT_SUCCESS;
}
//******************************************************************************
// чтение информации о прерывании
int tmkgetpxd(TTmkProxyData *pProxyData) {
  
  if(mode == RT_MODE) { // заполнение структуры прерывания для RT
    lseek(fd2416, amkAddr + LAST_COMMAND, SEEK_SET);
    read(fd2416, &dataw, WORD);
    
    if(((dataw&SUBADDR_MASK) == SUBADDR_MASK) || ((dataw&SUBADDR_MASK) == 0)) {
      pProxyData->nInt = RT_INT_CMD;
      pProxyData->rt.wCmd = dataw & CMD_MASK;
    }
    else {
      pProxyData->nInt = RT_INT_DATA;
      pProxyData->rt.wStatus = dataw;
    }
  }
  else {  // заполнение структуры прерывания для BC
    pProxyData->bc.wResult = BC_RES_GOOD;
  }
  return EXIT_SUCCESS;
}
//******************************************************************************
// заглушка
int tmk_lock() {
  return EXIT_SUCCESS;
}
//******************************************************************************
// заглушка
unsigned short tmkproxymode(unsigned short tmkProxyMode) {
  return EXIT_SUCCESS;
}
//******************************************************************************
// чтение регистра FIFO
unsigned short getfifo() {
  lseek(fd2416, amkAddr + READ_FIFO, SEEK_SET);
  read(fd2416, &dataw,sizeof(dataw)) ;
  return dataw;
}
//******************************************************************************
// сброс FIFO
void rstfifo() {
  lseek(fd2416, amkAddr + READ_FIFO, SEEK_SET);
  write(fd2416, &dataw, sizeof(dataw));
  return;
}

//******************************************************************************
// FUNCTONS FOR RT
//******************************************************************************

//******************************************************************************
// определение подадреса для чтения / записи данных
void rtdefsubaddr(unsigned short rtDir, unsigned short rtSubAddr) {
  if(rtDir == RT_RECEIVE) base_subaddr = base_subaddr_recv[rtSubAddr-1];
  else                    base_subaddr = base_subaddr_tr[rtSubAddr-1];
}
//******************************************************************************
// запись ИС для заданной команды режима управления
void rtputcmddata(unsigned short rtBusCommand, unsigned short rtData) {
  lseek(fd2416, base_cmd[rtBusCommand&NWORDS_MASK]+DATA_WORD_ADDR, SEEK_SET);
  write(fd2416, &rtData, WORD);
}
//******************************************************************************
// прочитать одно слово из выбранного подадреса
unsigned short rtgetw(unsigned short rtAddr) {
  lseek(fd2416, base_subaddr+rtAddr*WORD+WORD, SEEK_SET);
  read(fd2416, &dataw, WORD);
  return dataw;
} 
//******************************************************************************
// прочитать порцию слов из выбранного подадреса
int rtgetblk(unsigned short rtAddr, void *buff, unsigned short length) {
  //dataw = CLEAR_TAG_WORD;
  //lseek(fd2416, base_subaddr + TAG_WORD_ADDR, SEEK_SET);    // сброс признака обновления таблицы данных
  //write(fd2416, &dataw, WORD);
  
  lseek(fd2416, base_subaddr+rtAddr*WORD+WORD, SEEK_SET);
  read(fd2416, buff, length*sizeof(unsigned short));
  
  return EXIT_SUCCESS;
}
//******************************************************************************
// записать одно слово в выбранный подадрес
void rtputw(unsigned short rtAddr, unsigned short rtData) {
  lseek(fd2416, base_subaddr+rtAddr*WORD+WORD, SEEK_SET);
  write(fd2416, &rtData, WORD);
}
//******************************************************************************
// записать порцию слов в выбранный подадрес
int rtputblk(unsigned short rtAddr, void *buff, unsigned short length) {
  //dataw = SET_UPDATE_BIT;
  //lseek(fd2416, base_subaddr + TAG_WORD_ADDR, SEEK_SET);    // установка признака обновления таблицы данных
  //write(fd2416, &dataw, WORD);
  
  lseek(fd2416, base_subaddr+rtAddr*WORD+WORD, SEEK_SET);
  write(fd2416, buff, length * sizeof(unsigned short));
  return EXIT_SUCCESS;
}
//******************************************************************************
// установка адреса ОУ 
int rtdefaddress(unsigned short rtAddress) {
  dataw = (rtAddress<<ADDR_OFFSET);
  lseek(fd2416, amkAddr + BASIC_STATUS, SEEK_SET);
  write(fd2416, &dataw, WORD);
  return EXIT_SUCCESS;
}
//******************************************************************************
// заглушка
unsigned short rtgetirqmode() {
  return EXIT_SUCCESS;
}
//******************************************************************************
// заглушка
int rtansw_lock() {
  return EXIT_SUCCESS;
}
//******************************************************************************
// заглушка
int rtdefpage(unsigned short rtPage) {
  return EXIT_SUCCESS;
}
//******************************************************************************
// заглушка
int rtdefmode(unsigned short rtMode) {
  return EXIT_SUCCESS;
}
//******************************************************************************
// разрешение всех видов прерываний
int rtdefirqmode(unsigned short rtIrqMode) {
  datab = ALL_INTERRUPT_ON;
  lseek(fd2408, amkAddr+INTERRUPT_MASK, SEEK_SET);
  write(fd2408, &datab, BYTE);
  return EXIT_SUCCESS;
}
//******************************************************************************
// инициализация базовых адресов
int rtreset() {
  int i;
  
  // адрес таблицы адресов баз
  dataw = amkAddr + AMK_RAM_START_ADDR;
  lseek(fd2416, amkAddr+POINTER_TABLE_ADDRESS, SEEK_SET);
  write(fd2416, &dataw, WORD);
	
  // инициализация адресов баз приема
  for(i=0; i<IND_TBL_RECV_SIZE; i++) {
    base_subaddr_recv[i] = amkAddr + AMK_RAM_START_ADDR + NUM_INDEX_TBL*WORD + i*ONE_BASE_SIZE;
  }
  lseek(fd2416, amkAddr+AMK_RAM_START_ADDR + START_IND_TBL_RECV*WORD, SEEK_SET);
  write(fd2416, base_subaddr_recv, sizeof(base_subaddr_recv));
  
  
  // инициализация адресов баз передачи
  for(i=0;i<IND_TBL_TR_SIZE;i++) {
    base_subaddr_tr[i] = amkAddr + AMK_RAM_START_ADDR + NUM_INDEX_TBL*WORD + IND_TBL_RECV_SIZE*ONE_BASE_SIZE + i*ONE_BASE_SIZE;
  }
  lseek(fd2416, amkAddr+AMK_RAM_START_ADDR + START_IND_TBL_TR*WORD, SEEK_SET);
  write(fd2416, base_subaddr_tr, sizeof(base_subaddr_tr));
  
  // инициализация адресов обработки команд управления
  for(i=0;i<IND_TBL_CMD_SIZE;i++) {
    base_cmd[i] = amkAddr + AMK_RAM_START_ADDR + NUM_INDEX_TBL*WORD + IND_TBL_RECV_SIZE*ONE_BASE_SIZE + IND_TBL_TR_SIZE*ONE_BASE_SIZE + i*ONE_BASE_SIZE;
  }
  lseek(fd2416, amkAddr+AMK_RAM_START_ADDR+START_IND_TBL_CMD*WORD, SEEK_SET);
  write(fd2416, base_cmd, sizeof(base_cmd));
  
  return EXIT_SUCCESS;
}

//******************************************************************************
// FUNCTONS FOR BC
//******************************************************************************

//******************************************************************************
int bcreset() {
  int i;
  
  // адрес таблицы адресов баз
  dataw = amkAddr + AMK_RAM_START_ADDR;
  lseek(fd2416, amkAddr+POINTER_TABLE_ADDRESS, SEEK_SET);
  write(fd2416, &dataw, WORD);
  
  dataw = ((amkAddr + AMK_RAM_START_ADDR)>>1);
  lseek(fd2416, amkAddr + FRAME_A_POINTER, SEEK_SET); // установка адреса базы
  write(fd2416, &dataw, WORD);
  
  dataw = (END_INT_A | ONE_MESSAGE);
  lseek(fd2416, amkAddr + FRAME_A_LENGTH, SEEK_SET); // установка выдачи прерывания и количества сообщений во FRAME
  write(fd2416, &dataw, WORD);
  
  // инициализация адресов баз КК
  for(i=0; i<BC_BASES_NUM; i++)
    bc_bases[i] = amkAddr + BC_BASE_START_ADDR + i*BC_BASE_SIZE;
  
  return EXIT_SUCCESS;
}
//******************************************************************************
// старт обмена для КК
int bcstart(unsigned short bcBaseBus, unsigned short bcCtrlCode) {
  dataw = BC_MODE | START_BCU;
  lseek(fd2416, amkAddr + CONFIGURATION_1, SEEK_SET);
  write(fd2416, &dataw, WORD);
  return EXIT_SUCCESS;
}
//******************************************************************************
// установка базы контроллера канала
int bcdefbase(unsigned short bcBasePC) {
  base_bc_addr = bc_bases[bcBasePC];
  dataw = (base_bc_addr>>1);
  lseek(fd2416, amkAddr + AMK_RAM_START_ADDR, SEEK_SET);
  write(fd2416, &dataw, WORD);
  return EXIT_SUCCESS;
}
//******************************************************************************
// разрешение всех видов прерываний
int bcdefirqmode(unsigned short bcIrqMode) {
  datab = ALL_INTERRUPT_ON;
  lseek(fd2408, amkAddr+INTERRUPT_MASK, SEEK_SET);
  write(fd2408, &datab, BYTE);
  return EXIT_SUCCESS;
}
//******************************************************************************
// записать слово в базу КК
void bcputw(unsigned short bcAddr, unsigned short bcData) {
  if(bcAddr)
    bcAddr+=BC_DATA_OFFSET;
  lseek(fd2416, base_bc_addr + bcAddr*WORD + WORD, SEEK_SET);
  write(fd2416, &bcData, WORD);
}
//******************************************************************************
// записать порцию слов в базу КК
int bcputblk(unsigned short bcAddr, void *pcBuffer, unsigned short cwLength) {
  if(bcAddr)
    bcAddr+=BC_DATA_OFFSET;
  lseek(fd2416, base_bc_addr + bcAddr*WORD + WORD, SEEK_SET);
  write(fd2416, pcBuffer, cwLength*WORD);
  return EXIT_SUCCESS;
}
//******************************************************************************
// считать слово из базы КК
unsigned short bcgetw(unsigned short bcAddr) {
  if(bcAddr)
    bcAddr+=BC_DATA_OFFSET;
  lseek(fd2416, base_bc_addr + bcAddr*WORD + WORD, SEEK_SET);
  read(fd2416, &dataw, WORD);
  return dataw;
}
//******************************************************************************
// считать порцию слов из базы КК
int bcgetblk(unsigned short bcAddr, void *pcBuffer, unsigned short cwLength) {
  if(bcAddr)
    bcAddr+=BC_DATA_OFFSET;
  lseek(fd2416, base_bc_addr + bcAddr*WORD + WORD, SEEK_SET);
  read(fd2416, &pcBuffer, cwLength*WORD);
  return EXIT_SUCCESS;
}

//******************************************************************************
// END OF FILE
//******************************************************************************