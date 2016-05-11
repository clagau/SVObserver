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

BOOL SVOPPQList::AddPPQToList(CString sPPQName)
{
	BOOL bRet = FALSE;

	if (!IsPPQInList(sPPQName))
	{
		SVOPPQObjPtr pPPQObj = new SVOPPQObj();
		pPPQObj->SetPPQName(sPPQName);
		m_PPQList.AddTail(pPPQObj);
		bRet = TRUE;
	}

	return bRet;
}

BOOL SVOPPQList::RemovePPQFromList(CString sPPQName)
{
	BOOL bRet = FALSE;
	iterator pos = FindPPQPosition(sPPQName);

	if (pos != m_PPQList.end())
	{
		bRet = TRUE;
		m_PPQList.RemoveAt(pos);
	}

	return bRet;
}

BOOL SVOPPQList::AttachCameraToPPQ(CString sPPQName, CString sCamera)
{
	BOOL Result( FALSE );
	SVOPPQObjPtr pPPQObj = GetPPQObjectByName(sPPQName);
	ASSERT(nullptr != pPPQObj);

	if( nullptr != pPPQObj )
	{
		Result = pPPQObj->AttachCameraToPPQ(sCamera);
	}

	return Result;
}

BOOL SVOPPQList::AttachInspectToPPQ(CString sPPQName, CString sInspect)
{
	BOOL Result( FALSE );
	SVOPPQObjPtr pPPQObj = GetPPQObjectByName(sPPQName);
	ASSERT(nullptr != pPPQObj);

	if( nullptr != pPPQObj )
	{
		Result = pPPQObj->AttachInspectionToPPQ(sInspect);
	}

	return Result;
}

BOOL SVOPPQList::AttachTriggerToPPQ(CString sPPQName, CString sTrigger)
{
	SVOPPQObjPtr pPPQObj = GetPPQObjectByName(sPPQName);
	ASSERT(nullptr != pPPQObj);

	if( nullptr != pPPQObj )
	{
		pPPQObj->AttachTriggerToPPQ(sTrigger);
	}

	return TRUE;
}

BOOL SVOPPQList::IsPPQInList(CString sPPQName) const
{
	const_iterator pos( m_PPQList.begin() );
	BOOL bFound = FALSE;

	while( pos != m_PPQList.end() && (!bFound) )
	{
		const SVOPPQObjPtr pPPQObj = m_PPQList.GetAt(pos);

		if( nullptr != pPPQObj && sPPQName == pPPQObj->GetPPQName() )
		{
			bFound = TRUE;
		}
		else
		{
			++pos;
		}
	}

	return bFound;
}


SVOPPQList::iterator SVOPPQList::FindPPQPosition(CString sPPQName)
{
	iterator pos( m_PPQList.begin() );
	bool Found = false;

	while( pos != m_PPQList.end() && (!Found) )
	{
		SVOPPQObjPtr pPPQObj = m_PPQList.GetAt(pos);

		if( nullptr != pPPQObj && sPPQName == pPPQObj->GetPPQName() )
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

SVOPPQObjPtr SVOPPQList::GetPPQObjectByName(CString sPPQName)
{
	SVOPPQObjPtr pReturnObj( nullptr );

	iterator pos = FindPPQPosition(sPPQName);

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

