//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCPropPageList
//* .File Name       : $Workfile:   SVPLCPropPageList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:27:26  $
//******************************************************************************

#pragma once


#include "SVPLCPropSheet.h"
#include "afxcmn.h"
#include "SVPLCListCtrl.h"
// SVPLCPropPageList dialog

class SVPLCPropPageList : public CPropertyPage
{
	DECLARE_DYNAMIC(SVPLCPropPageList)

public:
	SVPLCPropPageList( SVPLCPropSheet* pParent = NULL );
	virtual ~SVPLCPropPageList();

// Dialog Data
	enum { IDD = IDD_PLC_PROP_LIST_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	SVPLCPropSheet* m_pParent;
	DECLARE_MESSAGE_MAP()
public:
	CEdit* m_pEdit;
	SVPLCListCtrl m_AddressList;
	virtual BOOL OnInitDialog();
	virtual BOOL OnWizardFinish();
	virtual LRESULT OnWizardBack();
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPLCPropPageList.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:27:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:09:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

