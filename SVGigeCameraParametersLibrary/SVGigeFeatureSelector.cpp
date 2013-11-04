//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeFeatureSelector
//* .File Name       : $Workfile:   SVGigeFeatureSelector.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:54:52  $
//******************************************************************************
#include "stdafx.h"
#include "SVGigeFeatureSelector.h"

SVGigeFeatureSelector::SVGigeFeatureSelector()
{
}

SVGigeFeatureSelector::SVGigeFeatureSelector(const SVString& featureName)
: m_featureName(featureName)
{
}

SVGigeFeatureSelector::SVGigeFeatureSelector(const SVString& featureName, const SVString& featureValue)
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

void SVGigeFeatureSelector::SetValue(const SVString& featureValue)
{
	m_featureValue = featureValue;
}

const SVString& SVGigeFeatureSelector::GetValue() const
{
	return m_featureValue;
}

const SVString& SVGigeFeatureSelector::GetName() const
{
	return m_featureName;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVGigeCameraParametersLibrary\SVGigeFeatureSelector.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:54:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:06:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
