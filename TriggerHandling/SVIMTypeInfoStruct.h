//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIMTypeInfoStruct
//* .File Name       : $Workfile:   TriggerHandling/SVIMTypeInfoStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:58:58  $
//******************************************************************************

#pragma once

namespace Seidenader { namespace TriggerHandling {

	struct SVIMTypeInfoStruct
	{
		bool m_Supported;
		short m_MaxDigitizers;
		short m_MaxTriggers;

		SVIMTypeInfoStruct( bool p_Supported, short p_MaxDigitizers, short p_MaxTriggers ) :
		  m_Supported( p_Supported )
		, m_MaxDigitizers( p_MaxDigitizers )
		, m_MaxTriggers( p_MaxTriggers)
		{
		}
	};

} /* namespace TriggerHandling */ } /* namespace Seidenader */

namespace SvTh = Seidenader::TriggerHandling;
