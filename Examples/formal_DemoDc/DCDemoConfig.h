/*-----------------------------------------------------------------------------
 * DCDemoConfig.h
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              EtherCAT Master demo configuration header
 *---------------------------------------------------------------------------*/

/*-INCLUDES------------------------------------------------------------------*/


/*-DEFINES-------------------------------------------------------------------*/

/*********************/
/* thread priorities */
/*********************/
#if defined WIN32 && !defined UNDER_CE && !defined RTOS_32 && !defined UNDER_RTSS
 /* we need to set all threads to the highest possible priority to avoid errors! */
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)THREAD_PRIORITY_TIME_CRITICAL) /* EtherCAT master trigger thread priority */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)THREAD_PRIORITY_TIME_CRITICAL) /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)THREAD_PRIORITY_TIME_CRITICAL) /* EtherCAT master interrupt service thread priority */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)THREAD_PRIORITY_LOWEST)  /* EtherCAT message logging thread priority */
 #define MAIN_THREAD_PRIO            ((EC_T_DWORD)THREAD_PRIORITY_NORMAL)
#elif (defined UNDER_RTSS)
 #define TIMER_THREAD_PRIO           (RT_PRIORITY_MAX-0) /* EtherCAT master trigger thread priority */
 #define JOBS_THREAD_PRIO            (RT_PRIORITY_MAX-1) /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            (RT_PRIORITY_MAX-2) /* EtherCAT master interrupt service thread priority */
 #define LOG_THREAD_PRIO             (RT_PRIORITY_MIN)   /* EtherCAT message logging thread priority */
 #define LOG_ROLLOVER                ((EC_T_WORD)0)      /* EtherCAT message logging rollover counter limit */
 #define MAIN_THREAD_PRIO            (RT_PRIORITY_MIN)
 #define JOBS_THREAD_STACKSIZE       0x8000
 #define LOG_THREAD_STACKSIZE        0x8000
#elif (defined RTOS_32)
 #define MAIN_THREAD_PRIO_OFFSET     10
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)64)    /* EtherCAT master trigger thread priority */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)63)    /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)62)    /* EtherCAT master interrupt service thread priority */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)1 )    /* EtherCAT message logging thread priority */
 #define MAIN_THREAD_PRIO            ((EC_T_DWORD)3+MAIN_THREAD_PRIO_OFFSET)        
 #define JOBS_THREAD_STACKSIZE       0x8000
 #define LOG_THREAD_STACKSIZE        0x8000
#elif (defined EC_VERSION_QNX)
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)100)   /* EtherCAT master trigger thread priority */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)99)    /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)98)    /* EtherCAT master interrupt service thread priority */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)40)    /* EtherCAT message logging thread priority */
 #define MAIN_THREAD_PRIO            ((EC_T_DWORD)30)
 #define REMOTE_RECV_THREAD_PRIO     ((EC_T_DWORD)50)    /* slightly higher than logger */
#elif (defined __RCX__)
 #define REALTIME_PRIORITY_OFFSET    TSK_PRIO_2
 #define MAIN_THREAD_PRIO_OFFSET     TSK_PRIO_50
 #define TIMER_THREAD_PRIO           (0+REALTIME_PRIORITY_OFFSET)   /* EtherCAT master trigger thread priority */
 #define JOBS_THREAD_PRIO            (1+REALTIME_PRIORITY_OFFSET)   /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            (2+REALTIME_PRIORITY_OFFSET)   /* EtherCAT master interrupt service thread priority */
 #define LOG_THREAD_PRIO             (5+MAIN_THREAD_PRIO_OFFSET)    /* EtherCAT message logging thread priority */
 #define LOG_ROLLOVER                ((EC_T_WORD)0)                 /* EtherCAT message logging rollover counter limit */
 #define MAIN_THREAD_PRIO            (MAIN_THREAD_PRIO_OFFSET)
#elif (defined RTAI)
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)99)   /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)5)    /* EtherCAT master job thread priority (tEcJobTask) */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)99)   /* EtherCAT master packet receive thread priority (tLOsaL_IST) */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)99)   /* EtherCAT message logging thread priority (tAtEmLog) */
#elif (defined LINUX)
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)99)   /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)98)   /* EtherCAT master job thread priority (tEcJobTask) */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)97)   /* EtherCAT master packet receive thread priority (tLOsaL_IST) */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)29)   /* EtherCAT message logging thread priority (tAtEmLog) */
 #define MAIN_THREAD_PRIO            ((EC_T_DWORD)39)   /* Main thread */
#elif (defined __INTEGRITY)
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)200)   /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)199)   /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)198)   /* EtherCAT master interrupt service thread priority */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)40)    /* EtherCAT message logging thread priority */
 #define MAIN_THREAD_PRIO            ((EC_T_DWORD)40)
#elif (defined __INTIME__)
 /*Priority 0 to 127 Used by the OS for servicing external interrupts. */
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)128)   /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)129)   /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)0)     /* Is not used for INtime. The receive thread priority is defined by the Interrupt Level  */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)200)   /* EtherCAT message logging thread priority */
 #define MAIN_THREAD_PRIO            ((EC_T_DWORD)130)
#elif (defined __TKERNEL)
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)1)     /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)2)     /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)3)     /* EtherCAT master interrupt service thread priority */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)4)     /* EtherCAT message logging thread priority */
 #define MAIN_THREAD_PRIO            ((EC_T_DWORD)10)
#elif (defined EC_VERSION_SYSBIOS)
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)12)   /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)12)   /* EtherCAT master job thread priority (tEcJobTask) */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)12)   /* EtherCAT master packet receive thread priority (tLOsaL_IST) */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)1)    /* EtherCAT message logging thread priority (tAtEmLog) */
 #define MAIN_THREAD_PRIO            ((EC_T_DWORD)2)    /* Main thread */
 #if defined SOC_AM572x
  #if defined LINKLAYER_ICSS
   #if defined(INCLUDE_TTS)
    #define DEMO_PARAMETERS            "-auxclk 1000 -v 2 -t 10000 -perf " \
                                       "-icss "                    \
                                       "4 "    /* Instance */      \
                                       "1 "    /* mode */          \
                                       "m "    /* master/slave */          \
                                       "am572x-idk "  /* reference board */ \
                                       "tts 1000 500 "  /* TTS mode and config time */
   #else
     #define DEMO_PARAMETERS            "-auxclk 1000 -v 2 -t 10000 -perf " \
                                      "-icss "                    \
                                      "4 "    /* Instance */      \
                                      "1 "    /* mode */          \
                                      "m "    /* master/slave */  \
                                      "am572x-idk "  /* reference board */ \
                               
   #endif
  #elif defined LINKLAYER_CPSW
   #define DEMO_PARAMETERS            "-auxclk 2000 -v 2 -t 10000 -perf " \
                                      "-cpsw "                    \
                                      "2 "    /* port */          \
                                      "1 "    /* mode */          \
                                      "1 "    /* priority */      \
                                      "m "    /* master flag */   \
                                      "am572x-idk " /* reference board */
  #endif /* LINKLAYER_ICSS,LINKLAYER_CPSW */
 #elif (defined SOC_AM335x)
  #if defined LINKLAYER_ICSS
    #define DEMO_PARAMETERS              "-auxclk 1000 -v 2 -t 10000 -perf " \
                                         "-icss "                    \
                                         "1 "    /* port */          \
                                         "1 "    /* mode */          \
                                         "m "    /* master flag */   \
                                         "am3359-icev2 "             \
                                         "-icss "                    \
                                         "2 "    /* port */          \
                                         "1 "    /* mode */          \
                                         "s "    /* master flag */   \
                                         "am3359-icev2 "
  #elif defined LINKLAYER_CPSW
    #define DEMO_PARAMETERS              "-auxclk 2000 -v 2 -t 10000 -perf " \
                                         "-cpsw "                    \
                                         "1 "    /* port */          \
                                         "1 "    /* mode */          \
                                         "1 "    /* priority */      \
                                         "m "    /* master flag */   \
                                         "custom am33XX " /* custom board for AM33xx*/   \
                                         "1 "    /* PHY address */   \
                                         "1 "    /* PHY connection mode: RGMII */ \
                                         "0 "    /* Not use DMA buffers */
  #endif
 #elif (defined SOC_AM437x)
  #define DEMO_PARAMETERS              "-auxclk 2000 -v 2 -t 10000 -perf " \
                                      "-cpsw "                    \
                                      "1 "    /* port */          \
                                      "1 "    /* mode */          \
                                      "1 "    /* priority */      \
                                      "m "    /* master flag */   \
                                      "am437x-idk " /* custom board for AM437x*/
 #endif

#elif (defined EC_VERSION_RIN32M3)
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)3)   /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)3)   /* EtherCAT master job thread priority (tEcJobTask) */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)3)   /* EtherCAT master packet receive thread priority (tLOsaL_IST) */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)14)    /* EtherCAT message logging thread priority (tAtEmLog) */
 #define DEMO_PARAMETERS             "-auxclk 1000 -t 0 -v 2 -perf -rin32m3"
#elif (defined EC_VERSION_XILINX_STANDALONE)
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)3)   /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)3)   /* EtherCAT master job thread priority (tEcJobTask) */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)3)   /* EtherCAT master packet receive thread priority (tLOsaL_IST) */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)14)    /* EtherCAT message logging thread priority (tAtEmLog) */
 #define DEMO_PARAMETERS             "-auxclk 1000 -v 2 -t 20000 -perf -rin32m3"
#elif (defined EC_VERSION_ETKERNEL)
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)1)     /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)2)     /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)3)     /* EtherCAT master interrupt service thread priority */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)4)     /* EtherCAT message logging thread priority */
 #define MAIN_THREAD_PRIO            ((EC_T_DWORD)10)
 #define PREVENT_IDLE_THREAD_PRIO    ((EC_T_DWORD)20)
 #define DEMO_PARAMETERS             "-b 2000 -fsletsec 1 1 -v 3 -t 10000 -perf"
#elif (defined EC_VERSION_RZT1)
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)1)     /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)2)     /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)3)     /* EtherCAT master interrupt service thread priority */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)4)     /* EtherCAT message logging thread priority */
 #define MAIN_THREAD_PRIO            ((EC_T_DWORD)10)
 #define DEMO_PARAMETERS             "-rzt1 1 -v 2 -t 20000 -perf -auxclk 1000"
 #define LOG_THREAD_STACKSIZE		 0x2000
#elif (defined EC_VERSION_RZGNOOS)
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)1)     /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)2)     /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)3)     /* EtherCAT master interrupt service thread priority */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)4)     /* EtherCAT message logging thread priority */
 #define MAIN_THREAD_PRIO            ((EC_T_DWORD)10)
 #define DEMO_PARAMETERS             "-sheth 1 1 rzg1e -v 3 -t 20000 -perf -auxclk 1000"
 #define LOG_THREAD_STACKSIZE        0x2000
#elif (defined EC_VERSION_ECOS)
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)1)     /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)2)     /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)3)     /* EtherCAT master interrupt service thread priority */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)4)     /* EtherCAT message logging thread priority */
#elif (defined EC_VERSION_JSLWARE)
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)12)   /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)12)   /* EtherCAT master job thread priority (tEcJobTask) */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)12)   /* EtherCAT master packet receive thread priority (tLOsaL_IST) */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)1)    /* EtherCAT message logging thread priority (tAtEmLog) */
 #define MAIN_THREAD_PRIO            ((EC_T_DWORD)2)    /* Main thread */
   #define DEMO_PARAMETERS            "-auxclk 500 -v 2 -t 20000 -perf " \
                                      "-cpsw "                    \
                                      "1 "    /* port */          \
                                      "1 "    /* mode */          \
                                      "1 "    /* priority */      \
                                      "m "    /* master flag */   \
                                      "custom am33XX " /* custom board for AM33xx*/   \
                                      "1 "    /* PHY address */   \
                                      "1 "    /* PHY connection mode: RGMII */ \
                                      "0 "    /* Not use DMA buffers */
#elif (defined EC_VERSION_UC3)
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)1)     /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)2)     /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)3)     /* EtherCAT master interrupt service thread priority */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)4)     /* EtherCAT message logging thread priority */
 #define MAIN_THREAD_PRIO            ((EC_T_DWORD)10)
 #define LOG_THREAD_STACKSIZE		 0x2000
#elif (defined EC_VERSION_UCOS)
#define TIMER_THREAD_PRIO           ((EC_T_DWORD)10)   /* EtherCAT master timer task (tEcTimingTask) */
#define JOBS_THREAD_PRIO            ((EC_T_DWORD)8)   /* EtherCAT master job thread priority (tEcJobTask) */
#define RECV_THREAD_PRIO            ((EC_T_DWORD)8)   /* EtherCAT master packet receive thread priority (tLOsaL_IST) */
#define LOG_THREAD_PRIO             ((EC_T_DWORD)8)    /* EtherCAT message logging thread priority (tAtEmLog) */
#define MAIN_THREAD_PRIO            ((EC_T_DWORD)8)    /* Main thread */
#define DEMO_PARAMETERS            "-v 2 -fslfec 1 1 custom imx6 rmii 1 -perf"

#define TIMER_THREAD_STACKSIZE       0x1000
#define JOBS_THREAD_STACKSIZE        0x1000
#define LOG_THREAD_STACKSIZE         0x1000

#elif (defined EC_VERSION_RTXC)
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)1)     /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)2)     /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)3)     /* EtherCAT master interrupt service thread priority */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)4)     /* EtherCAT message logging thread priority */
 #define DEMO_PARAMETERS             "-DW3504 2 1 lces1 -v 2 -b 2000 -perf -f b:\perfdc.xml"
 
 #define LOG_THREAD_STACKSIZE         0x1000
 #define TIMER_THREAD_STACKSIZE       0x1000
 #define JOBS_THREAD_STACKSIZE        0x1000
 
#elif (defined EC_VERSION_SYLIXOS)
 #define TIMER_THREAD_PRIO           1 		/* EtherCAT master trigger thread priority */
 #define JOBS_THREAD_PRIO            2 		/* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            3 		/* EtherCAT master interrupt service thread priority */
 #define LOG_THREAD_PRIO             4      /* EtherCAT message logging thread priority */
 #define MAIN_THREAD_PRIO            10
#else

 #ifdef VXWORKS_NORMAL_PRIO
  #define REALTIME_PRIORITY_OFFSET    60
 #else
  #define REALTIME_PRIORITY_OFFSET    2
 #endif

 #define TIMER_THREAD_PRIO           (0+REALTIME_PRIORITY_OFFSET)    /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            (1+REALTIME_PRIORITY_OFFSET)    /* EtherCAT master cyclic packet send thread priority */
 #define RECV_THREAD_PRIO            (2+REALTIME_PRIORITY_OFFSET)    /* EtherCAT master packet receive thread priority */
 #define MBX_THREAD_PRIO             (3+REALTIME_PRIORITY_OFFSET)    /* mailbox demo thread priority */

 #define LOG_THREAD_PRIO             (200)                           /* EtherCAT message logging thread priority */
 #define MAIN_THREAD_PRIO            (4+REALTIME_PRIORITY_OFFSET)
 #define REMOTE_RECV_THREAD_PRIO     0

#endif
#ifndef TIMER_THREAD_STACKSIZE
#define TIMER_THREAD_STACKSIZE       0x1000
#endif
#ifndef JOBS_THREAD_STACKSIZE
#define JOBS_THREAD_STACKSIZE        0x4000
#endif
#ifndef LOG_THREAD_STACKSIZE
#define LOG_THREAD_STACKSIZE         0x4000
#endif

/******************/
/* timer settings */
/******************/
#define CYCLE_TIME          1           /* 1 msec */
#define BASE_PERIOD         1000        /* 1000 usec */

/***********************************************/
/* static EtherCAT master configuration values */
/***********************************************/
#if !(defined EC_DEMO_TINY)
#define MASTER_CFG_ECAT_MAX_BUS_SLAVES       256    /* max number of pre-allocated bus slave objects */
#define MASTER_CFG_MAX_ACYC_FRAMES_QUEUED     32    /* max number of acyc frames queued, 127 = the absolute maximum number */
#define MASTER_CFG_MAX_ACYC_BYTES_PER_CYC   4096    /* max number of bytes sent during eUsrJob_SendAcycFrames within one cycle */
#else
#define MASTER_CFG_ECAT_MAX_BUS_SLAVES         8    /* max number of pre-allocated bus slave objects */
#define MASTER_CFG_MAX_ACYC_FRAMES_QUEUED     32    /* max number of acyc frames queued, 127 = the absolute maximum number */
#define MASTER_CFG_MAX_ACYC_BYTES_PER_CYC    512    /* max number of bytes sent during eUsrJob_SendAcycFrames within one cycle */
#endif /* EC_DEMO_TINY */
#define MASTER_CFG_MAX_ACYC_CMD_RETRIES        3

/* DCM controller set value: Distance between frame send time and sync impulse */
#define DCM_CONTROLLER_SETVAL_NANOSEC    ((BASE_PERIOD/2)*1000)            /* 50 % */

/* maximum controller error in nanoseconds */
#if defined WIN32 && !defined UNDER_CE
#define DCM_MAX_CTL_ERROR_NANOSEC       90000
#else
#define DCM_MAX_CTL_ERROR_NANOSEC       30000
#endif

/* maximum time to get in-sync */
#define DCM_SETVAL_TIMEOUT_MSEC         12000

/*-END OF SOURCE FILE--------------------------------------------------------*/
