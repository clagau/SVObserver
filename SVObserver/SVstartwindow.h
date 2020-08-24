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
class SVStartWindow : public CDialog
{
//******************************************************************************
// Class Wizard Generated Message Map Entries
//******************************************************************************
public:
// Dialogfelddaten
	//{{AFX_DATA(SVStartWindow)
	enum { IDD = IDD_START };
	SvMc::SVDlgBitmapClass	startImage;
	//}}AFX_DATA
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(SVStartWindow)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaletteIsChanging(CWnd* pRealizeWnd);
	virtual BOOL OnInitDialog() override;
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	SVStartWindow(CWnd* pParent = nullptr);   // Standardkonstruktor
	virtual ~SVStartWindow();

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(SVStartWindow)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL
};

