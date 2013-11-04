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

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\PropertyTree\SVRPropTreeState.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 16:43:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Dec 2009 10:16:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Jul 2004 11:21:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2004 11:16:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  428
 * SCR Title:  Improve Load Configuration time
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/