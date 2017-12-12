//*****************************************************************************
/// This file contains helper function for this library
//*****************************************************************************
#include "StdAfx.h"
#include "SVMatroxHelper.h"
#include "SVMatroxResourceMonitor.h"
#include "SVMatroxApplicationInterface.h"
#include "SVMatroxErrorEnum.h"

HRESULT DestroyMatroxId(SVMatroxIdentifier& rId, void(*pFreeFunc)(MIL_ID), SVMatroxIdentifierEnum identifierType)
{
	HRESULT Result(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (nullptr != pFreeFunc)
		{
			if (M_NULL != rId)
			{
				SVMatroxResourceMonitor::SVAutoLock AutoLock;

				Result = SVMatroxResourceMonitor::GetAutoLock(AutoLock);

				if (S_OK == Result)
				{
					pFreeFunc(rId);
					Result = SVMatroxApplicationInterface::GetLastStatus();
					if (S_OK == Result)
					{
						SVMatroxResourceMonitor::EraseIdentifier(identifierType, rId);

						rId = M_NULL;
					}
				}
			}
		}
		else
		{
			Result = SVMEE_BAD_POINTER;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		Result = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return Result;
}