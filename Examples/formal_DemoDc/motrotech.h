/*-----------------------------------------------------------------------------
 * motrotech.cpp            
 * Copyright                motrotech
 * Response                 Zhang Xiaopeng
 * Description              EtherCAT Master demo application
 * Version                  V6.0
 *---------------------------------------------------------------------------*/
#ifndef __MOTROTECH_H__
#define __MOTROTECH_H__     1

/*-INCLUDES------------------------------------------------------------------*/
#include "ecatNotification.h"
#include "ecatDemoCommon.h"
#include "SlaveInfo.h"
#include "EcLog.h"
/*-DEFINES-------------------------------------------------------------------*/
#define MOTROTECH_VERS_MAJ             0   /* major version */             
#define MOTROTECH_VERS_MIN             0   /* minor version */             
#define MOTROTECH_VERS_SERVICEPACK     6   /* service pack */           
#define MOTROTECH_VERS_BUILD           0   /* build number */   

#define MAX_SERVO_NUM           18

struct EC_T_MOTION_PARAMETER
{
   EC_T_REAL fCommandPos, fCommandVel, fCommandAcc, fCommandJerk;
   EC_T_REAL fFollowingError;
};

struct EC_T_LOGINFO
{
   EC_T_MOTION_PARAMETER curPos;
};

typedef struct _My_Slave_Type
{
     /*-PDO_OUTPUT-----------------------------------------*/
    EC_T_WORD	*myControlWord;		     // 0x6040 - ControlWord
    EC_T_SDWORD	*myTargetPosition;	     // 0x607A - TargetPosition
    EC_T_WORD   *myTouchProbeFunction;     //0x60B8  -Touch probe function
    EC_T_SDWORD *myTargetVelocity;        //60ff
    EC_T_SWORD   *myTargetTorque;         //6071
    EC_T_BYTE   *myModeOfOperation;      // 0x6060 - Mode of Operation
    EC_T_DWORD  *myMaxProfileVelocity;   //0x607F 

    /*-PDO_INPUT------------------------------------------*/

    EC_T_WORD	*myErrorCode;      // 0x603F - Error Code
    EC_T_WORD	*myStatusWord;	   // 0x6041 - StatusWord  
    EC_T_SDWORD	*myActPosition;	   // 0x6064 - ActualPosition
    EC_T_SDWORD	*myActVelocity;	   // 0x606C - ActVelocity
    EC_T_SWORD	*myActTorque;	   // 0x6077 - ActTorque
    
    EC_T_BYTE   *myModeOfOperationDisplay; //0x6061
    
//    EC_T_DWORD	*myActFollowErr;   // 0x60F4 - ActualFollowErr
    EC_T_WORD   *myTouchProbeStatus;// 0x60B9 - touch probe status
    EC_T_SDWORD  *myTouchProbePos1Value;//0x60BA-
    EC_T_SDWORD  *myTouchProbePos2Value;//0x60BC-
    EC_T_DWORD   *myInputs;        // 0x60FD/ - Input     
    
    EC_T_WORD   wState;
	  EC_T_WORD   wFixedAddress;
	  EC_T_BYTE   byModesOfOperation;

	  EC_T_CFG_SLAVE_INFO S_aSlaveList;
	  EC_T_LOGINFO        log;
} My_Slave_Type;

typedef struct _MotionPara_TYPE1
{
    EC_T_SDWORD       lTotalDist;             /* [Inc] Total distance  */

	EC_T_SDWORD          lStartVel;              /* [Inc/Cycle] Value of velocity and start of movement */
    EC_T_SDWORD         lConstantVel;           /* [Inc/Cycle] Value of the constant (middle) velocity */
    EC_T_SDWORD         lEndVel;                /* [Inc/Cycle] Value of velocity and end of movement */
 
    EC_T_SDWORD         lCommandVel;            /* [Inc/Cycle] commanded velocity */
    EC_T_SDWORD          lCommandAcc;            /* +/- [Inc/Cycle^2] commanded acceleration */

	EC_T_SDWORD         TargetPos;

	EC_T_DWORD          motorN;
	EC_T_DWORD          Seg_m1; /* Time Count of segment 1 */
	EC_T_DWORD          Seg_m2; /* Time Count of segment 2 */
	EC_T_DWORD          Seg_m3; /* Time Count of segment 3 */

	My_Slave_Type*      My_slave_motor;
	EC_T_DWORD          WorkCount;
}MotionPara_TYPE1;
 typedef struct _my_mem_data_t{
    EC_T_SDWORD axis_number;        //轴个数
    EC_T_SDWORD axis_Theta[16];     //轴位置
    EC_T_SDWORD axis_dTheta[16];    //轴速度
    EC_T_SDWORD axis_ddTheta[16];   //轴加速度
    EC_T_SDWORD offset[16];         //轴偏移
    EC_T_SDWORD deltaT;             //一次解析采样时间
    EC_T_SDWORD deltaNumber;        //二次解析细分点数
}my_mem_data_t;

typedef struct _my_mem_status_t{
    EC_T_SDWORD statusWord[16];        //伺服器状态机
    EC_T_SDWORD errorCode[16];         //错误代码

    EC_T_SDWORD axis_number;        //轴个数
    EC_T_SDWORD axis_Theta[16];     //轴位置
    EC_T_SDWORD axis_dTheta[16];    //轴速度
    EC_T_SDWORD axis_ddTheta[16];   //轴加速度
    EC_T_SDWORD offset[16];         //轴偏移
}my_mem_status_t;

typedef struct _my_mem_cmd_t{
    EC_T_SDWORD cmdWord[16];        //伺服器命令
    EC_T_SDWORD moveState[16];
}my_mem_cmd_t;
#define COMMAND_NONE            0
#define COMMAND_START	          1
#define COMMAND_RUN		          2 
#define COMMAND_STOP		        3

#define SERVO_VENDOR            ecvendor_sanyo_denki
#define SERVO_PRODUCT_CODE      0x2

/*DS402 object identifiers */

#define DRV_OBJ_ERROR_CODE                  0x603F
#define DRV_OBJ_CONTROL_WORD                0x6040//2 uint16
#define DRV_OBJ_STATUS_WORD                 0x6041//2 uint16
#define DRV_OBJ_MODES_OF_OPERATION          0x6060//1 
#define DRV_OBJ_INTERNEL_POS_ACTVAL         0x6063
#define DRV_OBJ_POSITION_ACTUAL_VALUE       0x6064//4 int 32
#define DRV_OBJ_VELOCITY_ACTUAL_VALUE       0x606C//4 int 32
#define DRV_OBJ_TARGET_TORQUE               0x6071//2 int16
#define DRV_OBJ_TORQUE_ACTUAL_VALUE         0x6077//2 int16
#define DRV_OBJ_TARGET_POSITION             0x607A//4 int32
#define DRV_OBJ_FOLLOWING_ERROR             0x60F4//4 int32
#define DRV_OBJ_MAX_PROFILE_VELOCITY        0x607F//4 uint 32

#define DRV_OBJ_TOUCH_PROBE_FUNCTION         0x60B8//2 u16
#define DRV_OBJ_MODE_OF_DISPLAY              0x6061//1 
#define DRV_OBJ_TOUCH_PROBE_STATUS           0x60B9//2 u 16
#define DRV_OBJ_TOUCH_PROBE_POS1_VALUE       0x60BA//4 int32
#define DRV_OBJ_TOUCH_PROBE_POS2_VALUE       0x60BC//4 int32

#define DRV_OBJ_DIGITAL_INPUT              0x60FD//4 uint32
//#define DRV_OBJ_DIGITAL_INPUT_SUBINDEX_1   0x1
//#define DRV_OBJ_DIGITAL_INPUT_SUBINDEX_2   0x2

#define DRV_OBJ_DIGITAL_OUTPUT              0x60FE//
#define DRV_OBJ_DIGITAL_OUTPUT_SUBINDEX_1   0x1//4 uint32
#define DRV_OBJ_DIGITAL_OUTPUT_SUBINDEX_2   0x2//4

/* In cyclic synchronous position mode, this object contains the input
value for velocity feed forward. In cyclic synchronous velocity mode it
contains the commanded offset of the drive device. */
#define DRV_OBJ_VELOCITY_OFFSET             0x60B1

/* In cyclic synchronous position mode and cyclic synchronous velocity
mode, this object contains the input value for torque feed forward. In cyclic
synchronous torque mode it contains the commanded additive torque of the
drive, which is added to the target torque value. */
#define DRV_OBJ_TORQUE_OFFSET               0x60B2

/* This object shall indicate the configured target velocity and shall be used as input for the
trajectory generator. */
#define DRV_OBJ_TARGET_VELOCITY             0x60FF

/* DS402 object 0x6040: Control word */
#define DRV_CRTL_SWITCH_ON          0x0001          /* Bit 0: */
#define DRV_CRTL_ENABLE_VOLTAGE     0x0002          /* Bit 1: */
#define DRV_CRTL_QUICK_STOP         0x0004          /* Bit 2: */
#define DRV_CRTL_ENABLE_OP          0x0008          /* Bit 3: */
#define DRV_CTRL_INTER_POS_ENA      0x0010          /* Bit 4: Interpolated position mode: enable interpolation */
#define DRV_CRTL_FAULT_RESET        0x0080          /* Bit 7: */
#define DRV_CRTL_HALT               0x0100          /* Bit 8: */
#define DRV_CRTL_OP_MODE_SPEC       0x0200          /* Bit 9: */
#define DRV_CRTL_RES_10             0x0400          /* Bit 10: */
#define DRV_CRTL_MANU_SPEC          0xF800          /* Bit 11-15: */
/* DS402 drive/device control commands */
#define DRV_CTRL_CMD_MASK               0x008F          /* Control commands Mask */
#define DRV_CTRL_CMD_SHUTDOWN           0x06          /* Shutdown (Transition 2, 6, 8) */
#define DRV_CTRL_CMD_SWITCHON           0x07          /* Switch On (Transition 3) */
#define DRV_CTRL_CMD_DIS_VOLTAGE        0x00          /* Disable Voltage (Transition 7, 9, 10, 12) */
#define DRV_CTRL_CMD_DIS_VOLTAGE_MASK   0x82          /* Disable Voltage Mask */
#define DRV_CTRL_CMD_QUICK_STOP         0x02          /* Quick Stop (Transition 7, 10, 11) */
#define DRV_CTRL_CMD_QUICK_STOP_MASK    0x86          /* Disable Voltage Mask */
#define DRV_CTRL_CMD_DIS_OPERATION      0x07          /* Disable Operation (Transition 5) */
#define DRV_CTRL_CMD_ENA_OPERATION      0x0F          /* Enable Operation (Transition 4) */
#define DRV_CTRL_CMD_RESET_MALFCT       DRV_CRTL_FAULT_RESET          /* Reset Malfunction (0->1 edge ) (Transition 15) */

/* DS402 object 0x6041: Status word */
#define DRV_STAT_RDY_SWITCH_ON          0x0001          /* Bit 0: Ready to switch on */
#define DRV_STAT_SWITCHED_ON            0x0002          /* Bit 1: Switched On */
#define DRV_STAT_OP_ENABLED             0x0004          /* Bit 2: Operation enabled */
#define DRV_STAT_FAULT                  0x0008          /* Bit 3: Fault */
#define DRV_STAT_VOLTAGE_ENABLED        0x0010          /* Bit 4: Optional bit: Voltage enabled */
#define DRV_STAT_QUICK_STOP             0x0020          /* Bit 5: Optional bit: Quick stop      */
#define DRV_STAT_SWITCH_ON_DIS          0x0040          /* Bit 6: Switch on disabled */
#define DRV_STAT_STATUS_TOGGLE          0x0400          /* Bit 10: Optional bit: Status toggle (csp, csv mode) */
#define DRV_STAT_VELOCITY_ZERO          0x0400          /* Bit 10: Optional bit: Velocity 0 (ip mode) */
#define DRV_STAT_OP_MODE_CSP            0x1000          /* Bit 12: Optional bit: CSP drive follows the command value */
#define DRV_STAT_FOLLOW_ERR             0x2000          /* Bit 13: Optional bit: Following error (csp, csv mode) */
#define DRV_STAT_RUNNING                0x4000          /* Bit 14: Running */
#define DRV_STAT_IDLE                   0x8000          /* Bit 15: Idle */


/*-ENUMS---------------------------------------------------------------------*/
enum eMoveState
{
   EMoveSetup,
   EMoving,
   EPause,
   Homing
};

/* DS402 modes of operation 0x6060 */
enum MC_T_CIA402_OPMODE
{
   DRV_MODE_OP_PROF_POS            = 1,          /* profile position mode */
   DRV_MODE_OP_VELOCITY            = 2,          /* velocity mode (frequency converter) */
   DRV_MODE_OP_PROF_VEL            = 3,          /* profile velocity mode */
   DRV_MODE_OP_PROF_TOR            = 4,          /* profile torque mode */

   DRV_MODE_OP_HOMING              = 6,          /* homing mode */
   DRV_MODE_OP_INTER_POS           = 7,          /* interpolated position mode */
   DRV_MODE_OP_CSP                 = 8,          /* cyclic synchronous position mode */
   DRV_MODE_OP_CSV                 = 9,          /* cyclic synchronous velocity mode */
   DRV_MODE_OP_CST                 = 10          /* cyclic synchronous torque   mode */
};

/* DS402 device control (state machine) states */
enum MC_T_CIA402_STATE
{
   DRV_DEV_STATE_NOT_READY         = 0,          /* Not ready to switch on : Status Word x0xx 0000 */
   DRV_DEV_STATE_SWITCHON_DIS      = 1,          /* Switch on disabled     : Status Word x1xx 0000 */
   DRV_DEV_STATE_READY_TO_SWITCHON = 2,          /* Ready to switch on     : Status Word x01x 0001 */
   DRV_DEV_STATE_SWITCHED_ON       = 3,          /* Switched on            : Status Word x011 0011 */
   DRV_DEV_STATE_OP_ENABLED        = 4,          /* Operation enabled      : Status Word x011 0111 */
   DRV_DEV_STATE_QUICK_STOP        = 5,          /* Quick stop active      : Status Word 0000 0111 */
   DRV_DEV_STATE_MALFCT_REACTION   = 6,          /* Malfunction/Fault reaction active Status Word (xxxx 1111) oder (xx10 1111) */
   DRV_DEV_STATE_MALFUNCTION       = 7           /* Malfunction/Fault                 */
};


/* Internal operation modes */
enum MC_T_OPMODE
{
   OPMODE_NOTSET = 0,
   OPMODE_CSP    = 1,         /* Position control / Lageregelung */
   OPMODE_CSV    = 2,         /* Velocity control / Geschwindigkeitsregelung */
   OPMODE_CST    = 3          /* Torque control / Momentenregelung */
};


EC_T_DWORD myInit (T_DEMO_THREAD_PARAM* pDemoThreadParam);

EC_T_DWORD myPrepare (
    T_DEMO_THREAD_PARAM* pDemoThreadParam, 
    EC_T_DWORD dwVendorId, 
    EC_T_DWORD dwProductCode);

EC_T_DWORD mySetup (T_DEMO_THREAD_PARAM* pDemoThreadParam, EC_T_BYTE* pbyPDIn,EC_T_BYTE* pbyPDOut); 

EC_T_VOID myWorkpd (
    T_DEMO_THREAD_PARAM*       pDemoThreadParam,          /* [in]  Logging instance */ 
    EC_T_BYTE*          pbyPDIn,        /* [in]  pointer to process data input buffer */
    EC_T_BYTE*          pbyPDOut        /* [in]  pointer to process data output buffer */
    );

EC_T_DWORD Process_Commands (T_DEMO_THREAD_PARAM* pDemoThreadParam);
EC_T_VOID Process_Switch ( EC_T_INT AxisNum, EC_T_DWORD command );
EC_T_DWORD Process_SetModOperation(  EC_T_WORD ControlMode );
EC_T_DWORD Error_Report(CAtEmLogging* poLog, EC_T_INT nVerbose);

#endif /* INC_MOTROTECH */
/*-END OF SOURCE FILE--------------------------------------------------------*/



/*Process for Driver the servo device

Step 1: init the variable
COMMITE:init the variable in myAppInit()

    myInit(poLog,S_aSlaveList);

Step 2: scan the special motor
COMMITE: implement the my Prepare in myAppPrepare()

    myPrepare(poLog, 0x1b9, 0x2,        &S_dwAppFoundSlaves, S_aSlaveList);        //scan the sanyo motor

Step 3: Setup the pointer for DS402 index
COMMITE: implement the mySetup in myAppSetup()

    mySetup(S_aSlaveList, ecatGetProcessImageInputPtr(), ecatGetProcessImageOutputPtr());

Step 4: Process the DS402 state machine in myAppWorkpd
COMMITE: process the state machine, and Target Position in myAppWorkpd()

    Process_Commands(poLog, nVerbose, &S_ProcessState);

    myWorkpd(poLog, nVerbose, pbyPDIn, pbyPDOut);

Step 5: run the Motor

    // ****************
    // * run the demo *
    // ****************
    S_ProcessState = COMMAND_START;
 
    if (dwDuration != 0)
    {
        oTimeout.Start(dwDuration);
    }


Step 6: Pause the Motor

Exit:
    if (0 != nVerbose) LogMsg( "\n========================" );
    if (0 != nVerbose) LogMsg( "Shutdown EtherCAT Master" );
    if (0 != nVerbose) LogMsg( "========================" );

    S_ProcessState = COMMAND_STOP;
*/
