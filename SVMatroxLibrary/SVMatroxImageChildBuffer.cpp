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

	if( 0 != m_Identifier )
	{
		SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

		if( SVMEE_STATUS_OK == SVMatroxResourceMonitor::GetAutoLock( l_AutoLock )  )
		{
			MIL_ID l_SystemID = MbufInquire( m_Identifier, M_OWNER_SYSTEM, nullptr );

			if( M_NULL != l_SystemID && S_OK == SVMatroxApplicationInterface::GetLastStatus()  )
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

