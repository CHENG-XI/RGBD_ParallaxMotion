//
// AdaptFocus.cpp
//
// The code calling the core functions of parallax motion generator
//
// DEMO -- use <meeting_small> image 14-16

#include "rgbd.h"
#include "rgbdDoc.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "AdaptFocus.h"
#include "depthFilter.h"

using namespace std;
using namespace cv;

int depthFrameNum = 30;

AdaptFocus::AdaptFocus() {}

AdaptFocus::~AdaptFocus() {}

void AdaptFocus::setDocument(RgbdDoc* doc)
{
	m_pDoc = doc;
}

// -----------------------------------------------------------------------------------------------------
// generate frames of images by depth-of-field focusing, from the min to max depth of the input image
// -----------------------------------------------------------------------------------------------------
int AdaptFocus::generateVaryFocusImages()
{
	if (m_pDoc->m_origDepthImages.size() < 1) {
		cout << "please load an image" << endl;
		return 0;
	}

	int	height = m_pDoc->m_origColorImages[0].size().height;
	int	width = m_pDoc->m_origColorImages[0].size().width;
	
	double min, max; 
	cv::minMaxLoc(m_pDoc->m_filterDepthImages[0], &min, &max); // min and max depth value (mm)

	m_pDoc->m_varyFocusLengthImages.resize(depthFrameNum);
	int defocusConst = m_pDoc->getDefocusConst();
	for (int frame = 0; frame < depthFrameNum; frame++)
	{
		cout << "generating frame " << frame << endl;

		m_pDoc->m_varyFocusLengthImages[frame] = Mat(height, width, CV_64FC3);
		double curfocalLength = min + double(max - min) / depthFrameNum * frame;
	
		m_pDoc->m_varyFocusLengthImages[frame] = FocalLengthAdaptFilter(m_pDoc->m_origColorImages[0], m_pDoc->m_filterDepthImages[0], curfocalLength, defocusConst);

		// convert parallax image to 8UC3 type
		Mat u8c3_image;
		m_pDoc->m_varyFocusLengthImages[frame].convertTo(u8c3_image, CV_8UC3);
		m_pDoc->m_varyFocusLengthImages[frame] = u8c3_image;
	}

	return 1;
}

// -----------------------------------------------------------------------------------------------
// depth-of-field focusing given a depth specified by a coordinate, on the firstly loaded image
// -----------------------------------------------------------------------------------------------
Mat AdaptFocus::defocusImageByCoord(Point2i &coord)
{
	double curfocalLength = m_pDoc->m_filterDepthImages[0].at<unsigned short>(coord.y, coord.x);
	int defocusConst = m_pDoc->getDefocusConst();

	Mat image = FocalLengthAdaptFilter(m_pDoc->m_origColorImages[0], m_pDoc->m_filterDepthImages[0], curfocalLength, defocusConst);

	image.convertTo(image, CV_8UC3);

	return image;
}


// -------------------------------------------------------------------------------------------
// depth-of-field focusing given a depth specified by a coordinate, on a specified image
// -------------------------------------------------------------------------------------------
Mat	 AdaptFocus::defocusImageByCoord(Mat &color, Mat &depth, Point2i &coord)
{
	double curfocalLength = depth.at<double>(coord.y, coord.x);
	int defocusConst = m_pDoc->getDefocusConst();

	Mat image = FocalLengthAdaptFilter(color, depth, curfocalLength, defocusConst);

	image.convertTo(image, CV_8UC3);

	return image;
}

// -------------------------------------------------------------------------------------------
// depth-of-field focusing given a depth specified by a coordinate, on a specified image
// -------------------------------------------------------------------------------------------
Mat	 AdaptFocus::defocusImageByCoord(Mat &color, Mat &depth, double curfocalLength)
{
	int defocusConst = m_pDoc->getDefocusConst();

	Mat image = FocalLengthAdaptFilter(color, depth, curfocalLength, defocusConst);

	image.convertTo(image, CV_8UC3);

	return image;
}