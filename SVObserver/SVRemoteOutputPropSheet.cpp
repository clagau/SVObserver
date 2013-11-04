//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteOutputPropSheet
//* .File Name       : $Workfile:   SVRemoteOutputPropSheet.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:53:18  $
//******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"
#include "SVRemoteOutputPropSheet.h"
#include "SVRemoteOutputPropPageList.h"


// SVRemoteOutputPropSheet

IMPLEMENT_DYNAMIC(SVRemoteOutputPropSheet, CPropertySheet)

SVRemoteOutputPropSheet::SVRemoteOutputPropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}

SVRemoteOutputPropSheet::SVRemoteOutputPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{

}

SVRemoteOutputPropSheet::~SVRemoteOutputPropSheet()
{
	// Remove and delete all pages...
	for( int i = GetPageCount() - 1; i >= 0; -- i )
	{
		CPropertyPage* pPage = GetPage( i );
		RemovePage( i );
		if( pPage )
		{
			delete pPage;
		}
	}
}


BEGIN_MESSAGE_MAP(SVRemoteOutputPropSheet, CPropertySheet)
END_MESSAGE_MAP()

void SVRemoteOutputPropSheet::AddPages()
{
	AddPage( new SVRemoteOutputPropPageList( this ) );
//	SetWizardMode();
}

// SVRemoteOutputPropSheet message handlers


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRemoteOutputPropSheet.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:53:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/