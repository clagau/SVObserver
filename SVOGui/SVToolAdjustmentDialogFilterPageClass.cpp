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
#include "SVCommandLibrary/SVObjectSynchronousCommandTemplate.h"
#include "TextDefinesSvOg.h"
#include "InspectionCommands/GetCreatableObjects.h"
#include "InspectionCommands/GetInstanceIDByTypeInfo.h"
#include "InspectionCommands/GetAvailableObjects.h"
#include "InspectionCommands/GetObjectTypeInfo.h"
#include "InspectionCommands/ConstructAndInsertTaskObject.h"
#include "InspectionCommands/ShouldInspectionReset.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "TextDefinesSvOg.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "InspectionCommands/CommandFunctionHelper.h"
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
		// Get Instance GUID for the Mask Operator...
		typedef SvCmd::GetInstanceIDByTypeInfo Command;
		typedef std::shared_ptr<Command> CommandPtr;

		SvDef::SVObjectTypeInfoStruct info(SvDef::SVUnaryImageOperatorListObjectType/*SvDef::SVUnaryImageOperatorObjectType*/, SvDef::SVNotSetSubObjectType);

		CommandPtr commandPtr = CommandPtr(new Command(m_TaskObjectID, info));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			m_UnaryImageOperatorID = commandPtr->GetInstanceID();
		}
	}

	SVToolAdjustmentDialogFilterPageClass::~SVToolAdjustmentDialogFilterPageClass()
	{
	}

	HRESULT SVToolAdjustmentDialogFilterPageClass::SetInspectionData()
	{
		UpdateData( TRUE ); // get data from dialog
		HRESULT hrOk = SvCmd::RunOnceSynchronous(m_InspectionID, m_TaskObjectID);

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
		typedef SvCmd::GetAvailableObjects Command;
		typedef std::shared_ptr<Command> CommandPtr;

		SvUl::NameGuidList availableList;
		CommandPtr commandPtr{ new Command(m_UnaryImageOperatorID, SvDef::SVObjectTypeInfoStruct(SvDef::SVFilterObjectType, SvDef::SVNotSetSubObjectType)) };
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			availableList = commandPtr->AvailableObjects();
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
			typedef SvCmd::GetObjectTypeInfo Command;
			typedef std::shared_ptr<Command> CommandPtr;
			CommandPtr commandPtr{ new Command(filterGUID) };
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if (S_OK == hr)
			{
				SvDef::SVObjectTypeInfoStruct info = commandPtr->GetTypeInfo();
				
				m_btnProperties.EnableWindow( SvDef::SVCustomFilterObjectType == info.SubType 
				|| SvDef::SVCustom2FilterObjectType == info.SubType
				|| SvDef::SVRankingFilterObjectType == info.SubType
				|| SvDef::SVThinningFilterObjectType == info.SubType
				|| SvDef::SVThickeningFilterObjectType == info.SubType
				|| SvDef::SVWatershedFilterObjectType == info.SubType );
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

		typedef SvCmd::GetCreatableObjects Command;
		typedef std::shared_ptr<Command> CommandPtr;

		SvUl::NameGuidList availableList;
		CommandPtr commandPtr {new Command(m_UnaryImageOperatorID, SvDef::SVObjectTypeInfoStruct(SvDef::SVFilterObjectType, SvDef::SVNotSetSubObjectType))};
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			availableList = commandPtr->AvailableObjects();
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
			typedef SvCmd::ConstructAndInsertTaskObject Command;
			typedef std::shared_ptr<Command> CommandPtr;
			CommandPtr commandPtr{ new Command(m_UnaryImageOperatorID, classID, destinyIndex) };
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if (S_OK != hr)
			{
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
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
		// For all Items in the Selected (Instantiated) Filter list
		typedef SvCmd::GetAvailableObjects Command;
		typedef std::shared_ptr<Command> CommandPtr;
		SvUl::NameGuidList availableList;
		CommandPtr commandPtr{ new Command(m_UnaryImageOperatorID, SvDef::SVObjectTypeInfoStruct(SvDef::SVFilterObjectType, SvDef::SVNotSetSubObjectType)) };
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			availableList = commandPtr->AvailableObjects();
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
				typedef SvCmd::ShouldInspectionReset ResetCommand;
				typedef std::shared_ptr<ResetCommand> ResetCommandPtr;
				ResetCommandPtr resetCommandPtr{ new ResetCommand(filterGUID) };
				SVObjectSynchronousCommandTemplate<ResetCommandPtr> resetCmd(m_InspectionID, resetCommandPtr);
				HRESULT hr = resetCmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				bReset |= resetCommandPtr->shouldResetInspection();
				
				SvPb::InspectionCmdMsgs Request, Response;
				// Close, Disconnect and Delete it
				SvPb::DestroyChildRequest* pDestroyChildRequest = Request.mutable_destroychildrequest();
				//if last object and it should be reset, set flag
				if (bReset && listSize - 1 == i)
				{
					pDestroyChildRequest->set_flag(SvPb::DestroyChildRequest::Flag_SetDefaultInputs_And_ResetInspection);
				}
				
				SvPb::SetGuidInProtoBytes(pDestroyChildRequest->mutable_taskobjectlistid(), m_UnaryImageOperatorID);
				SvPb::SetGuidInProtoBytes(pDestroyChildRequest->mutable_objectid(), filterGUID);
				SvCmd::InspectionCommandsSynchronous(m_InspectionID, &Request, &Response);
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
			typedef SvCmd::ShouldInspectionReset ResetCommand;
			typedef std::shared_ptr<ResetCommand> ResetCommandPtr;
			ResetCommandPtr resetCommandPtr{ new ResetCommand(filterGUID) };
			SVObjectSynchronousCommandTemplate<ResetCommandPtr> resetCmd(m_InspectionID, resetCommandPtr);
			HRESULT hr = resetCmd.Execute(TWO_MINUTE_CMD_TIMEOUT);

			if( S_OK == hr )
			{
				SvPb::InspectionCmdMsgs Request,Response;
				SvPb::DestroyChildRequest* pDestroyChildRequest = Request.mutable_destroychildrequest(); ;
				//if last object and it should be reset, set flag
				if (resetCommandPtr->shouldResetInspection())
				{
					pDestroyChildRequest->set_flag(SvPb::DestroyChildRequest::Flag_SetDefaultInputs_And_ResetInspection);
				}
				SvPb::SetGuidInProtoBytes(pDestroyChildRequest->mutable_taskobjectlistid(), m_UnaryImageOperatorID);
				SvPb::SetGuidInProtoBytes(pDestroyChildRequest->mutable_objectid(), filterGUID);
				SvCmd::InspectionCommandsSynchronous(m_InspectionID, &Request, &Response);
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
			typedef SvCmd::GetObjectTypeInfo Command;
			typedef std::shared_ptr<Command> CommandPtr;
			CommandPtr commandPtr{ new Command(filterGUID) };
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if (S_OK == hr)
			{
				SvDef::SVObjectTypeInfoStruct info = commandPtr->GetTypeInfo();

				switch (info.SubType)
				{
				case SvDef::SVCustomFilterObjectType:
					{
						SVCustomFilterDlg l_svDlg( m_InspectionID, filterGUID, this );
						l_svDlg.DoModal();
					}
					break;
				case SvDef::SVCustom2FilterObjectType:
					{
						Custom2FilterDlg l_svDlg( m_InspectionID, filterGUID, this );
						l_svDlg.DoModal();
					}
					break;
				case SvDef::SVRankingFilterObjectType:
					{
						SVRankingFilterDlg l_svDlg( m_InspectionID, filterGUID, this );
						l_svDlg.DoModal();
					}
					break;
				case SvDef::SVThinningFilterObjectType:
					{
						SVThinningFilterDlg l_svDlg( m_InspectionID, filterGUID, this );
						l_svDlg.DoModal();
					}
					break;
				case SvDef::SVThickeningFilterObjectType:
					{
						SVThickeningFilterDlg l_svDlg( m_InspectionID, filterGUID, this );
						l_svDlg.DoModal();
					}
					break;
				case SvDef::SVWatershedFilterObjectType:
					{
						SVWatershedFilterDlg l_svDlg( m_InspectionID, m_TaskObjectID, filterGUID, this );
						l_svDlg.DoModal();
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
