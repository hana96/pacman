#pragma once
#include <opencv2/opencv.hpp>
#include"Field.h"
#include"Player.h"
#include"Character.h"
#include"EnemyCharacter.h"
using namespace std;
class Pacman
{
#define	BL	32
private:
	Field field;
	Player pac;
	vector<EnemyCharacter> enemy;
	cv::Mat image;
public:
	Pacman(){}
	~Pacman(){}
	void start() {
		int map[] = {
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1,
			1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1,
			1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1,
			1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
			1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
			1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
			1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
			1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
			1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		};
		field = Field(16, 16, map);
		pac = Player(1, 1, 0);
		int n_enemycharacter = 0;
		while (n_enemycharacter < 3){
			EnemyCharacter ec;
			int ey = rand() % field.height();
			int ex = rand() % field.width();
			int ed = rand() % 4;
			if (field.isWall(ey, ex)) continue;
			ec.setPos(ey, ex); ec.setDir(ed); enemy.push_back(ec);
			n_enemycharacter++;
		}
		image = cv::Mat(field.height() * BL, field.width()*BL, CV_8UC3, cv::Scalar(0, 0, 0));
	}
	bool getState() {
		for (int i = 0; i < enemy.size(); i++){
			if (pac.isAte(enemy[i])) {
				draw();
				return false;
			}
		}
		return true;
	}
	void step() {
		pac.step(field);
		pac.walked(field);
		for (int i = 0; i < enemy.size(); i++) enemy[i].step(field);
	}
	void draw() {
		field.draw(image, BL, BL);
		pac.draw(image, BL, BL);
		for (int i = 0; i < enemy.size(); i++) enemy[i].draw(image, BL, BL);
		cv::imshow("Pacman", image);
	}
	bool gameClear(){return field.gameClear();}
	void turnEast() { pac.turnEast(); }
	void turnSouth() { pac.turnSouth(); }
	void turnWest() { pac.turnWest(); }
	void turnNorth() { pac.turnNorth(); }
};

