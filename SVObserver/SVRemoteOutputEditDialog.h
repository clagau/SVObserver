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
#pragma once

#pragma region Includes
#include "Definitions/ObjectDefines.h"
#include "SVIOLibrary/SVIOEntryHostStruct.h"

#pragma endregion Includes

class SVRemoteOutputObject;

class SVRemoteOutputEditDialog : public CDialog
{
	public:
	enum { IDD = IDD_REMOTE_OUTPUT_EDIT };
	std::string  m_GroupName;
	std::string m_ValueObjectSourceName;

protected:
	virtual BOOL OnInitDialog() override;
	virtual void OnCancel() override;
	virtual void OnOK() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;
	afx_msg void OnCbnSelchangeValueObjectNameCombo();
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	DECLARE_MESSAGE_MAP()

public:
	SVRemoteOutputEditDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVRemoteOutputEditDialog();

	uint32_t m_InputObjectID = SvDef::InvalidObjectId;
	SVIOEntryHostStructPtr m_TriggerCount;

protected:
	void UpdateValueObjectFromCombo();

private:
	CComboBox m_ValueObjectNameCombo;
	std::map<unsigned long, SVIOEntryHostStructPtr> m_Items;
};

