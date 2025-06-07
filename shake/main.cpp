#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <termios.h>
#include <fcntl.h>
using namespace std;

#define H 480 // 地图高度
#define W 800 // 地图宽度


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



const int dir[4][2] = { // 蛇运动的方向
    {-1,0}, // 上
    {1,0},  // 下
    {0,-1}, // 左
    {0,1}   // 右
};

enum BlockType {
    EMPTY = 0, // 没有食物，空状态为0
    FOOD = 1,  // 有食物状态为1
};

struct Map {
    BlockType data[H][W]; // 地图每个格子
    bool hasFood;         // 判断地图里有没有食物
};

struct Pos {
    int x;
    int y;
};

struct Snake {
    Pos snake[H * W];
    int snakeDir;        // 蛇的运动方向
    int snakeLength;     // 蛇的长度
    long long lastMoveTime; // 上次移动的时间
    int moveFrequency;
};

// 获取当前时间（微秒）
long long getCurrentTime() {
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

// 设置终端为原始模式，禁用缓冲和回显
void setRawMode(bool enabled) {
    static struct termios oldt, newt;
    
    if (enabled) {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
}

// 检查是否有按键输入
bool hasKeyPress() {
    struct timeval tv = {0,0};
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    return select(1, &readfds, NULL, NULL, &tv) == 1;
}

// 获取按键（非阻塞）
int getKey() {
    if (!hasKeyPress()) return -1;
    
    char ch;
    read(STDIN_FILENO, &ch, 1);
    return ch;
}

void initSnake(Snake* snk) { // 初始化蛇
    snk->snakeLength = 3;    // 蛇刚开始的长度为3
    snk->snakeDir = 2;       // 刚开始蛇的方向
    Pos p1 = {W / 2, H / 2};
    Pos p2 = {W / 2 - 1, H / 2};
    Pos p3 = {W / 2 - 2, H / 2};
    snk->snake[0] = p1; // 蛇初始位置在地图中间
    snk->snake[1] = p2;
    snk->snake[2] = p3;
    snk->lastMoveTime = getCurrentTime();
    snk->moveFrequency = 100000; // 移动间隔时间（微秒）
}

void initMap(Map* map) { // 初始化地图
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            map->data[y][x] = EMPTY;
        }
    }
    map->hasFood = false;
}

// 移动光标到指定位置
void moveCursor(int x, int y) {
    cout << "\033[" << y + 1 << ";" << x + 1 << "H";
}

// 清屏
void clearScreen() {
    cout << "\033[2J";
}

void drawMap(Map* map) { // 画地图
    clearScreen();
    moveCursor(0, 0);
    
    // 绘制顶部边界
    for (int i = 0; i < W + 2; i++) {
        cout << "#";
    }
    cout << endl;
    
    // 绘制中间部分
    for (int y = 0; y < H; y++) {
        cout << "#";
        for (int x = 0; x < W; x++) {
            if (map->data[y][x] == EMPTY) {
                cout << " ";
            } else if (map->data[y][x] == FOOD) {
                cout << "*";
            }
        }
        cout << "#" << endl;
    }
    
    // 绘制底部边界
    for (int i = 0; i < W + 2; i++) {
        cout << "#";
    }
    cout << endl;
}

void drawSnake(Snake* snk) { // 画蛇
    for (int i = 0; i < snk->snakeLength; i++) {
        moveCursor(snk->snake[i].x, snk->snake[i].y + 1);
        if (i == 0) {
            cout << "O"; // 蛇头用圆形表示
        } else {
            cout << "o"; // 蛇身用小圆形表示
        }
    }
    cout.flush();
}

bool checkOutBound(Pos p) { // 判断是否超出边界
    return (p.x < 0 || p.x >= W || p.y < 0 || p.y >= H);
}

void checkEatFood(Snake* snk, Pos tail, Map* map) { // 判断蛇头有没有碰撞到食物
    Pos head = snk->snake[0];
    if (map->data[head.y][head.x] == FOOD) {
        snk->snake[snk->snakeLength++] = tail;
        map->data[head.y][head.x] = EMPTY;
        map->hasFood = false;
        moveCursor(tail.x, tail.y + 1);
        cout << "O";
        cout.flush();
    }
}

void moveSnake(Snake* snk) {
    for (int i = snk->snakeLength - 1; i >= 1; i--) {
        snk->snake[i] = snk->snake[i - 1];
    }
    snk->snake[0].y += dir[snk->snakeDir][0];
    snk->snake[0].x += dir[snk->snakeDir][1];
}

bool doMove(Snake* snk, Map* map) { // 蛇的移动
    Pos tail = snk->snake[snk->snakeLength - 1];
    moveCursor(tail.x, tail.y + 1);
    cout << " ";
    moveSnake(snk);
    if (checkOutBound(snk->snake[0])) {
        return false;
    }
    checkEatFood(snk, tail, map);
    moveCursor(snk->snake[0].x, snk->snake[0].y + 1);
    cout << "O";
    cout.flush();
    return true;
}

bool checkSnakeMove(Snake* snk, Map* map) {
    long long curTime = getCurrentTime();
    if (curTime - snk->lastMoveTime > snk->moveFrequency) {
        if (!doMove(snk, map)) return false;
        snk->lastMoveTime = curTime;
    }
    return true;
}

void checkChangeDir(Snake* snk) { // 检测改变蛇方向
    int ch = getKey();
    if (ch != -1) {
        switch (ch) {
            case 'w': // w上
                if (snk->snakeDir != 1) snk->snakeDir = 0;
                break;
            case 'd': // d右
                if (snk->snakeDir != 2) snk->snakeDir = 3;
                break;
            case 's': // s下
                if (snk->snakeDir != 0) snk->snakeDir = 1;
                break;
            case 'a': // a左
                if (snk->snakeDir != 3) snk->snakeDir = 2;
                break;
            default:
                break;
        }
    }
}

void checkFoodGenerate(Snake* snk, Map* map) { // 生成食物
    if (!map->hasFood) {
        while (1) {
            int x = rand() % W;
            int y = rand() % H;
            int i = 0;
            while (i < snk->snakeLength) {
                if (x == snk->snake[i].x && y == snk->snake[i].y) {
                    break;
                }
                i++;
            }
            if (i == snk->snakeLength) {
				
                map->data[y][x] = FOOD;
                map->hasFood = true;
                moveCursor(x, y + 1);
                cout << "*";
                cout.flush();
                return;
            }
        }
    }
}

void initGame(Snake* snk, Map* map) { // 游戏设置的函数
    setRawMode(true); // 设置终端为原始模式
    system("stty -echo"); // 禁用回显
    initMap(map);
    initSnake(snk);
    drawMap(map);
    drawSnake(snk);
}

void endGame() {
    setRawMode(false); // 恢复终端模式
    system("stty echo"); // 恢复回显
    cout << "\033[?25h"; // 显示光标
    cout << endl;
}

int main() {
    srand(time(0));
    Map map;
    Snake snk;
    
	
    // 隐藏光标
    cout << "\033[?25l";
    
    initGame(&snk, &map);
    
    try {
        while (1) { // 游戏循环
            checkChangeDir(&snk);
            if (!checkSnakeMove(&snk, &map)) {
                break;
            }
            checkFoodGenerate(&snk, &map);
            usleep(10000); // 短暂休眠，减少CPU使用率
        }
    } catch (...) {
        endGame();
        throw;
    }
    
    // 显示游戏结束信息
    moveCursor(W/2 - 4, H/2);
    cout << "Game Over" << endl;
    
    endGame();
    return 0;
}