//******************************************************************************
//* COPYRIGHT (c) 2009 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeFeature
//* .File Name       : $Workfile:   SVGigeFeature.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:54:46  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVGigeFeature.h"
#pragma endregion Includes

SVGigeFeature::SVGigeFeature(const std::string& p_name, SVFeatureTypeEnum p_featureType)
: m_name(p_name), m_featureType(p_featureType), m_supported(SVGigeFeature::Supported), m_access(SVGigeFeature::ReadWrite)
{
}

SVGigeFeature::SVGigeFeature(const std::string& p_name, SVFeatureTypeEnum p_featureType, SVGigeFeature::FeatureSupportedEnum p_supported, SVGigeFeature::FeatureAccessEnum p_access)
: m_name(p_name), m_featureType(p_featureType), m_supported(p_supported), m_access(p_access)
{
}

SVGigeFeature::SVGigeFeature(const std::string& p_name, SVFeatureTypeEnum p_featureType, SVGigeFeature::FeatureSupportedEnum p_supported, SVGigeFeature::FeatureAccessEnum p_access, const SVGigeFeatureSelector& p_selector)
: m_name(p_name), m_featureType(p_featureType), m_supported(p_supported), m_access(p_access), m_selector(p_selector)
{
}

SVGigeFeature::SVGigeFeature(const std::string& p_name, SVFeatureTypeEnum p_featureType, SVGigeFeature::FeatureSupportedEnum p_supported, SVGigeFeature::FeatureAccessEnum p_access, const SVGigeFeatureSelector& p_selector, const SVGigeFeatureStringEnumList& p_stringEnums)
: m_name(p_name), m_featureType(p_featureType), m_supported(p_supported), m_access(p_access), m_selector(p_selector), m_stringEnums(p_stringEnums)
{
}

SVGigeFeature::SVGigeFeature(const SVGigeFeature& rFeature)
: m_name(rFeature.m_name), m_featureType(rFeature.m_featureType), m_supported(rFeature.m_supported), m_access(rFeature.m_access), m_selector(rFeature.m_selector), m_stringEnums(rFeature.m_stringEnums)
{
}

SVGigeFeature& SVGigeFeature::operator=(const SVGigeFeature& rFeature)
{
	if (this != &rFeature)
	{
		m_name = rFeature.m_name;
		m_featureType = rFeature.m_featureType;
		m_supported = rFeature.m_supported;
		m_access = rFeature.m_access;
		m_selector = rFeature.m_selector;
		m_stringEnums = rFeature.m_stringEnums;
	}
	return *this;
}

const std::string& SVGigeFeature::GetName() const
{
	return m_name;
}

SVFeatureTypeEnum SVGigeFeature::GetType() const
{
	return m_featureType;
}

bool SVGigeFeature::IsSupported() const
{
	return m_supported == SVGigeFeature::Supported;
}

bool SVGigeFeature::IsReadOnly() const
{
	return m_access == SVGigeFeature::ReadOnly;
}

bool SVGigeFeature::HasSelector() const
{
	return !m_selector.GetName().empty();
}

const SVGigeFeatureSelector& SVGigeFeature::GetSelector() const
{
	return m_selector;
}

bool SVGigeFeature::HasTranslation() const
{
	return m_stringEnums.HasTranslation();
}

HRESULT SVGigeFeature::GetGigeFeatureString(const std::string& rDeviceParam, std::string& rGigeFeature) const
{
	HRESULT hr = m_stringEnums.GetGigeFeatureString(rDeviceParam, rGigeFeature);
	
	return hr;
}

HRESULT SVGigeFeature::GetDeviceParamString(const std::string& rGigeFeature, std::string& rDeviceParam) const
{
	HRESULT hr =  m_stringEnums.GetDeviceParamString(rGigeFeature, rDeviceParam);

	return hr;
}

