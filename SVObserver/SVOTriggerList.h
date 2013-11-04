//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOTriggerList
//* .File Name       : $Workfile:   SVOTriggerList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:14:42  $
//******************************************************************************

#ifndef SVOTRIGGERLIST_H
#define SVOTRIGGERLIST_H

#include "SVContainerLibrary/SVList.h"

class CSVOTriggerObj;
class SVString;

class CSVOTriggerList  
{
public:
	typedef SVList< CSVOTriggerObj* > SVTriggerList;
	typedef SVTriggerList::iterator iterator;
	typedef SVTriggerList::const_iterator const_iterator;

	CSVOTriggerList();
	virtual ~CSVOTriggerList();

    BOOL AddTriggerToList(const SVString& sTriggerName, int iDigNumber);
    BOOL RemoveTriggerFromList(const SVString& sTriggerName);
	bool IsTriggerInList(const SVString& sTiggerName) const;
    int GetTriggerListCount() const;
    CSVOTriggerObj *GetTriggerObjectByName(const SVString& sTriggerName);
    CSVOTriggerObj *GetTriggerObjectByPosition(int iPos);

    void ResetContent();

	int GetNextTriggerID() const;

private:
	SVTriggerList m_TriggerList;
    iterator FindTriggerPosition(const SVString& sTriggerName);
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOTriggerList.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:14:42   bWalter
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
 *    Rev 1.3   09 Nov 2010 16:25:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Sep 2008 16:57:08   sjones
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
 *    Rev 1.1   22 Apr 2003 13:27:48   rschock
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
