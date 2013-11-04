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

#ifndef SVGIGEFEATURE_H
#define SVGIGEFEATURE_H

#include "SVUtilityLibrary/SVString.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerFeatureEnums.h"
#include "SVGigeFeatureSelector.h"
#include "SVGigeFeatureStringEnumList.h"

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

	SVGigeFeature(const SVString& p_name, SVMatroxDigitizerFeature::SVFeatureTypeEnum p_featureType);
	SVGigeFeature(const SVString& p_name, SVMatroxDigitizerFeature::SVFeatureTypeEnum p_featureType, FeatureSupportedEnum p_supported, FeatureAccessEnum p_access);
	SVGigeFeature(const SVString& p_name, SVMatroxDigitizerFeature::SVFeatureTypeEnum p_featureType, FeatureSupportedEnum p_supported, FeatureAccessEnum p_access, const SVGigeFeatureSelector& p_selector);
	SVGigeFeature(const SVString& p_name, SVMatroxDigitizerFeature::SVFeatureTypeEnum p_featureType, FeatureSupportedEnum p_supported, FeatureAccessEnum p_access, const SVGigeFeatureSelector& p_selector, const SVGigeFeatureStringEnumList& p_stringEnums);
	SVGigeFeature(const SVGigeFeature& rFeature);
	
	SVGigeFeature& operator=(const SVGigeFeature& rFeature);
	
	const SVString& GetName() const;
	SVMatroxDigitizerFeature::SVFeatureTypeEnum GetType() const;
	bool IsSupported() const;
	bool IsReadOnly() const;

	bool HasSelector() const;
	const SVGigeFeatureSelector& GetSelector() const;
	bool HasTranslation() const;
	HRESULT GetGigeFeatureString(const SVDeviceParamString& in, SVGigeFeatureString& out) const;
	HRESULT GetDeviceParamString(const SVGigeFeatureString& in, SVDeviceParamString& out) const;

private:
	SVString m_name;
	SVMatroxDigitizerFeature::SVFeatureTypeEnum m_featureType;
	FeatureSupportedEnum m_supported;
	FeatureAccessEnum m_access;
	SVGigeFeatureSelector m_selector;
	SVGigeFeatureStringEnumList m_stringEnums;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVGigeCameraParametersLibrary\SVGigeFeature.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:54:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Jul 2009 15:32:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:05:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

