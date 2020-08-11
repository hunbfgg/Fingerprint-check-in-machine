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

#define usart2_baund  9600//����2�����ʣ�����ָ��ģ�鲨���ʸ���
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
const  u8* kbd_menu[15]={"delete"," : ","input","1","2","3","4","5","6","7","8","9"," ","0"," "};//������

SysPara AS608Para;//ָ��ģ��AS608����
u16 ValidN;//ģ������Чָ�Ƹ���	

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

void Add_FR(void);	//¼ָ��
void Del_FR(void);	//ɾ��ָ��
void press_FR(void);//ˢָ��
void ShowErrMessage(u8 ensure);//��ʾȷ���������Ϣ
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
	LCD_Clear(WHITE);//����
	Show_Str(20,40,200,16,"��ʶ��ָ�ƣ�",16,1); 
	Show_Str(20,300,200,16,"����",16,1); 
	Show_Str(200,300,200,16,"����",16,1);
	
	while(flag_1)
	{
		ensure_1=PS_GetImage();
		if(ensure_1==0x00)//��ȡͼ��ɹ� 
		{	
			BEEP=1;//�򿪷�����	
			ensure_1=PS_GenChar(CharBuffer1);
			if(ensure_1==0x00) //���������ɹ�
			{		
				BEEP=0;//�رշ�����	
				ensure_1=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
				if(ensure_1==0x00)//�����ɹ�
				{				
					LCD_Fill(0,100,lcddev.width,160,WHITE);
					Show_Str_Mid(0,100,"ˢָ�Ƴɹ�",16,240);				
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
		 BEEP=0;//�رշ�����
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
			if(password_input()==1)           //������
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
	u8 fontok=0; //�����ֿ�У׼
//	count=0;
	
	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	usmart_dev.init(72);		//��ʼ��USMART		
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	usart2_init(usart2_baund);//��ʼ������2
	usart3_init(usart3_baund);		//��ʼ������3
 	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();					//��ʼ������
	LCD_Init();			   		//��ʼ��LCD 
	TP_Init();     //��������ʼ��,ע�������õ���24C02���洢У׼����,�����Ҫ24C02������֧��,��ֲ��ʱ��ע��
	W25QXX_Init();				//��ʼ��W25Q128
	PS_StaGPIO_Init();	//��ʼ��FR��״̬����
	BEEP_Init();  			//��ʼ��������
	Key_init1();
	 
	TIM3_Int_Init(1999,7199);
	RTC_Init();	  			  
//	RTC_Set(2020,2,7,17,25,50);  //����ʱ��	

/*SD����⼰�ļ�ϵͳ����*/
	while(SD_Init())//��ⲻ��SD��
	{
		LCD_ShowString(30,150,200,16,16,"SD Card Error!");
		delay_ms(500);					
		LCD_ShowString(30,150,200,16,16,"Please Check! ");
		delay_ms(500);
		LED0=!LED0;//DS0��˸
	}
 	exfuns_init();							//Ϊfatfs��ر��������ڴ�				 
  f_mount(fs[0],"0:",1); 		//����SD�� 
 	f_mount(fs[1],"1:",1); 		//����FLASH.				  			    
	
	while(PS_HandShake(&AS608Addr))//��AS608ģ������
	{
		delay_ms(300);
		LCD_Fill(0,40,240,80,WHITE);
//		Show_Str_Mid(0,40,"δ��⵽ģ��!!!",16,240);
		delay_ms(700);
		LCD_Fill(0,40,240,80,WHITE);
//		Show_Str_Mid(0,40,"��������ģ��...",16,240);		  
	}
	
/*�ֿ�У׼*/	
	fontok=font_init();			//����ֿ��Ƿ�OK
	if(fontok||key==KEY1_PRES)	//��Ҫ�����ֿ�				 
	{
		LCD_Clear(WHITE);		//����
 		POINT_COLOR=RED;		//��������Ϊ��ɫ	 						    

		key=update_font(20,110,16,"0:");//��SD������
		while(key)//����ʧ��		
		{			 		  
			LCD_ShowString(60,110,200,16,16,"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(20,110,200+20,110+16,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(60,110,200,16,16,"Font Update Success!");
		LCD_Clear(WHITE);//����	       
	}  
	
/*ָ��ģ�����*/
	ensure=PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	if(ensure!=0x00)
		ShowErrMessage(ensure);//��ʾȷ���������Ϣ	
	ensure=PS_ReadSysPara(&AS608Para);  //������ 
	myfree(SRAMIN,str);
	

	kaiji_xianshi();
	LCD_Clear(WHITE);//����
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
//			atk_8266_test();		//����ATK_ESP8266����
		
			File_xianshi();
		}
		
		if(key==KEY2_PRES)
		{	
//			IDentity();
			TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE );
			jiemian();
		}
		press_FR();//ˢָ��	

		if(key==WKUP_PRES)
		{	
			LCD_LED=1;				//��������
		}
	}
}
	





u8 key_input()
{
	u8 x=0,y=0,flag=0,b=0,c=0;
	keyboard(0,170,(u8**)kbd_menu);//�����������
	while(flag==0)
	{
		TP_Scan(0);
		if(tp_dev.x>0&&tp_dev.x<80&&tp_dev.y>207&&tp_dev.y<236)
		{
			x=1;b++;
			GPIO_SetBits(GPIOB,GPIO_Pin_8);//���0���رշ��������
			delay_ms(300);
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);//���0���رշ��������
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

//��ʾȷ���������Ϣ
void ShowErrMessage(u8 ensure)
{
	LCD_Fill(0,120,lcddev.width,160,WHITE);
	LCD_ShowString(80,120,240,16,16,"Search Error!");
	delay_ms(500);
}

//¼ָ��
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
					Show_Str_Mid(0,100,"�밴ָ��",16,240);
					delay_ms(500);
					ensure=PS_GetImage();
					if(ensure==0x00) 
					{
						BEEP=1;
						ensure=PS_GenChar(CharBuffer1);//��������
						BEEP=0;
						if(ensure==0x00)
						{
							LCD_Fill(0,120,lcddev.width,160,WHITE);
							Show_Str_Mid(0,120,"ָ������",16,240);
							delay_ms(500);
							i=0;
							processnum=1;//�����ڶ���						
						}else ShowErrMessage(ensure);				
					}else ShowErrMessage(ensure);						
					break;
				
				case 1:
					i++;
					LCD_Fill(0,100,lcddev.width,160,WHITE);
					Show_Str_Mid(0,100,"�밴�ٰ�һ��ָ��",16,240);
					delay_ms(500);
					ensure=PS_GetImage();
					if(ensure==0x00) 
					{
						BEEP=1;
						ensure=PS_GenChar(CharBuffer2);//��������
						BEEP=0;
						if(ensure==0x00)
						{
							LCD_Fill(0,120,lcddev.width,160,WHITE);
							Show_Str_Mid(0,120,"ָ������",16,240);
							delay_ms(500);
							i=0;
							processnum=2;//����������
						}else ShowErrMessage(ensure);	
					}else ShowErrMessage(ensure);		
					break;

				case 2:
					LCD_Fill(0,100,lcddev.width,160,WHITE);
					Show_Str_Mid(0,100,"�Ա�����ָ��",16,240);
					delay_ms(500);
					ensure=PS_Match();
					if(ensure==0x00) 
					{
						LCD_Fill(0,120,lcddev.width,160,WHITE);
						Show_Str_Mid(0,120,"�Աȳɹ�,����ָ��һ��",16,240);
						delay_ms(500);
						processnum=3;//�������Ĳ�
					}
					else 
					{
						LCD_Fill(0,100,lcddev.width,160,WHITE);
						Show_Str_Mid(0,100,"�Ա�ʧ�ܣ�������¼��ָ��",16,240);
						delay_ms(500);
						ShowErrMessage(ensure);
						i=0;
						processnum=0;//���ص�һ��		
					}
					delay_ms(1200);
					break;

				case 3:
					LCD_Fill(0,100,lcddev.width,160,WHITE);
					Show_Str_Mid(0,100,"����ָ��ģ��",16,240);
					delay_ms(500);
					ensure=PS_RegModel();
					if(ensure==0x00) 
					{
						LCD_Fill(0,120,lcddev.width,160,WHITE);
						Show_Str_Mid(0,120,"����ָ��ģ��ɹ�",16,240);
						delay_ms(500);
						processnum=4;//�������岽
					}else {processnum=0;ShowErrMessage(ensure);}
					delay_ms(1200);
					break;
					
				case 4:	
					LCD_Fill(0,100,lcddev.width,160,WHITE);
					Show_Str_Mid(0,100,"�����봢��ID,��Enter����",16,240);
					Show_Str_Mid(0,120,"0=< ID <=299",16,240);
					do
						ID=key_input();
					
					while(!(ID<AS608Para.PS_max));//����ID����С�����������ֵ
					ensure=PS_StoreChar(CharBuffer2,ID);//����ģ��
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
							Show_Str_Mid(0,120,"¼��ָ�Ƴɹ�",16,240);
							
						
						PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
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
		 if(i==5)//����5��û�а���ָ���˳�
			{
				LCD_Fill(0,100,lcddev.width,160,WHITE);
				break;	
			}				
	}
	TIM_Cmd(TIM3, ENABLE);
	LCD_Clear (WHITE);
}

//ˢָ��
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
		if(ensure==0x00)//��ȡͼ��ɹ� 
		{	
			LCD_LED=1;				//��������
			BEEP=1;//�򿪷�����	
			ensure=PS_GenChar(CharBuffer1);
			if(ensure==0x00) //���������ɹ�
			{		
				BEEP=0;//�رշ�����	
			}
			else
					ShowErrMessage(ensure);
				
				
			if(flag3)
			{
				ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
				if(ensure==0x00)//�����ɹ�
				{				
					LCD_Fill(0,100,lcddev.width,160,WHITE);
					Show_Str_Mid(0,100,"ˢָ�Ƴɹ�",16,240);				
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
				Show_Str(60,80,200,16,"δ��ǩ��ʱ��",16,0);
			
			if(flag4==1)
			{
			if(flag5==1&&flag4==1)
			{
				BEEP=1;//�򿪷�����	
				ensure=PS_GenChar(CharBuffer1);
				if(ensure==0x00) //���������ɹ�
				{		
					BEEP=0;//�رշ�����	
					ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
					if(ensure==0x00)//�����ɹ�
					{				
						LCD_Fill(0,100,lcddev.width,160,WHITE);
						Show_Str_Mid(0,100,"ˢָ�Ƴɹ�",16,240);				
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
			 BEEP=0;//�رշ�����
			 delay_ms(600);
			 LCD_Fill(0,100,lcddev.width,160,WHITE);
			}
			else
				Show_Str(60,80,200,16,"δ��ǩ��ʱ��",16,0);
			
			
			
			}
		delay_ms (1000);
		delay_ms (1000);
		LCD_Fill(60,80,200,100,WHITE );	
		 BEEP=0;//�رշ�����
		 delay_ms(600);
		 LCD_Fill(0,100,lcddev.width,160,WHITE);
		}
		
}

//ɾ��ָ��
void Del_FR(void)
{
	u8  ensure;
	u16 ValidN,num;
	LCD_Clear (YELLOW);
	LCD_Fill(0,100,lcddev.width,160,WHITE);
	Show_Str_Mid(0,100,"������Ҫɾ��ָ��ID��ȷ�Ϸ���",16,240);
	Show_Str_Mid(0,120,"0=< ID <=299",16,240);
	delay_ms(50);
	if(e==1)
	{
	num=key_input();//��ȡ���ص���ֵ
	if(num==0xFFFF)
		goto MENU ; //������ҳ��
	else if(num==0xFF00)
		ensure=PS_Empty();//���ָ�ƿ�
	else 
		ensure=PS_DeletChar(num,1);//ɾ������ָ��
	if(ensure==0)
	{
		if(e==1)
		{
			LCD_Clear (YELLOW);
			LCD_Fill(0,120,lcddev.width,160,WHITE);
			Show_Str_Mid(0,140,"ɾ��ָ�Ƴɹ�",16,240);	
			delay_ms(1200);
			Del_FR();		//ɾָ��
		}
	}
  else
		ShowErrMessage(ensure);	
	delay_ms(1200);
	PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	}	
	else
		goto MENU ;
MENU:	
	LCD_Fill(0,100,lcddev.width,160,WHITE);
	delay_ms(50);

	LCD_Clear (WHITE);
	TIM_Cmd(TIM3, ENABLE); 
}



//���ذ������棨�ߴ�x,yΪ240*150��
//x,y:������ʼ���꣨240*320�ֱ��ʵ�ʱ��x����Ϊ0��
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
		if(i==1)//�������2����:������Ҫ�м���ʾ
			Show_Str(x+(i%3)*80+2,y+7+30*(i/3),80,30,(u8*)kbd_tbl[i],16,0);	
		if(i==13)
			{
				Show_Str(20,300,200,16,"����",16,1);
			}
			if(i==14)
			{
				Show_Str(180,300,200,16,"ȷ��",16,1); 
			}
		if(i!=13||i!=14)
			Show_Str_Mid(x+(i%3)*80,y+7+30*(i/3),(u8*)kbd_tbl[i],16,80);
	} 
}


//����״̬����
//x,y:��������
//key:��ֵ(0~14)
//sta:״̬��0���ɿ���1�����£�
void AS608_key_staset(u16 x,u16 y,u8 keyx,u8 sta)
{		  
	u16 i=keyx/3,j=keyx%3;
	if(keyx>16)return;
	if(sta &&keyx!=1)//�������2����:������Ҫ���
		LCD_Fill(x+j*80+1,y+i*30+1,x+j*80+78,y+i*30+28,GREEN);
	else if(keyx!=1)
		LCD_Fill(x+j*80+1,y+i*30+1,x+j*80+78,y+i*30+28,WHITE);
	if(keyx!=1)   ;//���ǡ������
		Show_Str_Mid(x+j*80,y+7+30*i,(u8*)kbd_tbl[keyx],16,80);	
}



u8 jiemian()
{
	u8 key;
	u8 return_Interface_flag=1;
	s8  choose_function_flag=0;
	LCD_Clear (WHITE);
	Show_Str(60,80,200,16,"¼��ָ��",16,1); //a=0
	Show_Str(60,100,200,16,"ɾ��ָ��",16,1); //a=1
	Show_Str(60,120,200,16,"���ñ���ʱ��",16,1); //a=2
	Show_Str(60,140,200,16,"����ǩ��ʱ��",16,1);  //a=3
	Show_Str(60,160,200,16,"����ǩ��ʱ��",16,1);  //a=4
	Show_Str(20,300,200,16,"ȷ��",16,1); 
	Show_Str(200,300,200,16,"����",16,1);
	
	
	while(return_Interface_flag)
	{
		Show_Str(60,80,200,16,"¼��ָ��",16,1); //a=0
		Show_Str(60,100,200,16,"ɾ��ָ��",16,1); //a=1
		Show_Str(60,120,200,16,"���ñ���ʱ��",16,1); //a=2
		Show_Str(60,140,200,16,"����ǩ��ʱ��",16,1);  //a=3
		Show_Str(60,160,200,16,"����ǩ��ʱ��",16,1);  //a=4
		Show_Str(60,180,200,16,"��������",16,1);  //a=5
		Show_Str(60,200,200,16,"����ÿ��ֵ����Ա",16,1);  //a=6
		Show_Str(20,300,200,16,"ȷ��",16,1); 
		Show_Str(200,300,200,16,"����",16,1);
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
					Add_FR();		//¼ָ��
				count=0;
			}
			if(choose_function_flag==1)
			{ 
				LCD_Clear(YELLOW);
				e=1;
				Del_FR();		//ɾָ��
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

