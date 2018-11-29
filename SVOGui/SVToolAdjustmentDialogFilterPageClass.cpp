//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
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
#include "Definitions/ObjectDefines.h"
#include "SVCustomFilterDlg.h"
#include "SVRankingFilterDlg.h"
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
#include "InspectionCommands/CommandFunction.h"
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

	SVToolAdjustmentDialogFilterPageClass::SVToolAdjustmentDialogFilterPageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID) 
	: CPropertyPage(SVToolAdjustmentDialogFilterPageClass::IDD)
	, m_ImageController(rInspectionID, rTaskObjectID)
	, m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID)
	{
		SvPb::InspectionCmdMsgs requestMessage, responseMessage;
		auto* pRequest = requestMessage.mutable_getobjectidrequest()->mutable_info();
		SvPb::SetGuidInProtoBytes(pRequest->mutable_ownerid(), m_TaskObjectID);
		pRequest->mutable_infostruct()->set_objecttype(SvPb::SVUnaryImageOperatorListObjectType);

		HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &requestMessage, &responseMessage);
		if (S_OK == hr && responseMessage.has_getobjectidresponse())
		{
			m_UnaryImageOperatorID = SvPb::GetGuidFromProtoBytes(responseMessage.getobjectidresponse().objectid());
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
		SVGUID imageId = getFirstResultImageId(m_ImageController);
		IPictureDisp* pResultImage = m_ImageController.GetImage(imageId);

		// Set dialog image...
		dialogImage.setImage(pResultImage);
		dialogImage.Refresh();
	}

	void SVToolAdjustmentDialogFilterPageClass::refresh()
	{
		SvPb::InspectionCmdMsgs request, response;
		SvPb::GetAvailableObjectsRequest* pGetAvailableObjectsRequest = request.mutable_getavailableobjectsrequest();

		SvPb::SetGuidInProtoBytes(pGetAvailableObjectsRequest->mutable_objectid(), m_UnaryImageOperatorID);
		pGetAvailableObjectsRequest->mutable_typeinfo()->set_objecttype(SvPb::SVFilterObjectType);
		HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &request, &response);
		SvUl::NameGuidList availableList;
		if (S_OK == hr && response.has_getavailableobjectsresponse())
		{
			availableList = SvCmd::convertNameGuidList(response.getavailableobjectsresponse().list());
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
		SVGUID filterGUID	= m_filterListBox.getGUID(index);
		if (GUID_NULL != filterGUID)
		{
			SvPb::InspectionCmdMsgs request, response;
			SvPb::GetObjectParametersRequest* pGetObjectNameRequest = request.mutable_getobjectparametersrequest();

			SvPb::SetGuidInProtoBytes(pGetObjectNameRequest->mutable_objectid(), filterGUID);
			HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &request, &response);
			if (S_OK == hr && response.has_getobjectparametersresponse())
			{
				SvPb::SVObjectSubTypeEnum subType = response.getobjectparametersresponse().typeinfo().subtype();

				m_btnProperties.EnableWindow(SvPb::SVCustomFilterObjectType == subType
				|| SvPb::SVCustom2FilterObjectType == subType
				|| SvPb::SVRankingFilterObjectType == subType
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

		SvUl::NameGuidList availableList;
		SvPb::InspectionCmdMsgs request, response;
		SvPb::GetCreatableObjectsRequest* pGetCreatableObjectsRequest = request.mutable_getcreatableobjectsrequest();
		SvPb::SetGuidInProtoBytes(pGetCreatableObjectsRequest->mutable_objectid(), m_UnaryImageOperatorID);
		pGetCreatableObjectsRequest->mutable_typeinfo()->set_objecttype(SvPb::SVFilterObjectType);
		HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &request, &response);
		SvUl::InputNameGuidPairList connectedList;
		if (S_OK == hr && response.has_getcreatableobjectsresponse())
		{
			for (auto item : response.getcreatableobjectsresponse().list())
			{
				availableList.push_back({item.objectname(), SvPb::GetGuidFromProtoBytes(item.objectid())});
			}
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
		const SVGUID classID = m_availableFilterCB.getSelectedGUID();

		if( GUID_NULL != classID )
		{
			int destinyIndex	= m_filterListBox.GetCurSel();

			if( LB_ERR == destinyIndex  ) // First Entryitem
			{
				destinyIndex = 0;
			}

			// Construct and Create the Filter Class Object
			SvPb::InspectionCmdMsgs requestMessage, responseMessage;
			SvPb::ConstructAndInsertRequest* pRequest = requestMessage.mutable_constructandinsertrequest();
			SvPb::SetGuidInProtoBytes(pRequest->mutable_ownerid(), m_UnaryImageOperatorID);
			SvPb::SetGuidInProtoBytes(pRequest->mutable_classid(), classID);
			pRequest->set_taskobjectpos(destinyIndex);

			HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &requestMessage, &responseMessage);
			if (S_OK != hr)
			{
				SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
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
		SvPb::InspectionCmdMsgs request, response;
		SvPb::GetAvailableObjectsRequest* pGetAvailableObjectsRequest = request.mutable_getavailableobjectsrequest();

		SvPb::SetGuidInProtoBytes(pGetAvailableObjectsRequest->mutable_objectid(), m_UnaryImageOperatorID);
		pGetAvailableObjectsRequest->mutable_typeinfo()->set_objecttype(SvPb::SVFilterObjectType);
		HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &request, &response);
		SvUl::NameGuidList availableList;
		if (S_OK == hr && response.has_getavailableobjectsresponse())
		{
			availableList = SvCmd::convertNameGuidList(response.getavailableobjectsresponse().list());
		}

		// Delete them
		bool bReset = false;

		// remove all filter items (instantiated)
		size_t listSize = availableList.size();
		
		for (int i=0; i<listSize; ++i)
		{
			SVGUID filterGUID = availableList[i].second;
			if( GUID_NULL != filterGUID )
			{
				SvPb::InspectionCmdMsgs request, response;
				SvPb::ShouldInspectionResetRequest* pShouldResetRequest = request.mutable_shouldinspectionresetrequest();
				SvPb::SetGuidInProtoBytes(pShouldResetRequest->mutable_objectid(), filterGUID);
				HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &request, &response);
				if (S_OK == hr && response.has_shouldinspectionresetresponse())
				{
					bReset |= response.shouldinspectionresetresponse().shouldreset();
				}
				else
				{
					bReset = true;
				}
				
				// Close, Disconnect and Delete it
				SvPb::DestroyChildRequest* pDestroyChildRequest = request.mutable_destroychildrequest();
				//if last object and it should be reset, set flag
				if (bReset && listSize - 1 == i)
				{
					pDestroyChildRequest->set_flag(SvPb::DestroyChildRequest::Flag_SetDefaultInputs_And_ResetInspection);
				}
				
				SvPb::SetGuidInProtoBytes(pDestroyChildRequest->mutable_taskobjectlistid(), m_UnaryImageOperatorID);
				SvPb::SetGuidInProtoBytes(pDestroyChildRequest->mutable_objectid(), filterGUID);
				SvCmd::InspectionCommandsSynchronous(m_InspectionID, &request, &response);
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
		SVGUID filterGUID = m_filterListBox.getGUID(index);
		if( GUID_NULL != filterGUID ) 
		{
			SvPb::InspectionCmdMsgs request, response;
			SvPb::ShouldInspectionResetRequest* pShouldResetRequest = request.mutable_shouldinspectionresetrequest();
			SvPb::SetGuidInProtoBytes(pShouldResetRequest->mutable_objectid(), filterGUID);
			HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &request, &response);
			if (S_OK == hr && response.has_shouldinspectionresetresponse())
			{
				bool bReset = response.shouldinspectionresetresponse().shouldreset();

				SvPb::DestroyChildRequest* pDestroyChildRequest = request.mutable_destroychildrequest(); ;
				//if last object and it should be reset, set flag
				if (bReset)
				{
					pDestroyChildRequest->set_flag(SvPb::DestroyChildRequest::Flag_SetDefaultInputs_And_ResetInspection);
				}
				SvPb::SetGuidInProtoBytes(pDestroyChildRequest->mutable_taskobjectlistid(), m_UnaryImageOperatorID);
				SvPb::SetGuidInProtoBytes(pDestroyChildRequest->mutable_objectid(), filterGUID);
				SvCmd::InspectionCommandsSynchronous(m_InspectionID, &request, &response);
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
		SVGUID filterGUID	= m_filterListBox.getGUID(index);
		if( GUID_NULL != filterGUID ) 
		{
			SvPb::InspectionCmdMsgs request, response;
			SvPb::GetObjectParametersRequest* pGetObjectNameRequest = request.mutable_getobjectparametersrequest();

			SvPb::SetGuidInProtoBytes(pGetObjectNameRequest->mutable_objectid(), filterGUID);
			HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &request, &response);
			if (S_OK == hr && response.has_getobjectparametersresponse())
			{
				switch (response.getobjectparametersresponse().typeinfo().subtype())
				{
				case SvPb::SVCustomFilterObjectType:
					{
						SVCustomFilterDlg l_svDlg( m_InspectionID, filterGUID, this );
						l_svDlg.DoModal();
					}
					break;
				case SvPb::SVCustom2FilterObjectType:
					{
						Custom2FilterDlg l_svDlg( m_InspectionID, filterGUID, this );
						l_svDlg.DoModal();
					}
					break;
				case SvPb::SVRankingFilterObjectType:
					{
						SVRankingFilterDlg l_svDlg( m_InspectionID, filterGUID, this );
						l_svDlg.DoModal();
					}
					break;
				case SvPb::SVThinningFilterObjectType:
					{
						SVThinningFilterDlg l_svDlg( m_InspectionID, filterGUID, this );
						l_svDlg.DoModal();
					}
					break;
				case SvPb::SVThickeningFilterObjectType:
					{
						SVThickeningFilterDlg l_svDlg( m_InspectionID, filterGUID, this );
						l_svDlg.DoModal();
					}
					break;
				case SvPb::SVWatershedFilterObjectType:
					{
						SVWatershedFilterDlg l_svDlg( m_InspectionID, m_TaskObjectID, filterGUID, this );
						l_svDlg.DoModal();
					}
					break;
				case SvPb::EraseBorderBlobsFilterObjectType:
				case SvPb::ExtractHolesFilterObjectType:
				case SvPb::FillHolesFilterObjectType:
				{
					BlobReconstructFilterDlg dlg(m_InspectionID, filterGUID, this);
					dlg.DoModal();
				}
				break;
				case SvPb::ReconstructFilterObjectType:
				{
					ReconstructFilterDlg dlg(m_InspectionID, m_TaskObjectID, filterGUID, this);
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
