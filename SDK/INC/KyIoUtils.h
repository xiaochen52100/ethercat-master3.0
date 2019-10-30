#ifndef  KYIOUTILS_H
#define  KYIOUTILS_H

#include <unistd.h>
#include <stdio.h>

#include "KyDefines.h"

#define    RT_PRINT(fmt, args...) \
    do \
    { \
        printf(fmt, ##args); \
        fflush(stdout); \
    } while (0)

Int32_t Ky_fdReadable(Int32_t fd, Int32_t sec, Int32_t usec);
Int32_t Ky_fdWriteable(Int32_t fd, Int32_t sec, Int32_t usec);
Int32_t Ky_setFdNonblock(Int32_t fd);
Int32_t Ky_writeReliable(Int32_t fd, const void *buf, size_t count);
Int32_t Ky_writeCertainBytes(Int32_t fd, const void *buf, size_t count, Int32_t *written);
Int32_t Ky_readReliable(Int32_t fd, void *buf, size_t count);
Int32_t Ky_readCertainBytes(Int32_t fd, void *buf, size_t count, Int32_t *actual_got);
Int32_t Ky_printf2fd(Int32_t fd, string *fmt, ...);
Int32_t Ky_getTempFile(Int8_t *path);

#endif

