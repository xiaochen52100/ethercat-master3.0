#ifndef  KYUTILS_H
#define  KYUTILS_H

#include <stdint.h>
#include <signal.h>
#include <time.h>

#include "KyUtils.h"
#include "KySysUtils.h"
#include "KyDebug.h"
#include "KyDefines.h"
#include "KyLog.h"
#include "KySignalUtils.h"

void Ky_sleep(long sec, long nsec);
void Ky_printTime();
Int32_t Ky_registerSighandler(Int32_t signum, void (*handler)(Int32_t));
Int32_t Ky_fileExists(Int8_t *file_path);
Int32_t Ky_waitForFileExists(Int32_t sec, Int8_t *file_path);

Int32_t Ky_createTimer(timer_t *timerid, void *timer_handler, Int32_t sec, Int32_t nano_sec);
void Ky_itimerInit(Int32_t s, Int32_t us);

#endif

