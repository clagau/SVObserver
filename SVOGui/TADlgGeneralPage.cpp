//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgGeneralPage
//* .File Name       : $Workfile:   TADlgGeneralPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:50:40  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADlgGeneralPage.h"
#include "Definitions\NameValueVector.h"
#include "SVShowDependentsDialog.h"
#include <numeric>
#include "CreateInputResultDlg.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
void fillComboBox(SvMc::AvailableObjectListComboBox<uint32_t>& rComboBox, const SvUl::NameObjectIdList& rAvailableImages, const std::string& rSelctedName)
{
	for (auto pair : rAvailableImages)
	{
		rComboBox.AddString(pair.first.c_str());
	}
	if (!rSelctedName.empty())
	{
		rComboBox.SelectString(-1, rSelctedName.c_str());
	}
}
}

namespace SvOg
{
	TADlgGeneralPage::TADlgGeneralPage(uint32_t inspectionId, uint32_t taskObjectId, SvPb::SVObjectSubTypeEnum subType)
	: CPropertyPage(TADlgGeneralPage::IDD)
	, m_InspectionID(inspectionId)
	, m_TaskObjectID(taskObjectId)
	, m_subType(subType)
	, m_values(SvOgu::BoundValues(inspectionId, taskObjectId))
	, m_AuxExtentsController(inspectionId, taskObjectId)
	{
		//{{AFX_DATA_INIT(TADlgGeneralPage)
		//}}AFX_DATA_INIT
	}

	TADlgGeneralPage::~TADlgGeneralPage()
	{
	}

	HRESULT TADlgGeneralPage::SetInspectionData()
	{
		HRESULT hr = S_OK;

		UpdateData(true); // get data from dialog

		int CurrentSelection = m_drawToolCombo.GetCurSel();
		if (CB_ERR != CurrentSelection)
		{
			auto Value = static_cast<long> (m_drawToolCombo.GetItemData(CurrentSelection));
			m_values.Set<long>(SvPb::ConditionalToolDrawFlagEId, Value);
		}			
		m_AuxExtentsController.enableAuxExtents(m_bUpdateAuxiliaryExtents ? true : false);
		m_AuxExtentsController.enableShowOverlayInAncestor(m_showOverlayInAncestor ? true : false);

		m_AuxExtentsController.setUseUnitMapping(m_UseUnitMapping ? true : false);
		
		if (SvPb::GroupToolObjectType == m_subType)
		{
			m_values.Set<bool>(SvPb::IsClosedEId, m_bCloseTool);
		}
			
		hr = m_values.Commit();
		if (S_OK == hr)
		{
			hr = m_AuxExtentsController.Commit();
		}
		UpdateData(false);

		return hr;
	}

	void TADlgGeneralPage::refresh()
	{
		

		// Update dialog with freeze tool attributes...
		long CurrentSelection = m_values.Get<long>(SvPb::ConditionalToolDrawFlagEId);
		m_drawToolCombo.SetCurSelItemData(CurrentSelection);
	
		// Check, if drawToolCombo must be disabled/enabled...
		// based on What?
		CWnd* pWnd = GetDlgItem(IDC_DRAW_TOOL_COMBO);
		if (pWnd)
		{
			pWnd->EnableWindow(true);
		}
		if (m_bAuxExtentsAvailable)
		{
			GetDlgItem(IDC_SOURCE_IMAGE_COMBO)->EnableWindow(m_bUpdateAuxiliaryExtents);
			GetDlgItem(IDC_ANCESTOR_SELECTION_COMBO)->EnableWindow(m_showOverlayInAncestor);
			if (m_IsExternalTool)
			{
				GetDlgItem(IDC_CHECK_USE_1_1)->EnableWindow(true);
			}
		}

		UpdateData(false); // Send Data to Dialog...
	}
	
	void TADlgGeneralPage::SetupDrawFlagComboBox()
	{
		const SvDef::NameValueVector& rDrawCriteria = m_values.GetEnumTypes(SvPb::ConditionalToolDrawFlagEId);
		m_drawToolCombo.SetEnumTypes(rDrawCriteria);
		long CurrentSelection = m_values.Get<long>(SvPb::ConditionalToolDrawFlagEId);
		m_drawToolCombo.SetCurSelItemData(CurrentSelection);
	}

	void TADlgGeneralPage::SetImages()
	{
		m_AvailableSourceImageCombo.ResetContent();
		m_AncestorSelectionCombo.ResetContent();
		if (m_bAuxExtentsAvailable)
		{
			fillComboBox(m_AvailableSourceImageCombo, m_AuxExtentsController.getAvailableImageList(), m_AuxExtentsController.getAuxSourceImageName());
			fillComboBox(m_AncestorSelectionCombo, m_AuxExtentsController.getAvailableAncestorForOverlayImageList(), m_AuxExtentsController.getAncestorForOverlay().first);
		}
	}

	bool TADlgGeneralPage::CheckAuxiliaryExtentsAvailable() const
	{
		bool bRetVal = m_AuxExtentsController.areAuxiliaryExtentsAvailable();
		return bRetVal;
	}

	void TADlgGeneralPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(TADlgGeneralPage)
		DDX_Check(pDX, IDC_ENABLE_AUXILIARY_EXTENTS, m_bUpdateAuxiliaryExtents);
		DDX_Control(pDX, IDC_SOURCE_IMAGE_COMBO, m_AvailableSourceImageCombo);
		DDX_Check(pDX, IDC_SHOW_OVERLAY_IN_ANCESTOR, m_showOverlayInAncestor);
		DDX_Control(pDX, IDC_ANCESTOR_SELECTION_COMBO, m_AncestorSelectionCombo);
		DDX_Control(pDX, IDC_DRAW_TOOL_COMBO, m_drawToolCombo);
		DDX_Control(pDX, IDC_TOOL_OVERLAYCOLOR_COMBO, m_AvailableToolForColorOverlayCombo);
		DDX_Check(pDX, IDC_CLOSE_TOOL_CBOX, m_bCloseTool);
		DDX_Check(pDX, IDC_CHECK_USE_1_1, m_UseUnitMapping);
		//}}AFX_DATA_MAP
	}

	BEGIN_MESSAGE_MAP(TADlgGeneralPage, CPropertyPage)
		//{{AFX_MSG_MAP(TADlgGeneralPage)
		ON_BN_CLICKED(IDC_ENABLE_AUXILIARY_EXTENTS, OnUpdateAuxiliaryExtents)
		ON_CBN_SELCHANGE(IDC_SOURCE_IMAGE_COMBO, OnSelchangeSourceImageCombo)
		ON_BN_CLICKED(IDC_SHOW_OVERLAY_IN_ANCESTOR, OnUpdateShowOverlayInAncestor)
		ON_CBN_SELCHANGE(IDC_ANCESTOR_SELECTION_COMBO, OnSelchangeAncestorSelectionCombo)
		ON_BN_CLICKED(IDC_CLOSE_TOOL_CBOX, OnUpdateCloseTool)
		ON_CBN_SELCHANGE(IDC_DRAW_TOOL_COMBO, OnSelchangeDrawToolCombo)
		ON_CBN_SELCHANGE(IDC_TOOL_OVERLAYCOLOR_COMBO, OnSelchangeToolForOverlayColorCombo)
		ON_BN_CLICKED(ID_SHOW_RELATIONS, OnShowRelations)
		ON_BN_CLICKED(IDC_CHECK_USE_1_1, OnUpdateUseUnitMapping)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	/////////////////////////////////////////////////////////////////////////////
	// TADlgGeneralPage message handlers

	BOOL TADlgGeneralPage::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();

		m_AuxExtentsController.Init();
		m_values.Init();
		m_IsExternalTool = m_AuxExtentsController.isExternalTool();

		SetupDrawFlagComboBox();
		m_bAuxExtentsAvailable = CheckAuxiliaryExtentsAvailable();

		// Get tool's Update Auxiliary Extents...
		if (!m_bAuxExtentsAvailable)
		{
			// Disable Auxiliary Extents
			setAuxiliaryShowFlags(SW_HIDE);
		}
		else
		{
			// Enable Auxiliary Extents
			setAuxiliaryShowFlags(SW_SHOW);		
			
			m_bUpdateAuxiliaryExtents = m_AuxExtentsController.isUpdateAuxExtentsEnabled();
			GetDlgItem(IDC_SOURCE_IMAGE_COMBO)->EnableWindow(m_bUpdateAuxiliaryExtents);	

			m_showOverlayInAncestor = m_AuxExtentsController.isShowOverlayInAncestorEnabled();
			GetDlgItem(IDC_ANCESTOR_SELECTION_COMBO)->EnableWindow(m_showOverlayInAncestor);
		}

		if (!m_bAuxExtentsAvailable || !m_IsExternalTool)
		{
			GetDlgItem(IDC_CHECK_USE_1_1)->ShowWindow(SW_HIDE);
		}
		else
		{
			GetDlgItem(IDC_CHECK_USE_1_1)->ShowWindow(SW_SHOW);
			m_UseUnitMapping = m_AuxExtentsController.hasUnitMappeing();
		}
		
		if (m_bAuxExtentsAvailable)
		{
			SetImages();
		}


		if (SvPb::GroupToolObjectType != m_subType)
		{
			initToolForOverlayColor();
		}
		else
		{
			SetHelpID(IDD_TA_GENERAL_DIALOG_GROUPTOOL);
			m_bCloseTool = m_values.Get<bool>(SvPb::IsClosedEId);
			GetDlgItem(IDC_TOOL_OVERLAYCOLOR_TEXT)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_TOOL_OVERLAYCOLOR_COMBO)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_DRAW_TOOL_LABEL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_DRAW_TOOL_COMBO)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_CLOSE_TOOL_CBOX)->ShowWindow(SW_SHOW);
		}

		UpdateData(false);
		refresh();
		// Success...
		return true;  // return TRUE unless you set the focus to a control
						  // EXCEPTION: OCX Property Pages should return FALSE
	}

	void TADlgGeneralPage::OnSelchangeDrawToolCombo()
	{
		SetInspectionData();
		refresh();
	}

	void TADlgGeneralPage::OnUpdateAuxiliaryExtents() 
	{
		SetInspectionData();

		const SvUl::NameObjectIdPair& source = m_AuxExtentsController.getAuxSourceImage();
		if (!source.first.empty())
		{
			m_AuxExtentsController.setAuxSourceImage(source.first);
		}

		GetDlgItem(IDC_SOURCE_IMAGE_COMBO)->EnableWindow(m_bUpdateAuxiliaryExtents);
		refresh();
	}

	void TADlgGeneralPage::OnUpdateShowOverlayInAncestor()
	{
		SetInspectionData();

		const SvUl::NameObjectIdPair& source = m_AuxExtentsController.getAncestorForOverlay();
		if (!source.first.empty())
		{
			m_AuxExtentsController.setAncestorforOverlay(source.first);
		}

		GetDlgItem(IDC_ANCESTOR_SELECTION_COMBO)->EnableWindow(m_showOverlayInAncestor);
		refresh();
	}

	void TADlgGeneralPage::OnUpdateUseUnitMapping()
	{
		SetInspectionData();
		
		m_AuxExtentsController.resetTaskObject();
		m_AuxExtentsController.findAuxSourceImages();
		SetImages();


		GetDlgItem(IDC_SOURCE_IMAGE_COMBO)->EnableWindow(m_bUpdateAuxiliaryExtents);
		refresh();
	}


	void TADlgGeneralPage::OnUpdateCloseTool()
	{
		if (SvPb::GroupToolObjectType == m_subType)
		{
			UpdateData(true); // get data from dialog
			if (m_bCloseTool)
			{
				SvPb::InspectionCmdRequest requestCmd;
				SvPb::InspectionCmdResponse responseCmd;
				auto* pRequest = requestCmd.mutable_getinvaliddependenciesrequest();
				pRequest->set_objectid(m_TaskObjectID);
				HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
				if (S_OK == hr && responseCmd.has_getinvaliddependenciesresponse())
				{
					const auto& rList = responseCmd.getinvaliddependenciesresponse().invaliddependencies();
					if (0 < rList.size())
					{
						SvOg::CreateInputResultDlg dlg(m_InspectionID, m_TaskObjectID, rList);
						INT_PTR retCode = dlg.DoModal();
						if (IDCANCEL == retCode)
						{
							m_bCloseTool = false;
							UpdateData(false);
							return;
						}
					}
				}
			}
			SetInspectionData();
		}
	}

	void TADlgGeneralPage::OnSelchangeSourceImageCombo() 
	{
		UpdateData( TRUE ); // get data from dialog

		int index = static_cast<int>(m_AvailableSourceImageCombo.GetCurSel());
		if (CB_ERR != index)
		{
			CString name;
			m_AvailableSourceImageCombo.GetLBText(index, name);
			if (!name.IsEmpty())
			{
				m_AuxExtentsController.setAuxSourceImage( std::string(name.GetString() ));
			}
		}
		refresh();
	}

	void TADlgGeneralPage::OnSelchangeAncestorSelectionCombo()
	{
		UpdateData(TRUE); // get data from dialog

		int index = static_cast<int>(m_AncestorSelectionCombo.GetCurSel());
		if (CB_ERR != index)
		{
			CString name;
			m_AncestorSelectionCombo.GetLBText(index, name);
			if (!name.IsEmpty())
			{
				m_AuxExtentsController.setAncestorforOverlay(std::string(name.GetString()));
			}
		}
		refresh();
	}

	void TADlgGeneralPage::OnSelchangeToolForOverlayColorCombo()
	{
		UpdateData(TRUE); // get data from dialog

		int index = static_cast<int>(m_AvailableToolForColorOverlayCombo.GetCurSel());
		if (CB_ERR != index)
		{
			CString name;
			m_AvailableToolForColorOverlayCombo.GetLBText(index, name);
			std::string stdName = name.GetString();
			auto iter = std::find_if(m_availableToolList.begin(), m_availableToolList.end(), [stdName](const auto& rEntry) { return rEntry.first == stdName; });
			if (m_availableToolList.end() != iter)
			{
				SvPb::InspectionCmdRequest requestCmd;
				auto* pRequest = requestCmd.mutable_connecttoobjectrequest();
				pRequest->set_objectid(m_TaskObjectID);
				pRequest->set_embeddedid(SvPb::OverlayColorInputEId);
				pRequest->set_newconnectedid(iter->second);
				pRequest->set_objecttype(SvPb::SVToolObjectType);

				HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
				Log_Assert(S_OK == hr);

				
			}
		}
		refresh();
	}
	
	void TADlgGeneralPage::OnShowRelations() 
	{
		std::set<uint32_t> DependencySet;
		DependencySet.insert(m_TaskObjectID);
		SVShowDependentsDialog Dlg( DependencySet, SvPb::SVToolObjectType, nullptr, SVShowDependentsDialog::Show);
		Dlg.DoModal();
	}

	BOOL TADlgGeneralPage::OnSetActive() 
	{
		SetImages();
		refresh();

		return CPropertyPage::OnSetActive();
	}

	void TADlgGeneralPage::initToolForOverlayColor()
	{
		SvPb::InputData inputData;
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pDataRequest = requestCmd.mutable_getinputdatarequest();
		pDataRequest->set_objectid(m_TaskObjectID);
		pDataRequest->set_embeddedid(SvPb::OverlayColorInputEId);
		pDataRequest->set_desired_first_object_type_for_connected_name(SvPb::SVToolSetObjectType);
		pDataRequest->set_exclude_first_object_name_in_conntected_name(false);
		responseCmd.Clear();
		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getinputdataresponse())
		{
			inputData = responseCmd.getinputdataresponse().data();
		}
		else
		{
			Log_Assert(false);
		}

		auto* pAvailableRequest = requestCmd.mutable_getavailableobjectsrequest();
		auto* pInputSearchParameter = pAvailableRequest->mutable_input_search();
		pInputSearchParameter->set_input_connected_objectid(inputData.inputid());
		pAvailableRequest->set_desired_first_object_type_for_name(SvPb::SVToolSetObjectType);

		hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getavailableobjectsresponse())
		{
			m_availableToolList = SvCmd::convertNameObjectIdList(responseCmd.getavailableobjectsresponse().list());
			m_AvailableToolForColorOverlayCombo.Init(m_availableToolList, inputData.connected_objectdottedname(), "");
		}
		else
		{
			Log_Assert(false);
		}
	}

	void TADlgGeneralPage::setAuxiliaryShowFlags(int cmdShow)
	{
		GetDlgItem(IDC_AUXILIARY_GROUP)->ShowWindow(cmdShow);
		GetDlgItem(IDC_ENABLE_AUXILIARY_EXTENTS)->ShowWindow(cmdShow);
		GetDlgItem(IDC_SOURCE_IMAGE_TEXT)->ShowWindow(cmdShow);
		GetDlgItem(IDC_SOURCE_IMAGE_COMBO)->ShowWindow(cmdShow);

		GetDlgItem(IDC_OVERLAY_GROUP)->ShowWindow(cmdShow);
		GetDlgItem(IDC_SHOW_OVERLAY_IN_ANCESTOR)->ShowWindow(cmdShow);
		GetDlgItem(IDC_ANCESTOR_SELECTION_TEXT)->ShowWindow(cmdShow);
		GetDlgItem(IDC_ANCESTOR_SELECTION_COMBO)->ShowWindow(cmdShow);
	}
} //namespace SvOg

