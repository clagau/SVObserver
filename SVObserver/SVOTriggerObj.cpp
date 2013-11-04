//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOTriggerObj
//* .File Name       : $Workfile:   SVOTriggerObj.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:14:54  $
//******************************************************************************

#include "stdafx.h"
#include "SVOTriggerObj.h"
#include "SVSoftwareTriggerDefaults.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CSVOTriggerObj::CSVOTriggerObj(const SVString& name, int nDig)
: m_sTriggerDisplayName(name)
, m_iDigNumber(nDig)
, m_timerPeriod(SVSoftwareTriggerDefaults::TimerPeriod)
, m_bSoftwareTrigger(false)
{
}

CSVOTriggerObj::~CSVOTriggerObj()
{
}

LPCTSTR CSVOTriggerObj::GetTriggerDisplayName() const
{
	return m_sTriggerDisplayName.ToString();
}

int CSVOTriggerObj::GetTriggerDigNumber() const
{
    return m_iDigNumber;
}

// For Software trigger
void CSVOTriggerObj::SetTimerPeriod(long lPeriod)
{
	m_timerPeriod = lPeriod;
}

// For Software trigger
long CSVOTriggerObj::GetTimerPeriod() const
{
	return m_timerPeriod;
}

bool CSVOTriggerObj::IsSoftwareTrigger() const
{
	return m_bSoftwareTrigger;
}

void CSVOTriggerObj::SetSoftwareTrigger(bool bSoftwareTrigger)
{
	m_bSoftwareTrigger = bSoftwareTrigger;
}

bool CSVOTriggerObj::IsAcquisitionTrigger() const
{
	bool bRet = (m_sTriggerDisplayName.find(SV_CAMERA_TRIGGER_NAME) == 0);
	return bRet;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOTriggerObj.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:14:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   21 Jan 2013 11:31:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added IsAcquisitonTrigger method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Jan 2013 16:39:36   jspila
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
 *    Rev 1.4   30 Jul 2009 12:14:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   03 Oct 2008 17:31:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Changed Frequency To Period for the Software Timer Trigger
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Sep 2008 16:58:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use SVString
 * Added methods and member variables for Software Trigger
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 13:27:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
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
