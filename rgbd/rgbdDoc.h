// 
// rgbdDoc.h
//
// header file for Doc 
//

#ifndef RGBDDOC_H
#define RGBDDOC_H

#include "rgbd.h"
#include "parallaxMotion.h"
#include "adaptFocus.h"
#include "opencv2/core/core.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

class RgbdUI;

typedef Vec<double, 7> pose;

class RgbdDoc 
{
public:
	RgbdDoc();
    ~RgbdDoc();

	void	setUI(RgbdUI* ui);		// Assign the UI to use

	//int		loadImage(const char *iname);			// called by the UI to load image
	//int		saveImage(const char *iname, const char * type, int quality);// called by the UI to save image

	int     doMotion();                  // called by the UI to clear the drawing canvas
    int     writeToVideo();

	int		getDepth();						// get the UI size
	void	setDepth(int size);				// set the UI size

	int		getFrameNum();
	int		getFocusFrameIndex();
	int		getDefocusConst();
	
    int     loadImage();
	int     loadCameraPose();

	int		showParallaxImage();

	int     doAdaptFocalLength();
	int		showAdaptFocusImage();
	int     showImage(Mat &image);

	int		defocusImage(Point2i &coord);
	Mat		defocusImage(Mat &color, Mat &depth, Point2i &coord);
	Mat		defocusImage(Mat &color, Mat &depth, double curDepth);

// Attributes
public:
	
	// Dimensions of original window.
	int				m_nWidth, 
					m_nHeight; 

	unsigned char*	m_ucImage; // Images for original image and painting.
	int				m_nFocalLength;	// camera focal plane

	RgbdUI*			m_pUI;
    ParallaxMotion* m_pParaMot;
	AdaptFocus*     m_pAdaptFocus;

	// input image data
    std::vector<cv::Mat> m_origColorImages; 
    std::vector<cv::Mat> m_origDepthImages;
	
	// image from processing
	std::vector<cv::Mat> m_parallaxImages; 
    std::vector<cv::Mat> m_filterDepthImages;
	std::vector<cv::Mat> m_varyFocusLengthImages;

	// input camera pose data
	std::vector<Mat>     m_cameraPose; 

    bool				m_debugMode;
    bool				m_useOwnCapturedDataset; // indicates using self captured dataset
	bool				m_useDofFocusInParallax; // indicates apply depth-of-field focusing after parallax motion

	Point2i				m_focusCoord; // the coordinate of the focal point in 2D
};

extern void MessageBox(char *message);

#endif
