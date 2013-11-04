//******************************************************************************
//* COPYRIGHT (c) 2011 by SVSeidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxImageChildBuffer
//* .File Name       : $Workfile:   SVMatroxImageChildBuffer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:18:12  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxImageChildBuffer.h"
#include "SVMatroxApplicationInterface.h"
#include "SVMatroxErrorEnum.h"
#include "SVMatroxImagingLibrary.h"  // has MIL includes
#include "SVMatroxResourceMonitor.h"

SVMatroxImageChildBuffer::SVMatroxImageChildBuffer()
: m_ParentPtr(), SVMatroxBufferTemplate()
{
}

SVMatroxImageChildBuffer::~SVMatroxImageChildBuffer()
{
	assert( m_StartIdentifier == m_Identifier );

	if( m_Identifier != 0 )
	{
		SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

		if( SVMatroxResourceMonitor::GetAutoLock( l_AutoLock ) == SVMEE_STATUS_OK )
		{
			MIL_ID l_SystemID = MbufInquire( m_Identifier, M_OWNER_SYSTEM, NULL );

			if( l_SystemID != M_NULL && SVMatroxApplicationInterface::GetLastStatus() == S_OK )
			{
				SVMatroxResourceMonitor::EraseIdentifier( SVChildBufferID, m_Identifier );
			}
			else
			{
				assert( false );
			}

			MbufFree( m_Identifier );
		}

		m_Identifier = 0;
	}

	m_ParentPtr.clear();
}

SVMatroxImageChildBuffer::SVMatroxImageChildBuffer( SVMatroxBufferPtr p_ParentPtr, SVMatroxIdentifier p_Identifier, const SVString& p_rCreatorName )
: m_ParentPtr( p_ParentPtr ), SVMatroxBufferTemplate( p_Identifier, p_rCreatorName )
{
	SVMatroxResourceMonitor::InsertIdentifier( SVChildBufferID, m_Identifier );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxImageChildBuffer.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:18:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Sep 2011 14:29:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated Martox Library to include a new resource management system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Feb 2011 16:28:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to move data elements to appropriate objects to minimize tight object coupling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Feb 2011 13:57:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Jan 2011 10:36:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
