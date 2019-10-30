#ifndef  KYSIGNALUTILS_H
#define  KYSIGNALUTILS_H
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include "KyDefines.h"
#include "KyDebug.h"
#include "KyLog.h"

#define     RT_SIG_FOR_APP_MIN    (SIGRTMIN+10)
#define     RT_SIG_FOR_APP_MAX    (SIGRTMAX-10)
Int32_t Ky_getFreeSigAndRegisterProc(void *sig_handler);
Int32_t Ky_restoreSigDefaultProc(Int32_t sig);
Int32_t Ky_ignoreSig(Int32_t sig);
Int32_t Ky_registerSigProc(Int32_t sig, void *sig_handler);

#endif
