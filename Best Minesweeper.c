#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_ROWS 102 // 最大棋盘行数为 100（可自己进行修改）
#define MAX_COLS 102 // 最大棋盘列数为 100
// 定义难度等级的常量
#define EASY_COUNT 10
#define MED_COUNT 40
#define HARD_COUNT 99
// 定义空格和地雷的常量
#define EMPTY_CELL '0'
#define MINE_CELL '1'

typedef struct
{
    int row;
    int col;
} Cell;

typedef struct
{
    // 游戏地图行数
    int rows;
    // 游戏地图列数
    int cols;
    // 地雷数量
    int mineCount;
    // 地雷起始位置x坐标
    int startX;
    // 地雷起始位置y坐标
    int startY;
} GameConfig;

typedef struct
{
    // 游戏矩阵的行数
    int rows;
    // 游戏矩阵的列数
    int cols;
    // 地雷数量
    int mineCount;
    // 地雷的起始位置的x坐标
    int startX;
    // 地雷的起始位置的y坐标
    int startY;
    // 游戏矩阵
    char mine[MAX_ROWS][MAX_COLS];
    // 显示矩阵
    char show[MAX_ROWS][MAX_COLS];
    // 剩余的格子数
    int remainingCells;
    // 标记的雷数
    int flaggedMines; // 标记地雷数
    // 是否获胜
    int win;
    // 是否踩到地雷
    bool steppedOnMine;
    // 是否处于标记模式
    bool flagMode;
    // 是否处于提示模式
    bool tipMode;
    // 是否处于随机模式
    bool ranMode;
    // 是否返回主菜单
    bool returnedToMainMenu;
} GameState;

time_t gameStartTime;
char input[100];

void gameLoop();
// 打印菜单
void printMenu();
// 打印帮助信息
void printHelp();
// 重新开始游戏
bool replayGame();
// 清空屏幕
void clearScreen();
// 自定义游戏
void customizeGame();
// 显示已玩时间
void showTimeElapsed();
// 退出游戏
bool exitGame(char input[10]);
// 开始游戏
void startGame(int difficulty);
// 设置地雷
void setMines(GameState *gameState);
// 查找地雷
void findMines(GameState *gameState);
// 显示地雷
void revealMines(GameState *gameState);
// 显示棋盘
void displayBoard(GameState *gameState);
// 检查游戏状态
void checkGameState(GameState *gameState);
// 提示单元格
void tipCell(GameState *gameState, int x, int y);
// 标记单元格
void flagCell(GameState *gameState, int x, int y);
// 处理输入
void handleInput(GameState *gameState, char input[10]);
// 检查坐标是否有效
bool isValidCoordinate(int x, int y, int rows, int cols);
// 扩展空单元格
void expandEmptyCells(GameState *gameState, int x, int y);
// 获取地雷数量
int getMineCount(char mine[MAX_ROWS][MAX_COLS], int x, int y);
// 初始化棋盘
void initBoard(char board[MAX_ROWS][MAX_COLS], char set, int rows, int cols);

void initBoard(char board[MAX_ROWS][MAX_COLS], char set, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            board[i][j] = set;
        }
    }
}

void displayBoard(GameState *gameState)
{
    // 获取游戏状态的行数和列数
    int rows = gameState->rows;
    int cols = gameState->cols;
    // 获取游戏状态的show数组
    char(*board)[MAX_COLS] = gameState->show;
    // 计算未揭的格子数量
    int unrevealedCount = (rows - 2) * (cols - 2) - gameState->mineCount;

    // 清屏
    printf("\033[2J\033[H");
    // 打印行号
    printf("\033[1;34m   ");
    for (int j = 1; j < cols - 1; j++)
    {
        printf("%2d ", j);
    }
    printf("\n");
    // 打印列号
    for (int i = 1; i < rows - 1; i++)
    {
        printf("\033[1;34m%2d ", i);
        for (int j = 1; j < cols - 1; j++)
        {
            // 根据show数组中的值，打印不同的颜色
            switch (board[i][j])
            {
            case '0':
                printf("\033[42;34m %c \033[0m", board[i][j]);
                unrevealedCount--;
                break;
            case '1':
                printf("\033[47;2;34m %c \033[0m", board[i][j]);
                unrevealedCount--;
                break;
            case '*':
                printf("\033[44;1;34m %c \033[0m", board[i][j]);
                break;
            case '2':
                printf("\033[46;2;34m %c \033[0m", board[i][j]);
                unrevealedCount--;
                break;
            case 'F':
                printf("\033[45;1;34m %c \033[0m", board[i][j]);
                break;
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                printf("\033[43;1;34m %c \033[0m", board[i][j]);
                unrevealedCount--;
                break;
            case 'X':
                printf("\033[41;1;34m %c \033[0m", board[i][j]);
                break;
            default:
                printf("\033[0m");
                break;
            }
        }
        printf("\n");
    }
    // 打印未揭的格子数量
    printf("当前未揭数量：%d\n", unrevealedCount);
}

bool replayGame()
{
    // 询问用户是否重新开始游戏
    printf("是否要重新开始游戏?(Y/N)\n");
    char choice;
    fgets(input, sizeof(input), stdin);
    sscanf(input, " %c", &choice, sizeof(choice));
    if (choice == 'Y' || choice == 'y')
    {
        return true;
    }
    else
    {
        printf("退出游戏...\n");
        exit(0);
        return false;
    }
}

bool exitGame(char input[10])
{
    // 检查用户输入是否为exit或e
    const char EXIT_COMMAND[] = "exit\n";
    const char SHORT_EXIT_COMMAND[] = "e\n";
    if (strncmp(input, EXIT_COMMAND, sizeof(EXIT_COMMAND) - 1) == 0 || strncmp(input, SHORT_EXIT_COMMAND, sizeof(SHORT_EXIT_COMMAND) - 1) == 0)
    {
        printf("正在退出游戏, 返回到主界面...\n");
        return true;
    }
    return false;
}

void setMines(GameState *gameState)
{
    // 获取游戏状态中的行数、列数、地雷数量、初始位置和地雷位置
    int rows = gameState->rows;
    int cols = gameState->cols;
    int count = gameState->mineCount;
    int startX = gameState->startX;
    int startY = gameState->startY;
    char(*mine)[MAX_COLS] = gameState->mine;

    // 随机生成地雷位置
    int minesPlaced = 0;
    while (minesPlaced < count)
    {
        int x = rand() % (rows - 2) + 1;
        int y = rand() % (cols - 2) + 1;

        // 如果雷在初始位置或其周围，那么重新生成雷的位置
        if (abs(x - startX) <= 1 && abs(y - startY) <= 1) // 如果雷在初始位置或其周围，那么重新生成雷的位置
            continue;

        // 如果当前位置为空，则将其设置为地雷
        if (mine[x][y] == EMPTY_CELL)
        {
            mine[x][y] = MINE_CELL;
            minesPlaced++;
        }
    }
}

int getMineCount(char mine[MAX_ROWS][MAX_COLS], int x, int y)
{
    int count = 0;
    for (int i = x - 1; i <= x + 1; i++)
    {
        for (int j = y - 1; j <= y + 1; j++)
        {
            count += mine[i][j] - '0';
        }
    }
    return count;
}

bool isValidCoordinate(int x, int y, int rows, int cols)
{
    return (x >= 1 && x < rows - 1 && y >= 1 && y < cols - 1);
}

void handleInput(GameState *gameState, char input[10])
{
    int x, y;
    const char MINE = '1';

    // 检查输入是否为退出游戏
    if (exitGame(input))
    {
        gameState->returnedToMainMenu = true;
    }
    // 检查输入是否为显示时间
    else if (strncmp(input, "time\n", sizeof("time\n") - 1) == 0)
    {
        showTimeElapsed();
    }
    // 检查输入是否为进入标记模式
    else if (strncmp(input, "mark\n", sizeof("mark\n") - 1) == 0 || strncmp(input, "m\n", sizeof("m\n") - 1) == 0)
    {
        gameState->flagMode = true;
        gameState->tipMode = false;
        printf("已进入标记模式\n");
    }
    // 检查输入是否为显示帮助信息
    else if (strncmp(input, "help\n", sizeof("help\n") - 1) == 0 || strncmp(input, "h\n", sizeof("h\n") - 1) == 0)
    {
        printHelp();
    }
    // 检查输入是否为退出标记模式
    else if (strncmp(input, "mark exit\n", sizeof("mark exit\n") - 1) == 0 || strncmp(input, "me\n", sizeof("me\n") - 1) == 0)
    {
        gameState->flagMode ? (gameState->flagMode = false, printf("已退出标记模式，进入扫雷模式\n")) : printf("你未进入标记模式，输入 (mark)或(m) 进入标记模式\n");
    }
    // 检查输入是否为显示提示信息
    else if (strncmp(input, "tip\n", sizeof("tip\n") - 1) == 0 || strncmp(input, "t\n", sizeof("t\n") - 1) == 0)
    {
        gameState->tipMode = true;
    }
    // 检查输入是否为显示剩余格数
    else if (strncmp(input, "view\n", sizeof("view\n") - 1) == 0 || strncmp(input, "v\n", sizeof("v\n") - 1) == 0)
    {
        printf("当前的雷数为: %d\n", gameState->mineCount - gameState->flaggedMines);
    }
    // 检查输入是否为自动扫雷模式
    else if (strncmp(input, "auto sweep", sizeof("auto sweep") - 1) == 0 || strncmp(input, "as", sizeof("as") - 1) == 0)
    {
        int count;
        if (sscanf(input, "%*s %d", &count) == 1 && count > 0)
        {
            int sweptCount = 0;
            while (sweptCount < count)
            {
                int x = rand() % (gameState->rows - 2) + 1;
                int y = rand() % (gameState->cols - 2) + 1;

                if (gameState->show[x][y] == '*' && gameState->mine[x][y] != MINE)
                {
                    gameState->show[x][y] = getMineCount(gameState->mine, x, y) + '0';
                    gameState->remainingCells--;
                    gameState->win++;
                    sweptCount++;
                    if (getMineCount(gameState->mine, x, y) == 0)
                    {
                        expandEmptyCells(gameState, x, y);
                    }
                }
            }
            displayBoard(gameState);
            checkGameState(gameState);
        }
        else if (strncmp(input, "as e", sizeof("as e") - 1) == 0)
        {
            printf("退出自动扫雷模式！\n");
            gameState->ranMode = false;
            gameState->tipMode = false;
        }
        else
        {
            printf("无效的命令。请输入有效的命令。\n");
        }
    }
    // 检查输入是否为提示模式
    else if (gameState->tipMode && sscanf(input, "%d %d", &x, &y) == 2 && isValidCoordinate(x, y, gameState->rows, gameState->cols))
    {
        tipCell(gameState, x, y);
        gameState->tipMode = false;
    }
    // 检查输入是否为随机模式
    else if (strncmp(input, "ran\n", sizeof("ran\n") - 1) == 0)
    {
        if (gameState->tipMode)
        {
            gameState->ranMode = true;
            int x, y;
            do
            {
                x = rand() % (gameState->rows - 2) + 1;
                y = rand() % (gameState->cols - 2) + 1;
            } while (gameState->show[x][y] != '*' || gameState->mine[x][y] == '1');
            printf("(行:%d, 列:%d 无地雷)\n", x, y);
            gameState->tipMode = false;
        }
        else
        {
            printf("请先输入t进入提示模式！\n");
        }
    }
    // 检查输入是否为指定坐标
    else if (sscanf(input, "%d %d", &x, &y) == 2 && isValidCoordinate(x, y, gameState->rows, gameState->cols))
    {
        if (gameState->flagMode)
        {
            flagCell(gameState, x, y);
            displayBoard(gameState);
        }
        else if (gameState->show[x][y] != '*')
        {
            printf("该格已经被排查，请输入有效的坐标！\n");
        }
        else if (gameState->mine[x][y] == MINE)
        {
            printf("糟糕，你踩到地雷了！\n");
            gameState->steppedOnMine = true;
            revealMines(gameState);
            displayBoard(gameState);
            checkGameState(gameState);
        }
        else
        {
            gameState->win++;
            expandEmptyCells(gameState, x, y);
            displayBoard(gameState);
            checkGameState(gameState);
        }
    }
    else
    {
        printf("无效的坐标，请输入有效的坐标！\n");
    }
}

void checkGameState(GameState *gameState)
{
    // 如果玩家获胜，显示游戏结束信息
    if ((gameState->win == gameState->remainingCells - gameState->mineCount && !gameState->steppedOnMine) ||
        (gameState->flaggedMines == gameState->mineCount && gameState->steppedOnMine) ||
        (gameState->win == gameState->remainingCells - gameState->mineCount))
    {
        // 显示地雷信息
        revealMines(gameState);
        // 显示游戏板
        displayBoard(gameState);
        printf("恭喜你，你已排除所有地雷！\n");
        // 玩家重新开始游戏
        if (replayGame())
        {
            gameLoop();
        }
    }
    // 如果玩家踩到地雷，显示游戏结束信息
    else if (gameState->steppedOnMine)
    {
        // 显示地雷信息
        if (replayGame())
        {
            gameLoop();
        }
    }
}

void findMines(GameState *gameState)
{
    // 打印提示信息，提示用户输入模式字符或要排查的坐标（行 列）
    char input[10];
    char modeString[100];

    // 当剩余的格子数小于剩余的雷数，且没有踩到雷时，循环执行
    while (gameState->win < gameState->remainingCells - gameState->mineCount && !gameState->steppedOnMine)
    {
        // 清空输入缓冲区
        fflush(stdin);
        // 根据当前模式，打印提示信息
        strcpy(modeString, gameState->flagMode ? "输入(mark exit)或(me)可退出,标记的坐标（行 列）：" : gameState->tipMode ? "当前模式：指定提示(sp) 候选:|{随机提示(ran)}{一键扫雷模式(as+雷数)(as e退出)}|\n请指定需要提示的坐标(行 列): "
                                                                                                                          : "请输入模式字符或要排查的坐标（行 列）：");

        // 打印提示信息
        printf("%s", modeString);
        // 获取用户输入
        fgets(input, 10, stdin);
        // 处理用户输入
        handleInput(gameState, input);
        // 检查游戏状态
        checkGameState(gameState);
        // 如果返回主菜单，则重新开始游戏
        if (gameState->returnedToMainMenu)
        {
            gameLoop();
        }
    }
}

void expandEmptyCells(GameState *gameState, int x, int y)
{
    // 获取游戏状态中的行数和列数
    int rows = gameState->rows;
    int cols = gameState->cols;
    // 获取游戏状态中的雷坐标
    char(*mine)[MAX_COLS] = gameState->mine;
    // 获取游戏状态中的显示坐标
    char(*show)[MAX_COLS] = gameState->show;

    // 初始化访问标记数组
    bool visited[MAX_ROWS][MAX_COLS] = {false};
    // 初始化队列
    Cell queue[MAX_ROWS * MAX_COLS];
    // 初始化队列头尾指针
    int front = 0;
    int rear = 0;

    // 将当前坐标添加到队列中
    queue[rear].row = x;
    queue[rear].col = y;
    // 标记当前坐标已访问
    visited[x][y] = true;
    // 尾指针后移
    rear++;

    // 当队列头尾指针不指向同一个位置时，循环执行
    while (front != rear)
    {
        // 获取队列头指针指向的元素
        Cell cell = queue[front];
        // 头指针后移
        front++;

        // 获取当前坐标周围的雷数
        int count = getMineCount(mine, cell.row, cell.col);
        ;
        // 打印当前坐标周围的雷数
        // printf("当前坐标(%d, %d)周围的雷数为%d\n", cell.row, cell.col, count);
        // 设置当前坐标显示的雷数
        show[cell.row][cell.col] = count + '0';
        // 剩余格子数减一
        gameState->remainingCells--;

        // 如果当前坐标周围的雷数为0，则将当前坐标周围的坐标添加到队列中
        if (count == 0)
        {
            for (int i = cell.row - 1; i <= cell.row + 1; i++)
            {
                for (int j = cell.col - 1; j <= cell.col + 1; j++)
                {
                    // 如果当前坐标和周围坐标相同，则跳过
                    if (i == cell.row && j == cell.col)
                    {
                        continue;
                    }

                    // 如果当前坐标和周围坐标有效，且未访问过，且显示的雷数为*，则将周围坐标添加到队列中
                    if (isValidCoordinate(i, j, rows, cols) && !visited[i][j] && show[i][j] == '*')
                    {
                        queue[rear].row = i;
                        queue[rear].col = j;
                        visited[i][j] = true;
                        rear++;
                    }
                }
            }
        }
    }
}

void flagCell(GameState *gameState, int x, int y)
{
    // 获取show和mine数组
    char(*show)[MAX_COLS] = gameState->show;
    char(*mine)[MAX_COLS] = gameState->mine;

    // 定义一个8个元素的数组，用来存储每个格子的上下左右8个格子的状态
    int surroundingSquareStatus[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    // 如果当前格子是*，则开始标记
    if (show[x][y] == '*')
    {
        // 标记当前格子

        show[x][y] = 'F';
        // 遍历当前格子的上下左右8个格子
        for (int i = 0; i < 8; i++)
        {
            // 获取当前格子的上下左右8个格子的坐标
            int adjacentX = x + surroundingSquareStatus[i][0];
            int adjacentY = y + surroundingSquareStatus[i][1];

            // 判断坐标是否在棋盘内
            if (!isValidCoordinate(adjacentX, adjacentY, gameState->rows, gameState->cols))
            {
                continue;
            }

            // 如果当前格子的上下左右8个格子的状态是雷，则将当前格子的状态设置为雷，并结束循环
            if (show[adjacentX][adjacentY] - '0' == getMineCount(mine, adjacentX, adjacentY) && isValidCoordinate(adjacentX, adjacentY, gameState->rows, gameState->cols))
            {
                if (mine[x][y] == MINE_CELL)
                {
                    gameState->flaggedMines++;
                }
                break;
            }
        }
        printf("成功标记该格！\n");
    }
    // 如果当前格子是F，则取消标记
    else if (show[x][y] == 'F')
    {
        show[x][y] = '*';
        // 如果当前格子的状态是雷，则减少地雷数量
        if (mine[x][y] == MINE_CELL)
            gameState->flaggedMines--; // 取消地雷标记，减少地雷数量
        printf("取消标记该格！\n");
    }
    // 如果当前格子不是*也不是F，则提示输入有效坐标
    else
    {
        printf("该格已经被排查，请输入有效的坐标！\n");
    }
    gameState->ranMode = false;
}

void tipCell(GameState *gameState, int x, int y)
{
    // 获取棋盘行数和列数
    int rows = gameState->rows;
    int cols = gameState->cols;
    // 获取mine数组
    char(*mine)[MAX_COLS] = gameState->mine;

    // 判断坐标是否在棋盘内
    if (isValidCoordinate(x, y, rows, cols))
    {
        // 如果当前格子的状态是雷，则提示有地雷
        if (mine[x][y] == MINE_CELL)
        {
            printf("(行:%d, 列:%d 有地雷)\n", x, y);
        }
        // 否则提示没有地雷
        else
        {
            printf("(行:%d, 列:%d 无地雷)\n", x, y);
        }
    }
    // 如果坐标不在棋盘内，则提示输入有效坐标
    else
    {
        printf("无效的坐标，请输入有效的坐标！\n");
    }
    gameState->ranMode = false;
}

void revealMines(GameState *gameState)
{
    // 遍历所有非边界单元格，如果该单元格是地雷，则将其显示为X，并将其标记为已发现
    int rows = gameState->rows;
    int cols = gameState->cols;
    char(*mine)[MAX_COLS] = gameState->mine;
    char(*show)[MAX_COLS] = gameState->show;

    for (int i = 1; i < rows - 1; i++)
    {
        for (int j = 1; j < cols - 1; j++)
        {
            if (mine[i][j] == '1')
            {
                mine[i][j] = 'X';
                show[i][j] = 'X';
            }
        }
    }
    gameState->ranMode = false;
}

void customizeGame()
{
    // 用户自定义游戏
    GameConfig config;
    printf("请输入自定义游戏的行列和雷数。格式：(行 列 雷数)");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%d %d %d", &config.rows, &config.cols, &config.mineCount);

    int rows = config.rows + 2;
    int cols = config.cols + 2;

    // 检查输入是否有效
    if (rows < 5 || cols < 5 || config.mineCount >= config.rows * config.cols)
    {
        printf("无效的自定义参数。退出游戏...\n");
        return;
    }

    if (rows > MAX_ROWS || cols > MAX_COLS)
    {
        printf("无效的自定义参数。行数和列数不可以大于%d和%d. 退出游戏...\n", MAX_ROWS - 2, MAX_COLS - 2);
        return;
    }

    // 初始化随机数种子
    srand((unsigned int)time(NULL));
    GameState gameState;
    gameState.rows = rows;
    gameState.cols = cols;
    gameState.mineCount = config.mineCount;
    gameState.remainingCells = (rows - 2) * (cols - 2);
    gameState.win = 0;
    gameState.steppedOnMine = false;
    gameState.flagMode = false;
    gameState.tipMode = false;
    gameState.returnedToMainMenu = false;

    // 初始化棋盘
    initBoard(gameState.show, '*', rows, cols);
    displayBoard(&gameState);
    bool validInput = false;
    while (!validInput)
    {
        printf("请输入初始位置坐标（行 列）:");
        fgets(input, sizeof(input), stdin);
        int inputCount = sscanf(input, "%d %d", &config.startX, &config.startY); // 添加数量检查
        if (inputCount < 2)
        { // 如果输入数小于2
            printf("你输入的坐标不完整，请重新输入两个坐标。\n");
            while (getchar() != '\n')
                ;
            continue;
        }
        if (config.startX <= 0 || config.startY <= 0 || config.startX >= rows || config.startY >= cols)
        {
            printf("无效的坐标，请输入有效的坐标！\n");
        }
        else
        {
            validInput = true;
        }
    }
    // 设置地雷
    initBoard(gameState.mine, '0', rows, cols);
    gameState.startX = config.startX;
    gameState.startY = config.startY;
    setMines(&gameState);
    // 扩展未发现单元格
    expandEmptyCells(&gameState, config.startX, config.startY);
    displayBoard(&gameState);
    // 查找地雷
    findMines(&gameState);
    // 检查游戏状态
    checkGameState(&gameState);
}

void printMenu()
{
    printf("\n\033[1;33m欢迎来到扫雷游戏!\033[0m\n");
    printf("\033[1;32m1. 初级\n\033[0m");
    printf("\033[1;33m2. 中级\n\033[0m");
    printf("\033[1;31m3. 高级\n\033[0m");
    printf("\033[1;35m4. 自定义游戏模式(棋盘超出终端请Ctrl+滚轮调整大小)\n\033[0m");
    printf("\033[1;34m5. 必看|(游戏帮助)|\n\033[0m");
    printf("0. 退出游戏\n");
    printf("\033[1;36m请输入选项编号:\033[0m\n");
}

void printHelp()
{
    printf("\033[1;31m+-----------------------------------------------------------+\n");
    printf("|                     你选择了游戏提示。                    |\n");
    printf("|    (一)游戏指令提示：                                     |\n");
    printf("|    1.游戏过程中可以输出(mark)或(m)进入标记模式(连续标记)  |\n");
    printf("|      在标记模式下输入(mark exit)或(me)可以退出标记模式    |\n");
    printf("|    2.游戏过程中输入(tip)或(t)可进入提示模式(指定坐标提示) |\n");
    printf("|      在提示模式下输入(ran)可以随机提示一个未被探的无雷坐标|\n");
    printf("|      在任何游戏中输入(as+雷数)可随机揭开输入雷数的无雷坐标|\n");
    printf("|      在任何游戏中输入(as+雷数)可随机揭开输入雷数的无雷坐标|\n");
    printf("|    3.在游戏过程中输入(exit)或(e)可退出当前游戏返回主界面  |\n");
    printf("|      在主界面或游戏结束界面输入(e)或(exit)可退出整个游戏  |\n");
    printf("|    4.在游戏过程中输入(time)可显示从游戏开始到当前的时间   |\n");
    printf("|    5.在游戏过程中输入(view)或(v)可以显示当前难度总雷数量  |\n");
    printf("|    6.在游戏过程中输入(help)或(h)可以显示当前游戏帮助窗口  |\n");
    printf("|    (二)游戏过程提示：胜利条件为所有雷当前都可以被计算出   |\n");
    printf("|    1.标记策略优化:只有当一个位置可以根据当前已揭开坐标数  |\n");
    printf("|      字明确计算出是雷时,标记该位置才会减少查询到的当前雷数|\n");
    printf("|    2.游戏体验优化:初始坐标及其至少周围的8个坐标都无雷分布 |\n");
    printf("+-----------------------------------------------------------+\033[0m\n");
}

void clearScreen()
{
    printf("\033[2J\033[H"); // 转义符清屏可跨平台
}

void showTimeElapsed()
{
    // 获取当前时间
    time_t now = time(NULL);
    // 计算游戏开始时间到当前时间的差值
    double elapsed = difftime(now, gameStartTime);
    // 将差值转换为小时、分钟、秒
    int hours = (int)(elapsed / 3600);
    int minutes = (int)((elapsed - (hours * 3600)) / 60);
    int seconds = (int)(elapsed - (hours * 3600) - (minutes * 60));
    // 打印游戏开始时间到当前时间的差值
    printf("已进行游戏时间：%02d:%02d:%02d\n", hours, minutes, seconds);
}

void startGame(int difficulty)
{
    // 记录游戏开始时间
    gameStartTime = time(NULL);
    // 初始化游戏配置
    GameConfig config;
    // 初始化游戏状态
    GameState gameState;

    // 根据难度级别设置游戏配置
    switch (difficulty)
    {
    case 1:
        config.rows = 9;
        config.cols = 9;
        config.mineCount = EASY_COUNT;
        break;
    case 2:
        config.rows = 16;
        config.cols = 16;
        config.mineCount = MED_COUNT;
        break;
    case 3:
        config.rows = 16;
        config.cols = 30;
        config.mineCount = HARD_COUNT;
        break;
    case 4:
        // 调用自定义游戏函数
        customizeGame();
        return;
    case 0:
        // 退出游戏
        printf("退出游戏...\n");
        exit(0);
        return;
    default:
        // 无效的难度级别
        printf("无效的难度级别。请重新选择。\n");
        return;
    }

    // 计算行数和列数
    int rows = config.rows + 2;
    int cols = config.cols + 2;

    // 设置随机数种子
    srand((unsigned int)time(NULL));
    // 初始化游戏状态
    gameState.rows = rows;
    gameState.cols = cols;
    gameState.mineCount = config.mineCount;
    gameState.remainingCells = (rows - 2) * (cols - 2);
    gameState.win = 0;
    gameState.steppedOnMine = false;
    gameState.flagMode = false;
    gameState.tipMode = false;
    gameState.returnedToMainMenu = false;
    gameState.flaggedMines = 0;

    // 初始化游戏棋盘
    initBoard(gameState.show, '*', rows, cols);
    // 显示棋盘
    displayBoard(&gameState);
    // 初始化输入变量
    bool validInput = false;
    // 循环获取输入坐标
    while (!validInput)
    {
        // 提示用户输入坐标
        printf("请输入初始位置坐标（行 列）:");
        // 读取用户输入
        fgets(input, sizeof(input), stdin);
        // 解析用户输入
        if (sscanf(input, "%d %d", &config.startX, &config.startY) == 2)
        {
            // 判断输入坐标是否有效
            if (config.startX <= 0 || config.startY <= 0 || config.startX >= rows || config.startY >= cols)
            {
                // 输入坐标无效
                printf("无效的坐标，请输入有效的坐标！\n");
            }
            else
            {
                // 输入坐标有效
                validInput = true;
            }
        }
        else
        {
            // 输入格式无效
            printf("请输入有效的数字坐标格式！\n");
        }
    }
    // 初始化游戏棋盘
    initBoard(gameState.mine, '0', rows, cols);
    // 设置游戏起始坐标
    gameState.startX = config.startX;
    gameState.startY = config.startY;
    // 设置地雷
    setMines(&gameState);
    // 扩展空格
    expandEmptyCells(&gameState, config.startX, config.startY);
    // 显示棋盘
    displayBoard(&gameState);
    // 查找地雷
    findMines(&gameState);
}

void gameLoop()
{
    // 定义一个变量用于存储用户输入
    int choice;
    // 获取当前时间
    time_t currentTime = time(NULL);
    GameState gameState;
    // 初始化返回主菜单标志位
    gameState.returnedToMainMenu = false;
    // 清屏
    clearScreen();
    // 打印菜单
    printMenu();
    // 定义一个变量用于存储结束符
    char *end;
    // 使用do-while循环，当用户输入有效时，循环继续
    do
    {
        // 清空缓冲区
        fflush(stdin);
        // 定义一个数组用于存储用户输入
        char input[100];
        // 获取用户输入
        fgets(input, sizeof(input), stdin);
        // 将用户输入转换为数字
        choice = strtol(input, &end, 10);
        if (end == input || *end != '\n')
        {
            printf("无效的选择，请选择有效的选项。\n");
            continue;
        }
        // 根据用户输入，执行不同的操作
        switch (choice)
        {
        case 1:
            printf("你选择了初级难度。\n");
            startGame(1);
            break;
        case 2:
            printf("你选择了中级难度。\n");
            startGame(2);
            break;
        case 3:
            printf("你选择了高级难度。\n");
            startGame(3);
            break;
        case 4:
            printf("你选择了自定义游戏。\n");
            startGame(4);
            break;
        case 5:
            printHelp();
            break;
        case 0:
            printf("退出游戏...\n");
            exit(0);
            return;
        default:
            printf("无效的选择，请选择有效的选项。\n");
            break;
        }
    } while (true);
}

int main()
{
    gameLoop();
    return 0;
}