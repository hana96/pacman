#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
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

class KinectControl
{
private:
	INuiSensor *kinect;
	HANDLE imageStreamHandle, depthStreamHandle, streamEvent;
	DWORD width, height;

	void initialize() {
		createInstance();
		ERROR_CHECK(kinect->NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR
			| NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX
			| NUI_INITIALIZE_FLAG_USES_SKELETON));
		ERROR_CHECK(kinect->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, IMAGE_RESOLUTION, 0, 2, 0, &imageStreamHandle));
		ERROR_CHECK(kinect->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, IMAGE_RESOLUTION, 0, 2, 0, &depthStreamHandle));
		ERROR_CHECK(kinect->NuiSkeletonTrackingEnable(0, 0));
		streamEvent = CreateEvent(0,TRUE,FALSE,0);
		ERROR_CHECK(kinect->NuiSetFrameEndEvent(streamEvent,0));
		NuiImageResolutionToSize(IMAGE_RESOLUTION,width,height);
	}
	void createInstance() {
		if (count() == 0) throw runtime_error("No Kinnect");
		ERROR_CHECK(NuiCreateSensorByIndex(0, &kinect));
		if (kinect->NuiStatus() != S_OK) throw runtime_error("Cannot use Kinect");
	}
public:
	cv::Mat rgbImage;
	cv::Mat depthImage;
	cv::Mat playerImage;
	cv::Mat placeImage;
	vector<vector<pair<cv::Vec3f, int>>> skeleton;

	static const int segment[];
	static const int nseg;

public:
	KinectControl(){
		kinect = 0;
	}
	~KinectControl(){
		if (kinect != 0){
			kinect->NuiShutdown();
			kinect->Release();
			kinect = 0;
		}
	}
	int getWidth() { return width; }
	int getHeight(){ return height; }
	int count() {
		int count = 0;
		ERROR_CHECK(NuiGetSensorCount(&count));
		return count;
	}
	bool start(){
		if (kinect != 0)return false;
		initialize();
		depthImage = cv::Mat(height, width, CV_16UC1, cv::Scalar(0));
		playerImage = cv::Mat(height,width,CV_8UC1,cv::Scalar(0));
	}
	bool next() {
		DWORD result = WaitForSingleObject(streamEvent, INFINITE);
		ResetEvent(streamEvent);
		return true;
	}
	void run() {
		while (next()){
			setRgbImage(rgbImage);
			setDepthImage(depthImage,playerImage);
			setSkeleton(skeleton);
			if (skeleton.size() > 0) drawSegment(rgbImage, skeleton[0]);
			cv::imshow("Kinect", rgbImage);
			int key = cv::waitKey(10);
			if (key == 27)break;
		}
		cv::destroyAllWindows();
	}
	void setRgbImage() { setRgbImage(rgbImage); }
	void setRgbImage(cv::Mat &image){
		NUI_IMAGE_FRAME imageFrame = { 0 };
		ERROR_CHECK(kinect->NuiImageStreamGetNextFrame(imageStreamHandle, 0, &imageFrame));
		NUI_LOCKED_RECT colorData;
		imageFrame.pFrameTexture->LockRect(0, &colorData, 0, 0);
		image = cv::Mat(height, width, CV_8UC4, colorData.pBits);
		ERROR_CHECK(kinect->NuiImageStreamReleaseFrame(imageStreamHandle, &imageFrame));
	}
	void setDepthImage(){ setDepthImage(depthImage, playerImage); }
	void setDepthImage(cv::Mat &image, cv::Mat &playerImage){
		NUI_IMAGE_FRAME depthFrame = { 0 };
		ERROR_CHECK(kinect->NuiImageStreamGetNextFrame(depthStreamHandle, 0, &depthFrame));
		NUI_LOCKED_RECT depthData = { 0 };
		depthFrame.pFrameTexture->LockRect(0, &depthData, 0, 0);
		USHORT *depth = (USHORT*)depthData.pBits;
		for (unsigned int i = 0; i < (depthData.size / sizeof(USHORT)); i++){
			USHORT distance = NuiDepthPixelToDepth(depth[i]);
			USHORT player = NuiDepthPixelToPlayerIndex(depth[i]);
			LONG depthX = i % width;
			LONG depthY = i / width;
			LONG colorX = depthX, colorY = depthY;
			kinect->NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(IMAGE_RESOLUTION, IMAGE_RESOLUTION, 0, depthX, depthY, depth[i], &colorX, &colorY);
			image.at<USHORT>(colorY, colorX) = distance;
			playerImage.at<UCHAR>(colorY, colorX) = player;
		}
		ERROR_CHECK(kinect->NuiImageStreamReleaseFrame(depthStreamHandle, &depthFrame));
	}
	void setSkeleton() { setSkeleton(skeleton); }
	void setSkeleton(vector<vector<pair<cv::Vec3f, int>>> &skeleton){
		skeleton.clear();
		NUI_SKELETON_FRAME skeletonFrame = { 0 };
		kinect->NuiSkeletonGetNextFrame(0, &skeletonFrame);
		for (int i = 0; i < NUI_SKELETON_COUNT; i++){
			NUI_SKELETON_DATA& skeletonData = skeletonFrame.SkeletonData[i];
			if (skeletonData.eTrackingState != NUI_SKELETON_TRACKED) continue;
			vector<pair<cv::Vec3f, int>> skel;
			skel.resize(NUI_SKELETON_POSITION_COUNT);
			for (int j = 0; j < NUI_SKELETON_POSITION_COUNT; j++) {
				int state = skeletonData.eSkeletonPositionTrackingState[j];
				if (skeletonData.eSkeletonPositionTrackingState[j] == NUI_SKELETON_POSITION_NOT_TRACKED) continue;
				LONG depthX = 0, depthY = 0;
				USHORT depthZ = 0;
				NuiTransformSkeletonToDepthImage(skeletonData.SkeletonPositions[j], &depthX, &depthY, &depthZ, IMAGE_RESOLUTION);
				LONG colorX = 0, colorY = 0;
				kinect->NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(IMAGE_RESOLUTION, IMAGE_RESOLUTION, 0, (LONG)depthX, (LONG)depthY, 0, &colorX, &colorY);
				cv::Vec3f pt(colorX, colorY, NuiDepthPixelToDepth(depthZ));
				skel[j] = pair<cv::Vec3f, int>(pt,state);
			}
			skeleton.push_back(skel);
		}
	}
	void drawSegment(cv::Mat &image, vector<pair<cv::Vec3f, int>> &joints) {
		for (int i = 0; i < nseg / 2; i++){
			int si = segment[i * 2], di = segment[i * 2 + 1];
			pair<cv::Vec3f, int> sd = joints[si], dd = joints[di];
			if (sd.second == NUI_SKELETON_POSITION_NOT_TRACKED || dd.second == NUI_SKELETON_POSITION_NOT_TRACKED) continue;
			cv::line(image, cv::Point(sd.first[0], sd.first[1]), cv::Point(dd.first[0], dd.first[1]), cv::Scalar(0, 0, 255), 5);
		}
	}
};

