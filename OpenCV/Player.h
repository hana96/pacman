#pragma once
#include "Character.h"
class Player :
	public Character
	
{
private:
	vector<cv::Mat> pac;
	int count;
public:
	Player(){ init(); }
	Player(int x, int y, int dr) :Character(x, y, dr) { init(); }
	void init(){
		pac.push_back(cv::imread("u://Visual Studio 2013//Projects//Pro//pacman0.png"));
		pac.push_back(cv::imread("u://Visual Studio 2013//Projects//Pro//pacman1.png"));
		pac.push_back(cv::imread("u://Visual Studio 2013//Projects//Pro//pacman2.png"));
		count = 0;
	}
	~Player(){}
	void draw(cv::Mat &image, int bHeight, int bWidth){
		count++;
		cv::Mat window;
		if (count % 4){ window = image(cv::Rect(x*bWidth, y*bHeight, pac[0].cols, pac[0].rows)); pac[0].copyTo(window); }
		else if (count % 4 == 1||count % 4 == 3){ window = image(cv::Rect(x*bWidth, y*bHeight, pac[1].cols, pac[1].rows)); pac[1].copyTo(window); }
		else{ window = image(cv::Rect(x*bWidth, y*bHeight, pac[2].cols, pac[2].rows)); pac[2].copyTo(window); }
	}
};

