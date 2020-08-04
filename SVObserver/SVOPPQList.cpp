//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOPPQList
//* .File Name       : $Workfile:   SVOPPQList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 06:48:22  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOPPQList.h"
#include "SVOPPQObj.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVOPPQList::SVOPPQList()
{

}

SVOPPQList::~SVOPPQList()
{

}

bool SVOPPQList::AddPPQToList(LPCTSTR PPQName)
{
	bool bRet = false;

	if (!IsPPQInList( PPQName ))
	{
		SVOPPQObjPtr pPPQObj{ new SVOPPQObj() };
		pPPQObj->SetPPQName( PPQName );
		m_PPQList.insert(m_PPQList.end(), pPPQObj);
		bRet = true;
	}

	return bRet;
}

bool SVOPPQList::RemovePPQFromList(LPCTSTR PPQName)
{
	bool bRet = false;
	iterator Iter = FindPPQPosition( PPQName );

	if (Iter != m_PPQList.end())
	{
		bRet = true;
		m_PPQList.erase(Iter);
	}

	return bRet;
}

bool SVOPPQList::AttachCameraToPPQ(LPCTSTR PPQName, LPCTSTR Camera)
{
	bool Result( false );
	SVOPPQObjPtr pPPQObj = GetPPQObjectByName( PPQName );
	assert(nullptr != pPPQObj);

	if( nullptr != pPPQObj )
	{
		Result = pPPQObj->AttachCameraToPPQ( Camera );
	}

	return Result;
}

bool SVOPPQList::AttachInspectToPPQ(LPCTSTR PPQName, LPCTSTR Inspect)
{
	bool Result( false );
	SVOPPQObjPtr pPPQObj = GetPPQObjectByName( PPQName );
	assert(nullptr != pPPQObj);

	if( nullptr != pPPQObj )
	{
		Result = pPPQObj->AttachInspectionToPPQ( Inspect );
	}

	return Result;
}

void SVOPPQList::AttachTriggerToPPQ(LPCTSTR PPQName, LPCTSTR Trigger)
{
	SVOPPQObjPtr pPPQObj = GetPPQObjectByName( PPQName );
	assert(nullptr != pPPQObj);

	if( nullptr != pPPQObj )
	{
		pPPQObj->AttachTriggerToPPQ( Trigger );
	}
}

bool SVOPPQList::IsPPQInList(LPCTSTR PPQName) const
{
	const_iterator Iter( m_PPQList.begin() );
	bool bFound = false;

	while( Iter != m_PPQList.end() && (!bFound) )
	{
		const SVOPPQObjPtr pPPQObj = *Iter;

		if( nullptr != pPPQObj && PPQName == pPPQObj->GetPPQName() )
		{
			bFound = true;
		}
		else
		{
			++Iter;
		}
	}

	return bFound;
}


SVOPPQList::iterator SVOPPQList::FindPPQPosition(LPCTSTR PPQName)
{
	iterator Iter( m_PPQList.begin() );
	bool Found = false;

	while( Iter != m_PPQList.end() && (!Found) )
	{
		SVOPPQObjPtr pPPQObj = *Iter;

		if( nullptr != pPPQObj && PPQName == pPPQObj->GetPPQName() )
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

SVOPPQObjPtr SVOPPQList::GetPPQObjectByName(LPCTSTR PPQName)
{
	SVOPPQObjPtr pReturnObj( nullptr );

	iterator Iter = FindPPQPosition( PPQName );

	if (Iter != m_PPQList.end())
	{
		pReturnObj = *Iter;
	}
	return pReturnObj;
}

SVOPPQObjPtr SVOPPQList::GetPPQObjectByPosition(int iPos)
{
	SVOPPQObjPtr pResult( nullptr );

	if (0 <= iPos && iPos < static_cast<int>(m_PPQList.size()))
	{
		iterator Iter(m_PPQList.begin());
		std::advance(Iter, iPos);
		pResult = *Iter;
	}

	return pResult;
}

const SVOPPQObjPtr SVOPPQList::GetPPQObjectByPosition(int iPos) const
{
	SVOPPQObjPtr pResult(nullptr);

	if (0 <= iPos && iPos < static_cast<int>(m_PPQList.size()))
	{
		const_iterator Iter(m_PPQList.begin());
		std::advance(Iter, iPos);
		pResult = *Iter;
	}

	return pResult;
}

int SVOPPQList::GetPPQListCount() const
{
	return static_cast<int>(m_PPQList.size());
}

void SVOPPQList::ResetContent()
{
	m_PPQList.clear();
}

