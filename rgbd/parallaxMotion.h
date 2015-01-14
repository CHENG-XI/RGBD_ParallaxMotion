/* core functions for parallax motion*/

#ifndef PARALLAXMOTION_H
#define PARALLAXMOTION_H

#include "opencv2/core/core.hpp"

using namespace std;
using namespace cv;

class RgbdDoc;

typedef vector<Point3d> point3dCloud;
typedef Vec<double, 7> pose;

class ParallaxMotion
{
public:
    ParallaxMotion();
    ~ParallaxMotion();

    // Member functions
	void				setDocument(RgbdDoc* doc);
	RgbdDoc*	        getDocument();
    int					generateParallaxMotion();
	int					depthToCloud(Mat &depth, Mat &p3dCloud, int topleft_x, int topleft_y);
	int					point3dCloudSetTransform(Mat &pcloudOld, Mat &pcloudNew, Mat &poseOld, Mat &poseNew);

	Point3d				depthToCloud(Point2i &coord, double depth, int topleft_w, int topleft_h);
	Point3d				point3dTransform(Point3d &coord3d, Mat &poseOld, Mat &poseNew);

private:
	RgbdDoc*	        m_pDoc;		// pointer to document to communicate with the document
	vector<Mat>         m_point3dCloudSet; // point cloud set

};

#endif