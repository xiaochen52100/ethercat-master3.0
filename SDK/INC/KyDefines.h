#ifndef KYDEFINES_H
#define KYDEFINES_H

#include <stdint.h>

#define DEBUG_P

#ifdef DEBUG_P
#define MyDebug(format,...)  printf("File: "__FILE__", Line: %05d: "format"", __LINE__, ##__VA_ARGS__)
#else
#define MyDebug(format,...)
#endif

#define    FAIL      (-1)
#define    SUCCESS   (0)

#define    MAX_FILE_PATH_LEN    (1024)
#define    MAX_PATH_LEN         (MAX_FILE_PATH_LEN)

typedef signed char Int8_t;
typedef unsigned char Uint8_t;

typedef signed short Int16_t;
typedef unsigned short Uint16_t;

typedef signed int Int32_t;
typedef unsigned int Uint32_t;

typedef const char string;

typedef enum{
    Ky_NULL = 0x00,
    //------------etherCat主站-------------//
    Ky_etherCat_Cmd = 0x10,
    Ky_etherCat_Data,
    Ky_etherCat_State,
    //-------------Io设备------------//
    Ky_ioDevice_Cmd = 0x20,
    Ky_ioDevice_DataIn,
    Ky_ioDevice_DataOut,
    Ky_ioDevice_state,
    //-------------激光传感------------//
    Ky_laserSensor_Cmd = 0x30,
    Ky_laserSensor_Data,
    Ky_laserSensor_State,
    //-------------电弧传感------------//
    Ky_arcSensor_Cmd = 0x40,
    Ky_arcSensor_Data,
    Ky_arcSensor_State,
    //-------------焊机控制器------------//
    Ky_weld_Cmd = 0x50,
    Ky_weld_Data,
    Ky_weld_State
    //-------------------------//
}Ky_deviceMemId_t;

#endif
