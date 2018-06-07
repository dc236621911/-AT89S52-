#include <AT89X52.h>
#include <intrins.h>
/************************************LCD1602显示模块代码*************************************/
sbit LCD_RS=P3^0;//定义引脚
sbit LCD_RW=P3^1;
sbit LCD_E=P3^2;
#define LCD_Data P2
#define Busy    0x80 //用于检测LCD状态字中的Busy标识

void WriteDataLCD(unsigned char WDLCD);
void WriteCommandLCD(unsigned char WCLCD,BuysC);
unsigned char ReadStatusLCD(void);
void LCDInit(void);
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData);
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char code *DData);
/************************************LCD1602显示模块代码*************************************/

/************************************温度显示*************************************/
float upper_t = 40.0;//默认温度上限
float lower_t = 20.0;//默认温度下限
float current_t = 0;//当前温度

void Display_c(unsigned char Y);//显示温度单位
/************************************温度显示*************************************/

/************************************键盘处理程序*************************************/
#define Keyboard_data P1    //定义键盘接口
float idata input_value = 0;//输入值暂存变量
bit H=0;//键盘H标志位
bit L=0;//键盘L标志位
bit X=0;//小数标志
bit OK = 0;//键盘等待标志
void keyboard(void);//键盘处理程序
/************************************键盘处理程序*************************************/

/************************************浮点数显示*************************************/
void Display_float(unsigned char X,unsigned Y,float o_char);//数字转字符串
/************************************浮点数显示*************************************/

/************************************DB18S20处理程序*************************************/
sbit DQ=P3^3; 				//数据传输线接单片机的相应的引脚 
bit fg=1;        			//温度正负标志
void Init_DS18B20(void);
unsigned char ReadOneChar(void);
void WriteOneChar(unsigned char dat);
void ReadTemperature(void);
/************************************DB18S20处理程序*************************************/

/************************************公共延时程序*************************************/
void delay(unsigned char i);//自定义延时参数
void Delay5Ms(void);
void Delay400Ms(void);
/* 软件延时函数，延时时间(t*10)us */
void DelayX10us(unsigned char t);
/************************************公共延时程序*************************************/

/************************************蜂鸣器、led*************************************/
sbit F = P3^4;
sbit LED = P3^5;
/************************************蜂鸣器、led*************************************/
/*-------------------------------------主程序---------------------------------------*/
void main(void)
{	
	EA = 0;//关总中断
	F = 1;
	LED = 1;
	DQ=1; 
  Delay400Ms(); //启动等待，等LCD讲入工作状态
  LCDInit(); //LCM初始化
  Delay5Ms(); //延时片刻(可不要)
	Init_DS18B20(); 	//1820初始化
  while(1){
		keyboard();
		if(!OK){
			ReadTemperature();//读取温度值
			if(fg){
				DisplayListChar(0, 0, "Curre Tem:");//显示当前温度
				Display_float(10,1,current_t);//显示当前温度
				Display_c(1);//显示温度单位
			}else{//负温度显示
				DisplayListChar(0, 0, "Curre Tem:");//显示当前温度
				Display_float(10,1,current_t);//显示当前温度
				Display_c(1);//显示温度单位
				DisplayOneChar(9, 1, '-');//显示当前温度
			}
			
			if(current_t<lower_t)//温度低于设定温度下限
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
					ReadTemperature();//读取温度值
					Display_float(10,1,current_t);//显示当前温度
					Display_c(1);//显示温度单位
				}
			}else if(current_t > upper_t){//高于温度上限
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
					ReadTemperature();//读取温度值
					Display_float(10,1,current_t);//显示当前温度
					Display_c(1);//显示温度单位
				}
			}else{
				Delay400Ms();
				Delay400Ms();
				Delay400Ms();
			}
		}
	}
}
/*-------------------------------------主程序---------------------------------------*/
/************************************LCD1602显示模块代码*************************************/
void WriteDataLCD(unsigned char WDLCD)//写数据
{
 ReadStatusLCD(); //检测忙
 LCD_Data = WDLCD;
 LCD_RS = 1;
 LCD_RW = 0;
 LCD_E = 0; //若晶振速度太高可以在这后加小的延时
 LCD_E = 0; //延时
 LCD_E = 1;
}
void WriteCommandLCD(unsigned char WCLCD,BuysC) //写指令//BuysC为0时忽略忙检测
{
 if (BuysC) ReadStatusLCD(); //根据需要检测忙
 LCD_Data = WCLCD;
 LCD_RS = 0;
 LCD_RW = 0; 
 LCD_E = 0;
 LCD_E = 0;
 LCD_E = 1; 
}
unsigned char ReadStatusLCD(void)//读状态
{
 LCD_Data = 0xFF; 
 LCD_RS = 0;
 LCD_RW = 1;
 LCD_E = 0;
 LCD_E = 0;
 LCD_E = 1;
 while (LCD_Data & Busy); //检测忙信号
 return(LCD_Data);
}
void LCDInit(void) //LCM初始化
{
 LCD_Data = 0;
 WriteCommandLCD(0x38,0); //三次显示模式设置，不检测忙信号
 Delay5Ms(); 
 WriteCommandLCD(0x38,0);
 Delay5Ms(); 
 WriteCommandLCD(0x38,0);
 Delay5Ms(); 

 WriteCommandLCD(0x38,1); //显示模式设置,开始要求每次检测忙信号
 WriteCommandLCD(0x08,1); //关闭显示
 WriteCommandLCD(0x01,1); //显示清屏
 WriteCommandLCD(0x06,1); // 显示光标移动设置
 WriteCommandLCD(0x0C,1); // 显示开及光标设置
}
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)//按指定位置显示一个字符
{
 Y &= 0x1;
 X &= 0xF; //限制X不能大于15，Y不能大于1
 if (Y) X |= 0x40; //当要显示第二行时地址码+0x40;
 X |= 0x80; // 算出指令码
 WriteCommandLCD(X, 0); //这里不检测忙信号，发送地址码
 WriteDataLCD(DData);
}
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char code *DData)//按指定位置显示一串字符
{
 unsigned char ListLength;

  ListLength = 0;
 Y &= 0x1;
 X &= 0xF; //限制X不能大于15，Y不能大于1
 while (DData[ListLength]>=0x20) //若到达字串尾则退出
  {
   if (X <= 0xF) //X坐标应小于0xF
    {
     DisplayOneChar(X, Y, DData[ListLength]); //显示单个字符
     ListLength++;
     X++;
    }
  }
}
/************************************LCD1602显示模块代码*************************************/

/************************************温度显示*************************************/

void Display_c(unsigned char Y)//显示温度单位，参数Y表示第几行
{
	Y &= 0x1;
	DisplayOneChar(14,Y,0xdf);//显示温度单位的圆点
  DisplayOneChar(15,Y,'C');//显示温度单位的C
}
/************************************温度显示*************************************/

/************************************键盘处理程序*************************************/
void keyboard(void)//键盘处理程序
{
	unsigned char temp_c;//列值缓存值
	unsigned char temp_r;//行值缓存值
	unsigned char key_value='k';//按键值
	Keyboard_data = 0xF0;//对P0端口初始化，赋初值
	
	if(Keyboard_data == 0xF0)
		return;
	Delay5Ms();//延时去抖动
	if(Keyboard_data == 0xF0)
		return;
	temp_c = Keyboard_data;
	
	WriteCommandLCD(0x01,1); //有按键按下，LCD显示屏清屏
	OK = 1;
	switch(temp_c){
		case 0xE0://第一列
			Keyboard_data = 0x0F;
			temp_r = Keyboard_data;
		  while(Keyboard_data != 0x0F);
			switch(temp_r){
				case 0x0E://第一行
					key_value = 1;
					break;
				case 0x0D://第二行
					key_value = 4;
					break;
				case 0x0B://第三行
					key_value = 7;
					break;
				case 0x07://第四行
					key_value = 'c';
					break;
				default:
					DisplayListChar(0, 5, "Key Row Error!");
					break;
			}
			break;
		case 0xD0://第二列
			Keyboard_data = 0x0F;
			temp_r = Keyboard_data;
		  while(Keyboard_data != 0x0F);
			switch(temp_r){
				case 0x0E://第一行
					key_value = 2;
					break;
				case 0x0D://第二行
					key_value = 5;
					break;
				case 0x0B://第三行
					key_value = 8;
					break;
				case 0x07://第四行
					key_value = 0;
					break;
				default:
					DisplayListChar(0, 5, "Key Row Error!");
					break;
			}
			break;
		case 0xB0://第三列
			Keyboard_data = 0x0F;
			temp_r = Keyboard_data;
		  while(Keyboard_data != 0x0F);
			switch(temp_r){
				case 0x0E://第一行
					key_value = 3;
					break;
				case 0x0D://第二行
					key_value = 6;
					break;
				case 0x0B://第三行
					key_value = 9;
					break;
				case 0x07://第四行
					key_value = 'o';
					break;
				default:
					DisplayListChar(0, 5, "Key Row Error!");
					break;
			}
			break;
		case 0x70://第四列
			Keyboard_data = 0x0F;
			temp_r = Keyboard_data;
		  while(Keyboard_data != 0x0F);
			switch(temp_r){
				case 0x0E://第一行
					key_value = 'h';
					break;
				case 0x0D://第二行
					key_value = 'l';
					break;
				case 0x0B://第三行
					key_value = 'x';
					break;
				case 0x07://第四行
					key_value = 'k';
					//查看温度上、下限的设置值
					DisplayListChar(0, 0, "Upper Tem:");//温度上限
					Display_float(10,0,upper_t);
					Display_c(0);
					DisplayListChar(0, 5, "Lower Tem:");//温度下限
					Display_float(10,1,lower_t);
					Display_c(1);	
					Delay400Ms();// 延时等待
					Delay400Ms();// 延时等待
					Delay400Ms();// 延时等待
					Delay400Ms();// 延时等待
					OK = 0;//退出键盘事件
					H = 0;//温度上限标志清零
					L = 0;//温度下限标志清零
					WriteCommandLCD(0x01,1); //有按键按下，LCD显示屏清屏
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
		case 'k'://默认初始值
			break;
		case 'h'://温度上限标志
			H = 1;//温度上限设置标志位
			L = 0;//温度下限标志清零
			DisplayListChar(0,0, "Input Val:");
			DisplayListChar(0,1,"Upper Tem:");
			DisplayListChar(9,1,"pera...");//显示温度全英文
			break;
		case 'l'://温度下限标志
			H = 0;//温度上限标志清零
			L = 1;//温度下限设置标志位
			DisplayListChar(0,0, "Input Val:");
			DisplayListChar(0,1,"Lower Tem:");
			DisplayListChar(9,1,"pera...");//显示温度全英文
			break;
		case 'c'://取消设置按键
			DisplayListChar(0,0,"Cancel input OK!");
			input_value = 0;
			H = 0;
			L = 0;
			X = 0;
			OK = 0;
			Delay400Ms();
			Delay400Ms();
			WriteCommandLCD(0x01,1); //有按键按下，LCD显示屏清屏
			break;
		case 'x'://小数点按键
			if(input_value != 0){
				X = 1;
				//显示提示文字
				if(H){
					DisplayListChar(0,0, "Input Val:");
					DisplayListChar(0,1,"Upper Tem:");
					DisplayListChar(9,1,"pera...");//显示温度全英文
				}else if(L){
					DisplayListChar(0,0, "Input Val:");
					DisplayListChar(0,1,"Lower Tem:");
					DisplayListChar(9,1,"pera...");//显示温度全英文
				}
				//显示数字
				DisplayOneChar(10,0,input_value/10+0x30);
				DisplayOneChar(11,0,(int)input_value%10+0x30);
				DisplayOneChar(12,0,'.');
			}else{
				DisplayListChar(0,0,"Input Err!");
			}			
			break;
		case 'o'://确认键
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
				WriteCommandLCD(0x01,1); //有按键按下，LCD显示屏清屏
			}else{
				DisplayListChar(0,0,"Input Err!");
			}			
			break;
		default://数值
			if(!(H||L)){
				DisplayListChar(0,0,"Upper OR Lower?");
			}else{
				if(H){
					DisplayListChar(0,0, "Input Val:");
					DisplayListChar(0,1,"Upper Tem:");
					DisplayListChar(9,1,"pera...");//显示温度全英文
				}else if(L){
					DisplayListChar(0,0, "Input Val:");
					DisplayListChar(0,1,"Lower Tem:");
					DisplayListChar(9,1,"pera...");//显示温度全英文
				}
				//---------------数值处理-----------------------
				if(input_value == 0){
					input_value = key_value * 10;//十位数据
					DisplayOneChar(10,0,key_value+0x30);
				}else if(X){
					input_value = input_value + key_value * 0.1;//小数位数据
					Display_float(10,0,input_value);
					Display_c(0);
				}else{
					if(input_value >= 10){
						input_value = input_value + key_value;//个位数据
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
	Keyboard_data = 0xF0;//结束对P1端口初始化，赋初值
}
/************************************键盘处理程序*************************************/

/************************************浮点数显示*************************************/
void Display_float(unsigned char X,unsigned Y,float o_char)//数字转字符串
{
	unsigned int  buffer3,buffer4,buffer5,buffer6;
	
	if(o_char > 100){
		DisplayListChar(X,Y,"Err---");
	}else{
		o_char = o_char * 100;//去除小数
		buffer3 = (int)o_char;
		
		buffer4 = buffer3/1000;//十位
		buffer5 = buffer3 % 1000 / 100;//个位
		buffer6 = buffer3 % 1000 % 100/10;//小数位
		
		DisplayOneChar(X, Y, buffer4+0x30);
		DisplayOneChar(X+1, Y, buffer5+0x30);
		DisplayOneChar(X+2, Y, '.');
		DisplayOneChar(X+3, Y, buffer6+0x30);
	}
	
}
/************************************浮点数显示*************************************/

/************************************DB18S20处理程序*************************************/
void Init_DS18B20(void) 
{
	unsigned char x=0;
	DQ=1; 					//DQ先置高 
	delay(8); 				//稍延时
	DQ=0; 					//发送复位脉冲 
	delay(80); 				//延时（>480us) 
	DQ=1; 					//拉高数据线 
	delay(5); 				//等待（15~60us) 
	x=DQ; 					//用X的值来判断初始化有没有成功，18B20存在的话X=0，否则X=1 
	delay(20); 
}

//读一个字节
unsigned char ReadOneChar(void)  			//主机数据线先从高拉至低电平1us以上，再使数据线升为高电平，从而产生读信号
{
	unsigned char i=0; 		//每个读周期最短的持续时间为60us，各个读周期之间必须有1us以上的高电平恢复期
	unsigned char dat=0; 
	for (i=8;i>0;i--) 		//一个字节有8位 
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

//写一个字节
void WriteOneChar(unsigned char dat) 
{ 
	unsigned char i=0; 		//数据线从高电平拉至低电平，产生写起始信号。15us之内将所需写的位送到数据线上，
	for(i=8;i>0;i--) 		//在15~60us之间对数据线进行采样，如果是高电平就写1，低写0发生。 
	{
		DQ=0; 				//在开始另一个写周期前必须有1us以上的高电平恢复期。 
		DQ=dat&0x01; 
		delay(5); 
		DQ=1; 
		dat>>=1;
	} 
	delay(4);
}

//读温度值（低位放tempL;高位放tempH;）
void ReadTemperature(void)	
{ 
	unsigned int t1;
	unsigned char tempL=0; 		//设全局变量
	unsigned char tempH=0; 
	unsigned int sdata=0;			//测量到的温度的整数部分
	unsigned char xiaoshu1;		//小数第一位
	current_t = 0;//初始化当前温度
	
	Init_DS18B20(); 					//初始化
	WriteOneChar(0xcc); 				//跳过读序列号的操作
	WriteOneChar(0x44); 				//启动温度转换
	Delay400Ms(); 						//转换需要一点时间，延时 
	Delay400Ms();
	Init_DS18B20(); 					//初始化
	WriteOneChar(0xcc); 				//跳过读序列号的操作 
	WriteOneChar(0xbe); 				//读温度寄存器（头两个值分别为温度的低位和高位） 
	tempL=ReadOneChar(); 				//读出温度的低位LSB
	tempH=ReadOneChar(); 				//读出温度的高位MSB	
	//if(tempH>0x7f)      				//最高位为1时温度是负
	//{
		//tempL=~tempL;					//补码转换，取反加一
		//tempH=~tempH+1;       
		//fg=0;      						//读取温度为负时fg=0
	//}else{
		//fg=1;
	//}
	
	//sdata = tempL/16+tempH*16;      	//整数部分
	//xiaoshu1 = (tempL&0x0f)*10/16; 		//小数第一位
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
/************************************DB18S20处理程序*************************************/

/************************************公共延时程序*************************************/
void delay(unsigned char i)//自定义延时参数
{
	for(i;i>0;i--);
}
void Delay5Ms(void)//5ms延时
{
 unsigned int TempCyc = 5552;
 while(TempCyc--);
}
void Delay400Ms(void)//400ms延时
{
 unsigned char TempCycA = 5;
 unsigned int TempCycB;
 while(TempCycA--)
 {
  TempCycB=7269;
  while(TempCycB--);
 }
}
/* 软件延时函数，延时时间(t*10)us */
void DelayX10us(unsigned char t){
    do {
        _nop_();//延时一个机器周期 12M晶振的话 就是1us 只是为了更精确的延时
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
    } while (--t);
}
/************************************公共延时程序*************************************/
