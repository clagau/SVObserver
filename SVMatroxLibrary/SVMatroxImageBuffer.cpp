//******************************************************************************
//* COPYRIGHT (c) 2011 by SVSeidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxImageBuffer
//* .File Name       : $Workfile:   SVMatroxImageBuffer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:18:00  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxImageBuffer.h"
#include "SVMatroxApplicationInterface.h"
#include "SVMatroxErrorEnum.h"
#include "SVMatroxImagingLibrary.h"  // has MIL includes
#include "SVMatroxResourceMonitor.h"

SVMatroxImageBuffer::SVMatroxImageBuffer()
: SVMatroxBufferTemplate()
{
}

SVMatroxImageBuffer::SVMatroxImageBuffer( SVMatroxIdentifier p_Identifier, const SVString& p_rCreatorName )
: SVMatroxBufferTemplate( p_Identifier, p_rCreatorName )
{
	SVMatroxResourceMonitor::InsertIdentifier( SVBufferID, m_Identifier );
}

SVMatroxImageBuffer::~SVMatroxImageBuffer()
{
	#ifdef _DEBUG
		assert( m_StartIdentifier == m_Identifier );

		/*
		SVString l_Temp;

		l_Temp.Format( "EraseIdentifier: %ld-%s\n", m_Identifier, m_CreatorName.c_str() );

		::OutputDebugString( l_Temp.c_str() );
		*/
	#endif

	if( m_Identifier != 0 )
	{
		SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

		if( SVMatroxResourceMonitor::GetAutoLock( l_AutoLock ) == SVMEE_STATUS_OK )
		{
			MIL_ID l_SystemID = MbufInquire( m_Identifier, M_OWNER_SYSTEM, NULL );

			if( l_SystemID != M_NULL && SVMatroxApplicationInterface::GetLastStatus() == S_OK )
			{
				SVMatroxResourceMonitor::EraseIdentifier( SVBufferID, m_Identifier );
			}
			else
			{
				assert( false );
			}

			MbufFree( m_Identifier );
		}

		m_Identifier = 0;
	}
}



