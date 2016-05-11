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

#pragma region Includes
#include "stdafx.h"
#include "SVPropertySheetCancelImpl.h"
#pragma endregion Includes

SVPropertySheetCancelImpl::~SVPropertySheetCancelImpl()
{
}

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
			ISVCancel* pPageCancel = nullptr;
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

	ASSERT(nullptr == rpData);
	SVMultiCancelData* pCancelData = new SVMultiCancelData;
	rpData = pCancelData;

	int iCount = pThisSheet->GetPageCount();

	for( int i = 0; i < iCount; i++ )
	{
		CPropertyPage* pPage = pThisSheet->GetPage(i);
		if( pPage && pPage->GetSafeHwnd() ) 
		{
			ISVCancel* pPageCancel = nullptr;
			if (( pPageCancel = dynamic_cast<ISVCancel*>(pPage) ) && pPageCancel->CanCancel())
			{
				SVCancelData* pPageData = nullptr;
				HRESULT hr = pPageCancel->GetCancelData(pPageData);
				pCancelData->mapData[pPageCancel] = pPageData;
				if ( S_OK == hrResult )
				{
					hrResult = hr;
				}
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
	ASSERT(nullptr != pData);

	SVMultiCancelData* pCancelData = static_cast<SVMultiCancelData*> (pData);
	SVMultiCancelData::MapType::iterator iter;

	for ( iter = pCancelData->mapData.begin(); iter != pCancelData->mapData.end(); ++iter )
	{
		ISVCancel* pPageCancel = iter->first;
		SVCancelData* pPageData = iter->second;
		HRESULT hr = pPageCancel->SetCancelData(pPageData);
		if ( S_OK == hrResult )
		{
			hrResult = hr;
		}
	}

	return hrResult;
}

