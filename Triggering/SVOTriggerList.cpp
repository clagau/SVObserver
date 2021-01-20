//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOTriggerList
//* .File Name       : $Workfile:   SVOTriggerList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 07:01:52  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <set>
#include "SVOTriggerList.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvTrig
{
	SVOTriggerList::SVOTriggerList()
	{
	}

	SVOTriggerList::~SVOTriggerList()
	{
	}

	bool SVOTriggerList::AddTriggerToList(const std::string& sTriggerName, int iDigNumber)
	{
		bool bRet = false;

		if (!IsTriggerInList(sTriggerName))
		{
			SVOTriggerObjPtr pTriggerObj{ new SVOTriggerObj(sTriggerName, iDigNumber) };
			m_TriggerList.insert(m_TriggerList.end(), pTriggerObj);
			bRet = true;
		}
		return bRet;
	}


	bool SVOTriggerList::RemoveTriggerFromList(const std::string& sTriggerName)
	{
		bool bRet = false;
		iterator Iter = FindTriggerPosition(sTriggerName);
		if (Iter != m_TriggerList.end())
		{
			m_TriggerList.erase(Iter);
			bRet = true;
		}

		return bRet;
	}

	int SVOTriggerList::GetTriggerListCount() const
	{
		return static_cast<int>(m_TriggerList.size());
	}

	SVOTriggerObjPtr SVOTriggerList::GetTriggerObjectByName(const std::string& sTriggerName)
	{
		iterator Iter = FindTriggerPosition(sTriggerName);

		return *Iter;
	}

	SVOTriggerObjPtr SVOTriggerList::GetTriggerObjectByPosition(int iPos)
	{
		SVOTriggerObjPtr pResult;

		if (0 <= iPos && iPos < static_cast<int>(m_TriggerList.size()))
		{
			iterator Iter(m_TriggerList.begin());
			std::advance(Iter, iPos);
			pResult = *Iter;
		}

		return pResult;
	}

	int SVOTriggerList::GetNextTriggerID() const
	{
		typedef std::set<int> IDList;
		IDList idList;
	
		for (const_iterator it = m_TriggerList.begin();it != m_TriggerList.end();++it)
		{
			const SVOTriggerObjPtr pTriggerObj = (*it);
			if( nullptr != pTriggerObj )
			{
				idList.insert(pTriggerObj->GetTriggerDigNumber());
			}
		}
		int prevNo = -1;
		for (IDList::const_iterator it = idList.begin();it != idList.end();++it)
		{
			int number = (*it); 
			int delta = number - prevNo;
			if (delta > 1)
			{
				break;
			}
			prevNo = number;
		}
		return ++prevNo;
	}

	bool SVOTriggerList::IsTriggerInList(const std::string& sTriggerName) const
	{
		bool bFound = false;

		for (const_iterator it = m_TriggerList.begin();it != m_TriggerList.end() && !bFound;++it)
		{
			const SVOTriggerObjPtr pTriggerObj = (*it);

			if ( nullptr != pTriggerObj && std::string( pTriggerObj->GetTriggerDisplayName() ) == sTriggerName )
			{
				bFound = true;
			}
		}
		return bFound;
	}

	SVOTriggerList::iterator SVOTriggerList::FindTriggerPosition(const std::string& sTriggerName)
	{
		iterator Iter( m_TriggerList.begin() );
		bool Found = false;

		while( Iter != m_TriggerList.end() && !Found )
		{
			SVOTriggerObjPtr pTriggerObj =*Iter;

			if( nullptr != pTriggerObj && std::string( pTriggerObj->GetTriggerDisplayName() ) == sTriggerName )
			{
				Found = true;
			}
			else
			{
				++Iter;
			}
		}

		return Iter;
	}

	void SVOTriggerList::ResetContent()
	{
		m_TriggerList.clear();
	}
} //namespace SvTrig
