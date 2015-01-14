//
// rgbd.cpp
//
// The main driver program for the other parts. We have two major components,
// UI and Doc.
// They do have a link to each other as their member such that they can 
// communicate.
//

#include <stdio.h>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Shared_Image.H>

#include "rgbdUI.h"
#include "rgbdDoc.h"

#include "opencv2\opencv.hpp"

RgbdUI *rgbdUI;
RgbdDoc *rgbdDoc;

int main(int	argc, 
		 char**	argv) 
{
	rgbdDoc = new RgbdDoc();

	// Create the UI
	rgbdUI = new RgbdUI();

	// Set the impDoc which is used as the bridge between UI and brushes
	rgbdUI->setDocument(rgbdDoc);
	rgbdDoc->setUI(rgbdUI);

	Fl::visual(FL_DOUBLE|FL_INDEX);

	rgbdUI->show();

	return Fl::run();
}