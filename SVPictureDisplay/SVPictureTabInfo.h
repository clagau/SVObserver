//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPictureTabInfo
//* .File Name       : $Workfile:   SVPictureTabInfo.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:28:54  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <boost/config.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include "SVPictureDialog.h"
#pragma endregion Includes

#pragma region Declarations
typedef std::shared_ptr<SVPictureDialog> SVPictureDialogRef;
#pragma endregion Declarations

class SVPictureTabInfo
{
public:
#pragma region Constructor
	SVPictureTabInfo();
	// copy constructor
	SVPictureTabInfo(const SVPictureTabInfo& p_rhs);
	~SVPictureTabInfo();
#pragma endregion Constructor

#pragma region Public Methods
	const SVPictureTabInfo& operator=(const SVPictureTabInfo& p_rhs);
	int GetTabIndex() const;
	void SetTabIndex(int);
	SVPictureDialog* GetPictureDialog() const;
	std::basic_string<TCHAR> GetTabName() const;
	void SetTabName( std::basic_string<TCHAR> name );
	SVPictureDialogRef GetPictureDialogRef() const;
	void SetPictureDialogRef( SVPictureDialogRef p );
#pragma endregion Public Methods

#pragma region Member Variables
private:
	std::basic_string<TCHAR> m_tabName;
	// shared pointer to dialog
	SVPictureDialogRef m_pDialogRef;

	int m_iTabIndex;
#pragma endregion Member Variables
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVPictureDisplay\SVPictureTabInfo.h_v  $
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
 *    Rev 1.0   15 Mar 2010 13:25:30   jspila
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  1
 * SCR Title:  Initial Check-in of Prototype
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/