#include <iostream>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cmath>
using namespace std;

class touch_display
{
	public:
		int fd;
		char pathname[512]="/dev/input/event0";
		input_event input_data;
		
		
		
		
	void init()
	{
		this->fd = open(this->pathname,O_RDWR);
		if(this->fd==-1)
		{
			cout<<"打开 "<<this->fd<<"失败"<<endl;
		}
	}
	
	
	void get_info()
	{
		int x_flag=0;//x的起点坐标标志
		int y_flag=0;//y的起点坐标标志
		int b_x,b_y,s_x,s_y;//起点终点坐标
		
		while(1)//多次动作
		{
			x_flag=0;
			y_flag=0;
			while(1)//一次滑动动作
			{
				int read_result = read(this->fd,&input_data,sizeof(input_data));
				
				if(read_result!=sizeof(input_data))
				{
					cout<<"读取失败"<<endl;
					continue;
				}
				//分析信息
				cout<<"type: "<<input_data.type
				<<" code:"<<input_data.code
				<<" value:"<<input_data.value<<endl;
				
				if(input_data.type==EV_KEY && input_data.code==BTN_TOUCH)//触摸屏点击事件
				{
					if(input_data.value==1)//按下去了
					{
						cout<<"KEY DOWN"<<endl;
					}
					else//input_data.value==1//弹起来了
					{
						cout<<"KEY UP"<<endl;
						break;
					}
				}
				
				if(input_data.type==EV_ABS && input_data.code==ABS_X)//触摸屏x方向事件
				{
					if(x_flag==0)
					{
						b_x=input_data.value;
						x_flag=1;
					}
					s_x=input_data.value;
				}
				
				if(input_data.type==EV_ABS && input_data.code==ABS_Y)//触摸屏y方向事件
				{
					if(y_flag==0)
					{
						b_y=input_data.value;
						y_flag=1;
					}
					s_y=input_data.value;
				}
				
				
			}
			
			if( abs(s_x-b_x) - abs(s_y-b_y) > 0  && abs(s_x-b_x) >20)  //x方向的滑动
			{
				if(s_x -b_x >0)//右滑
				{
					cout<<"Right Slide "<<endl;
				}
				else//s_x -b_x <0  左滑
				{
					cout<<"Left Slide "<<endl;
				}
			}
			if(abs(s_x-b_x) - abs(s_y-b_y) <= 0  && abs(s_y-b_y) >20)//y方向的滑动
			{
				if(s_y -b_y >0)//下滑
				{
					cout<<"Down Slide "<<endl;
				}
				else//s_y -b_y <0  上滑
				{
					cout<<"Up Slide "<<endl;
				}
			}		
		}
	}
	
	void close_touch_display()
	{
		int close_result = close(this->fd);
		if(close_result==-1)
		{
			cout<<"close "<<this->fd<<"failed"<<endl;
		}
	}
	
};


int main()
{
	touch_display t;
	
	t.init();
	
	t.get_info();
	
	
	
	return  0;
}