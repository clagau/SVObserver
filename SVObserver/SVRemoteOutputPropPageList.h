//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteOutputPropPageList
//* .File Name       : $Workfile:   SVRemoteOutputPropPageList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:53:06  $
//******************************************************************************

#pragma once

#include "resource.h"
#include "SVRemoteOutputPropSheet.h"
#include "afxcmn.h"
#include "SVPLCListCtrl.h"
// SVRemoteOutputPropPageList dialog

class SVRemoteOutputPropPageList : public CPropertyPage
{
	DECLARE_DYNAMIC(SVRemoteOutputPropPageList)

public:
	SVRemoteOutputPropPageList( SVRemoteOutputPropSheet* pParent = NULL );
	virtual ~SVRemoteOutputPropPageList();

// Dialog Data
	enum { IDD = IDD_REMOTE_OUTPUT_PROP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	SVRemoteOutputPropSheet* m_pParent;
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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRemoteOutputPropPageList.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:53:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
