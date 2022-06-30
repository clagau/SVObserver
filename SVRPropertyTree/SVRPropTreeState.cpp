// ******************************************************************************
// * COPYRIGHT (c) 2000 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRPropTreeState
// * .File Name       : $Workfile:   SVRPropTreeState.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 16:43:20  $
// ******************************************************************************

#include "stdafx.h"
#include "SVRPropTreeState.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVRPropTreeState::SVRPropTreeState( const SVRPropTreeState& rRhs )
	:m_State{rRhs.m_State}
{
}

const SVRPropTreeState& SVRPropTreeState::operator = ( const SVRPropTreeState& rRhs )
{
	m_State = rRhs.m_State;
	return *this;
}

