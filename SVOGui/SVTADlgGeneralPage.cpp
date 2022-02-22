//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgGeneralPage
//* .File Name       : $Workfile:   SVTADlgGeneralPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:50:40  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTADlgGeneralPage.h"
#include "ObjectInterfaces\NameValueVector.h"
#include "SVShowDependentsDialog.h"
#include <numeric>
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	SVTADlgGeneralPage::SVTADlgGeneralPage(uint32_t inspectionId, uint32_t taskObjectId, SvPb::SVObjectSubTypeEnum subType)
	: CPropertyPage(SVTADlgGeneralPage::IDD)
	, m_InspectionID(inspectionId)
	, m_TaskObjectID(taskObjectId)
	, m_subType(subType)
	, m_values(SvOg::BoundValues(inspectionId, taskObjectId))
	, m_AuxExtentsController(inspectionId, taskObjectId)
	{
		//{{AFX_DATA_INIT(SVTADlgGeneralPage)
		//}}AFX_DATA_INIT
	}

	SVTADlgGeneralPage::~SVTADlgGeneralPage()
	{
	}

	HRESULT SVTADlgGeneralPage::SetInspectionData()
	{
		HRESULT hr = S_OK;

		UpdateData(true); // get data from dialog

		int CurrentSelection = m_drawToolCombo.GetCurSel();
		if (CB_ERR != CurrentSelection)
		{
			long Value = static_cast<long> (m_drawToolCombo.GetItemData(CurrentSelection));
			m_values.Set<long>(SvPb::ConditionalToolDrawFlagEId, Value);
		}			
		m_AuxExtentsController.EnableAuxExtents(m_bUpdateAuxiliaryExtents ? true : false);

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

	void SVTADlgGeneralPage::refresh()
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
			if (m_IsExternalTool)
			{
				GetDlgItem(IDC_CHECK_USE_1_1)->EnableWindow(true);
			}
		}
		
		
		


		UpdateData(false); // Send Data to Dialog...
	}
	
	void SVTADlgGeneralPage::SetupDrawFlagComboBox()
	{
		const SvOi::NameValueVector& rDrawCriteria = m_values.GetEnumTypes(SvPb::ConditionalToolDrawFlagEId);
		m_drawToolCombo.SetEnumTypes(rDrawCriteria);
		long CurrentSelection = m_values.Get<long>(SvPb::ConditionalToolDrawFlagEId);
		m_drawToolCombo.SetCurSelItemData(CurrentSelection);
	}

	void SVTADlgGeneralPage::SetImages()
	{
		m_AvailableSourceImageCombo.ResetContent();
		if (m_bAuxExtentsAvailable)
		{
			const SvUl::NameObjectIdList& availImages = m_AuxExtentsController.GetAvailableImageList();
			for (SvUl::NameObjectIdList::const_iterator it = availImages.begin();it != availImages.end();++it)
			{
				m_AvailableSourceImageCombo.AddString(it->first.c_str());
			}
			auto name = m_AuxExtentsController.GetAuxSourceImageName();
			if (!name.empty())
			{
				m_AvailableSourceImageCombo.SelectString(-1, name.c_str());
			}
		}
	}

	bool SVTADlgGeneralPage::CheckAuxiliaryExtentsAvailable() const
	{
		bool bRetVal = m_AuxExtentsController.AreAuxiliaryExtentsAvailable();
		return bRetVal;
	}

	void SVTADlgGeneralPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVTADlgGeneralPage)
		DDX_Control(pDX, IDC_SOURCE_IMAGE_COMBO, m_AvailableSourceImageCombo);
		DDX_Control(pDX, IDC_DRAW_TOOL_COMBO, m_drawToolCombo);
		DDX_Check(pDX, IDC_ENABLE_AUXILIARY_EXTENTS, m_bUpdateAuxiliaryExtents);
		DDX_Control(pDX, IDC_TOOL_OVERLAYCOLOR_COMBO, m_AvailableToolForColorOverlayCombo);
		DDX_Check(pDX, IDC_CLOSE_TOOL_CBOX, m_bCloseTool);
		DDX_Check(pDX, IDC_CHECK_USE_1_1, m_UseUnitMapping);
		//}}AFX_DATA_MAP
	}

	BEGIN_MESSAGE_MAP(SVTADlgGeneralPage, CPropertyPage)
		//{{AFX_MSG_MAP(SVTADlgGeneralPage)
		ON_CBN_SELCHANGE(IDC_DRAW_TOOL_COMBO, OnSelchangeDrawToolCombo)
		ON_BN_CLICKED(IDC_ENABLE_AUXILIARY_EXTENTS, OnUpdateAuxiliaryExtents)
		ON_BN_CLICKED(IDC_CLOSE_TOOL_CBOX, OnUpdateCloseTool)
		ON_CBN_SELCHANGE(IDC_SOURCE_IMAGE_COMBO, OnSelchangeSourceImageCombo)
		ON_CBN_SELCHANGE(IDC_TOOL_OVERLAYCOLOR_COMBO, OnSelchangeToolForOverlayColorCombo)
		ON_BN_CLICKED(ID_SHOW_RELATIONS, OnShowRelations)
		ON_BN_CLICKED(IDC_CHECK_USE_1_1, OnUpdateUseUnitMapping)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	/////////////////////////////////////////////////////////////////////////////
	// SVTADlgGeneralPage message handlers

	BOOL SVTADlgGeneralPage::OnInitDialog() 
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
			GetDlgItem(IDC_ENABLE_AUXILIARY_EXTENTS)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_AUXILIARY_GROUP)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_SOURCE_IMAGE_TEXT)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_SOURCE_IMAGE_COMBO)->ShowWindow(SW_HIDE);
		}
		else
		{
			// Enable Auxiliary Extents
			GetDlgItem(IDC_ENABLE_AUXILIARY_EXTENTS)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_AUXILIARY_GROUP)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_SOURCE_IMAGE_TEXT)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_SOURCE_IMAGE_COMBO)->ShowWindow(SW_SHOW);
		
			
			m_bUpdateAuxiliaryExtents = m_AuxExtentsController.IsUpdateAuxExtentsEnabled();

			GetDlgItem(IDC_SOURCE_IMAGE_COMBO)->EnableWindow(m_bUpdateAuxiliaryExtents);

			
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

	void SVTADlgGeneralPage::OnSelchangeDrawToolCombo()
	{
		SetInspectionData();
		refresh();
	}

	void SVTADlgGeneralPage::OnUpdateAuxiliaryExtents() 
	{
		SetInspectionData();

		const SvUl::NameObjectIdPair& source = m_AuxExtentsController.GetAuxSourceImage();
		if (!source.first.empty())
		{
			m_AuxExtentsController.SetAuxSourceImage(source.first);
		}

		GetDlgItem(IDC_SOURCE_IMAGE_COMBO)->EnableWindow(m_bUpdateAuxiliaryExtents);
		refresh();
	}


	void SVTADlgGeneralPage::OnUpdateUseUnitMapping()
	{
		SetInspectionData();
		
		m_AuxExtentsController.resetTaskObject();
		m_AuxExtentsController.FindAuxSourceImages();
		SetImages();


		GetDlgItem(IDC_SOURCE_IMAGE_COMBO)->EnableWindow(m_bUpdateAuxiliaryExtents);
		refresh();
	}


	void SVTADlgGeneralPage::OnUpdateCloseTool()
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
						std::string messageText = std::accumulate(rList.rbegin(), rList.rend(),
							std::string{ "There are invalid dependencies. Tool will become invalid, do you still want to continue?\n\nInvalid dependencies:" },
							[](std::string a, const auto& b) { return std::move(a) + "\n" + b.namefrom() + "  ->  " + b.nameto(); });
						int retCode = MessageBox(messageText.c_str(), "Invalid Dependencies", MB_YESNO);
						if (IDNO == retCode)
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

	void SVTADlgGeneralPage::OnSelchangeSourceImageCombo() 
	{
		UpdateData( TRUE ); // get data from dialog

		int index = static_cast<int>(m_AvailableSourceImageCombo.GetCurSel());
		if (CB_ERR != index)
		{
			CString name;
			m_AvailableSourceImageCombo.GetLBText(index, name);
			if (!name.IsEmpty())
			{
				m_AuxExtentsController.SetAuxSourceImage( std::string(name.GetString() ));
			}
		}
		refresh();
	}

	void SVTADlgGeneralPage::OnSelchangeToolForOverlayColorCombo()
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
				pRequest->set_inputname(m_inputName_toolForColorOverlay);
				pRequest->set_newconnectedid(iter->second);
				pRequest->set_objecttype(SvPb::SVToolObjectType);

				HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
				assert(S_OK == hr);
			}
		}
		refresh();
	}
	
	void SVTADlgGeneralPage::OnShowRelations() 
	{
		std::set<uint32_t> DependencySet;
		DependencySet.insert(m_TaskObjectID);
		SVShowDependentsDialog Dlg( DependencySet, SvPb::SVToolObjectType, nullptr, SVShowDependentsDialog::Show );
		Dlg.DoModal();
	}

	BOOL SVTADlgGeneralPage::OnSetActive() 
	{
		SetImages();
		refresh();

		return CPropertyPage::OnSetActive();
	}

	void SVTADlgGeneralPage::initToolForOverlayColor()
	{
		uint32_t selectedId = SvDef::InvalidObjectId;
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getinputsrequest();
		pRequest->set_objectid(m_TaskObjectID);
		pRequest->mutable_typeinfo()->set_objecttype(SvPb::SVToolObjectType);
		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getinputsresponse() && 0 < responseCmd.getinputsresponse().list_size())
		{
			auto inputValues = responseCmd.getinputsresponse().list(0);
			selectedId = inputValues.objectid();
			m_inputName_toolForColorOverlay = inputValues.inputname();
		}
		else
		{
			assert(false);
		}

		auto* pAvailableRequest = requestCmd.mutable_getavailableobjectsrequest();
		pAvailableRequest->set_objectid(m_InspectionID);
		pAvailableRequest->mutable_typeinfo()->set_objecttype(SvPb::SVToolObjectType);
		pAvailableRequest->set_objecttypetoinclude(SvPb::SVToolSetObjectType);
		pAvailableRequest->set_importantobjectforstopatborder(m_TaskObjectID);

		hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getavailableobjectsresponse())
		{
			m_availableToolList = SvCmd::convertNameObjectIdList(responseCmd.getavailableobjectsresponse().list());
			if (SvDef::InvalidObjectId == selectedId)
			{
				selectedId = m_TaskObjectID;
			}

			auto iter = std::find_if(m_availableToolList.begin(), m_availableToolList.end(), [selectedId](const auto& rEntry) { return rEntry.second == selectedId; });
			std::string selectedName{};
			if (m_availableToolList.end() != iter)
			{
				selectedName = iter->first;
			}
			m_AvailableToolForColorOverlayCombo.Init(m_availableToolList, selectedName, "");
		}
		else
		{
			assert(false);
		}
	}
} //namespace SvOg

