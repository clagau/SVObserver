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
#ifndef SVGIGEFEATURESELECTOR_H
#define SVGIGEFEATURESELECTOR_H

#include <comdef.h>
#include "SVUtilityLibrary/SVString.h"

class SVGigeFeatureSelector
{
private:
	SVString m_featureName;
	SVString m_featureValue;

public:
	SVGigeFeatureSelector();
	SVGigeFeatureSelector(const SVString& featureName);
	SVGigeFeatureSelector(const SVString& featureName, const SVString& featureValue);
	SVGigeFeatureSelector(const SVGigeFeatureSelector& rSelector);
	SVGigeFeatureSelector& operator=(const SVGigeFeatureSelector& rSelector);
	
	void SetValue(const SVString& featureValue);
	const SVString& GetValue() const;
	const SVString& GetName() const;
};

#endif

