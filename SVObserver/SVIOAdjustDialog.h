//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOAdjustDialogClass
//* .File Name       : $Workfile:   SVIOAdjustDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:16:38  $
//******************************************************************************

#ifndef SVIOADJUSTDIALOG_H
#define SVIOADJUSTDIALOG_H

#include "SVOMFCLibrary/SVDataItemManagerTemplate.h"
#include "SVIOEntryHostStruct.h"

class SVDigitalInputObject;
class SVDigitalOutputObject;
class SVIODoc;

class SVIOAdjustDialogClass : public CDialog
{
	protected:
	//{{AFX_MSG(SVIOAdjustDialogClass)
	afx_msg void OnForce0Radio();
	afx_msg void OnForce1Radio();
	afx_msg void OnForceCheck();
	afx_msg void OnInvertCheck();
	afx_msg void OnCombineACKRadio();
	afx_msg void OnCombineNAKRadio();
	afx_msg void OnCombineCheck();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeIOCombo();
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()	

public:
	SVIOAdjustDialogClass( CWnd* pParent = NULL );   // Standardkonstruktor
	virtual ~SVIOAdjustDialogClass();

	//{{AFX_VIRTUAL(SVIOAdjustDialogClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

public:
	SVIODoc					*m_pIODoc;
	SVIOEntryHostStructPtr m_pIOEntry;
	SVDigitalInputObject	*m_pDigInput;
	SVDigitalOutputObject	*m_pDigOutput;
	BOOL m_bInputMode;

	//{{AFX_DATA(SVIOAdjustDialogClass)
	enum { IDD = IDD_IOADJUST_DIALOG };
	CComboBox	IOCombo;
	CString		StrIOName;
	CString		StrIOValue;
	BOOL		IsForced;
	BOOL		IsInverted;
	BOOL		IsForcedTrue;
	BOOL		IsForcedFalse;
	BOOL		IsCombined;
	BOOL		IsCombinedACK;
	BOOL		IsCombinedNAK;
	//}}AFX_DATA

private:
	typedef SVDataItemManagerTemplate< SVIOEntryHostStructPtr > SVDataItemManager;

	SVDataItemManager m_Items;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

/**************************************************************************/
/* INCLUDE CONTROL:                                                        */
/**************************************************************************/
#endif	// SVIOADJUSTDIALOG_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIOAdjustDialog.h_v  $
 * 
 *    Rev 1.1   08 May 2013 16:16:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   24 Apr 2013 13:53:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add SVDataItemManagerTemplate to manage item data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:04:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   19 Apr 2011 16:17:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with IO Assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   18 Apr 2011 10:24:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   09 Dec 2010 07:08:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issue with public definition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   08 Dec 2010 12:51:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   25 Sep 2003 14:34:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  381
 * SCR Title:  Combine PPQ Data to Inspection Data for a Digital Output
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Add code to support the combining of digital outputs with PPQ values.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   22 Apr 2003 10:31:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
//** EOF **
