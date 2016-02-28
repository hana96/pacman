#pragma once
#include <opencv2/opencv.hpp>

#include <iostream>
#include <sstream>
#include <Windows.h>
#include <NuiApi.h>

#include "JointData.h" 

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
		ERROR_CHECK(kinect->NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR|NUI_INITIALIZE_FLAG_USES_DEPTH|NUI_INITIALIZE_FLAG_USES_SKELETON));
		ERROR_CHECK(kinect->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, IMAGE_RESOLUTION, 0, 2, 0, &imageStreamHandle));
		ERROR_CHECK(kinect->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH, IMAGE_RESOLUTION, 0, 2, 0, &depthStreamHandle));
		ERROR_CHECK(kinect->NuiSkeletonTrackingEnable(0, 0));
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
		cv::Mat image;
		cv::namedWindow("Kinect Sample");
		while (1) {
			DWORD result = WaitForSingleObject(streamEvent, INFINITE);
			ResetEvent(streamEvent);
			drawRgbImage(image);
			//drawDepthImage(image);
			drawSkeleton(image);
			cv::imshow("Kinect Sample", image);
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
			USHORT distance = NuiDepthPixelToDepth(depth[i]);
			LONG depthX = i % width;
			LONG depthY = i / width;
			LONG colorX = depthX, colorY = depthY;
			kinect->NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(IMAGE_RESOLUTION, IMAGE_RESOLUTION, 0, depthX, depthY, depth[i], &colorX, &colorY);
			if (distance > 1000){
				image.at<cv::Vec4b>(colorY, colorX)[0] = 255;
				image.at<cv::Vec4b>(colorY, colorX)[1] = 255;
				image.at<cv::Vec4b>(colorY, colorX)[2] = 255;
			}
		}
		ERROR_CHECK(kinect->NuiImageStreamReleaseFrame(depthStreamHandle, &depthFrame));
	}
	void drawSkeleton(cv::Mat &image){
		JointData joints[NUI_SKELETON_POSITION_COUNT];
		NUI_SKELETON_FRAME skeletonFrame = { 0 };
		kinect->NuiSkeletonGetNextFrame(0, &skeletonFrame);
		for (int i = 0; i < NUI_SKELETON_COUNT; i++){
			NUI_SKELETON_DATA& skeletonData = skeletonFrame.SkeletonData[i];
			if (skeletonData.eTrackingState != NUI_SKELETON_TRACKED) continue;
			for (int j = 0; j < NUI_SKELETON_POSITION_COUNT; j++){
				joints[j].state = skeletonData.eSkeletonPositionTrackingState[j];
				joints[j].x = joints[j].y = joints[j].z = 0L;
				if (skeletonData.eSkeletonPositionTrackingState[j] == NUI_SKELETON_POSITION_NOT_TRACKED) continue;
				LONG depthX = 0, depthY = 0;
				USHORT depthZ = 0;
				NuiTransformSkeletonToDepthImage(skeletonData.SkeletonPositions[j], &depthX, &depthY, &depthZ, IMAGE_RESOLUTION);
				LONG colorX = 0, colorY = 0;
				kinect->NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(IMAGE_RESOLUTION, IMAGE_RESOLUTION, 0, (LONG)depthX, (LONG)depthY, 0, &colorX, &colorY);
				joints[j].x = colorX; joints[j].y = colorY; joints[j].z = NuiDepthPixelToDepth(depthZ);
			}
			drawSegments(image, joints);
		}
	}
	void drawSegments(cv::Mat &image, JointData joints[]){
		for (int i = 0; i < JointData::nseg / 2; i++){
			int si = JointData::segment[i * 2], di = JointData::segment[i * 2 + 1];
			JointData sd = joints[si], dd = joints[di];
			if (sd.state == NUI_SKELETON_POSITION_NOT_TRACKED || dd.state == NUI_SKELETON_POSITION_NOT_TRACKED) continue;
			cv::line(image, cv::Point(sd.x, sd.y), cv::Point(dd.x,dd.y), cv::Scalar(0, 0, 255), 5);
		}
	}
};


