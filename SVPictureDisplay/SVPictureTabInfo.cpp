//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPictureTabInfo
//* .File Name       : $Workfile:   SVPictureTabInfo.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:28:54  $
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "resource.h"
#include "SVPictureTabInfo.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
// Standard Constructor
SVPictureTabInfo::SVPictureTabInfo()
: m_iTabIndex(-1)
{
}

// Copy Constructor
SVPictureTabInfo::SVPictureTabInfo(const SVPictureTabInfo& p_rhs)
: m_iTabIndex(p_rhs.m_iTabIndex)
, m_pDialogRef(p_rhs.m_pDialogRef)
, m_tabName(p_rhs.m_tabName)
{
}

// Destructor
SVPictureTabInfo::~SVPictureTabInfo()
{
}
#pragma endregion Constructor

#pragma region Public Methods
const SVPictureTabInfo& SVPictureTabInfo::operator=(const SVPictureTabInfo& p_rhs)
{
	m_iTabIndex = p_rhs.m_iTabIndex;
	m_pDialogRef = p_rhs.m_pDialogRef;
	m_tabName = p_rhs.m_tabName;
	return *this;
}

int SVPictureTabInfo::GetTabIndex() const
{
	return m_iTabIndex;
}

void SVPictureTabInfo::SetTabIndex(int index)
{
	 m_iTabIndex = index;
}

SVPictureDialog* SVPictureTabInfo::GetPictureDialog() const
{
	return m_pDialogRef.get();
}

std::basic_string<TCHAR> SVPictureTabInfo::GetTabName() const
{
	return m_tabName;
}

void SVPictureTabInfo::SetTabName( std::basic_string<TCHAR> name )
{
	m_tabName = name;
}

SVPictureDialogRef SVPictureTabInfo::GetPictureDialogRef() const
{
	return m_pDialogRef;
}

void SVPictureTabInfo::SetPictureDialogRef( SVPictureDialogRef p )
{
	m_pDialogRef = p;
}
#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVPictureDisplay\SVPictureTabInfo.cpp_v  $
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
