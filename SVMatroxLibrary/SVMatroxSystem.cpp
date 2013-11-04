//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxSystem
//* .File Name       : $Workfile:   SVMatroxSystem.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:27:28  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxSystem.h"
#include "SVMatroxSystemInterface.h"

SVMatroxSystem::SVMatroxSystem()
: m_SystemIdentifier(0)
{
}

SVMatroxSystem::~SVMatroxSystem()
{
	LocalClear();
}

bool SVMatroxSystem::operator<( const SVMatroxSystem& p_rSystem ) const 
{
	return m_SystemIdentifier < p_rSystem.m_SystemIdentifier; 
}

bool SVMatroxSystem::empty() const
{
	return ( m_SystemIdentifier == 0 );
}

void SVMatroxSystem::LocalClear()
{
	SVMatroxSystemInterface::Destroy(*this);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxSystem.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:27:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Jan 2011 10:39:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2009 15:35:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/