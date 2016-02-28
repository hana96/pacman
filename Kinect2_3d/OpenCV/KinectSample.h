#pragma once
#include <opencv2/opencv.hpp>

#include <iostream>
#include <sstream>
#include <Windows.h>
#include <NuiApi.h>

using namespace std;

#define ERROR_CHECK(ret) \
if (ret != S_OK) {\
	stringstream ss; \
	ss << "failed " #ret " " << hex << ret << endl; \
	throw runtime_error(ss.str().c_str()); \
}
#define IMAGE_RESOLUTION NUI_IMAGE_RESOLUTION_640x480

class KinectSample
{
private:
	INuiSensor *kinect;
	HANDLE imageStreamHandle,depthStreamHandle,streamEvent;
	DWORD width, height;
public:
	KinectSample();
	~KinectSample();
	void initialize() {
		createInstance();
		ERROR_CHECK(kinect->NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR|NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX));
		ERROR_CHECK(kinect->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, IMAGE_RESOLUTION, 0, 2, 0, &imageStreamHandle));
		ERROR_CHECK(kinect->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, IMAGE_RESOLUTION, 0, 2, 0, &depthStreamHandle));
		streamEvent = CreateEvent(0, TRUE, FALSE, 0);
		ERROR_CHECK(kinect->NuiSetFrameEndEvent(streamEvent,0));
		NuiImageResolutionToSize(IMAGE_RESOLUTION, width, height);
	}
	void createInstance() {
		int count = 0;
		ERROR_CHECK(NuiGetSensorCount(&count));
		cout << count << endl;
		if (count == 0) throw runtime_error("Connect Kinect");
		ERROR_CHECK(NuiCreateSensorByIndex(0, &kinect));
		if (kinect->NuiStatus() != S_OK) throw runtime_error("Cannot use Kinect");
	}
	void run() {
		cv::Mat image(height,width,CV_8UC4);
		cv::Mat dimage(height, width, CV_8UC3);
		cv::namedWindow("Kinect Sample");
		while (1) {
			DWORD result = WaitForSingleObject(streamEvent, INFINITE);
			ResetEvent(streamEvent);
			drawRgbImage(image);
			drawDepthImage(dimage);
			cv::imshow("Kinect Sample", image);
			cv::imshow("depth", dimage);
			int key = cv::waitKey(10);
			if (key == 27) break;
		}
		cv::destroyAllWindows();
	}
	void drawRgbImage(cv::Mat &image) {
		NUI_IMAGE_FRAME imageFrame = { 0 };
		ERROR_CHECK(kinect->NuiImageStreamGetNextFrame(imageStreamHandle, 0, &imageFrame));
		NUI_LOCKED_RECT colorData;
		imageFrame.pFrameTexture->LockRect(0, &colorData, 0, 0);
		image = cv::Mat(height, width, CV_8UC4, colorData.pBits);
		ERROR_CHECK(kinect->NuiImageStreamReleaseFrame(imageStreamHandle, &imageFrame));
	}
	void drawDepthImage(cv::Mat &image) {
		NUI_IMAGE_FRAME depthFrame = { 0 };
		ERROR_CHECK(kinect->NuiImageStreamGetNextFrame(depthStreamHandle, 0, &depthFrame));
		NUI_LOCKED_RECT depthData = { 0 };
		depthFrame.pFrameTexture->LockRect(0, &depthData, 0, 0);
		USHORT *depth = (USHORT*)depthData.pBits;
		for (unsigned int i = 0; i < (depthData.size / sizeof(USHORT)); i++){
			USHORT player = NuiDepthPixelToPlayerIndex(depth[i]);
			LONG depthX = i % width;
			LONG depthY = i / width;
			image.at<cv::Vec3b>(depthY, depthX)[0] = (player & 1) ? 0 : 255;
			image.at<cv::Vec3b>(depthY, depthX)[1] = ((player>>1) & 1) ? 0 : 255;
			image.at<cv::Vec3b>(depthY, depthX)[2] = ((player>>2) & 1) ? 0 : 255;
		}
		ERROR_CHECK(kinect->NuiImageStreamReleaseFrame(depthStreamHandle, &depthFrame));
	}
};


