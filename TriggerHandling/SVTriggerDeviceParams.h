//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerDeviceParams
//* .File Name       : $Workfile:   TriggerHandling/SVTriggerDeviceParams.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 12:28:54  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

namespace Seidenader { namespace TriggerHandling {

	struct SVTriggerDeviceParams
	{
		SVTriggerDeviceParams();
		SVTriggerDeviceParams( const SVTriggerDeviceParams& p_rObject );
		SVTriggerDeviceParams( LPCTSTR p_szName, int p_Channel );

		SVString m_Name;
		int m_Channel;
	};

	typedef std::vector< SVTriggerDeviceParams > SVTriggerDeviceParamsVector;

} /* namespace TriggerHandling */ } /* namespace Seidenader */

namespace SvTh = Seidenader::TriggerHandling;
