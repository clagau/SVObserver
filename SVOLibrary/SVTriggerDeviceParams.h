//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerDeviceParams
//* .File Name       : $Workfile:   SVTriggerDeviceParams.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 12:28:54  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>

#pragma endregion Includes

struct SVTriggerDeviceParams
{
	SVTriggerDeviceParams();
	SVTriggerDeviceParams( const SVTriggerDeviceParams& p_rObject );
	SVTriggerDeviceParams( LPCTSTR p_szName, int p_Channel );

	std::string m_Name;
	int m_Channel;
};

typedef std::vector< SVTriggerDeviceParams > SVTriggerDeviceParamsVector;
