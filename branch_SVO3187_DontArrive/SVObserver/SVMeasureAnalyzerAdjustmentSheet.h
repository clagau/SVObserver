//*****************************************************************************
/// \copyright (c) 2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// Adjustment Sheet for SVMeasureAnalyzer
//******************************************************************************

#pragma once

#pragma region Includes
#pragma endregion Includes

class SVTaskObjectClass;
class SVIPDoc;

class SVMeasureAnalyzerAdjustmentSheet : public CPropertySheet
{
protected:
	//{{AFX_MSG(SVMeasureAnalyzerAdjustmentSheet)
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	explicit SVMeasureAnalyzerAdjustmentSheet(UINT nIDCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	explicit SVMeasureAnalyzerAdjustmentSheet(LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);

	virtual ~SVMeasureAnalyzerAdjustmentSheet();

public:
	// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(SVMeasureAnalyzerAdjustmentSheet)
	public:
	virtual BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL
};
