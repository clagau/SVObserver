//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDCamBaslerDriver
//* .File Name       : $Workfile:   SVDCamBaslerDriver.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:32:58  $
//******************************************************************************

#include "stdafx.h"
#include "SVDCamBaslerDriver.h"
#include "SVOMFCLibrary/SVDeviceParamCollection.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVBoolValueDeviceParam.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"

SVDCamBaslerDriver::SVDCamBaslerDriver()
: SVDCamStandardDriver()
{
}

SVDCamBaslerDriver::~SVDCamBaslerDriver()
{
}

SVDCamDriver* SVDCamBaslerDriver::CreateNew()
{
	return new SVDCamBaslerDriver;
}

HRESULT SVDCamBaslerDriver::GetSoftwareTriggerRegisterAndValue(unsigned long& ulRegister, unsigned long& ulValue)
{
	ulRegister = DCAM_SOFTWARE_TRIGGER;
	ulValue = 0x80000000;
	return S_OK;
}

HRESULT SVDCamBaslerDriver::SetParameter( const SVDeviceParamWrapper& rw )
{
	HRESULT hr = S_OK;

	const SVDeviceParam* pParam = (const SVDeviceParam*) rw;
	if ( pParam != NULL )
	{
		switch ( pParam->Type() )
		{
			case DeviceParamTestPattern:
			{
				ULONG ulRegister = Basler_TEST_IMAGE;
				ULONG ulValue = 0;
				ULONG ulTestPattern = LongValue( rw );
				SetValueAtBits( &ulValue, Basler_TEST_IMAGE_TestImageBits, ulTestPattern );
				HRESULT hrRegister;
				#ifdef _DEBUG
					ULONG ulPreValue = 0;
					ULONG ulReadValue = 0;
					hrRegister = ReadCameraRegister( ulRegister, ulPreValue );
				#endif
			
				hrRegister = WriteCameraRegister( ulRegister, ulValue );

				#ifdef _DEBUG
					hrRegister = ReadCameraRegister( ulRegister, ulReadValue );
					hrRegister += 0;
				#endif
				break;
			}

			default:
				hr = SVDCamStandardDriver::SetParameter( rw );
				break;
		}// end switch ( pParam->Type() )
	}// end if ( pParam != NULL )

	return hr;
}

HRESULT SVDCamBaslerDriver::SetParameters( const SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = S_OK;

	bool bSetStrobeInfo = false;
	bool bSetTriggerInfo = false;

	SVDeviceParamMap::const_iterator iter;
	for ( iter = rDeviceParams.mapParameters.begin(); iter != rDeviceParams.mapParameters.end(); ++iter)
	{
		const SVDeviceParamWrapper& rw = iter->second;
		if ( rw.IsValid() )
		{
			switch ( rw->Type() )
			{
				default:
					SetParameter( rw );
					break;
			}// end switch ( w->Type() )
		}
	}// end for ( iter = rDeviceParams.mapParameters.begin(); iter != rDeviceParams.mapParameters.end(); iter++)

	return hr;
}

unsigned long SVDCamBaslerDriver::GetDCAMTriggerOnOff() const
{
	return 1; // always listen to hardware trigger; we don't want freerunning because it strobes continuously
}

