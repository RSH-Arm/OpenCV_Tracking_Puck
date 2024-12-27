#include "tracking.h"
#include <iostream>


int main()
{
	cv::VideoCapture video("C:\\Users\\Colonel\\Desktop\\Detect\\v1.mp4");

	if (!video.isOpened()) return -1;

//	Mat frame;
//	video.read(frame);
	auto start = std::chrono::steady_clock::now();


	TrackingPucks pt(video);
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << std::chrono::duration<double, std::milli>(diff).count() << " ms" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(50000));
//	pt.startPosition(frame);
//	pt.init(frame);
//	pt.startloop(video);

//	video.release();
//	cv::destroyAllWindows();
}