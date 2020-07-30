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
#include "SVOResource/resource.h"
#include "SVObjectLibrary/SVObjectReference.h"
#include "SVOGui/ISVPropertyPageDialog.h"
#include "SVMFCControls/SVEditNumbers.h"
#include "Tools/ArchiveMethodEnum.h"
#include "SVOGui/ObjectSelectorController.h"
#include "SVOGui/DataController.h"
#include "SVOGui/ValueEditWidgetHelper.h"

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

class SVToolAdjustmentDialogSheetClass;
#pragma endregion Declarations

class SVTADlgArchiveImagePage : public CPropertyPage, public SvOg::ISVPropertyPageDialog
{
	enum { IDD = IDD_TA_ARCHIVE_IMAGE_PAGE };

	class AlternativeImagePathController ///< provides access to widgets and IDs for alternative image path configuration
	{
		enum TupleContent : size_t { ValueEdit = 0, EmbeddedId, DottedNameSelectButton, EmbeddedLinkId }; //values must start with 0 and be consecutive
	public:
		explicit AlternativeImagePathController(SvOg::ValueController &rValueController, uint32_t inspectionId) :
			m_rValueController(rValueController),
			m_ObjectSelectorController(inspectionId, SvDef::InvalidObjectId, SvPb::viewable),
			m_vecValueAndGuiInfo
		    {
				{ m_EditBaseDirectoryname, SvPb::BaseDirectorynameEId, nullptr, SvPb::NoEmbeddedId, rValueController},
				{ m_EditDirectorynameIndex, SvPb::DirectorynameIndexEId, &m_ButtonDirectorynameIndex, SvPb::DirectorynameIndexLinkEId, rValueController},
				{ m_EditBaseFilename, SvPb::BaseFilenameEId, nullptr, SvPb::NoEmbeddedId, rValueController},
				{ m_EditFilenameIndex1, SvPb::FilenameIndex1EId, &m_ButtonFilenameIndex1, SvPb::FilenameIndex1LinkEId, rValueController},
				{ m_EditCenterFilename, SvPb::CenterFilenameEId, nullptr, SvPb::NoEmbeddedId, rValueController},
				{ m_EditFilenameIndex2, SvPb::FilenameIndex2EId, &m_ButtonFilenameIndex2, SvPb::FilenameIndex2LinkEId, rValueController},
				{ m_EditSubfolderSelection, SvPb::SubfolderSelectionEId, &m_ButtonSubfolderSelection, SvPb::SubfolderSelectionLinkEId, rValueController},
				{ m_EditSubfolderLocation, SvPb::SubfolderLocationEId, &m_ButtonSubfolderLocation, SvPb::SubfolderLocationLinkEId, rValueController} 
			}{}

		void EditboxesToTextValues();
		void TextValuesToEditboxes();
		
		void DoDataExchange(CDataExchange* pDX);
		afx_msg void OnButtonUseAlternativeImagePaths(BOOL enable);

		void SelectFilenameIndex1(CWnd* pParent);
		void SelectFilenameIndex2(CWnd* pParent);
		void SelectDirectorynameIndex(CWnd* pParent);
		void SelectSubfolderSelection(CWnd* pParent);
		void SelectSubfolderLocation(CWnd* pParent);

		std::vector<SvOg::ValueEditWidgetHelper> m_vecValueAndGuiInfo; //used to iterate over widgets and IDs

	private:

		SvOg::ValueController& m_rValueController;
		SvOg::ObjectSelectorController m_ObjectSelectorController;

		CEdit	m_EditBaseDirectoryname;
		CEdit	m_EditDirectorynameIndex;
		CButton m_ButtonDirectorynameIndex;
		CEdit	m_EditBaseFilename;
		CEdit	m_EditFilenameIndex1;
		CButton m_ButtonFilenameIndex1;
		CEdit	m_EditCenterFilename;
		CEdit	m_EditFilenameIndex2;
		CButton m_ButtonFilenameIndex2;
		CEdit	m_EditSubfolderSelection;
		CButton m_ButtonSubfolderSelection;
		CEdit	m_EditSubfolderLocation;
		CButton m_ButtonSubfolderLocation;

	};

#pragma region Constructor
public:
	SVTADlgArchiveImagePage( uint32_t inspectionId, uint32_t taskObjectId, SVToolAdjustmentDialogSheetClass* pParent );
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
	afx_msg void OnBrowseImageFilepathroot1();
	afx_msg void OnStopAtMaxClicked();
	afx_msg void OnSelchangeWhenToArchive();

	afx_msg void OnChangeEditMaxImages();
	afx_msg void OnButtonUseAlternativeImagePaths(); ///< enables or disables the GUI elements that define alternative image paths depending on m_useAlternativeImagePaths

	afx_msg void OnButtonImageFilepathroot1();
	afx_msg void OnButtonImageFilepathroot2();
	afx_msg void OnButtonImageFilepathroot3();

	void MemoryUsage();
	void ReadSelectedObjects();
	void ShowObjectSelector();
	bool validateImageFilepathRoot();  ///< makes sure that the directory as defined in the Archive Tool Adjustment Dialog is available

	void OnButtonFilenameIndex1()		{m_alternativeImagePaths.SelectFilenameIndex1(this);}
	void OnButtonFilenameIndex2()		{m_alternativeImagePaths.SelectFilenameIndex2(this);}
	void OnButtonDirectorynameIndex()	{m_alternativeImagePaths.SelectDirectorynameIndex(this);}
	void OnButtonSubfolderSelection()	{m_alternativeImagePaths.SelectSubfolderSelection(this);}
	void OnButtonSubfolderLocation()	{m_alternativeImagePaths.SelectSubfolderLocation(this);}

	bool checkImageMemory(uint32_t imageId, bool bNewState);
	__int64 CalculateToolMemoryUsage();
	__int64 CalculateFreeMem();

#pragma endregion Protected Methods

#pragma region Private Members
private:
	SVToolAdjustmentDialogSheetClass* m_pParent;		//Pointer to the Tool Adjust sheet
	CListCtrl   m_ItemsSelected;						//The selected list control
	SVObjectReferenceVector m_ImagesToBeArchived;       //contains references to Images that are to be archived
	SvTo::SVArchiveTool* m_pTool = nullptr;				//Pointer to the Archive Tool
	CImageList m_StateImageList;						//The state image list
	CButton m_Select;									//The tree select button
	CBitmap m_TreeBitmap;								//The bitmap for the tree button
	CString m_sMaxImageNumber;

	CStatic	m_wndAvailableArchiveImageMemory;
	CComboBox	m_WhenToArchive;
	SvMc::CEditNumbers	m_EditMaxImages;
	CEdit	m_ImageFilepathroot1;
	CEdit	m_ImageFilepathroot2;
	CButton m_ImageFilepathroot1Button;
	CButton m_ImageFilepathroot2Button;
	CButton m_ImageFilepathroot3Button;
	CEdit	m_ImageFilepathroot3;
	CButton m_StopAtMaxImagesButton;
	BOOL	m_useAlternativeImagePaths = FALSE;

	int		m_WhenToArchiveIndex = -1;
	SvTo::SVArchiveMethodEnum m_eSelectedArchiveMethod = SvTo::SVArchiveInvalidMethod;
	long m_ImagesToArchive = 0;
	__int64 m_TotalArchiveImageMemoryAvailable = 0;
	__int64 m_InitialArchiveImageMemoryUsageExcludingThisTool = 0;
	__int64 m_InitialToolImageMemoryUsage = 0;
	__int64 m_ToolImageMemoryUsage = 0;

	typedef std::map <SvIe::SVImageClass*, long> MapSelectedImageType;
	MapSelectedImageType m_mapSelectedImageMemUsage;
	MapSelectedImageType m_mapInitialSelectedImageMemUsage;

	SvOg::ValueController m_ValueController;
	SvOg::ValueEditWidgetHelper m_ImageFilepathroot1WidgetHelper;
	SvOg::ValueEditWidgetHelper m_ImageFilepathroot2WidgetHelper;
	SvOg::ValueEditWidgetHelper m_ImageFilepathroot3WidgetHelper;

	bool m_Init = false;

	SvOg::ObjectSelectorController m_RootPathObjectSelectorController;

	AlternativeImagePathController m_alternativeImagePaths;

#pragma endregion Private Members
};

