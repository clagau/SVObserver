// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxDisplayBuffer
// * .File Name       : $Workfile:   SVMatroxDisplayBuffer.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:12:38  $
// ******************************************************************************


#include "stdafx.h"
#include "SVMatroxDisplayBuffer.h"
#include "SVMatroxDisplayInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxDisplayBuffer::SVMatroxDisplayBuffer()
: m_DisplayBufferPtr()
{
}

SVMatroxDisplayBuffer::~SVMatroxDisplayBuffer()
{
}

bool SVMatroxDisplayBuffer::empty() const
{
	return m_DisplayBufferPtr.empty() || m_DisplayBufferPtr->empty();
}

void SVMatroxDisplayBuffer::clear()
{
	m_DisplayBufferPtr.clear();
}

bool SVMatroxDisplayBuffer::operator<( const SVMatroxDisplayBuffer& p_rBuf ) const
{
	bool l_Status = true;

	if( m_DisplayBufferPtr.empty() )
	{
		l_Status = !( p_rBuf.m_DisplayBufferPtr.empty() );
	}
	else
	{
		l_Status = ( p_rBuf.m_DisplayBufferPtr.empty() );

		if( !l_Status )
		{
			l_Status = ( m_DisplayBufferPtr->GetIdentifier() < p_rBuf.m_DisplayBufferPtr->GetIdentifier() );
		}
	}

	return l_Status;
}

SVMatroxIdentifier SVMatroxDisplayBuffer::GetIdentifier() const
{
	SVMatroxIdentifier l_Identifier = 0;

	if( !( m_DisplayBufferPtr.empty() ) )
	{
		l_Identifier = m_DisplayBufferPtr->GetIdentifier();
	}

	return l_Identifier;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxDisplayBuffer.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:12:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Jan 2011 10:36:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:20:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
