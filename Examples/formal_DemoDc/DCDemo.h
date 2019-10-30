/*-----------------------------------------------------------------------------
 * DCDemo.h
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              EtherCAT Master demo header
 *---------------------------------------------------------------------------*/

/*-INCLUDES------------------------------------------------------------------*/
#include "AtEthercat.h"
#include "DCDemoConfig.h"
#include "ecatDemoCommon.h"

#ifdef VXWORKS
#include "wvLib.h"
#endif

/*-DEFINES-------------------------------------------------------------------*/
#define MAX_LINKLAYER 5

/* the RAS server is necessary to support the EC-Engineer or other remote applications */
#if (!defined ATEMRAS_SERVER) && (defined EC_SOCKET_SUPPORTED)
#define ATEMRAS_SERVER
#endif

#define REMOTE_WD_TO_LIMIT          10000
#define REMOTE_CYCLE_TIME           2

/*-FUNCTION DECLARATIONS-----------------------------------------------------*/
EC_T_DWORD DCDemo(
     EC_T_CNF_TYPE      eCnfType
    ,EC_T_PBYTE         pbyCnfData
    ,EC_T_DWORD         dwCnfDataLen
    ,EC_T_DWORD         dwBusCycleTimeUsec
    ,EC_T_INT           nVerbose
    ,EC_T_DWORD         dwDuration
    ,EC_T_LINK_PARMS*   poLinkParms
    ,EC_T_VOID*         pvTimingEvent
    ,EC_T_DWORD         dwCpuIndex
    ,EC_T_BOOL          bEnaPerfJobs
    ,EC_T_INT            nFlashAddress
#ifdef ATEMRAS_SERVER
    ,EC_T_WORD          wServerPort
#endif
    ,EC_T_LINK_PARMS*   poLinkParmsRed
    ,EC_T_DCM_MODE      eDcmMode
    ,EC_T_BOOL          bCtlOff
    ,EC_T_OS_PARMS*     poOsParms
    );

/*--------------------------------------------------------------------------*/
/* Performance measurements of jobs                                         */
/* This is only available on CPUs with TSC support                          */
/*--------------------------------------------------------------------------*/
#define JOB_ProcessAllRxFrames  0
#define JOB_SendAllCycFrames    1
#define JOB_MasterTimer         2
#define JOB_SendAcycFrames      3
#define PERF_CycleTime          4
#define PERF_myAppWorkpd        5
#define PERF_DCM_Logfile        6
#define MAX_JOB_NUM             7

#define PERF_MEASURE_JOBS_INIT(msgcb)   ecatPerfMeasInit(&pDemoThreadParam->TscMeasDesc,0,MAX_JOB_NUM,msgcb);ecatPerfMeasEnable(&pDemoThreadParam->TscMeasDesc)
#define PERF_MEASURE_JOBS_DEINIT()      ecatPerfMeasDeinit(&pDemoThreadParam->TscMeasDesc)
#define PERF_MEASURE_JOBS_SHOW()        ecatPerfMeasShow(&pDemoThreadParam->TscMeasDesc,0xFFFFFFFF,S_aszMeasInfo)
#define PERF_JOB_START(nJobIndex)       ecatPerfMeasStart(&pDemoThreadParam->TscMeasDesc,(EC_T_DWORD)(nJobIndex))
#define PERF_JOB_END(nJobIndex)         ecatPerfMeasEnd(&pDemoThreadParam->TscMeasDesc,(EC_T_DWORD)(nJobIndex))

/*-END OF SOURCE FILE--------------------------------------------------------*/
