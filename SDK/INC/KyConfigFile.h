#ifndef KYCONFIGFILE_H
#define KYCONFIGFILE_H

#include "KyDefines.h"
#include "KyDebug.h"
#include "KyLog.h"

#define    MAX_VAR_NUM          (50)
#define    MAX_VAR_NAME_LEN     (128)
#define    MAX_VAR_VALUE_LEN    (MAX_PATH_LEN)
#define    MAX_ARGS_LEN         (128)

Int32_t Ky_parseConfigFile(Int8_t *pathFile, Int32_t needUpdate);
Int8_t *Ky_getConfigParameter(Int8_t *parameterName);
void Ky_freeConfig();
void Ky_getPathOfFile(Int8_t *file, Int8_t *dir_path);
void Ky_removeTrailingChars(Int8_t *path, Int8_t c);
Int32_t Ky_getKey(Int8_t **line, Int8_t **key, Int8_t **value);
void Ky_valueCopy(void *d, void *s, Int32_t var_num);
Int32_t Ky_writeConfigFile();
Int32_t Ky_setConfigParameter(Int8_t *ParaName, Int8_t *value);
Int32_t Ky_setConfigParameter2(Int8_t *paraName, Int8_t *value);
void Ky_printAllParameters();

#endif
