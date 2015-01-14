//
// rgbdUI.h
//
// The header file for the UI part
//

#ifndef RgbdUI_h
#define RgbdUI_h

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_File_Chooser.H>		// FLTK file chooser
#include <FL/Fl_Native_File_Chooser.H>	// FLTK add-on: native file chooser
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Color_Chooser.H>

#include "rgbd.h"
#include "imageView.h"

#include <vector>

using namespace std;

const int FLT_WIDTH = 5;
const int FLT_HEIGHT = 5;

class RgbdUI {
public:
	RgbdUI();
	~RgbdUI();

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;
								
	ImageView*		    m_imgView;
	//Fl_File_Chooser*	m_chooser;
	Fl_Native_File_Chooser*	m_nativeChooser;

    vector<char*>       m_rgbNames;
	vector<string>      m_depthNames;
	string              m_poseName;
	vector<int>         m_imageIndex;

	Fl_Slider*			m_CameraFocalLengthSlider;
	Fl_Slider*			m_CameraDefocusConstSlider;
    Fl_Button*          m_LoadImagesButton;
	Fl_Button*          m_StartParallaxMotionButton;
	Fl_Button*			m_StartAdaptFocusLengthButton;
	Fl_Slider*			m_FrameNumSlider;
    Fl_Check_Button*    m_debugModeButton;
    Fl_Check_Button*    m_datasetButton;
    Fl_Button*          m_writeToVideoButton;
	Fl_Button*			m_dofFocusButton;

	// Member functions
	void				setDocument(RgbdDoc* doc);
	RgbdDoc*	        getDocument();

	void				show();
	void				resize_windows(int w, int h);

	// Interface to get and set attributes
	int					getDepth();
	void				setDepth(int depth);

	int					getFrameNum();
	void				setFrameNum(int number);

	int					getFocusFrameIndex();

	int					getDefocusConst();
	void				setDefocusConst(int number);

private:
	RgbdDoc*	m_pDoc;		// pointer to document to communicate with the document

	// All attributes here
	int		m_nFocalLength;
	int     m_nFrameNum;
	int     m_nFrameFocus;
	int		m_nDefocusConst;

	// Static class members
	static Fl_Menu_Item		menuitems[];

	static RgbdUI*	whoami(Fl_Menu_* o);

	// Helper function
	const char* fileDialog(Fl_Native_File_Chooser::Type dialogType, const char* filter, const char* title=NULL);

	// All callbacks here.  Callbacks are declared 
	// static
	static void	cb_exit(Fl_Menu_* o, void* v);
	static void	cb_about(Fl_Menu_* o, void* v);
    static void	cb_load_image(Fl_Widget* o, void* v);
	static void	cb_start_motion_button(Fl_Widget* o, void* v);
	static void	cb_start_varyFocus_button(Fl_Widget* o, void* v);
    static void cb_debug_mode_button(Fl_Check_Button* o, void* v);
    static void cb_dataset_button(Fl_Check_Button* o, void* v);
    static void	cb_write_to_video_button(Fl_Widget* o, void* v);
	static void	cb_focalLengthSlides(Fl_Widget* o, void* v);
	static void	cb_frameNumSlides(Fl_Widget* o, void* v);
	static void cb_cameraDefocusConstSlides(Fl_Widget* o, void* v);
	static void cb_dofFocus_button(Fl_Check_Button* o, void* v);
};

#endif

