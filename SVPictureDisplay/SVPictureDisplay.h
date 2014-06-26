//******************************************************************************
//* COPYRIGHT (c) 2009 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPictureDisplay
//* .File Name       : $Workfile:   SVPictureDisplay.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:28:04  $
//******************************************************************************

#pragma once

#pragma region Includes
#if !defined( __AFXCTL_H__ )
#error "include 'afxctl.h' before including this file"
#endif

#include "resource.h"       // main symbols
#pragma endregion Includes

#pragma region Declarations
extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;
#pragma endregion Declarations

class SVPictureDisplayApp : public COleControlModule
{
#pragma region Public Methods
public:
	BOOL InitInstance();
	int ExitInstance();
#pragma endregion Public Methods
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVPictureDisplay\SVPictureDisplay.h_v  $
 * 
 *    Rev 1.0   26 Jun 2014 16:28:04   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Mar 2010 13:25:22   jspila
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  1
 * SCR Title:  Initial Check-in of Prototype
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/