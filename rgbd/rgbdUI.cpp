//
// rgbdUI.cpp
//
// The user interface part for the program.
//

#include <string>

#include <FL/fl_ask.H>

#include <algorithm>

#include <math.h>

#include "rgbdUI.h"
#include "rgbdDoc.h"

//#include "dialog.h"
#include <vector>
#include <iostream>

using namespace std;
/*
//------------------------------ Widget Examples -------------------------------------------------
Here is some example code for all of the widgets that you may need to add to the 
project.  You can copy and paste these into your code and then change them to 
make them look how you want.  Descriptions for all of the widgets here can be found 
in links on the fltk help session page.

//---------Window/Dialog and Menubar-----------------------------------
	
	//----To install a window--------------------------
	Fl_Window* myWindow = new Fl_Window(600, 300, "MyWindow");
		myWindow->user_data((void*)(this));	// record self to be used by static callback functions
		
		// install menu bar
		myMenubar = new Fl_Menu_Bar(0, 0, 600, 25);
		Fl_Menu_Item RgbdUI::myMenuItems[] = {
			{ "&File",		0, 0, 0, FL_SUBMENU },
				{ "&Load...",	FL_ALT + 'l', (Fl_Callback *)RgbdUI::cb_load },
				{ "&Save...",	FL_ALT + 's', (Fl_Callback *)RgbdUI::cb_save }.
				{ "&Quit",			FL_ALT + 'q', (Fl_Callback *)RgbdUI::cb_exit },
				{ 0 },
			{ "&Edit",		0, 0, 0, FL_SUBMENU },
				{ "&Copy",FL_ALT + 'c', (Fl_Callback *)RgbdUI::cb_copy, (void *)COPY },
				{ "&Cut",	FL_ALT + 'x', (Fl_Callback *)RgbdUI::cb_cut, (void *)CUT },
				{ "&Paste",	FL_ALT + 'v', (Fl_Callback *)RgbdUI::cb_paste, (void *)PASTE },
				{ 0 },
			{ "&Help",		0, 0, 0, FL_SUBMENU },
				{ "&About",	FL_ALT + 'a', (Fl_Callback *)RgbdUI::cb_about },
				{ 0 },
			{ 0 }
		};
		myMenubar->menu(myMenuItems);
    m_mainWindow->end();

	//----The window callback--------------------------
	// One of the callbacks
	void RgbdUI::cb_load(Fl_Menu_* o, void* v) 
	{	
		RgbdDoc *pDoc=whoami(o)->getDocument();

		char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName() );
		if (newfile != NULL) {
			pDoc->loadImage(newfile);
		}
	}


//------------Slider---------------------------------------

	//----To install a slider--------------------------
	Fl_Value_Slider * mySlider = new Fl_Value_Slider(10, 80, 300, 20, "My Value");
	mySlider->user_data((void*)(this));	// record self to be used by static callback functions
	mySlider->type(FL_HOR_NICE_SLIDER);
    mySlider->labelfont(FL_COURIER);
    mySlider->labelsize(12);
	mySlider->minimum(1);
	mySlider->maximum(40);
	mySlider->step(1);
	mySlider->value(m_nMyValue);
	mySlider->align(FL_ALIGN_RIGHT);
	mySlider->callback(cb_MyValueSlides);

	//----The slider callback--------------------------
	void RgbdUI::cb_MyValueSlides(Fl_Widget* o, void* v)
	{
		((RgbdUI*)(o->user_data()))->m_nMyValue=int( ((Fl_Slider *)o)->value() ) ;
	}
	

//------------Choice---------------------------------------
	
	//----To install a choice--------------------------
	Fl_Choice * myChoice = new Fl_Choice(50,10,150,25,"&myChoiceLabel");
	myChoice->user_data((void*)(this));	 // record self to be used by static callback functions
	Fl_Menu_Item RgbdUI::myChoiceMenu[3+1] = {
	  {"one",FL_ALT+'p', (Fl_Callback *)RgbdUI::cb_myChoice, (void *)ONE},
	  {"two",FL_ALT+'l', (Fl_Callback *)RgbdUI::cb_myChoice, (void *)TWO},
	  {"three",FL_ALT+'c', (Fl_Callback *)RgbdUI::cb_myChoice, (void *)THREE},
	  {0}
	};
	myChoice->menu(myChoiceMenu);
	myChoice->callback(cb_myChoice);
	
	//-----The choice callback-------------------------
	void RgbdUI::cb_myChoice(Fl_Widget* o, void* v)
	{
		RgbdUI* pUI=((RgbdUI *)(o->user_data()));
		RgbdDoc* pDoc=pUI->getDocument();

		int type=(int)v;

		pDoc->setMyType(type);
	}


//------------Button---------------------------------------

	//---To install a button---------------------------
	Fl_Button* myButton = new Fl_Button(330,220,50,20,"&myButtonLabel");
	myButton->user_data((void*)(this));   // record self to be used by static callback functions
	myButton->callback(cb_myButton);

	//---The button callback---------------------------
	void RgbdUI::cb_myButton(Fl_Widget* o, void* v)
	{
		RgbdUI* pUI=((RgbdUI*)(o->user_data()));
		RgbdDoc* pDoc = pUI->getDocument();
		pDoc->startPainting();
	}


//---------Light Button------------------------------------
	
	//---To install a light button---------------------
	Fl_Light_Button* myLightButton = new Fl_Light_Button(240,10,150,25,"&myLightButtonLabel");
	myLightButton->user_data((void*)(this));   // record self to be used by static callback functions
	myLightButton->callback(cb_myLightButton);

	//---The light button callback---------------------
	void RgbdUI::cb_myLightButton(Fl_Widget* o, void* v)
	{
		RgbdUI *pUI=((RgbdUI*)(o->user_data()));

		if (pUI->myBool==TRUE) pUI->myBool=FALSE;
		else pUI->myBool=TRUE;
	}

//----------Int Input--------------------------------------

    //---To install an int input-----------------------
	Fl_Int_Input* myInput = new Fl_Int_Input(200, 50, 5, 5, "&My Input");
	myInput->user_data((void*)(this));   // record self to be used by static callback functions
	myInput->callback(cb_myInput);

	//---The int input callback------------------------
	void RgbdUI::cb_myInput(Fl_Widget* o, void* v)
	{
		((RgbdUI*)(o->user_data()))->m_nMyInputValue=int( ((Fl_Int_Input *)o)->value() );
	}

//------------------------------------------------------------------------------------------------
*/

//------------------------------------- Help Functions --------------------------------------------

//------------------------------------------------------------
// This returns the UI, given the menu item.  It provides a
// link from the menu items to the UI
//------------------------------------------------------------
RgbdUI* RgbdUI::whoami(Fl_Menu_* o)	
{
	return ( (RgbdUI*)(o->parent()->user_data()) );
}

//------------------------------------------------------------------------
// This displays a modal file chooser with a native look-and-feel.
// The available dialog types are:
//   BROWSE_DIRECTORY       - Open a single directory
//   BROWSE_FILE            - Open a single file
//   BROWSE_MULTI_DIRECTORY - Open directories, allowing the user to
//                            select more than one at a time
//   BROWSE_MULTI_FILE      - Open files, allowing the user to select
//                            more than one at a time
//   BROWSE_SAVE_DIRECTORY  - Save a directory
//   BROWSE_SAVE_FILE       - Save a file
//
// The filter limits the displayed files. See cb_load_image for an example.
// title is optional, use NULL for the OS default title.
// The return value is the filepath.
//------------------------------------------------------------------------
const char* RgbdUI::fileDialog(Fl_Native_File_Chooser::Type dialogType, const char* filter, const char* title) {
	if(!m_nativeChooser)
		m_nativeChooser = new Fl_Native_File_Chooser(dialogType);
	else
		m_nativeChooser->type(dialogType);
	m_nativeChooser->filter(filter);
	m_nativeChooser->title(title);
	int ret = m_nativeChooser->show();
	if(ret == -1 ||	ret == 1) {
		//error or cancel respectively
		return NULL;
	}

    //std::cout << m_nativeChooser->count() << std::endl;

    if (m_nativeChooser->count() == 0) {
        return NULL;
    }
    else {
        // clear previous images and names
        RgbdDoc *pDoc = getDocument();
        pDoc->m_pUI->m_depthNames.clear();
        pDoc->m_pUI->m_rgbNames.clear();
		pDoc->m_pUI->m_imageIndex.clear();
        pDoc->m_cameraPose.clear();
        pDoc->m_filterDepthImages.clear();
        pDoc->m_origDepthImages.clear();
        pDoc->m_origColorImages.clear();
		pDoc->m_parallaxImages.clear();
		pDoc->m_varyFocusLengthImages.clear();
    }

    for (int i = 0; i < m_nativeChooser->count(); ++i) {
		if (i == 0) {
			string pcdname(m_nativeChooser->filename(i));
			string path = pcdname.substr(0, pcdname.find_last_of("\\/"));
			string file = pcdname.substr(pcdname.find_last_of("\\/") + 1, string::npos);
			string pathNEW = path.substr(0, path.find_last_of("\\/") + 1);
			string fileNEW = file.substr(0, file.find_last_of('_'));
			m_poseName = pathNEW + fileNEW + ".pose";;   // compose the .pose filename
		}
		m_rgbNames.push_back((char*)m_nativeChooser->filename(i));
		string depthname(m_nativeChooser->filename(i));
		string depthnameNEW = depthname.substr(0, depthname.find_last_of('.'));
		string imageIndex = depthnameNEW.substr(depthnameNEW.find_last_of('_') + 1, string::npos);
		m_imageIndex.push_back(atoi(imageIndex.c_str()));
		depthnameNEW += "_depth.png";  // compose the depth image filename
		m_depthNames.push_back(depthnameNEW);
    }

	return m_nativeChooser->filename(0);
}

//--------------------------------- Callback Functions --------------------------------------------

//------------------------------------------------------------------
// Brings up a file chooser and then loads the chosen image
// This is called by the UI when the load image menu item is chosen
//------------------------------------------------------------------
void RgbdUI::cb_load_image(Fl_Widget* o, void* v) 
{
	RgbdDoc *pDoc = ((RgbdUI*)(o->user_data()))->getDocument();

	const char* filename = pDoc->m_pUI->fileDialog( Fl_Native_File_Chooser::BROWSE_MULTI_FILE,
													"Image File (*.png; *.jpg, *.bmp)\t*.{png,jpg,bmp}");

	if (pDoc->m_pUI->m_rgbNames.size() > 0) {
		pDoc->loadImage();
		pDoc->loadCameraPose();
	}
}


////------------------------------------------------------------------
//// Brings up a file chooser and then saves the painted image
//// This is called by the UI when the save image menu item is chosen
////------------------------------------------------------------------
//void RgbdUI::cb_save_image(Fl_Menu_* o, void* v) 
//{
//	RgbdDoc *pDoc=whoami(o)->getDocument();
//	/*pDoc->m_pUI->m_chooser = new Fl_File_Chooser(".", "*.png\t*.jpg", Fl_File_Chooser::CREATE, "Save");
//    pDoc->m_pUI->m_chooser->show();
//    while(pDoc->m_pUI->m_chooser->shown())
//        { Fl::wait(); }*/
//	
//	const char* filename = pDoc->m_pUI->fileDialog( Fl_Native_File_Chooser::BROWSE_SAVE_FILE,
//													"PNG Image File (*.png)\t*.png\nJPEG Image File (*.jpg)\t*.jpg");
//	if(filename) {
//		std::string strFileName = (std::string)filename;
//		std::string ext;
//		int quality = 95;
//		switch(pDoc->m_pUI->m_nativeChooser->filter_value()) {
//			case 0:	ext = ".png"; break;
//			case 1:	ext = ".jpg"; break;
//		}
//#ifdef WIN32
//		char szExt[_MAX_EXT];
//		_splitpath_s(strFileName.c_str(), NULL,0, NULL,0, NULL, 0, szExt,_MAX_EXT);
//		if (_stricmp(szExt,".jpg") && _stricmp(szExt,".png")) {
//			strFileName += ext;
//		}
//		else
//			ext = szExt;
//#endif
//
//#if defined(WIN32) || defined(__WIN32) || defined(__WIN32__) || defined(_WIN32)
//		if(!_strcmpi(ext.c_str(),".jpg")) {
//#else
//		if(!strcasecmp(ext.c_str(),".jpg")) {
//#endif		
//			Dialog2 x(0,0,0,0,"ok");
//			quality = x.getValue();
//		}
//		pDoc->saveImage(strFileName.c_str(), ext.c_str(), quality);
//	}
//	/*const char *szFileName = pDoc->m_pUI->m_chooser->value();
//	if (szFileName) {
//		std::string strFileName = szFileName;
//		std::string ext = (std::string)pDoc->m_pUI->m_chooser->filter();
//		ext = ext.substr(1, ext.length() - 1);
//		#ifdef WIN32
//			char szExt[_MAX_EXT];
//			_splitpath_s(strFileName.c_str(), NULL,0, NULL,0, NULL, 0, szExt,_MAX_EXT);
//			// If user didn't type supported ext, add default one.
//			if (_stricmp(szExt,".jpg") && _stricmp(szExt,".png")) {
//				strFileName += ext;
//			}
//			else
//				ext = szExt;
//		#endif
//		int quality = 95;
//		if (!strcmp(ext.c_str(),".jpg")){
//			Dialog2 x(0,0,0,0,"ok");
//			quality = x.getValue();
//		}
//		pDoc->saveImage(strFileName.c_str(), ext.c_str(), quality);
//	}*/
//}

////-------------------------------------------------------------
//// Brings up the paint dialog
//// This is called by the UI when the brushes menu item
//// is chosen
////-------------------------------------------------------------
//void RgbdUI::cb_brushes(Fl_Menu_* o, void* v) 
//{
//	whoami(o)->m_cameraDialog->show();
//}

//------------------------------------------------------------
// Causes the Rgbd program to exit
// Called by the UI when the quit menu item is chosen
//------------------------------------------------------------
void RgbdUI::cb_exit(Fl_Menu_* o, void* v) 
{
	whoami(o)->m_mainWindow->hide();
	//whoami(o)->m_cameraDialog->hide();

}



//-----------------------------------------------------------
// Brings up an about dialog box
// Called by the UI when the about menu item is chosen
//-----------------------------------------------------------
void RgbdUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("RGBD FLTK version for CSE 557 final project");
}

////------- UI should keep track of the current for all the controls for answering the query from Doc ---------
////-------------------------------------------------------------
//// Sets the type of brush to use to the one chosen in the brush 
//// choice.  
//// Called by the UI when a brush is chosen in the brush choice
////-------------------------------------------------------------
//void RgbdUI::cb_brushChoice(Fl_Widget* o, void* v)
//{
//	RgbdUI* pUI=((RgbdUI *)(o->user_data()));
//	RgbdDoc* pDoc=pUI->getDocument();
//
//	int type=(int)v;
//}

//------------------------------------------------------------
// Generate parallax motion video
// Called by the UI when the clear canvas button is pushed
//------------------------------------------------------------
void RgbdUI::cb_start_motion_button(Fl_Widget* o, void* v)
{
	RgbdDoc * pDoc = ((RgbdUI*)(o->user_data()))->getDocument();

	pDoc->doMotion();
}

//------------------------------------------------------------
// Generate adaptive focus image stream
// Called by the UI when the clear canvas button is pushed
//------------------------------------------------------------
void RgbdUI::cb_start_varyFocus_button(Fl_Widget* o, void* v)
{
	RgbdDoc * pDoc = ((RgbdUI*)(o->user_data()))->getDocument();

	pDoc->doAdaptFocalLength();
}

void RgbdUI::cb_debug_mode_button(Fl_Check_Button* o, void* v)
{
    RgbdDoc * pDoc = ((RgbdUI*)(o->user_data()))->getDocument();

    pDoc->m_debugMode = o->value() ? true : false;
}

void RgbdUI::cb_dataset_button(Fl_Check_Button* o, void* v)
{
    RgbdDoc * pDoc = ((RgbdUI*)(o->user_data()))->getDocument();

    pDoc->m_useOwnCapturedDataset = o->value() ? true : false;
}

void RgbdUI::cb_dofFocus_button(Fl_Check_Button* o, void* v)
{
	RgbdDoc * pDoc = ((RgbdUI*)(o->user_data()))->getDocument();

	pDoc->m_useDofFocusInParallax = o->value() ? true : false;
}

void RgbdUI::cb_write_to_video_button(Fl_Widget* o, void* v)
{
	RgbdDoc * pDoc = ((RgbdUI*)(o->user_data()))->getDocument();

	pDoc->writeToVideo();
}

//-----------------------------------------------------------
// Updates the brush size to use from the value of the size
// slider
// Called by the UI when the size slider is moved
//-----------------------------------------------------------
void RgbdUI::cb_focalLengthSlides(Fl_Widget* o, void* v)
{
	((RgbdUI*)(o->user_data()))->m_nFrameFocus = int(((Fl_Slider *)o)->value());
	RgbdDoc *pDoc = ((RgbdUI*)(o->user_data()))->getDocument();
	pDoc->showAdaptFocusImage();
}



void RgbdUI::cb_frameNumSlides(Fl_Widget* o, void* v)
{
	((RgbdUI*)(o->user_data()))->m_nFrameNum = int(((Fl_Slider *)o)->value());
	RgbdDoc *pDoc = ((RgbdUI*)(o->user_data()))->getDocument();
    if (pDoc->m_parallaxImages.size() == NUM_OF_FRAMES) {
        pDoc->showParallaxImage();
    }
}


//
void RgbdUI::cb_cameraDefocusConstSlides(Fl_Widget* o, void* v)
{
	((RgbdUI*)(o->user_data()))->m_nDefocusConst = int(((Fl_Slider *)o)->value());
}

//---------------------------------- per instance functions --------------------------------------

//------------------------------------------------
// Return the RgbdDoc used
//------------------------------------------------
RgbdDoc* RgbdUI::getDocument()
{
	return m_pDoc;
}

//------------------------------------------------
// Draw the main window
//------------------------------------------------
void RgbdUI::show() {
	m_mainWindow->show();
	m_imgView->show();
}

//------------------------------------------------
// Change the paint and original window sizes to 
// w by h
//------------------------------------------------
void RgbdUI::resize_windows(int w, int h) {
	m_imgView->size(w,h);
}

//------------------------------------------------ 
// Set the RgbdDoc used by the UI to 
// communicate with the brushes 
//------------------------------------------------
void RgbdUI::setDocument(RgbdDoc* doc)
{
	m_pDoc = doc;

	m_imgView->m_pDoc = doc;
}

//------------------------------------------------
// Return the brush size
//------------------------------------------------
int RgbdUI::getDepth()
{
	return m_nFocalLength;
}

//-------------------------------------------------
// Set the brush size
//-------------------------------------------------
void RgbdUI::setDepth( int size )
{
	m_nFocalLength=size;

	if (size<=40) 
		m_CameraFocalLengthSlider->value(m_nFocalLength);
}

//
int RgbdUI::getFocusFrameIndex() 
{
	return m_nFrameFocus;
}

//------------------------------------------------
// Return the brush size
//------------------------------------------------
int RgbdUI::getFrameNum()
{
	return m_nFrameNum;
}

//-------------------------------------------------
// Set the brush size
//-------------------------------------------------
void RgbdUI::setFrameNum(int number)
{
	m_nFrameNum = number;

	if (number >= 100)
		m_FrameNumSlider->value(m_nFrameNum);
}

//
int	RgbdUI::getDefocusConst() 
{
	return m_nDefocusConst;
}

//
void RgbdUI::setDefocusConst(int number)
{
	m_nDefocusConst = number;

	if (number > 10) 
		m_CameraDefocusConstSlider->value(m_nDefocusConst);
}
// Main menu definition
Fl_Menu_Item RgbdUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		//{ "&Load Image...",	FL_ALT + 'l', (Fl_Callback *)RgbdUI::cb_load_image },
		//{ "&Save Video...",	FL_ALT + 's', (Fl_Callback *)RgbdUI::cb_save_image },
		//{ "&Camera Control...",	FL_ALT + 'b', (Fl_Callback *)RgbdUI::cb_brushes },
		
		{ "&Quit",			FL_ALT + 'q', (Fl_Callback *)RgbdUI::cb_exit },
		{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
		{ "&About",	FL_ALT + 'a', (Fl_Callback *)RgbdUI::cb_about },
		{ 0 },

	{ 0 }
};

// Brush choice menu definition
//Fl_Menu_Item RgbdUI::brushTypeMenu[NUM_BRUSH_TYPE+1] = {
//  {"Points",			FL_ALT+'p', (Fl_Callback *)RgbdUI::cb_brushChoice, (void *)BRUSH_POINTS},
//  {"Lines",				FL_ALT+'l', (Fl_Callback *)RgbdUI::cb_brushChoice, (void *)BRUSH_LINES},
//  {"Circles",			FL_ALT+'c', (Fl_Callback *)RgbdUI::cb_brushChoice, (void *)BRUSH_CIRCLES},
//  {"Scattered Points",	FL_ALT+'q', (Fl_Callback *)RgbdUI::cb_brushChoice, (void *)BRUSH_SCATTERED_POINTS},
//  {"Scattered Lines",	FL_ALT+'m', (Fl_Callback *)RgbdUI::cb_brushChoice, (void *)BRUSH_SCATTERED_LINES},
//  {"Scattered Circles",	FL_ALT+'d', (Fl_Callback *)RgbdUI::cb_brushChoice, (void *)BRUSH_SCATTERED_CIRCLES},
//  {0}
//};



//----------------------------------------------------
// Constructor.  Creates all of the widgets.
// Add new widgets here
//----------------------------------------------------
RgbdUI::RgbdUI() : m_nativeChooser(NULL) {
	// Create the main window
    m_mainWindow = new Fl_Window(900, 480, "Rgbd");
    {
        m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
        // install menu bar
        /*m_menubar = new Fl_Menu_Bar(0, 0, 600, 25);
        m_menubar->menu(menuitems);*/

		m_nFocalLength = 1;
		m_nFrameNum = 1;
		m_nDefocusConst = 4;
			
        // Create a group that will hold two sub windows inside the main
        // window
        Fl_Group* group = new Fl_Group(0, 0, 900, 480);
        {
            // install original view window
            m_imgView = new ImageView(260, 0, 640, 480, "This is the image viewer");//300jon
            m_imgView->box(FL_DOWN_FRAME);
            //m_imgView->deactivate();

            // Add button for debug mode
            m_debugModeButton = new Fl_Check_Button(10, 5, 25, 25, "Debug Mode");
            m_debugModeButton->user_data((void*)(this));
            m_debugModeButton->callback((Fl_Callback*)cb_debug_mode_button);

            // Add button for debug mode
            m_datasetButton = new Fl_Check_Button(120, 5, 25, 25, "Use Own Data");
            m_datasetButton->user_data((void*)(this));
            m_datasetButton->callback((Fl_Callback*)cb_dataset_button);

            // Add button for loading images
            m_LoadImagesButton = new Fl_Button(10, 35, 150, 25, "&Load Images");
            m_LoadImagesButton->user_data((void*)(this));
            m_LoadImagesButton->callback(cb_load_image);

            // Add button for start processing
			m_StartParallaxMotionButton = new Fl_Button(10, 85, 150, 25, "Start Parallax");
            m_StartParallaxMotionButton->user_data((void*)(this));
            m_StartParallaxMotionButton->callback(cb_start_motion_button);

			// Add button for using depth-of-field focusing in parallax
			m_dofFocusButton = new Fl_Check_Button(165, 85, 25, 25, "Dep-of-field");
			m_dofFocusButton->user_data((void*)(this));
			m_dofFocusButton->callback((Fl_Callback*)cb_dofFocus_button);

            // Add button for write to video
            m_writeToVideoButton = new Fl_Button(10, 115, 150, 25, "&Write to video");
            m_writeToVideoButton->user_data((void*)(this));
            m_writeToVideoButton->callback(cb_write_to_video_button);

			// Add frame number slider to the dialog 
			m_FrameNumSlider = new Fl_Value_Slider(10, 150, 150, 25, "Frame Index");
			m_FrameNumSlider->user_data((void*)(this));	// record self to be used by static callback functions
			m_FrameNumSlider->type(FL_HOR_NICE_SLIDER);
			m_FrameNumSlider->labelfont(FL_COURIER);
			m_FrameNumSlider->labelsize(12);
			m_FrameNumSlider->minimum(1);
			m_FrameNumSlider->maximum(NUM_OF_FRAMES);
			m_FrameNumSlider->step(1);
			m_FrameNumSlider->value(m_nFrameNum);
			m_FrameNumSlider->align(FL_ALIGN_RIGHT);
			m_FrameNumSlider->callback(cb_frameNumSlides);

			// Add button for start processing
			m_StartAdaptFocusLengthButton = new Fl_Button(10, 190, 150, 25, "&StartFocusAdaption");
			m_StartAdaptFocusLengthButton->user_data((void*)(this));
			m_StartAdaptFocusLengthButton->callback(cb_start_varyFocus_button);

			// Add focal length slider to the dialog 
			m_CameraDefocusConstSlider = new Fl_Value_Slider(10, 225, 150, 25, "Defocus Const.");
			m_CameraDefocusConstSlider->user_data((void*)(this));	// record self to be used by static callback functions
			m_CameraDefocusConstSlider->type(FL_HOR_NICE_SLIDER);
			m_CameraDefocusConstSlider->labelfont(FL_COURIER);
			m_CameraDefocusConstSlider->labelsize(12);
			m_CameraDefocusConstSlider->minimum(1);
			m_CameraDefocusConstSlider->maximum(10);
			m_CameraDefocusConstSlider->step(1);
			m_CameraDefocusConstSlider->value(m_nDefocusConst);
			m_CameraDefocusConstSlider->align(FL_ALIGN_RIGHT);
			m_CameraDefocusConstSlider->callback(cb_cameraDefocusConstSlides);

			// Add focal length slider to the dialog 
			m_CameraFocalLengthSlider = new Fl_Value_Slider(10, 260, 150, 25, "Focal Length");
			m_CameraFocalLengthSlider->user_data((void*)(this));	// record self to be used by static callback functions
			m_CameraFocalLengthSlider->type(FL_HOR_NICE_SLIDER);
			m_CameraFocalLengthSlider->labelfont(FL_COURIER);
			m_CameraFocalLengthSlider->labelsize(12);
			m_CameraFocalLengthSlider->minimum(1);
			m_CameraFocalLengthSlider->maximum(30);
			m_CameraFocalLengthSlider->step(1);
			m_CameraFocalLengthSlider->value(m_nFocalLength);
			m_CameraFocalLengthSlider->align(FL_ALIGN_RIGHT);
			m_CameraFocalLengthSlider->callback(cb_focalLengthSlides);
        }
        group->end();
        
        Fl_Group::current()->resizable(group);
    }
    m_mainWindow->end();	

}

RgbdUI::~RgbdUI()
{
	if(m_nativeChooser) delete m_nativeChooser;
}


