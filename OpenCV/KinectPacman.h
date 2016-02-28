#pragma once
#include "KinectControl.h"
class KinectPacman :
	public KinectControl
{
#define R1LENGTH	60
#define R2LENGTH	200
#define WIDTH	200
#define HEIGHT	1000

private:
	pair<cv::Vec3f, int> hip;
	cv::Vec3f hipf;
	cv::Mat image;
	int ix;
	int iz;
	int dx;
	int dz;
	bool debug;
public:
	KinectPacman() :debug(false), hipf(-1, -1, -100){ init();  }
	~KinectPacman(){}
	void setDebug() { debug = true; }
	void init(){ 
		dx = R1LENGTH;
		dz = R2LENGTH;
		image = cv::Mat(200, 200, CV_8UC3, cv::Scalar(0, 0, 0)); 
	}
	bool tracked(){
		if (skeleton.size() == 0) return false;
		hip = skeleton[0][NUI_SKELETON_POSITION_HIP_CENTER];
		if (hip.second == NUI_SKELETON_POSITION_NOT_TRACKED) return false;
		if (hipf[2] < 0) {
			hipf = hip.first;
			ix = hipf[0];
			iz = hipf[2];
		}
		return true;
	}
	bool rangeR(pair<cv::Vec3f, int> hip){
		double hr = hip.first[0] - hipf[0];
		cout << " hr=" << hr << endl;
		return hr < -R1LENGTH;
	}
	bool rangeB(pair<cv::Vec3f, int> hip) {
		double hb = hip.first[2]-hipf[2];
		cout << " hb=" << hb << endl;
		return hb > R2LENGTH;
	}
	bool rangeL(pair<cv::Vec3f, int> hip) {
		double hl = hip.first[0] - hipf[0];
		cout << " hl=" << hl << endl;
		return hl > R1LENGTH;
	}
	bool rangeF(pair<cv::Vec3f, int> hip){
		double hf = hip.first[2] - hipf[2];
		cout << " hf=" << hf << endl;
		return hf < -R2LENGTH;
	}
	void check() {
		if (!tracked()) return;
		cout << " hip=" << hip.first  << "  hipf=" <<hipf << endl;
	}
	bool moveR() {
		if (!tracked()) return false;
		return rangeR(hip);
	}
	bool moveB() {
		if (!tracked()) return false;
		return rangeB(hip);
	}
	bool moveL(){
		if (!tracked()) return false;
		return rangeL(hip);
	}
	bool moveF(){
		if (!tracked()) return false;
		return rangeF(hip);
	}
	void draw(){
		int sx = ix - 2*dx;
		int ex = ix + 2*dx;
		int sz = iz - 2*dz;
		int ez = iz + 2*dz;
		int x = ((hip.first[0] -ix)/(4*dx)+0.5)*image.cols;
		int z = ((hip.first[2] - iz)/(4*dz)+0.5)*image.rows;
		cv::Rect rect(0, 0, image.cols, image.rows);
		cv::rectangle(image, rect, cv::Scalar(0, 0, 0), -1);
		cv::circle(image, cv::Point(image.cols/2, image.rows/2), 10, cv::Scalar(0, 0, 200), -1); 
		cv::circle(image, cv::Point(x, z), 10, cv::Scalar(255, 165, 0), -1);
		cv::imshow("Place", image);
	}
};

