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
	SVMatroxIdentifier Identifier = GetIdentifier();
	if( 0 != Identifier )
	{
		SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

		if( S_OK == SVMatroxResourceMonitor::GetAutoLock( l_AutoLock )  )
		{
			MIL_ID l_SystemID = MbufInquire( Identifier, M_OWNER_SYSTEM, nullptr );

			if( M_NULL != l_SystemID && S_OK == SVMatroxApplicationInterface::GetLastStatus()  )
			{
				SVMatroxResourceMonitor::EraseIdentifier( SVChildBufferID, Identifier );
			}
			else
			{
				assert( false );
			}
		}

		freeBuffer();
	}

	m_ParentPtr.clear();
}

SVMatroxImageChildBuffer::SVMatroxImageChildBuffer( SVMatroxBufferPtr p_ParentPtr, SVMatroxIdentifier p_Identifier, const std::string& p_rCreatorName )
: m_ParentPtr( p_ParentPtr ), SVMatroxBufferTemplate( p_Identifier, p_rCreatorName )
{
	SVMatroxResourceMonitor::InsertIdentifier( SVChildBufferID, p_Identifier );
}

