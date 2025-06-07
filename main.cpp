#include <iostream>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cmath>
#include <list>
#include <cstdlib>  // 新增：用于随机数生成
#include <ctime>    // 新增：用于初始化随机数种子
#include <sys/mman.h>
#include <thread>       // 新增：C++标准线程库
#include <atomic>       // 新增：原子操作库
using namespace std;





class body//身体类   --王卫稳
{
	public:
		int pos_x;//当前身体x位置坐标
		int pos_y;//当前身体y位置坐标
		int height;//高度
		int width;//宽度
		int color;//颜色
	
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

class food//食物类	  --张锟
{
	public:
		int pos_x;//当前身体x位置坐标
		int pos_y;//当前身体y位置坐标
		int height;//高度
		int width;//宽度
		int color;//颜色
	
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
		
		
		
	// 随机生成新位置（需传入屏幕宽高）
    void random_position(int screen_width, int screen_height) 
	{
		pos_x = (rand() % ((screen_width - width) / 20)) * 20;  // 预留宽度
		pos_y = (rand() % ((screen_height - height) / 20)) * 20; // 预留高度
    }
	
	
	
};

class snake//蛇类   	--陈茂恒
{
	public:
		int body_cnt;//身体节数
		list<body> bodys;//List容器用来保存蛇的每一节身体
		int orientation;//蛇面向的方向 0上 1下 2左 3右
		int speed;
	
	//初始只有3个身体
	snake()
	{
		body_cnt = 3;
		speed = 10;
		int centerX = (800 >> 2); // 800 / 4 = 200
		int centerY = (480 >> 2); // 480 / 4 = 120
		
		// 初始方向向右，创建3个身体节点（横向排列）
		for(int i = 0; i < 3; i++) {
			body b;
			b.pos_x = centerX - (i * b.width); // 每个节点宽度为4
			b.pos_y = centerY;
			b.color = 0x00FF0000; // 红色
			bodys.push_back(b);
		}
		
		// 初始方向向右
		this->orientation = 3;
		
	}
	
	~snake()
	{
		
	}
	
	/*
		向上运动 
	*/
	void move_up()
	{
		//判断朝向是否与运动方向冲突
		if(this->orientation == 1) // 如果此时方向朝下就会冲突
		{
			cout<<"当前运动方向朝下不能朝反方向运动"<<endl;
			return;
		}
		
		// 更新蛇的朝向
		this->orientation = 0;
		
		// 保存蛇头当前位置
		int oldHeadX = bodys.front().pos_x;
		int oldHeadY = bodys.front().pos_y;
		
		// 更新蛇头位置
		bodys.front().pos_y -= speed;
		
		// 更新蛇身位置
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
		向下运动 
	*/
	void move_down()
	{
		if(this->orientation == 0) // 如果此时方向朝上就会冲突
		{
			cout<<"当前运动方向朝上不能朝反方向运动"<<endl;
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
		向左运动 
	*/
	void move_left()
	{
		if(this->orientation == 3) // 如果此时方向朝右就会冲突
		{
			cout<<"当前运动方向朝右不能朝反方向运动"<<endl;
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
		向右运动 
	*/
	void move_right()
	{
		if(this->orientation == 2) // 如果此时方向朝左就会冲突
		{
			cout<<"当前运动方向朝左不能朝反方向运动"<<endl;
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
	
	
	// 新增：自动移动方法（替代手动移动）
    void auto_move() {
        int oldHeadX = bodys.front().pos_x;
        int oldHeadY = bodys.front().pos_y;

        // 根据当前方向更新蛇头位置
        switch (orientation) {
            case 0: // 上
                bodys.front().pos_y -= speed;
                break;
            case 1: // 下
                bodys.front().pos_y += speed;
                break;
            case 2: // 左
                bodys.front().pos_x -= speed;
                break;
            case 3: // 右
                bodys.front().pos_x += speed;
                break;
            default:
                return; // 初始方向为右，无需处理
        }

        // 更新蛇身位置（与原有逻辑一致）
        int prevX = oldHeadX, prevY = oldHeadY;
        for (auto it = ++bodys.begin(); it != bodys.end(); ++it) {
            swap(it->pos_x, prevX);
            swap(it->pos_y, prevY);
        }
    }
	
	
	/*
		判断是否吃到食物	

		是 返回true
		否 返回false
	*/
	bool check_eat_food(const food& f)
	{
		// 蛇头坐标与食物坐标重合（考虑物体大小）
		body head = bodys.front();
		 // 考虑身体大小（蛇头矩形与食物矩形重叠）
        return (head.pos_x < f.pos_x + f.width &&
                head.pos_x + head.width > f.pos_x &&
                head.pos_y < f.pos_y + f.height &&
                head.pos_y + head.height > f.pos_y);
	}
	
	
	
	
	/*
		判断是否游戏结束	
		判断游戏是否结束（撞墙或撞自身）
		是 返回true
		否 返回false
	*/
	bool game_over()
	{
		
		/**
		 body head = bodys.front();//蛇头
		 
		// 边界检测（蛇头超出屏幕范围）
        if (head.pos_x < 0 || 
			head.pos_x + head.width >= 800 || // 考虑宽度
			head.pos_y < 0 || 
			head.pos_y + head.height >= 480) // 考虑高度
		{
            return true;
        }
		
		 // 自碰撞检测（蛇头与身体节点重合）
		 
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
		// 墙壁边界：左10，右790（800-10），上10，下470（480-10）
		if (head.pos_x < 10 || 
			head.pos_x + head.width > 790 || // 右边界
			head.pos_y < 10 || 
			head.pos_y + head.height > 470) { // 下边界
			return true;
		}
		// 自碰撞检测
		for (auto it = ++bodys.begin(); it != bodys.end(); ++it) {
			if (head.pos_x == it->pos_x && head.pos_y == it->pos_y) {
				return true;
			}
		}
		return false;
		
	}
	
	
	 /* 蛇身增长（吃食物后调用） */
    void grow() 
	{
        body last = bodys.back();
        // 直接在尾部添加一个与原尾部位置相同的节点
		bodys.push_back(body(last.pos_x, last.pos_y, last.color));
		body_cnt++;
    }
	
	
	
	
};


class Lcd //屏幕类 --尹佳微
{
	public:
		int fd;//文件描述符                
		int * addr_mapper;//映射首地址              
		int width;//宽度  
		int height;//高度
	
	Lcd(int width,int height):width(width), height(height) 
	{
		this->fd = open("/dev/fb0",O_RDWR);
		
		if(this->fd==-1)
		{
			cout<<"打开 "<<"/dev/fb0"<<"失败"<<endl;
		}
		
		this->addr_mapper =(int*)mmap(NULL,height*width*4,PROT_READ|PROT_WRITE,MAP_SHARED,this->fd,0);
		
		if(this->addr_mapper==MAP_FAILED)
		{
			cout<<"映射失败"<<endl;
			
		}
	}
	
	
	
	~Lcd()
	{
		//解除映射
		int munmap_result = munmap(this->addr_mapper,height*width*4);
		if(munmap_result==-1)
		{
			cout<<"解映射失败"<<endl;
			
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
			
		}
		else
		{
			cout<<"关闭文件"<<this->fd<<"成功"<<endl;
			
		}
	}
	
	
	/*
	显示像素点
	
		x:  x坐标
		y:  y坐标
		color : 颜色
	*/
	
	void show_pixel(int x,int y,int color)
	{
		//写入映射文件
		if(x<this->width && x>=0 && y<this->height && y>=0)
		{
			*(this->addr_mapper+y*width+x) = color;
		}
		
	}
	
	
	/*
	清屏
	*/
	void clear_display()
	{
		//写入映射文件
		for(int i=0;i<this->width;i++)
		{
			for(int j=0;j<this->height;j++)
			{
				*(this->addr_mapper+j*width+i)=0xFF000000;
			}
		}
	}
	
	
	// 新增：局部清屏（仅清除蛇和食物，保留墙壁）
    void clear_objects(snake& snake, food& food) {
        // 清除旧食物位置
        for (int dx = 0; dx < food.width; dx++) {
            for (int dy = 0; dy < food.height; dy++) {
                show_pixel(food.pos_x + dx, food.pos_y + dy, 0xFF000000); // 黑色背景
            }
        }
        // 清除蛇尾旧位置（蛇移动时尾部会被新位置覆盖）
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
	显示蛇的身体
	*/
	void show_snake(snake &snake)
	{
		for (const auto& b : snake.bodys) 
		{
            // 绘制矩形（简单实现：每个身体节点为 4x4 像素块）
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
	显示食物
	*/
	void show_food(food &food)
	{
		 // 绘制食物（10x10 像素块）
        for (int dx = 0; dx < food.width; dx++) 
		{
            for (int dy = 0; dy < food.height; dy++) 
			{
                show_pixel(food.pos_x + dx, food.pos_y + dy, food.color); // 绿色
            }
        }
	}
	
	// 新增：绘制墙壁（边框）
    void draw_wall() {
        const int wall_color = 0xFFFFFFFF; // 白色
        const int wall_width = 10; // 墙壁宽度与蛇身一致

        // 上边界
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < wall_width; y++) {
                show_pixel(x, y, wall_color);
            }
        }
        // 下边界
        for (int x = 0; x < width; x++) {
            for (int y = height - wall_width; y < height; y++) {
                show_pixel(x, y, wall_color);
            }
        }
        // 左边界
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < wall_width; x++) {
                show_pixel(x, y, wall_color);
            }
        }
        // 右边界
        for (int y = 0; y < height; y++) {
            for (int x = width - wall_width; x < width; x++) {
                show_pixel(x, y, wall_color);
            }
        }
    }
	
	
};






class touch_display//触摸屏类   --尹佳微
{
	public: 
		
		int ts_fd;	//触摸屏的文件描述符
		int b_x;//x的开始坐标
		int b_y;//y的开始坐标
		int s_x;//x的停止坐标
		int s_y;//y的停止坐标
		
		touch_display() //构造函数
		{
			
		}
		~touch_display() //析构函数
		{
			
		}
		/*
		//打开触摸屏
		成功返回0
		失败 -1
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
		//读取触摸屏信息,将最后点击的位置传给成员变量
		
		*/
		void read_ts() 
		{	
			struct input_event ev;
			int x_flag = 0;  	//x方向坐标起点坐标 标志位
			int y_flag = 0; 	//y方向坐标起点坐标 标志位
			
			
			while(1)		//一次滑动动作
			{
				int r = read(ts_fd, &ev, sizeof(ev));
				if(r != sizeof(ev))
				{
					cout << "read failed" << endl;
					continue;
				}
				//分析信息
				cout << "type = " << ev.type << "code = " << ev.code << "value = " << ev.value << endl;
				if(ev.type == EV_KEY && ev.code == BTN_TOUCH) //触摸屏按键事件-》点击
				{
					if(ev.value == 1) //按下去了
					{
						cout << "down" << endl;
					}
					else //ev.value == 0 弹起来了
					{
						cout << "up" << endl;
						break;
					}
				}
				if(ev.type == EV_ABS && ev.code == ABS_X)  //触摸屏x方向的事件
				{	
					if(x_flag == 0) //x的起点坐标
					{
						x_flag = 1;
						b_x = ev.value;
					}
					s_x = ev.value;		
				}
				if(ev.type == EV_ABS && ev.code == ABS_Y)  //触摸屏y方向的事件
				{
					if(y_flag == 0) //y的起点坐标
					{
						y_flag = 1;
						b_y = ev.value;
					}
					s_y = ev.value;
				}
			}
				
			if(abs(s_x - b_x) - abs(s_y - b_y) > 0 && abs(s_x - b_x) > 20) //x方向的滑动
			{
				if(s_x  - b_x > 0)  //右滑
				{
					cout << "right slide" << endl;
				}
				else //s_x - b_x < 0		//左滑
				{
					cout << "left slide" << endl;
				}
			}
			if(abs(s_x - b_x) - abs(s_y - b_y) <= 0 && abs(s_y - b_y) > 20)	//y方向的滑动
			{
				if(s_y - b_y > 0)  //下滑
				{
					cout << "down slide" << endl;
				}
				else		//上滑 
				{
					cout << "up slide" << endl;
				}
			}
			
			
		}
		
		
		
	/* 
	读取触摸屏输入并控制蛇的方向（返回方向值：0-上，1-下，2-左，3-右） 
	*/
    int get_direction() 
	{
        read_ts(); // 调用原有读取逻辑
        int dx = s_x - b_x;
        int dy = s_y - b_y;

        if (abs(dx) > abs(dy) && abs(dx) > 20) { // x方向滑动
            return (dx > 0) ? 3 : 2; // 右滑=3，左滑=2
        } else if (abs(dy) > 20) { // y方向滑动
            return (dy > 0) ? 1 : 0; // 下滑=1，上滑=0
        }
        return -1; // 无效滑动
    }
	
	
};

int main() // 主函数   --尹佳微
{
	
	// 初始化随机数种子
    srand(time(0));
	
	// 初始化屏幕、蛇和食物
    Lcd lcd(800, 480);
    snake my_snake;
    food my_food(0xFF00FF00);  // 初始食物颜色
	my_food.random_position(800, 480);
	
	//清除一下之前的
	lcd.clear_display();
	
	// **新增**：仅在游戏开始时绘制一次墙壁
    lcd.draw_wall();
	
	
	// 初始化触摸屏
    touch_display ts;
    if (ts.open_ts() != 0) {
        cout << "触摸屏初始化失败！" << endl;
        return -1;
    }
	
	
	// 原子标志位：控制线程运行
    std::atomic<bool> threadRunning(true);
    // 存储最新方向的原子变量
    std::atomic<int> currentDirection(-1);// -1表示无输入，使用蛇的orientation作为当前方向

    // 输入线程：仅更新方向，不直接移动蛇
    std::thread inputThread([&]() {
        while (threadRunning) {
            int dir = ts.get_direction();
            if (dir != -1) {
                // 检查方向是否合法（不能与当前方向相反）
                if ((dir == 0 && my_snake.orientation != 1) ||
                    (dir == 1 && my_snake.orientation != 0) ||
                    (dir == 2 && my_snake.orientation != 3) ||
                    (dir == 3 && my_snake.orientation != 2)) {
                    my_snake.orientation = dir; // 直接更新蛇的朝向
                }
            }
        }
    });
	
	
	
	// 游戏主循环
    bool game_running = true;
	
	
	 while (game_running) 
	{
        // 1. 清屏
        //lcd.clear_display();
		
		//局部清屏
        lcd.clear_objects(my_snake, my_food);
		
        // 自动移动蛇（核心逻辑）
        my_snake.auto_move();


        // 3. 检测吃食物
        if (my_snake.check_eat_food(my_food)) {
            my_snake.grow();
            my_food.random_position(800, 480);  // 生成新食物位置
        }

        // 4. 检测游戏结束
        if (my_snake.game_over()) 
		{
            cout << "游戏结束！得分：" << my_snake.body_cnt - 3 << endl;
            game_running = false;
        }

        // 5. 渲染画面
        lcd.show_snake(my_snake);
        lcd.show_food(my_food);

        // 6. 控制帧率
        //usleep(100000);  // 100ms，约10FPS
		//usleep(5000);
		 // 控制自动移动速度（100ms移动一次）
        //usleep(16666);
		usleep(100000);
    }
	
	
	// 优雅结束线程
    threadRunning = false;
    if (inputThread.joinable()) {
        inputThread.join(); // 等待线程结束
    }

	
	
	
	return 0;
}


//arm-linux-g++ -std=c++11 main.cpp -o main -lpthread
