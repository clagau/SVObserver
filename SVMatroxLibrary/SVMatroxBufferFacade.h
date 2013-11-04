//******************************************************************************
//* COPYRIGHT (c) 2011 by SVSeidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxBufferFacade
//* .File Name       : $Workfile:   SVMatroxBufferFacade.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:02:04  $
//******************************************************************************

#ifndef SVMATROXBUFFERFACADE_H
#define SVMATROXBUFFERFACADE_H

#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVMatroxTypedefs.h"

class SVMatroxBufferFacade
{
public:
	friend class SVMatroxBuffer;
	friend class SVMatroxDisplayBuffer;
	friend class SVMatroxImageChildBuffer;

	virtual ~SVMatroxBufferFacade();

	virtual bool empty() const = 0;

protected:
	SVMatroxBufferFacade();

	virtual SVMatroxIdentifier GetIdentifier() const = 0;

	// Do not implement this method
	SVMatroxBufferFacade( const SVMatroxBufferFacade& p_rObject );

	// Do not implement this method
	const SVMatroxBufferFacade& operator=( const SVMatroxBufferFacade& p_rObject );

};

typedef SVSharedPtr< SVMatroxBufferFacade > SVMatroxBufferPtr;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxBufferFacade.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:02:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Feb 2011 16:28:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to move data elements to appropriate objects to minimize tight object coupling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Feb 2011 13:57:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Jan 2011 10:31:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
