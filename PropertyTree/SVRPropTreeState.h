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

#ifndef SVRPROPTREESTATE_H
#define SVRPROPTREESTATE_H

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

#if 0

int i,j;
i = j = 0;			// OK
const int* pi = &i;	// OK
pi = &j;			// OK
*pi = 0;			// NOT OK; assigning the value
int* const pj = &j;	// OK
pj = &i;			// NOT OK; reassigning the pointer
*pj = 2;			// OK


#endif



#endif

