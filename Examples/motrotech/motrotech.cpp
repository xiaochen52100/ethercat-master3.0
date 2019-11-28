/*-INCLUDES------------------------------------------------------------------*/
#include <AtEthercat.h>
#include "motrotech.h"
#include "Logging.h"
#include "EcLog.h"

extern "C"
{
#include "KyDefines.h"
#include "KyLog.h"
#include "KySemMemery.h"
#include "KyUtils.h"
#include "KySysUtils.h"
#include "config.h"
#include <math.h>
}
/*-DEFINES-------------------------------------------------------------------*/
#define SLAVE_NOT_FOUND 0xFFFF

#define MBX_TIMEOUT 5000

#define MOTOR_TESTING

#define MY_MEM_DATA_BUFFER_NUMBER (5)

#define pEcLogContext (G_pEcLogParms->pLogContext)
#define dwEcLogLevel G_pEcLogParms->dwLogLevel
#define pLogMsgCallback G_pEcLogParms->pfLogMsg
#define EC_LOG_LEVEL_INFO 4

#define CIRCLE_LENGTH 431
#define MAXDEVICES 6
/////////////////////////////////////////////////////////

/*-GLOBAL VARIABLES-----------------------------------------------------------*/
My_Slave_Type My_Slave[MAX_SERVO_NUM];
EC_T_SDWORD first_analysis_cycle = 500;                                      //一次解析周期
EC_T_SDWORD second_analysis_nummber = 75;                                    //二次解析个数
EC_T_SDWORD zero_point[6];                                                   // = {17888, 2818, -53133, -120417, -107474, 25559};  //零点位置（绝对位置）
EC_T_SDWORD home_point[6] = {17888, 2818, -53133, -120417, -2237394, 25559}; //home位置（绝对位置）
EC_T_SDWORD angle_position[6];                                               // = {-2223, -735461, -462111, 1294, -709974, -1708}; //测试起点位置（位置增量）
//测试轨迹（位置增量）
my_mem_data_t My_Mem_Data;
my_mem_status_t My_Mem_Status;
my_mem_cmd_t My_Mem_Cmd;
KySemInfo_t sinfo_data;
KySemInfo_t sinfo_status;
KySemInfo_t sinfo_cmd;
/*-LOCAL VARIABLES-----------------------------------------------------------*/
static EC_T_INT MotorCount = 0;

static EC_T_DWORD S_ProcessState[MAXDEVICES];

/*-FUNCTION DEFINITIONS------------------------------------------------------*/

/******************************************************************************
Function: myInit
Description: Initialize variables
Called By: myAppInit
Return: EC_E_NOERROR
*******************************************************************************/
EC_T_DWORD myInit(T_DEMO_THREAD_PARAM *pDemoThreadParam)
{
    EC_T_DWORD dwRes = EC_E_NOERROR;
    EC_T_CHAR ZeroPoint[1024];
    EC_T_CHAR HomePoint[1024];
    EC_T_DWORD result;
    EC_UNREFPARM(pDemoThreadParam);
    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, " Motrotech: ___________________myInit_______________________________\n"));
    OsMemset(My_Slave, 0, MAX_SERVO_NUM * sizeof(My_Slave_Type));
    OsMemset(&My_Mem_Data, 0, sizeof(my_mem_data_t));
    OsMemset(&My_Mem_Status, 0, sizeof(my_mem_status_t));
    OsMemset(&My_Mem_Cmd, 0, sizeof(my_mem_cmd_t));
    result = GetConfigStringValue("system.cfg", "Robot", "ZeroPoint", ZeroPoint); //加载零点
    if (result != FILENAME_NOTEXIST && result != SECTIONNAME_NOTEXIST)
    {
        sscanf(ZeroPoint, "%d,%d,%d,%d,%d,%d", zero_point, zero_point + 1, zero_point + 2, zero_point + 3, zero_point + 4, zero_point + 5);
        EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "ZeroPoint:%d %d %d %d %d %d\n", zero_point[0], zero_point[1], zero_point[2], zero_point[3], zero_point[4], zero_point[5]));
    }
    else
    {
        dwRes = EC_E_ERROR;
        goto Exit;
    }

    result = GetConfigStringValue("system.cfg", "Robot", "HomePoint", HomePoint); //加载home点
    if (result != FILENAME_NOTEXIST && result != SECTIONNAME_NOTEXIST)
    {
        sscanf(HomePoint, "%d,%d,%d,%d,%d,%d", home_point, home_point + 1, home_point + 2, home_point + 3, home_point + 4, home_point + 5);
        EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "HomePoint:%d %d %d %d %d %d\n", home_point[0], home_point[1], home_point[2], home_point[3], home_point[4], home_point[5]));
    }
    else
    {
        dwRes = EC_E_ERROR;
        goto Exit;
    }

    for (EC_T_DWORD temp = 0; temp < MAXDEVICES; temp++)
    {
        My_Slave[temp].wState = DRV_DEV_STATE_NOT_READY;
        //angle_position[temp] = angle_position[temp] + zero_point[temp]; //初始化起点位置
    }
    //EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "angle_position:%d %d %d %d %d %d\n", angle_position[0], angle_position[1], angle_position[2], angle_position[3], angle_position[4], angle_position[5]));
    result = Ky_semMemNew(&sinfo_status, Ky_etherCat_State, 1, sizeof(my_mem_status_t), MY_MEM_DATA_BUFFER_NUMBER);
    if (result != SUCCESS)
    {
        dwRes = EC_E_ERROR;
        goto Exit;
    }
    else
    {
        Ky_semMemClear(&sinfo_status, 1);
    }
    result = Ky_semMemNew(&sinfo_data, Ky_etherCat_Data, 1, sizeof(my_mem_data_t), MY_MEM_DATA_BUFFER_NUMBER);
    if (result != SUCCESS)
    {
        dwRes = EC_E_ERROR;
        goto Exit;
    }
    else
    {
        Ky_semMemClear(&sinfo_data, 1);
    }
    result = Ky_semMemNew(&sinfo_cmd, Ky_etherCat_Cmd, 1, sizeof(my_mem_cmd_t), MY_MEM_DATA_BUFFER_NUMBER);
    if (result != SUCCESS)
    {
        dwRes = EC_E_ERROR;
        goto Exit;
    }
    else
    {
        Ky_semMemClear(&sinfo_cmd, 1);
    }
Exit:
    return dwRes;
}

/******************************************************************************
Function?myPrepare
Description: search target slave PDO address; modify the My_Slave number, using S_dwAppFoundSlaves as the My_slave index
Called By: myAppPrepare
Return: EC_E_NOERROR
*******************************************************************************/
EC_T_DWORD myPrepare(T_DEMO_THREAD_PARAM *pDemoThreadParam, EC_T_DWORD dwVendorId, EC_T_DWORD dwProductCode)
{
    EC_T_WORD wFixedAddress = 0;
    EC_T_WORD FoundFixedAddress = 0;

    EC_UNREFPARM(pDemoThreadParam);
    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, " Motrotech: ___________________myprepare_______________________________\n"));

    for (EC_T_DWORD i = 0; i < emGetNumConnectedSlaves(INSTANCE_MASTER_DEFAULT); i++)
    {
        if (FindSlaveGetFixedAddr(pDemoThreadParam, i, dwVendorId, dwProductCode, &wFixedAddress))
        {
            if ((FoundFixedAddress != wFixedAddress) && (wFixedAddress >= 1001))
            {
                MotorCount++;
                FoundFixedAddress = wFixedAddress;

                EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: (%d)my drive have find\n", MotorCount));
            }
        }
    }
    return EC_E_NOERROR;
}

/******************************************************************************
Function?mySetup
Description: Slave variable initialization; add index 0x6060; modify the scan into mysetup in next step
Called By: myAppSetup
Return: EC_E_NOERROR
*******************************************************************************/
EC_T_DWORD mySetup(
    T_DEMO_THREAD_PARAM *pDemoThreadParam, EC_T_BYTE *pbyPDIn /**< [in]  pointer to process data input buffer */
    ,
    EC_T_BYTE *pbyPDOut /**< [in]  pointer to process data output buffer */
)
{
    EC_T_DWORD dwRetVal = EC_E_NOERROR;
    EC_T_WORD wReadEntries = 0;
    EC_T_WORD wWriteEntries = 0;
    EC_T_WORD mySlaveOutVarInfoNum = 0;
    EC_T_WORD mySlaveInVarInfoNum = 0;
    EC_T_PROCESS_VAR_INFO_EX *pProcessVarInfoOut = EC_NULL;
    EC_T_PROCESS_VAR_INFO_EX *pProcessVarInfoIn = EC_NULL;

    EC_UNREFPARM(pDemoThreadParam);

    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, " Motrotech: ___________________mysetup______________________\n"));

    if ((pbyPDOut != EC_NULL) && (pbyPDIn != EC_NULL))
    {
        for (EC_T_INT MySlaves_num_tmp = 0; MySlaves_num_tmp < MotorCount; MySlaves_num_tmp++)
        {
            if (ecatGetCfgSlaveInfo(EC_TRUE, (EC_T_WORD)(1001 + MySlaves_num_tmp), &My_Slave[MySlaves_num_tmp].S_aSlaveList) != EC_E_NOERROR)
            {
                EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "ERROR: ecatGetCfgSlaveInfo() returns with error\n"));
                continue;
            }
            My_Slave[MySlaves_num_tmp].wFixedAddress = (EC_T_WORD)(1001 + MySlaves_num_tmp);

            /********************** PDO_OUT ***********************/
            mySlaveOutVarInfoNum = My_Slave[MySlaves_num_tmp].S_aSlaveList.wNumProcessVarsOutp;
            EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: OurVarInfoNum = %d", mySlaveOutVarInfoNum));

            pProcessVarInfoOut = (EC_T_PROCESS_VAR_INFO_EX *)OsMalloc(sizeof(EC_T_PROCESS_VAR_INFO_EX) * mySlaveOutVarInfoNum);
            if (pProcessVarInfoOut == EC_NULL)
            {
                dwRetVal = EC_E_NOMEMORY;
                EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: Malloc memory fail\n"));
            }
            else
            {
                dwRetVal = ecatGetSlaveOutpVarInfoEx(EC_TRUE, My_Slave[MySlaves_num_tmp].wFixedAddress, mySlaveOutVarInfoNum, pProcessVarInfoOut, &wReadEntries);
                if (EC_E_NOERROR != dwRetVal)
                {
                    EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, "ERROR: emGetSlaveInpVarInfoEx() (Result = %s 0x%x)", ecatGetText(dwRetVal), dwRetVal));
                }
            }

            for (int i = 0; i < mySlaveOutVarInfoNum; i++)
            {
                if (pProcessVarInfoOut[i].wIndex == DRV_OBJ_CONTROL_WORD) // 0x6040 - ControlWord
                {
                    My_Slave[MySlaves_num_tmp].myControlWord = (EC_T_WORD *)&(pbyPDOut[pProcessVarInfoOut[i].nBitOffs / 8]);
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: My_Slave[%d].myControlWord = %d", MySlaves_num_tmp, My_Slave[MySlaves_num_tmp].myControlWord));
                }
                else if (pProcessVarInfoOut[i].wIndex == DRV_OBJ_TARGET_POSITION) // 0x607A - TargetPosition
                {
                    My_Slave[MySlaves_num_tmp].myTargetPosition = (EC_T_SDWORD *)&(pbyPDOut[pProcessVarInfoOut[i].nBitOffs / 8]);
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: My_Slave[%d].myTargetPosition = %d", MySlaves_num_tmp, My_Slave[MySlaves_num_tmp].myTargetPosition));
                }
                else if (pProcessVarInfoOut[i].wIndex == DRV_OBJ_MODES_OF_OPERATION) // 0x6060 - MODE
                {
                    My_Slave[MySlaves_num_tmp].myModeOfOperation = (EC_T_BYTE *)&(pbyPDOut[pProcessVarInfoOut[i].nBitOffs / 8]);
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: My_Slave[%d].myModeOfOperation = %d", MySlaves_num_tmp, My_Slave[MySlaves_num_tmp].myModeOfOperation));
                }
                else if (pProcessVarInfoOut[i].wIndex == DRV_OBJ_TARGET_VELOCITY) // 0X60FF- target velocity
                {
                    My_Slave[MySlaves_num_tmp].myTargetVelocity = (EC_T_SDWORD *)&(pbyPDOut[pProcessVarInfoOut[i].nBitOffs / 8]);
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: My_Slave[%d].myTargetVelocity = %d", MySlaves_num_tmp, My_Slave[MySlaves_num_tmp].myTargetVelocity));
                }
                else if (pProcessVarInfoOut[i].wIndex == DRV_OBJ_TARGET_TORQUE) // 0X6071- target torque
                {
                    My_Slave[MySlaves_num_tmp].myTargetTorque = (EC_T_SWORD *)&(pbyPDOut[pProcessVarInfoOut[i].nBitOffs / 8]);
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: My_Slave[%d].myTargetTorque = %d", MySlaves_num_tmp, My_Slave[MySlaves_num_tmp].myTargetTorque));
                }
                else if (pProcessVarInfoOut[i].wIndex == DRV_OBJ_TOUCH_PROBE_FUNCTION) // 0X60B8- TOUCH probe function
                {
                    My_Slave[MySlaves_num_tmp].myTouchProbeFunction = (EC_T_WORD *)&(pbyPDOut[pProcessVarInfoOut[i].nBitOffs / 8]);
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: My_Slave[%d].myTouchProbeFunction = %d", MySlaves_num_tmp, My_Slave[MySlaves_num_tmp].myTouchProbeFunction));
                }
                else if (pProcessVarInfoOut[i].wIndex == DRV_OBJ_MAX_PROFILE_VELOCITY) // 0X607F- Max profile velocity
                {
                    My_Slave[MySlaves_num_tmp].myMaxProfileVelocity = (EC_T_DWORD *)&(pbyPDOut[pProcessVarInfoOut[i].nBitOffs / 8]);
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: My_Slave[%d].myMaxProfileVelocity = %d", MySlaves_num_tmp, My_Slave[MySlaves_num_tmp].myMaxProfileVelocity));
                }
                else
                {
                    //LogMsg("Motrotech: My_Slave[%d] Output undefine the index %d / usbindex %d", i, (pProcessVarInfoOut[i].wIndex, pProcessVarInfoOut[i].wSubIndex);
                }
            }

            /********************** PDO_IN ***********************/
            mySlaveInVarInfoNum = My_Slave[MySlaves_num_tmp].S_aSlaveList.wNumProcessVarsInp;
            EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: InVarInfoNum = %d", mySlaveInVarInfoNum));

            pProcessVarInfoIn = (EC_T_PROCESS_VAR_INFO_EX *)OsMalloc(sizeof(EC_T_PROCESS_VAR_INFO_EX) * mySlaveInVarInfoNum);
            if (pProcessVarInfoIn == EC_NULL)
            {
                dwRetVal = EC_E_NOMEMORY;
                EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: Malloc memory fail"));
            }
            else
            {
                dwRetVal = ecatGetSlaveInpVarInfoEx(EC_TRUE, My_Slave[MySlaves_num_tmp].wFixedAddress, mySlaveInVarInfoNum, pProcessVarInfoIn, &wWriteEntries);
                if (EC_E_NOERROR != dwRetVal)
                {
                    EcLogMsg(EC_LOG_LEVEL_ERROR, (pEcLogContext, EC_LOG_LEVEL_ERROR, (EC_T_CHAR *)"ERROR: emGetSlaveInpVarInfoEx() (Result = %s 0x%x)", ecatGetText(dwRetVal), dwRetVal));
                }
            }

            for (int i = 0; i < mySlaveInVarInfoNum; i++)
            {
                if (pProcessVarInfoIn[i].wIndex == DRV_OBJ_ERROR_CODE) // 0x603F - Error Code
                {
                    My_Slave[MySlaves_num_tmp].myErrorCode = (EC_T_WORD *)&(pbyPDIn[pProcessVarInfoIn[i].nBitOffs / 8]);
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: MySlave[%d].myErrorCode = %d", MySlaves_num_tmp, My_Slave[MySlaves_num_tmp].myErrorCode));
                }
                else if (pProcessVarInfoIn[i].wIndex == DRV_OBJ_STATUS_WORD) // 0x6041 - StatusWord
                {
                    My_Slave[MySlaves_num_tmp].myStatusWord = (EC_T_WORD *)&(pbyPDIn[pProcessVarInfoIn[i].nBitOffs / 8]);
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: MySlave[%d].myStatusWord = %d", MySlaves_num_tmp, My_Slave[MySlaves_num_tmp].myStatusWord));
                }
                else if (pProcessVarInfoIn[i].wIndex == DRV_OBJ_POSITION_ACTUAL_VALUE) // 0x6064 - ActualPosition
                {
                    My_Slave[MySlaves_num_tmp].myActPosition = (EC_T_SDWORD *)&(pbyPDIn[pProcessVarInfoIn[i].nBitOffs / 8]);
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: MySlave[%d].myActPosition = %d", MySlaves_num_tmp, My_Slave[MySlaves_num_tmp].myActPosition));
                }
                else if (pProcessVarInfoIn[i].wIndex == DRV_OBJ_VELOCITY_ACTUAL_VALUE) // 0x606C - ActualVelocity
                {
                    My_Slave[MySlaves_num_tmp].myActVelocity = (EC_T_SDWORD *)&(pbyPDIn[pProcessVarInfoIn[i].nBitOffs / 8]);
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: MySlave[%d].myActVelocity = %d", MySlaves_num_tmp, My_Slave[MySlaves_num_tmp].myActVelocity));
                }
                else if (pProcessVarInfoIn[i].wIndex == DRV_OBJ_TORQUE_ACTUAL_VALUE) // 0x6077 - ActTorque
                {
                    My_Slave[MySlaves_num_tmp].myActTorque = (EC_T_SWORD *)&(pbyPDIn[pProcessVarInfoIn[i].nBitOffs / 8]);
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: MySlave[%d].myActTorque = %d", MySlaves_num_tmp, My_Slave[MySlaves_num_tmp].myActTorque));
                }
                else if (pProcessVarInfoIn[i].wIndex == DRV_OBJ_MODE_OF_DISPLAY) // 0x6061 MODE of opration display
                {
                    My_Slave[MySlaves_num_tmp].myModeOfOperationDisplay = (EC_T_BYTE *)&(pbyPDIn[pProcessVarInfoIn[i].nBitOffs / 8]);
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: MySlave[%d].myModeOfOperationDisplay = %d", MySlaves_num_tmp, My_Slave[MySlaves_num_tmp].myModeOfOperationDisplay));
                }
                else if (pProcessVarInfoIn[i].wIndex == DRV_OBJ_TOUCH_PROBE_STATUS) // 0x60B9 - TOUCH probe status
                {
                    My_Slave[MySlaves_num_tmp].myTouchProbeStatus = (EC_T_WORD *)&(pbyPDIn[pProcessVarInfoIn[i].nBitOffs / 8]);
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: MySlave[%d].myTouchProbeStatus = %d", MySlaves_num_tmp, My_Slave[MySlaves_num_tmp].myTouchProbeStatus));
                }
                else if (pProcessVarInfoIn[i].wIndex == DRV_OBJ_TOUCH_PROBE_POS1_VALUE) // 0x60BA -
                {
                    My_Slave[MySlaves_num_tmp].myTouchProbePos1Value = (EC_T_SDWORD *)&(pbyPDIn[pProcessVarInfoIn[i].nBitOffs / 8]);
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: MySlave[%d].myTouchProbePos1Value = %d", MySlaves_num_tmp, My_Slave[MySlaves_num_tmp].myTouchProbePos1Value));
                }
                else if (pProcessVarInfoIn[i].wIndex == DRV_OBJ_TOUCH_PROBE_POS2_VALUE) // 0x60BC-
                {

                    My_Slave[MySlaves_num_tmp].myTouchProbePos2Value = (EC_T_SDWORD *)&(pbyPDIn[pProcessVarInfoIn[i].nBitOffs / 8]);

                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: MySlave[%d].myTouchProbePos2Value = %d", MySlaves_num_tmp, My_Slave[MySlaves_num_tmp].myTouchProbePos2Value));
                }
                else if (pProcessVarInfoIn[i].wIndex == DRV_OBJ_DIGITAL_INPUT) // 0x60FD - Input
                {
                    My_Slave[MySlaves_num_tmp].myInputs = (EC_T_DWORD *)&(pbyPDIn[pProcessVarInfoIn[i].nBitOffs / 8]);
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: MySlave[%d].myInputs = %d", MySlaves_num_tmp, My_Slave[MySlaves_num_tmp].myInputs));
                }
                else
                {
                    //LogMsg("Motrotech: My_Slave[%d] Input undefine the index %d / usbindex %d", i, (pProcessVarInfoIn[i].wIndex, pProcessVarInfoIn[i].wSubIndex);
                }
            }
        }
    }

    return EC_E_NOERROR;
}
/******************************************************************************
Function?arrayOr
Description: 判断数组中是否有全等于target目标值的，是则输出1，否则输出0
Return: EC_T_INT
*******************************************************************************/
EC_T_INT arrayAnd(
    EC_T_SDWORD *array,
    EC_T_SDWORD length,
    EC_T_SDWORD target)
{
    EC_T_INT count = 0;
    for (EC_T_INT i = 0; i < length; i++)
    {
        if (array[i] == target)
        {
            count++;
        }
    }
    if ((length == count) && length > 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/******************************************************************************
Function?arrayOr
Description: 判断数组中是否有等于target目标值的，有则输出1，没有则输出0
Return: EC_T_INT
*******************************************************************************/
EC_T_INT arrayOr(
    EC_T_SDWORD *array,
    EC_T_SDWORD length,
    EC_T_SDWORD target)
{
    EC_T_INT count = 0;
    for (EC_T_INT i = 0; i < length; i++)
    {
        if (array[i] == target)
        {
            count++;
        }
    }
    if (count > 0)
    {
        return count;
    }
    else
    {
        return 0;
    }
}
/******************************************************************************
Function?arrayNot
Description: 判断数组中是否有不等于target目标值的，有则输出0，没有则输出1
Return: EC_T_INT
*******************************************************************************/
EC_T_INT arrayNot(
    EC_T_SDWORD *array,
    EC_T_SDWORD length,
    EC_T_SDWORD target)
{
    EC_T_INT count = 0;
    for (EC_T_INT i = 0; i < length; i++)
    {
        if (array[i] != target)
        {
            count++;
        }
    }
    if (count > 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/******************************************************************************
Function?myWorkpd
Description: Servo control demo code, re-code the acc and dec
Called By: myAppWorkpd
Return: EC_E_NOERROR
*******************************************************************************/
EC_T_VOID myWorkpd(
    T_DEMO_THREAD_PARAM *pDemoThreadParam, /* [in]  Logging instance */
    EC_T_BYTE *pbyPDIn,                    /* [in]  pointer to process data input buffer */
    EC_T_BYTE *pbyPDOut                    /* [in]  pointer to process data output buffer */
)
{
    static EC_T_DWORD *dwCommand = S_ProcessState;
    static eMoveState eMotionState[MAXDEVICES] = {EPause, EPause, EPause, EPause, EPause, EPause}; //当前状态
    static EC_T_SDWORD curDistance[MAXDEVICES];                                                    //当前位置
    static EC_T_SDWORD wPositionArrivals[MAXDEVICES];                                              //位置到达标志
    static EC_T_SDWORD wDistance[MAXDEVICES];                                                      //位置增量
    static EC_T_REAL wDecimal[MAXDEVICES];                                                         //小数位误差
    static EC_T_REAL wCumulation[MAXDEVICES];                                                      //累积误差
    static EC_T_SDWORD homingDistance[MAXDEVICES];                                                 //复位位置增量
    static EC_T_SDWORD p2pDistance[MAXDEVICES];                                                    //点到点位置增量
    static EC_T_INT waitCount = 0;                                                                 //外部通信周期
    static EC_T_INT firstReadFlag = 0;                                                             //位置初始化标志,0：未完成初始化；1：完成初始化
    static EC_T_INT vCount[MAXDEVICES] = {0, 0, 0, 0, 0, 0};                                       //行走节拍计数
    static EC_T_INT homingCount[MAXDEVICES] = {0, 0, 0, 0, 0, 0};                                  //复位节拍计数
    static EC_T_INT p2pCount[MAXDEVICES] = {0, 0, 0, 0, 0, 0};                                     //点到点节拍计数
    static EC_T_INT targetIndex = 0;                                                               //轨迹计数
    static EC_T_INT maxHomingDistance = 0;                                                         //复位几个轴最大的偏差
    static EC_T_INT maxP2PDistance = 0;                                                            //点到点几个轴最大偏差
    static EC_T_INT homingTime = 0;                                                                //复位时间
    static EC_T_INT p2pTime = 0;                                                                   //点到点时间
    static EC_T_INT traCount = 0;                                                                  //多轨迹计数
    EC_T_INT result;
    EC_UNREFPARM(pDemoThreadParam);
    EC_UNREFPARM(pbyPDIn);
    EC_UNREFPARM(pbyPDOut);
    for (EC_T_INT i = 0; i < MotorCount; i++)
    {
        if (My_Slave[i].myTargetPosition != NULL)
        {
            if (EC_GETDWORD(My_Slave[i].myActPosition) != 0 && firstReadFlag == 0) //初始化位置
            {
                curDistance[i] = EC_GETDWORD(My_Slave[i].myActPosition); //初始化位置

                EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "InitPosition[%d]:%d\n\n", i, curDistance[i]));
            }
        }
    }
    if (arrayNot(curDistance, MotorCount, 0)) //初始化完成标志
    {
        firstReadFlag = 1;
    }
    //读取CMD
    result = Ky_semMemRead(&sinfo_cmd, &My_Mem_Cmd, sizeof(my_mem_cmd_t), 1);
    if (result != SUCCESS)
    {
    }
    else
    {
        for (EC_T_INT i = 0; i < MotorCount; i++)
        {
            if (dwCommand[i] == COMMAND_START)
            {
                if (My_Mem_Cmd.cmdWord[i] == 0) //指定伺服off
                {
                    Process_Switch(i, COMMAND_STOP);
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "switch off[%d]:%d\n", i, My_Mem_Cmd.cmdWord[i]));
                }
            }
            else
            {
                if (My_Mem_Cmd.cmdWord[i] == 1 && firstReadFlag == 1) //指定伺服on
                {
                    Process_Switch(i, COMMAND_START);
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "switch on[%d]:%d\n", i, My_Mem_Cmd.cmdWord[i]));
                }
            }
            eMotionState[i] = (enum eMoveState)My_Mem_Cmd.moveState[i];
        }
        if (arrayAnd(My_Mem_Cmd.moveState, MotorCount, 3)) //所有轴接到复位指令，此函数目的在于homing的时候所有轴同步
        {
            for (EC_T_INT j = 0; j < MotorCount; j++) //计算所有轴home的偏差
            {
                homingDistance[j] = home_point[j] - curDistance[j];
                wDistance[j] = 0;
            }
            maxHomingDistance = 0;
            homingTime = 0;
            for (EC_T_INT j = 0; j < MotorCount; j++) //计算偏差最大值
            {
                if (abs(maxHomingDistance) < abs(homingDistance[j]))
                {
                    maxHomingDistance = homingDistance[j];
                }
            }
            homingTime = abs(maxHomingDistance) / 200 + 1; //计算时间
            EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "maxHomingDistance:%d\n", maxHomingDistance));
            EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "homingTime:%d\n", homingTime));
            for (EC_T_INT j = 0; j < MotorCount; j++) //计算位置增量
            {
                wDistance[j] = homingDistance[j] / homingTime;
                EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "wDistance[%d]:%d\n", j, wDistance[j]));
            }
        }

        if ((arrayAnd(My_Mem_Cmd.moveState, MotorCount, 4)) && (MotorCount > 1)) //所有轴接到点到点指令，此函数目的在于homing的时候所有轴同步
        {
            //读取DATA
            result = Ky_semMemRead(&sinfo_data, &My_Mem_Data, sizeof(my_mem_data_t), 1);
            if (result != SUCCESS)
            {
                EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "read data false\n\n"));
            }
            else
            {
                if (arrayNot(My_Mem_Data.axis_Theta, MotorCount, 0))
                {
                    for (EC_T_INT j = 0; j < MotorCount; j++)
                    {
                        angle_position[j] = My_Mem_Data.axis_Theta[j] + zero_point[j];
                        p2pDistance[j] = angle_position[j] - curDistance[j];
                        wDistance[j] = 0;
                    }
                    maxP2PDistance = 0;
                    p2pTime = 0;
                    for (EC_T_INT j = 0; j < MotorCount; j++) //找到偏差绝对值的最大值
                    {
                        if (abs(maxP2PDistance) < abs(p2pDistance[j]))
                        {
                            maxP2PDistance = p2pDistance[j];
                        }
                    }
                    p2pTime = abs(maxP2PDistance) / 200 + 1; //计算时间
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "maxP2PDistance:%d\n", maxP2PDistance));
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "p2pTime:%d\n", p2pTime));
                    for (EC_T_INT j = 0; j < MotorCount; j++) //计算位置增量
                    {
                        wDistance[j] = p2pDistance[j] / p2pTime;
                        EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "wDistance[%d]:%d\n", j, wDistance[j]));
                    }
                }
            }
        }
        if (arrayOr(My_Mem_Cmd.moveState, MotorCount, 4) == 1) //某一个轴点到点
        {
            //读取DATA
            result = Ky_semMemRead(&sinfo_data, &My_Mem_Data, sizeof(my_mem_data_t), 1);
            if (result != SUCCESS)
            {
                EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "read data false\n\n"));
            }
            else
            {
                for (EC_T_INT j = 0; j < MotorCount; j++)
                {
                    if (eMotionState[j] == P2P)
                    {
                        angle_position[j] = My_Mem_Data.axis_Theta[j] + zero_point[j];
                        p2pDistance[j] = angle_position[j] - curDistance[j];
                        wDistance[j] = 0;
                        p2pTime = abs(p2pDistance[j]) / 200 + 1; //计算时间
                        wDistance[j] = p2pDistance[j] / p2pTime;
                    }
                    else
                    {
                        eMotionState[j] = EPause;
                    }
                }
            }
        }
    }

    for (EC_T_INT i = 0; i < MotorCount; i++)
    {
        if (My_Slave[i].myActPosition != NULL && firstReadFlag == 1)
        {
            My_Mem_Status.axis_Theta[i] = EC_GETDWORD(My_Slave[i].myActPosition);

            My_Mem_Status.statusWord[i] = firstReadFlag;
        }
    }
    //发送Status
    result = Ky_semMemWrite(&sinfo_status, &My_Mem_Status, sizeof(my_mem_status_t), 1);
    if (result != SUCCESS)
    {
    }
    else
    {
    }

    //如果所有轴到达目标点，则装载下一个目标点
    if (arrayAnd(wPositionArrivals, MotorCount, 2))
    {
        for (EC_T_INT j = 0; j < MotorCount; j++)
        {
            wPositionArrivals[j] = 1; //标志置位
            eMotionState[j] = EPause;
        }

        //读取DATA
        result = Ky_semMemRead(&sinfo_data, &My_Mem_Data, sizeof(my_mem_data_t), 1);
        if (result != SUCCESS)
        {
            EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "read data false\n\n"));
        }
        else
        {
            if (My_Mem_Data.deltaT != 0)
            {
                first_analysis_cycle = My_Mem_Data.deltaT;
            }
            if (My_Mem_Data.deltaNumber != 0)
            {
                second_analysis_nummber = My_Mem_Data.deltaNumber;
            }
            targetIndex++;
            //EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "My_Mem_Data.axis_Theta is :%d %d %d %d %d %d\n", My_Mem_Data.axis_Theta[0], My_Mem_Data.axis_Theta[1], My_Mem_Data.axis_Theta[2], My_Mem_Data.axis_Theta[3], My_Mem_Data.axis_Theta[4], My_Mem_Data.axis_Theta[5]));
            if (arrayNot(My_Mem_Data.axis_Theta, MotorCount, 0))
            {
                for (EC_T_INT j = 0; j < MotorCount; j++)
                {
                    wDistance[j] = (EC_T_SDWORD)(My_Mem_Data.axis_Theta[j] + zero_point[j] - curDistance[j]) / second_analysis_nummber;               //计算增量
                    wDecimal[j] = (My_Mem_Data.axis_Theta[j] + zero_point[j] - curDistance[j]) / ((EC_T_REAL)second_analysis_nummber) - wDistance[j]; //计算浮点误差
                    eMotionState[j] = EMoving;
                }
            }
        }

        //EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "wDistance is :%d %d %d %d %d %d\n", wDistance[0], wDistance[1], wDistance[2], wDistance[3], wDistance[4], wDistance[5]));
    }
    for (EC_T_INT i = 0; i < MotorCount; i++)
    {
        if (firstReadFlag == 1 && dwCommand[i] == COMMAND_START)
        {
            switch (eMotionState[i])
            {
            case EMoveSetup: //开始运行轨迹
            {
                wPositionArrivals[i] = 2;
            }
            break;

            case EMoving: //运动轨迹
            {
                vCount[i]++;
                if (vCount[i] > second_analysis_nummber)
                {
                    vCount[i] = 0;
                    wPositionArrivals[i] = 2;
                    eMotionState[i] = EPause;
                }
                else
                {
                    wCumulation[i] = wCumulation[i] + wDecimal[i];
                    if (wCumulation[i] >= 1) //累计误差到达1则装载误差
                    {
                        curDistance[i] = curDistance[i] + wDistance[i] + 1;
                        wCumulation[i] = 0;
                        //EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "+1\n"));
                    }
                    else if (wCumulation[i] <= -1)
                    {
                        curDistance[i] = curDistance[i] + wDistance[i] - 1;
                        wCumulation[i] = 0;
                        //EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "-1\n"));
                    }
                    else
                    {
                        curDistance[i] = curDistance[i] + wDistance[i];
                    }
                    EC_SETDWORD(My_Slave[i].myTargetPosition, curDistance[i]);
                }
            }
            break;

            case EPause: //暂停
                wPositionArrivals[i] = 1;
                break;

            case Homing:
                homingCount[i]++;
                if ((homingCount[i] == homingTime) && (homingCount[i] > 1))
                {
                    homingCount[i] = 0;
                    EC_SETDWORD(My_Slave[i].myTargetPosition, home_point[i]);
                    eMotionState[i] = EPause;
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Homing[%d] is Arrivaed \n", i));
                }
                else if (homingCount[i] < homingTime)
                {
                    curDistance[i] = curDistance[i] + wDistance[i];
                    EC_SETDWORD(My_Slave[i].myTargetPosition, curDistance[i]);
                }

                break;
            case P2P: //点到点移动
                p2pCount[i]++;
                if ((p2pCount[i] == p2pTime) && (p2pCount[i] > 1))
                {
                    p2pCount[i] = 0;
                    EC_SETDWORD(My_Slave[i].myTargetPosition, angle_position[i]);
                    eMotionState[i] = EPause;
                    EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "P2P[%d] is Arrivaed \n", i));
                }
                else if (p2pCount[i] < p2pTime)
                {
                    curDistance[i] = curDistance[i] + wDistance[i];
                    EC_SETDWORD(My_Slave[i].myTargetPosition, curDistance[i]);
                }

                break;
            default:
            {
                OsDbgAssert(0);
            }
            break;
            }
        }
    }
}

/******************************************************************************
Function: Process_Switch
Description: send the command for COMMAND_STOP & COMMAND_START
Called By: DCDemo
Return: EC_E_NOERROR
*******************************************************************************/
EC_T_VOID Process_Switch(EC_T_INT AxisNum,
                         EC_T_DWORD command)
{
    if (AxisNum < MotorCount)
    {
        switch (command)
        {
        case COMMAND_START:
            S_ProcessState[AxisNum] = COMMAND_START;
            break;
        case COMMAND_STOP:
            S_ProcessState[AxisNum] = COMMAND_STOP;
            break;
        }
    }
}

EC_T_DWORD Process_SetModOperation(
    EC_T_WORD ControlMode)
{

    static EC_T_DWORD *dwCommand = S_ProcessState;

    for (EC_T_INT mIndex = 0; mIndex < MotorCount; mIndex++)
    {
        if (My_Slave[mIndex].myModeOfOperation != EC_NULL)
        {
            if (dwCommand[mIndex] == COMMAND_START)
            {
                if (ControlMode == OPMODE_CSP)
                {
                    My_Slave[mIndex].byModesOfOperation = DRV_MODE_OP_CSP;
                }
                else if (ControlMode == OPMODE_CSV)
                {
                    My_Slave[mIndex].byModesOfOperation = DRV_MODE_OP_CSV;
                }
                else if (ControlMode == OPMODE_CST)
                {
                    My_Slave[mIndex].byModesOfOperation = DRV_MODE_OP_CST;
                }
            }

            EC_SETWORD(My_Slave[mIndex].myModeOfOperation, My_Slave[mIndex].byModesOfOperation);
        }
    }

    return EC_E_NOERROR;
}

/******************************************************************************
Function:Process_Commands
Description: instand for Demo_run and Demo_stop
Called By: DCDemo
Return: EC_E_NOERROR
*******************************************************************************/
EC_T_DWORD Process_Commands(
    T_DEMO_THREAD_PARAM *pDemoThreadParam)
{
    static EC_T_DWORD S_dwStatus = 0;
    static EC_T_WORD wControl[MAXDEVICES];
    static EC_T_SDWORD curDistance[MAXDEVICES];
    static EC_T_DWORD *dwCommand = S_ProcessState;

    EC_UNREFPARM(pDemoThreadParam);

    for (EC_T_INT mIndex = 0; mIndex < MotorCount; mIndex++)
    {
        if (My_Slave[mIndex].myStatusWord != EC_NULL)
        {
            S_dwStatus = EC_GETWORD(My_Slave[mIndex].myStatusWord);

            if (dwCommand[mIndex] == COMMAND_START)
            {
                if ((S_dwStatus & 0x8) == DRV_STAT_FAULT)
                {
                    wControl[mIndex] = DRV_CRTL_FAULT_RESET;
                    My_Mem_Status.statusWord[mIndex] = 2;
                }
                else
                {
                    switch (My_Slave[mIndex].wState)
                    {
                    case DRV_DEV_STATE_NOT_READY:
                        curDistance[mIndex] = EC_GETDWORD(My_Slave[mIndex].myActPosition);
                        EC_SETDWORD(My_Slave[mIndex].myTargetPosition, curDistance[mIndex]);

                        if ((S_dwStatus & 0x4F) == 0x00 || (S_dwStatus & 0x4F) == DRV_STAT_SWITCH_ON_DIS || (S_dwStatus & 0x4F) == (DRV_STAT_SWITCH_ON_DIS | DRV_STAT_QUICK_STOP))
                        {
                            EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Slave[%d] To Switch On Disabled %04x\n", mIndex, S_dwStatus));
                            wControl[mIndex] = DRV_CTRL_CMD_SHUTDOWN;
                            My_Slave[mIndex].wState = DRV_DEV_STATE_SWITCHON_DIS;
                        }
                        else if (S_dwStatus & DRV_STAT_FAULT)
                        {
                            EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Slave[%d] To Malfunction %04x\n", mIndex, S_dwStatus));
                            wControl[mIndex] = DRV_CRTL_FAULT_RESET;
                            My_Slave[mIndex].wState = DRV_DEV_STATE_MALFCT_REACTION;
                        }
                        else if ((S_dwStatus & DRV_STAT_RDY_SWITCH_ON) == DRV_STAT_RDY_SWITCH_ON)
                        {
                            My_Slave[mIndex].wState = DRV_DEV_STATE_SWITCHON_DIS;
                        }
                        break;
                    case DRV_DEV_STATE_SWITCHON_DIS:
                        if ((S_dwStatus & 0x4F) == DRV_STAT_SWITCH_ON_DIS)
                        {
                            EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Slave[%d] To Ready to Switch On %04x\n", mIndex, S_dwStatus));
                            wControl[mIndex] = DRV_CTRL_CMD_SHUTDOWN;
                            My_Slave[mIndex].wState = DRV_DEV_STATE_READY_TO_SWITCHON;
                        }
                        else if ((S_dwStatus & 0x4F) == DRV_STAT_RDY_SWITCH_ON)
                        {
                            My_Slave[mIndex].wState = DRV_DEV_STATE_READY_TO_SWITCHON;
                        }
                        else if ((S_dwStatus & 0x69) == (DRV_STAT_QUICK_STOP | DRV_STAT_RDY_SWITCH_ON))
                        {
                            My_Slave[mIndex].wState = DRV_DEV_STATE_SWITCHED_ON;
                        }
                        else if (S_dwStatus & DRV_STAT_FAULT)
                        {
                            My_Slave[mIndex].wState = DRV_DEV_STATE_MALFUNCTION;
                        }
                        break;
                    case DRV_DEV_STATE_READY_TO_SWITCHON:
                        if ((S_dwStatus & 0x6F) == (DRV_STAT_QUICK_STOP | DRV_STAT_RDY_SWITCH_ON))
                        {
                            EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Slave[%d] To Switched On %04x\n", mIndex, S_dwStatus));
                            wControl[mIndex] = DRV_CTRL_CMD_SWITCHON;
                            My_Slave[mIndex].wState = DRV_DEV_STATE_SWITCHED_ON;
                        }
                        break;
                    case DRV_DEV_STATE_SWITCHED_ON:
                        if ((S_dwStatus & 0x6F) == (DRV_STAT_QUICK_STOP | DRV_STAT_SWITCHED_ON | DRV_STAT_RDY_SWITCH_ON))
                        {
                            EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Slave[%d] To Operation Enabled %04x\n", mIndex, S_dwStatus));
                            wControl[mIndex] = DRV_CTRL_CMD_ENA_OPERATION;
                            My_Slave[mIndex].wState = DRV_DEV_STATE_OP_ENABLED;
                        }
                        break;
                    case DRV_DEV_STATE_OP_ENABLED:
                        if ((S_dwStatus & 0x7F) == (DRV_STAT_QUICK_STOP | DRV_STAT_VOLTAGE_ENABLED | DRV_STAT_OP_ENABLED | DRV_STAT_SWITCHED_ON | DRV_STAT_RDY_SWITCH_ON))
                        {
                            //LogMsg("Slave[%d] In Operation Enabled %04x\n", mIndex, S_dwStatus);
                            My_Mem_Status.statusWord[mIndex] = 1;
                        }
                        break;
                    case DRV_DEV_STATE_MALFCT_REACTION:
                        EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Slave[%d] To No Ready %04x\n", mIndex, S_dwStatus));
                        wControl[mIndex] = DRV_CRTL_FAULT_RESET;
                        My_Slave[mIndex].wState = DRV_DEV_STATE_NOT_READY;
                        break;
                    default:
                        EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Motrotech: State: %d, Status: %04x\n", My_Slave[mIndex].wState, S_dwStatus));
                        break;
                    }
                }
            }
            else if (dwCommand[mIndex] == COMMAND_STOP)
            {
                EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "Slave[%d] To Switch On Disabled %04x\n", mIndex, S_dwStatus));

                wControl[mIndex] = DRV_CTRL_CMD_SHUTDOWN;          //0x00;
                My_Slave[mIndex].wState = DRV_DEV_STATE_NOT_READY; //state_ready_to_switchon;
                My_Mem_Status.statusWord[mIndex] = 0;
                dwCommand[mIndex] = COMMAND_NONE;
            }

            if (My_Slave[mIndex].myControlWord != EC_NULL)
            {
                EC_SETWORD(My_Slave[mIndex].myControlWord, wControl[mIndex]);
            }
        }
        else
        {
            //LogMsg("Wrong State: %d\n", My_Slave[mIndex].myStatusWord);
        }
    }
    return EC_E_NOERROR;
}

/*-----------------------------end----------------------------------*/
