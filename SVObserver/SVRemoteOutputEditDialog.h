//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteOutputEditDialog
//* .File Name       : $Workfile:   SVRemoteOutputEditDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:43:36  $
//******************************************************************************

#ifndef SVREMOTEOUTPUTEDITDIALOG_H
#define SVREMOTEOUTPUTEDITDIALOG_H

#include <boost/config.hpp>
#include <boost/function.hpp>

#include "SVOMFCLibrary/SVDataItemManagerTemplate.h"
#include "SVIOEntryHostStruct.h"
#include "SVInfoStructs.h"

class SVRemoteOutputObject;

class SVRemoteOutputEditDialog : public CDialog
{
	DECLARE_DYNAMIC(SVRemoteOutputEditDialog)

	public:
	enum { IDD = IDD_REMOTE_OUTPUT_EDIT };
	CString m_strValueObjectSourceName;
	CString  m_strGroupName;
	CComboBox m_ValueObjectNameCombo;

protected:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);
	afx_msg void OnCbnSelchangeValueObjectNameCombo();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()

public:
	SVRemoteOutputEditDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~SVRemoteOutputEditDialog();

	GUID m_InputObjectGUID;
	SVIOEntryHostStructPtr m_TriggerCount;

protected:
	void UpdateValueObjectFromCombo();

private:
	typedef SVDataItemManagerTemplate< SVIOEntryHostStructPtr > SVDataItemManager;

	SVDataItemManager m_Items;

};

#endif // SVRemoteOutputEditDialog_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRemoteOutputEditDialog.h_v  $
 * 
 *    Rev 1.1   08 May 2013 16:43:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:51:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/