//******************************************************************************
//* COPYRIGHT (c) 2010 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
public:
	SVRemoteIOAdjustDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVRemoteIOAdjustDialog();

	enum { IDD = IDD_REMOTE_IOADJUST_DIALOG };

	void SetIOName( LPCTSTR p_szName );

	void SetIOValue( const _variant_t& p_rValue );
	void GetIOValue( _variant_t& p_rValue ) const;

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	DECLARE_MESSAGE_MAP()

	CStatic m_IOName;
	CEdit m_ValueEditControl;

	CString m_Name;
	_variant_t m_Value;
	double m_EditValue;
};

