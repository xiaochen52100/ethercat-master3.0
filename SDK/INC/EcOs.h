/*-----------------------------------------------------------------------------
 * EcOs.h                   
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              EC-Master OS-Layer header
 *---------------------------------------------------------------------------*/

#ifndef INC_ECOS
#define INC_ECOS

#ifndef INC_ECTYPE
#include "EcType.h"
#endif

/* API decoration */
#ifndef EC_FNNAME_DEFAULT
#ifdef __cplusplus
#define EC_FNNAME_DEFAULT extern "C"
#else
#define EC_FNNAME_DEFAULT
#endif
#endif /* EC_FNNAME_DEFAULT */

#ifndef EC_API_DEFAULT
#define EC_API_DEFAULT EC_FNNAME_DEFAULT
#endif

/*****************************************************************************
 * OsPlatform... declarations
 * TODO Fix #defines of OsMalloc in EcOsPlatform.h
 *****************************************************************************/
#ifndef INC_ECOSPLATFORM
#include "EcOsPlatform.h"
#endif

#ifndef EC_FNNAME
#define EC_FNNAME EC_FNNAME_DEFAULT
#endif
#ifndef EC_FNCALL
#define EC_FNCALL /* as default, no explicit calling convention is given */
#endif
#ifndef EC_API
#define EC_API EC_API_DEFAULT
#endif
#ifndef EC_API_FNCALL
#define EC_API_FNCALL EC_FNCALL
#endif

#ifndef EC_ARCH
#define EC_ARCH EC_ARCH_UNDEFINED
#endif

#ifndef INC_ECFEATURES
#include "EcFeatures.h"
#endif

#ifndef INC_ECERROR
#include "EcError.h"
#endif

/* legacy. TODO: remove */
#ifndef ATECAT_API
#define ATECAT_API EC_API
#endif

/* TODO: remove */
#if 1
#if (defined OsSleep && !defined OsPlatformImplSleep && !defined EXCLUDE_OSSLEEP_PROTOTYPE)
#define EXCLUDE_OSSLEEP_PROTOTYPE
#endif

#ifndef OsPlatformMalloc
#define OsPlatformMalloc(nSize)                 malloc((size_t)(nSize))
#endif

#ifndef OsPlatformFree
#define OsPlatformFree(pvMem)                   free((void*)(pvMem))
#endif

#ifndef OsPlatformRealloc
#define OsPlatformRealloc(pMem,nSize)           realloc((void*)(pMem),(size_t)(nSize))
#endif

#ifndef OsPlatformMemoryBarrier
#define OsPlatformMemoryBarrier()               { EC_T_DWORD dwDummy = 0; __asm xchg dwDummy, eax }
#endif

#ifndef OsPlatformSetEvent
#define OsPlatformSetEvent(hEvent)              SetEvent((HANDLE)(hEvent))
#endif

#ifndef OsPlatformResetEvent
#define OsPlatformResetEvent(hEvent)            ResetEvent(hEvent)
#endif

#endif /* *** legacy code handling OS-Layer change in V3.0.0.13 *** */

/*****************************************************************************
 * OsPlatform instrumentation
 *****************************************************************************/
#if (defined INSTRUMENT_OS)

/* TODO: remove */
#ifdef __cplusplus
extern "C"
{
#endif

#include "EcOsInstr.h"

/* TODO: remove */
#ifdef __cplusplus
}
#endif

#else /* !INSTRUMENT_OS */
 /* API function naming */
#ifdef  OsPlatformImplMalloc
#undef  OsPlatformImplMalloc
#define OsMalloc                                OsMalloc
#endif
#define OsPlatformImplMalloc                    OsMalloc

#ifdef  OsPlatformImplFree
#undef  OsPlatformImplFree
#define OsFree                                  OsFree
#endif
#define OsPlatformImplFree                      OsFree

#ifdef  OsPlatformImplRealloc
#undef  OsPlatformImplRealloc                        
#define OsRealloc                               OsRealloc
#endif
#define OsPlatformImplRealloc                   OsRealloc

#ifdef  OsPlatformImplCreateThread
#undef  OsPlatformImplCreateThread
#ifndef OsCreateThread
#define OsCreateThread                          OsCreateThread
#endif
#endif
#define OsPlatformImplCreateThread              OsCreateThread

#ifdef  OsPlatformImplDeleteThreadHandle
#undef  OsPlatformImplDeleteThreadHandle
#define OsDeleteThreadHandle                    OsDeleteThreadHandle
#endif
#define OsPlatformImplDeleteThreadHandle        OsDeleteThreadHandle

#ifdef  OsPlatformImplGetLinkLayerRegFunc
#undef  OsPlatformImplGetLinkLayerRegFunc
#define OsGetLinkLayerRegFunc                   OsGetLinkLayerRegFunc
#endif
#define OsPlatformImplGetLinkLayerRegFunc       OsGetLinkLayerRegFunc

#ifdef  OsPlatformImplSleep
#undef  OsPlatformImplSleep
#define OsSleep                                 OsSleep
#endif
#define OsPlatformImplSleep                     OsSleep
#endif /* !INSTRUMENT_OS */

/* No OS specific includes files allowed */
#if (defined __cplusplus) && !(defined EC_NEW)
#include <new>
#endif
#include <limits.h>

/* TODO: remove */
#if 1
#ifdef __cplusplus
extern "C"
{
#endif
#endif

/*-DEFINES-------------------------------------------------------------------*/

/* *** EC_INT32_MAX, EC_INT64_MAX *** */
#if (!defined EC_INT32_MAX) && (defined LONG_MAX)
#define EC_INT32_MAX    LONG_MAX
#endif
#if (!defined EC_INT64_MAX) && (defined LLONG_MAX)
#define EC_INT64_MAX    LLONG_MAX
#endif
#if (!defined EC_INT64_MAX) && (defined LONG_LONG_MAX)
#define EC_INT64_MAX    LONG_LONG_MAX
#endif
#if (!defined EC_INT32_MAX) && (defined _I32_MAX)
#define EC_INT32_MAX    _I32_MAX
#endif
#if (!defined EC_INT64_MAX) && (defined _I64_MAX)
#define EC_INT64_MAX    _I64_MAX
#endif

/* *** Socket support *** */
#if (defined EC_SOCKET_IP_SUPPORTED) || (defined EC_SOCKET_MSGQUEUE_WIN32_SUPPORTED) || (defined EC_SOCKET_MSGQUEUE_RTOSSHM_SUPPORTED) || (defined EC_SOCKET_RTOSLIB_SUPPORTED)
#define EC_SOCKET_SUPPORTED
#endif

#if (defined EC_SOCKET_SUPPORTED)

/* Socket layer internal error code selection */
#ifndef EC_SOCKET_NOERROR
#define EC_SOCKET_NOERROR 0
#endif

#if !(defined EC_E_BSD_ENOTCONN) && (defined ENOTCONN)
#define EC_E_BSD_ENOTCONN        ENOTCONN
#endif
#if !(defined EC_E_BSD_ENOTSOCK) && (defined ENOTSOCK)
#define EC_E_BSD_ENOTSOCK        ENOTSOCK
#endif
#if !(defined EC_E_BSD_ESHUTDOWN) && (defined ESHUTDOWN)
#define EC_E_BSD_ESHUTDOWN       ESHUTDOWN
#endif
#if !(defined EC_E_BSD_EHOSTUNREACH) && (defined EHOSTUNREACH)
#define EC_E_BSD_EHOSTUNREACH    EHOSTUNREACH
#endif
#if !(defined EC_E_BSD_EINVAL) && (defined EINVAL)
#define EC_E_BSD_EINVAL          EINVAL
#endif
#if !(defined EC_E_BSD_EMSGSIZE) && (defined EMSGSIZE)
#define EC_E_BSD_EMSGSIZE        EMSGSIZE
#endif
#if !(defined EC_E_BSD_ECONNABORTED) && (defined ECONNABORTED)
#define EC_E_BSD_ECONNABORTED    ECONNABORTED
#endif
#if !(defined EC_E_BSD_ETIMEDOUT) && (defined ETIMEDOUT)
#define EC_E_BSD_ETIMEDOUT       ETIMEDOUT
#endif
#if !(defined EC_E_BSD_ECONNRESET) && (defined ECONNRESET)
#define EC_E_BSD_ECONNRESET      ECONNRESET
#endif
#if !(defined EC_E_BSD_EPIPE) && (defined EPIPE)
#define EC_E_BSD_EPIPE           EPIPE
#endif

#ifndef EC_INVALID_SOCKET
#define EC_INVALID_SOCKET INVALID_SOCKET
#endif

#ifndef EC_SOCKET_ERROR
#define EC_SOCKET_ERROR SOCKET_ERROR
#endif

#ifndef EC_T_SOCKET
#define EC_T_SOCKET EC_T_INT
#endif

#ifndef EC_SOMAXCONN
#define EC_SOMAXCONN SOMAXCONN
#endif

#ifndef OsInetAddr
#define OsInetAddr inet_addr
#endif
#endif /* EC_SOCKET_SUPPORTED */

/*-TYPEDEFS------------------------------------------------------------------*/
typedef EC_T_VOID (EC_FNCALL *EC_PF_OSTIMER)(EC_T_VOID);
typedef EC_T_BOOL (EC_FNCALL *EC_PF_OSDBGMSGHK)(const EC_T_CHAR* szFormat, EC_T_VALIST vaArgs);
typedef EC_T_VOID (EC_FNCALL *EC_PF_THREADENTRY)(EC_T_VOID* pvParams);
struct _EC_T_LOG_CONTEXT;
typedef EC_T_DWORD(EC_FNCALL *EC_PF_LOGMSGHK)(struct _EC_T_LOG_CONTEXT* pContext, EC_T_DWORD dwLogMsgSeverity, const EC_T_CHAR* szFormat, ...);

struct _EC_T_LINK_DRV_DESC;
typedef EC_T_DWORD (EC_FNCALL *EC_PF_LLREGISTER)(struct _EC_T_LINK_DRV_DESC* pLinkDrvDesc, EC_T_DWORD dwLinkDrvDescSize);
typedef EC_T_DWORD (EC_FNCALL *EC_PF_HW_TIMER_GET_INPUT_FREQUENCY)(EC_T_DWORD* pdwTimerInputFreq);
typedef EC_T_DWORD (EC_FNCALL *EC_PF_HW_TIMER_MODIFY_INITIAL_COUNT)(EC_T_INT nAdjustPermil);
typedef EC_T_DWORD (EC_FNCALL *EC_PF_HW_TIMER_GET_CURRENT_COUNT)(EC_T_INT* pnCountPermil);

/* SMP support */
#ifndef EC_CPUSET_DEFINED
typedef unsigned long   EC_T_CPUSET;        /* CPU-set for SMP systems */
#endif

typedef EC_T_DWORD (EC_FNCALL *EC_PF_SYSTIME)(EC_T_UINT64* pqwSystemTime);
typedef EC_T_DWORD (EC_FNCALL *EC_PF_QUERY_MSEC_COUNT)(EC_T_VOID);

#if (defined EC_SOCKET_SUPPORTED)
#ifndef EC_T_FD_SET
typedef struct fd_set EC_T_FD_SET;
#endif

#ifndef EC_T_TIMEVAL
typedef struct timeval EC_T_TIMEVAL;
#endif

#ifndef EC_T_SOCKADDR
typedef struct sockaddr EC_T_SOCKADDR;
#endif

#ifndef EC_T_SOCKADDR_IN
typedef struct sockaddr_in EC_T_SOCKADDR_IN;
#endif
#endif /* EC_SOCKET_SUPPORTED */

/* -PACKED STRUCTURES------------------------------------------------------- */
#ifndef EC_T_OS_PARMS
#include EC_PACKED_INCLUDESTART(8)
typedef struct _EC_T_OS_PARMS
{
#define EC_OS_PARMS_SIGNATURE_PATTERN     (EC_T_DWORD)0x5BA00000      /* Mask 0xfff00000 */
#define EC_OS_PARMS_SIGNATURE_VERSION     (EC_T_DWORD)0x00010000      /* Version 1, mask 0x000f0000 */
#define EC_OS_PARMS_SIGNATURE (EC_T_DWORD)(EC_OS_PARMS_SIGNATURE_PATTERN|EC_OS_PARMS_SIGNATURE_VERSION)
    EC_T_DWORD                          dwSignature;                  /*< [in] Set to EC_OS_PARMS_SIGNATURE */
    EC_T_DWORD                          dwSize;                       /*< [in] Set to sizeof(EC_T_OS_PARMS) */
    struct _EC_T_LOG_PARMS*             pLogParms;                    /*< [in] Log Parms */
    EC_PF_SYSTIME                       pfSystemTimeGet;              /*< [in] function to get host time in nanoseconds since 1st January 2000. Used as time base for DC Initialization. */
    EC_T_DWORD                          dwSupportedFeatures;          /*< [in/out] reserved */
    EC_PF_QUERY_MSEC_COUNT              pfSystemQueryMsecCount;       /*< [in] function to get system's msec count */
    EC_PF_HW_TIMER_GET_INPUT_FREQUENCY  pfHwTimerGetInputFrequency;   /*< [in] function to get input frequency of HW timer */
    EC_PF_HW_TIMER_MODIFY_INITIAL_COUNT pfHwTimerModifyInitialCount;  /*< [in] function to modify initial count of HW timer */
    EC_PF_HW_TIMER_GET_CURRENT_COUNT    pfHwTimerGetCurrentCount;
} EC_PACKED(8) EC_T_OS_PARMS;
#include EC_PACKED_INCLUDESTOP
#define EC_T_OS_PARMS EC_T_OS_PARMS
#endif

/*-MACROS--------------------------------------------------------------------*/
#ifndef ATECAT_PLATFORMSTR
#define ATECAT_PLATFORMSTR ""
#endif

/* *** EC_INLINE *** */
#ifndef EC_INLINEKEYWORD
#define EC_INLINEKEYWORD __inline
#endif
#ifndef EC_INLINEATTRIBUTE
#define EC_INLINEATTRIBUTE
#endif

#ifndef EC_INLINESTART
#define EC_INLINESTART __inline
#endif
#ifndef EC_INLINESTOP
#define EC_INLINESTOP
#endif

#ifndef EC_OVERRIDE
#define EC_OVERRIDE
#endif

#if (defined INSTRUMENT_OS) || (defined INSTRUMENT_MASTER) || (defined INSTRUMENT_LL)
#define EC_INSTRUMENT_MOCKABLE_FUNC virtual
#define EC_INSTRUMENT_MOCKABLE_VAR public:
#else
#define EC_INSTRUMENT_MOCKABLE_FUNC
#define EC_INSTRUMENT_MOCKABLE_VAR
#endif

#define     EC_OFFSETOF(s,m)    ((size_t)&(((s *)0)->m))

#define     EC_INTSIZEOF(n)     ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )
#ifndef EC_VASTART
#define     EC_VASTART(ap,v)    ( ap = (EC_T_VALIST)&v + EC_INTSIZEOF(v) )
#endif
#ifndef EC_VAARG
#define     EC_VAARG(ap,t)      ( *(t *)((ap += EC_INTSIZEOF(t)) - EC_INTSIZEOF(t)) )
#endif
#ifndef EC_VAEND
#define     EC_VAEND(ap)        ( ap = (EC_T_VALIST)0 )
#endif

#define     EC_UNREFPARM(p)     {(p)=(p);}
#define     EC_MAX(a,b)         (((a) > (b)) ? (a) : (b))
#define     EC_MIN(a,b)         (((a) < (b)) ? (a) : (b))
#define     EC_AT_LEAST         EC_MAX
#define     EC_AT_MOST          EC_MIN
#define     EC_ENDOF(p)         ((p)+1)
#define     EC_IS_RANGE_OUT_OF_BOUNDS(loRange, hiRange, loBound, hiBound) (((hiRange) <= (loBound)) || ((loRange) >= (hiBound)))

#define     SafeOsFree(p)       {if (EC_NULL!=(p)) {OsFree    (p); (p) = EC_NULL;}}
#ifndef SafeDelete
#define     SafeDelete(p)       {if (EC_NULL!=(p)) {delete    (p); (p) = EC_NULL;}}
#endif
#ifndef SafeDeleteArray
#define     SafeDeleteArray(p)  {if (EC_NULL!=(p)) {delete [] (p); (p) = EC_NULL;}}
#endif

/* SMP support */
#ifndef EC_CPUSET_DEFINED
/* This macros will be overloaded by the specific EcOsPlatform.h */
#define     EC_CPUSET_ZERO(CpuSet)          (CpuSet) = 0    /* clear all CPU indexes in the CPU set */
#define     EC_CPUSET_SET(CpuSet,nCpuIndex) (CpuSet) = (1<<(nCpuIndex))    /* set CPU index nCpuIndex (0..x) in the CPU set */
#define     EC_CPUSET_SETALL(CpuSet)        (CpuSet) = 0xFFFFFFFF    /* set all CPU indexes in the CPU set */
#endif

#define EC_BIT_MASK(Bitsize)                            ((1<<(Bitsize))-1)
#define EC_BITFIELD_MASK(Bitpos,Bitsize)                ((EC_BIT_MASK((Bitsize)))<<(Bitpos))
#define EC_RESET_WORD_IN_BITFIELD(wVal,Bitpos,Bitsize)  ((wVal)&(~EC_BITFIELD_MASK((Bitpos),(Bitsize))))
#define EC_SET_WORD_IN_BITFIELD(Bitpos,Bitsize,wVal)    (((wVal)&(EC_BIT_MASK((Bitsize))))<<(Bitpos))
#define EC_GET_WORD_IN_BITFIELD(Bitpos,Bitsize,wVal)    (((wVal)>>(Bitpos))&EC_BIT_MASK((Bitsize)))

#define EC_BYTE0(x)     (EC_T_BYTE)(((x) >>  0) & 0xff)
#define EC_BYTE1(x)     (EC_T_BYTE)(((x) >>  8) & 0xff)
#define EC_BYTE2(x)     (EC_T_BYTE)(((x) >> 16) & 0xff)
#define EC_BYTE3(x)     (EC_T_BYTE)(((x) >> 24) & 0xff)
#define EC_BYTE4(x)     (EC_T_BYTE)(((x) >> 32) & 0xff)
#define EC_BYTE5(x)     (EC_T_BYTE)(((x) >> 40) & 0xff)
#define EC_BYTE6(x)     (EC_T_BYTE)(((x) >> 48) & 0xff)
#define EC_BYTE7(x)     (EC_T_BYTE)(((x) >> 56) & 0xff)

#define EC_BYTEN(ptr, n) \
    ((EC_T_BYTE*)(ptr))[(n)]

#define EC_WORD0(x)     (((x) >>  0) & 0xffff)
#define EC_WORD1(x)     (((x) >> 16) & 0xffff)

#if (!defined EC_WORDSWAP)
#define EC_WORDSWAP(x)  (EC_T_WORD)( \
                        ((  (EC_T_WORD)EC_BYTE0((x))) <<  8) | \
                        ((  (EC_T_WORD)EC_BYTE1((x))) <<  0))
#endif

#if (!defined EC_DWORDSWAP)
#define EC_DWORDSWAP(x) (EC_T_DWORD)( \
                        (( (EC_T_DWORD)EC_BYTE0((x))) << 24) | \
                        (( (EC_T_DWORD)EC_BYTE1((x))) << 16) | \
                        (( (EC_T_DWORD)EC_BYTE2((x))) <<  8) | \
                        (( (EC_T_DWORD)EC_BYTE3((x))) <<  0))
#endif

#if (!defined EC_QWORDSWAP)
#define EC_QWORDSWAP(x) (EC_T_UINT64)( \
                        (((EC_T_UINT64)EC_BYTE0((x))) << 56) | \
                        (((EC_T_UINT64)EC_BYTE1((x))) << 48) | \
                        (((EC_T_UINT64)EC_BYTE2((x))) << 40) | \
                        (((EC_T_UINT64)EC_BYTE3((x))) << 32) | \
                        (((EC_T_UINT64)EC_BYTE4((x))) << 24) | \
                        (((EC_T_UINT64)EC_BYTE5((x))) << 16) | \
                        (((EC_T_UINT64)EC_BYTE6((x))) <<  8) | \
                        (((EC_T_UINT64)EC_BYTE7((x))) <<  0))
#endif

#if !(defined EC_REALSWAP)
    static EC_INLINESTART EC_T_REAL EC_REALSWAP_INLINE(EC_T_REAL fSrc)
    {
         EC_T_VOID* pvSrc = (EC_T_VOID*)&fSrc;
         EC_T_UINT64 qwHelper = EC_QWORDSWAP(*((EC_T_UINT64*)pvSrc));
         EC_T_VOID* pvDest = (EC_T_VOID*)&qwHelper;
         return *((EC_T_REAL*)pvDest);
    } EC_INLINESTOP
    #define EC_REALSWAP(fSrc) EC_REALSWAP_INLINE(fSrc)
#endif /* EC_REALSWAP */


#if !(defined EC_MAKEWORD)
#define EC_MAKEWORD(hi, lo)     ((EC_T_WORD )(((EC_T_BYTE)(lo)) | ((EC_T_WORD )((EC_T_BYTE)(hi))) <<  8))
#endif
#if !(defined EC_MAKEDWORD)
#define EC_MAKEDWORD(hi, lo)    ((EC_T_DWORD)(((EC_T_WORD)(lo)) | ((EC_T_DWORD)((EC_T_WORD)(hi))) << 16))
#endif
#if !(defined EC_MAKEQWORD)
#define EC_MAKEQWORD(hi, lo)    ((EC_T_UINT64)(((EC_T_DWORD)(lo)) | ((EC_T_UINT64)((EC_T_DWORD)(hi))) << 32))
#endif

#if !(defined EC_LODWORD)
#define EC_LODWORD(qw)          ((EC_T_DWORD)((EC_T_UINT64)(qw) & 0xFFFFFFFF))
#endif
#if !(defined EC_HIDWORD)
#define EC_HIDWORD(qw)          ((EC_T_DWORD)(((EC_T_UINT64)(qw) >> 32) & 0xFFFFFFFF))
#endif

#if !(defined EC_LOWORD)
#define EC_LOWORD(dw)           ((EC_T_WORD)((dw) & 0xFFFF))
#endif
#if !(defined EC_HIWORD)
#define EC_HIWORD(dw)           ((EC_T_WORD)(((EC_T_DWORD)(dw) >> 16) & 0xFFFF))
#endif

#if !(defined EC_LOBYTE)
#define EC_LOBYTE(w)            ((EC_T_BYTE)((w) & 0xFF))
#endif
#if !(defined EC_HIBYTE)
#define EC_HIBYTE(w)            ((EC_T_BYTE)(((EC_T_WORD)(w)   >>  8) &   0xFF))
#endif


/* memory access */
#if (defined WITHALIGNMENT)
    #if !(defined EC_SETWORD)
    #define EC_SETWORD(ptr, val) \
    {   \
        EC_BYTEN((ptr), 0) = EC_BYTE0(((EC_T_WORD)val)); \
        EC_BYTEN((ptr), 1) = EC_BYTE1(((EC_T_WORD)val)); \
    }
    #endif

    #if !(defined EC_SETDWORD)
    /* little endian! LSB on [0], MSB on [3] */
    #define EC_SETDWORD(ptr, val) \
    {   \
        EC_BYTEN((ptr), 0) = EC_BYTE0(((EC_T_DWORD)val));  \
        EC_BYTEN((ptr), 1) = EC_BYTE1(((EC_T_DWORD)val));  \
        EC_BYTEN((ptr), 2) = EC_BYTE2(((EC_T_DWORD)val));  \
        EC_BYTEN((ptr), 3) = EC_BYTE3(((EC_T_DWORD)val));  \
    }
    #endif

    #if !(defined EC_SETQWORD)
    #define EC_SETQWORD(ptr, val)   \
    {   \
        EC_BYTEN((ptr), 0) = EC_BYTE0(((EC_T_UINT64)val));  \
        EC_BYTEN((ptr), 1) = EC_BYTE1(((EC_T_UINT64)val));  \
        EC_BYTEN((ptr), 2) = EC_BYTE2(((EC_T_UINT64)val));  \
        EC_BYTEN((ptr), 3) = EC_BYTE3(((EC_T_UINT64)val));  \
        EC_BYTEN((ptr), 4) = EC_BYTE4(((EC_T_UINT64)val));  \
        EC_BYTEN((ptr), 5) = EC_BYTE5(((EC_T_UINT64)val));  \
        EC_BYTEN((ptr), 6) = EC_BYTE6(((EC_T_UINT64)val));  \
        EC_BYTEN((ptr), 7) = EC_BYTE7(((EC_T_UINT64)val));  \
    }
    #endif

    #if !(defined EC_GETWORD)
    #define EC_GETWORD(ptr) (EC_T_WORD) \
            ((EC_BYTEN((ptr), 0) <<  0) | \
             (EC_BYTEN((ptr), 1) <<  8)   \
            )
    #endif

    #if !(defined EC_GETDWORD)
    #define EC_GETDWORD(ptr) (EC_T_DWORD) \
            ((EC_BYTEN((ptr), 0) <<  0) | \
             (EC_BYTEN((ptr), 1) <<  8) | \
             (EC_BYTEN((ptr), 2) << 16) | \
             (EC_BYTEN((ptr), 3) << 24)   \
            )
    #endif

    #if !(defined EC_GETQWORD)
    #define EC_GETQWORD(ptr) (EC_T_UINT64) \
            ((((EC_T_UINT64)(EC_BYTEN((ptr), 0))) <<  0) | \
             (((EC_T_UINT64)(EC_BYTEN((ptr), 1))) <<  8) | \
             (((EC_T_UINT64)(EC_BYTEN((ptr), 2))) << 16) | \
             (((EC_T_UINT64)(EC_BYTEN((ptr), 3))) << 24) | \
             (((EC_T_UINT64)(EC_BYTEN((ptr), 4))) << 32) | \
             (((EC_T_UINT64)(EC_BYTEN((ptr), 5))) << 40) | \
             (((EC_T_UINT64)(EC_BYTEN((ptr), 6))) << 48) | \
             (((EC_T_UINT64)(EC_BYTEN((ptr), 7))) << 56)   \
            )
    #endif

#else
    #if !(defined EC_SETWORD)
    static EC_INLINESTART EC_T_VOID EC_SETWORD(EC_T_VOID* pvAddress, EC_T_WORD wVal)
    {
        /* alignment does not need to be consired on x86 */
        *(EC_T_WORD*)pvAddress = wVal;
    } EC_INLINESTOP
    #endif
    #if !(defined EC_SETDWORD)
    static EC_INLINESTART EC_T_VOID EC_SETDWORD(EC_T_VOID* pvAddress, EC_T_DWORD dwVal)
    {
        /* alignment does not need to be consired on x86 */
        *(EC_T_DWORD*)pvAddress = dwVal;
    } EC_INLINESTOP
    #endif

    #if !(defined EC_GETWORD)
    static EC_INLINESTART EC_T_WORD EC_GETWORD(EC_T_VOID* pvAddress)
    {
        /* alignment does not need to be consired on x86 */
        return *(EC_T_WORD*)pvAddress;
    } EC_INLINESTOP
    #endif
    #if !(defined EC_GETDWORD)
    static EC_INLINESTART EC_T_DWORD EC_GETDWORD(EC_T_VOID* pvAddress)
    {
        /* alignment does not need to be consired on x86 */
        return *(EC_T_DWORD*)pvAddress;
    } EC_INLINESTOP
    #endif


    #ifdef QWORD_WITHALIGNMENT
        #if !(defined EC_SETQWORD)
        static EC_INLINESTART EC_T_VOID EC_SETQWORD(EC_T_VOID* pvAddress, EC_T_UINT64 qwVal)
        {
            ((EC_T_DWORD*)pvAddress)[0] = EC_LODWORD(qwVal);
            ((EC_T_DWORD*)pvAddress)[1] = EC_HIDWORD(qwVal);
        } EC_INLINESTOP
        #endif
        #if !(defined EC_GETQWORD)
        static EC_INLINESTART EC_T_UINT64 EC_GETQWORD(EC_T_VOID* pvAddress)
        {
            return EC_MAKEQWORD(((EC_T_DWORD*)pvAddress)[1], ((EC_T_DWORD*)pvAddress)[0]);
        } EC_INLINESTOP
        #endif
    #else
        #if !(defined EC_SETQWORD)
        static EC_INLINESTART EC_T_VOID EC_SETQWORD(EC_T_VOID* pvAddress, EC_T_UINT64 qwVal)
        {
            /* alignment does not need to be consired on x86 */
            *(EC_T_UINT64*)pvAddress = qwVal;
        } EC_INLINESTOP
        #endif
        #if !(defined EC_GETQWORD)
        static EC_INLINESTART EC_T_UINT64 EC_GETQWORD(EC_T_VOID* pvAddress)
        {
            /* alignment does not need to be consired on x86 */
            return *(EC_T_UINT64*)pvAddress;
        } EC_INLINESTOP
        #endif
    #endif
#endif

#if !(defined EC_SETBOOL)
#define EC_SETBOOL(ptr, val)    EC_SETDWORD(ptr, (EC_T_DWORD)val)
#endif
#if !(defined EC_GETBOOL)
#define EC_GETBOOL(ptr)         ((EC_T_BOOL)(EC_GETDWORD(ptr))!=EC_FALSE)
#endif

static EC_INLINESTART EC_T_VOID EC_COPYBITS_INLINE(EC_T_BYTE* pbyDst, EC_T_INT nDstBitOffs, EC_T_BYTE* pbySrc, EC_T_INT nSrcBitOffs, EC_T_INT nBitSize)
{
    EC_T_INT    nRemLen = nBitSize;            /* remaining length */
    EC_T_BYTE   byMask;
    EC_T_INT    nNumBits = 0;
    EC_T_BYTE*  pSrcHelp = EC_NULL;
    EC_T_WORD   wSrcWork;

    if (nBitSize == 0)
    {
        return;
    }
    /* how many bits we need for the next "destination" byte */
    nNumBits = EC_MIN((8 - (nDstBitOffs & 7)), nRemLen);

    byMask   = (EC_T_BYTE)((1 << nNumBits) - 1);
    byMask   = (EC_T_BYTE)(byMask<<(nDstBitOffs & 7));

    /* copy first byte */
    pSrcHelp = &pbySrc[nSrcBitOffs / 8];
    if ((nSrcBitOffs & 7) + nBitSize <= 8)
    {
        wSrcWork = (EC_T_WORD)pSrcHelp[0];
    }
    else
    {
        wSrcWork = (EC_T_WORD)((EC_T_WORD)pSrcHelp[0] | (((EC_T_WORD)pSrcHelp[1]) << 8));   /* xxxx xxxx xx-- ----  e. g. nSrcBitOffs=6, nDstBitOffs=2 */
    }
    wSrcWork = (EC_T_WORD)(wSrcWork >> (nSrcBitOffs & 7));                                  /* 0000 00xx xxxx xxxx */
    wSrcWork = (EC_T_WORD)(wSrcWork & ((1 << nNumBits) - 1));

    pbyDst   = &pbyDst[nDstBitOffs/8];
    *pbyDst  = (EC_T_BYTE)((*pbyDst & ~byMask) | (wSrcWork<<(nDstBitOffs & 7)));
    pbyDst++;
    
    nSrcBitOffs = nSrcBitOffs + nNumBits;
    nRemLen -= nNumBits;

    while (nRemLen > 0)
    {
        nNumBits  = EC_MIN(8, nRemLen);

        byMask    = (EC_T_BYTE)((1 << nNumBits) - 1);

        pSrcHelp = &pbySrc[nSrcBitOffs / 8];
        wSrcWork = (EC_T_WORD)(pSrcHelp[0] | (pSrcHelp[1] << 8));
        wSrcWork = (EC_T_WORD)(wSrcWork >> (nSrcBitOffs & 7));
        wSrcWork = (EC_T_WORD)(wSrcWork & (EC_T_WORD)byMask);

        *pbyDst = (EC_T_BYTE)((*pbyDst & ~byMask) | (wSrcWork));
        pbyDst++;

        nSrcBitOffs = nSrcBitOffs + nNumBits;
        nRemLen -= nNumBits;
    }
} EC_INLINESTOP
#if !(defined EC_COPYBITS)
    #define EC_COPYBITS(pbyDst, nDstBitOffs, pbySrc, nSrcBitOffs, nBitSize) \
        EC_COPYBITS_INLINE(pbyDst, nDstBitOffs, pbySrc, nSrcBitOffs, nBitSize)
#endif /* EC_COPYBITS */

#ifndef OsCopyBitsPdIn
#define OsCopyBitsPdIn          EC_COPYBITS
#endif

#ifndef OsCopyBitsPdOut
#define OsCopyBitsPdOut         EC_COPYBITS
#endif

#if !(defined EC_SETBIT)
    #define EC_SETBIT(pbyBuf, nBitOffs) pbyBuf[nBitOffs/8] |= (1<<(nBitOffs%8))
#endif

#if !(defined EC_CLRBIT)
    #define EC_CLRBIT(pbyBuf, nBitOffs) pbyBuf[nBitOffs/8] &= ~(1<<(nBitOffs%8))
#endif

#if !(defined EC_TESTBIT)
#define EC_TESTBIT(pbyBuf, nBitOffs) ((pbyBuf[nBitOffs/8] & (1<<(nBitOffs%8))) ? EC_TRUE : EC_FALSE)
#endif


#if !(defined EC_SETBITS)
    #define EC_SETBITS(pbyDstBuf, pbySrcData, nDstBitOffs, nBitSize) \
        EC_COPYBITS(pbyDstBuf, nDstBitOffs, pbySrcData, 0, nBitSize)
#endif

#if !(defined EC_GETBITS)
    #define EC_GETBITS(pbySrcBuf, pbyDstData, nSrcBitOffs, nBitSize) \
        EC_COPYBITS(pbyDstData, 0, pbySrcBuf, nSrcBitOffs, nBitSize)
#endif

static EC_INLINESTART EC_T_BYTE EcBoolToByte(EC_T_BOOL bValue) { return (EC_T_BYTE)(bValue ? 1 : 0);} EC_INLINESTOP
static EC_INLINESTART EC_T_BOOL EcByteToBool(EC_T_BYTE bValue) { return bValue == 1;} EC_INLINESTOP
static EC_INLINESTART EC_T_DWORD EcPtrToDword(EC_T_VOID* pvVal) {return (EC_T_DWORD)((EC_T_BYTE*)pvVal-(EC_T_BYTE*)EC_NULL);} EC_INLINESTOP

#if (defined EC_BIG_ENDIAN)
#define EC_NO_BITFIELDS /* big endian: do not use bitfields! */
#define EC_GET_FRM_BOOL(ptr)        EC_DWORDSWAP(EC_GETDWORD((ptr)))
#define EC_GET_FRM_WORD(ptr)        EC_WORDSWAP( EC_GETWORD((ptr)))
#define EC_GET_FRM_DWORD(ptr)       EC_DWORDSWAP(EC_GETDWORD((ptr)))
#define EC_GET_FRM_QWORD(ptr)       EC_QWORDSWAP(EC_GETQWORD((ptr)))

#define EC_SET_FRM_BOOL(ptr, dw)    EC_SETDWORD((ptr), EC_DWORDSWAP((dw)))
#define EC_SET_FRM_WORD(ptr, w)     EC_SETWORD((ptr),  EC_WORDSWAP((w)))
#define EC_SET_FRM_DWORD(ptr, dw)   EC_SETDWORD((ptr), EC_DWORDSWAP((dw)))
#define EC_SET_FRM_QWORD(ptr, qw)   EC_SETQWORD((ptr), EC_QWORDSWAP((qw)))

#define EC_GET_FRM_WORD_BITFIELD(Bitpos,Bitsize,wVal)           EC_GET_WORD_IN_BITFIELD((Bitpos),(Bitsize),EC_WORDSWAP((wVal)))
#define EC_SET_FRM_WORD_BITFIELD(wVal,wNewVal,Bitpos,Bitsize)   {(wVal) = ((wVal) & ~EC_WORDSWAP(EC_BITFIELD_MASK((Bitpos),(Bitsize)))) | (EC_WORDSWAP((wNewVal)<<(Bitpos)));}
#else /* !EC_BIG_ENDIAN */
#define EC_GET_FRM_BOOL(ptr)        EC_GETBOOL((ptr))
#define EC_GET_FRM_WORD(ptr)        EC_GETWORD((ptr))
#define EC_GET_FRM_DWORD(ptr)       EC_GETDWORD((ptr))
#define EC_GET_FRM_QWORD(ptr)       EC_GETQWORD((ptr))

#define EC_SET_FRM_BOOL(ptr,b)      EC_SETBOOL((ptr),(b))
#define EC_SET_FRM_WORD(ptr,w)      EC_SETWORD((ptr),(w))
#define EC_SET_FRM_DWORD(ptr,dw)    EC_SETDWORD((ptr),(dw))
#define EC_SET_FRM_QWORD(ptr,qw)    EC_SETQWORD((ptr),(qw))

#define EC_GET_FRM_WORD_BITFIELD(Bitpos,Bitsize,wVal)           EC_GET_WORD_IN_BITFIELD((Bitpos),(Bitsize),(wVal))
#define EC_SET_FRM_WORD_BITFIELD(wVal,wNewVal,Bitpos,Bitsize)   {(wVal) = (EC_T_WORD)(((wVal) & ~EC_BITFIELD_MASK((Bitpos),(Bitsize))) | ((wNewVal)<<(Bitpos)));}
#endif /* !EC_BIG_ENDIAN */

/* *** EC_NTOHS, EC_HTONS *** */
/* convert values between host and network byte order
 * On the i386 the host byte order is Least Significant Byte first,
 * whereas the network byte order, as used on the Internet, is Most Significant Byte first.
 */
#if (defined EC_BIG_ENDIAN)
#define EC_HTONS(w)                 (w)
#define EC_NTOHS(w)                 (w)
#define EC_HTONL(dw)                (dw)
#define EC_NTOHL(dw)                (dw)
#define EC_HTONLL(qw)               (qw)
#define EC_NTOHLL(qw)               (qw)
#else
#define EC_HTONS(w)                 EC_WORDSWAP((w))
#define EC_NTOHS(w)                 EC_WORDSWAP((w))
#define EC_HTONL(dw)                EC_DWORDSWAP((dw))
#define EC_NTOHL(dw)                EC_DWORDSWAP((dw))
#define EC_HTONLL(qw)               EC_QWORDSWAP((qw))
#define EC_NTOHLL(qw)               EC_QWORDSWAP((qw))
#endif

/*-FUNCTION DECLARATIONS-----------------------------------------------------*/
EC_API EC_T_DWORD EC_API_FNCALL OsInit(EC_T_OS_PARMS* pOsParms);
EC_API EC_T_DWORD EC_API_FNCALL OsDeinit(EC_T_VOID);

#ifndef OsMemset
#define OsMemset(pDest,nVal,nSize)              memset((void*)(pDest),(int)(nVal),(size_t)(nSize))
#endif

#ifndef OsMemsetPdIn
#define OsMemsetPdIn                            OsMemset
#endif

#ifndef OsMemsetPdOut
#define OsMemsetPdOut                           OsMemset
#endif

#ifndef OsMemcpy
#define OsMemcpy(pDest,pSrc,nSize)              memcpy((void*)(pDest),(const void*)(pSrc),(size_t)(nSize))
#endif

#ifndef OsMemcpyPdIn
#define OsMemcpyPdIn                            OsMemcpy
#endif

#ifndef OsMemcpyPdOut
#define OsMemcpyPdOut                           OsMemcpy
#endif

#ifndef OsMemcpyMasterRedPdIn
#define OsMemcpyMasterRedPdIn                   OsMemcpyPdIn
#endif

#ifndef OsMemcpyMasterRedPdOut
#define OsMemcpyMasterRedPdOut                  OsMemcpyPdOut
#endif

#ifndef OsMemcmp
#define OsMemcmp(pBuf1,pBuf2,nSize)             memcmp((void*)(pBuf1),(const void*)(pBuf2),(size_t)(nSize))
#endif

#ifndef OsMemmove
#define OsMemmove(pDest,pSrc,nSize)             memmove((void*)(pDest),(const void*)(pSrc),(size_t)(nSize))
#endif

#ifndef OsStrlen
#define OsStrlen(szString)                      strlen((const char*)(szString))
#endif

#ifndef OsStrcpy
#define OsStrcpy(szDest,szSrc)                  strcpy((char*)(szDest),(const char*)(szSrc))
#endif

#ifndef OsStrncpy
#define OsStrncpy(szDest,szSrc,nSize)           strncpy((char*)(szDest),(const char*)(szSrc),(size_t)(nSize))
#endif

#ifndef OsStrcmp
#define OsStrcmp(szStr1,szStr2)                 strcmp((const char*)(szStr1),(const char*)(szStr2))
#endif

#ifndef OsStrncmp
#define OsStrncmp(szStr1,szStr2, nSize)         strncmp((const char*)(szStr1),(const char*)(szStr2), (size_t)(nSize))
#endif

#ifndef OsStricmp
#define OsStricmp(szStr1,szStr2)                stricmp((const char*)(szStr1),(const char*)(szStr2))
#endif

#ifndef OsStrtok
#define OsStrtok(szToken,szDelimit)             strtok((char*)(szToken), (const char*)(szDelimit))
#endif

#ifndef OsStrtol
#define OsStrtol(szToken,ppEnd,nRadix)          strtol((const char*)(szToken), (ppEnd), (nRadix))
#endif

#ifndef OsAtoi
#define OsAtoi(szString)                        atoi((const char*)(szString))
#endif

#ifndef OsStrtoul
#define OsStrtoul(szString,ptr,base)            strtoul((const char*)(szString), (ptr), (base))
#endif

#ifndef OsPrintf
#define OsPrintf                                printf
#endif

#ifndef OsVprintf
#define OsVprintf                               vprintf
#endif

#ifndef OsVsnprintf
#define OsVsnprintf                             EcVsnprintf
#endif

#ifndef OsSnprintf
/** \def OsSnprintf(EC_T_CHAR* szDest, EC_T_DWORD dwSize, const EC_T_CHAR* szFormat, ...)
    \brief An macro for platform-abstracted snprintf(). 
    \param szDest buffer to write to
    \param dwSize max bytes to print
    \param szFormat format text to print
    \param ... variable args to be formatted printed.
    \return Length of string (without terminating zero character).
    Securely prints given parameters formatted to buffer.
*/
#define OsSnprintf                              EcSnprintf
#endif

#ifndef OsFopen
#define OsFopen                                 fopen
#endif

#ifndef OsFclose
#define OsFclose(pFile)                         fclose(((FILE*)pFile))
#endif

#ifndef OsFwrite
#define OsFwrite                                fwrite
#endif

#ifndef OsFread
#define OsFread(pDstBuf,dwElemSize,dwCnt,hFile) fread((pDstBuf),(dwElemSize),(dwCnt),((FILE*)hFile))
#endif

#ifndef OsFflush
#define OsFflush                                fflush
#endif

/* this function currently is only used in the MotionDemo application, see also AtXmlParser.cpp */
/*
#ifndef OsGetFileSize
EC_T_INLINE(EC_T_DWORD OsGetFileSize(FILE* fp)) {long l;fseek((fp),0,SEEK_END);l=ftell((fp));fseek((fp),0,SEEK_SET);return l;}
#endif
*/

#ifndef OsDbgAssert
#ifdef INCLUDE_ASSERT_FUNC
EC_FNNAME EC_T_VOID EC_FNCALL         OsDbgAssertFunc(EC_T_BOOL bAssertCondition, const EC_T_CHAR* szFile, EC_T_DWORD dwLine);
#define OsDbgAssert(bAssertCondition) OsDbgAssertFunc((bAssertCondition),(__FILE__), (__LINE__))
#else /* !INCLUDE_ASSERT_FUNC */
#ifdef DEBUG
#define OsDbgAssert                             assert
#else /* !DEBUG */
#define OsDbgAssert(x)
#endif /* !DEBUG */
#endif /* !INCLUDE_ASSERT_FUNC */
#endif /* OsDbgAssert */

#ifndef OsSetLastError
#define OsSetLastError(dwError)                 dwError
#endif

#ifndef OsReleaseLinkLayerRegFunc
#define OsReleaseLinkLayerRegFunc(szDriverIdent)
#endif

#ifdef __cplusplus
#ifndef EC_NEW
#define EC_NEW(x)    new (std::nothrow) x
#endif
#endif /* __cplusplus */

EC_FNNAME EC_T_VOID* EC_FNCALL OsCfgFileOpen(const EC_T_CHAR* szCfgFileName);
EC_FNNAME EC_T_INT   EC_FNCALL OsCfgFileClose(EC_T_VOID* pvCfgFile);
EC_FNNAME EC_T_INT   EC_FNCALL OsCfgFileRead(EC_T_VOID* pvCfgFile, EC_T_VOID* pvDst, EC_T_INT nLen);
EC_FNNAME EC_T_INT   EC_FNCALL OsCfgFileError(EC_T_VOID* pvCfgFile);
EC_FNNAME EC_T_INT   EC_FNCALL OsCfgFileEof(EC_T_VOID* pvCfgFile);

/* these functions are actually part of the master core */
EC_API  EC_T_INT   EC_API_FNCALL EcVsnprintf(EC_T_CHAR* szDest, EC_T_INT nMaxSize, const EC_T_CHAR* szFormat, EC_T_VALIST vaList);
EC_API  EC_T_INT   EC_API_FNCALL EcSnprintf(EC_T_CHAR* szDest, EC_T_INT nMaxSize, const EC_T_CHAR* szFormat, ...);

#ifndef OsQueryMsecCount
EC_API  EC_T_DWORD EC_API_FNCALL OsQueryMsecCount(EC_T_VOID);
#endif
#ifndef EXCLUDE_OSSLEEP_PROTOTYPE
EC_API  EC_T_VOID  EC_API_FNCALL OsPlatformImplSleep(EC_T_DWORD dwMsec);
#endif

/***************************/
/* performance measurement */
/***************************/
/* timestamp counter interface for performance measurements (main functions are part of the master core) */
#ifndef INCLUDE_OS_PLATFORM_TSC_SUPPORT

/* if platform does not have specific TSC support, check for defaults if it is yet available */
#if (defined UNDER_CE && defined _M_IX86) || (defined RTOS_32) || (defined VXWORKS && defined CPU && ((CPU==PENTIUM) || (CPU==PENTIUM4))) || (defined EC_VERSION_QNX) || ((defined WIN32) && !(defined UNDER_CE)) || ((defined LINUX) && (defined __i386__)) || defined __INTIME__
#define INCLUDE_OS_PLATFORM_TSC_SUPPORT
#define INCLUDE_PENTIUM_TSC
#endif
#if (defined __RCX__)
#define INCLUDE_ARM_TSC
#endif

#endif

/* TODO: EC_T_TSC_FNMESSAGE */
typedef EC_T_VOID (EC_FNCALL *EC_T_FNMESSAGE) (EC_T_CHAR* szMsg);

/* performance measurement descriptors */
#include EC_PACKED_INCLUDESTART(8)
typedef struct _EC_T_TSC_TIME
{
    EC_T_UINT64     qwStart;            /* start time */
    EC_T_UINT64     qwEnd;              /* end time */
    EC_T_DWORD      dwCurr;             /* [1/10   usec] */
    EC_T_DWORD      dwMin;              /* [1/10   usec] */
    EC_T_DWORD      dwMax;              /* [1/10   usec] */
    EC_T_DWORD      dwAvg;              /* [1/1600 usec] */
    EC_T_BOOL       bMeasReset;         /* EC_TRUE if measurement values shall be reset */
    EC_T_INT        nIntLevel;          /* for interrupt lockout handling */
} EC_PACKED(8) EC_T_TSC_TIME;
#include EC_PACKED_INCLUDESTOP

#include EC_PACKED_INCLUDESTART(8)
typedef struct _EC_T_TSC_MEAS_DESC
{
    EC_T_TSC_TIME*  aTscTime;           /* timestamp info array */
    EC_T_DWORD      dwNumMeas;          /* number of elements in aTscTime */
    EC_T_BOOL       bMeasEnabled;       /* EC_TRUE if measurement is enabled */
    EC_T_VOID*      pPrivateData;       /* OS layer private data */
    EC_T_FNMESSAGE  pfnMessage;         /* Function pointer for message drop */
} EC_PACKED(8) EC_T_TSC_MEAS_DESC;
#include EC_PACKED_INCLUDESTOP

#ifndef OsTscMeasDisableIrq
/* default macros to enable/disable interrupts */
#  if (defined UNDER_CE) && (defined _M_IX86)
#    define OsTscMeasDisableIrq(pTscMeasDesc,dwIndex)  { __asm  cli };
#    define OsTscMeasEnableIrq(pTscMeasDesc,dwIndex)   { __asm  sti };
#  elif (defined VXWORKS) && (!defined __RTP__)
#      include "intLib.h"
     /* Some customers don't define _WRS_VX_SMP, but using an SMP kernel.
      * intCpuLock() should also work with the UP kernel (at least for versions > 6) 
      * and will then fall back to intLock().
      */
#    if ( defined(_WRS_VXWORKS_MAJOR) && ((_WRS_VXWORKS_MAJOR == 6) && (_WRS_VXWORKS_MINOR >= 6)) \
         || (_WRS_VXWORKS_MAJOR > 6) ) || defined(_WRS_VX_SMP)
#      define OsTscMeasDisableIrq(pTscMeasDesc,dwIndex)  { pTscMeasDesc->aTscTime[dwIndex].nIntLevel = intCpuLock(); };
#      define OsTscMeasEnableIrq(pTscMeasDesc,dwIndex)   { intCpuUnlock(pTscMeasDesc->aTscTime[dwIndex].nIntLevel); };
#    else
#      define OsTscMeasDisableIrq(pTscMeasDesc,dwIndex)  { pTscMeasDesc->aTscTime[dwIndex].nIntLevel = intLock(); };
#      define OsTscMeasEnableIrq(pTscMeasDesc,dwIndex)   { intUnlock(pTscMeasDesc->aTscTime[dwIndex].nIntLevel); };
#    endif /* if ((defined _WRS_VXWORKS_MAJOR) && (_WRS_VXWORKS_MAJOR >= 6)) */
#  elif (defined EC_VERSION_QNX)
#    define OsTscMeasDisableIrq(pTscMeasDesc,dwIndex)  { ThreadCtl( _NTO_TCTL_IO, 0 );InterruptDisable(); };
#    define OsTscMeasEnableIrq(pTscMeasDesc,dwIndex)   { ThreadCtl( _NTO_TCTL_IO, 0 );InterruptEnable(); };
#  elif (defined RTOS_32)
#    define OsTscMeasDisableIrq(pTscMeasDesc,dwIndex)  { __asm  {cli }};
#    define OsTscMeasEnableIrq(pTscMeasDesc,dwIndex)   { __asm  {sti }};
#  else
#    define OsTscMeasDisableIrq(pTscMeasDesc,dwIndex)
#    define OsTscMeasEnableIrq(pTscMeasDesc,dwIndex)
#  endif
#endif /* ifndef OsTscMeasDisableIrq */

EC_API EC_T_VOID   EC_API_FNCALL OsMeasCalibrate(EC_T_UINT64 dwlFreqSet);
EC_API EC_T_DWORD  EC_API_FNCALL OsMeasGet100kHzFrequency(EC_T_VOID);
EC_API EC_T_UINT64 EC_API_FNCALL OsMeasGetCounterTicks(EC_T_VOID);

/* optional: redirect trace messages into OS specific function, e.g. to store trace log into file 
 * default: print trace log as debug message
 */
#ifndef OsTrcMsg
#define     OsTrcMsg    EC_DBGMSG
#endif

EC_API EC_T_VOID*  EC_API_FNCALL OsCreateLock(EC_T_VOID);
EC_API EC_T_VOID*  EC_API_FNCALL OsCreateLockTyped(EC_T_OS_LOCK_TYPE   eLockType);
EC_API EC_T_VOID   EC_API_FNCALL OsDeleteLock(EC_T_VOID* pvLock);
EC_API EC_T_VOID   EC_API_FNCALL OsLock(EC_T_VOID* pvLock);
EC_API EC_T_VOID   EC_API_FNCALL OsUnlock(EC_T_VOID* pvLock);

EC_API EC_T_VOID*  EC_API_FNCALL OsCreateEvent(EC_T_VOID);
EC_API EC_T_VOID   EC_API_FNCALL OsDeleteEvent(EC_T_VOID* pvEvent);
#ifndef OsWaitForEvent
EC_API EC_T_DWORD  EC_API_FNCALL OsWaitForEvent(EC_T_VOID* pvEvent, EC_T_DWORD dwTimeout);
#endif
EC_API EC_T_VOID   EC_API_FNCALL OsPlatformImplSetEvent(EC_T_VOID* pvEvent);
EC_API EC_T_VOID   EC_API_FNCALL OsPlatformImplResetEvent(EC_T_VOID* pvEvent);

EC_API EC_T_VOID*  EC_API_FNCALL OsPlatformImplCreateThread(EC_T_CHAR* szThreadName, EC_PF_THREADENTRY pfThreadEntry, EC_T_DWORD dwPrio, EC_T_DWORD dwStackSize, EC_T_VOID* pvParams);
EC_API EC_T_VOID   EC_API_FNCALL OsPlatformImplDeleteThreadHandle(EC_T_VOID* pvThreadObject);

#ifndef OsSetThreadPriority
EC_API EC_T_VOID   EC_API_FNCALL OsSetThreadPriority( EC_T_VOID* pvThreadObject, EC_T_DWORD dwPrio);
#endif
#ifndef OsSetThreadAffinity
EC_API EC_T_BOOL   EC_API_FNCALL OsSetThreadAffinity( EC_T_VOID* pvThreadObject, EC_T_CPUSET CpuSet);
#endif
#ifndef OsGetThreadAffinity
EC_FNNAME EC_T_BOOL EC_FNCALL OsGetThreadAffinity( EC_T_VOID* pvThreadObject, EC_T_CPUSET* pCpuSet);
#endif

EC_API EC_PF_LLREGISTER EC_API_FNCALL OsPlatformImplGetLinkLayerRegFunc(EC_T_CHAR* szDriverIdent);

EC_API EC_T_DWORD EC_API_FNCALL OsSystemTimeGet(EC_T_UINT64* pqwSystemTime);

#ifndef OsForceThreadPrioNormal
EC_API EC_T_DWORD EC_API_FNCALL OsForceThreadPrioNormal(EC_T_VOID);
#endif


#if (defined EC_SOCKET_IP_SUPPORTED)

#ifndef EC_FD_SET
#define EC_FD_SET FD_SET 
#endif

#ifndef EC_FD_ZERO
#define EC_FD_ZERO FD_ZERO
#endif

#ifndef EC_FD_ISSET
#define EC_FD_ISSET FD_ISSET
#endif

#ifndef OsSocketInit
EC_API EC_T_DWORD EC_API_FNCALL OsSocketInit(EC_T_VOID);
#endif
#ifndef OsSocketDeInit
EC_API EC_T_DWORD EC_API_FNCALL OsSocketDeInit(EC_T_VOID);
#endif

#ifndef OsSocket
#define OsSocket(nAddrFamily, nSockType, nProtocol) socket(nAddrFamily, nSockType, nProtocol)
#endif

#ifndef OsSocketBind
#define OsSocketBind(hSockHandle, oSockAddr, nSockAddrLen) bind(hSockHandle, oSockAddr, nSockAddrLen)
#endif

#ifndef OsSocketListen
#define OsSocketListen(hSockHandle, nBacklog) listen(hSockHandle, nBacklog)
#endif

#ifndef OsSocketSelect
#define OsSocketSelect(nNfds, poReadFds, poWriteFds, poExceptFds, poTimeout) select(nNfds, poReadFds, poWriteFds, poExceptFds, poTimeout)
#endif

#ifndef OsSocketAccept
#define OsSocketAccept(hSockHandle, oSockAddr, nSockAddrLen) accept(hSockHandle, oSockAddr, nSockAddrLen)
#endif

#ifndef OsSocketConnect
#define OsSocketConnect(hSockHandle, oSockAddr, nSockAddrLen) connect(hSockHandle, oSockAddr, nSockAddrLen) 
#endif

#ifndef OsSocketShutdown
#define OsSocketShutdown(hSockHandle, nFlags) shutdown(hSockHandle, nFlags)
#endif

#ifndef OsSocketSend
#define OsSocketSend(hSockHandle, pbyBuffer, dwBufferLen, dwFlags) send(hSockHandle, pbyBuffer, dwBufferLen, dwFlags)
#endif

#ifndef OsSocketSendTo
#define OsSocketSendTo(hSockHandle, pbyBuffer, dwBufferLen, dwFlags, oDstAddr, dwDstAddrLen) sendto(hSockHandle, pbyBuffer, dwBufferLen, dwFlags, oDstAddr, dwDstAddrLen)
#endif

#ifndef OsSocketRecv
#define OsSocketRecv(hSockHandle, pbyBuffer, dwBufferLen, dwFlags) recv(hSockHandle, pbyBuffer, dwBufferLen, dwFlags)
#endif

#ifndef OsSocketRecvFrom
#define OsSocketRecvFrom(hSockHandle, pbyBuffer, dwBufferLen, dwFlags, oSrcAddr, dwSrcAddrLen) recvfrom(hSockHandle, pbyBuffer, dwBufferLen, dwFlags, oSrcAddr, dwSrcAddrLen)
#endif

#ifndef OsSocketCloseSocket
#define OsSocketCloseSocket(hSockHandle) close(hSockHandle)
#endif

#ifndef OsSocketGetLastError
#error "OsSocketGetLastError has to be defined in EcOsPlatform.h"
#endif

#ifndef OsSocketSetSockOpt
#define OsSocketSetSockOpt(hSockHandle, nLevel, nOptName, pOptValue, nOptLen ) setsockopt(hSockHandle, nLevel, nOptName, pOptValue, nOptLen)
#endif

#ifndef OsHTONS
#define OsHTONS(wHostVal) htons(wHostVal)
#endif

#endif /* EC_SOCKET_IP_SUPPORTED */

#ifndef OsAuxClkInit
#define OsAuxClkInit(dwCpuIndex, dwFrequencyHz, pvOsEvent) EC_FALSE
#endif

#ifndef OsAuxClkDeinit
#define OsAuxClkDeinit(x)
#endif

#ifndef OsAuxClkInitialCountGet
#define OsAuxClkInitialCountGet(pdwInitialCount) EC_E_NOTSUPPORTED
#endif

#ifndef OsAuxClkInputFrequencyGet
#define OsAuxClkInputFrequencyGet(pdwFrequencyHz) EC_E_NOTSUPPORTED
#endif

#ifndef OsAuxClkOutputFrequencyGet
#define OsAuxClkOutputFrequencyGet(pdwFrequencyHz) EC_E_NOTSUPPORTED
#endif

#ifndef OsAuxClkCorrectionSet
#define OsAuxClkCorrectionSet(dwInitialCountNew, nIncrementDif, dwIntCountSet) EC_E_NOTSUPPORTED
#endif

#ifndef OsAuxClkTickSinceInterrupt
#define OsAuxClkTickSinceInterrupt(pqwTicksSinceInterrupt) EC_E_NOTSUPPORTED
#endif

#ifndef OsHwTimerGetInputFrequency
#define OsHwTimerGetInputFrequency(pdwFrequencyHz) EC_E_NOTSUPPORTED
#endif

#ifndef OsHwTimerModifyInitialCount
#define OsHwTimerModifyInitialCount(nAdjustPermil) EC_E_NOTSUPPORTED
#endif

#ifndef OsHwTimerGetCurrentCount
static EC_INLINESTART EC_T_DWORD OsHwTimerGetCurrentCountInline(EC_T_INT* pnCountPermil) { EC_UNREFPARM(pnCountPermil); return EC_E_NOTSUPPORTED; } EC_INLINESTOP
#define OsHwTimerGetCurrentCount OsHwTimerGetCurrentCountInline
#endif

#ifndef OsTerminateAppRequest
#ifdef  OsPlatformTerminateAppRequest
#define OsTerminateAppRequest OsPlatformTerminateAppRequest
#else
#define OsTerminateAppRequest() EC_FALSE
#endif
#endif
#ifndef OsMalloc
#ifdef  OsPlatformMalloc
#define OsMalloc OsPlatformMalloc
#endif
#endif
#ifndef OsRealloc
#ifdef  OsPlatformRealloc
#define OsRealloc OsPlatformRealloc
#endif
#endif
#ifndef OsFree
#ifdef  OsPlatformFree
#define OsFree OsPlatformFree
#endif
#endif
#ifndef OsSleep
#ifdef  OsPlatformSleep
#define OsSleep OsPlatformSleep
#endif
#endif
#ifndef OsMemoryBarrier
#ifdef  OsPlatformMemoryBarrier
#define OsMemoryBarrier OsPlatformMemoryBarrier
#endif
#endif
#ifndef OsSetEvent
#ifdef  OsPlatformSetEvent
#define OsSetEvent OsPlatformSetEvent              
#endif
#endif
#ifndef OsResetEvent
#ifdef  OsPlatformResetEvent
#define OsResetEvent OsPlatformResetEvent
#endif
#endif
#ifndef OsDeleteThreadHandle
#ifdef  OsPlatformDeleteThreadHandle
#define OsDeleteThreadHandle OsPlatformDeleteThreadHandle
#endif
#endif
#ifndef OsGetLinkLayerRegFunc
#ifdef  OsPlatformGetLinkLayerRegFunc
#define OsGetLinkLayerRegFunc OsPlatformGetLinkLayerRegFunc
#endif
#endif

/* TODO: remove */
#if 1
#ifdef __cplusplus
} /* extern "C"*/
#endif
#endif

#endif /* INC_ECOS */

/*-END OF SOURCE FILE--------------------------------------------------------*/
