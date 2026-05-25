#include <iostream>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

// Grid size
const int width = 30;
const int height = 20;

// Game variables
int x, y, foodX, foodY, score, level;
bool gameOver;

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
Direction dir;

vector<pair<int, int>> snake;

// Colors
#define GREEN 10
#define RED 12
#define YELLOW 14
#define WHITE 15
#define CYAN 11

// Set text color
void SetColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// Hide cursor
void HideCursor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

// Move cursor
void SetCursorPosition(int x, int y) {
    COORD coord = { (short)x, (short)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Setup game
void Setup() {
    srand((unsigned)time(0));

    gameOver = false;
    dir = STOP;

    x = width / 2;
    y = height / 2;

    score = 0;
    level = 1;

    snake.clear();
    snake.push_back({ x, y });

    // Generate food at valid position (not on snake)
    bool validPosition;
    do {
        validPosition = true;
        foodX = rand() % width;
        foodY = rand() % height;

        for (const auto& segment : snake) {
            if (segment.first == foodX && segment.second == foodY) {
                validPosition = false;
                break;
            }
        }
    } while (!validPosition);

    HideCursor();
}

// Draw game
void Draw() {
    SetCursorPosition(0, 0);

    // Top wall
    SetColor(CYAN);
    for (int i = 0; i < width + 2; i++)
        cout << char(219);
    cout << "\n";

    // Game board
    for (int i = 0; i < height; i++) {
        SetColor(CYAN);
        cout << char(219);

        for (int j = 0; j < width; j++) {
            bool printed = false;

            // Snake head
            if (i == y && j == x) {
                SetColor(GREEN);
                cout << "O";
                printed = true;
            }
            // Food (Apple emoji)
            else if (i == foodY && j == foodX) {
                SetColor(RED);
                cout << "🍎";
                printed = true;
            }
            // Snake body
            else {
                for (size_t k = 1; k < snake.size(); k++) {
                    if (snake[k].first == j && snake[k].second == i) {
                        SetColor(GREEN);
                        cout << "o";
                        printed = true;
                        break;
                    }
                }

                // Empty space
                if (!printed)
                    cout << " ";
            }
        }

        SetColor(CYAN);
        cout << char(219);
        cout << "\n";
    }

    // Bottom wall
    SetColor(CYAN);
    for (int i = 0; i < width + 2; i++)
        cout << char(219);
    cout << "\n";

    // Score board
    SetColor(YELLOW);
    cout << "Score : " << score << "    Level : " << level << "\n";

    SetColor(WHITE);
    cout << "Controls: W=Up A=Left S=Down D=Right X=Exit\n";
}

// Input
void Input() {
    if (_kbhit()) {
        char key = _getch();

        switch (tolower(key)) {
        case 'a':
            if (dir != RIGHT)
                dir = LEFT;
            break;
        case 'd':
            if (dir != LEFT)
                dir = RIGHT;
            break;
        case 'w':
            if (dir != DOWN)
                dir = UP;
            break;
        case 's':
            if (dir != UP)
                dir = DOWN;
            break;
        case 'x':
            gameOver = true;
            break;
        }
    }
}

// Game logic
void Logic() {
    if (dir == STOP)
        return;

    // Store new head position
    int newX = x;
    int newY = y;

    // Calculate new head position
    switch (dir) {
    case LEFT:
        newX--;
        break;
    case RIGHT:
        newX++;
        break;
    case UP:
        newY--;
        break;
    case DOWN:
        newY++;
        break;
    }

    // Check if food is eaten
    bool ateFood = (newX == foodX && newY == foodY);

    // Move snake body
    if (ateFood) {
        // Add new head and keep tail (snake grows)
        snake.insert(snake.begin(), {newX, newY});
        score += 10;

        // Update level (every 50 points)
        level = 1 + (score / 50);

        // Generate new food at valid position
        bool validPosition;
        do {
            validPosition = true;
            foodX = rand() % width;
            foodY = rand() % height;

            for (const auto& segment : snake) {
                if (segment.first == foodX && segment.second == foodY) {
                    validPosition = false;
                    break;
                }
            }
        } while (!validPosition);
    } else {
        // Normal movement - add new head and remove tail
        snake.insert(snake.begin(), {newX, newY});
        snake.pop_back();
    }

    // Update head position
    x = newX;
    y = newY;

    // Wall collision
    if (x < 0 || x >= width || y < 0 || y >= height) {
        gameOver = true;
        return;
    }

    // Self collision (skip head which is at index 0)
    for (size_t i = 1; i < snake.size(); i++) {
        if (snake[i].first == x && snake[i].second == y) {
            gameOver = true;
            return;
        }
    }
}

// Game speed
int GetSpeed() {
    int speed = 150 - (level - 1) * 15;
    if (speed < 50)
        speed = 50;
    return speed;
}

// Main function
int main() {
    // Enable UTF-8 for emoji support
    SetConsoleOutputCP(CP_UTF8);

    // Enable console for better display
    system("title Snake Game");
    system("mode con cols=80 lines=30");

    Setup();

    while (!gameOver) {
        Draw();
        Input();
        Logic();
        Sleep(GetSpeed());
    }

    // Game over screen
    system("cls");

    SetColor(RED);
    cout << "\n\n";
    cout << "=================================\n";
    cout << "          GAME OVER             \n";
    cout << "=================================\n";

    SetColor(YELLOW);
    cout << "\nFinal Score   : " << score << "\n";
    cout << "Level Reached : " << level << "\n";

    SetColor(WHITE);
    cout << "\n=================================\n";
    cout << "Thanks For Playing!\n\n";

    system("pause");

    return 0;
}
