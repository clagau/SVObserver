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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVGigeCameraParametersLibrary\SVGigeFeatureSelector.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:54:54   bWalter
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

