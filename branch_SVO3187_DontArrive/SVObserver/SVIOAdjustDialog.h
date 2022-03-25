//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOAdjustDialog
//* .File Name       : $Workfile:   SVIOAdjustDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:16:38  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVIOLibrary/SVIOEntryHostStruct.h"
#pragma endregion Includes

class PlcOutputObject;
class SVDigitalInputObject;
class SVDigitalOutputObject;
class SVIODoc;

class SVIOAdjustDialog : public CDialog
{
	DECLARE_MESSAGE_MAP()

protected:
	virtual void OnOK() override;
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSelChangeIOCombo();
	
public:
	SVIOAdjustDialog( CWnd* pParent = nullptr );
	virtual ~SVIOAdjustDialog() = default;

	//{{AFX_VIRTUAL(SVIOAdjustDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterst�tzung
	//}}AFX_VIRTUAL

public:
	SVIOEntryHostStructPtr m_pIOEntry;
	SVDigitalInputObject* m_pDigInput {nullptr};
	SVDigitalOutputObject* m_pDigOutput {nullptr};
	PlcOutputObject* m_pPlcOutput {nullptr};
	int m_PpqIndex {-1};		///This is only required when the type is PLC Output

	//{{AFX_DATA(SVIOAdjustDialog)
	enum { IDD = IDD_IOADJUST_DIALOG };
	CComboBox	IOCombo;
	CString		IOName;
	CString		IOValue;
	BOOL		IsForced {false};
	BOOL		IsInverted {false};
	BOOL		IsForcedTrue {false};
	BOOL		IsForcedFalse {false};
	BOOL		IsCombined {false};
	BOOL		IsCombinedACK {false};
	BOOL		IsCombinedNAK {false};
	//}}AFX_DATA

private:
	void showForcedGroup(int showState);
	void showInvertGroup(int showState);
	void showCombinedGroup(int showState);
	std::map<std::string, SVIOEntryHostStructPtr> m_Items;
};

