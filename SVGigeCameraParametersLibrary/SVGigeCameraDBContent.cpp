//******************************************************************************
//* COPYRIGHT (c) 2010 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#pragma warning( push )
#pragma warning( disable : 4471 )
#import <msxml3.dll> raw_interfaces_only 
#pragma warning( pop )
#include "SVGigeDeviceParameterStruct.h"
#include "SVGigeEmptyGetter.h"
#include "SVGigeEmptySetter.h"
#include "SVUtilityLibrary/StringToEnum.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVGigeCameraDBContent.h"
#pragma endregion Includes

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
constexpr std::array<std::pair<ElementTagEnums, LPCTSTR>, 9> cElements =
{{
	{ElementTagEnums::GigeParameterMap, _T("SVGigeParameterMap")},					// Gige Parameter Map (root tag)
	{ElementTagEnums::GigeParameter, _T("SVGigeParameter")},						// Gige Parameter 
	{ElementTagEnums::GigeDeviceParameterStruct, _T("SVGigeDeviceParameterStruct")},// Gige Device Parameter
	{ElementTagEnums::GigeParameterAccessor, _T("SVGigeParameterAccessor")},		// Gige Parameter Accessor
	{ElementTagEnums::GigeAccessor, _T("SVGigeAccessor")},							// Gige Accessor optional
	{ElementTagEnums::GigeFeature, _T("SVGigeFeature")},							// Gige Feature
	{ElementTagEnums::GigeFeatureSelector, _T("SVGigeFeatureSelector")},			// Gige Feature Selector (optional)
	{ElementTagEnums::GigeFeatureStringEnumList, _T("SVGigeFeatureStringEnumList")},// Gige Feature StringEnum List (only for string list)
	{ElementTagEnums::GigeFeatureStringEnum, _T("SVGigeFeatureStringEnum")}			// Gige Feature StringEnum (list entry)
}};

// GigeParameterEnum Map
constexpr std::array<std::pair<SvDef::SVGigeParameterEnum, LPCTSTR>, 68> cGigeParameterEnums =
{{
	{SvDef::SVGigeParameterExposureMode, "SVGigeParameterExposureMode"},
	{SvDef::SVGigeParameterXOffset, "SVGigeParameterXOffset"},
	{SvDef::SVGigeParameterYOffset, "SVGigeParameterYOffset"},
	{SvDef::SVGigeParameterXSize, "SVGigeParameterXSize"},
	{SvDef::SVGigeParameterYSize, "SVGigeParameterYSize"},
	{SvDef::SVGigeParameterColorFormat, "SVGigeParameterColorFormat"},
	{SvDef::SVGigeParameterBayerPattern, "SVGigeParameterBayerPattern"},
	{SvDef::SVGigeParameterFeatureBrightness, "SVGigeParameterFeatureBrightness"},
	{SvDef::SVGigeParameterFeatureAutoExposure, "SVGigeParameterFeatureAutoExposure",		},
	{SvDef::SVGigeParameterFeatureSharpness, "SVGigeParameterFeatureSharpness"},
	{SvDef::SVGigeParameterFeatureWhiteBalanceU, "SVGigeParameterFeatureWhiteBalanceU"},
	{SvDef::SVGigeParameterFeatureWhiteBalanceV, "SVGigeParameterFeatureWhiteBalanceV"},
	{SvDef::SVGigeParameterFeatureHue, "SVGigeParameterFeatureHue"},
	{SvDef::SVGigeParameterFeatureSaturation, "SVGigeParameterFeatureSaturation"},
	{SvDef::SVGigeParameterFeatureGamma, "SVGigeParameterFeatureGamma"},
	{SvDef::SVGigeParameterFeatureShutter, "SVGigeParameterFeatureShutter"},
	{SvDef::SVGigeParameterFeatureGain, "SVGigeParameterFeatureGain"},
	{SvDef::SVGigeParameterTriggerSource, "SVGigeParameterTriggerSource"},
	{SvDef::SVGigeParameterTriggerEdge, "SVGigeParameterTriggerEdge"},
	{SvDef::SVGigeParameterTriggerInvert, "SVGigeParameterTriggerInvert"},
	{SvDef::SVGigeParameterTriggerLine, "SVGigeParameterTriggerLine"},
	{SvDef::SVGigeParameterTriggerDelay, "SVGigeParameterTriggerDelay"},
	{SvDef::SVGigeParameterTriggerEnable, "SVGigeParameterTriggerEnable"},
	{SvDef::SVGigeParameterStrobeSource, "SVGigeParameterStrobeSource"},
	{SvDef::SVGigeParameterStrobeEdge, "SVGigeParameterStrobeEdge"},
	{SvDef::SVGigeParameterStrobeInvert, "SVGigeParameterStrobeInvert"},
	{SvDef::SVGigeParameterStrobeDelay, "SVGigeParameterStrobeDelay"},
	{SvDef::SVGigeParameterStrobeDuration, "SVGigeParameterStrobeDuration"},
	{SvDef::SVGigeParameterStrobeLine, "SVGigeParameterStrobeLine"},
	{SvDef::SVGigeParameterStrobeEnable, "SVGigeParameterStrobeEnable"},
	{SvDef::SVGigeParameterLutEnable, "SVGigeParameterLutEnable"},
	{SvDef::SVGigeParameterLutArray, "SVGigeParameterLutArray"},
	{SvDef::SVGigeParameterLineInput, "SVGigeParameterLineInput"},
	{SvDef::SVGigeParameterInputEvent, "SVGigeParameterInputEvent"},
	{SvDef::SVGigeParameterInputEventName, "SVGigeParameterInputEventName"},
	{SvDef::SVGigeParameterHorizontalBinning, "SVGigeParameterHorizontalBinning"},
	{SvDef::SVGigeParameterVerticalBinning, "SVGigeParameterVerticalBinning"},
	{SvDef::SVGigeParameterCameraDefaultSettings, "SVGigeParameterCameraDefaultSettings"},
	{SvDef::SVGigeParameterCustom1, "SVGigeParameterCustom1"},
	{SvDef::SVGigeParameterCustom2, "SVGigeParameterCustom2"},
	{SvDef::SVGigeParameterCustom3, "SVGigeParameterCustom3"},
	{SvDef::SVGigeParameterCustom4, "SVGigeParameterCustom4"},
	{SvDef::SVGigeParameterCustom5, "SVGigeParameterCustom5"},
	{SvDef::SVGigeParameterCustom6, "SVGigeParameterCustom6"},
	{SvDef::SVGigeParameterCustom7, "SVGigeParameterCustom7"},
	{SvDef::SVGigeParameterCustom8, "SVGigeParameterCustom8"},
	{SvDef::SVGigeParameterCustom9, "SVGigeParameterCustom9"},
	{SvDef::SVGigeParameterCustom10, "SVGigeParameterCustom10"},
	{SvDef::SVGigeParameterCustom11, "SVGigeParameterCustom11"},
	{SvDef::SVGigeParameterCustom12, "SVGigeParameterCustom12"},
	{SvDef::SVGigeParameterCustom13, "SVGigeParameterCustom13"},
	{SvDef::SVGigeParameterCustom14, "SVGigeParameterCustom14"},
	{SvDef::SVGigeParameterCustom15, "SVGigeParameterCustom15"},
	{SvDef::SVGigeParameterCustom16, "SVGigeParameterCustom16"},
	{SvDef::SVGigeParameterCustom17, "SVGigeParameterCustom17"},
	{SvDef::SVGigeParameterCustom18, "SVGigeParameterCustom18"},
	{SvDef::SVGigeParameterCustom19, "SVGigeParameterCustom19"},
	{SvDef::SVGigeParameterCustom20, "SVGigeParameterCustom20"},
	{SvDef::SVGigeParameterCustom21, "SVGigeParameterCustom21"},
	{SvDef::SVGigeParameterCustom22, "SVGigeParameterCustom22"},
	{SvDef::SVGigeParameterCustom23, "SVGigeParameterCustom23"},
	{SvDef::SVGigeParameterCustom24, "SVGigeParameterCustom24"},
	{SvDef::SVGigeParameterCustom25, "SVGigeParameterCustom25"},
	{SvDef::SVGigeParameterCustom26, "SVGigeParameterCustom26"},
	{SvDef::SVGigeParameterCustom27, "SVGigeParameterCustom27"},
	{SvDef::SVGigeParameterCustom28, "SVGigeParameterCustom28"},
	{SvDef::SVGigeParameterCustom29, "SVGigeParameterCustom29"},
	{SvDef::SVGigeParameterCustom30, "SVGigeParameterCustom30"}
}};


// VariantTypeEnum Map
constexpr std::array<std::pair<VARENUM, LPCTSTR>, 9> cVariantTypeEnum =
{{
	{VARENUM::VT_I1, "VT_I1"},
	{VARENUM::VT_I2, "VT_I2"},
	{VARENUM::VT_I4, "VT_I4"},
	{VARENUM::VT_UI1, "VT_UI1"},
	{VARENUM::VT_UI2, "VT_UI2"},
	{VARENUM::VT_UI4, "VT_UI4"},
	{VARENUM::VT_R4, "VT_R4"},
	{VARENUM::VT_R8, "VT_R8"},
	{VARENUM::VT_BSTR, "VT_BSTR"}
}};	 
	 
constexpr std::array<std::pair<SVGigeFeature::FeatureSupportedEnum, LPCTSTR>, 2> cFeatureSupportedStringEnums
{{
	{SVGigeFeature::FeatureSupportedEnum::NotSupported, _T("NotSupported")},
	{SVGigeFeature::FeatureSupportedEnum::Supported, _T("Supported")}
}};

constexpr std::array<std::pair<SVGigeFeature::FeatureAccessEnum, LPCTSTR>, 2> cFeatureAccessStringEnums
{{
	{SVGigeFeature::ReadOnly, _T("ReadOnly")},
	{SVGigeFeature::ReadWrite, _T("ReadWrite")}
}};

SVGigeCameraDBContent::SVGigeCameraDBContent()
: m_errorCode(S_OK), m_GigeParameterEnum(SvDef::SVGigeParameterFeatureOverrides)
{
}

SVGigeCameraDBContent::~SVGigeCameraDBContent()
{
}

HRESULT SVGigeCameraDBContent::startElement(unsigned short* , int , unsigned short* pwchLocalName, int , unsigned short* , int , MSXML2::ISAXAttributes* pAttributes)
{
	HRESULT hr = S_OK;

	std::string elementName {_bstr_t(reinterpret_cast<wchar_t*>(pwchLocalName))};
	
	ElementTagEnums enumValue {ElementTagEnums::GigeParameterMap};
	bool bRetVal = StringToEnum::GetEnum(cElements, elementName.c_str(), enumValue);
	if (bRetVal)
	{
		switch (enumValue)
		{
			case ElementTagEnums::GigeParameter:
				GetGigeParameterAttributes(pAttributes);
				break;

			case ElementTagEnums::GigeDeviceParameterStruct:
				GetGigeDeviceParameterStructAttributes(pAttributes);
				break;

			case ElementTagEnums::GigeAccessor:
				GetGigeAccessorAttributes(pAttributes);
				break;

			case ElementTagEnums::GigeFeature:
				GetGigeFeatureAttributes(pAttributes);
				break;

			case ElementTagEnums::GigeFeatureSelector:
				GetGigeFeatureSelectorAttributes(pAttributes);
				break;

			case ElementTagEnums::GigeFeatureStringEnum:
				GetGigeFeatureStringEnumAttributes(pAttributes);
				break;

			default:
				break;
		}
	}
    return hr;
}
 
HRESULT SVGigeCameraDBContent::endElement(unsigned short* , int , unsigned short* pwchLocalName, int , unsigned short* , int )
{
	std::string elementName {_bstr_t(reinterpret_cast<wchar_t*>(pwchLocalName))};

	ElementTagEnums enumValue {ElementTagEnums::GigeParameterMap};
	bool bRetVal = StringToEnum::GetEnum(cElements, elementName.c_str(), enumValue);
	if (bRetVal)
	{
		switch (enumValue)
		{
			case ElementTagEnums::GigeDeviceParameterStruct:
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

			default:
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
			std::string textValue {_bstr_t(value)};
			SvDef::SVGigeParameterEnum enumValue{ SvDef::SVGigeParameterEnum::SVGigeParameterStart };
			bool bRetVal = StringToEnum::GetEnum(cGigeParameterEnums, textValue.c_str(), enumValue);
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
			std::string textValue {_bstr_t(value)};
			SVFeatureTypeEnum enumValue{ SVFeatureTypeEnum::SVTypeCommand};
			bool bRetVal = StringToEnum::GetEnum(cFeatureTypeStringEnums, textValue.c_str(), enumValue);
			if (bRetVal)
			{
				m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_GigeFeatureType = enumValue;
			}
		}
		else if (wcsncmp(name, L"FeatureSupportedEnum", nameSize) == 0)
		{
			std::string textValue {_bstr_t(value)};
			SVGigeFeature::FeatureSupportedEnum enumValue;
			bool bRetVal = StringToEnum::GetEnum(cFeatureSupportedStringEnums, textValue.c_str(), enumValue);
			if (bRetVal)
			{
				m_GigeDeviceParameterInfo.m_GigeFeatureAccessor.m_feature.m_FeatureSupported = enumValue;
			}
		}
		else if (wcsncmp(name, L"FeatureAccessEnum", nameSize) == 0)
		{
			std::string textValue {_bstr_t(value)};
			SVGigeFeature::FeatureAccessEnum enumValue;
			bool bRetVal = StringToEnum::GetEnum(cFeatureAccessStringEnums, textValue.c_str(), enumValue);
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
				gigeParameterAccessorHolder = std::make_shared<SVGigeParameterAccessor>(feature, GetGetterFunc(), GetSetterFunc());
			}
			else
			{
				gigeParameterAccessorHolder = std::make_shared<SVGigeParameterAccessor>(feature);
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
				gigeParameterAccessorHolder = std::make_shared<SVGigeParameterAccessor>(feature, GetGetterFunc(), GetSetterFunc());
			}
			else
			{
				gigeParameterAccessorHolder = std::make_shared<SVGigeParameterAccessor>(feature);
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
				gigeParameterAccessorHolder = std::make_shared<SVGigeParameterAccessor>(feature, GetGetterFunc(), GetSetterFunc());
			}
			else
			{
				gigeParameterAccessorHolder = std::make_shared<SVGigeParameterAccessor>(feature);
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
				gigeParameterAccessorHolder = std::make_shared<SVGigeParameterAccessor>(feature, GetGetterFunc(), GetSetterFunc());
			}
			else
			{
				gigeParameterAccessorHolder = std::make_shared<SVGigeParameterAccessor>(feature);
			}
		}
	}
	return SVGigeParameterAccessor(*(gigeParameterAccessorHolder.get()));
}

// Convert String to Variant VARTYPE
VARTYPE SVGigeCameraDBContent::GetVarType(wchar_t* vartypeString) const
{
	VARTYPE varType = static_cast<VARTYPE> (VT_EMPTY);

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
		std::string varName {_bstr_t(vartypeString)};
		VARENUM enumValue {VT_EMPTY};
		bool bRetVal = StringToEnum::GetEnum(cVariantTypeEnum, varName.c_str(), enumValue);
		if (bRetVal)
		{
			varType = static_cast<VARTYPE> (enumValue | VT_ARRAY);
		}
	}
	else
	{
		std::string varName {_bstr_t(vartypeString)};
		VARENUM enumValue {VT_EMPTY};
		bool bRetVal = StringToEnum::GetEnum(cVariantTypeEnum, varName.c_str(), enumValue);
		if (bRetVal)
		{
			varType = static_cast<VARTYPE> (enumValue);
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

