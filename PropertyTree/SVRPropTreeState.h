// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRPropTreeState
// * .File Name       : $Workfile:   SVRPropTreeState.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 16:43:22  $
// ******************************************************************************

#pragma once

#include <map>

class SVRPropTreeState  
{
public:

	struct SVRPropTreeItemState
	{
		bool bExpanded;
	};
	typedef SVRPropTreeItemState ItemType;
	typedef std::map < CString, ItemType > MapType;

	SVRPropTreeState();
	virtual ~SVRPropTreeState();
	SVRPropTreeState( const SVRPropTreeState& rhs );
	const SVRPropTreeState& operator = ( const SVRPropTreeState& rhs );
	MapType m_State;
};
