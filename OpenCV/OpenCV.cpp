

// OpenCV.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include "Pacman.h"
#include "KinectSample.h"
#include "KinectPacman.h"

using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
	try {

		KinectPacman kinect;
		kinect.start();


		Pacman pacman;
		pacman.start();
		DWORD t1 = GetTickCount();
		int cmd = 0;

		while (kinect.next()){

			if (!pacman.getState()||pacman.gameClear()) break;
			
			kinect.setRgbImage();
			kinect.setDepthImage();
			kinect.setSkeleton();

			kinect.check();

			if (kinect.skeleton.size() > 0) {
				kinect.drawSegment(kinect.rgbImage, kinect.skeleton[0]);
				kinect.draw();
			}

			cv::imshow("Kinect", kinect.rgbImage);
			


			if (kinect.moveF()) cmd = 1;
			else if (kinect.moveR()) cmd = 2;
			else if (kinect.moveB()) cmd = 3;
			else if (kinect.moveL()) cmd = 4;


			switch (cmd){
			case 1:pacman.turnNorth(); break;//上
			case 2:pacman.turnWest(); break;//右
			case 3:pacman.turnSouth(); break;//下
			case 4:pacman.turnEast(); break;//左
			}
			cmd = 0;

			int key = cv::waitKey(1);
			if (key == 27) break;
			switch (key){
			case 'h':pacman.turnWest(); break;//右
			case 'j':pacman.turnSouth(); break;//下
			case 'k':pacman.turnNorth(); break;//上
			case 'l':pacman.turnEast(); break;//左
			}

			DWORD t2 = GetTickCount();
			if (t2 - t1 > 300){
				t1 = t2;
				pacman.step();
				pacman.draw();
			}

		}

		cv::destroyAllWindows();
		if (!pacman.getState()){
			cv::Mat gameo;
			gameo = cv::imread("u://Visual Studio 2013//Projects//Pro//GameOver.jpg");
			cv::namedWindow("src");
			cv::imshow("src", gameo);
			cv::waitKey(5000);
			cv::destroyAllWindows();
			return 0;
		}
		else if(pacman.gameClear()){
			cv::Mat gameo;
			gameo = cv::imread("u://Visual Studio 2013//Projects//Pro//GameClear.jpg");
			cv::namedWindow("src");
			cv::imshow("src", gameo);
			cv::waitKey(5000);
			cv::destroyAllWindows();
			return 0;
		}

	}
	catch (exception &ex) {
		cout << ex.what() << endl;
		string s;
		cin >> s;
	}
	return 0;
}