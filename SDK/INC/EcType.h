/*-----------------------------------------------------------------------------
 * EcType.h
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              EtherCAT Master type definitions
 *---------------------------------------------------------------------------*/

#ifndef INC_ECTYPE
#define INC_ECTYPE

/*-TYPEDEFS------------------------------------------------------------------*/
#define EC_T_VOID void
typedef void*           EC_T_PVOID;

typedef int             EC_T_BOOL;

typedef char            EC_T_CHAR;
typedef unsigned short  EC_T_WCHAR;

typedef unsigned char   EC_T_BYTE, *EC_T_PBYTE;
typedef unsigned short  EC_T_WORD;
typedef unsigned int    EC_T_DWORD;

typedef signed char     EC_T_SBYTE;
typedef signed short    EC_T_SWORD;
typedef signed int      EC_T_SDWORD;

typedef int             EC_T_INT;
typedef unsigned int    EC_T_UINT;

typedef short           EC_T_SHORT;
typedef unsigned short  EC_T_USHORT;

typedef double          EC_T_REAL;

typedef void*           EC_T_ADDRESS;

typedef void*           EC_T_HANDLE;

/* type of lock */
typedef enum
{
    eLockType_DEFAULT= 1,                           /*< Default mutex           */
    eLockType_SPIN,                                 /*< only jobs --> spin lock */
    eLockType_INTERFACE,                            /*< interface and jobs      */

    /* Borland C++ datatype alignment correction */
    eLockType_BCppDummy   = 0xFFFFFFFF
} EC_T_OS_LOCK_TYPE;


/*/////////////////////////////////////////////////////////////////////////////////////////
//
// Standard Types
*/
#define DEFTYPE_NULL              ((EC_T_WORD)0x0000)
#define DEFTYPE_BOOLEAN           ((EC_T_WORD)0x0001)       /* bit size: 1 */
#define DEFTYPE_INTEGER8          ((EC_T_WORD)0x0002)       /* bit size: 8 */
#define DEFTYPE_INTEGER16         ((EC_T_WORD)0x0003)       /* bit size: 16 */
#define DEFTYPE_INTEGER32         ((EC_T_WORD)0x0004)       /* bit size: 32 */
#define DEFTYPE_UNSIGNED8         ((EC_T_WORD)0x0005)       /* bit size: 8 */
#define DEFTYPE_UNSIGNED16        ((EC_T_WORD)0x0006)       /* bit size: 16 */
#define DEFTYPE_UNSIGNED32        ((EC_T_WORD)0x0007)       /* bit size: 32 */
#define DEFTYPE_REAL32            ((EC_T_WORD)0x0008)       /* bit size: 32 */
#define DEFTYPE_VISIBLESTRING     ((EC_T_WORD)0x0009)       /* bit size: 8*n */
#define DEFTYPE_OCTETSTRING       ((EC_T_WORD)0x000A)       /* bit size: 8*(n+1) */
#define DEFTYPE_UNICODESTRING     ((EC_T_WORD)0x000B)       /* bit size: 16*(n+1) */
#define DEFTYPE_TIMEOFDAY         ((EC_T_WORD)0x000C)
#define DEFTYPE_TIMEDIFFERENCE    ((EC_T_WORD)0x000D)
#define DEFTYPE_DOMAIN            ((EC_T_WORD)0x000F)
#define DEFTYPE_INTEGER24         ((EC_T_WORD)0x0010)       /* bit size: 24 */
#define DEFTYPE_REAL64            ((EC_T_WORD)0x0011)       /* bit size: 64 */
#define DEFTYPE_INTEGER40         ((EC_T_WORD)0x0012)       /* bit size: 40 */
#define DEFTYPE_INTEGER48         ((EC_T_WORD)0x0013)       /* bit size: 48 */
#define DEFTYPE_INTEGER56         ((EC_T_WORD)0x0014)       /* bit size: 56 */
#define DEFTYPE_INTEGER64         ((EC_T_WORD)0x0015)       /* bit size: 64 */
#define DEFTYPE_UNSIGNED24        ((EC_T_WORD)0x0016)       /* bit size: 24 */
#define DEFTYPE_UNSIGNED40        ((EC_T_WORD)0x0018)       /* bit size: 40 */
#define DEFTYPE_UNSIGNED48        ((EC_T_WORD)0x0019)       /* bit size: 48 */
#define DEFTYPE_UNSIGNED56        ((EC_T_WORD)0x001A)       /* bit size: 56 */
#define DEFTYPE_UNSIGNED64        ((EC_T_WORD)0x001B)       /* bit size: 64 */

#define DEFTYPE_GUID              ((EC_T_WORD)0x001D)       /* bit size: 128 */
#define DEFTYPE_BYTE              ((EC_T_WORD)0x001E)       /* bit size: 8 */
#define DEFTYPE_WORD              ((EC_T_WORD)0x001F)       /* bit size: 16 */
#define DEFTYPE_DWORD             ((EC_T_WORD)0x0020)       /* bit size: 32 */
#define DEFTYPE_PDOMAPPING        ((EC_T_WORD)0x0021)

#define DEFTYPE_IDENTITY          ((EC_T_WORD)0x0023)

#define DEFTYPE_COMMAND           ((EC_T_WORD)0x0025)
#define DEFTYPE_PDOCOMPAR         ((EC_T_WORD)0x0027)
#define DEFTYPE_ENUM              ((EC_T_WORD)0x0028)
#define DEFTYPE_SMPAR             ((EC_T_WORD)0x0029)
#define DEFTYPE_RECORD            ((EC_T_WORD)0x002A)
#define DEFTYPE_BACKUP_PARAMETER  ((EC_T_WORD)0x002B)
#define DEFTYPE_MODULAR_DEVICE_PROFILE ((EC_T_WORD)0x002C)
#define DEFTYPE_BITARR8           ((EC_T_WORD)0x002D)       /* bit size: 8 */
#define DEFTYPE_BITARR16          ((EC_T_WORD)0x002E)       /* bit size: 16 */
#define DEFTYPE_BITARR32          ((EC_T_WORD)0x002F)       /* bit size: 32 */
#define DEFTYPE_BIT1              ((EC_T_WORD)0x0030)       /* bit size: 1 */
#define DEFTYPE_BIT2              ((EC_T_WORD)0x0031)       /* bit size: 1 */
#define DEFTYPE_BIT3              ((EC_T_WORD)0x0032)       /* bit size: 1 */
#define DEFTYPE_BIT4              ((EC_T_WORD)0x0033)       /* bit size: 1 */
#define DEFTYPE_BIT5              ((EC_T_WORD)0x0034)       /* bit size: 1 */
#define DEFTYPE_BIT6              ((EC_T_WORD)0x0035)       /* bit size: 1 */
#define DEFTYPE_BIT7              ((EC_T_WORD)0x0036)       /* bit size: 1 */
#define DEFTYPE_BIT8              ((EC_T_WORD)0x0037)       /* bit size: 1 */

/* 0x40-0x5F Manufacturer Specific Complex Data Type */

#define DEFTYPE_ARRAY_OF_BYTE     ((EC_T_WORD)0x000A)       /* bit size:  8 * (n + 1) */
#define DEFTYPE_ARRAY_OF_UINT     ((EC_T_WORD)0x000B)       /* bit size: 16 * (n + 1) */
#define DEFTYPE_ARRAY_OF_INT      ((EC_T_WORD)0x0260)       /* bit size: 16 * (n + 1) */
#define DEFTYPE_ARRAY_OF_SINT     ((EC_T_WORD)0x0261)       /* bit size:  8 * (n + 1) */
#define DEFTYPE_ARRAY_OF_DINT     ((EC_T_WORD)0x0262)       /* bit size: 32 * (n + 1) */
#define DEFTYPE_ARRAY_OF_UDINT    ((EC_T_WORD)0x0263)       /* bit size: 32 * (n + 1) */

#define DEFTYPE_ERROR_SETTING     ((EC_T_WORD)0x0281)       /* bit size: - */
#define DEFTYPE_HISTORY           ((EC_T_WORD)0x0282)       /* bit size: - */
#define DEFTYPE_DIAGNOSIS_OBJECT  ((EC_T_WORD)0x0282)       /* bit size: - */
#define DEFTYPE_EXTERNAL_SYNC_STATUS ((EC_T_WORD)0x0283)    /* bit size: - */
#define DEFTYPE_EXTERNAL_SYNC_SETTINGS ((EC_T_WORD)0x0284)  /* bit size: - */
#define DEFTYPE_FSOEFRAME         ((EC_T_WORD)0x0285)       /* bit size: - */
#define DEFTYPE_FSOECOMMPAR       ((EC_T_WORD)0x0286)       /* bit size: - */


/*-MACROS--------------------------------------------------------------------*/
#define EC_FALSE            0
#define EC_TRUE             1
#define EC_NULL             0

#define EC_NOWAIT           ((EC_T_DWORD)0x00000000)
#define EC_WAITINFINITE     ((EC_T_DWORD)0xFFFFFFFF)


/*-MASTER FEATURES-----------------------------------------------------------*/

/* object dictionary */
#define EC_MASTER_DEVICE_NAME       "EC-Master"         /* Index: 0x1008 */
#define EC_MASTER_HW_VERSION        "V xx.xx.xx.xx"     /* Index: 0x1009 */
#define EC_MASTER_SW_VERSION        "V xx.xx.xx.xx"     /* Index: 0x100A */
#define EC_MASTER_VENDOR_ID         0x00004154          /* Index: 0x1018 Subindex 1 */
#define EC_MASTER_PRODUCT_CODE      0x6d657461          /* Index: 0x1018 Subindex 2 */

#define EC_ARCH_UNDEFINED 0
#define EC_ARCH_X86       1
#define EC_ARCH_X64       2
#define EC_ARCH_PPC       3
#define EC_ARCH_ARM       4

#endif /* INC_ECTYPE */

/*-END OF SOURCE FILE--------------------------------------------------------*/
