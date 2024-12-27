#ifndef TRACKING
#define TRACKING

#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/tracking.hpp"


#include <iostream>
#include "auto_grid.h"

using namespace std;
using namespace cv;

class Flag
{
public:
	Flag() : flag_{ false } {}

	void set()
	{
		{
			std::lock_guard<std::mutex> g(mutex_);
			flag_ = true;
		}
		cond_var_.notify_all();
	}

	void clear()
	{
		std::lock_guard<std::mutex> g(mutex_);
		flag_ = false;
	}

	void wait()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		cond_var_.wait(lock, [this]() { return flag_; });
	}
private:
	bool flag_;
	std::mutex mutex_;
	std::condition_variable cond_var_;
};

class TrackingPucks
{
	void tracking_puck(int);
	void rectDraw();

public:
	TrackingPucks(cv::Mat);

	void init(Mat&);
	void startloop(cv::VideoCapture&);
	void startPosition(Mat&);

private:

	cv::Mat frame_in;
	setting_grid s_grid;

	double square;
	float Radius;
	bool flag_end = false;

	vector<thread> pool_thread;
	vector<Rect> pool_point_puck;
	vector<cv::Ptr<cv::Tracker>> pool_tracker;
};

#endif //TRACKING