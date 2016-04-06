//******************************************************************************
//* COPYRIGHT (c) 2010 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteIOAdjustDialog
//* .File Name       : $Workfile:   SVRemoteIOAdjustDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:49:12  $
//******************************************************************************

#pragma once

class SVRemoteIOAdjustDialog : public CDialog
{
	DECLARE_DYNAMIC(SVRemoteIOAdjustDialog)

public:
	SVRemoteIOAdjustDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVRemoteIOAdjustDialog();

	enum { IDD = IDD_REMOTE_IOADJUST_DIALOG };

	void SetIOName( LPCTSTR p_szName );

	void SetIOValue( const _variant_t& p_rValue );
	void GetIOValue( _variant_t& p_rValue ) const;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()

	CStatic m_IOName;
	CEdit m_ValueEditControl;

	CString m_Name;
	_variant_t m_Value;
	double m_EditValue;
};

