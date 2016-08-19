#include <string.h>
#include "JY901.h"
struct STime		stcTime={0};
struct SAcc 		stcAcc={0};
struct SGyro 		stcGyro={0};
struct SAngle 		stcAngle={0};
struct SMag 		stcMag={0};
struct SDStatus 	stcDStatus={0};
struct SPress 		stcPress={0};
struct SLonLat 		stcLonLat={0};
struct SGPSV 		stcGPSV={0};

void CharToLong(char Dest[],char Source[])
{
	 *Dest 		= Source[3];
	 *(Dest+1) 	= Source[2];
	 *(Dest+2) 	= Source[1];
	 *(Dest+3) 	= Source[0];
}
void CopeSerialData(unsigned char ucData)
{
	static unsigned char ucRxBuffer[12];
	static unsigned char ucRxCnt = 0;	
	
	ucRxBuffer[ucRxCnt++]=ucData;
	if (ucRxBuffer[0]!=0x55) //数据头不对，则重新开始寻找0x55数据头
	{
		ucRxCnt=0;
		return;																																	  
	}
	if (ucRxCnt<11) {return;}//数据不满11个，则返回
	else
	{
		switch(ucRxBuffer[1])
		{
			case 0x50: stcTime.ucYear 		= ucRxBuffer[2];
						stcTime.ucMonth 	= ucRxBuffer[3];
						stcTime.ucDay 		= ucRxBuffer[4];
						stcTime.ucHour 		= ucRxBuffer[5];
						stcTime.ucMinute 	= ucRxBuffer[6];
						stcTime.ucSecond 	= ucRxBuffer[7];
						stcTime.usMiliSecond=((unsigned short)ucRxBuffer[9]<<8)|ucRxBuffer[8];
						break;
			case 0x51:	stcAcc.a[0] = ((unsigned short)ucRxBuffer[3]<<8)|ucRxBuffer[2];
						stcAcc.a[1] = ((unsigned short)ucRxBuffer[5]<<8)|ucRxBuffer[4];
						stcAcc.a[2] = ((unsigned short)ucRxBuffer[7]<<8)|ucRxBuffer[6];
						break;
			case 0x52:	stcGyro.w[0] = ((unsigned short)ucRxBuffer[3]<<8)|ucRxBuffer[2];
						stcGyro.w[1] = ((unsigned short)ucRxBuffer[5]<<8)|ucRxBuffer[4];
						stcGyro.w[2] = ((unsigned short)ucRxBuffer[7]<<8)|ucRxBuffer[6];
						break;
			case 0x53:	stcAngle.Angle[0] = ((unsigned short)ucRxBuffer[3]<<8)|ucRxBuffer[2];
						stcAngle.Angle[1] = ((unsigned short)ucRxBuffer[5]<<8)|ucRxBuffer[4];
						stcAngle.Angle[2] = ((unsigned short)ucRxBuffer[7]<<8)|ucRxBuffer[6];
						stcAngle.T = ((unsigned short)ucRxBuffer[9]<<8)|ucRxBuffer[8];
						break;
			case 0x54:	stcMag.h[0] = ((unsigned short)ucRxBuffer[3]<<8)|ucRxBuffer[2];
						stcMag.h[1] = ((unsigned short)ucRxBuffer[5]<<8)|ucRxBuffer[4];
						stcMag.h[2] = ((unsigned short)ucRxBuffer[7]<<8)|ucRxBuffer[6];
						stcAngle.T = ((unsigned short)ucRxBuffer[9]<<8)|ucRxBuffer[8];
						break;
			case 0x55:	stcDStatus.sDStatus[0] = ((unsigned short)ucRxBuffer[3]<<8)|ucRxBuffer[2];
						stcDStatus.sDStatus[1] = ((unsigned short)ucRxBuffer[5]<<8)|ucRxBuffer[4];
						stcDStatus.sDStatus[2] = ((unsigned short)ucRxBuffer[7]<<8)|ucRxBuffer[6];
						stcDStatus.sDStatus[3] = ((unsigned short)ucRxBuffer[9]<<8)|ucRxBuffer[8];
						break;
			case 0x56:	ucRxBuffer[2] = 0x12;ucRxBuffer[3] = 0x34;ucRxBuffer[4] = 0x56;ucRxBuffer[5] = 0x78;
						CharToLong((char*)&stcPress.lPressure,(char*)&ucRxBuffer[2]);
						CharToLong((char*)&stcPress.lAltitude,(char*)&ucRxBuffer[6]);
						break;
			case 0x57:	CharToLong((char*)&stcLonLat.lLon,(char*)&ucRxBuffer[2]);
						CharToLong((char*)&stcLonLat.lLat,(char*)&ucRxBuffer[6]);
						break;
			case 0x58:	stcGPSV.sGPSHeight = ((unsigned short)ucRxBuffer[3]<<8)|ucRxBuffer[2];
						stcGPSV.sGPSYaw = ((unsigned short)ucRxBuffer[5]<<8)|ucRxBuffer[4];
						CharToLong((char*)&stcGPSV.lGPSVelocity,(char*)&ucRxBuffer[6]);
						break;
		}
		ucRxCnt=0;
	}
}
