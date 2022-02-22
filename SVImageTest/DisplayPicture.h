// ******************************************************************************
// * COPYRIGHT (c) 2006 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "ObjectInterfaces/ITRCImage.h"
#pragma endregion Includes

class DisplayPicture : public CStatic
{
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DisplayPicture)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(DisplayPicture)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	DisplayPicture();

	SvOi::ITRCImagePtr GetNextBuffer();

	bool UpdateDisplayBufferInfo( long bufWidth, long bufHeight, int iFormat );

	SvOi::ITRCImagePtr m_pImage;

	virtual ~DisplayPicture();

	// Generated message map functions
};

