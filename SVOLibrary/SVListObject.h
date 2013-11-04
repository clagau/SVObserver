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

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOLibrary\SVListObject.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 11:53:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Dec 2010 16:04:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Nov 2002 10:17:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
