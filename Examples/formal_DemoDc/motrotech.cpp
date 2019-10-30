/*-------------------------------motrotech----------------------------------
 * motrotech.cpp            
 * Copyright                motrotech
 * Response                 John Zhang
 * Description              Servo drive control
 * Version                  V6.0
 *---------------------------------------------------------------------------*/
//#define MTRACE
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
}
/*-DEFINES-------------------------------------------------------------------*/
#define SLAVE_NOT_FOUND 0xFFFF

#define MBX_TIMEOUT 5000

#define MOTOR_TESTING

#define MY_MEM_DATA_BUFFER_NUMBER (5)

//////////////////////////////////////////////////////////wzz 2019/6/11
#define pEcLogContext (G_pEcLogParms->pLogContext)
#define dwEcLogLevel G_pEcLogParms->dwLogLevel
#define pLogMsgCallback G_pEcLogParms->pfLogMsg
#define EC_LOG_LEVEL_INFO 4
/////////////////////////////////////////////////////////

/*-GLOBAL VARIABLES-----------------------------------------------------------*/
My_Slave_Type My_Slave[MAX_SERVO_NUM];
EC_T_SDWORD first_analysis_cycle = 1;    //一次解析周期
EC_T_SDWORD second_analysis_nummber = 1; //二次解析个数
my_mem_data_t My_Mem_Data;
my_mem_status_t My_Mem_Status;
my_mem_status_t My_Mem_Status_Buf;
my_mem_cmd_t My_Mem_Cmd;
KySemInfo_t sinfo_data;
KySemInfo_t sinfo_status;
KySemInfo_t sinfo_cmd;
/*-LOCAL VARIABLES-----------------------------------------------------------*/
static EC_T_INT MotorCount = 0;

static EC_T_DWORD S_ProcessState[12];

/*-FUNCTION DEFINITIONS------------------------------------------------------*/

/******************************************************************************
Function: myInit
Description: Initialize variables
Called By: myAppInit
Return: EC_E_NOERROR
*******************************************************************************/
EC_T_DWORD myInit(T_DEMO_THREAD_PARAM *pDemoThreadParam)
{
    EC_T_DWORD result;
    EC_UNREFPARM(pDemoThreadParam);
    OsMemset(My_Slave, 0, MAX_SERVO_NUM * sizeof(My_Slave_Type));
    OsMemset(&My_Mem_Data, 0, sizeof(my_mem_data_t));
    OsMemset(&My_Mem_Status, 0, sizeof(my_mem_status_t));
    OsMemset(&My_Mem_Cmd, 0, sizeof(my_mem_cmd_t));
    OsMemset(&My_Mem_Status_Buf, 0, sizeof(my_mem_status_t));
    for (EC_T_DWORD temp = 0; temp < 18; temp++)
    {
        My_Slave[temp].wState = DRV_DEV_STATE_NOT_READY;
    }
    result = Ky_semMemNew(&sinfo_status, Ky_etherCat_State, 1, sizeof(my_mem_status_t), MY_MEM_DATA_BUFFER_NUMBER);
    if (result != SUCCESS)
    {
    }
    else
    {
        Ky_semMemClear(&sinfo_status, 1);
    }
    result = Ky_semMemNew(&sinfo_data, Ky_etherCat_Data, 1, sizeof(my_mem_data_t), MY_MEM_DATA_BUFFER_NUMBER);
    if (result != SUCCESS)
    {
    }
    else
    {
        Ky_semMemClear(&sinfo_data, 1);
    }
    result = Ky_semMemNew(&sinfo_cmd, Ky_etherCat_Cmd, 1, sizeof(my_mem_cmd_t), MY_MEM_DATA_BUFFER_NUMBER);
    if (result != SUCCESS)
    {
    }
    else
    {
        Ky_semMemClear(&sinfo_cmd, 1);
    }

    return EC_E_NOERROR;
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
    static eMoveState eMotionState[6] = {EPause, EPause, EPause, EPause, EPause, EPause};
    static EC_T_DWORD nWaitCounter1[6];
    static EC_T_DWORD eDistanceState[6];
    static EC_T_SDWORD Distance[6];
    static EC_T_SDWORD curDistance[6];
    static EC_T_SDWORD powerState[6];
    static EC_T_SDWORD wVelocity[6];
    static EC_T_SDWORD wDistance;
    static EC_T_INT waitCount = 0;
    EC_T_INT result;
    EC_UNREFPARM(pDemoThreadParam);
    EC_UNREFPARM(pbyPDIn);
    EC_UNREFPARM(pbyPDOut);

    waitCount++;
    if (waitCount >= first_analysis_cycle)
    {
        //读取CMD
        result = Ky_semMemRead(&sinfo_cmd, &My_Mem_Cmd, sizeof(my_mem_cmd_t), 1);
        if (result != SUCCESS)
        {
        }
        else
        {
            //EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "My_Mem_Cmd.cmdWord[5]:%d ,My_Mem_Cmd.move[5]:%d \n", My_Mem_Cmd.moveState[5]));
            for (EC_T_INT i = 0; i < MotorCount; i++)
            {
                if (dwCommand[i] == COMMAND_START)
                {
                    if (My_Mem_Cmd.cmdWord[i] == 0)
                    {
                        Process_Switch(i, COMMAND_STOP);
                        // EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "switch on[%d]:%d\n", i, My_Mem_Cmd.cmdWord[i]));
                    }
                }
                else
                {
                    if (My_Mem_Cmd.cmdWord[i] == 1)
                    {
                        Process_Switch(i, COMMAND_START);
                        // EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "switch on[%d]:%d\n", i, My_Mem_Cmd.cmdWord[i]));
                    }
                }
                eMotionState[i] = (enum eMoveState)My_Mem_Cmd.moveState[i];
            }
        }
        for (EC_T_INT i = 0; i < MotorCount; i++)
        {           
            if (My_Slave[i].myActPosition != NULL)
            {
                My_Mem_Status.axis_Theta[i] = EC_GETDWORD(My_Slave[i].myActPosition);
                My_Mem_Status.axis_dTheta[i] = EC_GETDWORD(My_Slave[i].myActVelocity);
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
        //读取DATA
        result = Ky_semMemRead(&sinfo_data, &My_Mem_Data, sizeof(my_mem_data_t), 1);
        if (result != SUCCESS)
        {
        }
        else
        {
            first_analysis_cycle=My_Mem_Data.deltaT;
            second_analysis_nummber=My_Mem_Data.deltaNumber;
            // EcLogMsg(EC_LOG_LEVEL_INFO, (pEcLogContext, EC_LOG_LEVEL_INFO, "axis_Theta:%x %x %x %x %x %x", My_Mem_Data.axis_Theta[0], My_Mem_Data.axis_Theta[1], My_Mem_Data.axis_Theta[2], my_mem_data_t.axis_Theta[3], my_mem_data_t.axis_Theta[4], my_mem_data_t.axis_Theta[5]));
            for (EC_T_INT i = 0; i < MotorCount; i++)
            {
                //curDistance[i] = My_Mem_Data.axis_Theta[i];
                wVelocity[i]= My_Mem_Data.axis_dTheta[i];

            }
        }

        //SeondaryPlanning(Test_v0, Test_vt, Test_s0, Test_st, TestV[0]);
        waitCount = 0;
    }

    for (EC_T_INT i = 0; i < MotorCount; i++)
    {

        if (My_Slave[i].myTargetPosition != NULL)
        {
            My_Slave_Type *pDemoAxis = &My_Slave[i];

            if ((pDemoAxis != EC_NULL) && (My_Slave[i].wState == DRV_DEV_STATE_OP_ENABLED))
            {
                /* we are either in state "move setup", "moving" or "pause" */
                switch (eMotionState[i])
                {
                case EMoveSetup: //测试反转
                {
                    curDistance[i] = EC_GETDWORD(My_Slave[i].myActPosition);
                    curDistance[i] -= wVelocity[i]*0.0005;
                    EC_SETDWORD(My_Slave[i].myTargetPosition, curDistance[i]);
                }
                break;

                case EMoving: //测试正转
                {
                    curDistance[i] = EC_GETDWORD(My_Slave[i].myActPosition);
                    curDistance[i] += wVelocity[i]*0.0005;
                    EC_SETDWORD(My_Slave[i].myTargetPosition, curDistance[i]);
                    //eMotionState[i]=EPause;
                }
                break;

                case EPause: //测试暂停

                    break;

                case Homing: //测试复位
                    curDistance[i] = EC_GETDWORD(My_Slave[i].myActPosition);
                    if (curDistance[i] < 300 && curDistance[i] > -300)
                    {
                        eMotionState[i] = EPause;
                    }
                    else
                    {
                        if (curDistance[i] > 0)
                            curDistance[i] -= 100;
                        else
                            curDistance[i] += 100;
                    }
                    EC_SETDWORD(My_Slave[i].myTargetPosition, curDistance[i]);

                    break;
                default:
                {
                    OsDbgAssert(0);
                }
                break;
                }
            }
        } /* loop through axis list */
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
    static EC_T_WORD wControl[6];
    static EC_T_SDWORD curDistance[6];
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
                    My_Mem_Status.statusWord[mIndex]=2;
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
                            My_Mem_Status.statusWord[mIndex]=1;
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
                My_Mem_Status.statusWord[mIndex]=0;
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
