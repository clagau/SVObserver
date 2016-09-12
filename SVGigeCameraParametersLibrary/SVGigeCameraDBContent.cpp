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
#import <msxml6.dll> raw_interfaces_only 
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <utility>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/assign.hpp>
#include "SVGigeDeviceParameterStruct.h"
#include "SVGigeEmptyGetter.h"
#include "SVGigeEmptySetter.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVUtilityLibrary/StringToEnum.h"
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
static ElementMap Elements = boost::assign::map_list_of<>
	(L"SVGigeParameterMap",				GigeParameterMap)			// Gige Parameter Map (root tag)
	(L"SVGigeParameter",				GigeParameter)				// Gige Parameter 
	(L"SVGigeDeviceParameterStruct",	GigeDeviceParameterStruct)	// Gige Device Parameter
	(L"SVGigeParameterAccessor",		GigeParameterAccessor)		// Gige Parameter Accessor
	(L"SVGigeAccessor",					GigeAccessor)				// Gige Accessor optional
	(L"SVGigeFeature",					GigeFeature)				// Gige Feature
	(L"SVGigeFeatureSelector",			GigeFeatureSelector)		// Gige Feature Selector (optional)
	(L"SVGigeFeatureStringEnumList",	GigeFeatureStringEnumList)	// Gige Feature StringEnum List (only for string list)
	(L"SVGigeFeatureStringEnum",		GigeFeatureStringEnum)		// Gige Feature StringEnum (list entry)
;

// GigeParameterEnum Map
typedef std::map<WString, SVGigeParameterEnum> GigeParameterEnumMap;
static GigeParameterEnumMap GigeParameterEnums = boost::assign::map_list_of<>
(L"SVGigeParameterExposureMode", SVGigeParameterExposureMode)
(L"SVGigeParameterXOffset", SVGigeParameterXOffset)
(L"SVGigeParameterYOffset", SVGigeParameterYOffset)
(L"SVGigeParameterXSize", SVGigeParameterXSize)
(L"SVGigeParameterYSize", SVGigeParameterYSize)
(L"SVGigeParameterColorFormat", SVGigeParameterColorFormat)
(L"SVGigeParameterBayerPattern", SVGigeParameterBayerPattern)
(L"SVGigeParameterFeatureBrightness", SVGigeParameterFeatureBrightness)
(L"SVGigeParameterFeatureAutoExposure", SVGigeParameterFeatureAutoExposure)
(L"SVGigeParameterFeatureSharpness", SVGigeParameterFeatureSharpness)
(L"SVGigeParameterFeatureWhiteBalanceU", SVGigeParameterFeatureWhiteBalanceU)
(L"SVGigeParameterFeatureWhiteBalanceV", SVGigeParameterFeatureWhiteBalanceV)
(L"SVGigeParameterFeatureHue", SVGigeParameterFeatureHue)
(L"SVGigeParameterFeatureSaturation", SVGigeParameterFeatureSaturation)
(L"SVGigeParameterFeatureGamma", SVGigeParameterFeatureGamma)
(L"SVGigeParameterFeatureShutter", SVGigeParameterFeatureShutter)
(L"SVGigeParameterFeatureGain", SVGigeParameterFeatureGain)
(L"SVGigeParameterTriggerSource", SVGigeParameterTriggerSource)
(L"SVGigeParameterTriggerEdge", SVGigeParameterTriggerEdge)
(L"SVGigeParameterTriggerInvert", SVGigeParameterTriggerInvert)
(L"SVGigeParameterTriggerLine", SVGigeParameterTriggerLine)
(L"SVGigeParameterTriggerDelay", SVGigeParameterTriggerDelay)
(L"SVGigeParameterTriggerEnable", SVGigeParameterTriggerEnable)
(L"SVGigeParameterStrobeSource", SVGigeParameterStrobeSource)
(L"SVGigeParameterStrobeEdge", SVGigeParameterStrobeEdge)
(L"SVGigeParameterStrobeInvert", SVGigeParameterStrobeInvert)
(L"SVGigeParameterStrobeDelay", SVGigeParameterStrobeDelay)
(L"SVGigeParameterStrobeDuration", SVGigeParameterStrobeDuration)
(L"SVGigeParameterStrobeLine", SVGigeParameterStrobeLine)
(L"SVGigeParameterStrobeEnable", SVGigeParameterStrobeEnable)
(L"SVGigeParameterLutEnable", SVGigeParameterLutEnable)
(L"SVGigeParameterLutArray", SVGigeParameterLutArray)
(L"SVGigeParameterHorizontalBinning", SVGigeParameterHorizontalBinning)
(L"SVGigeParameterVerticalBinning", SVGigeParameterVerticalBinning)
(L"SVGigeParameterLineInput", SVGigeParameterLineInput)
(L"SVGigeParameterInputEvent", SVGigeParameterInputEvent)
(L"SVGigeParameterInputEventName", SVGigeParameterInputEventName)
(L"SVGigeParameterCustom1", SVGigeParameterCustom1)
(L"SVGigeParameterCustom2", SVGigeParameterCustom2)
(L"SVGigeParameterCustom3", SVGigeParameterCustom3)
(L"SVGigeParameterCustom4", SVGigeParameterCustom4)
(L"SVGigeParameterCustom5", SVGigeParameterCustom5)
(L"SVGigeParameterCustom6", SVGigeParameterCustom6)
(L"SVGigeParameterCustom7", SVGigeParameterCustom7)
(L"SVGigeParameterCustom8", SVGigeParameterCustom8)
(L"SVGigeParameterCustom9", SVGigeParameterCustom9)
(L"SVGigeParameterCustom10", SVGigeParameterCustom10)
(L"SVGigeParameterCustom11", SVGigeParameterCustom11)
(L"SVGigeParameterCustom12", SVGigeParameterCustom12)
(L"SVGigeParameterCustom13", SVGigeParameterCustom13)
(L"SVGigeParameterCustom14", SVGigeParameterCustom14)
(L"SVGigeParameterCustom15", SVGigeParameterCustom15)
(L"SVGigeParameterCustom16", SVGigeParameterCustom16)
(L"SVGigeParameterCustom17", SVGigeParameterCustom17)
(L"SVGigeParameterCustom18", SVGigeParameterCustom18)
(L"SVGigeParameterCustom19", SVGigeParameterCustom19)
(L"SVGigeParameterCustom20", SVGigeParameterCustom20)
(L"SVGigeParameterCustom21", SVGigeParameterCustom21)
(L"SVGigeParameterCustom22", SVGigeParameterCustom22)
(L"SVGigeParameterCustom23", SVGigeParameterCustom23)
(L"SVGigeParameterCustom24", SVGigeParameterCustom24)
(L"SVGigeParameterCustom25", SVGigeParameterCustom25)
(L"SVGigeParameterCustom26", SVGigeParameterCustom26)
(L"SVGigeParameterCustom27", SVGigeParameterCustom27)
(L"SVGigeParameterCustom28", SVGigeParameterCustom28)
(L"SVGigeParameterCustom29", SVGigeParameterCustom29)
(L"SVGigeParameterCustom30", SVGigeParameterCustom30)
;


// VariantTypeEnum Map
typedef std::map<WString, VARTYPE> VariantTypeEnumMap;
static VariantTypeEnumMap VariantTypeEnum = boost::assign::map_list_of<>
(L"VT_I1",		VT_I1)
(L"VT_I2",		VT_I2)
(L"VT_I4",		VT_I4)
(L"VT_UI1",		VT_UI1)
(L"VT_UI2",		VT_UI2)
(L"VT_UI4",		VT_UI4)
(L"VT_R4",		VT_R4)
(L"VT_R8",		VT_R8)
(L"VT_BSTR",	VT_BSTR)
;

// FeatureTypeStringEnum Map
typedef std::map<WString, SVMatroxDigitizerFeature::SVFeatureTypeEnum> FeatureTypeStringEnumMap;
static FeatureTypeStringEnumMap FeatureTypeStringEnums = boost::assign::map_list_of<>
(L"SVTypeCommand",				SVMatroxDigitizerFeature::SVTypeCommand)
(L"SVTypeInt32",				SVMatroxDigitizerFeature::SVTypeInt32)
(L"SVTypeDouble",				SVMatroxDigitizerFeature::SVTypeDouble)
(L"SVTypeString",				SVMatroxDigitizerFeature::SVTypeString)
(L"SVTypeIntegerEnumeration",	SVMatroxDigitizerFeature::SVTypeIntegerEnumeration)
(L"SVTypeStringEnumeration",	SVMatroxDigitizerFeature::SVTypeStringEnumeration)
(L"SVTypeBool",					SVMatroxDigitizerFeature::SVTypeBool)
;

// FeatureSupportedStringEnum Map
typedef std::map<WString, SVGigeFeature::FeatureSupportedEnum> FeatureSupportedStringEnumMap;
static FeatureSupportedStringEnumMap FeatureSupportedStringEnums = boost::assign::map_list_of<>
(L"NotSupported",	SVGigeFeature::NotSupported)
(L"Supported",		SVGigeFeature::Supported)
;

// FeatureAccessStringEnum Map
typedef std::map<WString, SVGigeFeature::FeatureAccessEnum> FeatureAccessStringEnumMap;
static FeatureAccessStringEnumMap FeatureAccessStringEnums = boost::assign::map_list_of<>
(L"ReadOnly",	SVGigeFeature::ReadOnly)
(L"ReadWrite",	SVGigeFeature::ReadWrite)
;

// String Conversion
static SVString WideToAnsi(wchar_t* wideStr)
{
	USES_CONVERSION;
	return SVString(W2CA(wideStr));
}

SVGigeCameraDBContent::SVGigeCameraDBContent()
: m_errorCode(S_OK), m_GigeParameterEnum(SVGigeParameterFeatureOverrides)
{
}

SVGigeCameraDBContent::~SVGigeCameraDBContent()
{
}

HRESULT SVGigeCameraDBContent::startElement(unsigned short* pwchNamespaceUri, int cchNamespaceUri, unsigned short* pwchLocalName, int cchLocalName, unsigned short* pwchRawName, int cchRawName, MSXML2::ISAXAttributes* pAttributes)
{
	HRESULT hr = S_OK;

	WString elementName(reinterpret_cast<wchar_t *>(pwchLocalName), cchLocalName);
	
	ElementMap::const_iterator it = Elements.find(elementName);
	if (it != Elements.end())
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
       
HRESULT SVGigeCameraDBContent::endElement(unsigned short* pwchNamespaceUri, int cchNamespaceUri, unsigned short* pwchLocalName, int cchLocalName, unsigned short* pwchRawName, int cchRawName)
{
	WString elementName(reinterpret_cast<wchar_t *>(pwchLocalName), cchLocalName);
	
	ElementMap::const_iterator it = Elements.find(elementName);
	if (it != Elements.end())
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
					m_GigeParameterEnum = SVGigeParameterFeatureOverrides;
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
	m_errorText = WideToAnsi(pErrorMsg);
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
			SVGigeParameterEnum enumValue;
			bool bRetVal = StringToEnum<SVGigeParameterEnum, GigeParameterEnumMap, LPCWSTR>::GetEnum(GigeParameterEnums, value, enumValue); 
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
			m_GigeDeviceParameterInfo.m_GigeDeviceParameterName = WideToAnsi(value);
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
			m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_accessor.m_getterName = WideToAnsi(value);
		}
		else if (wcsncmp(name, L"Setter", nameSize) == 0)
		{
			m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_accessor.m_setterName = WideToAnsi(value);
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
			m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeFeatureName = WideToAnsi(value);
		}
		else if (wcsncmp(name, L"FeatureType", nameSize) == 0)
		{
			SVMatroxDigitizerFeature::SVFeatureTypeEnum enumValue;
			bool bRetVal = StringToEnum<SVMatroxDigitizerFeature::SVFeatureTypeEnum, FeatureTypeStringEnumMap, LPCWSTR>::GetEnum(FeatureTypeStringEnums, value, enumValue);
			if (bRetVal)
			{
				m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeFeatureType = enumValue;
			}
		}
		else if (wcsncmp(name, L"FeatureSupportedEnum", nameSize) == 0)
		{
			SVGigeFeature::FeatureSupportedEnum enumValue;
			bool bRetVal = StringToEnum<SVGigeFeature::FeatureSupportedEnum, FeatureSupportedStringEnumMap, LPCWSTR>::GetEnum(FeatureSupportedStringEnums, value, enumValue);
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
			gigeFeatureSelectorInfo.m_GigeFeatureSelectorName = WideToAnsi(value);
		}
		else if (wcsncmp(name, L"SelectorValue", nameSize) == 0)
		{
			gigeFeatureSelectorInfo.m_GigeFeatureSelectorValue = WideToAnsi(value);
		}
	}
	// Set Feature Selector
	m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeSelector = gigeFeatureSelectorInfo;
}

// Get XML Attributes for GetGigeFeatureStringEnumList
void SVGigeCameraDBContent::GetGigeFeatureStringEnumAttributes(MSXML2::ISAXAttributes* pAttributes)
{
	SVString deviceParamString;
	SVString gigeFeatureString;

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
			deviceParamString = WideToAnsi(value);
		}
		else if (wcsncmp(name, L"SVGigeFeatureString", nameSize) == 0)
		{
			gigeFeatureString = WideToAnsi(value);
		}
	}
	if (!deviceParamString.empty() && !gigeFeatureString.empty())
	{
		m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeFeatureStringEnumList.insert(std::make_pair(deviceParamString, gigeFeatureString));
	}
}

// Construct
SVGigeParameterAccessor SVGigeCameraDBContent::ConstructGigeParameterAccessor()
{
	// Construct SVGigeParameterAccessor
	SVSharedPtr<SVGigeParameterAccessor> gigeParameterAccessorHolder;

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
				gigeParameterAccessorHolder = new SVGigeParameterAccessor(feature, GetGetterFunc(), GetSetterFunc());
			}
			else
			{
				gigeParameterAccessorHolder = new SVGigeParameterAccessor(feature);
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
				gigeParameterAccessorHolder = new SVGigeParameterAccessor(feature, GetGetterFunc(), GetSetterFunc());
			}
			else
			{
				gigeParameterAccessorHolder = new SVGigeParameterAccessor(feature);
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
				gigeParameterAccessorHolder = new SVGigeParameterAccessor(feature, GetGetterFunc(), GetSetterFunc());
			}
			else
			{
				gigeParameterAccessorHolder = new SVGigeParameterAccessor(feature);
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
				gigeParameterAccessorHolder = new SVGigeParameterAccessor(feature, GetGetterFunc(), GetSetterFunc());
			}
			else
			{
				gigeParameterAccessorHolder = new SVGigeParameterAccessor(feature);
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
		VariantTypeEnumMap::const_iterator it = VariantTypeEnum.find(vartypeString);
		if (it != VariantTypeEnum.end())
		{
			varType = it->second | VT_ARRAY;
		}
	}
	else
	{
		VariantTypeEnumMap::const_iterator it = VariantTypeEnum.find(vartypeString);
		if (it != VariantTypeEnum.end())
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

