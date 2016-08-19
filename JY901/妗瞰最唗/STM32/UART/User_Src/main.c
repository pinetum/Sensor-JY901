/*
±àĞ´Õß£ºZhaowen
ÍøÖ·£ºhttp://RobotControl.taobao.com
×÷ÕßE-mail£ºzhaowenwin@139.com
±àÒë»·¾³£ºMDK-Lite  Version: 4.73
³õ°æÊ±¼ä: 2015-1-1
²âÊÔ£º STM32F103C8T6
¹¦ÄÜ£º½ÓÊÕGY-901Ä£¿é·¢ËÍµÄÊı¾İ£¬²¢ÓÃ´®¿Ú´òÓ¡³ö½âÎö³öÀ´µÄÊı¾İ£¬²¨ÌØÂÊ9600
		£STM32F103´®¿Ú1µÄRx½ÓJY-901µÄTx,ÓÃÓÚ½ÓÊÕJY-901Ä£¿éµÄÊı¾İ
			STM32F103´®¿Ú1µÄRx½ÓµçÄÔµÄUSB-TTLÄ£¿éµÄRx£¬ÓÃÓÚÏòµçÄÔ·¢ËÍ½âÎöÒÔºóµÄÊı¾İ¡£µçÄÔ¶ËÍ¨¹ı´®¿Úµ÷ÊÔÖúÊÖ²é¿´£¬²¨ÌØÂÊ9600£¬ASCIIÂë·½Ê½ÏÔÊ¾¡£
			Êı¾İ½âÎöÊ¹ÓÃ½á¹¹ÌåµÄ·½·¨½øĞĞ½âÎö¡£½á¹¹ÌåÔÚJY901.hÎÄ¼şÖĞ¶¨Òå

---------Ó²¼şÉÏµÄÒı½ÅÁ¬½Ó:----------
TTL½Ó¿Ú£º
PC(USB-TTL)			STM32F103						JY-901
RX			<-->	PA9(UART1-TXD)
							PA10(UART1-RXD)	<-->  	TX
------------------------------------
*/

#include "stm32f10x.h"
#include "UARTs.h"
#include "IOI2C.h"
#include "delay.h"
#include "JY901.h"
#include "string.h"

struct STime		stcTime;
struct SAcc 		stcAcc;
struct SGyro 		stcGyro;
struct SAngle 	stcAngle;
struct SMag 		stcMag;
struct SDStatus stcDStatus;
struct SPress 	stcPress;
struct SLonLat 	stcLonLat;
struct SGPSV 		stcGPSV;

//CopeSerialDataÎª´®¿ÚÖĞ¶Ïµ÷ÓÃº¯Êı£¬´®¿ÚÃ¿ÊÕµ½Ò»¸öÊı¾İ£¬µ÷ÓÃÒ»´ÎÕâ¸öº¯Êı¡£
void CopeSerialData(unsigned char ucData)
{
	static unsigned char ucRxBuffer[250];
	static unsigned char ucRxCnt = 0;	
	
	ucRxBuffer[ucRxCnt++]=ucData;
	if (ucRxBuffer[0]!=0x55) //Êı¾İÍ·²»¶Ô£¬ÔòÖØĞÂ¿ªÊ¼Ñ°ÕÒ0x55Êı¾İÍ·
	{
		ucRxCnt=0;
		return;
	}
	if (ucRxCnt<11) {return;}//Êı¾İ²»Âú11¸ö£¬Ôò·µ»Ø
	else
	{
		switch(ucRxBuffer[1])
		{
			case 0x50:	memcpy(&stcTime,&ucRxBuffer[2],8);break;//memcpyÎª±àÒëÆ÷×Ô´øµÄÄÚ´æ¿½±´º¯Êı£¬ĞèÒıÓÃ"string.h"£¬½«½ÓÊÕ»º³åÇøµÄ×Ö·û¿½±´µ½Êı¾İ¹²Í¬ÌåÀïÃæ£¬´Ó¶øÊµÏÖÊı¾İµÄ½âÎö¡£
			case 0x51:	memcpy(&stcAcc,&ucRxBuffer[2],8);break;
			case 0x52:	memcpy(&stcGyro,&ucRxBuffer[2],8);break;
			case 0x53:	memcpy(&stcAngle,&ucRxBuffer[2],8);break;
			case 0x54:	memcpy(&stcMag,&ucRxBuffer[2],8);break;
			case 0x55:	memcpy(&stcDStatus,&ucRxBuffer[2],8);break;
			case 0x56:	memcpy(&stcPress,&ucRxBuffer[2],8);break;
			case 0x57:	memcpy(&stcLonLat,&ucRxBuffer[2],8);break;
			case 0x58:	memcpy(&stcGPSV,&ucRxBuffer[2],8);break;
		}
		ucRxCnt=0;
	}
}

int main(void)
{
	char str[100];
	
  SystemInit();	/* ÅäÖÃÏµÍ³Ê±ÖÓÎª72M Ê¹ÓÃÍâ²¿8M¾§Ìå+PLL*/ 
	SysTick_init(72,10);		//ÑÓÊ±³õÊ¼»¯
	Initial_UART1(9600);
	delay_ms(1000);//µÈµÈJY-91³õÊ¼»¯Íê³É£
	while(1)
	{			
			delay_ms(500);
		sprintf(str,"Time:20%d-%d-%d %d:%d:%.3f\r\n",stcTime.ucYear,stcTime.ucMonth,stcTime.ucDay,stcTime.ucHour,stcTime.ucMinute,(float)stcTime.ucSecond+(float)stcTime.usMiliSecond/1000);
			UART1_Put_String(str);		
			delay_ms(10);//µÈ´ı´«ÊäÍê³É
		sprintf(str,"Acc:%.3f %.3f %.3f\r\n",(float)stcAcc.a[0]/32768*16,(float)stcAcc.a[1]/32768*16,(float)stcAcc.a[2]/32768*16);
			UART1_Put_String(str);
			delay_ms(10);//µÈ´ı´«ÊäÍê³É
		sprintf(str,"Gyro:%.3f %.3f %.3f\r\n",(float)stcGyro.w[0]/32768*2000,(float)stcGyro.w[1]/32768*2000,(float)stcGyro.w[2]/32768*2000);
			UART1_Put_String(str);
			delay_ms(10);//µÈ´ı´«ÊäÍê³É
		sprintf(str,"Angle:%.3f %.3f %.3f\r\n",(float)stcAngle.Angle[0]/32768*180,(float)stcAngle.Angle[1]/32768*180,(float)stcAngle.Angle[2]/32768*180);
			UART1_Put_String(str);
			delay_ms(10);//µÈ´ı´«ÊäÍê³É
		sprintf(str,"Mag:%d %d %d\r\n",stcMag.h[0],stcMag.h[1],stcMag.h[2]);
			UART1_Put_String(str);		
			delay_ms(10);//µÈ´ı´«ÊäÍê³É
		sprintf(str,"Pressure:%ld Height%.2f\r\n",stcPress.lPressure,(float)stcPress.lAltitude/100);
			UART1_Put_String(str);
			delay_ms(10);//µÈ´ı´«ÊäÍê³É
		sprintf(str,"DStatus:%d %d %d %d\r\n",stcDStatus.sDStatus[0],stcDStatus.sDStatus[1],stcDStatus.sDStatus[2],stcDStatus.sDStatus[3]);
			UART1_Put_String(str);
			delay_ms(10);//µÈ´ı´«ÊäÍê³É
		sprintf(str,"Longitude:%ldDeg%.5fm Lattitude:%ldDeg%.5fm\r\n",stcLonLat.lLon/10000000,(double)(stcLonLat.lLon % 10000000)/1e5,stcLonLat.lLat/10000000,(double)(stcLonLat.lLat % 10000000)/1e5);
			UART1_Put_String(str);
			delay_ms(10);//µÈ´ı´«ÊäÍê³É
		sprintf(str,"GPSHeight:%.1fm GPSYaw:%.1fDeg GPSV:%.3fkm/h\r\n\r\n",(float)stcGPSV.sGPSHeight/10,(float)stcGPSV.sGPSYaw/10,(float)stcGPSV.lGPSVelocity/1000);
			UART1_Put_String(str);
			delay_ms(10);//µÈ´ı´«ÊäÍê³É
	}//Ö÷Ñ­»· end 

}
