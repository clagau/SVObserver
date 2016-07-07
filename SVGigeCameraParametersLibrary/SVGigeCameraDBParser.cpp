//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraDBParser
//* .File Name       : $Workfile:   SVGigeCameraDBParser.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:46:20  $
//******************************************************************************
#pragma region Includes
#include "Stdafx.h"
#import <msxml6.dll> raw_interfaces_only 
#include "SVSystemLibrary/SVSAXErrorHandlerImpl.h"
#include "SVSystemLibrary/SVSAXContentHandlerImpl.h"
#include "SVGigeCameraDBParser.h"
#include "SVGigeCameraDBContent.h"
#pragma endregion Includes

HRESULT SVGigeCameraDBParser::Parse(BSTR data, SVGigeDeviceParameterMap& params)
{
	CoInitialize( nullptr );
	MSXML2::ISAXXMLReader* pRdr( nullptr );

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

