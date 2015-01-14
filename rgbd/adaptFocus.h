/* core functions for parallax motion*/

#ifndef ADAPTFOCUS_H
#define ADAPTFOCUS_H

#include "opencv2/core/core.hpp"

using namespace std;
using namespace cv;

class RgbdDoc;

class AdaptFocus
{
public:
	AdaptFocus();
	~AdaptFocus();

    // Member functions
	void				setDocument(RgbdDoc* doc);
	RgbdDoc*	        getDocument();

	int					handle(int event); 

    int			        generateVaryFocusImages();

	Mat					defocusImageByCoord(Point2i &coord);
	Mat					defocusImageByCoord(Mat &color, Mat &depth, Point2i &coord);
	Mat					defocusImageByCoord(Mat &color, Mat &depth, double curfocalLength);

private:
	RgbdDoc*			m_pDoc;		// pointer to document to communicate with the document

};

#endif