//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerDeviceParams
//* .File Name       : $Workfile:   SVTriggerDeviceParams.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 12:28:54  $
//******************************************************************************

#ifndef SVTRIGGERDEVICEPARAMS_H
#define SVTRIGGERDEVICEPARAMS_H

#include <vector>
#include "SVUtilityLibrary/SVString.h"

struct SVTriggerDeviceParams
{
	SVTriggerDeviceParams();
	SVTriggerDeviceParams( const SVTriggerDeviceParams& p_rObject );
	SVTriggerDeviceParams( LPCTSTR p_szName, int p_Channel );

	SVString m_Name;
	int m_Channel;
};

typedef std::vector< SVTriggerDeviceParams > SVTriggerDeviceParamsVector;

#endif

