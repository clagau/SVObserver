//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeFeatureSelector
//* .File Name       : $Workfile:   SVGigeFeatureSelector.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:54:54  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>

#pragma endregion Includes

class SVGigeFeatureSelector
{
private:
	std::string m_featureName;
	std::string m_featureValue;

public:
	SVGigeFeatureSelector();
	explicit SVGigeFeatureSelector(const std::string& featureName);
	SVGigeFeatureSelector(const std::string& featureName, const std::string& featureValue);
	SVGigeFeatureSelector(const SVGigeFeatureSelector& rSelector);
	SVGigeFeatureSelector& operator=(const SVGigeFeatureSelector& rSelector);
	
	void SetValue(const std::string& featureValue);
	const std::string& GetValue() const;
	const std::string& GetName() const;
};

