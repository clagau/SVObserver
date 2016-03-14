//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraDatabase
//* .File Name       : $Workfile:   SVGigeCameraDatabase.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   01 Oct 2013 10:49:00  $
//******************************************************************************
#include "stdafx.h"
#include <boost/config.hpp>
#include <boost/assign/list_of.hpp>
#include "SVGigeCameraDatabase.h"

// Enum to Diplay Name String mapping
const SVGigeCameraDatabase::SVGigeEnumToString SVGigeCameraDatabase::m_enumToString = boost::assign::map_list_of<>
( SVGigeParameterExposureMode,				L"Trigger Mode" )
( SVGigeParameterBayerPattern,				L"Bayer Pattern" )
( SVGigeParameterXOffset,					L"X Offset" )
( SVGigeParameterYOffset,					L"Y Offset" )
( SVGigeParameterXSize,						L"X Size" )
( SVGigeParameterYSize,						L"Y Size" )
( SVGigeParameterColorFormat,				L"Color Format" )
( SVGigeParameterFeatureBrightness,			L"Brightness" )
( SVGigeParameterFeatureAutoExposure,		L"Auto Exposure" )
( SVGigeParameterFeatureSharpness,			L"Sharpness" )
( SVGigeParameterFeatureWhiteBalanceU,		L"White Balance - U" )
( SVGigeParameterFeatureWhiteBalanceV,		L"White Balance - V" )
( SVGigeParameterFeatureHue,				L"Hue" )
( SVGigeParameterFeatureSaturation,			L"Saturation" )
( SVGigeParameterFeatureGamma,				L"Gamma" )
( SVGigeParameterFeatureShutter,			L"Shutter" )
( SVGigeParameterFeatureGain,				L"Gain" )
( SVGigeParameterTriggerSource,				L"Trigger Source" )
( SVGigeParameterTriggerEdge,				L"Trigger Edge" )
( SVGigeParameterTriggerDelay,				L"Trigger Delay" )
( SVGigeParameterTriggerInvert,				L"Trigger Invert" )
( SVGigeParameterTriggerEnable,				L"Trigger Enable" )
( SVGigeParameterStrobeSource,				L"Strobe Source" )
( SVGigeParameterStrobeEdge,				L"Strobe Edge" )
( SVGigeParameterStrobeInvert,				L"Strobe Invert" )
( SVGigeParameterStrobeDelay,				L"Strobe Delay" )
( SVGigeParameterStrobeDuration,			L"Strobe Duration" )
( SVGigeParameterStrobeEnable,				L"Strobe Enable" )
( SVGigeParameterPacketSize,				L"Packet Size" )
( SVGigeParameterLutEnable,					L"LUT Enable" )
( SVGigeParameterLutArray,					L"LUT" )
( SVGigeParameterVendorName,				L"Vendor Name" )
( SVGigeParameterModelName,					L"Model Name" )
( SVGigeParameterSerialNumber,				L"Serial Number" )
( SVGigeParameterIPAddress,					L"IP Address" )
( SVGigeParameterHorizontalBinning,			L"Horizontal Binning" )
( SVGigeParameterVerticalBinning,			L"Vertical Binning" )
;

SVGigeCameraDatabase::SVGigeCameraDatabase()
{
}

long SVGigeCameraDatabase::GetFeatureCount()
{
	long count = static_cast< long >( m_enumToString.size() );
	return count;
}

HRESULT SVGigeCameraDatabase::GetFeatureNames(SVGigeEnumNameList& list)
{
	list.clear();

	for (SVGigeEnumToString::const_iterator it = m_enumToString.begin();it != m_enumToString.end();++it)
	{
		list.push_back(it->second);
	}
	return S_OK;
}

