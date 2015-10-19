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

BOOL SVOPPQList::RemoveCameraFromPPQ(CString sPPQName, CString sCamera)
{
	BOOL Result( FALSE );

	SVOPPQObjPtr pPPQObj = GetPPQObjectByName(sPPQName);
	ASSERT(nullptr != pPPQObj);

	if( nullptr != pPPQObj )
	{
		Result =  pPPQObj->DetachCameraFromPPQ(sCamera);
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

BOOL SVOPPQList::RemoveInspectFromPPQ(CString sPPQName, CString sInspect)
{
	BOOL Result( FALSE );
	SVOPPQObjPtr pPPQObj = GetPPQObjectByName(sPPQName);
	ASSERT(nullptr != pPPQObj);

	if( nullptr != pPPQObj )
	{
		Result =  pPPQObj->DetachInspectionFromPPQ(sInspect);
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

BOOL SVOPPQList::RemoveTriggerFromPPQ(CString sPPQName)
{
	SVOPPQObjPtr pPPQObj = GetPPQObjectByName(sPPQName);
	ASSERT(nullptr != pPPQObj);

	if( nullptr != pPPQObj )
	{
		pPPQObj->DetachTriggerFromPPQ();
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOPPQList.cpp_v  $
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
 *    Rev 1.0   23 Apr 2013 13:10:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Nov 2010 16:25:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Sep 2008 16:34:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised GetPPQListCount to be const
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 13:17:18   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   06 Feb 2003 10:34:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added ASSERTs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Dec 2002 13:26:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  304
 * SCR Title:  Fix crash after deleting a PPQ and its attached inspection(s)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVOPPQList GetPPQObjectByName and GetPPQObjectByPosition methods to return NULL when a request is made for an object that does not exist.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:39:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

