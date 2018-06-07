#include <AT89X52.h>
#include <intrins.h>
/************************************LCD1602��ʾģ�����*************************************/
sbit LCD_RS=P3^0;//��������
sbit LCD_RW=P3^1;
sbit LCD_E=P3^2;
#define LCD_Data P2
#define Busy    0x80 //���ڼ��LCD״̬���е�Busy��ʶ

void WriteDataLCD(unsigned char WDLCD);
void WriteCommandLCD(unsigned char WCLCD,BuysC);
unsigned char ReadStatusLCD(void);
void LCDInit(void);
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData);
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char code *DData);
/************************************LCD1602��ʾģ�����*************************************/

/************************************�¶���ʾ*************************************/
float upper_t = 40.0;//Ĭ���¶�����
float lower_t = 20.0;//Ĭ���¶�����
float current_t = 0;//��ǰ�¶�

void Display_c(unsigned char Y);//��ʾ�¶ȵ�λ
/************************************�¶���ʾ*************************************/

/************************************���̴������*************************************/
#define Keyboard_data P1    //������̽ӿ�
float idata input_value = 0;//����ֵ�ݴ����
bit H=0;//����H��־λ
bit L=0;//����L��־λ
bit X=0;//С����־
bit OK = 0;//���̵ȴ���־
void keyboard(void);//���̴������
/************************************���̴������*************************************/

/************************************��������ʾ*************************************/
void Display_float(unsigned char X,unsigned Y,float o_char);//����ת�ַ���
/************************************��������ʾ*************************************/

/************************************DB18S20�������*************************************/
sbit DQ=P3^3; 				//���ݴ����߽ӵ�Ƭ������Ӧ������ 
bit fg=1;        			//�¶�������־
void Init_DS18B20(void);
unsigned char ReadOneChar(void);
void WriteOneChar(unsigned char dat);
void ReadTemperature(void);
/************************************DB18S20�������*************************************/

/************************************������ʱ����*************************************/
void delay(unsigned char i);//�Զ�����ʱ����
void Delay5Ms(void);
void Delay400Ms(void);
/* �����ʱ��������ʱʱ��(t*10)us */
void DelayX10us(unsigned char t);
/************************************������ʱ����*************************************/

/************************************��������led*************************************/
sbit F = P3^4;
sbit LED = P3^5;
/************************************��������led*************************************/
/*-------------------------------------������---------------------------------------*/
void main(void)
{	
	EA = 0;//�����ж�
	F = 1;
	LED = 1;
	DQ=1; 
  Delay400Ms(); //�����ȴ�����LCD���빤��״̬
  LCDInit(); //LCM��ʼ��
  Delay5Ms(); //��ʱƬ��(�ɲ�Ҫ)
	Init_DS18B20(); 	//1820��ʼ��
  while(1){
		keyboard();
		if(!OK){
			ReadTemperature();//��ȡ�¶�ֵ
			if(fg){
				DisplayListChar(0, 0, "Curre Tem:");//��ʾ��ǰ�¶�
				Display_float(10,1,current_t);//��ʾ��ǰ�¶�
				Display_c(1);//��ʾ�¶ȵ�λ
			}else{//���¶���ʾ
				DisplayListChar(0, 0, "Curre Tem:");//��ʾ��ǰ�¶�
				Display_float(10,1,current_t);//��ʾ��ǰ�¶�
				Display_c(1);//��ʾ�¶ȵ�λ
				DisplayOneChar(9, 1, '-');//��ʾ��ǰ�¶�
			}
			
			if(current_t<lower_t)//�¶ȵ����趨�¶�����
			{
				while(current_t<lower_t){
					F = 0;
					LED = 0;
					Delay400Ms();
					Delay400Ms();
					Delay400Ms();
					F = 1;
					LED = 1;
					Delay400Ms();
					Delay400Ms();
					Delay400Ms();
					ReadTemperature();//��ȡ�¶�ֵ
					Display_float(10,1,current_t);//��ʾ��ǰ�¶�
					Display_c(1);//��ʾ�¶ȵ�λ
				}
			}else if(current_t > upper_t){//�����¶�����
				while(current_t > upper_t){
					F = 0;
					LED = 0;
					Delay400Ms();
					Delay400Ms();
					Delay400Ms();
					Delay400Ms();
					Delay400Ms();
					F = 1;
					LED = 1;
					Delay400Ms();
					Delay400Ms();
					Delay400Ms();
					Delay400Ms();
					Delay400Ms();
					ReadTemperature();//��ȡ�¶�ֵ
					Display_float(10,1,current_t);//��ʾ��ǰ�¶�
					Display_c(1);//��ʾ�¶ȵ�λ
				}
			}else{
				Delay400Ms();
				Delay400Ms();
				Delay400Ms();
			}
		}
	}
}
/*-------------------------------------������---------------------------------------*/
/************************************LCD1602��ʾģ�����*************************************/
void WriteDataLCD(unsigned char WDLCD)//д����
{
 ReadStatusLCD(); //���æ
 LCD_Data = WDLCD;
 LCD_RS = 1;
 LCD_RW = 0;
 LCD_E = 0; //�������ٶ�̫�߿���������С����ʱ
 LCD_E = 0; //��ʱ
 LCD_E = 1;
}
void WriteCommandLCD(unsigned char WCLCD,BuysC) //дָ��//BuysCΪ0ʱ����æ���
{
 if (BuysC) ReadStatusLCD(); //������Ҫ���æ
 LCD_Data = WCLCD;
 LCD_RS = 0;
 LCD_RW = 0; 
 LCD_E = 0;
 LCD_E = 0;
 LCD_E = 1; 
}
unsigned char ReadStatusLCD(void)//��״̬
{
 LCD_Data = 0xFF; 
 LCD_RS = 0;
 LCD_RW = 1;
 LCD_E = 0;
 LCD_E = 0;
 LCD_E = 1;
 while (LCD_Data & Busy); //���æ�ź�
 return(LCD_Data);
}
void LCDInit(void) //LCM��ʼ��
{
 LCD_Data = 0;
 WriteCommandLCD(0x38,0); //������ʾģʽ���ã������æ�ź�
 Delay5Ms(); 
 WriteCommandLCD(0x38,0);
 Delay5Ms(); 
 WriteCommandLCD(0x38,0);
 Delay5Ms(); 

 WriteCommandLCD(0x38,1); //��ʾģʽ����,��ʼҪ��ÿ�μ��æ�ź�
 WriteCommandLCD(0x08,1); //�ر���ʾ
 WriteCommandLCD(0x01,1); //��ʾ����
 WriteCommandLCD(0x06,1); // ��ʾ����ƶ�����
 WriteCommandLCD(0x0C,1); // ��ʾ�����������
}
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)//��ָ��λ����ʾһ���ַ�
{
 Y &= 0x1;
 X &= 0xF; //����X���ܴ���15��Y���ܴ���1
 if (Y) X |= 0x40; //��Ҫ��ʾ�ڶ���ʱ��ַ��+0x40;
 X |= 0x80; // ���ָ����
 WriteCommandLCD(X, 0); //���ﲻ���æ�źţ����͵�ַ��
 WriteDataLCD(DData);
}
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char code *DData)//��ָ��λ����ʾһ���ַ�
{
 unsigned char ListLength;

  ListLength = 0;
 Y &= 0x1;
 X &= 0xF; //����X���ܴ���15��Y���ܴ���1
 while (DData[ListLength]>=0x20) //�������ִ�β���˳�
  {
   if (X <= 0xF) //X����ӦС��0xF
    {
     DisplayOneChar(X, Y, DData[ListLength]); //��ʾ�����ַ�
     ListLength++;
     X++;
    }
  }
}
/************************************LCD1602��ʾģ�����*************************************/

/************************************�¶���ʾ*************************************/

void Display_c(unsigned char Y)//��ʾ�¶ȵ�λ������Y��ʾ�ڼ���
{
	Y &= 0x1;
	DisplayOneChar(14,Y,0xdf);//��ʾ�¶ȵ�λ��Բ��
  DisplayOneChar(15,Y,'C');//��ʾ�¶ȵ�λ��C
}
/************************************�¶���ʾ*************************************/

/************************************���̴������*************************************/
void keyboard(void)//���̴������
{
	unsigned char temp_c;//��ֵ����ֵ
	unsigned char temp_r;//��ֵ����ֵ
	unsigned char key_value='k';//����ֵ
	Keyboard_data = 0xF0;//��P0�˿ڳ�ʼ��������ֵ
	
	if(Keyboard_data == 0xF0)
		return;
	Delay5Ms();//��ʱȥ����
	if(Keyboard_data == 0xF0)
		return;
	temp_c = Keyboard_data;
	
	WriteCommandLCD(0x01,1); //�а������£�LCD��ʾ������
	OK = 1;
	switch(temp_c){
		case 0xE0://��һ��
			Keyboard_data = 0x0F;
			temp_r = Keyboard_data;
		  while(Keyboard_data != 0x0F);
			switch(temp_r){
				case 0x0E://��һ��
					key_value = 1;
					break;
				case 0x0D://�ڶ���
					key_value = 4;
					break;
				case 0x0B://������
					key_value = 7;
					break;
				case 0x07://������
					key_value = 'c';
					break;
				default:
					DisplayListChar(0, 5, "Key Row Error!");
					break;
			}
			break;
		case 0xD0://�ڶ���
			Keyboard_data = 0x0F;
			temp_r = Keyboard_data;
		  while(Keyboard_data != 0x0F);
			switch(temp_r){
				case 0x0E://��һ��
					key_value = 2;
					break;
				case 0x0D://�ڶ���
					key_value = 5;
					break;
				case 0x0B://������
					key_value = 8;
					break;
				case 0x07://������
					key_value = 0;
					break;
				default:
					DisplayListChar(0, 5, "Key Row Error!");
					break;
			}
			break;
		case 0xB0://������
			Keyboard_data = 0x0F;
			temp_r = Keyboard_data;
		  while(Keyboard_data != 0x0F);
			switch(temp_r){
				case 0x0E://��һ��
					key_value = 3;
					break;
				case 0x0D://�ڶ���
					key_value = 6;
					break;
				case 0x0B://������
					key_value = 9;
					break;
				case 0x07://������
					key_value = 'o';
					break;
				default:
					DisplayListChar(0, 5, "Key Row Error!");
					break;
			}
			break;
		case 0x70://������
			Keyboard_data = 0x0F;
			temp_r = Keyboard_data;
		  while(Keyboard_data != 0x0F);
			switch(temp_r){
				case 0x0E://��һ��
					key_value = 'h';
					break;
				case 0x0D://�ڶ���
					key_value = 'l';
					break;
				case 0x0B://������
					key_value = 'x';
					break;
				case 0x07://������
					key_value = 'k';
					//�鿴�¶��ϡ����޵�����ֵ
					DisplayListChar(0, 0, "Upper Tem:");//�¶�����
					Display_float(10,0,upper_t);
					Display_c(0);
					DisplayListChar(0, 5, "Lower Tem:");//�¶�����
					Display_float(10,1,lower_t);
					Display_c(1);	
					Delay400Ms();// ��ʱ�ȴ�
					Delay400Ms();// ��ʱ�ȴ�
					Delay400Ms();// ��ʱ�ȴ�
					Delay400Ms();// ��ʱ�ȴ�
					OK = 0;//�˳������¼�
					H = 0;//�¶����ޱ�־����
					L = 0;//�¶����ޱ�־����
					WriteCommandLCD(0x01,1); //�а������£�LCD��ʾ������
					break;
				default:
					DisplayListChar(0, 5, "Key Row Error!");
					break;
			}
			break;
		default:
			DisplayListChar(0, 5, "Key Col Error!");
			break;
	}
	switch(key_value){
		case 'k'://Ĭ�ϳ�ʼֵ
			break;
		case 'h'://�¶����ޱ�־
			H = 1;//�¶��������ñ�־λ
			L = 0;//�¶����ޱ�־����
			DisplayListChar(0,0, "Input Val:");
			DisplayListChar(0,1,"Upper Tem:");
			DisplayListChar(9,1,"pera...");//��ʾ�¶�ȫӢ��
			break;
		case 'l'://�¶����ޱ�־
			H = 0;//�¶����ޱ�־����
			L = 1;//�¶��������ñ�־λ
			DisplayListChar(0,0, "Input Val:");
			DisplayListChar(0,1,"Lower Tem:");
			DisplayListChar(9,1,"pera...");//��ʾ�¶�ȫӢ��
			break;
		case 'c'://ȡ�����ð���
			DisplayListChar(0,0,"Cancel input OK!");
			input_value = 0;
			H = 0;
			L = 0;
			X = 0;
			OK = 0;
			Delay400Ms();
			Delay400Ms();
			WriteCommandLCD(0x01,1); //�а������£�LCD��ʾ������
			break;
		case 'x'://С���㰴��
			if(input_value != 0){
				X = 1;
				//��ʾ��ʾ����
				if(H){
					DisplayListChar(0,0, "Input Val:");
					DisplayListChar(0,1,"Upper Tem:");
					DisplayListChar(9,1,"pera...");//��ʾ�¶�ȫӢ��
				}else if(L){
					DisplayListChar(0,0, "Input Val:");
					DisplayListChar(0,1,"Lower Tem:");
					DisplayListChar(9,1,"pera...");//��ʾ�¶�ȫӢ��
				}
				//��ʾ����
				DisplayOneChar(10,0,input_value/10+0x30);
				DisplayOneChar(11,0,(int)input_value%10+0x30);
				DisplayOneChar(12,0,'.');
			}else{
				DisplayListChar(0,0,"Input Err!");
			}			
			break;
		case 'o'://ȷ�ϼ�
			if(input_value != 0){
				if(H){
					upper_t = input_value;
					DisplayListChar(0,0,"Set Upper_t OK!");
				}else if(L){
					lower_t = input_value;
					DisplayListChar(0,0,"Set Lower_t OK!");
				}
				if(upper_t < lower_t){
					upper_t = 40.0;
					lower_t = 20.0;
					DisplayListChar(0,0,"Temp Set Err!");
					DisplayListChar(0,1,"Temp Is Default");
				}
				input_value = 0;
				H = 0;
				L = 0;
				X = 0;
				OK = 0;
				Delay400Ms();
				Delay400Ms();
				WriteCommandLCD(0x01,1); //�а������£�LCD��ʾ������
			}else{
				DisplayListChar(0,0,"Input Err!");
			}			
			break;
		default://��ֵ
			if(!(H||L)){
				DisplayListChar(0,0,"Upper OR Lower?");
			}else{
				if(H){
					DisplayListChar(0,0, "Input Val:");
					DisplayListChar(0,1,"Upper Tem:");
					DisplayListChar(9,1,"pera...");//��ʾ�¶�ȫӢ��
				}else if(L){
					DisplayListChar(0,0, "Input Val:");
					DisplayListChar(0,1,"Lower Tem:");
					DisplayListChar(9,1,"pera...");//��ʾ�¶�ȫӢ��
				}
				//---------------��ֵ����-----------------------
				if(input_value == 0){
					input_value = key_value * 10;//ʮλ����
					DisplayOneChar(10,0,key_value+0x30);
				}else if(X){
					input_value = input_value + key_value * 0.1;//С��λ����
					Display_float(10,0,input_value);
					Display_c(0);
				}else{
					if(input_value >= 10){
						input_value = input_value + key_value;//��λ����
						DisplayOneChar(10,0,input_value/10+0x30);
						DisplayOneChar(11,0,key_value+0x30);
					}else{
						DisplayListChar(0,0,"Temp Set Err!");
						DisplayListChar(0,1,"Temp Is Default");
						input_value = 0;
						H = 0;
						L = 0;
						X = 0;
						OK = 0;
					}					
				}
			}
			break;
	}
	Keyboard_data = 0xF0;//������P1�˿ڳ�ʼ��������ֵ
}
/************************************���̴������*************************************/

/************************************��������ʾ*************************************/
void Display_float(unsigned char X,unsigned Y,float o_char)//����ת�ַ���
{
	unsigned int  buffer3,buffer4,buffer5,buffer6;
	
	if(o_char > 100){
		DisplayListChar(X,Y,"Err---");
	}else{
		o_char = o_char * 100;//ȥ��С��
		buffer3 = (int)o_char;
		
		buffer4 = buffer3/1000;//ʮλ
		buffer5 = buffer3 % 1000 / 100;//��λ
		buffer6 = buffer3 % 1000 % 100/10;//С��λ
		
		DisplayOneChar(X, Y, buffer4+0x30);
		DisplayOneChar(X+1, Y, buffer5+0x30);
		DisplayOneChar(X+2, Y, '.');
		DisplayOneChar(X+3, Y, buffer6+0x30);
	}
	
}
/************************************��������ʾ*************************************/

/************************************DB18S20�������*************************************/
void Init_DS18B20(void) 
{
	unsigned char x=0;
	DQ=1; 					//DQ���ø� 
	delay(8); 				//����ʱ
	DQ=0; 					//���͸�λ���� 
	delay(80); 				//��ʱ��>480us) 
	DQ=1; 					//���������� 
	delay(5); 				//�ȴ���15~60us) 
	x=DQ; 					//��X��ֵ���жϳ�ʼ����û�гɹ���18B20���ڵĻ�X=0������X=1 
	delay(20); 
}

//��һ���ֽ�
unsigned char ReadOneChar(void)  			//�����������ȴӸ������͵�ƽ1us���ϣ���ʹ��������Ϊ�ߵ�ƽ���Ӷ��������ź�
{
	unsigned char i=0; 		//ÿ����������̵ĳ���ʱ��Ϊ60us������������֮�������1us���ϵĸߵ�ƽ�ָ���
	unsigned char dat=0; 
	for (i=8;i>0;i--) 		//һ���ֽ���8λ 
	{
		DQ=1; 
		delay(1); 
		DQ=0;
		dat>>=1; 
		DQ=1; 
		if(DQ) 
		dat|=0x80; 
		delay(4);
	} 
	return(dat);
}

//дһ���ֽ�
void WriteOneChar(unsigned char dat) 
{ 
	unsigned char i=0; 		//�����ߴӸߵ�ƽ�����͵�ƽ������д��ʼ�źš�15us֮�ڽ�����д��λ�͵��������ϣ�
	for(i=8;i>0;i--) 		//��15~60us֮��������߽��в���������Ǹߵ�ƽ��д1����д0������ 
	{
		DQ=0; 				//�ڿ�ʼ��һ��д����ǰ������1us���ϵĸߵ�ƽ�ָ��ڡ� 
		DQ=dat&0x01; 
		delay(5); 
		DQ=1; 
		dat>>=1;
	} 
	delay(4);
}

//���¶�ֵ����λ��tempL;��λ��tempH;��
void ReadTemperature(void)	
{ 
	unsigned int t1;
	unsigned char tempL=0; 		//��ȫ�ֱ���
	unsigned char tempH=0; 
	unsigned int sdata=0;			//���������¶ȵ���������
	unsigned char xiaoshu1;		//С����һλ
	current_t = 0;//��ʼ����ǰ�¶�
	
	Init_DS18B20(); 					//��ʼ��
	WriteOneChar(0xcc); 				//���������кŵĲ���
	WriteOneChar(0x44); 				//�����¶�ת��
	Delay400Ms(); 						//ת����Ҫһ��ʱ�䣬��ʱ 
	Delay400Ms();
	Init_DS18B20(); 					//��ʼ��
	WriteOneChar(0xcc); 				//���������кŵĲ��� 
	WriteOneChar(0xbe); 				//���¶ȼĴ�����ͷ����ֵ�ֱ�Ϊ�¶ȵĵ�λ�͸�λ�� 
	tempL=ReadOneChar(); 				//�����¶ȵĵ�λLSB
	tempH=ReadOneChar(); 				//�����¶ȵĸ�λMSB	
	//if(tempH>0x7f)      				//���λΪ1ʱ�¶��Ǹ�
	//{
		//tempL=~tempL;					//����ת����ȡ����һ
		//tempH=~tempH+1;       
		//fg=0;      						//��ȡ�¶�Ϊ��ʱfg=0
	//}else{
		//fg=1;
	//}
	
	//sdata = tempL/16+tempH*16;      	//��������
	//xiaoshu1 = (tempL&0x0f)*10/16; 		//С����һλ
	//current_t = (float)sdata+xiaoshu1*0.1;
	
	t1 = tempH;
	t1 = t1<<8 | tempL;
	if(tempH>0x7f){
		t1 = ~t1+1;
		fg = 0;
	}else{
		fg = 1;
	}
	current_t = t1*0.0625;
	current_t = current_t*10+0.5;
	current_t = current_t*0.1;
}
/************************************DB18S20�������*************************************/

/************************************������ʱ����*************************************/
void delay(unsigned char i)//�Զ�����ʱ����
{
	for(i;i>0;i--);
}
void Delay5Ms(void)//5ms��ʱ
{
 unsigned int TempCyc = 5552;
 while(TempCyc--);
}
void Delay400Ms(void)//400ms��ʱ
{
 unsigned char TempCycA = 5;
 unsigned int TempCycB;
 while(TempCycA--)
 {
  TempCycB=7269;
  while(TempCycB--);
 }
}
/* �����ʱ��������ʱʱ��(t*10)us */
void DelayX10us(unsigned char t){
    do {
        _nop_();//��ʱһ���������� 12M����Ļ� ����1us ֻ��Ϊ�˸���ȷ����ʱ
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
    } while (--t);
}
/************************************������ʱ����*************************************/
