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

class SVPictureDisplayPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(SVPictureDisplayPropPage)
	DECLARE_OLECREATE_EX(SVPictureDisplayPropPage)

public:
#pragma region Constructor
	SVPictureDisplayPropPage();
#pragma endregion Constructor
// Dialog Data
	enum { IDD = IDD_PROPPAGE_SVPICTUREDISPLAY };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVPictureDisplay\SVPictureDisplayPropPage.h_v  $
 * 
 *    Rev 1.0   26 Jun 2014 16:28:54   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Mar 2010 13:25:28   jspila
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  1
 * SCR Title:  Initial Check-in of Prototype
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/