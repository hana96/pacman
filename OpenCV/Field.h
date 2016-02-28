#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <opencv2/opencv.hpp>
using namespace std;

class Field
{
#define EMPTY	0
#define WALL	1
#define WALKED	3

private:
	vector<vector<int>> field;
	cv::Mat block;
	void initialize(int h, int w){
		field.clear();
		field.resize(h);
		for (int i = 0; i < h; i++){
			field[i].resize(w);
			for (int j = 0; j < w; j++)
				field[i][j] = EMPTY;
		}
		block = cv::imread("u://Visual Studio 2013//Projects//Pro//pacblock.jpg");
	}

public:
	Field(){
		initialize(16, 16);
	}
	Field::Field(int h, int w){ initialize(h, w); }
	Field::Field(int h, int w, int *m){
		initialize(h, w);
		for (int i = 0; i < h*w; i++) field[i / w][i%w] = m[i];
	}
	~Field(){}
	int width() { return field[0].size(); }
	int height() { return field.size(); }
	int in(int y, int x) { return y >= 0 && y < height() && x >= 0 && x < width(); }
	int set(int y, int x, int val) {
		if (!in(y, x)){
			stringstream ss;
			ss << "out of range (" << y << "," << x << ")";
			throw runtime_error(ss.str().c_str());
		}
		int old = field[y][x];
		field[y][x] = val;
		return old;
	}
	int get(int y, int x){
		if (!in(y, x)) {
			stringstream ss;
			ss << "out of range (" << y << "," << x << ")";
			throw runtime_error(ss.str().c_str());
		}
		return field[y][x];
	}
	bool isEmpty(int y, int x){ return get(y, x) == EMPTY; }
	bool isWall(int y, int x){ return get(y, x) == WALL; }
	bool isWalked(int y, int x){ return get(y, x) == WALKED; }
	bool gameClear() {
		for (int i = 0; i < field.size(); i++){
			for (int j = 0; j < field[i].size(); j++){
				if (field[i][j] == EMPTY) return false;
			}
		}
		return true;
	}
	void clear(){
		for (int i = 0; i < field.size(); i++)
			for (int j = 0; j < field[i].size(); j++)
				field[i][j] = EMPTY;
	}
	void draw(cv::Mat &image, int bHeight, int bWidth){
		for (int y = 0; y < height(); y++){
			for (int x = 0; x < width(); x++){
				cv::Rect rect(x*bWidth, y*bHeight, bWidth, bHeight);
				cv::Scalar color(0, 0, 0);
				cv::rectangle(image, rect, color, -1);
				if (isWall(y, x)){
					cv::Mat window = image(cv::Rect(x*bWidth, y*bHeight, block.cols, block.rows));
					block.copyTo(window);
				}
				if (isWalked(y, x)){ color[0] = 180; color[1] = 180; color[2] = 180; cv::rectangle(image, rect, color, -1); }
			}
		}
	}
};

