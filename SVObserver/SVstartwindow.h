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

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#ifndef SVSTARTWINDOWCLASS_H
#define SVSTARTWINDOWCLASS_H

//#ifndef AFX_SVSTARTWINDOWCLASS_H__533A2501_5881_11D1_AEC4_444553540000__INCLUDED_
//#define AFX_SVSTARTWINDOWCLASS_H__533A2501_5881_11D1_AEC4_444553540000__INCLUDED_

// SVStartWindow.h : Header-Datei
//


//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// Includes which are necessary to declare or define types, constants and macros
////////////////////////////////////////////////////////////////////////////////


#include "SVMFCControls\SVDlgBitmap.h"


//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : // e.g. Base Class of module ...
// -----------------------------------------------------------------------------
// .Description : e.g. This base class capsules an Image and provides its ...
//              :
//              :
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :dd.mm.yyyy				First Implementation
//	:
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


//******************************************************************************
// Friend Declaration(s):
//******************************************************************************

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVStartWindowClass(CWnd* pParent = NULL);   // Standardkonstruktor

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	~SVStartWindowClass();

//******************************************************************************
// Operator(s):
//******************************************************************************

//******************************************************************************
// Operation(s) Of Writing Access:
//******************************************************************************

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************

//******************************************************************************
// Operation(s) Of Process;
//******************************************************************************

//******************************************************************************
// Operation(s) Of Visual Representation:
//******************************************************************************

//******************************************************************************
// Class Wizard Generated Virtual Function(s):
//******************************************************************************
public:
// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(SVStartWindowClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL
	

//******************************************************************************
// Debug Operation(s):
//******************************************************************************

//******************************************************************************
// Data Element(s):
//******************************************************************************

};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

//#endif // AFX_SVSTARTWINDOWCLASS_H__533A2501_5881_11D1_AEC4_444553540000__INCLUDED_

/**************************************************************************/
/* INCLUDE CONTROL:                                                        */
/**************************************************************************/
#endif	// SVSTARTWINDOWCLASS_H

//** EOF **

