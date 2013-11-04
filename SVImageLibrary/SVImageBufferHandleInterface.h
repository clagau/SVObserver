//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBufferHandleInterface
//* .File Name       : $Workfile:   SVImageBufferHandleInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:44:42  $
//******************************************************************************

#ifndef SVIMAGEBUFFERHANDLEINTERFACE_H
#define SVIMAGEBUFFERHANDLEINTERFACE_H

#include "SVUtilityLibrary/SVBitmapInfo.h"
#include "SVUtilityLibrary/SVPOINT.h"
#include "SVUtilityLibrary/SVSharedPtr.h"

class SVImageBufferHandleData;

class SVImageBufferHandleInterface  
{
public:
	virtual ~SVImageBufferHandleInterface();

	virtual bool empty() const = 0;
	virtual void clear() = 0;

	virtual SVPOINT GetPositionPoint() const = 0;
	virtual SVBitmapInfo GetBitmapInfo() const = 0;
	virtual unsigned char* GetBufferAddress() const = 0;

	virtual HRESULT GetData( SVImageBufferHandleData& p_rData ) const = 0;

protected:
	SVImageBufferHandleInterface();

private:
	// Do not implement
	SVImageBufferHandleInterface( const SVImageBufferHandleInterface& p_rObject );

	// Do not implement
	const SVImageBufferHandleInterface& operator=( const SVImageBufferHandleInterface& p_rObject );

};

typedef SVSharedPtr< SVImageBufferHandleInterface > SVSmartHandlePointer;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVImageBufferHandleInterface.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:44:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   18 Feb 2011 16:23:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to move data elements to appropriate objects to minimize tight object coupling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Feb 2011 13:48:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Jan 2011 09:54:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Dec 2010 15:41:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Dec 2009 11:34:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   13 Jul 2009 13:26:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated image library to include the new data manager objects to track the indexes.  Updated DLL interface to use the new buffer transfer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

