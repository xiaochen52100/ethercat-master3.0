#ifndef   _CONFIG_H
#define   _CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define  FILENAME_NOTEXIST      0x02 /*配置文件名不存在*/
#define  SECTIONNAME_NOTEXIST    0x03 /*节名不存在*/
#define  KEYNAME_NOTEXIST      0x04 /*键名不存在*/
#define  STRING_LENNOTEQUAL     0x05 /*两个字符串长度不同*/
#define  STRING_NOTEQUAL       0x06 /*两个字符串内容不相同*/
#define  STRING_EQUAL        0x00 /*两个字符串内容相同*/


int CompareString(char *pInStr1,char *pInStr2);
int GetKeyValue(FILE *fpConfig,char *pInKeyName,char *pOutKeyValue);
int GetConfigIntValue(char *pInFileName,char *pInSectionName,char *pInKeyName,int *pOutKeyValue);
int GetConfigStringValue(char *pInFileName,char *pInSectionName,char *pInKeyName,char *pOutKeyValue);
int GetConfigContinueStringValue(char *pInFileName,char *pInKeyName,char pOutKeyValue[][100]);
int GetConfigContinueIntValue(char *pInFileName,char *pInKeyName,double *pOutKeyValue);


#ifdef __cplusplus
}
#endif

#endif
