#ifndef _DS1302_H__			 //��ֹ����
#define _DS1302_H__
#define ds1302_second 0x80
#define ds1302_minute 0x82
#define ds1302_hour 0x84
#define ds1302_day 0x86
#define ds1302_week 0x8a
#define ds1302_month 0x88
#define ds1302_year 0x8c
#define uchar unsigned char
#define uint unsigned int	   //����
/**�˿ڶ���**/
sbit  IO=P3^7;
sbit CLK=P3^6;
sbit RST=P3^5;
sbit ACC0=ACC^0;
sbit ACC7=ACC^7;
char datastring[13];   ////���ʱ�������
typedef struct
{
 	uchar second;
	uchar minute;
	uchar hour;
	uchar day;
	uchar week;
	uchar month;
	uchar year;
}systemtime;//�����ʱ�������
void ds1302_write(uchar dat)
{
  uchar i,j;
  j=dat;
  for(i=0;i<8;i++)		//������һ�����������˼��������,�����j����ΪACC����Ϊ
   {					//for(i=0;i<8;i++)����ͻ᲻�ɹ�,����ָ����!!
     IO=j&0x01;			
	 CLK=1;
	 CLK=0;
	 j>>=1;
   }
}
/**ʵʱʱ�Ӷ���һ���ֽ�**/
uchar ds1302_read()
{
  uchar i;
  for(i=0;i<8;i++)
    {
	  ACC=ACC>>1;	
	  ACC7=IO;
	  CLK=1;
	  CLK=0;
	}
  return(ACC);
}
/**д������**/
void dat_write(uchar addr,uchar dat)   //addrΪҪд���ݵĵ�ַ,datΪҪд������
{
  RST=0;
  CLK=0;
  RST=1;
  ds1302_write(addr);
  ds1302_write(dat);
  CLK=1;
  RST=0;
}	
/**��������**/
uchar dat_read(uchar addr)
{
  uchar dat;
  RST=0;
  CLK=0;
  RST=1;
  ds1302_write(addr|0x01);
  dat=ds1302_read();
  CLK=1;
  RST=0;
  return(dat);
}
/**�Ƿ�д����**/
void ds1302_protect(bit flagg)
{
  if(flagg)
    dat_write(0x8e,0x10);			 //��ֹд
  else
    dat_write(0x8e,0x00);			 //����д
}
/**ʱ������**/
void settime(uchar addr,uchar value)
{
  ds1302_protect(0);
  dat_write(addr,value);
  ds1302_protect(1);
}
/**ʱ����������**/
void setjia(uchar adr)
{
  uchar dat;
  dat=dat_read(adr);
  if((dat&0x0f)==9)
	{
	 dat_write(adr,((dat&0xf0)+0x10));
	}
	else
	{
     dat_write(adr,++dat);
	}
}
void setjian(uchar adr)
{
    uchar dat;
    dat=dat_read(adr);
    if((dat&0x0f)==0)
	{
	  dat_write(adr,(((dat&0xf0)-0x10)|0x09));
	}
	else
	{
      dat_write(adr,--dat);
    }
 
}
/**12/24Сʱʱ������**/
void day_set(bit flagg)				 //flagΪ1Ϊ12ʱ��,Ϊ0��Ϊ24Сʱ��
{ 
  uchar hour;
  hour=(dat_read(0x85)&0x7f);		 //����Сʱ�Ĵ�����ԭ�е�ʱ��ֵ
  ds1302_protect(0)	;
  if(flagg)
    {
	  dat_write(0x84,0x80|hour);
	}
  else
    {
	  dat_write(0x84,0x00|hour);
	}
  ds1302_protect(1);
} 
/**ȡʱ��**/
void gettime(systemtime *time) 
{
  uchar readvalue;
  readvalue=dat_read(ds1302_second);
  time->second=((readvalue&0x70)>>4)*10+(readvalue&0x0f);
  readvalue=dat_read(ds1302_minute);
  time->minute=((readvalue&0x70)>>4)*10+(readvalue&0x0f);
  readvalue=dat_read(ds1302_hour);
  time->hour=((readvalue&0x30)>>4)*10+(readvalue&0x0f);
  readvalue=dat_read(ds1302_day);
  time->day=((readvalue&0x30)>>4)*10+(readvalue&0x0f);
  readvalue=dat_read(ds1302_week);
  time->week=(readvalue&0x07);
  readvalue=dat_read(ds1302_month);
  time->month=((readvalue&0x10)>>4)*10+(readvalue&0x0f);
  readvalue=dat_read(ds1302_year);
  time->year=((readvalue&0xf0)>>4)*10+(readvalue&0x0f);
  datastring[0]=time->year/10;
  datastring[1]=time->year%10;
  datastring[2]=time->month/10;
  datastring[3]=time->month%10;
  datastring[4]=time->day/10;
  datastring[5]=time->day%10;
  datastring[6]=time->hour/10;
  datastring[7]=time->hour%10;
  datastring[8]=time->minute/10;
  datastring[9]=time->minute%10;
  datastring[10]=time->second/10;
  datastring[11]=time->second%10;
  datastring[12]=time->week;
  }
   /**��ʼʱ������Ϊ2010-07-19 08ʱ08��08��**/
void init_time(void)
{
  day_set(0);
  settime(ds1302_second,0x08);
  settime(ds1302_minute,0x08);
  settime(ds1302_hour,0x08);
  settime(ds1302_day,0x19);
  settime(ds1302_month,0x08);
  settime(ds1302_year,0x10);
  settime(ds1302_week,0x01);
  }
   /**DS1302��ʼ��**/
void init_ds1302(void)
{
  uchar second=dat_read(ds1302_second);
  if(second&0x80)
    settime(ds1302_second,0);
}

#endif