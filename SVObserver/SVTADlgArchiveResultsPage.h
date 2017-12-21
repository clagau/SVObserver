//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgArchiveResultsPage
//* .File Name       : $Workfile:   SVTADlgArchiveResultsPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   28 Aug 2014 07:25:48  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOGui\ISVPropertyPageDialog.h"
#include "SVContainerLibrary/SelectorItem.h"
#include "SVArchiveTool.h"
#include "SVGlobal.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

#pragma region Declarations
class SVToolAdjustmentDialogSheetClass;
#pragma endregion Declarations

class SVTADlgArchiveResultsPage : public CPropertyPage, public SvOg::ISVPropertyPageDialog
{
	enum { IDD = IDD_TA_ARCHIVE_RESULTS_PAGE };

#pragma region Constructor
public:
	SVTADlgArchiveResultsPage( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* pParent );
	virtual ~SVTADlgArchiveResultsPage();
#pragma endregion Constructor

#pragma region Public Methods
public:
	// ISVPropertyPageDialog
	virtual bool QueryAllowExit() override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	afx_msg void OnDblClickListSelected(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSelectObjects();
	afx_msg void OnRemoveItem();
	afx_msg void OnRemoveAllItems();
	afx_msg void OnBrowse();
	afx_msg void OnBnClickedHeaderCheck();
	afx_msg void OnBnClickedHeaderBtn();

	void ReadSelectedObjects();
	void ShowObjectSelector();
	bool GetSelectedHeaderNamePairs(SvDef::StringPairVector& HeaderPairs );
	bool StoreHeaderValuesToTool(SvDef::StringPairVector& HeaderPairs );
#pragma endregion Protected Methods

#pragma region Private Members
private:
	SVToolAdjustmentDialogSheetClass* m_pParent; //Pointer to the Tool Adjust sheet
	CListCtrl   m_ItemsSelected;				//The selected list control
	SVObjectReferenceVector m_List;				//The selected list
	SVArchiveTool* m_pTool;						//Pointer to the Archive Tool
	CImageList m_StateImageList;				//The state image list
	CButton m_Select;							//The tree select button
	CBitmap m_TreeBitmap;						//The bitmap for the tree button
	CEdit	m_ArchiveFileName;					//Archive file name
	BOOL	m_AppendArchive;					//Append archive flag
	BOOL	m_ColumnHeaders;					//Column headers flag
#pragma endregion Private Members
};

