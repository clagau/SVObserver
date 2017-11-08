//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeFeature
//* .File Name       : $Workfile:   SVGigeFeature.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:54:48  $
//******************************************************************************
#pragma once

#pragma region Includes

#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerFeatureEnums.h"
#include "SVGigeFeatureSelector.h"
#include "SVGigeFeatureStringEnumList.h"
#pragma endregion Includes

class SVGigeFeature
{
public:
	enum FeatureSupportedEnum
	{
		NotSupported,
		Supported
	};

	enum FeatureAccessEnum
	{
		ReadOnly,
		ReadWrite
	};

	SVGigeFeature(const std::string& p_name, SVMatroxDigitizerFeature::SVFeatureTypeEnum p_featureType);
	SVGigeFeature(const std::string& p_name, SVMatroxDigitizerFeature::SVFeatureTypeEnum p_featureType, FeatureSupportedEnum p_supported, FeatureAccessEnum p_access);
	SVGigeFeature(const std::string& p_name, SVMatroxDigitizerFeature::SVFeatureTypeEnum p_featureType, FeatureSupportedEnum p_supported, FeatureAccessEnum p_access, const SVGigeFeatureSelector& p_selector);
	SVGigeFeature(const std::string& p_name, SVMatroxDigitizerFeature::SVFeatureTypeEnum p_featureType, FeatureSupportedEnum p_supported, FeatureAccessEnum p_access, const SVGigeFeatureSelector& p_selector, const SVGigeFeatureStringEnumList& p_stringEnums);
	SVGigeFeature(const SVGigeFeature& rFeature);
	
	SVGigeFeature& operator=(const SVGigeFeature& rFeature);
	
	const std::string& GetName() const;
	SVMatroxDigitizerFeature::SVFeatureTypeEnum GetType() const;
	bool IsSupported() const;
	bool IsReadOnly() const;

	bool HasSelector() const;
	const SVGigeFeatureSelector& GetSelector() const;
	bool HasTranslation() const;
	HRESULT GetGigeFeatureString(const std::string& rDeviceParam, std::string& rGigeFeature) const;
	HRESULT GetDeviceParamString(const std::string& rGigeFeature, std::string& rDeviceParam) const;

private:
	std::string m_name;
	SVMatroxDigitizerFeature::SVFeatureTypeEnum m_featureType;
	FeatureSupportedEnum m_supported;
	FeatureAccessEnum m_access;
	SVGigeFeatureSelector m_selector;
	SVGigeFeatureStringEnumList m_stringEnums;
};

