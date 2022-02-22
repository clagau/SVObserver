//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogFilterPageClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogFilterPageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   24 Oct 2014 11:45:54  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOResource/resource.h"
#include "SVToolAdjustmentDialogFilterPageClass.h"
#include "SVCustomFilterDlg.h"
#include "SVRankingFilterDlg.h"
#include "Ranking2FilterDlg.h"
#include "SVThinningFilterDlg.h"
#include "SVThickeningFilterDlg.h"
#include "Custom2FilterDlg.h"
#include "SVWatershedFilterDlg.h"
#include "TextDefinesSvOg.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "TextDefinesSvOg.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "BlobReconstructFilterDlg.h"
#include "ReconstructFilterDlg.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogFilterPageClass, CPropertyPage)
		//{{AFX_MSG_MAP(SVToolAdjustmentDialogFilterPageClass)
		ON_BN_CLICKED(IDC_BUTTON4, OnButtonProperties)
		ON_BN_CLICKED(IDC_BUTTON3, OnButtonClearAll)
		ON_BN_CLICKED(IDC_BUTTON2, OnButtonDeleteCurrentFilter)
		ON_BN_CLICKED(IDC_BUTTON1, OnButtonInsertNewFilter)
		ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	SVToolAdjustmentDialogFilterPageClass::SVToolAdjustmentDialogFilterPageClass(uint32_t inspectionID, uint32_t taskObjectID)
	: CPropertyPage(SVToolAdjustmentDialogFilterPageClass::IDD)
	, m_ImageController(inspectionID, taskObjectID)
	, m_InspectionID(inspectionID)
	, m_TaskObjectID(taskObjectID)
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getobjectidrequest()->mutable_info();
		pRequest->set_ownerid(m_TaskObjectID);
		pRequest->mutable_infostruct()->set_objecttype(SvPb::SVUnaryImageOperatorListObjectType);

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getobjectidresponse())
		{
			m_UnaryImageOperatorID = responseCmd.getobjectidresponse().objectid();
		}
	}

	SVToolAdjustmentDialogFilterPageClass::~SVToolAdjustmentDialogFilterPageClass()
	{
	}

	HRESULT SVToolAdjustmentDialogFilterPageClass::SetInspectionData()
	{
		UpdateData( TRUE ); // get data from dialog
		HRESULT hrOk = SvCmd::RunOnceSynchronous(m_InspectionID);

		UpdateData( FALSE );

		return hrOk;
	}
		
	void SVToolAdjustmentDialogFilterPageClass::setImages()
	{
		IPictureDisp* pResultImage = m_ImageController.GetImage(getFirstResultImageId(m_ImageController));

		// Set dialog image...
		dialogImage.setImage(pResultImage);
		dialogImage.Refresh();
	}

	void SVToolAdjustmentDialogFilterPageClass::refresh()
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getavailableobjectsrequest();
		pRequest->set_objectid(m_UnaryImageOperatorID);
		pRequest->mutable_typeinfo()->set_objecttype(SvPb::SVFilterObjectType);

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		SvUl::NameObjectIdList availableList;
		if (S_OK == hr && responseCmd.has_getavailableobjectsresponse())
		{
			availableList = SvCmd::convertNameObjectIdList(responseCmd.getavailableobjectsresponse().list());
		}

		// Populate filter list box and run filter...
		m_filterListBox.init( availableList, Filter_NoFilter );

		SetInspectionData();

		// Refresh dialog image...
		setImages();
	}

	void SVToolAdjustmentDialogFilterPageClass::OnSelchangeList1() 
	{
		int index = m_filterListBox.GetCurSel();
		uint32_t filterID = m_filterListBox.getObjectId(index);
		if (SvDef::InvalidObjectId != filterID)
		{
			SvPb::InspectionCmdRequest requestCmd;
			SvPb::InspectionCmdResponse responseCmd;
			auto* pRequest = requestCmd.mutable_getobjectparametersrequest();
			pRequest->set_objectid(filterID);

			HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
			if (S_OK == hr && responseCmd.has_getobjectparametersresponse())
			{
				SvPb::SVObjectSubTypeEnum subType = responseCmd.getobjectparametersresponse().typeinfo().subtype();

				m_btnProperties.EnableWindow(SvPb::SVCustomFilterObjectType == subType
				|| SvPb::SVCustom2FilterObjectType == subType
				|| SvPb::SVRankingFilterObjectType == subType
				|| SvPb::Ranking2FilterObjectType == subType
				|| SvPb::SVThinningFilterObjectType == subType
				|| SvPb::SVThickeningFilterObjectType == subType
				|| SvPb::SVWatershedFilterObjectType == subType
				|| SvPb::EraseBorderBlobsFilterObjectType == subType
				|| SvPb::ExtractHolesFilterObjectType == subType
				|| SvPb::FillHolesFilterObjectType == subType
				|| SvPb::ReconstructFilterObjectType == subType);
			}
		}// end if
	}// end OnSelchangeList1

	void SVToolAdjustmentDialogFilterPageClass::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVToolAdjustmentDialogFilterPageClass)
		DDX_Control(pDX, IDC_BUTTON4, m_btnProperties);
		DDX_Control(pDX, IDC_BUTTON1, insertFilter);
		DDX_Control(pDX, IDC_LIST1, m_filterListBox);
		DDX_Control(pDX, IDC_DIALOGIMAGE, dialogImage);
		DDX_Control(pDX, IDC_COMBO1, m_availableFilterCB);
		//}}AFX_DATA_MAP
	}

	BOOL SVToolAdjustmentDialogFilterPageClass::OnSetActive() 
	{
		setImages();
		return CPropertyPage::OnSetActive();
	}

	BOOL SVToolAdjustmentDialogFilterPageClass::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();

		m_btnProperties.EnableWindow( FALSE );

		dialogImage.AddTab(_T("Tool Result"));  
		m_ImageController.Init();

		setImages();

		SvUl::NameClassIdList availableList;
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getcreatableobjectsrequest();
		pRequest->set_objectid(m_UnaryImageOperatorID);
		pRequest->mutable_typeinfo()->set_objecttype(SvPb::SVFilterObjectType);

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getcreatableobjectsresponse())
		{
			const auto& rList = responseCmd.getcreatableobjectsresponse().list();
			std::transform(rList.begin(), rList.end(), std::back_inserter(availableList), 
				[](const auto& rEntry) -> SvUl::NameClassIdPair { return { rEntry.objectname(), rEntry.classid()}; });
		}

		m_availableFilterCB.Init(availableList, _T(""), Filter_NoFilterAvailable);

		UpdateData(FALSE);

		refresh();
		return TRUE;
		// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}


	////////////////////////////////////////////////////////////////////////////////
	// .Title       : OnButtonInsertNewFilter
	// -----------------------------------------------------------------------------
	// .Description : Insert New Filter in Current Filter Table
	////////////////////////////////////////////////////////////////////////////////
	void SVToolAdjustmentDialogFilterPageClass::OnButtonInsertNewFilter() 
	{ 
		auto classID = m_availableFilterCB.getSelectedValue();

		if( SvPb::NoObjectClassId != classID )
		{
			int destinyIndex = m_filterListBox.GetCurSel();
			if (LB_ERR == destinyIndex) // First Entryitem
			{
				destinyIndex = 0;
			}

			// Construct and Create the Filter Class Object
			SvPb::InspectionCmdRequest requestCmd;
			auto* pRequest = requestCmd.mutable_createobjectrequest();
			pRequest->set_ownerid(m_UnaryImageOperatorID);
			pRequest->set_classid(classID);
			pRequest->set_taskobjectpos(destinyIndex);

			HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
			if (S_OK != hr)
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_CreationFilterFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10231 );
			}

			// Refresh Dialog...
			refresh();
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	// .Title       : OnButtonClearAll
	// -----------------------------------------------------------------------------
	// .Description : Clear Filter Table
	////////////////////////////////////////////////////////////////////////////////
	void SVToolAdjustmentDialogFilterPageClass::OnButtonClearAll() 
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getavailableobjectsrequest();
		pRequest->set_objectid(m_UnaryImageOperatorID);
		pRequest->mutable_typeinfo()->set_objecttype(SvPb::SVFilterObjectType);

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		SvUl::NameObjectIdList availableList;
		if (S_OK == hr && responseCmd.has_getavailableobjectsresponse())
		{
			availableList = SvCmd::convertNameObjectIdList(responseCmd.getavailableobjectsresponse().list());
		}

		// Delete them
		bool bReset = false;

		// remove all filter items (instantiated)
		size_t listSize = availableList.size();
		
		for (int i=0; i<listSize; ++i)
		{
			uint32_t filterID = availableList[i].second;
			if (SvDef::InvalidObjectId != filterID)
			{
				requestCmd.Clear();
				responseCmd.Clear();
				SvPb::ShouldInspectionResetRequest* pShouldResetRequest = requestCmd.mutable_shouldinspectionresetrequest();
				pShouldResetRequest->set_objectid(filterID);

				hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
				if (S_OK == hr && responseCmd.has_shouldinspectionresetresponse())
				{
					bReset |= responseCmd.shouldinspectionresetresponse().shouldreset();
				}
				else
				{
					bReset = true;
				}
				
				requestCmd.Clear();
				// Close, Disconnect and Delete it
				SvPb::DeleteObjectRequest* pDeleteObjectRequest = requestCmd.mutable_deleteobjectrequest();
				//if last object and it should be reset, set flag
				if (bReset && listSize - 1 == i)
				{
					pDeleteObjectRequest->set_flag(SvPb::DeleteObjectRequest::Flag_SetDefaultInputs_And_ResetInspection);
				}
				pDeleteObjectRequest->set_objectid(filterID);

				SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
			}
		}

		m_filterListBox.SetCurSel( -1 );

		// Refresh Dialog...
		refresh();
	}

	////////////////////////////////////////////////////////////////////////////////
	// .Title       : OnButtonDeleteCurrentFilter
	// -----------------------------------------------------------------------------
	// .Description : Delete Current Filter in Current Filter Table
	////////////////////////////////////////////////////////////////////////////////
	void SVToolAdjustmentDialogFilterPageClass::OnButtonDeleteCurrentFilter() 
	{
		int index = m_filterListBox.GetCurSel();
		uint32_t filterID = m_filterListBox.getObjectId(index);
		if(SvDef::InvalidObjectId != filterID )
		{
			SvPb::InspectionCmdRequest requestCmd;
			SvPb::InspectionCmdResponse responseCmd;
			auto* pRequest = requestCmd.mutable_shouldinspectionresetrequest();
			pRequest->set_objectid(filterID);

			HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
			if (S_OK == hr && responseCmd.has_shouldinspectionresetresponse())
			{
				bool bReset = responseCmd.shouldinspectionresetresponse().shouldreset();

				requestCmd.Clear();
				SvPb::DeleteObjectRequest* pDeleteObjectRequest = requestCmd.mutable_deleteobjectrequest();
				//if last object and it should be reset, set flag
				if (bReset)
				{
					pDeleteObjectRequest->set_flag(SvPb::DeleteObjectRequest::Flag_SetDefaultInputs_And_ResetInspection);
				}
				pDeleteObjectRequest->set_objectid(filterID);

				SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
			}

			// Refresh Dialog...
			refresh();
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	// .Title       : OnButtonProperties
	// -----------------------------------------------------------------------------
	// .Description : Change properties of the Current custom Filter in Filter Table
	////////////////////////////////////////////////////////////////////////////////
	void SVToolAdjustmentDialogFilterPageClass::OnButtonProperties() 
	{
		int index = m_filterListBox.GetCurSel();
		uint32_t filterID = m_filterListBox.getObjectId(index);
		if (SvDef::InvalidObjectId != filterID)
		{
			SvPb::InspectionCmdRequest requestCmd;
			SvPb::InspectionCmdResponse responseCmd;
			auto* pRequest = requestCmd.mutable_getobjectparametersrequest();
			pRequest->set_objectid(filterID);

			HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
			if (S_OK == hr && responseCmd.has_getobjectparametersresponse())
			{
				switch (responseCmd.getobjectparametersresponse().typeinfo().subtype())
				{
				case SvPb::SVCustomFilterObjectType:
					{
						SVCustomFilterDlg l_svDlg( m_InspectionID, filterID, this );
						l_svDlg.DoModal();
					}
					break;
				case SvPb::SVCustom2FilterObjectType:
					{
						Custom2FilterDlg l_svDlg( m_InspectionID, filterID, this );
						l_svDlg.DoModal();
					}
					break;
				case SvPb::SVRankingFilterObjectType:
					{
						SVRankingFilterDlg l_svDlg( m_InspectionID, filterID, this );
						l_svDlg.DoModal();
					}
					break;
				case SvPb::Ranking2FilterObjectType:
				{
					Ranking2FilterDlg l_svDlg(m_InspectionID, filterID, this);
					l_svDlg.DoModal();
				}
				break;
				case SvPb::SVThinningFilterObjectType:
					{
						SVThinningFilterDlg l_svDlg( m_InspectionID, filterID, this );
						l_svDlg.DoModal();
					}
					break;
				case SvPb::SVThickeningFilterObjectType:
					{
						SVThickeningFilterDlg l_svDlg( m_InspectionID, filterID, this );
						l_svDlg.DoModal();
					}
					break;
				case SvPb::SVWatershedFilterObjectType:
					{
						SVWatershedFilterDlg l_svDlg( m_InspectionID, m_TaskObjectID, filterID, this );
						l_svDlg.DoModal();
					}
					break;
				case SvPb::EraseBorderBlobsFilterObjectType:
				{
					BlobReconstructFilterDlg dlg(IDS_CLASSNAME_ERASEBORDERBLOBSFILTER, m_InspectionID, filterID, this);
					dlg.DoModal();
				}
				break;
				case SvPb::ExtractHolesFilterObjectType:
				{
					BlobReconstructFilterDlg dlg(IDS_CLASSNAME_EXTRACTHOLESFILTER, m_InspectionID, filterID, this);
					dlg.DoModal();
				}
				break;
				case SvPb::FillHolesFilterObjectType:
				{
					BlobReconstructFilterDlg dlg(IDS_CLASSNAME_FILLHOLESFILTER, m_InspectionID, filterID, this);
					dlg.DoModal();
				}
				break;
				case SvPb::ReconstructFilterObjectType:
				{
					ReconstructFilterDlg dlg(m_InspectionID, m_TaskObjectID, filterID, this);
					dlg.DoModal();
				}
				break;
				default: //nothing to do
					break;
				} 
			}
		}// end if

		// Refresh Dialog...
		refresh();
	}
}  //namespace SvOg
