//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeFeature
//* .File Name       : $Workfile:   SVGigeFeature.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:54:46  $
//******************************************************************************
#include "stdafx.h"
#include "SVGigeFeature.h"

SVGigeFeature::SVGigeFeature(const SVString& p_name, SVMatroxDigitizerFeature::SVFeatureTypeEnum p_featureType)
: m_name(p_name), m_featureType(p_featureType), m_supported(SVGigeFeature::Supported), m_access(SVGigeFeature::ReadWrite)
{
}

SVGigeFeature::SVGigeFeature(const SVString& p_name, SVMatroxDigitizerFeature::SVFeatureTypeEnum p_featureType, SVGigeFeature::FeatureSupportedEnum p_supported, SVGigeFeature::FeatureAccessEnum p_access)
: m_name(p_name), m_featureType(p_featureType), m_supported(p_supported), m_access(p_access)
{
}

SVGigeFeature::SVGigeFeature(const SVString& p_name, SVMatroxDigitizerFeature::SVFeatureTypeEnum p_featureType, SVGigeFeature::FeatureSupportedEnum p_supported, SVGigeFeature::FeatureAccessEnum p_access, const SVGigeFeatureSelector& p_selector)
: m_name(p_name), m_featureType(p_featureType), m_supported(p_supported), m_access(p_access), m_selector(p_selector)
{
}

SVGigeFeature::SVGigeFeature(const SVString& p_name, SVMatroxDigitizerFeature::SVFeatureTypeEnum p_featureType, SVGigeFeature::FeatureSupportedEnum p_supported, SVGigeFeature::FeatureAccessEnum p_access, const SVGigeFeatureSelector& p_selector, const SVGigeFeatureStringEnumList& p_stringEnums)
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

const SVString& SVGigeFeature::GetName() const
{
	return m_name;
}

SVMatroxDigitizerFeature::SVFeatureTypeEnum SVGigeFeature::GetType() const
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

HRESULT SVGigeFeature::GetGigeFeatureString(const SVDeviceParamString& in, SVGigeFeatureString& out) const
{
	HRESULT hr = m_stringEnums.GetGigeFeatureString(in, out);
	
	return hr;
}

HRESULT SVGigeFeature::GetDeviceParamString(const SVGigeFeatureString& in, SVDeviceParamString& out) const
{
	HRESULT hr =  m_stringEnums.GetDeviceParamString(in, out);

	return hr;
}

