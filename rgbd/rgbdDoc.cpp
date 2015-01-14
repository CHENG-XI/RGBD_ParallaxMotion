// 
// rgbdDoc.cpp
//
// It basically maintain the Image for answering the color query from the brush.
// It also acts as the bridge between brushes and UI (including views)
//


#include <FL/fl_ask.H>

#include <algorithm>

#include "rgbdDoc.h"
#include "rgbdUI.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "depthFilter.h"


#define DESTROY(p)	{  if ((p)!=NULL) {delete [] p; p=NULL; } }

RgbdDoc::RgbdDoc() 
{
	m_nWidth		= -1;
	m_ucImage		= NULL;
    m_debugMode     = false;
    m_useOwnCapturedDataset = false;
	m_useDofFocusInParallax = false;
	m_pParaMot		= NULL;
	m_focusCoord.x  = 0;
	m_focusCoord.y  = 0;
}

RgbdDoc::~RgbdDoc() 
{
    if (m_ucImage) {
        delete[] m_ucImage;
        m_ucImage = NULL;
    }
	if (m_pParaMot) delete m_pParaMot;
	if (m_pAdaptFocus)  delete m_pAdaptFocus;
}


//---------------------------------------------------------
// Set the current UI 
//---------------------------------------------------------
void RgbdDoc::setUI(RgbdUI* ui) 
{
	m_pUI	= ui;
}

//---------------------------------------------------------
// Returns focal depth 
//---------------------------------------------------------
int RgbdDoc::getDepth()
{
	return m_pUI->getDepth();
}

//---------------------------------------------------------
// Returns the num of frames
//---------------------------------------------------------
int RgbdDoc::getFrameNum()
{
	return m_pUI->getFrameNum();
}

//
//
//
int RgbdDoc::getFocusFrameIndex()
{
	return m_pUI->getFocusFrameIndex();
}

//
int RgbdDoc::getDefocusConst()
{
	return m_pUI->getDefocusConst();
}

//---------------------------------------------------------
// Load the camera pose file and get info. from it.
// This is called by the UI when the load image button is 
// pressed.
//---------------------------------------------------------
int RgbdDoc::loadCameraPose()
{
	ifstream posefile(m_pUI->m_poseName.c_str());

	string line;
	int startline = 1;
	for (unsigned int i = 0; i < m_pUI->m_imageIndex.size(); i++){
		for (int k = startline; k < m_pUI->m_imageIndex[i]; k++) {
			getline(posefile, line);
			startline++;
		}
		getline(posefile, line);
		startline++;
		line = line.substr(line.find_last_of(':') + 2, string::npos);
		stringstream ss(stringstream::in | stringstream::out);
		ss << line;
		Mat camerapose = Mat::zeros(7, 1, CV_64F);
		ss >> camerapose.at<double>(0, 0) 
           >> camerapose.at<double>(1, 0) 
           >> camerapose.at<double>(2, 0) 
           >> camerapose.at<double>(3, 0) 
           >> camerapose.at<double>(4, 0) 
           >> camerapose.at<double>(5, 0)
           >> camerapose.at<double>(6, 0);
		m_cameraPose.push_back(camerapose);
	}
	cout << "num of camera pose = " << m_cameraPose.size() << endl;
	return 1;
}


//---------------------------------------------------------
// Load the specified image
// This is called by the UI when the load image button is 
// pressed.
//---------------------------------------------------------
int RgbdDoc::loadImage()
{
    for (unsigned int i = 0; i < m_pUI->m_rgbNames.size(); ++i) {
        cv::Mat image, imageRGB;
        // color
        image = cv::imread(m_pUI->m_rgbNames[i], CV_LOAD_IMAGE_COLOR);   // Read the file
        // cv::cvtColor(image, imageRGB, CV_BGR2RGB);
        m_origColorImages.push_back(image);

        // depth
        image = cv::imread(m_pUI->m_depthNames[i], CV_LOAD_IMAGE_ANYDEPTH);
        m_origDepthImages.push_back(image);
    }

    //cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
    //cv::imshow( "Display window", m_origDepthImages[0] );  

    int width = m_origColorImages[0].size().width;
    int height = m_origColorImages[0].size().height;
    
    if (m_ucImage == NULL)
        m_ucImage = new unsigned char [(int)width * (int)height * 3];

    m_nWidth = width;
    m_nHeight = height;

    m_pUI->m_mainWindow->resize(m_pUI->m_mainWindow->x(), 
								m_pUI->m_mainWindow->y(), 
								width + 260, 
								height);

	// display it on origView
	m_pUI->m_imgView->resizeWindow(width, height);

    // show first RGB image
    for (int y = 0; y < height; y++) {
		for (int x = 0; x < width ; x++) {
			m_ucImage[(height-1-y) * 3 * width + 3 * x    ] = m_origColorImages[0].data[y * 3 * width + 3 * x + 2]; // RGB to BGR
            m_ucImage[(height-1-y) * 3 * width + 3 * x + 1] = m_origColorImages[0].data[y * 3 * width + 3 * x + 1];
            m_ucImage[(height-1-y) * 3 * width + 3 * x + 2] = m_origColorImages[0].data[y * 3 * width + 3 * x    ];
		}
	}
    m_pUI->m_imgView->refresh();

    // generate filtered depth images
	m_filterDepthImages.resize(m_origDepthImages.size());
    for (unsigned int i = 0; i < m_origDepthImages.size(); ++i) {
        if (m_useOwnCapturedDataset) {
            // skip filtering depth
            m_filterDepthImages[i] = m_origDepthImages.at(i);
        }
        else {
            Mat filterDepth = depthFilter(m_origDepthImages.at(i), m_origColorImages.at(i), m_debugMode);
            m_filterDepthImages[i] = filterDepth;
        }
    }

	cout << "number of images = " << m_origDepthImages.size() << endl;

    return 1;
}

////----------------------------------------------------------------
//// Save the specified image
//// This is called by the UI when the save image menu button is 
//// pressed.
////----------------------------------------------------------------
//int RgbdDoc::saveImage(const char *iname, const char * type, int quality) 
//{
//
//	save(iname, m_ucPainting, m_nPaintWidth, m_nPaintHeight, type, quality);
//
//	return 1;
//}

//----------------------------------
// Do parallax motion
//----------------------------------
int RgbdDoc::doMotion() 
{
    m_pParaMot = new ParallaxMotion();

    // set pDoc
    m_pParaMot->setDocument(this);

    // do parallax motion
    m_pParaMot->generateParallaxMotion();

	return 1;
}


// ----------------------------------------------
// write the frames into a video
// ----------------------------------------------
int RgbdDoc::writeToVideo()
{
    if (m_parallaxImages.size() == NUM_OF_FRAMES) {
        VideoWriter parallax_video("parallaxMotion.avi", CV_FOURCC('D','I','B',' '), 15, Size(m_nWidth,m_nHeight), true);
        for (int i = 0; i < (int)m_parallaxImages.size(); ++i) {
            parallax_video.write(m_parallaxImages[i]);
        }
        cout << "Write parallax motion video!" << endl;
    }

    if (m_varyFocusLengthImages.size() == 15) {
        VideoWriter defocus_video("defocus.avi", CV_FOURCC('D','I','B',' '), 15, Size(m_nWidth,m_nHeight), true);
        for (int i = 0; i < (int)m_varyFocusLengthImages.size(); ++i) {
            defocus_video.write(m_varyFocusLengthImages[i]);
        }

        cout << "Write defocusing video!" << endl;
    }

    return 1;
}


// ----------------------------------------------
// show a parallax image
// ----------------------------------------------
int RgbdDoc::showParallaxImage() {
	for (int y = 0; y < m_nHeight; y++) {
		for (int x = 0; x < m_nWidth; x++) {
            m_ucImage[(m_nHeight - 1 - y) * 3 * m_nWidth + 3 * x    ] = m_parallaxImages[getFrameNum() - 1].data[y * 3 * m_nWidth + 3 * x + 2];
            m_ucImage[(m_nHeight - 1 - y) * 3 * m_nWidth + 3 * x + 1] = m_parallaxImages[getFrameNum() - 1].data[y * 3 * m_nWidth + 3 * x + 1];
            m_ucImage[(m_nHeight - 1 - y) * 3 * m_nWidth + 3 * x + 2] = m_parallaxImages[getFrameNum() - 1].data[y * 3 * m_nWidth + 3 * x    ];
		}
	}
	m_pUI->m_imgView->refresh();
	return 1;
}


// -----------------------------------------------------
// generate frames with changing depth-of-field
// -----------------------------------------------------
int RgbdDoc::doAdaptFocalLength() {

	m_pAdaptFocus = new AdaptFocus();

	// set pDoc
	m_pAdaptFocus->setDocument(this);

	// do parallax motion
	m_pAdaptFocus->generateVaryFocusImages();

	return 1;
}


// ---------------------------------------------------------
// show a image with specific depth-of-field
// ---------------------------------------------------------
int RgbdDoc::showAdaptFocusImage() {
	for (int y = 0; y < m_nHeight; y++) {
		for (int x = 0; x < m_nWidth; x++) {
            m_ucImage[(m_nHeight - 1 - y) * 3 * m_nWidth + 3 * x    ] = m_varyFocusLengthImages[getFocusFrameIndex() - 1].data[y * 3 * m_nWidth + 3 * x + 2];
            m_ucImage[(m_nHeight - 1 - y) * 3 * m_nWidth + 3 * x + 1] = m_varyFocusLengthImages[getFocusFrameIndex() - 1].data[y * 3 * m_nWidth + 3 * x + 1];
            m_ucImage[(m_nHeight - 1 - y) * 3 * m_nWidth + 3 * x + 2] = m_varyFocusLengthImages[getFocusFrameIndex() - 1].data[y * 3 * m_nWidth + 3 * x    ];
		}
	}
	m_pUI->m_imgView->refresh();
	return 1;
}


// -------------------------------------------
// show a image
// -------------------------------------------
int RgbdDoc::showImage(Mat &image) {
	if (image.size().height != m_nHeight || image.size().width != m_nWidth) {
		cout << "image size does not match " << endl;
		exit(0);
	}

	for (int y = 0; y < m_nHeight; y++) {
		for (int x = 0; x < m_nWidth; x++) {
			m_ucImage[(m_nHeight - 1 - y) * 3 * m_nWidth + 3 * x] = image.data[y * 3 * m_nWidth + 3 * x + 2];
			m_ucImage[(m_nHeight - 1 - y) * 3 * m_nWidth + 3 * x + 1] = image.data[y * 3 * m_nWidth + 3 * x + 1];
			m_ucImage[(m_nHeight - 1 - y) * 3 * m_nWidth + 3 * x + 2] = image.data[y * 3 * m_nWidth + 3 * x];
		}
	}
	m_pUI->m_imgView->refresh();
	return 1;
}


// ---------------------------------------------------------------------------------------------------------
// re-focus an image with specific depth-of-field determined by a coordinate, on the firstly loaded image
// ---------------------------------------------------------------------------------------------------------
int RgbdDoc::defocusImage(Point2i &coord)
{
	m_pAdaptFocus = new AdaptFocus();

	m_pAdaptFocus->setDocument(this);

	Mat image = m_pAdaptFocus->defocusImageByCoord(coord);

	showImage(image);
	return 1;
}


// ---------------------------------------------------------------------------------------------
// re-focus an image with specific depth-of-field determined by a coordinate, on a given image
// ---------------------------------------------------------------------------------------------
Mat RgbdDoc::defocusImage(Mat &color, Mat &depth, Point2i &coord)
{
	m_pAdaptFocus = new AdaptFocus();

	m_pAdaptFocus->setDocument(this);

	Mat image = m_pAdaptFocus->defocusImageByCoord(color, depth, coord);

	return image;
}

// ---------------------------------------------------------------------------------------------
// re-focus an image with specific depth-of-field determined by a coordinate, on a given image
// ---------------------------------------------------------------------------------------------
Mat RgbdDoc::defocusImage(Mat &color, Mat &depth, double curDepth)
{
	m_pAdaptFocus = new AdaptFocus();

	m_pAdaptFocus->setDocument(this);

	Mat image = m_pAdaptFocus->defocusImageByCoord(color, depth, curDepth);

	return image;
}
