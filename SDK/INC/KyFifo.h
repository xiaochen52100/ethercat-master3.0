#ifndef KYFIFO_H
#define KYFIFO_H

#include <unistd.h>
#include <stdio.h>

#include "KyDefines.h"
#include "KyLog.h"

typedef struct fifoHead
{
    Int32_t blockSize;
    Int32_t blocks;
    Int32_t lens;
    Int32_t rd_index;
    Int32_t wr_index;
}fifoDataHeader_t;

typedef struct KyfifoInfo{
    fifoDataHeader_t *headerPointer;
    void *buf;
}KyfifoInfo_t;

extern KyfifoInfo_t *Ky_fifoInit(Int32_t blksize, Int32_t blocks, KyfifoInfo_t *info);
extern void Ky_fifoClear(KyfifoInfo_t *info);
extern Int32_t Ky_fifoWrite(KyfifoInfo_t *info, void *buf, size_t size);
extern Int32_t Ky_fifoRead(KyfifoInfo_t *info, void *buf, size_t size);
extern Int32_t Ky_fifoLen(KyfifoInfo_t *info);

#endif
