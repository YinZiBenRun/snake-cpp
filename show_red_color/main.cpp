#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
using namespace std;

class display
{

	public:
		int length;
		int width;
		char pathname[512] = "/dev/fb0";
		
	display(int length,int width):length(length), width(width) {}
	
	display(int length,int width,const char * dist):length(length), width(width) 
	{
		strcpy(this->pathname,dist);
	}
	
	~display()
	{
		
	}
	
	
	/*
	显示缓冲区的像素   成功则返回1  失败为-1
	*/
	int show_color(const void * FrameBuffer ,int size)
	{
		//1）打开帧缓冲设备文件
		
		
		int open_result = open(this->pathname,O_RDWR);
		
		if(open_result==-1)
		{
			cout<<"打开 "<<pathname<<"失败"<<endl;
			return -1;
		}
		
		//2）定义一个800 * 480 int的缓冲区 int color[480][800],存800*480个大红色的颜色值
		//3）将缓冲区的颜色值写入到打开的帧缓冲设备文件中去
		int write_result = write(open_result,FrameBuffer,size);
		
		if(write_result!=length*width*4)
		{
			cout<<"写入缓冲时报错"<<endl;
			return -1;
		}
		else
		{
			cout<<"成功写入"<<write_result<<"字节文件"<<endl;
		}
		
		//4）关闭帧缓冲文件
		
		int close_result = close(open_result);
		
		if(close_result ==-1)
		{
			cout<<"关闭文件"<<open_result<<"失败"<<endl;
			return -1;
		}
		else
		{
			cout<<"关闭文件"<<open_result<<"成功"<<endl;
			return 1;
		}
		
		
	}
	
	
};



int main()
{
	
	display d(480,800);

	
	// 创建480x800的RGB缓冲区，每个像素4字节 (RGBA)
    int color[480][800];

	
	for(int i=0;i<480;i++)
	{
		for(int j=0;j<800;j++)
		{
			
			color[i][j]=0x00FF0000;

		}
		
	}
	
	
	int ret = d.show_color(color,480*800*4);
	
	if(ret==1)
	{
		cout<<"显示成功"<<endl;
	}
	else
	{
		cout<<"显示失败"<<endl;
	}
	
	return 0;
}