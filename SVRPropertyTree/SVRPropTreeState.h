// ******************************************************************************
// * COPYRIGHT (c) 2000 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRPropTreeState
// * .File Name       : $Workfile:   SVRPropTreeState.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 16:43:22  $
// ******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>

#pragma endregion Includes

class SVRPropTreeState  
{
public:

	typedef std::map<std::string, bool> MapStringTreeState;

	SVRPropTreeState() =default;
	virtual ~SVRPropTreeState() = default;
	SVRPropTreeState( const SVRPropTreeState& rRhs );
	const SVRPropTreeState& operator = ( const SVRPropTreeState& rRhs );
	MapStringTreeState m_State;
};
