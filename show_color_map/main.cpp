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
	��ʾ������������   �ɹ��򷵻�1  ʧ��Ϊ-1
	*/
	int init_display()
	{
		//1����֡�����豸�ļ�
		
		
		this->fd = open(this->pathname,O_RDWR);
		
		if(this->fd==-1)
		{
			cout<<"�� "<<pathname<<"ʧ��"<<endl;
			return -1;
		}
		
		this->map_addr =(int*)mmap(NULL,length*width*4,PROT_READ|PROT_WRITE,MAP_SHARED,this->fd,0);
		if(this->map_addr==MAP_FAILED)
		{
			cout<<"ӳ��ʧ��"<<endl;
			return -1;
		}
		else
		{
			cout<<"ӳ��ɹ�"<<endl;
		}
		
		return 1;
	}
	

	void draw_display(int x,int y,int color)
	{
		//д��ӳ���ļ�
		if(x<this->length && x>=0 && y<this->width && y>=0)
		{
			*(this->map_addr+y*length+x) = color;
		}
	}
	
	
	
	int munmap_display()
	{
		//���ӳ��
		int munmap_result = munmap(this->map_addr,length*width*4);
		if(munmap_result==-1)
		{
			cout<<"��ӳ��ʧ��"<<endl;
			return -1;
		}
		else
		{
			cout<<"��ӳ��ɹ�"<<endl;
		}
		
		//�ر�֡�����ļ�
		
		int close_result = close(this->fd);
		
		if(close_result ==-1)
		{
			cout<<"�ر��ļ�"<<this->fd<<"ʧ��"<<endl;
			return -1;
		}
		else
		{
			cout<<"�ر��ļ�"<<this->fd<<"�ɹ�"<<endl;
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