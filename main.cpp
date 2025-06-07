#include <iostream>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cmath>
#include <list>
#include <cstdlib>  // �������������������
#include <ctime>    // ���������ڳ�ʼ�����������
#include <sys/mman.h>
#include <thread>       // ������C++��׼�߳̿�
#include <atomic>       // ������ԭ�Ӳ�����
using namespace std;





class body//������   --������
{
	public:
		int pos_x;//��ǰ����xλ������
		int pos_y;//��ǰ����yλ������
		int height;//�߶�
		int width;//���
		int color;//��ɫ
	
		body()
		{
			this->height=10;
			this->width=10;
		}
	
		body(int pos_x,int pos_y,int color):pos_x(pos_x),pos_y(pos_y)
		{
			this->height=10;
			this->width=10;
		}
		
		~body()
		{
			
		}

};

class food//ʳ����	  --���
{
	public:
		int pos_x;//��ǰ����xλ������
		int pos_y;//��ǰ����yλ������
		int height;//�߶�
		int width;//���
		int color;//��ɫ
	
		food(int color):color(color)
		{
			this->height=10;
			this->width=10;
		}
	
	
		food(int pos_x,int pos_y,int color):pos_x(pos_x),pos_y(pos_y),color(color)
		{
			this->height=10;
			this->width=10;
		}
		
		~food()
		{
			
		}
		
		
		
	// ���������λ�ã��贫����Ļ��ߣ�
    void random_position(int screen_width, int screen_height) 
	{
		pos_x = (rand() % ((screen_width - width) / 20)) * 20;  // Ԥ�����
		pos_y = (rand() % ((screen_height - height) / 20)) * 20; // Ԥ���߶�
    }
	
	
	
};

class snake//����   	--��ï��
{
	public:
		int body_cnt;//�������
		list<body> bodys;//List�������������ߵ�ÿһ������
		int orientation;//������ķ��� 0�� 1�� 2�� 3��
		int speed;
	
	//��ʼֻ��3������
	snake()
	{
		body_cnt = 3;
		speed = 10;
		int centerX = (800 >> 2); // 800 / 4 = 200
		int centerY = (480 >> 2); // 480 / 4 = 120
		
		// ��ʼ�������ң�����3������ڵ㣨�������У�
		for(int i = 0; i < 3; i++) {
			body b;
			b.pos_x = centerX - (i * b.width); // ÿ���ڵ���Ϊ4
			b.pos_y = centerY;
			b.color = 0x00FF0000; // ��ɫ
			bodys.push_back(b);
		}
		
		// ��ʼ��������
		this->orientation = 3;
		
	}
	
	~snake()
	{
		
	}
	
	/*
		�����˶� 
	*/
	void move_up()
	{
		//�жϳ����Ƿ����˶������ͻ
		if(this->orientation == 1) // �����ʱ�����¾ͻ��ͻ
		{
			cout<<"��ǰ�˶������²��ܳ��������˶�"<<endl;
			return;
		}
		
		// �����ߵĳ���
		this->orientation = 0;
		
		// ������ͷ��ǰλ��
		int oldHeadX = bodys.front().pos_x;
		int oldHeadY = bodys.front().pos_y;
		
		// ������ͷλ��
		bodys.front().pos_y -= speed;
		
		// ��������λ��
		int prevX, prevY, tempX, tempY;
		prevX = oldHeadX;
		prevY = oldHeadY;
		
		for (auto it = ++bodys.begin(); it != bodys.end(); ++it) 
		{
			tempX = it->pos_x;
			tempY = it->pos_y;
			it->pos_x = prevX;
			it->pos_y = prevY;
			prevX = tempX;
			prevY = tempY;
		}
	}
	
	
	/*
		�����˶� 
	*/
	void move_down()
	{
		if(this->orientation == 0) // �����ʱ�����Ͼͻ��ͻ
		{
			cout<<"��ǰ�˶������ϲ��ܳ��������˶�"<<endl;
			return;
		}
		
		this->orientation = 1;
		
		int oldHeadX = bodys.front().pos_x;
		int oldHeadY = bodys.front().pos_y;
		
		bodys.front().pos_y += speed;
		
		int prevX, prevY, tempX, tempY;
		prevX = oldHeadX;
		prevY = oldHeadY;
		
		for (auto it = ++bodys.begin(); it != bodys.end(); ++it) {
			tempX = it->pos_x;
			tempY = it->pos_y;
			it->pos_x = prevX;
			it->pos_y = prevY;
			prevX = tempX;
			prevY = tempY;
		}
	}
	
	/*
		�����˶� 
	*/
	void move_left()
	{
		if(this->orientation == 3) // �����ʱ�����Ҿͻ��ͻ
		{
			cout<<"��ǰ�˶������Ҳ��ܳ��������˶�"<<endl;
			return;
		}
		
		this->orientation = 2;
		
		int oldHeadX = bodys.front().pos_x;
		int oldHeadY = bodys.front().pos_y;
		
		bodys.front().pos_x -= speed;
		
		int prevX, prevY, tempX, tempY;
		prevX = oldHeadX;
		prevY = oldHeadY;
		
		for (auto it = ++bodys.begin(); it != bodys.end(); ++it) {
			tempX = it->pos_x;
			tempY = it->pos_y;
			it->pos_x = prevX;
			it->pos_y = prevY;
			prevX = tempX;
			prevY = tempY;
		}
	}
	/*
		�����˶� 
	*/
	void move_right()
	{
		if(this->orientation == 2) // �����ʱ������ͻ��ͻ
		{
			cout<<"��ǰ�˶��������ܳ��������˶�"<<endl;
			return;
		}
		
		this->orientation = 3;
		
		int oldHeadX = bodys.front().pos_x;
		int oldHeadY = bodys.front().pos_y;
		
		bodys.front().pos_x += speed;
		
		int prevX, prevY, tempX, tempY;
		prevX = oldHeadX;
		prevY = oldHeadY;
		
		for (auto it = ++bodys.begin(); it != bodys.end(); ++it) {
			tempX = it->pos_x;
			tempY = it->pos_y;
			it->pos_x = prevX;
			it->pos_y = prevY;
			prevX = tempX;
			prevY = tempY;
		}
	}
	
	
	// �������Զ��ƶ�����������ֶ��ƶ���
    void auto_move() {
        int oldHeadX = bodys.front().pos_x;
        int oldHeadY = bodys.front().pos_y;

        // ���ݵ�ǰ���������ͷλ��
        switch (orientation) {
            case 0: // ��
                bodys.front().pos_y -= speed;
                break;
            case 1: // ��
                bodys.front().pos_y += speed;
                break;
            case 2: // ��
                bodys.front().pos_x -= speed;
                break;
            case 3: // ��
                bodys.front().pos_x += speed;
                break;
            default:
                return; // ��ʼ����Ϊ�ң����账��
        }

        // ��������λ�ã���ԭ���߼�һ�£�
        int prevX = oldHeadX, prevY = oldHeadY;
        for (auto it = ++bodys.begin(); it != bodys.end(); ++it) {
            swap(it->pos_x, prevX);
            swap(it->pos_y, prevY);
        }
    }
	
	
	/*
		�ж��Ƿ�Ե�ʳ��	

		�� ����true
		�� ����false
	*/
	bool check_eat_food(const food& f)
	{
		// ��ͷ������ʳ�������غϣ����������С��
		body head = bodys.front();
		 // ���������С����ͷ������ʳ������ص���
        return (head.pos_x < f.pos_x + f.width &&
                head.pos_x + head.width > f.pos_x &&
                head.pos_y < f.pos_y + f.height &&
                head.pos_y + head.height > f.pos_y);
	}
	
	
	
	
	/*
		�ж��Ƿ���Ϸ����	
		�ж���Ϸ�Ƿ������ײǽ��ײ����
		�� ����true
		�� ����false
	*/
	bool game_over()
	{
		
		/**
		 body head = bodys.front();//��ͷ
		 
		// �߽��⣨��ͷ������Ļ��Χ��
        if (head.pos_x < 0 || 
			head.pos_x + head.width >= 800 || // ���ǿ��
			head.pos_y < 0 || 
			head.pos_y + head.height >= 480) // ���Ǹ߶�
		{
            return true;
        }
		
		 // ����ײ��⣨��ͷ������ڵ��غϣ�
		 
		for (auto it = ++bodys.begin(); it != bodys.end(); ++it) 
		{
            if (head.pos_x == it->pos_x && head.pos_y == it->pos_y) 
			{
                return true;
            }
        }
		 
		return false;
		*/
		
		 body head = bodys.front();
		// ǽ�ڱ߽磺��10����790��800-10������10����470��480-10��
		if (head.pos_x < 10 || 
			head.pos_x + head.width > 790 || // �ұ߽�
			head.pos_y < 10 || 
			head.pos_y + head.height > 470) { // �±߽�
			return true;
		}
		// ����ײ���
		for (auto it = ++bodys.begin(); it != bodys.end(); ++it) {
			if (head.pos_x == it->pos_x && head.pos_y == it->pos_y) {
				return true;
			}
		}
		return false;
		
	}
	
	
	 /* ������������ʳ�����ã� */
    void grow() 
	{
        body last = bodys.back();
        // ֱ����β�����һ����ԭβ��λ����ͬ�Ľڵ�
		bodys.push_back(body(last.pos_x, last.pos_y, last.color));
		body_cnt++;
    }
	
	
	
	
};


class Lcd //��Ļ�� --����΢
{
	public:
		int fd;//�ļ�������                
		int * addr_mapper;//ӳ���׵�ַ              
		int width;//���  
		int height;//�߶�
	
	Lcd(int width,int height):width(width), height(height) 
	{
		this->fd = open("/dev/fb0",O_RDWR);
		
		if(this->fd==-1)
		{
			cout<<"�� "<<"/dev/fb0"<<"ʧ��"<<endl;
		}
		
		this->addr_mapper =(int*)mmap(NULL,height*width*4,PROT_READ|PROT_WRITE,MAP_SHARED,this->fd,0);
		
		if(this->addr_mapper==MAP_FAILED)
		{
			cout<<"ӳ��ʧ��"<<endl;
			
		}
	}
	
	
	
	~Lcd()
	{
		//���ӳ��
		int munmap_result = munmap(this->addr_mapper,height*width*4);
		if(munmap_result==-1)
		{
			cout<<"��ӳ��ʧ��"<<endl;
			
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
			
		}
		else
		{
			cout<<"�ر��ļ�"<<this->fd<<"�ɹ�"<<endl;
			
		}
	}
	
	
	/*
	��ʾ���ص�
	
		x:  x����
		y:  y����
		color : ��ɫ
	*/
	
	void show_pixel(int x,int y,int color)
	{
		//д��ӳ���ļ�
		if(x<this->width && x>=0 && y<this->height && y>=0)
		{
			*(this->addr_mapper+y*width+x) = color;
		}
		
	}
	
	
	/*
	����
	*/
	void clear_display()
	{
		//д��ӳ���ļ�
		for(int i=0;i<this->width;i++)
		{
			for(int j=0;j<this->height;j++)
			{
				*(this->addr_mapper+j*width+i)=0xFF000000;
			}
		}
	}
	
	
	// �������ֲ�������������ߺ�ʳ�����ǽ�ڣ�
    void clear_objects(snake& snake, food& food) {
        // �����ʳ��λ��
        for (int dx = 0; dx < food.width; dx++) {
            for (int dy = 0; dy < food.height; dy++) {
                show_pixel(food.pos_x + dx, food.pos_y + dy, 0xFF000000); // ��ɫ����
            }
        }
        // �����β��λ�ã����ƶ�ʱβ���ᱻ��λ�ø��ǣ�
        if (!snake.bodys.empty()) {
            body tail = snake.bodys.back();
            for (int dx = 0; dx < tail.width; dx++) {
                for (int dy = 0; dy < tail.height; dy++) {
                    show_pixel(tail.pos_x + dx, tail.pos_y + dy, 0xFF000000);
                }
            }
        }
    }

	
	
	
	/*
	��ʾ�ߵ�����
	*/
	void show_snake(snake &snake)
	{
		for (const auto& b : snake.bodys) 
		{
            // ���ƾ��Σ���ʵ�֣�ÿ������ڵ�Ϊ 4x4 ���ؿ飩
            for (int dx = 0; dx < b.width; dx++) 
			{
                for (int dy = 0; dy < b.height; dy++) 
				{
                    show_pixel(b.pos_x + dx, b.pos_y + dy, b.color);
                }
            }
        }
	}
	
	
	/*
	��ʾʳ��
	*/
	void show_food(food &food)
	{
		 // ����ʳ�10x10 ���ؿ飩
        for (int dx = 0; dx < food.width; dx++) 
		{
            for (int dy = 0; dy < food.height; dy++) 
			{
                show_pixel(food.pos_x + dx, food.pos_y + dy, food.color); // ��ɫ
            }
        }
	}
	
	// ����������ǽ�ڣ��߿�
    void draw_wall() {
        const int wall_color = 0xFFFFFFFF; // ��ɫ
        const int wall_width = 10; // ǽ�ڿ��������һ��

        // �ϱ߽�
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < wall_width; y++) {
                show_pixel(x, y, wall_color);
            }
        }
        // �±߽�
        for (int x = 0; x < width; x++) {
            for (int y = height - wall_width; y < height; y++) {
                show_pixel(x, y, wall_color);
            }
        }
        // ��߽�
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < wall_width; x++) {
                show_pixel(x, y, wall_color);
            }
        }
        // �ұ߽�
        for (int y = 0; y < height; y++) {
            for (int x = width - wall_width; x < width; x++) {
                show_pixel(x, y, wall_color);
            }
        }
    }
	
	
};






class touch_display//��������   --����΢
{
	public: 
		
		int ts_fd;	//���������ļ�������
		int b_x;//x�Ŀ�ʼ����
		int b_y;//y�Ŀ�ʼ����
		int s_x;//x��ֹͣ����
		int s_y;//y��ֹͣ����
		
		touch_display() //���캯��
		{
			
		}
		~touch_display() //��������
		{
			
		}
		/*
		//�򿪴�����
		�ɹ�����0
		ʧ�� -1
		*/
		int open_ts() 
		{
			ts_fd = open("/dev/input/event0",O_RDWR);
			if(ts_fd == -1)
			{
				cout << "open ts failed" << endl;
				return -1;
			}
			return 0;
		}
		
		/*
		//��ȡ��������Ϣ,���������λ�ô�����Ա����
		
		*/
		void read_ts() 
		{	
			struct input_event ev;
			int x_flag = 0;  	//x��������������� ��־λ
			int y_flag = 0; 	//y��������������� ��־λ
			
			
			while(1)		//һ�λ�������
			{
				int r = read(ts_fd, &ev, sizeof(ev));
				if(r != sizeof(ev))
				{
					cout << "read failed" << endl;
					continue;
				}
				//������Ϣ
				cout << "type = " << ev.type << "code = " << ev.code << "value = " << ev.value << endl;
				if(ev.type == EV_KEY && ev.code == BTN_TOUCH) //�����������¼�-�����
				{
					if(ev.value == 1) //����ȥ��
					{
						cout << "down" << endl;
					}
					else //ev.value == 0 ��������
					{
						cout << "up" << endl;
						break;
					}
				}
				if(ev.type == EV_ABS && ev.code == ABS_X)  //������x������¼�
				{	
					if(x_flag == 0) //x���������
					{
						x_flag = 1;
						b_x = ev.value;
					}
					s_x = ev.value;		
				}
				if(ev.type == EV_ABS && ev.code == ABS_Y)  //������y������¼�
				{
					if(y_flag == 0) //y���������
					{
						y_flag = 1;
						b_y = ev.value;
					}
					s_y = ev.value;
				}
			}
				
			if(abs(s_x - b_x) - abs(s_y - b_y) > 0 && abs(s_x - b_x) > 20) //x����Ļ���
			{
				if(s_x  - b_x > 0)  //�һ�
				{
					cout << "right slide" << endl;
				}
				else //s_x - b_x < 0		//��
				{
					cout << "left slide" << endl;
				}
			}
			if(abs(s_x - b_x) - abs(s_y - b_y) <= 0 && abs(s_y - b_y) > 20)	//y����Ļ���
			{
				if(s_y - b_y > 0)  //�»�
				{
					cout << "down slide" << endl;
				}
				else		//�ϻ� 
				{
					cout << "up slide" << endl;
				}
			}
			
			
		}
		
		
		
	/* 
	��ȡ���������벢�����ߵķ��򣨷��ط���ֵ��0-�ϣ�1-�£�2-��3-�ң� 
	*/
    int get_direction() 
	{
        read_ts(); // ����ԭ�ж�ȡ�߼�
        int dx = s_x - b_x;
        int dy = s_y - b_y;

        if (abs(dx) > abs(dy) && abs(dx) > 20) { // x���򻬶�
            return (dx > 0) ? 3 : 2; // �һ�=3����=2
        } else if (abs(dy) > 20) { // y���򻬶�
            return (dy > 0) ? 1 : 0; // �»�=1���ϻ�=0
        }
        return -1; // ��Ч����
    }
	
	
};

int main() // ������   --����΢
{
	
	// ��ʼ�����������
    srand(time(0));
	
	// ��ʼ����Ļ���ߺ�ʳ��
    Lcd lcd(800, 480);
    snake my_snake;
    food my_food(0xFF00FF00);  // ��ʼʳ����ɫ
	my_food.random_position(800, 480);
	
	//���һ��֮ǰ��
	lcd.clear_display();
	
	// **����**��������Ϸ��ʼʱ����һ��ǽ��
    lcd.draw_wall();
	
	
	// ��ʼ��������
    touch_display ts;
    if (ts.open_ts() != 0) {
        cout << "��������ʼ��ʧ�ܣ�" << endl;
        return -1;
    }
	
	
	// ԭ�ӱ�־λ�������߳�����
    std::atomic<bool> threadRunning(true);
    // �洢���·����ԭ�ӱ���
    std::atomic<int> currentDirection(-1);// -1��ʾ�����룬ʹ���ߵ�orientation��Ϊ��ǰ����

    // �����̣߳������·��򣬲�ֱ���ƶ���
    std::thread inputThread([&]() {
        while (threadRunning) {
            int dir = ts.get_direction();
            if (dir != -1) {
                // ��鷽���Ƿ�Ϸ��������뵱ǰ�����෴��
                if ((dir == 0 && my_snake.orientation != 1) ||
                    (dir == 1 && my_snake.orientation != 0) ||
                    (dir == 2 && my_snake.orientation != 3) ||
                    (dir == 3 && my_snake.orientation != 2)) {
                    my_snake.orientation = dir; // ֱ�Ӹ����ߵĳ���
                }
            }
        }
    });
	
	
	
	// ��Ϸ��ѭ��
    bool game_running = true;
	
	
	 while (game_running) 
	{
        // 1. ����
        //lcd.clear_display();
		
		//�ֲ�����
        lcd.clear_objects(my_snake, my_food);
		
        // �Զ��ƶ��ߣ������߼���
        my_snake.auto_move();


        // 3. ����ʳ��
        if (my_snake.check_eat_food(my_food)) {
            my_snake.grow();
            my_food.random_position(800, 480);  // ������ʳ��λ��
        }

        // 4. �����Ϸ����
        if (my_snake.game_over()) 
		{
            cout << "��Ϸ�������÷֣�" << my_snake.body_cnt - 3 << endl;
            game_running = false;
        }

        // 5. ��Ⱦ����
        lcd.show_snake(my_snake);
        lcd.show_food(my_food);

        // 6. ����֡��
        //usleep(100000);  // 100ms��Լ10FPS
		//usleep(5000);
		 // �����Զ��ƶ��ٶȣ�100ms�ƶ�һ�Σ�
        //usleep(16666);
		usleep(100000);
    }
	
	
	// ���Ž����߳�
    threadRunning = false;
    if (inputThread.joinable()) {
        inputThread.join(); // �ȴ��߳̽���
    }

	
	
	
	return 0;
}


//arm-linux-g++ -std=c++11 main.cpp -o main -lpthread
