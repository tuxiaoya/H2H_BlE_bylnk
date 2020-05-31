
#ifndef H2H_blynk_H
#define H2H_blynk_H

// blynk V引脚 定义
#define VirPort_ONbutton V0
#define VirPort_Terminal V1
#define VirPort_ObjTemp V5
#define VirPort_AmbTemp V6
#define VirPort_GETParameters V10
#define VirPort_SETParameters V11
#define VirPort_ParHwSG_radiant V20 //  发射率坡度  9.9   -9.9    20%--20%
#define VirPort_ParHwSG_420mARate V21   //  4-20MA 微调  9.9%   -9.9%
#define VirPort_ParHwSG_4mAStartPoint V22 //  X100
#define VirPort_ParHwSG_20mAENDtPoint V23  //   X100
#define VirPort_ParHwSG_TEMUPLimit V24    //   X100
#define VirPort_ParHwSG_TEMDOWNLimit V25  //   X100 
#define VirPort_ParHwSG_DisUpdatePeriod V26 //
#define VirPort_ParHwSG_DisStayPeriod V27   //  0.1-9.9
#define VirPort_ParHwSG_AntiBaseLine V28    //  20-40
#define VirPort_ParHwSG_OverSignalline V29 //
#define VirPort_ParHwSG_GapIn1Sec V30       //  Gap limit of thermometricbase in one second
#define VirPort_ParHwSG_UartID V31          //  0-F// bool HwSGsetup6_LockBit;            //  true or  faule


#endif