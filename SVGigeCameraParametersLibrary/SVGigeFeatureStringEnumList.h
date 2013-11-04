//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeFeatureStringEnum
//* .File Name       : $Workfile:   SVGigeFeatureStringEnumList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:54:58  $
//******************************************************************************
#ifndef SVGIGEFEATURESTRINGENUM_H
#define SVGIGEFEATURESTRINGENUM_H

#include "SVUtilityLibrary/SVString.h"
#include "SVContainerLibrary/SVBidirectionalMap.h"

typedef SVString SVDeviceParamString;
typedef SVString SVGigeFeatureString;

class SVGigeFeatureStringEnumList
{
	typedef SVBidirectionalMap<SVDeviceParamString, SVGigeFeatureString>::type GigeFeatureStringEnumList;
	GigeFeatureStringEnumList m_stringEnums;

public:
	SVGigeFeatureStringEnumList();
	~SVGigeFeatureStringEnumList();

	template<class _Iter>
	SVGigeFeatureStringEnumList(_Iter first, _Iter last);

	SVGigeFeatureStringEnumList(const SVGigeFeatureStringEnumList& rList);
	SVGigeFeatureStringEnumList& operator=(const SVGigeFeatureStringEnumList& rList);

	bool HasTranslation() const;
	HRESULT GetGigeFeatureString(const SVDeviceParamString& in, SVGigeFeatureString& out) const;
	HRESULT GetDeviceParamString(const SVGigeFeatureString& in, SVDeviceParamString& out) const;
};

#include "SVGigeFeatureStringEnumList.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVGigeCameraParametersLibrary\SVGigeFeatureStringEnumList.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:54:58   bWalter
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

