//
// parallaxMotion.cpp
//
// The code calling the core functions of parallax motion generator
//
// DEMO -- use <meeting_small> image 14-16

#include "rgbd.h"
#include "rgbdDoc.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "parallaxMotion.h"

#include "ppl.h"

using namespace std;
using namespace cv;

int center[2] = { 320 - 1, 240 - 1 }; // center of the image
double constant = 570.3; // focus length related parameter
int MM_PER_M = 1000; // 1m = 1000mm
double LARGE_NUM = 1e9; // basically a big numble
double SMALL_NUM = 1e-5; // a small number

int totFrame = NUM_OF_FRAMES; // total number of frames in the video

ParallaxMotion::ParallaxMotion() {}

ParallaxMotion::~ParallaxMotion() {}

void ParallaxMotion::setDocument(RgbdDoc* doc)
{
	m_pDoc = doc;
}

// ---------------------------------
// print the camera pose for debug
// ---------------------------------
int printScreen(Mat &pose) {
	cout << pose.at<double>(0, 0) << endl
		<< pose.at<double>(1, 0) << endl
		<< pose.at<double>(2, 0) << endl
		<< pose.at<double>(3, 0) << endl
		<< pose.at<double>(4, 0) << endl
		<< pose.at<double>(5, 0) << endl
		<< pose.at<double>(6, 0) << endl;

	return 1;
}


// ------------------------------------------
// generate frames with parallax motion
// ------------------------------------------
int ParallaxMotion::generateParallaxMotion()
{
	int numOfImage = m_pDoc->m_origDepthImages.size();
	// 1. fix depth image (bilateral filter, done when loading images)

	int height = 0, width = 0;
	if (numOfImage > 0) {
		height = m_pDoc->m_origColorImages[0].size().height;
		width = m_pDoc->m_origColorImages[0].size().width;
	}

	// 2. depth to point cloud
	m_point3dCloudSet.resize(numOfImage);
	for (int i = 0; i < numOfImage; ++i) {
		m_point3dCloudSet[i] = Mat::zeros(height * width, 3, CV_64F);
		Mat float_depth;
		m_pDoc->m_filterDepthImages[i].convertTo(float_depth, CV_64F); // use filtered depth
		depthToCloud(float_depth, m_point3dCloudSet[i], 0, 0);
	}

	// 3. transform point clouds and rgb values to the new image
	vector<Mat> point3dCloudSetPoseNEW; // store the transformed point cloud set
	point3dCloudSetPoseNEW.resize(numOfImage);

	vector<Mat> colorImagesTransformed; // store the transformed color images w.r.t new camera pose
	colorImagesTransformed.resize(numOfImage);

	vector<Mat> depthImagesTransformed; // store the transformed depth images w.r.t new camera pose
	depthImagesTransformed.resize(numOfImage);

	vector<Mat> weights; // store the weights when splatting occurs
	weights.resize(numOfImage);

	if (m_pDoc->m_debugMode)
		totFrame = 1;

	m_pDoc->m_parallaxImages.resize(totFrame);

	for (int frame = 0; frame < totFrame; frame++) {
		m_pDoc->m_parallaxImages[frame] = Mat(height, width, CV_64FC3);

		cout << "generating frame " << frame << endl;

		double alpha = 0.0;

		if (totFrame > 1)
			alpha = 1.0 / totFrame * frame;
		else
			alpha = 1.0 / NUM_OF_FRAMES * (m_pDoc->getFrameNum() - 1);

		// new camera pose
		Mat poseNEW;
		if (m_pDoc->m_useOwnCapturedDataset)
			addWeighted(m_pDoc->m_cameraPose[0], 1.0 - alpha, m_pDoc->m_cameraPose[numOfImage - 2 - 1], alpha, 0.0, poseNEW);
		else
			addWeighted(m_pDoc->m_cameraPose[0], 1.0 - alpha, m_pDoc->m_cameraPose[numOfImage - 1], alpha, 0.0, poseNEW);
		if (m_pDoc->m_debugMode) {
			printScreen(m_pDoc->m_cameraPose[0]);
			printScreen(m_pDoc->m_cameraPose[numOfImage - 1]);
			printScreen(poseNEW);
		}

		// transform cloud points followed by 
		// transform color images w.r.t the new camera pose
		//for (int i = 0; i < numOfImage; i++) {
        Concurrency::parallel_for(0, (int) numOfImage, [&](int i) {
			point3dCloudSetPoseNEW[i] = Mat(height * width, 3, CV_64F);
			point3dCloudSetTransform(m_point3dCloudSet[i], point3dCloudSetPoseNEW[i], m_pDoc->m_cameraPose[i], poseNEW);

			colorImagesTransformed[i] = Mat(height, width, CV_64FC3, Scalar::all(0.0));
			depthImagesTransformed[i] = Mat(height, width, CV_64F, Scalar(0.0));
			weights[i] = Mat(height, width, CV_64F, Scalar(0.0));

			Mat float_image;
			m_pDoc->m_origColorImages[i].convertTo(float_image, CV_64FC3);
			for (int j = 0; j < point3dCloudSetPoseNEW[i].size().height; j++) {
				Mat curPoint = point3dCloudSetPoseNEW[i].row(j);
				if (curPoint.at<double>(0, 2) == 0.0) continue; // skip 0.0
				double depth = max(curPoint.at<double>(0, 2) * MM_PER_M, 0.0);
				double xfloat = max(min(curPoint.at<double>(0, 0) * MM_PER_M * constant / depth + center[0], 2.0 * center[0] - 1), 0.0);
				double yfloat = max(min(curPoint.at<double>(0, 1) * MM_PER_M * constant / depth + center[1], 2.0 * center[1] - 1), 0.0);
				int x = int(xfloat),
					y = int(yfloat);
				double deltaX = xfloat - x,
					deltaY = yfloat - y;
				int row = j / width;
				int col = j % width;
				double weight0 = (1 - deltaX) * (1 - deltaY),
					weight1 = deltaX * (1 - deltaY),
					weight2 = (1 - deltaX) * deltaY,
					weight3 = deltaX * deltaY;
				colorImagesTransformed[i].at<Vec3d>(y, x) += float_image.at<Vec3d>(row, col) * weight0;
				colorImagesTransformed[i].at<Vec3d>(y, x + 1) += float_image.at<Vec3d>(row, col) * weight1;
				colorImagesTransformed[i].at<Vec3d>(y + 1, x) += float_image.at<Vec3d>(row, col) * weight2;
				colorImagesTransformed[i].at<Vec3d>(y + 1, x + 1) += float_image.at<Vec3d>(row, col) * weight3;

				depthImagesTransformed[i].at<double>(y, x) += depth * weight0;
				depthImagesTransformed[i].at<double>(y, x + 1) += depth * weight1;
				depthImagesTransformed[i].at<double>(y + 1, x) += depth * weight2;
				depthImagesTransformed[i].at<double>(y + 1, x + 1) += depth * weight3;

				weights[i].at<double>(y, x) += weight0;
				weights[i].at<double>(y, x + 1) += weight1;
				weights[i].at<double>(y + 1, x) += weight2;
				weights[i].at<double>(y + 1, x + 1) += weight3;
			}

			// normalize
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					if (weights[i].at<double>(y, x) > 0) {
						colorImagesTransformed[i].at<Vec3d>(y, x) /= weights[i].at<double>(y, x);
						depthImagesTransformed[i].at<double>(y, x) /= weights[i].at<double>(y, x);
					}
				}
			}

			// fill scattered holes (median filters)
			Mat u8c3_color, u8c3_color_fix, f64_color_fix,
				u16_depth, u16_depth_fix, f64_depth_fix;
			colorImagesTransformed[i].convertTo(u8c3_color, CV_8UC3);
			depthImagesTransformed[i].convertTo(u16_depth, CV_16U);
			// color
			medianBlur(u8c3_color, u8c3_color_fix, 9);
			u8c3_color = u8c3_color_fix;
			medianBlur(u8c3_color, u8c3_color_fix, 9);
			u8c3_color = u8c3_color_fix;
			medianBlur(u8c3_color, u8c3_color_fix, 9);
			u8c3_color_fix.convertTo(f64_color_fix, CV_64FC3);
			// depth
			medianBlur(u16_depth, u16_depth_fix, 5);
			u16_depth = u16_depth_fix;
			medianBlur(u16_depth, u16_depth_fix, 5);
			u16_depth = u16_depth_fix;
			medianBlur(u16_depth, u16_depth_fix, 5);
			u16_depth_fix.convertTo(f64_depth_fix, CV_64F);

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					if (colorImagesTransformed[i].at<Vec3d>(y, x) == Vec3d(0.0, 0.0, 0.0)) {
						colorImagesTransformed[i].at<Vec3d>(y, x) = f64_color_fix.at<Vec3d>(y, x);
					}
					if (depthImagesTransformed[i].at<double>(y, x) == 0.0) {
						depthImagesTransformed[i].at<double>(y, x) = f64_depth_fix.at<double>(y, x);
					}
				}
			}

			if (m_pDoc->m_debugMode) {
				Mat u8c3_image;
				//intermediate results
				colorImagesTransformed[i].convertTo(u8c3_image, CV_8UC3);
				cv::namedWindow("transform color " + to_string(i), cv::WINDOW_AUTOSIZE);// Create a window for display.
				cv::imshow("transform color " + to_string(i), u8c3_image);

				Mat scaleDepth;
				cv::convertScaleAbs(depthImagesTransformed[i], scaleDepth, 1 / 32.0);
				scaleDepth.convertTo(u8c3_image, CV_8U);
				cv::namedWindow("transform depth " + to_string(i), cv::WINDOW_AUTOSIZE);// Create a window for display.
				cv::imshow("transform depth " + to_string(i), u8c3_image);
			}
        }); // parallel for numOfImage
		//} // for numOfImage

		// 5. fuse different images (colro and depth)
		Mat weight;
		Mat sumWeight = Mat(height, width, CV_64F, Scalar(0.0));
		Mat channel[3];
		Mat weightedChannel;
		Mat fusedDepth = Mat(height, width, CV_64F, Scalar(0.0));

		vector<Mat> sumWeightedChannel;
		sumWeightedChannel.resize(4); // 4 is for depth
		for (int ch = 0; ch < 4; ++ch) {
			sumWeightedChannel[ch] = Mat(height, width, CV_64F, Scalar(0.0));
		}

		vector<Mat> fuseChannel;
		fuseChannel.resize(3);
		Mat ones = Mat(height, width, CV_64F, Scalar(1.0));
		for (int i = 0; i < numOfImage; i++) {
			divide(ones, depthImagesTransformed[i], weight); // 1 / depth;    if input divisor is 0, output is also 0
			sumWeight += weight;
			split(colorImagesTransformed[i], channel);
			for (int ch = 0; ch < 3; ++ch) { // process channel by channel
				multiply(channel[ch], weight, weightedChannel);
				sumWeightedChannel[ch] += weightedChannel;
			}
			if (m_pDoc->m_useDofFocusInParallax) {
				multiply(depthImagesTransformed[i], weight, weightedChannel);
				sumWeightedChannel[3] += weightedChannel;
			}
		}

		for (int ch = 0; ch < 3; ++ch) {
			divide(sumWeightedChannel[ch], sumWeight, fuseChannel[ch]);
		}
		if (m_pDoc->m_useDofFocusInParallax) {
			divide(sumWeightedChannel[3], sumWeight, fusedDepth);
		}
		merge(fuseChannel, m_pDoc->m_parallaxImages[frame]);

		if (m_pDoc->m_debugMode) {
			if (m_pDoc->m_useDofFocusInParallax) {
				Mat adjfilterDepth;
				convertScaleAbs(fusedDepth, adjfilterDepth, 1.0 / 32);
				cv::namedWindow("fused depth", cv::WINDOW_AUTOSIZE);// Create a window for display.
				cv::imshow("fused depth", adjfilterDepth);
			}
		}

		// 6. depth-of-field focusing
		if (m_pDoc->m_useDofFocusInParallax) {
			//Point2i coord2d = m_pDoc->m_focusCoord;
			//Point3d coord3d = depthToCloud(coord2d, fusedDepth.at<double>(coord2d.y, coord2d.x), 0, 0); // not sure
			//Point3d coord3dNEW = point3dTransform(coord3d, m_pDoc->m_cameraPose[0], poseNEW);
			//
			//double depth = max(coord3dNEW.z * MM_PER_M, 0.0);
			//double xfloat = max(min(coord3dNEW.x * MM_PER_M * constant / depth + center[0], 2.0 * center[0] - 1), 0.0);
			//double yfloat = max(min(coord3dNEW.y * MM_PER_M * constant / depth + center[1], 2.0 * center[1] - 1), 0.0);
			//Point2i focuscoord;
			//focuscoord.x = (int)xfloat;
			//focuscoord.y = (int)yfloat
			//m_pDoc->m_parallaxImages[frame] = m_pDoc->defocusImage(m_pDoc->m_parallaxImages[frame], fusedDepth, m_pDoc->m_focusCoord);

			Mat float_depth;
			m_pDoc->m_filterDepthImages[0].convertTo(float_depth, CV_64F); // use filtered depth
			double curDepth = float_depth.at<double>(m_pDoc->m_focusCoord.y, m_pDoc->m_focusCoord.x);
			m_pDoc->m_parallaxImages[frame] = m_pDoc->defocusImage(m_pDoc->m_parallaxImages[frame], fusedDepth, curDepth);
		}

		// convert parallax image to 8UC3 type
		Mat u8c3_image;
		m_pDoc->m_parallaxImages[frame].convertTo(u8c3_image, CV_8UC3);
		m_pDoc->m_parallaxImages[frame] = u8c3_image;

		// 7. show the synthesized image
		if (m_pDoc->m_debugMode) {
			cv::namedWindow("synthesized image", cv::WINDOW_AUTOSIZE);// Create a window for display.
			cv::imshow("synthesized image", m_pDoc->m_parallaxImages[frame]);
		}
	} // for each synthesize frame

	return 1;
}


// ------------------------------------------------------------------
// compute the rotate matrix from the rotation part of camera pose
// ------------------------------------------------------------------
Mat getQuatrotateMat(Mat &q)
{
	Mat qin;
	normalize(q, qin, 1.0, NORM_L2);
	Mat dcm = Mat(3, 3, CV_64F, Scalar(0.0));
	double a0 = qin.at<double>(0, 0),
		a1 = qin.at<double>(1, 0),
		a2 = qin.at<double>(2, 0),
		a3 = qin.at<double>(3, 0);

	dcm.at<double>(0, 0) = a0 * a0 + a1 * a1 - a2 * a2 - a3 * a3;
	dcm.at<double>(0, 1) = 2 * (a1 * a2 + a0 * a3);
	dcm.at<double>(0, 2) = 2 * (a1 * a3 - a0 * a2);
	dcm.at<double>(1, 0) = 2 * (a1 * a2 - a0 * a3);
	dcm.at<double>(1, 1) = a0 * a0 - a1 * a1 + a2 * a2 - a3 * a3;
	dcm.at<double>(1, 2) = 2 * (a2 * a3 + a0 * a1);
	dcm.at<double>(2, 0) = 2 * (a1 * a3 + a0 * a2);
	dcm.at<double>(2, 1) = 2 * (a2 * a3 - a0 * a1);
	dcm.at<double>(2, 2) = a0 * a0 - a1 * a1 - a2 * a2 + a3 * a3;

	return dcm;
}


// ---------------------------------------------------------
// convert the points in depth image into 3d point set
// ---------------------------------------------------------
int ParallaxMotion::depthToCloud(Mat &depth, Mat &p3dCloud, int topleft_w, int topleft_h)
{
	int height = depth.size().height;
	int width = depth.size().width;

	int w_offset = topleft_w - center[0];
	int h_offset = topleft_h - center[1];

	Mat vecDepth = depth.reshape(height * width, 1);
	for (int h = 0; h < height; h++) {
		for (int w = 0; w < width; w++) {
			double x = (w + w_offset) * depth.at<double>(h, w) / constant / MM_PER_M;
			double y = (h + h_offset) * depth.at<double >(h, w) / constant / MM_PER_M;
			double z = depth.at<double>(h, w) / MM_PER_M;
			p3dCloud.at<double>(h * width + w, 0) = x;
			p3dCloud.at<double>(h * width + w, 1) = y;
			p3dCloud.at<double>(h * width + w, 2) = z;
		}
	}

	return 1;
}


// ---------------------------------------------------------
// map point from 2D to 3D 
// ---------------------------------------------------------
Point3d ParallaxMotion::depthToCloud(Point2i &coord, double depth, int topleft_w, int topleft_h)
{
	int w_offset = topleft_w - center[0];
	int h_offset = topleft_h - center[1];

	Point3d coord3d;
	coord3d.x = (coord.x + w_offset) * depth / constant / MM_PER_M;
	coord3d.y = (coord.y + h_offset) * depth / constant / MM_PER_M;
	coord3d.z = depth / MM_PER_M;

	return coord3d;
}

// ------------------------------------------------------------------
// transform point cloud set to the space of the new camera pose
// ------------------------------------------------------------------
int ParallaxMotion::point3dCloudSetTransform(Mat &pcloudOld, Mat &pcloudNew, Mat &poseOld, Mat &poseNew)
{
	double newRotate[4] = { poseNew.at<double>(0, 0), -poseNew.at<double>(1, 0), -poseNew.at<double>(2, 0), -poseNew.at<double>(3, 0) };
	Mat newRot = Mat(4, 1, CV_64F, &newRotate);
	double newTranslate[3] = { poseNew.at<double>(4, 0), poseNew.at<double>(5, 0), poseNew.at<double>(6, 0) };
	Mat newTrans = Mat(3, 1, CV_64F, &newTranslate);

	double oldRotate[4] = { poseOld.at<double>(0, 0), -poseOld.at<double>(1, 0), -poseOld.at<double>(2, 0), -poseOld.at<double>(3, 0) };
	Mat oldRot = Mat(4, 1, CV_64F, &oldRotate);
	double oldTranslate[3] = { poseOld.at<double>(4, 0), poseOld.at<double>(5, 0), poseOld.at<double>(6, 0) };
	Mat oldTrans = Mat(3, 1, CV_64F, &oldTranslate);

	if (m_pDoc->m_useOwnCapturedDataset) {
		Mat ones = Mat(pcloudOld.size().height, 1, CV_64F, Scalar(1.0)); // Nx1
		Mat quatrotateMat = getQuatrotateMat(oldRot); // 3x3
		Mat intermediate = (quatrotateMat * (pcloudOld.t() + oldTrans * ones.t())); //3x3 x 3xN + 3x1 x 1xN = 3xN

		// compute the new location of the point
		Mat invquatrotateMat = getQuatrotateMat(newRot).inv(); // 3x3
		pcloudNew = (invquatrotateMat * intermediate - newTrans * ones.t()).t(); // 3x3 x 3xN - 3x1 x 1xN . t() = Nx3
	}
	else {
		Mat quatrotateMat = getQuatrotateMat(oldRot);
		Mat dstMat = (quatrotateMat * pcloudOld.t()).t();
		Mat ones = Mat(dstMat.size().height, 1, CV_64F, Scalar(1.0));
		dstMat += ((oldTrans - newTrans) * ones.t()).t();

		// compute the new location of the point
		invert(getQuatrotateMat(newRot), quatrotateMat);
		pcloudNew = (quatrotateMat * dstMat.t()).t();
	}

	return 1;
}


//
Point3d	ParallaxMotion::point3dTransform(Point3d &coord3d, Mat &poseOld, Mat &poseNew)
{
	double newRotate[4] = { poseNew.at<double>(0, 0), -poseNew.at<double>(1, 0), -poseNew.at<double>(2, 0), -poseNew.at<double>(3, 0) };
	Mat newRot = Mat(4, 1, CV_64F, &newRotate);
	double newTranslate[3] = { poseNew.at<double>(4, 0), poseNew.at<double>(5, 0), poseNew.at<double>(6, 0) };
	Mat newTrans = Mat(3, 1, CV_64F, &newTranslate);

	double oldRotate[4] = { poseOld.at<double>(0, 0), -poseOld.at<double>(1, 0), -poseOld.at<double>(2, 0), -poseOld.at<double>(3, 0) };
	Mat oldRot = Mat(4, 1, CV_64F, &oldRotate);
	double oldTranslate[3] = { poseOld.at<double>(4, 0), poseOld.at<double>(5, 0), poseOld.at<double>(6, 0) };
	Mat oldTrans = Mat(3, 1, CV_64F, &oldTranslate);

	Mat point3dOld = Mat(1, 3, CV_64F, Scalar(0.0));
	point3dOld.at<double>(0, 0) = coord3d.x; 
	point3dOld.at<double>(0, 1) = coord3d.y; 
	point3dOld.at<double>(0, 2) = coord3d.z;

	Mat point3dNEW = Mat(1, 3, CV_64F, Scalar(0.0));

	if (m_pDoc->m_useOwnCapturedDataset) {
		Mat ones = Mat(point3dOld.size().height, 1, CV_64F, Scalar(1.0)); // Nx1
		Mat quatrotateMat = getQuatrotateMat(oldRot); // 3x3
		Mat intermediate = (quatrotateMat * (point3dOld.t() + oldTrans * ones.t())); //3x3 x 3xN + 3x1 x 1xN = 3xN

		// compute the new location of the point
		Mat invquatrotateMat = getQuatrotateMat(newRot).inv(); // 3x3
		point3dNEW = (invquatrotateMat * intermediate - newTrans * ones.t()).t(); // 3x3 x 3xN - 3x1 x 1xN . t() = Nx3
	}
	else {
		Mat quatrotateMat = getQuatrotateMat(oldRot);
		Mat dstMat = (quatrotateMat * point3dOld.t()).t();
		Mat ones = Mat(dstMat.size().height, 1, CV_64F, Scalar(1.0));
		dstMat += ((oldTrans - newTrans) * ones.t()).t();

		// compute the new location of the point
		invert(getQuatrotateMat(newRot), quatrotateMat);
		point3dNEW = (quatrotateMat * dstMat.t()).t();
	}

	Point3d coord3dNEW;
	coord3dNEW.x = point3dNEW.at<double>(0, 0);
	coord3dNEW.y = point3dNEW.at<double>(0, 1);
	coord3dNEW.z = point3dNEW.at<double>(0, 2);

	return coord3dNEW;
}

