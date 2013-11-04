//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVParamListDeviceParam
//* .File Name       : $Workfile:   SVParamListDeviceParam.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 13:10:48  $
//******************************************************************************
#ifndef SVPARAMLISTDEVICEPARAM_H
#define SVPARAMLISTDEVICEPARAM_H

#include "SVDeviceParams.h"

class SVParamListDeviceParam : public SVDeviceParam
{
public:
	SVParamListDeviceParam();
	SVParamListDeviceParam(SVDeviceParamEnum typeEnum);
	SVParamListDeviceParam(const SVParamListDeviceParam& rhs);
	virtual SVClonable* CloneImpl() const;

	std::map <SVDeviceParamEnum, SVDeviceParamWrapper> mapParameters;

	static SVDeviceParam* CreateNew() { return new SVParamListDeviceParam; }
	DEFINE_VISITABLE()
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVParamListDeviceParam.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 13:10:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Apr 2013 13:37:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added constructor that takes an argument of type SVDeviceParamEnum
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   03 Jun 2009 08:51:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin (Moved from SVDeviceParams)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

