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

SVMatroxImageBuffer::SVMatroxImageBuffer( SVMatroxIdentifier p_Identifier, const std::string& p_rCreatorName )
: SVMatroxBufferTemplate( p_Identifier, p_rCreatorName )
{
	SVMatroxResourceMonitor::InsertIdentifier( SVBufferID, p_Identifier );
}

SVMatroxImageBuffer::~SVMatroxImageBuffer()
{
	SVMatroxIdentifier Identifier = GetIdentifier();
	if( 0 != Identifier )
	{
		SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

		if( S_OK == SVMatroxResourceMonitor::GetAutoLock( l_AutoLock ) )
		{
			MIL_ID l_SystemID = MbufInquire( Identifier, M_OWNER_SYSTEM, nullptr );

			if( M_NULL != l_SystemID && S_OK == SVMatroxApplicationInterface::GetLastStatus() )
			{
				SVMatroxResourceMonitor::EraseIdentifier( SVBufferID, Identifier );
			}
			else
			{
				assert( false );
			}
		}
		freeBuffer();
	}
}



