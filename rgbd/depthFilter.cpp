
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "depthFilter.h"

using namespace std;
using namespace cv;

int clip(int value, int min, int max)
{
	return std::min(std::max(value, min), max);
}


// ---------------------------------
// bilateral filtering
// ---------------------------------
unsigned short jointBilateralFilter(Mat &depth, Mat &gray, int h, int w)
{
	double result = 0.0;

	int height = depth.size().height;
	int width = depth.size().width;
	double sigma_color = 15.0;
	double sigma_space = 2.0;
	double sigma_depth = 200.0;
	int window_size = 3;
	int half_window_size = window_size >> 1;

	int center_gray = (int)gray.at<unsigned char>(h, w);
	int center_depth = (int)depth.at<unsigned short>(h, w);
	double weight = 0.0;
	double sum_of_weight = 0;
	double sum_of_depth = 0;
	int i_start = clip(h - half_window_size, 0, height - 1);
	int i_end = clip(h + half_window_size, 0, height - 1);
	int j_start = clip(w - half_window_size, 0, width - 1);
	int j_end = clip(w + half_window_size, 0, width - 1);

	for (int i = i_start; i <= i_end; ++i) {
		for (int j = j_start; j <= j_end; ++j) {
			int d = (int)depth.at<unsigned short>(i, j);
			if (d != 0) {
				int g = (int)gray.at<unsigned char>(i, j);
				double weight_color = exp(-(g - center_gray) * (g - center_gray) / (2.0 * sigma_color * sigma_color));
				double weight_space = exp(-((i - h) * (i - h) + (j - w) * (j - w)) / (2.0 * sigma_space * sigma_space));
				double weight_depth = (center_depth == 0) ? 1.0 : exp(-(d - center_depth) * (d - center_depth) / (2.0 * sigma_depth * sigma_depth));
				weight = weight_color /* weight_space * weight_depth*/;
				sum_of_depth += weight * d;
				sum_of_weight += weight;
			}
		}
	}


	if (sum_of_weight > 0.0) {
		result = sum_of_depth / sum_of_weight;
	}

	return (unsigned short)result;
}

// -------------------------------------------------------------------------------
// fill the holes in a depth image with the help of its corrsponding color image
// -------------------------------------------------------------------------------
Mat depthFilter(Mat &depth, Mat &color, bool debug)
{
	Mat filterDepth = depth.clone(); // copy depth

	int height = depth.size().height;
	int width = depth.size().width;

	Mat gray;
	cvtColor(color, gray, CV_RGB2GRAY);

	// scale the depth image by 1/32 to [0, 255] then display it
	Mat adjfilterDepth; 
	
	// show original
	if (debug) {
		convertScaleAbs(depth, adjfilterDepth, 1.0 / 32);
		cv::namedWindow("original depth", cv::WINDOW_AUTOSIZE);// Create a window for display.
		cv::imshow("original depth", adjfilterDepth);
	}

	// recursively fix zero value
	vector<Point> zeros;

	for (int i = height - 1; i >= 0; --i) {
		for (int j = width - 1; j >= 0; --j) {
			if (filterDepth.at<unsigned short>(i, j) == 0)
				zeros.push_back(Point(j, i));
		}
	}
	if (debug) {
		cout << zeros.size() << endl;
	}

	int previous_size = height * width;
	while ((int)zeros.size() < previous_size) {

		previous_size = zeros.size();

		// fill holes
		Mat temp = filterDepth.clone();
		while (zeros.size() > 0) {
			Point p = zeros.back();
			zeros.pop_back();
			filterDepth.at<unsigned short>(p) = jointBilateralFilter(temp, gray, p.y, p.x);
		}

		// count zeros again
		for (int i = height - 1; i >= 0; --i) {
			for (int j = width - 1; j >= 0; --j) {
				if (filterDepth.at<unsigned short>(i, j) == 0)
					zeros.push_back(Point(j, i));
			}
		}
		if(debug)cout << zeros.size() << endl;
	}

	// show after filter
	if (debug) {
		convertScaleAbs(filterDepth, adjfilterDepth, 1.0 / 32);
		cv::namedWindow("hole fix depth", cv::WINDOW_AUTOSIZE);// Create a window for display.
		cv::imshow("hole fix depth", adjfilterDepth);
	}

	// global filter
	Mat temp = filterDepth.clone();
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			filterDepth.at<unsigned short>(i, j) = jointBilateralFilter(temp, gray, i, j);
		}
	}

	// show after filter
	if (debug) {
		cv::namedWindow("filter depth", cv::WINDOW_AUTOSIZE);// Create a window for display.
		cv::imshow("filter depth", adjfilterDepth);
		convertScaleAbs(filterDepth, adjfilterDepth, 1.0 / 32);
	}

	return filterDepth;
}


// ---------------------------------
// focal length adaptive filter
// ---------------------------------
Mat FocalLengthAdaptFilter(Mat &color, Mat &depth, double focalLength, int defocusConst)
{
	Mat colorFloat;
	color.convertTo(colorFloat, CV_64FC3);
	Mat depthShort;
	depth.convertTo(depthShort, CV_16UC1);

	int height = color.size().height;
	int width = color.size().width;

	Mat filterColor = Mat(height, width, CV_64FC3, Scalar::all(0.0));

	for (int h = 0; h < height; h++) {
		for (int w = 0; w < width; w++) {

			double depthDiff = abs((double)depthShort.at<unsigned short>(h, w) - focalLength) / 1000;

			double sum_of_weight = 0;
			Vec3d sum_of_color(0.0, 0.0, 0.0);

			int window_size = 2 * (int)(depthDiff * defocusConst) + 1;
			int half_window_size = window_size >> 1;

			int i_start = clip(h - half_window_size, 0, height - 1);
			int i_end = clip(h + half_window_size, 0, height - 1);
			int j_start = clip(w - half_window_size, 0, width - 1);
			int j_end = clip(w + half_window_size, 0, width - 1);

			for (int i = i_start; i <= i_end; ++i) {
				for (int j = j_start; j <= j_end; ++j) {
					depthDiff = abs((double)depthShort.at<unsigned short>(i, j) - focalLength) / 1000;
					sum_of_color += colorFloat.at<Vec3d>(i, j);
					sum_of_weight++;
				}
			}
			
			if (sum_of_weight > 0.0) {
				filterColor.at<Vec3d>(h, w) = sum_of_color / sum_of_weight;
			}
			else {
				filterColor.at<Vec3d>(h, w) = colorFloat.at<Vec3d>(h, w);
			}
		}

	}

	return filterColor;
}

