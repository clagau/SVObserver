//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraDBContent
//* .File Name       : $Workfile:   SVGigeCameraDBContent.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2013 19:36:48  $
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#import <msxml3.dll> raw_interfaces_only 
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <memory>
//Moved to precompiled header: #include <utility>
//Moved to precompiled header: #include <boost/config.hpp>
#include "SVGigeDeviceParameterStruct.h"
#include "SVGigeEmptyGetter.h"
#include "SVGigeEmptySetter.h"
#include "SVUtilityLibrary/StringToEnum.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVGigeCameraDBContent.h"
#pragma endregion Includes

typedef std::wstring WString;

// Element Tags
enum ElementTagEnums
{
	GigeParameterMap,
	GigeParameter,
	GigeDeviceParameterStruct,
	GigeParameterAccessor,
	GigeAccessor,
	GigeFeature,
	GigeFeatureSelector,
	GigeFeatureStringEnumList,
	GigeFeatureStringEnum
};

// Element Map
typedef std::map<WString, ElementTagEnums> ElementMap;
static const ElementMap cElements
{
	{L"SVGigeParameterMap",				GigeParameterMap},			// Gige Parameter Map (root tag)
	{L"SVGigeParameter",				GigeParameter},				// Gige Parameter 
	{L"SVGigeDeviceParameterStruct",	GigeDeviceParameterStruct},	// Gige Device Parameter
	{L"SVGigeParameterAccessor",		GigeParameterAccessor},		// Gige Parameter Accessor
	{L"SVGigeAccessor",					GigeAccessor},				// Gige Accessor optional
	{L"SVGigeFeature",					GigeFeature},				// Gige Feature
	{L"SVGigeFeatureSelector",			GigeFeatureSelector},		// Gige Feature Selector (optional)
	{L"SVGigeFeatureStringEnumList",	GigeFeatureStringEnumList},	// Gige Feature StringEnum List (only for string list)
	{L"SVGigeFeatureStringEnum",		GigeFeatureStringEnum}		// Gige Feature StringEnum (list entry)
};

// GigeParameterEnum Map
typedef std::map<WString, SvDef::SVGigeParameterEnum> GigeParameterEnumMap;
static const GigeParameterEnumMap cGigeParameterEnums
{
	{L"SVGigeParameterExposureMode",			SvDef::SVGigeParameterExposureMode},
	{L"SVGigeParameterXOffset",					SvDef::SVGigeParameterXOffset},
	{L"SVGigeParameterYOffset",					SvDef::SVGigeParameterYOffset},
	{L"SVGigeParameterXSize",					SvDef::SVGigeParameterXSize},
	{L"SVGigeParameterYSize",					SvDef::SVGigeParameterYSize},
	{L"SVGigeParameterColorFormat",				SvDef::SVGigeParameterColorFormat},
	{L"SVGigeParameterBayerPattern",			SvDef::SVGigeParameterBayerPattern},
	{L"SVGigeParameterFeatureBrightness",		SvDef::SVGigeParameterFeatureBrightness},
	{L"SVGigeParameterFeatureAutoExposure",		SvDef::SVGigeParameterFeatureAutoExposure},
	{L"SVGigeParameterFeatureSharpness",		SvDef::SVGigeParameterFeatureSharpness},
	{L"SVGigeParameterFeatureWhiteBalanceU",	SvDef::SVGigeParameterFeatureWhiteBalanceU},
	{L"SVGigeParameterFeatureWhiteBalanceV",	SvDef::SVGigeParameterFeatureWhiteBalanceV},
	{L"SVGigeParameterFeatureHue",				SvDef::SVGigeParameterFeatureHue},
	{L"SVGigeParameterFeatureSaturation",		SvDef::SVGigeParameterFeatureSaturation},
	{L"SVGigeParameterFeatureGamma",			SvDef::SVGigeParameterFeatureGamma},
	{L"SVGigeParameterFeatureShutter",			SvDef::SVGigeParameterFeatureShutter},
	{L"SVGigeParameterFeatureGain",				SvDef::SVGigeParameterFeatureGain},
	{L"SVGigeParameterTriggerSource",			SvDef::SVGigeParameterTriggerSource},
	{L"SVGigeParameterTriggerEdge",				SvDef::SVGigeParameterTriggerEdge},
	{L"SVGigeParameterTriggerInvert",			SvDef::SVGigeParameterTriggerInvert},
	{L"SVGigeParameterTriggerLine",				SvDef::SVGigeParameterTriggerLine},
	{L"SVGigeParameterTriggerDelay",			SvDef::SVGigeParameterTriggerDelay},
	{L"SVGigeParameterTriggerEnable",			SvDef::SVGigeParameterTriggerEnable},
	{L"SVGigeParameterStrobeSource",			SvDef::SVGigeParameterStrobeSource},
	{L"SVGigeParameterStrobeEdge",				SvDef::SVGigeParameterStrobeEdge},
	{L"SVGigeParameterStrobeInvert",			SvDef::SVGigeParameterStrobeInvert},
	{L"SVGigeParameterStrobeDelay",				SvDef::SVGigeParameterStrobeDelay},
	{L"SVGigeParameterStrobeDuration",			SvDef::SVGigeParameterStrobeDuration},
	{L"SVGigeParameterStrobeLine",				SvDef::SVGigeParameterStrobeLine},
	{L"SVGigeParameterStrobeEnable",			SvDef::SVGigeParameterStrobeEnable},
	{L"SVGigeParameterLutEnable",				SvDef::SVGigeParameterLutEnable},
	{L"SVGigeParameterLutArray",				SvDef::SVGigeParameterLutArray},
	{L"SVGigeParameterLineInput",				SvDef::SVGigeParameterLineInput},
	{L"SVGigeParameterInputEvent",				SvDef::SVGigeParameterInputEvent},
	{L"SVGigeParameterInputEventName",			SvDef::SVGigeParameterInputEventName},
	{L"SVGigeParameterHorizontalBinning",		SvDef::SVGigeParameterHorizontalBinning},
	{L"SVGigeParameterVerticalBinning",			SvDef::SVGigeParameterVerticalBinning},
	{L"SVGigeParameterCameraDefaultSettings",	SvDef::SVGigeParameterCameraDefaultSettings},
	{L"SVGigeParameterCustom1",					SvDef::SVGigeParameterCustom1},
	{L"SVGigeParameterCustom2",					SvDef::SVGigeParameterCustom2},
	{L"SVGigeParameterCustom3",					SvDef::SVGigeParameterCustom3},
	{L"SVGigeParameterCustom4",					SvDef::SVGigeParameterCustom4},
	{L"SVGigeParameterCustom5",					SvDef::SVGigeParameterCustom5},
	{L"SVGigeParameterCustom6",					SvDef::SVGigeParameterCustom6},
	{L"SVGigeParameterCustom7",					SvDef::SVGigeParameterCustom7},
	{L"SVGigeParameterCustom8",					SvDef::SVGigeParameterCustom8},
	{L"SVGigeParameterCustom9",					SvDef::SVGigeParameterCustom9},
	{L"SVGigeParameterCustom10",				SvDef::SVGigeParameterCustom10},
	{L"SVGigeParameterCustom11",				SvDef::SVGigeParameterCustom11},
	{L"SVGigeParameterCustom12",				SvDef::SVGigeParameterCustom12},
	{L"SVGigeParameterCustom13",				SvDef::SVGigeParameterCustom13},
	{L"SVGigeParameterCustom14",				SvDef::SVGigeParameterCustom14},
	{L"SVGigeParameterCustom15",				SvDef::SVGigeParameterCustom15},
	{L"SVGigeParameterCustom16",				SvDef::SVGigeParameterCustom16},
	{L"SVGigeParameterCustom17",				SvDef::SVGigeParameterCustom17},
	{L"SVGigeParameterCustom18",				SvDef::SVGigeParameterCustom18},
	{L"SVGigeParameterCustom19",				SvDef::SVGigeParameterCustom19},
	{L"SVGigeParameterCustom20",				SvDef::SVGigeParameterCustom20},
	{L"SVGigeParameterCustom21",				SvDef::SVGigeParameterCustom21},
	{L"SVGigeParameterCustom22",				SvDef::SVGigeParameterCustom22},
	{L"SVGigeParameterCustom23",				SvDef::SVGigeParameterCustom23},
	{L"SVGigeParameterCustom24",				SvDef::SVGigeParameterCustom24},
	{L"SVGigeParameterCustom25",				SvDef::SVGigeParameterCustom25},
	{L"SVGigeParameterCustom26",				SvDef::SVGigeParameterCustom26},
	{L"SVGigeParameterCustom27",				SvDef::SVGigeParameterCustom27},
	{L"SVGigeParameterCustom28",				SvDef::SVGigeParameterCustom28},
	{L"SVGigeParameterCustom29",				SvDef::SVGigeParameterCustom29},
	{L"SVGigeParameterCustom30",				SvDef::SVGigeParameterCustom30}
};


// VariantTypeEnum Map
typedef std::map<WString, VARTYPE> VariantTypeEnumMap;
static const VariantTypeEnumMap cVariantTypeEnum
{
	{L"VT_I1",		VT_I1},
	{L"VT_I2",		VT_I2},
	{L"VT_I4",		VT_I4},
	{L"VT_UI1",		VT_UI1},
	{L"VT_UI2",		VT_UI2},
	{L"VT_UI4",		VT_UI4},
	{L"VT_R4",		VT_R4},
	{L"VT_R8",		VT_R8},
	{L"VT_BSTR",	VT_BSTR}
};

// FeatureTypeStringEnum Map
typedef std::map<WString, SVMatroxDigitizerFeature::SVFeatureTypeEnum> FeatureTypeStringEnumMap;
static const FeatureTypeStringEnumMap cFeatureTypeStringEnums
{
	{L"SVTypeCommand",				SVMatroxDigitizerFeature::SVTypeCommand},
	{L"SVTypeInt32",				SVMatroxDigitizerFeature::SVTypeInt32},
	{L"SVTypeDouble",				SVMatroxDigitizerFeature::SVTypeDouble},
	{L"SVTypeString",				SVMatroxDigitizerFeature::SVTypeString},
	{L"SVTypeIntegerEnumeration",	SVMatroxDigitizerFeature::SVTypeIntegerEnumeration},
	{L"SVTypeStringEnumeration",	SVMatroxDigitizerFeature::SVTypeStringEnumeration},
	{L"SVTypeBool",					SVMatroxDigitizerFeature::SVTypeBool}
};

// FeatureSupportedStringEnum Map
typedef std::map<WString, SVGigeFeature::FeatureSupportedEnum> FeatureSupportedStringEnumMap;
static const FeatureSupportedStringEnumMap cFeatureSupportedStringEnums
{
	{L"NotSupported",	SVGigeFeature::NotSupported},
	{L"Supported",		SVGigeFeature::Supported}
};

// FeatureAccessStringEnum Map
typedef std::map<WString, SVGigeFeature::FeatureAccessEnum> FeatureAccessStringEnumMap;
static const FeatureAccessStringEnumMap FeatureAccessStringEnums
{
	{L"ReadOnly",	SVGigeFeature::ReadOnly},
	{L"ReadWrite",	SVGigeFeature::ReadWrite}
};

SVGigeCameraDBContent::SVGigeCameraDBContent()
: m_errorCode(S_OK), m_GigeParameterEnum(SvDef::SVGigeParameterFeatureOverrides)
{
}

SVGigeCameraDBContent::~SVGigeCameraDBContent()
{
}

// cppcheck-suppress unusedFunction
HRESULT SVGigeCameraDBContent::startElement(unsigned short* , int , unsigned short* pwchLocalName, int cchLocalName, unsigned short* , int , MSXML2::ISAXAttributes* pAttributes)
{
	HRESULT hr = S_OK;

	WString elementName(reinterpret_cast<wchar_t *>(pwchLocalName), cchLocalName);
	
	ElementMap::const_iterator it = cElements.find(elementName);
	if (it != cElements.end())
	{
		switch (it->second)
		{
			case GigeParameterMap:
				break;

			case GigeParameter:
				GetGigeParameterAttributes(pAttributes);
				break;

			case GigeDeviceParameterStruct:
				GetGigeDeviceParameterStructAttributes(pAttributes);
				break;

			case GigeParameterAccessor:
				break;

			case GigeAccessor:
				GetGigeAccessorAttributes(pAttributes);
				break;

			case GigeFeature:
				GetGigeFeatureAttributes(pAttributes);
				break;

			case GigeFeatureSelector:
				GetGigeFeatureSelectorAttributes(pAttributes);
				break;

			case GigeFeatureStringEnumList:
				break;

			case GigeFeatureStringEnum:
				GetGigeFeatureStringEnumAttributes(pAttributes);
				break;
		}
	}
    return hr;
}
 
// cppcheck-suppress unusedFunction
HRESULT SVGigeCameraDBContent::endElement(unsigned short* , int , unsigned short* pwchLocalName, int cchLocalName, unsigned short* , int )
{
	WString elementName(reinterpret_cast<wchar_t *>(pwchLocalName), cchLocalName);
	
	ElementMap::const_iterator it = cElements.find(elementName);
	if (it != cElements.end())
	{
		switch (it->second)
		{
			case GigeParameterMap:
				break;

			case GigeParameter:
				break;

			case GigeDeviceParameterStruct:
				{
					// Construct
					SVGigeDeviceParameterStruct gigeDeviceParameterStruct(m_GigeDeviceParameterInfo.m_GigeDeviceParameterName, 
																			m_GigeDeviceParameterInfo.m_GigeDeviceParameterDataType,
																			ConstructGigeParameterAccessor());
					// Add to map
					m_GigeDeviceParameterMap.insert(std::make_pair(m_GigeParameterEnum, gigeDeviceParameterStruct));

					// Clear
					m_GigeDeviceParameterInfo.clear();
					m_GigeParameterEnum = SvDef::SVGigeParameterFeatureOverrides;
				}
				break;

			case GigeParameterAccessor:
				break;

			case GigeAccessor:
				break;

			case GigeFeature:
				break;

			case GigeFeatureSelector:
				break;

			case GigeFeatureStringEnumList:
				break;
		}
	}
    return S_OK;
}

void SVGigeCameraDBContent::ReportError(wchar_t* pErrorMsg, HRESULT errorCode)
{
	m_errorCode = errorCode;
	m_errorText = SvUl::to_utf8( std::wstring(pErrorMsg));
}

// Get XML Attributes for SVGigeParameter
void SVGigeCameraDBContent::GetGigeParameterAttributes(MSXML2::ISAXAttributes* pAttributes)
{
	int numAttributes = 0;
	pAttributes->getLength(&numAttributes);
	for (int i = 0;i < numAttributes;i++)
	{
		wchar_t* name(nullptr);
		wchar_t* value(nullptr);
		int nameSize = 0;
		int valueSize = 0;
		pAttributes->getLocalName(i, reinterpret_cast<unsigned short **>(&name), &nameSize);
		pAttributes->getValue(i, reinterpret_cast<unsigned short **>(&value), &valueSize);

		if (wcsncmp(name, L"SVGigeParameterEnum", nameSize) == 0)
		{
			SvDef::SVGigeParameterEnum enumValue;
			bool bRetVal = StringToEnum<SvDef::SVGigeParameterEnum, GigeParameterEnumMap, LPCWSTR>::GetEnum(cGigeParameterEnums, value, enumValue); 
			if (bRetVal)
			{
				m_GigeParameterEnum = enumValue;
			}
		}
	}
}

// Get XML Attributes for SVGigeDeviceParameterStruct
void SVGigeCameraDBContent::GetGigeDeviceParameterStructAttributes(MSXML2::ISAXAttributes* pAttributes)
{
	int numAttributes = 0;
	pAttributes->getLength(&numAttributes);
	for (int i = 0;i < numAttributes;i++)
	{
		wchar_t* name(nullptr);
		wchar_t* value(nullptr);
		int nameSize = 0;
		int valueSize = 0;
		pAttributes->getLocalName(i, reinterpret_cast<unsigned short **>(&name), &nameSize);
		pAttributes->getValue(i, reinterpret_cast<unsigned short **>(&value), &valueSize);

		if (wcsncmp(name, L"DeviceParameterName", nameSize) == 0)
		{
			m_GigeDeviceParameterInfo.m_GigeDeviceParameterName = SvUl::to_utf8( _bstr_t(value) );
		}
		else if (wcsncmp(name, L"DeviceParameterType", nameSize) == 0)
		{
			// Translate from String to VARTYPE
			m_GigeDeviceParameterInfo.m_GigeDeviceParameterDataType = GetVarType(value);
		}
	}
}

// Get XML Attributes for SVGigeParameterAccessor
void SVGigeCameraDBContent::GetGigeAccessorAttributes(MSXML2::ISAXAttributes* pAttributes)
{
	int numAttributes = 0;
	pAttributes->getLength(&numAttributes);
	for (int i = 0;i < numAttributes;i++)
	{
		wchar_t* name(nullptr);
		wchar_t* value(nullptr);
		int nameSize = 0;
		int valueSize = 0;
		pAttributes->getLocalName(i, reinterpret_cast<unsigned short **>(&name), &nameSize);
		pAttributes->getValue(i, reinterpret_cast<unsigned short **>(&value), &valueSize);

		// Lookup Attribute
		if (wcsncmp(name, L"Getter", nameSize) == 0)
		{
			m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_accessor.m_getterName = SvUl::to_utf8( _bstr_t(value) );
		}
		else if (wcsncmp(name, L"Setter", nameSize) == 0)
		{
			m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_accessor.m_setterName = SvUl::to_utf8( _bstr_t(value) );
		}
	}
}

// Get XML Attributes for SVGigeFeature
void SVGigeCameraDBContent::GetGigeFeatureAttributes(MSXML2::ISAXAttributes* pAttributes)
{
	int numAttributes = 0;
	pAttributes->getLength(&numAttributes);
	for (int i = 0;i < numAttributes;i++)
	{
		wchar_t* name(nullptr);
		wchar_t* value(nullptr);
		int nameSize = 0;
		int valueSize = 0;
		pAttributes->getLocalName(i, reinterpret_cast<unsigned short **>(&name), &nameSize);
		pAttributes->getValue(i, reinterpret_cast<unsigned short **>(&value), &valueSize);

		if (wcsncmp(name, L"FeatureName", nameSize) == 0)
		{
			m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeFeatureName = SvUl::to_utf8( _bstr_t(value) );
		}
		else if (wcsncmp(name, L"FeatureType", nameSize) == 0)
		{
			SVMatroxDigitizerFeature::SVFeatureTypeEnum enumValue;
			bool bRetVal = StringToEnum<SVMatroxDigitizerFeature::SVFeatureTypeEnum, FeatureTypeStringEnumMap, LPCWSTR>::GetEnum(cFeatureTypeStringEnums, value, enumValue);
			if (bRetVal)
			{
				m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeFeatureType = enumValue;
			}
		}
		else if (wcsncmp(name, L"FeatureSupportedEnum", nameSize) == 0)
		{
			SVGigeFeature::FeatureSupportedEnum enumValue;
			bool bRetVal = StringToEnum<SVGigeFeature::FeatureSupportedEnum, FeatureSupportedStringEnumMap, LPCWSTR>::GetEnum(cFeatureSupportedStringEnums, value, enumValue);
			if (bRetVal)
			{
				m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_FeatureSupported = enumValue;
			}
		}
		else if (wcsncmp(name, L"FeatureAccessEnum", nameSize) == 0)
		{
			SVGigeFeature::FeatureAccessEnum enumValue;
			bool bRetVal = StringToEnum<SVGigeFeature::FeatureAccessEnum, FeatureAccessStringEnumMap, LPCWSTR>::GetEnum(FeatureAccessStringEnums , value, enumValue);
			if (bRetVal)
			{
				m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_FeatureAccess = enumValue;
			}
		}
	}
}

// Get XML Attributes for SVGigeFeatureSelector
void SVGigeCameraDBContent::GetGigeFeatureSelectorAttributes(MSXML2::ISAXAttributes* pAttributes)
{
	SVGigeFeatureSelectorInfo gigeFeatureSelectorInfo;

	int numAttributes = 0;
	pAttributes->getLength(&numAttributes);
	for (int i = 0;i < numAttributes;i++)
	{
		wchar_t* name(nullptr);
		wchar_t* value(nullptr);
		int nameSize = 0;
		int valueSize = 0;
		pAttributes->getLocalName(i, reinterpret_cast<unsigned short **>(&name), &nameSize);
		pAttributes->getValue(i, reinterpret_cast<unsigned short **>(&value), &valueSize);

		if (wcsncmp(name, L"SelectorName", nameSize) == 0)
		{
			gigeFeatureSelectorInfo.m_GigeFeatureSelectorName = SvUl::to_utf8( _bstr_t(value) );
		}
		else if (wcsncmp(name, L"SelectorValue", nameSize) == 0)
		{
			gigeFeatureSelectorInfo.m_GigeFeatureSelectorValue = SvUl::to_utf8( _bstr_t(value) );
		}
	}
	// Set Feature Selector
	m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeSelector = gigeFeatureSelectorInfo;
}

// Get XML Attributes for GetGigeFeatureStringEnumList
void SVGigeCameraDBContent::GetGigeFeatureStringEnumAttributes(MSXML2::ISAXAttributes* pAttributes)
{
	std::string deviceParamString;
	std::string gigeFeatureString;

	int numAttributes = 0;
	pAttributes->getLength(&numAttributes);
	for (int i = 0;i < numAttributes;i++)
	{
		wchar_t* name(nullptr);
		wchar_t* value(nullptr);
		int nameSize = 0;
		int valueSize = 0;
		pAttributes->getLocalName(i, reinterpret_cast<unsigned short **>(&name), &nameSize);
		pAttributes->getValue(i, reinterpret_cast<unsigned short **>(&value), &valueSize);

		if (wcsncmp(name, L"SVDeviceParamString", nameSize) == 0)
		{
			deviceParamString = SvUl::to_utf8( _bstr_t(value) );
		}
		else if (wcsncmp(name, L"SVGigeFeatureString", nameSize) == 0)
		{
			gigeFeatureString = SvUl::to_utf8( _bstr_t(value) );
		}
	}
	if (!deviceParamString.empty() && !gigeFeatureString.empty())
	{
		m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeFeatureStringEnumList.push_back(std::make_pair(deviceParamString, gigeFeatureString));
	}
}

// Construct
SVGigeParameterAccessor SVGigeCameraDBContent::ConstructGigeParameterAccessor()
{
	// Construct SVGigeParameterAccessor
	std::shared_ptr<SVGigeParameterAccessor> gigeParameterAccessorHolder;

	// Check for Selector
	if (!m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeSelector.m_GigeFeatureSelectorName.empty())
	{
		if (m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeFeatureStringEnumList.size())
		{
			SVGigeFeature feature(m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeFeatureName, 
				m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeFeatureType,
				m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_FeatureSupported,
				m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_FeatureAccess,
				SVGigeFeatureSelector(m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeSelector.m_GigeFeatureSelectorName,
								m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeSelector.m_GigeFeatureSelectorValue),
				SVGigeFeatureStringEnumList(m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeFeatureStringEnumList.begin(), 
				m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeFeatureStringEnumList.end()));

			// check for Accessor(Getter/Setter) 
			if (!m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_accessor.m_getterName.empty())
			{
				gigeParameterAccessorHolder = std::shared_ptr<SVGigeParameterAccessor>{ new SVGigeParameterAccessor(feature, GetGetterFunc(), GetSetterFunc()) };
			}
			else
			{
				gigeParameterAccessorHolder = std::shared_ptr<SVGigeParameterAccessor>{ new SVGigeParameterAccessor(feature) };
			}
		}
		else
		{
			SVGigeFeature feature(m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeFeatureName, 
				m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeFeatureType,
				m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_FeatureSupported,
				m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_FeatureAccess,
				SVGigeFeatureSelector(m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeSelector.m_GigeFeatureSelectorName,
								m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeSelector.m_GigeFeatureSelectorValue));

			// check for Accessor(Getter/Setter) 
			if (!m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_accessor.m_getterName.empty())
			{
				gigeParameterAccessorHolder = std::shared_ptr<SVGigeParameterAccessor>{ new SVGigeParameterAccessor(feature, GetGetterFunc(), GetSetterFunc()) };
			}
			else
			{
				gigeParameterAccessorHolder = std::shared_ptr<SVGigeParameterAccessor>{ new SVGigeParameterAccessor(feature) };
			}
		}
	}
	else
	{
		// No Feature Selector, Check for String Enum List
		if (m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeFeatureStringEnumList.size())
		{
			SVGigeFeature feature(m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeFeatureName, 
				m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeFeatureType,
				m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_FeatureSupported,
				m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_FeatureAccess,
				SVGigeFeatureSelector(m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeSelector.m_GigeFeatureSelectorName,
								m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeSelector.m_GigeFeatureSelectorValue),
				SVGigeFeatureStringEnumList(m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeFeatureStringEnumList.begin(), 
				m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeFeatureStringEnumList.end()));

			// check for Accessor(Getter/Setter) 
			if (!m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_accessor.m_getterName.empty())
			{
				gigeParameterAccessorHolder = std::shared_ptr<SVGigeParameterAccessor>{ new SVGigeParameterAccessor(feature, GetGetterFunc(), GetSetterFunc()) };
			}
			else
			{
				gigeParameterAccessorHolder = std::shared_ptr<SVGigeParameterAccessor>{ new SVGigeParameterAccessor(feature) };
			}
		}
		else
		{
			// No Feature Selector, No StringEnumList
			SVGigeFeature feature(m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeFeatureName, 
					m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeFeatureType,
					m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_FeatureSupported,
					m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_FeatureAccess);

			// check for Accessor(Getter/Setter) 
			if (!m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_accessor.m_getterName.empty())
			{
				gigeParameterAccessorHolder = std::shared_ptr<SVGigeParameterAccessor>{ new SVGigeParameterAccessor(feature, GetGetterFunc(), GetSetterFunc()) };
			}
			else
			{
				gigeParameterAccessorHolder = std::shared_ptr<SVGigeParameterAccessor>{ new SVGigeParameterAccessor(feature) };
			}
		}
	}
	return SVGigeParameterAccessor(*(gigeParameterAccessorHolder.get()));
}

// Convert String to Variant VARTYPE
VARTYPE SVGigeCameraDBContent::GetVarType(wchar_t* vartypeString) const
{
	VARTYPE varType = VT_EMPTY;

	// check for array
	wchar_t* pPtr = wcsstr(vartypeString, L"VT_ARRAY");
	if (pPtr)
	{
		// strip it out
		pPtr--;
		while (pPtr > vartypeString)
		{
			if (pPtr[0] == L' ')
				pPtr--;
			else if (pPtr[0] == L'|')
				pPtr--;
			else
				break;
		}
		VariantTypeEnumMap::const_iterator it = cVariantTypeEnum.find(vartypeString);
		if (it != cVariantTypeEnum.end())
		{
			varType = it->second | VT_ARRAY;
		}
	}
	else
	{
		VariantTypeEnumMap::const_iterator it = cVariantTypeEnum.find(vartypeString);
		if (it != cVariantTypeEnum.end())
		{
			varType = it->second;
		}
	}
	return varType;
}

SVGigeParameterAccessor::ParamGetter SVGigeCameraDBContent::GetGetterFunc()
{
	SVGigeParameterAccessor::ParamGetter getter;
	if (m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_accessor.m_getterName == "SVGigeEmptyGetter")
	{
		getter = SVGigeParameterAccessor::ParamGetter(SVGigeEmptyGetter());
	}
	return getter;
}

SVGigeParameterAccessor::ParamSetter SVGigeCameraDBContent::GetSetterFunc()
{
	SVGigeParameterAccessor::ParamSetter setter;
	if (m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_accessor.m_setterName == "SVGigeEmptySetter")
	{
		setter = SVGigeParameterAccessor::ParamSetter(SVGigeEmptySetter());
	}
	
	return setter;
}

const SVGigeDeviceParameterMap& SVGigeCameraDBContent::GetGigeDeviceParameterMap() const
{
	return m_GigeDeviceParameterMap;
}

