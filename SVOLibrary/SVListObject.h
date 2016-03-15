// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVListObject
// * .File Name       : $Workfile:   SVListObject.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 11:53:22  $
// ******************************************************************************

#ifndef INC_SVLISTOBJECT_INCLUDED
#define INC_SVLISTOBJECT_INCLUDED

#include <deque>
#include <comdef.h>

class SVListObject
{
public:
	SVListObject();
	virtual ~SVListObject();

	BOOL Create();
	BOOL Destroy();
	BOOL IsCreated();

	BOOL Lock();
	BOOL Unlock();

	// Add to the List
	BOOL Add( void *pParam );

	// Get information about the List
	BOOL GetAt( long lPosition, void *&pParam );
	BOOL GetSize( long &lSize );

	// Remove from the List
	BOOL RemoveAt( long lPosition );
	BOOL RemoveAll();

private:
	typedef std::deque< void* > SVVoidDeque;

	CRITICAL_SECTION m_hCriticalSection;

	SVVoidDeque m_List;

	BOOL m_bCreated;

};

#endif /* _INC_SVLISTOBJECT_INCLUDED */

