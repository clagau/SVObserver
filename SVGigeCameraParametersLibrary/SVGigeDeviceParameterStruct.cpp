//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeDeviceParameterStruct
//* .File Name       : $Workfile:   SVGigeDeviceParameterStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:46:28  $
//******************************************************************************
#include "stdafx.h"
#include "SVGigeDeviceParameterStruct.h"

SVGigeDeviceParameterStruct::SVGigeDeviceParameterStruct(const SVString& p_name, VARTYPE p_dataType, const SVGigeParameterAccessor& p_accessor)
: name(p_name), dataType(p_dataType), accessor(p_accessor)
{
}

SVGigeDeviceParameterStruct::SVGigeDeviceParameterStruct(const SVGigeDeviceParameterStruct& rParam)
: name(rParam.name), dataType(rParam.dataType), accessor(rParam.accessor)
{
}

SVGigeDeviceParameterStruct& SVGigeDeviceParameterStruct::operator=(const SVGigeDeviceParameterStruct& rParam)
{
	if (this != &rParam)
	{
		name = rParam.name;
		dataType = rParam.dataType;
		accessor = rParam.accessor;
	}
	return *this;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVGigeCameraParametersLibrary\SVGigeDeviceParameterStruct.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:46:28   bWalter
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
