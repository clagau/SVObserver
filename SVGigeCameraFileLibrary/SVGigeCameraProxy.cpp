//******************************************************************************
//* COPYRIGHT (c) 2009 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraProxy
//* .File Name       : $Workfile:   SVGigeCameraProxy.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:43:36  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVGigeCameraProxy.h"
#include "CameraLibrary/SVDeviceParamCollection.h"
#include "Triggering/SVDigitizerLoadLibraryClass.h"
#pragma endregion Includes

SVGigeCameraProxy::SVGigeCameraProxy()
{ 
}

SVGigeCameraProxy::~SVGigeCameraProxy()
{
}

// moved from SVAcquistionClass - only useful for SVImageTest...
HRESULT SVGigeCameraProxy::SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams, SVDeviceParamCollection& rDestDeviceParams )
{
	HRESULT hr = S_OK;

	// don't do a straight assignment ( rDestDeviceParams = rDeviceParams )
	// only copy those parameters that are specified.
	SVDeviceParamMap::const_iterator iter;
	for (iter = rDeviceParams.mapParameters.begin(); iter != rDeviceParams.mapParameters.end(); ++iter)
	{
		const SVDeviceParamWrapper& w = iter->second;
		if ( nullptr != ((const SVDeviceParam*) w) )
		{
			rDestDeviceParams.GetParameter( iter->first ) = w;
		}
	}

	return hr;
}

