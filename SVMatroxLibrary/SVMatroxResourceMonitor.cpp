// ******************************************************************************
// * COPYRIGHT (c) 2011 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxResourceMonitor
// * .File Name       : $Workfile:   SVMatroxResourceMonitor.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   03 Jun 2014 14:14:12  $
// ******************************************************************************

#include "stdafx.h"
#include "SVMatroxResourceMonitor.h"
#include "SVMatroxApplicationInterface.h"
#include "SVMatroxErrorEnum.h"
#include "SVUtilityLibrary/StringHelper.h"


#ifdef _DEBUG
#define MONITOR_MIL_RESOURCES
#endif

SVMatroxResourceMonitor::SVMatroxResourceMonitor()
:m_Identifiers(SVIdentifierCount)
, m_AllIdentifiers()
{
}

SVMatroxResourceMonitor::~SVMatroxResourceMonitor()
{
#ifdef MONITOR_MIL_RESOURCES
	OutputDebug();
#endif 
}


#ifdef MONITOR_MIL_RESOURCES
SVMatroxResourceMonitor& SVMatroxResourceMonitor::Instance()
{
	static SVMatroxResourceMonitor l_Object;
	return l_Object;
}
#endif 

HRESULT SVMatroxResourceMonitor::InsertIdentifier( SVMatroxIdentifierEnum identifierType, __int64 identifier )
{
	HRESULT l_Status = S_OK;
	UNREFERENCED_PARAMETER(identifierType);
	UNREFERENCED_PARAMETER(identifier);

#ifdef MONITOR_MIL_RESOURCES
	SVMatroxResourceMonitor& monitor = SVMatroxResourceMonitor::Instance();

	std::lock_guard<std::mutex> Autolock(monitor.m_Mutex);

	SVIdentifierSet::iterator l_AllIter = monitor.m_AllIdentifiers.find(identifier);

	if (0 <= identifierType && identifierType < static_cast<int>(monitor.m_Identifiers.size()))
	{
		SVIdentifierSet& p_rSet = monitor.m_Identifiers[identifierType];
		SVIdentifierSet::iterator l_Iter = p_rSet.find(identifier);

		if (l_Iter == p_rSet.end())
		{
			p_rSet.insert(identifier);
		}
		else
		{
			l_Status = E_FAIL;
		}
		if (l_AllIter == monitor.m_AllIdentifiers.end())
		{
			monitor.m_AllIdentifiers.insert(identifier);
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	assert(S_OK == l_Status);
#endif
	return l_Status;
}

#ifdef MONITOR_MIL_RESOURCES
bool SVMatroxResourceMonitor::FindReference(__int64 Identifier) const
{
	bool bRetVal = false;
	for (std::vector<SVIdentifierSet>::const_iterator it = m_Identifiers.begin(); !bRetVal && it != m_Identifiers.end(); ++it)
	{
		const SVIdentifierSet& rSet = (*it);
		SVIdentifierSet::const_iterator idIter = rSet.find(Identifier);
		bRetVal = (idIter != rSet.end());
	}
	return bRetVal;
}
#endif 

HRESULT SVMatroxResourceMonitor::EraseIdentifier(SVMatroxIdentifierEnum identifierType, __int64 identifier)
{
	HRESULT l_Status = S_OK;
	UNREFERENCED_PARAMETER(identifierType);
	UNREFERENCED_PARAMETER(identifier);
#ifdef MONITOR_MIL_RESOURCES
	SVMatroxResourceMonitor& monitor = SVMatroxResourceMonitor::Instance();
	std::lock_guard<std::mutex> Autolock(monitor.m_Mutex);
	// remove the indentifier from the Set of Type Indentifiers
	if (0 <= identifierType && identifierType < static_cast<int>(monitor.m_Identifiers.size()))
	{
		SVIdentifierSet& p_rSet = monitor.m_Identifiers[identifierType];
		SVIdentifierSet::iterator l_Iter = p_rSet.find(identifier);

		if (l_Iter != p_rSet.end())
		{
			p_rSet.erase(identifier);
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	// If there are no other references to the identifier - remove it from the all identifiers list
	if (!monitor.FindReference(identifier))
	{
		SVIdentifierSet::iterator l_AllIter = monitor.m_AllIdentifiers.find(identifier);

		if (l_AllIter != monitor.m_AllIdentifiers.end())
		{
			monitor.m_AllIdentifiers.erase(identifier);
		}
		else
		{
			l_Status = E_FAIL;
		}

		assert(S_OK == l_Status);
	}


	assert(S_OK == l_Status);
#endif
	return l_Status;
}

void SVMatroxResourceMonitor::OutputDebug()
{
#ifdef MONITOR_MIL_RESOURCES
	constexpr LPCTSTR MatroxIdentifierName[] =
	{
		"SVApplicationID",
		"SVBarCodeID",
		"SVBlobFeatureListID",
		"SVBlobResultID",
		"SVBufferID",
		"SVChildBufferID",
		"SVDisplayID",
		"SVDisplayBufferID",
		"SVGraphicsID",
		"SVImageID",
		"SVOCRID",
		"SVOCRResultID",
		"SVPatternModelID",
		"SVPatternResultID",
		"SVSystemID"
	};

	SVMatroxResourceMonitor& rMonitor = SVMatroxResourceMonitor::Instance();
	std::lock_guard<std::mutex> Autolock(rMonitor.m_Mutex);
	for(int i=0; i < static_cast<int> (SVIdentifierCount); ++i)
	{
		const SVIdentifierSet& rSet = rMonitor.m_Identifiers[i];
		if(rSet.size() > 0)
		{
			std::string Text{MatroxIdentifierName[i]};
			for(const auto& rID : rSet)
			{
				Text += std::format("{}; ", rID);
			}
			Text += " Matrox IDs have not been freed\n";
			::OutputDebugString(Text.c_str());
		}
	}

#endif
}

HRESULT DestroyMatroxId(__int64& rId, void(*pFreeFunc)(MIL_ID), SVMatroxIdentifierEnum identifierType)
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

				pFreeFunc(rId);
				Result = SVMatroxApplicationInterface::GetLastStatus();
				if (S_OK == Result)
				{
					SVMatroxResourceMonitor::EraseIdentifier(identifierType, rId);

					rId = M_NULL;
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
