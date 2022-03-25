//******************************************************************************
//* COPYRIGHT (c) 2011 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxImageChildBuffer
//* .File Name       : $Workfile:   SVMatroxImageChildBuffer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:18:12  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVMatroxImageChildBuffer.h"
#include "SVMatroxApplicationInterface.h"
#include "SVMatroxResourceMonitor.h"
#pragma endregion Includes

SVMatroxImageChildBuffer::SVMatroxImageChildBuffer()
	: m_ParentPtr(), SVMatroxBufferTemplate()
{
}

SVMatroxImageChildBuffer::~SVMatroxImageChildBuffer()
{
	__int64 Identifier = GetIdentifier();
	if (0 != Identifier)
	{

		MIL_ID l_SystemID = MbufInquire(Identifier, M_OWNER_SYSTEM, nullptr);

		if (M_NULL != l_SystemID && S_OK == SVMatroxApplicationInterface::GetLastStatus())
		{
			SVMatroxResourceMonitor::EraseIdentifier(SVChildBufferID, Identifier);
		}
		else
		{
			assert(false);
		}
		//This needs to be freed so that the parent can also be freed properly
		freeBuffer();
	}

	m_ParentPtr.reset();
}

SVMatroxImageChildBuffer::SVMatroxImageChildBuffer( SVMatroxBufferPtr p_ParentPtr, __int64 p_Identifier, const std::string& p_rCreatorName )
	: m_ParentPtr(p_ParentPtr), SVMatroxBufferTemplate(p_Identifier, p_rCreatorName)
{
	SVMatroxResourceMonitor::InsertIdentifier(SVChildBufferID, p_Identifier);
}

