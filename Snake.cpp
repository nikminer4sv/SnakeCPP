#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <vector>
#include <string>
#include <thread>
#include <time.h>

using namespace std;

void spawnApple();
void draw();
void input();
void setup();
void tailCreating(int x, int y);

const int NO_TYPE = 0;
const int HEAD_TYPE = 1;
const int BODY_TYPE = 2;
const int APPLE_TYPE = 3;
const int DEFAULT_NUMBER_OF_SNAKE_ELEMENTS = 5;

const string TAIL_SYMBOL = "\x1b[32m█\x1b[0m"/*"▢█"*/;
const string HEAD_SYMBOL = "█"/*"\x1b[32m█\x1b[0m"*//*"▣"*/;
const string BOTTOM_BORDER_SYMBOL = "\x1b[37m█\x1b[0m"/*"▀"*/;
const string TOP_BORDER_SYMBOL = "\x1b[37m█\x1b[0m"/*"▄"*/;
const string DEFAULT_BORDER_SYMBOL = "\x1b[37m█\x1b[0m";
const string APPLE_SYMBOL = "\x1b[31m█\x1b[0m";

int width;
int height;
int score;
int isFinished;
int deltaTime;

bool isMoveWasMaked = true;

enum Destinations {
	LEFT,
	UP,
	RIGHT,
	DOWN
};

class Point {
public:

	int x, y;

	Point(int x, int y) {
		this->x = x;
		this->y = y;
	}

	Point() {}

};

class Cell {
public:

	int type;
	Point coords;

	Cell(int type, Point coords) {
		this->type = type;
		this->coords = coords;
	}

	Cell() {}

};

Cell currentApple = Cell(3, Point(0,0));

class Snake {
public:

	Destinations destination;
	vector<Cell> snakeElements;
	Cell head;
	int length;

	Snake() {

		int headX = width/2;
		int headY = height/2;
		destination = LEFT;
		length = DEFAULT_NUMBER_OF_SNAKE_ELEMENTS;
		snakeElements = vector<Cell>(DEFAULT_NUMBER_OF_SNAKE_ELEMENTS);
		Point coords = Point(headX, headY);
		head = Cell(HEAD_TYPE, coords);

		for (int i = 0; i < DEFAULT_NUMBER_OF_SNAKE_ELEMENTS; i++) {
			snakeElements[i] = Cell(BODY_TYPE, Point(headX+i+1, headY));
		}

	}

	Snake(int startLength) {

		int headX = width/2-1;
		int headY = height/2;
		destination = LEFT;
		length = startLength;
		snakeElements = vector<Cell>(startLength);
		Point coords = Point(headX, headY);
		head = Cell(HEAD_TYPE, coords);

		for (int i = 0; i < startLength; i++) {
			snakeElements[i] = Cell(BODY_TYPE, Point(headX+i+1, headY));
		}

	}

	Snake(int startLength, int x, int y) {}

	void ShowInfo() {
		cout << "Info: " << head.coords.x << " " << head.coords.y << " " << head.type << endl;
	}

	void move() {

		int temp_x = head.coords.x;
		int temp_y = head.coords.y;

		if (destination == LEFT) {
			head.coords.x -= 1;
		}

		if (destination == UP) {
			head.coords.y -= 1;
		}

		if (destination == RIGHT) {
			head.coords.x += 1;
		}

		if (destination == DOWN) {
			head.coords.y += 1;
		}

		if (head.coords.x < 1 || head.coords.x > width-1 || head.coords.y > height-1 || head.coords.y < 0) {
			isFinished = true;
			return;
		}

		for (int i = 0; i < length; i++) {
			if (head.coords.x == snakeElements[i].coords.x && head.coords.y == snakeElements[i].coords.y) {
				isFinished = true;
				return;
			}
		}

		if (head.coords.x == currentApple.coords.x && head.coords.y == currentApple.coords.y) {

			int lastElementX = snakeElements[length-1].coords.x;
			int lastElementY = snakeElements[length-1].coords.y;
			int notLastElementX = snakeElements[length-2].coords.x;
			int notLastElementY = snakeElements[length-2].coords.y;
			int newElementX;
			int newElementY;

			if (lastElementX == notLastElementX) {
				newElementX = lastElementX;
				if (lastElementY > notLastElementY) 
					newElementY = lastElementY+1;
				else
					newElementY = lastElementY-1; 
			} else {
				newElementY = lastElementY;
				if (lastElementX > notLastElementX) 
					newElementX = lastElementY+1;
				else
					newElementX = lastElementY-1; 
			}

			tailCreating(newElementX, newElementY);

			spawnApple();
			score += 1;
		}

		for (int i = 0; i < length; i++) {
			int tx = snakeElements[i].coords.x;
			int ty = snakeElements[i].coords.y;
			snakeElements[i].coords.x = temp_x;
			snakeElements[i].coords.y = temp_y;
			temp_x = tx;
			temp_y = ty;
		}

		isMoveWasMaked = true;

	};

	
};


Snake snake;

void tailCreating(int x, int y) {
	snake.length++;
	snake.snakeElements.push_back(Cell(2,Point(x,y)));
}

void draw() {

	system("clear");

	for (int i = 0; i < width+1; i++)
		cout << TOP_BORDER_SYMBOL;
	cout << endl;

	for (int i = 0; i < height; i++) {
		for (int j = 0;j < width+1; j++) {
			if (j == 0 || j == width)
				cout << DEFAULT_BORDER_SYMBOL;
			else {
				if (snake.head.coords.y == i && snake.head.coords.x == j)
					cout << HEAD_SYMBOL;
				else if (currentApple.coords.y == i && currentApple.coords.x == j)
					cout << APPLE_SYMBOL;
				else {
					bool flag = true;
					for (int z = 0; z < snake.length; z++) {
						if (snake.snakeElements[z].coords.y == i && snake.snakeElements[z].coords.x == j) {
							cout << TAIL_SYMBOL;
							flag = false;
							break;
						}

					}

					if (flag)
						cout << "  ";
				}
			}
		}
		cout << endl;
	}

	for (int i = 0; i < width+1; i++)
		cout << BOTTOM_BORDER_SYMBOL;
	cout << endl;

}

void input() {

	while (!isFinished) {

		struct termios oldt, newt;
    	int ch;
    	tcgetattr( STDIN_FILENO, &oldt );
    	newt = oldt;
    	newt.c_lflag &= ~( ICANON | ECHO );
    	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    	ch = getchar();
    	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );

    	if (isMoveWasMaked == true) {
    		if (ch == 119 || ch == 279165) {
    			if (snake.destination != DOWN) {
    				snake.destination = UP;
    				isMoveWasMaked = false;
    			}
    		}
    		else if (ch == 97 || ch == 279168) {
    			if (snake.destination != RIGHT){
    				snake.destination = LEFT;
    				isMoveWasMaked = false;
    			}
    		}
    		else if (ch == 115 || ch == 279166) {
    			if (snake.destination != UP) {
    				snake.destination = DOWN;
    				isMoveWasMaked = false;
    			}
    		}
    		else if (ch == 100 || ch == 279157) {
    			if (snake.destination != LEFT) {
    				snake.destination = RIGHT;
    				isMoveWasMaked = false;
    			}
    		}
    	}

	}

}

void spawnApple() {

	while (true) {

		int x = rand() % width;
		int y = rand() % height;

		if (snake.head.coords.x == x && snake.head.coords.y == y)
			continue;

		for (int i = 0; i < snake.length; i++) {
			if (snake.snakeElements[i].coords.x == x && snake.snakeElements[i].coords.y == y) 
				continue;
		}

		currentApple.coords.x = x;
		currentApple.coords.y = y;

		break;


	}

}

void setup() {

	width = 20;
	height = 15;
	score = 0;
	isFinished = false;
	deltaTime = 2;
	snake = Snake(3);
	spawnApple();

	cout << "\e[8;" << height+4<< ";" << width*2+2 <<";t";

}

int main() {

	srand(time(NULL));
	system("clear");
	setlocale(LC_ALL, "");

	setup();

	draw();
	usleep(1000000/deltaTime);

	thread inputThread(input);
	inputThread.detach();

	while (!isFinished) {
		snake.move();
		if (isFinished)
			break;
		draw();
		snake.ShowInfo();
		usleep(1000000/deltaTime);
	}	

	system("clear");
	cout << "\x1b[31mКонец игры!\x1b[0m" << endl;
	printf("\x1b[32mВаш счет: %d\x1b[1m", score);
	cout << endl;


}
