//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#pragma endregion Includes

class SVRemoteOutputEditDialog : public CDialog
{
	public:
	enum { IDD = IDD_REMOTE_OUTPUT_EDIT };
	std::string  m_GroupName;
	uint32_t m_InputObjectID = SvDef::InvalidObjectId;

public:
	SVRemoteOutputEditDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVRemoteOutputEditDialog();

protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	void OnObjectSelector();

	DECLARE_MESSAGE_MAP()

private:
	CComboBox	m_inspectionCtrl;
	CString		m_IOName;
};

