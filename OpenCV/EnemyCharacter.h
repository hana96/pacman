#pragma once
#include "Character.h"
class EnemyCharacter :
	public Character
{
	vector<vector<int>> policy;
	cv::Mat enemy;
public:
	EnemyCharacter(){
		policy = vector < vector<int>>{
			{ 0, 1, 3, 2 },
			{ 1, 0, 3, 2 },
			{ 3, 0, 1, 2 },
		};
		init();
	}
	~EnemyCharacter(){}
	void init(){ enemy = cv::imread("u://Visual Studio 2013//Projects//Pro//enemy1.png"); }
	void step(Field &f){
		int r = rand() % policy.size();
		for (int i = 0; i < policy[r].size(); i++){
			int d = (dir + policy[r][i]) % allDirs.size();
			if (canRMove(f, d)){
				dir = d;
				move(f);
				break;
			}
		}
	}
	void draw(cv::Mat &image, int bHeight, int bWidth){
		cv::Mat window = image(cv::Rect(x*bWidth, y*bHeight, enemy.cols, enemy.rows));
		enemy.copyTo(window);
	}
};

