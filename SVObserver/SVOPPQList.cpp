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
		SVOPPQObjPtr pPPQObj = new SVOPPQObj();
		pPPQObj->SetPPQName( PPQName );
		m_PPQList.AddTail(pPPQObj);
		bRet = true;
	}

	return bRet;
}

bool SVOPPQList::RemovePPQFromList(LPCTSTR PPQName)
{
	bool bRet = false;
	iterator pos = FindPPQPosition( PPQName );

	if (pos != m_PPQList.end())
	{
		bRet = true;
		m_PPQList.RemoveAt(pos);
	}

	return bRet;
}

bool SVOPPQList::AttachCameraToPPQ(LPCTSTR PPQName, LPCTSTR Camera)
{
	bool Result( false );
	SVOPPQObjPtr pPPQObj = GetPPQObjectByName( PPQName );
	ASSERT(nullptr != pPPQObj);

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
	ASSERT(nullptr != pPPQObj);

	if( nullptr != pPPQObj )
	{
		Result = pPPQObj->AttachInspectionToPPQ( Inspect );
	}

	return Result;
}

void SVOPPQList::AttachTriggerToPPQ(LPCTSTR PPQName, LPCTSTR Trigger)
{
	SVOPPQObjPtr pPPQObj = GetPPQObjectByName( PPQName );
	ASSERT(nullptr != pPPQObj);

	if( nullptr != pPPQObj )
	{
		pPPQObj->AttachTriggerToPPQ( Trigger );
	}
}

bool SVOPPQList::IsPPQInList(LPCTSTR PPQName) const
{
	const_iterator pos( m_PPQList.begin() );
	bool bFound = false;

	while( pos != m_PPQList.end() && (!bFound) )
	{
		const SVOPPQObjPtr pPPQObj = m_PPQList.GetAt(pos);

		if( nullptr != pPPQObj && PPQName == pPPQObj->GetPPQName() )
		{
			bFound = true;
		}
		else
		{
			++pos;
		}
	}

	return bFound;
}


SVOPPQList::iterator SVOPPQList::FindPPQPosition(LPCTSTR PPQName)
{
	iterator pos( m_PPQList.begin() );
	bool Found = false;

	while( pos != m_PPQList.end() && (!Found) )
	{
		SVOPPQObjPtr pPPQObj = m_PPQList.GetAt(pos);

		if( nullptr != pPPQObj && PPQName == pPPQObj->GetPPQName() )
		{
			Found = true;
		}
		else
		{
			++pos;
		}
	}

	return pos;
}

SVOPPQObjPtr SVOPPQList::GetPPQObjectByName(LPCTSTR PPQName)
{
	SVOPPQObjPtr pReturnObj( nullptr );

	iterator pos = FindPPQPosition( PPQName );

	if (pos != m_PPQList.end())
	{
		pReturnObj =  m_PPQList.GetAt(pos);
	}
	return pReturnObj;
}

SVOPPQObjPtr SVOPPQList::GetPPQObjectByPosition(int iPos)
{
	SVOPPQObjPtr pResult( nullptr );

	iterator pos = m_PPQList.FindIndex(iPos);

	if (pos != m_PPQList.end())
	{
		pResult =  m_PPQList.GetAt(pos);
	}
	return pResult;
}

const SVOPPQObjPtr SVOPPQList::GetPPQObjectByPosition(int iPos) const
{
	SVOPPQObjPtr pResult( nullptr );

	const_iterator pos = m_PPQList.FindIndex(iPos);

	if (pos != m_PPQList.end())
	{
		pResult =  m_PPQList.GetAt(pos);
	}
	return pResult;
}

int SVOPPQList::GetPPQListCount() const
{
	return static_cast<int>(m_PPQList.GetCount());
}

void SVOPPQList::ResetContent()
{
	m_PPQList.RemoveAll();
}

