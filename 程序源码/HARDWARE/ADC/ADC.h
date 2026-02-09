#ifndef  __ADC_H
#define  __ADC_H

#include "sys.h"

//uint8_t channel[4]={0x01,0x02,0x03,0x05};
//u16 ADC_Value[4];

void Adc1_Channe_Init(void);
u16 get_Adc_Value(u8 ch);
void ADC_Channel_Value(u8 *p,u16 *q);
//u16 get_Adc_Value2(u8 ch);
//u16 get_Adc_Value3(u8 ch);
//u16 get_Adc_Value4(u8 ch);
//u16 T_get_Adc_Value(u8 ch);
float V_get_alue(u16 J_value);

#endif
//{四通道采集显示
//    ADC_Value1=get_Adc_Value1(ADC_Channel_1);
//		ADC_Value2=get_Adc_Value2(ADC_Channel_2);
//	  printf("ADC_Value1=%d     ADC_Value2=%d\r\n",ADC_Value1,ADC_Value2);	
//    printf("**************************\r\n");		
//		ADC_Value3=get_Adc_Value2(ADC_Channel_3);
//		ADC_Value4=get_Adc_Value2(ADC_Channel_4);
//		printf("ADC_Value3=%d     ADC_Value4=%d\r\n",ADC_Value3,ADC_Value4);
//		printf("\r\n");
//}


//{内部温度
// T_ADC_Value=T_get_Adc_Value(ADC_Channel_16);
//		 V_Value=(float)T_ADC_Value*(3.3/4096);
//		 temp=(1.43-V_Value)/0.0043;
//		 temp=temp+25.0;
//		 printf("temp=%2.3fC\r\n",temp);
//		 delay_ms(1000);
//}
