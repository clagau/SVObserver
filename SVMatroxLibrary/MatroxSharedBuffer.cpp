#include "stdafx.h"
#include "MatroxSharedBuffer.h"


MatroxSharedBuffer::MatroxSharedBuffer(void)
	:SVMatroxBufferTemplate()
{
}

MatroxSharedBuffer::MatroxSharedBuffer( SVMatroxIdentifier Identifier, const SVString& rCreatorName )
	: SVMatroxBufferTemplate( Identifier, rCreatorName )
{
	
	//SVMatroxResourceMonitor::InsertIdentifier( SVBufferID, m_Identifier );
}

MatroxSharedBuffer::~MatroxSharedBuffer(void)
{
	
	/*SVMatroxIdentifier Identifier = GetIdentifier();
	if (0 != Identifier)
	{
		SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

		if (S_OK == SVMatroxResourceMonitor::GetAutoLock(l_AutoLock))
		{
			MIL_ID l_SystemID = MbufInquire(Identifier, M_OWNER_SYSTEM, nullptr);

			if (M_NULL != l_SystemID && S_OK == SVMatroxApplicationInterface::GetLastStatus())
			{
				SVMatroxResourceMonitor::EraseIdentifier(SVBufferID, Identifier);
			}
			else
			{
				assert(false);
			}
		}
		freeBuffer();
	}*/
	freeBuffer();
}