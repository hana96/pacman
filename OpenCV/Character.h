#pragma once
#include <iostream>
#include <sstream>
#include <opencv2/opencv.hpp>
#include "Field.h"
class Character
{
#define EAST	0
#define SOUTH	1
#define WEST	2
#define NORTH	3

protected:
	int x;
	int y;
	int xold;
	int yold;
	int dir;
	int nextDir;
	bool alive;
	vector<vector<int>> allDirs;

	void initialize() {
		allDirs = vector<vector<int>> {
			{ 0, 1 },
			{ 1, 0 },
			{ 0, -1},
			{-1, 0 }
		};
	}
public:
	Character() :x(0), y(0), dir(0), nextDir(-1), alive(true),xold(-1),yold(-1){
		initialize();
	}
	Character(int ny, int nx, int d){
		y = ny; x = nx; dir = d; nextDir = -1; alive = true;
		initialize();
	}
	~Character(){}
	void setPos(int ny, int nx){ y = ny; x = nx; }
	void setDir(int d) { dir = d; }
	void setNDir(int d){ nextDir = d; }
	void setAlive(bool l){ alive = l; }
	bool canRMove(Field &f, int d){
		return canRMove(f, allDirs[d][0], allDirs[d][1]);
	}
	bool canRMove(Field &f, int dy, int dx){
		int ny = y + dy;
		int nx = x + dx;
		return canMove(f, ny, nx);
	}
	bool canMove(Field &f, int y, int x){
		return f.in(y, x) && !f.isWall(y, x);
	}
	bool tryChgDir(Field &field){
		if (nextDir < 0 || nextDir >= allDirs.size() || !canRMove(field, nextDir)) return false;
		dir = nextDir;
		nextDir = -1;
		return true;
	}
	bool isAte(Character enemy){
		return (x == enemy.x && y == enemy.y)||((x==enemy.xold && y == enemy.yold)&&(xold == enemy.x && yold == enemy.y));
	}
	void step(Field &field){
		tryChgDir(field);
		move(field);
	}
	void walked(Field &f){ f.set(y, x,WALKED); }
	bool move(Field &f){
		if (!canRMove(f, dir))return false;
		xold = x;
		yold = y;
		y += allDirs[dir][0];
		x += allDirs[dir][1];
		return true;
	}
	void turnEast() { setNDir(EAST); }
	void turnSouth() { setNDir(SOUTH); }
	void turnWest() { setNDir(WEST); }
	void turnNorth() { setNDir(NORTH); }

	void turnRight() { setNDir((dir + 1) % allDirs.size()); }
	void turn180() { setNDir((dir + 2) % allDirs.size()); }
	void turnLeft() { setNDir( (dir + 3) % allDirs.size()); }

	string toString() {
		stringstream ss;
		ss << "x = " << x << " y = " << y << "dir = " << dir << "nextDir = " << dir << endl;
		return ss.str();
	}

	void draw(cv::Mat &image, int bHeight, int bWidth){
		cv::Rect rect(x*bWidth, y*bHeight, bWidth, bHeight);
		cv::rectangle(image, rect, cv::Scalar(0, 0, 255), -1);
	}
};

