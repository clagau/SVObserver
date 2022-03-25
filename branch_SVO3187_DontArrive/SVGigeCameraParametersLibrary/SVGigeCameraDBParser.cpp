//******************************************************************************
//* COPYRIGHT (c) 2010 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraDBParser
//* .File Name       : $Workfile:   SVGigeCameraDBParser.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:46:20  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#pragma warning( push )
#pragma warning( disable : 4471 )
#import <msxml3.dll> raw_interfaces_only 
#pragma warning( pop )
#include "SVSystemLibrary/SVSAXErrorHandlerImpl.h"
#include "SVGigeCameraDBParser.h"
#include "SVGigeCameraDBContent.h"
#pragma endregion Includes

HRESULT SVGigeCameraDBParser::Parse(BSTR data, SVGigeDeviceParameterMap& params)
{
	CoInitialize( nullptr );
	MSXML2::ISAXXMLReader* pRdr( nullptr );

	///Note MSXML2::SAXXMLReader60 does not work as the camera XML section is not parsed correctly
	///The problem seems to be in SVGigeCameraDBContent::GetGigeDeviceParameterStructAttributes at pAttributes->getLocalName
	///which seems to be filtered for version MS XML parser 6.0
	HRESULT hr = CoCreateInstance(__uuidof(MSXML2::SAXXMLReader), nullptr, CLSCTX_ALL, __uuidof(MSXML2::ISAXXMLReader), (void **)&pRdr);

	if (S_OK == hr) 
	{
		SVGigeCameraDBContent contentHandler;
		SVSAXErrorHandlerImpl<SVGigeCameraDBContent> ErrorHandler(contentHandler);
		hr = pRdr->putContentHandler(&contentHandler);
		
		if (S_OK == hr)
		{
			hr = pRdr->putErrorHandler(&ErrorHandler);
		}
		
		if (S_OK == hr)
		{
			// Convert to Variant containing BSTR
			_variant_t dataVar(data);
			hr = pRdr->parse(dataVar);
			if (S_OK != hr)
			{
				// Get Error Info
			}
			else
			{
				// Get GigeDeviceParameterMap
				params = contentHandler.GetGigeDeviceParameterMap();
			}
		}
		pRdr->putContentHandler( nullptr );
		pRdr->putErrorHandler( nullptr );
		pRdr->Release();
	}
	
	CoUninitialize();

	return hr;
}

