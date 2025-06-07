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
	��ʾ������������   �ɹ��򷵻�1  ʧ��Ϊ-1
	*/
	int show_color(const void * FrameBuffer ,int size)
	{
		//1����֡�����豸�ļ�
		
		
		int open_result = open(this->pathname,O_RDWR);
		
		if(open_result==-1)
		{
			cout<<"�� "<<pathname<<"ʧ��"<<endl;
			return -1;
		}
		
		//2������һ��800 * 480 int�Ļ����� int color[480][800],��800*480�����ɫ����ɫֵ
		//3��������������ɫֵд�뵽�򿪵�֡�����豸�ļ���ȥ
		int write_result = write(open_result,FrameBuffer,size);
		
		if(write_result!=length*width*4)
		{
			cout<<"д�뻺��ʱ����"<<endl;
			return -1;
		}
		else
		{
			cout<<"�ɹ�д��"<<write_result<<"�ֽ��ļ�"<<endl;
		}
		
		//4���ر�֡�����ļ�
		
		int close_result = close(open_result);
		
		if(close_result ==-1)
		{
			cout<<"�ر��ļ�"<<open_result<<"ʧ��"<<endl;
			return -1;
		}
		else
		{
			cout<<"�ر��ļ�"<<open_result<<"�ɹ�"<<endl;
			return 1;
		}
		
		
	}
	
	
};



int main()
{
	
	display d(480,800);

	
	// ����480x800��RGB��������ÿ������4�ֽ� (RGBA)
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
		cout<<"��ʾ�ɹ�"<<endl;
	}
	else
	{
		cout<<"��ʾʧ��"<<endl;
	}
	
	return 0;
}