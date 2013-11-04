// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOIntelRAIDStatusClass
// * .File Name       : $Workfile:   SVOIntelRAIDStatusClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 13:02:38  $
// ******************************************************************************

#ifndef _SVOINTELRAIDSTATUSCLASS_H
#define _SVOINTELRAIDSTATUSCLASS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SVOIntelRAIDStatusClass  
{
public:
	SVOIntelRAIDStatusClass();
	virtual ~SVOIntelRAIDStatusClass();

	HRESULT UpdateStatus();
	HRESULT CheckStatus();

	const CString& GetRaidStatus();
	const CString& GetErrorStatus();

	const HANDLE GetCheckEvent();

protected:

	HANDLE m_hCheckEvent;

	CString m_csRaidStatus;
	CString m_csErrorStatus;

};

#endif // _SVOINTELRAIDSTATUSCLASS_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOIntelRAIDStatusClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:02:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jun 2006 12:19:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  561
 * SCR Title:  Change RAID Status dialog to only report RAID Error Status Messages
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to only check the status string to see if there was a SATA RAID error and on any other error write it to an error file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Dec 2005 15:12:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  530
 * SCR Title:  Allow SVObserver to monitor Intel SATA RAID status
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new classes to handle getting the invalid RAID status to the GUI.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/