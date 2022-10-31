//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVOGuiUtility/DataController.h"
#include "SVOGuiUtility/LinkedValueWidgetHelper.h"
#include "Definitions/SVMatroxSimpleEnums.h"
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

	class AipCtr ///< (AlternativeImagePathController) provides access to widgets and IDs for alternative image path configuration
				/// short class name: used only locally
	{
		enum TupleContent : size_t { ValueEdit = 0, EmbeddedId, DottedNameSelectButton, EmbeddedLinkId }; //values must start with 0 and be consecutive
	public:
		explicit AipCtr(SvOgu::ValueController& rValueController, uint32_t inspectionId, uint32_t taskId) :
			m_source(inspectionId, taskId, &rValueController)
		{}

		enum Lve ///< (LinkedValueEnums) short name: used only locally
		{
			BaseDirectoryname,
			DirectorynameIndex,
			SubfolderSelection,
			SubfolderLocation,
			BaseFilename,
			FilenameIndex1,
			CenterFilename,
			FilenameIndex2,
			__Num__
		};

		void init();
		void DoDataExchange(CDataExchange* pDX);
		afx_msg void OnButtonUseAlternativeImagePath(BOOL enable);

		void OnButton(Lve widgetEnum);
		void OnKillFocus(Lve widgetEnum);

		std::array < std::unique_ptr<SvOgu::LinkedValueWidgetHelper>, Lve::__Num__> m_WidgetHelpers;

	private:

		SvOgu::WidgetHelperSource m_source;


		CEdit	m_EditBaseDirectoryname;
		CButton m_ButtonBaseDirectoryname;
		CEdit	m_EditDirectorynameIndex;
		CButton m_ButtonDirectorynameIndex;
		CEdit	m_EditSubfolderSelection;
		CButton m_ButtonSubfolderSelection;
		CEdit	m_EditSubfolderLocation;
		CButton m_ButtonSubfolderLocation;
		CEdit	m_EditBaseFilename;
		CButton m_ButtonBaseFilename;
		CEdit	m_EditFilenameIndex1;
		CButton m_ButtonFilenameIndex1;
		CEdit	m_EditCenterFilename;
		CButton m_ButtonCenterFilename;
		CEdit	m_EditFilenameIndex2;
		CButton m_ButtonFilenameIndex2;
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
	afx_msg void OnSelchangeImageFormat();
	afx_msg void OnDetails();

	afx_msg void OnChangeEditMaxImages();

	afx_msg void OnButtonUseAlternativeImagePath(); ///< enables or disables the GUI elements that define alternative image paths depending on m_useAlternativeImagePath

	afx_msg void OnButtonMaxImageQueueLength();
	afx_msg void OnChangeMaxImageQueueLength();

	afx_msg void OnButtonImageFilepathroot1();
	afx_msg void OnButtonImageFilepathroot2();
	afx_msg void OnButtonImageFilepathroot3();
	afx_msg void OnKillFocusImageFilepathroot1();
	afx_msg void OnKillFocusImageFilepathroot2();
	afx_msg void OnKillFocusImageFilepathroot3();

	void ReadSelectedObjects();
	void ShowObjectSelector();
	bool validateArchiveImageFilepath();  ///< makes sure that the directory as defined in the Archive Tool Adjustment Dialog is available
	void updateAndCommitImageValues();

	template<AipCtr::Lve lve>
	void OnButton() { m_alternativeImagePaths.OnButton(lve); }
	template<AipCtr::Lve lve>
	void OnKillFocus() { m_alternativeImagePaths.OnKillFocus(lve); }

	__int64 recalculateRemainingImageMemory();

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
	CComboBox	m_imageFormat;
	SvMc::EditNumbers	m_EditMaxImages;

	CEdit	m_maximumImageQueueLengthEditBox;
	CButton m_maximumImageQueueLengthButton;

	std::unique_ptr<SvOgu::LinkedValueWidgetHelper> m_maximumImageQueueLengthWidgetHelper;

	CStatic	m_maximumImageQueueLengthStaticText;

	CEdit	m_ImageFilepathroot1;
	CEdit	m_ImageFilepathroot2;
	CEdit	m_ImageFilepathroot3;

	CButton m_ImageFilepathroot1Button;
	CButton m_ImageFilepathroot2Button;
	CButton m_ImageFilepathroot3Button;

	CButton m_StopAtMaxImagesButton;
	CButton m_useAlternativeImagePathButton;
	uint32_t m_inspectionId;
	uint32_t m_taskId;

	int		m_WhenToArchiveIndex = -1;
	int		m_FormatIndex = -1;
	SvTo::ArchiveMode m_selectedMode = SvTo::ArchiveMode::invalid;
	ImageFileFormat m_selectedFormat = ImageFileFormat::invalid;
	long m_ImagesToArchive = 0;

	typedef std::map <SvIe::SVImageClass*, long> MapSelectedImageType;

	SvOgu::ValueController m_ValueController;
	std::array < std::unique_ptr<SvOgu::LinkedValueWidgetHelper>, 3> m_ImageFilepathrootWidgetHelpers;

	bool m_Init = false;
	AipCtr m_alternativeImagePaths;

#pragma endregion Private Members
};

