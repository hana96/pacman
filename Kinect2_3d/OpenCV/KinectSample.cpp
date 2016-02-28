

#include "stdafx.h"
#include "KinectSample.h"


KinectSample::KinectSample()
{
	kinect = 0;
}


KinectSample::~KinectSample()
{
	if (kinect != 0) {
		kinect->NuiShutdown();
		kinect->Release();
		//kinect = 0;
	}
}
