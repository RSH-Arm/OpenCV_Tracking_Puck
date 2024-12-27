
#include "tracking.h"

using namespace std;
using namespace cv;

///
/// Class --TrackingPucks-- Begin
///
TrackingPucks::TrackingPucks(cv::Mat frame)
{
	s_grid = grid(frame);
};

void TrackingPucks::tracking_puck(int i)
{
	while (!flag_end)
	{
		pool_tracker[i]->update(frame_in, pool_point_puck[i]);
	}
}

void TrackingPucks::rectDraw()
{
	for (int i = 0; i < pool_point_puck.size(); i++)
	{
		cv::rectangle(frame_in, Rect(pool_point_puck[i].x + pool_point_puck[i].width / 2 - 35, pool_point_puck[i].y + pool_point_puck[i].height / 2 - 35, 70, 70), cv::Scalar(255, 0, 0));
		
		Point2f center;
		center.x = pool_point_puck[i].x + pool_point_puck[i].width / 2;
		center.y = pool_point_puck[i].y + pool_point_puck[i].height / 2;


		line(frame_in, center, Point2f{ s_grid.center.x, center.y }, Scalar(0, 0, 255), 1, LINE_AA);	// X
		line(frame_in, center, Point2f{ center.x, s_grid.center.y }, Scalar(0, 0, 255), 1, LINE_AA);	// Y

		string str_hor = to_string((center.x - s_grid.center.x) / s_grid.hor);
		string str_ver = to_string((s_grid.center.y - center.y) / s_grid.ver);

		cv::putText(frame_in,									// target image
			{ "[" + str_hor + ", " + str_ver + "]" },			// text
			cv::Point(pool_point_puck[i].x, pool_point_puck[i].y),	// top-left position
			cv::FONT_HERSHEY_DUPLEX,
			0.6,												// scale
			CV_RGB(118, 185, 0),								// font color
			1);

	}

	cv::imshow("Video", frame_in);
	cv::waitKey(1);
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

void TrackingPucks::startloop(cv::VideoCapture& video_)
{
	while (true)
	{
		rectDraw();

		if (video_.read(frame_in))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(55));
		}
		else
		{
			flag_end = true;

			for (auto& thread : pool_thread) {
				thread.join();
			}
			break;
		}
	}
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