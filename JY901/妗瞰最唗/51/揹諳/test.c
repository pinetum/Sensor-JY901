//51单片机读取串口MPU6050模块的实例程序
//            BY:君悦智控
//   http://RobotControl.taobao.com
//测试平台：
//		51单片机开发板YL-39,芯片STC89C52
//注意:
//    1.51单片机的下载串口只有一个，同时又需要接MPU6050，因此在下载的时候需要拔下MPU6050的TX线，等程序下载成功以后再插上。
//接线方法：
//     51单片机		  JY901模块
//		+5V		----	VCC
//		TX		(不接)	RX
//		RX		----	TX
//		GND		----	GND
///////////////////////////////////////////////////////
/*预处理命令*/
#include <reg52.h> 		  //包含单片机寄存器的头文件
#include <stdio.h>
#include "JY901.h"

void delay_ms(unsigned short i)
{
unsigned short k;
	while(i--)
	for (k=0;k<100;k++); 
}

void main(void)
{
	unsigned char i=0;
	TMOD=0x20;		   //用定时器设置串口波特率	   9600 
	TH1=0xfd;
	TL1=0xfd;
	TR1=1;
	TI=1;
	REN=1;          //串口初始化
	SM0=0;
	SM1=1;
	EA=1;           //开启总中断
	ES=1;
	printf("STC89S52 Read JY901 module demo\r\n");
	printf("-------------BY:JYZK-------------\r\n");
	printf("---http://RobotControl.taobao.com---\r\n");
	while(1)
	{
		delay_ms(10);
		printf("Time:20%d-%d-%d %d:%d:%.3f\r\n",(short)stcTime.ucYear,(short)stcTime.ucMonth,
				(short)stcTime.ucDay,(short)stcTime.ucHour,(short)stcTime.ucMinute,(float)stcTime.ucSecond+(float)stcTime.usMiliSecond/1000);

		printf("Acc:%.3f %.3f %.3f\r\n",(float)stcAcc.a[0]/32768*16,(float)stcAcc.a[1]/32768*16,(float)stcAcc.a[2]/32768*16);

		printf("Gyro:%.3f %.3f %.3f\r\n",(float)stcGyro.w[0]/32768*2000,(float)stcGyro.w[1]/32768*2000,(float)stcGyro.w[2]/32768*2000);

		printf("Angle:%.3f %.3f %.3f\r\n",(float)stcAngle.Angle[0]/32768*180,(float)stcAngle.Angle[1]/32768*180,(float)stcAngle.Angle[2]/32768*180);

		printf("Mag:%d %d %d\r\n",stcMag.h[0],stcMag.h[1],stcMag.h[2]);

		printf("Pressure:%lx Height%.2f\r\n",stcPress.lPressure,(float)stcPress.lAltitude/100);

		printf("DStatus:%d %d %d %d\r\n",stcDStatus.sDStatus[0],stcDStatus.sDStatus[1],stcDStatus.sDStatus[2],stcDStatus.sDStatus[3]);

		printf("Longitude:%ldDeg%.5fm Lattitude:%ldDeg%.5fm\r\n",stcLonLat.lLon/10000000,(double)(stcLonLat.lLon % 10000000)/1e5,stcLonLat.lLat/10000000,(double)(stcLonLat.lLat % 10000000)/1e5);

		printf("GPSHeight:%.1fm GPSYaw:%.1fDeg GPSV:%.3fkm/h\r\n\r\n",(float)stcGPSV.sGPSHeight/10,(float)stcGPSV.sGPSYaw/10,(float)stcGPSV.lGPSVelocity/1000);
			
	}	
}
	  
void ser() interrupt 4
{
	if (RI)
	{	  
		RI=0;
		 CopeSerialData(SBUF);	    
	  }
	  
    
}