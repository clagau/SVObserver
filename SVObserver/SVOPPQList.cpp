//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOPPQList
//* .File Name       : $Workfile:   SVOPPQList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:10:24  $
//******************************************************************************

#include "stdafx.h"
#include "SVOPPQList.h"
#include "SVOPPQObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CSVOPPQList::CSVOPPQList()
{

}

CSVOPPQList::~CSVOPPQList()
{

}

BOOL CSVOPPQList::AddPPQToList(CString sPPQName)
{
	BOOL bRet = FALSE;

	if (!IsPPQInList(sPPQName))
	{
		CSVOPPQObj *pObj = new CSVOPPQObj();
		pObj->SetPPQName(sPPQName);
		m_PPQList.AddTail(pObj);
		bRet = TRUE;
	}

	return bRet;
}

BOOL CSVOPPQList::RemovePPQFromList(CString sPPQName)
{
	BOOL bRet = FALSE;
	iterator pos = FindPPQPosition(sPPQName);

	if (pos != m_PPQList.end())
	{
		bRet = TRUE;
		CSVOPPQObj *pObj = m_PPQList.GetAt(pos);
		m_PPQList.RemoveAt(pos);
		delete pObj;
	}

	return bRet;
}

BOOL CSVOPPQList::AttachCameraToPPQ(CString sPPQName, CString sCamera)
{
	CSVOPPQObj *pObj = GetPPQObjectByName(sPPQName);
	ASSERT(pObj);

	return pObj->AttachCameraToPPQ(sCamera);
}

BOOL CSVOPPQList::RemoveCameraFromPPQ(CString sPPQName, CString sCamera)
{
	CSVOPPQObj *pObj = GetPPQObjectByName(sPPQName);
	ASSERT(pObj);

	return pObj->DetachCameraFromPPQ(sCamera);
}


BOOL CSVOPPQList::AttachInspectToPPQ(CString sPPQName, CString sInspect)
{
	CSVOPPQObj *pObj = GetPPQObjectByName(sPPQName);
	ASSERT(pObj);

	return pObj->AttachInspectionToPPQ(sInspect);
}

BOOL CSVOPPQList::RemoveInspectFromPPQ(CString sPPQName, CString sInspect)
{
	CSVOPPQObj *pObj = GetPPQObjectByName(sPPQName);
	ASSERT(pObj);

	return pObj->DetachInspectionFromPPQ(sInspect);
}

BOOL CSVOPPQList::AttachTriggerToPPQ(CString sPPQName, CString sTrigger)
{
	CSVOPPQObj *pObj = GetPPQObjectByName(sPPQName);
	ASSERT(pObj);

	pObj->AttachTriggerToPPQ(sTrigger);

	return TRUE;
}

BOOL CSVOPPQList::RemoveTriggerFromPPQ(CString sPPQName)
{
	CSVOPPQObj *pObj = GetPPQObjectByName(sPPQName);
	ASSERT(pObj);

	pObj->DetachTriggerFromPPQ();

	return TRUE;
}




BOOL CSVOPPQList::IsPPQInList(CString sPPQName)
{
	iterator pos( m_PPQList.begin() );
	BOOL bFound = FALSE;

	while( pos != m_PPQList.end() && (!bFound) )
	{
		CSVOPPQObj* pObj = m_PPQList.GetAt(pos);

		if (pObj->GetPPQName() == sPPQName)
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


CSVOPPQList::iterator CSVOPPQList::FindPPQPosition(CString sPPQName)
{
	iterator pos( m_PPQList.begin() );
	BOOL bFound = FALSE;

	while( pos != m_PPQList.end() && (!bFound) )
	{
		CSVOPPQObj* pObj = m_PPQList.GetAt(pos);

		if (pObj->GetPPQName() == sPPQName)
		{
			bFound = TRUE;
		}
		else
		{
			++pos;
		}
	}

	return pos;
}

CSVOPPQObj *CSVOPPQList::GetPPQObjectByName(CString sPPQName)
{
	iterator pos = FindPPQPosition(sPPQName);

	if (pos != m_PPQList.end())
		return m_PPQList.GetAt(pos);
	else
		return NULL;
}

CSVOPPQObj *CSVOPPQList::GetPPQObjectByPosition(int iPos)
{
	iterator pos = m_PPQList.FindIndex(iPos);

	if (pos != m_PPQList.end())
		return m_PPQList.GetAt(pos);
	else
		return NULL;
}

int CSVOPPQList::GetPPQListCount() const
{
	return m_PPQList.GetCount();
}

void CSVOPPQList::ResetContent()
{
	iterator pos( m_PPQList.begin() );

	while( pos != m_PPQList.end() )
	{
		CSVOPPQObj* pObj = m_PPQList.GetAt(pos);
		pos = m_PPQList.erase(pos);
		delete pObj;
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOPPQList.cpp_v  $
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

