#ifndef  KYDEBUG_H
#define  KYDEBUG_H

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "KyDefines.h"

#define    DBG_PRINT(fmt, args...) \
	do {\
		printf("DBG:%s(%d)-%s:\n"fmt"\n", __FILE__,__LINE__,__FUNCTION__,##args); \
		fflush(stdout); \
	} while (0)

#define    DBG_PRINT_S(fmt, args...) \
	do {\
		printf("DBG:%s(%d)-%s:\n"fmt"\n", pure_file_name(__FILE__),__LINE__,__FUNCTION__,##args); \
		fflush(stdout); \
	} while (0)

#define    DBG_PRINT_QUIT(fmt, args...) \
	do {\
		DBG_PRINT(fmt,##args); \
		exit(1); \
	} while (0)

#define    ERR_DBG_PRINT(fmt, args...) \
	do {\
		printf("ERR_DBG:%s(%d)-%s:\n"fmt": %s\n", __FILE__,__LINE__,__FUNCTION__,##args, strerror(errno)); \
		fflush(stderr); \
	} while (0)

#define    ERR_DBG_PRINT_QUIT(fmt, args...) \
	do {\
		ERR_DBG_PRINT(fmt,##args); \
		abort(); \
	} while (0)

void print_mem(void *start_addr, Uint32_t length);


#endif

