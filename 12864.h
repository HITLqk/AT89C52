#ifndef _12864_H__
#define _12864_H__	
#define uchar unsigned char
#define uint unsigned int	   //����
sbit E=P2^0;
sbit RW=P2^1;
sbit RS=P2^2;
sbit CS2=P2^3;
sbit CS1=P2^4;			 //�˿ڶ���
/*1ms��ʱ����*/
void delay1ms(uint k)
{
  k=k*125;
  while(k--);
}
 /*12864��æ */
void chekbusy12864(void)
 {
  uchar dat;
  EX0=0;
  RS=0;       //ָ��ģʽ 
  RW=1;       //������
 do{
   P0=0x00;			 //
   E=1;
   dat=P0&0x80;
   E=0;
  }while(dat!=0x00);
  EX0=1;
 }
   /****ѡ��****/  
void choose12864(uchar i)//i��Ҫд����.0������,1������,2��˫����
{						 //�˴���Ӳ��������ʱi�ĵ�ƽȫ��������෴��
  switch (i)			 //�˰汾Ϊ����棻
  {
    case 0: CS1=0;CS2=1;break;	//����˴����Ҫ�ڵ�·��������Ӧ�ø�ΪCS=1��CS2=0��   
    case 1: CS1=1;CS2=0;break;
	case 2: CS1=0;CS2=0;break;
	default: break;
  }
}  
/******д����*****/
void cmd_w12864(uchar cmd)		//д����
{
  chekbusy12864();
  EX0=0;
  RS=0;	            //ָ��ģʽ
  RW=0;             //дģʽ
  E=1;
  P0=cmd;
  E=0;
  EX0=1;
}
/****д����******/
void  dat_w12864(uchar dat)
{
  chekbusy12864();
  EX0=0;
  RS=1;
  RW=0;
  E=1;
  P0=dat;
  E=0;
  EX0=1;
}
/*****����*****/
void clear12864(void)
{
  uchar page,row;
  for(page=0xb8;page<0xc0;page++)
  {
    cmd_w12864(page);
	cmd_w12864(0x40);
	for(row=0;row<64;row++)
	 {
	   dat_w12864(0x00); 	 //��12864���е�ַȫ��д��
	 }
  }
}
/***��ʼ��****/
void init12864(void)
{
  chekbusy12864();
  cmd_w12864(0xc0);	  //�ӵ����п�ʼ��ʾ
  cmd_w12864(0x3f);	  //LCD��ʾRAM�е�����
}
/**8X16�ַ�����ʾ**/
void play8(uchar ch,uchar row,uchar page,uchar *adr)
{
  uchar i;
  choose12864(ch);
  page=page<<1;			   //�����в��õ�λ���������˷�����,�������Դ�󽵵ʹ������ĸ���
  row=row<<3;
  cmd_w12864(row+0x40);
  cmd_w12864(page+0xb8);
  for(i=0;i<8;i++)
  {
    dat_w12864(*(adr+i));
  }
  cmd_w12864(row+0x40);
  cmd_w12864(page+0xb9);
  for(i=8;i<16;i++)
  {
    dat_w12864(*(adr+i));
  }
}
/**16X16��ʾ**/
void play16(uchar ch,uchar row,uchar page,uchar *adr)
{
  uchar i;
  choose12864(ch);
  page=page<<1;					   
  row=row<<3;
  cmd_w12864(row+0x40);
  cmd_w12864(page+0xb8);
  for(i=0;i<16;i++)
  {
    dat_w12864(*(adr+i));
  }
  cmd_w12864(row+0x40);
  cmd_w12864(page+0xb9);
  for(i=16;i<32;i++)
  {
    dat_w12864(*(adr+i));
  }
} 
uchar dat_r12864(uchar page,uchar arrange)  //pageҳ��ַ.arrange�е�ַ)
{
  uchar dat;
  chekbusy12864();
  EA=0;
  cmd_w12864(page+0xb8);
  cmd_w12864(arrange+0x40);
  EX0=0;						
  P0=0xff;
  RW=1;
  RS=1;
  E=1;
  E=0;				 //12864������ʱ�ڶ��ζ�����Ч����һ�ζ�ȡ��ֵ���ɼ�
  E=1;
  dat=P0;
  E=0;
  EA=1;
  return(dat);
  //EX0=1;
  
  }
  //������ʾ
 void play16_fb(uchar ch,uchar arrange,uchar page)
 {
   uchar i;
   uchar xdata dat_fb[32];
   choose12864(ch);
   for(i=0;i<16;i++)
   {
     dat_fb[i]=~(dat_r12864((page<<1),((arrange<<3)+i)));
	 dat_fb[i+16]=~(dat_r12864((page<<1)+1,((arrange<<3)+i)));
	}
   play16(ch,arrange,page,dat_fb);
 }
  /**����**/
void vertical(uchar y1,uchar y2,uchar x)//y1��ʾ���,y2��ʾ�յ�
{
  uchar i,sum=0;
  if(x>63)
   {
     choose12864(1);					 //y1��y2С��������������ߵĺ��������û���ķ���
     x=x-64;							 //��Ϊ�˼��ٵ�Ƭ���Ĵ�������
   }
   else
   {
     choose12864(0);
   }
  if((y1/8)!=(y2/8))
  {
   for(i=0;i<(8-y1%8);i++)	 
   {
      sum=sum|((2<<((y1%8)+i)));
   }
  cmd_w12864(x+0x40);
  cmd_w12864(y1/8+0xb8);
  dat_w12864(sum);
  sum=0;
   for(i=0;i<(y2/8-y1/8-1);i++)
    {
     cmd_w12864(x+0x40);
	 cmd_w12864((y1/8)+0xb9+i);
	 dat_w12864(0xff);
    }
  for(i=0;i<=(y2%8);i++)
   {
     sum=sum|(2<<i);
   }
   cmd_w12864(x+0x40);
   cmd_w12864(y2/8+0xb8);
   dat_w12864(sum|1);
   sum=0;	
 }
 else
 {
   for(i=0;i<=y2-y1;i++)
    {
	  sum=sum|(2<<(i+(y1%8)));
	}
	cmd_w12864(0x40|x);
	cmd_w12864(0xb8|(y1/8));
	dat_w12864(sum);
 } 
}
 /**�����ʾ**/
void dot(uchar x,uchar y)
{
  uchar dat;
  if(x>63)
  {
    choose12864(1);
	x=x-64;
  }
  else
  {
    choose12864(0);
  }
  dat=dat_r12864(y/8,x);
  cmd_w12864(0x40|x);
  cmd_w12864(0xb8|y/8);
  dat_w12864((1<<(y%8))|dat);
}
/**�¶ȶ�ͼ�ε�ת��**/
void play(void)
{
  static uchar n=63;
  uchar i,m,sum=0;
  m=temp[0]*10+temp[1]+(temp[2]+5)/10;
  if(m<60)
  {
   m=62-m;
   if(m<n)
   {
    vertical(m,63,124);
    }
   if(m>n)
   {
    if((m/8)>(n/8))
	{
	  cmd_w12864(n/8+0xb8);
	  cmd_w12864(0x40+60);
	  dat_w12864(0x00);
   	}
	for(i=0;i<(8-m%8);i++)
	   {
	   	 sum=sum+(2<<((m%8)+i));
	   }
	 cmd_w12864(m/8+0xb8);
	 cmd_w12864(0x40+60);
	 dat_w12864(sum);
	 n=m;
  }
  dot(60,3);
 }		   
}					 
clear8(uchar x,uchar y,uchar ch)
{
  uchar i;
  choose12864(ch);
  cmd_w12864(x*8+0x40);
  cmd_w12864(y+0xb8);
  for(i=0;i<8;i++)
   {
     dat_w12864(0x00);
   }
   cmd_w12864(x*8+0x40);
   cmd_w12864(y+0xb9);
  for(i=0;i<16;i++)
   {
      dat_w12864(0x00);
   }
}
/**4X4�����Ϊһ��**/
void change(uchar *x,uchar *y)     //x<24  y<15
{
  uchar i,m,n;					   //24��15�ֱ��ʾ��Ϸ�ķ�Χ
  if(*x<24&&*y<15)				   //����ĵ��Ǹ��л�ɫ�ĵ������һ��ʵ�ĵĵ�
  {
   m=(*x)<<2;
   n=((*y)<<2)+2;
   for(i=0;i<4;i++)
   {
    dot(31+m,n+i);
	dot(34+m,n+i);
   }
   dot(32+m,n);		 
   dot(32+m,n+1);
   dot(32+m,n+3);
   dot(33+m,n);
   dot(33+m,n+2);
   dot(33+m,n+3);
  }
}
/**4X4�����Ϊһ��**/
void change1(uchar x,uchar y)     //x<24  y<15
{
  
  uchar i,m,n;	
  EA=0;				                //24��15�ֱ��ʾ��Ϸ�ķ�Χ
  if(x<24&&y<15)				   //����ĵ��Ǹ��л�ɫ�ĵ������һ��ʵ�ĵĵ�
  {
   m=(x)<<2;
   n=(y<<2)+2;
   for(i=0;i<4;i++)
   {
    dot(31+m,n+i);
	dot(34+m,n+i);
   }
   dot(32+m,n);
   dot(32+m,n+1);
   dot(32+m,n+3);
   dot(33+m,n);
   dot(33+m,n+2);
   dot(33+m,n+3);
  }
  EA=1;

}


void cleardot(uchar x,uchar y)
{
  uchar dat,j;
  if(x>63)
  {
    choose12864(1);
	x=x-64;
  }
  else
  {
    choose12864(0);
  }
  dat=dat_r12864(y/8,x);
  cmd_w12864(0x40|x);
  cmd_w12864(0xb8|(y/8));
  j=~(1<<(y%8));
  dat_w12864(dat&j);
}
/*���һ��4X4�ĵ�*/
void clear(uchar *x,uchar *y)
{
  uchar i,m,n;
  m=((*x)<<2)+31;
  n=((*y)<<2)+2;
  for(i=0;i<4;i++)
  {
    cleardot(m,n+i);
	cleardot(m+1,n+i);
	cleardot(m+2,n+i);
	cleardot(m+3,n+i);
  }
}  /**���һ����**/
/*���һ��4X4�ĵ�*/
void clear1(uchar x,uchar y)
{
  uchar i,m,n;
  if(x>=0&&x<24&&y>=0&&y<15)
  {
   m=((x)<<2)+31;
   n=((y)<<2)+2;
   for(i=0;i<4;i++)
   {
    cleardot(m,n+i);
	cleardot(m+1,n+i);
	cleardot(m+2,n+i);
	cleardot(m+3,n+i);
   }
  }
}  /**���һ����**/
bit readfk(char x,char y)
{
  bit flagfk=0;
  uchar m,n;
  if(y>=0)
  {
  m=(x<<2)+31;
  n=(y<<2)+2;
  choose12864(0);
  if(m>63)
  {
   m=m-64;
   choose12864(1);
  }																				
  m=dat_r12864(n>>3,m);
  if((n%8)==2)
  {
    if((m&0x3c)==0x3c)
	{
	  flagfk=1;
	 }
	else
	{
	  flagfk=0;
	}
  }
   if((n%8)==6)
   {
    if((m&0xc0)==0xc0)
	{flagfk=1;

	}
	else
	{flagfk=0;}
   }
  }
  return(flagfk);

}


























