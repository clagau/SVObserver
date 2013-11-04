//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxSystem
//* .File Name       : $Workfile:   SVMatroxSystem.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:27:34  $
//******************************************************************************
#ifndef SVMATROXSYSTEM_H
#define SVMATROXSYSTEM_H

#include <boost/utility.hpp>
#include "SVMatroxLibrary/SVMatroxTypedefs.h"

class SVMatroxSystemInterface;
class SVMatroxBufferInterface;
class SVMatroxDigitizerInterface;

/**
@SVObjectName Matrox System

@SVObjectOverview This class is a data holder for a matrox system id.

@SVObjectOperations The empty function is used to detect if the handle is empty.  When this class is destroyed the the LocalClear will free the matrox handle.

*/

class SVMatroxSystem : public boost::noncopyable
{
	friend SVMatroxSystemInterface;
	friend SVMatroxBufferInterface;
	friend SVMatroxDigitizerInterface;

public:
	SVMatroxSystem();
	virtual ~SVMatroxSystem();

	bool operator<( const SVMatroxSystem& p_rSystem ) const;

	bool empty() const;

protected:
	SVMatroxIdentifier m_SystemIdentifier;

private:
	void LocalClear();
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxSystem.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:27:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Jan 2011 10:39:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2009 15:35:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/