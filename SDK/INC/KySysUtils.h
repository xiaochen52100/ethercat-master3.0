#ifndef  KYSYSUTILS_H
#define  KYSYSUTILS_H

#include <stdint.h>
#include <pthread.h>
#include "KyDefines.h"

Int32_t Ky_setThreadCpuRange(pthread_t thread, Int32_t begin, Int32_t end);
Int32_t Ky_bindCurThread2Cpu(Int32_t cpu);
Int32_t Ky_setCurThreadCpuRange(Int32_t begin, Int32_t end);
Int32_t Ky_bindThread2Cpu(pthread_t thread, Int32_t cpu);

Int32_t Ky_createThreadFull(pthread_t *thread
                           , void *fn
                           , void *arg
                           , Int32_t cpu_idx_begin
                           , Int32_t cpu_idx_end
                           , Int32_t policy
                           , Int32_t sched_priority
                           );

#endif

