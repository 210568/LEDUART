
/*
*��Ŀʵ��Ŀ�꣺��pc���� what is your name? 
                Pc�������� 
               ��������pc����hello ����
*��Ŀ��дʱ�䣺2022��3��23��
*��Ŀ��д�ˣ����
*/

#include "ioCC2530.h"

char data[] = "What is your name?\n";           //�����ַ���
char name_string[20];                           //���ջ���
unsigned char temp = 0;                         //��ʱ����
unsigned char RX_flag = 0;                      //������ձ�־λ
unsigned char counter = 0;                      //�ַ����ռ�����

/*
*�������ƣ�delay(unsigned int time)
*�������ܣ�ΪһЩָ����������ʱ
*������ڣ�void delay(unsigned int time)
*�����βΣ���
*��������ֵ����
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
*�������ƣ�Initial_Usart()
*�������ܣ���ʼ������0
*������ڣ�void Initial_Usart()
*�����βΣ���
*��������ֵ����
*/
void Initial_Usart()
{
  CLKCOMCMD &= ~0X7F;           //��������Ϊ32Mhz
  while(CLKCONSTA & 0x40);       //�ȴ������ȶ�
  CLKCONCMD &= ~0x47;           //����ϵͳ��ʱ��Ƶ��Ϊ 32MHz
  PERCFG = 0x00;                  //usart0 ʹ�ñ���λ��Ϊ 1 Tx-P0.3 Rx-P0.2
  P0SEL |= 0x3C;                //P0.2 P0.3 P0.4 P0.5 �û����蹦��
  P2DIR &= ~0xC0;                //P0������ΪUART��ʽ
  U0CSR |= 0xC0;                //uartģʽ �������
  U0GCR = 9;                    
  U0BAUD = 59;                  //��������Ϊ 19200
  URX0IF = 0;                   //uart0 RX �жϱ�־λ����
  UTX0IF = 0;                   //uart0 TX �жϱ�־λ����
  IEN0 = 0x84;                  //�����ж�ʹ�� ���ж�ʹ��
}

/*
*�������ƣ�Uart_Tx_String(char * data_tx,int len)
*�������ܣ����ڷ�������
*������ڣ�void Uart_Tx_String(char * data_tx,int len)
*�����βΣ���
*��������ֵ����
*/
void Uart_Tx_String(char * data_tx,int len)
{
  for (unsigned int j=0; j<len; j++)
  {
    U0DBUF = *data_tx++;        //�ַ�������
    while(UTX0IF == 0);         //��ѯ����״̬��������ɺ�Ӳ�����UTX0IF��1
    UTX0IF = 0;                 //��������жϱ�־λ
  }
}

/*
*�������ƣ�UART0_RX_ISR()
*�������ܣ�����0�жϷ���
*������ڣ�__intperrupt void UART0_RX_ISR(void)
*�����βΣ���
*��������ֵ����
*/
#pragma vector = URX0_VECTOR
__intperrupt void UART0_RX_ISR(void)
{
  URX0IF = 0;                   //�ᴥ�����жϱ�־λ
  temp = U0DBUF;                //�ѽ��ջ���Ĵ���������ݱ��浽��ʱ������
  RX_flag = 1;                  //��������ձ�־λ
}

/*
*�������ƣ�mian()
*�������ܣ����������������
*������ڣ�void main(void)
*�����βΣ���
*��������ֵ����
*/
void mian(void)
{
  Initial_Usart();              //����UART��ʼ������
  Uart_Tx_String(data,sizeof(data));            //�����ַ���
  while(1)
  {
    if(RX_flag == 1)
    {
      RX_flag = 0;              //���������ձ�־
      if (temp != '#')          //Pc�������ַ��������� # �Ž�β
      {
        name_string[counter++] = temp;          //�洢�������ݣ�����+#
      }
      else
      {
        U0CSR &= ~0x40;                 //��ֹ����
        Uart_Tx_String("Hello ",sizeof("Hello "));              //���ֽ��ս���������hello
        delay(1000);
        Uart_Tx_String(name_string, counter);           //���������ַ���
        counter = 0;
        U0CSR |= 0x40;
      }
    }
  }
}