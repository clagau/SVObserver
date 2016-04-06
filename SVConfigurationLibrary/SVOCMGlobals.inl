// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOCMGlobals
// * .File Name       : $Workfile:   SVOCMGlobals.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   03 Sep 2014 15:54:44  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVOCMArchive.h"
#include "ObsoleteItemChecker.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#pragma endregion Includes

template< typename SVTreeType >
HRESULT SVOCMLoadConfiguration(unsigned long ulSVOVersion, unsigned long& ulSVOConfigVersion, BSTR bstrFileName, SVTreeType& p_rTree)
{
	HRESULT hrOk = SVOCMArchive::GetSVOSavedVersionNumberFromConfigurationFile(bstrFileName, &ulSVOConfigVersion);

	if( SUCCEEDED( hrOk ) )
	{
		hrOk = SVOCMArchive::CreateTreeFromConfigurationFile(ulSVOVersion, ulSVOConfigVersion, bstrFileName, p_rTree);
	}
	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMSaveConfiguration(unsigned long ulSVOVersion, unsigned long &ulSECVersion, BSTR bstrFileName, SVTreeType& p_rTree)
{
	CString csFileName(bstrFileName);

	HRESULT hr = SVOCMArchive::CreateConfigurationFileFromTree (ulSVOVersion, nullptr, ulSECVersion, csFileName, p_rTree);
	return hr;
}

template< typename SVTreeType >
HRESULT SVOCMCheckObsoleteItems( SVTreeType& rTree, const unsigned long& ulSVOConfigVersion, CString& rItemType, int& errorCode )
{
	HRESULT hr = S_OK;
	if (0x00072800 > ulSVOConfigVersion) // if 7.40 or greater no need to check
	{
		hr = SvOc::HasObsoleteItem(rTree, rItemType, errorCode);
	}
	return hr;
}