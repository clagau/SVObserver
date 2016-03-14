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

BOOL SVOInspectionList::AddInspectionToList(CString sInspectLabel, CString sInspectName, bool NewInspection)
{
	BOOL bRet = FALSE;
	if (!IsInspectionInList(sInspectLabel,sInspectName))
	{
		SVOInspectionObjPtr pObj = new SVOInspectionObj();
		pObj->SetInspectionLabelName(sInspectLabel);
		pObj->SetInspectionName(sInspectName);
		pObj->SetToolsetImage( _T("") );
		pObj->SetNewDisableMethod( _T("Method 1") );
		pObj->SetEnableAuxiliaryExtent( 0 );
		if( NewInspection )
		{
			pObj->SetNewInspection();
		}
		m_InspectionList.AddTail(pObj);
	}
	return bRet;
}

BOOL SVOInspectionList::SetToolsetImage(CString sInspectLabel, CString sImage)
{
	BOOL bRet = FALSE;
	iterator pos = FindInspectionPosition(sInspectLabel);

	if (pos != m_InspectionList.end())
	{
		SVOInspectionObjPtr pObj = m_InspectionList.GetAt(pos);
		pObj->SetToolsetImage( sImage );
		bRet = TRUE;
	}

	return bRet;
}

BOOL SVOInspectionList::SetColor(CString sInspectLabel, bool Color)
{
	BOOL bRet = FALSE;
	iterator pos = FindInspectionPosition(sInspectLabel);

	if (pos != m_InspectionList.end())
	{
		SVOInspectionObjPtr pObj = m_InspectionList.GetAt(pos);
		pObj->SetColor( Color );
		bRet = TRUE;
	}

	return bRet;
}

BOOL SVOInspectionList::SetNewDisableMethod(CString sInspectLabel, CString sDisable)
{
	BOOL bRet = FALSE;
	iterator pos = FindInspectionPosition(sInspectLabel);

	if (pos != m_InspectionList.end())
	{
		SVOInspectionObjPtr pObj = m_InspectionList.GetAt(pos);
		pObj->SetNewDisableMethod( sDisable );
		bRet = TRUE;
	}

	return bRet;
}

BOOL SVOInspectionList::SetShowAuxExtent(CString sInspectLabel, bool p_bShow)
{
	BOOL bRet = FALSE;
	iterator pos = FindInspectionPosition(sInspectLabel);

	if (pos != m_InspectionList.end())
	{
		SVOInspectionObjPtr pObj = m_InspectionList.GetAt(pos);
		pObj->SetShowAuxExtent( p_bShow );
		bRet = TRUE;
	}

	return bRet;
}

BOOL SVOInspectionList::SetEnableAuxiliaryExtent(CString sInspectLabel, long lEnable)
{
	BOOL bRet = FALSE;
	iterator pos = FindInspectionPosition(sInspectLabel);

	if (pos != m_InspectionList.end())
	{
		SVOInspectionObjPtr pObj = m_InspectionList.GetAt(pos);
		pObj->SetEnableAuxiliaryExtent( lEnable );
		bRet = TRUE;
	}

	return bRet;
}


BOOL SVOInspectionList::ReNameInspection(CString sInspectLabel, CString sNewInspectName)
{
	BOOL bRet = FALSE;
	iterator pos = FindInspectionPosition(sInspectLabel);

	if (pos != m_InspectionList.end())
	{
		SVOInspectionObjPtr pObj = m_InspectionList.GetAt(pos);
		pObj->RenameInspection(sNewInspectName);
		m_InspectionList.SetAt(pos,pObj);
		bRet = TRUE;
	}


	return bRet;
}

BOOL SVOInspectionList::RemoveInspectionFromList(CString sInspectLabel)
{
	BOOL bRet = FALSE;
	iterator pos = FindInspectionPosition(sInspectLabel);

	if( pos != m_InspectionList.end() )
	{
		m_InspectionList.RemoveAt(pos);
		bRet = TRUE;
	}

	return bRet;
}

CString SVOInspectionList::GetInspectionName(CString sInspectLabel)
{
	CString InspectionName;

	SVOInspectionObjPtr pInspectionObj = GetInspectionByName(sInspectLabel);
	if( nullptr != pInspectionObj )
	{
		InspectionName = pInspectionObj->GetInspectionName();
	}

	return InspectionName;
}

SVOInspectionObjPtr SVOInspectionList::GetInspectionByName(CString sInspectLabel)
{
	SVOInspectionObjPtr pResult( nullptr );

	iterator pos = FindInspectionPosition(sInspectLabel);

	if( pos != m_InspectionList.end() )
	{
		pResult =  m_InspectionList.GetAt(pos);
	}

	return pResult;
}

CString SVOInspectionList::GetInspectionLabel(CString sInspectName)
{
	CString InspectionLabelName;

	iterator pos = FindInspectionPositionFromName(sInspectName);
	if( pos != m_InspectionList.end() )
	{
		SVOInspectionObjPtr pObj = m_InspectionList.GetAt(pos);
		InspectionLabelName = pObj->GetInspectionLabelName();
	}

	return InspectionLabelName;
}

SVOInspectionObjPtr SVOInspectionList::GetInspectionByPosition(int iPos)
{
	SVOInspectionObjPtr pResult( nullptr );

	iterator pos = m_InspectionList.FindIndex(iPos);

	if (pos != m_InspectionList.end())
	{
		pResult =  m_InspectionList.GetAt(pos);
	}

	return pResult;
}

BOOL SVOInspectionList::IsInspectionInList(CString sInspectLabel) const
{
	const_iterator pos( m_InspectionList.begin() );
	bool Found = false;

	while( pos != m_InspectionList.end() && !Found )
	{
		const SVOInspectionObjPtr pInspectionObj = m_InspectionList.GetAt(pos);
		if( nullptr != pInspectionObj )
		{
			//Check both InspectionLabelName & InspectionName - Can be different if an inspeciton has been renamed.
			if ( (sInspectLabel == pInspectionObj->GetInspectionLabelName()) || (sInspectLabel == pInspectionObj->GetInspectionName()) )
			{
				Found = true;
			}
		}

		++pos;
	}
	return Found;
}

BOOL SVOInspectionList::IsInspectionNameInList(CString sInspectName) const
{
	const_iterator pos = m_InspectionList.begin();
	BOOL bFound = FALSE;

	while( pos != m_InspectionList.end() && !bFound )
	{
		const SVOInspectionObjPtr pObj = m_InspectionList.GetAt(pos);

		if (sInspectName == pObj->GetInspectionName())
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
BOOL SVOInspectionList::IsInspectionInList(CString sInspectLabel,CString sInspectName) const
{
	const_iterator pos = m_InspectionList.begin();
	BOOL bFound = FALSE;

	while( pos != m_InspectionList.end() && !bFound )
	{
		const SVOInspectionObjPtr pInspectionObj = m_InspectionList.GetAt(pos);

		if( nullptr != pInspectionObj && sInspectLabel == pInspectionObj->GetInspectionLabelName())
		{
			if( sInspectName == pInspectionObj->GetInspectionName() )
			{
				bFound = TRUE;
			}
		}
		else
		{
			++pos;
		}
	}
	return bFound;
}

SVOInspectionList::iterator SVOInspectionList::FindInspectionPosition(CString sInspectLabel)
{
	iterator pos = m_InspectionList.begin();
	bool Found = false;

	while( pos != m_InspectionList.end() && !Found )
	{
		SVOInspectionObjPtr pInspectionObj = m_InspectionList.GetAt(pos);

		if ( nullptr != pInspectionObj && sInspectLabel == pInspectionObj->GetInspectionLabelName() )
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

SVOInspectionList::iterator SVOInspectionList::FindInspectionPositionFromName(CString sInspectName)
{
	iterator pos = m_InspectionList.begin();
	bool Found = false;

	while( pos != m_InspectionList.end() && !Found )
	{
		SVOInspectionObjPtr pInspectionObj = m_InspectionList.GetAt(pos);

		if( nullptr != pInspectionObj && sInspectName == pInspectionObj->GetInspectionName())
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

int SVOInspectionList::GetInspectionListCount() const
{
    return static_cast<int>(m_InspectionList.GetCount());
}

void SVOInspectionList::ResetContent()
{
	m_InspectionList.RemoveAll();
}


