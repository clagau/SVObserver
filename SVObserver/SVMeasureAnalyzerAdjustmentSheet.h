//*****************************************************************************
/// \copyright (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Adjustment Sheet for SVMeasureAnalyzer
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEdge.h"
#include "SVValueBaseNormalizerClass.h"
#include "SVValueObjectImpl.h"
#include "SVTaskObjectInterfaceClass.h"
#pragma endregion Includes

class SVLineAnalyzerClass;
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
	SVMeasureAnalyzerAdjustmentSheetClass(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	SVMeasureAnalyzerAdjustmentSheetClass(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);


//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVMeasureAnalyzerAdjustmentSheetClass();

public:
	// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(SVMeasureAnalyzerAdjustmentSheetClass)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
};
