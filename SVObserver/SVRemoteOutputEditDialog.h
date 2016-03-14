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

