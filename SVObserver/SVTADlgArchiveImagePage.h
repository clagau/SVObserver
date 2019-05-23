//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgArchiveImagePage
//* .File Name       : $Workfile:   SVTADlgArchiveImagePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   28 Aug 2014 07:25:48  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectReference.h"
#include "SVOGui/ISVPropertyPageDialog.h"
#include "SVMFCControls/SVEditNumbers.h"
#include "Tools/ArchiveMethodEnum.h"
#pragma endregion Includes

#pragma region Declarations
namespace SvIe
{
class SVImageClass;
}

namespace SvTo
{
class SVArchiveTool;
}

class SVGUID;
class SVToolAdjustmentDialogSheetClass;
#pragma endregion Declarations

class SVTADlgArchiveImagePage : public CPropertyPage, public SvOg::ISVPropertyPageDialog
{
	enum { IDD = IDD_TA_ARCHIVE_IMAGE_PAGE };

#pragma region Constructor
public:
	SVTADlgArchiveImagePage( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* pParent );
	virtual ~SVTADlgArchiveImagePage();
#pragma endregion Constructor

#pragma region Public Methods
	// ISVPropertyPageDialog
	bool QueryAllowExit();
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
	afx_msg void OnSelchangeModeCombo();
	afx_msg void OnChangeEditMaxImages();

	void MemoryUsage();
	void ReadSelectedObjects();
	void ShowObjectSelector();

	bool checkImageMemory( SVGUID ImageGuid , bool bNewState );
	__int64 CalculateToolMemoryUsage();
	__int64 CalculateFreeMem();

#pragma endregion Protected Methods

#pragma region Private Members
private:
	SVToolAdjustmentDialogSheetClass* m_pParent; //Pointer to the Tool Adjust sheet
	CListCtrl   m_ItemsSelected;				//The selected list control
	SVObjectReferenceVector m_List;				//The selected list
	SvTo::SVArchiveTool* m_pTool;				//Pointer to the Archive Tool
	CImageList m_StateImageList;				//The state image list
	CButton m_Select;							//The tree select button
	CBitmap m_TreeBitmap;						//The bitmap for the tree button
	CString m_sMaxImageNumber;

	CStatic	m_wndAvailableArchiveImageMemory;
	CComboBox	m_Mode;
	SvMc::CEditNumbers	m_MaxImages;
	CEdit	m_ImageFilesRoot;
	BOOL	m_StopAtMaxImages;
	int		m_iModeIndex;
	SvTo::SVArchiveMethodEnum m_eSelectedArchiveMethod;
	long m_ImagesToArchive;
	__int64 m_TotalArchiveImageMemoryAvailable;
	__int64 m_InitialArchiveImageMemoryUsageExcludingThisTool;
	__int64 m_InitialToolImageMemoryUsage;
	__int64 m_ToolImageMemoryUsage;

	typedef std::map <SvIe::SVImageClass*, long> MapSelectedImageType;
	MapSelectedImageType m_mapSelectedImageMemUsage;
	MapSelectedImageType m_mapInitialSelectedImageMemUsage;

	bool m_Init;
#pragma endregion Private Members
};

