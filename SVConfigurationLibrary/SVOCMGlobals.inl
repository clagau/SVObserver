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

#include "SVOCMArchive.h"

template< typename SVTreeType >
HRESULT SVOCMLoadConfiguration(unsigned long ulSVOVersion, 
															 unsigned long& ulSVOConfigVersion, 
															 BSTR bstrFileName, 
															 SVTreeType& p_rTree )
{
	HRESULT hrOk = S_FALSE;

	hrOk = SVOCMArchive::GetSVOSavedVersionNumberFromConfigurationFile( 
		bstrFileName, &ulSVOConfigVersion );

	if( SUCCEEDED( hrOk ) )
	{
		hrOk = SVOCMArchive::CreateTreeFromConfigurationFile( 
			ulSVOVersion, ulSVOConfigVersion, bstrFileName, p_rTree );
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMSaveConfiguration (unsigned long ulSVOVersion, 
																unsigned long &ulSECVersion, 
																BSTR bstrFileName, 
																SVTreeType& p_rTree )
{
	HRESULT hr = S_FALSE;

	CString csFileName(bstrFileName);

	hr = SVOCMArchive::CreateConfigurationFileFromTree (ulSVOVersion, 
		NULL,	// revision history
		ulSECVersion, 
		csFileName, 
		p_rTree);

	return hr;
}

