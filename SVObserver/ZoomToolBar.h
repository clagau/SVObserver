//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved 
//******************************************************************************
//* .Module Name     : ZoomToolBar
//* .File Name       : $Workfile:   ZoomToolBar.h  $
//* .Description     : A Toolbar with a CEdit and a CSliderCtrl.
//*     When the slider is moved, a WM_Command Message with ID_ZOOM_SLIDER_MOVED
//*     is sent to the frameWnd.  The zoom value is proportional to the slider
//*     position.  The factor is m_SliderProp.
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 13:14:22  $
//******************************************************************************

#pragma once

class ZoomToolBar : public CToolBar
{
#pragma region Constructor
public:
	ZoomToolBar();
	virtual ~ZoomToolBar();
#pragma endregion Constructor

#pragma region Public Methods
public:
	//************************************
	// Method:    GetValue
	// Description:  Return the zoom value.
	// Returns:   double
	//************************************
	double GetValue() const;

	//************************************
	// Method:    SetValue
	// Description:  Set the zoom value and move the slider.
	// Parameter: double val
	// Returns:   void
	//************************************
	void SetValue(double val);

	//************************************
	// Method:    SetRange
	// Description:  Set the min and max value and page size for the zoom slider.
	// Parameter: double min
	// Parameter: double max
	// Parameter: double page
	// Returns:   void
	//************************************
	void SetRange(double min, double max , double page);

	//************************************
	// Method:    Update
	// Description:  Send a message with the position and update the edit control.
	// Returns:   void
	//************************************
	void Update();

	//************************************
	// Method:    DisplayNumber
	// Description:  Show the number in the edit control.
	// Parameter: int nr
	// Returns:   void
	//************************************
	void DisplayNumber(int nr);

	//************************************
	// Method:    Enable
	// Description:  Enable the slider and the static window.
	// Parameter: bool enable
	// Returns:   void
	//************************************
	void Enable(bool enable);

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHandler);
#pragma endregion Public Methods

#pragma region Member variables
protected:
	CSliderCtrl	m_wndZoomSlider;
	CStatic		m_wndZoomStatic;
	double m_value;
	CFont m_font;
	double m_SliderProp;
#pragma endregion Member variables
};

