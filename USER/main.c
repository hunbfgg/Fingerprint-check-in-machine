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

#define usart2_baund  9600//´®¿Ú2²¨ÌØÂÊ£¬¸ù¾ÝÖ¸ÎÆÄ£¿é²¨ÌØÂÊ¸ü¸Ä
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
const  u8* kbd_menu[15]={"delete"," : ","input","1","2","3","4","5","6","7","8","9"," ","0"," "};//°´¼ü±í

SysPara AS608Para;//Ö¸ÎÆÄ£¿éAS608²ÎÊý
u16 ValidN;//Ä£¿éÄÚÓÐÐ§Ö¸ÎÆ¸öÊý	

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

void Add_FR(void);	//Â¼Ö¸ÎÆ
void Del_FR(void);	//É¾³ýÖ¸ÎÆ
void press_FR(void);//Ë¢Ö¸ÎÆ
void ShowErrMessage(u8 ensure);//ÏÔÊ¾È·ÈÏÂë´íÎóÐÅÏ¢
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
	LCD_Clear(WHITE);//ÇåÆÁ
	Show_Str(20,40,200,16,"ÇëÊ¶±ðÖ¸ÎÆ£º",16,1); 
	Show_Str(20,300,200,16,"ÃÜÂë",16,1); 
	Show_Str(200,300,200,16,"·µ»Ø",16,1);
	
	while(flag_1)
	{
		ensure_1=PS_GetImage();
		if(ensure_1==0x00)//»ñÈ¡Í¼Ïñ³É¹¦ 
		{	
			BEEP=1;//´ò¿ª·äÃùÆ÷	
			ensure_1=PS_GenChar(CharBuffer1);
			if(ensure_1==0x00) //Éú³ÉÌØÕ÷³É¹¦
			{		
				BEEP=0;//¹Ø±Õ·äÃùÆ÷	
				ensure_1=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
				if(ensure_1==0x00)//ËÑË÷³É¹¦
				{				
					LCD_Fill(0,100,lcddev.width,160,WHITE);
					Show_Str_Mid(0,100,"Ë¢Ö¸ÎÆ³É¹¦",16,240);				
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
		 BEEP=0;//¹Ø±Õ·äÃùÆ÷
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
			if(password_input()==1)           //ÊäÃÜÂë
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
	u8 fontok=0; //ÓÃÓÚ×Ö¿âÐ£×¼
//	count=0;
	
	delay_init();	    	 //ÑÓÊ±º¯Êý³õÊ¼»¯	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//ÉèÖÃÖÐ¶ÏÓÅÏÈ¼¶·Ö×éÎª×é2£º2Î»ÇÀÕ¼ÓÅÏÈ¼¶£¬2Î»ÏìÓ¦ÓÅÏÈ¼¶
	usmart_dev.init(72);		//³õÊ¼»¯USMART		
	uart_init(115200);	 	//´®¿Ú³õÊ¼»¯Îª115200
	usart2_init(usart2_baund);//³õÊ¼»¯´®¿Ú2
	usart3_init(usart3_baund);		//³õÊ¼»¯´®¿Ú3
 	LED_Init();		  			//³õÊ¼»¯ÓëLEDÁ¬½ÓµÄÓ²¼þ½Ó¿Ú
	KEY_Init();					//³õÊ¼»¯°´¼ü
	LCD_Init();			   		//³õÊ¼»¯LCD 
	TP_Init();     //´¥ÃþÆÁ³õÊ¼»¯,×¢ÒâÕâÀïÓÃµ½ÁË24C02À´´æ´¢Ð£×¼Êý¾Ý,Òò´ËÐèÒª24C02µÄÇý¶¯Ö§³Ö,ÒÆÖ²µÄÊ±ºò×¢Òâ
	W25QXX_Init();				//³õÊ¼»¯W25Q128
	PS_StaGPIO_Init();	//³õÊ¼»¯FR¶Á×´Ì¬Òý½Å
	BEEP_Init();  			//³õÊ¼»¯·äÃùÆ÷
	Key_init1();
	 
	TIM3_Int_Init(1999,7199);
	RTC_Init();	  			  
//	RTC_Set(2020,2,7,17,25,50);  //ÉèÖÃÊ±¼ä	

/*SD¿¨¼ì²â¼°ÎÄ¼þÏµÍ³¹ÒÔØ*/
	while(SD_Init())//¼ì²â²»µ½SD¿¨
	{
		LCD_ShowString(30,150,200,16,16,"SD Card Error!");
		delay_ms(500);					
		LCD_ShowString(30,150,200,16,16,"Please Check! ");
		delay_ms(500);
		LED0=!LED0;//DS0ÉÁË¸
	}
 	exfuns_init();							//ÎªfatfsÏà¹Ø±äÁ¿ÉêÇëÄÚ´æ				 
  f_mount(fs[0],"0:",1); 		//¹ÒÔØSD¿¨ 
 	f_mount(fs[1],"1:",1); 		//¹ÒÔØFLASH.				  			    
	
	while(PS_HandShake(&AS608Addr))//ÓëAS608Ä£¿éÎÕÊÖ
	{
		delay_ms(300);
		LCD_Fill(0,40,240,80,WHITE);
//		Show_Str_Mid(0,40,"Î´¼ì²âµ½Ä£¿é!!!",16,240);
		delay_ms(700);
		LCD_Fill(0,40,240,80,WHITE);
//		Show_Str_Mid(0,40,"³¢ÊÔÁ¬½ÓÄ£¿é...",16,240);		  
	}
	
/*×Ö¿âÐ£×¼*/	
	fontok=font_init();			//¼ì²é×Ö¿âÊÇ·ñOK
	if(fontok||key==KEY1_PRES)	//ÐèÒª¸üÐÂ×Ö¿â				 
	{
		LCD_Clear(WHITE);		//ÇåÆÁ
 		POINT_COLOR=RED;		//ÉèÖÃ×ÖÌåÎªºìÉ«	 						    

		key=update_font(20,110,16,"0:");//´ÓSD¿¨¸üÐÂ
		while(key)//¸üÐÂÊ§°Ü		
		{			 		  
			LCD_ShowString(60,110,200,16,16,"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(20,110,200+20,110+16,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(60,110,200,16,16,"Font Update Success!");
		LCD_Clear(WHITE);//ÇåÆÁ	       
	}  
	
/*Ö¸ÎÆÄ£¿é²Ù×÷*/
	ensure=PS_ValidTempleteNum(&ValidN);//¶Á¿âÖ¸ÎÆ¸öÊý
	if(ensure!=0x00)
		ShowErrMessage(ensure);//ÏÔÊ¾È·ÈÏÂë´íÎóÐÅÏ¢	
	ensure=PS_ReadSysPara(&AS608Para);  //¶Á²ÎÊý 
	myfree(SRAMIN,str);
	

	kaiji_xianshi();
	LCD_Clear(WHITE);//ÇåÆÁ
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
//			atk_8266_test();		//½øÈëATK_ESP8266²âÊÔ
		
			File_xianshi();
		}
		
		if(key==KEY2_PRES)
		{	
//			IDentity();
			TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE );
			jiemian();
		}
		press_FR();//Ë¢Ö¸ÎÆ	

		if(key==WKUP_PRES)
		{	
			LCD_LED=1;				//µãÁÁ±³¹â
		}
	}
}
	





u8 key_input()
{
	u8 x=0,y=0,flag=0,b=0,c=0;
	keyboard(0,170,(u8**)kbd_menu);//¼ÓÔØÐéÄâ¼üÅÌ
	while(flag==0)
	{
		TP_Scan(0);
		if(tp_dev.x>0&&tp_dev.x<80&&tp_dev.y>207&&tp_dev.y<236)
		{
			x=1;b++;
			GPIO_SetBits(GPIOB,GPIO_Pin_8);//Êä³ö0£¬¹Ø±Õ·äÃùÆ÷Êä³ö
			delay_ms(300);
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);//Êä³ö0£¬¹Ø±Õ·äÃùÆ÷Êä³ö
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

//ÏÔÊ¾È·ÈÏÂë´íÎóÐÅÏ¢
void ShowErrMessage(u8 ensure)
{
	LCD_Fill(0,120,lcddev.width,160,WHITE);
	LCD_ShowString(80,120,240,16,16,"Search Error!");
	delay_ms(500);
}

//Â¼Ö¸ÎÆ
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
					Show_Str_Mid(0,100,"Çë°´Ö¸ÎÆ",16,240);
					delay_ms(500);
					ensure=PS_GetImage();
					if(ensure==0x00) 
					{
						BEEP=1;
						ensure=PS_GenChar(CharBuffer1);//Éú³ÉÌØÕ÷
						BEEP=0;
						if(ensure==0x00)
						{
							LCD_Fill(0,120,lcddev.width,160,WHITE);
							Show_Str_Mid(0,120,"Ö¸ÎÆÕý³£",16,240);
							delay_ms(500);
							i=0;
							processnum=1;//Ìøµ½µÚ¶þ²½						
						}else ShowErrMessage(ensure);				
					}else ShowErrMessage(ensure);						
					break;
				
				case 1:
					i++;
					LCD_Fill(0,100,lcddev.width,160,WHITE);
					Show_Str_Mid(0,100,"Çë°´ÔÙ°´Ò»´ÎÖ¸ÎÆ",16,240);
					delay_ms(500);
					ensure=PS_GetImage();
					if(ensure==0x00) 
					{
						BEEP=1;
						ensure=PS_GenChar(CharBuffer2);//Éú³ÉÌØÕ÷
						BEEP=0;
						if(ensure==0x00)
						{
							LCD_Fill(0,120,lcddev.width,160,WHITE);
							Show_Str_Mid(0,120,"Ö¸ÎÆÕý³£",16,240);
							delay_ms(500);
							i=0;
							processnum=2;//Ìøµ½µÚÈý²½
						}else ShowErrMessage(ensure);	
					}else ShowErrMessage(ensure);		
					break;

				case 2:
					LCD_Fill(0,100,lcddev.width,160,WHITE);
					Show_Str_Mid(0,100,"¶Ô±ÈÁ½´ÎÖ¸ÎÆ",16,240);
					delay_ms(500);
					ensure=PS_Match();
					if(ensure==0x00) 
					{
						LCD_Fill(0,120,lcddev.width,160,WHITE);
						Show_Str_Mid(0,120,"¶Ô±È³É¹¦,Á½´ÎÖ¸ÎÆÒ»Ñù",16,240);
						delay_ms(500);
						processnum=3;//Ìøµ½µÚËÄ²½
					}
					else 
					{
						LCD_Fill(0,100,lcddev.width,160,WHITE);
						Show_Str_Mid(0,100,"¶Ô±ÈÊ§°Ü£¬ÇëÖØÐÂÂ¼ÈëÖ¸ÎÆ",16,240);
						delay_ms(500);
						ShowErrMessage(ensure);
						i=0;
						processnum=0;//Ìø»ØµÚÒ»²½		
					}
					delay_ms(1200);
					break;

				case 3:
					LCD_Fill(0,100,lcddev.width,160,WHITE);
					Show_Str_Mid(0,100,"Éú³ÉÖ¸ÎÆÄ£°å",16,240);
					delay_ms(500);
					ensure=PS_RegModel();
					if(ensure==0x00) 
					{
						LCD_Fill(0,120,lcddev.width,160,WHITE);
						Show_Str_Mid(0,120,"Éú³ÉÖ¸ÎÆÄ£°å³É¹¦",16,240);
						delay_ms(500);
						processnum=4;//Ìøµ½µÚÎå²½
					}else {processnum=0;ShowErrMessage(ensure);}
					delay_ms(1200);
					break;
					
				case 4:	
					LCD_Fill(0,100,lcddev.width,160,WHITE);
					Show_Str_Mid(0,100,"ÇëÊäÈë´¢´æID,°´Enter±£´æ",16,240);
					Show_Str_Mid(0,120,"0=< ID <=299",16,240);
					do
						ID=key_input();
					
					while(!(ID<AS608Para.PS_max));//ÊäÈëID±ØÐëÐ¡ÓÚÈÝÁ¿µÄ×î´óÖµ
					ensure=PS_StoreChar(CharBuffer2,ID);//´¢´æÄ£°å
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
							Show_Str_Mid(0,120,"Â¼ÈëÖ¸ÎÆ³É¹¦",16,240);
							
						
						PS_ValidTempleteNum(&ValidN);//¶Á¿âÖ¸ÎÆ¸öÊý
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
		 if(i==5)//³¬¹ý5´ÎÃ»ÓÐ°´ÊÖÖ¸ÔòÍË³ö
			{
				LCD_Fill(0,100,lcddev.width,160,WHITE);
				break;	
			}				
	}
	TIM_Cmd(TIM3, ENABLE);
	LCD_Clear (WHITE);
}

//Ë¢Ö¸ÎÆ
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
		if(ensure==0x00)//»ñÈ¡Í¼Ïñ³É¹¦ 
		{	
			LCD_LED=1;				//µãÁÁ±³¹â
			BEEP=1;//´ò¿ª·äÃùÆ÷	
			ensure=PS_GenChar(CharBuffer1);
			if(ensure==0x00) //Éú³ÉÌØÕ÷³É¹¦
			{		
				BEEP=0;//¹Ø±Õ·äÃùÆ÷	
			}
			else
					ShowErrMessage(ensure);
				
				
			if(flag3)
			{
				ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
				if(ensure==0x00)//ËÑË÷³É¹¦
				{				
					LCD_Fill(0,100,lcddev.width,160,WHITE);
					Show_Str_Mid(0,100,"Ë¢Ö¸ÎÆ³É¹¦",16,240);				
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
				Show_Str(60,80,200,16,"Î´µ½Ç©µ½Ê±¼ä",16,0);
			
			if(flag4==1)
			{
			if(flag5==1&&flag4==1)
			{
				BEEP=1;//´ò¿ª·äÃùÆ÷	
				ensure=PS_GenChar(CharBuffer1);
				if(ensure==0x00) //Éú³ÉÌØÕ÷³É¹¦
				{		
					BEEP=0;//¹Ø±Õ·äÃùÆ÷	
					ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
					if(ensure==0x00)//ËÑË÷³É¹¦
					{				
						LCD_Fill(0,100,lcddev.width,160,WHITE);
						Show_Str_Mid(0,100,"Ë¢Ö¸ÎÆ³É¹¦",16,240);				
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
			 BEEP=0;//¹Ø±Õ·äÃùÆ÷
			 delay_ms(600);
			 LCD_Fill(0,100,lcddev.width,160,WHITE);
			}
			else
				Show_Str(60,80,200,16,"Î´µ½Ç©ÍËÊ±¼ä",16,0);
			
			
			
			}
		delay_ms (1000);
		delay_ms (1000);
		LCD_Fill(60,80,200,100,WHITE );	
		 BEEP=0;//¹Ø±Õ·äÃùÆ÷
		 delay_ms(600);
		 LCD_Fill(0,100,lcddev.width,160,WHITE);
		}
		
}

//É¾³ýÖ¸ÎÆ
void Del_FR(void)
{
	u8  ensure;
	u16 ValidN,num;
	LCD_Clear (YELLOW);
	LCD_Fill(0,100,lcddev.width,160,WHITE);
	Show_Str_Mid(0,100,"ÇëÊäÈëÒªÉ¾³ýÖ¸ÎÆID°´È·ÈÏ·¢ËÍ",16,240);
	Show_Str_Mid(0,120,"0=< ID <=299",16,240);
	delay_ms(50);
	if(e==1)
	{
	num=key_input();//»ñÈ¡·µ»ØµÄÊýÖµ
	if(num==0xFFFF)
		goto MENU ; //·µ»ØÖ÷Ò³Ãæ
	else if(num==0xFF00)
		ensure=PS_Empty();//Çå¿ÕÖ¸ÎÆ¿â
	else 
		ensure=PS_DeletChar(num,1);//É¾³ýµ¥¸öÖ¸ÎÆ
	if(ensure==0)
	{
		if(e==1)
		{
			LCD_Clear (YELLOW);
			LCD_Fill(0,120,lcddev.width,160,WHITE);
			Show_Str_Mid(0,140,"É¾³ýÖ¸ÎÆ³É¹¦",16,240);	
			delay_ms(1200);
			Del_FR();		//É¾Ö¸ÎÆ
		}
	}
  else
		ShowErrMessage(ensure);	
	delay_ms(1200);
	PS_ValidTempleteNum(&ValidN);//¶Á¿âÖ¸ÎÆ¸öÊý
	}	
	else
		goto MENU ;
MENU:	
	LCD_Fill(0,100,lcddev.width,160,WHITE);
	delay_ms(50);

	LCD_Clear (WHITE);
	TIM_Cmd(TIM3, ENABLE); 
}



//¼ÓÔØ°´¼ü½çÃæ£¨³ß´çx,yÎª240*150£©
//x,y:½çÃæÆðÊ¼×ø±ê£¨240*320·Ö±æÂÊµÄÊ±ºò£¬x±ØÐëÎª0£©
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
		if(i==1)//°´¼ü±íµÚ2¸ö¡®:¡¯²»ÐèÒªÖÐ¼äÏÔÊ¾
			Show_Str(x+(i%3)*80+2,y+7+30*(i/3),80,30,(u8*)kbd_tbl[i],16,0);	
		if(i==13)
			{
				Show_Str(20,300,200,16,"·µ»Ø",16,1);
			}
			if(i==14)
			{
				Show_Str(180,300,200,16,"È·ÈÏ",16,1); 
			}
		if(i!=13||i!=14)
			Show_Str_Mid(x+(i%3)*80,y+7+30*(i/3),(u8*)kbd_tbl[i],16,80);
	} 
}


//°´¼ü×´Ì¬ÉèÖÃ
//x,y:¼üÅÌ×ø±ê
//key:¼üÖµ(0~14)
//sta:×´Ì¬£¬0£¬ËÉ¿ª£»1£¬°´ÏÂ£»
void AS608_key_staset(u16 x,u16 y,u8 keyx,u8 sta)
{		  
	u16 i=keyx/3,j=keyx%3;
	if(keyx>16)return;
	if(sta &&keyx!=1)//°´¼ü±íµÚ2¸ö¡®:¡¯²»ÐèÒªÇå³ý
		LCD_Fill(x+j*80+1,y+i*30+1,x+j*80+78,y+i*30+28,GREEN);
	else if(keyx!=1)
		LCD_Fill(x+j*80+1,y+i*30+1,x+j*80+78,y+i*30+28,WHITE);
	if(keyx!=1)   ;//²»ÊÇ¡®£º¡¯£
		Show_Str_Mid(x+j*80,y+7+30*i,(u8*)kbd_tbl[keyx],16,80);	
}



u8 jiemian()
{
	u8 key;
	u8 return_Interface_flag=1;
	s8  choose_function_flag=0;
	LCD_Clear (WHITE);
	Show_Str(60,80,200,16,"Â¼ÈëÖ¸ÎÆ",16,1); //a=0
	Show_Str(60,100,200,16,"É¾³ýÖ¸ÎÆ",16,1); //a=1
	Show_Str(60,120,200,16,"ÉèÖÃ±¾µØÊ±¼ä",16,1); //a=2
	Show_Str(60,140,200,16,"ÉèÖÃÇ©µ½Ê±¼ä",16,1);  //a=3
	Show_Str(60,160,200,16,"ÉèÖÃÇ©ÍËÊ±¼ä",16,1);  //a=4
	Show_Str(20,300,200,16,"È·ÈÏ",16,1); 
	Show_Str(200,300,200,16,"·µ»Ø",16,1);
	
	
	while(return_Interface_flag)
	{
		Show_Str(60,80,200,16,"Â¼ÈëÖ¸ÎÆ",16,1); //a=0
		Show_Str(60,100,200,16,"É¾³ýÖ¸ÎÆ",16,1); //a=1
		Show_Str(60,120,200,16,"ÉèÖÃ±¾µØÊ±¼ä",16,1); //a=2
		Show_Str(60,140,200,16,"ÉèÖÃÇ©µ½Ê±¼ä",16,1);  //a=3
		Show_Str(60,160,200,16,"ÉèÖÃÇ©ÍËÊ±¼ä",16,1);  //a=4
		Show_Str(60,180,200,16,"ÉèÖÃÃÜÂë",16,1);  //a=5
		Show_Str(60,200,200,16,"ÉèÖÃÃ¿ÈÕÖµ°àÈËÔ±",16,1);  //a=6
		Show_Str(20,300,200,16,"È·ÈÏ",16,1); 
		Show_Str(200,300,200,16,"·µ»Ø",16,1);
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
					Add_FR();		//Â¼Ö¸ÎÆ
				count=0;
			}
			if(choose_function_flag==1)
			{ 
				LCD_Clear(YELLOW);
				e=1;
				Del_FR();		//É¾Ö¸ÎÆ
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

