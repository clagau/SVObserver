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
#include "SVMFCControls/EditNumbers.h"
#include "Tools/ArchiveMethodEnum.h"
#include "SVOGui/DataController.h"
#include "SVOGui/LinkedValueWidgetHelper.h"
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
		explicit AlternativeImagePathController(SvOg::ValueController& rValueController, uint32_t inspectionId, uint32_t taskId) :
			m_inspectionId{inspectionId},
			m_taskId{taskId},
			m_rValueController(rValueController),
			m_vecValueAndGuiInfo
		{
			{ m_EditBaseDirectoryname, SvPb::BaseDirectorynameEId, rValueController},
			{ m_EditBaseFilename, SvPb::BaseFilenameEId, rValueController},
			{ m_EditCenterFilename, SvPb::CenterFilenameEId, rValueController}
		}
		{}

		enum LinkedValueEnums
		{
			DirectorynameIndex,
			FilenameIndex1,
			FilenameIndex2,
			SubfolderSelection,
			SubfolderLocation,
			__Num__
		};

		void EditboxesToTextValues();
		void TextValuesToEditboxes();
		
		void init();
		void DoDataExchange(CDataExchange* pDX);
		afx_msg void OnButtonUseAlternativeImagePaths(BOOL enable);

		void OnButton(LinkedValueEnums widgetEnum);
		void OnKillFocus(LinkedValueEnums widgetEnum);

		std::vector<SvOg::ValueEditWidgetHelper> m_vecValueAndGuiInfo; //used to iterate over widgets and IDs
		std::array < std::unique_ptr<SvOg::LinkedValueWidgetHelper>, LinkedValueEnums::__Num__> m_WidgetHelpers;

	private:

		SvOg::ValueController& m_rValueController;
		uint32_t m_inspectionId;
		uint32_t m_taskId;

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
	bool QueryAllowExit() override;

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
	afx_msg void OnKillFocusImageFilepathroot1();
	afx_msg void OnKillFocusImageFilepathroot2();
	afx_msg void OnKillFocusImageFilepathroot3();

	void MemoryUsage();
	void ReadSelectedObjects();
	void ShowObjectSelector();
	bool validateArchiveImageFilepath();  ///< makes sure that the directory as defined in the Archive Tool Adjustment Dialog is available

	void OnButtonFilenameIndex1()		{m_alternativeImagePaths.OnButton(AlternativeImagePathController::LinkedValueEnums::FilenameIndex1);}
	void OnButtonFilenameIndex2()		{m_alternativeImagePaths.OnButton(AlternativeImagePathController::LinkedValueEnums::FilenameIndex2);}
	void OnButtonDirectorynameIndex()	{m_alternativeImagePaths.OnButton(AlternativeImagePathController::LinkedValueEnums::DirectorynameIndex);}
	void OnButtonSubfolderSelection()	{m_alternativeImagePaths.OnButton(AlternativeImagePathController::LinkedValueEnums::SubfolderSelection);}
	void OnButtonSubfolderLocation()	{m_alternativeImagePaths.OnButton(AlternativeImagePathController::LinkedValueEnums::SubfolderLocation);}
	void OnKillFocusFilenameIndex1() { m_alternativeImagePaths.OnKillFocus(AlternativeImagePathController::LinkedValueEnums::FilenameIndex1); }
	void OnKillFocusFilenameIndex2() { m_alternativeImagePaths.OnKillFocus(AlternativeImagePathController::LinkedValueEnums::FilenameIndex2); }
	void OnKillFocusDirectorynameIndex() { m_alternativeImagePaths.OnKillFocus(AlternativeImagePathController::LinkedValueEnums::DirectorynameIndex); }
	void OnKillFocusSubfolderSelection() { m_alternativeImagePaths.OnKillFocus(AlternativeImagePathController::LinkedValueEnums::SubfolderSelection); }
	void OnKillFocusSubfolderLocation() { m_alternativeImagePaths.OnKillFocus(AlternativeImagePathController::LinkedValueEnums::SubfolderLocation); }

	bool checkImageMemory(uint32_t imageId, bool bNewState);
	__int64 CalculateToolMemoryUsage();
	__int64 CalculateFreeMem();

#pragma endregion Protected Methods

#pragma region Private Members
private:
	SVToolAdjustmentDialogSheetClass* m_pParent;		//Pointer to the Tool Adjust sheet
	CListCtrl   m_ItemsSelected;						//The selected list control
	SVObjectReferenceVector m_ImagesToBeArchived;       //contains references to Images that are to be archived
	SvTo::SVArchiveTool* m_pTool = nullptr;				//Pointer to Archive Tool
	CImageList m_StateImageList;						//The state image list
	CButton m_Select;									//The tree select button
	CBitmap m_TreeBitmap;								//The bitmap for the tree button
	CString m_sMaxImageNumber;

	CStatic	m_wndAvailableArchiveImageMemory;
	CComboBox	m_WhenToArchive;
	SvMc::EditNumbers	m_EditMaxImages;
	CEdit	m_ImageFilepathroot1;
	CEdit	m_ImageFilepathroot2;
	CButton m_ImageFilepathroot1Button;
	CButton m_ImageFilepathroot2Button;
	CButton m_ImageFilepathroot3Button;
	CEdit	m_ImageFilepathroot3;
	CButton m_StopAtMaxImagesButton;
	BOOL	m_useAlternativeImagePaths = FALSE;
	uint32_t m_inspectionId;
	uint32_t m_taskId;

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
	std::array < std::unique_ptr<SvOg::LinkedValueWidgetHelper>, 3> m_ImageFilepathrootWidgetHelpers;
	
	bool m_Init = false;
	AlternativeImagePathController m_alternativeImagePaths;

#pragma endregion Private Members
};

