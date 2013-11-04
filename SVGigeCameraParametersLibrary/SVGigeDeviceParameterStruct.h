//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeDeviceParameterStruct
//* .File Name       : $Workfile:   SVGigeDeviceParameterStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:46:32  $
//******************************************************************************

#ifndef SVGIGEDEVICEPARAMETERSTRUCT_H
#define SVGIGEDEVICEPARAMETERSTRUCT_H

#include <map>
#include <boost/config.hpp>
#include <boost/function.hpp>
#include "SVUtilityLibrary/SVString.h"
#include "SVGigeEnums.h"
#include "SVGigeParameterAccessor.h"

struct SVGigeDeviceParameterStruct
{
	SVString name;
	VARTYPE dataType;

	SVGigeParameterAccessor accessor;
	SVGigeDeviceParameterStruct(const SVString& p_name, VARTYPE p_dataType, const SVGigeParameterAccessor& accessor);
	SVGigeDeviceParameterStruct(const SVGigeDeviceParameterStruct& rParam);
	SVGigeDeviceParameterStruct& operator=(const SVGigeDeviceParameterStruct& rParam);
};

typedef std::map<SVGigeParameterEnum, SVGigeDeviceParameterStruct> SVGigeDeviceParameterMap;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVGigeCameraParametersLibrary\SVGigeDeviceParameterStruct.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:46:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:05:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
