//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCPropPageStart
//* .File Name       : $Workfile:   SVPLCPropPageStart.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:27:50  $
//******************************************************************************

#pragma once

#include "SVPLCPropSheet.h"

// SVPLCPropDlg dialog

class SVPLCPropPageStart : public CPropertyPage
{
	DECLARE_DYNAMIC(SVPLCPropPageStart)

public:
	SVPLCPropPageStart(SVPLCPropSheet* pParent = NULL);   // standard constructor
	virtual ~SVPLCPropPageStart();

// Dialog Data
	enum { IDD = IDD_PLC_PROP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	long m_lBlockCount;
	long m_lQueueSize;
	CString m_strPLCId;
	virtual BOOL OnInitDialog();
	SVPLCPropSheet* m_pParent;

	virtual LRESULT OnWizardNext();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPLCPropPageStart.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:27:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Jan 2009 08:54:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed behavior of return key in PLC Edit Dialogs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:09:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

