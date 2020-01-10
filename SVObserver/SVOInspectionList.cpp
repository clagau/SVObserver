//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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

BOOL SVOInspectionList::AddInspectionToList( LPCTSTR InspectLabel,  LPCTSTR InspectName, bool NewInspection)
{
	BOOL bRet = FALSE;
	if (!IsInspectionInList( InspectLabel,InspectName ))
	{
		SVOInspectionObjPtr pObj{ new SVOInspectionObj() };
		pObj->SetInspectionLabelName(InspectLabel );
		pObj->SetInspectionName( InspectName );
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

BOOL SVOInspectionList::SetToolsetImage( LPCTSTR InspectLabel,  LPCTSTR Image)
{
	bool bRet = false;
	SVOInspectionObjPtr pObj = GetInspectionByLabel( InspectLabel );

	if (nullptr != pObj)
	{
		pObj->SetToolsetImage( Image );
		bRet = true;
	}

	return bRet;
}

BOOL SVOInspectionList::SetColor( LPCTSTR InspectLabel, bool Color)
{
	bool bRet = false;
	SVOInspectionObjPtr pObj = GetInspectionByLabel( InspectLabel );

	if (nullptr != pObj)
	{
		pObj->SetColor( Color );
		bRet = true;
	}

	return bRet;
}

BOOL SVOInspectionList::SetNewDisableMethod( LPCTSTR InspectLabel,  LPCTSTR Disable)
{
	bool bRet = false;
	SVOInspectionObjPtr pObj = GetInspectionByLabel( InspectLabel );

	if (nullptr != pObj)
	{
		pObj->SetNewDisableMethod( Disable );
		bRet = true;
	}

	return bRet;
}

BOOL SVOInspectionList::SetShowAuxExtent( LPCTSTR InspectLabel, bool p_bShow)
{
	bool bRet = false;
	SVOInspectionObjPtr pObj = GetInspectionByLabel(InspectLabel);

	if (nullptr != pObj)
	{
		pObj->SetShowAuxExtent( p_bShow );
		bRet = true;
	}

	return bRet;
}

BOOL SVOInspectionList::SetEnableAuxiliaryExtent( LPCTSTR InspectLabel, long lEnable)
{
	bool bRet = false;
	SVOInspectionObjPtr pObj = GetInspectionByLabel(InspectLabel);

	if (nullptr != pObj)
	{
		pObj->SetEnableAuxiliaryExtent( lEnable );
		bRet = true;
	}

	return bRet;
}


BOOL SVOInspectionList::ReNameInspection( LPCTSTR InspectLabel,  LPCTSTR NewInspectName)
{
	bool bRet = false;
	SVOInspectionObjPtr pObj = GetInspectionByLabel(InspectLabel);

	if (nullptr != pObj)
	{
		pObj->RenameInspection( NewInspectName );
		bRet = true;
	}


	return bRet;
}

BOOL SVOInspectionList::RemoveInspectionFromList( LPCTSTR InspectLabel )
{
	bool bRet = false;
	auto& iter = std::find_if(m_InspectionList.begin(), m_InspectionList.end(), [&InspectLabel](auto& rEntry)
	{
		return (nullptr != rEntry && rEntry->GetInspectionLabelName() == InspectLabel);
	});
	
	if(iter != m_InspectionList.end() )
	{
		m_InspectionList.erase(iter);
		bRet = true;
	}

	return bRet;
}

std::string SVOInspectionList::GetInspectionName( LPCTSTR InspectLabel )
{
	std::string Result;

	SVOInspectionObjPtr pInspectionObj = GetInspectionByLabel( InspectLabel );
	if( nullptr != pInspectionObj )
	{
		Result = pInspectionObj->GetInspectionName();
	}

	return Result;
}

SVOInspectionObjPtr SVOInspectionList::GetInspectionByLabel( LPCTSTR InspectLabel )
{
	SVOInspectionObjPtr pResult(nullptr);
	const auto& iter = std::find_if(m_InspectionList.begin(), m_InspectionList.end(), [&InspectLabel](auto& rEntry)
	{
		return (nullptr != rEntry && rEntry->GetInspectionLabelName() == InspectLabel);
	});

	if (m_InspectionList.end() != iter)
	{
		return *iter;
	}
	return nullptr;
}

std::string SVOInspectionList::GetInspectionLabel( LPCTSTR InspectName )
{
	std::string Result;
	SVOInspectionObjPtr pResult(nullptr);
	auto& iter = std::find_if(m_InspectionList.begin(), m_InspectionList.end(), [&InspectName](auto& rEntry)
	{
		return (nullptr != rEntry && rEntry->GetInspectionName() == InspectName);
	});

	if (m_InspectionList.end() != iter)
	{
		Result = (*iter)->GetInspectionLabelName();
	}
	return Result;
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

BOOL SVOInspectionList::IsInspectionInList( LPCTSTR InspectLabel ) const
{
	const_iterator Iter( m_InspectionList.begin() );
	bool Found = false;

	while( Iter != m_InspectionList.end() && !Found )
	{
		const SVOInspectionObjPtr pInspectionObj = *Iter;
		if( nullptr != pInspectionObj )
		{
			//Check both InspectionLabelName & InspectionName - Can be different if an inspeciton has been renamed.
			if ( (InspectLabel == pInspectionObj->GetInspectionLabelName()) || (InspectLabel == pInspectionObj->GetInspectionName()) )
			{
				Found = true;
			}
		}

		++Iter;
	}
	return Found;
}

BOOL SVOInspectionList::IsInspectionNameInList( LPCTSTR InspectName) const
{
	const_iterator Iter = m_InspectionList.begin();
	BOOL bFound = false;

	while( Iter != m_InspectionList.end() && !bFound )
	{
		const SVOInspectionObjPtr pObj = *Iter;

		if (InspectName == pObj->GetInspectionName())
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
BOOL SVOInspectionList::IsInspectionInList( LPCTSTR InspectLabel, LPCTSTR InspectName) const
{
	const_iterator Iter = m_InspectionList.begin();
	BOOL bFound = FALSE;

	while( Iter != m_InspectionList.end() && !bFound )
	{
		const SVOInspectionObjPtr pInspectionObj = *Iter;

		if( nullptr != pInspectionObj && InspectLabel == pInspectionObj->GetInspectionLabelName())
		{
			if( InspectName == pInspectionObj->GetInspectionName() )
			{
				bFound = TRUE;
			}
		}
		else
		{
			++Iter;
		}
	}
	return bFound;
}

int SVOInspectionList::GetInspectionListCount() const
{
    return static_cast<int>(m_InspectionList.size());
}

void SVOInspectionList::ResetContent()
{
	m_InspectionList.clear();
}


