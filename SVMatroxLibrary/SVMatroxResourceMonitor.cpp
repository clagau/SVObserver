// ******************************************************************************
// * COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
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
#include "SVMatroxErrorEnum.h"

#ifdef _DEBUG
#define MONITOR_MIL_RESOURCES
#endif

SVMatroxResourceMonitor::SVMatroxResourceMonitor()
: m_CriticalSectionPtr()
, m_Identifiers( SVIdentifierCount )
, m_AllIdentifiers()
{
	ValidateCriticalSection();
}

SVMatroxResourceMonitor& SVMatroxResourceMonitor::Instance()
{
	static SVMatroxResourceMonitor l_Object;
	return l_Object;
}

HRESULT SVMatroxResourceMonitor::InsertIdentifier( SVMatroxIdentifierEnum p_IdentifierType, SVMatroxIdentifier p_Identifier )
{
	HRESULT l_Status = S_OK;
#ifdef MONITOR_MIL_RESOURCES
	SVMatroxResourceMonitor& monitor = SVMatroxResourceMonitor::Instance();

	SVAutoLock l_AutoLock;

	l_Status = GetAutoLock( l_AutoLock );

	if( l_Status == S_OK )
	{
		SVIdentifierSet::iterator l_AllIter = monitor.m_AllIdentifiers.find( p_Identifier );

		if( 0 <= p_IdentifierType && p_IdentifierType < static_cast< int >( monitor.m_Identifiers.size() ) )
		{
			SVIdentifierSet& p_rSet = monitor.m_Identifiers[ p_IdentifierType ];
			SVIdentifierSet::iterator l_Iter = p_rSet.find( p_Identifier );

			if( l_Iter == p_rSet.end() )
			{
				p_rSet.insert( p_Identifier );
			}
			else
			{
				l_Status = E_FAIL;
			}
			if( l_AllIter == monitor.m_AllIdentifiers.end() )
			{
				monitor.m_AllIdentifiers.insert( p_Identifier );
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}

	assert( S_OK == l_Status );
#endif
	return l_Status;
}

bool SVMatroxResourceMonitor::FindReference(SVMatroxIdentifier Identifier) const
{
	bool bRetVal = false;
	for (std::vector<SVIdentifierSet>::const_iterator it = m_Identifiers.begin();!bRetVal && it != m_Identifiers.end();++it)
	{
		const SVIdentifierSet& rSet = (*it);
		SVIdentifierSet::const_iterator idIter = rSet.find(Identifier);
		bRetVal = (idIter != rSet.end());
	}
	return bRetVal;
}

HRESULT SVMatroxResourceMonitor::EraseIdentifier(SVMatroxIdentifierEnum p_IdentifierType, SVMatroxIdentifier p_Identifier)
{
	HRESULT l_Status = S_OK;
#ifdef MONITOR_MIL_RESOURCES
	SVMatroxResourceMonitor& monitor = SVMatroxResourceMonitor::Instance();
	SVAutoLock l_AutoLock;

	l_Status = GetAutoLock(l_AutoLock);

	if (l_Status == S_OK)
	{
		// remove the indentifier from the Set of Type Indentifiers
		if (0 <= p_IdentifierType && p_IdentifierType < static_cast<int>( monitor.m_Identifiers.size()))
		{
			SVIdentifierSet& p_rSet = monitor.m_Identifiers[p_IdentifierType];
			SVIdentifierSet::iterator l_Iter = p_rSet.find(p_Identifier);

			if (l_Iter != p_rSet.end())
			{
				p_rSet.erase(p_Identifier);
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
		if (!monitor.FindReference(p_Identifier))
		{
			SVIdentifierSet::iterator l_AllIter = monitor.m_AllIdentifiers.find( p_Identifier );

			if (l_AllIter != monitor.m_AllIdentifiers.end())
			{
				monitor.m_AllIdentifiers.erase(p_Identifier);
			}
			else
			{
				l_Status = E_FAIL;
			}

			assert(S_OK == l_Status );
		}
	}

	assert( S_OK == l_Status );
#endif
	return l_Status;
}

HRESULT SVMatroxResourceMonitor::GetAutoLock( SVAutoLock& p_rAutoLock )
{
	HRESULT l_Status = S_OK;
#ifdef MONITOR_MIL_RESOURCES
	SVMatroxResourceMonitor& monitor = SVMatroxResourceMonitor::Instance();
	l_Status = monitor.ValidateCriticalSection();

	if( l_Status == S_OK )
	{
		if( !( p_rAutoLock.Assign( monitor.m_CriticalSectionPtr.get() ) ) )
		{
			l_Status = SVMEE_INVALID_LOCK;
		}
	}
#endif
	return l_Status;
}

HRESULT SVMatroxResourceMonitor::ValidateCriticalSection() const
{
	HRESULT l_Status = S_OK;

	if( nullptr == m_CriticalSectionPtr )
	{
		m_CriticalSectionPtr = SVCriticalSectionPtr{ new SVCriticalSection };

		if(nullptr == m_CriticalSectionPtr)
		{
			l_Status = SVMEE_INVALID_LOCK;
		}
	}

	return l_Status;
}

