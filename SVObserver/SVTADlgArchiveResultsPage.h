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
#include "Definitions/StringTypeDef.h"
#include "SVObjectLibrary/SVObjectReference.h"
#include "SVOGui/ISVPropertyPageDialog.h"
#pragma endregion Includes

#pragma region Declarations
namespace SvTo
{
class SVArchiveTool;
}

class SVToolAdjustmentDialogSheetClass;
#pragma endregion Declarations


class SVTADlgArchiveImagePage; //@TODO [Arvid][10.00][7.8.2020] should be removed ASAP
	
class SVTADlgArchiveResultsPage : public CPropertyPage, public SvOg::ISVPropertyPageDialog
{
	enum { IDD = IDD_TA_ARCHIVE_RESULTS_PAGE };

#pragma region Constructor
public:
	SVTADlgArchiveResultsPage( uint32_t inspectionId, uint32_t taskObjectId, SVToolAdjustmentDialogSheetClass* pParent,  SVTADlgArchiveImagePage *pArchiveImagePage);
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
	afx_msg void OnFormatResults();
	afx_msg void OnBnClickedHeaderCheck();
	afx_msg void OnBnClickedHeaderBtn();

	void ReadSelectedObjects();
	void ShowObjectSelector();
	bool GetSelectedHeaderNamePairs(SvDef::StringPairVector& HeaderPairs );
	bool StoreHeaderValuesToTool(SvDef::StringPairVector& HeaderPairs );
#pragma endregion Protected Methods

#pragma region Private Members
private:
	SVToolAdjustmentDialogSheetClass* m_pParent;		//Pointer to the Tool Adjust sheet
	class SVTADlgArchiveImagePage *m_pArchiveImagePage;	//@TODO [Arvid][10.00][7.8.2020] should be removed
	CListCtrl   m_ItemsSelected;						//The selected list control
	SVObjectReferenceVector m_ResultsToBeArchived;      //contains references to objects that are to be archived

	//@TODO [Arvid][10.00][7.8.2020] es ist ein absolutes Unding, dass im Rahmen dieses Dialoges (und auch von SVTADlgArchiveImagePage)
	// auf SVArchiveTool zugegriffen wird - und zwar nicht nur deswegen, weil es direkt über Pointer und nicht über geeignete Commands geschieht
	// (was übrigens schlimm genug ist) sondern auch (und aus meiner Sicht vor allem) deswegen, weil alle möglichen Resets und RunOnce-Aufrufe stattfinden,
	// was im Zusammenhang mit stark erweiterten Pfadänderungsmöglichkeiten im Rahmen der Version 10.00 dazu führt, dass ToolAdjustment-Pages aufeinander 
	// zugreifen müssen, um sinnvoll prüfen zu können, ob sie geschlossen werden dürfen.
	// Außerdem werden zurzeit vom Tool zu unterschiedlichen Zeiten alle möglichen Checks durchgeführt (und die Existenz von Pfaden sichergestellt),
	// was auch zu verschiedenen Verrenkungen in den TA-Dialogen zwingt.
	// Lösungsvorschlag: im TA-Dialog wird nur eine "Konfiguration" für das Archive Tool zusammengestellt (und beim Schließen überprüft), das Tool
	// selber tut noch gar nichts. Erst wenn geprüft wird, ob in den Run-Modus gegangen werden kann, verwendet das Tool diese "Konfiguration" 
	// und prüft seine eigene Lauffähigkeit
	SvTo::SVArchiveTool* m_pTool;						//Pointer to the Archive Tool
	CImageList m_StateImageList;						//The state image list
	CButton m_Select;									//The tree select button
	CBitmap m_TreeBitmap;								//The bitmap for the tree button
	CEdit	m_ArchiveFileName;							//Archive file name
	BOOL	m_AppendArchive;							//Append archive flag
	BOOL    m_FormatResults;							//Flag: Use special format for Archive Tool value strings?
	DWORD   m_TotalWidth;								//Minimum total number of characters for Archive Tool value strings
	CEdit	m_TotalWidthEdit;						    //Edit control for minimum total number of characters
	DWORD   m_Decimals;									//Number of decimal places for Archive Tool value strings - but only if if that value has decimal places
	CEdit	m_DecimalsEdit;								//Edit control for number of decimal places 
	BOOL	m_ColumnHeaders;							//Column headers flag
#pragma endregion Private Members
};

