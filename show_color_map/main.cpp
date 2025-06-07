#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cstring>
using namespace std;

class display
{

	public:
		int length;
		int width;
		char pathname[512] = "/dev/fb0";
		int * map_addr;
		int fd;
		
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
	int init_display()
	{
		//1）打开帧缓冲设备文件
		
		
		this->fd = open(this->pathname,O_RDWR);
		
		if(this->fd==-1)
		{
			cout<<"打开 "<<pathname<<"失败"<<endl;
			return -1;
		}
		
		this->map_addr =(int*)mmap(NULL,length*width*4,PROT_READ|PROT_WRITE,MAP_SHARED,this->fd,0);
		if(this->map_addr==MAP_FAILED)
		{
			cout<<"映射失败"<<endl;
			return -1;
		}
		else
		{
			cout<<"映射成功"<<endl;
		}
		
		return 1;
	}
	

	void draw_display(int x,int y,int color)
	{
		//写入映射文件
		if(x<this->length && x>=0 && y<this->width && y>=0)
		{
			*(this->map_addr+y*length+x) = color;
		}
	}
	
	
	
	int munmap_display()
	{
		//解除映射
		int munmap_result = munmap(this->map_addr,length*width*4);
		if(munmap_result==-1)
		{
			cout<<"解映射失败"<<endl;
			return -1;
		}
		else
		{
			cout<<"解映射成功"<<endl;
		}
		
		//关闭帧缓冲文件
		
		int close_result = close(this->fd);
		
		if(close_result ==-1)
		{
			cout<<"关闭文件"<<this->fd<<"失败"<<endl;
			return -1;
		}
		else
		{
			cout<<"关闭文件"<<this->fd<<"成功"<<endl;
			return 1;
		}
	}
	
};



int main()
{
	
	display d(800,480);

	
	d.init_display();
	

	for(int i=0;i<800;i++)
	{
		for(int j=0;j<480;j++)
		{
			d.draw_display(i,j,0x00FF0000);
		}
	}
	
	d.munmap_display();
	

	
	return 0;
}