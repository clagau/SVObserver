//*****************************************************************************
/// \copyright (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Adjustment Sheet for SVMeasureAnalyzer
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVValueBaseNormalizerClass.h"
#include "SVValueObject.h"
#include "SVTaskObjectInterfaceClass.h"
#pragma endregion Includes

class SVTaskObjectClass;
class SVIPDoc;

class SVMeasureAnalyzerAdjustmentSheetClass : public CPropertySheet
{
	DECLARE_DYNAMIC(SVMeasureAnalyzerAdjustmentSheetClass)

protected:
	//{{AFX_MSG(SVMeasureAnalyzerAdjustmentSheetClass)
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	SVMeasureAnalyzerAdjustmentSheetClass(UINT nIDCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	SVMeasureAnalyzerAdjustmentSheetClass(LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);

	virtual ~SVMeasureAnalyzerAdjustmentSheetClass();

public:
	// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(SVMeasureAnalyzerAdjustmentSheetClass)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
};
