#include "tracking.h"
#include <iostream>


int main()
{
	cv::VideoCapture video("PATH_to_Video");

	if (!video.isOpened()) return -1;

	Mat frame;
	video.read(frame);

	TrackingPucks pt(frame);

	pt.startPosition(frame);
	pt.init(frame);
	pt.startloop(video);

	video.release();
	cv::destroyAllWindows();
}