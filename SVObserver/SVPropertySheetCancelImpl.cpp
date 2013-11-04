// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVPropertySheetCancelImpl
// * .File Name       : $Workfile:   SVPropertySheetCancelImpl.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 14:38:00  $
// ******************************************************************************

#include "stdafx.h"
#include "SVPropertySheetCancelImpl.h"


bool SVPropertySheetCancelImpl::CanCancel()
{
	CPropertySheet* pThisSheet = dynamic_cast <CPropertySheet*> (this);
	ASSERT( pThisSheet );

	bool bCanCancel = true;

	int iCount = pThisSheet->GetPageCount();

	for( int i = 0; i < iCount; i++ )
	{
		CPropertyPage* pPage = pThisSheet->GetPage(i);
		if( pPage && pPage->GetSafeHwnd() ) 
		{
			ISVCancel* pPageCancel = NULL;
			bCanCancel = bCanCancel && (( pPageCancel = dynamic_cast<ISVCancel*>(pPage) ) && pPageCancel->CanCancel());
		}
	}

	return bCanCancel;
}

HRESULT SVPropertySheetCancelImpl::GetCancelData(SVCancelData*& rpData)
{
	HRESULT hrResult = S_OK;

	CPropertySheet* pThisSheet = dynamic_cast <CPropertySheet*> (this);
	ASSERT( pThisSheet );

	ASSERT(rpData == NULL);
	SVMultiCancelData* pCancelData = new SVMultiCancelData;
	rpData = pCancelData;

	int iCount = pThisSheet->GetPageCount();

	for( int i = 0; i < iCount; i++ )
	{
		CPropertyPage* pPage = pThisSheet->GetPage(i);
		if( pPage && pPage->GetSafeHwnd() ) 
		{
			ISVCancel* pPageCancel = NULL;
			if (( pPageCancel = dynamic_cast<ISVCancel*>(pPage) ) && pPageCancel->CanCancel())
			{
				SVCancelData* pPageData = NULL;
				HRESULT hr = pPageCancel->GetCancelData(pPageData);
				pCancelData->mapData[pPageCancel] = pPageData;
				if ( hrResult == S_OK )
					hrResult = hr;
			}
		}
	}

	return hrResult;
}

HRESULT SVPropertySheetCancelImpl::SetCancelData(SVCancelData* pData)
{
	HRESULT hrResult = S_OK;

	CPropertySheet* pThisSheet = dynamic_cast <CPropertySheet*> (this);
	ASSERT( pThisSheet );
	ASSERT(pData != NULL);

	SVMultiCancelData* pCancelData = static_cast<SVMultiCancelData*> (pData);
	SVMultiCancelData::MapType::iterator iter;

	for ( iter = pCancelData->mapData.begin(); iter != pCancelData->mapData.end(); ++iter )
	{
		ISVCancel* pPageCancel = iter->first;
		SVCancelData* pPageData = iter->second;
		HRESULT hr = pPageCancel->SetCancelData(pPageData);
		if ( hrResult == S_OK )
			hrResult = hr;
	}

	return hrResult;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPropertySheetCancelImpl.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:38:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Oct 2012 10:53:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2004 12:39:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

