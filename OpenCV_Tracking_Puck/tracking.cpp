
#include "tracking.h"

using namespace std;
using namespace cv;

///
/// Class --TrackingPucks-- Begin
///
TrackingPucks::TrackingPucks(cv::VideoCapture video)
{
	video_map.reserve(video.get(CAP_PROP_FRAME_COUNT));
	//	point_puck.reserve(video.get(CAP_PROP_FRAME_COUNT) + 1);

	FPS = video.get(CAP_PROP_FPS);

	cv::Mat frame;
	video.read(frame);

	frame_in = frame.clone();
	video_map.push_back(frame);

	startPosition(frame);
	s_grid = grid(frame);

	while (video.read(frame))
		video_map.push_back(frame.clone());


	startloop();

	video.release();
};

void TrackingPucks::tracking_puck(int i)
{
	cout << "Threads start - " << i << endl;
	auto puck = pool_point_puck[i];
	point_puck[i].push_back(puck);

	int k = 1;
	for (auto& frame : video_map)
	{
		pool_tracker[i]->update(frame, puck);
		point_puck[i].push_back(puck);
		k++;
	}
}

void TrackingPucks::rectDraw()
{
	vector<vector<Rect>::iterator> it;
	for (auto& ii : point_puck)
		it.push_back(ii.begin());

	for (auto& frame : video_map) {

		for (auto& rect : it)
		{
			cv::rectangle(frame, (*rect), cv::Scalar(255, 0, 0));

			Point2f center;
			center.x = (*rect).x + (*rect).width / 2;
			center.y = (*rect).y + (*rect).height / 2;


			line(frame, center, Point2f{ s_grid.center.x, center.y }, Scalar(0, 0, 255), 1, LINE_AA);	// X
			line(frame, center, Point2f{ center.x, s_grid.center.y }, Scalar(0, 0, 255), 1, LINE_AA);	// Y

			string str_hor = to_string((center.x - s_grid.center.x) / s_grid.hor);
			string str_ver = to_string((s_grid.center.y - center.y) / s_grid.ver);

			cv::putText(frame,									// target image
				{ "[" + str_hor + ", " + str_ver + "]" },			// text
				cv::Point((*rect).x, (*rect).y),	// top-left position
				cv::FONT_HERSHEY_DUPLEX,
				0.6,												// scale
				CV_RGB(118, 185, 0),								// font color
				1);
			rect++;
		}


		cv::imshow("Video", frame);
		cv::waitKey(30);
	}
}

void TrackingPucks::init(Mat& frame)
{
	frame_in = frame.clone();
	pool_tracker.resize(pool_point_puck.size());

	for (int i = 0; i < pool_point_puck.size(); i++)
	{
		pool_tracker[i] = cv::TrackerCSRT::create();			// TrackerKCF // TrackerCSRT	// TrackerGOTURN // trackermil
		pool_tracker[i]->init(frame_in, pool_point_puck[i]);
		pool_thread.emplace_back(&TrackingPucks::tracking_puck, this, i);
	}
}

void TrackingPucks::startloop()
{
	point_puck.reserve(pool_point_puck.size());
	pool_tracker.reserve(pool_point_puck.size());

	for (int i = 0; i < pool_point_puck.size(); i++)
	{
		cout << i << endl;
		pool_tracker[i] = cv::TrackerCSRT::create();			// TrackerKCF // TrackerCSRT	// TrackerGOTURN // trackermil

		pool_tracker[i]->init(frame_in, pool_point_puck[i]);

		pool_thread.emplace_back(&TrackingPucks::tracking_puck, this, i);
	}

	for (auto& thread : pool_thread) {
		thread.join();
	}

	cout << "start Draw" << endl;
	rectDraw();
}

void TrackingPucks::startPosition(Mat& frame)
{
	Mat img_gray;
	cvtColor(frame, img_gray, COLOR_BGR2GRAY);

	Mat thresh;
	threshold(img_gray, thresh, 50, 255, THRESH_BINARY);

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(thresh, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);

	// Получаем внутренние контуры
	vector<vector<Point>> internalcontours;
	vector<double> v_area;
	for (size_t i = 0; i < contours.size(); ++i) {
		// Находим ориентацию: cw или cc
		double area = contourArea(contours[i], true);
		if (area >= 0) {
			// Внутренний контур
			internalcontours.push_back(contours[i]);
			v_area.push_back(area);
		}
	}

	int j = 0, i = 0;
	double n = 0;
	for (const auto& v_point : internalcontours) {
		Point2f center;
		float radius;
		minEnclosingCircle(v_point, center, radius);
		radius = ceil(radius);

		if (radius >= 20 && radius < 30)
		{
			Radius += radius;
			n += v_area[j];
			i++;
			radius = 35;
			pool_point_puck.emplace_back((center.x - radius), (center.y - radius), radius * 2, radius * 2);

		}
		j++;
	}

	Radius = Radius / i + 5;
	square = n / i;
}
///
/// Class --TrackingPucks-- End
///
/// 
/// 
/// Time
	//auto start = std::chrono::steady_clock::now();

		//auto end = std::chrono::steady_clock::now();
		//auto diff = end - start;
		//std::cout << std::chrono::duration<double, std::milli>(diff).count() << " ms" << std::endl;