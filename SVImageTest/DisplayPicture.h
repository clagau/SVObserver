// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : DisplayPicture.h
// * .File Name       : $Workfile:   DisplayPicture.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:07:54  $
// ******************************************************************************
#pragma  once

#pragma region Includes
#include "TriggerRecordController/IImage.h"
#pragma endregion Includes

class CDisplayPicture : public CStatic
{
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisplayPicture)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CDisplayPicture)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	CDisplayPicture();

	SvTrc::IImagePtr GetNextBuffer();

	bool UpdateDisplayBufferInfo( long bufWidth, long bufHeight, int iFormat );

	SvTrc::IImagePtr m_pImage;

	int m_ScrollY;
	int m_ScrollX;
	virtual ~CDisplayPicture();

	// Generated message map functions
};

