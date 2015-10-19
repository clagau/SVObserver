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


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOInspectionList.cpp_v  $
 * 
 *    Rev 1.1   02 Oct 2013 06:48:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:04:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   09 Nov 2010 16:25:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Sep 2008 16:36:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised GetInspectionListCount to be const
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 Jan 2006 11:19:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to hide Aux Extent property when a gage tool is in the toolset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   24 Oct 2005 09:40:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changes to support adding a flag to the Inspection that will enable Auxiliary Extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Aug 2003 13:50:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  373
 * SCR Title:  Add a new disable method that doesn't copy forward all result values
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to the saving/loading/editing of configurations to add a new property for the inspection's disable method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 13:05:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:38:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
    
