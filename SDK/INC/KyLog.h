#ifndef  KYLOG_H
#define  KYLOG_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "KyDefines.h"

Int32_t Ky_getLogFd();

#define DEFAULT_LOG_FILE_MAX_SIZE   (4096)
Int32_t Ky_initLog(Int8_t *file_name, Int32_t file_size);
Int32_t Ky_writeLog(string *fmt, ...);

#define SysLog(fmt, args...) \
    do \
    { \
    } while (0)


#define ErrSysLog(fmt, args...) \
    do \
    { \
    } while (0)

#define ErrSysLogQuit(fmt, args...) \
    do \
    { \
    } while (0)

#endif

