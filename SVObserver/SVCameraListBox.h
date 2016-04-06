//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCameraListBox
//* .File Name       : $Workfile:   SVCameraListBox.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:49:56  $
//******************************************************************************

#pragma once

class SVCameraListBox : public CListBox
{
public:
	SVCameraListBox();
	virtual ~SVCameraListBox();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVCameraListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL


	// Generated message map functions
protected:
	//{{AFX_MSG(SVCameraListBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
