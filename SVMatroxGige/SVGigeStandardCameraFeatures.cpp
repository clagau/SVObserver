//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeStandardCameraFeatures
//* .File Name       : $Workfile:   SVGigeStandardCameraFeatures.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   12 Jun 2013 15:13:24  $
//******************************************************************************
#include "stdafx.h"
#include <comdef.h>
#include <boost/config.hpp>
#include <boost/assign/list_of.hpp>
#include "SVGigeStandardCameraFeatures.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerRef.h"
#include "SVGigeCameraParametersLibrary/SVGigeFeature.h"
#include "SVGigeCameraParametersLibrary/SVGigeEmptySetter.h"
#include "SVGigeCameraParametersLibrary/SVGigeEmptyGetter.h"
#include "SVGigeCameraParametersLibrary/SVGigeLUTSetter.h"
#include "SVGigeCameraParametersLibrary/SVGigeLUTGetter.h"
#include "SVGigeCameraParametersLibrary/SVGigeReadSerialNumber.h"
#include "SVGigeCameraParametersLibrary/SVGigeReadIPAddress.h"

struct SVGigeTriggerLineGet
{
	// iterate thru the Line Selectors (Line0/Line1) and get the first one whose LineMode is Input
	HRESULT operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, _variant_t& rValue) const
	{ 
		SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVMatroxString(rFeature.GetName().c_str()), rFeature.GetType(), rValue);
		
		_variant_t value("Input");
		l_Code = SVMatroxDigitizerInterface::GetFeature(*(Digitizer.get()), SVMatroxString(rFeature.GetName().c_str()), rFeature.GetType(), value);
		if (l_Code == SVMEE_STATUS_OK)
		{
			rValue = _bstr_t(SvUl_SF::createSVString(value).c_str()).Detach();
		}
		return l_Code;
	}
};

struct SVGigeTriggerLineSet
{
	// Setter - the value passed in is for the Feature Selector 
	HRESULT operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, const _variant_t& rValue) const
	{ 
		const SVGigeFeatureSelector& selector = rFeature.GetSelector();
		SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVMatroxString(selector.GetName().c_str()), SVMatroxDigitizerFeature::SVTypeStringEnumeration, rValue);
		if (l_Code == SVMEE_STATUS_OK)
		{
			l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVMatroxString(rFeature.GetName().c_str()), SVMatroxDigitizerFeature::SVTypeStringEnumeration, _variant_t("Input"));
		}
		return l_Code;
	}
};

struct SVGigeTriggerInvertGetter
{
	// Get the Trigger Line number
	HRESULT operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, _variant_t& rValue) const
	{ 
		//
		// Determine which Line is the Trigger...
		const SVGigeFeatureSelector& selector = rFeature.GetSelector();
		SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVMatroxString(selector.GetName().c_str()), SVMatroxDigitizerFeature::SVTypeStringEnumeration, rValue);
		if (l_Code == SVMEE_STATUS_OK)
		{
			_variant_t value(false);
			l_Code = SVMatroxDigitizerInterface::GetFeature(*(Digitizer.get()), SVMatroxString(rFeature.GetName().c_str()), rFeature.GetType(), value);
			if (l_Code == SVMEE_STATUS_OK)
			{
				rValue = value.Detach();
			}
		}
		return l_Code;
	}
};

struct SVGigeTriggerInvertSetter
{
	// Setter
	HRESULT operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, const _variant_t& rValue) const
	{ 
		SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVMatroxString(rFeature.GetName().c_str()), rFeature.GetType(), rValue);
		return l_Code;
	}
};

struct SVGigeStrobeLineGetter
{
	// iterate thru the Line Selectors (Line0/Line1) and get the first one whose LineMode is Output
	HRESULT operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& feature, _variant_t& rValue) const
	{ 
		_variant_t value("Output");
		SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::GetFeature(*(Digitizer.get()), SVMatroxString(feature.GetName().c_str()), feature.GetType(), value);
		if (l_Code == SVMEE_STATUS_OK)
		{
			rValue = _bstr_t(SvUl_SF::createSVString(value).c_str()).Detach();
		}
		return l_Code;
	}
};

struct SVGigeStrobeLineSetter
{
	HRESULT operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, const _variant_t& rValue) const
	{ 
		const SVGigeFeatureSelector& selector = rFeature.GetSelector();
		SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVMatroxString(selector.GetName().c_str()), SVMatroxDigitizerFeature::SVTypeStringEnumeration, rValue);
		if (l_Code == SVMEE_STATUS_OK)
		{
			l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVMatroxString(rFeature.GetName().c_str()), SVMatroxDigitizerFeature::SVTypeStringEnumeration, _variant_t("Output"));
		}
		return l_Code;
	}
};

struct SVGigeStrobeInvertGetter
{
	HRESULT operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, _variant_t& rValue) const
	{ 
		// Get which line is the Strobe (LineMode = Output)
		SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVMatroxString(rFeature.GetName().c_str()), rFeature.GetType(), rValue);
		
		_variant_t value("Output");
		l_Code = SVMatroxDigitizerInterface::GetFeature(*(Digitizer.get()), SVMatroxString(rFeature.GetName().c_str()), rFeature.GetType(), value);
		if (l_Code == SVMEE_STATUS_OK)
		{
			rValue = _bstr_t(SvUl_SF::createSVString(value).c_str()).Detach();
		}
		return l_Code;
	}
};

struct SVGigeStrobeInvertSetter
{
	// Setter
	HRESULT operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, const _variant_t& rValue) const
	{
		// Get which line is the Strobe (LineMode = Output)
		const SVGigeFeatureSelector& selector = rFeature.GetSelector();
		SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVMatroxString(selector.GetName().c_str()), SVMatroxDigitizerFeature::SVTypeStringEnumeration, rValue);
		if (l_Code == SVMEE_STATUS_OK)
		{
			l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVMatroxString(rFeature.GetName().c_str()), rFeature.GetType(), rValue);
		}
		return l_Code;
	}
};

struct SVGigeInputLineGetter
{
	// iterate thru the Line Selectors (Line0/Line1) and get the first one whose LineMode is Input ?
	HRESULT operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& feature, _variant_t& rValue) const
	{ 
		_variant_t value("Input");
		SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::GetFeature(*(Digitizer.get()), SVMatroxString(feature.GetName().c_str()), feature.GetType(), value);
		if (l_Code == SVMEE_STATUS_OK)
		{
			rValue = _bstr_t(SvUl_SF::createSVString(value).c_str()).Detach();
		}
		return l_Code;
	}
};

struct SVGigeInputLineSetter
{
	HRESULT operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, const _variant_t& rValue) const
	{ 
		const SVGigeFeatureSelector& selector = rFeature.GetSelector();
		SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVMatroxString(selector.GetName().c_str()), SVMatroxDigitizerFeature::SVTypeStringEnumeration, rValue);
		if (l_Code == SVMEE_STATUS_OK)
		{
			l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVMatroxString(rFeature.GetName().c_str()), SVMatroxDigitizerFeature::SVTypeStringEnumeration, _variant_t("Input"));
		}
		return l_Code;
	}
};

struct SVGigeEventSetter
{
	HRESULT operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, const _variant_t& rValue) const
	{
		SVString l_value = SvUl_SF::createSVString(rValue);
		SVMatroxString eventValue = l_value.c_str();
		SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::SetGigeEvent(*(Digitizer.get()), SVMatroxString(rFeature.GetName().c_str()), eventValue.c_str());
		return l_Code;
	}
};

const SVGigeDeviceParameterMap& SVGigeStandardCameraFeatures::GetStandardFeatures()
{
	static const SVGigeDeviceParameterMap features = boost::assign::map_list_of<>
	///////////////////////////////////////////////////////////////////////
	// Exposure Mode - not supported yet
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterExposureMode,				
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Trigger Mode")), VT_BSTR,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "ExposureMode",  SVMatroxDigitizerFeature::SVTypeStringEnumeration,
					SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
				),
				SVGigeEmptyGetter(),
				SVGigeEmptySetter()
			)
		)
	)
	
	///////////////////////////////////////////////////////////////////////
	// Bayer Pattern - not supported yet
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterBayerPattern,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Bayer Pattern")), VT_I4,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "", SVMatroxDigitizerFeature::SVTypeStringEnumeration,
					SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
				),
				SVGigeEmptyGetter(),
				SVGigeEmptySetter()
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// X Offset
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterXOffset,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("X Offset")), VT_I4,
			SVGigeParameterAccessor
			(
				SVGigeFeature( "OffsetX", SVMatroxDigitizerFeature::SVTypeInt32 )
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Y Offset
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterYOffset,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Y Offset")), VT_I4,
			SVGigeParameterAccessor
			(
				SVGigeFeature( "OffsetY", SVMatroxDigitizerFeature::SVTypeInt32 )
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Width
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterXSize,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("X Size")), VT_I4,
			SVGigeParameterAccessor
			(
				SVGigeFeature( "Width", SVMatroxDigitizerFeature::SVTypeInt32 )
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Height
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterYSize,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Y Size")), VT_I4,
			SVGigeParameterAccessor
			(
				SVGigeFeature("Height", SVMatroxDigitizerFeature::SVTypeInt32)
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Color Format
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterColorFormat,
		SVGigeDeviceParameterStruct
		( SVString(_T("Color Format")),	VT_BSTR,		
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "PixelFormat", SVMatroxDigitizerFeature::SVTypeStringEnumeration,
					SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
					SVGigeFeatureSelector( ),
					(  boost::assign::map_list_of<SVDeviceParamString, SVGigeFeatureString>
						( _T("Y"), "Mono8" )
						( _T("YUV422"), "YUV422Packed" )
					)
				)
			)
		) 
	)

	///////////////////////////////////////////////////////////////////////
	// Brightness
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterFeatureBrightness,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Brightness")), VT_I4,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "BlackLevelRaw", SVMatroxDigitizerFeature::SVTypeInt32,
					SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
					SVGigeFeatureSelector( "BlackLevelSelector", "All" )
				)
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Auto Exposure - not supported yet
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterFeatureAutoExposure,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Auto Exposure")), VT_I4,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "ExposureTimeAuto", SVMatroxDigitizerFeature::SVTypeStringEnumeration,
					SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
				),
				SVGigeEmptyGetter(),
				SVGigeEmptySetter()
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Sharpness - not supported yet
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterFeatureSharpness,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Sharpness")), VT_I4,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "", SVMatroxDigitizerFeature::SVTypeInt32,
					SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
				),
				SVGigeEmptyGetter(),
				SVGigeEmptySetter()
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// WhiteBalanceU - not supported yet
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterFeatureWhiteBalanceU,	
		SVGigeDeviceParameterStruct
		(
			SVString(_T("White Balance - U")), VT_I4,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "", SVMatroxDigitizerFeature::SVTypeInt32,
					SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
				),
				SVGigeEmptyGetter(), 
				SVGigeEmptySetter()
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// WhiteBalanceV - not supported yet
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterFeatureWhiteBalanceV,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("White Balance - V")), VT_I4,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "", SVMatroxDigitizerFeature::SVTypeInt32, 
					SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
				),
				SVGigeEmptyGetter(), 
				SVGigeEmptySetter()
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Hue - not supported yet
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterFeatureHue,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Hue")), VT_I4,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "", SVMatroxDigitizerFeature::SVTypeInt32,
					SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
				),
				SVGigeEmptyGetter(),
				SVGigeEmptySetter()
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Saturation - not supported yet
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterFeatureSaturation,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Saturation")), VT_I4,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "", SVMatroxDigitizerFeature::SVTypeInt32,
					SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
				),
				SVGigeEmptyGetter(),
				SVGigeEmptySetter()
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Gamma
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterFeatureGamma,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Gamma")), VT_R8,
			SVGigeParameterAccessor
			(
				SVGigeFeature( "Gamma", SVMatroxDigitizerFeature::SVTypeDouble )
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Shutter
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterFeatureShutter,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Shutter")), VT_R8,
			SVGigeParameterAccessor
			(
				SVGigeFeature( "ExposureTimeAbs", SVMatroxDigitizerFeature::SVTypeDouble )
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Gain
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterFeatureGain,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Gain")), VT_R8,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "GainAbs", SVMatroxDigitizerFeature::SVTypeDouble, 
					SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
					SVGigeFeatureSelector( "GainSelector", "All" )
				)
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Trigger Source
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterTriggerSource,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Trigger Source")), VT_BSTR,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "TriggerSource", SVMatroxDigitizerFeature::SVTypeStringEnumeration,
					SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
					SVGigeFeatureSelector( "TriggerSelector", "FrameStart" ),
					( boost::assign::map_list_of<SVDeviceParamString, SVGigeFeatureString>
						( _T( "External Trigger" ), "Line0" )
						( _T( "Software Trigger" ), "Software" )
					)
				)
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Trigger Edge
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterTriggerEdge,
		SVGigeDeviceParameterStruct
		( 
			SVString(_T("Trigger Edge")), VT_BSTR,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "TriggerActivation", SVMatroxDigitizerFeature::SVTypeStringEnumeration,
					SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
					SVGigeFeatureSelector( "TriggerSelector", "FrameStart" ),
					( boost::assign::map_list_of<SVDeviceParamString, SVGigeFeatureString>
						( _T( "Rising" ), "RisingEdge" )
						( _T( "Falling" ), "FallingEdge" )
					)
				)
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Trigger Invert
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterTriggerInvert,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Trigger Invert")), VT_BOOL,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "LineInvertor", SVMatroxDigitizerFeature::SVTypeBool,
					SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
				),
				SVGigeEmptyGetter(),
				SVGigeEmptySetter()
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Trigger Line
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterTriggerLine,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Trigger Line")), VT_BSTR,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "LineMode", SVMatroxDigitizerFeature::SVTypeStringEnumeration,
					SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
					SVGigeFeatureSelector( "LineSelector", "Line0" )
				),
				SVGigeTriggerLineGet(),
				SVGigeTriggerLineSet()
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Trigger Delay
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterTriggerDelay,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Trigger Delay")), VT_R8,
			SVGigeParameterAccessor
			(
				SVGigeFeature( "TriggerDelayAbs", SVMatroxDigitizerFeature::SVTypeDouble )
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Trigger Enable
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterTriggerEnable,
		 SVGigeDeviceParameterStruct
		 (
			 SVString(_T("Trigger Enable")), VT_BSTR,
			 SVGigeParameterAccessor
			 (
				SVGigeFeature
				( "TriggerMode", SVMatroxDigitizerFeature::SVTypeStringEnumeration,
					SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
					SVGigeFeatureSelector( "TriggerSelector", "FrameStart" ),
					( boost::assign::map_list_of<SVDeviceParamString, SVGigeFeatureString>
						( _T( "Enable" ), "On" )
						( _T( "Disable" ), "Off" )
					)
				)
			 )
		 )
	 )

	///////////////////////////////////////////////////////////////////////
	// Strobe Source - Non Standard
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterStrobeSource,
		 SVGigeDeviceParameterStruct
		 (
			 SVString(_T("Strobe Source")), VT_BSTR,
			 SVGigeParameterAccessor
			 (
				 SVGigeFeature
				 ( "StrobeSource", SVMatroxDigitizerFeature::SVTypeStringEnumeration,
					SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
				 ),
				 SVGigeEmptyGetter(),
				 SVGigeEmptySetter()
			 )
		 )
	 )

	///////////////////////////////////////////////////////////////////////
	// Strobe Edge - Non Standard
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterStrobeEdge,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Strobe Edge")), VT_BSTR,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "StrobePolarity", SVMatroxDigitizerFeature::SVTypeStringEnumeration,
					SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
				),
				SVGigeEmptyGetter(),
				SVGigeEmptySetter()
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Strobe Invert
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterStrobeInvert,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Strobe Invert")), VT_BOOL,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "LineInvertor", SVMatroxDigitizerFeature::SVTypeBool,
					SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
				),
				SVGigeEmptyGetter(),
				SVGigeEmptySetter()
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Strobe Delay - Non Standard
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterStrobeDelay,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Strobe Delay")), VT_R8,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "StrobeDelayAbs", SVMatroxDigitizerFeature::SVTypeDouble,
					SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
				),
				SVGigeEmptyGetter(),
				SVGigeEmptySetter()
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Strobe Duration - Non Standard
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterStrobeDuration,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Strobe Duration")),	VT_R8,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "StrobeDurationAbs", SVMatroxDigitizerFeature::SVTypeDouble,
					SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
				),
				SVGigeEmptyGetter(),
				SVGigeEmptySetter()
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Strobe Line
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterStrobeLine,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Strobe Line")), VT_BSTR,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "LineMode", SVMatroxDigitizerFeature::SVTypeStringEnumeration,
					SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
					SVGigeFeatureSelector( "LineSelector", "Line1" )
				),
				SVGigeStrobeLineGetter(),
				SVGigeStrobeLineSetter()
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Strobe Enable - Non Standard
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterStrobeEnable,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Strobe Enable")), VT_BSTR,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "StrobeEnable", SVMatroxDigitizerFeature::SVTypeStringEnumeration,
					SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
				),
				SVGigeEmptyGetter(),
				SVGigeEmptySetter()
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Horizontal Binning
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterHorizontalBinning,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Horizontal Binning")), VT_I4,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "BinningHorizontal", SVMatroxDigitizerFeature::SVTypeInt32,
					SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
				),
				SVGigeEmptyGetter(),
				SVGigeEmptySetter()
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Vertical Binning
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterVerticalBinning,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Vertical Binning")), VT_I4,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "BinningVertical", SVMatroxDigitizerFeature::SVTypeInt32,
					SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
				),
				SVGigeEmptyGetter(),
				SVGigeEmptySetter()
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Packet Size
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterPacketSize,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Packet Size")), VT_I4,
			SVGigeParameterAccessor
			(
				SVGigeFeature( "GevSCPSPacketSize", SVMatroxDigitizerFeature::SVTypeInt32 )
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// LUT Enable
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterLutEnable,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("LUT Enable")), VT_BOOL,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( 
					"LUTEnable", SVMatroxDigitizerFeature::SVTypeBool,
					SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
					SVGigeFeatureSelector( "LUTSelector", "Luminance" )
				)
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// LUT Value Array
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterLutArray,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("LUT")), VT_I4 | VT_ARRAY,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( 
					"LUTValue", SVMatroxDigitizerFeature::SVTypeInt32,
					SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
					SVGigeFeatureSelector( "LUTSelector", "Luminance" )
				),
				SVGigeLUTGetter(),
				SVGigeLUTSetter()
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Input
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterLineInput,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Input")), VT_BSTR,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "LineMode", SVMatroxDigitizerFeature::SVTypeStringEnumeration,
					SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
					SVGigeFeatureSelector( "LineSelector" )
				),
				SVGigeInputLineGetter(),
				SVGigeInputLineSetter()
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Input Event
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterInputEvent,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Input Event")), VT_BSTR,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "EventNotification", SVMatroxDigitizerFeature::SVTypeStringEnumeration,
					SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
					SVGigeFeatureSelector( "EventSelector" ),
					( boost::assign::map_list_of<SVDeviceParamString, SVGigeFeatureString>
						( _T( "Enable" ), "On" )
						( _T( "Disable" ), "Off" )
					)
				),
				SVGigeEmptyGetter(),
				SVGigeEventSetter()
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Vendor Name
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterVendorName,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Vendor Name")), VT_BSTR,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "DeviceVendorName", SVMatroxDigitizerFeature::SVTypeString,
					SVGigeFeature::Supported, SVGigeFeature::ReadOnly
				)
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Model Name
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterModelName,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Model Name")), VT_BSTR,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "DeviceModelName", SVMatroxDigitizerFeature::SVTypeString,
					SVGigeFeature::Supported, SVGigeFeature::ReadOnly
				)
			)
		)
	)

	///////////////////////////////////////////////////////////////////////
	// Serial Number
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterSerialNumber,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("Serial Number")), VT_BSTR,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "DeviceID", SVMatroxDigitizerFeature::SVTypeString,
					SVGigeFeature::Supported, SVGigeFeature::ReadOnly
				),
				SVGigeReadSerialNumber(),
				SVGigeEmptySetter()
			)
		)
	)
	///////////////////////////////////////////////////////////////////////
	// IP Address
	///////////////////////////////////////////////////////////////////////
	( SVGigeParameterIPAddress,
		SVGigeDeviceParameterStruct
		(
			SVString(_T("IP Address")), VT_BSTR,
			SVGigeParameterAccessor
			(
				SVGigeFeature
				( "DeviceID", SVMatroxDigitizerFeature::SVTypeString,
					SVGigeFeature::Supported, SVGigeFeature::ReadOnly
				),
				SVGigeReadIPAddress(),
				SVGigeEmptySetter()
			)
		)
	)
	;

	return features;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVMatroxGige\SVGigeStandardCameraFeatures.cpp_v  $
 * 
 *    Rev 1.2   12 Jun 2013 15:13:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   22 May 2013 10:15:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  833
 * SCR Title:  Change GigE Camera Manager to be based on IP Address
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added the SVGigeParameterIPAddress in GetStandardFeatures
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 May 2013 19:40:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   10 May 2013 10:13:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SVGigeParameterHorizontalBinning and SVGigeParameterVerticalBinning
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 14:36:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   21 Jan 2013 10:00:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added support for camera line input and events
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Jan 2012 14:55:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated definitions for declaration for camera features.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   23 Aug 2011 09:07:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  735
 * SCR Title:  Add Gige Color System
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Color Format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Jul 2010 13:50:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to foce the trigger edge parameter and strobe edge parameter to be updated correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Jun 2010 08:01:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes to allow for GigE Light Reference and LUT to work.  Added functionality to read GigE camera database from camera file to allow for changes to camera functionality to be externalized.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2009 07:50:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new buffer management and buffer transfer methodologies.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:17:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
