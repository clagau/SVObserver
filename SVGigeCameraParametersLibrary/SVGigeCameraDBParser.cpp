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
#include "Stdafx.h"
#import <msxml6.dll> raw_interfaces_only 
#include "SVSystemLibrary/SVSAXErrorHandlerImpl.h"
#include "SVSystemLibrary/SVSAXContentHandlerImpl.h"
#include "SVGigeCameraDBParser.h"
#include "SVGigeCameraDBContent.h"

HRESULT SVGigeCameraDBParser::Parse(BSTR data, SVGigeDeviceParameterMap& params)
{
	CoInitialize( nullptr );
	MSXML2::ISAXXMLReader* pRdr( nullptr );

	HRESULT hr = CoCreateInstance(__uuidof(MSXML2::SAXXMLReader), nullptr, CLSCTX_ALL, __uuidof(MSXML2::ISAXXMLReader), (void **)&pRdr);

	if (hr == S_OK) 
	{
		SVGigeCameraDBContent contentHandler;
		SVSAXErrorHandlerImpl<SVGigeCameraDBContent> ErrorHandler(contentHandler);
		hr = pRdr->putContentHandler(&contentHandler);
		
		if (hr == S_OK)
		{
			hr = pRdr->putErrorHandler(&ErrorHandler);
		}
		
		if (hr == S_OK)
		{
			// Convert to Variant containing BSTR
			_variant_t dataVar(data);
			hr = pRdr->parse(dataVar);
			if (hr != S_OK)
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

