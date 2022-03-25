//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeStandardCameraFeatures
//* .File Name       : $Workfile:   SVGigeStandardCameraFeatures.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   12 Jun 2013 15:13:24  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVGigeStandardCameraFeatures.h"
#include "SVGigeCameraParametersLibrary/SVGigeEmptySetter.h"
#include "SVGigeCameraParametersLibrary/SVGigeEmptyGetter.h"
#include "SVGigeCameraParametersLibrary/SVGigeLUTSetter.h"
#include "SVGigeCameraParametersLibrary/SVGigeLUTGetter.h"
#include "SVGigeCameraParametersLibrary/SVGigeReadSerialNumber.h"
#include "SVGigeCameraParametersLibrary/SVGigeReadIPAddress.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerInterface.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

struct SVGigeTriggerLineGet
{
	// iterate thru the Line Selectors (Line0/Line1) and get the first one whose LineMode is Input
	HRESULT operator()(SVMatroxDigitizerPtr Digitizer, const SVGigeFeature& rFeature, _variant_t& rValue) const
	{ 
		HRESULT l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), std::string(rFeature.GetName().c_str()), rFeature.GetType(), rValue);
		
		_variant_t value( _T("Input") );
		l_Code = SVMatroxDigitizerInterface::GetFeature(*(Digitizer.get()), std::string(rFeature.GetName().c_str()), rFeature.GetType(), value);
		if (l_Code == S_OK)
		{
			rValue = _bstr_t(SvUl::createStdString(value).c_str()).Detach();
		}
		return l_Code;
	}
};

struct SVGigeTriggerLineSet
{
	// Setter - the value passed in is for the Feature Selector 
	HRESULT operator()(SVMatroxDigitizerPtr Digitizer, const SVGigeFeature& rFeature, const _variant_t& rValue) const
	{ 
		const SVGigeFeatureSelector& selector = rFeature.GetSelector();
		HRESULT l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), std::string(selector.GetName().c_str()), SVFeatureTypeEnum::SVTypeStringEnumeration, rValue);
		if (l_Code == S_OK)
		{
			l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), std::string(rFeature.GetName().c_str()), SVFeatureTypeEnum::SVTypeStringEnumeration, _variant_t("Input"));
		}
		return l_Code;
	}
};

struct SVGigeTriggerInvertGetter
{
	// Get the Trigger Line number
	HRESULT operator()(SVMatroxDigitizerPtr Digitizer, const SVGigeFeature& rFeature, _variant_t& rValue) const
	{ 
		//
		// Determine which Line is the Trigger...
		const SVGigeFeatureSelector& selector = rFeature.GetSelector();
		HRESULT l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), std::string(selector.GetName().c_str()), SVFeatureTypeEnum::SVTypeStringEnumeration, rValue);
		if (l_Code == S_OK)
		{
			_variant_t value(false);
			l_Code = SVMatroxDigitizerInterface::GetFeature(*(Digitizer.get()), std::string(rFeature.GetName().c_str()), rFeature.GetType(), value);
			if (l_Code == S_OK)
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
	HRESULT operator()(SVMatroxDigitizerPtr Digitizer, const SVGigeFeature& rFeature, const _variant_t& rValue) const
	{ 
		HRESULT l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), std::string(rFeature.GetName().c_str()), rFeature.GetType(), rValue);
		return l_Code;
	}
};

struct SVGigeStrobeLineGetter
{
	// iterate thru the Line Selectors (Line0/Line1) and get the first one whose LineMode is Output
	HRESULT operator()(SVMatroxDigitizerPtr Digitizer, const SVGigeFeature& feature, _variant_t& rValue) const
	{ 
		_variant_t value( _T("Output") );
		HRESULT l_Code = SVMatroxDigitizerInterface::GetFeature(*(Digitizer.get()), std::string(feature.GetName().c_str()), feature.GetType(), value);
		if (l_Code == S_OK)
		{
			rValue = _bstr_t(SvUl::createStdString(value).c_str()).Detach();
		}
		return l_Code;
	}
};

struct SVGigeStrobeLineSetter
{
	HRESULT operator()(SVMatroxDigitizerPtr Digitizer, const SVGigeFeature& rFeature, const _variant_t& rValue) const
	{ 
		const SVGigeFeatureSelector& selector = rFeature.GetSelector();
		HRESULT l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), std::string(selector.GetName().c_str()), SVFeatureTypeEnum::SVTypeStringEnumeration, rValue);
		if (l_Code == S_OK)
		{
			l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), std::string(rFeature.GetName().c_str()), SVFeatureTypeEnum::SVTypeStringEnumeration, _variant_t("Output"));
		}
		return l_Code;
	}
};

struct SVGigeStrobeInvertGetter
{
	HRESULT operator()(SVMatroxDigitizerPtr Digitizer, const SVGigeFeature& rFeature, _variant_t& rValue) const
	{ 
		// Get which line is the Strobe (LineMode = Output)
		HRESULT l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), std::string(rFeature.GetName().c_str()), rFeature.GetType(), rValue);
		
		_variant_t value( _T("Output") );
		l_Code = SVMatroxDigitizerInterface::GetFeature(*(Digitizer.get()), std::string(rFeature.GetName().c_str()), rFeature.GetType(), value);
		if (l_Code == S_OK)
		{
			rValue = _bstr_t(SvUl::createStdString(value).c_str()).Detach();
		}
		return l_Code;
	}
};

struct SVGigeStrobeInvertSetter
{
	// Setter
	HRESULT operator()(SVMatroxDigitizerPtr Digitizer, const SVGigeFeature& rFeature, const _variant_t& rValue) const
	{
		// Get which line is the Strobe (LineMode = Output)
		const SVGigeFeatureSelector& selector = rFeature.GetSelector();
		HRESULT l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), std::string(selector.GetName().c_str()), SVFeatureTypeEnum::SVTypeStringEnumeration, rValue);
		if (l_Code == S_OK)
		{
			l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), std::string(rFeature.GetName().c_str()), rFeature.GetType(), rValue);
		}
		return l_Code;
	}
};

struct SVGigeInputLineGetter
{
	// iterate thru the Line Selectors (Line0/Line1) and get the first one whose LineMode is Input ?
	HRESULT operator()(SVMatroxDigitizerPtr Digitizer, const SVGigeFeature& feature, _variant_t& rValue) const
	{ 
		_variant_t value( _T("Input") );
		HRESULT l_Code = SVMatroxDigitizerInterface::GetFeature(*(Digitizer.get()), std::string(feature.GetName().c_str()), feature.GetType(), value);
		if (l_Code == S_OK)
		{
			rValue = _bstr_t(SvUl::createStdString(value).c_str()).Detach();
		}
		return l_Code;
	}
};

struct SVGigeInputLineSetter
{
	HRESULT operator()(SVMatroxDigitizerPtr Digitizer, const SVGigeFeature& rFeature, const _variant_t& rValue) const
	{ 
		const SVGigeFeatureSelector& selector = rFeature.GetSelector();
		HRESULT l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), std::string(selector.GetName().c_str()), SVFeatureTypeEnum::SVTypeStringEnumeration, rValue);
		if (l_Code == S_OK)
		{
			l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), std::string(rFeature.GetName().c_str()), SVFeatureTypeEnum::SVTypeStringEnumeration, _variant_t("Input"));
		}
		return l_Code;
	}
};

struct SVGigeEventSetter
{
	HRESULT operator()(SVMatroxDigitizerPtr Digitizer, const SVGigeFeature& rFeature, const _variant_t& rValue) const
	{
		std::string l_value = SvUl::createStdString(rValue);
		std::string eventValue = l_value.c_str();
		HRESULT l_Code = SVMatroxDigitizerInterface::SetGigeEvent(*(Digitizer.get()), std::string(rFeature.GetName().c_str()), eventValue.c_str());
		return l_Code;
	}
};

const SVGigeDeviceParameterMap& SVGigeStandardCameraFeatures::GetStandardFeatures()
{
	static const SVGigeDeviceParameterMap cFeatures
	{
		///////////////////////////////////////////////////////////////////////
		// Exposure Mode - not supported yet
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterExposureMode,
			SVGigeDeviceParameterStruct
			{
				std::string(_T("Trigger Mode")), VT_BSTR,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{	
						_T("ExposureMode"),  SVFeatureTypeEnum::SVTypeStringEnumeration, 
						SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
					},
					SVGigeEmptyGetter(),
					SVGigeEmptySetter()
				}
			}
		},
	
		///////////////////////////////////////////////////////////////////////
		// Bayer Pattern - not supported yet
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterBayerPattern,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Bayer Pattern") ), VT_I4,
				SVGigeParameterAccessor
				{
					SVGigeFeature{ _T(""), SVFeatureTypeEnum::SVTypeStringEnumeration,
						SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite},
					SVGigeEmptyGetter(),
					SVGigeEmptySetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// X Offset
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterXOffset,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("X Offset") ), VT_I4,
				SVGigeParameterAccessor
				{ 
					SVGigeFeature{ _T("OffsetX"), SVFeatureTypeEnum::SVTypeInt32 }
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Y Offset
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterYOffset,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Y Offset") ), VT_I4,
				SVGigeParameterAccessor
				{
					SVGigeFeature{ _T("OffsetY"), SVFeatureTypeEnum::SVTypeInt32}
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Width
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterXSize,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("X Size") ), VT_I4,
				SVGigeParameterAccessor
				{
					SVGigeFeature{_T("Width"), SVFeatureTypeEnum::SVTypeInt32}
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Height
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterYSize,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Y Size") ), VT_I4,
				SVGigeParameterAccessor
				{
					SVGigeFeature{_T("Height"), SVFeatureTypeEnum::SVTypeInt32}
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Color Format
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterColorFormat,
			SVGigeDeviceParameterStruct
			{ 
				std::string( _T("Color Format") ),	VT_BSTR,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{ 
						_T("PixelFormat"), SVFeatureTypeEnum::SVTypeStringEnumeration,
						SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
						SVGigeFeatureSelector(),
						SVGigeFeatureStringEnumList
						{
							{{_T("Y"),			_T("Mono8")},
							{_T("YUV422"),		_T("YUV422Packed")},
							{_T("BayerRG8"),	_T("BayerRG8")},
							{_T("BayerRG12"),	_T("BayerRG12")},
							{_T("BayerGB8"),	_T("BayerGB8")},
							{_T("BayerGB10"),	_T("BayerGB10")},
							{_T("BayerGB12"),	_T("BayerGB12")},
							{_T("RGB8"),		_T("RGB8")},
							{_T("BGR8"),		_T("BGR8")}}
						}
					}
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Brightness
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterFeatureBrightness,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Brightness") ), VT_I4,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("BlackLevelRaw"), SVFeatureTypeEnum::SVTypeInt32,
						SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
						SVGigeFeatureSelector
						{
							_T("BlackLevelSelector"), 
							_T("All")
						}
					}
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Auto Exposure - not supported yet
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterFeatureAutoExposure,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Auto Exposure") ), VT_I4,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("ExposureTimeAuto"), SVFeatureTypeEnum::SVTypeStringEnumeration,
						SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
					},
					SVGigeEmptyGetter(),
					SVGigeEmptySetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Sharpness - not supported yet
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterFeatureSharpness,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Sharpness") ), VT_I4,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{ 
						_T(""), SVFeatureTypeEnum::SVTypeInt32,
						SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
					},
					SVGigeEmptyGetter(),
					SVGigeEmptySetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// WhiteBalanceU - not supported yet
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterFeatureWhiteBalanceU,	
			SVGigeDeviceParameterStruct
			{
				std::string( _T("White Balance - U") ), VT_I4,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T(""), SVFeatureTypeEnum::SVTypeInt32,
						SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
					},
					SVGigeEmptyGetter(), 
					SVGigeEmptySetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// WhiteBalanceV - not supported yet
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterFeatureWhiteBalanceV,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("White Balance - V") ), VT_I4,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T(""), SVFeatureTypeEnum::SVTypeInt32, 
						SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
					},
					SVGigeEmptyGetter(), 
					SVGigeEmptySetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Hue - not supported yet
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterFeatureHue,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Hue") ), VT_I4,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T(""), SVFeatureTypeEnum::SVTypeInt32,
						SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
					},
					SVGigeEmptyGetter(),
					SVGigeEmptySetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Saturation - not supported yet
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterFeatureSaturation,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Saturation") ), VT_I4,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{ 
						_T(""), SVFeatureTypeEnum::SVTypeInt32,
						SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
					},
					SVGigeEmptyGetter(),
					SVGigeEmptySetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Gamma
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterFeatureGamma,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Gamma") ), VT_R8,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("Gamma"), SVFeatureTypeEnum::SVTypeDouble
					}
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Shutter
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterFeatureShutter,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Shutter") ), VT_R8,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("ExposureTimeAbs"), SVFeatureTypeEnum::SVTypeDouble
					}
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Gain
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterFeatureGain,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Gain") ), VT_R8,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("GainAbs"), SVFeatureTypeEnum::SVTypeDouble, 
						SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
						SVGigeFeatureSelector
						{
							_T("GainSelector"), 
							_T("All")
						}
					}
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Trigger Source
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterTriggerSource,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Trigger Source") ), VT_BSTR,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("TriggerSource"), SVFeatureTypeEnum::SVTypeStringEnumeration,
						SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
						SVGigeFeatureSelector
						{
							_T("TriggerSelector"), 
							_T("FrameStart")
						},
						SVGigeFeatureStringEnumList
						{
							{{_T("All Trigger"), _T("All")},
							{_T("External Trigger"), _T("Line0")},
							{_T("Software Trigger"), _T("Software")}}
						}
					}
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Trigger Edge
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterTriggerEdge,
			SVGigeDeviceParameterStruct
			{ 
				std::string( _T("Trigger Edge") ), VT_BSTR,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("TriggerActivation"), SVFeatureTypeEnum::SVTypeStringEnumeration,
						SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
						SVGigeFeatureSelector
						{	
							_T("TriggerSelector"),
							_T("FrameStart") 
						},
						SVGigeFeatureStringEnumList
						{
							{{_T("Rising"), _T("RisingEdge")},
							{_T("Falling"), _T("FallingEdge")}}
						}
					}
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Trigger Invert
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterTriggerInvert,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Trigger Invert") ), VT_BOOL,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("LineInvertor"), SVFeatureTypeEnum::SVTypeBool,
						SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
					},
					SVGigeEmptyGetter(),
					SVGigeEmptySetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Trigger Line
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterTriggerLine,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Trigger Line") ), VT_BSTR,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("LineMode"), SVFeatureTypeEnum::SVTypeStringEnumeration,
						SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
						SVGigeFeatureSelector
						{
							_T("LineSelector"),
							_T("Line0")
						}
					},
					SVGigeTriggerLineGet(),
					SVGigeTriggerLineSet()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Trigger Delay
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterTriggerDelay,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Trigger Delay") ), VT_R8,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("TriggerDelayAbs"), 
						SVFeatureTypeEnum::SVTypeDouble
					}
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Trigger Enable
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterTriggerEnable,
			 SVGigeDeviceParameterStruct
			{
				std::string( _T("Trigger Enable") ), VT_BSTR,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("TriggerMode"), SVFeatureTypeEnum::SVTypeStringEnumeration,
						SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
						SVGigeFeatureSelector
						{
							_T("TriggerSelector"), 
							_T("FrameStart")
						},
						SVGigeFeatureStringEnumList
						{
							{{_T("Enable"), _T("On")},
							{_T("Disable"), _T("Off")}}
						}
					}
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Strobe Source - Non Standard
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterStrobeSource,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Strobe Source") ), VT_BSTR,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("StrobeSource"), SVFeatureTypeEnum::SVTypeStringEnumeration,
						SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
					},
					SVGigeEmptyGetter(),
					SVGigeEmptySetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Strobe Edge - Non Standard
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterStrobeEdge,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Strobe Edge") ), VT_BSTR,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("StrobePolarity"), SVFeatureTypeEnum::SVTypeStringEnumeration,
						SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
					},
					SVGigeEmptyGetter(),
					SVGigeEmptySetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Strobe Invert
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterStrobeInvert,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Strobe Invert") ), VT_BOOL,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("LineInvertor"), SVFeatureTypeEnum::SVTypeBool,
						SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
					},
					SVGigeEmptyGetter(),
					SVGigeEmptySetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Strobe Delay - Non Standard
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterStrobeDelay,
			SVGigeDeviceParameterStruct
			{
				std::string(_T("Strobe Delay")), VT_R8,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("StrobeDelayAbs"), SVFeatureTypeEnum::SVTypeDouble,
						SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
					},
					SVGigeEmptyGetter(),
					SVGigeEmptySetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Strobe Duration - Non Standard
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterStrobeDuration,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Strobe Duration") ),	VT_R8,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("StrobeDurationAbs"), SVFeatureTypeEnum::SVTypeDouble,
						SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
					},
					SVGigeEmptyGetter(),
					SVGigeEmptySetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Strobe Line
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterStrobeLine,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Strobe Line") ), VT_BSTR,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("LineMode"), SVFeatureTypeEnum::SVTypeStringEnumeration,
						SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
						SVGigeFeatureSelector
						{
							_T("LineSelector"), 
							_T("Line1")
						}
					},
					SVGigeStrobeLineGetter(),
					SVGigeStrobeLineSetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Strobe Enable - Non Standard
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterStrobeEnable,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Strobe Enable") ), VT_BSTR,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("StrobeEnable"), SVFeatureTypeEnum::SVTypeStringEnumeration,
						SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
					},
					SVGigeEmptyGetter(),
					SVGigeEmptySetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Horizontal Binning
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterHorizontalBinning,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Horizontal Binning") ), VT_I4,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("BinningHorizontal"), SVFeatureTypeEnum::SVTypeInt32,
						SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
					},
					SVGigeEmptyGetter(),
					SVGigeEmptySetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Vertical Binning
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterVerticalBinning,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Vertical Binning") ), VT_I4,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("BinningVertical"), SVFeatureTypeEnum::SVTypeInt32,
						SVGigeFeature::NotSupported, SVGigeFeature::ReadWrite
					},
					SVGigeEmptyGetter(),
					SVGigeEmptySetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Packet Size
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterPacketSize,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Packet Size") ), VT_I4,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("GevSCPSPacketSize"), 
						SVFeatureTypeEnum::SVTypeInt32
					}
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// LUT Enable
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterLutEnable,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("LUT Enable") ), VT_BOOL,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{ 
						_T("LUTEnable"), SVFeatureTypeEnum::SVTypeBool,
						SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
						SVGigeFeatureSelector
						{
							_T("LUTSelector"),
							_T("Luminance")
						}
					}
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// LUT Value Array
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterLutArray,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("LUT") ), VT_I4 | VT_ARRAY,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{ 
						_T("LUTValue"), SVFeatureTypeEnum::SVTypeInt32,
						SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
						SVGigeFeatureSelector
						{
							_T("LUTSelector"), 
							_T("Luminance")
						}
					},
					SVGigeLUTGetter(),
					SVGigeLUTSetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Input
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterLineInput,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Input") ), VT_BSTR,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("LineMode"), SVFeatureTypeEnum::SVTypeStringEnumeration,
						SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
						SVGigeFeatureSelector
						{
							_T("LineSelector")
						}
					},
					SVGigeInputLineGetter(),
					SVGigeInputLineSetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Input Event
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterInputEvent,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Input Event") ), VT_BSTR,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("EventNotification"), SVFeatureTypeEnum::SVTypeStringEnumeration,
						SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
						SVGigeFeatureSelector
						{
							_T("EventSelector")
						},
						SVGigeFeatureStringEnumList
						{
							{{_T("Enable"), _T("On")},
							{_T("Disable"), _T("Off")}}
						}
					},
					SVGigeEmptyGetter(),
					SVGigeEventSetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Vendor Name
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterVendorName,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Vendor Name") ), VT_BSTR,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("DeviceVendorName"), SVFeatureTypeEnum::SVTypeString,
						SVGigeFeature::Supported, SVGigeFeature::ReadOnly
					}
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Model Name
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterModelName,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Model Name") ), VT_BSTR,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("DeviceModelName"), SVFeatureTypeEnum::SVTypeString,
						SVGigeFeature::Supported, SVGigeFeature::ReadOnly
					}
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Serial Number
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterSerialNumber,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Serial Number") ), VT_BSTR,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("DeviceID"), SVFeatureTypeEnum::SVTypeString,
						SVGigeFeature::Supported, SVGigeFeature::ReadOnly
					},
					SVGigeReadSerialNumber(),
					SVGigeEmptySetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// IP Address
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterIPAddress,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("IP Address") ), VT_BSTR,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("DeviceID"), SVFeatureTypeEnum::SVTypeString,
						SVGigeFeature::Supported, SVGigeFeature::ReadOnly
					},
					SVGigeReadIPAddress(),
					SVGigeEmptySetter()
				}
			}
		},

		///////////////////////////////////////////////////////////////////////
		// Camera Default Settings
		///////////////////////////////////////////////////////////////////////
		{ SvDef::SVGigeParameterCameraDefaultSettings,
			SVGigeDeviceParameterStruct
			{
				std::string( _T("Default Settings") ), VT_BSTR,
				SVGigeParameterAccessor
				{
					SVGigeFeature
					{
						_T("UserSetDefaultSelector"), SVFeatureTypeEnum::SVTypeStringEnumeration,
						SVGigeFeature::Supported, SVGigeFeature::ReadWrite,
						SVGigeFeatureSelector(),
						SVGigeFeatureStringEnumList
						{
							{{_T("Enable"),	_T("Default")},
							{_T("Disable"), _T("UserSet1")}}
						}
					}
				}
			}
		}
	};

	return cFeatures;
}

