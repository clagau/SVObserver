// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOCMArchive
// * .File Name       : $Workfile:   SVOCMArchive.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   03 Sep 2014 15:52:50  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVOCMArchive.h"
#include "SVXmlStream.h"
#include "SVOLibrary/SVOLibrary.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVSAFEARRAY.h"
#include "SVConfigurationTags.h"
#include "SVXMLLibrary/SVXMLClass.h"
#include "SVLibrary/SVBStr.h"
#include "SVObserverEnums.h"
#include <msxml6.h>
#include "SVXMLLibrary/SaxXMLHandler.h"
#pragma endregion Includes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Primitives section
// the HandleErrorCondition call is not made within primitives so that more meaningful
// error placement and reporting can be made
// a primitive for this purpose is defined as a basic function which is called from more than
// one place in SVOCMArchive
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< typename SVTreeType >
HRESULT SVOCMArchive::CreateTreeFromConfigurationFile (unsigned long  ulSVOVersion, 
																											 unsigned long  ulConfigVersion, 
																											 BSTR			bstrFilename,
																											 SVTreeType& p_rTree)
{
	USES_CONVERSION;

	HRESULT hr( S_OK );

	SVString l_FileName( SvUl_SF::createSVString(bstrFilename) );

	CString cstrMessage;
	BSTR bstrRevisionHistory = NULL;

	while (1)
	{
		if( ulConfigVersion < 0x00040000 )
		{
			//-		Configuration files prior to version 4.00 should not be able to get
			//-		here.
			hr = -1663;
			break;
		}
		else if( 0x00040000 <= ulConfigVersion && ulConfigVersion < 0x00043200 )
		{
			SVXmlStream< SVTreeType > xml( p_rTree );

			if( !xml.Load_XML_Document( l_FileName.c_str() ) )
			{
				hr = E_FAIL;
			}
		}
		else if( 0x00043200 <= ulConfigVersion )
		{
			BSTR bstrChangedNode( NULL );
			SvXml::SaxXMLHandler<SVTreeType>  SaxHandler;
			hr = SaxHandler.BuildFromXMLFile(&p_rTree, bstrFilename);
			
		}
		else
		{
			hr = E_FAIL;
		}

		break;
	}

	SysFreeString (bstrRevisionHistory);
	bstrRevisionHistory = NULL;

	return hr;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::CreateConfigurationFileFromTree(unsigned long ulSVOVersion, BSTR bstrRevisionHistory, unsigned long ulSECVersion, LPCTSTR pstrFilename, SVTreeType& p_rTree)
{
	HRESULT hr = S_OK;

	SVTreeType l_XMLTree( p_rTree );
	SVXMLClass l_XMLTreeBuilder;
	
	hr = l_XMLTreeBuilder.Initialize( 0,		// do not use checksums
		1, 						// use program version/data revision info
		0);						// do not use encryption

	if( SUCCEEDED( hr ) )
	{
		hr = l_XMLTreeBuilder.PreserveWhitespace( true );
	}

	if( SUCCEEDED( hr ) )
	{
		SVBStr bstrFilename;
		SVBStr bstrFormatName;

		bstrFilename = pstrFilename;

		hr = bstrFormatName.CopyFromWChar (L"SVObserver");

		if( SUCCEEDED( hr ) )
		{
			hr = l_XMLTreeBuilder.CopyTreeToXMLFile( l_XMLTree, ulSVOVersion, bstrFormatName, bstrRevisionHistory, bstrFilename );
		}
	}
	return hr;
}

