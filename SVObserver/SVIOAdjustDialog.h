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
#pragma once

#pragma region Includes
#include "SVLibrary/SVDataItemManagerTemplate.h"
#include "SVLibrary/SVIOEntryHostStruct.h"
#pragma endregion Includes

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
	virtual void OnOK() override;
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSelChangeIOCombo();
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()	

public:
	SVIOAdjustDialogClass( CWnd* pParent = nullptr );   // Standardkonstruktor
	virtual ~SVIOAdjustDialogClass();

	//{{AFX_VIRTUAL(SVIOAdjustDialogClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
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

