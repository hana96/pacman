

// OpenCV.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include "KinectSample.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	try {
		KinectSample kinect;
		kinect.initialize();
		kinect.run();
	}
	catch (exception &ex) {
		cout << ex.what() << endl;
	}
	return 0;
}
