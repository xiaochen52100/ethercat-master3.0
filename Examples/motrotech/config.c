extern "C"
{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
}
#define  SUCCESS           0x00 /*成功*/
#define  FAILURE           0x01 /*失败*/
int GetConfigStringValue(char *pInFileName, char *pInSectionName, char *pInKeyName, char *pOutKeyValue)
{
	FILE *fpConfig;
	char szBuffer[150];
	char *pStr1, *pStr2;
	int iRetCode = 0;

	/*test

	printf("pInFileName:%s\n",pInFileName);
	printf("pInSectionName:%s\n",pInSectionName);
	printf("pInKeyName:%s\n",pInKeyName);
	*/

	memset(szBuffer, 0, sizeof(szBuffer));
	if (NULL == (fpConfig = fopen(pInFileName, "r")))
		return FILENAME_NOTEXIST;

	while (!feof(fpConfig))
	{
		if (NULL == fgets(szBuffer, 150, fpConfig))
			break;
		pStr1 = szBuffer;
		while ((' ' == *pStr1) || ('\t' == *pStr1))
			pStr1++;
		if ('[' == *pStr1)
		{
			pStr1++;
			while ((' ' == *pStr1) || ('\t' == *pStr1))
				pStr1++;
			pStr2 = pStr1;
			while ((']' != *pStr1) && ('\0' != *pStr1))
				pStr1++;
			if ('\0' == *pStr1)
				continue;
			while (' ' == *(pStr1 - 1))
				pStr1--;
			*pStr1 = '\0';

			iRetCode = CompareString(pStr2, pInSectionName);
			if (!iRetCode) /*检查节名*/
			{
				iRetCode = GetKeyValue(fpConfig, pInKeyName, pOutKeyValue);
				fclose(fpConfig);

				return iRetCode;
			}
		}
	}

	fclose(fpConfig);
	return SECTIONNAME_NOTEXIST;
}

/*区分大小写*/
int CompareString(char *pInStr1, char *pInStr2)
{
	//printf("line:%d\n",__LINE__);

	if (strlen(pInStr1) != strlen(pInStr2))
	{
		return STRING_LENNOTEQUAL;
	}

	//printf("line:%d\n",__LINE__);

	/*while( toupper(*pInStr1)==toupper(*pInStr2) )*/ /*#include <ctype.h>*/
	while (*pInStr1 == *pInStr2)
	{

		if ('\0' == *pInStr1)
		{
			break;
		}
		pInStr1++;
		pInStr2++;
	}
	//printf("line:%d\n",__LINE__);

	if ('\0' == *pInStr1)
		return STRING_EQUAL;

	return STRING_NOTEQUAL;
}

int GetKeyValue(FILE *fpConfig, char *pInKeyName, char *pOutKeyValue)
{
	char szBuffer[150];
	char *pStr1, *pStr2, *pStr3;
	unsigned int uiLen;
	int iRetCode = 0;

	memset(szBuffer, 0, sizeof(szBuffer));
	while (!feof(fpConfig))
	{
		if (NULL == fgets(szBuffer, 150, fpConfig))
			break;
		pStr1 = szBuffer;
		while ((' ' == *pStr1) || ('\t' == *pStr1))
			pStr1++;
		if ('#' == *pStr1)
			continue;
		if (('/' == *pStr1) && ('/' == *(pStr1 + 1)))
			continue;
		if (('\0' == *pStr1) || (0x0d == *pStr1) || (0x0a == *pStr1))
			continue;
		if ('[' == *pStr1)
		{
			pStr2 = pStr1;
			while ((']' != *pStr1) && ('\0' != *pStr1))
				pStr1++;
			if (']' == *pStr1)
				break;
			pStr1 = pStr2;
		}
		pStr2 = pStr1;
		while (('=' != *pStr1) && ('\0' != *pStr1))
			pStr1++;
		if ('\0' == *pStr1)
			continue;
		pStr3 = pStr1 + 1;
		if (pStr2 == pStr1)
			continue;
		*pStr1 = '\0';
		pStr1--;
		while ((' ' == *pStr1) || ('\t' == *pStr1))
		{
			*pStr1 = '\0';
			pStr1--;
		}

		//printf("line:%d\n",__LINE__);

		iRetCode = CompareString(pStr2, pInKeyName);
		if (!iRetCode) /*检查键名*/
		{
			pStr1 = pStr3;
			while ((' ' == *pStr1) || ('\t' == *pStr1))
				pStr1++;
			pStr3 = pStr1;
			while (('\0' != *pStr1) && (0x0d != *pStr1) && (0x0a != *pStr1))
			{
				if (('/' == *pStr1) && ('/' == *(pStr1 + 1)))
					break;
				pStr1++;
			}
			*pStr1 = '\0';
			uiLen = strlen(pStr3);

			memcpy(pOutKeyValue, pStr3, uiLen);
			*(pOutKeyValue + uiLen) = '\0';

			return SUCCESS;
		}
	}

	return KEYNAME_NOTEXIST;
}

int GetConfigIntValue(char *pInFileName, char *pInSectionName, char *pInKeyName, int *pOutKeyValue)
{
	int iRetCode = 0;
	char szKeyValue[16], *pStr;

	memset(szKeyValue, 0, sizeof(szKeyValue));
	iRetCode = GetConfigStringValue(pInFileName, pInSectionName, pInKeyName, szKeyValue);
	if (iRetCode)
		return iRetCode;
	pStr = szKeyValue;
	while ((' ' == *pStr) || ('\t' == *pStr))
		pStr++;
	if (('0' == *pStr) && (('x' == *(pStr + 1)) || ('X' == *(pStr + 1))))
		sscanf(pStr + 2, "%x", pOutKeyValue);
	else
		sscanf(pStr, "%d", pOutKeyValue);

	return SUCCESS;
}
int getLine(FILE *fp, char *string, int maxLength)
{
	int i = 0;
	char *rt = 0;
	char byte[1];

	printf("FILE:%p\n", fp);

	while (i < maxLength)
	{
		rt = fgets(byte, 1, fp);
		if (NULL == rt)
		{
			break;
		}
		printf("i:%d %02x %p\n", i, byte[0], rt);
		if (byte[0] == 0x0A)
		{
			break;
		}
		string[i++] = byte[0];
	}
	return i;
}

int GetConfigContinueStringValue(char *pInFileName, char *pInKeyName, char pOutKeyValue[][100])
{
	FILE *fpConfig;
	char szBuffer[1000];
	char *pStr1, *pStr2;
	int byteCount = 0;
	int stringCount = 0;
	char *rt = 0;

	memset(szBuffer, 0, sizeof(szBuffer));
	if (NULL == (fpConfig = fopen(pInFileName, "r")))
	{
		return -1;
	}

	//byteCount = getLine(fpConfig,szBuffer,100);

	while (1)
	{
		rt = fgets(szBuffer, 1000, fpConfig);
		if (NULL == rt)
		{
			return -2;
		}

		pStr1 = strstr(szBuffer, pInKeyName);

		if (pStr1 != NULL)
		{
			break;
		}
	}

	if (pStr1 == NULL)
	{
		//Not find KeyName
		fclose(fpConfig);
		return -3;
	}
	pStr1 = pStr1 + strlen(pInKeyName);
	pStr2 = pStr1;

	printf("%s:%s:", pInFileName, pInKeyName);
	while (1)
	{
		if (*pStr2 == ';')
		{
			memcpy(pOutKeyValue[stringCount], pStr1, pStr2 - pStr1);
			pStr1 = pStr2 + 1;
			printf("%s;", pOutKeyValue[stringCount]);

			stringCount++;
		}
		if (*pStr2 == 0x0A || *pStr2 == 0x00)
		{
			break;
		}
		pStr2++;
	}
	fclose(fpConfig);
	printf("\n");

	return stringCount;
}

int GetConfigContinueIntValue(char *pInFileName, char *pInKeyName, double *pOutKeyValue)
{
	FILE *fpConfig;
	char szBuffer[1000];
	char *pStr1, *pStr2;

	int intCount = 0;
	char intString[200];
	char *rt = 0;

	memset(szBuffer, 0, sizeof(szBuffer));
	if (NULL == (fpConfig = fopen(pInFileName, "r")))
	{
		return -1;
	}

	//byteCount = getLine(fpConfig,szBuffer,100);
	while (1)
	{
		rt = fgets(szBuffer, 1000, fpConfig);
		if (NULL == rt)
		{
			return -2;
		}

		pStr1 = strstr(szBuffer, pInKeyName);

		if (pStr1 != NULL)
		{
			break;
		}
	}
	if (pStr1 == NULL)
	{
		//Not find KeyName
		fclose(fpConfig);
		return -3;
	}
	pStr1 = pStr1 + strlen(pInKeyName);
	pStr2 = pStr1;

	printf("%s:%s:", pInFileName, pInKeyName);
	while (1)
	{
		if (*pStr2 == ';')
		{
			memset(intString, 0, sizeof(intString));
			memcpy(intString, pStr1, pStr2 - pStr1);

			printf("%s;", intString);

			pOutKeyValue[intCount++] = atof(intString);
			pStr1 = pStr2 + 1;
		}
		if (*pStr2 == 0x0A || *pStr2 == 0x00)
		{
			break;
		}
		pStr2++;
	}
	fclose(fpConfig);
	printf("\n");

	return intCount;
}
