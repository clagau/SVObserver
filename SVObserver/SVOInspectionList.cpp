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
		SVOInspectionObjPtr pObj = new SVOInspectionObj();
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
	BOOL bRet = FALSE;
	iterator Iter = FindInspectionPosition( InspectLabel );

	if (Iter != m_InspectionList.end())
	{
		SVOInspectionObjPtr pObj = *Iter;
		pObj->SetToolsetImage( Image );
		bRet = TRUE;
	}

	return bRet;
}

BOOL SVOInspectionList::SetColor( LPCTSTR InspectLabel, bool Color)
{
	BOOL bRet = FALSE;
	iterator Iter = FindInspectionPosition( InspectLabel );

	if (Iter != m_InspectionList.end())
	{
		SVOInspectionObjPtr pObj = *Iter;
		pObj->SetColor( Color );
		bRet = TRUE;
	}

	return bRet;
}

BOOL SVOInspectionList::SetNewDisableMethod( LPCTSTR InspectLabel,  LPCTSTR Disable)
{
	BOOL bRet = FALSE;
	iterator Iter = FindInspectionPosition( InspectLabel );

	if (Iter != m_InspectionList.end())
	{
		SVOInspectionObjPtr pObj = *Iter;
		pObj->SetNewDisableMethod( Disable );
		bRet = TRUE;
	}

	return bRet;
}

BOOL SVOInspectionList::SetShowAuxExtent( LPCTSTR InspectLabel, bool p_bShow)
{
	BOOL bRet = FALSE;
	iterator Iter = FindInspectionPosition( InspectLabel );

	if (Iter != m_InspectionList.end())
	{
		SVOInspectionObjPtr pObj = *Iter;
		pObj->SetShowAuxExtent( p_bShow );
		bRet = TRUE;
	}

	return bRet;
}

BOOL SVOInspectionList::SetEnableAuxiliaryExtent( LPCTSTR InspectLabel, long lEnable)
{
	BOOL bRet = FALSE;
	iterator Iter = FindInspectionPosition( InspectLabel );

	if (Iter != m_InspectionList.end())
	{
		SVOInspectionObjPtr pObj = *Iter;
		pObj->SetEnableAuxiliaryExtent( lEnable );
		bRet = TRUE;
	}

	return bRet;
}


BOOL SVOInspectionList::ReNameInspection( LPCTSTR InspectLabel,  LPCTSTR NewInspectName)
{
	BOOL bRet = FALSE;
	iterator Iter = FindInspectionPosition( InspectLabel );

	if (Iter != m_InspectionList.end())
	{
		SVOInspectionObjPtr pObj = *Iter;
		pObj->RenameInspection( NewInspectName );
		*Iter = pObj;
		bRet = TRUE;
	}


	return bRet;
}

BOOL SVOInspectionList::RemoveInspectionFromList( LPCTSTR InspectLabel )
{
	BOOL bRet = FALSE;
	iterator Iter = FindInspectionPosition( InspectLabel );

	if( Iter != m_InspectionList.end() )
	{
		m_InspectionList.erase(Iter);
		bRet = TRUE;
	}

	return bRet;
}

std::string SVOInspectionList::GetInspectionName( LPCTSTR InspectLabel )
{
	std::string Result;

	SVOInspectionObjPtr pInspectionObj = GetInspectionByName( InspectLabel );
	if( nullptr != pInspectionObj )
	{
		Result = pInspectionObj->GetInspectionName();
	}

	return Result;
}

SVOInspectionObjPtr SVOInspectionList::GetInspectionByName( LPCTSTR InspectLabel )
{
	SVOInspectionObjPtr pResult( nullptr );

	iterator Iter = FindInspectionPosition( InspectLabel );

	if( Iter != m_InspectionList.end() )
	{
		pResult =  *Iter;
	}

	return pResult;
}

std::string SVOInspectionList::GetInspectionLabel( LPCTSTR InspectName )
{
	std::string Result;

	iterator Iter = FindInspectionPositionFromName( InspectName );
	if( Iter != m_InspectionList.end() )
	{
		SVOInspectionObjPtr pObj = *Iter;
		Result = pObj->GetInspectionLabelName();
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

SVOInspectionList::iterator SVOInspectionList::FindInspectionPosition( LPCTSTR InspectLabel)
{
	iterator Iter = m_InspectionList.begin();
	bool Found = false;

	while( Iter != m_InspectionList.end() && !Found )
	{
		SVOInspectionObjPtr pInspectionObj = *Iter;

		if ( nullptr != pInspectionObj && InspectLabel == pInspectionObj->GetInspectionLabelName() )
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

SVOInspectionList::iterator SVOInspectionList::FindInspectionPositionFromName( LPCTSTR InspectName)
{
	iterator Iter = m_InspectionList.begin();
	bool Found = false;

	while( Iter != m_InspectionList.end() && !Found )
	{
		SVOInspectionObjPtr pInspectionObj = *Iter;

		if( nullptr != pInspectionObj && InspectName == pInspectionObj->GetInspectionName())
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

int SVOInspectionList::GetInspectionListCount() const
{
    return static_cast<int>(m_InspectionList.size());
}

void SVOInspectionList::ResetContent()
{
	m_InspectionList.clear();
}


