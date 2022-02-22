//******************************************************************************
//* COPYRIGHT (c) 2010 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraDBContent
//* .File Name       : $Workfile:   SVGigeCameraDBContent.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:46:16  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVSystemLibrary/SVSAXContentHandlerImpl.h"

#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerFeatureEnums.h"
#include "Definitions/SVGigeEnums.h"
#include "SVGigeDeviceParameterStruct.h"
#include "SVGigeFeature.h"
#include "SVGigeParameterAccessor.h"
#pragma endregion Includes

// class to handle SAX parsing for GigeDeviceParameter database
class SVGigeCameraDBContent : public SVSAXContentHandlerImpl  
{
private:
	// GigeFeatureSelector Info
	struct SVGigeFeatureSelectorInfo
	{
		std::string m_GigeFeatureSelectorName;
		std::string m_GigeFeatureSelectorValue;
		
		void clear() 
		{ 
			m_GigeFeatureSelectorName.clear(); 
			m_GigeFeatureSelectorValue.clear(); 
		}
	};

	// GigeFeature Info
	struct SVGigeFeatureInfo
	{
		std::string m_GigeFeatureName;
		SVMatroxDigitizerFeature::SVFeatureTypeEnum m_GigeFeatureType;
		SVGigeFeature::FeatureSupportedEnum m_FeatureSupported;
		SVGigeFeature::FeatureAccessEnum m_FeatureAccess;

		SVGigeFeatureSelectorInfo m_GigeSelector;

		std::vector<std::pair<std::string, std::string>> m_GigeFeatureStringEnumList;

		void clear() 
		{ 
			m_GigeFeatureName.clear(); 
			//m_GigeFeatureType = 
			m_FeatureSupported = SVGigeFeature::NotSupported; 
			m_FeatureAccess = SVGigeFeature::ReadOnly;

			m_GigeSelector.clear();

			m_GigeFeatureStringEnumList.clear();
		}
	};

	// GigeAccessor Info
	struct SVGigeAccessorInfo
	{
		std::string m_getterName;
		std::string m_setterName;

		void clear()
		{
			m_getterName.clear();
			m_setterName.clear();
		}
	};

	// GigeParameterAccessor Info
	struct SVGigeParameterAccessorInfo
	{
		SVGigeAccessorInfo m_accessor;
		SVGigeFeatureInfo m_feature;

		void clear()
		{
			m_accessor.clear();
			m_feature.clear();
		}
	};
	
	// GigeDeviceParameterStruct Info
	struct SVGigeDeviceParameterStructInfo
	{
		std::string m_GigeDeviceParameterName;
		VARTYPE m_GigeDeviceParameterDataType;

		SVGigeParameterAccessorInfo m_GigeFeatureAccessor;

		void clear()
		{
			m_GigeDeviceParameterName.clear();
			m_GigeDeviceParameterDataType = VT_EMPTY;
			m_GigeFeatureAccessor.clear();
		}
	};

	SVGigeDeviceParameterStructInfo m_GigeDeviceParameterInfo {};
	SvDef::SVGigeParameterEnum m_GigeParameterEnum; // SVGigeDeviceParameterMap Key
	SVGigeDeviceParameterMap m_GigeDeviceParameterMap;
	HRESULT m_errorCode;
	std::string m_errorText;
	
public:
    SVGigeCameraDBContent();
    virtual ~SVGigeCameraDBContent();
        
	virtual HRESULT STDMETHODCALLTYPE startElement(unsigned short* pwchNamespaceUri, int cchNamespaceUri, unsigned short* pwchLocalName, int cchLocalName, unsigned short* pwchRawName, int cchRawName, MSXML2::ISAXAttributes* pAttributes) override;
	virtual HRESULT STDMETHODCALLTYPE endElement(unsigned short* pwchNamespaceUri, int cchNamespaceUri, unsigned short* pwchLocalName, int cchLocalName, unsigned short* pwchRawName, int cchRawName) override;
	
	void ReportError(wchar_t* pErrorMsg, HRESULT erorCode);

	const SVGigeDeviceParameterMap& GetGigeDeviceParameterMap() const;

private:
	void GetGigeParameterAttributes(MSXML2::ISAXAttributes* pAttributes);
	void GetGigeDeviceParameterStructAttributes(MSXML2::ISAXAttributes* pAttributes);
	void GetGigeAccessorAttributes(MSXML2::ISAXAttributes* pAttributes);
	void GetGigeFeatureAttributes(MSXML2::ISAXAttributes* pAttributes);
	void GetGigeFeatureSelectorAttributes(MSXML2::ISAXAttributes* pAttributes);
	void GetGigeFeatureStringEnumAttributes(MSXML2::ISAXAttributes* pAttributes);

	SVGigeParameterAccessor ConstructGigeParameterAccessor();

	VARTYPE GetVarType(wchar_t* vartypeString) const;
	SVGigeParameterAccessor::ParamGetter GetGetterFunc();
	SVGigeParameterAccessor::ParamSetter GetSetterFunc();
};

