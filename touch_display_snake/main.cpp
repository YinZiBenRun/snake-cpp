#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <termios.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cstring>
using namespace std;

#define H 27 // 地图高度
#define W 60 // 地图宽度
#define SCALE 8 // 每个游戏格子对应的像素大�?

// 颜色定义 (ARGB格式)
const int COLOR_BACKGROUND = 0xFF000000; // 黑色背景
const int COLOR_SNAKE_HEAD = 0xFFFF0000; // 红色蛇头
const int COLOR_SNAKE_BODY = 0xFF00FF00; // 绿色蛇身
const int COLOR_FOOD = 0xFF0000FF;      // 蓝色食物
const int COLOR_WALL = 0xFFFFFFFF;      // 白色墙壁

const int dir[4][2] = { // 蛇运动的方向
    {-1,0}, // �?
    {1,0},  // �?
    {0,-1}, // �?
    {0,1}   // �?
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
    long long lastMoveTime; // 上次移动的时�?
    int moveFrequency;
};

class Display {
public:
    int length;
    int width;
    char pathname[512] = "/dev/fb0";
    int* map_addr;
    int fd;

    Display(int length, int width) : length(length), width(width) {}
    
    Display(int length, int width, const char* dist) : length(length), width(width) {
        strcpy(this->pathname, dist);
    }
    
    ~Display() {
        if (map_addr != nullptr) {
            munmap_display();
        }
    }

    int init_display() {
        this->fd = open(this->pathname, O_RDWR);
        if (this->fd == -1) {
            cout << "�� " << pathname << "ʧ��" << endl;
            return -1;
        }

        this->map_addr = (int*)mmap(NULL, length * width * 4, PROT_READ | PROT_WRITE, MAP_SHARED, this->fd, 0);
        if (this->map_addr == MAP_FAILED) {
            cout << "ӳ��ʧ��" << endl;
            return -1;
        }
        return 1;
    }

    void draw_display(int x, int y, int color) {
        if (x < this->length && x >= 0 && y < this->width && y >= 0) {
            *(this->map_addr + y * length + x) = color;
        }
    }

    int munmap_display() {
        int munmap_result = munmap(this->map_addr, length * width * 4);
        if (munmap_result == -1) {
            cout << "��ӳ��ʧ��" << endl;
            return -1;
        }

        int close_result = close(this->fd);
        if (close_result == -1) {
            cout << "�ر��ļ�fd=" << this->fd << "ʧ��" << endl;
            return -1;
        }
        return 1;
    }
};

// 获取当前时间（微秒）
long long getCurrentTime() {
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

// 设置终端为原始模式，禁用缓冲和回�?
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

// 获取按键（非阻塞�?
int getKey() {
    if (!hasKeyPress()) return -1;
    
    char ch;
    read(STDIN_FILENO, &ch, 1);
    return ch;
}

void initSnake(Snake* snk) {
    snk->snakeLength = 3;
    snk->snakeDir = 2;
    Pos p1 = {W / 2, H / 2};
    Pos p2 = {W / 2 - 1, H / 2};
    Pos p3 = {W / 2 - 2, H / 2};
    snk->snake[0] = p1;
    snk->snake[1] = p2;
    snk->snake[2] = p3;
    snk->lastMoveTime = getCurrentTime();
    snk->moveFrequency = 100000;
}

void initMap(Map* map) {
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            map->data[y][x] = EMPTY;
        }
    }
    map->hasFood = false;
}

void drawMap(Map* map, Display* display) {
    // 绘制顶部边界
    for (int i = 0; i < W + 2; i++) {
        for (int j = 0; j < SCALE; j++) {
            for (int k = 0; k < SCALE; k++) {
                display->draw_display(i * SCALE + j, k, COLOR_WALL);
            }
        }
    }
    
    // 绘制中间部分
    for (int y = 0; y < H; y++) {
        // 绘制左边�?
        for (int j = 0; j < SCALE; j++) {
            for (int k = 0; k < SCALE; k++) {
                display->draw_display(j, (y + 1) * SCALE + k, COLOR_WALL);
            }
        }
        
        for (int x = 0; x < W; x++) {
            if (map->data[y][x] == EMPTY) {
                // 绘制背景
                for (int i = 0; i < SCALE; i++) {
                    for (int j = 0; j < SCALE; j++) {
                        display->draw_display((x + 1) * SCALE + i, (y + 1) * SCALE + j, COLOR_BACKGROUND);
                    }
                }
            } else if (map->data[y][x] == FOOD) {
                // 绘制食物
                for (int i = 0; i < SCALE; i++) {
                    for (int j = 0; j < SCALE; j++) {
                        display->draw_display((x + 1) * SCALE + i, (y + 1) * SCALE + j, COLOR_FOOD);
                    }
                }
            }
        }
        
        // 绘制右边�?
        for (int j = 0; j < SCALE; j++) {
            for (int k = 0; k < SCALE; k++) {
                display->draw_display((W + 1) * SCALE + j, (y + 1) * SCALE + k, COLOR_WALL);
            }
        }
    }
    
    // 绘制底部边界
    for (int i = 0; i < W + 2; i++) {
        for (int j = 0; j < SCALE; j++) {
            for (int k = 0; k < SCALE; k++) {
                display->draw_display(i * SCALE + j, (H + 1) * SCALE + k, COLOR_WALL);
            }
        }
    }
}

void drawSnake(Snake* snk, Display* display) {
    for (int i = 0; i < snk->snakeLength; i++) {
        if (i == 0) {
            // 绘制蛇头
            for (int x = 0; x < SCALE; x++) {
                for (int y = 0; y < SCALE; y++) {
                    display->draw_display((snk->snake[i].x + 1) * SCALE + x, 
                                        (snk->snake[i].y + 1) * SCALE + y, 
                                        COLOR_SNAKE_HEAD);
                }
            }
        } else {
            // 绘制蛇身
            for (int x = 0; x < SCALE; x++) {
                for (int y = 0; y < SCALE; y++) {
                    display->draw_display((snk->snake[i].x + 1) * SCALE + x, 
                                        (snk->snake[i].y + 1) * SCALE + y, 
                                        COLOR_SNAKE_BODY);
                }
            }
        }
    }
}

bool checkOutBound(Pos p) {
    return (p.x < 0 || p.x >= W || p.y < 0 || p.y >= H);
}

void checkEatFood(Snake* snk, Pos tail, Map* map) {
    Pos head = snk->snake[0];
    if (map->data[head.y][head.x] == FOOD) {
        snk->snake[snk->snakeLength++] = tail;
        map->data[head.y][head.x] = EMPTY;
        map->hasFood = false;
    }
}

void moveSnake(Snake* snk) {
    for (int i = snk->snakeLength - 1; i >= 1; i--) {
        snk->snake[i] = snk->snake[i - 1];
    }
    snk->snake[0].y += dir[snk->snakeDir][0];
    snk->snake[0].x += dir[snk->snakeDir][1];
}

bool doMove(Snake* snk, Map* map, Display* display) {
    Pos tail = snk->snake[snk->snakeLength - 1];
    // 清除蛇尾
    for (int x = 0; x < SCALE; x++) {
        for (int y = 0; y < SCALE; y++) {
            display->draw_display((tail.x + 1) * SCALE + x, 
                                (tail.y + 1) * SCALE + y, 
                                COLOR_BACKGROUND);
        }
    }
    
    moveSnake(snk);
    if (checkOutBound(snk->snake[0])) {
        return false;
    }
    checkEatFood(snk, tail, map);
    return true;
}

bool checkSnakeMove(Snake* snk, Map* map, Display* display) {
    long long curTime = getCurrentTime();
    if (curTime - snk->lastMoveTime > snk->moveFrequency) {
        if (!doMove(snk, map, display)) return false;
        snk->lastMoveTime = curTime;
    }
    return true;
}

void checkChangeDir(Snake* snk) {
    int ch = getKey();
    if (ch != -1) {
        switch (ch) {
            case 'w':
                if (snk->snakeDir != 1) snk->snakeDir = 0;
                break;
            case 'd':
                if (snk->snakeDir != 2) snk->snakeDir = 3;
                break;
            case 's':
                if (snk->snakeDir != 0) snk->snakeDir = 1;
                break;
            case 'a':
                if (snk->snakeDir != 3) snk->snakeDir = 2;
                break;
            default:
                break;
        }
    }
}

void checkFoodGenerate(Snake* snk, Map* map, Display* display) {
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
                // 绘制食物
                for (int i = 0; i < SCALE; i++) {
                    for (int j = 0; j < SCALE; j++) {
                        display->draw_display((x + 1) * SCALE + i, 
                                            (y + 1) * SCALE + j, 
                                            COLOR_FOOD);
                    }
                }
                return;
            }
        }
    }
}

void initGame(Snake* snk, Map* map, Display* display) {
    setRawMode(true);
    system("stty -echo");
    initMap(map);
    initSnake(snk);
    drawMap(map, display);
    drawSnake(snk, display);
}

void endGame() {
    setRawMode(false);
    system("stty echo");
}

int main() {
    srand(time(0));
    Map map;
    Snake snk;
    
    // 初始化帧缓冲显示
    Display display((W + 2) * SCALE, (H + 2) * SCALE);
    if (display.init_display() == -1) {
        cout << "��ʾ��ʼ��ʧ��" << endl;
        return -1;
    }
    
    initGame(&snk, &map, &display);
    
    try {
        while (1) {
            checkChangeDir(&snk);
            if (!checkSnakeMove(&snk, &map, &display)) {
                break;
            }
            checkFoodGenerate(&snk, &map, &display);
            drawSnake(&snk, &display);
            usleep(10000);
        }
    } catch (...) {
        endGame();
        throw;
    }
    
    endGame();
    display.munmap_display();
    return 0;
}
