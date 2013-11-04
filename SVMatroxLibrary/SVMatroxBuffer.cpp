//******************************************************************************
//* COPYRIGHT (c) 2007 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxBuffer
//* .File Name       : $Workfile:   SVMatroxBuffer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:59:46  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxBuffer.h"
#include "SVMatroxBufferInterface.h"
#include "SVMatroxDisplayBuffer.h"

SVMatroxBuffer::SVMatroxBuffer()
: m_BufferPtr()
{
}

SVMatroxBuffer::SVMatroxBuffer(const SVMatroxBuffer& p_rBuf)
{
	m_BufferPtr = p_rBuf.m_BufferPtr;
}

SVMatroxBuffer::SVMatroxBuffer(const SVMatroxDisplayBuffer& p_rBuf)
{
	m_BufferPtr = p_rBuf.m_DisplayBufferPtr;
}

SVMatroxBuffer::~SVMatroxBuffer()
{
}

bool SVMatroxBuffer::operator<(const SVMatroxBuffer& p_rBuf) const 
{
	bool l_Status = true;

	if( m_BufferPtr.empty() )
	{
		l_Status = !( p_rBuf.m_BufferPtr.empty() );
	}
	else
	{
		l_Status = ( p_rBuf.m_BufferPtr.empty() );

		if( !l_Status )
		{
			l_Status = ( m_BufferPtr->GetIdentifier() < p_rBuf.m_BufferPtr->GetIdentifier() );
		}
	}

	return l_Status;
}

bool SVMatroxBuffer::empty() const
{
	return ( m_BufferPtr.empty() || m_BufferPtr->empty() );
}

void SVMatroxBuffer::clear()
{
	m_BufferPtr.clear();
}

void SVMatroxBuffer::swap( SVMatroxBuffer& p_rBuf )
{
	SVMatroxBufferPtr l_ID = m_BufferPtr;
	m_BufferPtr = p_rBuf.m_BufferPtr;
	p_rBuf.m_BufferPtr = l_ID;
}

const SVMatroxBuffer& SVMatroxBuffer::operator=( const SVMatroxBuffer& p_rBuf )
{
	if( this != &p_rBuf )
	{
		m_BufferPtr = p_rBuf.m_BufferPtr;
	}

	return *this;
}

SVMatroxIdentifier SVMatroxBuffer::GetIdentifier() const
{
	SVMatroxIdentifier l_Identifier = 0;

	if( !( m_BufferPtr.empty() ) )
	{
		l_Identifier = m_BufferPtr->GetIdentifier();
	}

	return l_Identifier;
}

bool SVMatroxBuffer::IsParent( const SVMatroxBuffer& p_rParent ) const
{
	bool l_Status = ( SVMatroxBufferInterface::IsParent( p_rParent, *this ) == S_OK );

	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxBuffer.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:59:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   18 Feb 2011 16:28:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to move data elements to appropriate objects to minimize tight object coupling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Feb 2011 13:57:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Jan 2011 10:31:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Jun 2009 16:00:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Export method to be const
 * Moved code from header to implementation file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2009 15:52:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Export method to be const.
 * Moved code from header to implementation file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:18:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
