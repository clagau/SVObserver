//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCPropSheet
//* .File Name       : $Workfile:   SVPLCPropSheet.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:28:02  $
//******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"
#include "SVPLCPropSheet.h"
#include "SVPLCPropPageStart.h"
#include "SVPLCPropPageList.h"


// SVPLCPropSheet

IMPLEMENT_DYNAMIC(SVPLCPropSheet, CPropertySheet)

SVPLCPropSheet::SVPLCPropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}

SVPLCPropSheet::SVPLCPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{

}

SVPLCPropSheet::~SVPLCPropSheet()
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


BEGIN_MESSAGE_MAP(SVPLCPropSheet, CPropertySheet)
END_MESSAGE_MAP()

void SVPLCPropSheet::AddPages()
{
	AddPage( new SVPLCPropPageStart( this ) );
	AddPage( new SVPLCPropPageList( this ) );
	SetWizardMode();
}

// SVPLCPropSheet message handlers


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPLCPropSheet.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:28:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:10:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

