//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOInspectionList
//* .File Name       : $Workfile:   SVOInspectionList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 06:48:22  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOInspectionList.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVOInspectionList::SVOInspectionList()
{

}

SVOInspectionList::~SVOInspectionList()
{

}

bool SVOInspectionList::AddInspectionToList(LPCTSTR InspectionName, bool NewInspection)
{
	bool bRet = false;
	if (!IsInspectionInList(InspectionName))
	{
		SVOInspectionObjPtr pObj{ new SVOInspectionObj() };
		pObj->SetOriginalName(InspectionName);
		pObj->SetInspectionName(InspectionName);
		pObj->SetToolsetImage( _T("") );
		pObj->SetNewDisableMethod( _T("Method 1") );
		pObj->SetEnableAuxiliaryExtent( 0 );
		if( NewInspection )
		{
			pObj->SetNewInspection();
		}
		m_InspectionList.insert(m_InspectionList.end(), pObj);
	}
	return bRet;
}

bool SVOInspectionList::SetToolsetImage( LPCTSTR InspectionName,  LPCTSTR Image)
{
	bool bRet = false;
	SVOInspectionObjPtr pObj = GetInspectionByName(InspectionName);

	if (nullptr != pObj)
	{
		pObj->SetToolsetImage( Image );
		bRet = true;
	}

	return bRet;
}

bool SVOInspectionList::SetColor( LPCTSTR InspectionName, bool Color)
{
	bool bRet = false;
	SVOInspectionObjPtr pObj = GetInspectionByName(InspectionName);

	if (nullptr != pObj)
	{
		pObj->SetColor( Color );
		bRet = true;
	}

	return bRet;
}

bool SVOInspectionList::SetNewDisableMethod( LPCTSTR InspectionName,  LPCTSTR Disable)
{
	bool bRet = false;
	SVOInspectionObjPtr pObj = GetInspectionByName(InspectionName);;

	if (nullptr != pObj)
	{
		pObj->SetNewDisableMethod( Disable );
		bRet = true;
	}

	return bRet;
}

bool SVOInspectionList::SetShowAuxExtent( LPCTSTR InspectionName, bool p_bShow)
{
	bool bRet = false;
	SVOInspectionObjPtr pObj = GetInspectionByName(InspectionName);

	if (nullptr != pObj)
	{
		pObj->SetShowAuxExtent( p_bShow );
		bRet = true;
	}

	return bRet;
}

bool SVOInspectionList::SetEnableAuxiliaryExtent( LPCTSTR InspectionName, long lEnable)
{
	bool bRet = false;
	SVOInspectionObjPtr pObj = GetInspectionByName(InspectionName);

	if (nullptr != pObj)
	{
		pObj->SetEnableAuxiliaryExtent( lEnable );
		bRet = true;
	}

	return bRet;
}

bool SVOInspectionList::SetObjectIdIndex(LPCTSTR InspectionName, DWORD objectIdIndex)
{
	SVOInspectionObjPtr pObj = GetInspectionByName(InspectionName);

	if (nullptr != pObj)
	{
		pObj->SetObjectIdIndex(static_cast<long> (objectIdIndex));
		return true;
	}

	return false;
}

bool SVOInspectionList::RenameInspection( LPCTSTR InspectionName,  LPCTSTR NewInspectionName)
{
	bool bRet = false;
	SVOInspectionObjPtr pObj = GetInspectionByName(InspectionName);

	if (nullptr != pObj)
	{
		pObj->RenameInspection(NewInspectionName);
		bRet = true;
	}


	return bRet;
}

bool SVOInspectionList::RemoveInspectionFromList(LPCTSTR InspectionName)
{
	bool bRet = false;
	auto iter = std::find_if(m_InspectionList.begin(), m_InspectionList.end(), [&InspectionName](auto& rEntry)
	{
		return (nullptr != rEntry && rEntry->GetInspectionName() == InspectionName);
	});
	
	if(iter != m_InspectionList.end() )
	{
		m_InspectionList.erase(iter);
		bRet = true;
	}

	return bRet;
}

SVOInspectionObjPtr SVOInspectionList::GetInspectionByName( LPCTSTR InspectionName)
{
	SVOInspectionObjPtr pResult(nullptr);
	const auto& iter = std::find_if(m_InspectionList.begin(), m_InspectionList.end(), [&InspectionName](auto& rEntry)
	{
		return (nullptr != rEntry && rEntry->GetInspectionName() == InspectionName);
	});

	if (m_InspectionList.end() != iter)
	{
		return *iter;
	}
	return nullptr;
}

SVOInspectionObjPtr SVOInspectionList::GetInspectionByOriginalName(LPCTSTR InspectionName)
{
	SVOInspectionObjPtr pResult(nullptr);
	const auto& iter = std::find_if(m_InspectionList.begin(), m_InspectionList.end(), [&InspectionName](auto& rEntry)
		{
			return (nullptr != rEntry && rEntry->GetOriginalName() == InspectionName);
		});

	if (m_InspectionList.end() != iter)
	{
		return *iter;
	}
	return nullptr;
}

SVOInspectionObjPtr SVOInspectionList::GetInspectionByPosition( int iPos )
{
	SVOInspectionObjPtr pResult( nullptr );

	if (0 <= iPos && iPos < static_cast<int>(m_InspectionList.size()))
	{
		iterator Iter(m_InspectionList.begin());
		std::advance(Iter, iPos);
		pResult = *Iter;
	}

	return pResult;
}

bool SVOInspectionList::IsInspectionInList( LPCTSTR InspectionName ) const
{
	const_iterator Iter( m_InspectionList.begin() );
	bool Found = false;

	while( Iter != m_InspectionList.end() && !Found )
	{
		const SVOInspectionObjPtr pInspectionObj = *Iter;
		if( nullptr != pInspectionObj )
		{
			if (InspectionName == pInspectionObj->GetInspectionName())
			{
				Found = true;
			}
		}

		++Iter;
	}
	return Found;
}

bool SVOInspectionList::IsOriginalInspectionInList(LPCTSTR InspectionName) const
{
	const_iterator Iter(m_InspectionList.begin());
	bool Found = false;

	while (Iter != m_InspectionList.end() && !Found)
	{
		const SVOInspectionObjPtr pInspectionObj = *Iter;
		if (nullptr != pInspectionObj)
		{
			if (InspectionName == pInspectionObj->GetOriginalName())
			{
				Found = true;
			}
		}

		++Iter;
	}
	return Found;
}

int SVOInspectionList::GetInspectionListCount() const
{
    return static_cast<int>(m_InspectionList.size());
}

void SVOInspectionList::ResetContent()
{
	m_InspectionList.clear();
}


