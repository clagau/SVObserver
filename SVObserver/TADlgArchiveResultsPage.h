//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgArchiveResultsPage
//* .File Name       : $Workfile:   TADlgArchiveResultsPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   28 Aug 2014 07:25:48  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "Definitions/StringTypeDef.h"
#include "SVObjectLibrary/SVObjectReference.h"
#include "SVOGui/ISVPropertyPageDialog.h"
#include "SVOGuiUtility/LinkedValueWidgetHelper.h"
#pragma endregion Includes

#pragma region Declarations
namespace SvTo
{
class SVArchiveTool;
}

class TADlgSheetClass;
#pragma endregion Declarations


class TADlgArchiveResultsPage : public CPropertyPage, public SvOg::ISVPropertyPageDialog
{
	enum { IDD = IDD_TA_ARCHIVE_RESULTS_PAGE };

#pragma region Constructor
public:
	TADlgArchiveResultsPage( uint32_t inspectionId, uint32_t taskObjectId, TADlgSheetClass* pParent);
	virtual ~TADlgArchiveResultsPage() = default;
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
	afx_msg void OnFormatResults();
	afx_msg void OnBnClickedHeaderCheck();
	afx_msg void OnBnClickedHeaderBtn();

	afx_msg void OnButtonResultFolderpathPart1();
	afx_msg void OnButtonResultFolderpathPart2();
	afx_msg void OnButtonResultFilename();
	afx_msg void OnKillFocusResultFolderpathPart1();
	afx_msg void OnKillFocusResultFolderpathPart2();
	afx_msg void OnKillFocusResultFilename();

	virtual BOOL OnKillActive() override { return QueryAllowExit(); }

	void ReadSelectedObjects();
	void ShowObjectSelector();
	SvDef::StringPairVector GetSelectedHeaderNamePairs();
	void StoreHeaderValuesToTool(SvDef::StringPairVector& HeaderPairs );
#pragma endregion Protected Methods

#pragma region Private Members
private:
	TADlgSheetClass* m_pParent;							//Pointer to the Tool Adjust sheet
	CListCtrl   m_selectedResultsWidget;			    //The list control containing the selected results
	SVObjectReferenceVector m_ResultsToBeArchived;      //contains references to objects that are to be archived
	SvTo::SVArchiveTool* m_pArchiveTool = nullptr;		//Pointer to Archive Tool
	CImageList m_StateImageList;						//The state image list
	CButton m_Select;									//The tree select button
	CBitmap m_TreeBitmap;								//The bitmap for the tree button
	BOOL	m_appendArchive = false;					//Append archive flag
	BOOL    m_formatResults = false;					//Flag: Use special format for Archive Tool value strings?
	DWORD   m_minimumNumberOfCharacters = 8;							//Minimum total number of characters for Archive Tool value strings
	CEdit	m_minimumNumberOfCharactersEdit;						    //Edit control for minimum total number of characters
	DWORD   m_numberOfDecimals = 2;  							//Number of decimal places for Archive Tool value strings - but only if if that value has decimal places
	CEdit	m_numberOfDecimalsEdit;								//Edit control for number of decimal places 
	BOOL	m_useColumnHeaders = false;					//Column headers flag
	uint32_t m_inspectionId;
	uint32_t m_taskId;

	std::string m_inspectionName;

	//edit boxes for the parts of the archive file path and buttons to manipulate them
	CEdit	m_resultFolderpathPart1Edit;					
	CButton m_resultFolderpathPart1Button;
	CEdit	m_resultFolderpathPart2Edit;
	CButton m_resultFolderpathPart2Button;
	CEdit	m_ResultFilenameEdit;
	CButton m_ResultFilenameButton;

	SvOgu::ValueController m_ValueController;
	std::array < std::unique_ptr<SvOgu::LinkedValueWidgetHelper>, 3> m_ResultFolderpathWidgetHelpers;

#pragma endregion Private Members
};

