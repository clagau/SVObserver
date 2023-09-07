//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	afx_msg void OnObjectSelector();
	afx_msg void OnSelchangeInspection();

public:
	explicit SVIOAdjustDialog(const std::vector<std::string>& rUsedOutputList, CWnd* pParent = nullptr );
	virtual ~SVIOAdjustDialog() = default;

	//{{AFX_VIRTUAL(SVIOAdjustDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

public:
	SVObjectClass* m_pLinkedObject {nullptr};
	std::array<SVObjectClass*, SvDef::cObjectIndexMaxNr> m_pObjectLinkList {nullptr, nullptr, nullptr, nullptr};
	SVIOObjectType m_ioObjectType {SVIOObjectType::IO_INVALID_OBJECT};
	SVDigitalInputObject* m_pDigInput {nullptr};
	SVDigitalOutputObject* m_pDigOutput {nullptr};
	PlcOutputObject* m_pPlcOutput {nullptr};
	int m_ppqIDNr {-1};		///This is only required when the type is PLC Output

	//{{AFX_DATA(SVIOAdjustDialog)
	enum { IDD = IDD_IOADJUST_DIALOG };
	CComboBox	m_inspectionCtrl;
	CString		m_IOName;
	CString		m_IOValue;
	BOOL		m_isForced {false};
	BOOL		m_isInverted {false};
	BOOL		m_isForcedTrue {false};
	BOOL		m_isForcedFalse {false};
	BOOL		m_sCombined {false};
	BOOL		m_isCombinedACK {false};
	BOOL		m_isCombinedNAK {false};
	//}}AFX_DATA

private:
	void UpdateGroups();
	void showForcedGroup(int showState);
	void showInvertGroup(int showState);
	void showCombinedGroup(int showState);

	std::string m_moduleReadyName;
	const std::vector<std::string>& m_rUsedOuputList;
};

