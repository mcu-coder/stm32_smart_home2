#include "sys.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "ADC.h"
#include "usart3.h"
#include "key.h"
#include "oled.h"
#include "math.h"
#include "dht11.h"
#include "bmp280.h"
#include "gizwits_product.h" 

u32 STM32_xx0=0X4E4C4A;
u32 STM32_xx1=0X364B1322;
u32 STM32_xx2=0X132D13  ;

#define FLASH_SAVE_ADDR 0x08010000

u8 buff[30];//参数显示缓存数组
u8 count;
dataPoint_t currentDataPoint;//云端数据点
u8 wifi_sta;//wifi连接状态标志
u8 mode=0;//模式控制自动和手动
u8 NTP_sta=0;//网诺时间获取状态
u8 display_contrl=0;
u8 curtain_flag=0;
u8 last_curtain_flag=0;

extern u8 DHT11_Temp,DHT11_Hum;		//温湿度
u16 Pre;							//气压检测值
u16 gz_value;						//光照检测值			
u16 m2_value;						//烟雾检测值
u16 m7_value;						//一氧化碳检测值
u16 m135_value;						//空气质量检测值

u16 A_DHT11_Temp=35;				//温度阈值   			 高于阈值时报警
u16 A_DHT11_Hum=20;					//湿度阈值				 低于阈值时报警
u16 A_pre=1500;							//气压阈值				 低于阈值时报警
u16 A_gz_value=1000;				//光照强度阈值		 低于阈值时打开窗帘和照明灯
u16 A_m2_value=4000;				//烟雾阈值				 高于阈值时报警
u16 A_m7_value=4000;				//一氧化碳阈值  	 高于阈值时报警
u16 A_m135_value=1500;			//空气质量阈值		 低于阈值时报警


//协议初始化
void Gizwits_Init(void)
{	
    TIM3_Int_Init(9,7199);//1MS系统定时
    usart3_init(9600);//WIFI初始化
    memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));//设备状态结构体初始化
    gizwitsInit();//缓冲区初始化
}

//数据采集
void userHandle(void)
{
    currentDataPoint.valueLED=!LED0;
    currentDataPoint.valueCurtain=curtain_flag;
    currentDataPoint.valueTemp=DHT11_Temp;

}

void Get_Data(u16 count);//获取传感器数据
void WIFI_Contection(u8 key);//WiFi连接控制
void Canshu_Change(u8 key);//系统参数调节
void Mode_Change(u8 key);//模式切换
void BUJING_Cotrol(u8 mode,u16 time,u16 count);

int main(void)
{
    u8 t=0;
    int key_value;		//按键值
    uart_init(115200);	 		//串口初始化为115200
    delay_init();	    //延时函数初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
    Adc1_Channe_Init();			//ADC通道初始化
    KEY_Init();					//按键引脚初始化
    bmp280Init();
    while(DHT11_Init());	
    LED_Init();		  			//初始化与LED连接的硬件接口
    OLED_Init();				//OLED初始化
    OLED_Clear();				//OLED清屏
    Gizwits_Init();		
    BEEP = 0;
//	STMFLASH_Write(FLASH_SAVE_ADDR+10,&A_DHT11_Temp,2);	//开机读取温度阈值
//	STMFLASH_Write(FLASH_SAVE_ADDR+12,&A_DHT11_Hum,2);	//开机读取湿度阈值
//	STMFLASH_Write(FLASH_SAVE_ADDR+14,&A_pre,2);			//开机读取气压阈值
//	STMFLASH_Write(FLASH_SAVE_ADDR+18,&A_gz_value,2);	//开机读取光照强度阈值
//	STMFLASH_Write(FLASH_SAVE_ADDR+20,&A_m2_value,2);	//开机读取烟雾阈值
//	STMFLASH_Write(FLASH_SAVE_ADDR+22,&A_m7_value,2);	//开机读取一氧化碳阈值
//	STMFLASH_Write(FLASH_SAVE_ADDR+24,&A_m135_value,2);	//开机读取空气质量阈值
	
//	STMFLASH_Read(FLASH_SAVE_ADDR+10,&A_DHT11_Temp,2);	//开机读取温度阈值
//	STMFLASH_Read(FLASH_SAVE_ADDR+12,&A_DHT11_Hum,2);	//开机读取湿度阈值
//	STMFLASH_Read(FLASH_SAVE_ADDR+14,&A_pre,2);			//开机读取气压阈值
//	STMFLASH_Read(FLASH_SAVE_ADDR+18,&A_gz_value,2);	//开机读取光照强度阈值
//	STMFLASH_Read(FLASH_SAVE_ADDR+20,&A_m2_value,2);	//开机读取烟雾阈值
//	STMFLASH_Read(FLASH_SAVE_ADDR+22,&A_m7_value,2);	//开机读取一氧化碳阈值
//	STMFLASH_Read(FLASH_SAVE_ADDR+24,&A_m135_value,2);	//开机读取空气质量阈值

    while(1)
    {
        Get_Data(0);
        if(gz_value<=A_gz_value&&mode==0)
        {
            LED0=0;
            curtain_flag=0;
        }else if(gz_value>A_gz_value&&mode==0){
            LED0=1;
            curtain_flag=1;
        }
        
        if(display_contrl%2==0&&t>=10)
        {
            OLED_ShowCHinese(0,0,0);
            OLED_ShowCHinese(16,0,2);
            sprintf((char*)buff,":%2dC",DHT11_Temp);
            OLED_ShowString(32,0,buff,16);	//显示温度
            OLED_ShowCHinese(64,0,1);
            OLED_ShowCHinese(82,0,2);
            sprintf((char*)buff,":%2d%%",DHT11_Hum);
            OLED_ShowString(96,0,buff,16);//显示湿度	
            
            OLED_ShowCHinese(0,2,3);
            OLED_ShowCHinese(16,2,4);
            sprintf((char*)buff,":%2dhpa     ",Pre);
            OLED_ShowString(32,2,buff,16);//显示气压
            
            OLED_ShowCHinese(0,4,5);
            OLED_ShowCHinese(16,4,6);
            OLED_ShowCHinese(32,4,7);
            OLED_ShowCHinese(48,4,8);
            sprintf((char*)buff,":%4dppm",gz_value);
            OLED_ShowString(64,4,buff,16);//显示光照强度
            
            OLED_ShowCHinese(0,6,21);
            OLED_ShowCHinese(16,6,22);
            OLED_ShowCHinese(32,6,23);
            OLED_ShowCHinese(48,6,24);
            //sprintf((char*)buff,":%4dppm",m135_value);
            //OLED_ShowString(64,6,buff,16);//显示空气质量
            OLED_ShowString(64,6,":",16);
        }else if(display_contrl%2==1&&t>=10)
        {
            OLED_ShowCHinese(0,0,13);
            OLED_ShowCHinese(16,0,14);
            OLED_ShowCHinese(32,0,15);
            OLED_ShowCHinese(48,0,16);
            sprintf((char*)buff,":%4dppm",m2_value);
            OLED_ShowString(64,0,buff,16);//显示M2数据
            
            OLED_ShowCHinese(0,2,17);
            OLED_ShowCHinese(16,2,18);
            OLED_ShowCHinese(32,2,19);
            OLED_ShowCHinese(48,2,20);
            sprintf((char*)buff,":%4dppm",m7_value);
            OLED_ShowString(64,2,buff,16);//显示M7数据
            
            OLED_ShowCHinese(0,4,5);
            OLED_ShowCHinese(16,4,6);
            OLED_ShowCHinese(32,4,7);
            OLED_ShowCHinese(48,4,8);
            sprintf((char*)buff,":%4dppm",gz_value);
            OLED_ShowString(64,4,buff,16);//显示光照强度
            
            OLED_ShowCHinese(0,6,21);
            OLED_ShowCHinese(16,6,22);
            OLED_ShowCHinese(32,6,23);
            OLED_ShowCHinese(48,6,24);
//            sprintf((char*)buff,":%4dppm",m135_value);
//            OLED_ShowString(64,6,buff,16);//显示M135数据
            
            
            
            
        }
       
        

        userHandle();		//用户数据采集
        WIFI_Contection(key_value);//WiFi连接控制
        gizwitsHandle((dataPoint_t *)&currentDataPoint);//机智云协议处理
        
        Canshu_Change(key_value);
        
        t++;
        delay_ms(100);
    }
}
 
void WIFI_Contection(u8 key)//WiFi连接控制
{
   
}

void Canshu_Change(u8 key)
{
    u8 obj=7;
    if(key==1)
    {
        BEEP=0;
        OLED_Clear();
        while(1)
        {
            key = KEY_Scan(0);
            if(key==1)
            {
                obj++;
                if(obj>=8)
                {
                    obj=0;
                }
            }
            sprintf((char *)buff,"Working md:%4d",mode);
           
            OLED_ShowString(8,7,buff,12);
            if(obj==0)
            {
                OLED_ShowString(0,0," ",12);
                OLED_ShowString(0,1,">",12);
                OLED_ShowString(0,2," ",12);
                OLED_ShowString(0,3," ",12);
                OLED_ShowString(0,4," ",12);
                OLED_ShowString(0,5," ",12);
                OLED_ShowString(0,6," ",12);
                OLED_ShowString(0,7," ",12);
            }
            if(obj==1)
            {
                OLED_ShowString(0,0," ",12);
                OLED_ShowString(0,1," ",12);
                OLED_ShowString(0,2,">",12);
                OLED_ShowString(0,3," ",12);
                OLED_ShowString(0,4," ",12);
                OLED_ShowString(0,5," ",12);
                OLED_ShowString(0,6," ",12);
                OLED_ShowString(0,7," ",12);
            }
            if(obj==2)
            {
                OLED_ShowString(0,0," ",12);
                OLED_ShowString(0,1," ",12);
                OLED_ShowString(0,2," ",12);
                OLED_ShowString(0,3,">",12);
                OLED_ShowString(0,4," ",12);
                OLED_ShowString(0,5," ",12);
                OLED_ShowString(0,6," ",12);
                OLED_ShowString(0,7," ",12);
            }
            if(obj==3)
            {
                OLED_ShowString(0,0," ",12);
                OLED_ShowString(0,1," ",12);
                OLED_ShowString(0,2," ",12);
                OLED_ShowString(0,3," ",12);
                OLED_ShowString(0,4,">",12);
                OLED_ShowString(0,5," ",12);
                OLED_ShowString(0,6," ",12);
                OLED_ShowString(0,7," ",12);
            }
            if(obj==4)
            {
                OLED_ShowString(0,0," ",12);
                OLED_ShowString(0,1," ",12);
                OLED_ShowString(0,2," ",12);
                OLED_ShowString(0,3," ",12);
                OLED_ShowString(0,4," ",12);
                OLED_ShowString(0,5,">",12);
                OLED_ShowString(0,6," ",12);
                OLED_ShowString(0,7," ",12);
            }
            if(obj==5)
            {
                OLED_ShowString(0,0," ",12);
                OLED_ShowString(0,1," ",12);
                OLED_ShowString(0,2," ",12);
                OLED_ShowString(0,3," ",12);
                OLED_ShowString(0,4," ",12);
                OLED_ShowString(0,5," ",12);
                OLED_ShowString(0,6,">",12);
                OLED_ShowString(0,7," ",12);
            }
            if(obj==6)
            {
                OLED_ShowString(0,0," ",12);
                OLED_ShowString(0,1," ",12);
                OLED_ShowString(0,2," ",12);
                OLED_ShowString(0,3," ",12);
                OLED_ShowString(0,4," ",12);
                OLED_ShowString(0,5," ",12);
                OLED_ShowString(0,6," ",12);
                OLED_ShowString(0,7,">",12);
            }
            if(obj==7)
            {
                OLED_ShowString(0,0,">",12);
                OLED_ShowString(0,1," ",12);
                OLED_ShowString(0,2," ",12);
                OLED_ShowString(0,3," ",12);
                OLED_ShowString(0,4," ",12);
                OLED_ShowString(0,5," ",12);
                OLED_ShowString(0,6," ",12);
                OLED_ShowString(0,7," ",12);
            }
            if(obj==0)
            {
                if(key==3)
                {
                    A_DHT11_Temp+=1;
                }
                if(key==2)
                {
                    A_DHT11_Temp-=1;
                }
            }
            if(obj==1)
            {
                if(key==3)
                {
                    A_DHT11_Hum+=1;
                }
                if(key==2)
                {
                    A_DHT11_Hum-=1;
                }
            }
            if(obj==2)
            {
                if(key==3)
                {
                    A_pre+=20;
                }
                if(key==2)
                {
                    A_pre-=20;
                }
            }
            if(obj==3)
            {
                if(key==3)
                {
                    A_gz_value+=50;
                }
                if(key==2)
                {
                    A_gz_value-=50;
                }
            }
            if(obj==4)
            {
                if(key==3)
                {
                    A_m2_value+=50;
                }
                if(key==2)
                {
                    A_m2_value-=50;
                }
            }
            if(obj==5)
            {
                if(key==3)
                {
                    A_m7_value+=50;
                }
                if(key==2)
                {
                    A_m7_value-=50;
                }
            }
            if(obj==6)
            {
                if(key==3)
                {
                    A_m135_value+=50;
                }
                if(key==2)
                {
                    A_m135_value-=50;
                }
            }
            if(obj==7)
            {
                if(key==3)
                {
                    mode+=1;
                }
                if(key==2)
                {
                    mode-=1;
                }
                if(mode >= 2) mode = 0;
            }
            if(key==4)
            {
    //				STMFLASH_Write(FLASH_SAVE_ADDR+10,&A_DHT11_Temp,2);	//开机读取温度阈值
    //				STMFLASH_Write(FLASH_SAVE_ADDR+12,&A_DHT11_Hum,2);	//开机读取湿度阈值
    //				STMFLASH_Write(FLASH_SAVE_ADDR+14,&A_pre,2);			//开机读取气压阈值
    //				STMFLASH_Write(FLASH_SAVE_ADDR+18,&A_gz_value,2);	//开机读取光照强度阈值
    //				STMFLASH_Write(FLASH_SAVE_ADDR+20,&A_m2_value,2);	//开机读取烟雾阈值
    //				STMFLASH_Write(FLASH_SAVE_ADDR+22,&A_m7_value,2);	//开机读取一氧化碳阈值
    //				STMFLASH_Write(FLASH_SAVE_ADDR+24,&A_m135_value,2);	//开机读取空气质量阈值
                OLED_Clear();
                break;
            }
        
        }
    }
}

void Get_Data(u16 count)//获取传感器数据
{
    static float bmp280_press,bmp280;				//气压
 

    m135_value = 4096 - get_Adc_Value(0x06);
}
//void Mode_Change(u8 key)
//{
//	if(key==2)
//	{
//		mode++;
//		if(mode>=2) mode = 0;
//	}
//	if(mode==0) OLED_ShowString(0,0,"MODE:Auto",12); //HM
//	if(mode==1) OLED_ShowString(0,0,"MODE:H_M_",12); //HM
//}

void BUJING_Cotrol(u8 mode,u16 time,u16 count)
    {
    if(mode==0)
    {
        while(count--)
        {
            BUJ1=1; BUJ2=0; BUJ3=0;BUJ4=0;
            delay_ms(time);
            BUJ1=0; BUJ2=1; BUJ3=0;BUJ4=0;
            delay_ms(time);
            BUJ1=0; BUJ2=0; BUJ3=1;BUJ4=0;
            delay_ms(time);
            BUJ1=0; BUJ2=0; BUJ3=0;BUJ4=1;
            delay_ms(time);
        }
    }
    if(mode==1)
    {
        while(count--)
        {
            BUJ1=0; BUJ2=0; BUJ3=0;BUJ4=1;
            delay_ms(time);
            BUJ1=0; BUJ2=0; BUJ3=1;BUJ4=0;
            delay_ms(time);
            BUJ1=0; BUJ2=1; BUJ3=0;BUJ4=0;
            delay_ms(time);
            BUJ1=1; BUJ2=0; BUJ3=0;BUJ4=0;
            delay_ms(time);
        }
    }
}
