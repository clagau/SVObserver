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
#include "SVOGui/ValuesAccessor.h"
#pragma endregion Includes


struct ValueAndGuiInformation ///< holds GUI elements and IDs required for (possibly linked) Values that are to be modified using a dialog
{
	CEdit& m_valueEdit;
	SvPb::EmbeddedIdEnum m_embeddedId;
	CButton* m_pDottedNameSelectButton;
	SvPb::EmbeddedIdEnum m_embeddedLinkId;

	ValueAndGuiInformation(CEdit& valueEdit, SvPb::EmbeddedIdEnum embeddedId, CButton* pDottedNameSelectButton, SvPb::EmbeddedIdEnum embeddedLinkId) :
		m_valueEdit(valueEdit),
		m_embeddedId(embeddedId),
		m_pDottedNameSelectButton(pDottedNameSelectButton),
		m_embeddedLinkId(embeddedLinkId) {}
};

typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> Controller;

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

	class AlternativeImagePaths ///< provides access to widgets and IDs for alternative image path configuration
	{
		enum TupleContent : size_t { ValueEdit = 0, EmbeddedId, DottedNameSelectButton, EmbeddedLinkId }; //values must start with 0 and be consecutive
	public:
		explicit AlternativeImagePaths(Controller &rValues) :
			m_rValues(rValues),
			m_vecValueAndGuiInfo{
				{ m_EditBaseDirectoryname, SvPb::BaseDirectorynameEId, nullptr, SvPb::NoEmbeddedId },
				{ m_EditDirectorynameIndex, SvPb::DirectorynameIndexEId, &m_ButtonDirectorynameIndex, SvPb::DirectorynameIndexLinkEId },
				{ m_EditBaseFilename, SvPb::BaseFilenameEId, nullptr, SvPb::NoEmbeddedId },
				{ m_EditFilenameIndex1, SvPb::FilenameIndex1EId, &m_ButtonFilenameIndex1, SvPb::FilenameIndex1LinkEId },
				{ m_EditCenterFilename, SvPb::CenterFilenameEId, nullptr, SvPb::NoEmbeddedId },
				{ m_EditFilenameIndex2, SvPb::FilenameIndex2EId, &m_ButtonFilenameIndex2, SvPb::FilenameIndex2LinkEId },
				{ m_EditSubfolderSelection, SvPb::SubfolderSelectionEId, &m_ButtonSubfolderSelection, SvPb::SubfolderSelectionLinkEId },
				{ m_EditSubfolderLocation, SvPb::SubfolderLocationEId, &m_ButtonSubfolderLocation, SvPb::SubfolderLocationLinkEId } }
		{
			m_downArrowBitmap.LoadOEMBitmap(OBM_DNARROW);
		}

		void EditboxesToTextValues();
		void TextValuesToEditboxes();
		
		void DoDataExchange(CDataExchange* pDX);
		afx_msg void OnButtonUseAlternativeImagePaths(BOOL enable);

		void SelectFilenameIndex1(SvOg::ObjectSelectorController& rObjectSelector, CWnd* pParent);
		void SelectFilenameIndex2(SvOg::ObjectSelectorController& rObjectSelector, CWnd* pParent);
		void SelectDirectorynameIndex(SvOg::ObjectSelectorController& rObjectSelector, CWnd* pParent);
		void SelectSubfolderSelection(SvOg::ObjectSelectorController& rObjectSelector, CWnd* pParent);
		void SelectSubfolderLocation(SvOg::ObjectSelectorController& rObjectSelector, CWnd* pParent);

		std::vector<ValueAndGuiInformation> m_vecValueAndGuiInfo; //used to iterate over widgets and IDs

	protected:
		void EditboxToTextValue(ValueAndGuiInformation&);
		void TextValueToEditbox(ValueAndGuiInformation&);

	private:

		CBitmap m_downArrowBitmap;
		Controller& m_rValues;

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
	afx_msg void OnBrowse();
	afx_msg void UpdateMaxImageWidgetState();
	afx_msg void OnSelchangeModeCombo();

	afx_msg void OnChangeEditMaxImages();
	afx_msg void OnButtonUseAlternativeImagePaths(); ///< enables or disables the GUI elements that define alternative image paths depending on m_useAlternativeImagePaths

	void MemoryUsage();
	void ReadSelectedObjects();
	void ShowObjectSelector();
	void EnableMaxImagesAccordingToOtherSettings();

	void OnButtonFilenameIndex1() { m_alternativeImagePaths.SelectFilenameIndex1(m_objectSelector, this); }
	void OnButtonFilenameIndex2() {m_alternativeImagePaths.SelectFilenameIndex2(m_objectSelector, this);}
	void OnButtonDirectorynameIndex() {m_alternativeImagePaths.SelectDirectorynameIndex(m_objectSelector, this);}
	void OnButtonSubfolderSelection() { m_alternativeImagePaths.SelectSubfolderSelection(m_objectSelector, this); }
	void OnButtonSubfolderLocation() { m_alternativeImagePaths.SelectSubfolderLocation(m_objectSelector, this); }

	bool checkImageMemory(uint32_t imageId, bool bNewState);
	__int64 CalculateToolMemoryUsage();
	__int64 CalculateFreeMem();

#pragma endregion Protected Methods

#pragma region Private Members
private:
	void forceStopAtMaxIfRequired();

	SVToolAdjustmentDialogSheetClass* m_pParent; //Pointer to the Tool Adjust sheet
	CListCtrl   m_ItemsSelected;				//The selected list control
	SVObjectReferenceVector m_List;				//The selected list
	SvTo::SVArchiveTool* m_pTool = nullptr;		//Pointer to the Archive Tool
	CImageList m_StateImageList;				//The state image list
	CButton m_Select;							//The tree select button
	CBitmap m_TreeBitmap;						//The bitmap for the tree button
	CString m_sMaxImageNumber;

	CStatic	m_wndAvailableArchiveImageMemory;
	CComboBox	m_Mode;
	SvMc::CEditNumbers	m_EditMaxImages;
	CEdit	m_ImageFilesRoot;
	CButton m_UseTriggerCountButton;
	CButton m_StopAtMaxImagesButton;
	BOOL	m_UseTriggerCount = false;
	BOOL	m_useAlternativeImagePaths = FALSE;

	int		m_iModeIndex = -1;
	SvTo::SVArchiveMethodEnum m_eSelectedArchiveMethod = SvTo::SVArchiveInvalidMethod;
	long m_ImagesToArchive = 0;
	__int64 m_TotalArchiveImageMemoryAvailable = 0;
	__int64 m_InitialArchiveImageMemoryUsageExcludingThisTool = 0;
	__int64 m_InitialToolImageMemoryUsage = 0;
	__int64 m_ToolImageMemoryUsage = 0;

	typedef std::map <SvIe::SVImageClass*, long> MapSelectedImageType;
	MapSelectedImageType m_mapSelectedImageMemUsage;
	MapSelectedImageType m_mapInitialSelectedImageMemUsage;

	Controller m_Values;

	bool m_Init = false;

	SvOg::ObjectSelectorController m_objectSelector;

	AlternativeImagePaths m_alternativeImagePaths;

#pragma endregion Private Members
};

