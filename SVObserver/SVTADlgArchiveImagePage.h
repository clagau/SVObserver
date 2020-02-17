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

#include <tuple>

//this tuple hold hold GUI elements and GUIDs required for (possibly linked) Values that are to be modified using a dialog
typedef std::tuple<CEdit&, const GUID&, CButton*, const GUID*> ValueAndGuiInformation; //@TODO[Arvid]  it would be better to use std::optional instead of pointers here: but not available in VS2015!
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

class SVGUID;
class SVToolAdjustmentDialogSheetClass;
#pragma endregion Declarations

class SVTADlgArchiveImagePage : public CPropertyPage, public SvOg::ISVPropertyPageDialog
{
	enum { IDD = IDD_TA_ARCHIVE_IMAGE_PAGE };

	class AlternativeImagePaths ///< provides access to widgets and GUIDs for alternative image path configuration
	{
		enum TupleContent: size_t { ValueEdit = 0, ValueGuid, DottedNameSelectButton, DottedNameGuid }; //values must start with 0 and be consecutive
	public:
		explicit AlternativeImagePaths(Controller &rValues):
			m_rValues(rValues), 
			m_vecValueAndGuiInfo{
				{ m_EditBaseDirectoryname, SVBaseDirectorynameGUID, nullptr, nullptr },
				{ m_EditDirectorynameIndex, SVDirectorynameIndexGUID, &m_ButtonDirectorynameIndex, &SVDirectorynameIndexLink_GUID },
				{ m_EditBaseFilename, SVBaseFilenameGUID, nullptr, nullptr },
				{ m_EditFilenameIndex1, SVFilenameIndex1GUID, &m_ButtonFilenameIndex1, &SVFilenameIndex1Link_GUID },
				{ m_EditCenterFilename, SVCenterFilenameGUID, nullptr, nullptr },
				{ m_EditFilenameIndex2, SVFilenameIndex2GUID, &m_ButtonFilenameIndex2, &SVFilenameIndex2Link_GUID },
				{ m_EditSubfolderSelection, SVSubfolderSelectionGUID, &m_ButtonSubfolderSelection, &SVSubfolderSelectionLink_GUID } }
		{
			m_downArrowBitmap.LoadOEMBitmap(OBM_DNARROW);
		}

		void EditboxesToTextValues();
		void TextValuesToEditboxes();
		
		void DoDataExchange(CDataExchange* pDX);
		afx_msg void EnableAlternativeImagePaths(BOOL enable);

		void SelectFilenameIndex1(SvOg::ObjectSelectorController& rObjectSelector, CWnd* pParent);
		void SelectFilenameIndex2(SvOg::ObjectSelectorController& rObjectSelector, CWnd* pParent);
		void SelectDirectorynameIndex(SvOg::ObjectSelectorController& rObjectSelector, CWnd* pParent);
		void SelectSubfolderSelection(SvOg::ObjectSelectorController& rObjectSelector, CWnd* pParent);

		std::vector<ValueAndGuiInformation> m_vecValueAndGuiInfo; //used to iterate over widgets and GUIDs

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

	};

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
	afx_msg void UpdateMaxImageWidgetState();
	afx_msg void OnSelchangeModeCombo();
	afx_msg void OnChangeEditMaxImages();
	afx_msg void EnableAlternativeImagePaths(); ///< enables or disables the GUI elements that define alternative image paths depending on m_useAlternativeImagePaths

	void MemoryUsage();
	void ReadSelectedObjects();
	void ShowObjectSelector();
	void EnableMaxImagesAccordingToOtherSettings();

	void OnButtonFilenameIndex1() { m_alternativeImagePaths.SelectFilenameIndex1(m_objectSelector, this); }
	void OnButtonFilenameIndex2() {m_alternativeImagePaths.SelectFilenameIndex2(m_objectSelector, this);}
	void OnButtonDirectorynameIndex() {m_alternativeImagePaths.SelectDirectorynameIndex(m_objectSelector, this);}
	void OnButtonSubfolderSelection() {m_alternativeImagePaths.SelectSubfolderSelection(m_objectSelector, this);}

	bool checkImageMemory( SVGUID ImageGuid , bool bNewState );
	__int64 CalculateToolMemoryUsage();
	__int64 CalculateFreeMem();

#pragma endregion Protected Methods

#pragma region Private Members
private:
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
	BOOL	m_StopAtMaxImages = false;
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

