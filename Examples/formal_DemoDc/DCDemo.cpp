/*-----------------------------------------------------------------------------
 * DCDemo.cpp
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              EC-Master demo application
 *---------------------------------------------------------------------------*/

/*-INCLUDES------------------------------------------------------------------*/
#include "DCDemo.h"
#include "motrotech.h"    //wzz

#ifndef INC_ATETHERCAT
#include "AtEthercat.h"
#endif

#ifdef ATEMRAS_SERVER
#include "AtEmRasSrv.h"
#endif

/*-Logging-------------------------------------------------------------------*/
#define DCM_ENABLE_LOGFILE

/*-LOCAL VARIABLES-----------------------------------------------------------*/
static EC_T_REGISTERRESULTS S_oRegisterResults;
static T_DEMO_THREAD_PARAM  S_DemoThreadParam;
static EC_T_PVOID           S_pvtJobThread    = EC_NULL;
#ifdef ATEMRAS_SERVER
static EC_T_BOOL            S_bRasSrvStarted  = EC_FALSE;
static EC_T_PVOID           S_pvRemoteApiSrvH = EC_NULL;
#endif
static EC_T_DCM_MODE        S_eDcmMode        = eDcmMode_Off;
static EC_T_BOOL            S_bCtlOff         = EC_FALSE;
static EC_T_CHAR*           S_aszMeasInfo[MAX_JOB_NUM] =
{
    (EC_T_CHAR*)"JOB_ProcessAllRxFrames",
    (EC_T_CHAR*)"JOB_SendAllCycFrames  ",
    (EC_T_CHAR*)"JOB_MasterTimer       ",
    (EC_T_CHAR*)"JOB_SendAcycFrames    ",
    (EC_T_CHAR*)"Cycle Time            ",
    (EC_T_CHAR*)"myAppWorkPd           ",
    (EC_T_CHAR*)"Write DCM logfile     "
};


/*-FUNCTION DECLARATIONS-----------------------------------------------------*/
static EC_T_DWORD ecatNotifyCallback(EC_T_DWORD dwCode, EC_T_NOTIFYPARMS* pParms);
#if (defined ATEMRAS_SERVER)
static EC_T_DWORD RasNotifyWrapper(EC_T_DWORD dwCode, EC_T_NOTIFYPARMS* pParms);
#endif
static EC_T_VOID  tEcJobTask(EC_T_VOID* pvThreadParamDesc);

/*-MYAPP---------------------------------------------------------------------*/
/* Demo code: Remove/change this in your application */
static EC_T_DWORD myAppInit     (T_DEMO_THREAD_PARAM* pDemoThreadParam);
static EC_T_DWORD myAppPrepare  (T_DEMO_THREAD_PARAM* pDemoThreadParam);
static EC_T_DWORD myAppSetup    (T_DEMO_THREAD_PARAM* pDemoThreadParam);
static EC_T_DWORD myAppWorkpd   (T_DEMO_THREAD_PARAM* pDemoThreadParam, EC_T_BYTE* pbyPDIn, EC_T_BYTE* pbyPDOut);
static EC_T_DWORD myAppDiagnosis(T_DEMO_THREAD_PARAM* pDemoThreadParam);
static EC_T_DWORD myAppNotify   (EC_T_DWORD dwCode, EC_T_NOTIFYPARMS* pParms);
/* Demo code: End */

/*-FUNCTION DEFINITIONS------------------------------------------------------*/

/********************************************************************************/
/** \brief EC-Master demo application.
*
* This is an EC-Master demo application.
*
* \return  Status value.
*/
EC_T_DWORD DCDemo(
    EC_T_CNF_TYPE       eCnfType            /* [in]  Enum type of configuration data provided */
   ,EC_T_PBYTE          pbyCnfData          /* [in]  Configuration data */
   ,EC_T_DWORD          dwCnfDataLen        /* [in]  Length of configuration data in byte */
   ,EC_T_DWORD          dwBusCycleTimeUsec  /* [in]  bus cycle time in usec */
   ,EC_T_INT            nVerbose            /* [in]  verbosity level */
   ,EC_T_DWORD          dwDuration          /* [in]  test duration in msec (0 = forever) */
   ,EC_T_LINK_PARMS*    poLinkParms         /* [in]  pointer to link parameter */
   ,EC_T_VOID*          pvTimingEvent       /* [in]  Timing event handle */
   ,EC_T_DWORD          dwCpuIndex          /* [in]  SMP only: CPU index */
   ,EC_T_BOOL           bEnaPerfJobs        /* [in]  Performance measurement */
   ,EC_T_INT            nFlashAddress       /* [in]  Flash: -1= no,  0=all outputs, >0=slave address *///slave fixed address wzz2019/6/6
#ifdef ATEMRAS_SERVER
   ,EC_T_WORD           wServerPort         /* [in]   Remote API Server Port */
#endif
   ,EC_T_LINK_PARMS*    poLinkParmsRed      /* [in]  Redundancy Link Layer Parameter */
   ,EC_T_DCM_MODE       eDcmMode            /* [in]  DCM mode */
   ,EC_T_BOOL           bCtlOff             /* [in]  Disables / Enables the control loop for diagnosis */
   ,EC_T_OS_PARMS*      poOsParms           /* [in]  pointer to OS parameters  */
)
{
    EC_T_DWORD           dwRetVal         = EC_E_NOERROR;
    EC_T_DWORD           dwRes            = EC_E_NOERROR;
    EC_T_BOOL            bRes             = EC_FALSE;
    T_DEMO_THREAD_PARAM* pDemoThreadParam = &S_DemoThreadParam;
    CEcTimer             oDcmStatusTimer;
    EC_T_BOOL            bFirstDcmStatus  = EC_TRUE;
    EC_T_CPUSET CpuSet;
    EC_CPUSET_ZERO(CpuSet);
    EC_CPUSET_SET(CpuSet, dwCpuIndex);

    /* store parameters */
    OsMemset(pDemoThreadParam, 0, sizeof(T_DEMO_THREAD_PARAM));
    pDemoThreadParam->pvTimingEvent = pvTimingEvent;
    OsMemcpy(&pDemoThreadParam->LogParms, G_pEcLogParms, sizeof(EC_T_LOG_PARMS));
    pDemoThreadParam->nVerbose = nVerbose;
    pDemoThreadParam->dwCpuIndex = dwCpuIndex;
    pDemoThreadParam->dwMasterID = INSTANCE_MASTER_DEFAULT;
    pDemoThreadParam->dwBusCycleTimeUsec = dwBusCycleTimeUsec;
    pDemoThreadParam->FlashData.nFlashAddress = nFlashAddress;

    S_eDcmMode = eDcmMode;
    S_bCtlOff = bCtlOff;

    /* check if interrupt mode is selected */
    if (poLinkParms->eLinkMode != EcLinkMode_POLLING)
    {
        dwRetVal = EC_E_INVALIDPARM;
        EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Error: Link layer in 'interrupt' mode is not supported by EcMasterDemo. Please select 'polling' mode.\n"));
        goto Exit;
    }
    /* set thread affinity */
    {
        bRes = OsSetThreadAffinity(EC_NULL, CpuSet);
        if (!bRes)
        {
            dwRetVal = EC_E_INVALIDPARM;
            EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Error: Set thread affinitiy, invalid CPU index %d\n", dwCpuIndex));
            goto Exit;
        }
    }
    /* create notification context */
    pDemoThreadParam->pNotInst = EC_NEW(CEmNotification(pDemoThreadParam->dwMasterID));
    if (EC_NULL == pDemoThreadParam->pNotInst)
    {
        dwRetVal = EC_E_NOMEMORY;
        goto Exit;
    }
    if (bEnaPerfJobs)
    {
        PERF_MEASURE_JOBS_INIT(EC_NULL);
        EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "OsMeasGet100kHzFrequency(): %d MHz\n", OsMeasGet100kHzFrequency() / 10));
    }
    /*****************************************************************************/
    /* Demo code: Remove/change this in your application: Initialize application */
    /*****************************************************************************/
    dwRes = myAppInit(pDemoThreadParam);
    if (EC_E_NOERROR != dwRes)
    {
        EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, (EC_T_CHAR*)"myAppInit failed: %s (0x%lx))\n", ecatGetText(dwRes), dwRes));
        dwRetVal = dwRes;
        goto Exit;
    }
#ifdef ATEMRAS_SERVER
    /* start RAS server if enabled */
    if (0xFFFF != wServerPort)
    {
        ATEMRAS_T_SRVPARMS oRemoteApiConfig;

        OsMemset(&oRemoteApiConfig, 0, sizeof(ATEMRAS_T_SRVPARMS));
        oRemoteApiConfig.dwSignature        = ATEMRASSRV_SIGNATURE;
        oRemoteApiConfig.dwSize             = sizeof(ATEMRAS_T_SRVPARMS);
        oRemoteApiConfig.oAddr.dwAddr       = 0;                    /* INADDR_ANY */
        oRemoteApiConfig.wPort              = wServerPort;
        oRemoteApiConfig.dwCycleTime        = REMOTE_CYCLE_TIME;    /* 2 msec */
        oRemoteApiConfig.dwWDTOLimit        = (REMOTE_WD_TO_LIMIT/REMOTE_CYCLE_TIME); /* WD Timeout after 10secs */
        oRemoteApiConfig.dwReConTOLimit     = 6000;                 /* Reconnect Timeout after 6000*2msec + 10secs */
#if (defined LINUX) || (defined XENOMAI)
        oRemoteApiConfig.dwMasterPrio       = (CpuSet << 16) | MAIN_THREAD_PRIO;
        oRemoteApiConfig.dwClientPrio       = (CpuSet << 16) | MAIN_THREAD_PRIO;
#else
        oRemoteApiConfig.dwMasterPrio       = MAIN_THREAD_PRIO;
        oRemoteApiConfig.dwClientPrio       = MAIN_THREAD_PRIO;
#endif
        oRemoteApiConfig.pvNotifCtxt        = pDemoThreadParam->pNotInst;        /* Notification context */
        oRemoteApiConfig.pfNotification     = RasNotifyWrapper;     /* Notification function for emras Layer */
        oRemoteApiConfig.dwConcNotifyAmount = 100;                  /* for the first pre-allocate 100 Notification spaces */
        oRemoteApiConfig.dwMbxNotifyAmount  = 50;                   /* for the first pre-allocate 50 Notification spaces */
        oRemoteApiConfig.dwMbxUsrNotifySize = 3000;                 /* 3K user space for Mailbox Notifications */
        oRemoteApiConfig.dwCycErrInterval   = 500;                  /* span between to consecutive cyclic notifications of same type */
        if (1 <= nVerbose)
        {
            OsMemcpy(&oRemoteApiConfig.LogParms, G_pEcLogParms, sizeof(EC_T_LOG_PARMS));
            oRemoteApiConfig.LogParms.dwLogLevel = EC_LOG_LEVEL_ERROR;
        }
        EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Start Remote API Server now\n"));
        dwRes = emRasSrvStart(&oRemoteApiConfig, &S_pvRemoteApiSrvH);
        if (EC_E_NOERROR != dwRes)
        {
            EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "ERROR: Cannot spawn Remote API Server\n"));
        }
        S_bRasSrvStarted = EC_TRUE;
    }
#endif
    /* Initialize EtherCAT master */
    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "==========================\n"));
    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Initialize EtherCAT Master\n"));
    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "==========================\n"));
    {
        EC_T_INIT_MASTER_PARMS oInitParms;

        OsMemset(&oInitParms, 0, sizeof(EC_T_INIT_MASTER_PARMS));
        oInitParms.dwSignature                   = ATECAT_SIGNATURE;
        oInitParms.dwSize                        = sizeof(EC_T_INIT_MASTER_PARMS);
        oInitParms.pOsParms                      = poOsParms;
        oInitParms.pLinkParms                    = poLinkParms;
        oInitParms.pLinkParmsRed                 = poLinkParmsRed;
        oInitParms.dwBusCycleTimeUsec            = dwBusCycleTimeUsec;
        oInitParms.dwMaxBusSlaves                = MASTER_CFG_ECAT_MAX_BUS_SLAVES;
        oInitParms.dwMaxAcycFramesQueued         = MASTER_CFG_MAX_ACYC_FRAMES_QUEUED;
        if (oInitParms.dwBusCycleTimeUsec >= 1000)
        {
            oInitParms.dwMaxAcycBytesPerCycle    = MASTER_CFG_MAX_ACYC_BYTES_PER_CYC;
        }
        else
        {
            oInitParms.dwMaxAcycBytesPerCycle    = 1500;
            oInitParms.dwMaxAcycFramesPerCycle   = 1;
            oInitParms.dwMaxAcycCmdsPerCycle     = 20;
        }
        oInitParms.dwEcatCmdMaxRetries           = MASTER_CFG_MAX_ACYC_CMD_RETRIES;
        if (1 <= nVerbose)
        {
            OsMemcpy(&oInitParms.LogParms, G_pEcLogParms, sizeof(EC_T_LOG_PARMS));
            switch (nVerbose)
            {
            case 0: oInitParms.LogParms.dwLogLevel = EC_LOG_LEVEL_SILENT;      break;
            case 1: oInitParms.LogParms.dwLogLevel = EC_LOG_LEVEL_WARNING;     break;
            case 2: oInitParms.LogParms.dwLogLevel = EC_LOG_LEVEL_WARNING;     break;
            case 3: oInitParms.LogParms.dwLogLevel = EC_LOG_LEVEL_WARNING;     break;
            case 4: oInitParms.LogParms.dwLogLevel = EC_LOG_LEVEL_INFO;        break;
            case 5: oInitParms.LogParms.dwLogLevel = EC_LOG_LEVEL_VERBOSE;     break;
            default: /* no break */
            case 6: oInitParms.LogParms.dwLogLevel = EC_LOG_LEVEL_VERBOSE_CYC; break;
            }
        }
        dwRes = ecatInitMaster(&oInitParms);
	//OsSleep(1000);//wzz 2019/6/3  motrotech zhanglishan  advise
        if (dwRes != EC_E_NOERROR)
        {
            dwRetVal = dwRes;
            EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Cannot initialize EtherCAT-Master: %s (0x%lx))\n", ecatGetText(dwRes), dwRes));
            goto Exit;
        }
    }
    /* Print MAC address */
    if (nVerbose > 0)
    {
        ETHERNET_ADDRESS oSrcMacAddress;

        dwRes = ecatGetSrcMacAddress(&oSrcMacAddress);
        if (dwRes != EC_E_NOERROR)
        {
            EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Cannot get MAC address: %s (0x%lx))\n", ecatGetText(dwRes), dwRes));
        }
        EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "EtherCAT network adapter MAC: %02X-%02X-%02X-%02X-%02X-%02X\n",
            oSrcMacAddress.b[0], oSrcMacAddress.b[1], oSrcMacAddress.b[2], oSrcMacAddress.b[3], oSrcMacAddress.b[4], oSrcMacAddress.b[5]));
    }
    /* Create cyclic task to trigger master jobs */
    /*********************************************/
    {
        CEcTimer oTimeout;

        pDemoThreadParam->bJobThreadRunning  = EC_FALSE;
        pDemoThreadParam->bJobThreadShutdown = EC_FALSE;
        S_pvtJobThread = OsCreateThread((EC_T_CHAR*)"tEcJobTask", tEcJobTask,
#if !(defined EC_VERSION_GO32)
            JOBS_THREAD_PRIO,
#else
            dwBusCycleTimeUsec,
#endif
            JOBS_THREAD_STACKSIZE, (EC_T_VOID*)pDemoThreadParam);
#ifdef RTAI
        OsMakeThreadPeriodic(S_pvtJobThread, dwBusCycleTimeUsec);
#endif
        /* wait until thread is running */
        oTimeout.Start(2000);
        while (!oTimeout.IsElapsed() && !pDemoThreadParam->bJobThreadRunning)
        {
            OsSleep(10);
        }
        if (!pDemoThreadParam->bJobThreadRunning)
        {
            dwRetVal = EC_E_TIMEOUT;
            EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Timeout starting JobTask\n"));
            goto Exit;
        }
    }
    /* Configure master */
    dwRes = ecatConfigureMaster(eCnfType, pbyCnfData, dwCnfDataLen);
    if (dwRes != EC_E_NOERROR)
    {
        dwRetVal = dwRes;
        EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Cannot configure EtherCAT-Master: %s (0x%lx))\n", ecatGetText(dwRes), dwRes));
        goto Exit;
    }
    /* configure DC/DCM master is started with ENI */
    if (pbyCnfData != EC_NULL)
    {
        /* configure DC */
        {
            EC_T_DC_CONFIGURE oDcConfigure;

            OsMemset(&oDcConfigure, 0, sizeof(EC_T_DC_CONFIGURE));
            oDcConfigure.dwTimeout          = ETHERCAT_DC_TIMEOUT;
            oDcConfigure.dwDevLimit         = ETHERCAT_DC_DEV_LIMIT;
            oDcConfigure.dwSettleTime       = ETHERCAT_DC_SETTLE_TIME;
            if (eDcmMode_MasterRefClock == eDcmMode)
            {
                oDcConfigure.dwTotalBurstLength = 10000;
                oDcConfigure.dwBurstBulk        = 1;
            }
            else
            {
                oDcConfigure.dwTotalBurstLength = ETHERCAT_DC_ARMW_BURSTCYCLES;
                if (dwBusCycleTimeUsec < 1000)
                {
                    /* if the cycle time is below 1000 usec, we have to reduce the number of frames sent within one cycle */
                    oDcConfigure.dwBurstBulk = ETHERCAT_DC_ARMW_BURSTSPP / 2;
                }
                else
                {
                    oDcConfigure.dwBurstBulk = ETHERCAT_DC_ARMW_BURSTSPP;
                }
            }
#if (defined INCLUDE_DCX)
            if (eDcmMode_Dcx == S_eDcmMode)
            {
                oDcConfigure.bAcycDistributionDisabled = EC_FALSE; /* Enable acyclic distribution if cycle time is above 1000 usec to get DCX in sync */
            }
            else
            {
                oDcConfigure.bAcycDistributionDisabled = EC_TRUE;
            }
#else
            oDcConfigure.bAcycDistributionDisabled = EC_TRUE;
#endif
            dwRes = ecatDcConfigure(&oDcConfigure);
            if (dwRes != EC_E_NOERROR )
            {
                dwRetVal = dwRes;
                EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Cannot configure DC! (Result = 0x%x)", dwRes));
                goto Exit;
            }
        }
        /* configure DCM */
        {
            EC_T_DCM_CONFIG oDcmConfig;
            EC_T_BOOL       bLogEnabled = EC_FALSE;
            EC_T_INT        nCtlSetVal  = ((dwBusCycleTimeUsec*2)/3)*1000; /* set value in nanosec, 66% of the bus cycle */
#ifdef DCM_ENABLE_LOGFILE
            if (3 <= nVerbose)
            {
                bLogEnabled = EC_TRUE;
            }
#endif
            OsMemset(&oDcmConfig, 0, sizeof(EC_T_DCM_CONFIG));
            switch (eDcmMode)
            {
            case eDcmMode_Off:
                oDcmConfig.eMode = eDcmMode_Off;
                break;
            case eDcmMode_BusShift:
                oDcmConfig.eMode = eDcmMode_BusShift;
                oDcmConfig.u.BusShift.nCtlSetVal    = nCtlSetVal;
                oDcmConfig.u.BusShift.dwInSyncLimit = (dwBusCycleTimeUsec*1000)/5;    /* 20 % limit in nsec for InSync monitoring */
                oDcmConfig.u.BusShift.bLogEnabled = bLogEnabled;
                if (S_bCtlOff)
                {
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "DCM control loop disabled for diagnosis!\n"));
                    oDcmConfig.u.BusShift.bCtlOff = EC_TRUE;
                }
                break;
             case eDcmMode_MasterShift:
                oDcmConfig.eMode = eDcmMode_MasterShift;
                oDcmConfig.u.MasterShift.nCtlSetVal    = nCtlSetVal;
                oDcmConfig.u.MasterShift.dwInSyncLimit = (dwBusCycleTimeUsec*1000)/5;    /* 20 % limit in nsec for InSync monitoring */
                oDcmConfig.u.MasterShift.bLogEnabled = bLogEnabled;
                if (S_bCtlOff)
                {
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "DCM control loop disabled for diagnosis!\n"));
                    oDcmConfig.u.MasterShift.bCtlOff = EC_TRUE;
                }
                break;
            case eDcmMode_MasterRefClock:
                oDcmConfig.eMode = eDcmMode_MasterRefClock;
                oDcmConfig.u.MasterRefClock.nCtlSetVal  = nCtlSetVal;
                oDcmConfig.u.MasterRefClock.bLogEnabled = bLogEnabled;
                break;
            case eDcmMode_LinkLayerRefClock:
                oDcmConfig.eMode = eDcmMode_LinkLayerRefClock;
                oDcmConfig.u.LinkLayerRefClock.nCtlSetVal = nCtlSetVal;
                oDcmConfig.u.LinkLayerRefClock.bLogEnabled = bLogEnabled;
                break;
#if (defined INCLUDE_DCX)
            case eDcmMode_Dcx:
                oDcmConfig.eMode = eDcmMode_Dcx;
                /* Mastershift */
                oDcmConfig.u.Dcx.MasterShift.nCtlSetVal = nCtlSetVal;
                oDcmConfig.u.Dcx.MasterShift.dwInSyncLimit = (dwBusCycleTimeUsec * 1000) / 5;    /* 20 % limit in nsec for InSync monitoring */
                oDcmConfig.u.Dcx.MasterShift.bLogEnabled = bLogEnabled;
                /* Dcx Busshift */
                oDcmConfig.u.Dcx.nCtlSetVal = nCtlSetVal;
                oDcmConfig.u.Dcx.dwInSyncLimit = (dwBusCycleTimeUsec * 1000) / 5;    /* 20 % limit in nsec for InSync monitoring */
                oDcmConfig.u.Dcx.bLogEnabled = bLogEnabled;
                oDcmConfig.u.Dcx.dwExtClockTimeout = 1000;
                oDcmConfig.u.Dcx.wExtClockFixedAddr = 0; /* 0 only when clock adjustment in external mode configured by EcEngineer */
                if (S_bCtlOff)
                {
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "DCM control loop disabled for diagnosis!\n"));

                    oDcmConfig.u.Dcx.MasterShift.bCtlOff = EC_TRUE;
                    oDcmConfig.u.Dcx.bCtlOff = EC_TRUE;
                }
                break;
#endif
            default:
                dwRetVal = EC_E_NOTSUPPORTED;
                EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "DCM mode is not supported!"));
                goto Exit;

            }
            dwRes = ecatDcmConfigure(&oDcmConfig, 0);
            switch (dwRes)
            {
            case EC_E_NOERROR:
                break;
            case EC_E_FEATURE_DISABLED:
                EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Cannot configure DCM mode!"));
                EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Start with -dcmmode off to run the DC demo without DCM, or prepare the ENI file to support the requested DCM mode"));
                EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "In ET9000 for example, select under ""Advanced settings\\Distributed clocks"" ""DC in use"" and ""Slave Mode"""));
                EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "to support BusShift and MasterRefClock modes."));
                EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Please refer to the class A manual for further information"));
                dwRetVal = dwRes;
                goto Exit;
            default:
                EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Cannot configure DCM mode! %s (Result = 0x%x)", ecatGetText(dwRes), dwRes));
                dwRetVal = dwRes;
                goto Exit;
            }
        }
    }
    /* Register client */
    OsMemset(&S_oRegisterResults, 0, sizeof(EC_T_REGISTERRESULTS));
    dwRes = ecatRegisterClient(ecatNotifyCallback, pDemoThreadParam, &S_oRegisterResults);
    if (dwRes != EC_E_NOERROR)
    {
        dwRetVal = dwRes;
        EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Cannot register client: %s (0x%lx))\n", ecatGetText(dwRes), dwRes));
        goto Exit;
    }
    pDemoThreadParam->pNotInst->SetClientID(S_oRegisterResults.dwClntId);
    pDemoThreadParam->FlashData.dwFlashPDOutSize = S_oRegisterResults.dwPDOutSize;

    /* Print found slaves */
    if (nVerbose >= 2)
    {
        dwRes = ecatScanBus(ETHERCAT_SCANBUS_TIMEOUT);
        pDemoThreadParam->pNotInst->ProcessNotificationJobs();
        switch (dwRes)
        {
        case EC_E_NOERROR:
        case EC_E_BUSCONFIG_MISMATCH:
        case EC_E_LINE_CROSSED:
            PrintSlaveInfos(pDemoThreadParam);
            break;
        default:
            EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Cannot scan bus: %s (0x%lx)\n", ecatGetText(dwRes), dwRes));
            break;
        }
        if (dwRes != EC_E_NOERROR)
        {
            dwRetVal = dwRes;
            goto Exit;
        }
    }
    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "=====================\n"));
    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Start EtherCAT Master\n"));
    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "=====================\n"));

    /* set master and bus state to INIT */
    dwRes = ecatSetMasterState(ETHERCAT_STATE_CHANGE_TIMEOUT, eEcatState_INIT);
    pDemoThreadParam->pNotInst->ProcessNotificationJobs();
    if (dwRes != EC_E_NOERROR)
    {
        EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Cannot start set master state to INIT: %s (0x%lx))\n", ecatGetText(dwRes), dwRes));
        dwRetVal = dwRes;
        goto Exit;
    }
    /******************************************************/
    /* Demo code: Remove/change this in your application  */
    /******************************************************/
    dwRes = myAppPrepare(pDemoThreadParam);
    if (EC_E_NOERROR != dwRes)
    {
        EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, (EC_T_CHAR*)"myAppPrepare failed: %s (0x%lx))\n", ecatGetText(dwRes), dwRes));
        dwRetVal = dwRes;
        goto Exit;
    }
    /* set master and bus state to PREOP */
    dwRes = ecatSetMasterState(ETHERCAT_STATE_CHANGE_TIMEOUT, eEcatState_PREOP);
    pDemoThreadParam->pNotInst->ProcessNotificationJobs();
    if (dwRes != EC_E_NOERROR)
    {
        EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Cannot start set master state to PREOP: %s (0x%lx))\n", ecatGetText(dwRes), dwRes));
        dwRetVal = dwRes;
        goto Exit;
    }
    /* skip this step if demo started without ENI */
    if (pbyCnfData != EC_NULL)
    {
        /******************************************************/
        /* Demo code: Remove/change this in your application  */
        /******************************************************/
        dwRes = myAppSetup(pDemoThreadParam);
        if (EC_E_NOERROR != dwRes)
        {
            EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, (EC_T_CHAR*)"myAppSetup failed: %s (0x%lx))\n", ecatGetText(dwRes), dwRes));
            dwRetVal = dwRes;
            goto Exit;
        }
        /* set master and bus state to SAFEOP */
        dwRes = ecatSetMasterState(ETHERCAT_STATE_CHANGE_TIMEOUT, eEcatState_SAFEOP);
        pDemoThreadParam->pNotInst->ProcessNotificationJobs();
        if (dwRes != EC_E_NOERROR)
        {
            EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Cannot start set master state to SAFEOP: %s (0x%lx))\n", ecatGetText(dwRes), dwRes));

            /* most of the time SAFEOP is not reachable due to DCM */
            if ((eDcmMode_Off != eDcmMode) && (eDcmMode_LinkLayerRefClock != eDcmMode))
            {
            EC_T_DWORD dwStatus = 0;
            EC_T_INT   nDiffCur = 0, nDiffAvg = 0, nDiffMax = 0;

                dwRes = ecatDcmGetStatus(&dwStatus, &nDiffCur, &nDiffAvg, &nDiffMax);
                if (dwRes == EC_E_NOERROR)
                {
                    if (dwStatus != EC_E_NOERROR)
                    {
                        EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "DCM Status: %s (0x%08X)\n", ecatGetText(dwStatus), dwStatus));
                    }
                }
                else
                {
                    EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Cannot get DCM status! %s (0x%08X)\n", ecatGetText(dwRes), dwRes));
                }
            }
            dwRetVal = dwRes;
            goto Exit;
        }
        /* set master and bus state to OP */
        dwRes = ecatSetMasterState(ETHERCAT_STATE_CHANGE_TIMEOUT, eEcatState_OP);
        pDemoThreadParam->pNotInst->ProcessNotificationJobs();
        if (dwRes != EC_E_NOERROR)
        {
            EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Cannot start set master state to OP: %s (0x%lx))\n", ecatGetText(dwRes), dwRes));
            dwRetVal = dwRes;
            goto Exit;
        }
    }
    else
    {
        EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "No ENI file provided. EC-Master started with generated ENI file.\n"));
    }

    if (pDemoThreadParam->TscMeasDesc.bMeasEnabled)
    {
        EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "\n"));
        EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Job times during startup <INIT> to <%s>:\n", ecatStateToStr(ecatGetMasterState())));
        PERF_MEASURE_JOBS_SHOW();       /* show job times */
        EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "\n"));
        ecatPerfMeasReset(&pDemoThreadParam->TscMeasDesc, 0xFFFFFFFF);        /* clear job times of startup phase */
    }

#if (defined DEBUG) && (defined XENOMAI)
    /* Enabling mode switch warnings for shadowed task */
    dwRes = rt_task_set_mode(0, T_WARNSW, NULL);
    if (0 != dwRes)
    {
        EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "EnableRealtimeEnvironment: rt_task_set_mode returned error 0x%lx\n", dwRes));
        OsDbgAssert(EC_FALSE);
    }
#endif

    /* run the demo */
    if (dwDuration != 0)
    {
        pDemoThreadParam->oDuration.Start(dwDuration);
    }  

    // for(EC_T_DWORD i= 0;i< 12;i++)
    // {
    // 	Process_Switch(i, COMMAND_START);
    // }
    //Process_Switch(5, COMMAND_START);

    while (bRun && (!pDemoThreadParam->oDuration.IsStarted() || !pDemoThreadParam->oDuration.IsElapsed()))
    {
        if (nVerbose >= 2)
        {
            PERF_MEASURE_JOBS_SHOW();       /* show job times */
        }
        bRun = !OsTerminateAppRequest();/* check if demo shall terminate */

        /*****************************************************************************************/
        /* Demo code: Remove/change this in your application: Do some diagnosis outside job task */
        /*****************************************************************************************/
        myAppDiagnosis(pDemoThreadParam);

        if (pbyCnfData != EC_NULL)
        {
            if ((eDcmMode_Off != S_eDcmMode) && (eDcmMode_LinkLayerRefClock != S_eDcmMode))
            {
                EC_T_DWORD dwStatus = 0;
                EC_T_BOOL  bWriteDiffLog = EC_FALSE;
                EC_T_INT   nDiffCur = 0, nDiffAvg = 0, nDiffMax = 0;

                if (!oDcmStatusTimer.IsStarted() || oDcmStatusTimer.IsElapsed())
                {
                    bWriteDiffLog = EC_TRUE;
                    oDcmStatusTimer.Start(5000);
                }

                dwRes = ecatDcmGetStatus(&dwStatus, &nDiffCur, &nDiffAvg, &nDiffMax);
                if (dwRes == EC_E_NOERROR)
                {
                    if (bFirstDcmStatus)
                    {
                        EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "DCM during startup (<INIT> to <%s>)", ecatStateToStr(ecatGetMasterState())));
                    }
                    if ((dwStatus != EC_E_NOTREADY) && (dwStatus != EC_E_BUSY) && (dwStatus != EC_E_NOERROR))
                    {
                        EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "DCM Status: %s (0x%08X)\n", ecatGetText(dwStatus), dwStatus));
                    }
                    if (bWriteDiffLog && (nVerbose >= 3))
                    {
                        EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "DCM Diff(ns): Cur=%7d, Avg=%7d, Max=%7d", nDiffCur, nDiffAvg, nDiffMax));
                    }
                }
                else
                {
                    if ((eEcatState_OP == ecatGetMasterState()) || (eEcatState_SAFEOP == ecatGetMasterState()))
                    {
                        EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Cannot get DCM status! %s (0x%08X)\n", ecatGetText(dwRes), dwRes));
                    }
                }
#if (defined INCLUDE_DCX)
                if (eDcmMode_Dcx == S_eDcmMode && EC_E_NOERROR == dwRes)
                {
                EC_T_INT64 nTimeStampDiff = 0;
                    dwRes = ecatDcxGetStatus(&dwStatus, &nDiffCur, &nDiffAvg, &nDiffMax, &nTimeStampDiff);
                    if (EC_E_NOERROR == dwRes)
                    {
                        if (bFirstDcmStatus)
                        {
                            EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "DCX during startup (<INIT> to <%s>)", ecatStateToStr(ecatGetMasterState())));
                        }
                        if ((dwStatus != EC_E_NOTREADY) && (dwStatus != EC_E_BUSY) && (dwStatus != EC_E_NOERROR))
                        {
                            EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "DCX Status: %s (0x%08X)\n", ecatGetText(dwStatus), dwStatus));
                        }
                        if (bWriteDiffLog && (nVerbose >= 3))
                        {
                            EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "DCX Diff(ns): Cur=%7d, Avg=%7d, Max=%7d, TimeStamp=%7d", nDiffCur, nDiffAvg, nDiffMax, nTimeStampDiff));
                        }
                    }
                    else
                    {
                        if ((eEcatState_OP == ecatGetMasterState()) || (eEcatState_SAFEOP == ecatGetMasterState()))
                        {
                            EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Cannot get DCX status! %s (0x%08X)\n", ecatGetText(dwRes), dwRes));
                        }
                    }
                }
#endif
                if (bFirstDcmStatus && (EC_E_NOERROR == dwRes))
                {
                    bFirstDcmStatus = EC_FALSE;
#if (defined ATECAT_VERSION) && (ATECAT_VERSION >= 0x02040106)
                    ecatDcmResetStatus();
#endif
                }
            }
        }
        /* process notification jobs */
        pDemoThreadParam->pNotInst->ProcessNotificationJobs();

        OsSleep(5);
    }

    if (pDemoThreadParam->TscMeasDesc.bMeasEnabled)
    {
        EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "\n"));
        EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Job times before shutdown\n"));
        PERF_MEASURE_JOBS_SHOW();       /* show job times */
    }
    for(EC_T_DWORD i=0; i< 12;i++)
    {
    	Process_Switch(i,COMMAND_STOP);
    }

Exit:
    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "========================\n"));
    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Shutdown EtherCAT Master\n"));
    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "========================\n"));

    /* Set Master state back to INIT */
    if (eEcatState_UNKNOWN != ecatGetMasterState())
    {
        dwRes = ecatSetMasterState(ETHERCAT_STATE_CHANGE_TIMEOUT, eEcatState_INIT);
        if (EC_E_NOERROR != dwRes)
        {
            EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Cannot stop EtherCAT-Master: %s (0x%lx))\n", ecatGetText(dwRes), dwRes));
        }
    }
    /* Unregister client */
    if (S_oRegisterResults.dwClntId != 0)
    {
        dwRes = ecatUnregisterClient(S_oRegisterResults.dwClntId);
        if (EC_E_NOERROR != dwRes)
        {
            EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Cannot unregister client: %s (0x%lx))\n", ecatGetText(dwRes), dwRes));
        }
        S_oRegisterResults.dwClntId = 0;
    }
#if (defined DEBUG) && (defined XENOMAI)
    /* Disable PRIMARY to SECONDARY MODE switch warning */
    dwRes = rt_task_set_mode(T_WARNSW, 0, NULL);
    if (dwRes != 0)
    {
        EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "OsCreateThread: rt_task_set_mode returned error %d\n", dwRes));
        OsDbgAssert(EC_FALSE);
    }
#endif /* XENOMAI */
    /* Shutdown tEcJobTask */
    {
        CEcTimer oTimeout;
        pDemoThreadParam->bJobThreadShutdown = EC_TRUE;
        oTimeout.Start(2000);
        while (pDemoThreadParam->bJobThreadRunning && !oTimeout.IsElapsed())
        {
            OsSleep(10);
        }
        if (S_pvtJobThread != EC_NULL)
        {
            OsDeleteThreadHandle(S_pvtJobThread);
            S_pvtJobThread = EC_NULL;
        }
    }
    /* Deinitialize master */
    dwRes = ecatDeinitMaster();
    if (EC_E_NOERROR != dwRes)
    {
        EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Cannot de-initialize EtherCAT-Master: %s (0x%lx)\n", ecatGetText(dwRes), dwRes));
    }
#ifdef ATEMRAS_SERVER
    /* Stop RAS server */
    if (S_bRasSrvStarted)
    {
        EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Stop Remote Api Server\n"));

        if (EC_E_NOERROR != emRasSrvStop(S_pvRemoteApiSrvH, 2000))
        {
            EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Remote API Server shutdown failed\n"));
        }
    }
#endif
    if (pDemoThreadParam->TscMeasDesc.bMeasEnabled)
    {
        PERF_MEASURE_JOBS_DEINIT();
    }
    /* delete notification context */
    SafeDelete(pDemoThreadParam->pNotInst);

    return dwRetVal;
}


/********************************************************************************/
/** \brief  Trigger jobs to drive master, and update process data.
*
* \return N/A
*/
static EC_T_VOID tEcJobTask(EC_T_VOID* pvThreadParamDesc)
{
    EC_T_DWORD           dwRes             = EC_E_ERROR;
    T_DEMO_THREAD_PARAM* pDemoThreadParam  = (T_DEMO_THREAD_PARAM*)pvThreadParamDesc;
    EC_T_CPUSET          CpuSet;
    EC_T_INT             nOverloadCounter  = 0;               /* counter to check if cycle time is to short */
    EC_T_BOOL            bOk;
    EC_T_USER_JOB_PARMS  oJobParms;
    OsMemset(&oJobParms, 0, sizeof(EC_T_USER_JOB_PARMS));

    EC_CPUSET_ZERO(CpuSet);
    EC_CPUSET_SET(CpuSet, pDemoThreadParam->dwCpuIndex);
    bOk = OsSetThreadAffinity(EC_NULL, CpuSet);
    if (!bOk)
    {
        EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Error: Set job task affinitiy, invalid CPU index %d\n", pDemoThreadParam->dwCpuIndex));
        goto Exit;
    }

    /* Start periodic task right before where it is used,
     * otherwise, OsSleepTillTick() might fail if this thread is suspended before being able to call OsMakeThreadPeriodic() */
#if (defined XENOMAI)
    OsMakeThreadPeriodic(rt_task_self(), pDemoThreadParam->dwBusCycleTimeUsec);
#endif

    /* demo loop */
    pDemoThreadParam->bJobThreadRunning = EC_TRUE;
    do
    {
        /* wait for next cycle (event from scheduler task) */
#if (defined RTAI) || (defined XENOMAI)
        OsSleepTillTick(); /* period is set after creating jobtask */
#else
        OsWaitForEvent(pDemoThreadParam->pvTimingEvent, EC_WAITINFINITE);
#endif /* !RTAI && !XENOMAI*/

        PERF_JOB_END(PERF_CycleTime);
        PERF_JOB_START(PERF_CycleTime);

        /* process all received frames (read new input values) */
        PERF_JOB_START(JOB_ProcessAllRxFrames);
        dwRes = ecatExecJob(eUsrJob_ProcessAllRxFrames, &oJobParms);
        if (EC_E_NOERROR != dwRes && EC_E_INVALIDSTATE != dwRes && EC_E_LINK_DISCONNECTED != dwRes)
        {
            EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "ERROR: ecatExecJob( eUsrJob_ProcessAllRxFrames): %s (0x%lx)\n", ecatGetText(dwRes), dwRes));
        }
        PERF_JOB_END(JOB_ProcessAllRxFrames);

        if (EC_E_NOERROR == dwRes)
        {
            if (!oJobParms.bAllCycFramesProcessed)
            {
                /* it is not reasonable, that more than 5 continuous frames are lost */
                nOverloadCounter += 10;
                if (nOverloadCounter >= 50)
                {
                    EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "Error: System overload: Cycle time too short or huge jitter!\n"));
                }
                else
                {
                    EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "eUsrJob_ProcessAllRxFrames - not all previously sent frames are received/processed (frame loss)!\n"));
                }
                if (pDemoThreadParam->TscMeasDesc.bMeasEnabled)
                {
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "PerfMsmt '%s' (current) [usec]: %3d.%d\n", S_aszMeasInfo[PERF_CycleTime], pDemoThreadParam->TscMeasDesc.aTscTime[PERF_CycleTime].dwCurr / 10, pDemoThreadParam->TscMeasDesc.aTscTime[PERF_CycleTime].dwCurr % 10));
                }
            }
            else
            {
                /* everything o.k.? If yes, decrement overload counter */
                if (nOverloadCounter > 0)    nOverloadCounter--;
            }
        }
        /* Handle DCM logging                                    */
#ifdef DCM_ENABLE_LOGFILE
        PERF_JOB_START(PERF_DCM_Logfile);
        {
            EC_T_CHAR* pszLog = EC_NULL;

            ecatDcmGetLog(&pszLog);
            if ((EC_NULL != pszLog))
            {
                ((CAtEmLogging*)pEcLogContext)->LogDcm(pszLog);
            }
        }
        PERF_JOB_END(PERF_DCM_Logfile);
#endif
        /*****************************************************/
        /* Demo code: Remove/change this in your application: Working process data cyclic call */
        /*****************************************************/
        PERF_JOB_START(PERF_myAppWorkpd);
        {
            EC_T_BYTE* abyPdIn = ecatGetProcessImageInputPtr();
            EC_T_BYTE* abyPdOut = ecatGetProcessImageOutputPtr();

            if((abyPdIn != EC_NULL) && (abyPdOut != EC_NULL))
            {
                myAppWorkpd(pDemoThreadParam, abyPdIn, abyPdOut);
            }
        }
        PERF_JOB_END(PERF_myAppWorkpd);

        /* write output values of current cycle, by sending all cyclic frames */
        PERF_JOB_START(JOB_SendAllCycFrames);
        dwRes = ecatExecJob(eUsrJob_SendAllCycFrames, &oJobParms);
        if (EC_E_NOERROR != dwRes && EC_E_INVALIDSTATE != dwRes && EC_E_LINK_DISCONNECTED != dwRes)
        {
            EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "ecatExecJob( eUsrJob_SendAllCycFrames,    EC_NULL ): %s (0x%lx)\n", ecatGetText(dwRes), dwRes));
        }
        PERF_JOB_END(JOB_SendAllCycFrames);

        /* remove this code when using licensed version */
        if (EC_E_EVAL_EXPIRED == dwRes)
        {
            bRun = EC_FALSE;        /* set shutdown flag */
        }

        /* Execute some administrative jobs. No bus traffic is performed by this function */
        PERF_JOB_START(JOB_MasterTimer);
        dwRes = ecatExecJob(eUsrJob_MasterTimer, EC_NULL);
        if (EC_E_NOERROR != dwRes && EC_E_INVALIDSTATE != dwRes)
        {
            EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "ecatExecJob(eUsrJob_MasterTimer, EC_NULL): %s (0x%lx)\n", ecatGetText(dwRes), dwRes));
        }
        PERF_JOB_END(JOB_MasterTimer);

        /* send queued acyclic EtherCAT frames */
        PERF_JOB_START(JOB_SendAcycFrames);
        dwRes = ecatExecJob(eUsrJob_SendAcycFrames, EC_NULL);
        if (EC_E_NOERROR != dwRes && EC_E_INVALIDSTATE != dwRes && EC_E_LINK_DISCONNECTED != dwRes)
        {
            EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "ecatExecJob(eUsrJob_SendAcycFrames, EC_NULL): %s (0x%lx)\n", ecatGetText(dwRes), dwRes));
        }
        PERF_JOB_END(JOB_SendAcycFrames);

#if !(defined NO_OS)
    } while (!pDemoThreadParam->bJobThreadShutdown);

    PERF_MEASURE_JOBS_SHOW();

    pDemoThreadParam->bJobThreadRunning = EC_FALSE;
#else
    /* in case of NO_OS the job task function is called cyclically within the timer ISR */
    } while (EC_FALSE);
    pDemoThreadParam->bJobThreadRunning = !pDemoThreadParam->bJobThreadShutdown;
#endif

Exit:
#if (defined EC_VERSION_RTEMS)
    rtems_task_delete(RTEMS_SELF);
#endif
    return;
}

/********************************************************************************/
/** \brief  Handler for master notifications
*
* \return  Status value.
*/
static EC_T_DWORD ecatNotifyCallback(
    EC_T_DWORD         dwCode,  /**< [in]   Notification code */
    EC_T_NOTIFYPARMS*  pParms   /**< [in]   Notification parameters */
                                         )
{
    EC_T_DWORD dwRetVal = EC_E_NOERROR;
    T_DEMO_THREAD_PARAM* pDemoThreadParam = EC_NULL;

    if ((EC_NULL == pParms) || (EC_NULL == pParms->pCallerData))
    {
        dwRetVal = EC_E_INVALIDPARM;
        goto Exit;
    }

    pDemoThreadParam = (T_DEMO_THREAD_PARAM*)(pParms->pCallerData);

    /* notification for application ? */
    if ((dwCode >= EC_NOTIFY_APP) && (dwCode <= EC_NOTIFY_APP+EC_NOTIFY_APP_MAX_CODE))
    {
        /*****************************************************/
        /* Demo code: Remove/change this in your application */
        /* to get here the API ecatNotifyApp(dwCode, pParms) has to be called */
        /*****************************************************/
        dwRetVal = myAppNotify(dwCode - EC_NOTIFY_APP, pParms);
    }
    else
    {
        /* call the default handler */
        dwRetVal = pDemoThreadParam->pNotInst->ecatNotify(dwCode, pParms);
    }

Exit:
    return dwRetVal;
}


/********************************************************************************/
/** \brief  Handler for master RAS notifications
*
*
* \return  Status value.
*/
#ifdef ATEMRAS_SERVER
static EC_T_DWORD RasNotifyWrapper(
                            EC_T_DWORD         dwCode,
                            EC_T_NOTIFYPARMS*  pParms
                            )
{
    EC_T_DWORD                      dwRetVal                = EC_E_NOERROR;
    CEmNotification*                pNotInst                = EC_NULL;

    if ((EC_NULL == pParms)||(EC_NULL==pParms->pCallerData))
    {
        dwRetVal = EC_E_INVALIDPARM;
        goto Exit;
    }

    pNotInst = (CEmNotification*)(pParms->pCallerData);
    dwRetVal = pNotInst->emRasNotify(dwCode, pParms);
Exit:

    return dwRetVal;
}
#endif

/*-MYAPP---------------------------------------------------------------------*/

#define DEMO_MAX_NUM_OF_SLAVES     16//WZZ

/***************************************************************************************************/
/**
\brief  Initialize Application

\return EC_E_NOERROR on success, error code otherwise.
*/
static EC_T_DWORD myAppInit(T_DEMO_THREAD_PARAM* pDemoThreadParam)
{
    EC_UNREFPARM(pDemoThreadParam);

    myInit(pDemoThreadParam);//wzz
    return EC_E_NOERROR;
}

/***************************************************************************************************/
/**
\brief  Initialize Slave Instance.

Find slave parameters.
\return EC_E_NOERROR on success, error code otherwise.
*/
static EC_T_DWORD myAppPrepare(T_DEMO_THREAD_PARAM* pDemoThreadParam)
{
    EC_T_WORD           wFixedAddress = INVALID_FIXED_ADDR;

    EC_UNREFPARM(pDemoThreadParam);
    myPrepare(pDemoThreadParam,ecvendor_tsino_dynatron,0x1);
    return EC_E_NOERROR;
}

/***************************************************************************************************/
/**
\brief  Setup slave parameters (normally done in PREOP state

  - SDO up- and Downloads
  - Read Object Dictionary

\return EC_E_NOERROR on success, error code otherwise.
*/
static EC_T_DWORD myAppSetup(T_DEMO_THREAD_PARAM* pDemoThreadParam)
{
    EC_UNREFPARM(pDemoThreadParam);
    mySetup(pDemoThreadParam,ecatGetProcessImageInputPtr(),ecatGetProcessImageOutputPtr());
    return EC_E_NOERROR;
}

/***************************************************************************************************/
/**
\brief  demo application working process data function.

  This function is called in every cycle after the the master stack is started.

*/
static EC_T_DWORD myAppWorkpd(T_DEMO_THREAD_PARAM* pDemoThreadParam,
    EC_T_BYTE*          pbyPDIn,        /* [in]  pointer to process data input buffer */
    EC_T_BYTE*          pbyPDOut        /* [in]  pointer to process data output buffer */
    )
{
    EC_UNREFPARM(pDemoThreadParam);
    EC_UNREFPARM(pbyPDIn);
   
   Process_Commands(pDemoThreadParam);
   Process_SetModOperation(OPMODE_CSP);
   myWorkpd(pDemoThreadParam,pbyPDIn,pbyPDOut);
    return EC_E_NOERROR;
}

/***************************************************************************************************/
/**
\brief  demo application doing some diagnostic tasks

  This function is called in sometimes from the main demo task
*/
static EC_T_DWORD myAppDiagnosis(T_DEMO_THREAD_PARAM* pDemoThreadParam)
{
    EC_UNREFPARM(pDemoThreadParam);

    return EC_E_NOERROR;
}

/********************************************************************************/
/** \brief  Handler for application notifications
*
*  !!! No blocking API shall be called within this function!!!
*  !!! Function is called by cylic task                    !!!
*
* \return  Status value.
*/
static EC_T_DWORD myAppNotify(
    EC_T_DWORD              dwCode,     /* [in]  Application notification code */
    EC_T_NOTIFYPARMS*       pParms      /* [in]  Notification parameters */
    )
{
    EC_T_DWORD dwRetVal = EC_E_ERROR;
#if (defined INCLUDE_LOG_MESSAGES)
    T_DEMO_THREAD_PARAM* pDemoThreadParam = (T_DEMO_THREAD_PARAM*)(pParms->pCallerData);
#else
    EC_UNREFPARM(pParms);
#endif

    /* dispatch notification code */
    switch(dwCode)
    {
    case 1:
        EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Application notification code=%d received\n", dwCode));
        /* dwRetVal = EC_E_NOERROR; */
        break;
    case 2:
        break;
    default:
        break;
    }

    return dwRetVal;
}

/*-END OF SOURCE FILE--------------------------------------------------------*/
