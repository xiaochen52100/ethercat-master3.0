/*-----------------------------------------------------------------------------
 * AtEmRasSrv.h             file
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Willig, Andreas
 * Description              description of file
 * Date                     2007/5/4::7:18
 *---------------------------------------------------------------------------*/

#ifndef INC_ATEMRASSRV
#define INC_ATEMRASSRV 1

/*-INCLUDES------------------------------------------------------------------*/
#include <EcType.h>
#include <AtEmRasError.h>
#include <EcOs.h>
#include <AtEmRasType.h>
#include <AtEthercat.h>
#include "AtEmRasSrvVersion.h"

/* legacy */
#ifndef EC_API
#define EC_API ATECAT_API
#endif
#ifndef EC_API_FNCALL
#define EC_API_FNCALL
#endif

/*-TYPEDEFS------------------------------------------------------------------*/
#include EC_PACKED_INCLUDESTART(4)
#if (defined INCLUDE_RAS_SPOCSUPPORT)
typedef enum _ATEMRAS_T_ORDINAL
{
    ord_emInitMaster                = 201,  /*  c9 */
    ord_emDeinitMaster              = 202,  /*  ca */
    ord_emStart                     = 203,  /*  cb */
    ord_emStop                      = 204,  /*  cc */
    ord_emIoControl                 = 205,  /*  cd */
    ord_emGetSlaveId                = 207,  /*  cf */
    ord_emMbxTferCreate             = 208,  /*  d0 */
    ord_emMbxTferDelete             = 209,  /*  d1 */
    ord_emCoeSdoDownloadReq         = 210,  /*  d2 */
    ord_emCoeSdoUploadReq           = 211,  /*  d3 */
    ord_emCoeGetODList              = 212,  /*  d4 */
    ord_emCoeGetObjectDesc          = 213,  /*  d5 */
    ord_emCoeGetEntryDesc           = 214,  /*  d6 */
    ord_emGetSlaveProp              = 218,  /*  da */
    ord_emGetSlaveState             = 219,  /*  db */
    ord_emSetSlaveState             = 220,  /*  dc */
    ord_emTferSingleRawCmd          = 221,  /*  dd */
    ord_emGetSlaveIdAtPosition      = 225,  /*  e1 */
    ord_emGetNumConfiguredSlaves    = 226,  /*  e2 */
    ord_emConfigureMaster           = 227,  /*  e3 */
    ord_emSetMasterState            = 228,  /*  e4 */
    ord_emQueueRawCmd               = 229,  /*  e5 */
    ord_emCoeRxPdoTfer              = 230,  /*  e6 */
    ord_emExecJob                   = 231,  /*  e7 */
    ord_emGetProcessData            = 234,  /*  ea */
    ord_emSetProcessData            = 235,  /*  eb */
    ord_emGetMasterState            = 236,  /*  ec */
    ord_emFoeFileUpload             = 237,  /*  ed */
    ord_emFoeFileDownload           = 238,  /*  ee */
    ord_emFoeUpoadReq               = 239,  /*  ef */
    ord_emFoeDownloadReq            = 240,  /*  f0 */
    ord_emCoeSdoDownload            = 241,  /*  f1 */
    ord_emCoeSdoUpload              = 242,  /*  f2 */
    ord_emGetNumConnectedSlaves     = 243,  /*  f3 */
    ord_emResetSlaveController      = 244,  /*  f4 */
    ord_emGetSlaveInfo              = 245,  /*  f5 */
    ord_emIsSlavePresent            = 246,  /*  f6 */
    ord_emAoeWriteReq               = 247,  /*  f7 */
    ord_emAoeReadReq                = 248,  /*  f8 */
    ord_emAoeWrite                  = 249,  /*  f9 */
    ord_emAoeRead                   = 250,  /*  fa */
    ord_emAoeGetSlaveNetId          = 251,  /*  fb */
    ord_emGetFixedAddr              = 252,  /*  fc */
    ord_emGetSlaveProcVarInfoNumOf  = 253,  /*  fd */
    ord_emGetSlaveProcVarInfo       = 254,  /*  fe */
    ord_emFindProcVarByName         = 255,  /*  ff */
    ord_emGetProcessDataBits        = 256,  /* 100 */
    ord_emSetProcessDataBits        = 257,  /* 101 */
    ord_emReloadSlaveEEPRom         = 258,  /* 102 */
    ord_emReadSlaveEEPRom           = 259,  /* 103 */
    ord_emWriteSlaveEEPRom          = 260,  /* 104 */
    ord_emAssignSlaveEEPRom         = 261,  /* 105 */
    ord_emSoeRead                   = 262,  /* 106 */
    ord_emSoeWrite                  = 263,  /* 107 */
    ord_emSoeAbortProcCmd           = 264,  /* 108 */
    ord_emGetNumConnectedSlavesMain = 265,  /* 109 */
    ord_emGetNumConnectedSlavesRed  = 266,  /* 10a */
    ord_emNotifyApp                 = 267,  /* 10b */
    ord_emAoeReadWriteReq           = 268,  /* 10c */
    ord_emAoeReadWrite              = 269,  /* 10d */
    ord_emGetCfgSlaveInfo           = 270,  /* 10e */
    ord_emGetBusSlaveInfo           = 271,  /* 10f */
    ord_emReadSlaveIdentification   = 272,  /* 110 */
    ord_emSetSlaveDisabled          = 273,  /* 111 */
    ord_emSetSlaveDisconnected      = 274,  /* 112 */
    ord_emRescueScan                = 275,  /* 113 */
    ord_emGetMasterInfo             = 276,  /* 114 */
    ord_emConfigExtend              = 277,  /* 115 */
    ord_emAoeWriteControl           = 278,  /* 116 */
    ord_emSetSlavesDisabled         = 279,  /* 117 */
    ord_emSetSlavesDisconnected     = 280,  /* 118 */
	ord_emSetMbxProtocolsSerialize  = 281,  /* 119 */
    /* See also EC-Master API and other branches! */

    /* Borland C++ datatype alignment correction */
    ord_BCppDummy                   = 0xFFFFFFFF
} ATEMRAS_T_ORDINAL;

#define PARAMETER_IGNORE        ((EC_T_DWORD)0xffffffff)


/* Default RAS SPOC access configuration */
#define RASSPOCCFGINITDEFAULT \
{ \
    /* \
      Level required                    Ordinal                           Index                                       Subindex            Reserved \
    */ \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emStart,                    PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emStop,                     PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emConfigureMaster,          PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emSetMasterState,           PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emGetMasterState,           PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emSetSlaveState,            PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emGetSlaveState,            PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    /* Info configured slaves */ \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emGetFixedAddr,             PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emGetSlaveId,               PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emGetNumConnectedSlaves,    PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emGetNumConfiguredSlaves,   PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emIsSlavePresent,           PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emSetSlaveDisabled,         PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emGetSlaveProp,             PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emGetSlaveIdAtPosition,     PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emGetSlaveInfo,             PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emIsSlavePresent,           PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    /* Process data */ \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emGetProcessData,           PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emSetProcessData,           PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emGetProcessDataBits,       PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emSetProcessDataBits,       PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emGetSlaveProcVarInfoNumOf, PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emGetSlaveProcVarInfo,      PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emFindProcVarByName,        PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    /* Mailbox protocols */ \
    { ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emMbxTferCreate,            PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emMbxTferDelete,            PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emCoeSdoDownloadReq,        PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emCoeSdoUploadReq,          PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emCoeGetODList,             PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emCoeGetObjectDesc,         PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emCoeGetEntryDesc,          PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emCoeSdoUpload,             PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emCoeSdoDownload,           PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emFoeFileUpload,            PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emFoeFileDownload,          PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emAoeWriteReq,              PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emAoeReadReq,               PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emAoeWrite,                 PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emAoeRead,                  PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emAoeGetSlaveNetId,         PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emAoeReadWrite,             PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emSoeRead,                  PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emSoeWrite,                 PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emSoeAbortProcCmd,          PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emAoeWriteControl,          PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
	{ ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emSetMbxProtocolsSerialize, PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    /* Misc.*/ \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emResetSlaveController,     PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emReloadSlaveEEPRom,        PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emReadSlaveEEPRom,          PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emWriteSlaveEEPRom,         PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emAssignSlaveEEPRom,        PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emIoControl,                EC_IOCTL_REGISTERCLIENT,                    PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emIoControl,                EC_IOCTL_UNREGISTERCLIENT,                  PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emIoControl,                EC_IOCTL_ISLINK_CONNECTED,                  PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emIoControl,                EC_IOCTL_GET_PDMEMORYSIZE,                  PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emIoControl,                EC_IOCTL_SLAVE_LINKMESSAGES,                PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emIoControl,                EC_IOCTL_DC_SLV_SYNC_STATUS_GET,            PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emIoControl,                EC_IOCTL_SB_RESTART,                        PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emIoControl,                EC_IOCTL_SB_STATUS_GET,                     PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emIoControl,                EC_IOCTL_SB_SET_BUSCNF_VERIFY,              PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emIoControl,                EC_IOCTL_SB_SET_BUSCNF_VERIFY_PROP,         PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emIoControl,                EC_IOCTL_SB_BUSCNF_GETSLAVE_INFO,           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emIoControl,                EC_IOCTL_SB_BUSCNF_GETSLAVE_INFO_EEP,       PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emIoControl,                EC_IOCTL_SB_ENABLE,                         PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emIoControl,                EC_IOCTL_SB_BUSCNF_GETSLAVE_INFO_EX,        PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emIoControl,                EC_IOCTL_SLV_ALIAS_ENABLE,                  PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emIoControl,                EC_IOCTL_SET_SLVSTAT_PERIOD,                PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emIoControl,                EC_IOCTL_FORCE_SLVSTAT_COLLECTION,          PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emIoControl,                EC_IOCTL_GET_SLVSTATISTICS,                 PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_BLOCK_ALL,  ord_emTferSingleRawCmd,         0x00000000,                                 PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emTferSingleRawCmd,         0x00000001,                                 PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emTferSingleRawCmd,         0x00000004,                                 PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emTferSingleRawCmd,         0x00000007,                                 PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emTferSingleRawCmd,         0x0000000A,                                 PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emTferSingleRawCmd,         0x00000002,                                 PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emTferSingleRawCmd,         0x00000003,                                 PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emTferSingleRawCmd,         0x00000005,                                 PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emTferSingleRawCmd,         0x00000006,                                 PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emTferSingleRawCmd,         0x00000008,                                 PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emTferSingleRawCmd,         0x00000009,                                 PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emTferSingleRawCmd,         0x0000000B,                                 PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emTferSingleRawCmd,         0x0000000C,                                 PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emTferSingleRawCmd,         0x0000000D,                                 PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emTferSingleRawCmd,         0x0000000E,                                 PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_BLOCK_ALL,  ord_emQueueRawCmd,              EC_CMD_TYPE_NOP,                            PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emQueueRawCmd,              EC_CMD_TYPE_APRD,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emQueueRawCmd,              EC_CMD_TYPE_FPRD,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emQueueRawCmd,              EC_CMD_TYPE_BRD,                            PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emQueueRawCmd,              EC_CMD_TYPE_LRD,                            PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emQueueRawCmd,              EC_CMD_TYPE_APWR,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emQueueRawCmd,              EC_CMD_TYPE_APRW,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emQueueRawCmd,              EC_CMD_TYPE_FPWR,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emQueueRawCmd,              EC_CMD_TYPE_FPRW,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emQueueRawCmd,              EC_CMD_TYPE_BWR,                            PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emQueueRawCmd,              EC_CMD_TYPE_BRW,                            PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emQueueRawCmd,              EC_CMD_TYPE_LWR,                            PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emQueueRawCmd,              EC_CMD_TYPE_LRW,                            PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emQueueRawCmd,              EC_CMD_TYPE_ARMW,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emQueueRawCmd,              EC_CMD_TYPE_FRMW,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emGetCfgSlaveInfo,          PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READONLY,   ord_emGetBusSlaveInfo,          PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_READWRITE,  ord_emReadSlaveIdentification,  PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emSetSlaveDisconnected,     PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emRescueScan,               PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
    { ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emConfigExtend,             PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
	{ ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emSetSlavesDisabled,        PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }, \
	{ ATEMRAS_ACCESS_LEVEL_ALLOW_ALL,  ord_emSetSlavesDisconnected,    PARAMETER_IGNORE,                           PARAMETER_IGNORE, 0 /* reserved */ }  \
}
#endif /* INCLUDE_RAS_SPOCSUPPORT */

typedef struct _ATEMRAS_T_SPOCCFG
{
    EC_T_DWORD              dwAccessLevel;
    EC_T_DWORD              dwOrdinal;
    EC_T_DWORD              dwIndex;
    EC_T_DWORD              dwSubIndex;
    EC_T_DWORD              dwReserved;
} EC_PACKED(4) ATEMRAS_T_SPOCCFG;

#define ATEMRASSRV_SIGNATURE_PATTERN                    0xEAB00000
#define ATEMRASSRV_SIGNATURE (  ATEMRASSRV_SIGNATURE_PATTERN       \
                             | (ATEMRASSRV_VERS_MAJ         << 16) \
                             | (ATEMRASSRV_VERS_MIN         << 12) \
                             | (ATEMRASSRV_VERS_SERVICEPACK <<  8) \
                             | (ATEMRASSRV_VERS_BUILD       <<  0) \
                             )
typedef struct _ATEMRAS_T_SRVPARMS
{
    EC_T_DWORD          dwSignature;            /**< [in]   Set to ATEMRASSRV_SIGNATURE */
    EC_T_DWORD          dwSize;                 /**< [in]   Set to sizeof(ATEMRAS_T_SRVPARMS) */
    EC_T_LOG_PARMS      LogParms;

    ATEMRAS_T_IPADDR    oAddr;                  /**< [in]   Server Bind IP Address */
    EC_T_WORD           wPort;                  /**< [in]   Server Bind IP Port */
    EC_T_WORD           wReserved;              /**< [in]   Alignment of following members */
    EC_T_DWORD          dwCycleTime;            /**< [in]   Cycle Time of Ras Network access (acceptor, receiver)*/
                                                
    EC_T_DWORD          dwWDTOLimit;            /**< [in]   Amount of cycles without receiving commands (idles) before
                                                  *         Entering state wdexpired  */

    EC_T_DWORD          dwReConTOLimit;         /**< [in]   obsolete */
                                                
    EC_T_DWORD          dwMasterPrio;           /**< [in]   Thread Priority of RAS Master Thread */
    EC_T_DWORD          dwClientPrio;           /**< [in]   Thread Priority of RAS Client Threads */
                                                
    EC_T_DWORD          dwConcNotifyAmount;     /**< [in]   Amount of concurrently queue able Notifications */

    EC_T_DWORD          dwMbxNotifyAmount;      /**< [in]   Amount of concurrent active mailbox transfers */
    EC_T_DWORD          dwMbxUsrNotifySize;     /**< [in]   User definable Notification prolongation */
    
    EC_T_PVOID          pvNotifCtxt;            /**< [in]   Notification context returned while calling pfNotification */
    EC_PF_NOTIFY        pfNotification;         /**< [in]   Function pointer called to notify error and status 
                                                  *         information generated by Remote API Layer */
    EC_T_DWORD          dwCycErrInterval;       /**< [in]   Interval which allows cyclic Notifications */
} EC_PACKED(4) ATEMRAS_T_SRVPARMS, *ATEMRAS_PT_SRVPARMS;
#include EC_PACKED_INCLUDESTOP

/*-FUNCTION DECLARATION------------------------------------------------------*/
EC_API EC_T_DWORD EC_API_FNCALL emRasSrvGetVersion( EC_T_VOID );
EC_API EC_T_DWORD EC_API_FNCALL emRasSrvStart( ATEMRAS_T_SRVPARMS* pParms, EC_T_PVOID* ppHandle);
EC_API EC_T_DWORD EC_API_FNCALL emRasSrvStop( EC_T_PVOID pvHandle, EC_T_DWORD dwTimeout);

#if (defined INCLUDE_RAS_TRACESUPPORT)
EC_API EC_T_DWORD EC_API_FNCALL emRasSrvTraceEnable(EC_T_BOOL bEnable);
#endif

EC_API EC_T_DWORD EC_API_FNCALL emRasSrvSetAccessLevel(EC_T_PVOID pvHandle, EC_T_DWORD dwAccessLevel);
EC_API EC_T_DWORD EC_API_FNCALL emRasSrvGetAccessLevel(EC_T_PVOID pvHandle, EC_T_DWORD* pdwAccessLevel);
EC_API EC_T_DWORD EC_API_FNCALL emRasSrvConfigAccessLevel(EC_T_PVOID pvHandle, ATEMRAS_T_SPOCCFG* poConfigData, EC_T_DWORD dwCnt);
EC_API EC_T_DWORD EC_API_FNCALL emRasSrvSetAccessControl(EC_T_PVOID pvHandle, EC_T_BOOL bActive);
EC_API EC_T_DWORD EC_API_FNCALL emRasSrvSetCallAccessLevel(
    EC_T_PVOID      pvHandle        /**< [in]   Handle to previously started Server */
    , EC_T_DWORD    dwOrdinal       /**< [in]   Function call ID */
    , EC_T_DWORD    dwIndex         /**< [in]   Function call index */
    , EC_T_DWORD    dwSubIndex      /**< [in]   Function call subindex */
    , EC_T_DWORD    dwAccessLevel   /**< [in]   New Access level */
    );
#endif /* INC_ATEMRASSRV */

/*-END OF SOURCE FILE--------------------------------------------------------*/
