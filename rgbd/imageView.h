//
// imageview.h
//
// The header file for image view of the input images
//

#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <FL/Fl.H>

#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <stdlib.h>

class RgbdDoc;

class ImageView : public Fl_Gl_Window
{
public:
	ImageView(int x, int y, int w, int h, const char *l);
	
	void draw();
	void refresh();

	void resizeWindow(int width, int height);

	int	 handle(int event);

	RgbdDoc*	m_pDoc;

private:
	int	m_nWindowWidth,
		m_nWindowHeight,
		m_nDrawWidth,
		m_nDrawHeight,
		m_nStartRow,
		m_nEndRow,
		m_nStartCol,
		m_nEndCol;
		
};

#endif

