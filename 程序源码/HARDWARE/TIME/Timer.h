#ifndef __TIMER_H
#define __TIMER_H
#include "include.h"

void TIM3_Int_Init(u16 arr,u16 psc);
void TIM2_Int_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u16 arr,u16 psc);
void TIM2_PWM_Init(u16 arr,u16 psc);
void TIM4_PWM_Init(u16 arr,u16 psc);
void TIM7_Int_Init(u16 arr,u16 psc);
#endif

//舵机控制基本数据（S-D5型号）
/*{
u16 djpwmval=0;
TIM3_PWM_Init(39999,35);	 //分频系数35，周期20ms
djpwmval=34900;          //舵机中值36500 左最大值38400（预留50缓冲） 右最大值34900（无缓冲预留）
TIM_SetCompare2(TIM3,djpwmval);		//大于ccr寄存器值时输出高电平
}*/
/*******************************************************/
/*{
//			 if(dir==1)  djpwmval+=5;
//			 if(dir==0)  djpwmval-=5;
//			 if(djpwmval>=38400) dir=0;
//			 if(djpwmval<=34900) dir=1;
//		   TIM_SetCompare3(TIM3,djpwmval);	
}*/








