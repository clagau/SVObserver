//******************************************************************************
//* COPYRIGHT (c) 2009 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPictureDisplayPropPage
//* .File Name       : $Workfile:   SVPictureDisplayPropPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:28:54  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "resource.h"
#pragma endregion Includes

class SVPictureDisplayPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(SVPictureDisplayPropPage)
	DECLARE_OLECREATE_EX(SVPictureDisplayPropPage)

public:
#pragma region Constructor
	SVPictureDisplayPropPage();
	virtual ~SVPictureDisplayPropPage();
#pragma endregion Constructor
// Dialog Data
	enum { IDD = IDD_PROPPAGE_SVPICTUREDISPLAY };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
};

