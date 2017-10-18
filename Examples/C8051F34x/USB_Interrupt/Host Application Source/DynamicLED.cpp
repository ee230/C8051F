// DynamicLED.cpp : implementation file
//

#include "stdafx.h"
#include "DynLED.h"
#include "DynamicLED.h"

#define ID_TIMER_START 1001

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDynamicLED

CDynamicLED::CDynamicLED()
{
	// Initialize the variables
	m_bBlink = TRUE;
	m_bBright = FALSE;
	m_nTimerInterval = 0;
	m_nPrevTimerInterval = 0;
}

CDynamicLED::~CDynamicLED()
{
}

BEGIN_MESSAGE_MAP(CDynamicLED, CStatic)
	//{{AFX_MSG_MAP(CDynamicLED)
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDynamicLED message handlers

void CDynamicLED::SetLED(CWnd *pWnd, UINT nIDColor, UINT nIDShape, int nTimerInterval)
{
	this->m_hWnd = pWnd->m_hWnd;

	// Save the color,shape and the timer interval of the control as it 
	// will be used later when resetting the LED to its previous state.
	m_nID = nIDColor;
	m_nShape = nIDShape;
	
	m_nTimerInterval = nTimerInterval;
	m_nPrevTimerInterval = nTimerInterval;

	// Kickoff the timer
	SetTimer(ID_TIMER_START,m_nTimerInterval,NULL);
}

void CDynamicLED::ResetLED(UINT nIDColor, UINT nIDShape, int nTimerInterval)
{
	// This function is used internally by the class to get the previously
	// stored values and reset the LED. Please dont use this function directly
	// from your class as it would throw a compilation error as this has been
	// declared as a private function.

	m_nID = nIDColor;
	m_nShape = nIDShape;
	m_nTimerInterval = nTimerInterval;
	SetTimer(ID_TIMER_START,m_nTimerInterval,NULL);
}

void CDynamicLED::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// I'm doing nothing here. If you want to do something here,feel free to do so
}

void CDynamicLED::OnTimer(UINT nIDEvent) 
{
	// If the timer value is zero, we dont want to do anything
	// It means that the LED is in a switched off state. So just return
	if(m_nTimerInterval==0)
		return;

	UpdateLED();
/*	// Get the Device Context
	CClientDC dc(this);
	
	// Get the rectangle of the window where we are going to draw
	CRect rcClient;
	GetClientRect(&rcClient);

	// If the pen has been selected already, then we have to delete it
	// so that it doesnt throw an assertion

	if(m_PenBright.m_hObject!=NULL)
		m_PenBright.DeleteObject();

	if(m_BrushBright.m_hObject!=NULL)
		m_BrushBright.DeleteObject();

	if(m_PenDark.m_hObject!=NULL)
		m_PenDark.DeleteObject();

	if(m_BrushDark.m_hObject!=NULL)
		m_BrushDark.DeleteObject();	

	// If the user has selected RED as the color of the LED
	if(m_nID==ID_LED_RED)
	{
		// I'm creating a light shade of red here for displaying the bright
		// LED. You can change the values to any colour that you want
		m_PenBright.CreatePen(0,1,RGB(250,0,0));
		m_BrushBright.CreateSolidBrush(RGB(250,0,0));

		// Here i'm creating a dark shade of red. You can play with the values to
		// see the effect on the LED control

		m_PenDark.CreatePen(0,1,RGB(150,0,0));
		m_BrushDark.CreateSolidBrush(RGB(150,0,0));
	}
	else if(m_nID==ID_LED_GREEN)
	{
		// If the user has selected GREEN as the color of the LED

		m_PenBright.CreatePen(0,1,RGB(0,250,0));
		m_BrushBright.CreateSolidBrush(RGB(0,250,0));

		m_PenDark.CreatePen(0,1,RGB(0,150,0));
		m_BrushDark.CreateSolidBrush(RGB(0,150,0));
	}
	else if(m_nID==ID_LED_BLUE)
	{
		// If the user has selected BLUE as the color of the LED

		m_PenBright.CreatePen(0,1,RGB(0,0,250));
		m_BrushBright.CreateSolidBrush(RGB(0,0,250));

		m_PenDark.CreatePen(0,1,RGB(0,0,150));
		m_BrushDark.CreateSolidBrush(RGB(0,0,150));
	}
	else if(m_nID==ID_LED_YELLOW)
	{
		// If the user has selected YELLOW as the color of the LED

		m_PenBright.CreatePen(0,1,RGB(200,200 ,0));
		m_BrushBright.CreateSolidBrush(RGB(200,200,0));

		m_PenDark.CreatePen(0,1,RGB(150,150,0));
		m_BrushDark.CreateSolidBrush(RGB(150,150,0));
	}


	if(m_bBright==TRUE)
	{
		// If we have to switch on the LED to display the bright colour select
		// the bright pen and brush that we have created above

		dc.SelectObject(&m_PenBright);
		dc.SelectObject(&m_BrushBright);

		m_BrushCurrent.m_hObject = m_BrushBright.m_hObject;

		m_bBright = FALSE;
	}
	else
	{
		// If we have to switch off the LED to display the dark colour select
		// the bright pen and brush that we have created above

		dc.SelectObject(&m_PenDark);
		dc.SelectObject(&m_BrushDark);

		m_BrushCurrent.m_hObject = m_BrushDark.m_hObject;

		m_bBright = TRUE;
	}

	// If the round shape has been selected for the control 
	if(m_nShape==ID_SHAPE_ROUND)
	{
		// Draw the actual colour of the LED
		dc.Ellipse(rcClient);

		// Draw a thick dark gray coloured circle
		CPen Pen;
		Pen.CreatePen(0,2,RGB(128,128,128));
		dc.SelectObject(&Pen);
		
		dc.Ellipse(rcClient);

		// Draw a thin light gray coloured circle
		Pen.DeleteObject();
		Pen.CreatePen(0,1,RGB(192,192,192));
		dc.SelectObject(&Pen);
		dc.Ellipse(rcClient);

		// Draw a white arc at the bottom
		Pen.DeleteObject();
		Pen.CreatePen(0,1,RGB(255,255,255));
		dc.SelectObject(&Pen);

		// The arc function is just to add a 3D effect for the control
		CPoint ptStart,ptEnd;
		ptStart = CPoint(rcClient.Width()/2,rcClient.bottom);
		ptEnd	= CPoint(rcClient.right,rcClient.top);

		dc.MoveTo(ptStart);
		dc.Arc(rcClient,ptStart,ptEnd);

		CBrush Brush;
		Brush.CreateSolidBrush(RGB(255,255,255));
		dc.SelectObject(&Brush);

		// Draw the actual ellipse
		dc.Ellipse(rcClient.left+4,rcClient.top+4,rcClient.left+6,rcClient.top+6);
	}
	else if(m_nShape==ID_SHAPE_SQUARE)
	{
		// If you have decided that your LED is going to look square in shape, then

		// Draw the actual rectangle
		dc.FillRect(rcClient,&m_BrushCurrent);

		// The  code below gives a 3D look to the control. It does nothing more

		// Draw the dark gray lines
		CPen Pen;
		Pen.CreatePen(0,1,RGB(128,128,128));
		dc.SelectObject(&Pen);

		dc.MoveTo(rcClient.left,rcClient.bottom);
		dc.LineTo(rcClient.left,rcClient.top);
		dc.LineTo(rcClient.right,rcClient.top);
		
		Pen.DeleteObject();

		// Draw the light gray lines
		Pen.CreatePen(0,1,RGB(192,192,192));
		dc.SelectObject(&Pen);

		dc.MoveTo(rcClient.right,rcClient.top);
		dc.LineTo(rcClient.right,rcClient.bottom);
		dc.LineTo(rcClient.left,rcClient.bottom);	
	}
*/
	CStatic::OnTimer(nIDEvent);
}

void CDynamicLED::SwitchOn()
{
	// To switch on the LED, we have to get the previously set timer interval

	m_nTimerInterval = m_nPrevTimerInterval;

	// and then reset the LED
	ResetLED(m_nID,m_nShape,m_nTimerInterval);
}

void CDynamicLED::SwitchOff()
{
	// Before switching it off, store the timer interval set by the user so that
	// we can use it later when we have to switch on the LED again

	m_nPrevTimerInterval = m_nTimerInterval;
	m_nTimerInterval = 0;
}

void CDynamicLED::LedOn()
{
	m_bBlink = FALSE;
	m_bBright = TRUE;
}

void CDynamicLED::LedOff()
{
	m_bBlink = FALSE;
	m_bBright = FALSE;
}


void CDynamicLED::UpdateLED() 
{

	// Get the Device Context
	CClientDC dc(this);
	
	// Get the rectangle of the window where we are going to draw
	CRect rcClient;
	GetClientRect(&rcClient);

	// If the pen has been selected already, then we have to delete it
	// so that it doesnt throw an assertion

	if(m_PenBright.m_hObject!=NULL)
		m_PenBright.DeleteObject();

	if(m_BrushBright.m_hObject!=NULL)
		m_BrushBright.DeleteObject();

	if(m_PenDark.m_hObject!=NULL)
		m_PenDark.DeleteObject();

	if(m_BrushDark.m_hObject!=NULL)
		m_BrushDark.DeleteObject();	

	// If the user has selected RED as the color of the LED
	if(m_nID==ID_LED_RED)
	{
		// I'm creating a light shade of red here for displaying the bright
		// LED. You can change the values to any colour that you want
		m_PenBright.CreatePen(0,1,RGB(250,0,0));
		m_BrushBright.CreateSolidBrush(RGB(250,0,0));

		// Here i'm creating a dark shade of red. You can play with the values to
		// see the effect on the LED control

		m_PenDark.CreatePen(0,1,RGB(150,0,0));
		m_BrushDark.CreateSolidBrush(RGB(150,0,0));
	}
	else if(m_nID==ID_LED_GREEN)
	{
		// If the user has selected GREEN as the color of the LED

		m_PenBright.CreatePen(0,1,RGB(0,250,0));
		m_BrushBright.CreateSolidBrush(RGB(0,250,0));

		m_PenDark.CreatePen(0,1,RGB(0,150,0));
		m_BrushDark.CreateSolidBrush(RGB(0,150,0));
	}
	else if(m_nID==ID_LED_BLUE)
	{
		// If the user has selected BLUE as the color of the LED

		m_PenBright.CreatePen(0,1,RGB(0,0,250));
		m_BrushBright.CreateSolidBrush(RGB(0,0,250));

		m_PenDark.CreatePen(0,1,RGB(0,0,150));
		m_BrushDark.CreateSolidBrush(RGB(0,0,150));
	}
	else if(m_nID==ID_LED_YELLOW)
	{
		// If the user has selected YELLOW as the color of the LED

		m_PenBright.CreatePen(0,1,RGB(200,200 ,0));
		m_BrushBright.CreateSolidBrush(RGB(200,200,0));

		m_PenDark.CreatePen(0,1,RGB(150,150,0));
		m_BrushDark.CreateSolidBrush(RGB(150,150,0));
	}

	if(m_bBright==TRUE)
	{
		// If we have to switch on the LED to display the bright colour select
		// the bright pen and brush that we have created above

		dc.SelectObject(&m_PenBright);
		dc.SelectObject(&m_BrushBright);

		m_BrushCurrent.m_hObject = m_BrushBright.m_hObject;

		if (m_bBlink)		// Check if toggle should occur
			m_bBright = FALSE;
	}
	else
	{
		// If we have to switch off the LED to display the dark colour select
		// the bright pen and brush that we have created above

		dc.SelectObject(&m_PenDark);
		dc.SelectObject(&m_BrushDark);

		m_BrushCurrent.m_hObject = m_BrushDark.m_hObject;

		if (m_bBlink)		// Check if toggle should occur
			m_bBright = TRUE;
	}

	// If the round shape has been selected for the control 
	if(m_nShape==ID_SHAPE_ROUND)
	{
		// Draw the actual colour of the LED
		dc.Ellipse(rcClient);

		// Draw a thick dark gray coloured circle
		CPen Pen;
		Pen.CreatePen(0,2,RGB(128,128,128));
		dc.SelectObject(&Pen);
		
		dc.Ellipse(rcClient);

		// Draw a thin light gray coloured circle
		Pen.DeleteObject();
		Pen.CreatePen(0,1,RGB(192,192,192));
		dc.SelectObject(&Pen);
		dc.Ellipse(rcClient);

		// Draw a white arc at the bottom
		Pen.DeleteObject();
		Pen.CreatePen(0,1,RGB(255,255,255));
		dc.SelectObject(&Pen);

		// The arc function is just to add a 3D effect for the control
		CPoint ptStart,ptEnd;
		ptStart = CPoint(rcClient.Width()/2,rcClient.bottom);
		ptEnd	= CPoint(rcClient.right,rcClient.top);

		dc.MoveTo(ptStart);
		dc.Arc(rcClient,ptStart,ptEnd);

		CBrush Brush;
		Brush.CreateSolidBrush(RGB(255,255,255));
		dc.SelectObject(&Brush);

		// Draw the actual ellipse
		dc.Ellipse(rcClient.left+4,rcClient.top+4,rcClient.left+6,rcClient.top+6);
	}
	else if(m_nShape==ID_SHAPE_SQUARE)
	{
		// If you have decided that your LED is going to look square in shape, then

		// Draw the actual rectangle
		dc.FillRect(rcClient,&m_BrushCurrent);

		// The  code below gives a 3D look to the control. It does nothing more

		// Draw the dark gray lines
		CPen Pen;
		Pen.CreatePen(0,1,RGB(128,128,128));
		dc.SelectObject(&Pen);

		dc.MoveTo(rcClient.left,rcClient.bottom);
		dc.LineTo(rcClient.left,rcClient.top);
		dc.LineTo(rcClient.right,rcClient.top);
		
		Pen.DeleteObject();

		// Draw the light gray lines
		Pen.CreatePen(0,1,RGB(192,192,192));
		dc.SelectObject(&Pen);

		dc.MoveTo(rcClient.right,rcClient.top);
		dc.LineTo(rcClient.right,rcClient.bottom);
		dc.LineTo(rcClient.left,rcClient.bottom);	
	}
}
