#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h" 		 	 
#include "lcd.h"  
#include "key.h"  
#include "key1.h"
#include "beep.h"	   
#include "malloc.h"
#include "sdio_sdcard.h"  
#include "w25qxx.h"   
#include "ff.h"  
#include "exfuns.h"   		
#include "usart3.h"
#include "common.h" 
#include "rtc.h" 
#include "sql.h"
#include "usart2.h"
#include "AS608.h"
#include "timer.h"
#include "touch.h"
#include "ezio.h"
#include "text.h"
#include "admin.h"
#include "fontupd.h"
#include "usmart.h"
#include "fattester.h"
#include "stmflash.h"

#define usart2_baund  9600//串口2波特率，根据指纹模块波特率更改
#define usart3_baund 11500
FIL fill;
UINT bw_2;

u8 cun[10]={' ',' ','0','0',' ',' ',' ',' ',' ',' '};
u8 cun_1[10]={' ',' ','0','0',' ',' ',' ',' ',' ',' '};

int depart_1[7];
int depart_2[7];
int depart_3[7];
int depart_4[7];
int depart_5[7];
int depart_6[7];
int depart_7[7];
int depart_8[7];

u8 number_1[30];
u8 number_2[30];
int member_num;

u8** kbd_tbl;
const  u8* kbd_menu[15]={"delete"," : ","input","1","2","3","4","5","6","7","8","9"," ","0"," "};//按键表

SysPara AS608Para;//指纹模块AS608参数
u16 ValidN;//模块内有效指纹个数	

u8 s_1=0;
u8 IDnum;
u8 shu_flag;
u8 shu_flag_1;
u8 flag_1=0;
u8 flag_2=1;
u8 flag3;
u8 flag4=0;
u8 flag5;
u8 e=1;
u8 f=1;
u8 min_five=0;
u8 count;
u8 depart_flag;
u8 data_flag;

void Add_FR(void);	//录指纹
void Del_FR(void);	//删除指纹
void press_FR(void);//刷指纹
void ShowErrMessage(u8 ensure);//显示确认码错误信息
void keyboard(u16 x,u16 y,u8 **kbtbl);
void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode);
void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len);
void AS608_key_staset(u16 x,u16 y,u8 keyx,u8 sta);
u8 jiemian(void );


void kaiji_xianshi()
{
	POINT_COLOR=RED; 
	LCD_ShowChar(50,160,'W',24,1);	
	delay_ms(150);
	LCD_ShowChar(60,160,'E',24,1);
	delay_ms(150);
	LCD_ShowChar(70,160,'L',24,1);		
	delay_ms(150);
	LCD_ShowChar(80,160,'C',24,1);		
	delay_ms(150);
	LCD_ShowChar(90,160,'O',24,1);		
	delay_ms(150);
	LCD_ShowChar(100,160,'M',24,1);
	delay_ms(600);
	
	POINT_COLOR=BLUE; 
	LCD_ShowChar(110,160,'E',24,1);		
	delay_ms(150);
	LCD_ShowChar(150,160,'U',24,1);		
	delay_ms(150);
	LCD_ShowChar(160,160,'s',24,1);		
	delay_ms(150);
	LCD_ShowChar(170,160,'e',24,1);		
	delay_ms(500);
}

void IDentity()
{
	SearchResult seach;
	u8 key;
	u8 ensure_1;
	u8 z_2=0;
	flag_1=1;
	
	TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE );
	LCD_Clear(WHITE);//清屏
	Show_Str(20,40,200,16,"请识别指纹：",16,1); 
	Show_Str(20,300,200,16,"密码",16,1); 
	Show_Str(200,300,200,16,"返回",16,1);
	
	while(flag_1)
	{
		ensure_1=PS_GetImage();
		if(ensure_1==0x00)//获取图像成功 
		{	
			BEEP=1;//打开蜂鸣器	
			ensure_1=PS_GenChar(CharBuffer1);
			if(ensure_1==0x00) //生成特征成功
			{		
				BEEP=0;//关闭蜂鸣器	
				ensure_1=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
				if(ensure_1==0x00)//搜索成功
				{				
					LCD_Fill(0,100,lcddev.width,160,WHITE);
					Show_Str_Mid(0,100,"刷指纹成功",16,240);				
					if(flag_1==1)
					{
						if(seach.pageID==99)
							z_2=1;
						else
							z_2=0;
					}
					min_five=0;
				}
				else 
					ShowErrMessage(ensure_1);					
			}
			else
				ShowErrMessage(ensure_1);
		 BEEP=0;//关闭蜂鸣器
		 delay_ms(600);
		 LCD_Fill(0,100,lcddev.width,160,WHITE);
		}	
		if(z_2==1)
		{
			LCD_Clear (WHITE );
			jiemian();
			flag_1=0;
		}
		key=KEY_Scan(0);
		
		if(key==KEY0_PRES)
			flag_1=0;
			
		if(key==KEY2_PRES)
		{
			if(password_input()==1)           //输密码
			{
				LCD_Clear (WHITE );
				jiemian();
				flag_1=0;
			}
			else
				flag_1=0;
		}
	}
	return_time_flag=1;
	LCD_Clear (WHITE );
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );
}

int main(void)
{	     
	u8 ensure;
	char *str;	  
	u8 key;
	u8 fontok=0; //用于字库校准
//	count=0;
	
	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	usmart_dev.init(72);		//初始化USMART		
	uart_init(115200);	 	//串口初始化为115200
	usart2_init(usart2_baund);//初始化串口2
	usart3_init(usart3_baund);		//初始化串口3
 	LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();					//初始化按键
	LCD_Init();			   		//初始化LCD 
	TP_Init();     //触摸屏初始化,注意这里用到了24C02来存储校准数据,因此需要24C02的驱动支持,移植的时候注意
	W25QXX_Init();				//初始化W25Q128
	PS_StaGPIO_Init();	//初始化FR读状态引脚
	BEEP_Init();  			//初始化蜂鸣器
	Key_init1();
	 
	TIM3_Int_Init(1999,7199);
	RTC_Init();	  			  
//	RTC_Set(2020,2,7,17,25,50);  //设置时间	

/*SD卡检测及文件系统挂载*/
	while(SD_Init())//检测不到SD卡
	{
		LCD_ShowString(30,150,200,16,16,"SD Card Error!");
		delay_ms(500);					
		LCD_ShowString(30,150,200,16,16,"Please Check! ");
		delay_ms(500);
		LED0=!LED0;//DS0闪烁
	}
 	exfuns_init();							//为fatfs相关变量申请内存				 
  f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.				  			    
	
	while(PS_HandShake(&AS608Addr))//与AS608模块握手
	{
		delay_ms(300);
		LCD_Fill(0,40,240,80,WHITE);
//		Show_Str_Mid(0,40,"未检测到模块!!!",16,240);
		delay_ms(700);
		LCD_Fill(0,40,240,80,WHITE);
//		Show_Str_Mid(0,40,"尝试连接模块...",16,240);		  
	}
	
/*字库校准*/	
	fontok=font_init();			//检查字库是否OK
	if(fontok||key==KEY1_PRES)	//需要更新字库				 
	{
		LCD_Clear(WHITE);		//清屏
 		POINT_COLOR=RED;		//设置字体为红色	 						    

		key=update_font(20,110,16,"0:");//从SD卡更新
		while(key)//更新失败		
		{			 		  
			LCD_ShowString(60,110,200,16,16,"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(20,110,200+20,110+16,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(60,110,200,16,16,"Font Update Success!");
		LCD_Clear(WHITE);//清屏	       
	}  
	
/*指纹模块操作*/
	ensure=PS_ValidTempleteNum(&ValidN);//读库指纹个数
	if(ensure!=0x00)
		ShowErrMessage(ensure);//显示确认码错误信息	
	ensure=PS_ReadSysPara(&AS608Para);  //读参数 
	myfree(SRAMIN,str);
	

	kaiji_xianshi();
	LCD_Clear(WHITE);//清屏
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );
	zhuanhua(0);
	while(1)
	{
		key=KEY_Scan(0);
		flag_1=0;
		flag_2=1; 
		
		if(key==KEY0_PRES)
		{	
			TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE );
			LCD_Clear(WHITE);
//			atk_8266_test();		//进入ATK_ESP8266测试
		
			File_xianshi();
		}
		
		if(key==KEY2_PRES)
		{	
//			IDentity();
			TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE );
			jiemian();
		}
		press_FR();//刷指纹	

		if(key==WKUP_PRES)
		{	
			LCD_LED=1;				//点亮背光
		}
	}
}
	





u8 key_input()
{
	u8 x=0,y=0,flag=0,b=0,c=0;
	keyboard(0,170,(u8**)kbd_menu);//加载虚拟键盘
	while(flag==0)
	{
		TP_Scan(0);
		if(tp_dev.x>0&&tp_dev.x<80&&tp_dev.y>207&&tp_dev.y<236)
		{
			x=1;b++;
			GPIO_SetBits(GPIOB,GPIO_Pin_8);//输出0，关闭蜂鸣器输出
			delay_ms(300);
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);//输出0，关闭蜂鸣器输出
			tp_dev.y=0;
		}
		if(tp_dev.x>80&&tp_dev.x<160&&tp_dev.y>207&&tp_dev.y<236)
		{
			x=2;b++;
			GPIO_SetBits(GPIOB,GPIO_Pin_8);
			delay_ms(300);
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);
			tp_dev.y=0;
		}
		if(tp_dev.x>160&&tp_dev.x<240&&tp_dev.y>207&&tp_dev.y<236)
		{
			x=3;b++;
			GPIO_SetBits(GPIOB,GPIO_Pin_8);
			delay_ms(300);
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);
			tp_dev.y=0;
		}
		if(tp_dev.x>0&&tp_dev.x<80&&tp_dev.y>237&&tp_dev.y<266)
		{
			x=4;b++;
			GPIO_SetBits(GPIOB,GPIO_Pin_8);
			delay_ms(300);
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);
			tp_dev.y=0;
		}
		if(tp_dev.x>80&&tp_dev.x<160&&tp_dev.y>237&&tp_dev.y<266)
		{
			x=5;b++;
			GPIO_SetBits(GPIOB,GPIO_Pin_8);
			delay_ms(300);
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);
			tp_dev.y=0;
		}
		if(tp_dev.x>160&&tp_dev.x<240&&tp_dev.y>237&&tp_dev.y<266)
		{
			x=6;b++;
			GPIO_SetBits(GPIOB,GPIO_Pin_8);
			delay_ms(300);
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);
			tp_dev.y=0;
		}
		if(tp_dev.x>0&&tp_dev.x<80&&tp_dev.y>267&&tp_dev.y<296)
		{
			x=7;b++;
			GPIO_SetBits(GPIOB,GPIO_Pin_8);
			delay_ms(300);
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);
			tp_dev.y=0;
		}
		if(tp_dev.x>80&&tp_dev.x<160&&tp_dev.y>267&&tp_dev.y<296)
		{
			x=8;b++;
			GPIO_SetBits(GPIOB,GPIO_Pin_8);
			delay_ms(300);
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);
			tp_dev.y=0;
		}
		if(tp_dev.x>160&&tp_dev.x<240&&tp_dev.y>267&&tp_dev.y<296)
		{
			x=9;b++;
			GPIO_SetBits(GPIOB,GPIO_Pin_8);
			delay_ms(300);
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);
			tp_dev.y=0;
		}
		if(tp_dev.x>80&&tp_dev.x<160&&tp_dev.y>297&&tp_dev.y<320)
		{
			x=0;b++;
			GPIO_SetBits(GPIOB,GPIO_Pin_8);
			delay_ms(300);
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);
			tp_dev.y=0;
		}
		
		if(tp_dev.x>160&&tp_dev.x<240&&tp_dev.y>297&&tp_dev.y<320)
		{
			c++;
			e=1;f=1;
			GPIO_SetBits(GPIOB,GPIO_Pin_8);
			delay_ms(300);
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);
			tp_dev.y=0;
		}
		
		if(tp_dev.x>0&&tp_dev.x<80&&tp_dev.y>297&&tp_dev.y<320)
		{
			flag=1;
			e=2;f=2;
			GPIO_SetBits(GPIOB,GPIO_Pin_8);
			delay_ms(300);
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);
			tp_dev.y=0;
		}
		if(b==1)
		{
			y=x;
		}
		if(b==1&&c==1)
		{
			flag=1;
		}
		if(b==2&&c==1)
		{
			x=y*10+x;
			flag=1;
		}
		LCD_ShowNum(130,140,x,2,16);
		delay_ms(10);
		
	}
	LCD_Clear(WHITE); 
	return x;
	
}

//显示确认码错误信息
void ShowErrMessage(u8 ensure)
{
	LCD_Fill(0,120,lcddev.width,160,WHITE);
	LCD_ShowString(80,120,240,16,16,"Search Error!");
	delay_ms(500);
}

//录指纹
void Add_FR(void)
{
	u8 i,ensure ,processnum=0;
	u8 ID;
	while(1)
	{
		if(f==1)
		{
			LCD_Clear(LBBLUE);
			switch (processnum)
			{
				case 0:
					i++;
					LCD_Fill(0,100,lcddev.width,160,WHITE);
					Show_Str_Mid(0,100,"请按指纹",16,240);
					delay_ms(500);
					ensure=PS_GetImage();
					if(ensure==0x00) 
					{
						BEEP=1;
						ensure=PS_GenChar(CharBuffer1);//生成特征
						BEEP=0;
						if(ensure==0x00)
						{
							LCD_Fill(0,120,lcddev.width,160,WHITE);
							Show_Str_Mid(0,120,"指纹正常",16,240);
							delay_ms(500);
							i=0;
							processnum=1;//跳到第二步						
						}else ShowErrMessage(ensure);				
					}else ShowErrMessage(ensure);						
					break;
				
				case 1:
					i++;
					LCD_Fill(0,100,lcddev.width,160,WHITE);
					Show_Str_Mid(0,100,"请按再按一次指纹",16,240);
					delay_ms(500);
					ensure=PS_GetImage();
					if(ensure==0x00) 
					{
						BEEP=1;
						ensure=PS_GenChar(CharBuffer2);//生成特征
						BEEP=0;
						if(ensure==0x00)
						{
							LCD_Fill(0,120,lcddev.width,160,WHITE);
							Show_Str_Mid(0,120,"指纹正常",16,240);
							delay_ms(500);
							i=0;
							processnum=2;//跳到第三步
						}else ShowErrMessage(ensure);	
					}else ShowErrMessage(ensure);		
					break;

				case 2:
					LCD_Fill(0,100,lcddev.width,160,WHITE);
					Show_Str_Mid(0,100,"对比两次指纹",16,240);
					delay_ms(500);
					ensure=PS_Match();
					if(ensure==0x00) 
					{
						LCD_Fill(0,120,lcddev.width,160,WHITE);
						Show_Str_Mid(0,120,"对比成功,两次指纹一样",16,240);
						delay_ms(500);
						processnum=3;//跳到第四步
					}
					else 
					{
						LCD_Fill(0,100,lcddev.width,160,WHITE);
						Show_Str_Mid(0,100,"对比失败，请重新录入指纹",16,240);
						delay_ms(500);
						ShowErrMessage(ensure);
						i=0;
						processnum=0;//跳回第一步		
					}
					delay_ms(1200);
					break;

				case 3:
					LCD_Fill(0,100,lcddev.width,160,WHITE);
					Show_Str_Mid(0,100,"生成指纹模板",16,240);
					delay_ms(500);
					ensure=PS_RegModel();
					if(ensure==0x00) 
					{
						LCD_Fill(0,120,lcddev.width,160,WHITE);
						Show_Str_Mid(0,120,"生成指纹模板成功",16,240);
						delay_ms(500);
						processnum=4;//跳到第五步
					}else {processnum=0;ShowErrMessage(ensure);}
					delay_ms(1200);
					break;
					
				case 4:	
					LCD_Fill(0,100,lcddev.width,160,WHITE);
					Show_Str_Mid(0,100,"请输入储存ID,按Enter保存",16,240);
					Show_Str_Mid(0,120,"0=< ID <=299",16,240);
					do
						ID=key_input();
					
					while(!(ID<AS608Para.PS_max));//输入ID必须小于容量的最大值
					ensure=PS_StoreChar(CharBuffer2,ID);//储存模板
					switch (depart_flag)
					{
						case 1: member_num=sizeof(depart_1);depart_1[member_num]=ID;    break;
						case 2: member_num=sizeof(depart_2);depart_1[member_num]=ID;    break;
						case 3: member_num=sizeof(depart_3);depart_1[member_num]=ID;   	break;
						case 4: member_num=sizeof(depart_4);depart_1[member_num]=ID;    break;
						case 5: member_num=sizeof(depart_5);depart_1[member_num]=ID;    break;
						case 6: member_num=sizeof(depart_6);depart_1[member_num]=ID;    break;
						case 7: member_num=sizeof(depart_7);depart_1[member_num]=ID;    break;
						case 8: member_num=sizeof(depart_8);depart_1[member_num]=ID;    break;
					}
					LCD_Clear(LBBLUE);
					if(ensure==0x00) 
					{			
						LCD_Fill(0,100,lcddev.width,160,WHITE);		
						if(f==1)	
							Show_Str_Mid(0,120,"录入指纹成功",16,240);
							
						
						PS_ValidTempleteNum(&ValidN);//读库指纹个数
						delay_ms(1500);
						LCD_Fill(0,100,240,160,WHITE);
						if(f==1)
							Add_FR();
						if(f==2)
						{
							TIM_Cmd(TIM3, ENABLE); 
							LCD_Clear (WHITE);
						}
						return ;
					}else {processnum=0;ShowErrMessage(ensure);}					
					break;				
			}
		}
			delay_ms(400);
		 if(i==5)//超过5次没有按手指则退出
			{
				LCD_Fill(0,100,lcddev.width,160,WHITE);
				break;	
			}				
	}
	TIM_Cmd(TIM3, ENABLE);
	LCD_Clear (WHITE);
}

//刷指纹
void press_FR(void)
{
	SearchResult seach;
	u8 ensure;
//	u8 d[50];
//	u8 s;
	u8 z_1=0;
	u8 z_2=0;
	//	char *str;
		ensure=PS_GetImage();
		if(ensure==0x00)//获取图像成功 
		{	
			LCD_LED=1;				//点亮背光
			BEEP=1;//打开蜂鸣器	
			ensure=PS_GenChar(CharBuffer1);
			if(ensure==0x00) //生成特征成功
			{		
				BEEP=0;//关闭蜂鸣器	
			}
			else
					ShowErrMessage(ensure);
				
				
			if(flag3)
			{
				ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
				if(ensure==0x00)//搜索成功
				{				
					LCD_Fill(0,100,lcddev.width,160,WHITE);
					Show_Str_Mid(0,100,"刷指纹成功",16,240);				
					delay_ms(1000);	
					delay_ms(1000);
					IDnum=seach.pageID;
//					s=0;
//					sql();
					number_1 [z_1]=IDnum;
					z_1++;
//					for(z_1=0;z_1<=shu_flag;z_1++)
//					{
//						if(IDnum==d[z_1])
//							s=1;
//					}
//					if(s==0)
//					{
//						shu_flag++;
//						cun[2]=shu_flag/10+'0';
//						cun[3]=shu_flag%10+'0';
//						d[shu_flag]=IDnum;
//						
//					}
					min_five=0;
				}
				else 
					ShowErrMessage(ensure);		
			}
			else
				Show_Str(60,80,200,16,"未到签到时间",16,0);
			
			if(flag4==1)
			{
			if(flag5==1&&flag4==1)
			{
				BEEP=1;//打开蜂鸣器	
				ensure=PS_GenChar(CharBuffer1);
				if(ensure==0x00) //生成特征成功
				{		
					BEEP=0;//关闭蜂鸣器	
					ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
					if(ensure==0x00)//搜索成功
					{				
						LCD_Fill(0,100,lcddev.width,160,WHITE);
						Show_Str_Mid(0,100,"刷指纹成功",16,240);				
						delay_ms(1000);
						delay_ms(1000);
						IDnum=seach.pageID;
//						s=0;
//						sql();
						number_2 [z_2]=IDnum;
						z_2++;
//						for(z_1=0;z_1<=shu_flag;z_1++)
//						{
//							if(IDnum==d[z_1])
//								s=1;
//						}
//						if(s==0)
//						{
//							shu_flag_1++;
//							cun_1[2]=shu_flag_1/10+'0';
//							cun_1[3]=shu_flag_1%10+'0';
//							d[shu_flag]=IDnum;
//							
//						}
						min_five=0;
					}
					else 
						ShowErrMessage(ensure);					
				}
				else
					ShowErrMessage(ensure);
			 BEEP=0;//关闭蜂鸣器
			 delay_ms(600);
			 LCD_Fill(0,100,lcddev.width,160,WHITE);
			}
			else
				Show_Str(60,80,200,16,"未到签退时间",16,0);
			
			
			
			}
		delay_ms (1000);
		delay_ms (1000);
		LCD_Fill(60,80,200,100,WHITE );	
		 BEEP=0;//关闭蜂鸣器
		 delay_ms(600);
		 LCD_Fill(0,100,lcddev.width,160,WHITE);
		}
		
}

//删除指纹
void Del_FR(void)
{
	u8  ensure;
	u16 ValidN,num;
	LCD_Clear (YELLOW);
	LCD_Fill(0,100,lcddev.width,160,WHITE);
	Show_Str_Mid(0,100,"请输入要删除指纹ID按确认发送",16,240);
	Show_Str_Mid(0,120,"0=< ID <=299",16,240);
	delay_ms(50);
	if(e==1)
	{
	num=key_input();//获取返回的数值
	if(num==0xFFFF)
		goto MENU ; //返回主页面
	else if(num==0xFF00)
		ensure=PS_Empty();//清空指纹库
	else 
		ensure=PS_DeletChar(num,1);//删除单个指纹
	if(ensure==0)
	{
		if(e==1)
		{
			LCD_Clear (YELLOW);
			LCD_Fill(0,120,lcddev.width,160,WHITE);
			Show_Str_Mid(0,140,"删除指纹成功",16,240);	
			delay_ms(1200);
			Del_FR();		//删指纹
		}
	}
  else
		ShowErrMessage(ensure);	
	delay_ms(1200);
	PS_ValidTempleteNum(&ValidN);//读库指纹个数
	}	
	else
		goto MENU ;
MENU:	
	LCD_Fill(0,100,lcddev.width,160,WHITE);
	delay_ms(50);

	LCD_Clear (WHITE);
	TIM_Cmd(TIM3, ENABLE); 
}



//加载按键界面（尺寸x,y为240*150）
//x,y:界面起始坐标（240*320分辨率的时候，x必须为0）
void keyboard(u16 x,u16 y,u8 **kbtbl)
{
	u16 i;
	POINT_COLOR=RED;
	kbd_tbl=kbtbl;
	LCD_Fill(x,y,x+240,y+150,WHITE);
	
	LCD_DrawLine(x,207,x,320);
	LCD_DrawLine(x+240,207,x+240,320);
	LCD_DrawLine(x+80,200,x+80,320);
	LCD_DrawLine(x+160,200,x+160,320);
	LCD_DrawLine(x+80,y+150,x+160,y+150);
	LCD_DrawLine(x,320,x+240,320);
	LCD_DrawRectangle(x,y+30,x+240,y+60);
	LCD_DrawRectangle(x,y+90,x+240,y+120);
	POINT_COLOR=BLUE;
	for(i=3;i<15;i++)
	{
		if(i==1)//按键表第2个‘:’不需要中间显示
			Show_Str(x+(i%3)*80+2,y+7+30*(i/3),80,30,(u8*)kbd_tbl[i],16,0);	
		if(i==13)
			{
				Show_Str(20,300,200,16,"返回",16,1);
			}
			if(i==14)
			{
				Show_Str(180,300,200,16,"确认",16,1); 
			}
		if(i!=13||i!=14)
			Show_Str_Mid(x+(i%3)*80,y+7+30*(i/3),(u8*)kbd_tbl[i],16,80);
	} 
}


//按键状态设置
//x,y:键盘坐标
//key:键值(0~14)
//sta:状态，0，松开；1，按下；
void AS608_key_staset(u16 x,u16 y,u8 keyx,u8 sta)
{		  
	u16 i=keyx/3,j=keyx%3;
	if(keyx>16)return;
	if(sta &&keyx!=1)//按键表第2个‘:’不需要清除
		LCD_Fill(x+j*80+1,y+i*30+1,x+j*80+78,y+i*30+28,GREEN);
	else if(keyx!=1)
		LCD_Fill(x+j*80+1,y+i*30+1,x+j*80+78,y+i*30+28,WHITE);
	if(keyx!=1)   ;//不是‘：’�
		Show_Str_Mid(x+j*80,y+7+30*i,(u8*)kbd_tbl[keyx],16,80);	
}



u8 jiemian()
{
	u8 key;
	u8 return_Interface_flag=1;
	s8  choose_function_flag=0;
	LCD_Clear (WHITE);
	Show_Str(60,80,200,16,"录入指纹",16,1); //a=0
	Show_Str(60,100,200,16,"删除指纹",16,1); //a=1
	Show_Str(60,120,200,16,"设置本地时间",16,1); //a=2
	Show_Str(60,140,200,16,"设置签到时间",16,1);  //a=3
	Show_Str(60,160,200,16,"设置签退时间",16,1);  //a=4
	Show_Str(20,300,200,16,"确认",16,1); 
	Show_Str(200,300,200,16,"返回",16,1);
	
	
	while(return_Interface_flag)
	{
		Show_Str(60,80,200,16,"录入指纹",16,1); //a=0
		Show_Str(60,100,200,16,"删除指纹",16,1); //a=1
		Show_Str(60,120,200,16,"设置本地时间",16,1); //a=2
		Show_Str(60,140,200,16,"设置签到时间",16,1);  //a=3
		Show_Str(60,160,200,16,"设置签退时间",16,1);  //a=4
		Show_Str(60,180,200,16,"设置密码",16,1);  //a=5
		Show_Str(60,200,200,16,"设置每日值班人员",16,1);  //a=6
		Show_Str(20,300,200,16,"确认",16,1); 
		Show_Str(200,300,200,16,"返回",16,1);
		key=KEY_Scan(0);
		if(key==KEY1_PRES)
		{
			choose_function_flag++;
			min_five=0;
			LCD_ShowString(40,80+(choose_function_flag-1)*20,16,16,16,"  ");
			if(choose_function_flag>6) choose_function_flag=0;
		}
		if(key==WKUP_PRES)
		{
			choose_function_flag--;
			min_five=0;
			LCD_ShowString(40,80+(choose_function_flag+1)*20,16,16,16,"  ");
			if(choose_function_flag<0) choose_function_flag=6;
			
		}
		if(key==KEY2_PRES)
		{
			min_five=0;
			if(choose_function_flag==0)
			{
				LCD_Clear(LBBLUE);
				f=1;
				if(depart_choose())
					Add_FR();		//录指纹
				count=0;
			}
			if(choose_function_flag==1)
			{ 
				LCD_Clear(YELLOW);
				e=1;
				Del_FR();		//删指纹
				count=0;
			}
			if(choose_function_flag==2)
			{
				Local_time_set();
			}
			if(choose_function_flag==3)
			{
				worktime_set();
			}
			if(choose_function_flag==4)
			{
				finworktime_set();
			}
			if(choose_function_flag==5)
			{
				s_1=1;
				password_set();
				LCD_Clear (WHITE );
				s_1=0;
			}
			if(choose_function_flag==6)
			{
				data_choose();
				
			}
		}
		if(key==KEY0_PRES)
		{
			return_Interface_flag=0;
			TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );
			min_five=0;
		}
		LCD_ShowString(40,80+choose_function_flag*20,16,16,16,"->");
	}
	LCD_Clear (WHITE);
	return_time_flag=1;
	flag_2=0;
	return 0;
}

