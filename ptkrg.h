#ifndef PTKRG_H
#define PTKRG_H

// Внутренние регистры ПОТОК ---------------------------------------------------
// Регистры конфигурации (доступ в режиме A16D08) ------------------------------
//------------------------------------------------------------------------------
#define PTK_BASE_A16                    0x2000

#define A24BASE                         0x0001  // регистр базового адреса области AMK (R/W)
#define AM_MASK                         0x0003  // управление типом доступа к модулю (R/W)
#define LED2                            0x0080
#define LED1                            0x0040
#define LED0                            0x0020
#define NHI_BIG                         0x0010
#define UD_MASK                         0x0008
#define UP_MASK                         0x0004
#define SD_MASK                         0x0002
#define SP_MASK                         0x0001
#define IRQPr                           0x0005  // регистр прерываний (R/W)
#define STATUS_ID_OFFSET                0x0003
#define IRQN_OFFSET                     0x0000
#define Version                         0x0007  // номер версии модуля (R)
//------------------------------------------------------------------------------

// Регистры модуля (доступ в режиме A24D08 и A24D16) ---------------------------
#define CONTROL                         0x0000 // Регистр управления АМК (R/W)
#define POINTER_TABLE_ADDRESS           0x0002 // Адрес расположения таблицы указателей ОУ (R/W)
//------------------------------------------------------------------------------
#define BASIC_STATUS                    0x0004 // Регистр базового ответного слова ОУ (R/W)
#define ADDR_OFFSET                     0x000B
#define MERR                            0x0400
#define INSTR                           0x0200  
#define SREQ                            0x0100
#define BUSY                            0x0008
#define SSF                             0x0004
#define DBCA                            0x0002
#define TF                              0x0001
//------------------------------------------------------------------------------
#define INTERRUPT_MASK                  0x0007 // Маска запрета запросов прерывания (R/W)
#define ALL_INTERRUPT_ON                0x0000
#define ALL_INTERRUPT_OFF               0x00FF
#define VALID_TXRX                      0x0001
#define INVALID_TXRX                    0x0002
#define VALID_MODE_CODE                 0x0004
#define INVALID_MODE_CODE               0x0008
#define FIFO_OVERFLOW                   0x0010
#define VALID_BROADCAST                 0x0020
#define INVALID_BROADCAST               0x0040
#define FAILSAFE_TIMEOUT                0x0080
//------------------------------------------------------------------------------
#define INTERRUPT_VECTOR                0x0006 //Вектор запроса прерывания (R)
#define INTERRUPT_REQUEST               0x0006 //Регистр имитации запросов прерывания (W)
#define AUXILIARY_VECTOR                0x0008 //Доп. Вектор запроса прерывания (R)
#define CONFIGURATION_2                 0x0008 //Регистр конфигурации N2 (W)
#define RTC_HIGH_WORD                   0x000A //Старшее слово RTC (R)
#define RTC_LOW_WORD                    0x000C //Младшее слово RTC (R)
#define RTC_CONTROL                     0x000E //Регистр управления RTC (R/W)
#define READ_FIFO                       0x0010 //Чтение FIFO контроллера прерываний (R)
#define RESET_FIFO                      0x0010 //Сброс FIFO контроллера прерываний (W)
#define CONFIGURATION_1                 0x0012 //Регистр конфигурации N1 (R/W)
#define RG_RT_MODE                      0x0000
#define RG_BC_MODE                      0x0100
#define MT_CONTROL                      0x0014 //Регистр управления монитором АМК (R/W)
#define LAST_COMMAND                    0x0016 //Последнее командное слово ОУ (R)
#define LAST_STATUS                     0x0018 //Последнее ответное слово ОУ (R)
#define FRAME_A_POINTER                 0x001A //Адрес списка сообщений FRAME_A (R/W)
#define FRAME_A_LENGTH                  0x001C //Длина списка сообщений FRAME_A (R/W)
#define END_INT_A                       0x4000 // выдавать прерывание по окончании работы с FRAME_A

#define RESET_TERMINAL                  0x001E //Программный сброс АМК (W)
#define FRAME_B_POINTER                 0x0020 //Адрес списка сообщений FRAME_B (R/W)
#define FRAME_B_LENGTH                  0x0022 //Длина списка сообщений FRAME_B (R/W)
#define ENCODER_STATUS                  0x0024 //Состояние кодера мультиплексного канала (R)
#define CONDITION                       0x0026 //Регистр Состояния АМК (R)
#define BCU_FRAME_GAP                   0x0028 //Пауза между списками сообщений КШ (R/W)
#define CONFIGURATION_3                 0x002A //Регистр конфигурации N3 (R/W)
#define READ_MT_FIFO_WORD               0x002C //Чтение МТ_FIFO (R)
#define ENCODER_DATA                    0x002E //Данные кодера мультиплексного канала (R/W)
#define ENCODER_DATA_TX_REQUEST         0x0030 //Запрос на выдачу кодером слова данных,(с синхроимпульсом слова данных) (W)
#define ENCODER_COMAND_TX_REQUEST       0x0032//Запрос на выдачу кодером командного слова,(с синхроимпульсом команды) (W)
#define MT_FIFO_LW                      0x0034 //Младшие 16 бит слова MT_FIFO (R)
#define MT_FIFO_HW                      0x0036 //Старшие 16 бит слова MT_FIFO (R)
#define MT_STATUS                       0x0038 //Регистр состояния МТ
//------------------------------------------------------------------------------

//---------------- Вспомогательные определения  --------------------------------
#define AMK0_BASE_ADDR                 0x0000 // базовый адрес нулевой микросхемы
#define AMK1_BASE_ADDR                 0x8000 // базовый адрес первой микросхемы

#define AMK_BASE_SIZE                  0x8000 // размер ОЗУ для одной микросхемы

#define AMK_RAM_START_ADDR             0x0080 // адрес буферного ОЗУ микросхемы
#define ONE_BASE_SIZE                  0x0042 // размер ОЗУ для каждого подадреса

#define START_IND_TBL_RECV             0x0001 // индекс таблиц подадресов на прием
#define START_IND_TBL_TR               0x0021 // индекс таблиц подадресов на передачу
#define START_IND_TBL_CMD              0x0040 // индекс таблиц обработки команд управления
#define IND_TBL_RECV_SIZE              0x001E // количество таблиц подадресов на прием
#define IND_TBL_TR_SIZE                0x001E // количество таблиц подадресов на передачу
#define IND_TBL_CMD_SIZE               0x001F // количество таблиц обработки команд управления
#define NUM_INDEX_TBL                  0x0080 // количество индексов таблиц
#define TAG_WORD_ADDR                  0x0000 // адрес служебного слова в пространстве подареса
#define SET_UPDATE_BIT                 0x8000 // установить только бит UPDATE в TAG_WORD
#define CLEAR_TAG_WORD                 0x0000 // сбросить TAG_WORD

#define DATA_WORD_ADDR                 0x0002 // адрес слов данных в пространстве подареса
#define WORD                           0x0002 // размер слова данных
#define BYTE                           0x0001 // размер байта

#define BC_BASES_NUM                   0x000A // количества баз для КК
#define BC_BASE_SIZE                   0x004C // размер одной базы для КК
#define BC_BASE_START_ADDR             0x0400 // стартовый адрес отчета баз КК
#define BC_DATA_OFFSET                 0x0003 // смещение в базе для чтения / записи слов данных
#define ONE_MESSAGE                    0x0001 // Длина FRAME для КК

#define START_BCU                      0x0800 // старт обмена для КК

#endif