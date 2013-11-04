//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOPPQList
//* .File Name       : $Workfile:   SVOPPQList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:10:34  $
//******************************************************************************

#ifndef SVOPPQLIST_H
#define SVOPPQLIST_H

#include "SVContainerLibrary/SVList.h"

class CSVOPPQObj;

class CSVOPPQList  
{
public:
	typedef SVList< CSVOPPQObj* > SVPPQList;
	typedef SVPPQList::iterator iterator;

	CSVOPPQList();
	virtual ~CSVOPPQList();

    BOOL AddPPQToList(CString sPPQName);
    BOOL RemovePPQFromList(CString sPPQName);

    BOOL AttachCameraToPPQ(CString sPPQName, CString sCamera);
    BOOL RemoveCameraFromPPQ(CString sPPQName, CString sCamera);

    BOOL AttachInspectToPPQ(CString sPPQName, CString sInspect);
    BOOL RemoveInspectFromPPQ(CString sPPQName, CString sInspect);
    
    BOOL AttachTriggerToPPQ(CString sPPQName, CString sTrigger);
    BOOL RemoveTriggerFromPPQ(CString sPPQName);

    int GetPPQListCount() const;
    CSVOPPQObj *GetPPQObjectByName(CString sPPQName);
    CSVOPPQObj *GetPPQObjectByPosition(int iPos);
    BOOL IsPPQInList(CString sPPQName);
    void ResetContent();
    
private:
	SVPPQList m_PPQList;
    
	iterator FindPPQPosition(CString sPPQName);

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOPPQList.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:10:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Nov 2010 16:25:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Sep 2008 16:34:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised GetPPQListCount to be const
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 13:17:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:39:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
