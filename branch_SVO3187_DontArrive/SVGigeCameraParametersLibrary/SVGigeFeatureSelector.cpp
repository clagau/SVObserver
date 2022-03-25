//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeFeatureSelector
//* .File Name       : $Workfile:   SVGigeFeatureSelector.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:54:52  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVGigeFeatureSelector.h"
#pragma endregion Includes

SVGigeFeatureSelector::SVGigeFeatureSelector()
{
}

SVGigeFeatureSelector::SVGigeFeatureSelector(const std::string& featureName)
: m_featureName(featureName)
{
}

SVGigeFeatureSelector::SVGigeFeatureSelector(const std::string& featureName, const std::string& featureValue)
: m_featureName(featureName), m_featureValue(featureValue)
{
}

SVGigeFeatureSelector::SVGigeFeatureSelector(const SVGigeFeatureSelector& rSelector)
: m_featureName(rSelector.m_featureName), m_featureValue(rSelector.m_featureValue)
{
}

SVGigeFeatureSelector& SVGigeFeatureSelector::operator=(const SVGigeFeatureSelector& rSelector)
{
	if (this != &rSelector)
	{
		m_featureName = rSelector.m_featureName;
		m_featureValue = rSelector.m_featureValue;
	}
	return *this;
}

void SVGigeFeatureSelector::SetValue(const std::string& featureValue)
{
	m_featureValue = featureValue;
}

const std::string& SVGigeFeatureSelector::GetValue() const
{
	return m_featureValue;
}

const std::string& SVGigeFeatureSelector::GetName() const
{
	return m_featureName;
}

