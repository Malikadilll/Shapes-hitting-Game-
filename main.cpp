#include <graphics.h>
#include <conio.h>
#include <dos.h>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <fstream>
#include <iostream>
#include <string>

#define WIDTH 640
#define HEIGHT 480

#define PLAYER_WIDTH 50
#define PLAYER_HEIGHT 20
#define PLAYER_STEP 10

#define BULLET_WIDTH 5
#define BULLET_HEIGHT 10
#define BULLET_STEP 5

#define SCORE_INCREMENT 10

enum GameState { MENU, GAME, HIGHSCORE, EXIT };

class GameObject {
public:
    int x, y;
    int width, height;

    GameObject(int X, int Y, int w, int h){
    	x=X;
    	y=Y;
    	width=w;
    	height=h;
	}

    virtual void draw() = 0;
    virtual bool checkCollision(GameObject &other) = 0;
};

class Player : public GameObject {
public:
    Player(int x, int y) : GameObject(x, y, PLAYER_WIDTH, PLAYER_HEIGHT) {}

    void draw() override {
        setfillstyle(SOLID_FILL, WHITE);
        bar(x, y, x + width, y + height);
        setcolor(7);
        outtextxy(x + 10, y + 5, "Adil");
    }

    void moveLeft() {
        if (x > 0) {
            x -= PLAYER_STEP;
        }
    }

    void moveRight() {
        if (x < WIDTH - width) {
            x += PLAYER_STEP;
        }
    }

    bool checkCollision(GameObject &other) override {
        return !(x + width < other.x || x > other.x + other.width || y + height < other.y || y > other.y + other.height);
    }
};

class Bullet : public GameObject {
public:
    bool fired;

    Bullet() : GameObject(0, 0, BULLET_WIDTH, BULLET_HEIGHT), fired(false) {}

    void draw() override {
        if (fired) {
            setfillstyle(SOLID_FILL, GREEN);
            bar(x, y, x + width, y + height);
        }
    }

    void fire(int startX, int startY) {
        if (!fired) {
            x = startX;
            y = startY;
            fired = true;
        }
    }

    void move() {
        if (fired) {
            y -= BULLET_STEP;
            if (y < 0) {
                fired = false;
            }
        }
    }

    bool checkCollision(GameObject &other) override {
        return !(x + width < other.x || x > other.x + other.width || y + height < other.y || y > other.y + other.height);
    }
};

class Shape : public GameObject {
public:
    bool alive;

    Shape(int x, int y, int width, int height) : GameObject(x, y, width, height), alive(true) {}

    virtual void respawn() = 0;
};

class RectangleShape : public Shape {
public:
    RectangleShape(int x, int y) : Shape(x, y, 40, 20) {}

    void draw() override {
        if (alive) {
            setfillstyle(SOLID_FILL, RED);
            bar(x, y, x + width, y + height);
        }
    }

    void respawn() override {
        x = rand() % (WIDTH - width);
        y = rand() % (HEIGHT / 2);
        alive = true;
    }

    bool checkCollision(GameObject &other) override {
        return !(x + width < other.x || x > other.x + other.width || y + height < other.y || y > other.y + other.height);
    }
};

class CircleShape : public Shape {
public:
    CircleShape(int x, int y) : Shape(x, y, 20, 20) {}

    void draw() override {
        if (alive) {
            setfillstyle(SOLID_FILL, BLUE);
            fillellipse(x + width / 2, y + height / 2, width / 2, height / 2);
        }
    }

    void respawn() override {
        x = rand() % (WIDTH - width);
        y = rand() % (HEIGHT / 2);
        alive = true;
    }

    bool checkCollision(GameObject &other) override {
        int dx = (x + width / 2) - (other.x + other.width / 2);
        int dy = (y + height / 2) - (other.y + other.height / 2);
        int distance = sqrt(dx * dx + dy * dy);
        return distance < (width / 2 + other.width / 2);
    }
};

class TriangleShape : public Shape {
public:
    TriangleShape(int x, int y) : Shape(x, y, 30, 30) {}

    void draw() override {
        if (alive) {
            setfillstyle(SOLID_FILL, YELLOW);
            int poly[6] = {x, y + height, x + width / 2, y, x + width, y + height};
            fillpoly(3, poly);
        }
    }

    void respawn() override {
        x = rand() % (WIDTH - width);
        y = rand() % (HEIGHT / 2);
        alive = true;
    }

    bool checkCollision(GameObject &other) override {
        return !(x + width < other.x || x > other.x + other.width || y + height < other.y || y > other.y + other.height);
    }
};

int readHighScore() {
    std::ifstream file("highscore.txt");
    int highScore = 0;
    if (file.is_open()) {
        file >> highScore;
        file.close();
    }
    return highScore;
}

void writeHighScore(int highScore) {
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << highScore;
        file.close();
    }
}

GameState displayMenu() {
    cleardevice();
    setcolor(WHITE);

    outtextxy(WIDTH / 2 - 50, HEIGHT / 2 - 30, "1. Start Game");
    outtextxy(WIDTH / 2 - 50, HEIGHT / 2 - 10, "2. View High Score");
    outtextxy(WIDTH / 2 - 50, HEIGHT / 2 + 10, "3. Exit");

    while (true) {
        if (kbhit()) {
            char choice = getch();
            switch (choice) {
                case '1':
                    return GAME;
                case '2':
                    return HIGHSCORE;
                case '3':
                    return EXIT;
                default:
                    break;
            }
        }
    }
}

void displayHighScore(int highScore) {
    cleardevice();
    setcolor(WHITE);

    char highScoreText[30];
    sprintf(highScoreText, "High Score: %d", highScore);
    outtextxy(WIDTH / 2 - 70, HEIGHT / 2, highScoreText);
    outtextxy(WIDTH / 2 - 70, HEIGHT / 2 + 20, "Press any key to return");

    getch();
}

int main() {
    int gd = DETECT, gm;
    initgraph(&gd, &gm, "");

    srand(time(0)); 

    Player player(WIDTH / 2 - PLAYER_WIDTH / 2, HEIGHT - PLAYER_HEIGHT - 10);
    Shape* shapes[3]; 
    shapes[0] = new RectangleShape(rand() % (WIDTH - 40), rand() % (HEIGHT / 2));
    shapes[1] = new CircleShape(rand() % (WIDTH - 20), rand() % (HEIGHT / 2));
    shapes[2] = new TriangleShape(rand() % (WIDTH - 30), rand() % (HEIGHT / 2));
    Bullet bullet;

    int highScore = readHighScore();
    GameState state = MENU;

    while (state != EXIT) {
        if (state == MENU) {
            state = displayMenu();
        } else if (state == HIGHSCORE) {
            displayHighScore(highScore);
            state = MENU;
        } else if (state == GAME) {
            bool game_over = false;
            int score = 0;

            while (!game_over) {
                cleardevice();

                player.draw();
                for (int i = 0; i < 3; ++i) {
                    shapes[i]->draw();
                }
                bullet.draw();

                
                setcolor(WHITE);
                char scoreText[20];
                sprintf(scoreText, "Score: %d", score);
                outtextxy(10, 10, scoreText);

                char highScoreText[30];
                sprintf(highScoreText, "High Score: %d", highScore);
                outtextxy(10, 30, highScoreText);

                if (kbhit()) {
                    char ch = getch();
                    if (ch == 27) {
                        game_over = true;
                        state = MENU;
                    }
                    if (ch == 75) {
                        player.moveLeft();
                    }
                    if (ch == 77) {
                        player.moveRight();
                    }
                    if (ch == 32) {
                        bullet.fire(player.x + player.width / 2 - BULLET_WIDTH / 2, player.y - BULLET_HEIGHT);
                    }
                }

                bullet.move();

                for (int i = 0; i < 3; ++i) {
                    if (bullet.fired && shapes[i]->alive && bullet.checkCollision(*shapes[i])) {
                        shapes[i]->alive = false;
                        bullet.fired = false;
                        shapes[i]->respawn(); 
                        score += SCORE_INCREMENT;
                    }
                }

                delay(30);
            }

            
            if (score > highScore) {
                highScore = score;
                writeHighScore(highScore);
            }

            
            for (int i = 0; i < 3; ++i) {
                delete shapes[i];
            }

            state = MENU;
        }
    }

    closegraph();
    return 0;
}


