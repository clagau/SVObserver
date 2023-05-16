//******************************************************************************
/// \copyright (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//******************************************************************************
/// A List-Ctrl-class for the Result Window.
//******************************************************************************

#pragma once

#pragma region Includes
#include "InspectionEngine/SVIPResultData.h"
#pragma endregion Includes

#pragma region Declarations
class SVIPDoc;
#pragma endregion Declarations

class ResultListCtrl : public CListCtrl
{
	DECLARE_DYNCREATE( ResultListCtrl )
	DECLARE_MESSAGE_MAP()

#pragma region Constructor
public:
	ResultListCtrl();
	virtual ~ResultListCtrl();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	void setIPDoc(SVIPDoc* pDoc) { m_pDoc = pDoc; };
	/// The method updates the list
	void updateList();
#pragma endregion Public Methods

#pragma region Private Methods
private:
	int updateResults(bool bRedrawDefinitions);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;

	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	std::string CalcProcessesPerSecond(double p_LastTriggerDistance);
	void addColumnHeadings();
	void setColumnWidths();
#pragma endregion Private Methods

#pragma region Member variables
private:
	typedef std::vector<SvIe::SVIPResultItemDefinition> SVResultDefinitionVector;

	
	SVResultDefinitionVector m_ResultDefinitions;
	SvIe::SVIPResultData m_ResultData;
	bool m_ColumnWidthSet = false;
	mutable double m_UpdateTimeStamp = 0.0;
	SVIPDoc* m_pDoc = nullptr;

#pragma endregion Member variables
};

