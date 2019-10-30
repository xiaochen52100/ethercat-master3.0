/*-----------------------------------------------------------------------------
 * selectLinkLayer.h
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Paul Bussmann
 * Description              EC-Master link layer selection
 *---------------------------------------------------------------------------*/

#ifndef INC_SELECTLINKAYER
#define INC_SELECTLINKAYER 1

/*-INCLUDES------------------------------------------------------------------*/

#ifndef INC_ECOS
#include "EcOs.h"
#endif
#include "stdio.h"
#include "stdlib.h"
#ifndef INC_ECLINK
#include "EcLink.h"
#endif

#if (defined INCLUDE_DUMMY)
#include "EcLinkDummy.h"
#endif


#if (defined _WRS_VXWORKS_MAJOR) && (defined VX_CPU_FAMILY) && (VX_CPU_FAMILY==arm)
#define INCLUDE_HNX
#endif

#if ((defined __RCX__) || (defined INCLUDE_HNX) )
#define LINK_HNX

#if (defined __RCX__)
#define BOOTLOADER
#endif
#define INCLUDE_NETX_DB500SYS
#endif

#if (defined(LINUX) && defined(_ARCH_PPC)) || (defined(VXWORKS) && defined(_VX_CPU) && ((_VX_CPU_FAMILY==_VX_ARM) || (_VX_CPU==_VX_PPC32)))
#ifndef INCLUDE_ETSEC
#define INCLUDE_ETSEC
#endif
#endif

#if (defined LINUX) && (defined __arm__)
#ifndef INCLUDE_CPSW
#define INCLUDE_CPSW
#endif
#ifndef INCLUDE_GEM
#define INCLUDE_GEM
#endif
#ifndef INCLUDE_DW3504
#define INCLUDE_DW3504
#endif
#ifndef INCLUDE_ETSEC
#define INCLUDE_ETSEC
#endif
#ifndef INCLUDE_SHETH
#define INCLUDE_SHETH
#endif
#endif /* LINUX && __arm__ */

#if (defined STARTERWARE_NOOS) || (defined EC_VERSION_SYSBIOS) || (defined EC_VERSION_JSLWARE)
 #if defined LINKLAYER_ICSS
  #define INCLUDE_ICSS
#else
  #define INCLUDE_CPSW
#endif

#endif /* STARTERWARE_NOOS */

#if (defined EC_VERSION_SYSBIOS)
  #ifndef INCLUDE_CPSW
  #define INCLUDE_CPSW
  #endif
#endif /* STARTERWARE_NOOS */

#if (defined EC_VERSION_ECOS)
 #if(defined __i386__)
  #ifndef INCLUDE_I8254X
  #define INCLUDE_I8254X
  #endif
 #endif
 #if (defined __arm__)
  #ifndef INCLUDE_ANTAIOS
  #define INCLUDE_ANTAIOS
  #endif
 #endif
#endif

#if (defined(VXWORKS) && defined(_VX_CPU) && (_VX_CPU==_VX_PPC440))
#define INCLUDE_EMAC
#endif

#ifdef LINK_HNX
#include "LinkHnx.h"
#endif
#ifdef __INTIME__
#include <stdio.h>
#elif _MSC_VER
#include "warn_dis.h"
#include <windows.h>
#include <tchar.h>
#define TCHAR_DEFINED
#include <stdio.h>
#include "warn_ena.h"
#endif

/*-DEFINES-------------------------------------------------------------------*/

#ifndef SYNTAX_ERROR
#define SYNTAX_ERROR      -2
#endif

/*-TYPEDEFS------------------------------------------------------------------*/
#ifndef TCHAR_DEFINED
typedef char TCHAR;
#endif

/*-FUNCTION DECLARATION------------------------------------------------------*/
EC_T_CHAR* GetNextWord(EC_T_CHAR **ppCmdLine, EC_T_CHAR *pStorage);

EC_T_DWORD CreateLinkParmsFromCmdLine(EC_T_CHAR** ptcWord, EC_T_CHAR** lpCmdLine, EC_T_CHAR* tcStorage, EC_T_BOOL* pbGetNextWord, EC_T_LINK_PARMS** ppLinkParms
#if defined(INCLUDE_TTS)
                                      , EC_T_DWORD* pdwTtsBusCycleUsec /* [out] TTS Bus Cycle overrides original one when TTS is used */
                                      , EC_T_VOID** pvvTtsEvent        /* [out] TTS Cycle event. Should override original one when TTS is used */
#endif
                                    );
EC_T_BOOL ParseIpAddress(EC_T_CHAR* ptcWord, EC_T_BYTE* pbyIpAddress);

EC_T_VOID ShowLinkLayerSyntax1(EC_T_VOID);
EC_T_VOID ShowLinkLayerSyntax2(EC_T_VOID);
#endif /* INC_SELECTLINKAYER */
 
/*-END OF SOURCE FILE--------------------------------------------------------*/
