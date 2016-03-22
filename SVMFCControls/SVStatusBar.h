//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStatusBar
//* .File Name       : $Workfile:   SVStatusBar.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:13:48  $
//******************************************************************************

#pragma once

#include "SVOResource\resource.h"

static UINT indicators[] =
{
	ID_SEPARATOR,
	ID_INDICATOR_INFO,
	ID_INDICATOR_MODE,
	ID_INDICATOR_FBWF, ///< for file based write filter information
	ID_INDICATOR_USER,
	ID_INDICATOR_GIGE_LICENSE_ERROR,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

class SVStatusBar : public CStatusBar  
{
public:
	SVStatusBar();
	virtual ~SVStatusBar();
	// sets information for the file based write filter
	void SetFbwfInfo(UINT PaneIndex,bool FbwfAvailable, bool FbwfActive, bool FbwfChanging);
	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	COLORREF m_lModeBKColor;
	COLORREF m_lModeForeColor;
	COLORREF m_lErrorBKColor;
	COLORREF m_lErrorForeColor;
	COLORREF m_lFbwfBKColor;
	COLORREF m_lFbwfForeColor;
	CString m_strModeText;
	CString m_strErrorText;
	CString m_strFbwfText; ///< for file based write filter information

};

