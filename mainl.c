
/*
*项目实现目标：向pc发送 what is your name? 
                Pc返回名字 
               而后再向pc发送hello 名字
*项目编写时间：2022年3月23日
*项目编写人：李俊辉
*/

#include "ioCC2530.h"

char data[] = "What is your name?\n";           //发送字符串
char name_string[20];                           //接收缓存
unsigned char temp = 0;                         //临时变量
unsigned char RX_flag = 0;                      //软件接收标志位
unsigned char counter = 0;                      //字符接收计数器

/*
*函数名称：delay(unsigned int time)
*函数功能：为一些指定语句进行延时
*函数入口：void delay(unsigned int time)
*函数形参：无
*函数返回值：无
*/
void delay(unsigned int time)
{
  for ( unsigned int i=0; i<time; i++ )
    for ( unsigned char j=0; j<240; j++ )
    {
      asm("NOP");
      asm("NOP");
      asm("NOP");
    }
}

/*
*函数名称：Initial_Usart()
*函数功能：初始化串口0
*函数入口：void Initial_Usart()
*函数形参：无
*函数返回值：无
*/
void Initial_Usart()
{
  CLKCOMCMD &= ~0X7F;           //晶振设置为32Mhz
  while(CLKCONSTA & 0x40);       //等待晶振稳定
  CLKCONCMD &= ~0x47;           //设置系统主时钟频率为 32MHz
  PERCFG = 0x00;                  //usart0 使用备用位置为 1 Tx-P0.3 Rx-P0.2
  P0SEL |= 0x3C;                //P0.2 P0.3 P0.4 P0.5 用户外设功能
  P2DIR &= ~0xC0;                //P0优先作为UART方式
  U0CSR |= 0xC0;                //uart模式 允许接收
  U0GCR = 9;                    
  U0BAUD = 59;                  //波特率设为 19200
  URX0IF = 0;                   //uart0 RX 中断标志位清零
  UTX0IF = 0;                   //uart0 TX 中断标志位清零
  IEN0 = 0x84;                  //接收中断使能 总中断使能
}

/*
*函数名称：Uart_Tx_String(char * data_tx,int len)
*函数功能：串口发送数据
*函数入口：void Uart_Tx_String(char * data_tx,int len)
*函数形参：无
*函数返回值：无
*/
void Uart_Tx_String(char * data_tx,int len)
{
  for (unsigned int j=0; j<len; j++)
  {
    U0DBUF = *data_tx++;        //字符串发送
    while(UTX0IF == 0);         //轮询发送状态，发送完成后硬件会把UTX0IF置1
    UTX0IF = 0;                 //清除发送中断标志位
  }
}

/*
*函数名称：UART0_RX_ISR()
*函数功能：串口0中断服务
*函数入口：__intperrupt void UART0_RX_ISR(void)
*函数形参：无
*函数返回值：无
*/
#pragma vector = URX0_VECTOR
__intperrupt void UART0_RX_ISR(void)
{
  URX0IF = 0;                   //轻触接收中断标志位
  temp = U0DBUF;                //把接收缓冲寄存器里的数据保存到临时变量中
  RX_flag = 1;                  //置软件接收标志位
}

/*
*函数名称：mian()
*函数功能：主函数，程序入口
*函数入口：void main(void)
*函数形参：无
*函数返回值：无
*/
void mian(void)
{
  Initial_Usart();              //调用UART初始化函数
  Uart_Tx_String(data,sizeof(data));            //发送字符串
  while(1)
  {
    if(RX_flag == 1)
    {
      RX_flag = 0;              //清除软件接收标志
      if (temp != '#')          //Pc发来的字符串必须以 # 号结尾
      {
        name_string[counter++] = temp;          //存储接收数据：名字+#
      }
      else
      {
        U0CSR &= ~0x40;                 //禁止接收
        Uart_Tx_String("Hello ",sizeof("Hello "));              //名字接收结束，发送hello
        delay(1000);
        Uart_Tx_String(name_string, counter);           //发送名字字符串
        counter = 0;
        U0CSR |= 0x40;
      }
    }
  }
}