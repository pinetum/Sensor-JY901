#include "stm32f10x.h"
#include "UARTs.h"
#include "IOI2C.h"
#include "delay.h"
#include "AHRSREG.h"


void SysTick_init(u8 SYSCLK,u16 nms);

void ShortToChar(short sData,unsigned char cData[])
{
	cData[0]=sData&0xff;
	cData[1]=sData>>8;
}
short CharToShort(unsigned char cData[])
{
	return ((short)cData[1]<<8)|cData[0];
}

void SetPWMOut()
{
  unsigned char chrTemp[2];
	//设置D0为PWM输出
	ShortToChar(DIO_MODE_DOPWM,chrTemp);//准备数据
	IICwriteBytes(0x50, D0MODE, 2,&chrTemp[0]);		//IIC发送

	//设置D0为PWM输出周期
	ShortToChar(20000,chrTemp);//准备数据
	IICwriteBytes(0x50, D0PWMT, 2,&chrTemp[0]);		//IIC发送
	//设置D0为PWM输出搞点钱宽度
	ShortToChar(1500,chrTemp);//准备数据
	IICwriteBytes(0x50, D0PWMH, 2,&chrTemp[0]);		//IIC发送
}
/**************************实现函数********************************************
*函数原型:		int main(void)
*功　　能:		主程序
*******************************************************************************/
int main(void)
{
	int16_t Math_hz=0;
	short i=0;
  unsigned char chrTemp[30];
	unsigned char str[100];
	float a[3],w[3],h[3],Angle[3];
     
  SystemInit();	/* 配置系统时钟为72M 使用外部8M晶体+PLL*/ 
	SysTick_init(72,10);		//延时初始化
	Initial_UART1(9600);
	IIC_Init();	 //初始化I2C接口
	delay_ms(1000);//等等JY-91初始化完成
	
	SetPWMOut();//演示设置D0端口为PWM输出。
	while(1)
	{			
		IICreadBytes(0x50, AX, 24,&chrTemp[0]);
		a[0] = (float)CharToShort(&chrTemp[0])/32768*16;
		a[1] = (float)CharToShort(&chrTemp[2])/32768*16;
		a[2] = (float)CharToShort(&chrTemp[4])/32768*16;
		w[0] = (float)CharToShort(&chrTemp[6])/32768*2000;
		w[1] = (float)CharToShort(&chrTemp[8])/32768*2000;
		w[2] = (float)CharToShort(&chrTemp[10])/32768*2000;
		h[0] = CharToShort(&chrTemp[12]);
		h[1] = CharToShort(&chrTemp[14]);
		h[2] = CharToShort(&chrTemp[16]);
		Angle[0] = (float)CharToShort(&chrTemp[18])/32768*180;
		Angle[1] = (float)CharToShort(&chrTemp[20])/32768*180;
		Angle[2] = (float)CharToShort(&chrTemp[22])/32768*180;
		
		delay_ms(500);
		sprintf(str,"a:%.3f %.3f %.3f w:%.3f %.3f %.3f  h:%.0f %.0f %.0f  Angle:%.3f %.3f %.3f \r\n",a[0],a[1],a[2],w[0],w[1],w[2],h[0],h[1],h[2],Angle[0],Angle[1],Angle[2]);
		UART1_Put_String(str);		
	}//主循环 end 

}
