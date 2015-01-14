//
// imageview.cpp
//
// The code maintaining the image view of the input images
//

#include "rgbd.h"
#include "rgbdDoc.h"
#include "imageView.h"

#define LEFT_MOUSE_DOWN		1
#define LEFT_MOUSE_DRAG		2
#define LEFT_MOUSE_UP		3
#define RIGHT_MOUSE_DOWN	4
#define RIGHT_MOUSE_DRAG	5
#define RIGHT_MOUSE_UP		6

#ifndef WIN32
#define min(a, b)	( ( (a)<(b) ) ? (a) : (b) )
#endif

static int		eventToDo;
static int		isAnEvent = 0;
static Point2i	coord;
static Point2i  coord_rightMouseDown;

ImageView::ImageView(int			x, 
						   int			y, 
						   int			w, 
						   int			h, 
						   const char*	l)
							: Fl_Gl_Window(x,y,w,h,l)
{
	m_nWindowWidth	= w;
	m_nWindowHeight	= h;

}

// ---------------------------------
// draw 
// ---------------------------------
void ImageView::draw()
{
	if(!valid())
	{
		glClearColor(0.7f, 0.7f, 0.7f, 1.0);

		// We're only using 2-D, so turn off depth 
		glDisable( GL_DEPTH_TEST );

		// Tell openGL to read from the front buffer when capturing
		// out paint strokes 
		glReadBuffer( GL_FRONT );
		ortho();
	}

	glClear( GL_COLOR_BUFFER_BIT );

	if ( m_pDoc->m_ucImage ) 
	{
		// note that both OpenGL pixel storage and the Windows BMP format
		// store pixels left-to-right, BOTTOM-to-TOP!!  thus all the fiddling
		// around with startrow.

		m_nWindowWidth=w();
		m_nWindowHeight=h();

		int drawWidth, drawHeight;
		GLvoid* bitstart;
        
		drawWidth	= std::min( m_nWindowWidth, m_pDoc->m_nWidth );
		drawHeight	= std::min( m_nWindowHeight, m_pDoc->m_nHeight );

		int	startrow	= m_pDoc->m_nHeight - drawHeight;
		if ( startrow < 0 ) 
			startrow = 0;

		bitstart = m_pDoc->m_ucImage + 3 * (m_pDoc->m_nWidth * startrow);

		// just copy image to GLwindow conceptually
		glRasterPos2i( 0, m_nWindowHeight - drawHeight );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glPixelStorei( GL_UNPACK_ROW_LENGTH, m_pDoc->m_nWidth );
		glDrawBuffer( GL_BACK );
		glDrawPixels( drawWidth, drawHeight, GL_RGB, GL_UNSIGNED_BYTE, bitstart );
	}
			
	glFlush();
}


// ---------------------------------
// redraw 
// ---------------------------------
void ImageView::refresh()
{
	redraw();
}


// ---------------------------------
// resize the window
// ---------------------------------
void ImageView::resizeWindow(int	width, 
								int	height)
{
	resize(x(), y(), width, height);
}


// ---------------------------------
// handle all events
// ---------------------------------
int ImageView::handle(int event)
{
	switch (event)
	{
	case FL_ENTER:
		redraw();
		break;
	case FL_PUSH:
		coord.x = Fl::event_x();
		coord.y = Fl::event_y();
		if (Fl::event_button() > 1) { 
			eventToDo = RIGHT_MOUSE_DOWN;
			isAnEvent = 1;
			coord_rightMouseDown.x = Fl::event_x();
			coord_rightMouseDown.y = Fl::event_y();
			cout << "right mouse clicked @ <" << coord_rightMouseDown.y << " , " << coord_rightMouseDown.x << ">" << endl;
			m_pDoc->defocusImage(coord_rightMouseDown);
			m_pDoc->m_focusCoord = coord_rightMouseDown;
		}
		else { eventToDo = LEFT_MOUSE_DOWN; }
		redraw();
		break;
	case FL_DRAG:
		redraw();
		break;
	case FL_RELEASE:
		redraw();
		break;
	case FL_MOVE:
		break;
	default:
		return 0;
		break;
	}

	return 1;
}