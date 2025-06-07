#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
using namespace std;

#define H 27 // 地图高度
#define W 60 // 地图宽度

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
	int lastMoveTime;
	int moveFrequency;
};

void initSnake(Snake* snk) { // 初始化蛇
	snk->snakeLength = 3;    // 蛇刚开始的长度为3
	snk->snakeDir = 2;       // 刚开始蛇的方向
	Pos p1 = {W / 2, H / 2};
	Pos p2 = {W / 2 - 1, H / 2};
	Pos p3 = {W / 2 - 2, H / 2};
	snk->snake[0] = p1; // 蛇初始位置在地图中间
	snk->snake[1] = p2;
	snk->snake[2] = p3;
	snk->lastMoveTime = 0;   // 上次移动的时间
	snk->moveFrequency = 10; // 移动间隔时间改为10ms
}

void initMap(Map* map) { // 初始化地图
	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++) {
			map->data[y][x] = EMPTY; // 地图里每个格子都为空，也就是没有一条蛇和食物为0
		}
	}
	map->hasFood = false; // 地图没有食物
}

void drawUnit(Pos p, const char* unit) { // 画单元的目的就是在特定位置打印字符串
	move(p.y + 1, p.x + 1);
	printw("%s", unit);
}

void drawMap(Map* map) { // 画地图
	clear(); // 清空控制台
	for (int i = 0; i < W + 2; i++) {
		mvaddch(0, i, '#');
	}
	for (int y = 0; y < H; y++) {
		mvaddch(y + 1, 0, '#');
		for (int x = 0; x < W; x++) {
			if (map->data[y][x] == EMPTY) {
				mvaddch(y + 1, x + 1, ' ');
			}
		}
		mvaddch(y + 1, W + 1, '#');
	}
	for (int i = 0; i < W + 2; i++) {
		mvaddch(H + 1, i, '#');
	}
	refresh();
}

void drawSnake(Snake* snk) { // 画蛇
	for (int i = 0; i < snk->snakeLength; i++) {
		if (i == 0) {
			drawUnit(snk->snake[i], "O"); // 蛇头用圆形表示
		} else {
			drawUnit(snk->snake[i], "o"); // 蛇身用小圆形表示
		}
	}
}

bool checkOutBound(Pos p) { // 判断是否超出边界，超出返回true，否则返回false
	return (p.x < 0 || p.x >= W || p.y < 0 || p.y >= H);
}

void checkEatFood(Snake* snk, Pos tail, Map* map) { // 判断蛇头有没有碰撞到食物，碰撞到蛇就变长
	Pos head = snk->snake[0]; // 蛇头
	if (map->data[head.y][head.x] == FOOD) {
		snk->snake[snk->snakeLength++] = tail; // 蛇变长，把原来的尾巴位置
		map->data[head.y][head.x] = EMPTY; // 食物位置变为空
		map->hasFood = false; // 地图没有食物了
		drawUnit(tail, "O"); // 画尾巴
	}
}

void moveSnake(Snake* snk) {
	for (int i = snk->snakeLength - 1; i >= 1; i--) {
		snk->snake[i] = snk->snake[i - 1]; // 贪吃蛇的后移
	}    // 因为蛇是向前走的，每个节点位置必须是前一个节点的位置
	snk->snake[0].y += dir[snk->snakeDir][0]; // 改变蛇头的位置
	snk->snake[0].x += dir[snk->snakeDir][1];
}

bool doMove(Snake* snk, Map* map) { // 蛇的移动
	Pos tail = snk->snake[snk->snakeLength - 1]; // 得到蛇的尾部
	drawUnit(tail, " "); // 因为蛇向前走，所以蛇尾原来的位置就为空
	moveSnake(snk);
	if (checkOutBound(snk->snake[0])) { // 如果移动以后判断是否超出边界，超出就停止运动
		return false;
	}
	checkEatFood(snk, tail, map);
	drawUnit(snk->snake[0], "O"); // 蛇向前走，蛇头前一个位置要画一个圆
	return true; // 如果移动超出边界时返回false，也就是移动失败
}

bool checkSnakeMove(Snake* snk, Map* map) {
	int curTime = clock();
	if (curTime - snk->lastMoveTime > snk->moveFrequency) { // 时间间隔
		if (!doMove(snk, map)) return false;
		snk->lastMoveTime = curTime;
	}
	return true; // 如果移动成功返回true，没有移动返回false
}

void checkChangeDir(Snake* snk) { // 检测改变蛇方向
	int ch = getch();
	if (ch != ERR) {
		switch (ch) {
			case 'w': // w上
				if (snk->snakeDir != 1) snk->snakeDir = 0; // 如果为上方向，再按上键就不通
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
			int x = rand() % W; // 随机生成食物的x和y坐标
			int y = rand() % H;
			int i = 0;
			while (i < snk->snakeLength) { // 食物位置不能在蛇身上
				if (x == snk->snake[i].x && y == snk->snake[i].y) { // 判断食物位置是否在蛇身上
					break;
				}
				i++;
			}
			if (i == snk->snakeLength) {
				map->data[y][x] = FOOD;
				map->hasFood = true;
				Pos foodPos = {x, y};
				drawUnit(foodPos, "*"); // 食物用"*"表示
				return;
			}
		}
	}
}

void initGame(Snake* snk, Map* map) { // 游戏设置的函数
	initscr(); // 初始化ncurses
	cbreak(); // 禁用行缓冲
	noecho(); // 不显示输入的字符
	curs_set(0); // 隐藏光标
	keypad(stdscr, TRUE); // 启用功能键
	timeout(0); // 非阻塞输入
	initMap(map); // 初始化地图
	initSnake(snk); // 初始化蛇
	drawMap(map); // 画地图
	drawSnake(snk); // 画蛇
}

int main() {
	srand(time(0));
	Map map;
	Snake snk;
	initGame(&snk, &map);
	while (1) { // 游戏循环
		checkChangeDir(&snk);
		if (!checkSnakeMove(&snk, &map)) {
			break; // 如果蛇移动失败，终止游戏
		}
		checkFoodGenerate(&snk, &map);
		usleep(100000); // 控制游戏速度，改为100微秒（0.1毫秒）
	}
	mvprintw(H/2, W/2 - 4, "Game Over"); // 结束时在中间显示"Game Over"
	refresh();
	getch(); // 等待用户按键
	endwin(); // 结束ncurses
	return 0;
}