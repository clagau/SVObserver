//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOTriggerList
//* .File Name       : $Workfile:   SVOTriggerList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:14:30  $
//******************************************************************************

#include "stdafx.h"
#include <set>
#include "SVUtilityLibrary/SVString.h"
#include "SVOTriggerList.h"
#include "SVOTriggerObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CSVOTriggerList::CSVOTriggerList()
{
}

CSVOTriggerList::~CSVOTriggerList()
{
}

BOOL CSVOTriggerList::AddTriggerToList(const SVString& sTriggerName, int iDigNumber)
{
    BOOL bRet = FALSE;

    if (!IsTriggerInList(sTriggerName))
    {
        CSVOTriggerObj *pObj = new CSVOTriggerObj(sTriggerName, iDigNumber);
        m_TriggerList.AddTail(pObj);
        bRet = TRUE;
    }
    return bRet;
}


BOOL CSVOTriggerList::RemoveTriggerFromList(const SVString& sTriggerName)
{
    BOOL bRet = FALSE;
	iterator pos = FindTriggerPosition(sTriggerName);
	if (pos != m_TriggerList.end())
    {
        CSVOTriggerObj *pObj = m_TriggerList.GetAt(pos);
        m_TriggerList.RemoveAt(pos);
        delete pObj;
        bRet = TRUE;
    }

    return bRet;
}

int CSVOTriggerList::GetTriggerListCount() const
{
    return m_TriggerList.GetCount();
}

CSVOTriggerObj *CSVOTriggerList::GetTriggerObjectByName(const SVString& sTriggerName)
{
	iterator pos = FindTriggerPosition(sTriggerName);

	return m_TriggerList.GetAt(pos);
}

CSVOTriggerObj *CSVOTriggerList::GetTriggerObjectByPosition(int iPos)
{
	iterator pos = m_TriggerList.FindIndex(iPos);

	return m_TriggerList.GetAt(pos);
}

int CSVOTriggerList::GetNextTriggerID() const
{
	typedef std::set<int> IDList;
	IDList idList;
	
	for (const_iterator it = m_TriggerList.begin();it != m_TriggerList.end();++it)
	{
		const CSVOTriggerObj* pObj = (*it);
		idList.insert(pObj->GetTriggerDigNumber());
	}
	int prevNo = -1;
	for (IDList::const_iterator it = idList.begin();it != idList.end();++it)
	{
		int number = (*it); 
		int delta = number - prevNo;
		if (delta > 1)
		{
			break;
		}
		prevNo = number;
	}
	return ++prevNo;
}

bool CSVOTriggerList::IsTriggerInList(const SVString& sTriggerName) const
{
	bool bFound = false;

	for (const_iterator it = m_TriggerList.begin();it != m_TriggerList.end() && !bFound;++it)
	{
		const CSVOTriggerObj* pObj = (*it);

		if (pObj->GetTriggerDisplayName() == sTriggerName)
		{
			bFound = true;
		}
	}
	return bFound;
}

CSVOTriggerList::iterator CSVOTriggerList::FindTriggerPosition(const SVString& sTriggerName)
{
	iterator pos( m_TriggerList.begin() );
	BOOL bFound = FALSE;

	while( pos != m_TriggerList.end() && !bFound )
	{
		CSVOTriggerObj* pObj = m_TriggerList.GetAt(pos);

		if (sTriggerName == pObj->GetTriggerDisplayName())
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

void CSVOTriggerList::ResetContent()
{
	iterator pos( m_TriggerList.begin() );

	while( pos != m_TriggerList.end() )
	{
		CSVOTriggerObj* pObj = m_TriggerList.GetAt(pos);
		pos = m_TriggerList.erase(pos);
		delete pObj;
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOTriggerList.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:14:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Jan 2013 16:39:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Updated and added methods to handle the new look-up  and add  functionality.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Nov 2010 16:25:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Sep 2008 16:57:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed unused methods
 * Revised to use SVString
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 13:27:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:39:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
