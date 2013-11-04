//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizer
//* .File Name       : $Workfile:   SVMatroxDigitizer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:27:06  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxDigitizer.h"
#include "SVMatroxDigitizerInterface.h"

SVMatroxDigitizer::SVMatroxDigitizer()
: m_DigitizerIdentifier(0)
{
}

SVMatroxDigitizer::~SVMatroxDigitizer()
{
	LocalClear();
}

bool SVMatroxDigitizer::operator<( const SVMatroxDigitizer& p_rDigitizer ) const 
{
	return m_DigitizerIdentifier < p_rDigitizer.m_DigitizerIdentifier; 
}

bool SVMatroxDigitizer::empty() const
{
	return (m_DigitizerIdentifier == 0);
}

void SVMatroxDigitizer::LocalClear()
{
	SVMatroxDigitizerInterface::Destroy(*this);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxDigitizerLibrary\SVMatroxDigitizer.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:27:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Jan 2011 10:17:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Jul 2009 16:12:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2009 15:28:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/