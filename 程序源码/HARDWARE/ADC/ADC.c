#include "ADC.h"
//模拟量采集使用引脚：
//PA4,PA5,PA6,PA7
void Adc1_Channe_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	ADC_DeInit(ADC1);
	
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;//连续转化模式设置
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//数据左对齐
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//软件触发
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;//独立模式
	ADC_InitStructure.ADC_NbrOfChannel=1;//通道数
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;//扫描模式设置
	ADC_Init(ADC1,&ADC_InitStructure);
	
	//ADC_TempSensorVrefintCmd(ENABLE); //开启内部温度传感器
	
	ADC_Cmd(ADC1,ENABLE);//使能指定ADC外设
	
	ADC_ResetCalibration(ADC1);//使能复位校准
	while(ADC_GetResetCalibrationStatus(ADC1));//等待复位校准结束
	ADC_StartCalibration(ADC1);//开启AD校准
	ADC_GetCalibrationStatus(ADC1);//等待AD校准结束
}

u16 get_Adc_Value(u8 ch)//通道一采样值
{	
   ADC_RegularChannelConfig(ADC1,ch,1,ADC_SampleTime_239Cycles5);
	 ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	 while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
	 return ADC_GetConversionValue(ADC1);
}

void ADC_Channel_Value(u8 *p,u16 *q)//p:AD通道地址，q：AD通道采集值地址 四路AD
{
	  static int i=0,j=0;//数组行列控制系数
      static u16 AD_Val_LB[6][4]={0};//滤波值
	  u16 sum[6]={0};
//*********************************************************/
		 for(i=0;i<6;i++)   //递推算法
		 {
			 for(j=2;j>=0;j--)
			 {
				 AD_Val_LB[i][j+1]=AD_Val_LB[i][j];
			 }
		 }	
/*********************************************************/
     for(i=0;i<6;i++)
		 {
			 *(q+i)=get_Adc_Value(*(p+i));//获取通道值
			 AD_Val_LB[i][0]=*(q+i);//每次采集的值赋值给每行数组头元素
			 for(j=0;j<4;j++)
			 {
				 sum[i]+=AD_Val_LB[i][j];//求和
			 }		
       *(q+i)=sum[i]/4;//平均值赋值给对应AD通道采集值地址上
		 }
/**********************************************************/
     for(i=0;i<6;i++)//串口打印濾波后的AD值
     {
			 printf("%d   ",*(q+i));
		 }		
		 printf("\r\n");
}

//	Adc1_Channe_Init(); 			//ADC初始化

//uint16 AD_CH_LV(uint8 CHx,uint8 M,ADCn_Ch_e adcn_ch, ADC_nbit bit)//AD?????
//{
//	long int sum=0;
//	static uint8 i=0,j=0,CHy[6]=0;//CHx?????????,CHy?????????
//	static uint16 ADValue[6][15]=0;//??????,??6?,15??
//	ADValue[CHx][CHy[CHx]++]=adc_once (adcn_ch, bit);//ADValue[CHx][CHy[CHx]++]:????????
//	if(CHy[CHx]>=M) CHy[CHx]=0;//???????,?????
//	for(j=0;j<M;j++)
//	{
//            sum=sum+ADValue[CHx][j];//?M??????
//	}
//	sum=sum/M;//??????
//	return sum;//?????
//}








