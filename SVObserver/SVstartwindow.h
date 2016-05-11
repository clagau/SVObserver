//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStartWindow
//* .File Name       : $Workfile:   SVstartwindow.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:09:22  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVMFCControls\SVDlgBitmap.h"
#pragma endregion Includes

////////////////////////////////////////////////////////////////////////////////
// .Title       : // e.g. Base Class of module ...
// -----------------------------------------------------------------------------
// .Description : e.g. This base class capsules an Image and provides its ...
////////////////////////////////////////////////////////////////////////////////
class SVStartWindowClass : public CDialog
{
//******************************************************************************
// Class Wizard Generated Message Map Entries
//******************************************************************************
public:
// Dialogfelddaten
	//{{AFX_DATA(SVStartWindowClass)
	enum { IDD = IDD_START };
	SvMc::SVDlgBitmapClass	startImage;
	//}}AFX_DATA
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(SVStartWindowClass)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaletteIsChanging(CWnd* pRealizeWnd);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	SVStartWindowClass(CWnd* pParent = nullptr);   // Standardkonstruktor
	virtual ~SVStartWindowClass();

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(SVStartWindowClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL
};

